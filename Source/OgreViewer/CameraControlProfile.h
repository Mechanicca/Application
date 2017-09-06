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

/* Project specific inclusions */
#include "Exception.h"

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

enum class ControlsAction : unsigned short
{
	NOTHING,
	MOUSE_MOVE,
	MOUSE_WHEEL,
	MOUSE_BUTTON_PRESSED,
	MOUSE_BUTTON_RELEASED,
	KEY_PRESSED,
	KEY_RELEASED
};

class CameraControlsState
{
public:
	/* Mouse specific camera action profile item constructor */
	CameraControlsState( const ControlsAction controlsAction, const Qt::MouseButtons mouseButtons = Qt::NoButton, const Qt::KeyboardModifiers keyboardModifiers = Qt::NoModifier, const int key = 0 )
		:	mControlsAction( controlsAction ), mMouseButtons( mouseButtons ), mKeyboardModifiers( keyboardModifiers ), mKey( key )
	{}

	CameraControlsState( void )
		:	mControlsAction( ControlsAction::NOTHING ), mMouseButtons( Qt::NoButton ), mKeyboardModifiers( Qt::NoModifier ), mKey( 0 )
	{}

	ControlsAction			mControlsAction;
	Qt::MouseButtons 		mMouseButtons;
	Qt::KeyboardModifiers	mKeyboardModifiers;
	int mKey;
};

class CameraControlsStateCompare
{
public:
	bool operator()( const CameraControlsState & lhs, const CameraControlsState & rhs ) const
	{
		return( this->compare(
					std::pair<ControlsAction, ControlsAction>( lhs.mControlsAction, rhs.mControlsAction ),
					std::pair<Qt::MouseButtons, Qt::MouseButtons>( lhs.mMouseButtons, rhs.mMouseButtons ),
					std::pair<Qt::KeyboardModifiers, Qt::KeyboardModifiers>( lhs.mKeyboardModifiers, rhs.mKeyboardModifiers ),
					std::pair<int, int>( lhs.mKey, rhs.mKey ) )
				);
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
		/* If both pair items are equal, evaluate next pair in Others. If first element is lower than second, return true, false otherwise. */
		return( ( Pair.first == Pair.second ) ? compare( Others... ) : ( Pair.first < Pair.second ) ? true : false );
	}
};

class CameraControlProfile
{
public:
	CameraAction getAction( const CameraControlsState * ControlsState ) const
	{
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

	void add( const CameraAction cameraAction, const ControlsAction controlsAction, const Qt::MouseButtons mouseButtons, const Qt::KeyboardModifiers keyboardModifiers, const int Key = 0 )
	{
		using TCameraControlsStatePair = std::pair<CameraControlsState, CameraAction>;

		if( !( this->mActionMapping.insert( TCameraControlsStatePair( CameraControlsState( controlsAction, mouseButtons, keyboardModifiers, Key ), cameraAction ) ) ).second )
		{
			/* From it's principle, the combination of camera controls (mouse action, mouse buttons, keys...) must be unique.
			 * So during the intialization (insertion of configuration elements), it is checked wheteher the controls configuration is already used.
			 * If so, the insertion would lead to duplicity, an exception is thrown. */
			BOOST_THROW_EXCEPTION( typename Exception::OgreViewer::CameraControlProfileDuplicity() << Core::Exception::Message( "Duplicity found in selected camera control profile." ) );
		}
	}

private:

	std::map<CameraControlsState, CameraAction, CameraControlsStateCompare> mActionMapping;
};

class CADNavigationProfile
	:	public CameraControlProfile
{
public:
	CADNavigationProfile( void )
	{
		this->add( CameraAction::SELECTION, ControlsAction::MOUSE_BUTTON_RELEASED, Qt::LeftButton, Qt::NoModifier );
		this->add( CameraAction::ORBIT, ControlsAction::MOUSE_MOVE, Qt::RightButton, Qt::NoModifier );
		this->add( CameraAction::PANNING, ControlsAction::MOUSE_MOVE, Qt::MiddleButton, Qt::NoModifier );
		this->add( CameraAction::FREELOOK, ControlsAction::MOUSE_MOVE, Qt::RightButton, Qt::NoModifier, Qt::Key_F );
		this->add( CameraAction::ZOOM, ControlsAction::MOUSE_WHEEL, Qt::NoButton, Qt::NoModifier );
	}
};

#endif /* OGREVIEWER_CAMERACONTROLPROFILE_H_ */
