# Executables
add_executable(FitterStd FitterStd.cpp)

set(PDF2WS_STD_LIBS Pdf2WsStdMass Pdf2WsStdProptime Pdf2WsStdProptimeErr Pdf2WsStdCommonFuncs)

# Linker information
target_link_libraries(FitterStd ${PDF2WS_STD_LIBS} ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES})
