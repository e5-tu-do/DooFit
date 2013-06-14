root_generate_dictionaries_local(extraroopdfsDict RooBMixAsymDecay.h)
add_library(extraroopdfs SHARED RooBMixAsymDecay.cpp RooBMixAsymDecay.h RooBMixAsymDecay_dict.cc RooBMixAsymDecay_dict.h)
target_link_libraries(extraroopdfs ${ALL_LIBRARIES})

# not sure if it actually works, I guess not. Idea: Should help in multithreaded builds.
add_dependencies(extraroopdfs extraroopdfsDict)

install(TARGETS extraroopdfs DESTINATION lib)
install(FILES RooBMixAsymDecay.h DESTINATION include/doofit/extraroopdfs)


# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Manuel Schiller, LHCb week 06/2013, aks Tobias Brambach, Christophe Cauet, or Florian Kruse
# from PhysFit/B2DXFitters v
# ______________________________________________________________________________________________________________
add_library(schillerroopdfs SHARED DecRateCoeff.cxx DecRateCoeff.h RooAbsEffResModel.cxx RooAbsEffResModel.h RooBinnedPdf.cxx RooBinnedPdf.h RooEffConvGenContext.h RooEffResModel.cxx RooEffResModel.h)

install(TARGETS schillerroopdfs DESTINATION lib)
install(FILES DecRateCoeff.h RooAbsEffResModel.h RooBinnedPdf.h RooEffConvGenContext.h RooEffResModel.h DESTINATION include/doofit/extraroopdfs)