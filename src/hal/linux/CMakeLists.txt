add_library(uni_lib_hal
    gpio_linux.c
)

target_include_directories(uni_lib_hal
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
