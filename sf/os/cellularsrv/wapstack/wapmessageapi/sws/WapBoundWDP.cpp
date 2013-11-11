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

#include "WapBoundWDP.h"
#include "WapMsgUtils.h"


CSWSWapBoundWDPService* CSWSWapBoundWDPService::NewL()
/**
Static new function
@internalComponent
@released
@since v8.0
*/
	{
	CSWSWapBoundWDPService* me = new(ELeave)CSWSWapBoundWDPService();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CSWSWapBoundWDPService::CSWSWapBoundWDPService()
	: CWapBoundDatagramService()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

CSWSWapBoundWDPService::~CSWSWapBoundWDPService()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	delete iAgent;	
	}

void CSWSWapBoundWDPService::ConstructL()
/**
Second Phase Constructor
@internalComponent
@released
@since v8.0
*/
	{
	// Parent class construction
	CWapBoundDatagramService::ConstructL();
	iAgent=CWdpMessageApiAgent::NewL();	
	}

TInt CSWSWapBoundWDPService::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TInetAddr /*aInetAddr*/)
/** 
Connect to the SWS, opening an endpoint that can be used to listen for subsequent incoming datagrams.
@internalComponent
@released
@since v8.0
@param aBearer the bearer to listen on (use EAll for all bearers)
@param aPort the port to listen on. If set to 0, a local port will be chosen for the client's first SendTo
@param aInetAddr the ip address of the network interface that should be used in a multihomed system.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->Connect(aBearer, aPort, EFalse);
	}

TInt CSWSWapBoundWDPService::Connect(Wap::TBearer aBearer, Wap::TPort aPort)
/**
Connect to the SWS, opening an endpoint that can be used to listen for subsequent incoming datagrams.
@internalComponent
@released
@since v8.0
@param aBearer the bearer to listen on (use EAll for all bearers)
@param aPort the port to listen on. If set to 0, a local port will be chosen for the client's first SendTo
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInetAddr inetAddr( KInetAddrAny, 0);
	return (Connect(aBearer, aPort, inetAddr));
	}

TInt CSWSWapBoundWDPService::SendTo(const TDesC8& aRemoteHost, Wap::TPort aRemotePort,
									 const TDesC8& aBuffer, Wap::TBearer aBearer)
/**
Send data to a remote endpoint.
@internalComponent
@released
@since v8.0
@param aRemoteHost the bearer-dependent address of the remote host to which the data will be sent
@param aRemotePort the port on the remote host to which the data will be sent
@param aBuffer the data buffer to be written over the connection
@param aBearer the bearer to be used, if the bound connection was opened with 'all'
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->SendWdpMessage(aBuffer, aRemoteHost, aRemotePort, aBearer);
	}

TInt CSWSWapBoundWDPService::AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus)
/**
Wait for a datagram to be received, and discover how large a buffer is required to retrieve it.
This asyncronous method waits for a datagram to be received and will then complete allowing the client to discover how large a buffer
is needed to retrieve the entire datagram that has been received.
A subsiquent call to RecvFrom with a buffer of sufficent size will then allow the client to retrieve the datagram fully.
@internalComponent
@released
@since v8.0
@param aDataSizePckg the size of data subsequently received, in bytes
@param aReqStatus used to signal when a data size is known
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->AwaitRecvDataSize(aDataSizePckg, aReqStatus);
	}

TInt CSWSWapBoundWDPService::RecvFrom(TDes8& aRemoteHost, Wap::TPort& aRemotePort, TDes8& aBuffer,
									  TBool& aTruncated, TRequestStatus& aReqStatus, TUint32 aTimeout)
/**
receive data on a bound port.
An asynchronous notification is sent to the client when data arrives.
@internalComponent
@released
@since v8.0
@param aRemoteHost the bearer-dependent address of the remote host from which the data was received
@param aRemotePort the port on the remote host from which the data was received
@param aBuffer a client-allocated data buffer to be filled with data received. Data that overflows the buffer is discarded.
@param aTruncated indicates whether the received datagram was truncated to fit in the client's supplied buffer
@param aReqStatus used to notify the client that a datagram was received
@param aTimeout an optional millisecond time-out which allows a timed read to be made.  If no data is received 
within the timeout period the request completes with KErrTimedOut. If a value of 0 is supplied the timeout is infinite.
@retval TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->ReceiveWdpMessage(aRemoteHost, aRemotePort, aBuffer, aTruncated, aReqStatus, aTimeout);
	}

void CSWSWapBoundWDPService::CancelRecv()
/**
Cancel a previously requested asynchronous RecvFrom or AwaitRecvDataSize notification. 
If a datagram arrives at the local host, it will be discarded.
@internalComponent
@released
@since v8.0
*/
	{
	iAgent->CancelRequest();
	}

TInt CSWSWapBoundWDPService::GetLocalPort(Wap::TPort& aPort)
/** 
Get the local port of this endpoint.Useful if one was chosen automatically.
@internalComponent
@released
@since v8.0
@param aPort the port number
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetLocalPort(aPort);
	}

TInt CSWSWapBoundWDPService::GetLocalAddress(HBufC8*& aLocalHost)
/**
Get the local address of this endpoint.
@internalComponent
@released
@since v8.0
@param aLocalHost the address of the local host. A reference to a HBufC8 pointer should 
be passed in. This pointer MUST be null! A HBufC8 will be allocated to hold the address, ownership 
of this buffer will be passed over to the client.
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetLocalAddress(aLocalHost);
	}

TInt CSWSWapBoundWDPService::GetBearer(Wap::TBearer& aBearer)
/**
Get the bearer on which a received datagram arrived.
Useful when EAll was specified in Connect()
@internalComponent
@released
@since v8.0
@param aBearer the bearer
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iAgent->GetBearer(aBearer);
	}

TInt CSWSWapBoundWDPService::GetDatagramSizes(TUint16& aMaxSize, TUint16& aNominalSize)
/**
Query the WDP bearer for its maximum datagram size and its nominal datagram size.
@internalComponent
@released
@since v8.0
@param aMaxSize the maximum datagram size
@param aNominalSize the nominal datagram size (nominal = size within which a datagram won't
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

