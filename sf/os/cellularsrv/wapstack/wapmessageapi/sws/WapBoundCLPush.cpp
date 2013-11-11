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

#include "WapBoundCLPush.h"
#include "WapMsgUtils.h"
#include <wapmsgerr.h>

CSWSWapBoundCLPushService* CSWSWapBoundCLPushService::NewL()
/**
Static new function
@internalComponent
@released
@since v8.0
*/
	{
	CSWSWapBoundCLPushService* me = new(ELeave)CSWSWapBoundCLPushService();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CSWSWapBoundCLPushService::~CSWSWapBoundCLPushService()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	delete iAgent;
	}

CSWSWapBoundCLPushService::CSWSWapBoundCLPushService()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

void CSWSWapBoundCLPushService::ConstructL()
/**
Second Phase Constructor
@internalComponent
@released
@since v8.0
*/
	{
	// Parent class construction
	CWapBoundCLPushService::ConstructL();
	iAgent=CWspMessageApiAgent::NewL();
	}

TInt CSWSWapBoundCLPushService::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure, TInetAddr /*aInetAddr*/)
/**
Opens a Wsp EndPoint which is to be used to listen for subsequent incoming Push messages from any sender;
@internalComponent
@released
@since v8.0
@param aBearer (in) the bearer to listen on (use EAll for all bearers)
@param aPort (in) the port to listen on. If set to 0, a local port will be chosen for the client's first SendTo
@param aSecure (in) security flag indicates whether WTLS will be used or not
@param aInetAddr (in) the address of the adapter to use
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if(aSecure) 
		{
		return KErrNotSupported;
		}
	return iAgent->Connect(aBearer, aPort, aSecure);
	}

TInt CSWSWapBoundCLPushService::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure)
/**
Opens a socket which is to be used to listen for subsequent incoming Push messages from any sender;
@internalComponent
@released
@since v8.0
@param aBearer (in) the bearer to listen on (use EAll for all bearers)
@param aPort (in) the port to listen on. If set to 0, a local port will be chosen for the client's first SendTo
@param aSecure (in) security flag indicates whether WTLS will be used or not
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInetAddr inetAddr( KInetAddrAny, 0);
	return( Connect(aBearer, aPort, aSecure, inetAddr) );
	}

TInt CSWSWapBoundCLPushService::AwaitPush(TDes8& aPushHeaders, TDes8& aPushBody, TPckgBuf<TUint8>& aPushIdPckg, TRequestStatus& aReqStatus)
/**
Request an asynchronous notification upon arrival of the next push messages on the listening connection.
The request completes upon receipt of the message, filling the buffers with as much received data as possible.
A return code will indicate whether further data remains.  The call must be re-issued for subsequent messages
or to receive remaining data from a previous push message.
@internalComponent
@released
@since v8.0
@param aPushHeaders (out) (client-allocated) - when a push message arrives the header data is written here
@param aPushBody (out) (client-allocated) - when a push message arrives the body data is written here
@param aPushIdPckg (out) when a push message arrives an integer ID that uniquely specifies the message is written here
@param aReqStatus (inout) used by the service provider to notify the client when a push has arrived.
EMoreData is returned if more pushed data is available
@retval KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInt err = iAgent->ReceiveWspMessage(aPushHeaders, aPushBody, aPushIdPckg, iWspStatus, aReqStatus, 0);
	if(err == Wap::EMoreData)
	{
		return KErrNone;
	}
	return err;
	}

void CSWSWapBoundCLPushService::CancelAwaitPush()
/**
Cancel a previously-requested push message notification. If a push message arrives the client will not be notified.
@internalComponent
@released
@since v8.0
*/
	{
	iAgent->CancelRequest();
	}

TInt CSWSWapBoundCLPushService::GetLocalPort(Wap::TPort& aPort)
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

TInt CSWSWapBoundCLPushService::GetLocalAddress(HBufC8*& aLocalHost)
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

TInt CSWSWapBoundCLPushService::GetBearer(Wap::TBearer& aBearer)
/**
Get the bearer on which the push message arrived.
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

TInt CSWSWapBoundCLPushService::GetServerAddress(HBufC8*& aRemoteHost)
/**
Get the address of the remote server
@internalComponent
@released
@since v8.0
Cannot be called when there is an outstanding AwaitPush().
@param aRemoteHost the address of the remote server.
A reference to a HBufC8 pointer should be passed in. An HBufC8 will be allocated
to hold the address ,ownership of this buffer will be passed over to the client.
@returns KErrNone on successful completion, KErrNotSupported if not implemented or one of the system error codes on failure.
*/
	{
	return iAgent->GetServerAddress(aRemoteHost);
	}

