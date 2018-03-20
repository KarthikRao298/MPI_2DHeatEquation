#!/bin/sh
# File Name       :plotHeatStrong.sh
# Description     :Script to plot the speedup of 2D heat equation with strong scaling
# Author          :Karthik Rao
# Date            :Dec 06 2017
# Version         :0.1


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
	FILE=${RESULTDIR}/seqHeatEqn_${N}
	if [ ! -f ${FILE} ]
	then
	    echo missing sequential result file "${FILE}". Did you run queue_seqHeatEqn and wait for completion?
	fi

	seqtime=$(cat ${RESULTDIR}/seqHeatEqn_${N})
	
	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/heatEqnStrong_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing heatEqn result file "${FILE}". Did you run queue_HeatEqn and wait for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/heatEqnStrong_${N}_${PROC})
	    
	    echo ${PROC} ${seqtime} ${partime}
	done > ${RESULTDIR}/speedupHeatEqnStrong_${N}


	GNUPLOTSTRONG="${GNUPLOTSTRONG} set title 'strong scaling. n=${N} '; plot '${RESULTDIR}/speedupHeatEqnStrong_${N}' u 1:(\$2/\$3);"
    done


gnuplot <<EOF
set terminal pdf
set output 'heat_strong_plots.pdf'

set style data linespoints

set key top left

set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}


EOF
