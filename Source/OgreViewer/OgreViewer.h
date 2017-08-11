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

/* Forward declarations */
namespace Ogre
{
	class Root;
	class Camera;
	class RenderWindow;
	class CompositorWorkspace;
}
class QWidget;
class CameraControl;

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
	 * @brief exposeEvent (overrides QObject::eventFilter via QWindow)
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
	 * @brief frameRenderingQueued (Overrides Ogre::FrameListener::frameRenderingQueued via Ogre::FrameListener)
	 */

	bool frameRenderingQueued(const Ogre::FrameEvent & Event);

	void render( void );

	void createSampleScene( void );

private:
	Ogre::Root *					mRoot;
	Ogre::RenderWindow *			mRenderWindow;
	Ogre::SceneManager *			mSceneManager;
	Ogre::Camera *					mCamera;
	Ogre::CompositorWorkspace *		mWorkspace;

	CameraControl * 				mCameraControl;

	bool mUpdatePending;
};

#endif /* OGREVIEWER_OGREVIEWER_H_ */
