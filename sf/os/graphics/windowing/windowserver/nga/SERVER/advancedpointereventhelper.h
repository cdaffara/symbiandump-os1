// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// advanced pointer event helper header file
// 
//

#ifndef __ADANCEDPOINTEREVENTHELPER_H__
#define __ADANCEDPOINTEREVENTHELPER_H__

#include "W32STD.H"

class TAdvancedPointerEventHelper
	{
public:	
	static void InitAdvancedPointerEvent(TWsEvent& aEvent, TPointerEvent::TType aType, TUint aModifiers, const TPoint3D &aPoint3D, TUint8 aPointerNumber);
	static void InitAdvancedPointerEvent(TWsEvent& aEvent, TPointerEvent::TType aType, TUint aModifiers, const TPoint3D &aPoint3D, const TPoint &aParentPosition, TUint8 aPointerNumber);
	static TUint8 PointerNumber(const TWsEvent& aWsEvent);	
	static void SetPointerNumber(TWsEvent& aWsEvent, TUint8 aPointerNumber);
	static TInt Z(const TWsEvent& aWsEvent);
	static void SetZ(TWsEvent& aWsEvent, TInt aZ);
	static void Copy(const TAdvancedPointerEvent& aSrcEvent, TAdvancedPointerEvent& aDestEvent);
	};

#endif // __ADANCEDPOINTEREVENTHELPER_H__
