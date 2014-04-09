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
from icecube import icetray, dataio, dataclasses, simclasses, portia
from LumiProp import *
import glob


p_lumi = lumiProp("SC2")

import os

atwd_wave_name =  "CalibratedATWD"
fadc_wave_name =  "CalibratedFADC"
portia_event_name = "PortiaEventSummary"


# See what lumi we are after
lumi = "1" # %
reg  = 0
if len(sys.argv) > 1:
    lumi = str(sys.argv[1])
if len(sys.argv) > 2:
    reg = int(sys.argv[2])

# Input and output files
physFileList = 'SC2_100per_EHEClean_DOMcalib_WaveCalib.i3.gz'
dataDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/i3files/'
treeDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/trees/'
#outrootfile = treeDir + 'SC2_String54_55_waveform_filter'+lumi+'_reg'+str(reg)+'.root'
outrootfile = treeDir + 'SC2_String54_55_waveform_filter'+lumi+'.root'

load("libicetray")
load("libdataclasses")
load('libdataio')
load("libphys-services")
load("libsim-services")
load("libportia")
load("libwaveform-tree")

tray = I3Tray()
#file_list =  glob.glob(dataDir + physFileList)
#file_list.sort()
#file_list.insert(0,gcdFile)
file_list = []
file_list += [dataDir + physFileList]

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

tray.AddModule(checkLumi, "checkLumi")

#***************************************************************
# Going to add the cutwavetime method so I can check quickly
# two regions for 1% and 3% filter.
#***************************************************************
def cutwavetime(frame, Streams6=[icetray.I3Frame.DAQ]):
    if 'ATWDPortiaPulse' not in frame:
        print 'no pulse'
        return False

    pulses = frame['ATWDPortiaPulse']
    domlaunch = frame['CleanInIceRawData']
    calib_atwd = frame['CalibratedATWD']

    # Define the SC string doms
    near_sc = [OMKey(55,d) for d in range(37, 51)]

    # Check if have enough pulses.  400 is from
    # Aya's code, so I will check why...
    if len(pulses) < 400:
        return False

    
    counter = 0
    max_time_diff = 0.0
    min_amp = 1.0
    for omkey, portiapulse in pulses:
        if omkey in near_sc:

            # Load variables for plotting
            amplitude = portiapulse.GetAmplitude() / I3Units.volt
            letime = portiapulse.GetLETime()
            waveform_series = calib_atwd.get(omkey)

            # Add counters to make sure we have at least
            # 14 doms above some amplitude
            

            # Examine Time DIfference stuff
            for waveform in waveform_series:
                wavetime = waveform.time
                if wavetime < letime:
                    time_diff = (letime-wavetime) / I3Units.nanosecond
                    if time_diff > max_time_diff:
                        max_time_diff = time_diff
            
    if lumi == "1" and reg == 0:
        if max_time_diff <= 80: return True
        else:                   return False
    elif lumi == "1" and reg == 1:
        if max_time_diff > 80:  return True
        else:                   return False

    elif lumi == "3" and reg == 0:
        if max_time_diff <= 100: return True
        else:                    return False
    elif lumi == "3" and reg == 1:
        if max_time_diff > 100:  return True
        else:                    return False


    # Otherwise return false if not lumi == 1,3
    return False

#tray.AddModule(cutwavetime, "cutwavetime")

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
