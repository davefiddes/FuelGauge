# Compile as a library
file( GLOB SRCS *.c )
add_library(FuelGaugeLib ${SRCS})

# Publish the libraries includes
target_include_directories (FuelGaugeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
