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
p_lumi = lumiProp()
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
outfile = TFile("../plots/RootPlots/cutWaveTimeVars_refined.root","recreate")

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
for lumi in allowedLumis:
    h_tDiff_perLumi.append( TH1F("h_tDiff_"+lumi,"",timebins,tmin,tmax) )
    h_maxTDiff_perLumi.append( TH1F("h_maxTDiff_"+lumi,"",timebins,tmin,tmax) )



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
    min_amp = 1.0
    for omkey, portiapulse in pulses:
        if omkey in near_sc:

            # Load variables for plotting
            amplitude = portiapulse.GetAmplitude() / I3Units.volt
            letime = portiapulse.GetLETime()
            waveform_series = calib_atwd.get(omkey)

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
            
    h_maxTDiff.Fill( max_time_diff )
    if lumi_point >=0:
        h_maxTDiff_perLumi[lumi_point].Fill(max_time_diff)

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
tray.AddModule(cutwavetime, "cutwavetime")
tray.AddModule("TrashCan","can")
tray.Execute()
tray.Finish()

outfile.Write()
outfile.Close()

