#!/bin/bash

infile="condor_files/filterDataTemplate.cf"

############################
# Pick the Standard Candle
############################

#SC="SC1"
#arguments=(0 1 2 3 4 5 6)

SC="SC2"
#arguments=(1 3 10 30 51 100)
arguments=(30 51 100)


############################
# Loop
############################
for arg in ${arguments[@]};
do

    cat ${infile} | sed 's/args/'${SC}' '${arg}'/' | sed 's/tag/'${SC}'_filter'${arg}'/' > temp.cf
    condor_submit temp.cf
    rm temp.cf

done