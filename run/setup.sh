#!/bin/bash

# Some miscellanous directories for log files used
# on the condor system will also be created.  You 
# can delete these if you don't need them.

#mkdir outLogs
#mkdir errLogs


# A setup script to source the environment variables
# and anything else needed to run the scripts in this 
# area.

# It is assumed that you checked this out in the 
# build directory of your meta-project, so the
# shell script to be sourced is two directories up

source ../../env-shell.sh
