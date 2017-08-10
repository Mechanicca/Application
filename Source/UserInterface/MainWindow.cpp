/*
 * ApplicationUI_MainWindow.cpp
 *
 *  Created on: 13. 10. 2016
 *      Author: martin
 */

/* Project specific inclusions */
#include "OgreViewer/OgreViewer.h"
#include "MainWindow.h"

MainWindow::MainWindow( const std::weak_ptr<QWidget> ParentWidget )
	:	/* Construct main windows first */
		mWindow( std::make_shared<QMainWindow>( ( !ParentWidget.expired() ) ? ( ParentWidget.lock() ).get() : nullptr ) ),
		/* and then it's status bar */
		mStatusBar( std::make_unique<QStatusBar>( mWindow.get() ) )
{
	/* Setup main window name and title */
	this->mWindow->setObjectName( QStringLiteral( "Mechanicca Main Window" ) );
	this->mWindow->setWindowTitle( QStringLiteral( "Mechanicca" ) );

	/* Resize the window */
	this->mWindow->resize( MainWindowWidth::value, MainWindowHeight::value );

	/* Setup status bar constructed previously */
	this->mWindow->setStatusBar( this->mStatusBar.get() );

	/* Connect all the Qt slots by name */
	QMetaObject::connectSlotsByName( this->mWindow.get() );

	/* OgreViewer widget */
	this->mViewerWidget = std::move( std::unique_ptr<QWidget>( QWidget::createWindowContainer( new OgreViewer( this->mWindow ) ) ) );

	this->mWindow->setCentralWidget( this->mViewerWidget.get() );
}

void MainWindow::show( void )
{
	mWindow->show();
}
