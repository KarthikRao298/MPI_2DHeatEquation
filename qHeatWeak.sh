#!/bin/sh
# File Name       :qHeatWeak.sh
# Description     :Script to execute weak scaling of 2D heat equation on cluster
# Author          :Karthik Rao
# Date            :Dec 06 2017
# Version         :0.1

# weak scaling

RESULTDIR=result/
if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


# to generate values for p=2, N=12k, 16K, 23K

NS_2="12000 16000 23000"
PROCS_2="2"


    for N in ${NS_2};
    do	

	for PROC in ${PROCS_2}
	do
	
	    FILE=${RESULTDIR}/heatEqnWeak_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./runHeatWeak.sh
	    fi

	done

    done


# to generate values for p=4, N=16K, 23K, 32K

NS_4="16000 23000 32000"
PROCS_4="4"


    for N in ${NS_4};
    do	

	for PROC in ${PROCS_4}
	do
	
	    FILE=${RESULTDIR}/heatEqnWeak_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./runHeatWeak.sh
	    fi

	done

    done




# to generate values for p=8, N=23K, 32K, 46K

NS_8="23000 32000 46000"
PROCS_8="8"


    for N in ${NS_8};
    do	

	for PROC in ${PROCS_8}
	do
	
	    FILE=${RESULTDIR}/heatEqnWeak_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./runHeatWeak.sh
	    fi

	done

    done
    


# to generate values for p=16, N=32k, 46K, 64K

NS_16="32000 46000 64000"
PROCS_16="16"


    for N in ${NS_16};
    do	

	for PROC in ${PROCS_16}
	do
	
	    FILE=${RESULTDIR}/heatEqnWeak_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./runHeatWeak.sh
	    fi

	done

    done


# to generate values for p=32, N=44k, 64K, 90K

NS_32="46016 64000 90016"
PROCS_32="32"


    for N in ${NS_32};
    do	

	for PROC in ${PROCS_32}
	do
	
	    FILE=${RESULTDIR}/heatEqnWeak_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./runHeatWeak.sh
	    fi

	done

    done
    
