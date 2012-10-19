MESSAGE(STATUS "Looking for DooFit...")

SET(DooFit_FOUND FALSE)

IF(IS_DIRECTORY $ENV{DOOFITSYS})
  SET(DooFit_BASE $ENV{DOOFITSYS})
  SET(DooFit_FOUND TRUE)
  SET(DooFit_INCLUDES "${DooFit_BASE}/include")
  SET(DooFit_INCLUDE_DIR "${DooFit_BASE}/include")
  SET(DooFit_LIBRARY_DIR "${DooFit_BASE}/lib")
  SET(DooFit_LIBRARIES "-lToy -lPdf2WsStd -lPlotting -lConfig")
  #SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${DooFit_LIBRARY_DIR})
  MESSAGE(STATUS "Looking for DooFit... - found at ${DooFit_BASE}")
ENDIF(IS_DIRECTORY $ENV{DOOFITSYS})
