# ------------------------------------------------------------------------------
# Authors: Christophe Cauet
# 2014-08-01
# ______________________________________________________________________________

root_generate_dictionaries_local(dfBDecayDict 
CalibratedMistag.h
CombinedTag.h
CombinedMistag.h
Coefficient.h
SimpleCoefficient.h
Coefficient2.h
)

add_library(dfBDecay SHARED 
CalibratedMistag.h CalibratedMistag.cxx CalibratedMistag_dict.h CalibratedMistag_dict.cc
CombinedTag.h CombinedTag.cxx CombinedTag_dict.h CombinedTag_dict.cc
CombinedMistag.h CombinedMistag.cxx CombinedMistag_dict.h CombinedMistag_dict.cc
Coefficient.h Coefficient.cxx Coefficient_dict.h Coefficient_dict.cc
SimpleCoefficient.h SimpleCoefficient.cxx SimpleCoefficient_dict.h SimpleCoefficient_dict.cc
Coefficient2.h Coefficient2.cxx Coefficient2_dict.h Coefficient2_dict.cc
)

target_link_libraries(dfBDecay ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfBDecay dfBDecayDict)

install(TARGETS dfBDecay DESTINATION lib)
install(FILES 
CalibratedMistag.h
CombinedTag.h
CombinedMistag.h
Coefficient.h
SimpleCoefficient.h
Coefficient2.h
DESTINATION include/doofit/roofit/functions/bdecay)