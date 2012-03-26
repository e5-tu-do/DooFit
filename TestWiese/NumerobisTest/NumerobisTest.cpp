// from STL
#include <iostream>
#include <string>
#include <vector>

// from RooFit
#include "RooWorkspace.h"

// from Project
#include "doofit/Config/CommonConfig.h"
#include "doofit/Builder/numerobis/numerobis.h"
#include "doofit/Builder/numerobis/numerobis_config.h"

#include "doofit/Builder/numerobis/blueprint/elements/registrar.h"
#include "doofit/Builder/numerobis/blueprint/elements/dimreal.h"
#include "doofit/Builder/numerobis/blueprint/elements/parambasic.h"
#include "doofit/Builder/numerobis/blueprint/elements/formula.h"

using namespace std;

using namespace doofit;
using namespace doofit::builder::numerobis;


int main( int argc, char *argv[] ){
  
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
  
  builder.PrintLogo();
  
  blueprint::elements::Registrar registrar;
  
  std::vector<std::string> elements;
  elements.push_back("a");
  elements.push_back("b");
  
  blueprint::elements::Formula f("f","f","@0*@1",elements);
  registrar.Declare(f);


  std::vector<std::string> elements2;
  elements2.push_back("f");
  elements2.push_back("c");
  
  blueprint::elements::Formula f2("f2","f2","@0*@1",elements2);
  registrar.Declare(f2);

  registrar.Print();
  
  blueprint::elements::DimReal a("a","a","a",0.3,3,"ps");
  //a.AddToWorkspace(&ws);
  registrar.Declare(a);
  
  blueprint::elements::ParamBasic b("b","b","b",0.4, 0.3,3,"ps");
  registrar.Declare(b);

  blueprint::elements::ParamBasic c("c","c","c",0.5, 0.3,3,"ps");
  registrar.Declare(c);
  
//  registrar.Register(&ws, a);
//  registrar.Register(&ws, b);
  registrar.Register(&ws, f2);
  
  registrar.Print();
  
  ws.Print("t");
  
  
  
  return 0;
}