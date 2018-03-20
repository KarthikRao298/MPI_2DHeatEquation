#!/bin/sh
# File Name       :plotHeatWeak.sh
# Description     :Script to plot the speedup of 2D heat equation with weak scaling
# Author          :Karthik Rao
# Date            :Dec 06 2017
# Version         :0.1


#NS="12000 36000 80000"

PROCS="1"
INTENSITIES="1"
RESULTDIR=result/

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


# 1st plot, every node has 500MB of data

for INTENSITY in ${INTENSITIES};
do
	
	for PROC in ${PROCS}
	do  
	    FILE_2=${RESULTDIR}/heatEqnWeak_12000_2
	    if [ ! -f ${FILE_2} ]
	    then
		echo missing heatEqnWeak result file "${FILE_2}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_12000_2)
	    echo 2 ${partime}


	    FILE_4=${RESULTDIR}/heatEqnWeak_16000_4
	    if [ ! -f ${FILE_4} ]
	    then
		echo missing heatEqnWeak result file "${FILE_4}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_16000_4)
	    echo 4 ${partime}
        
	    FILE_8=${RESULTDIR}/heatEqnWeak_23000_8
	    if [ ! -f ${FILE_8} ]
	    then
		echo missing heatEqnWeak result file "${FILE_8}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_23000_8)
	    echo 8 ${partime}

	    FILE_16=${RESULTDIR}/heatEqnWeak_32000_16
	    if [ ! -f ${FILE_16} ]
	    then
		echo missing heatEqnWeak result file "${FILE_16}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_32000_16)
	    echo 16 ${partime}


	    FILE_32=${RESULTDIR}/heatEqnWeak_46016_32
	    if [ ! -f ${FILE_32} ]
	    then
		echo missing heatEqnWeak result file "${FILE_32}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_46016_32)
	    echo 32 ${partime}

	done > ${RESULTDIR}/time_heatEqnWeak_ni_500MB

	GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. 500MB at every node '; plot '${RESULTDIR}/time_heatEqnWeak_ni_500MB' u 1:2;"
done





# 2nd plot, every node has 1GB of data

for INTENSITY in ${INTENSITIES};
do
	
	for PROC in ${PROCS}
	do  
	    FILE_2=${RESULTDIR}/heatEqnWeak_16000_2
	    if [ ! -f ${FILE_2} ]
	    then
		echo missing heatEqnWeak result file "${FILE_2}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_16000_2)
	    echo 2 ${partime}


	    FILE_4=${RESULTDIR}/heatEqnWeak_23000_4
	    if [ ! -f ${FILE_4} ]
	    then
		echo missing heatEqnWeak result file "${FILE_4}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_23000_4)
	    echo 4 ${partime}
        
	    FILE_8=${RESULTDIR}/heatEqnWeak_32000_8
	    if [ ! -f ${FILE_8} ]
	    then
		echo missing heatEqnWeak result file "${FILE_8}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_32000_8)
	    echo 8 ${partime}

	    FILE_16=${RESULTDIR}/heatEqnWeak_46000_16
	    if [ ! -f ${FILE_16} ]
	    then
		echo missing heatEqnWeak result file "${FILE_16}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_46000_16)
	    echo 16 ${partime}


	    FILE_32=${RESULTDIR}/heatEqnWeak_64000_32
	    if [ ! -f ${FILE_32} ]
	    then
		echo missing heatEqnWeak result file "${FILE_32}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_64000_32)
	    echo 32 ${partime}

	done > ${RESULTDIR}/time_heatEqnWeak_ni_1GB

	GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. 1GB at every node '; plot '${RESULTDIR}/time_heatEqnWeak_ni_1GB' u 1:2;"
done




# 3rd plot, every node has 2GB of data

for INTENSITY in ${INTENSITIES};
do
	
	for PROC in ${PROCS}
	do  
	    FILE_2=${RESULTDIR}/heatEqnWeak_23000_2
	    if [ ! -f ${FILE_2} ]
	    then
		echo missing heatEqnWeak result file "${FILE_2}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_23000_2)
	    echo 2 ${partime}


	    FILE_4=${RESULTDIR}/heatEqnWeak_32000_4
	    if [ ! -f ${FILE_4} ]
	    then
		echo missing heatEqnWeak result file "${FILE_4}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_32000_4)
	    echo 4 ${partime}
        
	    FILE_8=${RESULTDIR}/heatEqnWeak_46000_8
	    if [ ! -f ${FILE_8} ]
	    then
		echo missing heatEqnWeak result file "${FILE_8}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_46000_8)
	    echo 8 ${partime}

	    FILE_16=${RESULTDIR}/heatEqnWeak_64000_16
	    if [ ! -f ${FILE_16} ]
	    then
		echo missing heatEqnWeak result file "${FILE_16}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_64000_16)
	    echo 16 ${partime}


	    FILE_32=${RESULTDIR}/heatEqnWeak_90016_32
	    if [ ! -f ${FILE_32} ]
	    then
		echo missing heatEqnWeak result file "${FILE_32}". Did you run qHeatWeak and wait for completion?
	    fi
	    partime=$(cat ${RESULTDIR}/heatEqnWeak_90016_32)
	    echo 32 ${partime}

	done > ${RESULTDIR}/time_heatEqnWeak_ni_2GB

	GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. 2GB at every node '; plot '${RESULTDIR}/time_heatEqnWeak_ni_2GB' u 1:2;"
done




gnuplot <<EOF
set terminal pdf
set output 'heat_weak_plots.pdf'

set style data linespoints

set key top left

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}



EOF

