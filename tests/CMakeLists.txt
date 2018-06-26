cmake_minimum_required(VERSION 3.5)

add_executable(tests "")

target_sources(tests PRIVATE main.cpp)

find_package(SDL2 REQUIRED)

target_include_directories(tests PRIVATE .
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(tests rasterion ${SDL2_LIBRARIES})

target_compile_features(tests PUBLIC cxx_std_14)

