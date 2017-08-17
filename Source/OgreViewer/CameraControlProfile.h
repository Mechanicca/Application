/*
 * CameraControlProfile.h
 *
 *  Created on: 15. 8. 2017
 *      Author: martin
 */

#ifndef OGREVIEWER_CAMERACONTROLPROFILE_H_
#define OGREVIEWER_CAMERACONTROLPROFILE_H_

/* Qt5 inclusions */
#include <QMouseEvent>

/** @enum mapper::CameraAction
 *  @brief is a strongly typed enum class representing the camera action selected
 */
enum class CameraAction : unsigned short
{
	NOTHING,	/**< No camera action is performed */
	SELECTION,	/**< Mouse / Keys are used to select an object in a scene captured by the camera */
	FREELOOK,	/**< Camera free movement / rotations */
	ORBIT,		/**< Camera is looking at the target and allows the rotations around */
	PANNING,	/**< Camera panning */
	ZOOM
};

/* TODO: Becomes obsolete, remove */
enum class ButtonKeyEventType : unsigned short
{
	PRESSED,
	RELEASED
};

class CameraActionProfileItem
{
public:
	CameraActionProfileItem( Qt::MouseButtons MouseButtons, Qt::KeyboardModifiers KeyboardModifiers )
		:	mMouseButtons( MouseButtons ), mKeyboardModifiers( KeyboardModifiers )
	{}

	Qt::MouseButtons 		mMouseButtons;
	Qt::KeyboardModifiers	mKeyboardModifiers;
};

struct CameraActionProfileItemCompare
{
	bool operator()( const CameraActionProfileItem & lhs, const CameraActionProfileItem & rhs ) const
	{
		if( lhs.mMouseButtons < rhs.mMouseButtons )
		{
			return( true );
		}
		else if( lhs.mMouseButtons == rhs.mMouseButtons )
		{
			return( lhs.mKeyboardModifiers < rhs.mKeyboardModifiers );
		}
		else
		{
			return( false );
		}
	}
};

class CameraControlProfile
{
public:
	/* TODO: getAction shall accept the event directly - even if QMouseEvent or QWheelEvent */
	CameraAction getAction( Qt::MouseButtons MouseButtons, Qt::KeyboardModifiers KeyboardModifiers )
	{
		CameraAction tAction = CameraAction::NOTHING;
		CameraActionProfileItem tItem ( MouseButtons, KeyboardModifiers );

		/* Try to search for the desired mouse buttons and modifiers combination */
		/* Once the find() method is equal to the iterator returned by end(), it means the buttons/modifiers combination was not found
		 * in the mapping and thus is being ignored. */
		if( !( this->mActionMapping.find( tItem ) == this->mActionMapping.end() ) )
		{
			/* Mouse buttons / modifiers combination found in the action mapping. Get the assigned action */
			tAction = this->mActionMapping.at( tItem );
		}

		return( tAction );
	}

protected:
	/* Make the constructor protected in order to deny direct instantiation of CameraControlProfile */
	CameraControlProfile( void ) = default;

	virtual ~CameraControlProfile( void ) = default;

	std::map<CameraActionProfileItem, CameraAction, CameraActionProfileItemCompare> mActionMapping;

private:

};

class CADNavigationProfile
	:	public CameraControlProfile
{
public:
	CADNavigationProfile( void )
	{
		using TCameraActionProfileItemPair = std::pair<CameraActionProfileItem, CameraAction>;

		this->mActionMapping.insert( TCameraActionProfileItemPair( CameraActionProfileItem( Qt::RightButton, Qt::NoModifier ), CameraAction::ORBIT ) );
		this->mActionMapping.insert( TCameraActionProfileItemPair( CameraActionProfileItem( Qt::LeftButton, Qt::NoModifier ), CameraAction::SELECTION ) );
		this->mActionMapping.insert( TCameraActionProfileItemPair( CameraActionProfileItem( Qt::MiddleButton, Qt::NoModifier ), CameraAction::PANNING ) );
		this->mActionMapping.insert( TCameraActionProfileItemPair( CameraActionProfileItem( Qt::RightButton, Qt::ControlModifier ), CameraAction::FREELOOK ) );
	}
};

#endif /* OGREVIEWER_CAMERACONTROLPROFILE_H_ */
