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
//


#include "lbsearlycompletionupdatehandler.h"
#include <lbs/lbsextendedsatellite.h>

CEarlyCompletionUpdateHandler * CEarlyCompletionUpdateHandler::NewL
	(
	MLbsEarlyCompletionUpdateObserver* aObserver
	)
	{

	CEarlyCompletionUpdateHandler * self = new(ELeave) CEarlyCompletionUpdateHandler (aObserver);

	CleanupStack::PushL(self);

	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}
	

	
CEarlyCompletionUpdateHandler::CEarlyCompletionUpdateHandler(MLbsEarlyCompletionUpdateObserver* aObserver) : 
CActive(CActive::EPriorityStandard),	
iObserver(aObserver)
	{

	}

void CEarlyCompletionUpdateHandler::DoCancel()
	{
	// cancel RLbsPositionUpdateRequests aync request
	iEarlyCompletionPosUpdates.CancelNotifyPositionUpdate();
	}

void CEarlyCompletionUpdateHandler::ListenForEarlyCompletionUpdates()
	{
	Cancel();
	iEarlyCompletionPosUpdates.NotifyPositionUpdate(iStatus);
	SetActive();	
	}

void CEarlyCompletionUpdateHandler::ConstructL()
	{
	iEarlyCompletionPosUpdates.OpenL(KLbsLocServerUid);
	CActiveScheduler::Add(this);
	}

//------------------------------------------------------------------------------------------------------------
CEarlyCompletionUpdateHandler::~CEarlyCompletionUpdateHandler()
	{
	Cancel();
	iEarlyCompletionPosUpdates.Close();
	}
 

TInt CEarlyCompletionUpdateHandler::RunError(TInt aError)
{
	// handle RunL leaving - in our case we do nothing 
	// as this means we have had encountered a program inconsistency
	// By doing nothing here  causes the main manager thread to exit
	// and the Lbs root process resolves any issues - by stopping and restarting
	// lbs components
 	return aError;
 	
}

void CEarlyCompletionUpdateHandler::RunL()
	{
	TInt status = iStatus.Int();
	
	// Expect further updates
	ListenForEarlyCompletionUpdates();
	
	switch (status)
		{
		case KErrNone:
			{
			TPositionExtendedSatelliteInfo satInfo;
			TTime targetTime;
			TTime actualTime;
			TBool conflict;
			
			TInt ret = iEarlyCompletionPosUpdates.GetAsMuchAsCanPositionInfo(conflict, &satInfo, sizeof(satInfo), targetTime, actualTime);
			
			// Loc Server must publish the position with this code:
			if (KPositionEarlyComplete == ret)
				{
				// only pass on good locations - discard any associated with errors
				__ASSERT_DEBUG(iObserver!= NULL,User::Panic(_L("CEarlyCompletionUpdateHandler1"), KErrArgument));
				iObserver->EarlyCompletionUpdate(satInfo, targetTime, actualTime); // call back to observer (MainLogic)
				}
			__ASSERT_DEBUG(ret == KPositionEarlyComplete, User::Panic(_L("CEarlyCompletionUpdateHandler2"), KErrArgument));	
			
			break;
			}
		default:
			{
			User::LeaveIfError(status);
			break;
			}
		}
	}




