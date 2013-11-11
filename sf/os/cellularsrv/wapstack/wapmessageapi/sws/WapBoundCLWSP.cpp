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

#include "WapBoundCLWSP.h"
#include "WapMsgUtils.h"
#include "CLWSPPduHandler.h"



CSWSWapBoundCLWSPService* CSWSWapBoundCLWSPService::NewL()
/**
Static new function
@internalComponent
@released
@since v8.0
*/
	{
	CSWSWapBoundCLWSPService* me = new(ELeave)CSWSWapBoundCLWSPService();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CSWSWapBoundCLWSPService::CSWSWapBoundCLWSPService()
	: CWapBoundCLWSPService()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

CSWSWapBoundCLWSPService::~CSWSWapBoundCLWSPService()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	delete iAgent;
	}

void CSWSWapBoundCLWSPService::ConstructL()
/**
Second phase constructor
@internalComponent
@released
@since v8.0
*/
	{
	// Parent class construction
	CWapBoundCLWSPService::ConstructL();
	iAgent=CWspMessageApiAgent::NewL();
	}

TInt CSWSWapBoundCLWSPService::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure)
/**
Opens a End-point which is to be used to listen for the incoming method result messages from any sender;
@internalComponent
@released
@since v8.0
@param aBearer (in) the bearer to listen on (use EAll for all bearers)
@param aPort (in) the port to listen on. If set to 0, a local port will be chosen for the client's first SendTo
@param aSecure (in) security flag indicates whether WTLS will be used or not
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if(aSecure) 
		{
		return KErrNotSupported;
		}
	return iAgent->Connect(aBearer, aPort, aSecure);
	}

TInt CSWSWapBoundCLWSPService::MethodInvoke(Wap::TBearer aBearer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, const TUint8 aTransactionId)
/**
A synchronous call to send a method invoke method to the remote host.
@internalComponent
@released
@since v8.0
@param aBearer (in) the bearer that will be used
@param aRemoteHost (in) the remotehost to be sent message to
@param aRemotePort (in) the remoteport to be sent message to
@param aMethod (in) the method that will be invoked on server
@param aURI (in) the uri to build wsp message
@param aReqHeaders (in) the wsp header to be used to build method invoke message
@param aReqBody (in) the wsp body to be used to build method invoke message
@param aTransactionId (in) the transaction Id of the method invoke message
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->SendWspMessage(aBearer, aRemoteHost, aRemotePort, TWSPPduType(aMethod), aURI, aReqHeaders, aReqBody, aTransactionId);
	}

TInt CSWSWapBoundCLWSPService::MethodResult(TDes8& aReqHeaders, TDes8& aReqBody, TPckgBuf<TUint8>& aTransactionIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout)
/**
Request an asynchronous notification upon arrival of the method result messages on the listening connection.
The request completes upon receipt of the message, filling the buffers with as much received data as possible.
A return code will indicate whether further data remains.  The call must be re-issued for subsequent messages
or to receive remaining data from a previous method result message.
@internalComponent
@released
@since v8.0
@param aReqHeaders (out) (client-allocated) - when a method result message arrives the header data is written here
@param aReqBody (out) (client-allocated) - when a method result message arrives the body data is written here
@param aTransactionIdPckg (out) when a method result message arrives an integer ID that uniquely specifies the message is written here
@param aWspStatus (out) when a method result message arrives, the wsp status is written here.
@param aReqStatus (inout) used by the service provider to notify the client when a method result has arrived
@param aTimeout (in) an optional millisecond time-out which allows a timed read to be made.  If no data is received 
within the timeout period the request completes with KErrTimedOut. If a value of 0 is supplied the timeout is infinite.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->ReceiveWspMessage(aReqHeaders, aReqBody, aTransactionIdPckg, aWspStatus, aReqStatus, aTimeout);
	}

void CSWSWapBoundCLWSPService::CancelReq()
/**
Cancel a previously-requested method result notification. If a method result message arrives the client will not be notified.
@internalComponent
@released
@since v8.0
*/
	{
	iAgent->CancelRequest();
	}

TInt CSWSWapBoundCLWSPService::GetLocalPort(Wap::TPort& aPort)
/**
Get the local address of this endpoint.
@internalComponent
@released
@since v8.0
@param aPort (out) the port of the local host
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetLocalPort(aPort);
	}

TInt CSWSWapBoundCLWSPService::GetLocalAddress(HBufC8*& aLocalHost)
/**
Get the local address of this endpoint.
@internalComponent
@released
@since v8.0
@param aLocalHost (inout) the address of the local host. A reference to a HBufC8 pointer should
be passed in. This pointer MUST be null! A HBufC8 will be allocated to hold the address, ownership
of this buffer will be passed over to the client.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetLocalAddress(aLocalHost);
	}

TInt CSWSWapBoundCLWSPService::GetBearer(Wap::TBearer& aBearer)
/**
Get the bearer on which the method result message arrived.
Useful when EAll was specified in Connect()
@internalComponent
@released
@since v8.0
@param aBearer (out) the bearer
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetBearer(aBearer);
	}

TInt CSWSWapBoundCLWSPService::GetServerAddress(HBufC8*& aServerAddress)
/**
Get the remote address of the last wsp method result message
@internalComponent
@released
@since v8.0
Cannot be called when there is an outstanding MethodResult().
@param aServerAddress the address of the remote server.
A reference to a HBufC8 pointer should be passed in. An HBufC8 will be allocated
to hold the address ,ownership of this buffer will be passed over to the client.
@returns KErrNone on successful completion, KErrNotSupported if not implemented or one of the system error codes on failure.
*/
	{
	return iAgent->GetServerAddress(aServerAddress);
	}

