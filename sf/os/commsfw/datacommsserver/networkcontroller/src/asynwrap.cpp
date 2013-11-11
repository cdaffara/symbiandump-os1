// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file ASYNWRAP.CPP
*/

#include "asynwrap.h"

CASyncEtelRequestWrapper* CASyncEtelRequestWrapper::NewL(MAsyncRequestsObserver* aObserver)
/**
This function is constructing an object of class CASyncEtelRequestWrapper,pushing it to the cleanup stack
and popping it back.

@param aObserver,a pointer to class MAsyncRequestsObserver to register the asyncronous requests
@return self, make asyncronous requests synchronous whilst not blocking the thread and thus allowing other 
active objects to run as normal
*/
	{
	CASyncEtelRequestWrapper* self = new (ELeave) CASyncEtelRequestWrapper();
	CleanupStack::PushL(self);
	self->iObserver = aObserver;
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CASyncEtelRequestWrapper::CASyncEtelRequestWrapper()
	: CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CASyncEtelRequestWrapper::ConstructL()
	{
	iAsyncRequestState = ENoRequests;
	iAsyncRequestData.iETelAsynError.iError = KErrNone;
	iAsyncRequestData.iETelAsynError.iFailedRequest = 0x00;
	iAsyncRequestData.iMSClass = RPacketService::EMSClassUnknown;
	iAsyncRequestData.iRegistrationStatus = RPacketService::ENotRegisteredNotSearching;
	}

CASyncEtelRequestWrapper::~CASyncEtelRequestWrapper()
/**
Destructor
*/
	{
	Cancel();
	iPacket.Close();
	}

void CASyncEtelRequestWrapper::StartAsyncRequests(RTelServer&, RMobilePhone& aMmPhone)
/**
Pass the object on which the request was made and the request id - 
this is needed to cancel the request if necessary

@param aTelServer, establish a root telephony server session.
@param aMmPhone, encapsulates access to a mobile phone
*/
	{
	// Open PacketService
	TInt ret = iPacket.Open(aMmPhone);
	if(ret == KErrNone)
		{
		// The first request to be made is GetMSClass from packet session
		iAsyncRequestState = EGetMSClass;
		iReqToCancel = EPacketGetMSClass;
		iPacket.GetMSClass(iStatus,iAsyncRequestData.iMSClass,iMaxMSClass);
		// Set active and start to wait for the request response
		if(!IsActive())
			SetActive();
		}
	// Error, complete to the observer
	else
		{
		// Save the error into iError and pass it to GenConn
		iAsyncRequestData.iETelAsynError.iError = ret;
		iAsyncRequestData.iETelAsynError.iFailedRequest = 0x00;
		iObserver->ETelAsyncRequestsComplete(iAsyncRequestData);
		}
	}

void CASyncEtelRequestWrapper::DoCancel()
	{
	// If the state is not ENoRequests, then we are requesting 
	// something from ETel
	if(iAsyncRequestState != ENoRequests)
		{
		// Cancel the current packet service request, it is saved
		// in the iReqToCancel member. 
		iPacket.CancelAsyncRequest(iReqToCancel);
		}

	iAsyncRequestState = ENoRequests;
	iAsyncRequestData.iMSClass = RPacketService::EMSClassUnknown;
	iAsyncRequestData.iRegistrationStatus = RPacketService::ENotRegisteredNotSearching;
	}

void CASyncEtelRequestWrapper::RunL()
	{
	if(iStatus != KRequestPending)
		{
		TInt err = iStatus.Int();
		// If the request was not cancelled, start requesting the next one
		if(err!=KErrCancel)
			{
			// MSClass information received
			if(iAsyncRequestState == EGetMSClass)
				{
				// If error is not KErrNone and KErrNotSupported, then this state failed. 
				// Save the error and report it to GenConn when the last state is handled.
				if(err!=KErrNone && err!= KErrNotSupported)
					{
					iAsyncRequestData.iETelAsynError.iError = err;
					iAsyncRequestData.iETelAsynError.iFailedRequest = EPacketGetMSClass;
					}
				// Get current network registration status
				iAsyncRequestState = EGetNwRegStatus;
				iReqToCancel = EPacketGetNtwkRegStatus;
				iAsyncRequestData.iRegistrationStatus = RPacketService::EUnknown;
				iPacket.GetNtwkRegStatus(iStatus, iAsyncRequestData.iRegistrationStatus);
				// Start the next request
				if(!IsActive())
					{
					SetActive();
					}
				}
			else if(iAsyncRequestState == EGetNwRegStatus)
				{
				// If error is not KErrNone and KErrNotSupported, then this state failed. 
				// Save the error and report it to GenConn when the last state is handled.
				if(err!=KErrNone && err!= KErrNotSupported)
					{
					iAsyncRequestData.iETelAsynError.iError = err;
					iAsyncRequestData.iETelAsynError.iFailedRequest = EPacketGetNtwkRegStatus;
					}
				// This is the last request
				iAsyncRequestState = ENoRequests;
				// All done, move the data to observer
				iObserver->ETelAsyncRequestsComplete(iAsyncRequestData);
				}
			else
				{
				// When there is going to be more requests, add them here.
				iAsyncRequestState = ENoRequests;
				}
			}
		}
	}

