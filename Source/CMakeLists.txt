project( Mechanicca )

add_executable( ${PROJECT_NAME} "" )

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