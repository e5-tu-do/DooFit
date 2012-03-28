// from STL
#include <iostream>
#include <string>
#include <vector>
#include <map>

// from RooFit
#include "RooWorkspace.h"

// from Project
#include "doofit/Config/CommonConfig.h"
#include "doofit/Builder/numerobis/numerobis.h"
#include "doofit/Builder/numerobis/numerobis_config.h"

#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/factory.h"

#include "doofit/Builder/numerobis/blueprint/pdfs/gausspdf.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"

using namespace std;

using namespace doofit;
using namespace doofit::builder::numerobis;

int run( int argc, char *argv[] ){
  namespace numi = doofit::builder::numerobis::blueprint;
  using namespace doofit::utils;
  
  //string filename = argv[1];

  Config::CommonConfig cfg_com("common");
  cfg_com.InitializeOptions(argc, argv);
  
  NumerobisConfig cfg_bld("numerobis");
  cfg_bld.InitializeOptions(cfg_com);
  
  // More or less mandatory step after initialization of all Config objects.
  // Check if the help flag is set and (if so) print a help message.
  cfg_com.CheckHelpFlagAndPrintHelp();
  

  Numerobis builder(cfg_com,cfg_bld);
  
  RooWorkspace ws("ws");
  //builder.AddToWorkspace(ws);
  
  //builder.PrintLogo();
  
  numi::elements::Registrar registrar_elements;
  numi::elements::Factory factory(registrar_elements);
  
  numi::pdfs::Registrar registrar_pdfs(registrar_elements);
  
  numi::pdfs::GaussPdf* pdf = new numi::pdfs::GaussPdf("pdfGauss", "pdfGauss", "a", "b", "c");
  registrar_pdfs.Declare(pdf);
  sdebug << registrar_pdfs.CheckReady("pdfGauss") << endmsg;
  sdebug << registrar_pdfs.Register(&ws, "pdfGauss") << endmsg;
  
  std::vector<std::string> elements;
  elements.push_back("a");
  elements.push_back("b");
  
  factory.AssembleFormula("f","f","@0*@1",elements);

  std::vector<std::string> elements2;
  elements2.push_back("f");
  elements2.push_back("c");

  factory.AssembleFormula("f2","f2","@0*@1",elements2);

  registrar_elements.Print();
  registrar_pdfs.Print();
  
  factory.AssembleDimReal("a","a","a",0.3,3,"ps");

  factory.AssembleParamBasic("b", "b", "b", 0.4, 0.3, 3, "ps");
  factory.AssembleParamBasic("c", "c", "c", 0.5, 0.3, 3, "ps");
  
  registrar_elements.Print();
  registrar_pdfs.Print();
  
  sdebug << registrar_pdfs.CheckReady("pdfGauss") << endmsg;
  
  registrar_pdfs.Print();

  sdebug << registrar_pdfs.Register(&ws, "pdfGauss") << endmsg;
  
  registrar_pdfs.Print();
  
  registrar_elements.Register(&ws, "f2");
  registrar_elements.Register(&ws, "f");
  
  registrar_elements.Print();
  registrar_pdfs.Print();

  ws.Print("t");
  
  return 0;
}


int main( int argc, char *argv[] ) {
  for (int i=0; i<1; ++i) {
    run(argc,argv);
  }
}