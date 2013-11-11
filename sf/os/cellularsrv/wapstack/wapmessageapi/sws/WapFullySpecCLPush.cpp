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

#include "WapFullySpecCLPush.h"
#include "WapMsgUtils.h"
#include <wapmsgerr.h>

CSWSWapFullySpecCLPushService* CSWSWapFullySpecCLPushService::NewL()
/**
Static new function
@internalComponent
@released
@since v8.0
*/
	{
	CSWSWapFullySpecCLPushService* me = new(ELeave)CSWSWapFullySpecCLPushService();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CSWSWapFullySpecCLPushService::~CSWSWapFullySpecCLPushService()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	delete iAgent;
	}

CSWSWapFullySpecCLPushService::CSWSWapFullySpecCLPushService()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

void CSWSWapFullySpecCLPushService::ConstructL()
/**
Second Phase Constructor
@internalComponent
@released
@since v8.0
*/
	{
	// Parent class construction
	CWapFullySpecCLPushService::ConstructL();
	iAgent=CWspMessageApiAgent::NewL();
	}

TInt CSWSWapFullySpecCLPushService::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure, TInetAddr /*aInetAddr*/)
/**
Opens a socket which is to be used only with a single, named remote host.
@internalComponent
@released
@since v8.0
@param aRemoteHost the bearer-dependent address of the remote host with which the data will be exchanged
@param aRemotePort the port on the remote host to which data will be sent
@param aBearer the bearer to use (not EAll)
@param aSecure security flag indicates whether WTLS will be used or not
@param aInetAddr the address of the adapter to use
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if(aSecure) 
		{
		return KErrNotSupported;
		}
	return iAgent->Connect(aRemoteHost, aRemotePort, aBearer, aSecure);
	}

TInt CSWSWapFullySpecCLPushService::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure)
/**
Opens a socket which is to be used only with a single, named remote host.
@internalComponent
@released
@since v8.0
@param aRemoteHost (in) the bearer-dependent address of the remote host with which the data will be exchanged
@param aRemotePort (in) the port on the remote host to which data will be sent
@param aBearer (in) the bearer to use (not EAll)
@param aSecure (in) security flag indicates whether WTLS will be used or not
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInetAddr inetAddr( KInetAddrAny, 0);
	return( Connect(aRemoteHost, aRemotePort, aBearer, aSecure, inetAddr) );
	}

TInt CSWSWapFullySpecCLPushService::AwaitPush(TDes8& aPushHeaders, TDes8& aPushBody, TPckgBuf<TUint8>& aPushIdPckg, TRequestStatus& aReqStatus)
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
@param aReqStatus (inout) used by the service provider to notify the client when a push has arrived
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInt err = iAgent->ReceiveWspMessage(aPushHeaders, aPushBody, aPushIdPckg, iWspStatus, aReqStatus, 0);
	if(err == Wap::EMoreData)
	{
		return KErrNone;
	}
	return err;
	}

void CSWSWapFullySpecCLPushService::CancelAwaitPush()
/**
Cancel a previously-requested push message notification.  If a push message arrives the client will not be notified.
@internalComponent
@released
@since v8.0
*/
	{
	iAgent->CancelRequest();
	}

TInt CSWSWapFullySpecCLPushService::GetLocalPort(Wap::TPort& aPort)
/**
Get the local port of this endpoint.Useful if one was chosen automatically.
@internalComponent
@released
@since v8.0
@param aPort (out) the port number
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetLocalPort(aPort);
	}

TInt CSWSWapFullySpecCLPushService::GetLocalAddress(HBufC8*& aLocalHost)
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

