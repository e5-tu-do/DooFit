root_generate_dictionaries_local(extraroopdfsDict RooBMixAsymDecay.h)
add_library(extraroopdfs SHARED RooBMixAsymDecay.cpp RooBMixAsymDecay.h RooBMixAsymDecay_dict.cc RooBMixAsymDecay_dict.h)
target_link_libraries(extraroopdfs ${ALL_LIBRARIES})

# not sure if needed, should help in multithreaded builds.
add_dependencies(extraroopdfs RooBMixAsymDecay_TARGET)

install(TARGETS extraroopdfs DESTINATION lib)
install(FILES RooBMixAsymDecay.h DESTINATION include/doofit/extraroopdfs)
