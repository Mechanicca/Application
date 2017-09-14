/*
 * OgreViewer.h
 *
 *  Created on: 19. 7. 2017
 *      Author: martin
 */

#ifndef OGREVIEWER_OGREVIEWER_H_
#define OGREVIEWER_OGREVIEWER_H_

/* Standard library inclusions */
#include <memory>

/* Qt5 inclusions */
#include <QtGui/QWindow>

/* Ogre3D inclusions */
#include <OgreFrameListener.h>

/* Project specific inclusions */
#include "CameraControlProfile.h"

#define DEBUG_CONSOLE_OUTPUT		false

/* Forward declarations */
namespace Ogre
{
	class Root;
	class Camera;
	class RenderWindow;
	class CompositorWorkspace;
}
class QWidget;

class OgreViewer
	:	/* OgreViewer inherits from Qt5 QWindow */
		public QWindow,
		/* Ogre3D Frame listener */
		public Ogre::FrameListener
{
	Q_OBJECT

public:
	explicit OgreViewer( const std::weak_ptr<QWidget> Parent = std::weak_ptr<QWidget>() );

	~OgreViewer( void );

public slots:
	/**
	 * @brief exposeEvent (overrides QObject::eventFilter via QWindow)
	 */
	bool eventFilter( QObject * Target, QEvent * Event ) override;

signals:
	/**
	 * @brief Entity Selection Signal
	 */
	void entitySelected( Ogre::Item * Item );

protected:

	static Ogre::Root * createRoot( const Ogre::String PluginConfigFileName );

	static Ogre::RenderWindow * createRenderWindow( Ogre::Root * Root, QSize Dimensions, WId WinID );

	static Ogre::SceneManager * createSceneManager( Ogre::Root * Root );

	static Ogre::Camera * createCamera( Ogre::SceneManager * SceneManager );

	static Ogre::CompositorWorkspace * createCompositorWorkspace( Ogre::Root * Root, Ogre::SceneManager * SceneManager, Ogre::RenderWindow * RenderWindow, Ogre::Camera * Camera );

	void initializeHlms( void );

	void configureResources( const Ogre::String FileName ) const;

	virtual void keyPressEvent( QKeyEvent * Event ) override;

	virtual void keyReleaseEvent( QKeyEvent * Event ) override;

	virtual void mousePressEvent( QMouseEvent * Event ) override;

	virtual void mouseReleaseEvent( QMouseEvent * Event ) override;

	virtual void mouseMoveEvent( QMouseEvent * Event ) override;

	virtual void wheelEvent( QWheelEvent * Event ) override;

	/**
	 * @brief exposeEvent (overrides QWindow::exposeEvent via Ogre::FrameListener)
	 */
	virtual void exposeEvent( QExposeEvent * Event ) override;

	/**
	 * @brief event (overrides QWindow::event)
	 *
	 * QWindow's "message pump". The base method that handles all
	 * QWindow events. As you will see there are other methods that
	 * actually process the keyboard/other events of Qt and the
	 * underlying OS.
	 */
	virtual bool event( QEvent * Event ) override;

	/**
	 * @brief Camera Target Selection
	 *
	 * Use this method to set the camera target. If an invalid target is about to be set,
	 * no target is set at all and auto tracking is disabled. Once valid target is set
	 * auto tracking is enabled.
	 */
	void setTarget( Ogre::SceneNode * Target );

	/**
	 * @brief Camera position setup
	 *
	 * Set camera position using Yaw, Pitch and Distance parameters.
	 */
	void setCameraYawPitchDistance( Ogre::Radian Yaw, Ogre::Radian Pitch, Ogre::Real Distance );

	/**
	 * @brief No Camera Action
	 *
	 * This method is used to reset all the specific settings of any previous camera action
	 */
	inline void resetCameraAction( void );

	/**
	 * @brief Selection Action
	 *
	 * Scene object selection action. Once the user uses the camera controls as configured
	 * in camera control profile assigned to selection, the coordinates are forwarded.
	 * If any selectable object is selected, Qt signal is emitted.
	 *
	 * @param [in] Coordinates	2D coordinates where the selection has been done
	 */
	inline void selection( QPoint Coordinates );

	/**
	 * @brief Camera Orbit Action
	 *
	 * The camera fixes itself to the target object selected and orbits around using the
	 * configured controls (configured in camera control profile)
	 *
	 * @param [in] Coordinates	2D delta coordinates holding the controls movement (mouse by default)
	 */
	inline void cameraOrbit( QPoint Coordinates );

	/**
	 * @brief Camera Freelook Action
	 *
	 * The camera looks around using the configured controls (configured in camera control profile)
	 *
	 * @param [in] Coordinates	2D delta coordinates holding the controls movement (mouse by default)
	 */
	inline void cameraFreelook( QPoint Coordinates );

	/**
	 * @brief Camera Zoom Action
	 *
	 * The camera zoom using the configured controls (configured in camera control profile)
	 *
	 * @param [in] Coordinates	2D delta coordinates. X coordinate is not used while Y is expected to
	 * 							hold the delta controls movement
	 */
	inline void cameraZoom( QPoint Coordinates );

	/**
	 * @brief Camera Panning Action
	 *
	 * The camera panning using the configured controls (configured in camera control profile)
	 *
	 * @param [in] Coordinates	2D delta coordinates holding the controls movement (mouse by default)
	 */
	inline void cameraPan( QPoint Coordinates );

	/**
	 * @brief frameRenderingQueued (Overrides Ogre::FrameListener::frameRenderingQueued via Ogre::FrameListener)
	 */
	bool frameRenderingQueued( const Ogre::FrameEvent & Event );

	void render( void );

	void createSampleScene( void );

private:
	void setCameraAction( CameraAction Action );

	void doCameraAction( CameraAction Action, QPoint Coordinates );

	Ogre::Root *					mRoot;
	Ogre::RenderWindow *			mRenderWindow;
	Ogre::SceneManager *			mSceneManager;
	Ogre::Camera *					mCamera;
	Ogre::CompositorWorkspace *		mWorkspace;

	CameraAction					mCameraAction;
	CameraControlsState	*			mCurrentCameraControlsState;
	CameraControlProfile * 			mCameraControlProfile;
	Ogre::SceneNode	*				mDefaultTarget;
	Ogre::SceneNode * 				mTarget;

	bool mDoCameraActionReset;

	bool mUpdatePending;
};

#endif /* OGREVIEWER_OGREVIEWER_H_ */
