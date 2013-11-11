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
// This is the class implementation for the Notify position update tests
// TODO - should this be moved somewhere more central (testutils, say)?
// 
//

/**
 @file ctlbsclientnotifyposupdao.cpp
*/

#include "ctlbsclientnotifyposupdao.h"
#include <lbs.h>
#include <lbs/lbsadmin.h>

_LIT(KNotifyPosUpdAO, "NotifyPosUpdAO");

CT_LbsClientNotifyPosUpdAO::~CT_LbsClientNotifyPosUpdAO()
/**
 * 	Destructor
 */
	{
	Cancel();
	}
	
		

CT_LbsClientNotifyPosUpdAO::CT_LbsClientNotifyPosUpdAO(MT_NotifyPosUpdateObserver& aCaller) : CActive(EPriorityIdle), iCaller(aCaller)
/**
 * 	Constructor - will not leave
 */
 	{ 	
 	CActiveScheduler::Add(this);
	}


CT_LbsClientNotifyPosUpdAO* CT_LbsClientNotifyPosUpdAO::NewL(MT_NotifyPosUpdateObserver& aCaller)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsClientNotifyPosUpdAO* me = new(ELeave)CT_LbsClientNotifyPosUpdAO(aCaller);
	
	return me;
	}
	
	
void CT_LbsClientNotifyPosUpdAO::NotifyPosUpdateL(RPositioner& aPositioner, TPositionInfo& aPosInfo)
/**
 * 	wrapper for async positioner function NotifyPositionUpdate(). 
 *  Will panic if there's another outstanding request.
 */
	{	
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KNotifyPosUpdAO, KErrInUse));
	
	iPositioner = aPositioner;
	iRequestId = EPositionerNotifyPositionUpdate;	// required for cancel
	
	// TO DO: this requestor stuff will be removed when we're using the real Location Server:
	User::LeaveIfError(iPositioner.SetRequestor(CRequestor::ERequestorService,
												CRequestor::EFormatApplication,
												_L("Tom Tom")));
	
	
	// Make async call
	aPositioner.NotifyPositionUpdate(aPosInfo, iStatus);
	
	// Let the active scheduler know we're waiting (active)
	SetActive();
	}
	
void CT_LbsClientNotifyPosUpdAO::DoCancel()
	{	
	TInt err = iPositioner.CancelRequest(iRequestId);
	}
	
 
void CT_LbsClientNotifyPosUpdAO::RunL()
	{
	// iStatus will contain error code eg KErrUnknown if no position cached
	// async request completed. Notify caller via callback:
	iCaller.NotifyPositionUpdateCallback(iStatus);
	}

TInt CT_LbsClientNotifyPosUpdAO::RunError(TInt aError)
	{	// called if RunL leaves. aError contains the error code
	return aError;
	}
// EOF

