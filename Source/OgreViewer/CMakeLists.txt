include_directories( ${OGRE_INCLUDE_DIRS} )

target_sources( ${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/OgreViewer.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/CameraControlProfile.h"
        "${CMAKE_CURRENT_LIST_DIR}/Exception.h"
        "${CMAKE_CURRENT_LIST_DIR}/OgreViewer.cpp"
)