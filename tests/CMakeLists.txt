add_executable(test_gpio test_gpio.c)
target_link_libraries(test_gpio PRIVATE uni_lib_hal)
target_include_directories(test_gpio PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME test_gpio COMMAND test_gpio)
