#!/bin/sh
# File Name       :runHeatStrong.sh
# Description     :Script to execute strong scaling of heat equation
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
    
mpirun ./heatEqn ${N} 2> ${RESULTDIR}/heatEqnStrong_${N}_${PROC}  >/dev/null


