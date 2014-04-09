#!/usr/bin/env python

###################################################
# Luminosity seems to have a few basic properties #
# that would be much better served to be kept     #
# in a simple object. This will be that object    #
###################################################

class LumiObj:

    #------------------------------#
    # Constructor
    #------------------------------#
    def __init__(self,lumi,lowBound,upBound,factor,LEThres):
        self.lumi        = lumi      # String for lumi number
        self.timeLow     = lowBound  # Lower bound for timing cut
        self.timeHigh    = upBound   # Upper bound for timing cut
        self.LEThreshold = LEThres   # LE threshold for Portia

        # Scale time up by the appropriate factor
        # This is done for convenience so I don't have
        # to type in 10e14 (or something else) all the time
        self.timeLow  *= factor
        self.timeHigh *= factor

    #------------------------------#
    # Get time range
    #------------------------------#
    def getRange(self):
        return [self.timeLow, self.timeHigh]

    #------------------------------#
    # Method to check range
    #------------------------------#
    def inRange(self,time):
        if self.timeLow < time and time <= self.timeHigh:
            return True

        # Otherwise not in range
        return False
