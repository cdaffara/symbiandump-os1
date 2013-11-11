/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include "testEPos_CPosLastKnownPosAreaHandler.h"
#include  "EPos_CPosCallbackTimer.h"


// CONSTANTS
#ifdef _DEBUG
_LIT(KTraceFileName, "testEPos_CPosLastKnownPosAreaHandler.cpp");
#endif


CPosLastKnownPosAreaHandler* CPosLastKnownPosAreaHandler::NewL()
	{
	CPosLastKnownPosAreaHandler* self = new( ELeave ) CPosLastKnownPosAreaHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CPosLastKnownPosAreaHandler::CPosLastKnownPosAreaHandler()
	{
	CActiveScheduler::Add(this);
	}

void CPosLastKnownPosAreaHandler::ConstructL()
	{
    TCallBack timeoutCallBack(HandleTimeOut, this);
    iTimeoutTimer = CPosCallbackTimer::NewL(timeoutCallBack);
	}

CPosLastKnownPosAreaHandler::~CPosLastKnownPosAreaHandler()
	{
	
	if (iStatus==KRequestPending)
		{
		// Cancel the request sent to the location monitor
		Cancel();
		}

	if ( iLocMonAreaPositioner.SubSessionHandle() )
		{
		iLocMonAreaPositioner.Close();
		}
	
	delete iTimeoutTimer;
	
	// The requests on the queue are completed by the base class destructor
	}

void CPosLastKnownPosAreaHandler::GetLastKnownPosAreaL(RLbsLocMonitorSession& aLocMonSession, const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLastKnownPosAreaHandler::GetLastKnownPosAreaL", __LINE__)

	if ( !(aLocMonSession.Handle()))
		{
		// Session with the location monitor is not found
		RequestComplete(aMessage, KErrCouldNotConnect);
		return;
		}

	if (!(iLocMonAreaPositioner.SubSessionHandle()))
		{
		TInt err = iLocMonAreaPositioner.OpenL(aLocMonSession);
		if (err != KErrNone)
			{
			RequestComplete(aMessage, err);
			return;
			}
		}
	
	// Copy the buffers from the address space of the client
	HBufC8* lkposclientbuf = Global::CopyClientBuffer8LC(aMessage, KParamLKPAreaReqPos);
	HBufC8* lkposareaclientbuf = Global::CopyClientBuffer8LC(aMessage, KParamLKPAreaReqArea);

	TPositionInfoBase& posInfoBase = reinterpret_cast<TPositionInfoBase&>
	(const_cast<TUint8&>(*lkposclientbuf->Ptr()));
	TPositionAreaInfoBase& posAreaInfoBase = reinterpret_cast<TPositionAreaInfoBase&>
	(const_cast<TUint8&>(*lkposareaclientbuf->Ptr()));

	Global::ValidatePositionClassBufferL(posInfoBase, lkposclientbuf->Des());
	Global::ValidatePositionClassTypeL(posInfoBase, EPositionInfoClass, KErrArgument);
	Global::ValidatePositionClassBufferL(posAreaInfoBase, lkposareaclientbuf->Des());

	switch (posAreaInfoBase.PositionClassType())
		{
		case EPositionAreaInfoClass:
			Global::ValidatePositionClassTypeL(posAreaInfoBase, EPositionAreaInfoClass, KErrArgument);
			break;
		case (EPositionAreaExtendedInfoClass+EPositionAreaInfoClass): //TODO Check LbsAreaInfo.cpp [constructor]
			Global::ValidatePositionClassTypeL(posAreaInfoBase, (EPositionAreaExtendedInfoClass+EPositionAreaInfoClass), KErrArgument);
			break;
		default:
			User::Leave(KErrArgument); 								//TODO - Check if this is correct ?
			break;
		}

	CleanupStack::PopAndDestroy(lkposareaclientbuf);
	CleanupStack::PopAndDestroy(lkposclientbuf);

	// Check the identity of the subsession and add the request to the queue
	CheckAndAddReqToQueueL(EReqOnSubSession, aMessage);
	
	if ((iLocMonitorReqQ.Count()>0) && !(this->iStatus==KRequestPending) )
		{
		TPosAreaReqParams aParameters;
		// Always ask for extended information from the location monitor
		// as initiating a new request for clients that require extended information
		// is inefficient
		aParameters.iPositionAreaType = (EPositionAreaExtendedInfoClass+EPositionAreaInfoClass);
		
		// Initiate a new last known position request with the location monitor
		iLocMonAreaPositioner.GetLastKnownPositionArea(iPositionInfo, iPositionAreaInfo, aParameters, iStatus);
		SetActive();
		
	    // Start timer if necessary
	    if (KLastKnownPosAreaTimeOut.Int64()>0)
	        {
	        DEBUG_TRACE("CPosLastKnownPosHandler::GetLastKnownPosAreaL() Start Timeout Timer", __LINE__)
	        iTimeoutTimer->StartTimer(KLastKnownPosAreaTimeOut);
	        }
		}

	}

void CPosLastKnownPosAreaHandler::CancelGetLastKnownPosAreaL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLastKnownPosAreaHandler::CancelGetLastKnownPosAreaL", __LINE__)

	// The subsession with the location monitor is not found
	if ( !(iLocMonAreaPositioner.SubSessionHandle()) )
		{
		RequestComplete(aMessage, KErrCouldNotConnect); //TODO - KErrCouldNotConnect ?
		return;
		}
	
	// Call CancelRequest inherited from the baseclass
	CancelRequest(EReqOnSubSession, aMessage);

	}


void  CPosLastKnownPosAreaHandler::NotifyOnEmptyLastKnownPosStoreReq()
	{
	// Complete all the requests on the queue with KErrCancel
	QRequestsComplete(KErrCancel);
	
	if (iStatus==KRequestPending)
		{
		Cancel();
		}
	}

void CPosLastKnownPosAreaHandler::RunL()
	{
	
	// Cancel the timeout timer
	iTimeoutTimer->Cancel();
	
	// Serving all the outstanding requests based on the current update
	// from the Location Monitor
	while (iLocMonitorReqQ.Count()>0)
		{
		TInt numReqs = iLocMonitorReqQ.Count()-1;
		// Retrieve the next request to be serviced [first element in the queue - FIFO]
		if (iStatus.Int()==KErrNone)
			{
			// Copy the buffers from the address space of the client
			const TUint8* startAddress = reinterpret_cast <const TUint8*>(&iPositionInfo)+sizeof(TPositionClassTypeBase);
			TInt chunkSize = sizeof(TPositionInfo)-sizeof(TPositionClassTypeBase);
			TPtr8 copyFromDesc(const_cast<TUint8*>(startAddress),chunkSize,chunkSize);
			
			TInt err = Global::Write(iLocMonitorReqQ[numReqs],KParamLKPAreaReqPos,copyFromDesc,sizeof(TPositionClassTypeBase));
			//TInt err = iLocMonitorReqQ[numReqs].Write(KParamLKPAreaReqPos,copyFromDesc,sizeof(TPositionClassTypeBase));

			// To identify the type of the request - last known position area / last known position area + extended info.
			HBufC8* lkposareaclientbuf = Global::CopyClientBuffer8LC(iLocMonitorReqQ[numReqs], KParamLKPAreaReqArea);
			TPositionAreaInfoBase& posAreaInfoBase = reinterpret_cast<TPositionAreaInfoBase&>(const_cast<TUint8&>(*lkposareaclientbuf->Ptr()));
			startAddress = reinterpret_cast <const TUint8*>(&iPositionAreaInfo)+sizeof(TPositionClassTypeBase);
			
			switch (posAreaInfoBase.PositionClassType())
				{
				case EPositionAreaInfoClass:
					chunkSize = sizeof(TPositionAreaInfo)-sizeof(TPositionClassTypeBase);
					copyFromDesc.Set(const_cast<TUint8*>(startAddress),chunkSize,chunkSize);
					//err = iLocMonitorReqQ[numReqs].Write(KParamLKPAreaReqArea,copyFromDesc,sizeof(TPositionClassTypeBase));
					err = Global::Write(iLocMonitorReqQ[numReqs],KParamLKPAreaReqArea,copyFromDesc,sizeof(TPositionClassTypeBase));
					RequestComplete(iLocMonitorReqQ[numReqs], err);
					break;
				case (EPositionAreaExtendedInfoClass+EPositionAreaInfoClass): //TODO Check LbsAreaInfo.cpp [constructor]
					chunkSize = sizeof(TPositionAreaExtendedInfo)-sizeof(TPositionClassTypeBase);
					copyFromDesc.Set(const_cast<TUint8*>(startAddress),chunkSize,chunkSize);
					err = Global::Write(iLocMonitorReqQ[numReqs],KParamLKPAreaReqArea,copyFromDesc,sizeof(TPositionClassTypeBase));
					//err = iLocMonitorReqQ[numReqs].Write(KParamLKPAreaReqArea,copyFromDesc,sizeof(TPositionClassTypeBase));		
					RequestComplete(iLocMonitorReqQ[numReqs], err);
					break;
				default:
					RequestComplete(iLocMonitorReqQ[numReqs], KErrArgument);								//TODO - Check if this is correct ?
					break;
				}
			// Destroy the buffer used to hold the client's data
			CleanupStack::PopAndDestroy(lkposareaclientbuf);
			
			}
		else
			{
			// Complete the client request with aReason
			RequestComplete(iLocMonitorReqQ[numReqs],iStatus.Int());
			}

		// Remove the request that has just been serviced [last element]
		iLocMonitorReqQ.Remove(numReqs);
		}
	
	// Close the subsession with the location monitor when we receive the last known position area from it
	iLocMonAreaPositioner.Close();

	}

TInt CPosLastKnownPosAreaHandler::RunError(TInt aError)
	{
	return aError;
	}

void CPosLastKnownPosAreaHandler::DoCancel()
	{
	// Cancel the timer as the request with the location monitor is going to be cancelled
	iTimeoutTimer->Cancel();
	
	DEBUG_TRACE("calling RLbsAreaPositioner::CancelGetLastKnownPosition()", __LINE__)
	__ASSERT_DEBUG((iLocMonAreaPositioner.SubSessionHandle())!=NULL, DebugPanic(EPosServerPanicPositionerNotInitialized));
	
	TInt err = iLocMonAreaPositioner.CancelGetLastKnownPositionArea();
	// As the cancel request is immediately completed, this return value 
	// is not useful.
	}

TInt CPosLastKnownPosAreaHandler::HandleTimeOut(TAny* aRequestHandler)
    {

    DEBUG_TRACE("CPosLastKnownPosAreaHandler::HandleTimeOut()", __LINE__)    
    
    CPosLastKnownPosAreaHandler* self = reinterpret_cast<CPosLastKnownPosAreaHandler*>(aRequestHandler);
    // The request with the location monitor has timed out. So complete all the outstanding 
    // requests with KErrTimedOut
    self->QRequestsComplete(KErrTimedOut);
    // Cancel the pending request with the location monitor
    self->Cancel();

    return KErrNone;
    
    }

