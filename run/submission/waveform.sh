#!/bin/bash

infile="condor_files/waveformTemplate.cf"

#isData=1 # 1 for data, 0 for MC
isData=0 # 1 for data, 0 for MC

############################
# Data files
############################
if [ ${isData} -eq 1 ]
then

    nFiles=6
    arguments=(
	SC2_filter1_cutNDOM400_WaveCalib.i3.gz
	SC2_filter3_cutNDOM400_WaveCalib.i3.gz
	SC2_filter10_cutNDOM400_WaveCalib.i3.gz
	SC2_filter30_cutNDOM400_WaveCalib.i3.gz
	SC2_filter51_cutNDOM400_WaveCalib.i3.gz
	SC2_filter100_cutNDOM400_WaveCalib.i3.gz
    )
    tags=(
	Data_filter1
	Data_filter3
	Data_filter10
	Data_filter30
	Data_filter51
	Data_filter100
    )
	
fi

############################
# MC Files
############################
if [ ${isData} -eq 0 ]
then

    nFiles=5
    arguments=(
	#SC2_SpiceMie_filter1_TA0003.i3.gz
	SC2_SpiceMie_filter3_TA0003.i3.gz
	SC2_SpiceMie_filter10_TA0003.i3.gz
	SC2_SpiceMie_filter30_TA0003.i3.gz
	SC2_SpiceMie_filter51_TA0003.i3.gz
	SC2_SpiceMie_filter100_TA0003.i3.gz
    )
    tags=(
	#MC_filter1
	MC_filter3
	MC_filter10
	MC_filter30
	MC_filter51
	MC_filter100
    )

fi

############################
# Loop
############################
for (( i=0; i<${nFiles}; i++))
do

    cat ${infile} | sed 's/args/'${arguments[$i]}'/' | sed 's/tag/'${tags[i]}'/' > temp.cf
    condor_submit temp.cf
    rm temp.cf
done