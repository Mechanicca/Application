project( Mechanicca )

add_executable( ${PROJECT_NAME} "" )

# TODO: Fix that nasty hack some other way --> create FindCore.cmake script?
## Core module references
# The root directory where all Core header files are located
include_directories( ${CMAKE_SOURCE_DIR}/Core/Source )
# Some Core module headers are generated during the build
include_directories( ${CMAKE_BINARY_DIR}/Core/Source )

# Add current source directories to include directories list
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include( OgreViewer/CMakeLists.txt )
include( UserInterface/CMakeLists.txt )

target_sources( ${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/Mechanicca.h"
        "${CMAKE_CURRENT_LIST_DIR}/Mechanicca.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
)

target_link_libraries( ${PROJECT_NAME} 
    Core
)