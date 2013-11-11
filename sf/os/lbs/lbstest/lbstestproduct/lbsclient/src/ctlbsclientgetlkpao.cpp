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
// TODO - should this be moved somewhere more central (testutils, say)?
// 
//

/**
 @file ctlbsclientgetlkpao.cpp
*/

#include "ctlbsclientgetlkpao.h"
#include <lbs.h>

_LIT(KGetLastKnownPosAO, "GetLastKnownPosAO");

CT_LbsClientGetLastKnownPosAO::~CT_LbsClientGetLastKnownPosAO()
/**
 * 	Destructor
 */
	{
	Cancel();
	}
	
		

CT_LbsClientGetLastKnownPosAO::CT_LbsClientGetLastKnownPosAO(MT_GetLastKnownPosObserver& aCaller) : CActive(EPriorityIdle), iCaller(aCaller)
/**
 * 	Constructor - will not leave
 */
 	{ 	
 	CActiveScheduler::Add(this);
	}


CT_LbsClientGetLastKnownPosAO* CT_LbsClientGetLastKnownPosAO::NewL(MT_GetLastKnownPosObserver& aUser)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsClientGetLastKnownPosAO* self = new(ELeave)CT_LbsClientGetLastKnownPosAO(aUser);
	
	return self;
	}
	
	
void CT_LbsClientGetLastKnownPosAO::GetLastKnownPosL(RPositioner& aPositioner, TPositionInfo& aPosInfo)
/**
 * 	wrapper for async positioner function GetLastKnownPosition(). 
 *  Will panic if there's another outstanding request.
 */
	{	
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KGetLastKnownPosAO, KErrInUse));
	
	iPositioner = aPositioner;
	
	// TO DO: this requestor stuff will be removed when we're using the real Location Server:
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
												CRequestor::EFormatApplication,
												_L("Tom Tom")));
	
	iRequestId = EPositionerGetLastKnownPosition;	// required for cancel
	
	// Make async call
	aPositioner.GetLastKnownPosition(aPosInfo, iStatus);
	
	// Let the active scheduler know we're waiting (active)
	SetActive();
	}
	
void CT_LbsClientGetLastKnownPosAO::DoCancel()
	{	
	iPositioner.CancelRequest(iRequestId);
	}
 
void CT_LbsClientGetLastKnownPosAO::RunL()
	{
	// iStatus will contain error code eg KErrUnknown if no position cached
	// async request completed. Notify caller via callback:
	iCaller.GetLastKnownPositionCallback(iStatus);
	}

TInt CT_LbsClientGetLastKnownPosAO::RunError(TInt aError)
	{	// called if RunL leaves. aError contains the error code
	return aError;
	}
// EOF

