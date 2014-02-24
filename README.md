SCAna
=====

Standard Candle Analysis code repository.

This code will slowly evolve to contain all the scripts necessary
to run the standard candle analysis.  Since I will be writing a 
lot of this for the first time, it should be heavily annotated and
serve as launching point for future projects

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//                          PACKAGE STRUCTURE                             //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

python:
	This directory will contain all the code necessary to process the
	input i3 files and produce ROOT trees for analysis. I hope to make
	it more modular so that one can easily toggle on and off modules 
	and settings for each run.

scripts:
	This directory will contain all the analysis scripts to make the 
	plots.  So far, it seems like everyone uses TCuts to plot things
	directly from the tree.  This is ok as a first step, since I really
	don't know much about how they are storing their tree info.  Might
	want something more complicated in the future.

trees:
	This is where the root trees will be written too.  Since some will 
	probably be big, .gitignore is configured to never commit the output
	to github.

i3files:
	Similarly for trees, this will contain the i3 output files that are
	generated after filtering and after making MC.  I am not sure if these
	are useful, but for now I will keep them saved.  In the future I can
	change this if needed.

plots:
	This will store the plots generated from the scripts.  Most likely I will
	save them as .png or some similar small format.

run:
	This is where everything should be executed from.  The structure will be
	made such that the analysis can be done from this directory.

Changelog:
	Try to keep track of changes and tags.  For example, when a meta-release
	is updated and those changes are propagated to the users' code, make
	a tag so that one can go backwards and run on old SC data.

