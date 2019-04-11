file(GLOB SRCS *.cpp)
add_executable(FuelGaugeDemo ${SRCS})

# Extra linking for the project.
target_link_libraries (FuelGaugeDemo LINK_PUBLIC FuelGaugeLib)
