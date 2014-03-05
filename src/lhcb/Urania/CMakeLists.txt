# --------------------------------------------------------------------------------------------------------------
# Pdfs for Mass description from Diego Martinez Santos:
# documentation of Ipatia at http://arxiv.org/abs/1312.5000
# from Phys/Urania  (URANIA_v2r1)
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(dfuraniapdfs_dict
RooIpatia.h
#RooAmorosoPdf.h RooApollonios.h RooGeneralizedHyperbolic.h RooJohnsonSU.h RooLogGamma.h RooReparamGHYP.h RooIpatia2.h
)

add_library(dfuraniapdfs SHARED
#RooAmorosoPdf.cxx RooAmorosoPdf.h RooAmorosoPdf_dict.cc RooAmorosoPdf_dict.h
#RooApollonios.cxx RooApollonios.h RooApollonios_dict.cc RooApollonios_dict.h
#RooGeneralizedHyperbolic.cxx  RooGeneralizedHyperbolic.h  RooGeneralizedHyperbolic_dict.cc  RooGeneralizedHyperbolic_dict.h
RooIpatia.cxx RooIpatia.h RooIpatia_dict.cc RooIpatia_dict.h
#RooIpatia2.cxx RooIpatia2.h RooIpatia2_dict.cc RooIpatia2_dict.h
#RooJohnsonSU.cxx RooJohnsonSU.h RooJohnsonSU_dict.cc RooJohnsonSU_dict.h
#RooLogGamma.cxx RooLogGamma.h RooLogGamma_dict.cc RooLogGamma_dict.h
#RooReparamGHYP.cxx RooReparamGHYP.h RooReparamGHYP_dict.cc RooReparamGHYP_dict.h
)

target_link_libraries(dfuraniapdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfuraniapdfs dfuraniapdfs_dict)

install(TARGETS dfuraniapdfs DESTINATION lib)
install(FILES RooIpatia.h
#RooAmorosoPdf.h RooApollonios.h RooGeneralizedHyperbolic.h  RooJohnsonSU.h RooLogGamma.h RooReparamGHYP.h RooIpatia2.h
DESTINATION include/URANIA)