/*
 * Mechanicca.cpp
 *
 *  Created on: 14. 7. 2017
 *      Author: martin
 */

#include "Mechanicca.h"

Mechanicca::Mechanicca( void )
	:	/* Construct application's main window having no parent widget --> give empty weak_ptr */
		mMainWindow( std::make_unique<MainWindow>( std::weak_ptr<QWidget>() ) ) //,
{
	/* Show application's UI */
	mMainWindow->show();
}
