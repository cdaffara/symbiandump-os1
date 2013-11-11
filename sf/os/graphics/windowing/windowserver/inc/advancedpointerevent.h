// Copyright (c) 1994-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ADVANCEDPOINTEREVENT_H
#define ADVANCEDPOINTEREVENT_H

#ifndef __E32STD_H__
#include <e32std.h>
#endif
#ifndef __E32KEYS_H__
#include <e32keys.h>
#endif
#ifndef POINTEREVENT_H
#include <pointerevent.h>
#endif


/** Subclass of TPointerEvent created in order to maintain binary compatibility while
extending TPointerEvent with new data:
(1) pointer number of the pointer whose state change is described by the event,
(2) proximity of the pointer to the screen's surface,
(3) pressure applied by the pointer to the screen.

These new attributes will be present only on devices that support them, otherwise
they will be set to 0.

@see TPointerEvent
@see TPointerEvent::AdvancedPointerEvent()
@see TPointerEvent::IsAdvancedPointerEvent()
@see TWsEvent::Pointer()
@publishedAll
@released */
class TAdvancedPointerEvent : public TPointerEvent
	{
public:
	enum
		{
		/**
		The value used by Symbian OS as the default pointer number for pointer-related 
		event objects which do not contain a pointer number member.
		
		WARNING: Enum for internal use ONLY. Compatibility is not guaranteed in future releases.		
		*/
		EDefaultPointerNumber = 0,
		/**
		Maximum number of pointers that WServ is able to support. On each particular platform,
		the number of supported pointers must be less than or equal to this value.
		
		WARNING: Enum for internal use ONLY. Compatibility is not guaranteed in future releases.		
		*/
		EMaximumWServNumberOfPointers = 8
		};
	
	// Public API's for WServ Clients
	inline TAdvancedPointerEvent();	
	inline TInt PointerNumber() const;
	inline TInt Proximity() const;
	inline TInt Pressure() const;
	inline TInt ProximityAndPressure() const;
	inline TPoint3D Position3D() const;
	inline TPoint3D Pressure3D() const;
	inline TPoint3D PositionAndPressure3D() const;

	IMPORT_C TAdvancedPointerEvent(const TAdvancedPointerEvent& aPointerEvent);
	IMPORT_C TAdvancedPointerEvent& operator=(const TAdvancedPointerEvent& aPointerEvent);	
	
private:
	// The methods in this section are internalTechnology and intended for use by TWsEvent 
	// and TAdvancedPointerEventHelper (internal WSERV class).
	friend class TWsEvent;	
	friend class TAdvancedPointerEventHelper;
	
	enum
		{
		/**
		Value used as return value from PointerNumber API when the object is a TPointerEvent
		not a TAdvancedPointerEvent (as indicated by the iModifier flags). 
		 */
		ENonAdvancedPointerPointerNumber = EDefaultPointerNumber,
		};
	enum
		{
		/**
		Value used to distinguish between pressure and proximity Z values by Pressure or
		Proximity Specific APIs. 
		 */			
		EPressureProximityCrossover = 0,
		/**
		Value used as return Z value from APIs when the object is a TPointerEvent
		not a TAdvancedPointerEvent (as indicated by the iModifier flags). 
		 */		
		ENonAdvancedPointerZCoordinate = EPressureProximityCrossover,
		};
	
	// Private API's for external usage, via inlined accessors
	IMPORT_C TInt DoGetPointerNumber() const;
	IMPORT_C TInt DoGetProximity() const;
	IMPORT_C TInt DoGetPressure() const;
	IMPORT_C TInt DoGetProximityAndPressure() const;
	
	// Private API's for internal WSERV usage. It is impossible to inline them into
	// client's binaries, thus implementation may be changed in the future.
	inline void Init();
	inline void Init(const TPointerEvent::TType aType, const TUint aModifiers, const TPoint3D &aPoint3D, const TUint8 aPointerNumber);
	inline void SetPointerNumber(TUint8 aPointerNumber);
	inline void SetPointerZ(TInt aZ);
	inline void Copy(const TAdvancedPointerEvent& aPointerEvent);		
		
	/** WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	union
		{
		struct {TInt iSpare1; TInt iSpare2;} ints; // 8 bytes to fill TWsEvent
		struct {TInt32 iPosZ; TUint8 iPointerNumber;} multiPos3D;
		} iData;
	};

/** Default Constructor for TAdvancedPointerEvent

@publishedAll
@released
*/
inline TAdvancedPointerEvent::TAdvancedPointerEvent() {}

inline const TAdvancedPointerEvent* TPointerEvent::AdvancedPointerEvent() const
/** Casts this TPointerEvent to TAdvancedPointerEvent in order to provide access
	to event's pointer number, pressure and proximity (these attributes are accessible
	through appropriate getters of TAdvancedPointerEvent class).
	
	If this is not an instance of TAdvancedPointerEvent, NULL will be returned.
	Please note that TAdvancedPointerEvents are delivered only to windows for
	which they have been enabled using RWindowBase::EnableAdvancedPointers() method.

	@return A pointer to this object as TAdvancedPointerEvent or NULL if this is not
	        an instance of TAdvancedPointerEvent.
	@see TPointerEvent::IsAdvancedPointerEvent()
	@see RWindowBase::EnableAdvancedPointers()
	@publishedAll
	@released */
	{
	return (iModifiers&EModifierAdvancedPointerEvent ? static_cast<const TAdvancedPointerEvent*>(this) : NULL);	
	}

#include "advancedpointerevent.inl"

#endif /* ADVANCEDPOINTEREVENT_H */
