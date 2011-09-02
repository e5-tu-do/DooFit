#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"

// from RooFit
#include "RooDataSet.h"

// from Project
#include "Config/CommonConfig.h"
#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

using namespace ROOT;
using namespace RooFit;

ToyFactoryStd::ToyFactoryStd(const CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac) :
config_common_(cfg_com),
config_toyfactory_(cfg_tfac)
{
  
}

ToyFactoryStd::~ToyFactoryStd(){
  
}

ToyFactoryStd::Generate() {
  
}