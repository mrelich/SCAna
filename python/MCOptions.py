#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# This is a script that will hold the parameters that can #
# easily be changed or configured during runtime in order #
# to generate the standard candle MC simulations.         #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

from StandardCandle import *
import sys

class MCOptions:
    
    #---------------------------#
    # Variables
    #---------------------------#

    # Configurable variables
    SCName   = "NONE"                  # Which SC are we using    
    Lumi     = "Not Set"               # Filtering luminosity to use
    I3Name   = "../i3files/"
    TreeName = "../trees/"
    

    # Hardcoded variables
    seed     = 795                     # Random number for seed
    nEvts    = 500                     # Number of events to simulate     
    GCDFile  = 'GCDFiles/Level2_IC86.2012_data_Run00120946_1116_GCD.i3.gz'
    GDOMName = "TA0003"
    QThresh  = 0.1                     # pC
    
    #---------------------------#
    # Constructor
    #---------------------------#
    def __init__(self,argv):
        
        # Extract the SC name and the
        # filter efficiency.
        for i in range(len(argv)):
            if "SC" in argv[i]:
                self.SCName = argv[i]
            
            # Try to get luminosity
            elif "-h" == argv[i]: 
                self.printHelp()
                sys.exit()
            # Try to get lumi
            else:
                try:
                    isNumeric = int(argv[i])
                except:
                    isNumeric = -1
                if not isNumeric == -1:
                    self.Lumi = str(isNumeric)
                    
        # end loop over arguments

        # Check to make sure SCName and Lumi are set
        if self.SCName == "NONE":    
            print "\n"
            print "SC Name not set"
            print "\n"
            sys.exit()
        if self.Lumi == "Not Set":
            print "\n"
            print "Lumi not set"
            print "\n"
            sys.exit()
        
        # Initialize the SC properties
        self.SC = StandardCandle(self.SCName, self.Lumi)
        if not self.SC.Initialized:
            print "SC not set. Please see StandardCandle.py"
            sys.exit()
        

        # Set the I3Name and TreeName
        fileBase =  self.SCName + "_SpiceMie"
        fileBase += "_filter" + self.Lumi
        fileBase += "_" + self.GDOMName
        
        self.I3Name   += fileBase + ".i3.gz"
        self.TreeName += fileBase + "_tree.root"

    #---------------------------#
    # Print help info
    #---------------------------#
    def printHelp(self):
        print "\n*******************************************"
        print "Welcome to the help menu!"
        print "Options are:"
        print "\t SC1   -- for standard candle 1"
        print "\t SC2   -- for standard candle 2"
        print "\t <int> -- for luminosity"
        print "\t\t Only certain luminosities are supported"
        print "\t\t See StandardCandle.py for info"
        print "********************************************\n"
