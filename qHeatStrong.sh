#!/bin/sh
# File Name       :qHeatStrong.sh
# Description     :Script to execute strong scaling of 2D heat equation on cluster
# Author          :Karthik Rao
# Date            :Dec 06 2017
# Version         :0.1

#. ./params.sh


NS="12000 36000 80000"

PROCS="2 4 8 16 32"

RESULTDIR=result/



if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

    for N in ${NS};
    do	

	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/heatEqnStrong_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./runHeatStrong.sh
	    fi

	done

    done
