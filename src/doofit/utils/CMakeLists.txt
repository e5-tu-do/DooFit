#set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
#set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_library(utils SHARED utils.cpp utils.h MsgStream.cpp MsgStream.h FileLock.cpp FileLock.h)
target_link_libraries(utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})
install(TARGETS utils DESTINATION lib)
install(FILES utils.h MsgStream.h FileLock.h DESTINATION include/doofit/utils)
