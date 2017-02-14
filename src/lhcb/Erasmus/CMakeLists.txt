# --------------------------------------------------------------------------------------------------------------
# Pdfs for Mass description from Paolo Gandini:
# from Phys/Erasmus  Bu2D0H
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(dfhilldini_dict RooHILLdini.h)
root_generate_dictionaries_local(dfhornsdini_dict RooHORNSdini.h)

add_library(dfhilldini SHARED RooHILLdini.cxx RooHILLdini.h RooHILLdini_dict.cc RooHILLdini_dict.h)
add_library(dfhornsdini SHARED RooHORNSdini.cxx RooHORNSdini.h RooHORNSdini_dict.cc RooHORNSdini_dict.h)

target_link_libraries(dfhilldini ${ALL_LIBRARIES})
target_link_libraries(dfhornsdini ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfhilldini dfhilldini_dict)
add_dependencies(dfhornsdini dfhornsdini_dict)

install(TARGETS dfhilldini dfhornsdini DESTINATION lib)
install(FILES RooHILLdini.h RooHORNSdini.h DESTINATION include/Erasmus)
install(FILES ${CMAKE_BINARY_DIR}/src/lhcb/Erasmus/RooHILLdini_dict_rdict.pcm ${CMAKE_BINARY_DIR}/src/lhcb/Erasmus/RooHORNSdini_dict_rdict.pcm DESTINATION lib)