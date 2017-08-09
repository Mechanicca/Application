/*
 * ApplicationUI_MainWindow.h
 *
 *  Created on: 13. 10. 2016
 *      Author: martin
 */

#ifndef USERINTERFACE_MAINWINDOW_H_
#define USERINTERFACE_MAINWINDOW_H_

/* Standard library inclusions */
#include <memory>

/* Qt5 inclusions */
#include <QMainWindow>
#include <QStatusBar>

/* Project specific inclusions */
#include "OgreViewer/OgreViewer.h"

class MainWindow
{
public:
	MainWindow( const std::weak_ptr<QWidget> ParentWidget = std::weak_ptr<QWidget>() );

	void setViewerWidget( const std::weak_ptr<QWidget> ViewerWidget );

	std::weak_ptr<QMainWindow> getWidget( void ) const;

	void show( void );

protected:

	MainWindow( void ) = delete;

private:
	using MainWindowWidth = std::integral_constant<int, 850>;
	using MainWindowHeight = std::integral_constant<int, 500>;

	std::shared_ptr<QMainWindow>	mWindow;

	std::shared_ptr<QStatusBar>		mStatusBar;
};

#endif /* USERINTERFACE_MAINWINDOW_H_ */
