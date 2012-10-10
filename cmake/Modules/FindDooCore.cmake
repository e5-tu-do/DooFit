MESSAGE(STATUS "Looking for DooCore...")

SET(DooCore_FOUND FALSE)

IF(IS_DIRECTORY $ENV{DOOCORESYS})
  SET(DooCore_BASE $ENV{DOOCORESYS})
  SET(DooCore_FOUND TRUE)
  SET(DooCore_INCLUDES "${DooCore_BASE}/include")
  SET(DooCore_INCLUDE_DIR "${DooCore_BASE}/include")
  SET(DooCore_LIBRARY_DIR "${DooCore_BASE}/lib")
  SET(DooCore_LIBRARIES "-lSystem -lIO -lLUtils")
  #SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${DooCore_LIBRARY_DIR})
  MESSAGE(STATUS "Looking for DooCore... - found at ${DooCore_BASE}")
ENDIF(IS_DIRECTORY $ENV{DOOCORESYS})
