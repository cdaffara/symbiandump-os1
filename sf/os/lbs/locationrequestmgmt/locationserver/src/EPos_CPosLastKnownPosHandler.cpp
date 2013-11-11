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
*				implements the functions specific to Last Known Position request.
*
*/



#include "EPos_CPosLastKnownPosHandler.h"
#include  "EPos_CPosCallbackTimer.h"

// ============================ CONSTANTS ===========================================================
#ifdef _DEBUG
_LIT(KTraceFileName, "EPos_CPosLastKnownPosHandler.cpp");
#endif

// ============================== MEMBER FUNCTIONS ===================================================
CPosLastKnownPosHandler* CPosLastKnownPosHandler::NewL()
	{
	CPosLastKnownPosHandler* self = new( ELeave ) CPosLastKnownPosHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CPosLastKnownPosHandler::CPosLastKnownPosHandler()
	{
	CActiveScheduler::Add(this);
	}

void CPosLastKnownPosHandler::ConstructL()
	{
    TCallBack timeoutCallBack(HandleTimeOut, this);
    iTimeoutTimer = CPosCallbackTimer::NewL(timeoutCallBack);
	}

CPosLastKnownPosHandler::~CPosLastKnownPosHandler()
	{
	
	if (iStatus==KRequestPending)
		{
		// Cancel the request sent to the location monitor
		Cancel();
		}
	
	if (iLocMonPositioner.SubSessionHandle())
		{
		iLocMonPositioner.Close();
		}
    
	delete iTimeoutTimer;
	
	// The requests on the queue are completed by the base class destructor
	}

/** 
 * GetLastKnownPosL 
 * 		>>  Initiate a new request with the location monitor if the request queue is empty. 
 * 			Otherwise, check if the session and subsession id of any request on the queue matches that of the 
 *			new request [ie. a duplicate request from the client]. If it matches panic the client. 
 * 			If not add the new request to the queue. 
 *
 * @param  aLocMonSession    - The handle to the session with the location monitor passed on from
 * 							   CPosLocMonitorReqHandlerHub.
 * @param  aMessage  		 - The new request from the client 
 */
void CPosLastKnownPosHandler::GetLastKnownPosL(RLbsLocMonitorSession& aLocMonSession, const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLastKnownPosHandler::RequestPosL", __LINE__)

	if ( !(aLocMonSession.Handle()) )
		{
		// Session with the location monitor is not found
		RequestComplete(aMessage, KErrCouldNotConnect);
		return;
		}

	if ( !(iLocMonPositioner.SubSessionHandle()) )
		{
		iLocMonPositioner.OpenL(aLocMonSession);
		}
	
	CheckAndAddReqToQueueL(EReqOnSubSession, aMessage);
	
	if ((iLocMonitorReqQ.Count()>0) && !IsActive())
		{
		// Initiate a new last known position request with the location monitor
		iLocMonPositioner.GetLastKnownPosition(iPositionInfo, iStatus);
		SetActive();
		
	    // Start timer if necessary
	    if (KLastKnownPosTimeOut.Int64()>0)
	        {
	        DEBUG_TRACE("CPosLastKnownPosHandler::GetLastKnownPosL() Start Timeout Timer", __LINE__)
	        iTimeoutTimer->StartTimer(KLastKnownPosTimeOut);
	        }
		}

	}

/** 
 * CancelGetLastKnownPosL 
 * 		>> Cancel the outstanding request with the location monitor if there is only one request on the queue. 
 *		>> Otherwise just complete the cancel request with KErrNone, remove the original request from the queue
 * 		   and complete it with KErrCancel.
 * @param  aMessage  		 - The cancel request from the client 
 */
void CPosLastKnownPosHandler::CancelGetLastKnownPosL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLastKnownPosHandler::CancelGetLastKnownPosL", __LINE__)

	
	if ( iLocMonPositioner.SubSessionHandle() ) 
		{
		// Call CancelRequest inherited from the baseclass
		CancelRequest(EReqOnSubSession, aMessage);
		}
	else
		{
		// The subsession with the location monitor is not found
		RequestComplete(aMessage, KErrCouldNotConnect); //TODO - KErrCouldNotConnect ?
		}
	}


/** 
 * RunL 
 * 		>> Complete all the requests on the queue.
 */
void CPosLastKnownPosHandler::RunL()
	{
	
	// Cancel the timeout timer
	iTimeoutTimer->Cancel();
	
	// Serving all the outstanding requests based on the current update
	// from the Location Monitor
	while (iLocMonitorReqQ.Count()>0)
		{
		// Retrieve the next request to be serviced
		TInt numReqs = iLocMonitorReqQ.Count()-1;
		 
		if (iStatus.Int()==KErrNone)
			{
			// |TPositionClassTypeBase|TPositionInfoBase|TPositionInfo|...
			// No matter what the type of the class derived from TPositionInfo, the location monitor
			// just returns TPositionInfo and we write only the part |TPositionInfoBase|TPositionInfo|
			// skipping the TPositionClassTypeBase as it contains the position class type and size
			// that should not be corrupted.
			
			const TUint8* startAddress = reinterpret_cast <const TUint8*>(&iPositionInfo)+sizeof(TPositionClassTypeBase);
			TInt chunkSize = sizeof(TPositionInfo)-sizeof(TPositionClassTypeBase);
			TPtr8 copyFromDesc(const_cast<TUint8*>(startAddress),chunkSize,chunkSize);
			
			TInt err = iLocMonitorReqQ[numReqs].Write(0,copyFromDesc,sizeof(TPositionClassTypeBase));
			RequestComplete(iLocMonitorReqQ[numReqs], err);
			
			}
		else
			{
			// Complete the client request with aReason
			RequestComplete(iLocMonitorReqQ[numReqs],iStatus.Int());
			}
		
		// Remove the request that has just been serviced [last element]
		iLocMonitorReqQ.Remove(numReqs);
		}

	
	// Close the subsession with the location monitor when we receive the last known position from it
	iLocMonPositioner.Close();
	}

TInt CPosLastKnownPosHandler::RunError(TInt aError)
	{
	return aError;
	}


/** 
 * DoCancel 
 * 		>> Cancel the active object.
 */
void CPosLastKnownPosHandler::DoCancel()
	{
	__ASSERT_DEBUG((iLocMonPositioner.SubSessionHandle())!=NULL, DebugPanic(EPosServerPanicPositionerNotInitialized));
	
	// Cancel the timer as the request with the location monitor is going to be cancelled
	iTimeoutTimer->Cancel();
	
	DEBUG_TRACE("calling RLbsAreaPositioner::CancelGetLastKnownPosition()", __LINE__)
	TInt err = iLocMonPositioner.CancelGetLastKnownPosition();
	// As the cancel request is immediately completed, this return value 
	// is not useful.
	}

/** 
 * NotifyOnEmptyLastKnownPosStoreReq 
 * 		>> Cancel the outstanding get last known position area requests with KErrCancel
 * 		>> as an empty last known position store request has been issued.
 */
void  CPosLastKnownPosHandler::NotifyOnEmptyLastKnownPosStoreReq()
	{
	// Complete all the requests on the queue with KErrCancel
	QRequestsComplete(KErrCancel);
	
	if (iStatus==KRequestPending)
		{
		Cancel();
		}
	}

/** 
 * HandleTimeOut 
 * 		>> Complete all the requests on the queue with KErrTimedOut.
 * @param  aRequestHandler - self pointer used to call the appropriate timeout handling method
 */
TInt CPosLastKnownPosHandler::HandleTimeOut(TAny* aRequestHandler)
    {

    DEBUG_TRACE("CPosLastKnownPosHandler::HandleTimeOut()", __LINE__)    
    
    CPosLastKnownPosHandler* self = reinterpret_cast<CPosLastKnownPosHandler*>(aRequestHandler);
    // The request with the location monitor has timed out. So complete all the outstanding 
    // requests with KErrTimedOut
    self->QRequestsComplete(KErrTimedOut);
    // Cancel the pending request with the location monitor
    self->Cancel();

    return KErrNone;
    
    }


