cmake_minimum_required(VERSION 3.5)

add_library(rasterion STATIC "")

target_sources(rasterion PRIVATE
    rasterion.cpp
    ../include/rasterion.h)

find_package(SDL2 CONFIG REQUIRED)

target_include_directories(rasterion PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
    PRIVATE .)

target_link_libraries(rasterion hq SDL2::SDL2 SDL2::SDL2main)

target_compile_features(rasterion PUBLIC cxx_std_14)

