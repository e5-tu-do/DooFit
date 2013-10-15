#ifndef ROO_EFF_CONV_GEN_CONTEXT
#define ROO_EFF_CONV_GEN_CONTEXT

// use C preprocessor to smuggle in a friend class into RooAbsGenContext
//
// *** this is an ugly hack but seems to work well enough for now, and is
// *** better than hacking the header files that come with ROOT for now...
#define RooEffGenContext RooEffGenContext; friend class RooEffConvGenContext;

#include "RooConvGenContext.h"
#include "RooArgSet.h"

class RooAbsAnaConvPdf;
class RooDataSet;
class RooAbsReal;

class RooEffConvGenContext : public RooConvGenContext {
public:
   RooEffConvGenContext(const RooFFTConvPdf &model, const RooArgSet &vars, const RooDataSet *prototype= 0,
                        const RooArgSet* auxProto=0, Bool_t _verbose= kFALSE);
   RooEffConvGenContext(const RooNumConvPdf &model, const RooArgSet &vars, const RooDataSet *prototype= 0,
                        const RooArgSet* auxProto=0, Bool_t _verbose= kFALSE);
   RooEffConvGenContext(const RooAbsAnaConvPdf &model, const RooArgSet &vars, const RooDataSet *prototype= 0,
                        const RooArgSet* auxProto=0, Bool_t _verbose= kFALSE);
   virtual ~RooEffConvGenContext() { }


protected:

   virtual void initGenerator(const RooArgSet& theEvent) ;
   virtual void generateEvent(RooArgSet &theEvent, Int_t remaining);

   RooEffConvGenContext(const RooEffConvGenContext& other) ;

private:

   void initEfficiency();
   const RooAbsReal* efficiency();

   double _maxEff;

   ClassDef(RooEffConvGenContext,0) // Context for generating a dataset from a PDF
};

#endif
