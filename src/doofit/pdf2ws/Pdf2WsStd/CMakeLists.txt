add_library(Pdf2WsStd SHARED Pdf2WsStdCommonFuncs.cpp Pdf2WsStdCommonFuncs.h Pdf2WsStdMass.cpp Pdf2WsStdMass.h Pdf2WsStdProptime.cpp Pdf2WsStdProptime.h Pdf2WsStdProptimeErr.cpp Pdf2WsStdProptimeErr.h)
target_link_libraries(Pdf2WsStd ${ALL_LIBRARIES})
install(TARGETS Pdf2WsStd DESTINATION lib)
install(FILES Pdf2WsStdCommonFuncs.h Pdf2WsStdMass.h Pdf2WsStdProptime.h Pdf2WsStdProptimeErr.h DESTINATION include/doofit/Pdf2Ws/Pdf2WsStd)
