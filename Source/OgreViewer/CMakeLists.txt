include_directories( ${OGRE_INCLUDE_DIRS} )

message( STATUS "Ogre media dir: ${OGRE_MEDIA_DIR}" )
message( STATUS "Ogre binary rel: ${OGRE_BINARY_DBG}" )

target_sources( ${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/OgreViewer.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/OgreViewer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CameraControl.h"
        "${CMAKE_CURRENT_LIST_DIR}/CameraControl.cpp"
)

target_link_libraries( ${PROJECT_NAME} 
    ${OGRE_LIBRARIES}
    OgreHlmsUnlit
    OgreHlmsPbs    
)