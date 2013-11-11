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


#include <e32base.h>
#include <lbsareainfo.h>
#include "lbsdevloggermacros.h"
#include "clbslocmonitorrequesthandler.h"
#include "clbslocmonitorutils.h"

const TInt KLbsLocMonitorMaxReadArraySize = 10;
const TInt KLbsLocMonitorMaxWriteArraySize = 10;
const TInt KLbsLocMonDbAccessMaxTime = 4000000; // 4 seconds
const TInt KObserverTimerId = 1;

CLbsLocMonitorRequestHandler* CLbsLocMonitorRequestHandler::NewL()	
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::NewL");
	CLbsLocMonitorRequestHandler* self = new (ELeave) CLbsLocMonitorRequestHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CLbsLocMonitorRequestHandler::ConstructL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::ConstructL");
	iDbOperationTimer = CLbsCallbackTimer::NewL(*this);
	
	// Allocate space for a queue of 10 client messages
	iRequestQueue.ReserveL(KLbsLocMonitorMaxReadArraySize);

	// Pre-allocate space for a queue of positions waiting
	// to be written to the database
	iPositionsQueue.ReserveL(KLbsLocMonitorMaxWriteArraySize);

	// Allocate space for 1 pointer to area info source object (Global Cell Id)
	iCurrentAreaInfo.ReserveL(ELastAreaInfoType);
	TLbsLocMonitorAreaInfoGci* locMonitorAreaInfoGci = new(ELeave) TLbsLocMonitorAreaInfoGci;
	CleanupStack::PushL(locMonitorAreaInfoGci);
	iCurrentAreaInfo.InsertL(static_cast<TLbsLocMonitorAreaInfoBase*>(locMonitorAreaInfoGci), EGlobalCellIdType);
	CleanupStack::Pop(locMonitorAreaInfoGci);
	
	iDb.OpenL();
	}
	
CLbsLocMonitorRequestHandler::~CLbsLocMonitorRequestHandler()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::~CLbsLocMonitorRequestHandler");
	// Cancel the async request
	Cancel();

	iDbOperationTimer->Cancel();
	delete iDbOperationTimer;
	
	// Complete every message in the queue and remove it
	TInt count;
	for(count = iRequestQueue.Count(); count > 0; count--)
		{
		iRequestQueue[count-1].Complete(KErrCancel);
		iRequestQueue.Remove(count-1);
		}
	
	iPositionsQueue.Reset();
	iPositionsQueue.Close();
	
	iRequestQueue.Reset();
	iRequestQueue.Close();
	
	iCurrentAreaInfo.ResetAndDestroy();
	
	// close the handle with the database
	iDb.Close();
	}

CLbsLocMonitorRequestHandler::CLbsLocMonitorRequestHandler():
								CActive(EPriorityStandard),
								iAreaInfoAvailable(EFalse)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CLbsLocMonitorRequestHandler");
	CActiveScheduler::Add(this);
	}

/*
This method is called by subsessions of the location monitor to request
a database read operation. The request may be processed or queued.
*/
void CLbsLocMonitorRequestHandler::PositionRequestL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::PositionRequestL");
	__ASSERT_DEBUG((aMessage.Function() == EGetLastKnownPosition) ||
				   (aMessage.Function() == EGetLastKnownPositionArea),
				   User::Invariant());

	if(iRequestQueue.Count()< KLbsLocMonitorMaxReadArraySize)
		{
		iRequestQueue.AppendL(aMessage);
		}
	else
		{
		//Unable to handle more requests
		aMessage.Complete(KErrServerBusy);
		}

	if (!IsActive())
		{
		ProcessNextRequest();	
		}
	}
	
/* Method called to request that a position is
stored in the database. This method is called
as a consequence of a new position being observed
on an internal position bus.
*/
void CLbsLocMonitorRequestHandler::PositionAvailableL(const TPosition& aPosition)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::PositionAvailableL");
	if(iPositionsQueue.Count() < KLbsLocMonitorMaxWriteArraySize && iAreaInfoAvailable)
		{
		TInt err = iPositionsQueue.Append(aPosition);
		if( err!= KErrNone )
			{
			LBSLOG(ELogP1,"->iPositionsQueue.Append failed!!");
			}
		}
	else
		{
		// Unable to handle more positions or we don't have valid area parameters
		// The position will be lost.
		}

	// Store the position in the DB
	if (!IsActive())
		{
		ProcessNextRequest();	
		}
	}

/* Method called to request that a last known position sent from
the Location Server is stored in the database.
*/
void CLbsLocMonitorRequestHandler::StoreLastKnownPosition(const TPosition& aPosition)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::StoreLastKnownPosition");
	iLastKnownPositionAvailable = ETrue;
	iLastKnownPosition = aPosition;
	
	if (!IsActive())
		{
		ProcessNextRequest();	
		}
	}

/*
This method is called by sessions of the location monitor to inform that
the database is about to be deleted. All of the 'read' requests are completed.
*/
TInt CLbsLocMonitorRequestHandler::DeleteDatabaseL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::DeleteDatabaseL");
	if(IsActive())
		{
		// Terminate ongoing request to the DB
		Cancel();
		}

	// Complete all requests in the queue
	for (TInt index = iRequestQueue.Count()-1; index >=0; index--)
		{
		CompleteClientRequest(index, KErrNotFound);
		}
		
	// Clear all positions currenly waiting to be stored
	for (TInt index = iPositionsQueue.Count()-1; index >=0; index--)
		{
		iPositionsQueue.Remove(index);
		}

	// Clear indication that a last known position
	// is available for storage
	iLastKnownPositionAvailable = EFalse;

	// Send deletion request to the DB interface
	return iDb.ClearDatabase();
	}

/*
Cancel a previous request to delete the database.
*/
void CLbsLocMonitorRequestHandler::CancelDeleteDatabaseL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CancelDeleteDatabaseL");
	// Cancel is not possible as deletion is done
	// synchronously. This method just completes the
	// client request with KErrNotFound.
	aMessage.Complete(KErrNotFound);
	}

/*
Called when a subsession is closed. Its outstanding requests
have to be completed/destroyed.
*/
void CLbsLocMonitorRequestHandler::ClientTerminated(const CLbsLocMonitorUtils::sessionAndSubSessionId& aSubsessionId)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::ClientTerminated");
	// Find the requests from this client, complete them and delete them
	if (iRequestQueue.Count()>0)
		{
		TInt index = iRequestQueue.FindReverse(aSubsessionId, CLbsLocMonitorUtils::MatchClient);
		if (index != KErrNotFound)
			{
		 	while (index != KErrNotFound)
				{
				CompleteClientRequest(index, KErrServerTerminated);
				// check if there is another one from the same client
				index = iRequestQueue.FindReverse(aSubsessionId, CLbsLocMonitorUtils::MatchClient);
				}

			// Cancel ongoing read operation if no client requests are left
			// in the queue that are interested in the ongoing read operation
			if (IsActive())
				{
				if (ELocMonDbGetPosArea == iOperationInProgress) 
					{
					if(KErrNotFound == iRequestQueue.FindReverse(static_cast<TInt>(EGetLastKnownPositionArea), CLbsLocMonitorUtils::MatchOperation))
						{
						Cancel();
						// There could be outstanding clients interested in a different operation
						ProcessNextRequest();	
						}
					}
				else if (ELocMonDbGetLast == iOperationInProgress)
					{
					if (iAreaInfoAvailable)
						{
						if(KErrNotFound == iRequestQueue.FindReverse(static_cast<TInt>(EGetLastKnownPosition), CLbsLocMonitorUtils::MatchOperation))
							{
							// No other client has requests for the ongoing operation
							Cancel();
							// There could be outstanding clients interested in a different operation
							ProcessNextRequest();	
							}
						}
					else
						{
						// When no area info available, both the clients wanting GLKP and GLKPA are interested in the results
						// of a the ongoing ELocMonDbGetLast DB operation. Only cancel if there are no clients of neither type.
						if((KErrNotFound == iRequestQueue.FindReverse(static_cast<TInt>(EGetLastKnownPosition), CLbsLocMonitorUtils::MatchOperation) &&
						   (KErrNotFound == iRequestQueue.FindReverse(static_cast<TInt>(EGetLastKnownPositionArea), CLbsLocMonitorUtils::MatchOperation))))
							{
							Cancel();
							// There could be outstanding clients interested in a different operation
							ProcessNextRequest();	
							}	
						}				
					}
				// else {no other async client operations need cancelling}
				}
			}
		}
	}

/*
Cancel a previous request from the same client to retrieve the
last position stored in the database.
*/
void CLbsLocMonitorRequestHandler::CancelPositionRequestL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CancelPositionRequestL");
	TInt opcodeCancelled(0);
	EDbOperationType dbOperationCancelled = ELocMonDbOperationNone;
	
	// What is this message actually cancelling ?
	if(aMessage.Function() == ECancelGetLastKnownPosition)
		{
		opcodeCancelled = EGetLastKnownPosition;
		dbOperationCancelled = ELocMonDbGetLast;
		}
	else if (aMessage.Function() == ECancelGetLastKnownPositionArea)
		{
		opcodeCancelled = EGetLastKnownPositionArea;
		dbOperationCancelled = ELocMonDbGetPosArea;		
		}
	else
		{
		// Should never happen
		__ASSERT_DEBUG(EFalse, User::Invariant());
		aMessage.Complete(KErrNotSupported);
		return;
		}
	
	// There should be a queued request originating in the same client whose type
	// is the one cancelled by aMessage
	TIdentityRelation<RMessage2> cancelToRequestMatcher(CLbsLocMonitorUtils::MatchCancelAndRequest);
	TInt index = iRequestQueue.Find(aMessage, cancelToRequestMatcher);
	if (index != KErrNotFound)
		{
		CompleteClientRequest(index, KErrCancel);
		// complete the synchronous cancel request
		aMessage.Complete(KErrNone);
		
		// If the type of the read operation currently in progress is 
		// the one being cancelled, and there are no more requests for that
		// operation in the queue, send a cancel request to the DB.
		if (IsActive() &&
		   (iOperationInProgress == dbOperationCancelled) &&
		   (KErrNotFound == iRequestQueue.Find(opcodeCancelled, CLbsLocMonitorUtils::MatchOperation)))
			{
			Cancel();	
			}
		else
			{
			// Let the read operation continue on behalf of the 
			// remaining requestors of the same read operation.
			}
		}
	else
		{
		// Cient sent a cancel and there is no request to cancel.
		// Nothing to do.
		aMessage.Complete(KErrNotFound);
		}
	}

/*
 Called when a DB operation completes.
 
 If it was a 'read' operation, the positon returned
 by the DB is used to complete all of the requests 
 in the queue that would prompt the same read operation.
*/
void CLbsLocMonitorRequestHandler::RunL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::RunL");
	TInt err = iStatus.Int();
	iDbOperationTimer->Cancel();
	
	if(KErrNone == err)
		{
		switch (iOperationInProgress)
			{
			case ELocMonDbSavePos:
				// Position at index 0 of the array has been stored.
				// Remove it from the queue.
				iPositionsQueue.Remove(0);
			break;

			case ELocMonDbGetPosArea:
			case ELocMonDbGetLast:
				iPositionInfo.SetPosition(iPosition);
				CompleteSameTypeMessages(iOperationInProgress, KErrNone);
				// When no area information is known, use the results of a call to GetLastPosition
				// to complete also the clients that requested GetLastKnownPositionArea
				if ((!iAreaInfoAvailable) && (ELocMonDbGetLast == iOperationInProgress))
					{
					CompleteSameTypeMessages(ELocMonDbGetPosArea, KErrNone);
					}
			break;

			default:
				// nothing to do 
			break;
			}
		}
	else if (KErrCancel == err)
		{
		// If this happens, that is because a cancel was sent earlier.
		// The requests affected have been completed and removed from the queue
		// and there are no outstanding requests of the type just cancelled.
		}
	else
		{
		HandleDbAccessError(err);
		}

	// Check if there are any requests left (different operation)
	// 
	ProcessNextRequest();
	}
	
/* This method issues the read operation on the DB
that is required by the client message at position 0 of
the iRequestQueue.
*/
void CLbsLocMonitorRequestHandler::ProcessNextRequest()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::ProcessNextRequest");
	__ASSERT_DEBUG(!IsActive(), User::Invariant());
	
	ClearRequestVariables();
	iOperationInProgress = ELocMonDbOperationNone;
	
	// Process queued read operations from clients first
	if (iRequestQueue.Count() > 0)
		{	
		// handle the first request in the queue
		switch(iRequestQueue[0].Function())
			{
			case EGetLastKnownPosition:
				iOperationInProgress = ELocMonDbGetLast;
				iDbOperationTimer->EventAfter(TTimeIntervalMicroSeconds32(KLbsLocMonDbAccessMaxTime), KObserverTimerId);
				iDb.GetLastStoredPosition(iPosition, iStatus);
				SetActive();
			break;
			
			case EGetLastKnownPositionArea:
				if (iAreaInfoAvailable)
					{
					iOperationInProgress = ELocMonDbGetPosArea;
					iDb.GetPosition(iPosition, iCurrentAreaInfo, iAreaMatchInfo, iStatus);					
					}
				else
					{
					iOperationInProgress = ELocMonDbGetLast;
					iDb.GetLastStoredPosition(iPosition, iStatus);	
					}
				iDbOperationTimer->EventAfter(TTimeIntervalMicroSeconds32(KLbsLocMonDbAccessMaxTime), KObserverTimerId);
				SetActive();
			break;

			default:
				//Mesage of unexpected type !
				__ASSERT_DEBUG(EFalse, User::Invariant());
				CompleteClientRequest(0,KErrNotSupported);
			break;
			}
		}

	// Process write operations if no read operation was
	// started.
	if (!IsActive())
		{
		if (iLastKnownPositionAvailable)
			{
			iLastKnownPositionAvailable = EFalse;
			
			// Check that we have valid data before trying to save to the database - ignore otherwise.
			if (iAreaInfoAvailable)
			    {
			    iOperationInProgress = ELocMonDbSaveLastPos;
			    iDb.SavePosition(iLastKnownPosition, iCurrentAreaInfo, ETrue, iStatus);
			    SetActive();
			    }
			}
		else if (iPositionsQueue.Count() > 0)
			{
			iOperationInProgress = ELocMonDbSavePos;
			iDb.SavePosition(iPositionsQueue[0], iCurrentAreaInfo, ETrue, iStatus);
			SetActive();
			}	
		}
	}

//
// Complete a client request. If aCompletionCode indicates no error,
// this method sets into the request (RMessage2) the information it
// requested.
//
void CLbsLocMonitorRequestHandler::CompleteClientRequest(TInt aIndex, TInt aCompletionCode)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteClientRequest");
	if ((aIndex < 0) || (aIndex >= iRequestQueue.Count()))
		{
		ASSERT(EFalse);
		return;
		}
	
	if (KErrNone == aCompletionCode)
		{
		switch(iRequestQueue[aIndex].Function())
			{
			case EGetLastKnownPositionArea:
				{
				LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteClientRequest - EGetLastKnownPositionArea");
				// Copy over to client space position data equal to the size 
				// of the iPositionInfo object, minus the class type at the 
				// start. This will leave the client position info class
				// type as it was, but it will get the TPosition into the
				// right position of the client buffer.
				const TUint8* start = reinterpret_cast<const TUint8*>(&iPositionInfo) + sizeof(TPositionClassTypeBase);
				TInt length = sizeof(TPositionInfo) - sizeof(TPositionClassTypeBase);
				TPtr8 ptr(const_cast<TUint8*>(start), length, length);
				TInt err = iRequestQueue[aIndex].Write(0, ptr, sizeof(TPositionClassTypeBase)); // position at index 0
				
				// Read in the request parameters
				TPtr8 paramsPtr(reinterpret_cast<TUint8*>(&iRequestParams),
							sizeof(iRequestParams),
							sizeof(iRequestParams));
							
				MessageUtils::Read(iRequestQueue[aIndex], 2, paramsPtr); // params at index 2	

				// Copy over to client space the correct type of area information.
				// If the client has passed a TPositionAreaExtendedInfo, just copy
				// iAreaMatchInfo. Otherwise populate a TPositionAreaInfo and copy that
				// accross.
				switch (iRequestParams.iPositionAreaType)
					{
					case EPositionAreaInfoClass:
						{
						LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteClientRequest - EPositionAreaInfoClass");
						TPositionAreaInfo areaInfo(iAreaMatchInfo.Area());
						ptr.Set(reinterpret_cast<TUint8*>(&areaInfo),
							sizeof(areaInfo),
							sizeof(areaInfo));
						MessageUtils::Write(iRequestQueue[aIndex], 1, ptr); // area info at index 1
						}
					break;
									
					case (EPositionAreaExtendedInfoClass|EPositionAreaInfoClass):
						{
						LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteClientRequest - EPositionAreaExtendedInfoClass");						
						ptr.Set(reinterpret_cast<TUint8*>(&iAreaMatchInfo),
								sizeof(iAreaMatchInfo),
								sizeof(iAreaMatchInfo));
						MessageUtils::Write(iRequestQueue[aIndex], 1, ptr); // area info at index 1
						}
					break;
					

					default:
						__ASSERT_DEBUG(EFalse, User::Invariant());
					break;
					}
				
				break;
				}
			
			case EGetLastKnownPosition:
				{
				LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteClientRequest - EGetLastKnownPosition");				
				// Copy over to client space position data equal to the size 
				// of the iPositionInfo object, minus the class type at the 
				// start. This will leave the client position info class
				// type as it was, but it will get the TPosition into the
				// right position of the client buffer.
				const TUint8* start = reinterpret_cast<const TUint8*>(&iPositionInfo) + sizeof(TPositionClassTypeBase);
				TInt length = sizeof(TPositionInfo) - sizeof(TPositionClassTypeBase);
				TPtr8 ptr(const_cast<TUint8*>(start), length, length);
				iRequestQueue[aIndex].Write(0, ptr, sizeof(TPositionClassTypeBase)); // position at index 0
				break;
				}	

			
			default:
				__ASSERT_DEBUG(EFalse, User::Invariant());
			break;
			}
		}
	else
		{
		// Nothing to add to the RMessage2
		LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteClientRequest - !KErrNone");						
		}

	iRequestQueue[aIndex].Complete(aCompletionCode);
	// Dequeue completed request
	iRequestQueue.Remove(aIndex);
	}


/**
from CActive
Simply fwds any errors to the observers

@internalComponent
@released
*/
TInt CLbsLocMonitorRequestHandler::RunError(TInt aError)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::RunError");
	return aError;	
	}

/**
from CActive
Cancels the request for position update notification
It will eventually cancel an outstanding subscription request for this property handle.

@internalComponent
@released
*/
void CLbsLocMonitorRequestHandler::DoCancel()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::DoCancel");
	iDbOperationTimer->Cancel();
	//Cancel the last request sent to the DB
	switch (iOperationInProgress)
		{
		case ELocMonDbGetLast:
			iDb.CancelGetLastStoredPosition();
		break;
		
		case ELocMonDbGetPosArea:
			iDb.CancelGetPosition();
		break;
		
		case ELocMonDbSavePos:
		case ELocMonDbSaveLastPos:
			iDb.CancelSavePosition();
		break;

		default:
			//intentionally empty
		break;
		}
	}

//
// This method is called by external classes that monitor changes in area information.
// This method stores the new data info overwriting previous area info of the same
// type. The stored area information can be used in database searches.
//
void CLbsLocMonitorRequestHandler::AreaInfoUpdate(const TLbsLocMonitorAreaInfoBase& aAreaUpdate)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::AreaInfoUpdate");
	
	switch(aAreaUpdate.AreaInfoClassType())
		{
		case TLbsLocMonitorAreaInfoBase::EAreaGciClass:
			{
			// GCI is pointed to from position zero of the array.
			// Overwrite it with the new area info.
			TLbsLocMonitorAreaInfoGci& ref = const_cast<TLbsLocMonitorAreaInfoGci&>(static_cast<const TLbsLocMonitorAreaInfoGci&>(aAreaUpdate));

			// check that the update is valid before updating the current position.
			// If not, we need to mark area info as not being available.
			if (ref.iValidity)
				{
				iAreaInfoAvailable = ETrue;
				*(static_cast<TLbsLocMonitorAreaInfoGci*>(iCurrentAreaInfo[EGlobalCellIdType])) = ref;
				}
			else
				iAreaInfoAvailable = EFalse;

			break;	
			}
			
		default:
		// No other type currently supported
		__ASSERT_DEBUG(EFalse, User::Invariant());	
		break;
		}	
	}

// Called before sending a new request to the database
void CLbsLocMonitorRequestHandler::ClearRequestVariables()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::ClearRequestVariables");
	iAreaMatchInfo.SetArea(TPositionAreaInfo::EAreaUnknown);
	iAreaMatchInfo.SetLocationAreaCodeMatch(EFalse);
	iAreaMatchInfo.SetMobileCountryCodeMatch(EFalse);
	iAreaMatchInfo.SetMobileNetworkCodeMatch(EFalse);
	iAreaMatchInfo.SetCellIdMatch(EFalse);
	
	TRealX nan; nan.SetNaN();
	iPosition.SetCoordinate(nan,nan);
	}

// This method completes all of the queued RMessage2 requests that have 
// the same operation code. They are completed with aError
TInt CLbsLocMonitorRequestHandler::CompleteSameTypeMessages(EDbOperationType aDbReadOperation, TInt aError)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::CompleteSameTypeMessages");
	TInt opCode = DbReadOperationToOpCode(aDbReadOperation);
	// Complete all of the RMessage2 objects in the queue that request the same operation
	// (there might be none due to client(s) cancelling before read operation completes).
	TInt index = iRequestQueue.FindReverse(opCode, CLbsLocMonitorUtils::MatchOperation);
	while(index != KErrNotFound)
		{
		CompleteClientRequest(index, aError);
		index = iRequestQueue.FindReverse(opCode, CLbsLocMonitorUtils::MatchOperation);
		}
	return opCode;	
	}

// Returns the opcode present in an RMessage2 that triggered
// the asynchronous 'read' operation type in aDbOperationType.
TInt CLbsLocMonitorRequestHandler::DbReadOperationToOpCode(EDbOperationType aDbOperationType)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::DbReadOperationToOpCode");
	TInt ret = KErrNone;
	switch(aDbOperationType)
		{
		case ELocMonDbOperationNone:
		case ELocMonDbSavePos:
		case ELocMonDbSaveLastPos:
			{
			ret = KErrNotFound;
			break;
			}

		
		case ELocMonDbGetPosArea:
			{
			ret = EGetLastKnownPositionArea;
			break;	
			}

		 
		case ELocMonDbGetLast:
			{
			ret = EGetLastKnownPosition;
			break;	
			}

		default:
			{
			ASSERT(EFalse);
			ret = KErrNotFound;
			break;
			}	
		}
	return ret;
	}
	
void CLbsLocMonitorRequestHandler::OnTimerEventL(TInt /*aTimerId*/)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::OnTimerEventL");
	// The DB request has timed out. Complete the active request
	// (the first int the queue)
	Cancel();
	HandleDbAccessError(KErrTimedOut);
	ProcessNextRequest();
	}

TInt CLbsLocMonitorRequestHandler::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::OnTimerError");
	ASSERT(EFalse);
	return aError;			
	}
	
void CLbsLocMonitorRequestHandler::HandleDbAccessError(TInt aError)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorRequestHandler::HandleDbAccessError");
	// A request sent to the DB failed. If it was a 'read' operation the client
	// is waiting for a response (first message in q must be completed).
	switch (iOperationInProgress)
		{
		case ELocMonDbGetPosArea:
		case ELocMonDbGetLast:
			// Check the request is still there before completing it
			if (iRequestQueue.Count() > 0)
				{
				CompleteClientRequest(0, aError);
				}
		break;
		
		case ELocMonDbSavePos:
			// Remove the (unsaved) position from the queue
			// as nothing else can be done.
			iPositionsQueue.Remove(0);
		break;
		
		default:
			// Nothing can be done
		break;
		
		}
	}
