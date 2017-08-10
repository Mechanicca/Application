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
#include <QGridLayout>
#include <QVBoxLayout>
#include <QStatusBar>

/* Project specific inclusions */


class MainWindow
{
public:
	MainWindow( const std::weak_ptr<QWidget> ParentWidget = std::weak_ptr<QWidget>() );

	void show( void );

protected:

	MainWindow( void ) = delete;

private:
	using MainWindowWidth = std::integral_constant<int, 850>;
	using MainWindowHeight = std::integral_constant<int, 500>;

	std::shared_ptr<QMainWindow>	mWindow;

	std::unique_ptr<QWidget> 		mViewerWidget;

	std::unique_ptr<QStatusBar>		mStatusBar;
};

#endif /* USERINTERFACE_MAINWINDOW_H_ */
