
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// This plot will draw the number of photo electrons for data and //
// simulation in order to determine that shift of data w.r.t.     //
// the simulated events.                                          //
// -------------------------------------------------------------- //
// NOTE: Right now just plotting data because I don't have MC     //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


// My plotting tools
#include "../anaScripts/tools.C"

//----------------------------------------------------//
// Main
//----------------------------------------------------//
void DrawNPE()
{

  //
  // Specify files
  //

  vector<TFile*> files;
  files.push_back(new TFile("../trees/sc1_detectortree.root"));
  
  vector<TString> treeNames;
  treeNames.push_back("RealTree");

  vector<TString> fnames;
  fnames.push_back("SC1");
  

  //
  // Load the trees
  //
  vector<TTree*> trees;
  cout<<"*********************************************"<<endl;
  for(unsigned int f=0; f<files.size(); ++f){
    trees.push_back( (TTree*) files.at(f)->Get(treeNames.at(f).Data()) );
    TTree* temp = trees.back();
    cout<<"File: "<<fnames.at(f)<<" has "<<temp->GetEntries()<<endl;
  }
  cout<<"*********************************************"<<endl;

  // Plot NPE
  //plot(trees, fnames);

}

//----------------------------------------------------//
// Plotting method.
// Currently only one now, but could expand.
//----------------------------------------------------//
void plot(vector<TTree*> trees, vector<TString> fnames)
{
  

}
