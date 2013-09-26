# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Dortmund
# Authors: Ulrich Eitschberger (RooBMixAsymDecay), Frank Meier (RooBCPGenUt), Julian Wishahi (BiasDelta)
# 2013-09-20
# ______________________________________________________________________________________________________________

root_generate_dictionaries_local(extraroopdfsDict RooBMixAsymDecay.h RooBCPGenDecayUt.h BiasDelta.h)
add_library(dfdoroopdfs SHARED RooBMixAsymDecay.cpp RooBMixAsymDecay.h RooBMixAsymDecay_dict.cc RooBMixAsymDecay_dict.h RooBCPGenDecayUt.cxx RooBCPGenDecayUt.h RooBCPGenDecayUt_dict.cc RooBCPGenDecayUt_dict.h BiasDelta.cpp BiasDelta.h BiasDelta_dict.cc BiasDelta_dict.h)
target_link_libraries(dfdoroopdfs ${ALL_LIBRARIES})

# not sure if it actually works, I guess not. Idea: Should help in multithreaded builds.
add_dependencies(dfdoroopdfs dfdoroopdfsDict)

install(TARGETS dfdoroopdfs DESTINATION lib)
install(FILES RooBMixAsymDecay.h RooBCPGenDecayUt.h BiasDelta.h DESTINATION include/doofit/extraroopdfs/dopdfs)
