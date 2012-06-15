// STL
#include <vector>
#include <sstream>
#include <utility>

// Boost
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

// ROOT
#include "TCanvas.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObjectTable.h"
#include "TStopwatch.h"

// RooFit
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h" 
#include "RooProdPdf.h"
#include "Roo1DTable.h"
#include "RooFitResult.h"

// from Project
#include "doofit/Config/CommonConfig.h"

#include "doofit/Builder/BuilderStd/BuilderStd.h"
#include "doofit/Builder/BuilderStd/BuilderStdConfig.h"

#include "doofit/Pdf2Ws/Pdf2WsStd/Pdf2WsStdMass.h"
#include "doofit/Pdf2Ws/Pdf2WsStd/Pdf2WsStdCommonFuncs.h"

#include "doofit/Toy/ToyFactoryStd/ToyFactoryStd.h"
#include "doofit/Toy/ToyFactoryStd/ToyFactoryStdConfig.h"

#include "doofit/Toy/ToyStudyStd/ToyStudyStd.h"
#include "doofit/Toy/ToyStudyStd/ToyStudyStdConfig.h"

#include "doofit/utils//MsgStream.h"

using namespace ROOT;
using namespace RooFit;
using namespace doofit;
using namespace doofit::utils;

class Worker
{
public: 
  
  Worker(unsigned N, float guess, unsigned iter) 
  : m_Number(N),
  m_Guess(guess),
  m_Iterations(iter)
  {
  }
  
  void operator()()
  {
    std::cout << "Worker: calculating sqrt(" << m_Number
    << "), itertations = "
    << m_Iterations << std::endl;
    
    // Use Newton's Method
    float   x;
    float   x_last = m_Guess;
    
    for (unsigned i=0; i < m_Iterations; i++)
    {
      x = x_last - (x_last*x_last-m_Number)/
      (2*x_last);
      x_last = x;
      
      std::cout << "Iter " << i << " = "
      << x << std::endl;
    }
    
    std::cout << "Worker: Answer = " << x << std::endl;
  }
  
private:
  
  unsigned    m_Number;
  float       m_Guess;
  unsigned    m_Iterations;
};


RooWorkspace* BuildPDF() {
  RooWorkspace* ws = new RooWorkspace("ws");
  ws->Print();
  
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean1", "mean1", 5200, 5150, 5250, "MeV/c^{2}");
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean2", "mean2", 5300, 5250, 5350, "MeV/c^{2}");
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean3", "mean3", 5400, 5350, 5450, "MeV/c^{2}");
  
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean_time1", "mean_time1", 5200, 5150, 5250, "MeV/c^{2}");
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean_time2", "mean_time2", 5300, 5250, 5350, "MeV/c^{2}");
  Pdf2WsStd::CommonFuncs::getVar(ws, "mean_time3", "mean_time3", 5400, 5350, 5450, "MeV/c^{2}");
  
  RooGaussian* pdf1 = Pdf2WsStd::Mass::Gaussian(ws, "test1", "Gaussian test pdf #1","mass","mean1", "abweichung");
  RooGaussian* pdf2 = Pdf2WsStd::Mass::Gaussian(ws, "test2", "Gaussian test pdf #2","mass","mean2", "abweichung_bkg");
  RooGaussian* pdf3 = Pdf2WsStd::Mass::Gaussian(ws, "test3", "Gaussian test pdf #3","mass","mean3", "abweichung_bkg2");
  
  RooGaussian* time1 = Pdf2WsStd::Mass::Gaussian(ws, "time1", "Gaussian test pdf #1 (time)","time","mean_time1", "sigma_time1");
  RooGaussian* time2 = Pdf2WsStd::Mass::Gaussian(ws, "time2", "Gaussian test pdf #2 (time)","time","mean_time2", "sigma_time2");
  RooGaussian* time3 = Pdf2WsStd::Mass::Gaussian(ws, "time3", "Gaussian test pdf #3 (time)","time","mean_time3", "sigma_time3");
  
  RooProdPdf pdf_prod1("pdf_prod1", "pdf_prod1", RooArgList(*pdf1, *time1));
  RooProdPdf pdf_prod2("pdf_prod2", "pdf_prod2", RooArgList(*pdf2, *time2));
  RooProdPdf pdf_prod3("pdf_prod3", "pdf_prod3", RooArgList(*pdf3, *time3));
  
  RooRealVar yield1("yield1", "pdf yield", 10000, 0, 1000000);
  RooExtendPdf pdf_extend1("pdf_extend1", "extended pdf #1", pdf_prod1, yield1);
  
  RooRealVar yield2("yield2", "pdf yield", 50000, 0, 1000000);
  RooExtendPdf pdf_extend2("pdf_extend2", "extended pdf #2", pdf_prod2, yield2);
  
  RooRealVar yield3("yield3", "pdf yield", 5000, 0, 1000000);
  RooExtendPdf pdf_extend3("pdf_extend3", "extended pdf #3", pdf_prod3, yield3);
  
  RooRealVar coeff1("coeff1", "coeff1", 0.1, 0.0, 1.0);
  RooRealVar coeff2("coeff2", "coeff2", 0.1, 0.0, 1.0);
  RooAddPdf pdf_add("pdf_add", "added pdf", RooArgSet(pdf_extend1, pdf_extend2, pdf_extend3));
  //RooAddPdf pdf_add("pdf_add", "added pdf", RooArgList(*pdf1, *pdf2, *pdf3), RooArgList(coeff1, coeff2));
  //RooAddPdf pdf_add("pdf_add", "added pdf", *pdf1, *pdf2, coeff1);
  
  ws->import(pdf_add);
  
  RooArgSet argset_obs("argset_obs");
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "")));
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "time", "", 0, 0, 0, "")));
  argset_obs.add(*(Pdf2WsStd::CommonFuncs::getVar(ws, "tag2", "tag of B meson", 0, -10, 10, "")));
  RooRealVar* tag2 = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "tag2", "", 0, 0, 0, "");
  
  RooCategory* tag = new RooCategory("tag", "tag");
  tag->defineType("b0", 1);
  tag->defineType("b0_bar", -1);
  ws->import(*tag);
  delete tag;
  tag = ws->cat("tag");
  
  argset_obs.add(*tag);
  ws->defineSet("argset_obs",argset_obs);  
  
  ws->Print("t");
  
  TFile wsfile("ws.root", "recreate");
  ws->Write("ws");
  wsfile.Close();
  
  return ws;
}

void PlotToyFit(RooWorkspace* ws, const RooAbsPdf* pdf) {
  TFile f("data.root","read");
  RooDataSet* data = (RooDataSet*)f.Get("dataset");
  
  RooRealVar* mass = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "mass", "", 0, 0, 0, "");
  RooRealVar* time = (RooRealVar*)Pdf2WsStd::CommonFuncs::getVar(ws, "time", "", 0, 0, 0, "");
  
  RooPlot* mass_frame = mass->frame();
  RooPlot* time_frame = time->frame();
  RooPlot* tag2_frame = ws->var("tag2")->frame();
  
  data->plotOn(mass_frame);
  pdf->plotOn(mass_frame);
  
  data->plotOn(time_frame);
  pdf->plotOn(time_frame);
  
  data->plotOn(tag2_frame);
  
  TCanvas c1("c1", "c1", 800, 600);
  mass_frame->Draw();
  c1.SaveAs("mass.pdf");
  
  time_frame->Draw();
  c1.SaveAs("time.pdf");
  
  tag2_frame->Draw();
  c1.SaveAs("c2.pdf");
  
  Roo1DTable* table = data->table(RooArgSet(*ws->cat("tag")));
  table->Print();
  delete table;
  
  delete mass_frame;
  delete tag2_frame;
  
  delete data;
}

void TestToys(int argc, char *argv[]) {
  using namespace Toy;
  using namespace RooFit;
  
  Config::CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  BuilderStdConfig cfg_bld("builder");
  cfg_bld.InitializeOptions(cfg_com);
  
  ToyFactoryStdConfig cfg_tfac("toyfac");
  cfg_tfac.InitializeOptions(cfg_bld);
  
  ToyStudyStdConfig cfg_tstudy("toystudy");
  cfg_tstudy.InitializeOptions(cfg_tfac);
  
  cfg_com.CheckHelpFlagAndPrintHelp();
    
  //RooWorkspace* ws = BuildPDF();
  TFile ws_file("ws.root", "read");
  RooWorkspace* ws = (RooWorkspace*)ws_file.Get("ws");
  
  RooAbsPdf* pdf = ws->pdf("pdf_add");
  ws->Print();
  
  cfg_tfac.set_workspace(ws);
  //cfg_tfac.set_generation_pdf_workspace("pdfFull");
  //cfg_tfac.set_argset_generation_observables_workspace("argset_obs");
  
  ToyFactoryStd tfac(cfg_com, cfg_tfac);

  cfg_com.PrintAll();
  
  RooDataSet* data = NULL;
  ToyStudyStd tstudy(cfg_com, cfg_tstudy);

  for (int i=0; i<2; ++i) {
    data = tfac.Generate();
    //    delete data;
    //    gObjectTable->Print(); 
    //    utils::sdebug << i << utils::endmsg;
    //  }
    
    pdf->getParameters(data)->readFromFile("generation.par");
    RooFitResult* fit_result = pdf->fitTo(*data, NumCPU(2), Extended(true), Save(true), Strategy(2), Minos(false), Hesse(false), Verbose(false),Timer(true));
    RooFitResult* fit_result2 = pdf->fitTo(*data, NumCPU(2), Extended(true), Save(true), Strategy(2), Minos(false), Hesse(false), Verbose(false),Timer(true));
    
    tstudy.StoreFitResult(fit_result, fit_result2);
    delete data;
    
  }
  tstudy.FinishFitResultSaving();
    
//  tstudy.ReadFitResults("fr1");
//  std::vector<const RooFitResult*> results = tstudy.GetFitResults();
//  tstudy.EmptyResults();
//  for (std::vector<const RooFitResult*>::const_iterator it = results.begin();
//       it != results.end(); ++it) {
//    (*it)->Print();
//  }
//  tstudy.ReadFitResults("fr2");
//  std::vector<const RooFitResult*> results2 = tstudy.GetFitResults();
//  tstudy.EmptyResults();
//  for (std::vector<const RooFitResult*>::const_iterator it = results2.begin();
//       it != results2.end(); ++it) {
//    (*it)->Print();
//  }

  tstudy.ReadFitResults();
  tstudy.EvaluateFitResults();
  tstudy.PlotEvaluatedParameters();
  
  //PlotToyFit(ws, pdf);
  delete ws;
  
  //while (true) {}
}

    boost::recursive_mutex bla;
    boost::condition_variable_any v1;
    boost::condition_variable_any v2;
    boost::recursive_mutex::scoped_lock lock(bla);
    int got_processed = 0;
    const int n = 10;

    void ProcessNIterations() {
      got_processed = 0;
      // have some mutex or whatever unlocked here so that the worker thread can 
      // start or resume. 
      // my idea: have some sort of mutex lock that unlocks here and a condition
      // variable v1 that is notified while the thread is waiting for that.
      lock.unlock();
      v1.notify_one();
      
      // while the thread is working to do the iterations this function should wait
      // because there is no use to proceed until the n iterations are done
      // my idea: have another condition v2 variable that we wait for here and lock
      // afterwards so the thread is blocked/paused
      while (got_processed < n) {
        v2.wait(lock);
      }
    }

    void WorkerThread() {
      int counter = 0;
      // wait for something to start
      // my idea: acquire a mutex lock here that was locked elsewhere before and 
      // wait for ProcessNIterations() to unlock it so this can start
      boost::recursive_mutex::scoped_lock internal_lock(bla);
      
      for (;;) {
        for (;;) {
          // here do the iterations
          counter++;
          std::cout << "iteration #" << counter << std::endl;
          got_processed++;
          
          if (counter >= n) {
            // we've done n iterations; pause here
            // my idea: unlock the mutex, notify v2
            internal_lock.unlock();
            v2.notify_one();
            
            while (got_processed > 0) {
              // when ProcessNIterations() is called again, resume here
              // my idea: wait for v1 reacquiring the mutex again
              v1.wait(internal_lock);
            }
            counter = 0;
          }
        }
      }
    }

int main(int argc, char *argv[]) {
  TestToys(argc, argv);
}


    