//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// This script will plot waveforms for given string //
// and DOM for MC vs. Simulation.                   //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "tools.C"

int scString = 55;

// Get waveform channel
WaveFormChannel* chan = new WaveFormChannel();

// Savedirectory
//TString savedir = "../plots/DataMCWaveforms/";
TString savedir = "../plots/DataMCWaveforms_WavetimeCut/";

//--------------------------------------//
// Main
//--------------------------------------//
void WaveFormComp()
{

  // Specify the DOMs to look at
  vector<int> doms;
  doms.push_back(40);
  doms.push_back(41);
  doms.push_back(42);
  doms.push_back(43);
  doms.push_back(44);
  doms.push_back(45);
  doms.push_back(46);
  doms.push_back(48);

  // Specify the lumis to plot
  vector<TString> lumis;
  //lumis.push_back("1");
  //lumis.push_back("3");
  //lumis.push_back("10");
  lumis.push_back("30");
  lumis.push_back("51");
  lumis.push_back("100");

  // Build Files
  TString indir = "../trees/";
  vector<TString> f_data;
  vector<TString> f_mc;
  for(unsigned int f=0; f<lumis.size(); ++f){
    //f_data.push_back(indir+"SC2_filter"+lumis.at(f)+"_cutNDOM400_WaveCalib_Waveform_tree.root");
    //f_data.push_back(indir+"SC2_filter"+lumis.at(f)+"_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";)
    f_data.push_back(indir+"SC2_filter"+lumis.at(f)+"_cutNDOM400_TRCut_TimeCut_WaveCalib_Waveform_tree.root");
    f_mc.push_back(indir+"SC2_SpiceMie_filter"+lumis.at(f)+"_TA0003_Waveform_tree.root");
  }

  // Plot waveforms
  plotWaveforms(f_data, f_mc, lumis, doms);

}

//--------------------------------------//
// Plot Waveforms
//--------------------------------------//
void plotWaveforms(vector<TString> f_datas,
		   vector<TString> f_mcs,
		   vector<TString> lumis,
		   vector<int> doms)
{

  // Make Canvas
  TCanvas* c = makeCanvas("c");
  
  // Make a generic frame
  TH1F* frame = makeFrame("h",1,0,500,"time [ns]","Voltage [V]");
  frame->SetMinimum(0);
  frame->SetMaximum(7.5);
  frame->Draw();

  // Make legend
  TLegend* leg = makeLegend(0.15,0.30,0.8,0.95);
  
  // Dummy histograms for the legend
  TH1F* h_dt = makeFrame("data",1,0,500,"","");
  h_dt->SetLineColor(kBlack);
  TH1F* h_mc = makeFrame("mc",1,0,500,"","");
  h_mc->SetLineColor(kBlue);
  
  
  // Now loop over the files
  vector<TString> f_current;
  int colors[] = {kBlack, kBlue};
  for(unsigned int f=0; f<f_datas.size(); ++f){
    f_current.clear();
    f_current.push_back( f_datas.at(f) );
    f_current.push_back( f_mcs.at(f) );

    // Set Lumi
    TString lumi = lumis.at(f);

    // Printout
    cout<<"Working on lumi: "<<lumi<<endl;

    // For this file, loop over the different DOMs
    for(unsigned int d=0; d<doms.size(); ++d){
      int DOM = doms.at(d);

      // Output current DOM
      cout<<"\tCurrent DOM = "<<DOM<<endl;

      // Draw the frame
      frame->Draw();

      // Clear legend and add header
      leg->Clear();
      stringstream ss;
      ss << "String: " << scString << " DOM: " << DOM;
      ss << " Filter: " << lumi << "%";
      leg->SetHeader(ss.str().c_str());
      leg->AddEntry(h_dt, "Data", "l");
      leg->AddEntry(h_mc, "Spice-Mie","l");

      // Now loop over the current files
      for(unsigned int cf=0; cf<f_current.size(); ++cf){
	int color = colors[cf];

	// Create Chain
	TChain* ch = new TChain("WaveFormTree");
	ch->Add(f_current.at(cf).Data());
	Long64_t ch_entries = ch->GetEntries();
	
	// Set Tree Address
	WaveFormTree* tree = new WaveFormTree();
	ch->SetBranchAddress("waveform",&tree);
	
	// Loop over chain plotting
	for(int entry = 0; entry<ch_entries; ++entry){
	  ch->GetEvent(entry);
	  
	  TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
	  plotLoop(chan_array, color, c, DOM);

	}// end loop over entries
	
	delete tree;
	delete ch;
	
      }// end loop over current file
      
      // Here we plot legend and save
      leg->Draw("same");

      // come up with output name
      stringstream save;
      save << savedir << "SC2_string" << scString;
      save << "_DOM" << DOM;
      save << "_filter" << lumi;
      save << ".png";
      //cout<<save.str()<<endl;
      c->SaveAs(save.str().c_str());
      
    }// end loop over doms
    
  }// end loop over all files
  
  delete c;
  

}

//--------------------------------------//
// Plot graphs
//--------------------------------------//
void plotLoop(TClonesArray* ch_array, 
	      int color,
	      TCanvas* &c,
	      int DOM)
{

  c->cd();
  
  // Get Number of Channels
  int nCh = ch_array->GetEntries();  
  for(int j=0; j<nCh; ++j){
    chan = (WaveFormChannel*) ch_array->At(j);

    // Check DOM and string
    int OMNum     = chan->GetOMNumber();
    int stringNum = chan->GetStringNumber();
    if( stringNum != scString || OMNum != DOM ) continue;
    
    // Get the graph 
    TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
    atwd->SetLineColor(color);
    atwd->SetLineWidth(1.5);
    
    // Reset the start time
    resetTime(atwd,0);
    
    // Draw
    atwd->DrawClone("LPSame");
    
  }// end loop over ch entries

  //delete chan;

}
