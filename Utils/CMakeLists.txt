find_package(ROOT REQUIRED)

#include_directories(${ROOT_INCLUDE_DIR})

#set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
#set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_library(Utils Utils.cpp Utils.h MsgStream.cpp MsgStream.h)

target_link_libraries(Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES})
