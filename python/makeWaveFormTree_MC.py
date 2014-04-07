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
import glob
import os

eve_num = int(10)

atwd_wave_name =  "CalibratedATWD"
fadc_wave_name =  "CalibratedFADC"
portia_event_name = "PortiaEventSummary"


# See what lumi we are after
lumi = "1" # %
if len(sys.argv) > 1:
    lumi = str(sys.argv[1])

# Input and output directory
dataDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna/i3files/'
treeDir = '/home/mrelich/workarea/icecube/standardcandle/ehe_v04_04_00RC1/build/SCAna_nofilter/trees/'

# Specify input and output files
outrootfile  = treeDir
physFileList = dataDir
if lumi == "1":
    physFileList += "SC2_spicemie_hole100cm_1per.i3*.gz"
    outrootfile  += "SC2_spicemie_1per_waveform.root"
elif lumi == "3":
    physFileList += "SC2_spicemie_hole100cm_3per.i3*.gz"
    outrootfile  += "SC2_spicemie_3per_waveform.root"
elif lumi == "10":
    physFileList += "SC2_spicemie_hole100cm_10per.i3*.gz"
    outrootfile  += "SC2_spicemie_10per_waveform.root"
elif lumi == "30":
    physFileList += "SC2_spicemie_hole100cm_30per.i3*.gz"
    outrootfile  += "SC2_spicemie_30per_waveform.root"
elif lumi == "51":
    physFileList += "SC2_spicemie_hole100cm_51per.i3*.gz"
    outrootfile  += "SC2_spicemie_51per_waveform.root"
elif lumi == "100":
    physFileList += "SC2_spicemie_hole100cm_100per.i3*.gz"
    outrootfile  += "SC2_spicemie_100per_waveform.root"
else:
    print "Lumi is not supported"
    sys.exit()

load("libicetray")
load("libdataclasses")
load('libdataio')
load("libphys-services")
load("libsim-services")
load("libportia")
load("libwaveform-tree")

tray = I3Tray()
file_list =  glob.glob(physFileList)
file_list.sort()

print file_list
print dataDir
print physFileList

#*************************************************
tray.AddModule("I3Reader", "reader")(
    ("FileNameList",file_list)
    )

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
