#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# This script will take the output after making the data/MC #
# trees using either FilterBase.py or SpiceMieGen.py. The   #
# user will need to specify the input i3 file.              #
# Right now currently considering two types of files:       #
#     * Data                                                #
#     * Simulation -- specifically SpiceMie, which must be  #
#                     in the name!!                         #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

#---------------------------#
# Import necessaries
#---------------------------#

from I3Tray import *
from icecube.icetray import I3Units
from math import *
from icecube import icetray, dataio, dataclasses, simclasses, portia
from LumiProp import *
import os

#---------------------------#
# Load Libraries
#---------------------------#
load("libicetray")
load("libdataclasses")
load('libdataio')
load("libphys-services")
load("libsim-services")
load("libportia")
load("libwaveform-tree")


#---------------------------#
# Read in the input file
#---------------------------#

argv      = sys.argv
pwd       = os.environ['PWD']
inputfile = pwd + "/../i3files/"
if len(argv) == 2:
    inputfile += str(argv[1])
else:
    print "The script expects the input file as an"
    print "argument. Either didn't specify, or you"
    print "passed too many arguments"
    print "Arguments:"
    print argv
    sys.exit()

#
## Check to make sure it is an i3 file
#
if "i3.gz" not in inputfile:
    print "File type must be i3.gz format"
    sys.exit()

#
## Decide if we have Data or Simulation
#
isMC = False
if "SpiceMie" in inputfile:
    isMC = True

#
## Generate the output file name
#

# Last entry is actual i3 file
fname = (inputfile.split("/")[-1]).split(".")[0]
SC    = fname.split("_")[0]

# Now build file name based on input
outName = fname + "_Waveform_tree"

#---------------------------#
# Set the calibrated names
#---------------------------#
atwd_wave_name    =  "CalibratedATWD"
fadc_wave_name    =  "CalibratedFADC"
portia_event_name = "PortiaEventSummary"
in_ice_rawdata       = "CleanInIceRawData"
if isMC:
    in_ice_rawdata = "HLCCleanInIceRawData"

#---------------------------#
# Specify the output file
# and input file
#---------------------------# 

outrootfile   = pwd + "/../" + outName + ".root"
inputFileList = [inputfile,] 


#---------------------------#
# Need a method to get 
# DOMs to make waveforms for
#---------------------------#
def getFilledKeys(SC):
    keys = []
    if SC == "SC2":
        for i in range(37,51):
            keys.append(OMKey(55,i))
            keys.append(OMKey(54,i))
    elif SC == "SC1":
        for i in range(1,23):
            keys.append(OMKey(40,i))
            keys.append(OMKey(29,i))
    else:
        print "The SC is not recognized"
        print "SC = ", SC
        print "Options are SC1 or SC2"
        sys.exit()

    return keys

filledKeys = getFilledKeys(SC)

print "Running for SC: ", SC
print "Keys: "
print filledKeys
print "ISMC: ", isMC
print "Outfile: ", outrootfile
print "input: ", inputFileList

##########################################################################
#        SIMPLE CODE TO TAKE THE FILTERED DATA TO THE WAVEFORMS          #
##########################################################################

#---------------------------#
# Define tray object
#---------------------------#
tray = I3Tray()

#---------------------------#
# Add I3Reader Module
#---------------------------#

tray.AddModule("I3Reader", "reader")(
    ("FileNameList",inputFileList)
    )

#---------------------------#
# Root Tree Maker
#---------------------------#
tray.AddModule("I3WaveFormTreeMakerModule","tree-maker")(
    ("outTreeName",            "WaveFormTree"),
    ("outFileName",            outrootfile),
    
    #("inDOMLaunchName",        "HLCCleanInIceRawData"),
    ("inDOMLaunchName",        in_ice_rawdata),
    ("PortiaEventName",        portia_event_name),
    ("ATWDWaveformName",       atwd_wave_name), # use calibration mode 1 */
    ("FADCWaveformName",       fadc_wave_name), # use calibration mode 1 */
    ("FilledKeys",             filledKeys)
    )

#---------------------------#
# Clean up
#---------------------------#
tray.AddModule("Dump","dump")
tray.AddModule("TrashCan", "the can")

#---------------------------#
# Run
#---------------------------#
tray.Execute()
tray.Finish()
