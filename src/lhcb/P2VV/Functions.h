// $Id: $
#ifndef FUNCTIONS_H 
#define FUNCTIONS_H 1

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "Rtypes.h"

class TH1l;
class TTree;
class RooDataSet;

double sigmaFromFT( const TH1& h1, const double dMs, const double dMsErr, std::ostream& out = std::cout );

void addSWeightToTree(const std::vector<double>& weights, TTree& tree, const std::string& branch_name);
void addSWeightToTree(double* weights, size_t n, TTree& tree, const std::string& branch_name);
void addSWeightToTree(const RooDataSet& ds, TTree& tree, const std::string& branchname,
                      const std::string& cut = std::string("1"));
void addRunPeriodToTree(TTree& tree, Int_t period, const char* branchName);

void addVertexErrors(TTree* tree, const std::list<RooDataSet*>& dss, const std::string& cut);

TTree* RooDataSetToTree(const RooDataSet& dataSet, const char* name = 0,
      const char* title = 0, const char* weightName = 0,
      const char* branchList = 0, Bool_t RooFitFormat = kTRUE);

RooDataSet* TreeToRooDataSet(TTree& tree, const RooArgSet& observables,
      const char* name = 0, const char* title = 0, const char* cuts = 0,
      const char* indexName = 0, RooDataSet* origDataSet = 0);

#endif // FUNCTIONS_H
