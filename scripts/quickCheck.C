
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// TO BE DELETED.  Just a quick check
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "tools.C"

//-----------------------------//
// Main
//-----------------------------//
void quickCheck()
{

  // DOM to plot
  vector<TString> DOMs;
  DOMs.push_back("40");
  DOMs.push_back("41");
  DOMs.push_back("42");
  DOMs.push_back("43");
  DOMs.push_back("44");
  DOMs.push_back("45");
  DOMs.push_back("46");
  DOMs.push_back("48");

  // Lumis to plot
  vector<TString> lumis;
  lumis.push_back("1");
  lumis.push_back("3");
  lumis.push_back("10");
  lumis.push_back("30");
  lumis.push_back("51");
  lumis.push_back("100");


  // Make canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();

  // make legend
  TLegend* leg = makeLegend(0.15,0.3,0.7,0.93);
  
  // Set histograms
  TH1* hists[6];

  // Loop over the doms
  for(unsigned int d=0; d<DOMs.size(); ++d){
    TString dom = DOMs.at(d);

    leg->Clear();
    leg->SetHeader(("String: 55 DOM: "+dom).Data());

    float maximum = -999;
    for(unsigned int l=0; l<lumis.size(); ++l){
      TString lumi = lumis.at(l);

      // Open file
      TFile* file = new TFile(("../plots/RootPlots/cutDT_"+lumi+"per.root").Data());
      
      // Get histogram
      TString fname = "h_DOM"+dom;
      hists[l] = getHist(file, fname, "#Deltat [ns]", "Entries/bin",
			 m_colors[l], 20);
      hists[l]->SetDirectory(0);

      // Close file
      delete file;
      
      // get maximum 
      if( maximum < hists[l]->GetMaximum() )
	maximum = hists[l]->GetMaximum();
      
      // add legend
      leg->AddEntry(hists[l], (lumi + "%").Data(), "l");

    }// have histograms
    
    hists[0]->SetMaximum(1.2*maximum);
    hists[0]->Draw();
    for(unsigned int l=1; l<lumis.size(); ++l)
      hists[l]->Draw("same");
    leg->Draw("same");

    // Save
    TString save = "../plots/FilterTimingCheck/deltaT_DOM"+dom+".png";
    c->SaveAs(save.Data());

    // delete
    for(unsigned int l=0; l<lumis.size(); ++l)
      delete hists[l];

  }// end loop over DOMs

}
