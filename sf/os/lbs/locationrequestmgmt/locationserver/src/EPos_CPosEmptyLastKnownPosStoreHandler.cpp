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
* Description:  	This class inherits the common functionalities for requests to the 
*				Location Monitor from EPos_CPosLocMonitorReqHandlerBase.h and also
*				implements the functions specific to empty last known position store req.
*
*/



#include "EPos_CPosEmptyLastKnownPosStoreHandler.h"
#include  "EPos_CPosCallbackTimer.h"


// ============================ CONSTANTS ===========================================================
#ifdef _DEBUG
_LIT(KTraceFileName, "EPos_CPosEmptyLastKnownPosStoreHandler.cpp");
#endif


// ============================== MEMBER FUNCTIONS ===================================================
CPosEmptyLastKnownPosStoreHandler* CPosEmptyLastKnownPosStoreHandler::NewL()
	{
	CPosEmptyLastKnownPosStoreHandler* self = new( ELeave ) CPosEmptyLastKnownPosStoreHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CPosEmptyLastKnownPosStoreHandler::CPosEmptyLastKnownPosStoreHandler()

	{
	CActiveScheduler::Add(this);
	}

void CPosEmptyLastKnownPosStoreHandler::ConstructL()
	{
    TCallBack timeoutCallBack(HandleTimeOut, this);
    iTimeoutTimer = CPosCallbackTimer::NewL(timeoutCallBack);
	}

CPosEmptyLastKnownPosStoreHandler::~CPosEmptyLastKnownPosStoreHandler()
	{
	
	if (iStatus==KRequestPending)
		{
		// Cancel the request sent to the location monitor
		Cancel();
		}
	// The requests on the queue are completed by the base class destructor
	
	delete iTimeoutTimer;
	}

/** 
 * EmptyLastKnownPosStoreL 
 * 		>>  Initiate a new request with the location monitor if the request queue is empty. 
 * 			Otherwise, check if the session id of any request on the queue matches that of the 
 *			new request [ie. a duplicate request from the client]. If it matches panic the client. 
 * 			If not add the new request to the queue. 		 
 *
 * @param  aLocMonSession    - The handle to the session with the location monitor passed on from
 * 							   CPosLocMonitorReqHandlerHub.
 * @param  aMessage  		 - The new request from the client 
 */
void CPosEmptyLastKnownPosStoreHandler::EmptyLastKnownPosStoreL(const RLbsLocMonitorSession& aLocMonSession, const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosEmptyLastKnownPosStoreHandler::EmptyLastKnownPosStoreL", __LINE__)

	if (!(aLocMonSession.Handle()))
		{
		// Session with the location monitor is not found
		RequestComplete(aMessage, KErrCouldNotConnect);
		return;
		}
	// Check the session id of the new request before adding to the queue of requests.
	CheckAndAddReqToQueueL(EReqOnSession, aMessage);
	
	if ((iLocMonitorReqQ.Count()>0) && !IsActive())
		{
		// Store the session handle for use in a subsequent cancel request
		iLocMonitorSession = const_cast<RLbsLocMonitorSession&> (aLocMonSession);

		// Initiate a new last known position request with the location monitor
		iLocMonitorSession.WipeOutDatabase(iStatus);
		SetActive();
		
	    // Start timer if necessary
	    if (KEmptyLastKnownPosStoreTimeOut.Int64()>0)
	        {
	        DEBUG_TRACE("CPosEmptyLastKnownPosStoreHandler::EmptyLastKnownPosStoreL() Start Timeout Timer", __LINE__)
	        iTimeoutTimer->StartTimer(KEmptyLastKnownPosStoreTimeOut);
	        }
	    
		}

	}

/** 
 * CancelEmptyLastKnownPosStoreL 
 * 		>> Cancel the outstanding request with the location monitor if there is only one request on the queue. 
  *		>> Otherwise just complete the cancel request with KErrNone, remove the original request from the queue
 * 		   and complete it with KErrCancel.
 * @param  aMessage  		 - The cancel request from the client 
 */
void CPosEmptyLastKnownPosStoreHandler::CancelEmptyLastKnownPosStoreL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosEmptyLastKnownPosStoreHandler::CancelEmptyLastKnownPosStoreL", __LINE__)

	// The subsession with the location monitor is not found
	if (!(iLocMonitorSession.Handle()))
		{
		RequestComplete(aMessage, KErrCouldNotConnect); //TODO - Validate KErrCouldNotConnect 
		return;
		}
	
	// Call CancelRequest inherited from the baseclass
	CancelRequest(EReqOnSession, aMessage);

	}

/** 
 * RunL 
 * 		>> Complete all the requests on the queue.
 */
void CPosEmptyLastKnownPosStoreHandler::RunL()
	{
	// Cancel the timeout timer
	iTimeoutTimer->Cancel();
	
	// Complete all the requests in the queue with the result 
	// of the empty last known position store operation
	QRequestsComplete(iStatus.Int());

	}

TInt CPosEmptyLastKnownPosStoreHandler::RunError(TInt aError)
	{
	return aError;
	}

/** 
 * DoCancel 
 * 		>> Cancel the active object.
 */
void CPosEmptyLastKnownPosStoreHandler::DoCancel()
	{
	__ASSERT_DEBUG((iLocMonitorSession.Handle())!=NULL, DebugPanic(EPosServerPanicPositionerNotInitialized));
	
	// Cancel the timer as the request with the location monitor is going to be cancelled
	iTimeoutTimer->Cancel();

	DEBUG_TRACE("calling RLbsAreaPositioner::CancelDatabaseWipeOut()", __LINE__)
	TInt err = iLocMonitorSession.CancelDatabaseWipeOut();
	// As the cancel request is immediately completed, this return value 
	// is not useful.
	}


/** 
 * HandleTimeOut 
 * 		>> Complete all the requests on the queue with KErrTimedOut.
 * @param  aRequestHandler - self pointer used to call the appropriate timeout handling method
 */
TInt CPosEmptyLastKnownPosStoreHandler::HandleTimeOut(TAny* aRequestHandler)
    {
    DEBUG_TRACE("CPosEmptyLastKnownPosStoreHandler::HandleTimeOut()", __LINE__)    
    
    CPosEmptyLastKnownPosStoreHandler* self = reinterpret_cast<CPosEmptyLastKnownPosStoreHandler*>(aRequestHandler);
    // The request with the location monitor has timed out. So complete all the outstanding 
    // requests with KErrTimedOut
    self->QRequestsComplete(KErrTimedOut);
    
    // Cancel the pending request with the location monitor
    self->Cancel();

    return KErrNone;
    
    }

