#!/usr/bin/env python

# This is copied from AYA and simply modified output files and
# the gcd files.

import os, sys, glob
from icecube import icetray, dataio, dataclasses
import I3Tray
from icecube.icetray import I3Units
from icecube.icetray import OMKey
from icecube import portia
from icecube import WaveCalibrator

#import pylab as p
import numpy as n
from ROOT import TCanvas, TH1F, gROOT, TH2F
# from DumpModule import DumpModule
from icecube.BadDomList import bad_dom_list_static

tray = I3Tray.I3Tray()

outdir = "../trees"
i3outdir = "../i3files"
argvs = sys.argv 
argc = len(argvs)
###########################################################################
#print "arguments ", argvs
#print "number of arg " argc
###########################################################################
if argc < 2:
    print "Usage: script.py <number>, <number> must be 3, 10, 30, 50 or 100"
    sys.exit(0)
try:
    numeric_argv = int(argvs[1])
except:
    print '1st argument must be numeric.'
    sys.exit(0)
###########################################################################
#fileList = ['sc2_2011/GCD_Run117447.i3.gz']
#infiles1 = glob.glob('sc2_2011/StanCan2Filter_TestData_PhysicsFiltering_Run00117411_Subrun00000000_00000???.i3.gz')      
fileList = ['GCDFiles/Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz']
infiles1 = glob.glob('standard-candle/sc2/StandardCandle_2_Filtering_Run00120946_AllSubrunsMerged.i3.gz')
#infiles1 = glob.glob('sc2_2011/StanCan2Filter_TestData_PhysicsFiltering_Run00117411_Subrun00000000_00000000?.i3.gz')       
#infiles2 = glob.glob('sc2_2011/StanCan2Filter_TestData_PhysicsFiltering_Run00117411_Subrun00000000_00000001?.i3.gz')       
#infiles3 = glob.glob('sc2_2011/StanCan2Filter_TestData_PhysicsFiltering_Run00117411_Subrun00000000_00000002?.i3.gz')       
infiles1.sort()
fileList += infiles1
#fileList += infiles2
#fileList += infiles3
###########################################################################
if numeric_argv < 3 : #1%
    outname = "timediff_LEthres0.005V_SC1per_cut.hist.root"
    outrootfile = outdir + "/SC2_allNearbyDOM_SC1per_DOMcalib_before.tree.root"
    outi3file = i3outdir + "/SC2_1per_EHEClean.i3.gz"
    LEthres = 0.005 * I3Units.volt
elif numeric_argv < 10 : #3%
    outname = "timediff_LEthres0.01V_SC3per_cut.hist.root"
    outrootfile = outdir + "/SC2_allNearbyDOM_SC3per_DOMcalib_before.tree.root"
    outi3file = i3outdir + "/SC2_3per_EHEClean_DOMcalib_WaveCalib.i3.gz"
    LEthres = 0.01 * I3Units.volt
elif numeric_argv < 30 : #10%
    outname = "timediff_LEthres0.03V_SC10per_cut.hist.root"
    outrootfile = outdir + "/SC2_allNearbyDOM_SC10per_DOMcalib_before.tree.root"
    outi3file = i3outdir + "/SC2_10per_EHEClean_DOMcalib_WaveCalib.i3.gz"
    LEthres = 0.03 * I3Units.volt
elif numeric_argv < 50 : #30%
    outname = "timediff_LEthres0.07V_SC30per_cut.hist.root"
    outrootfile = outdir + "/SC2_allNearbyDOM_SC30per_DOMcalib_before.tree.root"
    outi3file = i3outdir + "/SC2_30per_EHEClean_DOMcalib_WaveCalib.i3.gz"
    LEthres = 0.07 * I3Units.volt
elif numeric_argv < 100 : #51%
    outname = "timediff_LEthres0.1V_SC51per_cut.hist.root"
    outrootfile = outdir + "/SC2_allNearbyDOM_SC51per_DOMcalib_before.tree.root"
    outi3file = i3outdir + "/SC2_51per_EHEClean_DOMcalib_WaveCalib.i3.gz"
    LEthres = 0.1 * I3Units.volt
else : #100%
    outname = outdir + "/timediff_LEthres0.28V_SC100per_cut.hist.root"
    outrootfile = outdir + "/SC2_allNearbyDOM_SC100per_DOMcalib_before.tree.root"
    LEthres = 0.28 * I3Units.volt
    outi3file = i3outdir + "/SC2_100per_EHEClean_DOMcalib_WaveCalib.i3.gz"
print outname, outrootfile
###########################################################################
i = 0
i2=0
results=[]
#c1 = TCanvas('c1','Example',200,10,700,500)
hpx = TH1F('hpx','px',100,-40,560)
amp = TH1F('amp','amp',100,0.0,0.5)
nch = TH1F('nch', 'nch', 100, 0, 1500)
tVsAmp = TH2F('tVsAmp','tVsAmp',30000,27990,28020,100000,0,1000)
###########################################################################
# This i only count p frames (?)
def count(frame, Streams1=[icetray.I3Frame.DAQ]):
    global i
    i += 1
    print "======================================"
    print "first counter", i
    print "======================================"
###########################################################################
# This i only count p frames (?)
def count2(frame, Streams2=[icetray.I3Frame.DAQ]):
    global i2
    i2 += 1
    print "======================================"
    print "second counter", i2
    print "======================================"
###########################################################################
def utctimes(frame, Streams3=[icetray.I3Frame.DAQ]):
    if 'I3EventHeader' not in frame:
        return False
    
    header = frame['I3EventHeader']
    start_time = header.start_time
    print start_time.utc_daq_time
    if numeric_argv < 3 : #1%
        min_start_time_utc_daq_time = 1054.5*1e13
        max_start_time_utc_daq_time = 1056.6*1e13
    elif numeric_argv < 10 : #3%
        min_start_time_utc_daq_time = 1056.7*1e13
        max_start_time_utc_daq_time = 1058.8*1e13
    elif numeric_argv < 30 : #10%
        min_start_time_utc_daq_time = 1059.5*1e13
        max_start_time_utc_daq_time = 1061.7*1e13
    elif numeric_argv < 50 : #30%
        min_start_time_utc_daq_time = 1061.7*1e13
        max_start_time_utc_daq_time = 1063.8*1e13
    elif numeric_argv < 100 : #51%
        min_start_time_utc_daq_time = 1063.8*1e13
        max_start_time_utc_daq_time = 1065.9*1e13
    else : #100%
        min_start_time_utc_daq_time = 1066.0*1e13
        max_start_time_utc_daq_time = 1068.1*1e13

    if start_time.utc_daq_time < min_start_time_utc_daq_time or start_time.utc_daq_time > max_start_time_utc_daq_time :
        print 'time out of range'
        return False 
    else:
        print 'time is within the range', start_time.utc_daq_time
        return True
#####################################################################
def checkophelia(frame, Streams4=[icetray.I3Frame.Physics]):
    if 'OpheliaFirstGuess1BTW' not in frame:
        print 'no LineFit'
        return False
#####################################################################
def checkndom(frame, Streams5=[icetray.I3Frame.DAQ]):
    if 'CleanInIceRawData' not in frame:
        print 'no DOMLaunch'
        return False

    domlaunch = frame['CleanInIceRawData']
    print 'number of DOM launches ', len(domlaunch)

    if len(domlaunch) < 400:
        return False
#####################################################################
def saveTvsE(frame, Streams7=[icetray.I3Frame.Physics]):
    if 'I3EventHeader' not in frame:
        return False
    if 'ATWDPortiaPulse' not in frame:
        return false

    # Get time
    header = frame['I3EventHeader']
    time = header.start_time.utc_daq_time / 1.e13
    print "have time", time
    # Make sure we are close to SC, so we can
    # see maximum NPE to see differentiation
    near_sc = [OMKey(55,d) for d in range(37, 51)]
    pulses = frame['ATWDPortiaPulse']
    print "have pulses", pulses
    for omkey, portiapulse in pulses:
        if omkey in near_sc:
            print "\tHave dom near sc"
            amplitude = portiapulse.GetAmplitude()/I3Units.volt
            print "\thave amp: ", amplitude
            tVsAmp.Print()
            tVsAmp.Fill(time, amplitude)
            print "\tfilled"

    print "returning"

    return True
#####################################################################
def cutwavetime(frame, Streams6=[icetray.I3Frame.DAQ]):
    if 'ATWDPortiaPulse' not in frame:
        print 'no pulse'
        return False
#   
    pulses = frame['ATWDPortiaPulse']
    domlaunch = frame['CleanInIceRawData']
    calib_atwd = frame['CalibratedATWD']

    if numeric_argv < 3 : #1%
        max_time_diff_thres = 500.0
    elif numeric_argv < 10 : #3%
        max_time_diff_thres = 250.0
    elif numeric_argv < 30 : #10%
        max_time_diff_thres = 250.0
    elif numeric_argv < 50 : #30%
        max_time_diff_thres = 260.0
    elif numeric_argv < 100 : #51%
        max_time_diff_thres = 280.0
    else : #100%
        max_time_diff_thres = 300.0
        
        
    near_sc = [OMKey(55,d) for d in range(37, 51)]
    print 'number of pulses', len(pulses)
    if len(pulses) < 400:
        return False

    #print 'number of nearby doms', len(near_sc)
    
    counter = 0
    max_time_diff = 0.0
    min_amp = 1.0
    for omkey, portiapulse in pulses:
        #print(omkey.string, omkey.om)
        if omkey in near_sc:
            
            amplitude = portiapulse.GetAmplitude()
            letime = portiapulse.GetLETime()
            launchtime = portiapulse.GetLaunchTime()
            waveform_series = calib_atwd.get(omkey)
            
            print 'amplitude ', amplitude / I3Units.volt, ' ', LEthres / I3Units.volt
            #print 'amplitude ', amplitude,  ' ', LEthres

            if amplitude < min_amp:
                min_amp = amplitude
                
            if amplitude > LEthres:
                counter += 1

            for waveform in waveform_series:
                wavetime = waveform.time
                if wavetime < letime:
                    time_diff = (letime-wavetime) / I3Units.nanosecond
                    if time_diff > max_time_diff:
                        max_time_diff = time_diff
                        #print max_time_diff
                    #print letime / I3Units.nanosecond, wavetime / I3Units.nanosecond
                        
    print '  the maximum time diff  ', max_time_diff, ' the maximum time diff thres ', max_time_diff_thres
    print "nearby number of DOM ", counter
    if counter < 14:
        print "skip this event"
        return False

    if max_time_diff > 560 or max_time_diff < -40:
        print "max time out of range!!!---------",max_time_diff
        return False

    if max_time_diff > max_time_diff_thres or max_time_diff < 20:
        return False
    else:
        hpx.Fill(max_time_diff)
        amp.Fill(min_amp / I3Units.volt)
        nch.Fill(len(domlaunch))
        return True
#####################################################################
tray.AddModule( "I3Reader", "Reader")(
    ("Filenamelist", fileList)
    )
tray.AddModule(count, "count")
#tray.AddModule(utctimes, "utctimes") #TURN BACK ON
#tray.AddModule("QConverter", "qify", WritePFrame=True)
#*************************************************
I3Tray.load("libDomTools")
tray.AddModule("I3OMSelection<I3DOMLaunchSeries>","badoms_inice")(
    ("OmittedKeys",bad_dom_list_static.IC86_static_bad_dom_list()),
    ("OmittedStrings",[81, 82, 83, 84, 85, 86, 87]), #all deep core strings excluded for simplification 
    ("OutputOMSelection","BadDomsListOffline"), # created IC79 list appended to the IC40 list above
    ("InputResponse","InIceRawData"),
    ("OutputResponse","CleanInIceRawData")
    )

tray.AddModule(checkndom, "checkndom")
#**************************************************************
#I3Tray.load("libWaveCalibrator")
#tray.AddModule("I3WaveCalibrator", "calibrator",
#               Launches="CleanInIceRawData",
#               Waveforms="CalibratedWaveforms",
#               ATWDSaturationMargin=123, # 1023-900 == 123
#               FADCSaturationMargin=0,
#               )           
#***************************************************************
#I3Tray.load("libDOMcalibrator")                                    
#tray.AddModule("I3DOMcalibrator","calibrateandlisten-inice")(
#    ("InputRawDataName","CleanInIceRawData"),
#    ("CorrectPedestalDroopDualTau", False ),
#    ("CorrectPedestalDroop", False),
#    ("OutputATWDDataName", "CalibratedATWD"),
#    ("OutputFADCDataName", "CalibratedFADC"),
#    ("ATWDSaturationLevel", 900),
#    ("FADCSaturationLevel", 1023),
#    ("CalibrationMode", 1)
#    )
#***************************************************************
#########################################################
#tray.AddModule(WaveCalibrator.BeaconModule, "baseliner",
#               TablePath="/home/aya/icecube/beacon"
#               )

tray.AddModule("I3WaveCalibrator", "calibrator",
               Launches="CleanInIceRawData",
               Waveforms="CalibratedWaveforms",
               ATWDSaturationMargin=123, # 1023-900 == 123          
               FADCSaturationMargin=0,
               CorrectDroop=False
               )

#tray.AddModule("I3WaveformSplitter", "split",
#     Input="CalibratedWaveforms",
#     HLC_ATWD = "CalibratedATWD_Wave",
#     HLC_FADC = "CalibratedFADC_Wave",
#     PickUnsaturatedATWD=True
#)

#So I noticed in Keiichi's script that he has
# the same names for HLC_ATWD as he does for
# the ATWDWaveformName...  I am not sure
# if this is due to some software update, or 
# who is right...  I can print out the frame
# in the method that is crashing and what I see
# is that CalibratedATWD doesn't exist, but 
# CalibratedATWD_Wave does!  So I make them same
# for now...
tray.AddModule("I3WaveformSplitter", "split",
     Input="CalibratedWaveforms",
     HLC_ATWD = "CalibratedATWD",
     HLC_FADC = "CalibratedFADC",
     PickUnsaturatedATWD=True
)
###########################################################################               
I3Tray.load("libportia")                                    
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
#tray.AddModule(cutwavetime, "cutwavetime_after_portia") TURN BACK ON
#tray.AddModule(saveTvsE, "t_vs_e")
#******************#***************************************************************
I3Tray.load("libophelia")   
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
tray.AddModule( checkophelia, "checkreco")
#******************************
# Root Tree Maker
#******************************
I3Tray.load("libtree-maker")                                    
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
#*********************************************
tray.AddModule(count2, "count2")
#*********************************************
tray.AddModule("I3Writer","writer")(
    ("filename", outi3file),
    ("SkipKeys", ["I3DAQData"]),
#   ("sizelimit", 10**9), # 1G
    ("DropOrphanStreams", [ icetray.I3Frame.DAQ, icetray.I3Frame.Calibration ]),
    ("Streams", [icetray.I3Frame.Geometry, icetray.I3Frame.Calibration, icetray.I3Frame.DetectorStatus, icetray.I3Frame.DAQ, icetray.I3Frame.Physics] )
   )

#tray.AddModule( DumpModule ,"dumper")
tray.AddModule("TrashCan", "bye")

tray.Execute()
tray.Finish()

# Turn off plotting for now
#c1.Divide(3)
#c1.cd(1)
#hpx.Draw()
#c1.cd(2)
#amp.Draw()
#c1.cd(3)
#nch.Draw()
#c1.Update()
#c1.Print(outname)
#p.plot(results, linestyle='steps') 
#p.savefig("test.png")
