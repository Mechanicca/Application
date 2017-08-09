/*
 * ApplicationUI_MainWindow.cpp
 *
 *  Created on: 13. 10. 2016
 *      Author: martin
 */

#include "MainWindow.h"

MainWindow::MainWindow( const std::weak_ptr<QWidget> ParentWidget )
	:	/* Construct main windows first */
		mWindow( std::make_shared<QMainWindow>( ( !ParentWidget.expired() ) ? ( ParentWidget.lock() ).get() : nullptr ) ),
		/* and then it's status bar */
		mStatusBar( std::make_shared<QStatusBar>( mWindow.get() ) )
{
	mWindow->setObjectName( QStringLiteral( "Name of main window" ) );
	mWindow->setWindowTitle( QStringLiteral( "Mechanicca" ) );
	mWindow->resize( MainWindowWidth::value, MainWindowHeight::value );
	mWindow->setStatusBar( mStatusBar.get() );

	QMetaObject::connectSlotsByName( mWindow.get() );
}

void MainWindow::setViewerWidget( const std::weak_ptr<QWidget> ViewerWidget )
{
	if( !ViewerWidget.expired() )
		mWindow->setCentralWidget( ( ViewerWidget.lock() ).get() );
	else
		/* TODO: Throw an exception here */
		;
}

std::weak_ptr<QMainWindow> MainWindow::getWidget( void ) const
{
	return( mWindow );
}

void MainWindow::show( void )
{
	mWindow->show();
}
