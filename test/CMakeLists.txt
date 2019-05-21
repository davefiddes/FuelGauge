# Unit test the library
file(GLOB SRCS *.cpp)
add_executable(FuelGaugeTest ${SRCS})

# Generic Google Test libraries
find_package(Threads REQUIRED)
target_link_libraries(FuelGaugeTest PUBLIC Threads::Threads)
target_link_libraries(FuelGaugeTest PUBLIC gtest)

# Extra linking for the project.
target_link_libraries (FuelGaugeTest PUBLIC FuelGaugeLib)

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable FuelGaugeTest to see those specific tests.
add_test(NAME FuelGaugeTest COMMAND FuelGaugeTest)
