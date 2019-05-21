# Unit test the library
enable_testing()

file(GLOB SRCS *.cpp)
add_executable(FuelGaugeTest ${SRCS})

# Generic Google Test libraries
find_package(Threads)
target_link_libraries(FuelGaugeTest LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(FuelGaugeTest LINK_PUBLIC gtest)
target_link_libraries(FuelGaugeTest LINK_PUBLIC gtest_main)

# Extra linking for the project.
target_link_libraries (FuelGaugeTest LINK_PUBLIC FuelGaugeLib)

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable FuelGaugeTest to see those specific tests.
add_test(NAME FuelGaugeTest COMMAND FuelGaugeTest)
