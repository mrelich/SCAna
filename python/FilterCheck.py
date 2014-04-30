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

# Get Icetray pointer
tray = I3Tray.I3Tray()

# 
## Determine lumi for this job
#
if len(sys.argv) != 2:
    sys.exit()

lumi = str(sys.argv[1])

# Allowed lumis
allowed = ["1","3","10","30","51","100"]
if lumi not in allowed:
    print "lumi not found ", lumi
    sys.exit()

print "Running Lumi: ",lumi
#
## Specify the input file
#
pwd = os.getenv('PWD')
fileList = [pwd +'/../i3files/SC2_filter'+lumi+'_cutNDOM400_WaveCalib.i3.gz']

#
## Specify the output file and histograms
#

# File
#outfile = TFile(pwd+"/../plots/RootPlots/cutDT_"+lumi+"per.root","recreate")
outfile = TFile("test.root","recreate")

# Want to look at the DOMs on the SC string
# and check the deltaT for those doms
SCDoms = [40,41,42,43,44,45,46,47,48]

h_dT_perDom = []
dTbins = 100
dTmin  = 0
dTmax  = 500
for dom in SCDoms:
    dTName = "h_DOM"+str(dom)
    h_dT_perDom.append( TH1F(dTName,"",dTbins,dTmin,dTmax) )


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
    #print cleanData
    # Loop over OM Keys and get calibrated ATWD
    # Set SC pulses
    SC_DOMs = [OMKey(55,d) for d in SCDoms]
    for key, domlaunchs in cleanData:
        for i in range(len(SCDoms)):

            if key == SC_DOMs[i]:
                domTime = 9999999
                passLC  = False
                for launch in domlaunchs:
                    if launch.time < domTime:
                        domTime = launch.time
                        passLC  = launch.lc_bit
        
                # Do not look at events where LCBit fail
                if not passLC:  continue
                if domTime < 0: continue

                # Now we have start time, loop over calibrated
                # waveforms and keep the waveform that has shortest
                # time
                waveForms = calib_atwd.get(key)
                dT       = -999
                maxV     = -999
                for waveform in waveForms:
                    if waveform.time <= domTime:
                        # This is it, record info
                        voltages = waveform.waveform
                        binSize  = waveform.binWidth
                        for j in range(len(voltages)):
                            Voltage = voltages[j]/I3Units.V
                            if maxV < Voltage:
                                maxV = Voltage
                                dT   = ((len(voltages) - j)*binSize)/I3Units.ns
                        break

                # Now save the wavetime, if non-negative
                if dT < 0: continue
                
                # Fill
                h_dT_perDom[i].Fill(dT)

            # end if have right DOM
        # end for loop over SC DOMS
    # end loop over clean data
    return True
# End method

#
## Add some modules
#
tray.AddModule("I3Reader", "Reader")(
               ("Filenamelist", fileList)
               )
tray.AddModule(fillDT, "dtcheck")
tray.AddModule("TrashCan","can")
tray.Execute()
tray.Finish()

outfile.Write()
outfile.Close()

