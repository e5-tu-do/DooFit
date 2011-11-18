# Executables
add_executable(BobTheBuilderTest BobTheBuilderTest.cpp)

# Aliases for module libs

set(PDFS_LIBS Builder_BobTheBuilder_PdfsCommon Builder_BobTheBuilder_PdfsMass Builder_BobTheBuilder_PdfsTag)
set(BUILDER_LIBS Builder_BobTheBuilder_Bob)
set(UTILS_LIBS Utils)

# Linker information
target_link_libraries(BobTheBuilderTest ${BUILDER_LIBS} ${PDFS_LIBS} ${UTILS_LIBS} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
