cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" /reference \"pdCalc_utilities.ifc\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" /reference \"pdCalc_backend.ifc\" ")
endif()

set(SIMPLE_GUI_TARGET pdCalc-simple-gui)

if(WIN32)
    add_executable(${SIMPLE_GUI_TARGET} WIN32 main.cpp)
else()
    add_executable(${SIMPLE_GUI_TARGET} WIN32 main.cpp)
endif()

set_target_properties(${SIMPLE_GUI_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})


set(PDCALC_LIBS pdCalcUi
                pdCalcBackend 
                pdCalcUtilities)

link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(${SIMPLE_GUI_TARGET} ${PDCALC_LIBS})
