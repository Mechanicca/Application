/*
 * CameraControl.h
 *
 *  Created on: 10. 8. 2017
 *      Author: martin
 */

#ifndef SOURCE_OGREVIEWER_CAMERACONTROL_H_
#define SOURCE_OGREVIEWER_CAMERACONTROL_H_

/* Standard library inclusions */

/* Qt5 inclusions */
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>

/* Ogre3D inclusions */
#include <OgrePrerequisites.h>
#include <OgreVector3.h>

/* Forward declarations */
namespace Ogre
{
	class Camera;
	class SceneNode;
}

/* Source: http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Integrating+Ogre+into+QT5 */

class CameraControl
{
public:
	enum class CameraStyle
	{
		FREELOOK,
		ORBIT,
		MANUAL
	};

	CameraControl( Ogre::Camera * Camera );

	void setTarget( Ogre::SceneNode * Target );

	Ogre::SceneNode * getTarget( void ) const;

	void setTopSpeed( Ogre::Real TopSpeed );

	Ogre::Real getTopSpeed( void ) const;

	void setStyle( CameraStyle Style );

	void manualStop( void );

	bool frameRenderingQueued( const Ogre::FrameEvent & Event );

	void setYawPitchDistance( Ogre::Radian Yaw, Ogre::Radian Pitch, Ogre::Real Distance );

	/* TODO: Think whether it would be possible to merge keyDownEvent and keyUpEvent into single method */
	void keyDownEvent( const QKeyEvent & Event );
	void keyUpEvent( const QKeyEvent & Event );
	void mouseMoveEvent( const int RelX, const int RelY );
	void wheelMoveEvent( const QWheelEvent & Event );
	/* TODO: Think whether it would be possible to merge mouseDownEvent and mouseUpEvent into single method */
	void mouseDownEvent( const QMouseEvent & Event );
	void mouseUpEvent( const QMouseEvent & Event );

protected:
	virtual ~CameraControl( void ) = default;

private:
	Ogre::Camera * 		mCamera;
	CameraStyle			mStyle;
	Ogre::SceneNode * 	mTarget;
	Ogre::Real 			mTopSpeed;
	Ogre::Vector3 		mVelocity;
	bool mOrbiting;
	bool mZooming;
	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mGoingUp;
	bool mGoingDown;
	bool mFastMove;
};

#endif /* SOURCE_OGREVIEWER_CAMERACONTROL_H_ */
