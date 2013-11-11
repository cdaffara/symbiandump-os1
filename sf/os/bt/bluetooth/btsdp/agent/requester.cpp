// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <utf.h>
#include <es_sock.h>
#include <btsdp.h>
#include "ipcinternals.h"
#include "sdpconsts.h"
#include "sdputil.h"
#include "SDPDatabase.h"
#include "DataEncoder.h"
#include "SDPAttribute.h"
#include "EncoderVisitor.h"
#include "ExtractorVisitor.h"
#include "agtypes.h"
#include "agutil.h"
#include "agconsts.h"
#include "ProtocolWrapper.h"
#include "requester.h"
#include "engine.h"


void CSdpRequesterBase::NewRequestL(const TBTDevAddr& aRemDev)
/**
	Attempts to connect to the remote device. 
	This is the first thing to do on receipt of a new request. 
**/
	{
	//If active, must only be active on the idle timer in idle state.
	__ASSERT_ALWAYS(iState == EIdle || !IsActive(), AgPanic(ESdpAgentTwoRequests));
	if(iState != EDisconnected && aRemDev == iRemoteAddress)
		{// Already Connected. Push us through to process state.
		Cancel(); //NB iState should be EIdle if we are here
		iState = EDisconnected;
		TRequestStatus* pS = &iStatus;
		User::RequestComplete(pS, KErrNone);
		SetActive();
		return;
		}
	iRetryCount = 0;
	iRemoteAddress = aRemDev;
	ConnectL();
	}

void CSdpRequesterBase::ConnectL()
/**
	Ask to connect to remote device address currently set-up in this active object.
	Note that we ensure that we are disconnected first...we could
	be connected to a different device.
**/
	{
	Disconnect();
	User::LeaveIfError(iIdleTimer.CreateLocal());
	User::LeaveIfError(iRequester.Open(iSdpSession));
	iRequester.Connect(iRemoteAddress, iStatus);
	SetActive();
	}

void CSdpRequesterBase::Disconnect()
/**
	Bring down a currently existing connection.
**/
	{
	iState = EDisconnected;
	iIdleTimer.Close();
	iRequester.Close();
	}

void CSdpRequesterBase::RetrieveResponseL()
/**
	The initial response to a search or attribute request contains
	a buffer size. To obtain the actual result, the response buffer
	has to be made big enough, and then have the actual result copied 
	into it from the server.
	RetrieveResponseL() does just this.
**/
	{
	if(!iResponseHBuf ||
		iResponseHBuf->Size() < iResultSize)
		{
		delete iResponseHBuf;
		iResponseHBuf = 0;
		iResponseHBuf = HBufC8::NewMaxL(iResultSize);
		}
	iResponse.Set(iResponseHBuf->Des());

	User::LeaveIfError(iRequester.RetrieveResult(iResponse));
	}

CSdpRequesterBase::	CSdpRequesterBase(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
: CActive(CActive::EPriorityStandard),
 iSdpSession(aSdpSession),
 iParent(aParent),
 iRemoteAddress(0),
 iResponse(0,0)
/**
	Constructor: This is where ESock session and the CSdpAgentEng parent are provided.
	Other member variables are given default values.
**/
	{
	}

CSdpRequesterBase::~CSdpRequesterBase()
/**
	Destructor: Deletes buffer for responses. Ensures we are disconnected.
**/
	{
	delete iResponseHBuf;
	Disconnect();
	}


void CSdpRequesterBase::RunL()
/**
	Runs state machine.
	If no errors then:
	@verbatim
	state disconnected		->		state requesting
									The request was stored before
									connection, and now is made.
	state requesting		->		state idle.
									The request has completed. See
									if any further requests come in
									over a brief period.
	state idle				->		state disconnected 
									if idle state interrupted by new request,
									this is notional - the RunL is immediately
									called again to set state to requesting
									and issue the actual request.
	@endverbatim
**/
	{
	if(iStatus != KErrNone)
		{
		if(iState == ERequesting && 
		   ++iRetryCount < KRetryLimit)
			{// Comms Error in request. Try to connect again.
			ConnectL();
			return;
			}
		User::Leave(iStatus.Int());
		}

	switch(iState)
		{
	case EDisconnected: //Now connected
		iState = ERequesting;
		IssueRequestL();
		break;
	case ERequesting: // Actual request complete
		iState = EIdle;
		iIdleTimer.After(iStatus, KSdpAgentIdleTimeout);
		SetActive();
		//RequestCompleteL() might DELETE us, 
		//so it must be called last (before 'return').
		RequestCompleteL();
		return; 
	case EIdle:
		Reset();
		break;
		}
	}
void CSdpRequesterBase::Reset()
	{
	Disconnect(); //Causes iState to become EDisconnected.
	delete iResponseHBuf; //A chance to delete this - it may have become quite large
	iResponseHBuf = 0;
	iRetryCount = 0;
	}

void CSdpRequesterBase::DoCancel()
/**
	Cancels current asynchronous request.
	This will be either an SDP query being performed 
	through ESock or, if this CSdpRequesterBase is 
	in the idle state, a timer (set-up to perform
	a disconnection after a defined length of time).
**/
	{
	if(iState == EIdle)
		{
		__ASSERT_DEBUG(iIdleTimer.Handle(), AgPanic(ESdpAgentBadStateAtCancel));
		if(iIdleTimer.Handle())
			{
			iIdleTimer.Cancel();
			}
		}
	else
		{ 
		__ASSERT_DEBUG(iRequester.IsOpen(), AgPanic(ESdpAgentBadStateAtCancel));
		if(iRequester.IsOpen())
			{
			iRequester.Cancel();
			}
		}
	}

CSdpSearchRequester* CSdpSearchRequester::NewL(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
/**
	Standard NewL method.
	Note: need to pass the client side object of the SDP service session,
	and the CSdpA.gentEng parent which is managing this CSdpAttributeRequester 
	active object.
**/
	{
	CSdpSearchRequester* self = new(ELeave) CSdpSearchRequester(aSdpSession, aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpSearchRequester::~CSdpSearchRequester()
/**
	Destructor
**/
	{
	Cancel();
	}

void CSdpSearchRequester::SearchRequestL(const TBTDevAddr& aRemoteDev,
										 CSdpSearchPattern& aUUIDFilter,
										 TUint16 aMaxRecCount, 
										 const TDesC8& aContState)
/**
	Sets up appropriate member variables, then attempts to connect to remote device.
	(If connection is made the state machine takes over to make the actual 
	search request.)
**/
	{
	__ASSERT_DEBUG(iUUIDFilter==NULL,AgPanic(ESdpAgentTwoRequests));
	iUUIDFilter  = &aUUIDFilter;
	iMaxRecCount = aMaxRecCount;
	iContState.Set(aContState);
	NewRequestL(aRemoteDev);
	}


CSdpSearchRequester::CSdpSearchRequester(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
:CSdpRequesterBase(aSdpSession, aParent),
 iContState(0,0)
/**
	Constructor: Adds this CSdpAttributeRequester to Active Scheduler.
	Sets parent, and passes client side object for 
	session with SDP server (currently ESock).
	Sets up cont state member to default 0 length descriptor. 
**/
	{
	CActiveScheduler::Add(this);
	}

void CSdpSearchRequester::ConstructL()
/**
	Currently does nothing.
**/
	{
	}


void CSdpSearchRequester::IssueRequestL()
/**
	Requester is now connected.
	Called down from requester base class.
	Time to actually issue the request
**/
	{
	iRequester.SdpSearchRequestL(iResultSize, 
								 *iUUIDFilter,
								 iMaxRecCount,
								 iContState, 
								 iStatus);
	SetActive();
	}

void CSdpSearchRequester::RequestCompleteL()
/**
	Response parameter format is
 @verbatim
		Total service record count		TUint16
		Current service record count	TUint16
		Service Record Handles			unadorned list of TUint32's
		Continuation State				1 + 0-16 bytes
 @endverbatim

**/
	{
	RetrieveResponseL(); // Use base class to suck result over IPC. Loads iResponse
	iUUIDFilter = NULL; // Indicates request is complete
	const TInt KRspTotalRecCountOffset = 0;
	const TInt KRspCurrentRecCountOffset = 2;
	const TInt KRspRecHandlesOffset = 4;
	const TInt KMinRspLength = 5; // From Spec. 2 + 2 + 1

	if(iResponse.Length() < KMinRspLength)
		User::Leave(KErrSdpBadResultData);
//
	TUint16 totalRecCount = BigEndian::Get16(&iResponse[KRspTotalRecCountOffset]);
	TUint16 currentRecCount = BigEndian::Get16(&iResponse[KRspCurrentRecCountOffset]);
	if(totalRecCount < currentRecCount)
		User::Leave(KErrSdpBadResultData);
	TInt byteCount = currentRecCount*sizeof(TSdpServRecordHandle);
	if(byteCount + KRspRecHandlesOffset + 1 > iResponse.Length())
		User::Leave(KErrSdpBadResultData);
//
	TInt contLen = iResponse[KRspRecHandlesOffset+byteCount];
	if(contLen > KSdpContinuationStateMaxLength ||
	   KRspRecHandlesOffset + byteCount + 1 + contLen != iResponse.Length())
		User::Leave(KErrSdpBadResultData);
//
	TPtrC8 recHandles(&iResponse[KRspRecHandlesOffset], byteCount);
	TPtrC8 contState;
	if(iResponse.Length()>KRspRecHandlesOffset + 1 + byteCount && contLen)
		{
		contState.Set(&iResponse[KRspRecHandlesOffset + 1 + byteCount], contLen);
		}
	else
		{
		contState.Set(0,0);
		}

	//Call this last: it may well end up calling back into this
	//CSdpSearchRequester object.
	iParent.HandleServiceSearchResponseL(totalRecCount, 
										 currentRecCount, 
										 recHandles, 
										 contState);
	}

void CSdpSearchRequester::Reset()
/**
	Called when base requester idle timer expires,
	and when an error is reported
	Cleans up and makes sure we really are disconnected.
**/
	{
	CSdpRequesterBase::Reset();
	}

void CSdpSearchRequester::DoCancel()
/**
	Cancels current asynchronous requests.
	This could be with the Sdp server (currently ESOCK)
	or if this CSdpRequesterBase is in idle state
	it cancels the time out timer (set-up to perform
	a disconnection after a defined length of time).
**/
	{
	CSdpRequesterBase::DoCancel();

	// We only need to reset iUUIDFilter if the Cancel
	// has been called externally.
	// If the state is EIdle, either iUUIDFilter is already NULL
	// or the Cancel is being called internally by NewRequestL 
	if(iState != EIdle)
		{
		iUUIDFilter = NULL; // Indicates request is complete
		}
	}

TInt CSdpSearchRequester::RunError(TInt aError)
/**
	Called automatically when a RunL leaves.
	Calls appropriate error handling function in the CSdpAgentEng parent.
**/
	{
	/*
	Make sure we are not active. RunL can leave after SetActive is called.
	*/
	Cancel(); 

	/*
	Indicates request has completed - albeit with an error.
	Do NOT rely on Cancel to have called DoCancel to do this.
	It will only do this if the CSdpSearchRequester is active.
	*/
	iUUIDFilter = NULL;

	/*
	We may or may not be disconnected.
	We may not be able to be re-connect
	We need to be in a known state after this call.
	So make sure we are disconnnected,
	and leave it to the user to try to
	reconnect. He will do this automatically
	by trying to start a new request.
	*/
	Reset();
		
	iParent.HandleServiceSearchError(aError);
	return KErrNone;
	}


CSdpAttributeRequester* CSdpAttributeRequester::NewL(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
/**
	Standard NewL method.
	Note: need to pass the client side object of the SDP service session,
	and the CSdpA.gentEng parent which is managing this CSdpAttributeRequester 
	active object.
**/
	{
	CSdpAttributeRequester* self = new(ELeave) CSdpAttributeRequester(aSdpSession, aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpAttributeRequester::~CSdpAttributeRequester()
/**
	Destructor
**/
	{
	Cancel();
	}

void CSdpAttributeRequester::AttributeRequestL(const TBTDevAddr& aRemoteDev,
											   TSdpServRecordHandle aHandle, 
											   TUint16 aMaxAttrByteCount,
											   CSdpAttrIdMatchList& aMatchList,
											   const TDesC8& aContState)
/**
	Sets up appropriate member variables, then attempts to connect to remote device.
	(If connection is made the state machine takes over to make the actual 
	attribute request.)
**/
	{
	__ASSERT_DEBUG(iMatchList==NULL,AgPanic(ESdpAgentTwoRequests));
	iHandle  = aHandle;
	iMaxAttrByteCount = aMaxAttrByteCount;
	iMatchList = &aMatchList;
	iContState.Set(aContState);
	NewRequestL(aRemoteDev);
	}


CSdpAttributeRequester::CSdpAttributeRequester(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
:CSdpRequesterBase(aSdpSession, aParent),
 iContState(0,0)
/**
	Constructor: Adds this CSdpAttributeRequester to Active Scheduler.
	Sets parent, and passes client side object for 
	session with SDP server (currently ESock).
	Sets up cont state member to default 0 length descriptor. 
**/
	{
	CActiveScheduler::Add(this);
	}

void CSdpAttributeRequester::ConstructL()
/**
	Currently blank.
**/
	{
	}


void CSdpAttributeRequester::IssueRequestL()
/**
	Requester is now connected.
	Called down from requester base class.
	Time to actually issue the request
**/
	{
	iRequester.SdpAttributeRequestL(iResultSize, 
									iHandle,
									iMaxAttrByteCount,
									*iMatchList, 
									iContState, 
									iStatus);
	SetActive();
	}

void CSdpAttributeRequester::RequestCompleteL()
/**
	Response parameter format is
 @verbatim
		byte count of attr list		TUint16
		Attribute ID & Value		DES
		Continuation State			1 + 0-16 bytes
 @endverbatim

**/
	{
	RetrieveResponseL(); // Use base class to suck result over IPC. Loads iResponse
	iMatchList = NULL; // Indicates request is complete
	const TInt KRspTotalCountOffset = 0;
	const TInt KRspAttributeListOffset = 2;
	const TInt KMinRspLength = 5; // From Spec. 2 + 2 + 1

	if(iResponse.Length() < KMinRspLength)
		User::Leave(KErrSdpBadResultData);
//
	TUint16 byteCount = BigEndian::Get16(&iResponse[KRspTotalCountOffset]);
	if(byteCount + KRspAttributeListOffset + 1 > iResponse.Length())
		User::Leave(KErrSdpBadResultData);
//
	TInt contLen = iResponse[KRspAttributeListOffset+byteCount];
	if(contLen > KSdpContinuationStateMaxLength ||
	   KRspAttributeListOffset + byteCount + 1 + contLen != iResponse.Length())
		User::Leave(KErrSdpBadResultData);
//
	TPtrC8 attrList(&iResponse[KRspAttributeListOffset], byteCount);
	TPtrC8 contState;
	if(iResponse.Length()>KRspAttributeListOffset + 1 + byteCount && contLen)
		{
		contState.Set(&iResponse[KRspAttributeListOffset + 1 + byteCount], contLen);
		}
	else
		{
		contState.Set(0,0);
		}

	//Call this last: it may well end up calling back into this
	//CSdpAttributeRequester object.
	iParent.HandleAttributeResponseL(attrList, contState);
	}

void CSdpAttributeRequester::Reset()
/**
	Called when base requester idle timer expires,
	and when an error is reported
	Cleans up and makes sure we really are disconnected.
**/
	{
	CSdpRequesterBase::Reset();
	}

void CSdpAttributeRequester::DoCancel()
/**
	Cancels current asynchronous requests.
	This could be with the Sdp server (currently ESOCK)
	or if this CSdpRequesterBase is in idle state
	it cancels the time out timer (set-up to perform
	a disconnection after a defined length of time).
**/
	{
	CSdpRequesterBase::DoCancel();

	// We only need to reset iMatchList if the Cancel
	// has been called externally.
	// If the state is EIdle, either iMatchList is already NULL
	// or the Cancel is being called internally by NewRequestL 
	if(iState != EIdle)
		{
		iMatchList = NULL; // Indicates request is complete
		}
	}

TInt CSdpAttributeRequester::RunError(TInt aError)
/**
	Called automatically when a RunL leaves.
	Calls appropriate error handling function in the CSdpAgentEng parent.
**/
	{
	/*
	Make sure we are not active. RunL can leave after SetActive is called.
	*/
	Cancel(); 

	/*
	Indicates request has completed - albeit with an error.
	Do NOT rely on Cancel to have called DoCancel to do this.
	It will only do this if the CSdpAttributeRequester is active
	and the idle timer is not running.
	*/
	iMatchList = NULL; //indicates request has completed - albeit with an error

	/*
	We may or may not be disconnected.
	We may not be able to be re-connect
	We need to be in a known state after this call.
	So make sure we are disconnnected,
	and leave it to the user to try to
	reconnect. He will do this automatically
	by trying to start a new request.
	*/
	Reset();
		
	iParent.HandleAttributeError(aError);
	return KErrNone;
	}




