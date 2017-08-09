/*
 * Mechanicca.cpp
 *
 *  Created on: 14. 7. 2017
 *      Author: martin
 */

#include "Mechanicca.h"

Mechanicca::Mechanicca( void )
	:	/* Construct application's main window having no parent widget --> give empty weak_ptr */
		mMainWindow( std::make_shared<MainWindow>( std::weak_ptr<QWidget>() ) ),
		/* OgreViewer widget */
		mViewerWidget( std::shared_ptr<QWidget>( QWidget::createWindowContainer( new OgreViewer( mMainWindow->getWidget() ) ) ) )
{
	/* Once the OpenCascade Viewer is ready, set it as central Widget of application's main window */
	mMainWindow->setViewerWidget( mViewerWidget );

	/* Show application's UI */
	mMainWindow->show();
}
