/*
 * Main.cpp
 *
 *  Created on: 14. 7. 2017
 *      Author: martin
 */

/* Standard library inclusions */

/* Qt5 inclusions */
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>

/* Project specific inclusions */
#include "Mechanicca.h"
#include "OgreViewer/OgreViewer.h"
#include "UserInterface/MainWindow.h"

int main( int argc, char *argv[] )
{
    QApplication Application( argc, argv );

    Mechanicca w;

    return Application.exec();
}


