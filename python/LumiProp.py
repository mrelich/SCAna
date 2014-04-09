#!/usr/bin/env python

#################################################
# It seems like it's nice to have the SC lumi   #
# slices saved in a common location. I will put #
# the properties here in a lumi class           #
#################################################

from LumiObj import * 

class LumiProp:
    
    #------------------------------#
    # Constructor
    #------------------------------#
    def __init__(self,sc):
        
        # Initialize regions for luminosity
        # based on what SC we are looking at
        factor      = 10e14
        self.regions = []

        if( sc == "SC2" ):

            # Initialize the Luminosity holders
            # The values for LEThreshold are taken from Aya's script
            self.regions.append( LumiObj("1",280.001,280.022,factor,0.005) )
            self.regions.append( LumiObj("3",280.022,280.0432,factor,0.01) )
            self.regions.append( LumiObj("10",280.0432,280.0645,factor,0.03) )
            self.regions.append( LumiObj("30",280.0645,280.086,factor,0.07) )
            self.regions.append( LumiObj("51",280.086,280.107,factor,0.1) )
            self.regions.append( LumiObj("100",280.107,280.128,factor,0.28) )

        # end if SC2
        # Start SC1, don't know lumi's yet
        elif( sc == "SC1" ):

            self.regions.append( LumiObj("0",276.679,276.702,factor,0) )
            self.regions.append( LumiObj("1",276.702,276.723,factor,0) )
            self.regions.append( LumiObj("2",276.723,276.7449,factor,0) )
            self.regions.append( LumiObj("3",276.7449,276.7655,factor,0) )
            self.regions.append( LumiObj("4",276.7655,276.787,factor,0) )
            self.regions.append( LumiObj("5",276.787,276.8069,factor,0) )
            self.regions.append( LumiObj("6",276.8069,276.829,factor,0) )


        #end elif SC1
        else:
            print "SC not supported in LumiProp.py"
            print "Your entry: ", sc
            print "Supported is: SC1 or SC2"
    #end constructor

    #------------------------------#
    # Method to get timing range
    #------------------------------#
    def getRange(self, lumi):
        
        for i in range(len(self.regions)):
            if lumi == self.regions[i].lumi:
                return self.regions[i].getRange()

        print "Lumi not supported for ", lumi
        print "Returning value for 100%"
        last = int(len(self.regions) - 1)
        return self.regions[last].getRange()

    #------------------------------#
    # Get LEThres
    #------------------------------#
    def getLEThreshold(self,lumi):
        for i in range(len(self.regions)):
            if lumi == self.regions[i].lumi:
                return self.regions[i].LEThreshold
        
        print "Lumi not supported for ", lumi
        print "Returning zero"
        return 0.

    #------------------------------#
    # Method to retrieve vector of 
    # supported lumi percentages
    #------------------------------#
    def getLumis(self):
        lumis = []
        for i in range(len(self.regions)):
            lumis.append( self.regions[i].lumi )

        return lumis
    
    #------------------------------#
    # Have a method to check to see 
    # a given time is in some range
    #------------------------------#
    def lumiInRange(self, lumi, timing):
        
        # Loop over available lumis
        for i in range(len(self.regions)):
            if lumi == self.regions[i].lumi:
                return self.regions[i].inRange(timing)

        # Not in Range
        return False
    
