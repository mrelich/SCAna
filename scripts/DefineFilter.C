//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// In Aya's script there are several hard coded values  //
// used for the filtering. I want to understand if they //
// are still appropriate for the latest SC run.         //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "../scripts/tools.C"
#include "TTree.h"

// directory to save plots in
TString savedir = "../plots/Unfiltered/";

//------------------------------------------//
// Main
//------------------------------------------//
void DefineFilter(int opt)
{

  // Specify the file you want to examine
  TString dir = "../trees/";
  TFile* infile = new TFile(dir+"SC2_allNearbyDOM_SC100per_DOMcalib_before.tree.root");
  
  // Give the tree name here
  TString treename = "RealTree";

  // Load the tree
  TTree* tree = (TTree*) infile->Get(treename.Data());

  // Options to run here:
  if(opt == 0)      checkUTCTimes(tree);
  else if(opt == 1) checkNDOM(tree);
}

//------------------------------------------//
// Problem 1: there is a function called
// utctimes(...).  I want to figure out 
// where I should place cuts to seperate
// the various luminosities
//------------------------------------------//
void checkUTCTimes(TTree* tree)
{

  // Variables to plot
  TString varx = "startUTCDaqTime_/1.e15"; // scale by 1e15
  TString vary = "DetectorResponseEvent_.totalBestEstimatedNPE_/1000.";
  //TString vary = "AtwdResponseChannels_.estimatedNPE_";

  // Make canvas
  TCanvas* c = makeCanvas("c");
  
  // Make a histogram to save
  TH2F* frame = new TH2F("frame","",1, 279.98, 280.15,1,0,900);
  setAtt(frame,"Start DAQ Time / 10^{14}","Total Best Estimated NPE / 10^{3}");
  frame->Draw();

  // Draw from tree
  tree->Draw((vary+":"+varx).Data(),"","same");
  
  // Save the plot
  TString pname = "timeVsAmp.png";
  c->SaveAs((savedir+pname).Data());

}

//------------------------------------------//
// There is a requirement that at least
// 400 DOMs launch.  I want to look at the 
// average, and maybe also the average for
// given event range.
//------------------------------------------//
void checkNDOM(TTree* tree)
{

  // Plot the number of DOM's per event
  TString var = "DetectorResponseEvent_.totalNumberOfDom_";

  // Make canvas
  TCanvas* c  = makeCanvas("c");

  // Draw
  tree->Draw(var.Data());

}















