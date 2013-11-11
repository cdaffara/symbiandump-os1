// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// TAdvancedPointerEvent encapsulates additional information to that contained in TPointerEvent.
// For multi-touch, pressure and proximity.
// 
//

#include "W32STD.H"


EXPORT_C TInt TAdvancedPointerEvent::DoGetPointerNumber() const
/** Gets the pointer number for the event
  
  @return TInt. The pointer number for the event
  @internalTechnology */
	{
	return iData.multiPos3D.iPointerNumber;
	}

EXPORT_C TInt TAdvancedPointerEvent::DoGetProximity() const
/** Gets the pointer event proximity
  
  @return TInt.  The pointer event proximity
  @internalTechnology */
	{
	return Min(iData.multiPos3D.iPosZ,EPressureProximityCrossover);
	}

EXPORT_C TInt TAdvancedPointerEvent::DoGetPressure() const
/** Gets the pointer event pressure
  
  @return TInt.  The pointer event pressure
  @internalTechnology */
	{
	return Max(iData.multiPos3D.iPosZ,EPressureProximityCrossover);
	}

EXPORT_C TInt TAdvancedPointerEvent::DoGetProximityAndPressure() const
/** Gets the pointer event combined proximity and pressure.
  
  Negative values are proximity, positive values are pressure.
  
  @return TInt.  The pointer event combined proximity and pressure
  @internalTechnology */
	{
	return iData.multiPos3D.iPosZ;
	}

EXPORT_C TAdvancedPointerEvent::TAdvancedPointerEvent(const TAdvancedPointerEvent& aPointerEvent)
/** Copy Constructor for TAdvancedPointerEvent
  
  @param  const TAdvancedPointerEvent& aPointerEvent.  The event to copy.
  @internalTechnology */
	{
	Copy(aPointerEvent);
	}

EXPORT_C TAdvancedPointerEvent& TAdvancedPointerEvent::operator=(const TAdvancedPointerEvent& aPointerEvent) // Operator = overload
/** Operator= overload for TAdvancedPointerEvent
  
  @param  const TAdvancedPointerEvent& aPointerEvent.  The event to copy.
  @internalTechnology */
	{
	if(this != &aPointerEvent)
		{
		Copy(aPointerEvent);
		}
	return *this;
	}

EXPORT_C void TWsEvent::InitAdvancedPointerEvent(TPointerEvent::TType aType, TUint aModifiers, const TPoint3D &aPoint3D, TUint8 aPointerNumber)
/** Initializes TAdvancedPointerEvent contained in this TWsEvent.
 @param aType type of TAdvancedPointerEvent
 @param aModifiers the state of the modifier keys at the time when the event occured, 
                   defined in TEventModifier
 @param aPoint3D x and y coordinates of the pointer plus z coordinate representing proximity of the pointer
                 combined with its pressure, as returned by TAdvancedPointerEvent::PositionAndPressure3D()
 @param aPointerNumber pointer number of the pointer whose state change is described by this event
 @see TWsEvent::Pointer()
 @see TAdvancedPointerEvent::PositionAndPressure3D()
 @see TAdvancedPointerEvent::PointerNumber()
 @publishedPartner To become publishedAll with WSERV NGA APIs
 @prototype To become released with WSERV NGA APIs */
	{
	Pointer()->Init(aType, aModifiers, aPoint3D, aPointerNumber);
	}

EXPORT_C void TWsEvent::SetPointerNumber(TUint8 aPointerNumber)
/** Changes pointer number of TAdvancedPointerEvent contained in this TWsEvent.
 @param aPointerNumber new pointer number to set in TAdvancedPointerEvent contained in this
                       TWsEvent
 @see TWsEvent::Pointer()
 @see TAdvancedPointerEvent::PointerNumber()
 @publishedPartner To become publishedAll with WSERV NGA APIs
 @prototype To become released with WSERV NGA APIs */
	{
	Pointer()->SetPointerNumber(aPointerNumber);
	}

EXPORT_C void TWsEvent::SetPointerZ(TInt aZ)
/** Changes pressure and proximity of TAdvancedPointerEvent contained in this TWsEvent.
 @param aZ new pressure and proximity to set, combined the same way as returned
           by TAdvancedPointerEvent::ProximityAndPressure()
 @see TWsEvent::Pointer()
 @see TAdvancedPointerEvent::ProximityAndPressure()
 @publishedPartner To become publishedAll with WSERV NGA APIs
 @prototype To become released with WSERV NGA APIs */
	{
	Pointer()->SetPointerZ(aZ);
	}
