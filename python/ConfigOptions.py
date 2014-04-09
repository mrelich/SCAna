#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# This class will handle the user inputs and create a useful object #
# for setting options to run the data filter.  The hope is to keep  #
# a place for hardcoded values that can be easily changed and found #
# in FilterBase.py                                                  #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

from LumiProp import *
import sys

###########################################################
#                    Begin Class                          #
###########################################################

class ConfigOptions:

    # Configurable variables
    lumi    = "NOT SPECIFIED"    # Specify the lumi
    SC      = "NONE"             # Which SC to look at
    cutUTC  = False              # Cut on utc times if lumi specified
    cutNDOM = True               # Cut number of doms, on by defualt

    # Hardcoded variables
    nDOMReq = 400                # To clean up remaining noise
    QThresh = 0.1                # Taken from Aya's script

    # Holders for file names. 
    # Now directories are hardcoded
    # These will need to be updated if
    # running over different files
    GCDFile = 'GCDFiles/'
    SCFile  = 'standard-candle/'
    
    # Placeholders for output i3 and 
    # root trees
    I3Name   = "../i3files/"
    TreeName = "../trees/"

    # Flag to say everything initiated ok
    Initialized = False


    ##################################
    # Constructor
    ##################################
    def __init__(self, argv):

        # Initialize SC lumi object,
        # check if cut nDOM is turned off
        # or help menu is called
        for i in range(len(argv)):
            if "SC1" == argv[i] or "SC2" == argv[i]:
                self.SC = argv[i]
                self.p_lumi = LumiProp(argv[i])
                self.Initialized = True
            if "-c" == argv[i]:
                self.cutNDOM = False
            if "-h" == argv[i]:
                self.printHelp()
                sys.exit()
        
        # If SC not specified, cannot continue
        if not self.Initialized: return
        
        # Check if one of the allowed lumi
        allowedRegions = self.p_lumi.getLumis()
        for i in range(len(argv)):
            for j in range(len(allowedRegions)):
                if argv[i] == allowedRegions[j]:
                    self.lumi = str(argv[i])
                    self.cutUTC = True

        # Print some information
        print "Configuration initialized"
        print "SC:   ", self.SC
        print "Lumi: ", self.lumi 

        # Set the input file names
        self.setInput()

        # Set the output file names
        self.setOutput()

    ##################################
    # Print help menu
    ##################################
    def printHelp(self):
        print "\n"
        print "*******************************************"
        print "Welcome to help menu!"
        print "Options are:"
        print "\t SC1   -- Set for standard candle 1"
        print "\t SC2   -- Set for standard candle 2"
        print "\t -c    -- Turn off nDOM check"
        print "\t <int> -- Specify Lumi region"
        print "\n"
        print "*******************************************"

    ##################################
    # Are we initialized?
    ##################################
    def isValid(self):
        return self.Initialized

    ##################################
    # Set input files
    ##################################
    def setInput(self):
        if self.SC == "SC1":
            self.GCDFile += 'Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz'
            self.SCFile  += 'sc1/StandardCandle_1_Filtering_Run00120946_AllSubrunsMerged.i3.gz'
        elif self.SC == "SC2":
            self.GCDFile += 'Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz'
            self.SCFile  += 'sc2/StandardCandle_2_Filtering_Run00120946_AllSubrunsMerged.i3.gz'

    ##################################
    # Set output file names
    ##################################
    def setOutput(self):
        outname = self.SC
        if self.cutUTC:  outname += "_filter" + self.lumi
        if self.cutNDOM: outname += "_cutNDOM"+str(self.nDOMReq)

        self.I3Name   += outname + "_WaveCalib.i3.gz"
        self.TreeName += outname + "_tree.root"

        
