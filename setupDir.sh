#!/bin/bash

###############################################
# This is a setup script that will create     #
# the necessary directory structure to save   #
# condor logs, trees, and i3files.  It is     #
# recommended to make sure you have enough    #
# space in the i3files and trees directory    #
# to accomodate 100s of GBs depending on your #
# running options. If you prefer to put them  #
# on someother disk, simply create a soft     #
# link.                                       #
###############################################

#*************************************#
# User specified variables
#*************************************#

SCLocation=/disk1/data/IceCube/RealData/86strings/standard-candle
GCDLocation=/disk1/data/IceCube/RealData/86strings/2012/GCDFiles

#
## Link GCDfiles and standard-candle dirs
#

ln -s ${SCLocation} run/standard-candle
ln -s ${GCDLocation} run/GCDFiles

#
## Make Condor log directories
#

mkdir run/outLogs
mkdir run/errLogs

#
## Make i3 and trees dir
#

mkdir i3files
mkdir trees
