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
from LumiProp import *
import glob


p_lumi = lumiProp()

import os

eve_num = int(10)

use_reformed_pulse = False 
#use_reformed_pulse = True

if not (use_reformed_pulse):
    #atwd_wave_name =  "CalibratedATWD_Wave"
    #fadc_wave_name =  "CalibratedFADC_Wav"e
    #portia_event_name = "PortiaEventSummaryStandard"
    atwd_wave_name =  "CalibratedATWD"
    fadc_wave_name =  "CalibratedFADC"
    portia_event_name = "PortiaEventSummary"
else:
    atwd_wave_name =  "ReformedATWD_Wave"
    fadc_wave_name =  "ReformedFADC_Wave"
    portia_event_name = "PortiaEventSummaryReformed"


#gcdFile = "GCDFiles/Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz"
#physFileList = "StandardCandle_2_Filtering_Run00120946_AllSubrunsMerged.i3.gz"
#dataDir = "standard-candle/sc2/"



# See what lumi we are after
lumi = "1" # %
if len(sys.argv) > 1:
    lumi = str(sys.argv[1])

# Input and output files
physFileList = 'SC2_100per_EHEClean_DOMcalib_WaveCalib*.i3.gz'
dataDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/i3files/'
treeDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/trees/'
outrootfile = treeDir + 'SC2_String54_55_waveform_filter'+lumi+'.root'

load("libicetray")
load("libdataclasses")
load('libdataio')
load("libphys-services")
load("libsim-services")
load("libportia")
load("libwaveform-tree")

tray = I3Tray()
file_list =  glob.glob(dataDir + 'SC2_100per_EHEClean_DOMcalib_WaveCalib.i3.gz')
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
    if p_lumi.lumiInRange(lumi,stTime):
        return True
 
    # otherwise not in range
    return False


#*************************************************
tray.AddModule("I3Reader", "reader")(
    ("FileNameList",file_list)
    )

#tray.AddModule(checkLumi, "checkLumi")

def check(frame, Streams2=[icetray.I3Frame.Physics]):
    if 'CalibratedATWD' not in frame:
        return False
    if 'CalibratedFADC' not in frame:
        return False
    if 'PortiaEventSummary' not in frame:
        return False
    return True

tray.AddModule(check,"check")


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
    ("FilledKeys",[ OMKey( 55, 37 ),          #
		    OMKey( 55, 38 ),          #
		    OMKey( 55, 39 ),          #
		    OMKey( 55, 40 ),          # 
		    OMKey( 55, 41 ),          # 
		    OMKey( 55, 42 ),          # Blackberry
		    OMKey( 55, 43 ),          # Hydrogen
		    OMKey( 55, 44 ),          # Aspudden
		    OMKey( 55, 45 ),          # Liljeholmen
		    OMKey( 55, 46 ),
		    OMKey( 55, 47 ),
		    OMKey( 55, 48 ),
		    OMKey( 55, 49 ),
		    OMKey( 55, 50 ),        # Meldi
                    OMKey( 54, 37 ),
		    OMKey( 54, 38 ),
		    OMKey( 54, 39 ),
		    OMKey( 54, 40 ),
		    OMKey( 54, 41 ),
		    OMKey( 54, 42 ),
		    OMKey( 54, 43 ),
		    OMKey( 54, 44 ),
		    OMKey( 54, 45 ),
		    OMKey( 54, 46 ),
		    OMKey( 54, 47 ),
		    OMKey( 54, 48 ),
		    OMKey( 54, 49 ),
		    OMKey( 54, 50 )]),
    
    )
#tray.AddModule("I3EventCounter", "counter")(
#    ("NEvents", eve_num)
#    )
tray.AddModule("Dump","dump")
tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()
