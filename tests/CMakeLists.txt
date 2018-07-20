cmake_minimum_required(VERSION 3.5)

add_executable(tests "")

target_sources(tests PRIVATE main.cpp
    tinyOBJLoader.h
    TestInterface.h
    Test5.h)

find_package(SDL2 REQUIRED)

target_include_directories(tests PRIVATE .
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(tests rasterion ${SDL2_LIBRARIES})

target_compile_features(tests PUBLIC cxx_std_14)

add_custom_command(TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/teapot.obj
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/default.mtl
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/default.png
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/diablo3_pose.obj
    $<TARGET_FILE_DIR:tests>/assets
    )
