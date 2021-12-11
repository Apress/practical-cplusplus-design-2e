cmake_minimum_required(VERSION 3.16.3)

set(DRIVER_TARGET testPdCalc)

add_executable(${DRIVER_TARGET} main.cpp)
set_target_properties(${DRIVER_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(DRIVER_LIBS pdCalcUtilities
                pdCalcBackend
                pdCalcUi
                pdCalcUiTest
                pdCalcBackendTest
                pdCalcUtilitiesTest
                pdCalcPluginsTest)

find_package(Qt${QT_V} COMPONENTS Core Widgets Test REQUIRED)
link_directories(${CMAKE_BINARY_DIR}/lib)
target_link_libraries(${DRIVER_TARGET} ${DRIVER_LIBS} Qt::Core Qt::Widgets Qt::Test)
