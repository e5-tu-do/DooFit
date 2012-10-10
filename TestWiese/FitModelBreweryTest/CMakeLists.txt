# Executables
add_executable(FitModelBreweryTest FitModelBreweryTest.cpp)

# Aliases for module libs

set(RECIPE_LIBS Brewery_Recipe Brewery_Recipe_Category Brewery_Recipe_Dimension Brewery_Recipe_Parameter)

# Linker information
target_link_libraries(FitModelBreweryTest ${RECIPE_LIBS} ${ALL_LIBRARIES})
