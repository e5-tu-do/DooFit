# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Dortmund
# Authors: Ulrich Eitschberger (RooBMixAsymDecay), Frank Meier (RooBCPGenUt)
# 2013-09-20
# ______________________________________________________________________________________________________________

root_generate_dictionaries_local(extraroopdfsDict RooBMixAsymDecay.h RooBCPGenDecayUt.h)
add_library(dfdoroopdfs SHARED RooBMixAsymDecay.cpp RooBMixAsymDecay.h RooBMixAsymDecay_dict.cc RooBMixAsymDecay_dict.h RooBCPGenDecayUt.cxx RooBCPGenDecayUt.h RooBCPGenDecayUt_dict.cc RooBCPGenDecayUt_dict.h)
target_link_libraries(dfdoroopdfs ${ALL_LIBRARIES})

# not sure if it actually works, I guess not. Idea: Should help in multithreaded builds.
add_dependencies(dfdoroopdfs dfdoroopdfsDict)

install(TARGETS dfdoroopdfs DESTINATION lib/dopdfs)
install(FILES RooBMixAsymDecay.h RooBCPGenDecayUt.h DESTINATION include/doofit/extraroopdfs/dopdfs)
