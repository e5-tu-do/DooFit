# Executables
add_executable(BuilderStandardTest BuilderStandardTest.cpp)

# Aliases for module libs

set(PDFS_LIBS Builder_Standard_PdfsCommon Builder_Standard_PdfsMass Builder_Standard_PdfsTag)
set(BUILDER_LIBS Builder_Standard_Constructor)
set(UTILS_LIBS Utils)

# Linker information
target_link_libraries(BuilderStandardTest ${BUILDER_LIBS} ${PDFS_LIBS} ${UTILS_LIBS} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
