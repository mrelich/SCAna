#!/usr/bin/env python

#########################################################
# This script is going to investigate the use of the    #
# cutwavetime(...) method that appears in Aya's         #
# filtering script.  There are many hardcoded variables #
# that I do not fully understand.                       #
#########################################################

import os, sys, glob
from icecube import icetray, dataio, dataclasses
import I3Tray
from icecube.icetray import I3Units
from icecube.icetray import OMKey
from icecube import portia
from icecube import WaveCalibrator
from LumiProp import *

from ROOT import *

# Create my user defined lumi object
p_lumi = lumiProp("SC2")
allowedLumis = p_lumi.getLumis()

# Get Icetray pointer
tray = I3Tray.I3Tray()

#
## Specify the input file
#

fileList = ['../i3files/SC2_100per_EHEClean_DOMcalib_WaveCalib.i3.gz']

#
## Specify the output file and histograms
#

# File
#outfile = TFile("../plots/RootPlots/cutWaveTimeVars_refined.root","recreate")
#outfile = TFile("../plots/RootPlots/cutWaveTimeVars_wNDOMCheck.root","recreate")
#outfile = TFile("../plots/RootPlots/cutWaveTimeVars_AddedCounter.root","recreate")
#outfile = TFile("../plots/RootPlots/cutWaveTimeVars_CutOnCounter.root","recreate")
#outfile = TFile("../plots/RootPlots/cutWaveTimeVars_AddedMaxVTime.root","recreate")
outfile = TFile("../plots/RootPlots/cutWaveTimeVars.root","recreate")
#outfile = TFile("../plots/RootPlots/cutWaveTimeVars_AddedMaxVTime_LEThresh0_1.root","recreate")

# Amplitude bins
nampbins = 10000
ampmin   = 0
ampmax   = 10
h_amp = TH1F("h_amp","",nampbins,ampmin,ampmax)
h_amp_perLumi = []
for lumi in allowedLumis:
    h_amp_perLumi.append( TH1F("h_amp_"+lumi,"",nampbins,ampmin,ampmax) )

# Length of pulse vector
npbins = 2000
pmin   = 0
pmax   = 2000   
h_npulse = TH1F("h_npulse","",npbins,pmin,pmax)
h_npulse_perLumi = []
for lumi in allowedLumis:
    h_npulse_perLumi.append( TH1F("h_npulse_"+lumi,"",npbins,pmin,pmax) )

# Time difference hists
timebins = 500
tmin     = 0
tmax     = 500 # nanoseconds
h_tDiff = TH1F("h_tDiff","",timebins,tmin,tmax)
h_tDiff_perLumi = []
h_maxTDiff = TH1F("h_maxTDiff","",timebins,tmin,tmax)
h_maxTDiff_perLumi = []
h_maxTDiff_passLCBit_perLumi = []
h_maxTDiff_forMaxV_perLumi = []
for lumi in allowedLumis:
    h_tDiff_perLumi.append( TH1F("h_tDiff_"+lumi,"",timebins,tmin,tmax) )
    h_maxTDiff_perLumi.append( TH1F("h_maxTDiff_"+lumi,"",timebins,tmin,tmax) )
    h_maxTDiff_passLCBit_perLumi.append( TH1F("h_maxTDiff_passLCBit_"+lumi,"",timebins,tmin,tmax) )
    h_maxTDiff_forMaxV_perLumi.append( TH1F("h_maxTDiff_forMaxV_"+lumi,"",timebins,tmin,tmax) )

# Time difference vs. NPE
NPEbins = 1000
NPEmin  = 0
NPEmax  = 900e3
h_maxTDiff_vs_NPE = TH2F("h_maxTDiff_vs_NPE","",timebins,tmin,tmax,NPEbins,NPEmin,NPEmax)
h_maxTDiff_vs_NPE_perLumi = []
for lumi in allowedLumis:
    h_maxTDiff_vs_NPE_perLumi.append( TH2F("h_maxTDiff_vs_NPE_"+lumi,"",timebins,tmin,tmax,NPEbins,NPEmin,NPEmax) )
    

# Need counters to see # of doms that
# fire and the maximum time diff
DOMbins = 50
DOMmin  = -0.5
DOMmax  = DOMbins + DOMmin
h_nDOM_perLumi = []
h_nDOM_vs_maxTDiff_perLumi = []
for lumi in allowedLumis:
    h_nDOM_perLumi.append( TH1F("h_nDOM_"+lumi,"",DOMbins,DOMmin,DOMmax) )
    h_nDOM_vs_maxTDiff_perLumi.append( TH2F("h_nDOM_vs_maxTDiff_"+lumi,"",DOMbins,DOMmin,DOMmax,timebins,tmin,tmax) )

# Want to look at the DOMs on the SC string
# and check the deltaT for those doms
SCDoms = [40,41,42,43,44,45,46,47,48]
h_dT_perDom_perLumi = []
dTbins = 100
dTmin  = 0
dTmax  = 500
for lumi in allowedLumis:
    temp = []
    for dom in SCDoms:
        dTName = "h_DOM"+str(dom)+"_"+lumi
        temp.append( TH1F(dTName,"",dTbins,dTmin,dTmax) )

    h_dT_perDom_perLumi.append( temp )

#
## Fill dT for each dom per lumi
#
def fillDT(frame, Streams2=[icetray.I3Frame.DAQ]):
    if 'ATWDPortiaPulse' not in frame:
        print 'no pulse'
        return False
    
    # Get ATWD info
    calib_atwd = frame['CalibratedATWD']
    cleanData  = frame['CleanInIceRawData']

    # Get lumi point
    lumi_point = checkTimes(frame)
    if lumi_point < 0: 
        return False

    # Loop over OM Keys and get calibrated ATWD
    # Set SC pulses
    SC_DOMs = [OMKey(55,d) for d in SCDoms]
    for i in range(len(SCDoms)):
        
        # Make OMKey
        key        = OMKey(55,i)

        # Get DOM launch
        domlaunchs = cleanData.get(key)
        
        # Get Earliest time DOM launch
        domTime = -1
        passLC  = False
        for launch in domlaunchs:
            if domTime < launch.GetStartTime():
                domTime = launch.GetStartTime()
                passLC  = launch.GetLCBit()
        
        # Do not look at events where LCBit fail
        if not passLC: return False
        if domTime < 0: return False

        # Now we have start time, loop over calibrated
        # waveforms and keep the waveform that has shortest
        # time
        waveForms = calib_atwd.get(key)
        dT       = -999
        maxV     = -999
        for waveform in waveForms:
            if waveform.time <= domTime:
                # This is it, record info
                binSize  = waveform.GetBinWidth()
                startT   = waveform.GetStartTime()
                voltages = waveform.GetWaveform()
                for i in range(len(voltages)):
                    if maxV < voltages[i]:
                        maxV = voltages[i]
                        dT   = (i*binSize)/I3Units.ns
                break
        
        # Now save the wavetime, if non-negative
        if dT < 0: return False
        
        # Fill
        h_dT_perDom_perLumi[i][lumi_point]->Fill(dT)

    # end loop over doms
    return True
# End method
        
    

#
## Timing Check
#
def checkTimes(frame):
    notInRange = -1
    if 'I3EventHeader' not in frame:
        return notInRange
    
    # Get header which has timing info
    header = frame['I3EventHeader']
    start_time = header.start_time.utc_daq_time
    
    # Now determine if the timing info is 
    # in bounds of one of the allowed luminosities
    for i in range(len(allowedLumis)):
        if p_lumi.lumiInRange(allowedLumis[i],start_time):
            return i
    
    # If we are here, it isn't in range
    return notInRange

#
## Add nDOM check
#
def checkndom(frame, Streams5=[icetray.I3Frame.DAQ]):
    if 'CleanInIceRawData' not in frame:
        print 'no DOMLaunch'
        return False

    domlaunch = frame['CleanInIceRawData']    
    if len(domlaunch) < 400:
        return False

#
## Define the cutwavetime method but this time
## fill the variables that are normally cut on
#

def cutwavetime(frame, Streams6=[icetray.I3Frame.DAQ]):
    if 'ATWDPortiaPulse' not in frame:
        print 'no pulse'
        return False

    pulses = frame['ATWDPortiaPulse']
    domlaunch = frame['CleanInIceRawData']
    calib_atwd = frame['CalibratedATWD']

    # Get the point for a given luminosity based
    # on the timing information
    lumi_point = checkTimes(frame)

    # Define the SC string doms
    near_sc = [OMKey(55,d) for d in range(37, 51)]

    # Check if have enough pulses.  400 is from
    # Aya's code, so I will check why...
    h_npulse.Fill( len(pulses) )
    if lumi_point >= 0: h_npulse_perLumi[lumi_point].Fill( len(pulses) )
    if len(pulses) < 400:
        return False

    
    counter = 0
    max_time_diff = 0.0
    max_amp = 0
    m_NPE     = 0
    m_LCBit   = 0
    LEThres   = 0.01
    max_time_largestV = 0
    for omkey, portiapulse in pulses:
        if omkey in near_sc:

            # Load variables for plotting
            amplitude = portiapulse.GetAmplitude() / I3Units.volt
            NPE       = portiapulse.GetEstimatedNPE()
            letime = portiapulse.GetLETime()
            waveform_series = calib_atwd.get(omkey)
            m_LCBit         = portiapulse.GetLCBit()

            # Count above amplitude
            if amplitude > LEThres:
                counter += 1
            
            # Fill hists for timing info
            h_amp.Fill(amplitude)
            if lumi_point >= 0:
                h_amp_perLumi[lumi_point].Fill(amplitude)

            # Examine Time DIfference stuff
            for waveform in waveform_series:
                wavetime = waveform.time
                if wavetime < letime:
                    time_diff = (letime-wavetime) / I3Units.nanosecond
                    h_tDiff.Fill(time_diff)
                    if lumi_point >=0: 
                        h_tDiff_perLumi[lumi_point].Fill(time_diff)
                    if time_diff > max_time_diff:
                        max_time_diff = time_diff
                        m_NPE = NPE
                    if amplitude > max_amp:
                        max_amp = amplitude
                        max_time_largestV = time_diff

    # Actually place the cut now
    if counter < 14 : return False

    h_maxTDiff.Fill( max_time_diff )
    h_maxTDiff_vs_NPE.Fill(max_time_diff, m_NPE)
    if lumi_point >=0:
        h_maxTDiff_perLumi[lumi_point].Fill(max_time_diff)
        h_maxTDiff_vs_NPE_perLumi[lumi_point].Fill(max_time_diff, m_NPE)
        if m_LCBit !=0 : 
            h_maxTDiff_passLCBit_perLumi[lumi_point].Fill(max_time_diff)
        h_nDOM_perLumi[lumi_point].Fill( counter )
        h_nDOM_vs_maxTDiff_perLumi[lumi_point].Fill(counter, max_time_diff)
        h_maxTDiff_forMaxV_perLumi[lumi_point].Fill(max_time_largestV)
    # Hoping this will speed things up
    # by returning False...  not really
    # any noticable increase in speed
    return False


#
## Add some modules
#
tray.AddModule("I3Reader", "Reader")(
               ("Filenamelist", fileList)
               )
tray.AddModule(checkndom, "checkndom")
tray.AddModul(fillDT, "dtcheck")
#tray.AddModule(cutwavetime, "cutwavetime")
tray.AddModule("TrashCan","can")
tray.Execute()
tray.Finish()

outfile.Write()
outfile.Close()

