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

#include "doofit/Builder/numerobis/blueprint/blueprint.h"

#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/factory.h"

#include "doofit/Builder/numerobis/blueprint/pdfs/gausspdf.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/registrar.h"
#include "doofit/Builder/numerobis/blueprint/pdfs/factory.h"

#include "doofit/Builder/numerobis/blueprint/component.h"

#include "doofit/utils//MsgStream.h"

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
  
  numi::Blueprint blueprint;
  
  blueprint.fac_pdfs().AssembleGaussPdf("pdfGauss", "pdfGauss", "a", "b", "c");
  sdebug << blueprint.reg_pdfs().CheckReady("pdfGauss") << endmsg;
  sdebug << blueprint.reg_pdfs().Register(&ws, "pdfGauss") << endmsg;
  
  std::vector<std::string> elements;
  elements.push_back("a");
  elements.push_back("b");
  
  blueprint.fac_elements().AssembleFormula("f","f","@0*@1",elements);

  std::vector<std::string> elements2;
  elements2.push_back("f");
  elements2.push_back("c");

  blueprint.fac_elements().AssembleFormula("f2","f2","@0*@1",elements2);

  blueprint.reg_elements().Print();
  blueprint.reg_pdfs().Print();

  blueprint.AssembleDimension("pdfSigMass", "Mass", "a", "pdfGauss");
  std::vector<std::string> dimensions;
  dimensions.push_back("pdfSigMass");
  blueprint.AssembleComponent("pdfSig", "Sig", "pdfSigYield", dimensions);
  
  sdebug << "Registering components: " << blueprint.RegisterComponents(&ws) << endmsg;
  
  blueprint.fac_elements().AssembleDimReal("a","a","a",0.3,3,"ps");

  blueprint.fac_elements().AssembleParamBasic("b", "b", "b", 0.4, 0.3, 3, "ps");
  blueprint.fac_elements().AssembleParamBasic("c", "c", "c", 0.5, 0.3, 3, "ps");
  
  blueprint.reg_elements().Print();
  blueprint.reg_pdfs().Print();
  
  sdebug << blueprint.reg_pdfs().CheckReady("pdfGauss") << endmsg;
  
  blueprint.reg_pdfs().Print();

  sdebug << "Registering components: " << blueprint.RegisterComponents(&ws) << endmsg;
  
  //sdebug << blueprint.reg_pdfs().Register(&ws, "pdfGauss") << endmsg;
  
  blueprint.reg_pdfs().Print();
  
  blueprint.reg_elements().Register(&ws, "f2");
  blueprint.reg_elements().Register(&ws, "f");
  
  blueprint.reg_elements().Print();
  blueprint.reg_pdfs().Print();

  blueprint.fac_elements().AssembleParamBasic("pdfSigYield", "pdfSigYield", "pdfSigYield", 1, 0, 3e8);
  
  sdebug << "Registering components: " << blueprint.RegisterComponents(&ws) << endmsg;
  
  ws.Print("t");
  
  return 0;
}


int main( int argc, char *argv[] ) {
  for (int i=0; i<1; ++i) {
    run(argc,argv);
  }
}