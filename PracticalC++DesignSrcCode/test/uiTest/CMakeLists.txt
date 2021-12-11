cmake_minimum_required(VERSION 3.16.3)

add_definitions(-DCLI_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set(UI_TEST_TARGET pdCalcUiTest)

set(UI_TEST_SRC
    CliTest.cpp
    DisplayTest.cpp)

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_V} COMPONENTS Widgets Test REQUIRED)

add_library(${UI_TEST_TARGET} SHARED ${UI_TEST_SRC})
target_link_libraries(${UI_TEST_TARGET} Qt::Widgets Qt::Test pdCalcUi pdCalcBackend pdCalcUtilities)
set_target_properties(${UI_TEST_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
