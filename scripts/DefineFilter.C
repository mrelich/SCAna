//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// In Aya's script there are several hard coded values  //
// used for the filtering. I want to understand if they //
// are still appropriate for the latest SC run.         //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "../scripts/tools.C"
#include "TTree.h"
#include "TCut.h"

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
  else if(opt == 1) plotNDOM(tree);
  else if(opt == 2) checkNDOM(tree);
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
// Plot nDOM with out any other reqs.
//------------------------------------------//
void plotNDOM(TTree* tree)
{

  // Plot the number of DOM's per event
  TString var = "DetectorResponseEvent_.totalNumberOfDom_";

  // Make canvas
  TCanvas* c  = makeCanvas("c");

  // Create histogram holders
  int nbins = 100;
  int min   = 0;
  int max   = 2000;
  TString xtitle = "nDOMs";
  TString ytitle = "Entries";
  TH1F* hist = makeFrame("hist",nbins,min,max,
			 xtitle,ytitle);

  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.8,0.9);
  leg->SetHeader("Luminosity");

  // Draw
  tree->Draw((var+">>hist").Data(),"","hist");

  // Add to legend
  leg->AddEntry(hist, "All", "l");

  // Now draw the figures
  hist->Draw("hist");
  leg->Draw("same");

  c->SaveAs((savedir+"nDoms_total.png").Data());
}

//------------------------------------------//
// Problem 2: There is a requirement that 
// at least 400 DOMs launch.  I want to look 
// at the average, and maybe also the average 
// for given event range.
//------------------------------------------//
void checkNDOM(TTree* tree)
{

  // Plot the number of DOM's per event
  TString var = "DetectorResponseEvent_.totalNumberOfDom_";

  // Make canvas
  TCanvas* c  = makeCanvas("c");

  // Make a vector of TCuts
  vector<TString> cuts;
  cuts.push_back("1");   // 1%
  cuts.push_back("3");   // 3%
  cuts.push_back("10");  // 10%
  cuts.push_back("30");  // 30%
  cuts.push_back("51");  // 51%
  cuts.push_back("100"); // 100%

  // Create histogram holders
  TH1F* hists[6];
  int nbins = 100;
  int min   = 0;
  int max   = 2000;
  TString xtitle = "nDOMs";
  TString ytitle = "Entries";
  
  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.6,0.9);
  leg->SetHeader("Luminosity");

  // Loop and draw
  float maximum = -999;
  for(unsigned int i=0; i<cuts.size(); ++i){
    // get cut value
    TString cutval = cuts.at(i); 
    // create histogram
    hists[i] = makeFrame("hist_"+cutval,nbins,min,max,
			 xtitle,ytitle);

    // cut the TCut object from value
    TCut cut = lumiCut(cutval);

    // Draw and set att
    tree->Draw((var+">>hist_"+cutval).Data(),cut,"hist");
    setAtt(hists[i],xtitle,ytitle,m_colors[i],m_markers[i]);
    setMax(hists[i],maximum);

    // Add to legend
    leg->AddEntry(hists[i],(cutval+"%").Data(),"l");
  }

  // Now draw the figures
  hists[0]->SetMaximum(maximum*1.1);
  hists[0]->Draw("hist");
  for(unsigned int i=1; i<cuts.size(); ++i)
    hists[i]->Draw("samehist");
  leg->Draw("same");

  c->SaveAs((savedir+"nDoms_perLumi.png").Data());
}

//------------------------------------------//
// Method to place utc time cuts for a 
// given luminosity.  
//------------------------------------------//
TCut lumiCut(TString lumi)
{

  TString low  = 0;
  TString high = 100;

  if(lumi == "1"){
    low = "280*10e14";
    high = "280.02*10e14";
  }
  else if(lumi == "3"){
    low = "280.02*10e14";
    high = "280.045*10e14";
  }
  else if(lumi == "10"){
    low = "280.045*10e14";
    high = "280.065*10e14";
  }
  else if(lumi == "30"){
    low = "280.065*10e14";
    high = "280.085*10e14";
  }
  else if(lumi == "51"){
    low = "280.09*10e14";
    high = "280.11*10e14";
  }
  else if(lumi == "100"){
    low = "280.11*10e14";
    high = "280.13*10e14";
  }
  else
    cout<<"Lumi not known"<<endl;
  
  // Construct TCut
  TString var = "startUTCDaqTime_";
  TCut cut = TCut((low+"<"+var+"&&"+var+"<"+high).Data());
  return cut;

}


//------------------------------------------//
// Problem 3: There is a requrement on the
// cutwavetime(...) which I do not fully 
// understand.  However, I may not be able
// to easily do this from the output tree.
// Therefore I will first try to look at it
// with a python script looping over I3file
// that was output with the tree.
//------------------------------------------//











