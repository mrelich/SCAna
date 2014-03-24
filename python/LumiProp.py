#!/usr/bin/env python

#################################################
# It seems like it's nice to have the SC lumi   #
# slices saved in a common location. I will put #
# the properties here in a lumi class           #
#################################################

class lumiProp:
    
    # Specify the time ranges for luminosity
    factor      = 10e14
    per1_low    = 280     * factor
    per1_high   = 280.02  * factor
    per3_low    = 280.02  * factor
    per3_high   = 280.045 * factor
    per10_low   = 280.045 * factor
    per10_high  = 280.065 * factor
    per30_low   = 280.065 * factor
    per30_high  = 280.085 * factor
    per51_low   = 280.09  * factor
    per51_high  = 280.11  * factor
    per100_low  = 280.11  * factor
    per100_high = 280.13  * factor

    # Supported lumis expressed as filter percentages
    lumi_supported = ["1",
                      "3",
                      "10",
                      "30",
                      "51",
                      "100"]

    # Method to get timing range
    def getRange(self, lumi):
        if lumi == "1":
            return [self.per1_low, self.per1_high]
        elif lumi == "3":
            return [self.per3_low, self.per3_high]
        elif lumi == "10":
            return [self.per10_low, self.per10_high]
        elif lumi == "30":
            return [self.per30_low, self.per30_high]
        elif lumi == "51":
            return [self.per51_low, self.per51_high]
        elif lumi == "100":
            return [self.per100_low, self.per100_high]
        else:
            print "Lumi not supported for ", lumi
            print "Returning value for 100%"
            return [self.per100_low, self.per100_high]

    # Method to retrieve vector of supported lumi percentages
    def getLumis(self):
        return self.lumi_supported
    

    # Have a method to check to see a given time is in some range
    def lumiInRange(self, lumi, timing):
        timing_pair = self.getRange(lumi)
        if timing_pair[0] < timing and timing <= timing_pair[1]:
            return True
        
        return False
