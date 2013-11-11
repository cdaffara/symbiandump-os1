/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include "clbslocmonitordbengine.h"
#include "lbsdevloggermacros.h"
#include "lbsposition.h"

#include "../inc/rlbslocmonitordb.h"

/** Default constructor 
@released*/
RLbsLocMonitorDb::RLbsLocMonitorDb()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::RLbsLocMonitorDb()");
	iShortDelayNextRequest = EFalse;
	iLongDelayNextRequest = EFalse; 
	iFailNextRequest = EFalse;
	iCallCount = 0;
	iTestCommandListener = NULL;
	iTimer = NULL; 
	iClientPosition = NULL;
	iClientAreaInfo = NULL;
	iClientMatchAreaInfo = NULL;
	iClientStatus = NULL;
	}

/** Default destructor */
RLbsLocMonitorDb::~RLbsLocMonitorDb()
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorDb::~RLbsLocMonitorDb() \n");
	}

void RLbsLocMonitorDb::OpenL()
	{
	//This shouldn't be called more than once
	LBSLOG(ELogP1, "RLbsLocMonitorDb::Open()");
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	
	// Properties for communications with the test
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	User::LeaveIfError(RProperty::Define(EUnitLocMonDbDelay,RProperty::EInt,KAllowAllPolicy,KAllowAllPolicy));
	User::LeaveIfError(RProperty::Define(EUnitLocMonDbFeedback,RProperty::ELargeByteArray,KAllowAllPolicy,KAllowAllPolicy));
	
	iTestCommandListener = CTestCommandListener::NewL(*this);
	}

void RLbsLocMonitorDb::Close()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::Close()");
	iCallCount = 0;
	delete iTimer;
	delete iTestCommandListener;
	__ASSERT_ALWAYS(KErrNone == RProperty::Delete(EUnitLocMonDbDelay), User::Invariant());//failed to delete property?
    __ASSERT_ALWAYS(KErrNone == RProperty::Delete(EUnitLocMonDbFeedback), User::Invariant());//failed to delete property?	
	}

/*
 This method publishes to a P&S property the aPosition and the first item in aAreaInfo 
 array for the test to validate.
 
 In some testcases, the completion of the client request is delayed (this allowed the build up of requests
 in the Location Monitor or waiting for cancel requests).
*/
TInt RLbsLocMonitorDb::SavePosition(const TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& /*aAreaInfo*/, TBool /*aUserPosition*/, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::SavePosition()");
	
	TInt completionCode = KErrNone;
	aStatus = KRequestPending;
	iCallCount ++;
	
	if (iFailNextRequest)
		{
		completionCode = KErrNotFound;
		iFailNextRequest = EFalse;
		}
	else
		{
		TDbFeedback feedbackItem;
		feedbackItem.iPosition = aPosition;
		TPtr8 ptr(reinterpret_cast<TUint8*>(&feedbackItem), sizeof(TDbFeedback), sizeof(TDbFeedback));
			
		RProperty::Set(KTestLocMonUid, EUnitLocMonDbFeedback, ptr);
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, completionCode);
	return KErrNone;	
	}

/*
This method returns a position and area info to the client.
*/
TInt RLbsLocMonitorDb::GetPosition(TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& /*aAreaInfo*/, TPositionAreaExtendedInfo& aMatchingAreaInfo, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::GetPosition()");
	TInt completionCode = KErrNone;
	aStatus = KRequestPending;
	iCallCount ++;
	
	if (iFailNextRequest)
		{
		completionCode = KErrNotFound;
		iFailNextRequest = EFalse;	
		}
	else if(iShortDelayNextRequest || iLongDelayNextRequest)
		{
		// Remember client request's return params and
		// force a delay.
		iClientPosition = &aPosition;
		iClientMatchAreaInfo = &aMatchingAreaInfo;
		iClientStatus = &aStatus;
		if (!iTimer->IsActive())
			{
			TInt delayInMicroSecs = iLongDelayNextRequest?15000000:1000000; // 1 or 15 seconds
			iTimer->Start(delayInMicroSecs, delayInMicroSecs, TCallBack(StopWaiting, this));	
			}
		else
			{
			// A delay has already been applied. Nothing to do.
			}
		
		iShortDelayNextRequest = EFalse;
		iLongDelayNextRequest = EFalse;
		
		return KErrNone;	
		}
	else
		{
		switch (iCallCount)
			{
			case 1:
			// Set a position as if it came from the DB
			aPosition.SetCoordinate(KLatGetPos1, KLongGetPos1);
			
			// Set a match level as if it was found in the DB
			aMatchingAreaInfo.SetArea(TPositionAreaInfo::EAreaCity);
			aMatchingAreaInfo.SetMobileCountryCodeMatch(ETrue);
			aMatchingAreaInfo.SetMobileNetworkCodeMatch(ETrue);
			aMatchingAreaInfo.SetLocationAreaCodeMatch(ETrue);
			aMatchingAreaInfo.SetCellIdMatch(ETrue);	
			break;

			// Return different values for second and later requests
			// that occurred in the same test.
			default:
			aPosition.SetCoordinate(KLatGetPos2, KLongGetPos2);
			// Set a match level as if it was found in the DB
			aMatchingAreaInfo.SetArea(TPositionAreaInfo::EAreaCountry);
			aMatchingAreaInfo.SetMobileCountryCodeMatch(ETrue);
			aMatchingAreaInfo.SetMobileNetworkCodeMatch(EFalse);
			aMatchingAreaInfo.SetLocationAreaCodeMatch(EFalse);
			aMatchingAreaInfo.SetCellIdMatch(EFalse);
			break;
			}
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, completionCode);
	return KErrNone;
	}


TInt RLbsLocMonitorDb::CancelGetPosition()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::GetPosition()");
	User::RequestComplete(iClientStatus, KErrCancel);
	iTimer->Cancel();
	return KErrNone;
	}

/*
This method returns a position to the client. In some testcases, the completion of the 
asynchronous request may be delayed.
*/
TInt RLbsLocMonitorDb::GetLastStoredPosition(TPosition& aPosition, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::GetLastStoredPosition()");

	TInt completionCode = KErrNone;
	aStatus = KRequestPending;
	iCallCount ++;
	
	if (iFailNextRequest)
		{
		completionCode = KErrNotFound;
		iFailNextRequest = EFalse;	
		}
	else if(iShortDelayNextRequest || iLongDelayNextRequest)
		{
		// Remember client request's return parms and
		// force a delay.
		iClientPosition = &aPosition;
		iClientStatus = &aStatus;
		if (!iTimer->IsActive())
			{
			TInt delayInMicroSecs = iLongDelayNextRequest?15000000:1000000; // 15 or 1 second
			iTimer->Start(delayInMicroSecs, delayInMicroSecs, TCallBack(StopWaiting, this));	
			}
		else
			{
			// A delay has already been applied. Nothing to do.
			}
			
		iShortDelayNextRequest = EFalse;
		iLongDelayNextRequest = EFalse;
		return KErrNone;	
		}
	else
		{
		switch (iCallCount)
			{
			case 1:
			// Set a position as if it came from the DB
			aPosition.SetCoordinate(KLatGetLast1, KLongGetLast1);
			break;

			// Return different values for second and later requests
			// that occurred in the same test.
			default:
			aPosition.SetCoordinate(KLatGetLast2, KLongGetLast2);
			break;
			}
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, completionCode);
	return KErrNone;
	}


// Called by the timer when the delay finishes
TInt RLbsLocMonitorDb::StopWaiting(TAny* aPtr)
	{
	//Need to call DoStopWaiting on real object (this method is static)
	RLbsLocMonitorDb* thisInstance = static_cast<RLbsLocMonitorDb*>(aPtr);
	if (thisInstance)
		{
		thisInstance->DoStopWaiting();
		}
	return KErrNone;
	}


void RLbsLocMonitorDb::DoStopWaiting()
	{
	if (*iClientStatus == KRequestPending)
		{
		// Set a position as if it came from a DB (set predetermined coordinates
		// for first request call since the test only applies a delay to the first
		// request it injects).
		iClientPosition->SetCoordinate(KLatGetPos1, KLongGetPos1);
		
		// All the tests request a delay of a call to GetLastKnownPositionArea
		// so iClientMatchAreaInfo should be available
		if (iClientMatchAreaInfo)
			{
			// Set a match level as if it was found in the DB
			iClientMatchAreaInfo->SetArea(TPositionAreaInfo::EAreaCity);
			iClientMatchAreaInfo->SetMobileCountryCodeMatch(ETrue);
			iClientMatchAreaInfo->SetMobileNetworkCodeMatch(ETrue);
			iClientMatchAreaInfo->SetLocationAreaCodeMatch(ETrue);
			iClientMatchAreaInfo->SetCellIdMatch(ETrue);	
			}

		iTimer->Cancel();
		User::RequestComplete(iClientStatus, KErrNone);
		}
	}

TInt RLbsLocMonitorDb::CancelGetLastStoredPosition()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::CancelGetLastStoredPosition()");
	User::RequestComplete(iClientStatus, KErrCancel);
	iTimer->Cancel();
	return KErrNone;
	}

TInt RLbsLocMonitorDb::CancelSavePosition()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::CancelSavePosition()");
	return KErrNone;
	}

TInt RLbsLocMonitorDb::ClearDatabase()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::ClearDatabase()");
	TDbFeedback feedbackItem;
	TPtr8 ptr(reinterpret_cast<TUint8*>(&feedbackItem), sizeof(TDbFeedback), sizeof(TDbFeedback));

	feedbackItem.iDbDeleted = ETrue;

	iCallCount++;
	
	RProperty::Set(KTestLocMonUid, EUnitLocMonDbFeedback, ptr);
	return KErrNone;
	}

void RLbsLocMonitorDb::ShortDelayNextRequest()
	{
	iShortDelayNextRequest = ETrue;		
	}
void RLbsLocMonitorDb::LongDelayNextRequest()
	{
	iLongDelayNextRequest = ETrue;		
	}
void RLbsLocMonitorDb::FailNextRequest()
	{
	iFailNextRequest = ETrue;		
	}

// Called at the start of a test to reset 'state variables'
// that may have been altered by a previous test
void RLbsLocMonitorDb::KillProcess()
	{
	iCallCount = 0;		
	}

/*******************************************************************************
 CTestCommandListener
********************************************************************************/

CTestCommandListener::CTestCommandListener(RLbsLocMonitorDb& aDbInterface):
						CActive(EPriorityStandard),
						iDbInterface (aDbInterface)
	{
	CActiveScheduler::Add(this);
	
	}

CTestCommandListener* CTestCommandListener::NewL(RLbsLocMonitorDb& aDbInterface)
	{
	CTestCommandListener* self = new(ELeave) CTestCommandListener(aDbInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestCommandListener::ConstructL()
	{
	User::LeaveIfError(iDelayProperty.Attach(KTestLocMonUid,EUnitLocMonDbDelay,EOwnerThread));
	iDelayProperty.Subscribe(iStatus);
	SetActive();
	}
	
CTestCommandListener::~CTestCommandListener()
	{
	Cancel();
	iDelayProperty.Close();
	}

void CTestCommandListener::RunL()
	{
	if (KErrNone == iStatus.Int())
		{
		TInt command;
		iDelayProperty.Subscribe(iStatus);
		SetActive();
		
		iDelayProperty.Get(command);

		switch (command)
			{
			case EDbSetupShortDelay:
				// Inform the DB that a short delay is required
				iDbInterface.ShortDelayNextRequest();
			break;
			
			case EDbSetupLongDelay:
				// Inform the DB that a short delay is required
				iDbInterface.LongDelayNextRequest();
			break;
			
			case EDbSetupFail:
				// Inform the DB that a it must fail the next request
				iDbInterface.FailNextRequest();
			break;
			
			case EDbKillProcess:
				// Inform the DB that a new test is starting
				iDbInterface.KillProcess();
			break;
			
			default:
				ASSERT(EFalse);
			break;
			}

		// Publish "dummy feedback" as a means to 
		// let the Test know that the DB got the 
		// message and a delay will be applied on the
		// processing of the next request.
		TDbFeedback AckFeedbackItem;
		TPtr8 ptr(reinterpret_cast<TUint8*>(&AckFeedbackItem), sizeof(TDbFeedback), sizeof(TDbFeedback));
		RProperty::Set(KTestLocMonUid, EUnitLocMonDbFeedback, ptr);		
		}
	}


TInt CTestCommandListener::RunError(TInt aError)
	{
	// Just panic
	ASSERT(EFalse);
	return aError;
	}

void CTestCommandListener::DoCancel()
	{
	iDelayProperty.Cancel();
	}

