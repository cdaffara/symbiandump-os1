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
// Helper to initialise and set/get info from TAdvancedPointerEvent
// 
//

#include "advancedpointereventhelper.h"


void TAdvancedPointerEventHelper::InitAdvancedPointerEvent(TWsEvent& aEvent, TPointerEvent::TType aType, TUint aModifiers, const TPoint3D &aPoint3D, TUint8 aPointerNumber)
	{
	aEvent.Pointer()->Init(aType, aModifiers, aPoint3D, aPointerNumber);
	};

void TAdvancedPointerEventHelper::InitAdvancedPointerEvent(TWsEvent& aEvent, TPointerEvent::TType aType, TUint aModifiers, const TPoint3D &aPoint3D, const TPoint &aParentPosition, TUint8 aPointerNumber)
	{
	aEvent.Pointer()->Init(aType, aModifiers, aPoint3D, aPointerNumber);
	aEvent.Pointer()->iParentPosition = aParentPosition;
	};

TUint8 TAdvancedPointerEventHelper::PointerNumber(const TWsEvent& aWsEvent)
	{
	return aWsEvent.Pointer()->iData.multiPos3D.iPointerNumber;
	}

void TAdvancedPointerEventHelper::SetPointerNumber(TWsEvent& aWsEvent, TUint8 aPointerNumber)
	{
	aWsEvent.Pointer()->SetPointerNumber(aPointerNumber);
	}

TInt TAdvancedPointerEventHelper::Z(const TWsEvent& aWsEvent)
	{
	return aWsEvent.Pointer()->iData.multiPos3D.iPosZ;
	}

void TAdvancedPointerEventHelper::SetZ(TWsEvent& aWsEvent, TInt aPosZ)
	{
	aWsEvent.Pointer()->SetPointerZ(aPosZ);
	}

void TAdvancedPointerEventHelper::Copy(const TAdvancedPointerEvent& aSrcEvent, TAdvancedPointerEvent& aDestEvent)
	{
	aDestEvent.Copy(aSrcEvent);
	}
