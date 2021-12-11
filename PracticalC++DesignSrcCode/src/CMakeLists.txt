cmake_minimum_required(VERSION 3.16.3)
include_directories(${PROJECT_SOURCE_DIR}/src)   

add_subdirectory(utilities)
add_subdirectory(backend)
add_subdirectory(ui)
add_subdirectory(app)
add_subdirectory(plugins/hyperbolicLnPlugin)
