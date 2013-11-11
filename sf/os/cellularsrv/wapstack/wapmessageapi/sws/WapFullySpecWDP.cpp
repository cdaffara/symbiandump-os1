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

#include "WapFullySpecWDP.h"
#include "WapMsgUtils.h"

CSWSWapFullySpecWDPService* CSWSWapFullySpecWDPService::NewL()
/**
Static new function
@internalComponent
@released
@since v8.0
*/
	{
	CSWSWapFullySpecWDPService* me = new(ELeave)CSWSWapFullySpecWDPService();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CSWSWapFullySpecWDPService::CSWSWapFullySpecWDPService()
	: CWapFullySpecDatagramService()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

CSWSWapFullySpecWDPService::~CSWSWapFullySpecWDPService()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	delete iAgent;
	}

void CSWSWapFullySpecWDPService::ConstructL()
/**
Second Phase Constructor
@internalComponent
@released
@since v8.0
*/
	{
	// Parent class construction
	CWapFullySpecDatagramService::ConstructL();
	iAgent=CWdpMessageApiAgent::NewL();
	}

TInt CSWSWapFullySpecWDPService::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TInetAddr /*aInetAddr*/)
/**
Connect to the SWS, opening an endpoint which is to be used only with a single, named remote host.All CWapFullySpecDatagramService implementaions must automatically close this endpoint upon destruction.
@internalComponent
@released
@since v8.0
@param aRemoteHost (in) the bearer-dependent address of the remote host with which the data will be exchanged
@param aRemotePort (in) the port on the remote host to which data will be sent
@param aBearer (in) the bearer to use (not EAll)
@param aInetAddr (in) the ip address of the network interface that should be used in a multihomed system.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->Connect(aRemoteHost, aRemotePort, aBearer, EFalse);
	}

TInt CSWSWapFullySpecWDPService::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer)
/**
Connect to the wapstack, opening an endpoint which is to be used only with a single, named remote host.
@internalComponent
@released
@since v8.0
@param aRemoteHost (in) the bearer-dependent address of the remote host with which the data will be exchange
@param aRemotePort (in) the port on the remote host to which data will be sent
@param aBearer (in) the bearer to use (not EAll)
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInetAddr inetAddr( KInetAddrAny, 0);
	return(Connect(aRemoteHost, aRemotePort, aBearer, inetAddr));
	}

TInt CSWSWapFullySpecWDPService::Send(const TDesC8& aBuffer)
/**
Send data on a fully-specified connection.
@internalComponent
@released
@since v8.0
@param aBuffer (in) the data buffer to be written over the connection
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->SendWdpMessage(aBuffer);
	}

TInt CSWSWapFullySpecWDPService::AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus)
/**
receive data on a bound port.
@internalComponent
@released
@since v8.0
An asynchronous notification is sent to the client when data arrives.
@param aDataSizePckg size of data received
@param aReqStatus (inout) used to notify the client that a datagram was received
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->AwaitRecvDataSize(aDataSizePckg, aReqStatus);
	}

TInt CSWSWapFullySpecWDPService::Recv(TDes8& aBuffer, TBool& aTruncated, TRequestStatus& aReqStatus, TUint32 aTimeout)
/**
receive data on a bound port.
An asynchronous notification is sent to the client when data arrives.
@internalComponent
@released
@since v8.0
@param aBuffer (out) a client-allocated data buffer to be filled with data received. Data that overflows the buffer is discarded.
@param aTruncated (out) indicates whether the received datagram was truncated to fit in the client's supplied buffer
@param aReqStatus (inout) used to notify the client that a datagram was received
@param aTimeout (in) an optional millisecond time-out which allows a timed read to be made.  If no data is received
within the timeout period the request completes with KErrTimedOut. If a value of 0 is supplied the timeout is infinite.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->ReceiveWdpMessage(iHostName, iPort, aBuffer, aTruncated, aReqStatus, aTimeout);
	}

void CSWSWapFullySpecWDPService::CancelRecv()
/**
Cancel a previously requested asynchronous Recv or AwaitRecvDataSize notification.
If a datagram arrives at the local host, it will be discarded.
@internalComponent
@released
@since v8.0
*/
	{
	iAgent->CancelRequest();
	}

TInt CSWSWapFullySpecWDPService::GetLocalPort(Wap::TPort& aPort)
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

TInt CSWSWapFullySpecWDPService::GetLocalAddress(HBufC8*& aLocalHost)
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

TInt CSWSWapFullySpecWDPService::GetDatagramSizes(TUint16& aMaxSize, TUint16& aNominalSize)
/**
Query the WDP bearer for its maximum datagram size and its nominal datagram size.
@internalComponent
@released
@since v8.0
@param aMaxSize (out) the maximum datagram size
@param aNominalSize (out) the nominal datagram size (nominal = size within which a datagram won't
have to be split into smaller individual messages and then re-assembled at the other end).
This will fail for a stream connection
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	// Can only retrieve this info from the capcodec when its a connection, not datagram!
	aMaxSize = KWapStackMaxDatagramSize;
	aNominalSize = KWapStackNorminalDatagramSize;
	return KErrNone;
	}

