cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" /reference \"pdCalc_utilities.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_backend.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/ui/cli\" /reference \"pdCalc_cli.ifc\" ")
endif()

set(SIMPLE_CLI_TARGET pdCalc-simple-cli)

add_executable(${SIMPLE_CLI_TARGET} main.cpp)
set_target_properties(${SIMPLE_CLI_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(PDCALC_LIBS pdCalcUi
                pdCalcBackend 
                pdCalcUtilities)

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(${SIMPLE_CLI_TARGET} ${PDCALC_LIBS})
