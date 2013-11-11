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

/**
 @file
 @released since 399
*/

#include <es_sock.h>
#include <comms-infras/es_connectionserv.h>
#include <comms-infras/es_availability.h>
#include <comms-infras/es_connectionservermessages.h>
#include "cs_connectionservimpl.h"
#include <connpref.h>
#include <comms-infras/ss_log.h>
#include <rsshared.h>
#include <es_ver.h>


using namespace ConnectionServ;

CConnectionServImpl::CConnectionServImpl(RConnectionServ& aCS) :
	iConnectionServ(aCS)
/** 
 Empty ctor
 */	
	{
	LOG( ESockLog::Printf(_L8("CConnectionServImpl %08x: CConnectionServImpl() tid %d"), this, (TUint)RThread().Id()); );
	}


CConnectionServImpl* CConnectionServImpl::NewL(RConnectionServ& aCS)
	{
	LOG( ESockLog::Printf(_L8("CConnectionServImpl : NewL() tid %d"), (TUint)RThread().Id()); );
	CConnectionServImpl* inst = new(ELeave) CConnectionServImpl(aCS);
	return inst;
	}


CConnectionServImpl::~CConnectionServImpl()
/** 
 Closes the comms manager sub-session 

 Clean up resources then drop to base class
 */
	{
	LOG( ESockLog::Printf(_L8("CConnectionServImpl%08x: ~CConnectionServImpl() tid %d"), this, (TUint)RThread().Id()); );

	// Stop any active queries and remove them from the active scheduler.

	// This is a lot cheaper on ROM than implementing server-side code to handle shutdown

	CancelAccessPointStatusQuery();
	delete iAccessPointStatusRequest;

	CancelAccessPointNotification();
	delete iAccessPointNotificationRequest;
	}


//
// Access Point Status
//

void CConnectionServImpl::AccessPointStatusQuery(const CConnectionServParameterBundle& aQuery, CConnectionServParameterBundle& aResult, TRequestStatus& aStatus)
/**
Retrieve information from the Tier(s) specified in the Connect function about the availability of one or more services.  
aQuery is an in parameter.  A client adds a query parameter set to specify the Access Point or group of Access Points
	it wishes to enquire about and adds one or more parameter sets to carry the information they are interested to retrieve
	for each Access Points
aResult is an out parameter. The Connection Server populates this bundle with information about the access points that
	matched the query.
The time taken to perform this action depends on the implementation in the stack plug-in that serves the request
*/
	{
	LOG( ESockLog::Printf(_L8("CConnectionServImpl %08x: AccessPointStatusQuery() tid %d"), this, (TUint)RThread().Id()); );

	if(iAccessPointStatusRequest && iAccessPointStatusRequest->IsActive())
		{
		// complete the request with failure
		TRequestStatus* status = &aStatus;
    	User::RequestComplete(status, KErrInUse);
		return;
		}

	if(!iAccessPointStatusRequest)
		{
		// Delegate the call to the active object.. NewL creates and sets up the object..
		// We give it a pointer to this so it can call SendReceive.
		// We only hold onto the pointer to it so we can send a cancel to it.
		TRAPD(result, iAccessPointStatusRequest = CAccessPointStatusRequest::NewL(*this) );
		if(result)
			{
			// complete the request with failure
			TRequestStatus* status = &aStatus;
	    	User::RequestComplete(status, KErrArgument);
	    	return;
			}
		}

	// Delegate the call to the active object.. NewL creates and sets up the object and
	//    adds it to the active scheduler.
	// 		Once it's finished it'll take itself off and delete itself.
	// We only hold onto the pointer to it so we can send a cancel to it.
	iAccessPointStatusRequest->Start(aQuery,aResult,aStatus);
	}


void CConnectionServImpl::CancelAccessPointStatusQuery()
/**
Tell the Connection Server we are cancelling our status query request. If no query is active, does nothing.
*/
	{
	LOG( ESockLog::Printf(_L8("CConnectionServImpl %08x: CancelAccessPointStatusQuery() tid %d"), this, (TUint)RThread().Id()); );
	if(iAccessPointStatusRequest)
		{
		// this will cause it to clean itself up (i.e. forward the cancel to the server then remove itself from the scheduler)
		iAccessPointStatusRequest->Cancel();
		}
	else
		{
		LOG( ESockLog::Printf(_L8("Tried to cancel inactive query. Ignoring.")); );
		}
	}


//
// Access Point Notification
//

void CConnectionServImpl::AccessPointNotification(const CConnectionServParameterBundle& aQuery, MAccessPointNotificationObserver& aObserver)
/**
As for GetAvailability, but will notify regularly of changes to availability of requested Access Point(s).
The aQuery defines a view of the access points, as soon as this view is established all its contents are reported
 (i.e. we get initial status), after that, Access Points entering or leaving the view get reported to the observer.
Queueing of multiple events happens automatically server side
MAccessPointNotificationObserver contains a Notify(CConnectionServParameterBundle aResult) method and an Error() method to catch
	errors from this function
*/
	{
	LOG( ESockLog::Printf(_L8("CConnectionServImpl %08x: AccessPointNotification() tid %d"), this, (TUint)RThread().Id()); );

	if(iAccessPointNotificationRequest && iAccessPointNotificationRequest->IsActive())
		{
		// complete the request with failure
		aObserver.AccessPointNotificationError(KErrInUse);
		return;
		}

	if(!iAccessPointNotificationRequest)
		{
		// Delegate the call to the active object.. NewL creates and sets up the object..
		// We give it a pointer to this so it can call SendReceive.
		// We only hold onto the pointer to it so we can send a cancel to it.
		TRAPD(result, iAccessPointNotificationRequest = CAccessPointNotificationRequest::NewL(*this) );
		if(result)
			{
			// complete the request with failure
			aObserver.AccessPointNotificationError(KErrArgument);
			return;
			}
		}

	// Delegate the call to the active object.. NewL creates and sets up the object and
	//    adds it to the active scheduler.
	// 		Once it's finished it'll take itself off the scheduler.
	// We only hold onto the pointer to it so we can send a cancel to it.
	iAccessPointNotificationRequest->Start(aQuery,aObserver);
	}


void CConnectionServImpl::CancelAccessPointNotification()
/**
Tell the Connection Server we are cancelling our notification session. If no notification session is active, does nothing.
*/
	{	
	LOG( ESockLog::Printf(_L8("CConnectionServImpl %08x: CancelAccessPointNotification() tid %d"), this, (TUint)RThread().Id()); );
	if(iAccessPointNotificationRequest && iAccessPointNotificationRequest->IsActive())
		{
		// this will cause it to clean itself up (i.e. forward the cancel to the server then remove itself from the scheduler)
		iAccessPointNotificationRequest->Cancel();
		}
	else
		{
		LOG( ESockLog::Printf(_L8("Tried to cancel inactive query. Ignoring.")); );
		}

	}







CAccessPointStatusRequest::CAccessPointStatusRequest(CConnectionServImpl& aImpl):
	CActive(CActive::EPriorityStandard), iConnectionServImpl(aImpl)
	{
	}

CAccessPointStatusRequest::~CAccessPointStatusRequest()
	{
	iQueryBuffer.Close();
	iResultBuffer.Close();
	}

void CAccessPointStatusRequest::ConstructL()
	{
	User::LeaveIfError(iQueryBuffer.Create(KInitialBufferSize));
	User::LeaveIfError(iResultBuffer.Create(KInitialBufferSize));
	}

/* static*/
CAccessPointStatusRequest* CAccessPointStatusRequest::NewL(CConnectionServImpl& aImpl)
	{
	CAccessPointStatusRequest *inst = new(ELeave) CAccessPointStatusRequest(aImpl);
	inst->ConstructL();
	return inst;
	}

void CAccessPointStatusRequest::Start(const CConnectionServParameterBundle& aQuery, CConnectionServParameterBundle& aResult, TRequestStatus& aStatus)
/**
 *
 *
 */
	{
	CActiveScheduler::Add(this);

	iResult = &aResult;
	iClientRequest = &aStatus;

	TInt ret = CConnectionServImpl::EnsureBufferIsBigEnough(iQueryBuffer,aQuery.Length());

	if(ret == KErrNone)
		{
		iQueryBuffer.SetLength(0);
		ret = aQuery.Store(iQueryBuffer);
		}

	if (ret != KErrNone)
		{
		LOG( ESockLog::Printf(_L8("CAccessPointStatusRequest %08x: Start() tid %d failed when storing parameter bundle in serialization buffer: %d"), this, (TUint)RThread().Id(), ret); );
		CompleteClientRequest(ret);
		return;
		}

	iSentRequest = ECMAccessPointStatusQuery_DoThenGetResultOrSize;
	iConnectionServImpl.SendReceive(iSentRequest, TIpcArgs(&iQueryBuffer,  &iResultBuffer),iStatus);
	SetActive();
	}

void CAccessPointStatusRequest::RunL()
	{
	TInt result = iStatus.Int();
	if(result == KErrNone)
		{
		// all is well. let's deserialise result buffer into result bundle..
		result = iResult->Load(iResultBuffer);
		}
	else
	if(result > KErrNone &&  // result held error or required buffer length
       iSentRequest == ECMAccessPointStatusQuery_DoThenGetResultOrSize )
		{
		// ok so it finished but buffer isn't big enough.. so (re)allocate and re-call
		result = CConnectionServImpl::EnsureBufferIsBigEnough(iResultBuffer,result);

		if(result == KErrNone)
			{
			iSentRequest = ECMAccessPointStatusQuery_GetResult;
			iConnectionServImpl.SendReceive(iSentRequest, TIpcArgs(&iResultBuffer),iStatus);
			SetActive();
			return;
			}
		}

	CompleteClientRequest(result);
	}

TInt CConnectionServImpl::EnsureBufferIsBigEnough(RBuf8& aBufToCheck, TUint aSize)
	{
	return (aBufToCheck.MaxSize() < aSize) ? aBufToCheck.ReAlloc(aSize) : KErrNone ;
	}


void CAccessPointStatusRequest::CompleteClientRequest(TInt aError)
	{
	User::RequestComplete(iClientRequest, aError);
//	CActiveScheduler::Remove(this); 
	}

void CAccessPointStatusRequest::DoCancel()
	{
	iSentRequest = ECMAccessPointStatusQuery_Cancel;
	iConnectionServImpl.SendReceive(iSentRequest);
	CompleteClientRequest(KErrCancel);
	}

TInt CAccessPointStatusRequest::RunError(TInt aError)
	{
	CompleteClientRequest(aError);
	
	// Error has gone to client.. No need to error the ActiveScheduler
	return KErrNone;
	}





CAccessPointNotificationRequest::CAccessPointNotificationRequest(CConnectionServImpl& aImpl):
	CActive(CActive::EPriorityStandard), iConnectionServImpl(aImpl)
	{
	}

void CAccessPointNotificationRequest::ConstructL()
	{
	User::LeaveIfError(iResultBuffer.Create(KInitialBufferSize));
	}

/* static*/
CAccessPointNotificationRequest* CAccessPointNotificationRequest::NewL(CConnectionServImpl& aImpl)
	{
	CAccessPointNotificationRequest *inst = new(ELeave) CAccessPointNotificationRequest(aImpl);
	inst->ConstructL();
	return inst;
	}

void CAccessPointNotificationRequest::Start(const CConnectionServParameterBundle& aQuery, MAccessPointNotificationObserver& aObserver)
	{
	CActiveScheduler::Add(this);

	iObserver = &aObserver;

	TInt ret = CConnectionServImpl::EnsureBufferIsBigEnough(iQueryBuffer,aQuery.Length());

	if(ret == KErrNone)
		{
		iQueryBuffer.SetLength(0);
		ret = aQuery.Store(iQueryBuffer);
		}

	if (ret != KErrNone)
		{
		LOG( ESockLog::Printf(_L8("CAccessPointNotificationRequest %08x: Start() tid %d failed when storing parameter bundle in serialization buffer: %d"), this, (TUint)RThread().Id(), ret); );
		iObserver->AccessPointNotificationError(KErrNoMemory);
		return;
		}

	iSentRequest = ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize;
	iConnectionServImpl.SendReceive(iSentRequest, TIpcArgs(&iQueryBuffer,  &iResultBuffer),iStatus);
	SetActive();
	}


void CAccessPointNotificationRequest::DoCancel()
	{
	iSentRequest = ECMAccessPointNotification_Cancel;
	iConnectionServImpl.SendReceive(iSentRequest);
	iObserver->AccessPointNotificationError(KErrCancel);
	}

void CAccessPointNotificationRequest::RunL()
	{
	TInt result = iStatus.Int();
	if(result == KErrNone)
		{
		// all is well. let's deserialise result buffer into result bundle..
		CConnectionServParameterBundle *bundle = CConnectionServParameterBundle::LoadL(iResultBuffer);

		// issue next request immediately
		iSentRequest = ECMAccessPointNotification_AwaitThenGetResultOrSize;
		iConnectionServImpl.SendReceive(iSentRequest, TIpcArgs(0,&iResultBuffer),iStatus);
		SetActive();

		// report notification to observer
		iObserver->AccessPointNotification(bundle);
		return;
		}
	else
	if(result > KErrNone &&  // result held error or required buffer length
       iSentRequest == ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize ||
	   iSentRequest == ECMAccessPointNotification_AwaitThenGetResultOrSize)
		{
		// ok so it finished but buffer isn't big enough.. so (re)allocate and re-call
		result = CConnectionServImpl::EnsureBufferIsBigEnough(iResultBuffer,result);

		if(result == KErrNone)
			{
			iSentRequest = ECMAccessPointNotification_GetResult;
			iConnectionServImpl.SendReceive(iSentRequest, TIpcArgs(0,&iResultBuffer),iStatus);
			SetActive();
			return;
			}
		}

	iObserver->AccessPointNotificationError(result);
	}

TInt CAccessPointNotificationRequest::RunError(TInt aError)
	{
	iObserver->AccessPointNotificationError(aError);
	
	// Error has gone to client.. No need to error the ActiveScheduler
	return KErrNone;
	}


