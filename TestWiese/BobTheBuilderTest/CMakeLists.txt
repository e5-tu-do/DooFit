# Executables
add_executable(BobTheBuilderTest BobTheBuilderTest.cpp)

# Aliases for module libs

set(PDFS_LIBS Builder_BobTheBuilder_Bob Builder_BobTheBuilder_PdfsMass Builder_BobTheBuilder_PdfsTag Builder_BobTheBuilder_PdfsCommon)

set(UTILS_LIBS Utils)

# Linker information
target_link_libraries(BobTheBuilderTest ${PDFS_LIBS} ${UTILS_LIBS} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
