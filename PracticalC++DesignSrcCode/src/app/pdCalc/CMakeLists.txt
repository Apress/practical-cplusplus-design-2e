cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" /reference \"pdCalc_utilities.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_backend.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_command.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_commandDispatcher.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_plugins.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_pluginManagement.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/ui/cli\" /reference \"pdCalc_cli.ifc\" ")
endif()

set(PDCALC_TARGET pdCalc)

add_executable(${PDCALC_TARGET} main.cpp)
set_target_properties(${PDCALC_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(PDCALC_LIBS pdCalcUi
                pdCalcBackend 
                pdCalcUtilities)

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(${PDCALC_TARGET} ${PDCALC_LIBS})
