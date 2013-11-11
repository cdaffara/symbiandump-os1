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
// ao wrapper around the a-gps mgr function NotifyPositionUpdate
// 
//

/**
 @file ctlbsdoposupd.cpp
*/

#include "ctlbsdoposupdate.h"
#include <lbs/test/tlbsutils.h>


CT_LbsDoPosUpdate* CT_LbsDoPosUpdate::NewL(MT_LbsDoPosUpdateObserver* aObserver, TPositionModuleInfo::TTechnologyType aTechnologyType)
/**
 *	'public constructor' may leave
 */
	{
	TPositionCriteria dummyCritera;
	CT_LbsDoPosUpdate* doPosUpdate = new(ELeave)CT_LbsDoPosUpdate(aObserver);
	CleanupStack::PushL(doPosUpdate);
	doPosUpdate->ConstructL(EOpenSubSessionByModuleId,aTechnologyType,dummyCritera);
	CleanupStack::Pop(doPosUpdate);

	return doPosUpdate;
	}


CT_LbsDoPosUpdate* CT_LbsDoPosUpdate::NewL(MT_LbsDoPosUpdateObserver* aObserver, const TPositionCriteriaBase& aCriteria)
/**
 *	'public constructor' may leave
 */
	{
	TUint dummyTechnologyType = TPositionModuleInfo::ETechnologyUnknown;
	CT_LbsDoPosUpdate* doPosUpdate = new(ELeave)CT_LbsDoPosUpdate(aObserver);
	CleanupStack::PushL(doPosUpdate);
	doPosUpdate->ConstructL(EOpenSubSessionByCriteria,dummyTechnologyType,aCriteria);
	CleanupStack::Pop(doPosUpdate);

	return doPosUpdate;
	}


CT_LbsDoPosUpdate::CT_LbsDoPosUpdate(MT_LbsDoPosUpdateObserver* aObserver) : CActive(EPriorityStandard), iObserver(aObserver)
/**
 * 	Constructor - will not leave
 */
	{ 	
	CActiveScheduler::Add(this);
	}


void CT_LbsDoPosUpdate::OpenPositioner(TPositionModuleInfo::TTechnologyType aTechnologyType)
	{
	// Open the positioner.
	if(TPositionModuleInfo::ETechnologyUnknown == aTechnologyType)	// use default 
		{
		User::LeaveIfError(iPositioner.Open(iServer));
		}
	else if(TPositionModuleInfo::ETechnologyTerminal == aTechnologyType)
		{
		T_LbsUtils utils;
		// Get position using Agps module
		TPositionModuleId moduleId;
		moduleId = utils.GetAGpsModuleIdL(iServer);
		User::LeaveIfError(iPositioner.Open(iServer, moduleId));	
		}
	else if(TPositionModuleInfo::ETechnologyNetwork == aTechnologyType)
		{
		T_LbsUtils utils;
		// Get position using network module
		TPositionModuleId moduleId;
		moduleId = utils.GetNetworkModuleIdL(iServer);
		User::LeaveIfError(iPositioner.Open(iServer, moduleId));	
		}
	else
		{
		__ASSERT_ALWAYS(EFalse, User::Panic(KCT_LbsDoPosUpdate, KErrNotSupported));	// not supported yet
		}
	}
	
void CT_LbsDoPosUpdate::ClosePositioner(void)
	{
	iPositioner.Close();
	}
	
void CT_LbsDoPosUpdate::ConnectServer()
	{
	User::LeaveIfError(iServer.Connect());
	}

void CT_LbsDoPosUpdate::CloseServer()
	{
	iServer.Close();
	}
	
void CT_LbsDoPosUpdate::ConstructL(TOpenSubSessionType aOpenSubSessionType,TPositionModuleInfo::TTechnologyType aTechnologyType, const TPositionCriteriaBase& aCriteria)
	{
	// Connect to the LBS self locate server.
	User::LeaveIfError(iServer.Connect());
	
	if(aOpenSubSessionType == EOpenSubSessionByModuleId)
		{
		OpenPositioner(aTechnologyType);
		}
	
	else if(aOpenSubSessionType == EOpenSubSessionByCriteria)
		{
		User::LeaveIfError(iPositioner.Open(iServer, aCriteria));
		}
	
	// set requester will be removed, don't forget the pushl above
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
													CRequestor::EFormatApplication,
	    											_L("Tom Tom")));
	
	}
	

CT_LbsDoPosUpdate::~CT_LbsDoPosUpdate()
/**
 * 	Destructor
 */
	{
	Cancel();

	iPositioner.Close();
	iServer.Close();
	}


TInt CT_LbsDoPosUpdate::SetOptions(const TPositionUpdateOptionsBase& aPosOption)
	{
	return iPositioner.SetUpdateOptions(aPosOption);
	}


void CT_LbsDoPosUpdate::StartL(TPositionInfo& aPosInfo)
/**
 * 	wrapper for async  function RLbsPositionUpdates::NotifyPositionUpdate(). 
 *  Will panic if there's another outstanding request.
 */
	{	
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KCT_LbsDoPosUpdate, KErrInUse));

	// Invoke the position update.	
	iPositioner.NotifyPositionUpdate(aPosInfo, iStatus);
	SetActive();
	}

void CT_LbsDoPosUpdate::CancelRequest()
	{
	// To allow us to cancel the request but not the active object, normally Cancel() would be used.
	DoCancel();	
	}

TInt CT_LbsDoPosUpdate::CancelRequestWithResult()
	{
	// To allow us to cancel the request but not the active object, normally Cancel() would be used.
	return (DoCancelWithResult());
	}


void CT_LbsDoPosUpdate::DoCancel()
	{	
	iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
	}

TInt CT_LbsDoPosUpdate::DoCancelWithResult()
	{	
	return (iPositioner.CancelRequest(EPositionerNotifyPositionUpdate));
	}


void CT_LbsDoPosUpdate::RunL()
	{
	TInt err = iStatus.Int();
	
	// iStatus will contain error code
	// async request completed. Notify caller via callback:
	if (iObserver)
		{
		iObserver->MT_LbsDoPosUpdateCallback(iStatus);
		}
	}

TInt CT_LbsDoPosUpdate::RunError(TInt aError)
	{
	return aError;
	}

TInt CT_LbsDoPosUpdate::CompleteRequest(TInt aRequestId)
	{
	return iPositioner.CompleteRequest(aRequestId);
	}

