# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Gerhard Raven, see:
# link: https://indico.cern.ch/conferenceDisplay.py?confId=258524
# link: https://cds.cern.ch/record/1563169
# from PhysFit/P2VV
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(dfp2vvpdfs_dict RooAbsEffResModel.h RooEffResAddModel.h RooAbsGaussModelEfficiency.h RooCubicSplineFun.h RooCubicSplineKnot.h RooEffConvGenContext.h RooEffResModel.h RooGaussEfficiencyModel.h)

add_library(dfp2vvpdfs SHARED 
RooAbsEffResModel.h RooAbsEffResModel_dict.cc RooAbsEffResModel_dict.h 
RooAbsGaussModelEfficiency.cxx RooAbsGaussModelEfficiency.h RooAbsGaussModelEfficiency_dict.cc RooAbsGaussModelEfficiency_dict.h 
RooCubicSplineFun.cxx RooCubicSplineFun.h RooCubicSplineFun_dict.cc RooCubicSplineFun_dict.h 
RooCubicSplineKnot.cxx RooCubicSplineKnot.h RooCubicSplineKnot_dict.cc RooCubicSplineKnot_dict.h 
RooEffConvGenContext.cxx RooEffConvGenContext.h RooEffConvGenContext_dict.cc RooEffConvGenContext_dict.h 
RooEffResModel.cxx RooEffResModel.h RooEffResModel_dict.cc RooEffResModel_dict.h
RooEffResAddModel.cxx RooEffResAddModel.h RooEffResAddModel_dict.cc RooEffResAddModel_dict.h 
RooGaussEfficiencyModel.cxx RooGaussEfficiencyModel.h RooGaussEfficiencyModel_dict.cc RooGaussEfficiencyModel_dict.h
)

target_link_libraries(dfp2vvpdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfp2vvpdfs dfp2vvpdfs_dict)

install(TARGETS dfp2vvpdfs DESTINATION lib)
install(FILES RooAbsEffResModel.h RooAbsGaussModelEfficiency.h
RooCubicSplineFun.h RooCubicSplineKnot.h RooEffConvGenContext.h
RooEffResModel.h RooEffResAddModel.h RooGaussEfficiencyModel.h DESTINATION
include/P2VV)
