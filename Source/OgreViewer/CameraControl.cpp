/*
 * CameraControl.cpp
 *
 *  Created on: 10. 8. 2017
 *      Author: martin
 */

/* Standard library inclusions */
#include <cmath>
#include <iostream>

/* Ogre3D inclusions */
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreFrameListener.h>

#include "CameraControl.h"

CameraControl::CameraControl( Ogre::Camera * Camera )
	:	mCamera( Camera ),
		mStyle( CameraStyle::ORBIT ),
		mTarget( NULL ),
		mTopSpeed( 150 ),
		mVelocity( Ogre::Vector3::ZERO ),
		mOrbiting( false ),
		mZooming( false ),
		mGoingForward( false ),
		mGoingBack( false ),
		mGoingLeft( false ),
		mGoingRight( false ),
		mGoingUp( false ),
		mGoingDown( false ),
		mFastMove( false )
{}

void CameraControl::setTarget( Ogre::SceneNode * Target )
{
	if( Target != this->mTarget )
	{
		this->mTarget = Target;

		if( Target != NULL )
		{
			/* TODO: Replace hard coded values here. */
			this->setYawPitchDistance( Ogre::Degree( 0 ), Ogre::Degree( 15 ), 15 );
			this->mCamera->setAutoTracking( true, this->mTarget );
		}
		else
		{
			this->mCamera->setAutoTracking( false );
		}
	}
}

Ogre::SceneNode * CameraControl::getTarget( void ) const
{
	return( this->mTarget );
}

void CameraControl::setTopSpeed( Ogre::Real TopSpeed )
{
	this->mTopSpeed = TopSpeed;
}

Ogre::Real CameraControl::getTopSpeed( void ) const
{
	return( this->mTopSpeed );
}

void CameraControl::setStyle( CameraStyle Style )
{
	if( ( this->mStyle != CameraStyle::ORBIT ) && ( Style == CameraStyle::ORBIT ) )
	{
		this->setTarget( ( this->mTarget ) ? this->mTarget : this->mCamera->getSceneManager()->getRootSceneNode() );
		this->mCamera->setFixedYawAxis( true );
		this->manualStop();
		/* TODO: Replace hard coded values here. */
		this->setYawPitchDistance( Ogre::Degree( 0 ), Ogre::Degree( 15 ), 15 );
	}
	else if( ( this->mStyle != CameraStyle::FREELOOK ) && ( Style == CameraStyle::FREELOOK ) )
	{
		this->mCamera->setAutoTracking( false );
		this->mCamera->setFixedYawAxis( true );
	}
	else if( ( this->mStyle != CameraStyle::MANUAL ) && ( Style == CameraStyle::MANUAL ) )
	{
		this->mCamera->setAutoTracking( false );
		this->manualStop();
	}

	this->mStyle = Style;
}

void CameraControl::manualStop( void )
{
    if( this->mStyle == CameraStyle::FREELOOK )
	{
    	this->mGoingForward = false;
    	this->mGoingBack 	= false;
    	this->mGoingLeft 	= false;
    	this->mGoingRight	= false;
    	this->mGoingUp 		= false;
    	this->mGoingDown 	= false;
    	this->mVelocity 	= Ogre::Vector3::ZERO;
	}
}

bool CameraControl::frameRenderingQueued( const Ogre::FrameEvent & Event )
{
	if( this->mStyle == CameraStyle::FREELOOK )
	{
		Ogre::Vector3 tAcceleration = Ogre::Vector3::ZERO;
		if( this->mGoingForward )	tAcceleration += mCamera->getDirection();
		if( this->mGoingBack ) 		tAcceleration -= mCamera->getDirection();
		if( this->mGoingRight ) 	tAcceleration += mCamera->getRight();
		if( this->mGoingLeft ) 		tAcceleration -= mCamera->getRight();
		if( this->mGoingUp ) 		tAcceleration += mCamera->getUp();
		if( this->mGoingDown ) 		tAcceleration -= mCamera->getUp();

		/* TODO: Rework hard coded value */
		Ogre::Real tTopSpeed = this->mFastMove ? this->mTopSpeed * 20 : this->mTopSpeed;

		if( tAcceleration.squaredLength() != 0 )
		{
			tAcceleration.normalise();
			/* TODO: Rework hard coded value */
			this->mVelocity += tAcceleration * tTopSpeed * Event.timeSinceLastFrame * 10;
		}
		else
		{
			/* TODO: Rework hard coded value */
			this->mVelocity -= this->mVelocity * Event.timeSinceLastFrame * 10;
		}

		Ogre::Real tTooSmall = std::numeric_limits<Ogre::Real>::epsilon();

		/* Keep camera velocity below top speed and above epsilon */
		if( this->mVelocity.squaredLength() > std::pow( tTopSpeed, 2 ) )
		{
			this->mVelocity.normalise();
			this->mVelocity *= tTopSpeed;
		}
		else if( this->mVelocity.squaredLength() < std::pow( tTooSmall, 2 ) )
		{
			this->mVelocity = Ogre::Vector3::ZERO;
		}

		if( this->mVelocity != Ogre::Vector3::ZERO )
		{
			this->mCamera->move( mVelocity * Event.timeSinceLastFrame );
		}
	}

	return( true );
}

void CameraControl::setYawPitchDistance( Ogre::Radian Yaw, Ogre::Radian Pitch, Ogre::Real Distance )
{
	this->mCamera->setPosition( this->mTarget->_getDerivedPosition() );
	this->mCamera->setOrientation( this->mTarget->_getDerivedOrientation() );
	this->mCamera->yaw( Yaw );
	this->mCamera->pitch( - Pitch );
	this->mCamera->moveRelative( Ogre::Vector3( 0, 0, Distance ) );
}

/* TODO: Make the Keys configurable and defined on one place to unify keyDown and keyUp events */
void CameraControl::keyDownEvent( const QKeyEvent & Event )
{
	if( this->mStyle == CameraStyle::FREELOOK )
	{
		if( ( Event.key() == Qt::Key_W ) || ( Event.key() == Qt::Key_Up ) ) 		this->mGoingForward = true;
		else if( ( Event.key() == Qt::Key_S ) || ( Event.key() == Qt::Key_Down ) ) 	this->mGoingBack = true;
		else if( ( Event.key() == Qt::Key_A ) || ( Event.key() == Qt::Key_Left ) ) 	this->mGoingLeft = true;
		else if( ( Event.key() == Qt::Key_D ) || ( Event.key() == Qt::Key_Right ) ) this->mGoingRight = true;
		else if( Event.key() == Qt::Key_PageUp ) 	this->mGoingUp = true;
		else if( Event.key() == Qt::Key_PageDown ) 	this->mGoingDown = true;
		else if( Event.key() == Qt::Key_Shift ) 	this->mFastMove = true;
	}
}

/* TODO: Make the Keys configurable and defined on one place to unify keyDown and keyUp events */
void CameraControl::keyUpEvent( const QKeyEvent & Event )
{
	if( this->mStyle == CameraStyle::FREELOOK )
	{
		if( ( Event.key() == Qt::Key_W ) || ( Event.key() == Qt::Key_Up ) ) 		this->mGoingForward = false;
		else if( ( Event.key() == Qt::Key_S ) || ( Event.key() == Qt::Key_Down ) ) 	this->mGoingBack = false;
		else if( ( Event.key() == Qt::Key_A ) || ( Event.key() == Qt::Key_Left ) ) 	this->mGoingLeft = false;
		else if( ( Event.key() == Qt::Key_D ) || ( Event.key() == Qt::Key_Right ) ) this->mGoingRight = false;
		else if( Event.key() == Qt::Key_PageUp ) 	this->mGoingUp = false;
		else if( Event.key() == Qt::Key_PageDown ) 	this->mGoingDown = false;
		else if( Event.key() == Qt::Key_Shift ) 	this->mFastMove = false;
	}
}

void CameraControl::mouseMoveEvent( const int RelX, const int RelY )
{
	if( this->mStyle == CameraStyle::ORBIT )
	{
		Ogre::Real tDistance = ( this->mCamera->getPosition() - this->mTarget->_getDerivedPosition() ).length();

		if( this->mOrbiting )
		{
			this->mCamera->setPosition( this->mTarget->_getDerivedPosition() );
			/* TODO: Rework hard-coded values */
			this->mCamera->yaw( Ogre::Degree( -RelX * 0.25f ) );
			this->mCamera->pitch( Ogre::Degree( -RelY * 0.25f ) );
			this->mCamera->moveRelative( Ogre::Vector3( 0, 0, tDistance ) );
		}
		else if( this->mZooming )
		{
			/* TODO: Rework hard-coded values */
			this->mCamera->moveRelative( Ogre::Vector3( 0, 0, RelY * 0.004f * tDistance ) );
		}
	}
	else if( this->mStyle == CameraStyle::FREELOOK )
	{
		/* TODO: Rework hard-coded values */
		this->mCamera->yaw( Ogre::Degree( -RelX * 0.15f ) );
		this->mCamera->pitch( Ogre::Degree( -RelY * 0.15f ) );
	}
}

void CameraControl::wheelMoveEvent( const QWheelEvent & Event )
{
	int tRelZ = Event.delta();

	if( this->mStyle == CameraStyle::ORBIT )
	{
		Ogre::Real tDistance = ( this->mCamera->getPosition() - this->mTarget->_getDerivedPosition() ).length();

		if( tRelZ != 0 )  // move the camera toward or away from the target
		{
			// the further the camera is, the faster it moves
			/* TODO: Rework hard-coded values */
			mCamera->moveRelative( Ogre::Vector3( 0, 0, -tRelZ * 0.0008f * tDistance ) );
		}
	}
}

void CameraControl::mouseDownEvent( const QMouseEvent & Event )
{
	if( this->mStyle == CameraStyle::ORBIT )
	{
		if( Event.buttons() & Qt::LeftButton ) 		 this->mOrbiting = true;
		else if( Event.buttons() & Qt::RightButton ) this->mZooming = true;
	}
}

void CameraControl::mouseUpEvent( const QMouseEvent & Event )
{
	if( this->mStyle == CameraStyle::ORBIT )
	{
		if( Event.buttons() & Qt::LeftButton ) 		 this->mOrbiting = false;
		else if( Event.buttons() & Qt::RightButton ) this->mZooming = false;
	}
}
