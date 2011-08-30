# Executables
add_executable(FitterStd FitterStd.cpp)

# Aliases for module libs
set(UTILS_LIBS Utils)
set(CONFIG_LIBS CommonConfig Config)
set(PDF2WS_STD_LIBS Pdf2WsStd)
set(BUILDER_STD_LIBS BuilderStd BuilderStdConfig)

# Linker information
target_link_libraries(FitterStd ${BUILDER_STD_LIBS} ${PDF2WS_STD_LIBS} ${CONFIG_LIBS} ${UTILS_LIBS} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
