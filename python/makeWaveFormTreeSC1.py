#!/usr/bin/env python

#########################################################
# Copying Shigeru's script so I can save the waveform.  #
# I will think of including this in the other filtering #
# script, but for now lets keep it simple               #
#########################################################

from I3Tray import *
from icecube.icetray import I3Units
from math import *
from os.path import expandvars
from icecube import icetray, dataio, dataclasses, simclasses
from SC1LumiReg import *
import glob

#eve_num = int(10)

p_lumi = lumiReg()

import os

use_reformed_pulse = False 
#use_reformed_pulse = True

atwd_wave_name =  "CalibratedATWD"
fadc_wave_name =  "CalibratedFADC"
portia_event_name = "PortiaEventSummary"

# See what lumi we are after
region = "0" # %
if len(sys.argv) > 1:
    region = str(sys.argv[1])

# Input and output files
physFileList = 'SC1_100per_EHEClean_DOMcalib_WaveCalib*.i3.gz'
dataDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/i3files/'
treeDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/trees/'
outrootfile = treeDir + 'SC1_String54_55_waveform_filter'+region+'.root'

load("libicetray")
load("libdataclasses")
load('libdataio')
load("libphys-services")
load("libsim-services")
load("libportia")
load("libwaveform-tree")

tray = I3Tray()
#icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level(icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit('I3WaveFormTreeMakerModule',
#                               icetray.I3LogLevel.LOG_DEBUG)

file_list =  glob.glob(dataDir + physFileList)
file_list.sort()
#file_list.insert(0,gcdFile)

print file_list
print dataDir
print physFileList

#***************************************************************
# Add method to check lumi
#***************************************************************
def checkLumi(frame, Streams1=[icetray.I3Frame.DAQ]):
    if 'I3EventHeader' not in frame:
        return False

    # Get time
    header = frame['I3EventHeader']
    stTime = header.start_time.utc_daq_time

    # Check if in range
    if p_lumi.timeInRange(region,stTime):
        return True
 
    # otherwise not in range
    return False


#*************************************************
tray.AddModule("I3Reader", "reader")(
    ("FileNameList",file_list)
    )

tray.AddModule(checkLumi, "checkLumi")

#***************************************************************
# Root Tree Maker
#***************************************************************
tray.AddModule("I3WaveFormTreeMakerModule","tree-maker")(
    ("outTreeName",            "WaveFormTree"),
    ("outFileName",            outrootfile),

    #("inDOMLaunchName",        "HLCCleanInIceRawData"),
    ("inDOMLaunchName",        "CleanInIceRawData"),
    ("PortiaEventName",        portia_event_name),
    ("ATWDWaveformName",       atwd_wave_name), # use calibration mode 1 */
    ("FADCWaveformName",       fadc_wave_name), # use calibration mode 1 */
    #("FilledKeys",[OMKey(40,22), OMKey(40,23)])
    ("FilledKeys",[ OMKey(40,d) for d in range (1,22)] + 
     [ OMKey(49,d) for d in range (1,22)])
    )

#tray.AddModule("I3EventCounter", "counter")(
#    ("NEvents", eve_num)
#    )
tray.AddModule("Dump","dump")
tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()
