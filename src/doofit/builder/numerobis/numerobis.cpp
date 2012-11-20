#include "doofit/builder/numerobis/numerobis.h"

// from project
#include "doofit/config/CommonConfig.h"
#include "doofit/builder/numerobis/numerobis_config.h"
#include "doofit/builder/numerobis/blueprint/blueprint.h"

//=== OLD SPB stuff ===========//
// from STL
#include <string>
#include <map>
#include <vector>

// from boost
#include <boost/bimap.hpp>
#include <boost/foreach.hpp>

// from ROOT

// from RooFit
#include "RooConstVar.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

// from RooFit - Category
#include "RooCategory.h"
#include "RooSuperCategory.h"

// from RooFit - Blinded
#include "RooUnblindUniform.h"

// from RooFit Pdfs - Comb
#include "RooAddPdf.h"
#include "RooEffProd.h"
#include "RooExtendPdf.h"
#include "RooProdPdf.h"
#include "RooSimultaneous.h"

// from RooFit Pdfs - Basic
#include "RooGaussian.h"
#include "RooExponential.h"

// from RooFit Pdfs - Basic w. Resol
#include "RooDecay.h"
#include "RooBCPGenDecay.h"

// from RooFit Resols - Comb
#include "RooAddModel.h"

// from RooFit Resols - Basic
#include "RooGaussModel.h"
#include "RooTruthModel.h"

//=== END OF OLD SPB stuff ====//


namespace doofit {
namespace builder {
namespace numerobis {

using namespace std;
using namespace RooFit;

using doofit::config::CommonConfig;

Numerobis::Numerobis(const CommonConfig& cfg_com, const NumerobisConfig& cfg_num) 
  : cfg_com_(cfg_com),
    cfg_num_(cfg_num),
    workspace_(NULL),
    import_arg_(Silence()),
    cf_fixed_(true)
{
  
}

Numerobis::~Numerobis() {
  
}

void Numerobis::Configure() {
  
}

void Numerobis::AddToWorkspace(RooWorkspace& ws) {
  workspace_ = &ws;
  
  
} 

  
RooRealVar* Numerobis::CreateDimReal(const string& name, const string& desc, double val_min, double val_max, const string& unit, const string& dim_id) {
  RooRealVar temp_var(name.c_str(),desc.c_str(),val_min,val_max,unit.c_str());
  workspace_->import(temp_var,import_arg_);
  return workspace_->var(name.c_str());
}

RooCategory* Numerobis::CreateDimDisc(const string& name, const string& desc ) {
  RooCategory temp_cat(name.c_str(),desc.c_str());
  workspace_->import(temp_cat,import_arg_);
  return workspace_->cat(name.c_str());
}

void Numerobis::PrintLogo() {
  cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . . . . . : : . . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . : . j G : . L D D E i # j : . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . .   K G . : G D E i i ; # W E . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . j : L . : L D j ; j # # i ; K . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . G . E : L G L ; i j W ; ; i i : : . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . : D . G . G G i i t G # t , . t L ; : . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . j . G : L ; G E i i i ; i   # ; ; ; i , L : : . . . . . . . ." << endl;
  cout << ". . . . . . . . . G ; L . f L G j i ; t t ; j ; ; t ; i i i ; L . . . . . . . ." << endl;
  cout << ". . . . . . . . i G D , f f D G G E ; ; i f i ; ; i i i i i i i . : . . . . . ." << endl;
  cout << ". . . . . . . : L : D i : i G D E L t i i i ; i i i i i i i i i G . . . . . . ." << endl;
  cout << ". . . . . . . . L , f G ; i i D # f ; i i i i K t i i i i i i i i . . . . . . ." << endl;
  cout << ". . . . . . .   G G . t i ; i i D K ; i i L # i E i i i i i i i i . . . . . . ." << endl;
  cout << ". . . . . . . D : : G # ; G ; ; i # ; ; i ; W t ; t i i i i i i i . . . . . . ." << endl;
  cout << ". . . . . . . D G : G W t i K L i G i i i ; W W t K ; ; i i i G W D . . . . . ." << endl;
  cout << ". . . . . . . : i L . : ; j L i t t i ; i i E W i ; # i i i i , ; W f . . . . ." << endl;
  cout << ". . . . . . : ; , : D D W ; ; ; i i G ; i ; G D i ; ; W i K G f f E j . . . . ." << endl;
  cout << ". . . . . : t : i . . . : : i , K i i t i f E D ; i ; i . i i i i t : . . . . ." << endl;
  cout << ". . . . . : , . i . t , . . : j L W ; i t i i i i ; ; t D i i i t i ; . . . . ." << endl;
  cout << ". . . . . : t t i : , . . . f ,     i W ; E D t K L . : j E i G , i ; . . . . ." << endl;
  cout << ". . . . . . . . , t j . . :           # j W # # # #   ; ; j i i i i L . . . . ." << endl;
  cout << ". . . . . . . . .   , . j ;             ; D # # # # D i ; ; i K , ; . . . . . ." << endl;
  cout << ". . . . . . . . . . : .   . :   , W i i ;   # t , : i i i ; i i i j . . . . . ." << endl;
  cout << ". . . . . . . . . . . . E   i # W ; i ; i i     . t i i i ; t t ; : . . . . . ." << endl;
  cout << ". . . . . . . . . . . j .   , .   j ; i i ; ; i ; i i i i t t i # . . . . . . ." << endl;
  cout << ". . . . . . . . . . . E     . L   , ; i i ; i i i i i ; i W ; i j . . . . . . ." << endl;
  cout << ". . . . . . . : . :   #             D ; i ; ; i i i i i # t i ; . . . . . . . ." << endl;
  cout << ". . . . . . . . . W # # #             G t j i ; i K t i ; i L : . . . . . . . ." << endl;
  cout << ". . . . . . : .     j # # W               f i     E i i ; t . . . . . . . . . ." << endl;
  cout << ". . . . . : . t       # # # # D                   W E ; D . . . . . . . . . . ." << endl;
  cout << ". . . . . : G           # # # # # # K G ;   .     W . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .             j # # # # # # t # # f   j   . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .                 # # # # j W # # j # t . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .                   G # # j # # # j # . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . .                       ; t # # # f #   . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . :           , ; ; i , j E # # # G   , . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . L                   ; j # # # E ;   f . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . D                   f t # # # G     K . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . : L             . i   G j # # # j     W . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . f       i t   .     t K # # # t :   K . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . L           t     . t G K # # j t   , . . . . . . . . . . . . . . :" << endl;
  cout << ". . . . . . .   i i             t t j j j j L E . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . t                     . t L D . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . G                   . :     L . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . : # K             t           t . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . :   : K # # # # # E D D # # # # K : . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . .     ,     t K W L E ; f f :     i D L , . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . : ; E         : G j ; ; t t i i ; f i i G L j . . . . . . . . . . . ." << endl;
  cout << ". . . . . . G i ; t ; ; ; ; i i ; L , f t t t ; G t i G G f . . . . . . . . . ." << endl;
  cout << ". . . . . . . . : : . , G E G G f ; D t G G # i j W G E L ; . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . : : . . : . : . . . . . . . : . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . .N.U.M.E.R.O.B.I.S. . . . . . . . . . . . . . . . . . . ." << endl;
  cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << endl;
  
  
  
}

} // namespace numerobis 
} // namespace builder 
} // namespace doofit