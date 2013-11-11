// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Includes
// System
// 
//

#include <e32std.h>

// Component
#include "LbsOomListener.h"


//****************************************************************
// Class COomListener
//****************************************************************
/** Static constructor.
@released
@internalComponent
*/
EXPORT_C CLbsOomListener* CLbsOomListener::NewL()
	{
	CLbsOomListener* self = new(ELeave)CLbsOomListener();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Constructor.
@released
@internalComponent
*/
CLbsOomListener::CLbsOomListener():
	CActive(EPriorityHigh),
	iFlag(EFalse)
	{
	}

/** Second phase constructor.
@released
@internalComponent
*/
void CLbsOomListener::ConstructL()
	{
	iRequestOom.OpenL();
	CActiveScheduler::Add(this);		
	}

/** Destructor.
@released
@internalComponent
*/
EXPORT_C CLbsOomListener::~CLbsOomListener()
	{
	iRequestOom.Close();
	}


/** Handle request on the Oom bus
This function handles the incomming Oom requests on 
the bus, and enables/disables heap allocation failing
@see CActive::RunL
@released
@internalComponent
*/
void CLbsOomListener::RunL()
	{
	StartGettingRequests();
	
	TBool memoryFail = iRequestOom.GetOomTestNotifiyUpdates();
 	if(memoryFail)
		{
		iFlag = ETrue;
		__UHEAP_BURSTFAILNEXT(1,1000);
		}
	else
		{
		if(iFlag) // only reset if OOM Test has been invoked before
			{
			iFlag = EFalse;
			__UHEAP_RESET;		
			}
		}
	}

/** 
@see CActive::DoCancel
@released
@internalComponent
*/
void CLbsOomListener::DoCancel()
	{
	iRequestOom.CancelToReceiveOomSignal();
	}
	
	
	
/** Subscribe to the Oom request bus
@released
@internalComponent
*/
EXPORT_C void CLbsOomListener::StartGettingRequests()
	{
 	iRequestOom.OomTestNotifiyUpdates(iStatus);
	SetActive();
	}
