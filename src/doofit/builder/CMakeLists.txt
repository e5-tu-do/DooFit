add_subdirectory(BuilderStd)
add_subdirectory(bobthebuilder)
add_subdirectory(FitModelBrewery)
#add_subdirectory(Sin2bSPBBuilder)
add_subdirectory(numerobis)

add_library(Builder SHARED EasyPdf/EasyPdf.h EasyPdf/EasyPdf.cpp)
target_link_libraries(Builder Config ${ALL_LIBRARIES})

install(TARGETS Builder DESTINATION lib)
install(FILES EasyPdf/EasyPdf.h DESTINATION include/doofit/builder/EasyPdf)

