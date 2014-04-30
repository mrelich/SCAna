//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Script to analyze the waveform tree.  At first //
// I will focus on SC2, but later try to update   //
// to also look at SC1.                           //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "tools.C"
#include <sstream>

bool isMC = false;
//bool isMC = true;
//TString savedir   = "../plots/Waveform/";
//TString savedir   = "../plots/Waveform_WaveTimeCut/";
TString savedir   = "../plots/Waveform_TRCut/";

//------------------------------------//
// Main
//------------------------------------//
void WaveFormAna(int fopt, int ropt)
{

  // fopt = file options
  // ropt = run options

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
  //DOMs.push_back(42);
  DOMs.push_back(43);
  //DOMs.push_back(44);
  //DOMs.push_back(45);
  //DOMs.push_back(46);
  //DOMs.push_back(48);

  // Deal with options
  if(fopt == 0){               // Standard Candle 2
    //lumis.push_back("1");
    //lumis.push_back("3");
    //lumis.push_back("10");
    //lumis.push_back("30");
    lumis.push_back("51");
    //lumis.push_back("100");
    TString base = indir+"SC2_filter";    
    if(isMC) base = indir+"SC2_SpiceMie_filter";
    for(unsigned int i=0; i<lumis.size(); ++i){
      //if(!isMC) fnames.push_back(base + lumis.at(i) + "_cutNDOM400_WaveCalib_Waveform_tree.root");
      if(!isMC) fnames.push_back(base + lumis.at(i) + "_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root");
      else      fnames.push_back(base + lumis.at(i) + "_TA0003_Waveform_tree.root");
    }// end loop over files
  }
  else if(fopt == 1){           // Standard Candle 1
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
  if(ropt == 0)
    for(unsigned int i=0; i<lumis.size(); ++i)
      for(unsigned int id=0; id<DOMs.size(); ++id)
	plotWaveforms(fnames.at(i),lumis.at(i),
		      sc_string, DOMs.at(id));
  else if(ropt == 1)
    //for(unsigned int i=0; i<lumis.size(); ++i)
    for(unsigned int id=0; id<DOMs.size(); ++id)
      //plotDeltaT(fnames.at(i), lumis.at(i), sc_string, DOMs.at(id));
      plotDeltaT(fnames, lumis, sc_string, DOMs.at(id));

  else if( ropt == 2)
    for(unsigned int i=0; i<lumis.size(); ++i)
      plotWavetimeVsNPE(fnames.at(i),lumis.at(i));

  else if( ropt == 3)
    for(unsigned int i=0; i<lumis.size(); ++i)
      plotV(fnames.at(i),lumis.at(i));
  
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
  TH1F* frame = makeFrame("h",1,0,500,"time [ns]","Voltage [V]");
  frame->SetMinimum(0);
  frame->SetMaximum(7.5);
  frame->Draw();

  // Filter name
  //TString filter = "Filter: " + lumi;
  stringstream filter;
  filter << "Filter: " << lumi << "%";
  
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
      if(isMC) atwd->SetLineColor(kBlue);
      else     atwd->SetLineColor(kBlack);
      atwd->SetLineWidth(1.5);

      // **** NEW ****
      // Timing cut for check
      double end, V;
      int npoints = atwd->GetN();
      atwd->GetPoint(npoints-1,end,V);

      double maxloc = findLocationMax(atwd);

      //if( end - maxloc < 300 ) continue;

      // Reset so it starts at zero
      resetTime(atwd, 0);

      // Add legend if not added
      if( nUsed == 0) leg->AddEntry(atwd,filter.str().c_str(),"l");	

      // Draw
      atwd->DrawClone("LPSame");
      
      // count
      nUsed++;
      
    }// end loop over DOMs
  }// end loop over events
  leg->Draw("same");
  
  // Save the canvas
  stringstream save;
  save << savedir << "SC2_string" << sc_string;
  save << "_DOM" << DOM;
  save << "_filter" << lumi;
  if(isMC) save << "_sim";
  else     save << "_data";
  save << ".png";
  cout<<save.str()<<endl;

  // Save
  c->SaveAs(save.str().c_str());

  // clean
  /*
  delete frame;
  delete c;
  delete leg;
  //delete chan;
  //delete tree;
  delete chain;
  */
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
  hist->SetTitle("");
  hist->SetStats(0);

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

//------------------------------------//
// Looking at the delta T of all 
// the waveforms on one string for
// given lumi
//------------------------------------//
void plotDeltaTIndividual(TString fname, TString lumi,
			  int sc_string, int DOM)
{

  // Make canvas for plotting
  TCanvas* c = makeCanvas("c");
  c->SetLogy();

  // Get Chain
  TChain* chain = new TChain("WaveFormTree");
  chain->Add(fname.Data());
  Long64_t ch_entries = chain->GetEntries();

  // Set Tree address
  WaveFormTree* tree = new WaveFormTree();
  chain->SetBranchAddress("waveform",&tree);

  // New waveform channel
  WaveFormChannel* chan = new WaveFormChannel();

  // Look at delta T from amx to end time
  TH1F* h_dT_max  = makeFrame("dT_max",100,0,500,
			      "#Deltat [ns]",
			      "Entries/bin");
  
  // Add Legend
  TLegend* leg = makeLegend(0.15,0.3,0.8,0.95);  

  // Loop over entries and draw              
  int nUsed = 0;
  for(int entry=0; entry<ch_entries; ++entry){
    chain->GetEvent(entry);
  
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);

      // Check DOM and string we want to plot
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();
      if( OMNum != DOM || sc_string != stringNum ) continue;


      // Get waveform graph
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
      
      // Now get the relevant times
      // start = when waveform started
      // end   = when waveform is over
      // maxloc = when waveform is at max
      double end, V;
      int npoints = atwd->GetN();
      atwd->GetPoint(npoints-1,end,V);
      
      double maxloc = findLocationMax(atwd);
      
      h_dT_max->Fill(end - maxloc);
      
      
    }// end loop over DOMs
    
  }// end loop over events
  
  //h_dT_full->Draw();
  h_dT_max->Draw();

}

//------------------------------------//
// Plot delta T for each waveform
// for a given string and dom for all
// the lumis together.
//------------------------------------//
void plotDeltaT(vector<TString> fnames, vector<TString> lumis,
		int sc_string, int DOM)
{
  
  // Make canvas for plotting
  TCanvas* c = makeCanvas("c");
  //c->SetLogy();

  // Look at delta T from amx to end time
  TH1F* h_dT_max[6];
  
  // Add Legend
  TLegend* leg = makeLegend(0.15,0.3,0.6,0.95);  
  stringstream ss;
  ss << "String: " << sc_string;
  ss << " DOM: " << DOM;
  leg->SetHeader(ss.str().c_str());

  // Loop over the files
  float maximum = 0;
  for(unsigned int f=0; f<fnames.size(); ++f){
    TString fname = fnames.at(f);
    TString lumi  = lumis.at(f);
    cout<<"Working on..."<<lumi<<endl;

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
    h_dT_max[f] = makeFrame(("dT_max_"+lumi).Data(),100,0,500,
			    "#Deltat [ns]",
			    "Entries/bin");
    h_dT_max[f]->SetLineColor(m_colors[f]);
    leg->AddEntry(h_dT_max[f], (lumi + "%").Data(), "l");

    // Loop over entries and draw              
    int nUsed = 0;
    for(int entry=0; entry<ch_entries; ++entry){
      chain->GetEvent(entry);
      
      TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
      int nChannels = chan_array->GetEntries();      

      //if(tree->totalBestNpeLog_ > 5.88) continue;

      for(int j=0; j<nChannels; ++j){
	chan = (WaveFormChannel*) chan_array->At(j);
	
	// Check DOM and string we want to plot
	int OMNum = chan->GetOMNumber();
	int stringNum = chan->GetStringNumber();
	if( OMNum != DOM || sc_string != stringNum ) continue;
	
	
	// Get waveform graph
	TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
	
	// Now get the relevant times
	// start = when waveform started
	// end   = when waveform is over
	// maxloc = when waveform is at max
	//double end, V;
	//int npoints = atwd->GetN();
	//atwd->GetPoint(npoints-1,end,V);
	//double maxloc = findLocationMax(atwd);
	
	// Try plotting dT between start time of wave and 
	// leading edge (above 0.1 threshold)
	double start = chan->GetAtwdStartTime();
	double tThresh = 0;
	int np = atwd->GetN();
	double t=0, V=0;
	for(int n=0; n<np; ++n){
	  atwd->GetPoint(n,t,V);
	  if( V > 0.1 ){
	    tThresh = t;
	    break;
	  }
	}	      

	//h_dT_max[f]->Fill(end - maxloc);
	h_dT_max[f]->Fill(tThresh - start);
      
      
      }// end loop over DOMs
      
    }// end loop over events
    
    delete chain;
    delete tree;
    
    // Set maximum
    if( maximum < h_dT_max[f]->GetMaximum() )
      maximum = h_dT_max[f]->GetMaximum();
    
  }// end loop over files
  
  h_dT_max[0]->SetMaximum(1.2*maximum);
  h_dT_max[0]->Draw();
  for(unsigned int f=1; f<fnames.size(); ++f)
    h_dT_max[f]->Draw("same");
  
  leg->Draw("same");

  // save name
  stringstream save;
  save << savedir << "SC2_deltaT";
  save << "_string" << sc_string;
  save << "_DOM" << DOM;
  if(isMC ) save << "_sim";
  else      save << "_data";
  save << ".png";
  
  // Save the plot
  //c->SaveAs(save.str().c_str());

  // Clean
  //delete c;
  //for(unsigned int f=0; f<fnames.size(); ++f)
  //  delete h_dT_max[f];
  //delete leg;
}

//------------------------------------//
// Plot the voltage distributions of 
// DOMs on the SC string for a given 
// luminosity
//------------------------------------//
void plotV(TString fname, TString lumi)
{

  // Make plot for following DOMs
  int sc_string = 55;
  vector<int> DOMs; 
  /*
  DOMs.push_back(37);
  DOMs.push_back(38);
  DOMs.push_back(39);
  DOMs.push_back(40);
  DOMs.push_back(41);
  DOMs.push_back(42);
  DOMs.push_back(43);
  DOMs.push_back(44);
  DOMs.push_back(45);
  DOMs.push_back(46);
  DOMs.push_back(47);
  DOMs.push_back(48);
  DOMs.push_back(49);
  DOMs.push_back(50);
  DOMs.push_back(51);
  */
  DOMs.push_back(42);
  
  int colors[] = {kBlack, kBlue, kRed, kYellow+2, kGreen+2,
		  kCyan+1, kMagenta, kOrange+8, kGreen-1, kBlue +2,
		  kViolet+3, kPink+7, kOrange+2, kSpring-5, kAzure+9};
  
  // Make canvas for plotting
  TCanvas* c = makeCanvas("c");
  //c->SetLogy();
  c->SetLogz();

  // Look at delta T from amx to end time
  TH1F* h_V[15];
  TH2F* h_tVsV = new TH2F("tVsV","",100,0,500,100,5.8,5.95);
  h_tVsV->SetStats(0);

  // Add Legend
  TLegend* leg = makeLegend(0.15,0.3,0.6,0.95);  
  leg->SetHeader("DOM Number");

  // Get Chain
  TChain* chain = new TChain("WaveFormTree");
  chain->Add(fname.Data());
  Long64_t ch_entries = chain->GetEntries();
  
  // Set Tree address
  WaveFormTree* tree = new WaveFormTree();
  chain->SetBranchAddress("waveform",&tree);
    
  // New waveform channel
  WaveFormChannel* chan = new WaveFormChannel();
  
  // Loop over the doms
  stringstream ss;
  float maximum = -999;
  for(unsigned int d=0; d<DOMs.size(); ++d){
    int DOM = DOMs.at(d);
    ss.str("");
    ss << DOM;
    cout<<"Working on DOM: "<<DOM<<endl;

    // Create histogram
    h_V[d] = makeFrame(("V_DOM"+ss.str()).c_str(),100,0,7,
			    "Peak Voltage [V]",
			    "Entries/bin");
    h_V[d]->SetLineColor(colors[d]);

    // Add to legend
    leg->AddEntry(h_V[d],ss.str().c_str(),"l");

    // Loop over entries and fill histograms
    for(int entry=0; entry<ch_entries; ++entry){
      chain->GetEvent(entry);
      
      TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
      int nChannels = chan_array->GetEntries();      
      
      for(int j=0; j<nChannels; ++j){
	chan = (WaveFormChannel*) chan_array->At(j);
	
	// Check DOM and string we want to plot
	int OMNum     = chan->GetOMNumber();
	int stringNum = chan->GetStringNumber();
	if( OMNum != DOM || sc_string != stringNum ) continue;
	
	
	// Get waveform graph
	TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
	
	// Get maximum
	double Vmax = 0;
	double dT   = 0;
	double end = 0;
	double t = 0, V = 0;
	int np = atwd->GetN();
	atwd->GetPoint(np-1,end,V);
	for(int n = 0; n<atwd->GetN(); ++n){
	  atwd->GetPoint(n,t,V);
	  if( V > Vmax ){
	    Vmax = V;
	    dT   = end - t;
	  }
	}
	h_V[d]->Fill(Vmax);
	h_tVsV->Fill(dT,tree->totalBestNpeLog_);
	
      }// end loop over DOMs
      
    }// end loop over entries
    
   
    // Set maximum
    if( maximum < h_V[d]->GetMaximum() )
      maximum = h_V[d]->GetMaximum();
    
  }// end loop over DOMs
  
  delete chain;
  delete tree;

  //h_V[0]->SetMaximum(1.2*maximum);
  //h_V[0]->Draw();
  //for(unsigned int d=1; d<DOMs.size(); ++d)
  //  h_V[d]->Draw("same");
  
  //leg->Draw("same");

  h_tVsV->Draw("colz");

  // save name
  stringstream save;
  save << savedir << "SC2_deltaT";
  save << "_string" << sc_string;
  save << "_DOM" << DOM;
  if(isMC ) save << "_sim";
  else      save << "_data";
  save << ".png";
  
  // Save the plot
  //c->SaveAs(save.str().c_str());

  // Clean
  //delete c;
  //for(unsigned int f=0; f<fnames.size(); ++f)
  //  delete h_V[f];
  //delete leg;
}


