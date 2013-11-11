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
// inline implementations for TAdvancedPointerEvent
// 
//

#ifndef ADVANCEDPOINTEREVENT_INL_
#define ADVANCEDPOINTEREVENT_INL_


/**	Does basic initialisation of a TAdvancedPointerEvent
	@internalTechnology */
inline void TAdvancedPointerEvent::Init()
	{
	iModifiers=EModifierAdvancedPointerEvent;
	iData.ints.iSpare1=0;
	iData.ints.iSpare2=0;
	}

/**	Does specific initialisation of a TAdvancedPointerEvent
	@param aType The pointer event type.
	@param aModifiers Additional modifiers for the event.
	@param aPoint3D The 3D coordinated of the event.
	@param aPointerNumber The pointer number for the event.
	@internalTechnology */
inline void TAdvancedPointerEvent::Init(const TPointerEvent::TType aType, const TUint aModifiers, const TPoint3D &aPoint3D, const TUint8 aPointerNumber)
	{
	Init();
	iType=aType;
	iModifiers|=aModifiers; //add the new modifiers to EModifierAdvancedPointerEvent
	iPosition.iX=aPoint3D.iX;
	iPosition.iY=aPoint3D.iY;
	iData.multiPos3D.iPosZ=aPoint3D.iZ;
	iData.multiPos3D.iPointerNumber=aPointerNumber;
	}

/** Sets the pointer number and enforces pointer event to an advanced pointer event.
	@internalTechnology */
inline void TAdvancedPointerEvent::SetPointerNumber(TUint8 aPointerNumber)
	{
	iModifiers|=EModifierAdvancedPointerEvent; // ensure we're an AdvancePointerEvent 
	iData.multiPos3D.iPointerNumber=aPointerNumber;
	}

/** Sets Z and enforces pointer event to advanced pointer event
	@internalTechnology */
inline void TAdvancedPointerEvent::SetPointerZ(TInt aZ)
	{
	iModifiers|=EModifierAdvancedPointerEvent; // ensure we're an advanced pointer event 
	iData.multiPos3D.iPosZ=aZ;
	}

inline void TAdvancedPointerEvent::Copy(const TAdvancedPointerEvent& aPointerEvent)
	{
	if(aPointerEvent.iModifiers&EModifierAdvancedPointerEvent)
		{
		iType=aPointerEvent.iType;
		iModifiers=aPointerEvent.iModifiers;
		iPosition=aPointerEvent.iPosition;
		iParentPosition=aPointerEvent.iParentPosition;	
		iData=aPointerEvent.iData;
		}
	else
		{
		// delegate to TPointerEvent::Copy
		TPointerEvent::Copy(aPointerEvent);
		}
	}

#endif /*ADVANCEDPOINTEREVENT_INL_*/
