cmake_minimum_required(VERSION 3.16.3)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ifcSearchDir \"${PROJECT_BINARY_DIR}/src/utilities\" ")
endif()

set(BACKEND_TARGET pdCalcBackend)

set(BACKEND_MODULE_INTERFACES
    ${PROJECT_SOURCE_DIR}/src/ui/UserInterface.m.cpp
    Stack.m.cpp
    Command.m.cpp
    CoreCommands.m.cpp
    CommandFactory.m.cpp
    CommandManager.m.cpp
    StoredProcedure.m.cpp
    CommandInterpreter.m.cpp
    AppObservers.m.cpp
    DynamicLoader.m.cpp
    PlatformFactory.m.cpp
    Plugin.m.cpp
    PluginLoader.m.cpp
    StackPluginInterface.m.cpp
    CommandDispatcher.m.cpp
)

set(BACKEND_SRC
    Command.cpp
    StoredProcedure.cpp
    CommandInterpreter.cpp
    PlatformFactory.cpp
    StackPluginInterface.cpp
 )

if(UNIX)
    set(BACKEND_MODULE_INTERFACES ${BACKEND_MODULE_INTERFACES}
        PosixDynamicLoader.m.cpp
        PosixFactory.m.cpp)
    set(BACKEND_SRC ${BACKEND_SRC}
        PosixFactory.cpp
    )
elseif(WIN32)
    set(BACKEND_MODULE_INTERFACES ${BACKEND_MODULE_INTERFACES}
        WindowsDynamicLoader.m.cpp
        WindowsFactory.m.cpp
    )
    set(BACKEND_SRC ${BACKEND_SRC}
        WindowsFactory.cpp
    )
endif()

denote_module_interface(${BACKEND_MODULE_INTERFACES})

add_library(${BACKEND_TARGET} SHARED ${BACKEND_MODULE_INTERFACES} ${BACKEND_SRC})
set_target_properties(${BACKEND_TARGET} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

if(UNIX)
    target_link_libraries(${BACKEND_TARGET} dl)
elseif(WIN32)
    target_link_libraries(${BACKEND_TARGET} pdCalcUtilities)
endif()
