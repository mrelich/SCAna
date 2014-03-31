#!/usr/bin/env python

###################################################
# The SC1 doesn't have some simple lumi trend     #
# like was seen for SC2. Therefore I have defined #
# 7 lumi regions which will just be accessed by   #
# those numbers for studying the properties.      #
###################################################


class lumiReg:
    
    # Specify the time ranges for luminosity
    factor      = 10e14
    reg0_low    = 276.679   * factor
    reg0_high   = 276.702   * factor
    reg1_low    = reg0_high
    reg1_high   = 276.723   * factor
    reg2_low    = reg1_high
    reg2_high   = 276.7449  * factor
    reg3_low    = reg2_high
    reg3_high   = 276.7655  * factor
    reg4_low    = reg3_high
    reg4_high   = 276.787   * factor
    reg5_low    = reg4_high
    reg5_high   = 276.8069  * factor
    reg6_low    = reg5_high
    reg6_high   = 276.829   * factor

    # Supported lumis expressed as filter percentages
    reg_supported = ["0","1","2","3","4","5","6"]        

    # Method to get timing range
    def getRange(self, reg):
        if reg == "0":
            return [self.reg0_low, self.reg0_high]
        elif reg == "1":
            return [self.reg1_low, self.reg1_high]
        elif reg == "2":
            return [self.reg2_low, self.reg2_high]
        elif reg == "3":
            return [self.reg3_low, self.reg3_high]
        elif reg == "4":
            return [self.reg4_low, self.reg4_high]
        elif reg == "5":
            return [self.reg5_low, self.reg5_high]
        elif reg == "6":
            return [self.reg6_low, self.reg6_high]
        else:
            print "region not supported for ", reg
            print "Returning value for region 6"
            return [self.reg6_low, self.reg6_high]

    # method to get supported regions
    def getRegions(self):
        return self.reg_supported
    

    # Have a method to check to see a given time is in some range
    def timeInRange(self, reg, timing):
        timing_pair = self.getRange(reg)
        if timing_pair[0] < timing and timing <= timing_pair[1]:
            return True
        
        return False
