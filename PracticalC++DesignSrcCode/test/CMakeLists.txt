cmake_minimum_required(VERSION 3.16.3)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(utilitiesTest)
add_subdirectory(backendTest)
add_subdirectory(uiTest)
add_subdirectory(pluginsTest)
add_subdirectory(testDriver)
