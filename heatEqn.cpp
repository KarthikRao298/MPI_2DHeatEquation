/*
 * File Name       :heatEqn.cpp
 * Description     :Script to plot the speedup of matrix multiplication with strong scaling
 * Author          :Karthik Rao
 * Date            :Dec 06 2017
 * Version         :0.1
 *
 * To compile :
 * Make heatEqn
 *
 * To compile :
 * mpicxx -std=c++11 heatEqn.cpp -o heatEqn
 * 
 * Sample command line execution :
 * 
 * mpirun -n 3 ./heatEqn  9
 *
 *
 * qsub -d $(pwd) -q mamba -l procs=2 -v mpirun -n 2 ./heatEqn  100
 *
 */

/* Debug prints will be enabled if set to 1 */
#define DEBUG 0
/* This macro is added to simplify debugging of the code */
#define DIVISOR 9
#define NODE_0 0
#define HEAT_MAT_SENDER_FIRST_ROW 0
#define HEAT_MAT_SENDER_LAST_ROW 1

#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <string.h>
#include <cmath>

#include "CommonHeader.h"

/* function to initialise the matrix */
void InitHeatMatrix (double *** heatSubMatCur, int heatSubMatRowSize, int heatSubMatColmSize, int indexValue);
/* function to print the matrix of amy dimention */
void printMatrix(double **mat, int rows, int colms);
/* function to print 1 X N matrix of amy dimention */
void printRow(double *mat, int colms);

/*==============================================================================
 *  main
 *=============================================================================*/

int main (int argc, char* argv[]) {


    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <heatMatSize>"<<std::endl;

        return -1;
    }

    /* 
     * 1. based on the rank of the node create a set rows of N*N matrix with random values,
     * this would be few rows of H(t-1) matrix, similarly create few rows of H(t) matrix
     * whose elements are set to 0
     * 
     * 2. send (use MPI_Isend) the neighbour rows to the neighbour nodes.
     * 3. start computing the interior of the matrix.
     * 4. once the interior rows of the matrix are done then 
     *    receive (use MPI_recv) the required neighbour rows from the neighbour nodes.
     * 5. compute the neighbour rows.
     * 6. Once H(t) is computed, wait for all the async sends to complete
     * 7. check if H(20) is computed, if not then start computing the next heat matrix
     *    else break out of the loop.
     *
     */ 


    MPI_Init(NULL, NULL);


    int heatMatSize  = atoi (argv[1]);

    int heatSubMatColmSize, heatSubMatRowSize;
    int firstRow, lastRow;
    int commSize;
    int procRank;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    double ** heatSubMatPast;
    double ** heatSubMatCur;
    heatSubMatColmSize = heatMatSize;
    heatSubMatRowSize = heatMatSize / commSize;

    int firstNode = NODE_0;
    int lastNode = commSize - 1;

    MPI_Request SendReq[2];
    MPI_Status Status[2];

    int i,j;
    /* measure time taken for computation */
    std::chrono::time_point<std::chrono::system_clock> StartTime;
    std::chrono::time_point<std::chrono::system_clock>  EndTime;
    std::chrono::duration<double> ElapsedTime;

    MPI_Barrier( MPI_COMM_WORLD ) ;
    if (procRank == NODE_0){
        StartTime = std::chrono::system_clock::now();
    }
    MPI_Barrier( MPI_COMM_WORLD ) ;

    InitHeatMatrix (&heatSubMatPast, heatSubMatRowSize, heatSubMatColmSize, 1);
    InitHeatMatrix (&heatSubMatCur, heatSubMatRowSize, heatSubMatColmSize, 0);

    firstRow = 0 ;
    lastRow  = heatSubMatRowSize - 1;

    /* allocate memory for the received rows */
    double * receivedRow = new double [heatSubMatColmSize];


    DLOG (C_VERBOSE, "Node[%d] firstRow  = %d. lastRow  = %d\n",procRank, firstRow, lastRow);
    DLOG (C_VERBOSE, "Node[%d] heatSubMatRowSize  = %d. heatSubMatColmSize  = %d\n",procRank, heatSubMatRowSize, heatSubMatColmSize);
    DLOG (C_VERBOSE, "Node[%d] Creating firstRow subarray datatype\n",procRank);
    /* create a datatype to describe the first row of the subarray */

    int sizes[2]    = {heatSubMatRowSize, heatSubMatColmSize};
    int subsizes[2] = {1, heatSubMatColmSize};
    int starts[2]   = {0,0};
    MPI_Datatype firstRowSubArray;
    MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_DOUBLE, &firstRowSubArray);
    MPI_Type_commit(&firstRowSubArray);

    /* end of first row subarray data type creation */


    DLOG (C_VERBOSE, "Node[%d] Creating lastRow subarray datatype\n",procRank);
    /* create a datatype to describe the last row of the subarray */

    int sizes_1[2]    = {heatSubMatRowSize, heatSubMatColmSize};
    int subsizes_1[2] = {1, heatSubMatColmSize};
    int starts_1[2]   = {0,0};
    MPI_Datatype lastRowSubArray;
    MPI_Type_create_subarray(2, sizes_1, subsizes_1, starts_1, MPI_ORDER_C, MPI_DOUBLE, &lastRowSubArray);
    MPI_Type_commit(&lastRowSubArray);

    /* end of first row subarray data type creation */


    for (int k = 0; k < 20 ; k++) {

#if (DEBUG)
        DLOG (C_VERBOSE, "Node[%d] Printing heatSubMatPast\n",procRank);
        printMatrix (heatSubMatPast, heatSubMatRowSize, heatSubMatColmSize);
#endif

        if (procRank == firstNode) {
            DLOG (C_VERBOSE, "Node[%d] procRank == firstNode so, sending lastRow\n",procRank);

            MPI_Isend (&heatSubMatPast[lastRow][0], 1, lastRowSubArray, procRank + 1,
                    HEAT_MAT_SENDER_LAST_ROW, MPI_COMM_WORLD, &SendReq[1]); 

        } else if (procRank == lastNode) {
            DLOG (C_VERBOSE, "Node[%d] procRank == lastNode so, sending firstRow\n",procRank);
            MPI_Isend (&heatSubMatPast[firstRow][0], 1, firstRowSubArray, procRank - 1,
                    HEAT_MAT_SENDER_FIRST_ROW, MPI_COMM_WORLD, &SendReq[0]); 


        } else {
            DLOG (C_VERBOSE, "Node[%d] procRank is neither firstNode nor lastNode so, sending both firstRow & lastRow\n",procRank);
            MPI_Isend (&heatSubMatPast[lastRow][0], 1, lastRowSubArray, procRank + 1,
                    HEAT_MAT_SENDER_LAST_ROW, MPI_COMM_WORLD, &SendReq[0]); 


            MPI_Isend (&heatSubMatPast[firstRow][0], 1, firstRowSubArray, procRank - 1,
                    HEAT_MAT_SENDER_FIRST_ROW, MPI_COMM_WORLD, &SendReq[1]); 

        }



        /* calculate the inner part of the submatrix */


        for (i = firstRow + 1 ; i < lastRow ; i++) {

            for (j = 0; j < heatSubMatColmSize; j++){

                if (j == 0 ){
                    /* for first column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( 0 + heatSubMatPast[i-1][j] + heatSubMatPast[i-1][j+1] + 
                            0 + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            0 + heatSubMatPast[i+1][j] + heatSubMatPast[i+1][j+1]   ) / DIVISOR;

                } else if ( j == heatSubMatColmSize - 1) {

                    /* for last column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( heatSubMatPast[i-1][j-1] + heatSubMatPast[i-1][j] + 0 + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]   + 0 +  
                            heatSubMatPast[i+1][j-1] + heatSubMatPast[i+1][j] + 0   ) / DIVISOR;

                } else {

                    heatSubMatCur[i][j] = ( heatSubMatPast[i-1][j-1] + heatSubMatPast[i-1][j] + heatSubMatPast[i-1][j+1] + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            heatSubMatPast[i+1][j-1] + heatSubMatPast[i+1][j] + heatSubMatPast[i+1][j+1]  ) / DIVISOR;
                }
            }

        }


        /* compute first row . 
         * if its a first node then the undefined values of H(t-1) are assumed to be 0 
         * so the heat equation is modified accordingly */

        if (procRank == firstNode) {
            DLOG (C_VERBOSE, "Node[%d] procRank == firstNode , so assuming 0 for ghost rows\n",procRank);

            i = 0;   
            for (j = 0; j < heatSubMatColmSize; j++){


                if (j == 0 ){
                    /* for first column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( 0 + 0 + 0 + 
                            0 + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            0 + heatSubMatPast[i+1][j] + heatSubMatPast[i+1][j+1]   ) / DIVISOR;

                } else if ( j == heatSubMatColmSize - 1) {

                    /* for last column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( 0 + 0 + 0 + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]   + 0 +  
                            heatSubMatPast[i+1][j-1] + heatSubMatPast[i+1][j] + 0   ) / DIVISOR;

                } else {

                    heatSubMatCur[i][j] = ( 0 + 0 + 0 + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            heatSubMatPast[i+1][j-1] + heatSubMatPast[i+1][j] + heatSubMatPast[i+1][j+1]  ) / DIVISOR;
                }


            }/* end of colm for loop */

        } else {
            DLOG (C_VERBOSE, "Node[%d] procRank != firstNode, so receiving upper row from the predessor node\n",procRank);

            /* receive the upper row from the predessor node and compute  */
            MPI_Recv (&receivedRow[0], heatSubMatColmSize, MPI_DOUBLE, procRank-1, HEAT_MAT_SENDER_LAST_ROW, MPI_COMM_WORLD, &Status[0]);
            i = firstRow;

#if (DEBUG)            
            DLOG (C_VERBOSE, "Node[%d] Printing receivedRow \n",procRank);
            printRow (receivedRow, heatSubMatColmSize);
#endif
            for (j = 0; j < heatSubMatColmSize; j++){

                if (j == 0 ){
                    /* for first column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( 0 + receivedRow[j] + receivedRow[j+1] + 
                            0 + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            0 + heatSubMatPast[i+1][j] + heatSubMatPast[i+1][j+1]   )/ DIVISOR;



                } else if ( j == heatSubMatColmSize-1) {

                    /* for last column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( receivedRow[j-1] + receivedRow[j] + 0 + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]  + 0 +  
                            heatSubMatPast[i+1][j-1] + heatSubMatPast[i+1][j] + 0   )/ DIVISOR;


                } else {

                    heatSubMatCur[i][j] = ( receivedRow[j-1] + receivedRow[j] + receivedRow[j+1] + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]  + heatSubMatPast[i][j+1]   + 
                            heatSubMatPast[i+1][j-1] + heatSubMatPast[i+1][j] + heatSubMatPast[i+1][j+1]  )/ DIVISOR;


                }
            }/* end of colm for loop */

        }

        /* compute last row . if its a last node then the undefined values of H(t-1) are assumed to be 0 */

        if (procRank == lastNode) {
            DLOG (C_VERBOSE, "procRank is lastNode. computing last row\n");

            i = lastRow;   
            for (j = 0; j < heatSubMatColmSize; j++){


                if (j == 0 ){
                    /* for first column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( 0 + heatSubMatPast[i-1][j] + heatSubMatPast[i-1][j+1] + 
                            0 + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            0 + 0 + 0  ) / DIVISOR;

                } else if ( j == heatSubMatColmSize - 1) {

                    /* for last column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( heatSubMatPast[i-1][j-1] + heatSubMatPast[i-1][j] + 0 + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]   + 0 +  
                            0 + 0 + 0   ) / DIVISOR;

                } else {

                    heatSubMatCur[i][j] = ( heatSubMatPast[i-1][j-1] + heatSubMatPast[i-1][j] + heatSubMatPast[i-1][j+1] + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]   + heatSubMatPast[i][j+1]   + 
                            0 + 0 + 0   ) / DIVISOR;
                }



            }/* end of colm for loop */

        } else {
            DLOG (C_VERBOSE, "procRank != lastNode. Receiving last row from the successor node\n");

            /* receive the lower row from the successor node and compute  */
            MPI_Recv (&receivedRow[0], heatSubMatColmSize, MPI_DOUBLE, procRank+1, HEAT_MAT_SENDER_FIRST_ROW, MPI_COMM_WORLD, &Status[1]);

            i = lastRow;
#if (DEBUG)            
            DLOG (C_VERBOSE, "Node[%d] Printing receivedRow \n",procRank);
            printRow (receivedRow, heatSubMatColmSize);
#endif

            for (j = 0; j < heatSubMatColmSize; j++){

                if (j == 0 ){
                    /* for first column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( 0 + heatSubMatPast[i-1][j] + heatSubMatPast[i-1][j+1] + 
                            0 + heatSubMatPast[i][j]  + heatSubMatPast[i][j+1]   + 
                            0 + receivedRow[j] + receivedRow[j+1]   )/ DIVISOR;

                } else if ( j == heatSubMatColmSize-1) {

                    /* for last column the  undefined values of H(t-1) are assumed to be 0 */
                    heatSubMatCur[i][j] = ( heatSubMatPast[i-1][j-1] + heatSubMatPast[i-1][j] + 0 + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]  + 0 +  
                            receivedRow[j-1] + receivedRow[j] + 0   )/ DIVISOR;


                } else {

                    heatSubMatCur[i][j] = ( heatSubMatPast[i-1][j-1] + heatSubMatPast[i-1][j] + heatSubMatPast[i-1][j+1] + 
                            heatSubMatPast[i][j-1]   + heatSubMatPast[i][j]  + heatSubMatPast[i][j+1]   + 
                            receivedRow[j-1] + receivedRow[j] + receivedRow[j+1]  )/ DIVISOR;
                }

            }/* end of colm for loop */

        }


        /* since the buffer is being reused immediately, we would wait for all the async transfers to complete
         * so that we can proceed to the next iteration
         */
        if (procRank == firstNode) {
            DLOG (C_VERBOSE, "Node[%d] waiting for the MPI_Isend to complete\n", procRank);
            MPI_Wait (&SendReq[1], MPI_STATUS_IGNORE);

        } else if (procRank == lastNode) {
            DLOG (C_VERBOSE, "Node[%d] waiting for the MPI_Isend to complete\n", procRank);
            MPI_Wait (&SendReq[0], MPI_STATUS_IGNORE);

        } else {
            DLOG (C_VERBOSE, "Node[%d] waiting for the MPI_Isend to complete\n", procRank);
            MPI_Waitall (2, &SendReq[0], MPI_STATUS_IGNORE);
        }

        DLOG (C_VERBOSE, "Node[%d] Copying heatSubMatCur to heatSubMatPast\n",procRank);
        for (i = 0; i < heatSubMatRowSize ; i++){
            memcpy ( &heatSubMatPast[i][0], &heatSubMatCur[i][0], heatSubMatColmSize * sizeof(double));
        }

#if (DEBUG)
        DLOG (C_VERBOSE, "Node[%d] Printing heatSubMatCur\n",procRank);
        printMatrix (heatSubMatCur, heatSubMatRowSize, heatSubMatColmSize);
#endif


    }/* end of loop to compute 20 iterations */

    MPI_Barrier( MPI_COMM_WORLD ) ;
    /* compute the time taken for the computation */
    if (procRank == NODE_0){
        EndTime = std::chrono::system_clock::now();
        ElapsedTime = EndTime - StartTime;

        std::cerr<<ElapsedTime.count()<<std::endl;
    }
    MPI_Barrier( MPI_COMM_WORLD ) ;



    for (i = 0; i < heatSubMatRowSize; i++){
        delete [] heatSubMatCur[i];
    }
    delete [] heatSubMatCur;

    for (i = 0; i < heatSubMatRowSize; i++){
        delete [] heatSubMatPast[i];
    }
    delete [] heatSubMatPast;
    delete [] receivedRow;


    MPI_Type_free(&lastRowSubArray);
    MPI_Type_free(&firstRowSubArray);
    MPI_Finalize();

    return 0;
}


/*==============================================================================
 *  InitHeatMatrix
 *=============================================================================*/

void InitHeatMatrix (double *** heatSubMatCur, int heatSubMatRowSize, int heatSubMatColmSize, int indexValue) {

    int i,j;
    double ** heatMat = new double * [heatSubMatRowSize];

    (*heatSubMatCur) = heatMat;
    int procRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    DLOG (C_VERBOSE, "Node[%d] Enter\n",procRank);

    for (i = 0; i < heatSubMatRowSize; i++){
        heatMat[i] = new double [heatSubMatColmSize];
        (*heatSubMatCur)[i] = heatMat[i];
    }

    if (indexValue == 0) {
        /* set the array elements to 0, use memset */
        for (i = 0; i < heatSubMatRowSize ; i++ ) {
            for ( j = 0; j < heatSubMatColmSize ; j++ ) {
                heatMat[i][j] = 0; 
            }/* end of colm for loop */
        }/* end of row for loop */

    } else if (indexValue == 1) {
        /* set elements in a particular order */

        for (i = 0; i < heatSubMatRowSize ; i++ ) {
            for ( j = 0; j < heatSubMatColmSize ; j++ ) {


                if ( (i+j) % 2 == 0) {
                    heatMat[i][j] = 1; 
                } else {
                    heatMat[i][j] = 0; 
                }


            }/* end of colm for loop */
        }/* end of row for loop */

    } else if (indexValue == 2) {
        /* generate elements randomly  */
        for (i = 0; i < heatSubMatRowSize ; i++ ) {
            for ( j = 0; j < heatSubMatColmSize ; j++ ) {


                if (i == j) {
                    heatMat[i][j] = 1; 
                } else {
                    heatMat[i][j] = 0; 
                }


            }/* end of colm for loop */
        }/* end of row for loop */


    }


    DLOG (C_VERBOSE, "Node[%d] Exit\n",procRank);
}

/*==============================================================================
 *  printMatrix
 *=============================================================================*/

void printMatrix(double **mat, int rows, int colms)
{
    int i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < colms; j++)
        {
            std::cout<<mat[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}


/*==============================================================================
 *  printRow
 *=============================================================================*/

void printRow(double *mat, int colms)
{
    int j;

    for (j = 0; j < colms; j++)
    {
        std::cout<<mat[j]<<" ";
    }
    std::cout<<std::endl;
}
