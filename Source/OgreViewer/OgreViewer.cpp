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
		mRoot( NULL ),
		mCamera( NULL ),
		mWorkspace( NULL ),
		mRenderWindow( NULL ),
		mSceneManager( NULL ),
		mUpdatePending( false )
{
	if( !Parent.expired() )
	{
		std::shared_ptr<QWidget> tParent = Parent.lock();

		this->setWidth( tParent->width() );
		this->setHeight( tParent->height() );
	}

	this->installEventFilter( this );

	this->initialize();

	this->createSampleScene();
}

OgreViewer::~OgreViewer( void )
{
	delete this->mRoot;
}

/* TODO: Move the whole method body directly to the constructor? */
void OgreViewer::initialize( void )
{
	/* TODO: Handle plugins.cfg file better way -> check for existence etc. */

	/* Use plugins.cfg from OGRE installation */
	this->mRoot = new Ogre::Root( Ogre::String( "/usr/local/share/OGRE/plugins.cfg" ) );

	this->initializeRenderWindow();

	/* TODO: Init overlay system here */

	/* TODO: Manage resource.cfg file here */

	/* Initialize High Level Material System */
	this->initializeHlms();

	/* Compositors resource location */
	/* TODO: Change hard coded path here */
	/* TODO: All those paths shall be defined in resources.cfg file */
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "Data/Graphics/Compositors/", "FileSystem", "Popular" );
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "Data/Graphics/Models/", "FileSystem", "Essential" );

	/* Coming from /usr/local/share/OGRE/resources2.cfg */
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "/usr/local/share/OGRE/Media/2.0/scripts/materials/Common", "FileSystem", "General" );
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "/usr/local/share/OGRE/Media/2.0/scripts/materials/Common/GLSL", "FileSystem", "General" );
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "/usr/local/share/OGRE/Media/2.0/scripts/materials/Common/HLSL", "FileSystem", "General" );
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "/usr/local/share/OGRE/Media/2.0/scripts/materials/Common/Metal", "FileSystem", "General" );

	/* TODO: 'true' reason is not known */
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups( true );

	/* TODO: Create Scene manager */
	/* TODO: Change the number of threads handling ( currently it is '1') */
	this->mSceneManager = mRoot->createSceneManager( Ogre::ST_GENERIC, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD, "Mechanicca" );

	/* TODO: Enable once the overlay system is available */
#if false
	this->mSceneManager->addRenderQueueListener( mOverlaySystem );
#endif

	this->mSceneManager->setShadowDirectionalLightExtrusionDistance( 500.0f );
	this->mSceneManager->setShadowFarDistance( 500.0f );

	/* TODO: Create Camera */
	this->mCamera = this->mSceneManager->createCamera( "Main Camera" );
	this->mCamera->setPosition( Ogre::Vector3( 0, 5, 15 ) );
	this->mCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );
	this->mCamera->setNearClipDistance( 0.2f );
	this->mCamera->setFarClipDistance( 1000.0f );
	this->mCamera->setAutoAspectRatio( true );

	/* Setup Workspace
	 * The workspace uses the compositor defined in Mechanicca.compositor file
	 */
	this->mWorkspace = this->mRoot->getCompositorManager2()->addWorkspace( this->mSceneManager, this->mRenderWindow, this->mCamera, "Mechanicca", true );
}

void OgreViewer::initializeRenderWindow( void )
{
	const Ogre::RenderSystemList & tRenderSystemList = this->mRoot->getAvailableRenderers();

	Ogre::RenderSystem * tRenderSystem = tRenderSystemList[0];

	/* Setting size and VSync on windows will solve a lot of problems */
	QString dimensions = QString( "%1 x %2" ).arg( this->width() ).arg( this->height() );
	tRenderSystem->setConfigOption( "Video Mode", dimensions.toStdString() );
	tRenderSystem->setConfigOption( "Full Screen", "No" );
	tRenderSystem->setConfigOption( "VSync", "Yes" );
	mRoot->setRenderSystem( tRenderSystem );
	mRoot->initialise(false);

	Ogre::NameValuePairList tParameters;

	/* Flag within the Parameters set so that Ogre3D initialises an
	 OpenGL context on its own. */
	tParameters["currentGLContext"] = Ogre::String( "false" );

	/* We need to supply the low level OS window handle to this QWindow
	   so that Ogre3D knows where to draw the scene. Below is a
	   cross-platform method on how to do this. If you set both options
	   (externalWindowHandle and parentWindowHandle) this code will work
	   with OpenGL and DirectX. */
	tParameters["externalWindowHandle"] = Ogre::StringConverter::toString( (unsigned long)( this->winId() ) );
	tParameters["parentWindowHandle"] =	Ogre::StringConverter::toString( (unsigned long)( this->winId() ) );

	/* FIXME: unsure but has an effect on background. */
	tParameters.insert( std::make_pair("gamma", "true") );

	this->mRenderWindow = mRoot->createRenderWindow("QT Window", this->width(), this->height(), false, &tParameters );
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
