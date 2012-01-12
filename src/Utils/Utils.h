/*
 *
 */

#ifndef Utils_h
#define Utils_h

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <sys/stat.h>
#include <utility>

#include "TLorentzVector.h"
#include "TLorentzRotation.h"
#include "TVector3.h"
#include "TChain.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TPad.h"
#include "TDatime.h"
#include "TPaveText.h"
#include "TH1.h"
#include "TH2.h"
#include "TColor.h"
#include "TMatrixD.h"

//from Roofit
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooHist.h"
#include "RooCmdArg.h"
#include "RooGlobalFunc.h"

namespace Utils
{
  /// Terminal color enum to be used for example in MsgStream class.
  enum TerminalColor {
    kTextBlack   = 0,
    kTextRed     = 1,
    kTextGreen   = 2,
    kTextYellow  = 3,
    kTextBlue    = 4,
    kTextMagenta = 5,
    kTextCyan    = 6,
    kTextWhite   = 7,
    kTextNone    = -1
  };
  
	//	
	// plots
	//
	void		setStyle(TString option="");
	void		setHotColdPalette(TH2* h=0);
	void		setRedBluePalette(TH2* h=0);

	void		printPlot(TCanvas* c, TString name, TString dir="");
	void		drawDate(float x1=0.10, float y1=0.02, float x2=0.5, float y2=0.1);	
	void		drawOrdered(TH1* h1, TH1* h2, TH1* h3=0, TH1* h4=0);

	void		addEtaPtLabels(TH2D* h);
	void 		PlotResiduals(TString pName, RooPlot * pFrame, RooRealVar * pVar, RooAbsPdf * pPDF, 
	                      TString pDir = "", bool normalize = true, bool plot_log = false, 
	                      TLatex label = TLatex(1.,1.,""));
  //
	// stuff
	//
	bool		fileExists(TString strFilename);
	int		  fileNLines(TString strFilename);
	void 		printSystemRecources(TString cmd);
  /// \brief Sleep, i.e. halt everything.
  /// \param sleep_time Time to sleep in seconds.
  void    Sleep(double sleep_time);
	
  /// \brief Set the terminal color.
  ///
  /// \param color The color to be used from now on.
  void    SetTerminalColor(TerminalColor color);
  /// Reset the terminal color to none again.
  void    ResetTerminal();
  
	//
	// math
	//
	double		Round(double value, unsigned int digits);
	Double_t	Covariance(Int_t n, Double_t xdata[], Double_t ydata[]);
	Double_t	Variance(Int_t n, Double_t data[]);
	inline double	Sq(double value){return pow(value, 2);}
	inline double	Max(double v1, double v2){return v1>v2 ? v1 : v2;}
	inline double	RadToDeg(double rad){return rad*180.0/TMath::Pi();}
	inline double	DegToRad(double deg){return deg/180.0*TMath::Pi();}
	void		Symmetrize(TMatrixD &m);
  
  /**
    *  @brief Median limits for tuple (here: RooDataSet)
    *
    *  This function will evaluate the distribution of the variable with name 
    *  var_name in the dataset. It will return a pair as (min, max) as plotting
    *  range for the distribution neglecting outliers by a quantile based 
    *  approach.
    *
    *  @param dataset RooDataSet to evaluate
    *  @param var_name name of variable in dataset to evaluate
    *  @return pair of (double,double) as (min,max) to use for plotting
    */
  std::pair<double,double> MedianLimitsForTuple(const RooDataSet& dataset, std::string var_name);
};

#endif // #ifdef Utils_h
