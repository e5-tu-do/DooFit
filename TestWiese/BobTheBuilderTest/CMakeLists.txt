# Executables
add_executable(BobTheBuilderTest BobTheBuilderTest.cpp)

# Aliases for module libs

set(PDFS_LIBS Builder_BobTheBuilder_Bob Builder_BobTheBuilder_PdfsMass Builder_BobTheBuilder_PdfsTag Builder_BobTheBuilder_PdfsCommon)

# Linker information
target_link_libraries(BobTheBuilderTest ${PDFS_LIBS} ${ALL_LIBRARIES})
