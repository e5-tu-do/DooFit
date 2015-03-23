# --------------------------------------------------------------------------------------------------------------
# Pdfs for Mass description from Diego Martinez Santos:
# documentation of Ipatia at http://arxiv.org/abs/1312.5000
# from Phys/Urania  (URANIA_v2r1)
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(dfamorosopdf_dict RooAmorosoPdf.h)
root_generate_dictionaries_local(dfapollonios_dict RooApollonios.h)
root_generate_dictionaries_local(dfgeneralizedhyperbolic_dict RooGeneralizedHyperbolic.h)
root_generate_dictionaries_local(dfipatia_dict RooIpatia.h)
root_generate_dictionaries_local(dfipatia2_dict RooIpatia2.h)
root_generate_dictionaries_local(dfjohnsonsu_dict RooJohnsonSU.h)
root_generate_dictionaries_local(dfloggamma_dict RooLogGamma.h)
root_generate_dictionaries_local(dfreparamghyp_dict RooReparamGHYP.h)
root_generate_dictionaries_local(dfdecratecoeff_dict DecRateCoeff.h)

add_library(dfamorosopdf SHARED RooAmorosoPdf.cxx RooAmorosoPdf.h RooAmorosoPdf_dict.cc RooAmorosoPdf_dict.h)
add_library(dfapollonios SHARED RooApollonios.cxx RooApollonios.h RooApollonios_dict.cc RooApollonios_dict.h)
add_library(dfgeneralizedhyperbolic SHARED RooGeneralizedHyperbolic.cxx  RooGeneralizedHyperbolic.h  RooGeneralizedHyperbolic_dict.cc  RooGeneralizedHyperbolic_dict.h)
add_library(dfipatia SHARED RooIpatia.cxx RooIpatia.h RooIpatia_dict.cc RooIpatia_dict.h)
add_library(dfipatia2 SHARED RooIpatia2.cxx RooIpatia2.h RooIpatia2_dict.cc RooIpatia2_dict.h)
add_library(dfjohnsonsu SHARED RooJohnsonSU.cxx RooJohnsonSU.h RooJohnsonSU_dict.cc RooJohnsonSU_dict.h)
add_library(dfloggamma SHARED RooLogGamma.cxx RooLogGamma.h RooLogGamma_dict.cc RooLogGamma_dict.h)
add_library(dfreparamghyp SHARED RooReparamGHYP.cxx RooReparamGHYP.h RooReparamGHYP_dict.cc RooReparamGHYP_dict.h)
add_library(dfdecratecoeff SHARED DecRateCoeff.cxx DecRateCoeff.h DecRateCoeff_dict.cc DecRateCoeff_dict.h)

target_link_libraries(dfamorosopdf ${ALL_LIBRARIES})
target_link_libraries(dfapollonios ${ALL_LIBRARIES})
target_link_libraries(dfgeneralizedhyperbolic ${ALL_LIBRARIES})
target_link_libraries(dfipatia ${ALL_LIBRARIES})
target_link_libraries(dfipatia2 ${ALL_LIBRARIES})
target_link_libraries(dfjohnsonsu ${ALL_LIBRARIES})
target_link_libraries(dfloggamma ${ALL_LIBRARIES})
target_link_libraries(dfreparamghyp ${ALL_LIBRARIES})
target_link_libraries(dfdecratecoeff ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfamorosopdf dfamorosopdf_dict)
add_dependencies(dfapollonios dfapollonios_dict)
add_dependencies(dfgeneralizedhyperbolic dfgeneralizedhyperbolic_dict)
add_dependencies(dfipatia dfipatia_dict)
add_dependencies(dfipatia2 dfipatia2_dict)
add_dependencies(dfjohnsonsu dfjohnsonsu_dict)
add_dependencies(dfloggamma dfloggamma_dict)
add_dependencies(dfreparamghyp dfreparamghyp_dict)
add_dependencies(dfdecratecoeff dfdecratecoeff_dict)

install(TARGETS dfamorosopdf dfapollonios dfgeneralizedhyperbolic dfipatia dfipatia2 dfjohnsonsu dfloggamma dfreparamghyp dfdecratecoeff DESTINATION lib)
install(FILES RooIpatia2.h RooAmorosoPdf.h RooApollonios.h RooGeneralizedHyperbolic.h  RooJohnsonSU.h RooLogGamma.h RooReparamGHYP.h RooIpatia.h DecRateCoeff.h DESTINATION include/Urania)
