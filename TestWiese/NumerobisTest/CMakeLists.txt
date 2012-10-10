# Executables
add_executable(NumerobisTest NumerobisTest.cpp)

# Aliases for module libs

set(NUMEROBIS_LIBS Builder_Numerobis Builder_Numerobis_Blueprint)

# Linker information
target_link_libraries(NumerobisTest ${NUMEROBIS_LIBS} Config ${ALL_LIBRARIES})
