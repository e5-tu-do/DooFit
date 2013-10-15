// $Id: $
#ifndef ROOEFFRESADDMODEL_H 
#define ROOEFFRESADDMODEL_H 1

// Include files
#include <RooAddModel.h>

#include <P2VV/RooAbsEffResModel.h>

/** @class RooEffResAddModel RooEffResAddModel.h P2VV/RooEffResAddModel.h
 *  
 *
 *  @author Roel Aaij
 *  @date   2013-05-11
 */
class RooEffResAddModel : public RooAddModel, public RooAbsEffResModel {
public: 
   /// Standard constructor
   RooEffResAddModel();
   RooEffResAddModel(const char *name, const char *title, const RooArgList& modelList,
                     const RooArgList& coefList, Bool_t ownPdfList=kFALSE);
   /// Copy constructor
   RooEffResAddModel(const RooEffResAddModel& other, const char* newName = 0);
   RooEffResAddModel(const RooAddModel& other, const char* newName = 0);
   
   /// Destructor
   virtual ~RooEffResAddModel( );

   /// Virtual constructor
   virtual TObject* clone(const char* name) const {
      return new RooEffResAddModel(*this, name);
   }

   /** 
    * Get a RooArgSet of all observables, caller takes ownership.
    * (pointer because genreflex dictionaries can't handle value)
    * 
    * @return RooArgSet of observables
    */
   virtual RooArgSet* observables() const;

   virtual const RooAbsReal* efficiency() const;

protected:

   RooResolutionModel* convolution(RooFormulaVar* inBasis, RooAbsArg* owner) const;

private:

   mutable RooAddModel* _addModel;

   ClassDef(RooEffResAddModel, 1)

};
#endif // P2VV_ROOEFFRESADDMODEL_H
