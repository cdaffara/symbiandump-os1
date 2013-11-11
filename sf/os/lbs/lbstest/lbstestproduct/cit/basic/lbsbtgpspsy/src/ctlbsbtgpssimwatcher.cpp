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
// Watcher for BT GPS Simulation test server
//

#include "ctlbsbtgpssimwatcher.h"


CBtGpsSimWatcher* CBtGpsSimWatcher::NewL(MBtGpsSimEventObserver& aObserver)
	{
	CBtGpsSimWatcher* self = new (ELeave) CBtGpsSimWatcher(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CBtGpsSimWatcher::~CBtGpsSimWatcher()
	{
	Cancel();
	iSim.Close();
	}


void CBtGpsSimWatcher::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		switch (iEventInfo().Event())
			{
			case EBtGpsSimEventConnect:
				iObserver.OnDeviceConnect(iEventInfo().DeviceAddress());
				break;
	
			case EBtGpsSimEventDisconnect:
				iObserver.OnDeviceDisconnect(iEventInfo().DeviceAddress());
				break;
	
			case EBtGpsSimEventError:
				iObserver.OnDeviceError(iEventInfo().DeviceAddress());
				break;
				
			default:
				break;
			}
		}

	// Request notification for next event
	iSim.NotifyEvent(iEventInfo, iStatus);
	SetActive();
	}


void CBtGpsSimWatcher::DoCancel()
	{
	iSim.NotifyCancel();
	}


TInt CBtGpsSimWatcher::RunError(TInt aError)
	{
	return aError;
	}

	
CBtGpsSimWatcher::CBtGpsSimWatcher(MBtGpsSimEventObserver& aObserver)
	: CActive(CActive::EPriorityStandard),
	  iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


void CBtGpsSimWatcher::ConstructL()
	{
	User::LeaveIfError(iSim.Connect());
	iSim.NotifyEvent(iEventInfo, iStatus);
	SetActive();
	}
