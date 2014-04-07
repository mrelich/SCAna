//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Script to analyze the waveform tree.  At first //
// I will focus on SC2, but later try to update   //
// to also look at SC1.                           //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "tools.C"

//------------------------------------//
// Main
//------------------------------------//
void WaveFormAna(int opt)
{

  // Some necessary variables
  vector<TString> fnames;
  vector<TString> lumis;
  TString indir = "../trees/";

  if(opt == 0){               // Standard Candle 2
    //lumis.push_back("1");
    lumis.push_back("3");
    //lumis.push_back("10");
    //lumis.push_back("30");
    //lumis.push_back("51");
    //lumis.push_back("100");
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
  else{
    cout<<"Option not supported"<<endl;
    return;
  }

  // Plot all waveforms
  //for(unsigned int i=0; i<lumis.size(); ++i)
  //plotWaveforms(fnames.at(i),lumis.at(i));

  for(unsigned int i=0; i<lumis.size(); ++i)
    plotWavetimeVsNPE(fnames.at(i),lumis.at(i));
  
}

//------------------------------------//
// Plot the waveforms
//------------------------------------//
void plotWaveforms(TString fname, TString lumi)
{

  // Make a generic canvas
  TCanvas* c = makeCanvas("c");

  // Create TChain
  TChain* chain = new TChain("WaveFormTree");
  chain->Add(fname.Data());
  Long64_t ch_entries = chain->GetEntries();

  // Set Tree Address
  WaveFormTree* tree = new WaveFormTree();
  chain->SetBranchAddress("waveform",&tree);

  // Create a waveform channel
  WaveFormChannel* chan = new WaveFormChannel();

  // Make frame for histogram
  TH1F* frame = makeFrame("h",1,9.5e3,10.5e3,"time [ns]","Voltage [V]");
  frame->SetMinimum(0);
  frame->SetMaximum(7);
  frame->Draw();
  
  // Loop over entries and draw
  int nUsed = 0;
  for(int entry=0; entry<ch_entries; ++entry){
    chain->GetEvent(entry);
    //if(nUsed > 100) break;
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);
      
      // Check what DOM and string we are looking at
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();
      if( stringNum != 55 || OMNum !=42 ) continue;
      //cout<<"OM "<<OMNum<<" string: "<<stringNum<<endl;
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
      atwd->SetLineColor(kBlue);
      atwd->SetLineWidth(1.5);
      atwd->DrawClone("LPSame");
      nUsed++;
    }// end loop over DOMs
  }// end loop over events

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
