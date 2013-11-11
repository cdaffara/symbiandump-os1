// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server priority key queue handling
// 
//

#include "PRIKEY.H"


CPriorityKey::CPriorityKey(CWsClient *aOwner) : CEventBase(aOwner)
	{
	__DECLARE_NAME(_S("CPriorityKey"));
	}

void CPriorityKey::PriorityKey(TInt aHandle, const TKeyEvent &aKeyEvent)
//
// Called when a priority key is pressed
//
	{
	if (!iEventMsg.IsNull())
		{
		iPriorityKeyHandle=aHandle;
		iPriorityKey = aKeyEvent;
		SignalEvent();
		}
	}

void CPriorityKey::GetData()
//
// If there is an outstanding abort event in the queue, reply with it's data, else send a null abort event
//
	{
	TWsPriorityKeyEvent event;
	*event.Key() = iPriorityKey;
	event.Key()->iRepeats = 0;
	event.SetHandle(iPriorityKeyHandle);
	CEventBase::GetData(&event,sizeof(event));
	iPriorityKeyHandle=0;
	}

TPriorityKey::TPriorityKey(TUint aKeycode,TUint aModifierMask,TUint aModifiers,TPriorityKey *aPriorityKeys) :
	iNext(aPriorityKeys),
	iKeyCode(aKeycode),
	iModifiers(aModifiers),
	iModifierMask(aModifierMask)
	{}

TInt TPriorityKey::Equals(TUint aKeycode,TUint aModifierMask,TUint aModifiers)
	{
	return(iKeyCode==aKeycode && iModifierMask==aModifierMask && iModifiers==aModifiers);
	}

TInt TPriorityKey::KeyMatches(const TKeyEvent &aKeyEvent)
	{
	return(iKeyCode == aKeyEvent.iCode && (iModifierMask & aKeyEvent.iModifiers) == iModifiers);
	}
