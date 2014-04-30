#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
# MC simulation for Standard candle. This method will create an instance #
# of MCoptions.py class and then generate MC using the specifications in #
# that class.  If you require more options, please add them to MCOptions #
# and incorporate their use into this script. This will help keep one    #
# place where one can modify the parameters needed to edit this code.    #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

#-----------------------------#
# Import necessaries
#-----------------------------#

from I3Tray import *
from math import *
from icecube import icetray, dataclasses, dataio, WaveCalibrator
from icecube import phys_services
from icecube.WaveCalibrator import DOMSimulatorCalibrator
from icecube.sim_services.sim_utils.gcd_utils import get_time
import os, sys

from icecube.BadDomList import bad_dom_list_static

#
## User classes to make things more comparct
#

from MCOptions import *
from PastConfigs import *

#-----------------------------#
# Load Libraries for modules
#-----------------------------#
load("libicetray")
load("libdataclasses")
load("libphys-services")
load("libdataio")
load("libc2j-icetray")
load("libromeo")
load("libromeo-interface")
load("libI3Db")
load("libDOMsimulator")
load("libSC-generator")
load("libbrutus")
load("libDomTools")
load("libportia")
load("libtree-maker")
load("libtrigger-sim")
load('wavedeform')
load("libphotonics-service")

#-----------------------------#
# Get arguments and configure
# the MC Options
#-----------------------------#

m_MCOpt = MCOptions(sys.argv)

###############################################################
#                   USER DEFINED METHODS                      #
###############################################################

#-----------------------------#
# User printing method
#-----------------------------#
def userPrint(message):
    print "User Message: ", message
    return

#-----------------------------#
# Add driving time. Not sure
# what this is for...
#-----------------------------#
def DrivingTime( frame ):
	if "DrivingTime" in frame :
		del frame["DrivingTime"]
	frame.Put("DrivingTime", time )

###############################################################
# BELOW ADD ALL OF THE NECESSARY ICECUBE MODULES IN ORDER TO  #
#           GENERATE STANDARD CANDLE SIMULATION               #
###############################################################

#
## Create instance of I3Tray
#
tray = I3Tray()


#-----------------------------#
# Add Random number serivce
#-----------------------------#
tray.AddService("I3SPRNGRandomServiceFactory","random")(
        ("Seed",m_MCOpt.seed),    # Seed for random number generator
        ("NStreams",500),         # Number of streams used in cluster
        ("StreamNum",1)           # Thread number for this generator
	)

userPrint("Added random services")

#-----------------------------#
# Add Photonic information
#-----------------------------#
tray.AddService('I3PhotoSplineServiceFactory', 'photonics-service',
                AmplitudeTable='/misc/home/mase/work/I3/analysis/SC/process/photonics_tables/v173/SC2_SPICEMIE/data/new/sc2_mie.absfits',
		TimingTable='/misc/home/mase/work/I3/analysis/SC/process/photonics_tables/v173/SC2_SPICEMIE/data/new/sc2_mie.probfits',
                TimingSigma=0)

userPrint("Added photonic information")

#-----------------------------#
# Source with geometry
#-----------------------------#
tray.AddModule("I3InfiniteSource", "somanyevents",
               Prefix= m_MCOpt.GCDFile,
               Stream=icetray.I3Frame.DAQ)

userPrint("Added Geometry (GCD) file")

#-----------------------------#
# Timing module
#-----------------------------#
time = get_time( dataio.I3File(m_MCOpt.GCDFile))
tray.AddModule( DrivingTime, "dt",
                Streams = [icetray.I3Frame.DAQ])

userPrint("Timing info included")

#-----------------------------#
# Generate event header info
#-----------------------------#
tray.AddModule("I3MCEventHeaderGenerator","time-gen")(
    ("Year",time.utc_year),
    ("DAQTime",time.utc_daq_time)
    )

userPrint("Event header infor added")

#-----------------------------#
# SC-generator 
#-----------------------------#
tray.AddModule("I3SCGenerator","generator")(
    ("NumPhotons", m_MCOpt.SC.intensity), 
    ("xPosition",m_MCOpt.SC.SCpos[0]*I3Units.m),
    ("yPosition",m_MCOpt.SC.SCpos[1]*I3Units.m),
    ("zPosition",m_MCOpt.SC.SCpos[2]*I3Units.m)
    )


userPrint("SC properties have been set")

#-----------------------------#
# Add hit maker (Brutus)
#-----------------------------#
tray.AddModule("I3HitMakerModuleEHE","hit-maker")(
    ("MCTreeName","I3MCTree"),
    ("MaxMCHits", 500000) # standard EHE analysis = 500k,  special analysis = 50000k   
    )

userPrint("Hit maker set")


#-----------------------------#
# romeo-interface 
#-----------------------------#
tray.AddModule("I3RomeoInterfaceModule","romeo")(
    ("SkipPhotoCathodeSimulator", False),          # Do the 2D photocathod simulation
    ("StoreFakeRecoPulseMap", False),              # skip to store IceCubePMTPulse 
    ("RomeoOutputTimeResolution", 10.*I3Units.ns), # same MCHitBinSize of I3MCHitCompressor
    ("RomeoSquarePulseThreshold",10),              # output squqre pulse when 10 p.e. or more
    ("StoreRomeoInfo",False),                      # Do not store the Romeo info.   
    ("ModelPMTname",m_MCOpt.GDOMName)
    )

userPrint("Romeo interface setup")

#-----------------------------#
# Simulate DOM response
#-----------------------------#
tray.AddModule("I3DOMsimulator","domsimulator")(
    ("SimulatePedestalDroop", True)           # Simulate droop, ie. dip below zero
    )


#-----------------------------#
# Clean out "Bad Doms"
#-----------------------------#

tray.AddModule("I3DOMLaunchCleaning","BadDomCleaning")(
    ("InIceInput", "InIceRawData"),
    ("IceTopInput", "IceTopRawData"),
    ("InIceOutput", "CleanInIceRawData"),
    ("IceTopOutput", "CleanIceTopRawData"),
    ("FirstLaunchCleaning",False),
    ("CleanedKeysList",""),
    ("IcePickServiceKey",""),
    ("CleanedKeys",bad_dom_list_static.IC86_static_bad_dom_list())
    )



userPrint("First round of cleaning added")

#-----------------------------#
# Clean the launches
# TODO: Look at what this does!!
#-----------------------------#
cleanedKeys = [] # empty for IC86
if m_MCOpt.StringConfig == "IC79":
    cleanedKeys = backToIC79()

tray.AddModule("I3DOMLaunchCleaning","LaunchCleaning")(
    ("InIceInput", "CleanInIceRawData"),
    ("IceTopInput", "CleanIceTopRawData"),
    ("InIceOutput", "CleanInIceRawDataFLC"),
    ("IceTopOutput", "CleanIceTopRawDataFLC"),
    ("FirstLaunchCleaning",True),
    ("CleanedKeysList",""),
    ("IcePickServiceKey",""),
    ("CleanedKeys",cleanedKeys)
    )

userPrint("Second round of cleaning added")

#-----------------------------#
# Simulate trigger, although
# I don't think this is needed
#-----------------------------#
tray.AddModule("SimpleMajorityTrigger","smtrigger") (
    ("DataReadoutName", "CleanInIceRawDataFLC"),
    ("TriggerConfigID", 1006)
    )

tray.AddModule("I3GlobalTriggerSim","globaltrigger")(
    ("I3DOMLaunchSeriesMapNames", ["CleanInIceRawDataFLC", "CleanIceTopRawDataFLC"])
    )

userPrint("Trigger packages added")

#-----------------------------#
# Add pruner... 
# TODO: Look this module up
#-----------------------------#
tray.AddModule("I3Pruner","pruner")(
    ("DOMLaunchSeriesMapNames", ["CleanInIceRawDataFLC", "CleanIceTopRawDataFLC"])
    )

userPrint("Pruner added")

#-----------------------------#
# Use time shifter
# TODO: Look this module up
#-----------------------------#
tray.AddModule("I3TimeShifter","timeshifter")(
    ("I3DoubleNames",["dmadd_multtime","dmadd_stringmulttime"]),
    ("I3DOMLaunchSeriesMapNames",["InIceRawData","IceTopRawData","CleanInIceRawData","CleanIceTopRawData","CleanInIceRawDataFLC","CleanIceTopRawDataFLC"]),
    ("I3MCPMTResponseMapNames",["MCPMTResponseMap"]),
    ("I3MCTreeNames",["I3MCTree"]),
    ("I3MCHitSeriesMapNames",["MCHitSeriesMap"]),
    )

userPrint("Time shifter")

#-----------------------------#
# Final HLC Cleaning
# TODO: Should I add this to data too?
#-----------------------------#
tray.AddModule("I3LCCleaning","InIceLCClean")(
    ("IcePickServiceKey",""),
    ("InIceInput", "CleanInIceRawDataFLC"),
    ("InIceOutput", "HLCCleanInIceRawData"),
    ("InIceOutputSLC", "SLCInIceRawData")   #SLC                                       
    )

userPrint("Local Coincidence cleaning added")

#-----------------------------#
# DOM Calibrator
#-----------------------------#
tray.AddSegment(DOMSimulatorCalibrator, 'calibrator',
                Launches='HLCCleanInIceRawData', 
		Waveforms='CalibratedWaveforms',
                DOMsimulatorWorkArounds=True, 
		FADCSaturationMargin=1,
                CorrectDroop=False
                )

userPrint("DOM calibrator added")

#-----------------------------#
# Split the waveform
#-----------------------------#
tray.AddModule("I3WaveformSplitter", "ehewaveformsplit")(
    ("Input","CalibratedWaveforms"),
    ("HLC_ATWD","CalibratedATWD"),
    ("HLC_FADC","CalibratedFADC"),
    ("SLC","EHEGarbage"),
    ("Force",True),
    ("PickUnsaturatedATWD", True)
    )

tray.AddModule("I3NullSplitter", "fullevent")

userPrint("Waveform splitters included")

#-----------------------------#
# Portia
# Note LEThresh is different
# here than in Data...
#-----------------------------#
tray.AddModule( "I3Portia", "OfflinePortiaPulseExtractor") (
    ( "DataReadoutName",            "HLCCleanInIceRawData" ),
    ( "OutPortiaEventName",         "FirstPortiaEventSummary" ),
    ( "MakeIceTopPulse",             False ),
    ( "ATWDPulseSeriesName",        "ATWDFirstPulseSeries" ),
    ( "ATWDPortiaPulseName",        "ATWDFirstPortiaPulse" ),
    ( "ATWDWaveformName",           "CalibratedATWD" ),
    ( "ATWDBaseLineOption",         "eheoptimized" ),
    ( "FADCBaseLineOption",         "eheoptimized" ),
    ( "ATWDThresholdCharge",        m_MCOpt.QThresh * I3Units.pC ),
    ( "ATWDLEThresholdAmplitude",   0.5 * I3Units.mV ),
    ( "UseFADC",                    True ),
    ( "FADCPulseSeriesName",        "FADCFirstPulseSeries" ),
    ( "FADCPortiaPulseName",        "FADCFirstPortiaPulse" ),
    ( "FADCWaveformName",           "CalibratedFADC" ),
    ( "FADCThresholdCharge",        m_MCOpt.QThresh * I3Units.pC ),
    ( "FADCLEThresholdAmplitude",   0.5 * I3Units.mV ),
    ( "MakeBestPulseSeries",        False ),
    ( "PMTGain",                    10000000),
    )

userPrint("First portia added")

#
## Run portia again with best pulse series = true
#
tray.AddModule( "I3Portia", "OfflinePortiaBestPulseExtractor") (
    ( "DataReadoutName",            "HLCCleanInIceRawData" ),
    ( "OutPortiaEventName",         "PortiaEventSummary" ),
    ( "MakeIceTopPulse",             False ),
    ( "ATWDPulseSeriesName",        "ATWDPulseSeries" ),
    ( "ATWDPortiaPulseName",        "ATWDPortiaPulse" ),
    ( "ATWDWaveformName",           "CalibratedATWD" ),
    ( "ATWDBaseLineOption",         "eheoptimized" ),
    ( "FADCBaseLineOption",         "eheoptimized" ),
    ( "ATWDThresholdCharge",        m_MCOpt.QThresh * I3Units.pC ),
    ( "ATWDLEThresholdAmplitude",   0.5 * I3Units.mV ),
    ( "UseFADC",                    True ),
    ( "FADCPulseSeriesName",        "FADCPulseSeries" ),
    ( "FADCPortiaPulseName",        "FADCPortiaPulse" ),
    ( "FADCWaveformName",           "CalibratedFADC" ),
    ( "FADCThresholdCharge",        m_MCOpt.QThresh * I3Units.pC ),
    ( "FADCLEThresholdAmplitude",   0.5 * I3Units.mV ),
    ( "MakeBestPulseSeries",        True ),
    ( "PMTGain",                    10000000),
    )

userPrint("Second portia added")

#-----------------------------#
# Add EHE first guess module
#-----------------------------#
tray.AddModule( "I3EHEFirstGuess", "EheFirstGuess" ) (
    ( "InputLaunchName",             "HLCCleanInIceRawData" ),
    ( "InputPortiaEventName",        "PortiaEventSummary" ),
    ( "MinimumNumberPulseDom",       8 ),
    ( "OutputFirstguessName",        "OpheliaFirstGuess" ),
    ( "OutputFirstguessNameBtw",     "OpheliaFirstGuessBTW" ),
    ( "InputPulseName1",             "ATWDPortiaPulse" ),
    ( "InputPulseName2",             "FADCPortiaPulse" ),
    ( "ChargeOption",                0 ),
    ( "LCOption",                    True ),
    )

userPrint("EHE First guess module included")

#-----------------------------#
# Ophelia first guess converter
#-----------------------------#
tray.AddModule( "I3OpheliaConvertFirstGuessTrack", "FirstGuessConverter" ) (
    ( "InputOpheliaFGTrackName",     "OpheliaFirstGuessBTW" ),
    ( "OutputParticleName",          "OpheliaFGParticleBTW" ),
    )

userPrint("Ophelia first guess converter included")

#-----------------------------#
# Tree Maker module
#-----------------------------#
tray.AddModule("I3TreeMakerModule","tree-maker")(
    ("outTreeName",                   "JulietTree"),
    ("outFileName",                   m_MCOpt.TreeName),
    #
    ("doJulietTree",                  True),
    ("inMCTreeName",                  "I3MCTree"),
    ("frameMCWeightName",             "JulietWeightDict"),
    ("frameCosmicRayEnergyWeightName","CosmicRayEnergyDist"),
    ("framePropagationMatrixVectorName","PropagationMatrix"),
    ("inAtmosphericMuonFluxName",     "ElbertModelIC22"),
    #
    ("doNuGenTree",            False),
    ("doStdParticleTree",      False),
    #
    ("doMCHitTree",            True),
    ("inMCHitSeriesName",      "MCHitSeriesMap"),
    ("McHitCompressionOption", True),
    #
    ("useThinWeight",          False),
    ("thinMuWeightName",       "I3ThinMuWeight"),
    #
    ("doDetectorTree",          True),
    ("inDOMLaunchName",         "HLCCleanInIceRawData"),
    ("doPulseChannelTree",      True),
    ("inAtwdPortiaName",        "ATWDPortiaPulse"),
    ("inFadcPortiaName",        "FADCPortiaPulse"),
    ("inPortiaEventName",       "PortiaEventSummary"), 
    ("doWaveformDraw",           False),
    #
    ("doFirstGuessTree",        True),
    ("inFirstguessName",        "OpheliaFirstGuess"),
    ("inFirstguessNameBtw",     "OpheliaFirstGuessBTW"),
    #
    ("doMMCTree",              False),
    ("inMMCTrackListName",     "MMCTrackList"),
    #
    ("doEheStarTree",           False),
    ("doTopTree",               False),
    #
    ("doAddPulseEventTree",     True),
    ("inAddPulseEventName",     "FirstPortiaEventSummary"),                               
    ("inAddDOMLaunchName",      "HLCCleanInIceRawData")                    
)

userPrint("Tree Maker added")


#-----------------------------#
# I3 ouput file
#-----------------------------#
tray.AddModule("I3Writer","writer", filename = m_MCOpt.I3Name,
               Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.Physics],
	       skipkeys=["CleanIceTopRawData", "CleanIceTopRawDataFLC","CleanInIceRawData", 
                         "CleanInIceRawDataFLC", "MCPMTResponseMap", "CalibratedWaveforms"]
               )

userPrint("I3 Output file writer added")

#-----------------------------#
# Cleaning up
#-----------------------------#	    
tray.AddModule("Dump","dump")
tray.AddModule("TrashCan", "the can")

#-----------------------------#
# Execute
#-----------------------------#
tray.Execute(m_MCOpt.nEvts)
tray.Finish()

#-----------------------------#
# Say thanks
#-----------------------------#

print "Thanks for generating MC with me!"
