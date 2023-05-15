#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TFile.h"
#include "TPaveStats.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TPolyMarker.h"
#include "TSpectrum.h"
#include "TTree.h"
#include "TString.h"
#include "string.h"
#include <fstream>
#include <iostream>
//#include <filesystem>
#include <dirent.h>
//namespace fs = std::filesystem;
#include <sstream>

using namespace std;

// n0 vas not initialized
int n0;

double landau_gauss(double *x, double *par) {
  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 200.0;      // number of convolution steps
  Double_t sc = 5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow, xupp;
  Double_t step;
  Double_t i;

  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp - xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for (i = 1.0; i <= np / 2; i++) {
    xx = xlow + (i - .5) * step;
    fland = TMath::Landau(xx, mpc, par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0], xx, par[3]);

    xx = xupp - (i - .5) * step;
    fland = TMath::Landau(xx, mpc, par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0], xx, par[3]);
  }

  return (par[2] * step * sum * invsq2pi / par[3]);
}

TF1* LandauGausFit(TH1F &hist)
{
  double max = hist.GetMaximum();
  double lower_bound = hist.GetXaxis()->GetBinLowEdge(hist.FindFirstBinAbove(max/4));
  double upper_bound = hist.GetXaxis()->GetBinUpEdge(hist.FindLastBinAbove (max/4));
  double parameters[4];

  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function

  parameters[0] = 50;

  parameters[1] = hist.GetBinCenter(hist.GetMaximumBin());
  parameters[2] = hist.Integral("width")*1.5;
  parameters[3] = 50;

  //cout << "Lower bound : " << lower_bound << " Upper bound : " << upper_bound << endl;

  TF1 *landau_gauss_function  = new TF1("landau_gauss_function",  landau_gauss, lower_bound, upper_bound, 4);
  TF1 *landau_gauss_nobound = new TF1("landau_gauss_function",  landau_gauss, 0, 4000, 4);
  TF1 *landau_gauss_iterations[3];

  // Set style for convenience-function plots
  landau_gauss_nobound  ->SetLineStyle(5);
  landau_gauss_function ->SetNpx(300);
  landau_gauss_nobound  ->SetNpx(300);
  landau_gauss_function ->SetParameters(parameters);
  landau_gauss_function ->SetParNames("landau width", "mpv", "area", "gauss width");
  // Logging
  landau_gauss_iterations[0] = (TF1*)landau_gauss_function->Clone("0th_iteration");
  landau_gauss_iterations[0]->SetLineColor(46);

  // 1st fit fitting only area
  landau_gauss_function->FixParameter(0, parameters[0]);
  landau_gauss_function->FixParameter(1, parameters[1]);
  landau_gauss_function->FixParameter(3, parameters[3]);
  hist.Fit(landau_gauss_function, "RQB");
  std::cout << "First fit parameters[1] (mpv)  = " << parameters[1] << std::endl;
  // Logging
  landau_gauss_iterations[1] = (TF1*)landau_gauss_function->Clone("1st_iteration");
  landau_gauss_iterations[1]->SetLineColor(43);

  // 2nd fit releasing landau width, mpv and gauss width
  landau_gauss_function->ReleaseParameter(0);
  landau_gauss_function->ReleaseParameter(1);
  landau_gauss_function->ReleaseParameter(3);

  if(landau_gauss_function->GetParameter(0) < 15)   landau_gauss_function->SetParameter(0, 15);
  if(landau_gauss_function->GetParameter(0) > 1000) landau_gauss_function->SetParameter(0, 1000);
  if(landau_gauss_function->GetParameter(3) < 20)   landau_gauss_function->SetParameter(3, 20);
  if(landau_gauss_function->GetParameter(3) > 100)  landau_gauss_function->SetParameter(3, 100);
  landau_gauss_function->SetParLimits(0, 15, 1000);
  landau_gauss_function->SetParLimits(3, 20, 500);

  hist.Fit(landau_gauss_function, "RQB");
  std::cout << "Second fit parameters[1] (mpv) = " << parameters[1] << std::endl;
  // Logging
  landau_gauss_iterations[2] = (TF1*)landau_gauss_function->Clone("2nd_iteration");
  landau_gauss_iterations[2]->SetLineColor(41);

  // 3rd fit with corrected range
  TH1F tmp("tmp", "tmp", 2501, -.5, 2500.5);
  max = landau_gauss_function->GetMaximum();
  tmp.Eval(landau_gauss_function);
  lower_bound = tmp.GetBinCenter(tmp.FindFirstBinAbove(.30*max));
  upper_bound = tmp.GetBinCenter(tmp.FindLastBinAbove (.30*max));
  landau_gauss_function->SetRange(lower_bound, upper_bound);

  // correct parameters landau and gauss width
  landau_gauss_function->GetParameters(parameters);
  if(parameters[3] >= 90. || parameters[3] <= 35.) parameters[3] = 50;
  if(parameters[0] < 30.) parameters[3] = 30;
  landau_gauss_function->SetParameters(parameters);
  landau_gauss_function->SetParLimits(0, 5, 1000);
  landau_gauss_function->SetParLimits(3, 15, 500);
  TFitResultPtr result = hist.Fit(landau_gauss_function, "RQBS");
  std::cout << "Third fit parameters[1] (mpv)  = " << parameters[1] << std::endl;

  landau_gauss_function->GetParameters(parameters);
  landau_gauss_nobound ->SetParameters(parameters);

  // add functions to histograms to be saved in root file
  //hist.GetListOfFunctions()->Add(landau_gauss_iterations[0]);
  //hist.GetListOfFunctions()->Add(landau_gauss_iterations[1]);
  //hist.GetListOfFunctions()->Add(landau_gauss_iterations[2]);
  hist.GetListOfFunctions()->Add(landau_gauss_nobound);
  hist.GetListOfFunctions()->Add(landau_gauss_function, "R");
  std::cout << "Last landau_gauss_function parameter[1] = " << landau_gauss_function->GetParameter(1) << std::endl;

  return landau_gauss_function;
}

TCanvas *params_stats(TH1F *hist2draw, TCanvas *se){
  char stat[100];
  //TCanvas *se = new TCanvas;
  TF1 *gaus0, *gaus1, *gp;

  gaus0 = (TF1*)hist2draw -> FindObject("peak0");
  gaus1 = (TF1*)hist2draw -> FindObject("peak1");
  gp = (TF1*)hist2draw -> FindObject("generpoiss");

  gStyle->SetOptStat();
  //gStyle->SetOptFit(111);
  hist2draw -> Draw("E");
  se->Update();

  TPaveStats *ps = (TPaveStats*)se->GetPrimitive("stats");
  ps->SetName("mystats");
  TList *listOfLines = ps->GetListOfLines();
  ps->DeleteText();
  TText *tconst = ps->GetLineWith("Entries");
  listOfLines->Remove(tconst);
  TText *tconst1 = ps->GetLineWith("Mean");
  listOfLines->Remove(tconst1);
  TText *tconst2 = ps->GetLineWith("Std Dev");
  listOfLines->Remove(tconst2);


  TLatex *params = new TLatex(0,0,"GP parameters:");
  params ->SetTextFont(42);
  params ->SetTextSize(0.0);
  listOfLines->Add(params);


  sprintf(stat, "#chi^{2} / ndf = %4.2f / %4.2d", gp->GetChisquare(), gp->GetNDF());
  TLatex *chisc = new TLatex(0,0,stat);
  chisc ->SetTextFont(42);
  chisc ->SetTextSize(0.0);
  listOfLines->Add(chisc);

  sprintf(stat, "#lambda = %4.2f #pm %3.2f", gp->GetParameter(0), gp->GetParError(0));
  TLatex *lambda = new TLatex(0,0,stat);
  lambda ->SetTextFont(42);
  lambda ->SetTextSize(0.0);
  listOfLines->Add(lambda);

  sprintf(stat, "Gain_{SPE} = %4.2f #pm %3.2f", gp->GetParameter(1), gp->GetParError(1));
  TLatex *gainspe = new TLatex(0,0,stat);
  gainspe ->SetTextFont(42);
  gainspe ->SetTextSize(0.0);
  listOfLines->Add(gainspe);

  sprintf(stat, "Mean_{0} = %5.2f #pm %3.2f", gp->GetParameter(4), gp->GetParError(4));
  TLatex *mean0 = new TLatex(0,0,stat);
  mean0 ->SetTextFont(42);
  mean0 ->SetTextSize(0.0);
  listOfLines->Add(mean0);

  sprintf(stat, "Sigma_{0} = %5.2f #pm %3.2f", gp->GetParameter(5), gp->GetParError(5));
  TLatex *sigma0 = new TLatex(0,0,stat);
  sigma0 ->SetTextFont(42);
  sigma0 ->SetTextSize(0.0);
  listOfLines->Add(sigma0);

  TPolyMarker *pm = (TPolyMarker*)hist2draw->GetListOfFunctions()->FindObject("TPolyMarker");
  hist2draw -> GetListOfFunctions() -> Remove(pm);
  delete pm;

  // the following line is needed to avoid that the automatic redrawing of stats
  hist2draw->SetStats(0);
  se->Modified();
  return se;
}

double generpoiss(double *x, double *p){
  /* Function description is here - https://indico.cern.ch/event/725114/contributions/2983038/attachments/1641357/2621413/he_sipm_dpg2704_v2.pdf#page=6 */
  double xx = x[0];
  double u0 = p[4];
  double s0 = p[5];
  double g = p[1];
  double sG = p[2];
  double u = p[3];
  double lambda = p[0];
  double N = p[6];
  double npeaks = p[7];
  double fitval = 0.;

  for (int k=0; k<npeaks+1; k++) {
    double uk = u + k*lambda;
    double G = u * pow(uk, k-1) * exp(-uk) / TMath::Factorial(k);
    double sk2 = s0*s0 + k*sG*sG;
    double dx = xx - u0 - k*g;
    fitval += N * G * exp(-0.5*dx*dx/sk2) / sqrt(2*TMath::Pi()) / sqrt(sk2);
  }
  return fitval;
}

struct GenPoisParams{
  GenPoisParams():gain(0),u0(0),s0(0),sG(0),lambda(0),navg(0),npeaks(0),gain_spe_01(0),gs0(0),gs1(0),gm0(0),gm1(0),gN0(0),gN1(0),g0Chi2(0), g1Chi2(0), g0NDF(0), g1NDF(0),g0Integral(0),g1Integral(0){}

  /* GP fit */
  double gain, u0, s0, sG, lambda, navg, npeaks;

  /* Gaussian fits */
  double gain_spe_01, gs0, gs1, gm0, gm1;
  double gN0, gN1;
  double g0Chi2, g1Chi2, g0NDF, g1NDF;
  /* Integrals of gaussian fits */
  double g0Integral, g1Integral;

  /* Info from histogram */

} fp;


TF1 *GP(TH1F* hist2proc, int xlow=0, int xhigh=500){
  double npe_peaks, mu_hist, par[6], lastbin_x;
  double binwidth;
  int ntotal;

  hist2proc -> GetXaxis() -> UnZoom();
  binwidth=hist2proc->GetBinWidth(1);
  // uncomment this for day 7
  //xhigh = hist2proc-> GetBinCenter(hist2proc->FindLastBinAbove(0,1)); // in case of only GP in histogram
  hist2proc -> GetXaxis() -> SetRangeUser(xlow, xhigh);
  ntotal=hist2proc->Integral();

  // Find all peaks and write their positions to vector
  TSpectrum s(15);
  int nfound = s.Search(hist2proc, 1.5, "golf", 0.15);
  double *_xpeaks = s.GetPositionX();
  std::vector<float> xpeaks;
  xpeaks.clear();
  if (_xpeaks==nullptr) {
    cout << "_xpeaks is nullptr" << endl;
  } else {
    for (int p = 0; p < nfound; p++) xpeaks.push_back(_xpeaks[p]);
  }

  std::sort(xpeaks.begin(), xpeaks.end());

  // Fit first two peaks with Gauss function. Fit range is 1/3 of distance between two peaks
  TF1 g1("peak0","gaus",xpeaks[0]-(xpeaks[1]-xpeaks[0])/3,xpeaks[0]+(xpeaks[1]-xpeaks[0])/3);
  hist2proc -> Fit(&g1,"RQ");
  TF1 g2("peak1","gaus",xpeaks[1]-(xpeaks[1]-xpeaks[0])/3,xpeaks[1]+(xpeaks[1]-xpeaks[0])/3);
  hist2proc -> Fit(&g2,"RQ+");

  fp.g1Chi2 = g2.GetChisquare();
  fp.g0NDF = g1.GetNDF();
  fp.g1NDF = g2.GetNDF();
  fp.g0Integral = g1.Integral(xpeaks[0]-(xpeaks[1]-xpeaks[0])/3,xpeaks[0]+(xpeaks[1]-xpeaks[0])/3);
  fp.g1Integral = g2.Integral(xpeaks[1]-(xpeaks[1]-xpeaks[0])/3,xpeaks[1]+(xpeaks[1]-xpeaks[0])/3);


  g1.SetLineColor(kGreen+2);
  g2.SetLineColor(kGreen+2);
  g1.GetParameters(&par[0]);
  g2.GetParameters(&par[3]);

  // Get mean and sigma of two Gaussians
  fp.gN0 = g1.GetParameter(0); // g1params[0]
  fp.gm0 = g1.GetParameter(1);
  fp.gs0 = g1.GetParameter(2);
  fp.gN1 = g2.GetParameter(0);
  fp.gm1 = g2.GetParameter(1);
  fp.gs1 = g2.GetParameter(2);
  fp.gain_spe_01 = fp.gm1 - fp.gm0;

  // Set new range for histogram (Mu1-Mu0)/2 around Mu0
  hist2proc->GetXaxis()->SetRangeUser(fp.gm0-(fp.gm1-fp.gm0)/2,fp.gm0+(fp.gm1-fp.gm0)/2);
  n0 = hist2proc -> Integral();
  hist2proc->GetXaxis()->SetRangeUser(xlow, xhigh);
  npe_peaks = TMath::Nint((xhigh-fp.gm0)/(fp.gm1-fp.gm0));

  mu_hist= -TMath::Log(1.*n0/ntotal);

  // Construct GP function in range [Mu0-(Mu1-Mu0)/2, xhigh]
  TF1 *func = new TF1("generpoiss", generpoiss, fp.gm0-(fp.gm1-fp.gm0)/2, xhigh, 8);
  // Set initial parameters for GP and limits for them
  func -> SetParameters( 0.1, fp.gm1-fp.gm0, sqrt(abs(fp.gs1*fp.gs1 - fp.gs0*fp.gs0)), mu_hist, fp.gm0, fp.gs0, binwidth*ntotal, npe_peaks); //initial parameters
  func -> SetParLimits(0, 0., 1.);
  func -> FixParameter(7, npe_peaks);
  func -> SetParNames("lambda", "gain_SPE", "sigma_gain", "mu_avg", "mu0", "sigma0", "NxW", "npe_peaks");
  func -> SetNpx(1000);
  hist2proc -> Fit(func, "R+");

  // put parameters of GP to fp structure
  fp.gain = func->GetParameter(1);
  fp.u0 = func->GetParameter(4);
  fp.s0 = func->GetParameter(5);
  fp.sG = func->GetParameter(2);
  fp.lambda = func->GetParameter(0);
  fp.navg = func->GetParameter(3);
  fp.npeaks = func->GetParameter(7);

  //for (int i_par = 0; i_par < 8; i_par++) {
  //  cout << func->GetParName(i_par) << " = " << func->GetParameter(i_par) << endl;
  //}

  // return GP function
  return (func);
}



Double_t langaufun(Double_t *x, Double_t *par) {
  /*
    Taken from here - https://root.cern.ch/doc/v614/langaus_8C_source.html
  */

   //Fit parameters:
   //par[0]=Width (scale) parameter of Landau density
   //par[1]=Most Probable (MP, location) parameter of Landau density
   //par[2]=Total area (integral -inf to inf, normalization constant)
   //par[3]=Width (sigma) of convoluted Gaussian function
   //
   //In the Landau distribution (represented by the CERNLIB approximation),
   //the maximum is located at x=-0.22278298 with the location parameter=0.
   //This shift is corrected within this function, so that the actual
   //maximum is identical to the MP parameter.

      // Numeric constants
      Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
      Double_t mpshift  = -0.22278298;       // Landau maximum location

      // Control constants
      Double_t np = 100.0;      // number of convolution steps
      Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

      // Variables
      Double_t xx;
      Double_t mpc;
      Double_t fland;
      Double_t sum = 0.0;
      Double_t xlow,xupp;
      Double_t step;
      Double_t i;


      // MP shift correction
      mpc = par[1] - mpshift * par[0];

      // Range of convolution integral
      xlow = x[0] - sc * par[3];
      xupp = x[0] + sc * par[3];

      step = (xupp-xlow) / np;

      // Convolution integral of Landau and Gaussian by sum
      for(i=1.0; i<=np/2; i++) {
         xx = xlow + (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);

         xx = xupp - (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);
      }

      return (par[2] * step * sum * invsq2pi / par[3]);
}



TF1 *langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF)
{
   // Once again, here are the Landau * Gaussian parameters:
   //   par[0]=Width (scale) parameter of Landau density
   //   par[1]=Most Probable (MP, location) parameter of Landau density
   //   par[2]=Total area (integral -inf to inf, normalization constant)
   //   par[3]=Width (sigma) of convoluted Gaussian function
   //
   // Variables for langaufit call:
   //   his             histogram to fit
   //   fitrange[2]     lo and hi boundaries of fit range
   //   startvalues[4]  reasonable start values for the fit
   //   parlimitslo[4]  lower parameter limits
   //   parlimitshi[4]  upper parameter limits
   //   fitparams[4]    returns the final fit parameters
   //   fiterrors[4]    returns the final fit errors
   //   ChiSqr          returns the chi square
   //   NDF             returns ndf

   Int_t i;
   Char_t FunName[100];

   sprintf(FunName,"Fitfcn_%s",his->GetName());

   TF1 *ffitold = (TF1*)gROOT->GetListOfFunctions()->FindObject(FunName);
   if (ffitold) delete ffitold;

   TF1 *ffit = new TF1(FunName,langaufun,fitrange[0],fitrange[1],4);
   ffit->SetParameters(startvalues);
   ffit->SetParNames("Width","MP","Area","GSigma");

   for (i=0; i<4; i++) {
      ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
   }

   his->Fit(FunName,"R+");   // fit within specified range, use ParLimits, do not plot

   ffit->GetParameters(fitparams);    // obtain fit parameters
   for (i=0; i<4; i++) {
      fiterrors[i] = ffit->GetParError(i);     // obtain fit parameter errors
   }
   ChiSqr[0] = ffit->GetChisquare();  // obtain chi^2
   NDF[0] = ffit->GetNDF();           // obtain ndf

   return (ffit);              // return fit function

}


void analyze(char * filename){
  //TFile *fin = new TFile("day4_CALICE_92_11_4.root", "READ");
    TFile *fin = TFile::Open(filename);
    TH1F *hist = (TH1F*)fin->Get("hQDCA_13");
    cout << "filename: " << filename << endl;

//    string outfile(filename);
    ofstream outfile;
    char file_name_array[500];
    sprintf(file_name_array, "%s", filename);
    outfile.open(string(strtok(file_name_array, ".")) + "_fitresults.tsv");
    cout << "data will be saved to: " << endl
    << string(strtok(file_name_array, ".")) + "_fitresults.tsv" << endl;
    //outfile.open("/home/tacuser/KAM/temp/fitresults.tsv");
    //
    //char * ShortFileName = strtok(filename, "/");
    //cout << "ShortFileName: " << ShortFileName << endl;
    std::string name = string(filename);
    cout << "string: " << name << endl;
    unsigned first = name.find_last_of("/");
    unsigned last = name.find_last_of(".");
    string ShortFileName = name.substr(first,last-first);
    ShortFileName.erase(0,1);


//    string outfile(strtok(filename, ".")) + "_fitresults.tsv";
    std::cout << "Construct hists" << std::endl;
    TH1F *hsps = new TH1F("hSPS","SPS",4096,0,4096);
    TH1F *hmip = new TH1F("hMIP","MIP",4096,0,4096);

    int ADCsig;
    int ADCtrig;
    float SiPM_Voltage;
    float temperature;
    TTree *myTree = (TTree*)fin->Get("data");

    myTree->SetBranchAddress("qdcch13", &ADCsig);
    myTree->SetBranchAddress("qdcch15", &ADCtrig);
    myTree->SetBranchAddress("SiPM_Voltage", &SiPM_Voltage);
    myTree->SetBranchAddress("temperature", &temperature);


    for(int entry = 0; entry < myTree->GetEntries(); entry++)
    {
        myTree->GetEntry(entry);
        // my lines
        //if (entry % 10000 == 0) {
        //  cout << entry << endl 
        //  << "ADCsig  = " << ADCsig << endl
        //  << "ADCtrig = " << ADCtrig << endl;
        //}
        if(ADCtrig == 4096)
        {
          if(ADCsig > 300) hmip->Fill(ADCsig);
        }
        else hsps->Fill(ADCsig);
    }

    Double_t fr[2];
    Double_t sv[4], pllo[4], plhi[4], fitpars[4], fitparerrors[4];
    fr[0]=750; //0.3*hist->GetMean();
    fr[1]=3500; //3.0*hist->GetMean();

    pllo[0]=10.; pllo[1]=150.0; pllo[2]=1000.0; pllo[3]=10.;
    plhi[0]=5000.0; plhi[1]=5000.0; plhi[2]=100000000.0; plhi[3]=2000.0;
    sv[0]=176.; sv[1]=1500.0; sv[2]=50000.0; sv[3]=1700.0;

    Double_t chisqr;
    Int_t    ndf;
    TCanvas *c1 = new TCanvas("c1","SPS",200,10,1800,1000);
    TCanvas *c2 = new TCanvas("c2","MIP",200,10,1800,1000);
//    hsps->Draw();
//
//    TH1F *histcl = (TH1F*)(hsps->Clone());
//
    c1->cd();
    //TF1 *genp = GP(hsps);
    TF1 *genp = new TF1(); 
    genp = GP(hsps);
    hsps->GetFunction("generpoiss")->SetLineColor(kGreen+2);
    hsps->GetFunction("generpoiss")->SetLineWidth(2);
    hsps->Draw("E");
    hsps->GetXaxis()->SetRangeUser(0, 1000);

//    histcl->Draw("sames");
//    histcl->SetLineWidth(0);
    // Write all these in one line and "\t" instead of endl
    outfile << "FileName" "\t" "SPS Plot" "\t" "MIP Plot" "\t"
    << "SPS_mu0" "\t" "SPS_sig0" "\t"  "SPS_Gain" "\t" "SPS_sGain" "\t" "SPS_Npe" "\t"
    << "SPS_lambda" << "\t" //<< endl;
    << "SPS_mean" "\t" "SPS_stdev" "\t" "SPS_skew" "\t"
    << "SPC_chi2" "\t" "SPS_NDF" "\t"
    << "Gauss0_mean" "\t" "Gauss0_sigma" "\t" "Gauss0_N" "\t" "Gauss0_Integral" "\t" "Gauss0_Chi2" "\t" "Gauss0_NDF" "\t"
    << "Gauss1_mean" "\t" "Gauss1_sigma" "\t" "Gauss1_N" "\t" "Gauss1_Integral" "\t" "Gauss1_Chi2" "\t" "Gauss1_NDF" "\t"
    << "SPS_hist_entries" "\t"
    // comment these lines for day 7, no MIP
    << "MIP_MPV" "\t" "MIP_mean" "\t" "MIP_stdev" "\t" "MIP_skew" "\t"
    << "MIP_chi2" "\t" "MIP_NDF" "\t"
    // Temperature, Voltage and other data
    << "SiPM_Voltage" "\t" "Temperature" << endl;


    // FileName, SPS plot, MIP Plot
    outfile << ShortFileName + ".root" << "\t" << ShortFileName + "_SPS.png" << "\t" << ShortFileName + "_SPS.png" << "\t";
    // Write GP functions parameters to file
    outfile << genp->GetParameter(4) << "\t" << genp->GetParameter(5) << "\t" << genp->GetParameter(1) << "\t" << genp->GetParameter(2) << "\t" << genp->GetParameter(3) << "\t" << genp->GetParameter(0) << "\t";
    outfile << hsps->GetMean(1) << "\t" << hsps->GetStdDev(1) << "\t" << hsps->GetSkewness(1) << "\t"; //endl;
    outfile << genp->GetChisquare() << "\t" << genp->GetNDF() << "\t";// << endl; // endl in case no MIP
    // Parameters of 1st and 2nd peaks gauss fits
    outfile << fp.gm0 << "\t" << fp.gs0 << "\t" << fp.gN0 << "\t" << hsps->Integral(fp.gm0-(fp.gm1-fp.gm0)/2,fp.gm0+(fp.gm1-fp.gm0)/2) << "\t" << fp.g0Chi2 << "\t" << fp.g0NDF << "\t";
    outfile << fp.gm1 << "\t" << fp.gs1 << "\t" << fp.gN1 << "\t" << hsps->Integral(fp.gm1-(fp.gm1-fp.gm0)/2,fp.gm1+(fp.gm1-fp.gm0)/2) << "\t" << fp.g1Chi2 << "\t" << fp.g1NDF << "\t";
    outfile << hsps->GetEntries() << "\t";

    TPaveStats *statb = (TPaveStats*)(hsps->FindObject("stats"));
//    statb->SetLineColor(2);
//    TPaveStats *statb1 = (TPaveStats*)(histcl->FindObject("stats"));
//    statb1->SetLineColor(4);
//
    c1->Update();
    c1->Modified();

    params_stats(hsps, c1);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);


    c1->GetPad(0)->SetLogy();
    c1->Print(TString(strtok(file_name_array, ".")) + "_SPS.png");
    //c1->Print("/home/tacuser/KAM/temp/hist_SPS.png");

    // Comment this all for day 7, because there is no MIP
    c2->cd();

    // Group 4 bins in 1 
    hmip->Rebin(4);
    // Construct two functions
    TF1 *fitsnr1 = langaufit(hmip,fr,sv,pllo,plhi,fitpars,fitparerrors,&chisqr,&ndf);
    TF1 *fitmip = LandauGausFit(*hmip);

    hmip->Draw("E");
    // Sometimes fitsnr1 looks not very good (multiple peaks on one MIP)
    // but we don't put it's parameters to .tsv so this is not a problem
    fitsnr1->SetLineColor(kYellow+2);
    fitsnr1->SetLineWidth(2);
    fitsnr1->Draw("same");
    fitmip->SetLineColor(kRed);
    fitmip->SetLineWidth(2);
    fitmip->Draw("same");
    // My addition
    gStyle->SetOptFit(1);
    c2->Update();
    c2->Print(TString(strtok(file_name_array, ".")) + "_MIP.png");
    //c2->Print("/home/tacuser/KAM/temp/hist_MIP.png");

    cout << "fitsnr1 MPV      = " << fitsnr1->GetMaximumX() << endl
    << "fitmip MPV       = " << fitmip->GetMaximumX() << endl
    << "hmip max value X = " << hmip->GetMaximumBin() * 4 << endl;
    outfile << fitmip->GetMaximumX() << "\t" << hmip->GetMean(1) << "\t" << hmip->GetStdDev(1) << "\t" << hmip->GetSkewness(1) << "\t";

    outfile << fitmip->GetChisquare() << "\t" << fitmip->GetNDF() << "\t";
    outfile << SiPM_Voltage << "\t" << temperature << endl;

    outfile.close();

    delete genp;
};


// void analyze_multiple(std::string dir)
void analyze_multiple() {
    std::cout << "Start analyze" << std::endl;

    // temporary solution
    string dir = "/home/andy/GitStuff/CMS_soft/DESY_TAC/TestFiles/Files_29-10-2021/";
    std::string path = dir + "ListOfDataFiles1.txt";
    std::cout << path << std::endl;
    // Convert to char*
    const char *cpath = path.c_str();
    ifstream infile(cpath);
    std::cout << cpath << std::endl;
    vector<string> lines;
    std::string line;
    while (getline(infile, line))
    {
        lines.push_back(line);
        std::cout << line << std::endl;
        std::string filepath = dir + line;
        const char* cfilepath = filepath.c_str();
        char cfilepath_nonconst[300];
        sprintf(cfilepath_nonconst, "%s", cfilepath);
        analyze(cfilepath_nonconst);
    }

    infile.close();
};
