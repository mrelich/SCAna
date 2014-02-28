
#include "tools.C"
#include <sstream>

TString chan = "Atwd";
//TString chan = "Fadc";

//--------------------------------------------------//
// Main
//--------------------------------------------------//
void CompareStrings(int opt)
{

  // Vars
  vector<TTree*> trees; 
  vector<TString> tnames;
  TString percent = "0";

  if(opt == 0) percent = "1";
  else if(opt == 1) percent = "3";
  else if(opt == 2) percent = "10";
  else if(opt == 3) percent = "30";
  else if(opt == 4) percent = "51";
  else if(opt == 5) percent = "100";
  else{
    cout<<"Option not supported"<<endl;
    return;
  }

  // Load files
  TFile* f_data = new TFile( buildName(percent, true) );
  TFile* f_mc   = new TFile( buildName(percent, false) );

  // Get trees
  trees.push_back( (TTree*) f_data->Get("RealTree") );
  trees.push_back( (TTree*) f_mc->Get("JulietTree") );

  // Specify names for plots
  tnames.push_back("Data");
  tnames.push_back("SpiceMie");

  // Load trees
  plotStringComp(trees, tnames, percent);

}

//--------------------------------------------------//
// Plotting method
//--------------------------------------------------//
void plotStringComp(vector<TTree*> trees,
		    vector<TString> tnames,
		    TString percent)
{

  // Histogram holders
  TH1F* h_npe_79[10];
  TH1F* h_npe_86[10];

  // Make canvas
  TCanvas* c = makeCanvas("c");

  // Make legend
  TLegend* leg = makeLegend(0.5,0.7,0.7,0.9);

  // Create NPE histogram holders
  for(unsigned int i=0; i<tnames.size(); ++i){
    h_npe_79[i] = makeBestHist("npe_79_"+tnames.at(i), percent);
    setAtt(h_npe_79[i], "Estimated NPE ("+chan+")", "Normalized", m_colors[i],20);
    h_npe_86[i] = makeBestHist("npe_86_"+tnames.at(i), percent);
    setAtt(h_npe_86[i], "Estimated NPE ("+chan+")", "Normalized", m_colors[i+2],20);
  }    
  
  // Total number of events to loop over
  int nEvents[10];
  for(unsigned int it=0; it<trees.size(); ++it)
    nEvents[it] = getNEvents(trees.at(it));

  // The Variable to plot
  TString var = chan+"ResponseChannels_.estimatedNPE_";
  
  float maximum = -999;
  for(unsigned int it=0; it<trees.size(); ++it){
    TTree* tree = trees.at(it);

    double totalNPE_79 = 0;
    double totalNPE_86 = 0;

    leg->AddEntry(h_npe_79[it], tnames.at(it) + " 79 strings", "l");
    leg->AddEntry(h_npe_86[it], tnames.at(it) + " 86 strings", "l");

    int events = nEvents[it];
    for(int event =0; event<events; ++event){
      
      //if( event > 40 ) break; // for testing plotting

      // Printout statement
      if( event % 100 == 0 )
	cout<<"Getting event: "<<event<<" from tree: "<<it<<endl;
      
      // Get total NPE
      totalNPE_79 = getTotalEventNPE(tree, event, var, 79);
      totalNPE_86 = getTotalEventNPE(tree, event, var, 86);

      // Make sure we have something
      if(totalNPE_79 > 10) h_npe_79[it]->Fill(totalNPE_79);    
      if(totalNPE_86 > 10) h_npe_86[it]->Fill(totalNPE_86);
    
      totalNPE_79 = 0;    
      totalNPE_86 = 0; 
    }
    
    // Normalize
    h_npe_79[it]->Scale(1/h_npe_79[it]->Integral());
    h_npe_86[it]->Scale(1/h_npe_86[it]->Integral());

    // Record maximum
    if( maximum < h_npe_79[it]->GetMaximum() )
      maximum = h_npe_79[it]->GetMaximum();
    if( maximum < h_npe_86[it]->GetMaximum() )
      maximum = h_npe_86[it]->GetMaximum();

  }

  // Now draw everything
  h_npe_79[0]->SetMaximum(maximum*1.2);
  h_npe_79[0]->Draw();
  h_npe_86[0]->Draw("same");
  for(unsigned int it = 1; it<trees.size(); ++it){
    h_npe_79[it]->Draw("same");
    h_npe_86[it]->Draw("same");
  }
  leg->Draw("same");

  c->SaveAs("../plots/NPE_ShapeComp/" + chan +"_filter_"+percent+"per.png");

  // Print out the mean information
  cout<<endl;
  cout<<"****************************************"<<endl; 
  for(unsigned int it=0; it<tnames.size(); ++it){
    TString tname = tnames.at(it);
    double mean79 = h_npe_79[it]->GetMean();
    double mean86 = h_npe_86[it]->GetMean();
    cout<<tname<<endl;
    cout<<"\tMean 79: "<<mean79<<endl;
    cout<<"\tMean 86: "<<mean86<<endl;
    cout<<endl;
  }
  cout<<"****************************************"<<endl; 
  cout<<endl;
}

//--------------------------------------------------//
// Get total NPE from an event
//--------------------------------------------------//
double getTotalEventNPE(TTree* ttree, int event, TString var, int maxString)
{

  // Temp histogram to hold stuff
  TH1F* temp = new TH1F("temp","",100000,0,100000);
  
  // Event cut
  stringstream ss;
  ss << "eventNumber_ == " << event;
  TCut eventcut = TCut(ss.str().c_str());

  // Cut on strings
  ss.str("");
  ss << chan + "ResponseChannels_.string_<=" << maxString;
  TCut stringcut = TCut(ss.str().c_str());

  ttree->Draw((var+">>temp"),(eventcut&&stringcut),"hist");
  double eventSum = getSum(temp);
  delete temp;
  return eventSum;

}
//--------------------------------------------------//
// Sum up npe
//--------------------------------------------------//
double getSum(TH1F* hist)
{

  // Get the weighted sum of NPE
  int nbins = hist->GetNbinsX();
  double sum = 0;
  for(int bin=1; bin<=nbins; ++bin)
    sum += bin * hist->GetBinContent(bin);
  
  return sum;

}

//--------------------------------------------------//
// Get number of events
//--------------------------------------------------//
int getNEvents(TTree* ttree)
{

  TH1F* h_nEvents = new TH1F("events","",100000,0,100000);
  ttree->Draw("eventNumber_ >> events");
  int nEvents = h_nEvents->GetEntries();
  cout<<"Number of events: "<<nEvents<<endl;
  delete h_nEvents;

  return nEvents;

}

//--------------------------------------------------//
// Make best histogram
//--------------------------------------------------//
TH1F* makeBestHist(TString name, TString percent)
{
  TString hname = name +"_"+ percent;

  if( percent == "1" )   return new TH1F(hname.Data(),"",100,45e3,65e3);
  if( percent == "3" )   return new TH1F(hname.Data(),"",200,70e3,105e3);
  if( percent == "10" )  return new TH1F(hname.Data(),"",200,130e3,200e3);
  if( percent == "30" )  return new TH1F(hname.Data(),"",200,220e3,350e3);
  if( percent == "51" )  return new TH1F(hname.Data(),"",100,280e3,400e3);
  if( percent == "100" ) return new TH1F(hname.Data(),"",200,500e3,900e3);

  return new TH1F(hname.Data(),"",500,54e3,1000e3);

}

//------------------------------------------------------//                                                                           
// Get tree names                                                                                                                    
//------------------------------------------------------//                                                                           
TString buildName(TString percent, bool isData)
{

  if(isData){
    TString fname = "../trees/SC2_allNearbyDOM_SC";
    fname += percent;
    fname += "per_DOMcalib_before.tree.root";
    return fname;
  }

  TString fname = "../trees/SC2_spicemie_hole100cm_";
  fname += percent;
  fname += "per.root";
  return fname;

}
