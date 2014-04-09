//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Script to analyze the waveform tree.  At first //
// I will focus on SC2, but later try to update   //
// to also look at SC1.                           //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "tools.C"
#include <sstream>

//------------------------------------//
// Main
//------------------------------------//
void WaveFormAna(int opt)
{

  // Some necessary variables
  vector<TString> fnames;
  vector<TString> fnames1;
  vector<TString> lumis;
  TString indir = "../trees/";

  // String and DOM's to look at
  int sc_string = 55;
  vector<int> DOMs;
  //DOMs.push_back(40);
  //DOMs.push_back(41);
  DOMs.push_back(42);
  //DOMs.push_back(43);

  // Deal with options
  if(opt == 0){               // Standard Candle 2
    //lumis.push_back("1");
    //lumis.push_back("3");
    //lumis.push_back("10");
    //lumis.push_back("30");
    //lumis.push_back("51");
    lumis.push_back("100");
    TString base = indir+"SC2_String54_55_waveform_filter";    
    for(unsigned int i=0; i<lumis.size(); ++i)
      fnames.push_back(base + lumis.at(i) + ".root");
    
  }
  else if(opt == 1){           // Standard Candle 1
    lumis.push_back("0");
    //lumis.push_back("1");
    ////lumis.push_back("2");
    //lumis.push_back("3");
    //lumis.push_back("4");
    //lumis.push_back("5");
    //lumis.push_back("6");
    TString base = indir+"SC1_String54_55_waveform_filter";
    for(unsigned int i=0; i<lumis.size(); ++i)
      fnames.push_back(base + lumis.at(i) + ".root");
  }
  else if(opt == 2){
    lumis.push_back("1");
    //lumis.push_back("3");
    TString base = indir+"SC2_String54_55_waveform_filter";    
    for(unsigned int i=0; i<lumis.size(); ++i){
      fnames.push_back(base + lumis.at(i) + "_reg0.root");
      fnames1.push_back(base + lumis.at(i) + "_reg1.root");
    }
  }
  else{
    cout<<"Option not supported"<<endl;
    return;
  }

  // Plot all waveforms
  if(opt == 0 || opt == 1)
    for(unsigned int i=0; i<lumis.size(); ++i)
      for(unsigned int id=0; id<DOMs.size(); ++id)
	plotWaveforms(fnames.at(i),lumis.at(i),
		      sc_string, DOMs.at(id));
  
  //for(unsigned int i=0; i<lumis.size(); ++i)
  //plotWavetimeVsNPE(fnames.at(i),lumis.at(i));
  
  // Plot region comparison
  if( opt == 2 )
    for(unsigned int i=0; i<lumis.size(); ++i)
      plotRegionComparison(fnames.at(i), fnames1.at(i), lumis.at(i));

}

//------------------------------------//
// Plot the waveforms
//------------------------------------//
void plotWaveforms(TString fname, TString lumi,
		   int sc_string, int DOM)
{

  // Make a generic canvas
  TCanvas* c = makeCanvas("c");

  // Create TChain
  TChain* chain = new TChain("WaveFormTree");
  chain->Add(fname.Data());
  Long64_t ch_entries = chain->GetEntries();
  cout<<"There are: "<<ch_entries<<" entries"<<endl;
  // Set Tree Address
  WaveFormTree* tree = new WaveFormTree();
  chain->SetBranchAddress("waveform",&tree);

  // Create a waveform channel
  WaveFormChannel* chan = new WaveFormChannel();

  // Make frame for histogram
  //TH1F* frame = makeFrame("h",1,9.5e3,10.7e3,"time [ns]","Voltage [V]");
  TH1F* frame = makeFrame("h",1,9.5e3,15e3,"time [ns]","Voltage [V]");
  frame->SetMinimum(0);
  frame->SetMaximum(7);
  frame->Draw();

  // Filter name
  TString filter = "Filter: " + lumi;
  
  // TLegend
  TLegend* leg = makeLegend(0.15,0.3, 0.95, 0.85);
  stringstream ss;
  ss << "String: " << sc_string;
  ss <<" DOM: "<< DOM;
  leg->SetHeader(ss.str().c_str());
  
  // Loop over entries and draw
  int nUsed = 0;
  for(int entry=0; entry<ch_entries; ++entry){
    chain->GetEvent(entry);

    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);
      
      // Check what DOM and string we are looking at
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();
      if( stringNum != sc_string || OMNum != DOM ) continue;
      
      // Get Graph and plot
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
      atwd->SetLineColor(kBlue);
      atwd->SetLineWidth(1.5);
      //if( nUsed == 0) leg->AddEntry(atwd,filter.Data(),"l");	
      atwd->DrawClone("LPSame");
      
      nUsed++;
      
    }// end loop over DOMs
  }// end loop over events
  leg->Draw("same");

}

//------------------------------------//
// Plot wave form LE start time vs.
// the best NPE
//------------------------------------//
void plotWavetimeVsNPE(TString fname, TString lumi)
{

  // Make canvas for plotting
  TCanvas* c = makeCanvas("c");

  // Get Chain
  TChain* chain = new TChain("WaveFormTree");
  chain->Add(fname.Data());
  Long64_t ch_entries = chain->GetEntries();

  // Set Tree address
  WaveFormTree* tree = new WaveFormTree();
  chain->SetBranchAddress("waveform",&tree);

  // New waveform channel
  WaveFormChannel* chan = new WaveFormChannel();

  // Make histogram
  TH2F* hist = new TH2F("hist","",1000,9000,11000,100,0,10);

  // Loop over entries and draw              
  int nUsed = 0;
  for(int entry=0; entry<ch_entries; ++entry){
    chain->GetEvent(entry);
    //if(nUsed > 100) break;                                                                        
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);

      // Check DOM and string we want to plot
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();
      if( stringNum != 55 || OMNum !=42 ) continue;
      
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
      double time, V;
      atwd->GetPoint(0,time,V);
      //double time = chan->GetAtwdStartTime();
      double NPE  = tree->totalBestNpeLog_;
      cout<<"Time: "<<time<<" NPE: "<<NPE<<endl;

      hist->Fill(time,NPE);

    }// end loop over DOMs
    
  }// end loop over events

  hist->Draw("");

}

//------------------------------------//
// Plot region comparison
//------------------------------------//
void plotRegionComparison(TString fname0,
			  TString fname1,
			  TString lumi)
{

  cout<<"Plotting Region Comparison"<<endl;

  // Canvas
  TCanvas* c = makeCanvas("c");

  // put names in a fector
  vector<TString> regions;
  regions.push_back(fname1);
  regions.push_back(fname0);

  vector<TString> legnames;
  legnames.push_back("Region 2");
  legnames.push_back("Region 1");

  // Specify colors
  int colors[] = {kBlack, kRed};

  // Make and draw frame
  TH1F* frame = makeFrame("h",1,9.5e3,15.8e3,"time [ns]","Voltage [V]");
  frame->SetMinimum(0);
  frame->SetMaximum(7.5);
  frame->Draw();

  // Make Legend
  TLegend* leg = makeLegend(0.15,0.3,0.95,0.8);
  leg->SetHeader(("Filter = "+lumi+"%").Data());

  for(unsigned int ir=0; ir<regions.size(); ++ir){
    
    TChain* chain = new TChain("WaveFormTree");
    chain->Add(regions.at(ir));
    Long64_t ch_entries = chain->GetEntries();

    cout<<"Working on Chain: "<<ir<<endl;
    
    // Address
    WaveFormTree* tree = new WaveFormTree();
    chain->SetBranchAddress("waveform",&tree);
    
    // Waveform channel
    WaveFormChannel* chan = new WaveFormChannel();

    // Loop and save
    int nUsed = 0;
    for(int entry=0; entry<ch_entries; ++entry){
      chain->GetEvent(entry);
      
      TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
      int nChannels = chan_array->GetEntries();
      for(int j=0; j<nChannels; ++j){
	chan = (WaveFormChannel*) chan_array->At(j);
	
        int OMNum = chan->GetOMNumber();
	int stringNum = chan->GetStringNumber();
	if( stringNum != 55 || OMNum !=42 ) continue;
	
	TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
	atwd->SetLineColor(colors[ir]);
	atwd->SetLineWidth(1.5);
	atwd->DrawClone("LPSame");
	if( nUsed == 0 ) leg->AddEntry(atwd,legnames.at(ir).Data(),"l");
	nUsed++;
      }// end loop over DOMs
      
    }// end loop over events
    
    //delete chan;
    //delete tree;
    //delete chain;
  }// end loop over chains

  leg->Draw("same");
  TString save = "../plots/CutwavetimeCheck/WaveformsPerRegion_filter"+lumi+".png";
  //c->SaveAs(save.Data());
  //delete c;
  //delete frame;
}
