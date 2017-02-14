# --------------------------------------------------------------------------------------------------------------
# Extra SchillerRooPdfs from Manuel Schiller, LHCb week 06/2013, ask Tobias Brambach, Christophe Cauet, or Florian Kruse
# from PhysFit/B2DXFitters v
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(schillerroopdfsDict SchillerRooAbsEffResModel.h SchillerRooBinnedPdf.h SchillerRooEffConvGenContext.h SchillerRooEffResModel.h)

add_library(dfschillerroopdfs SHARED 
SchillerRooAbsEffResModel.cxx SchillerRooAbsEffResModel.h SchillerRooAbsEffResModel_dict.cc SchillerRooAbsEffResModel_dict.h 
SchillerRooBinnedPdf.cxx SchillerRooBinnedPdf.h SchillerRooBinnedPdf_dict.cc SchillerRooBinnedPdf_dict.h 
SchillerRooEffConvGenContext.cxx SchillerRooEffConvGenContext.h SchillerRooEffConvGenContext_dict.cc SchillerRooEffConvGenContext_dict.h 
SchillerRooEffResModel.cxx SchillerRooEffResModel.h SchillerRooEffResModel_dict.cc SchillerRooEffResModel_dict.h 
)

target_link_libraries(dfschillerroopdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfschillerroopdfs dfschillerroopdfsDict)

install(TARGETS dfschillerroopdfs DESTINATION lib)
install(FILES SchillerRooAbsEffResModel.h SchillerRooBinnedPdf.h SchillerRooEffConvGenContext.h SchillerRooEffResModel.h DESTINATION
include/PrivateSchiller)
install(FILES
${CMAKE_BINARY_DIR}/src/lhcb/PrivateSchiller/SchillerRooAbsEffResModel_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/PrivateSchiller/SchillerRooBinnedPdf_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/PrivateSchiller/SchillerRooEffConvGenContext_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/lhcb/PrivateSchiller/SchillerRooEffResModel_dict_rdict.pcm
DESTINATION lib)
