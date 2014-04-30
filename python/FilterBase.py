#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# This will be where my filtering script will develop. I think what   #
# I will do is make this it's own class.  The user will then pass the #
# relevant parameters to this class and then call execute, which will #
# filter the data putting the output into i3files/ and data/ in the   #
# SCAna directory.                                                    #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

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

#from LumiProp import *
from ConfigOptions import *

#-----------------------------#
# Load libraries for modules
#-----------------------------#
I3Tray.load("libDomTools")
I3Tray.load("libophelia")
I3Tray.load("libportia")
I3Tray.load("libtree-maker")

#-----------------------------#
# Load the arguments and set
# configure options
#-----------------------------#

#
## Get arguments
#

m_config = ConfigOptions(sys.argv)
if not m_config.isValid():
    print "Configure options not set properly"
    print "See ConfigOptions.py for details"
    sys.exit()

#
## Define the tray
#
tray = I3Tray.I3Tray()

#
## Temporary
#
#histFile = TFile("histFile.root","recreate")
#h_dt = TH1F("h_dt","",100,0,500)

###############################################################
#                   USER FILTERING METHODS                    #
############################################################### 

#-----------------------------#
# Printing method 
#-----------------------------#
def userPrint(message):
    print "User Message: ", message
    return


#-----------------------------#
# Define cut on utc times
#-----------------------------#
def utctimes(frame, Streams3=[icetray.I3Frame.DAQ]):
    if 'I3EventHeader' not in frame:
        return False
    
    header     = frame['I3EventHeader']
    start_time = header.start_time
    daq_time   = start_time.utc_daq_time
    
    inRange = m_config.p_lumi.lumiInRange(m_config.lumi, daq_time)
    
    # if not in range, false
    if not inRange: return False
    
    # Check if it is in transition region
    inTR = m_config.p_lumi.inTRRegion(daq_time)
    if inTR: return False

    # Otherwise we are good
    return True

#-----------------------------#
# Cut on nDOMS
#-----------------------------#
def checkndom(frame, Streams5=[icetray.I3Frame.DAQ]):
    if 'CleanInIceRawData' not in frame:
        return False
    
    domlaunch = frame['CleanInIceRawData']
    
    if len(domlaunch) < m_config.nDOMReq: 
        return False
            
    # otherwise return true
    return True

#-----------------------------#
# Check on Ophelia
# This is on by default!!
#-----------------------------#
def opheliacheck(frame, Streams2=[icetray.I3Frame.DAQ]):
    if 'OpheliaFirstGuess1BTW' not in frame:
        return False
    return True
        
userPrint("Filtering Modules defined")

#-----------------------------#
# Defining waveform cut. Look
# at closest DOM to SC and 
# require dT(peak,end) of 
# waveform to be above some 
# threshold.
# NOTE: Right now this only
# works for SC2!!!!
#-----------------------------#
def wavetimeCut(frame, Streams1=[icetray.I3Frame.DAQ]):
    
    global h_dt

    # Currently only works for SC2, will
    # always be true for SC1
    if "SC1" in m_config.SC:
        return True
    
    # Only placing requirement for 30,
    # 51, and 100% filters
    lumis = ["30","51","100"]
    if m_config.lumi not in lumis:
        return True

    # Get ATWD and DOM launch info
    calib_atwd = frame['CalibratedATWD']
    cleanData  = frame['CleanInIceRawData']     
    
    # Loop over OM Keys and get calibrated ATWD
    nearestDOMs = [OMKey(55,d) for d in range(37,51)]
    maxLETime   = 0
    for key, domlaunchs in cleanData:
        for i in range(len(nearestDOMs)):

            if key == nearestDOMs[i]:
                # Get earliest dom launch
                domTime = sys.float_info.max
                passLC  = False     
                for launch in domlaunchs:
                    if launch.time < domTime:
                        domTime = launch.time
                        passLC  = launch.lc_bit
                
                # Not considering DOM when LCBit failed
                if not passLC:  continue
                if domTime < 0: continue

                # Now have start time. Loop over calibrated
                # waveforms and keep waveform that is closest
                # to domlaunch time.
                Waveforms = calib_atwd.get(key)
                for waveform in Waveforms:
                    if waveform.time <= domTime:
                        binSize  = waveform.binWidth
                        voltages = waveform.waveform
                        for j in range(len(voltages)):
                            Voltage = voltages[j]/I3Units.V
                            if m_config.p_lumi.lumiVThresh(m_config.lumi) < Voltage:
                                LETime = ((j * binSize))/I3Units.ns
                                if LETime > maxLETime:
                                    maxLETime = LETime
                                    break # done with loop since found waveform
                        break # done with loop since found waveform
    
    # end loop over DOMs

    if maxLETime > m_config.p_lumi.lumiTimeCut(m_config.lumi):
        return False

    return True


userPrint("wavetimeCut defined")


############################################################### 
#   BELOW ADD ALL NECESSARY ICECUBE MODULES AND TURN ON USER  #
#            MODULES BASED ON USER CONFIGURATION              #
############################################################### 

#-----------------------------#
# Add the reader
#-----------------------------#
fileList = []
fileList += [m_config.GCDFile]
fileList += [m_config.SCFile]
tray.AddModule("I3Reader", "Reader")(
    ("Filenamelist", fileList)
    )

userPrint("Reader added")

#-----------------------------#
# testing
#-----------------------------#
counter = 0
def count(frame,Streams2=[icetray.I3Frame.DAQ]):
    global counter
    if counter % 100 == 0:
        print "--------------------------------"
        print "Count: ", counter
    counter+=1

tray.AddModule(count, "count")

#-----------------------------#
# ADD UTC FILTER
#-----------------------------#
if m_config.cutUTC:
    tray.AddModule(utctimes,"utctimes")
    userPrint("Cutting on UTC times")

#-----------------------------#
# DOM Tools
# Will be first cleaning for doms
# by getting rid of bad DOMs
#-----------------------------#
tray.AddModule("I3OMSelection<I3DOMLaunchSeries>","BadDoms")(
    ("OmittedKeys",bad_dom_list_static.IC86_static_bad_dom_list()),
    ("OmittedStrings",[81, 82, 83, 84, 85, 86, 87]), #omit deep core strings
    ("OutputOMSelection","BadDomsListOffline"), 
    ("InputResponse","InIceRawData"),
    ("OutputResponse","CleanInIceRawData")
    )
userPrint("DOM Selection added")

#-----------------------------#
# ADD NDOM CHECK
#-----------------------------#
if m_config.cutNDOM:
    tray.AddModule(checkndom, "ndomcheck")
    userPrint("Cutting on number of DOMs")


#-----------------------------#
# Wave Calibrator
# This will calibrate the waves from
# the Cleaned data from above
#-----------------------------#
tray.AddModule("I3WaveCalibrator", "calibrator",
               Launches="CleanInIceRawData",
               Waveforms="CalibratedWaveforms",
               ATWDSaturationMargin=123, # 1023-900 == 123          
               FADCSaturationMargin=0,
               CorrectDroop=False
               )
userPrint("Wave Calibrator added")

#-----------------------------#
# Waveform splitter
# Maybe this breaks things into ATWD
# and FADC separately?
#-----------------------------#
tray.AddModule("I3WaveformSplitter", "split",
               Input="CalibratedWaveforms",
               HLC_ATWD = "CalibratedATWD",
               HLC_FADC = "CalibratedFADC",
               PickUnsaturatedATWD=True
               )
userPrint("Wave Splitter added")

# Place wavetime cut here.
tray.AddModule(wavetimeCut, "userWavetimeCut")

#-----------------------------#
# Adding Portia
# Portia will take the waveforms and
# actually calibrate them
#-----------------------------#
LEThresh = 0
if m_config.cutUTC: # if lumi cut is defined
    LEThresh = m_config.p_lumi.getLEThreshold(m_config.lumi) * I3Units.V
userPrint("Setting threshold: " + str(LEThresh))
tray.AddModule( "I3Portia", "Portia") (
    ( "DataReadoutName",            "CleanInIceRawData" ),
    ( "OutPortiaEventName",         "PortiaEventSummary" ),
    ( "MakeIceTopPulse",             False ),
    ( "ATWDPulseSeriesName",        "ATWDPulseSeries" ),
    ( "ATWDPortiaPulseName",        "ATWDPortiaPulse" ),
    ( "ATWDWaveformName",           "CalibratedATWD" ),
    ( "ATWDBaseLineOption",         "eheoptimized" ),
    ( "FADCBaseLineOption",         "eheoptimized" ),
    ( "ATWDThresholdCharge",        m_config.QThresh * I3Units.pC ),
    ( "ATWDLEThresholdAmplitude",   LEThresh ),
    ( "UseFADC",                    True ),
    ( "FADCPulseSeriesName",        "FADCPulseSeries" ),
    ( "FADCPortiaPulseName",        "FADCPortiaPulse" ),
    ( "FADCWaveformName",           "CalibratedFADC" ),
    ( "FADCThresholdCharge",        m_config.QThresh * I3Units.pC ),
    ( "FADCLEThresholdAmplitude",   LEThresh ),
    ( "MakeBestPulseSeries",        False ),
    ( "PMTGain",                    10000000),
    ) 
userPrint("Portia added")

#-----------------------------#
# Add Ophelia
# I think this will make a guess based
# on calibrated input whether the event 
# satisfies some EHE analysis criteria
#-----------------------------#
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
userPrint("EHEFirst Guess added")

#-----------------------------#
# Turn on Ophelia check
#-----------------------------#
tray.AddModule( opheliacheck, "checkreco")
userPrint("Cut on ophelia first guess")

#-----------------------------#
# Add ROOT tree maker
#-----------------------------#
tray.AddModule("I3TreeMakerModule","tree-maker")(
    ("outTreeName",             "RealTree"),
    ("outFileName",             m_config.TreeName),
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
userPrint("Treemaker added")

#-----------------------------#
# Add I3 writer to save the output
#-----------------------------#
tray.AddModule("I3Writer","writer")(
    ("filename", m_config.I3Name),
    ("SkipKeys", ["I3DAQData"]),
    #   ("sizelimit", 10**9), # 1G
    ("DropOrphanStreams", [ icetray.I3Frame.DAQ, icetray.I3Frame.Calibration ]),
    ("Streams", [icetray.I3Frame.Geometry, icetray.I3Frame.Calibration, icetray.I3Frame.DetectorStatus, icetray.I3Frame.DAQ, icetray.I3Frame.Physics] )
    )
userPrint("I3Output module added")

#-----------------------------#
# Clean up
#-----------------------------#
tray.AddModule("TrashCan", "bye_bye")
userPrint("Trash Can added")

#-----------------------------#
# Finish
#-----------------------------#
userPrint("Calling execute")
tray.Execute()
tray.Finish()

#-----------------------------#
# Print user message
#-----------------------------#
userPrint("Job is done.")

#histFile.Write()
#histFile.Close()
