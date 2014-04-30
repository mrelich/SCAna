#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#
# This will hold the properties of the standard candle  #
# for easy editing and use for MC generation.           #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-#

class StandardCandle:

    #---------------------------#
    # Variables
    #---------------------------#

    SCName    = "SC"
    intensity = 0
    SCpos     = [0,0,0]

    # Possibile SC positions
    SC1_pos   = [544.1, 55.9, 136.84]    # (x,y,z) in m
    SC2_pos   = [11.87, 179.19, -205.64] # (x,y,z) in m

    # Possible intensities for SC2
    # Shigeru's revised numbers
    SC2_intensities = [("1",   1.8e11),  # 1%
                       ("3",   3.7e11),  # 3%
                       ("10",  1.0e12),  # 10%
                       ("30",  3.0e12),  # 30%
                       ("51",  4.3e12),  # 51%
                       ("100", 1.1e13)]  # 100%

    # Possible intensities for SC1
    # Taken from SC wiki.
    # I am ignoring 0.5% filter
    SC1_intensities = [("1",   5.6e10),  # 1%
                       ("3",   1.4e11),  # 3%
                       ("10",  4.4e11),  # 10%
                       ("30",  1.29e12), # 30%
                       ("50",  2.2e12),  # 50%
                       ("100", 4.0e12)]  # 100%

    # Flag to check if initialized
    Initialized = False

    #---------------------------#
    # Constructor
    #---------------------------#
    def __init__(self,SC,lumi):
        
        if SC == "SC1":
            self.SCName = "SC1"
            self.SCpos  = self.SC1_pos
            
            for pair in self.SC1_intensities:
                if lumi == pair[0]:
                    self.intensity = pair[1]
                    self.Initialized = True

        # end if SC1
        elif SC == "SC2":
            self.SCName = "SC2"
            self.SCpos  = self.SC2_pos

            for pair in self.SC2_intensities:
                if lumi == pair[0]:
                    self.intensity = pair[1]
                    self.Initialized = True

        #end if SC2
        else:
            print "SC not recognized: ", SC
            print "Variables not set"
            self.Initialized = False

        
        # Print some info about standard candle
        # properties that have been set
        print "Standard Candle set:"
        print "\t Initialized: ", self.Initialized
        print "\t SCName:      ", self.SCName
        print "\t NPhotons:    ", self.intensity
        print "\n"
