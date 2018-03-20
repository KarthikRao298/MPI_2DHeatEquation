#!/bin/sh
# File Name       :queue_seqHeatEqn.sh
# Description     :Script to execute sequential 2D heat equation on cluster
# Author          :Karthik Rao
# Date            :Dec 06 2017
# Version         :0.1

#qsub -q mamba -l nodes=1:ppn=16 -d $(pwd) ./run_SeqHeatEqn.sh


#. ./params.sh

INTENSITIES="1"
#NS="1000 1000000 1000000000"
#these values are for strong scaling, for weak scaling they may have to be increased further
NS="12000 36000 80000 16000 23000 32000 46000 46016 64000 90016"

PROCS="2 4 8 16 32"

RESULTDIR=result/


if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

for INTENSITY in ${INTENSITIES};
do
    for N in ${NS};
    do	

	
	    FILE=${RESULTDIR}/seqHeatEqn_${N}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=1 -v N=${N} ./run_SeqHeatEqn.sh
	    fi

    done
done


