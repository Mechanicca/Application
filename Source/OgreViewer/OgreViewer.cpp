/*
 * OgreViewer.cpp
 *
 *  Created on: 19. 7. 2017
 *      Author: martin
 */

/* Standard library inclusions */
#include <iostream>

/* Qt5 inclusions */
#include <QApplication>
#include <QWidget>

/* Ogre3D inclusions */
#include <OgrePrerequisites.h>
#include <OgreFrameListener.h>
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreRenderWindow.h>
#include <OgreItem.h>
#include <OgreConfigFile.h>

#include <OgreMatrix4.h>
#include <OgreVector2.h>
#include <OgreVector3.h>

#include <OgreArchiveManager.h>

#include <OgreWindowEventUtilities.h>

#include <Hlms/Pbs/OgreHlmsPbs.h>
#include <Hlms/Unlit/OgreHlmsUnlit.h>
#include <Hlms/Unlit/OgreHlmsUnlitDatablock.h>
#include <OgreHlmsManager.h>

#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <Compositor/OgreCompositorShadowNode.h>

/* Project specific inclusions */
#include "OgreViewer.h"

OgreViewer::OgreViewer( const std::weak_ptr<QWidget> Parent )
/* FIXME: The parent window is QWidget, not QWindow. It is unknown how to convert the two.
 * The solution is to setup required properties manualy here, e.g. width, height...
 */
#if false
	:	/* QWindow( ( !Parent.expired() ) ? ( Parent.lock() ).get() : NULL ), */
#else
	:	QWindow(),
#endif
		mRoot( nullptr ),
		mRenderWindow( nullptr ),
		mSceneManager( nullptr ),
		mCamera( nullptr ),
		mWorkspace( nullptr ),
		/* Default camera action is to do nothing */
		mCameraAction( CameraAction::NOTHING ),
		/* Current camera control state */
		mCurrentCameraControlsState( new CameraControlsState() ),
		/* Create camera control profile to implement camera action to mouse buttons and its modifiers mapping */
		mCameraControlProfile( new CADNavigationProfile() ),
		mDefaultTarget( nullptr ),
		mTarget( nullptr ),
		mUpdatePending( false )
{
	if( !Parent.expired() )
	{
		std::shared_ptr<QWidget> tParent = Parent.lock();

		this->setWidth( tParent->width() );
		this->setHeight( tParent->height() );
	}

	this->installEventFilter( this );

	/* Create Ogre root */
	this->mRoot = createRoot( "Data/Graphics/plugins.cfg" );

	this->mRenderWindow = createRenderWindow( this->mRoot, QSize( this->width(), this->height() ), this->winId() );

	/* TODO: Init overlay system here */

	/* Create Scene manager */
	this->mSceneManager = createSceneManager( this->mRoot );

	/* Create Camera */
	this->mCamera = createCamera( this->mSceneManager );

	this->mDefaultTarget = this->mSceneManager->getRootSceneNode();

	/* Setup viewer target */
	this->setTarget( this->mDefaultTarget );

	/* Initialize High Level Material System */
	this->initializeHlms();

	this->configureResources( "Data/Graphics/resources.cfg" );

	/* Setup Workspace */
	this->mWorkspace = createCompositorWorkspace( this->mRoot, this->mSceneManager, this->mRenderWindow, this->mCamera );

	this->createSampleScene();
}

OgreViewer::~OgreViewer( void )
{
	delete this->mRoot;
}

Ogre::Root * OgreViewer::createRoot( const Ogre::String PluginConfigFileName )
{
	/* TODO: Handle plugins.cfg file better way -> check for existence etc. */
	return( new Ogre::Root( PluginConfigFileName ) );
}

Ogre::RenderWindow * OgreViewer::createRenderWindow( Ogre::Root * Root, QSize Dimensions, WId WinID )
{
	const Ogre::RenderSystemList & tRenderSystemList = Root->getAvailableRenderers();

	Ogre::RenderSystem * tRenderSystem = tRenderSystemList[0];

	/* Setting size and VSync on windows will solve a lot of problems */
	tRenderSystem->setConfigOption( "Video Mode", QString( "%1 x %2" ).arg( Dimensions.width() ).arg( Dimensions.height() ).toStdString() );
	tRenderSystem->setConfigOption( "Full Screen", "No" );
	tRenderSystem->setConfigOption( "VSync", "Yes" );
	Root->setRenderSystem( tRenderSystem );
	Root->initialise(false);

	Ogre::NameValuePairList tParameters;

	/* Flag within the Parameters set so that Ogre3D initialises an
	 OpenGL context on its own. */
	tParameters["currentGLContext"] = Ogre::String( "false" );

	/* We need to supply the low level OS window handle to this QWindow
	   so that Ogre3D knows where to draw the scene. Below is a
	   cross-platform method on how to do this. If you set both options
	   (externalWindowHandle and parentWindowHandle) this code will work
	   with OpenGL and DirectX. */
	tParameters["externalWindowHandle"] = Ogre::StringConverter::toString( (unsigned long)( WinID ) );
	tParameters["parentWindowHandle"] =	Ogre::StringConverter::toString( (unsigned long)( WinID ) );

	/* FIXME: unsure but has an effect on background. */
	tParameters.insert( std::make_pair("gamma", "true") );

	return( Root->createRenderWindow("QT Window", Dimensions.width(), Dimensions.height(), false, &tParameters ) );
}

Ogre::SceneManager * OgreViewer::createSceneManager( Ogre::Root * Root )
{
	Ogre::SceneManager * tSceneManager = Root->createSceneManager( Ogre::ST_GENERIC, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD, "Mechanicca" );
	/* TODO: Rework hard-coded values */
	tSceneManager->setShadowDirectionalLightExtrusionDistance( 500.0f );
	tSceneManager->setShadowFarDistance( 500.0f );

	/* TODO: Enable once the overlay system is available */
#if false
	tSceneManager->addRenderQueueListener( OverlaySystem );
#endif

	return( tSceneManager );
}

Ogre::Camera * OgreViewer::createCamera( Ogre::SceneManager * SceneManager )
{
	Ogre::Camera * tCamera = SceneManager->createCamera( "Main Camera" );
	tCamera->setPosition( Ogre::Vector3( 0, 5, 15 ) );
	tCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );
	tCamera->setNearClipDistance( 0.2f );
	tCamera->setFarClipDistance( 1000.0f );
	tCamera->setAutoAspectRatio( true );

	return( tCamera );
}

Ogre::CompositorWorkspace * OgreViewer::createCompositorWorkspace( Ogre::Root * Root, Ogre::SceneManager * SceneManager, Ogre::RenderWindow * RenderWindow, Ogre::Camera * Camera )
{
	/* The workspace uses the compositor defined in Mechanicca.compositor file */
	return( Root->getCompositorManager2()->addWorkspace( SceneManager, RenderWindow, Camera, "Mechanicca", true ) );
}

void OgreViewer::initializeHlms( void )
{
	/* Register High Level Material System (HLMS) resources */
	/* TODO: Change the hard coded paths */
	Ogre::Archive * ArchiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load( "/usr/local/share/OGRE/Media/Hlms/Common/GLSL", "FileSystem", true );
	Ogre::Archive * ArchivePbs = Ogre::ArchiveManager::getSingletonPtr()->load( "/usr/local/share/OGRE/Media/Hlms/Pbs/GLSL", "FileSystem", true );
	Ogre::Archive * ArchiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load( "/usr/local/share/OGRE/Media/Hlms/Unlit/GLSL", "FileSystem", true );

	Ogre::Archive * ArchiveLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load( "/usr/local/share/OGRE/Media/Hlms/Common/Any", "FileSystem", true );
    Ogre::Archive * ArchivePbsLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load( "/usr/local/share/OGRE/Media/Hlms/Pbs/Any", "FileSystem", true );
    Ogre::Archive * ArchiveUnlitLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load( "/usr/local/share/OGRE/Media/Hlms/Unlit/Any", "FileSystem", true );

	Ogre::ArchiveVec library;

	library.push_back( ArchiveLibrary );
	library.push_back( ArchiveLibraryAny );
	library.push_back( ArchivePbsLibraryAny );
	library.push_back( ArchiveUnlitLibraryAny );


	Ogre::HlmsPbs * hlmsPbs = OGRE_NEW Ogre::HlmsPbs( ArchivePbs, &library );
	Ogre::HlmsUnlit * hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit( ArchiveUnlit, &library );

	Ogre::Root::getSingleton().getHlmsManager()->registerHlms( hlmsPbs );
	Ogre::Root::getSingleton().getHlmsManager()->registerHlms( hlmsUnlit );
}

void OgreViewer::configureResources( const Ogre::String ResourcesFileName ) const
{
	Ogre::ConfigFile tResourcesConfigFile;

	tResourcesConfigFile.load( ResourcesFileName );

	Ogre::ConfigFile::SectionIterator tSectionIterator = tResourcesConfigFile.getSectionIterator();
	Ogre::String tSectionName, tTypeName, tArchiveName;

	while( tSectionIterator.hasMoreElements() )
	{
		tSectionName = tSectionIterator.peekNextKey();

		Ogre::ConfigFile::SettingsMultiMap * tSettings = tSectionIterator.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = tSettings->begin(); i != tSettings->end(); ++i)
		{
			tTypeName = i->first;
			tArchiveName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation( tArchiveName, tTypeName, tSectionName );
		}
	}

	/* TODO: 'true' reason is not known */
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups( true );
}

void OgreViewer::setCameraAction( CameraAction Action )
{
	/* Set the camera specific to requested Action */
	if( ( this->mCameraAction != CameraAction::ORBIT ) && ( Action == CameraAction::ORBIT ) )
	{
		this->setTarget( ( this->mTarget ) ? this->mTarget : this->mDefaultTarget );
		this->mCamera->setFixedYawAxis( true );
	}
	else if( ( this->mCameraAction != CameraAction::FREELOOK ) && ( Action == CameraAction::FREELOOK ) )
	{
		this->mCamera->setAutoTracking( false );
		this->mCamera->setFixedYawAxis( true );
	}

	/* Finally, select the camera action */
	this->mCameraAction = Action;


	/* TODO: Remove, temporary debug prints */
#if DEBUG_CONSOLE_OUTPUT
	switch( this->mCameraAction )
	{
		case CameraAction::NOTHING : std::cout << "CameraAction = NOTHING" << std::endl; break;
		case CameraAction::SELECTION : std::cout << "CameraAction = SELECTION" << std::endl; break;
		case CameraAction::FREELOOK : std::cout << "CameraAction = FREELOOK" << std::endl; break;
		case CameraAction::ORBIT : std::cout << "CameraAction = ORBIT" << std::endl; break;
		case CameraAction::PANNING : std::cout << "CameraAction = PANNING" << std::endl; break;
		case CameraAction::ZOOM : std::cout << "CameraAction = ZOOM" << std::endl; break;
	}
#endif
}

void OgreViewer::keyPressEvent( QKeyEvent * Event )
{
	this->mCurrentCameraControlsState->mKey = Event->key();

	this->setCameraAction( this->mCameraControlProfile->getAction( this->mCurrentCameraControlsState ) );
}

void OgreViewer::keyReleaseEvent( QKeyEvent * Event )
{
	/* Once the key is released, no key is logically pressed */
	this->mCurrentCameraControlsState->mKey = 0;

	/* Camera action setup makes sense even if the key is released as there might be valid combination
	 * of controls after the key release. */
	this->setCameraAction( this->mCameraControlProfile->getAction( this->mCurrentCameraControlsState ) );
}

void OgreViewer::mousePressEvent( QMouseEvent * Event )
{
	this->mCurrentCameraControlsState->mMouseButtons = Event->buttons();

	this->setCameraAction( this->mCameraControlProfile->getAction( this->mCurrentCameraControlsState ) );
}

void OgreViewer::mouseReleaseEvent( QMouseEvent * Event )
{
	/* Once the selection action button / modifiers combination is released, perform the selection itself */
	if( this->mCameraAction == CameraAction::SELECTION )
	{
		this->selection( Event );
	}

	this->mCurrentCameraControlsState->mMouseButtons = Event->buttons();

	this->setCameraAction( this->mCameraControlProfile->getAction( this->mCurrentCameraControlsState ) );
}

void OgreViewer::mouseMoveEvent( QMouseEvent * Event )
{
	static int lastX = Event->x();
	static int lastY = Event->y();
	int relX = Event->x() - lastX;
	int relY = Event->y() - lastY;
	lastX = Event->x();
	lastY = Event->y();

	switch( this->mCameraAction )
	{
		case CameraAction::ORBIT : 		this->cameraOrbit( relX, relY ); break;
		case CameraAction::FREELOOK : 	this->cameraFreelook( relX, relY ); break;
		case CameraAction::ZOOM :		this->cameraZoom( - relY ); break;
		case CameraAction::PANNING :	this->cameraPan( relX, relY ); break;
		default : break;
	}
}

/* TODO: Make the wheelEvent configurable as well. It must not serve for zooming only */
void OgreViewer::wheelEvent( QWheelEvent * Event )
{
#if false
	this->setCameraAction( this->mCameraControlProfile->getAction( this->mCurrentCameraControlsState ) );
#endif

	switch( this->mCameraAction )
	{
		case CameraAction::ZOOM : 	this->cameraZoom( Event->delta() ); break;
		default : break;
	}
}

bool OgreViewer::eventFilter( QObject * Target, QEvent * Event )
{
	if( ( Target == this ) && ( Event->type() == QEvent::Resize ) && ( this->isExposed() ) && this->mRenderWindow != nullptr )
	{
		this->mRenderWindow->resize( this->width(), this->height() );
	}

	return false;
}

void OgreViewer::exposeEvent( QExposeEvent * Event )
{
	/* Suppress compiler warning about unused `event` */
	Q_UNUSED( Event );

	if( this->isExposed() )
	{
		this->render();
	}
}

bool OgreViewer::event( QEvent * Event )
{
	switch( Event->type() )
	{
		case QEvent::UpdateRequest:
		{
			mUpdatePending = false;

			this->render();

			return( true );
		}
		break;

		default:
			/* Forward unprocessed events */
			return( QWindow::event( Event ) );
	}
}

void OgreViewer::setTarget( Ogre::SceneNode * Target )
{
	/* If Target is a valid pointer to Ogre::SceneNode to look at */
	if( Target )
	{
		/* If mTarget is not valid - means not yet set */
		if( !( this->mTarget ) )
		{
			/* Set the Target */
			this->mTarget = Target;
		}
		/* mTarget is already set valid */
		else
		{
			/* New target is different to the one already set */
			if( Target != this->mTarget )
			{
				this->mTarget = Target;

				/* TODO: Replace hard coded values here. */
				this->setCameraYawPitchDistance( Ogre::Degree( 0 ), Ogre::Degree( 15 ), 15 );

				this->mCamera->setAutoTracking( true, this->mTarget );
			}
		}
	}
	else
	{
		this->mCamera->setAutoTracking( false );
	}
}

void OgreViewer::setCameraYawPitchDistance( Ogre::Radian Yaw, Ogre::Radian Pitch, Ogre::Real Distance )
{
	this->mCamera->setPosition( this->mTarget->_getDerivedPosition() );
	this->mCamera->setOrientation( this->mTarget->_getDerivedOrientation() );
	this->mCamera->yaw( Yaw );
	this->mCamera->pitch( - Pitch );
	this->mCamera->moveRelative( Ogre::Vector3( 0, 0, Distance ) );
}

void OgreViewer::selection( QMouseEvent * Event )
{
	QPoint Position = Event->pos();

	Ogre::Ray tMouseRay = this->mCamera->getCameraToViewportRay( (Ogre::Real)Position.x() / this->mRenderWindow->getWidth(), (Ogre::Real)Position.y() / this->mRenderWindow->getHeight()	);

	Ogre::RaySceneQuery * tSceneQuery = this->mSceneManager->createRayQuery( tMouseRay );
	tSceneQuery->setSortByDistance( true );

	Ogre::RaySceneQueryResult tQueryResult = tSceneQuery->execute();

	for( size_t ui = 0; ui < tQueryResult.size(); ui++ )
	{
		if( tQueryResult[ui].movable )
		{
			std::string tMovableType = tQueryResult[ui].movable->getMovableType();

			if( tMovableType.compare( "Item" ) == 0 )
			{
				std::cout << "Entity selected." << std::endl;

				/* TODO: Use static cast instead of explicit conversion */
				emit entitySelected( ( Ogre::Item * ) tQueryResult[ui].movable );
			}
		}
	}

	this->mSceneManager->destroyQuery( tSceneQuery );
}

void OgreViewer::cameraOrbit( const int RelX, const int RelY )
{
	Ogre::Real tDistance = ( this->mCamera->getPosition() - this->mTarget->_getDerivedPosition() ).length();

	this->mCamera->setPosition( this->mTarget->_getDerivedPosition() );
	/* TODO: Rework hard-coded values */
	this->mCamera->yaw( Ogre::Degree( -RelX * 0.25f ) );
	this->mCamera->pitch( Ogre::Degree( -RelY * 0.25f ) );
	this->mCamera->moveRelative( Ogre::Vector3( 0, 0, tDistance ) );
}

void OgreViewer::cameraFreelook( const int RelX, const int RelY )
{
	/* TODO: Rework hard-coded values */
	this->mCamera->yaw( Ogre::Degree( -RelX * 0.15f ) );
	this->mCamera->pitch( Ogre::Degree( -RelY * 0.15f ) );
}

void OgreViewer::cameraZoom( const int RelZ )
{
	Ogre::Real tDistance = ( this->mCamera->getPosition() - this->mTarget->_getDerivedPosition() ).length();

	if( RelZ != 0 )  // move the camera toward or away from the target
	{
		// the further the camera is, the faster it moves
		/* TODO: Rework hard-coded values */
		mCamera->moveRelative( Ogre::Vector3( 0, 0, -RelZ * 0.0008f * tDistance ) );
	}
}

void OgreViewer::cameraPan( const int RelX, const int RelY )
{
	/* TODO: Rework hard-coded values */
	this->mCamera->moveRelative( Ogre::Vector3( - RelX * 0.025, RelY  * 0.025, 0.0f ) );
}

bool OgreViewer::frameRenderingQueued(const Ogre::FrameEvent & Event)
{
#if false
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
#endif

	return( true );
}

void OgreViewer::render( void )
{
	/* Do nothing if the window is invisible */
	if ( !this->isExposed() ) return;

	Ogre::WindowEventUtilities::messagePump();

	this->mRoot->renderOneFrame();

	/* TODO: Implement the rest here */

	/* Handle update pending flag. This must be done at the end of render() method */
	if( !this->mUpdatePending )
	{
		this->mUpdatePending = true;
	    QApplication::postEvent( this, new QEvent( QEvent::UpdateRequest ) );
	}
}

void OgreViewer::createSampleScene( void )
{
	/* Create light */
	Ogre::Light * Light = this->mSceneManager->createLight();

	Ogre::SceneNode * LightNode = this->mSceneManager->getRootSceneNode()->createChildSceneNode();
	LightNode->attachObject( Light );
	Light->setPowerScale( 1.0f );
	Light->setType( Ogre::Light::LT_DIRECTIONAL );
	Light->setDirection( Ogre::Vector3( 1.0f, -1.0f, -0.6f ).normalisedCopy() );

	/* Create model to display */
	Ogre::Item * Item = this->mSceneManager->createItem( "boostercube.mesh", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC );
	Ogre::SceneNode * Node = this->mSceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->createChildSceneNode( Ogre::SCENE_DYNAMIC );
	Node->setPosition( 0.0f, 0.0f, 0.0f );
	Node->setScale( 1.0f, 1.0f, 1.0f );
	Node->attachObject( Item );
}
