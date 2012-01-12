
#include "Utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include <boost/thread/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

//from Roofit
#include "RooDataSet.h"

using namespace std;

/*
 * Set global layout
 *
 *  option = "", "2d"
 */
void Utils::setStyle(TString option)
{
	// text font
	int font = 132;
	gStyle->SetTitleFont(font, "xyz");	// set the all 3 axes title font
	gStyle->SetTitleFont(font, " ");	// set the pad title font
	gStyle->SetLabelFont(font, "xyz");
	gStyle->SetTextFont(font);
	gStyle->SetStatFont(font);

	// no default boxes
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	gStyle->SetOptFit(0);
	
	// rainbow colors in 2d plots
	gStyle->SetPalette(1);

	// use plain black on white colors
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameFillColor(0);
	gStyle->SetCanvasBorderMode(0);
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadColor(0);
	gStyle->SetCanvasColor(0);
	gStyle->SetStatColor(0);
	gStyle->SetTitleFillColor(0);

  /// \todo Put some more documentation in here.
  
	// canvas default size
	gStyle->SetCanvasDefH(480);
	gStyle->SetCanvasDefW(640);
	
	// text
	gStyle->SetTextSize(0.07);
	gStyle->SetTextAlign(13);
	
	// set the paper & margin sizes
	gStyle->SetPadTopMargin(0.05);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadBottomMargin(0.14);
	if ( option=="2d" ) gStyle->SetPadBottomMargin(0.15);
	gStyle->SetPadLeftMargin(0.14);
	if ( option=="2d" ) gStyle->SetPadRightMargin(0.18);

	// use bold lines and markers
	gStyle->SetMarkerStyle(8);
	gStyle->SetHistLineWidth(2);
	gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
	
	// get rid of X error bars
	gStyle->SetErrorX(0.001);

	// axes
	gStyle->SetTitleSize(0.065,	"xyz");
	gStyle->SetLabelSize(0.06,	"xyz");
	gStyle->SetTitleOffset(1.07,	"y");
	gStyle->SetLabelOffset(0.015,	"x");
	gStyle->SetNdivisions(507,	"x");
	
	// put tick marks on top and right hand side of plots
	//if ( option!="2d" )
	//{
		gStyle->SetPadTickX(1);
		gStyle->SetPadTickY(1);
	//}

}


/*
 * Set a 2d color palette which emphasizes the region
 * around 0. Positive numbers are printed reddish ("hot"),
 * negative numbers blueish ("cold").
 *
 * Note: It is important to use a symmetric z-axis range!
 * The axis is adjusted automatically when the histogram
 * is given as an argument.
 */
void Utils::setHotColdPalette(TH2* h)
{
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0,0.4,0.5,0.6,1 };
	Double_t red[NRGBs]   = { 0,0,1,1,1 };
	Double_t green[NRGBs] = { 1,0,1,0,1 };
	Double_t blue[NRGBs]  = { 1,1,1,0,0 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);
	gStyle->SetPaintTextFormat(".1f");

	if ( h )
	{
		float max = h->GetBinContent(h->GetMaximumBin());
		float min = h->GetBinContent(h->GetMinimumBin());
		float range = Max(max, min);
		h->SetAxisRange(-range, range, "Z");
	}
}

void Utils::setRedBluePalette(TH2* h)
{
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0,  0.25, 0.5,  0.75, 1 };
	Double_t red[NRGBs]   = { 0,  0,    1,    1,    0.5 };
	Double_t green[NRGBs] = { 0,  0,    1,    0,    0 };
	Double_t blue[NRGBs]  = { 0.5,1,    1,    0,    0 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);
	gStyle->SetPaintTextFormat(".1f");

	if ( h )
	{
		float max = h->GetBinContent(h->GetMaximumBin());
		float min = h->GetBinContent(h->GetMinimumBin());
		float range = Max(max, min);
		h->SetAxisRange(-range, range, "Z");
	}
}


void Utils::drawDate(float x1, float y1, float x2, float y2)
{
	TDatime timer;
	TPaveText *pt = new TPaveText(x1, y1, x2, y2,"BRNDC");
	pt->SetBorderSize(0);
	pt->SetFillStyle(0);
	pt->SetTextAlign(13);
	pt->SetTextSize(0.03);
	pt->SetTextFont(42);
	pt->SetTextColor(kGray);
	pt->AddText(timer.AsString());
	pt->Draw();
}


void Utils::printSystemRecources(TString cmd)
{
	cout << endl;
	system("ps aux|head -n 1");
	system("ps aux|grep " + cmd + " |grep -v grep");
	cout << endl;
}


void Utils::printPlot(TCanvas* c, TString name, TString dir)
{
        cout << "Utils::printPlot() : printing plots ..." << endl;

        if ( dir!="" && !dir.EndsWith("/") ) dir += "/";

	system("mkdir -p " + dir+"eps/");
	system("mkdir -p " + dir+"C/");
	system("mkdir -p " + dir+"png/");
	system("mkdir -p " + dir+"pdf/");

        c->Print(dir+"eps/" + name + ".eps");
	c->Print(dir+"C/"   + name + ".C");
        c->Print(dir+"png/" + name + ".png");
        c->Print(dir+"pdf/" + name + ".pdf");
}

/* 
 * doesn't work with 3GB files.
 *
 */
bool Utils::fileExists( TString strFilename )
{
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat( strFilename, &stFileInfo );
	
	if(intStat == 0) {
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	} else {
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
	}

	return(blnReturn);
}


int Utils::fileNLines( TString strFilename )
{
	if ( ! fileExists(strFilename) )
	{
		cout << "FileNLines() : ERROR : file not found : " + strFilename << endl;
		return 0;
	}

	ifstream file(strFilename);
	int n = 0;
	string tmpstr;
	while(!file.eof())
	{
		getline(file, tmpstr);
		n++;
	}
	file.close();
	return n-1;
}

void Utils::Sleep(double sleep_time) {
  boost::this_thread::sleep(boost::posix_time::microseconds(sleep_time*1e6));
}

void Utils::SetTerminalColor(TerminalColor color) {
  if (color != kTextNone) {
    printf("%c[%d;%dm",27,1,30+color);
  }
}

void Utils::ResetTerminal() {
  printf("%c[%dm",27,0);
}

/*
 * round doubles
 */
double Utils::Round(double value, unsigned int digits)
{
	value *= pow(10., (int)digits);
	if (value >= 0)
		value = floor(value + 0.5);
	else
		value = ceil(value - 0.5);
	value /= pow(10., (int)digits);
	return value;
}


/*
 * Calculate the covariance between two variables.
 */
Double_t Utils::Covariance(Int_t n, Double_t xdata[], Double_t ydata[])
{
	if ( n==0 )
	{
		cout << "Utils::Covariance() : ERROR : No datapoints! Returning 0." << endl;
		return 0.0;
	}
	
	Double_t sumx(0), sumy(0), sumxy(0);

	for(Int_t i= 0; i < n; i++)
	{
		//cout << xdata[i] << " " << ydata[i] << endl;
		sumx  += xdata[i];
		sumy  += ydata[i];
		sumxy += xdata[i]*ydata[i];
	}
	
	Double_t v = sumxy/(float)n - sumx/(float)n*sumy/(float)n;

	if ( v<0.0 )
	{
		cout << "Utils::Covariance() : ERROR : Variance < 0. Returning 0." << endl;
		return 0.0;
	}	

	return v;
}


/*
 * Calculate the variance of a dataset.
 */
Double_t Utils::Variance(Int_t n, Double_t data[])
{
	return Covariance(n, data, data);
}


/*
 * Given up to four histograms, draw the largest one first, then
 * the smaller ones upon it in order. This is to avoid the clash with the "same"
 * option of TH1::Draw().
 */
void Utils::drawOrdered(TH1* h1, TH1* h2, TH1* h3, TH1* h4)
{
	TH1* h[4];
	h[0] = h1;
	h[1] = h2;
	h[2] = h3;
	h[3] = h4;
	
	int nmax = 2;
	if ( h3!=0 ) nmax++;
	if ( h4!=0 ) nmax++;
	
	// copy over axes because the plot will have the axes from
	// the histogram we draw first
	for ( int i=1; i<nmax; i++ )
	{
		h[i]->GetXaxis()->SetTitle(h[0]->GetXaxis()->GetTitle());
		h[i]->GetYaxis()->SetTitle(h[0]->GetYaxis()->GetTitle());
	}	
	
	int ord[4] = {0};

	for ( int i=0; i<nmax; i++ )
	{
		int greaterThan = 0;
		
		for ( int j=0; j<nmax; j++ )
		{
			if ( h[i]->GetMaximum() > h[j]->GetMaximum() ) greaterThan++;
		}
		
		ord[nmax-1-greaterThan] = i;
	}
	
	// don't use up all plot space
	h[ord[0]]->GetYaxis()->SetRangeUser(0., h[ord[0]]->GetMaximum()*1.3);
	
	for ( int i=0; i<nmax; i++ )
	{
		cout << "Utils::DrawOrdered() : Drawing " << h[ord[i]]->GetName() << " ..." << endl;
		if ( i==0 ) h[ord[i]]->Draw();
		else        h[ord[i]]->Draw("same");
	}
}


/*
 * Symmetrize a matrix by copying the upper left triangle
 * to the lower right one, which needs to be entirely filled
 * with zeroes.
 */
void Utils::Symmetrize(TMatrixD &m)
{
	if ( m.GetNrows()!=m.GetNcols() )
	{
		cout << "Utils::Symmetrize() : ERROR : Matrix not quadratic!" << endl;
		return;
	}
	
	for ( int i=0;   i<m.GetNrows(); i++ )
	for ( int j=i+1; j<m.GetNcols(); j++ )
	{
		if ( m[j][i]!=0.0 )
		{
			cout << "Utils::Symmetrize() : ERROR : Matrix element is non-zero!" << endl;
			return;
		}
		m[j][i] = m[i][j];
	}
}


/*
 * Add axis labels to the IncPhi phase space histograms. Sure this is
 * somewhere else, too, but I have no idea where. TMK.
 */
void Utils::addEtaPtLabels(TH2D* h)
{
	if ( !h ) return;
	h->GetXaxis()->SetTitle("p_{T} [MeV/c^{2}]");
	h->GetYaxis()->SetTitle("y");
}

void Utils::PlotResiduals(TString pName, RooPlot * pFrame, RooRealVar * pVar, RooAbsPdf * pPDF, 
                          TString pDir, bool normalize, bool plot_log,
                          TLatex label
                          ) {
	setStyle();
  gStyle->SetTitle(0);
  
  // some global definitions
  double pad_border       = 0.02;
  double pad_relysplit    = 0.3;
  double left_margin      = 0.16;
  double top_label_size   = 0.06;
  double top_title_offset = 1.2;
  double title2label_size_ratio = 1.1;
  
  // derived definitions
  double pad_ysplit     = (1.0-2.*pad_border)*pad_relysplit;
  double bottom_label_size = top_label_size*(1.-pad_relysplit)/pad_relysplit;
  double bottom_title_offset = top_title_offset/(1.-pad_relysplit)*pad_relysplit;
  
  double plot_min = pFrame->GetXaxis()->GetXmin();
  double plot_max = pFrame->GetXaxis()->GetXmax();
  
  TCanvas c1("c1","c1",900,900);
  c1.Divide(1,2);
  
  TPad* pad = (TPad*)c1.cd(1);
  label.Draw();
  if(plot_log){
    pad->SetLogy(1);
  }
  pad->SetPad(pad_border,pad_ysplit,1.-pad_border,1.-pad_border);
  pad->SetLeftMargin(left_margin);
  pad->SetBottomMargin(0.);

	RooHist* resid = pFrame->residHist(0,0,normalize);
  pFrame->SetLabelSize(0.0,"x");
  pFrame->SetLabelSize(top_label_size,"y");
  pFrame->SetTitleSize(top_label_size*title2label_size_ratio,"y");
  pFrame->SetXTitle("");
  pFrame->GetYaxis()->SetTitleOffset(top_title_offset);
  //pFrame->SetMaximum(1.3*pFrame->GetMaximum());
  
	pFrame->Draw();
          
  // lower frame with residuals plot
  pad = (TPad*)c1.cd(2);
  pad->SetPad(pad_border,pad_border,1.-pad_border,pad_ysplit);
  pad->SetLeftMargin(left_margin);
  pad->SetTopMargin(0.);
  pad->SetBottomMargin(0.4);
  
  RooPlot * residFrame = pVar->frame(RooFit::Title("Residuals"));
  residFrame->GetXaxis()->SetLimits(plot_min,plot_max);
  
  residFrame->addPlotable(resid, "P");
  residFrame->SetAxisRange(-5.8,5.8,"Y");
  
  residFrame->SetTitle("");
  if (normalize){
    residFrame->SetYTitle("Pull");
  } else {
    residFrame->SetYTitle("Residual");
  }
  residFrame->SetLabelSize(bottom_label_size, "xy");
  residFrame->SetTitleSize(bottom_label_size*title2label_size_ratio, "xy");
  residFrame->GetYaxis()->SetTitleOffset(bottom_title_offset);  
  residFrame->GetYaxis()->SetNdivisions(5,5,0);
  residFrame->Draw();
  
  pad = (TPad*)c1.cd(0);
  label.SetTextSize(0.05);
  label.Draw();
  
  printPlot(&c1, pName, pDir);
}

std::pair<double,double> Utils::MedianLimitsForTuple(const RooDataSet& dataset, std::string var_name) {
  int num_entries = dataset.numEntries();
  std::vector<double> entries(num_entries);
  
  // convert entries into vector (for sorting)
  const RooArgSet* args = NULL;
  for (int i = 0; i < num_entries; ++i) {
    args = dataset.get(i);
    entries[i] = dynamic_cast<RooRealVar*>(args->find(var_name.c_str()))->getVal();
  }
  
  std::sort(entries.begin(), entries.end());
  int idx_median = num_entries/2;       
  
  std::pair<double, double> minmax;
  
  minmax.first  = -4*entries[idx_median]+5*entries[(int)(idx_median*0.32)];
  minmax.second = -4*entries[idx_median]+5*entries[(int)(num_entries-idx_median*0.32)];
  
  if (minmax.first == minmax.second) {
    minmax.first = entries[idx_median]*0.98;
    minmax.first = entries[idx_median]*1.02;
  }
  
  return minmax;
}

