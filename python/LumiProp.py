#!/usr/bin/env python

#################################################
# It seems like it's nice to have the SC lumi   #
# slices saved in a common location. I will put #
# the properties here in a lumi class           #
#################################################

class lumiProp:

    # 
    ## Constructor
    #
    def __init__(self,sc):
        
        # Initialize regions for luminosity
        # based on what SC we are looking at
        factor      = 10e14

        if( sc == "SC2" ):
            self.regions = [280.001  * factor, # 1%
                            280.022  * factor,
                            280.0432 * factor, # 3%
                            280.0645 * factor, # 10%
                            280.086  * factor, # 30%
                            280.107  * factor, # 51%
                            280.128  * factor, # 100%
                            0]                 # not used

            # Supported lumis expressed as filter percentages
            self.lumi_supported = ["1",
                                   "3",
                                   "10",
                                   "30",
                                   "51",
                                   "100"]
        # end if SC2
        # Start SC1, don't know lumi's yet
        elif( sc == "SC1" ):
            self.regions = [276.679   * factor,  # reg0
                            276.702   * factor,  
                            276.723   * factor,  # reg1
                            276.7449  * factor,  # reg2
                            276.7655  * factor,  # reg3
                            276.787   * factor,  # reg4
                            276.8069  * factor,  # reg5
                            276.829   * factor]  # reg6

            self.lumi_supported = ["0",
                                   "1",
                                   "2",
                                   "3",
                                   "4",
                                   "5",
                                   "6"]

        #end elif
        else:
            print "SC not supported in LumiProp.py"
            print "Your entry: ", sc
            print "Supported is: SC1 or SC2"
    #end constructor

    #
    ## Method to get timing range
    #
    def getRange(self, lumi):
        
        for i in range(len(self.lumi_supported)):
            if lumi == self.lumi_supported[i]:
                return [self.regions[i], self.regions[i+1]]

        print "Lumi not supported for ", lumi
        print "Returning value for 100%"
        last = int(len(self.lumi_supported) - 1)
        return [self.regions[last-1], self.per100_high[last]]

    #
    ## Method to retrieve vector of supported lumi percentages
    #
    def getLumis(self):
        return self.lumi_supported
    
    #
    ## Have a method to check to see a given time is in some range
    #
    def lumiInRange(self, lumi, timing):
        timing_pair = self.getRange(lumi)
        if timing_pair[0] < timing and timing <= timing_pair[1]:
            return True
        
        return False
    
