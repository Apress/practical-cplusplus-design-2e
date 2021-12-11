cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/backend\" ")
endif()


set(UI_MOD_SRC UserInterface.m.cpp
    Cli.m.cpp)

set(UI_SRC
    CommandButton.cpp
                Display.cpp
                GuiModel.cpp
                InputWidget.cpp
                LookAndFeel.cpp
                MainWindow.cpp
                StoredProcedureDialog.cpp)

denote_module_interface(${UI_MOD_SRC})
set(CMAKE_AUTOMOC ON)
# cmake creates moc files in the binary directory, but using private moc files
# requires including it during compilation, so the source must be able to find it
include_directories(${PROJECT_BINARY_DIR}/src/ui)

find_package(Qt${QT_V} COMPONENTS Widgets REQUIRED)

set(UI_TARGET pdCalcUi)

add_library(${UI_TARGET} SHARED ${UI_MOD_SRC} ${UI_SRC})
set_target_properties(${UI_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

target_link_libraries(${UI_TARGET} Qt::Widgets)
if(WIN32)
    target_link_libraries(${UI_TARGET} pdCalcUtilities)
    target_link_libraries(${UI_TARGET} pdCalcBackend)
endif()
