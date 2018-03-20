/*
 * File Name       :seqHeatEqn.cpp
 * Description     :code to simulate sequential 2D heat equation
 * Author          :Karthik Rao
 * Date            :Dec 06 2017
 * Version         :0.1
 *
 * To compile :
 * Make seqHeatEqn
 *
 * To compile :
 * g++ -std=c++11 seqHeatEqn.cpp -o seqHeatEqn
 * 
 * Sample command line execution :
 * 
 * g++ -n 3 ./seqHeatEqn  9
 *
 *
 * qsub -d $(pwd) -q mamba -l procs=2 -v mpirun -n 2 ./seqHeatEqn  100
 *
 */

/* Debug prints will be enabled if set to 1 */
#define DEBUG 0
/* This macro is added to simplify debugging of the code */
#define DIVISOR 9

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



    int heatMatSize  = atoi (argv[1]);

    int heatSubMatColmSize, heatSubMatRowSize;
    int firstRow, lastRow;
    int procRank = 0;

    double ** heatSubMatPast;
    double ** heatSubMatCur;
    heatSubMatColmSize = heatMatSize;
    heatSubMatRowSize = heatMatSize;

    int i,j;
    /* measure time taken for integration */
    std::chrono::time_point<std::chrono::system_clock> StartTime;
    std::chrono::time_point<std::chrono::system_clock>  EndTime;
    std::chrono::duration<double> ElapsedTime;

    StartTime = std::chrono::system_clock::now();


    InitHeatMatrix (&heatSubMatPast, heatSubMatRowSize, heatSubMatColmSize, 1);
    InitHeatMatrix (&heatSubMatCur, heatSubMatRowSize, heatSubMatColmSize, 0);

    firstRow = 0 ;
    lastRow  = heatSubMatRowSize - 1;

    /* allocate memory for the received rows */
    double * receivedRow = new double [heatSubMatColmSize];


    DLOG (C_VERBOSE, "Node[%d] firstRow  = %d. lastRow  = %d\n",procRank, firstRow, lastRow);
    DLOG (C_VERBOSE, "Node[%d] heatSubMatRowSize  = %d. heatSubMatColmSize  = %d\n",procRank, heatSubMatRowSize, heatSubMatColmSize);

    /* end of first row subarray data type creation */


    for (int k = 0; k < 20 ; k++) {

#if (DEBUG)
        DLOG (C_VERBOSE, "Node[%d] Printing heatSubMatPast\n",procRank);
        printMatrix (heatSubMatPast, heatSubMatRowSize, heatSubMatColmSize);
#endif

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
         * so the heat equation is modified accordingly
         */
        {
            DLOG (C_VERBOSE, "Node[%d] procRank = 0, so assuming 0 for ghost rows\n",procRank);

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

        }

        /* compute last row . if its a last node then the undefined values of H(t-1) are assumed to be 0 */
        {
            DLOG (C_VERBOSE, "procRank = 0. computing last row\n");

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

    /* compute the time taken to compute the sum and display the same */
    EndTime = std::chrono::system_clock::now();
    ElapsedTime = EndTime - StartTime;

    std::cerr<<ElapsedTime.count()<<std::endl;

    for (i = 0; i < heatSubMatRowSize; i++){
        delete [] heatSubMatCur[i];
    }
    delete [] heatSubMatCur;

    for (i = 0; i < heatSubMatRowSize; i++){
        delete [] heatSubMatPast[i];
    }
    delete [] heatSubMatPast;
    delete [] receivedRow;




    return 0;
}


/*==============================================================================
 *  InitHeatMatrix
 *=============================================================================*/

void InitHeatMatrix (double *** heatSubMatCur, int heatSubMatRowSize, int heatSubMatColmSize, int indexValue) {

    int i,j;
    double ** heatMat = new double * [heatSubMatRowSize];

    (*heatSubMatCur) = heatMat;
    int procRank = 0;

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


                if ( (i+j) % 2 == 0) {
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

