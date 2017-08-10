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

protected:

	void initialize( void );

	void initializeRenderWindow( void );

	void initializeHlms( void );

	/**
	 * @brief exposeEvent (overrides QWindow::exposeEvent via Ogre::FrameListener)
	 */
	void exposeEvent( QExposeEvent * Event ) override;

	/**
	 * @brief event (overrides QWindow::event)
	 *
	 * QWindow's "message pump". The base method that handles all
	 * QWindow events. As you will see there are other methods that
	 * actually process the keyboard/other events of Qt and the
	 * underlying OS.
	 */
	bool event( QEvent * Event ) override;

	void render( void );

	void createSampleScene( void );

private:
	Ogre::Root *					mRoot;
	Ogre::Camera *					mCamera;
	Ogre::CompositorWorkspace *		mWorkspace;
	Ogre::RenderWindow *			mRenderWindow;

	Ogre::SceneManager *			mSceneManager;

	bool mUpdatePending;
};

#endif /* OGREVIEWER_OGREVIEWER_H_ */
