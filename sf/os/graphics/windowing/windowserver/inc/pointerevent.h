// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef POINTEREVENT_H
#define POINTEREVENT_H

class TAdvancedPointerEvent;

/** Pointer event details.

The function TWsEvent::Pointer() is used to get this structure for a pointer
event.

@publishedAll
@released */
struct TPointerEvent
	{
	/** Pointer event types.
	
	Note that the user actions that cause these events will vary according to
	the type of pointing device used. */
	enum TType
		{
		/** Button 1 or pen down. */
		EButton1Down,
		/** Button 1 or pen up. */
		EButton1Up,
		/** Button 2 down.

		This is the middle button of a 3 button mouse. */
		EButton2Down,
		/** Button 2 up.

		This is the middle button of a 3 button mouse. */
		EButton2Up,
		/** Button 3 down. */
		EButton3Down,
		/** Button 3 up. */
		EButton3Up,
		/** Drag event.

		These events are only received when button 1 is down. */
		EDrag,
		/** Move event.

		These events are only received when button 1 is up and the XY input mode is
		not pen. */
		EMove,
		/** Button repeat event. */
		EButtonRepeat,
		/** Switch on event caused by a screen tap. */
		ESwitchOn,
		/** Out Of Range event caused by 3D pointer leaving the detection volume.
		It implies that the pointer cannot be tracked any more by device's sensors
		and that from now the pointer number of this pointer may be used for identifying
		another physical pointer (for example another finger).
		As EOutOfRange event is generated when pointer's position is unknown,
		it contains last known coordinates of the pointer and is delivered to the
		window based on these coordinates.
		@see TAdvancedPointerEvent::PointerNumber()
 		@publishedAll
 		@released */
		EOutOfRange,
		/**	Caused by a 3D pointer getting close to the screen. 
		It is fired when a pointer's Z coordinate crosses the threshold for EEnterCloseProximity 
		from the OutOfRange state or from the Up/OutOfCloseProximity state.
		Please refer to the system documentation for an explanation of pointer states.  
		@see RWsSession::SetCloseProximityThresholds
		@see RWsSession::GetEnterCloseProximityThreshold 
 		@publishedAll
 		@released */
		EEnterCloseProximity,
		/**	Caused by a 3D pointer moving away from the screen. 
		It is generally fired when a pointer's Z coordinate crosses the threshold for EExitCloseProximity 
		from the Up/InCloseProximity state.
		Please refer to the system documentation for an explanation of pointer states.  
		@see RWsSession::SetCloseProximityThresholds
		@see RWsSession::GetExitCloseProximityThreshold 
 		@publishedAll
 		@released */
		EExitCloseProximity,
		/**	Caused by a 3D pointer pressing the screen. 
		It is generally fired when a pointer's Z coordinate crosses the threshold for EEnterHighPressure 
		from the Down/OutOfHighPressure state.
		Please refer to the system documentation for an explanation of pointer states.  
		@see RWsSession::SetHighPressureThresholds
		@see RWsSession::GetEnterHighPressureThreshold 
 		@publishedAll
 		@released */
		EEnterHighPressure,
		/**	Caused by s 3D pointer pressing the screen with reducing force. 
		It is fired when a pointer's Z coordinate crosses the threshold for EExitHighPressure 
		from the Down/InHighPressure state.
		Please refer to the system documentation for an explanation of pointer states.  
		@see RWsSession::SetHighPressureThresholds
		@see RWsSession::GetExitHighPressureThreshold 
 		@publishedAll
 		@released */
		EExitHighPressure,
		/** WSERV will never generate TPointerEvent with this type.
		    
		    WARNING: Enum for internal use ONLY.  Compatibility is not guaranteed in future releases. */
		ENullType = -1
		};

	inline TPointerEvent();
	inline TPointerEvent(const TPointerEvent& aPointerEvent);
	inline TPointerEvent(TType aType, TUint aModifiers, const TPoint &aPosition, const TPoint &aParentPosition);	
	inline TPointerEvent& operator=(const TPointerEvent& aPointerEvent);
	
	inline void Copy(const TPointerEvent& aPointerEvent);
	inline TBool IsAdvancedPointerEvent() const;
	inline const TAdvancedPointerEvent* AdvancedPointerEvent() const;
	/** The type of pointer event. */
	TType iType;
	/** The state of the modifier keys, defined in TEventModifier. */
	TUint iModifiers;	// State of pointing device and associated buttons
	/** Co-ordinates of the pointer event relative to the origin of the window it occurred
	in.

	Positive co-ordinates indicate a position to the right of and down from
	the window's origin, negative co-ordinates indicate a position to the left
	and up. */
	TPoint iPosition;
	/** Co-ordinates of the pointer event relative to the parent window of the window
	it occurred in.

	Positive co-ordinates indicate a position to the right of and down from the window's
	origin, negative co-ordinates indicate a position to the left and up. */
	TPoint iParentPosition;
	};

inline TBool TPointerEvent::IsAdvancedPointerEvent() const
/** Check if this pointer event is an instance of TAdvancedPointerEvent
    containing pointer number, proximity and pressure.
    
    @return ETrue if this pointer event is an instance of TAdvancedPointerEvent;
            EFalse otherwise.
    @see TPointerEvent::AdvancedPointerEvent()
    @see RWindowBase::EnableAdvancedPointers()
 	@publishedAll
 	@released */
	{
	return ((iModifiers&EModifierAdvancedPointerEvent)!=0);
	}

/** Default Constructor

 	@publishedAll
 	@released */
inline TPointerEvent::TPointerEvent() {}

/** Copy Constructor

	@param aPointerEvent the pointer event to copy

 	@publishedAll
 	@released */
inline TPointerEvent::TPointerEvent(const TPointerEvent& aPointerEvent) // Copy Constructor
	{
	Copy(aPointerEvent);
	}

/* TPointerEvent Constructor

	@param aType The pointer event type
	@param aModifiers The event modifiers
	@param aPosition The pointer position
	@param aParentPosition The pointer position relative to the parent window
*/
inline TPointerEvent::TPointerEvent(TType aType, TUint aModifiers, const TPoint &aPosition, const TPoint &aParentPosition)
: iType(aType), iModifiers(aModifiers), iPosition(aPosition), iParentPosition(aParentPosition)  
	{}

/** Operator= override

	@param aPointerEvent the pointer event to copy

 	@publishedAll
 	@released */
inline TPointerEvent& TPointerEvent::operator=(const TPointerEvent& aPointerEvent) // Operator = overload
	{
	if(this != &aPointerEvent)
		{
		Copy(aPointerEvent);
		}
	return *this;
	}

/** Copies from a TPointerEvent object

	@param aPointerEvent the pointer event to copy

 	@publishedAll
 	@released */
inline void TPointerEvent::Copy(const TPointerEvent& aPointerEvent)
	{
	iType=aPointerEvent.iType;
	iModifiers=(aPointerEvent.iModifiers&~EModifierAdvancedPointerEvent); // Clear the advanced pointer flag
	iPosition=aPointerEvent.iPosition;
	iParentPosition=aPointerEvent.iParentPosition;
	}

#endif /* POINTEREVENT_H */
