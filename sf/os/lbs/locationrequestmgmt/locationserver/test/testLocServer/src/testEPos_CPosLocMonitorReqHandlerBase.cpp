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
* Description:  	The base class that provides the common functionalities of 
*				all the requests to the Location Monitor
*
*/



#include "testEPos_CPosLocMonitorReqHandlerBase.h"

/**
 * C++ default constructor
 * 
 */

CPosLocMonitorReqHandlerBase::CPosLocMonitorReqHandlerBase() :
CActive(EPriorityStandard),
iSubsessionIdentity(CPosLocMonitorReqHandlerBase::MatchSessionAndSubsession),
iSessionIdentity(CPosLocMonitorReqHandlerBase::MatchSession)
	{
	
	
	}

CPosLocMonitorReqHandlerBase::~CPosLocMonitorReqHandlerBase()
	{
	QRequestsComplete(KErrNotFound);

	iLocMonitorReqQ.Close();
	}


void CPosLocMonitorReqHandlerBase::RequestComplete(const RMessage2& aMessage, TInt aCompleteCode)
	{
	// Complete the client request with the respective status code
	if (!aMessage.IsNull())
		{
		aMessage.Complete(aCompleteCode);
		}
	}

// More than one request on the queue
void CPosLocMonitorReqHandlerBase::CheckAndAddReqToQueueL(TLocMonitorRequestType aRequestType, const RMessage2& aMessage)
	{
		// Find if any request from this client already exists on the queue
    	TInt reqQMatchIndex=KErrNotFound;
    	reqQMatchIndex = MatchRequestInQueue(aRequestType,aMessage);
    	
    	if (KErrNotFound==reqQMatchIndex)
    		{
    		// Request has to be queued
    		iLocMonitorReqQ.AppendL(aMessage);
    		}
    		else
    		{
    		// Remove the request from the queue
    		iLocMonitorReqQ.Remove(reqQMatchIndex);
    		// 
//    		aMessage.Complete(EPositionDuplicateRequest); 						//TODO [1] or	
    		aMessage.Panic(KPosClientFault, EPositionDuplicateRequest); 		//TODO [2] or
    		//aMessage.Panic(KPosClientFault, EPositionRequestsNotCancelled); 	//TODO [3]
    		}
	}

// More than one request on the queue
void CPosLocMonitorReqHandlerBase::CancelRequest(TLocMonitorRequestType aRequestType, const RMessage2& aMessage)
	{
	// Atleast one oustanding request exists
	if (iLocMonitorReqQ.Count()>0)
		{
		// [The request queue is not empty]
		if (iLocMonitorReqQ.Count()==1)
			{
			// If there is currently only one ongoing request and a cancel has been issued
			// Complete the requests immediately and then call cancel on the active object
			// which cancels the request to the location monitor
			
			// Complete the pending last known position request with KErrCancel
			RequestComplete(iLocMonitorReqQ[0], KErrCancel);
			
			// Remove the request from the queeu
			iLocMonitorReqQ.Remove(0);

			// Complete the request to cancel the last known position request with KErrNone
			RequestComplete(aMessage, KErrNone);
			
			// Calling Cancel inturn calls DoCancel which cancels the request with the 
			// location monitor
			Cancel();
			}
		else
			{
			FindRequestAndCancel(aRequestType, aMessage);
			}
		}

	else
		{
		// No request found (on the queue) -> No oustanding request
		RequestComplete(aMessage, KErrNotFound); // TODO KErrNotFound is correct ? 
		}
	}
	
void CPosLocMonitorReqHandlerBase::FindRequestAndCancel(TLocMonitorRequestType aRequestType, const RMessage2& aMessage)
	{
	// Called only if there is more than one request on the queue
	// Find the index of the request to be removed from the queue
	TInt reqQMatchIndex=KErrNotFound;
	reqQMatchIndex = MatchRequestInQueue(aRequestType, aMessage);

	if (KErrNotFound==reqQMatchIndex)
		{
		RequestComplete(aMessage,KErrNotFound); // TODO KErrNotFound is correct ? 
		}
	else
		{
		// Complete the pending LKPosArea request with KErrCancel
		RequestComplete(iLocMonitorReqQ[reqQMatchIndex], KErrCancel);
		// Remove the request from the queue
		iLocMonitorReqQ.Remove(reqQMatchIndex);

		// Complete the request to cancel LKPosArea request with KErrNone
		RequestComplete(aMessage,KErrNone);
		}
	
	}


TInt CPosLocMonitorReqHandlerBase::MatchRequestInQueue(TLocMonitorRequestType aRequestType, const RMessage2& aMessage)
	{
	TInt reqQMatchIndex=KErrNotFound;
	
	switch (aRequestType)
		{
		case EReqOnSession :
			// Match the session
			reqQMatchIndex = iLocMonitorReqQ.Find(aMessage,iSessionIdentity);
			break;
		case EReqOnSubSession :
			// Match the session and the subsession
			reqQMatchIndex = iLocMonitorReqQ.Find(aMessage,iSubsessionIdentity);
			break;
		default :
			break;
		}
	
	return reqQMatchIndex;
	}

/** 
 * MatchSession 
 * 		>> Check if a the Session corresponding to an outstanding request [on the queue] 
 * 		>> matches that of the new request
 *
 * @param  qClient    - An entry from the queue of oustanding requests
 * @param  newClient  - The new request from the client 
 */
TBool CPosLocMonitorReqHandlerBase::MatchSession(const RMessage2& qClient, const RMessage2& newClient)
	{
	return ( qClient.Session()== newClient.Session() );
	}

/** 
 * MatchSubsession 
 * 		>> Check if a the subsession corresponding to an outstanding request [on the queue] 
 * 		>> matches that of the new request
 *
 * @param  qClient    - An entry from the queue of oustanding requests
 * @param  newClient  - The new request from the client 
 */
TBool CPosLocMonitorReqHandlerBase::MatchSessionAndSubsession(const RMessage2& qClient, const RMessage2& newClient)
	{
	return ( (qClient.Session()== newClient.Session()) && (qClient.Int3()== newClient.Int3()) );
	}


void CPosLocMonitorReqHandlerBase::QRequestsComplete(TInt aCompleteCode)
	{
	// Complete all the requests on the queue with the given completion code
	while(iLocMonitorReqQ.Count()>0)
		{
		TInt index = iLocMonitorReqQ.Count()-1;
		// Complete all the requests in the queue with KErrServerTerminated
		if (!iLocMonitorReqQ[index].IsNull())
			{
			iLocMonitorReqQ[index].Complete(aCompleteCode);
			}
		iLocMonitorReqQ.Remove(index);
		}
	
	}

void CPosLocMonitorReqHandlerBase::NotifySubSessionClosed(const RMessage2& aMessage)
	{
	// If the subsession is closed before the Last Known Position is received from the Loc Monitor,
	// then complete the requests from that subsession with KErrDisconnected
	TInt reqQMatchIndex=KErrNotFound;
	reqQMatchIndex = iLocMonitorReqQ.Find(aMessage,iSubsessionIdentity);

	if (reqQMatchIndex != KErrNotFound)
		{
		if ((iLocMonitorReqQ.Count()==1)&&(iStatus==KRequestPending))
			{
			// Only one outstanding request - issue a cancel to the location monitor
			Cancel();
			}
		if (iLocMonitorReqQ.Count()>=1)
			{
			// Remove the request from the queue
			iLocMonitorReqQ.Remove(reqQMatchIndex);
			}
		}
	}

void CPosLocMonitorReqHandlerBase::NotifySessionClosed(const CSession2* aSessionPtr)
	{
	// As it is easier to delete the entries from the end of the queue
	// store the index of the final element
	TInt index = iLocMonitorReqQ.Count()-1;

	// Clear all the outstanding Last Known Position Requests on the queue 
	// and panic the client with EPositionRequestsNotCancelled
	while (index>=0)
		{
		if ( aSessionPtr ==  const_cast<const CSession2*>( (iLocMonitorReqQ[index].Session()) ) )
			{
			//TODO Panic or RequestComplete? RequestComplete(iLKPosQ[index],KErrDisconnected);
			iLocMonitorReqQ[index].Panic(KPosClientFault, EPositionRequestsNotCancelled);
			iLocMonitorReqQ.Remove(index);
			}
		index--;
		}

	// No more request on the queue - So issue a cancel to the location monitor
	if ((iLocMonitorReqQ.Count()<=0)&&(iStatus==KRequestPending))
		{
		Cancel();
		}
	}


void CPosLocMonitorReqHandlerBase::NotifyServerShutDown()
	{
		// Complete all the requests on the queue with KErrServerTerminated
		// The Client/Server framework doesnot guarantee the completion
		QRequestsComplete(KErrServerTerminated);
	}
