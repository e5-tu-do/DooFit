# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Gerhard Raven, see:
# link: https://indico.cern.ch/conferenceDisplay.py?confId=258524
# link: https://cds.cern.ch/record/1563169
# from PhysFit/P2VV
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(dfp2vvpdfs_dict RooAmoroso.h
RooCruijff.h RooAbsEffResModel.h RooEffResAddModel.h RooAbsGaussModelEfficiency.h RooCubicSplineFun.h RooCubicSplineKnot.h RooEffConvGenContext.h RooEffResModel.h RooGaussEfficiencyModel.h RooBinnedFun.h)

add_library(dfp2vvpdfs SHARED
RooAmoroso.cxx RooAmoroso.h RooAmoroso_dict.cc RooAmoroso_dict.h
RooCruijff.cxx RooCruijff.h RooCruijff_dict.cc RooCruijff_dict.h
RooAbsEffResModel.h RooAbsEffResModel_dict.cc RooAbsEffResModel_dict.h 
RooAbsGaussModelEfficiency.cxx RooAbsGaussModelEfficiency.h RooAbsGaussModelEfficiency_dict.cc RooAbsGaussModelEfficiency_dict.h 
RooCubicSplineFun.cxx RooCubicSplineFun.h RooCubicSplineFun_dict.cc RooCubicSplineFun_dict.h 
RooCubicSplineKnot.cxx RooCubicSplineKnot.h RooCubicSplineKnot_dict.cc RooCubicSplineKnot_dict.h 
RooEffConvGenContext.cxx RooEffConvGenContext.h RooEffConvGenContext_dict.cc RooEffConvGenContext_dict.h 
RooEffResModel.cxx RooEffResModel.h RooEffResModel_dict.cc RooEffResModel_dict.h
RooEffResAddModel.cxx RooEffResAddModel.h RooEffResAddModel_dict.cc RooEffResAddModel_dict.h 
RooGaussEfficiencyModel.cxx RooGaussEfficiencyModel.h RooGaussEfficiencyModel_dict.cc RooGaussEfficiencyModel_dict.h
RooBinnedFun.cxx RooBinnedFun.h RooBinnedFun_dict.cc RooBinnedFun_dict.h

)

target_link_libraries(dfp2vvpdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfp2vvpdfs dfp2vvpdfs_dict)

install(TARGETS dfp2vvpdfs DESTINATION lib)
install(FILES RooAmoroso.h RooCruijff.h RooAbsEffResModel.h RooAbsGaussModelEfficiency.h
RooCubicSplineFun.h RooCubicSplineKnot.h RooEffConvGenContext.h
RooEffResModel.h RooEffResAddModel.h RooGaussEfficiencyModel.h RooBinnedFun.h DESTINATION
include/P2VV)
install(FILES
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooAmoroso_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooCruijff_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooAbsEffResModel_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooAbsGaussModelEfficiency_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooCubicSplineFun_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooCubicSplineKnot_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooEffConvGenContext_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooEffResModel_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooEffResAddModel_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooGaussEfficiencyModel_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/P2VV/RooBinnedFun_dict_rdict.pcm
DESTINATION lib)
