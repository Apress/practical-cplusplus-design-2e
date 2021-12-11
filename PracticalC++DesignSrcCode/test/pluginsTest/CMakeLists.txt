cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" ")
endif()

if(UNIX)
    set(PLUGIN_TEST_PLUGIN_FILE plugins.unix.pdp)
elseif(WIN32)
        set(PLUGIN_TEST_PLUGIN_FILE plugins.win.pdp)
endif()

add_definitions(-DPLUGIN_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DPLUGIN_TEST_FILE="${PLUGIN_TEST_PLUGIN_FILE}")

set(PLUGIN_TEST_TARGET pdCalcPluginsTest)

set(TEST_PLUGIN_SRC HyperbolicLnPluginTest.cpp)

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_V} COMPONENTS Test REQUIRED)

add_library(${PLUGIN_TEST_TARGET} SHARED ${TEST_PLUGIN_SRC})
target_link_libraries(${PLUGIN_TEST_TARGET} Qt::Test)
if(WIN32)
    target_link_libraries(${PLUGIN_TEST_TARGET} pdCalcUtilities pdCalcBackend)
endif()
set_target_properties(${PLUGIN_TEST_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
