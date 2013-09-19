root_generate_dictionaries_local(extraroopdfsDict RooBMixAsymDecay.h RooBCPGenDecayUt.h)
add_library(extraroopdfs SHARED RooBMixAsymDecay.cpp RooBMixAsymDecay.h RooBMixAsymDecay_dict.cc RooBMixAsymDecay_dict.h RooBCPGenDecayUt.cxx RooBCPGenDecayUt.h RooBCPGenDecayUt_dict.cc RooBCPGenDecayUt_dict.h)
target_link_libraries(extraroopdfs ${ALL_LIBRARIES})

# not sure if it actually works, I guess not. Idea: Should help in multithreaded builds.
add_dependencies(extraroopdfs extraroopdfsDict)

install(TARGETS extraroopdfs DESTINATION lib)
install(FILES RooBMixAsymDecay.h RooBCPGenDecayUt.h DESTINATION include/doofit/extraroopdfs)


# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Manuel Schiller, LHCb week 06/2013, ask Tobias Brambach, Christophe Cauet, or Florian Kruse
# from PhysFit/B2DXFitters v
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(schillerroopdfsDict DecRateCoeff.h RooAbsEffResModel.h RooBinnedPdf.h RooEffConvGenContext.h RooEffResModel.h 
#RooAbsGaussModelEfficiency.h RooCubicSplineFun.h RooCubicSplineKnot.h RooGaussEfficiencyModel.h
)

add_library(schillerroopdfs SHARED 
DecRateCoeff.cxx DecRateCoeff.h DecRateCoeff_dict.cc DecRateCoeff_dict.h 
RooAbsEffResModel.cxx RooAbsEffResModel.h RooAbsEffResModel_dict.cc RooAbsEffResModel_dict.h 
RooBinnedPdf.cxx RooBinnedPdf.h RooBinnedPdf_dict.cc RooBinnedPdf_dict.h 
RooEffConvGenContext.cxx RooEffConvGenContext.h RooEffConvGenContext_dict.cc RooEffConvGenContext_dict.h 
RooEffResModel.cxx RooEffResModel.h RooEffResModel_dict.cc RooEffResModel_dict.h 
#RooAbsGaussModelEfficiency.cxx RooAbsGaussModelEfficiency.h RooAbsGaussModelEfficiency_dict.cc RooAbsGaussModelEfficiency_dict.h 
#RooCubicSplineFun.cxx RooCubicSplineFun.h RooCubicSplineFun_dict.cc RooCubicSplineFun_dict.h 
#RooCubicSplineKnot.cxx RooCubicSplineKnot.h RooCubicSplineKnot_dict.cc RooCubicSplineKnot_dict.h 
#RooGaussEfficiencyModel.cxx RooGaussEfficiencyModel.h RooGaussEfficiencyModel_dict.cc RooGaussEfficiencyModel_dict.h
)

target_link_libraries(schillerroopdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(schillerroopdfs schillerroopdfsDict)

install(TARGETS schillerroopdfs DESTINATION lib)
install(FILES DecRateCoeff.h RooAbsEffResModel.h RooBinnedPdf.h RooEffConvGenContext.h RooEffResModel.h 
#RooAbsGaussModelEfficiency.h RooCubicSplineFun.h RooCubicSplineKnot.h RooGaussEfficiencyModel.h 
DESTINATION include/doofit/extraroopdfs)
