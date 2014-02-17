# --------------------------------------------------------------------------------------------------------------
# Extra SchillerRooPdfs from Manuel Schiller, LHCb week 06/2013, ask Tobias Brambach, Christophe Cauet, or Florian Kruse
# from PhysFit/B2DXFitters v
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(schillerroopdfsDict DecRateCoeff.h SchillerRooAbsEffResModel.h SchillerRooBinnedPdf.h SchillerRooEffConvGenContext.h SchillerRooEffResModel.h)

add_library(dfschillerroopdfs SHARED 
DecRateCoeff.cxx DecRateCoeff.h DecRateCoeff_dict.cc DecRateCoeff_dict.h 
SchillerRooAbsEffResModel.cxx SchillerRooAbsEffResModel.h SchillerRooAbsEffResModel_dict.cc SchillerRooAbsEffResModel_dict.h 
SchillerRooBinnedPdf.cxx SchillerRooBinnedPdf.h SchillerRooBinnedPdf_dict.cc SchillerRooBinnedPdf_dict.h 
SchillerRooEffConvGenContext.cxx SchillerRooEffConvGenContext.h SchillerRooEffConvGenContext_dict.cc SchillerRooEffConvGenContext_dict.h 
SchillerRooEffResModel.cxx SchillerRooEffResModel.h SchillerRooEffResModel_dict.cc SchillerRooEffResModel_dict.h 
)

target_link_libraries(dfschillerroopdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfschillerroopdfs dfschillerroopdfsDict)

install(TARGETS dfschillerroopdfs DESTINATION lib)
install(FILES DecRateCoeff.h SchillerRooAbsEffResModel.h SchillerRooBinnedPdf.h SchillerRooEffConvGenContext.h SchillerRooEffResModel.h DESTINATION include/doofit/extraroopdfs/schillerpdfs)

