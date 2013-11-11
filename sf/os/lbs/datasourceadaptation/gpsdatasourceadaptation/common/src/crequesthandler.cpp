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
// requesthandler.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "crequesthandler.h"
#include "lbsdevloggermacros.h"
#include "cpositionerq.h"
#include "psylogging.h"
#include "psypanic.h"
#include "mpositioner.h"
#include "trequestparameters.h"
#include "tpositionercall.h"

/*
* CRequestHandler::~CRequestHandler
* Destructor.
*/
CRequestHandler::~CRequestHandler()
    {
	if(iPowerStandByTimer)
		{
		iPowerStandByTimer->Cancel();
		}

	delete iPowerStandByTimer;
	iPowerStandByTimer = NULL;
    }

/**
@param aModuleId Position module Id
@param aChannel Channel identifier
@param aCache A reference to lastknown position cache
*/
CRequestHandler::CRequestHandler() 
	{
	}
	
void CRequestHandler::ConstructL()
	{
	LBSLOG(ELogP1, "CRequestHandler::ConstructL()");
	iPowerStandByTimer = CLbsCallbackTimer::NewL(*this);
	ResetMergedRequest();
	}

/**
* Update the request parameters
*	
* @param aRequest The request to be updated
* @param aTimeForFix The maximum fix time
* @param aQuality The requested position info quality
*/
void CRequestHandler::BuildUpdateRequest(TLbsLocRequestQuality& aRequest, const TTimeIntervalMicroSeconds& aTimeForFix,
										const TPositionQuality& aQuality)
	{
	aRequest.SetMinHorizontalAccuracy(aQuality.HorizontalAccuracy());
	aRequest.SetMinVerticalAccuracy(aQuality.VerticalAccuracy());
	aRequest.SetMaxFixTime(aTimeForFix);
	}

/**
* Compare the current and merged requests
* 
* @ret TBool, ETrue if the current and merged requests are not the same 
*/
TBool CRequestHandler::MergedDifferent()
	{
	TBool different = EFalse;

	different = iMergedRequest.RequestQuality().MinHorizontalAccuracy() != 
				iCurrentRequest.RequestQuality().MinHorizontalAccuracy();
	different |= iMergedRequest.RequestQuality().MinVerticalAccuracy() != 
				iCurrentRequest.RequestQuality().MinVerticalAccuracy();
	different |= iMergedRequest.TargetTime() != iCurrentRequest.TargetTime();

	TLbsNetPosMethodInt mergedMethod;
	TLbsNetPosMethodInt currentMethod;
	iMergedRequest.RequestMethod().GetPosMethod(0, mergedMethod);
	iCurrentRequest.RequestMethod().GetPosMethod(0, currentMethod);
	different |= mergedMethod.PosMode() != currentMethod.PosMode();

	return different;
	}
	

/**
* Cancels the location retrieval if no outstanding requests
* Does nothing if there are outstanding requests
* @param aError, reason for cancel
*/
void CRequestHandler::CancelRequestL()
	{
	LBSLOG(ELogP1, "CRequestHandler::CancelRequestL()");
	TActiveClients active;
	if(!(iPositionerQ->IterETrue(active)))
		{
		LBSLOG(ELogP1, "No active clients. Send cancel to manager");
		TTracking tracking;
		IssueCancel(iPositionerQ->IterETrue(tracking));
		ResetMergedRequest();
		}
	LBSLOG(ELogP1, "CPositionerQ::CancelRequestL() end");
	}


/**
* Remerges any outstanding requests and Issues either new merged request if any, or issues cancel if none
* @param aError, reason for cancel
*/
void CRequestHandler::ReMergeWithCancelRequestL(TInt aError)
	{
	LBSLOG(ELogP1, "CRequestHandler::ReMergeCancelRequestL()");
	TActiveClients active;
	if(iPositionerQ->IterETrue(active))
		{
		LBSLOG(ELogP1, "Active clients");
		SubmitRequestL();
		}
	else // no outstanding requests
		{
		LBSLOG(ELogP1, "No active clients");
		if(aError != KErrTimedOut)// Don't pass to mgr if it's a LS timeout
			{
			LBSLOG(ELogP1, "Send cancel to manager");
			TTracking tracking;
			IssueCancel(iPositionerQ->IterETrue(tracking));
			}
		ResetMergedRequest();
		}
	LBSLOG(ELogP1, "CPositionerQ::ReMergeCancelRequestL() end");
	}

/**
* Triggers merging of new request
* Assumes the new request is now active so will be picked up below
*/
void CRequestHandler::SubmitNewRequestL(MPositioner* aNewRequest)
	{
	LBSLOG(ELogP1, "CRequestHandler::SubmitNewRequestL()");
	TInt newReqIndex = iPositionerQ->Index(*aNewRequest);
		
	TInt error = MergeNewRequest(newReqIndex);
	if(KErrNone == error)
		{
		if(MergedDifferent())
			{
			IssueMergedRequestL();
			
			iPowerStandByTimer->Cancel();
			}
		}
	else
		{
		aNewRequest->UpdateFailed(error);
		}
	}

TInt CRequestHandler::MergeNewRequest(TInt aNewRequest)
	{
	LBSLOG(ELogP1, "CRequestHandler::MergeNewRequest()");
	TBool active = (iPositionerQ->Positioner(aNewRequest)).IsActive();
	
	__ASSERT_DEBUG(active, User::Panic(KAdaptationPanicCategory, EPanicNewRequestInactive));

	TTracking tracking;
	TInt error = MergeRequest( (iPositionerQ->Positioner(aNewRequest)).PositionRequestParams(), 
		iPositionerQ->IterETrue(tracking), TLbsPositionUpdateRequestBase::EPowerAdviceOn );

	return error;
	}



/**
* Triggers merging of existing requests
*/
void CRequestHandler::SubmitRequestL()
	{
	LBSLOG(ELogP1, "CRequestHandler::SubmitRequestL()");
	MergeAllRequests();
		
	if(MergedDifferent())
		{
		IssueMergedRequestL();
		
		iPowerStandByTimer->Cancel();
		}
	}

/**
* Merges all active requests
*/
void CRequestHandler::MergeAllRequests()
	{
	LBSLOG(ELogP1, "CRequestHandler::MergeAllRequests()");
	ResetMergedRequest();
	
    TInt count = iPositionerQ->Count();
    for(TInt i=0; i<count; i++)
        {
        if((iPositionerQ->Positioner(i)).IsActive())
            {
   			TTracking tracking;
   			TInt error = MergeRequest( (iPositionerQ->Positioner(i)).PositionRequestParams(), iPositionerQ->IterETrue(tracking), TLbsPositionUpdateRequestBase::EPowerAdviceOn );
			__ASSERT_DEBUG(KErrNone == error, User::Panic(KAdaptationPanicCategory, EPanicModeMergeFailed));
            }
        }
	}

/**
* 
*/
void CRequestHandler::WarmDownTimerExpired()
	{
	LBSLOG(ELogP1, "CRequestHandler::WarmDownTimerExpired()");
	// Iterate positioners. If none active and no warmdown timers running, send a cancel to the manager
	TActiveClients active;
	TWarmingDownClients warmdown;
	TTracking tracking;
	
	if(!iPositionerQ->IterETrue(active) && !iPositionerQ->IterETrue(warmdown))
		{
		IssueCancel(iPositionerQ->IterETrue(tracking));
		ResetMergedRequest();
		}
	}
	
/**
* CRequestHandler::NotifyRequestComplete
* 
* Notifies the request handler that a location fix has been received 
* and recombines any outstanding requests for active positioners. This
* is because all positioners are held in the queue not just active ones.
*/
void CRequestHandler::NotifyRequestComplete()
	{
	LBSLOG(ELogP1, "CRequestHandler::NotifyRequestComplete()");
	TActiveClients active;
	if(!(iPositionerQ->IterETrue(active)))
		{
		TInactivityTimeout inactivity;

		TTimeIntervalMicroSeconds32 timeout = iPositionerQ->IterGreatest(inactivity);
		iPowerStandByTimer->Cancel();
		iPowerStandByTimer->EventAfter(timeout, EPowerStandByTimerEvent);
		ResetMergedRequest();
		}
	else
		{
		// If an error occurred sending a request then complete
		// all outstanding clients with the error code
		TRAPD(result, SubmitRequestL());
		if(result != KErrNone)
			{
			TUpdateFailed call(result);
			iPositionerQ->PositionerIterator(call);
			}
		}
	}

/**
TimeOutTimerEvent handling
*/	
void CRequestHandler::TimeOutTimerEvent()
	{
    LBSLOG(ELogP1, "CRequestHandler::TimeOutTimerEvent()");

	iPowerStandByTimer->Cancel();

	TActiveClients active;
	if(!(iPositionerQ->IterETrue(active)))
		{
		// if queue has no active positioners then send advice EPowerAdviceStandby
		TTracking tracking;
		IssueStatus(iPositionerQ->IterETrue(tracking), TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
		}
	}
	
/**
TimeOutTimer error handling
*/	
TInt CRequestHandler::TimeOutTimerError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
from MLbsCallbackTimerObserver

@param aTimerId Time event ID to identify two different time events
*/
void CRequestHandler::OnTimerEventL(TInt aTimerId)	
	{
	switch(aTimerId)
		{
	case EPowerStandByTimerEvent:
		TimeOutTimerEvent();
		break;

	default:
		__ASSERT_DEBUG(0, User::Panic(KAdaptationPanicCategory, KUnknownTimerEventId));
		}
	}
/**
Timer error handling

@param aError Time error code
@param aTimerId Time event ID to identify two different time events
*/	
TInt CRequestHandler::OnTimerError(TInt aError, TInt aTimerId)
	{
	switch(aTimerId)
		{
	case EPowerStandByTimerEvent:
		TimeOutTimerError(aError);
		break;

	default:
		__ASSERT_DEBUG(0, User::Panic(KAdaptationPanicCategory, KUnknownTimerEventId));
		}
	return KErrNone; // we have handled the error locally	
	}
	

