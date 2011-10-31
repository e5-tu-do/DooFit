# Executables
add_executable(FitterStd FitterStd.cpp)

# Aliases for module libs
set(UTILS_LIBS Utils)
set(CONFIG_LIBS CommonConfig Config)
set(PDF2WS_STD_LIBS Pdf2WsStd)
set(BUILDER_STD_LIBS BuilderStd BuilderStdConfig)
set(BUILDER_S2B_LIBS BuilderS2b BuilderS2bConfig)



# Linker information
target_link_libraries(FitterStd ${BUILDER_S2B_LIBS} ${PDF2WS_STD_LIBS} ${CONFIG_LIBS} ${UTILS_LIBS} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
