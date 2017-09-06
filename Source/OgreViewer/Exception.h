/*
 * Exception.h
 *
 *  Created on: 6. 9. 2017
 *      Author: martin
 */

#ifndef SOURCE_OGREVIEWER_EXCEPTION_H_
#define SOURCE_OGREVIEWER_EXCEPTION_H_

/* Project specific inclusions */
#include "Exception/Exception.h"

namespace Exception
{
	namespace OgreViewer
	{
		struct CameraControlProfileDuplicity : virtual Core::Exception::Generic {};
	}
}

#endif /* SOURCE_OGREVIEWER_EXCEPTION_H_ */
