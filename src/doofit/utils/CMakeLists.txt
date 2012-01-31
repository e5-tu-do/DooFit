#set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
#set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_library(Utils SHARED utils.cpp utils.h MsgStream.cpp MsgStream.h FileLock.cpp FileLock.h)
target_link_libraries(Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})
install(TARGETS Utils DESTINATION lib)
install(FILES Utils.h MsgStream.h FileLock.h DESTINATION include/doofit/utils)
