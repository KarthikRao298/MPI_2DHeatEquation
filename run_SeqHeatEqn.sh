#!/bin/sh
# File Name       :run_SeHeatEqn.sh
# Description     :Script to run the sequential 2D heat equation
# Author          :Karthik Rao
# Date            :Dec 06 2017
# Version         :0.1

RESULTDIR=result/
h=`hostname`

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi
    
./seqHeatEqn ${N} 2> ${RESULTDIR}/seqHeatEqn_${N}  >/dev/null

