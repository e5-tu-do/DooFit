
root_generate_dictionaries_local(dfPdfsDict 
CubicSplinePdf.h)

add_library(dfPdfs SHARED 
CubicSplinePdf.h CubicSplinePdf.cxx CubicSplinePdf_dict.h CubicSplinePdf_dict.cc)

target_link_libraries(dfPdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfPdfs dfPdfsDict)

install(TARGETS dfPdfs DESTINATION lib)
install(FILES 
CubicSplinePdf.h
DESTINATION include/doofit/roofit/pdfs)
