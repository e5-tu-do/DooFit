# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Manuel Schiller, LHCb week 06/2013, ask Tobias Brambach, Christophe Cauet, or Florian Kruse
# from PhysFit/B2DXFitters v
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(schillerroopdfsDict DecRateCoeff.h RooAbsEffResModel.h RooBinnedPdf.h RooEffConvGenContext.h RooEffResModel.h)

add_library(dfschillerroopdfs SHARED 
DecRateCoeff.cxx DecRateCoeff.h DecRateCoeff_dict.cc DecRateCoeff_dict.h 
RooAbsEffResModel.cxx RooAbsEffResModel.h RooAbsEffResModel_dict.cc RooAbsEffResModel_dict.h 
RooBinnedPdf.cxx RooBinnedPdf.h RooBinnedPdf_dict.cc RooBinnedPdf_dict.h 
RooEffConvGenContext.cxx RooEffConvGenContext.h RooEffConvGenContext_dict.cc RooEffConvGenContext_dict.h 
RooEffResModel.cxx RooEffResModel.h RooEffResModel_dict.cc RooEffResModel_dict.h 
)

target_link_libraries(dfschillerroopdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfschillerroopdfs dfschillerroopdfsDict)

install(TARGETS dfschillerroopdfs DESTINATION lib/schillerpdfs)
install(FILES DecRateCoeff.h RooAbsEffResModel.h RooBinnedPdf.h RooEffConvGenContext.h RooEffResModel.h DESTINATION include/doofit/extraroopdfs/schillerpdfs)

