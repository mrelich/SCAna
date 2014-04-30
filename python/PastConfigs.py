#!/usr/bin/env python

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
# This is just some simple methods to exclude certain strings in #
# order to go back to some previous string configuration. For    #
# example, I want to exclude IC86 strings to go back to IC79     #
# configuration.                                                 #
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

from I3Tray import OMKey

#------------------------------#
# Take me back to IC79
#------------------------------#
def backToIC79():
    keys = [OMKey(a,b) for a in range(79,87) for b in range(1,61)]
    return keys




