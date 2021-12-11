cmake_minimum_required(VERSION 3.16.3)

set(UTILITIES_SRC Exception.m.cpp
               Observer.m.cpp
               Publisher.m.cpp
               Tokenizer.m.cpp
               Utilities.m.cpp
               )

denote_module_interface( ${UTILITIES_SRC} )

add_library(pdCalcUtilities SHARED ${UTILITIES_SRC})
set_target_properties(pdCalcUtilities PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
