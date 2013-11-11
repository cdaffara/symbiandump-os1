// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of classes for priority keys
// 
//

#if !defined(__PRIKEY_H__)
#define __PRIKEY_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__E32SVR_H__)
#include <e32svr.h>
#endif
#if !defined(__EVQUEUE_H__)
#include "EVQUEUE.H"
#endif


class TPriorityKey
	{
public:
	TPriorityKey(TUint aKeycode,TUint aModifierMask,TUint aModifiers,TPriorityKey *aPriorityKeys);
	TInt Equals(TUint aKeycode,TUint aModifierMask,TUint aModifiers);
	TInt KeyMatches(const TKeyEvent &aKeyEvent);
public:
	TPriorityKey *iNext;
private:
	TUint iKeyCode;
	TUint iModifiers;
	TUint iModifierMask;
	};

class CPriorityKey : public CEventBase
	{
public:
	CPriorityKey(CWsClient *aOwner);
	void PriorityKey(TInt aHandle, const TKeyEvent &aKeyEvent);
	void GetData();
private:
	TInt iPriorityKeyHandle;
	TKeyEvent iPriorityKey;
	};

#endif
