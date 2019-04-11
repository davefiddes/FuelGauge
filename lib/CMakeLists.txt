# Compile as a library
add_library(FuelGaugeLib mapper.c)

# Publish the libraries includes
target_include_directories (FuelGaugeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
