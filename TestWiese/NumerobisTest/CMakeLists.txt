# Executables
add_executable(NumerobisTest NumerobisTest.cpp)

# Aliases for module libs

set(NUMEROBIS_LIBS Builder_Numerobis)
set(UTILS_LIBS Utils)

# Linker information
target_link_libraries(NumerobisTest ${NUMEROBIS_LIBS} Config ${UTILS_LIBS} ${ROOFIT_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES})