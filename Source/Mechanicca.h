/*
 * Mechanicca.h
 *
 *  Created on: 14. 7. 2017
 *      Author: martin
 */

#ifndef SOURCE_MECHANICCA_H_
#define SOURCE_MECHANICCA_H_

/* Standard library inclusions */
#include <memory>

/* Project specific inclusions */
#include "OgreViewer/OgreViewer.h"
#include "UserInterface/MainWindow.h"

class Mechanicca
{
public:
    Mechanicca( void );

    ~Mechanicca() = default;

private:

    std::shared_ptr<MainWindow>		mMainWindow;

    std::shared_ptr<QWidget> 		mViewerWidget;

};

#endif /* SOURCE_MECHANICCA_H_ */
