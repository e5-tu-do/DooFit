#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition
# and dump it in the cache along with proper documentation, otherwise set
# CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options
are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo
MinSizeRel.")
ENDIF()

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


# Project Definition
project(DooFit)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(Config)
add_subdirectory(Fitter)
add_subdirectory(Builder)
add_subdirectory(BuilderSim)
add_subdirectory(PdfToWorkspace)
add_subdirectory(Data)
add_subdirectory(Plotter)
add_subdirectory(ToyFactory)
add_subdirectory(ToyStudy)
add_subdirectory(ExtraRooPdf)

add_subdirectory(Utils)
add_subdirectory(TestWiese)

