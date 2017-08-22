/*
 * CameraControlProfile.h
 *
 *  Created on: 15. 8. 2017
 *      Author: martin
 */

#ifndef OGREVIEWER_CAMERACONTROLPROFILE_H_
#define OGREVIEWER_CAMERACONTROLPROFILE_H_

/* Standard library inclusions */
#include <iostream>

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


class CameraControlsState
{
public:
	/* Mouse specific camera action profile item constructor */
	CameraControlsState( const Qt::MouseButtons MouseButtons, const Qt::KeyboardModifiers KeyboardModifiers, const int Key = 0 )
		:	mMouseButtons( MouseButtons ), mKeyboardModifiers( KeyboardModifiers ), mKey( Key )
	{}

	Qt::MouseButtons 		mMouseButtons;
	Qt::KeyboardModifiers	mKeyboardModifiers;
	int mKey;
};

class CameraControlsStateCompare
{
public:

	/*
	 * The comparison method must provide strict weak ordering (https://en.wikipedia.org/wiki/Weak_ordering#Strict_weak_orderings)
	 * https://stackoverflow.com/a/24689122/5677080
	 */
	bool operator()( const CameraControlsState & lhs, const CameraControlsState & rhs ) const
	{
#if true
		if( lhs.mMouseButtons < rhs.mMouseButtons )
		{
			return( true );
		}
		else if( lhs.mMouseButtons == rhs.mMouseButtons )
		{
			if( lhs.mKeyboardModifiers < rhs.mKeyboardModifiers )
			{
				return( true );
			}
			else if( lhs.mKeyboardModifiers == rhs.mKeyboardModifiers )
			{
				return( lhs.mKey < rhs.mKey );
			}
			else
			{
				return( false );
			}
		}
		else
		{
			return( false );
		}
#else
		return( this->compare( std::pair<Qt::MouseButtons, Qt::MouseButtons>( lhs.mMouseButtons, rhs.mMouseButtons ), std::pair<Qt::KeyboardModifiers, Qt::KeyboardModifiers>( lhs.mKeyboardModifiers, rhs.mKeyboardModifiers ) ) );
#endif
	}

private:

	/*
	 * The comparison method must provide strict weak ordering (https://en.wikipedia.org/wiki/Weak_ordering#Strict_weak_orderings)
	 * https://stackoverflow.com/a/24689122/5677080
	 */

	template<typename T>
	bool compare( const std::pair<T, T> Pair ) const
	{
		return( Pair.first < Pair.second );
	}

	template<typename T, typename ... OTHER_PAIRS>
	bool compare( const std::pair<T, T> Pair, OTHER_PAIRS... Others ) const
	{
		return( ( Pair.first < Pair.second ) ? true : compare( Others... ) );
	}
};

class CameraControlProfile
{
public:
	CameraAction getAction( const CameraControlsState * ControlsState )
	{
		/* TODO: Remove, temporary debug prints */
#if DEBUG_CONSOLE_OUTPUT
		std::cout << "Camera control action detected. Searching appropriate action..." << std::endl;

		std::cout << "Key(s) = " << ControlsState->mKey << std::endl;
		std::cout << "Modifier(s) = " << ControlsState->mKeyboardModifiers << std::endl;
		std::cout << "Mouse button(s) = " << ControlsState->mMouseButtons << std::endl;
#endif

		CameraAction tAction = CameraAction::NOTHING;

		/* Try to search for the desired mouse buttons and modifiers combination */
		/* Once the find() method is equal to the iterator returned by end(), it means the buttons/modifiers combination was not found
		 * in the mapping and thus is being ignored. */
		if( !( this->mActionMapping.find( (* ControlsState) ) == this->mActionMapping.end() ) )
		{
			/* Mouse buttons / modifiers combination found in the action mapping. Get the assigned action */
			tAction = this->mActionMapping.at( (* ControlsState) );
		}

		return( tAction );
	}

protected:
	/* Make the constructor protected in order to deny direct instantiation of CameraControlProfile */
	CameraControlProfile( void ) = default;

	virtual ~CameraControlProfile( void ) = default;

	std::map<CameraControlsState, CameraAction, CameraControlsStateCompare> mActionMapping;

private:

};

class CADNavigationProfile
	:	public CameraControlProfile
{
public:
	CADNavigationProfile( void )
	{
		using TCameraControlsStatePair = std::pair<CameraControlsState, CameraAction>;

		this->mActionMapping.insert( TCameraControlsStatePair( CameraControlsState( Qt::RightButton, Qt::NoModifier ), CameraAction::ORBIT ) );
		this->mActionMapping.insert( TCameraControlsStatePair( CameraControlsState( Qt::LeftButton, Qt::NoModifier ), CameraAction::SELECTION ) );
		this->mActionMapping.insert( TCameraControlsStatePair( CameraControlsState( Qt::MiddleButton, Qt::NoModifier ), CameraAction::PANNING ) );
		this->mActionMapping.insert( TCameraControlsStatePair( CameraControlsState( Qt::RightButton, Qt::ControlModifier ), CameraAction::FREELOOK ) );
		this->mActionMapping.insert( TCameraControlsStatePair( CameraControlsState( Qt::NoButton, Qt::ControlModifier, Qt::Key_P ), CameraAction::PANNING ) );
	}
};

#endif /* OGREVIEWER_CAMERACONTROLPROFILE_H_ */
