#!/usr/bin/env pythong

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
# This will be where my filtering script will develop. I thinks what #
# I will do is have it read in a text file that can set the relevant #
# parameters needed to execute, and then it will run.  The output    #
# will be written to i3files/ and data/ in the SCAna directory.      # 
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

#-----------------------------#
# Import necessaries
#-----------------------------#

import os, sys, glob
from icecube import icetray, dataio, dataclasses
import I3Tray
from icecube.icetray import I3Units
from icecube.icetray import OMKey
from icecube import portia
from icecube import WaveCalibrator
import numpy as n
from icecube.BadDomList import bad_dom_list_static

#
## User classes to make things more compact
#
from LumiProp import *

#-----------------------------#
# Load libraries for modules
#-----------------------------#

I3Tray.load("libDomTools")
I3Tray.load("libophelia")
I3Tray.load("libportia")
I3Tray.load("libtree-maker")

#-----------------------------#
# Get User Inputs
#-----------------------------#
argv = sys.argv
argc = len(argv)

# get inputs
if argc != 1:
    print "Usage script.py config.txt"
    sys.exit()

#-----------------------------#
# Initialize variables
#-----------------------------#

#
## I3Tray object
#
tray = I3Tray.I3Tray()

#
## Lumi thresholds and methods for
## user convenience
#
p_lumi = lumiProp()

#
## Specify output directories
#

rootoutdir = "../trees"
i3outdir   = "../i3files"


#-----------------------------#
# Specify input files and the
# output files
#-----------------------------#

#fileList =  ['GCDFiles/Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz']
#fileList += ['standard-candle/sc2/StandardCandle_2_Filtering_Run00120946_AllSubrunsMerged.i3.gz']
fileList =  ['GCDFiles/Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz']
fileList += ['standard-candle/sc1/StandardCandle_1_Filtering_Run00120946_AllSubrunsMerged.i3.gz']

#
## Now Check the luminosity to designate the 
## output fules and some constants
#
outTreeName = rootoutdir + "/SC1_nofilter" + lumi_arg + "_tree.root"
outI3Name   = i3outdir + "/SC1_nofilter" + lumi_arg + "_WaveCalib.i3.gz"
LEthres = 0 * I3Units.volt

#
## Specify the LEthres based on lumi
#

if lumi_arg == "1":     LEthres = 0.005 * I3Units.volt
elif lumi_arg == "3":   LEthres = 0.01  * I3Units.volt
elif lumi_arg == "10":  LEthres = 0.03  * I3Units.volt
elif lumi_arg == "30":  LEthres = 0.07  * I3Units.volt
elif lumi_arg == "51":  LEthres = 0.1   * I3Units.volt
elif lumi_arg == "100": LEthres = 0.28  * I3Units.volt

#########################################################################################
#        USER SHOULD NOT NEED TO EDIT BELOW HERE UNLESS TO TURN ON/OFF MODULES          # 
#########################################################################################

#-----------------------------#
# Filtering Methods
# These are turned off right now
#-----------------------------#

#
## Cut on UTC Times
#
def utctimes(frame, Streams3=[icetray.I3Frame.DAQ]):
    if 'I3EventHeader' not in frame:
        return False
    
    header     = frame['I3EventHeader']
    start_time = header.start_time
    daq_time   = start_time.utc_daq_time

    inRange = p_lumi.lumiInRange(lumi_arg, daq_time)

    # if not in range, false
    if not inRange: return False
    
    # Otherwise we are good
    return True

#
## Cut on the # Doms that have launched
#
def checkndom(frame, Streams5=[icetray.I3Frame.DAQ]):
    if 'CleanInIceRawData' not in frame:
        return False

    domlaunch = frame['CleanInIceRawData']

    if len(domlaunch) < 400: #TODO: Hardcoded values into python file
        return False

#
## Cut on wave time
#
## TODO: Put hardcoded values into python file
## Add cutime later, it clutters up right now
#def cutwavetime(frame, Streams6=[icetray.I3Frame.DAQ]):


#-----------------------------#
# Add modules
#-----------------------------#

#
## Add the reader
#
tray.AddModule("I3Reader", "Reader")(
    ("Filenamelist", filelist)
    )

#
## DOM Tools
## Will be first cleaning for doms
## by getting rid of bad DOMs
#
tray.AddModule("I3OMSelection<I3DOMLaunchSeries>","BadDoms")(
    ("OmittedKeys",bad_dom_list_static.IC86_static_bad_dom_list()),
    ("OmittedStrings",[81, 82, 83, 84, 85, 86, 87]), #all deep core strings excluded for simplification 
    ("OutputOMSelection","BadDomsListOffline"), # created IC79 list appended to the IC40 list above
    ("InputResponse","InIceRawData"),
    ("OutputResponse","CleanInIceRawData")
    )

#
## Wave Calibrator
## This will calibrate the waves from
## the Cleaned data from above
#
tray.AddModule("I3WaveCalibrator", "calibrator",
               Launches="CleanInIceRawData",
               Waveforms="CalibratedWaveforms",
               ATWDSaturationMargin=123, # 1023-900 == 123          
               FADCSaturationMargin=0,
               CorrectDroop=False
               )

#
## Waveform splitter
## Maybe this breaks things into ATWD
## and FADC separately?
#
tray.AddModule("I3WaveformSplitter", "split",
               Input="CalibratedWaveforms",
               HLC_ATWD = "CalibratedATWD",
               HLC_FADC = "CalibratedFADC",
               PickUnsaturatedATWD=True
               )

#
## Adding Portia
## Portia will take the waveforms and
## actually calibrate them
#
tray.AddModule( "I3Portia", "Portia") (
    ( "DataReadoutName",            "CleanInIceRawData" ),
    ( "OutPortiaEventName",         "PortiaEventSummary" ),
    ( "MakeIceTopPulse",             False ),
    ( "ATWDPulseSeriesName",        "ATWDPulseSeries" ),
    ( "ATWDPortiaPulseName",        "ATWDPortiaPulse" ),
    ( "ATWDWaveformName",           "CalibratedATWD" ),
    ( "ATWDBaseLineOption",         "eheoptimized" ),
    ( "FADCBaseLineOption",         "eheoptimized" ),
    ( "ATWDThresholdCharge",        0.1 * I3Units.pC ),
    ( "ATWDLEThresholdAmplitude",   LEthres ),
    ( "UseFADC",                    True ),
    ( "FADCPulseSeriesName",        "FADCPulseSeries" ),
    ( "FADCPortiaPulseName",        "FADCPortiaPulse" ),
    ( "FADCWaveformName",           "CalibratedFADC" ),
    ( "FADCThresholdCharge",        0.1 * I3Units.pC ),
    ( "FADCLEThresholdAmplitude",   LEthres ),
    ( "MakeBestPulseSeries",        False ),
    ( "PMTGain",                    10000000),
    ) 

#
## Add Ophelia
## I think this will make a guess based
## on calibrated input whether the event 
## satisfies some EHE analysis criteria
#
tray.AddModule("I3EHEFirstGuess","reco")(       
        ("MinimumNumberPulseDom",   8),
        ("InputLaunchName",        "CleanInIceRawData"),
        ("OutputFirstguessName",   "OpheliaFirstGuess1"),
        ("OutputFirstguessNameBtw","OpheliaFirstGuess1BTW"),
        ("InputPulseName1",        "ATWDPortiaPulse"),
        ("InputPulseName2",        "FADCPortiaPulse"),
        ("ChargeOption",            0),
        ("LCOption",                True),
        ("InputPortiaEventName",  "PortiaEventSummary"),
        ("NPEThreshold",         0.0)
)

#
## Turn on Ophelia check
#
tray.AddModule( checkophelia, "checkreco")

#
## Add ROOT tree maker
#
tray.AddModule("I3TreeMakerModule","tree-maker")(
        ("outTreeName",             "RealTree"),
        ("outFileName",             outrootfile),
        #       
        ("doJulietTree",           False),
        ("doNuGenTree",            False),
        #
        ("doDetectorTree",          True),
        ("inDOMLaunchName",         "CleanInIceRawData"),
        ("doPulseChannelTree",      True),
        ("inAtwdPortiaName",        "ATWDPortiaPulse"),
        ("inFadcPortiaName",        "FADCPortiaPulse"),
        ("inPortiaEventName",       "PortiaEventSummary"),
        ("doWaveformDraw",           False),
        #
        ("doFirstguessTree",        True),
        ("inFirstguessName",        "OpheliaFirstGuess1"),
        ("inFirstguessNameBtw",     "OpheliaFirstGuess1BTW"),
        #
        ("doMMCTree",               False),
        #
        ("doEheStarTree",           False),
        ("doTopTree",               False)
)

#
## Add I3 writer to save the output
#
tray.AddModule("I3Writer","writer")(
    ("filename", outi3file),
    ("SkipKeys", ["I3DAQData"]),
#   ("sizelimit", 10**9), # 1G
    ("DropOrphanStreams", [ icetray.I3Frame.DAQ, icetray.I3Frame.Calibration ]),
    ("Streams", [icetray.I3Frame.Geometry, icetray.I3Frame.Calibration, icetray.I3Frame.DetectorStatus, icetray.I3Frame.DAQ, icetray.I3Frame.Physics] )
   )

#
## Clean up
#
tray.AddModule("TrashCan", "bye_bye")

#
## Finish
#
tray.Execute()
tray.Finish()
