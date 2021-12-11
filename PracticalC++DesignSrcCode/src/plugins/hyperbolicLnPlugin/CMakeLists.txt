cmake_minimum_required(VERSION 3.16.3)

# to show that we should not reuse compiled module files for a plugin, we recompile
# any needed module interfaces from the main pdCalc

set(BE_DIR ${PROJECT_SOURCE_DIR}/src/backend)

set(MODULE_INTERFACES
    ${BE_DIR}/Command.m.cpp
    ${BE_DIR}/Plugin.m.cpp
    ${BE_DIR}/StackPluginInterface.m.cpp
    )

set(HYPERBOLIC_LN_PLUGIN_TARGET hyperbolicLnPlugin)

set(HYPERBOLIC_LN_SRC HyperbolicLnPlugin.cpp)

denote_module_interface(${MODULE_INTERFACES})

find_package(Qt${QT_V} COMPONENTS Widgets REQUIRED)

add_library(${HYPERBOLIC_LN_PLUGIN_TARGET} SHARED ${MODULE_INTERFACES} ${HYPERBOLIC_LN_SRC})
set_target_properties(${HYPERBOLIC_LN_PLUGIN_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
target_link_libraries(${HYPERBOLIC_LN_PLUGIN_TARGET} Qt::Widgets)

if(UNIX)
    set(PLUGIN_FILE plugins.pdp.unix)
elseif(WIN32)
    set(PLUGIN_FILE plugins.pdp.win)
    target_link_libraries(${HYPERBOLIC_LN_PLUGIN_TARGET} pdCalcBackend)
endif()

add_custom_command(TARGET ${HYPERBOLIC_LN_PLUGIN_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/src/plugins/${PLUGIN_FILE}
                ${EXECUTABLE_OUTPUT_PATH}/plugins.pdp)
