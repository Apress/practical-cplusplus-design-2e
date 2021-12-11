cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/ui\" ")
endif()

if(UNIX)
    set(BACKEND_TEST_PLUGIN_FILE plugins.unix.pdp)
elseif(WIN32)
        set(BACKEND_TEST_PLUGIN_FILE plugins.win.pdp)
endif()

add_definitions(-DBACKEND_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DPLUGIN_TEST_FILE="${BACKEND_TEST_PLUGIN_FILE}")

set(BACKEND_TEST_TARGET pdCalcBackendTest)

set(TEST_BACKEND_SRC CommandInterpreterTest.cpp
                     CommandFactoryTest.cpp
                     CommandManagerTest.cpp   
                     CoreCommandsTest.cpp  
                     PluginLoaderTest.cpp 
                     StackTest.cpp 
                     StoredProcedureTest.cpp)

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_V} COMPONENTS Test REQUIRED)

add_library(${BACKEND_TEST_TARGET} SHARED ${TEST_BACKEND_SRC})
target_link_libraries(${BACKEND_TEST_TARGET} Qt::Test)
if(WIN32)
    target_link_libraries(${BACKEND_TEST_TARGET} pdCalcUtilities pdCalcBackend)
endif()
set_target_properties(${BACKEND_TEST_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
