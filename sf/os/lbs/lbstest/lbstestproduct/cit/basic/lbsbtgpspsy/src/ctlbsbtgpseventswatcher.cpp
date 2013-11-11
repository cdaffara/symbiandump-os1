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
// Watcher for BT GPS PNOK Events
//

#include "ctlbsbtgpseventswatcher.h"

CBtGpsEventsWatcher* CBtGpsEventsWatcher::NewL(MBtGpsEventsEventObserver& aObserver, TUint aMask)
	{
	CBtGpsEventsWatcher* self = new (ELeave) CBtGpsEventsWatcher(aObserver, aMask);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CBtGpsEventsWatcher::~CBtGpsEventsWatcher()
	{
	Cancel();
	iProperty.Close();
	}


void CBtGpsEventsWatcher::RunL()
	{
	//Get the updated value of the Property
	TInt value;
	iProperty.Get(KPsUidBluetoothGpsPsy, iEventsMask, value);
	
	//Give the updated value to the observer along with the mask value so they can tell
	// what Property has been updated
	iObserver.OnEventsUpdated(iEventsMask, value);	
	
	//Start up the notifier again to listen for any changes
	iProperty.Subscribe(iStatus);
	SetActive();
	}


void CBtGpsEventsWatcher::DoCancel()
	{
	iProperty.Cancel();
	}


TInt CBtGpsEventsWatcher::RunError(TInt aError)
	{
	return aError;
	}

	
CBtGpsEventsWatcher::CBtGpsEventsWatcher(MBtGpsEventsEventObserver& aObserver, TUint aMask)
	: CActive(CActive::EPriorityStandard),
	  iObserver(aObserver),
	  iEventsMask(aMask)
	{
	CActiveScheduler::Add(this);
	}


void CBtGpsEventsWatcher::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(KPsUidBluetoothGpsPsy, iEventsMask));
	iProperty.Subscribe(iStatus);
	SetActive();
	}
