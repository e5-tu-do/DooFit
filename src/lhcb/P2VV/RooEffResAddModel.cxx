#include <set>
#include <vector>

#include "Riostream.h"
#include "RooMsgService.h"
#include <RooAddModel.h>

#include <P2VV/RooAbsEffResModel.h>
#include <P2VV/RooEffResAddModel.h>

//_____________________________________________________________________________R
RooEffResAddModel::RooEffResAddModel()
   : RooAddModel(), RooAbsEffResModel(),
     _addModel(0)
{

}

//_____________________________________________________________________________
RooEffResAddModel::RooEffResAddModel(const char *name, const char *title, const RooArgList& modelList,
                  const RooArgList& coefList, Bool_t ownPdfList)
   : RooAddModel(name, title, modelList, coefList, ownPdfList),
     RooAbsEffResModel(),
     _addModel(0)
{
   RooFIter iter = modelList.fwdIterator();
   while(RooAbsArg* model = iter.next()) {
      //TODO: verify all models share the same efficiency 
      RooAbsEffResModel* effModel = dynamic_cast<RooAbsEffResModel*>(model);
      if (effModel == 0) {
        coutE(InputArguments) << "RooEffResAddModel(" << GetName()
            << "): \"" << model->GetName() << "\" is not RooAbsEffResModel"
            << std::endl;
        assert(effModel);
      }
   }
}
 
//_____________________________________________________________________________
RooEffResAddModel::RooEffResAddModel(const RooEffResAddModel& other, const char* name)
   : RooAddModel(other, name),
     RooAbsEffResModel(),
     _addModel(0)
{

}

//_____________________________________________________________________________
RooEffResAddModel::RooEffResAddModel(const RooAddModel& other, const char* name)
   : RooAddModel(other, name),
     RooAbsEffResModel(),
     _addModel(0)
{

}

//_____________________________________________________________________________
RooEffResAddModel::~RooEffResAddModel( )
{
   delete _addModel;
}

//_____________________________________________________________________________
RooResolutionModel* RooEffResAddModel::convolution(RooFormulaVar* inBasis, RooAbsArg* owner) const
{
   _addModel = static_cast<RooAddModel*>(RooAddModel::convolution(inBasis, owner));
   return new RooEffResAddModel(*_addModel);
}
   
//_____________________________________________________________________________
const RooAbsReal* RooEffResAddModel::efficiency() const
{
   std::set<const RooAbsReal*> eff_set;
   RooFIter it = pdfList().fwdIterator();
   while(RooAbsArg* arg = it.next()) {
      const RooAbsEffResModel* model = dynamic_cast<const RooAbsEffResModel*>(arg);
      const RooAbsReal* model_eff = model->efficiency();
      eff_set.insert(model_eff);
   }
   assert(eff_set.size() == 1);
   return *eff_set.begin();
}

//_____________________________________________________________________________
RooArgSet* RooEffResAddModel::observables() const {
   // Return pointer to pdf in product
   return new RooArgSet(RooAddModel::convVar());
}
