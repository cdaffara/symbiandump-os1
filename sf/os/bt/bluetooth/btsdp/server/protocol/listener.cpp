// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "listener.h"
#include "reqhandler.h"
#include "sdpconsts.h"
#include "epocsvr.h"
#include <bt_sock.h>
#include <es_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDP_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("sdplist");
#endif

//The size of a the buffer that's used to read a fragment of the incoming data
//Typical SDP requests are under 100 bytes. 512 is chosen because it is the 
//closest power of 2 to the spec default MTU (672). We expect it is big enough
//to handle most requests while not taking too much unnecessary space
const TInt KRecvFragmentSize = 512;
//The initial size of the buffer used to assemble the fragmented data from multiple 
//continuation read. It is initilised to 4 times of the fragment size in the hope 
//that on demand growing of the buffer will rarely be necessary.
const TInt KRecvInitSize	 = 2048;

CSdpConnection* CSdpConnection::NewL(RSocket& aSocket, CSdpDatabase& aDatabase)
/**
	Spawn of a new connection, using this pre-connected socekt.
**/
	{
	LOG_STATIC_FUNC
	CSdpConnection* self = new (ELeave)CSdpConnection(aSocket, aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpConnection::~CSdpConnection()
	{
	LOG_FUNC
	LOG1(_L("CSdpConnection::~CSdpConnection() 0x%08x"), this);
	Cancel();

	// Delete the active objects that callback on the connection before
	// closing the socket so that they don't try and use a bad handle.
	delete iSDPClientTimer;
	delete iReader;

	iSocket.Close();
	
	// Delete the various buffers used for the connection
	delete iReadFragmentHBuf;
	delete iReadTotalHBuf;
	delete iWriteHBuf;

	iLink.Deque();	// Take ourselves off the connection Q.
	}

/**
Handles the completion of asynchronous write and shutdown operations.
(reads are handled by the separate CSdpConnectionReader active object).
*/
void CSdpConnection::RunL()
	{
	LOG_FUNC
	LOG2(_L("CSdpConnection::RunL() 0x%08x state=%d"), this, iConnState);
	if (iConnState == EShutting)
		{
		delete this;
		}
	else
		{
		// If not in the shutting state, then we must be in the writing state.
		__ASSERT_DEBUG(iConnState == EWriting, PanicServer(ESdpBadState));
		User::LeaveIfError(iStatus.Int()); // On error go to the error handling routine
		// If here then a write successfully completed, so return to
		// the reading state.
		iConnState = EReading;
		}
	}

void CSdpConnection::HandleReadL()
	{
	LOG_FUNC
	iSDPClientTimer->Cancel();	// client is alive, cancel timer
	LOG1(_L("Received %d bytes of request data [contents]"), iReadFragmentBuf.Length());
	FTRACE(FHex(iReadFragmentBuf));

	// Adjust the fragmented reading buffers as appropriate
	TInt remainingDataLength = iRemainingDataLength();
	TInt totalDataLength = remainingDataLength + iReadFragmentBuf.Length();
	if(iReadTotalBuf.Length() + totalDataLength > iReadTotalBuf.MaxLength())
		{
		iReadTotalHBuf = iReadTotalHBuf->ReAllocL(iReadTotalBuf.MaxLength() + totalDataLength);
		iReadTotalBuf.Set(iReadTotalHBuf->Des()); //in case the buffer has been moved in ReAlloc
		}
	iReadTotalBuf.Append(iReadFragmentBuf);

	// Queue the next read up to always have a read outstanding.
	// NOTE: This should be called before the ParseNextPacketL to
	// ensure the connection state is correct.
	QueRead();

	// If we have a complete L2CAP datagram then parse it, otherwise
	// continue the fragmented read.
	if(remainingDataLength > 0)
		{
        LOG1(_L("Receive Continue...(%d bytes left to read)"), iRemainingDataLength());
		}
	else
		{
		ParseNextPacketL();
		iReadTotalBuf.Zero();
		}
	}

void CSdpConnection::DoCancel()
	{
	LOG_FUNC
   	// There isn't async break in handling state, hence something is wrong if we are here
	__ASSERT_DEBUG(iConnState != EHandling, PanicServer(ESdpBadState));

	iReader->Cancel();
	iSocket.CancelWrite();
	// Can't cancel a shutdown request
	}

TInt CSdpConnection::RunError(TInt __DEBUG_ONLY(aError))
	{
	LOG_FUNC
	// Not a recoverable error, disconnect
	LOG1(_L("CSdpConnection::Error(%d). Aborting link!"), aError);
	ShutdownConnection();

	return KErrNone;
	}

void CSdpConnection::ClientTimeout()
	{
	LOG_FUNC
	LOG(_L("CSdpListener::ClientTimeout()"));
	LOG(_L("Disconnecting Inactive client"));
	delete this;
	}

CSdpConnection::CSdpConnection(RSocket& aSocket, CSdpDatabase& aDatabase)
	: CActive(CActive::EPriorityStandard),
	iSocket(aSocket),
	iDatabase(aDatabase),
	iReadFragmentBuf(0,0),
	iReadTotalBuf(0,0),
	iWriteBuf(0,0)
	{
	LOG_FUNC
	LOG1(_L("CSdpConnection::CSdpConnection 0x%08x"), this);
	CActiveScheduler::Add(this);
	}

void CSdpConnection::ConstructL()
	{
	LOG_FUNC
	TInt outMTU;
	User::LeaveIfError(iSocket.GetOpt(KL2CAPOutboundMTUForBestPerformance, KSolBtL2CAP, outMTU));
	TInt mru;
	User::LeaveIfError(iSocket.GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, mru));

	iReadFragmentHBuf = HBufC8::NewL(KRecvFragmentSize);
	iReadFragmentBuf.Set(iReadFragmentHBuf->Des());
	iReadTotalHBuf = HBufC8::NewL(KRecvInitSize); //will grow on demand
	iReadTotalBuf.Set(iReadTotalHBuf->Des());
	
	iWriteHBuf = HBufC8::NewL(outMTU);
	iWriteBuf.Set(iWriteHBuf->Des());
	iWriteBuf.SetMax();
	iResponse.iParams.Set(&iWriteBuf[KSdpPduHeaderSize], 0, outMTU-KSdpPduHeaderSize);

	iSDPClientTimer = CSdpClientTimer::NewL(*this);
	iReader = CSdpConnectionReader::NewL(*this);
	QueRead();
	}

void CSdpConnection::ParseNextPacketL()
/**
	Parse a single SDP PDU out of the L2CAP packet received.
**/
	{
	LOG_FUNC
	LOG(_L("CSdpConnection::ParseNextPacketL()"));
	iConnState = EHandling;	// So that the RunError will handle leaves correctly
	TInt rem = iReadTotalBuf.Length();
	if (rem < KSdpPduHeaderSize)
		{
		// Incomplete header. Let request hadler cope with the error
		User::Leave(KErrUnderflow); // Will cause "Invalid Pdu Size" error
		}
	iRequest.iPduId   = iReadTotalBuf[KSdpPduIdOffset];
	iRequest.iTransId = BigEndian::Get16(&iReadTotalBuf[KSdpPduTransIdOffset]);
	iResponse.iTransId = iRequest.iTransId; // Response has same TransID as request.

	TUint16 paramlen= BigEndian::Get16(&iReadTotalBuf[KSdpPduParamLengthOffset]);
	if ((rem != KSdpPduHeaderSize + paramlen) || (paramlen == 0))
		{
		// Not enough parameter data.
		User::Leave(KErrUnderflow); // Will cause "Invalid Pdu Size" error
		}
	iRequest.iParams.Set(&iReadTotalBuf[KSdpPduHeaderSize], paramlen, paramlen);
	iResponse.iParams.Zero();
	LOG3(_L("Parsed SDP PDU ID %d, transaction ID %d, paramater length %d, [params]"), iRequest.iPduId, iRequest.iTransId, paramlen);
	FTRACE(FHex(iRequest.iParams));

	SdpReqHandler::HandleL(iDatabase, iRequest, iResponse);
	// No leave occured, so response was formed okay.
	// Now write it back to the client
	WriteResponse();
	}

void CSdpConnection::QueRead()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iReader->IsActive(), PanicServer(ESdpReadAlreadyOutstanding));
	iConnState = EReading;
	iSocket.Recv(iReadFragmentBuf, KSockReadContinuation, iReader->iStatus, iRemainingDataLength);
	iReader->SetActive();
	iSDPClientTimer->Start(); // gate the read with a timeout to guard against inactive clients
	}

void CSdpConnection::WriteResponse()
	{
	LOG_FUNC
	LOG1(_L("Writing %d bytes of response data [contents]"), iWriteBuf.Length());
	__ASSERT_DEBUG(!IsActive(), PanicServer(ESdpOutstandingOperation));
	TInt paramLen = iResponse.iParams.Length();
	ASSERT_DEBUG(paramLen <= static_cast<TInt>(KMaxTUint16));
	
	iWriteBuf.SetLength(paramLen + KSdpPduHeaderSize);
	iWriteBuf[KSdpPduIdOffset] = iResponse.iPduId;
	BigEndian::Put16(&iWriteBuf[KSdpPduTransIdOffset], iResponse.iTransId);
	BigEndian::Put16(&iWriteBuf[KSdpPduParamLengthOffset], TUint16(paramLen));
	
	FTRACE(FHex(iWriteBuf));
	iConnState = EWriting;
	iSocket.Write(iWriteBuf, iStatus);
	SetActive();
	}

void CSdpConnection::ShutdownConnection()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!IsActive(), PanicServer(ESdpOutstandingOperation));
	iConnState = EShutting;
	iSocket.Shutdown(RSocket::ENormal, iStatus);
	SetActive();
	}

/**
Handles the completion of a read operation.
*/
void CSdpConnection::ReadComplete(TInt aStatus)
	{
	LOG_FUNC
	LOG1(_L("CSdpConnection::ReadComplete with %d"), aStatus);
	if(aStatus == KErrNone)
		{
		if(iConnState == EReading)
			{
			TRAP(aStatus, HandleReadL());

			if ((aStatus != KErrNone) && (iConnState == EHandling))
				{
				// Problem with received pdu, send an error response to the remote
				aStatus = SdpReqHandler::RunError(aStatus, iRequest, iResponse);
				if (aStatus == KErrNone)
					{
					LOG(_L("Handled bad SDP request. Sending response..."));
					WriteResponse();
			        iReadTotalBuf.Zero();
					}
				}
			}
		else
			{
			// If we complete a read while handling an existing request
			// the other side is behaving badly.  We deal with this by
			// ignoring them and disconnecting the connection.
			LOG1(_L("ReadComplete during another operation(%d)!! Disconnecting..."), iConnState);
			Cancel();
			ShutdownConnection();
			}
		}

	// deal with any unhandled errors by disconnecting
	if(aStatus != KErrNone)
		{
		Cancel();
		ShutdownConnection();
		}
	}

void CSdpConnection::CancelRead()
	{
	LOG_FUNC
	iSDPClientTimer->Cancel();
	iSocket.CancelRecv();	
	}

CSdpConnectionReader* CSdpConnectionReader::NewL(CSdpConnection& aConnection)
	{
	LOG_STATIC_FUNC
	CSdpConnectionReader* self = new(ELeave) CSdpConnectionReader(aConnection);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSdpConnectionReader::CSdpConnectionReader(CSdpConnection& aConnection)
	: CActive(EPriorityStandard)
	, iConnection(aConnection)
	{
	LOG_FUNC
	}

void CSdpConnectionReader::ConstructL()
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CSdpConnectionReader::~CSdpConnectionReader()
	{
	LOG_FUNC
	Cancel();
	}

void CSdpConnectionReader::RunL()
	{
	LOG_FUNC
	iConnection.ReadComplete(iStatus.Int());
	}

void CSdpConnectionReader::DoCancel()
	{
	LOG_FUNC
	iConnection.CancelRead();
	}

CSdpClientTimer* CSdpClientTimer::NewL(CSdpConnection& aConnection)
	{
	LOG_STATIC_FUNC
	CSdpClientTimer* t = new (ELeave) CSdpClientTimer(aConnection);
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop(t);
	return t;
	}


CSdpClientTimer::CSdpClientTimer(CSdpConnection& aConnection)
: CTimer(EPriorityStandard), iConnection(aConnection)
	{
	LOG_FUNC
	}

void CSdpClientTimer::Start()
	{
	LOG_FUNC
	After(KSDPClientTimeout * 1000000);
	}

void CSdpClientTimer::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CSdpClientTimer::RunL()
	{
	LOG_FUNC
	iConnection.ClientTimeout();
	}


CSdpListener* CSdpListener::NewL(RSocketServ& aSockServ, TInt aQueSize, CSdpDatabase& aDatabase)
	{
	LOG_STATIC_FUNC
	CSdpListener* self = NewLC(aSockServ, aQueSize, aDatabase);
	CleanupStack::Pop();
	return self;
	}

CSdpListener* CSdpListener::NewLC(RSocketServ& aSockServ, TInt aQueSize, CSdpDatabase& aDatabase)
	{
	LOG_STATIC_FUNC
	CSdpListener* self = new (ELeave) CSdpListener(aSockServ, aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL(aQueSize);
	return self;
	}


CSdpListener::~CSdpListener()
	{
	LOG_FUNC
	while (!iConns.IsEmpty())
		{
		CSdpConnection* conn = iConns.First();
		delete conn;
		}
	
	Cancel(); //Ensure this active object is not still active.

	iListener.Close();
	iAccepter.Close();
	iDelayAcceptTimer.Close();
	}

CSdpListener::CSdpListener(RSocketServ& aSockServ, CSdpDatabase& aDatabase)
	: CActive(EPriorityLow), iSockServ(aSockServ), iDatabase(aDatabase),
	iIsAcceptDelayed(ETrue), iAcceptDelay(KInitialAcceptDelay),
	iConns(_FOFF(CSdpConnection, iLink))
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CSdpListener::ConstructL(TInt aQueSize)
	{
	LOG_FUNC
	User::LeaveIfError(iDelayAcceptTimer.CreateLocal());
	iQueSize = aQueSize; //set before calling OpenListeningSocket
	QueAcceptL();
	}

void CSdpListener::OpenListeningSocketL()
	{
	LOG_FUNC
	User::LeaveIfError(iListener.Open(iSockServ, _L("L2CAP")));

	TL2CAPSockAddr addr;
	addr.SetPort(KSDPPSM);
	TBTServiceSecurity sdpSecurity;
	sdpSecurity.SetUid(KUidServiceSDP);
	sdpSecurity.SetAuthentication(EMitmNotRequired);
	sdpSecurity.SetAuthorisation(EFalse);
	sdpSecurity.SetEncryption(EFalse);
	sdpSecurity.SetDenied(EFalse);

	addr.SetSecurity(sdpSecurity);

	User::LeaveIfError(iListener.Bind(addr));
	TPckgBuf<TBool> noSecurityRequired;
	noSecurityRequired() = ETrue;

	User::LeaveIfError(iListener.SetOpt(KBTSetNoSecurityRequired, KSolBtSAPBase, noSecurityRequired));
	User::LeaveIfError(iListener.Listen(iQueSize));
	}

void CSdpListener::TryRestartL()
	{
	LOG_FUNC
	if (iIsAcceptDelayed)
		{
		LOG(_L("SDP Server: New session open when accept delayed. Retrying right now"));
		Cancel();
		QueAcceptL();
		}
	}

void CSdpListener::RunL()
	{
	LOG_FUNC
	if(!iIsAcceptDelayed)
		{
		User::LeaveIfError(iStatus.Int());
		CSdpConnection *conn = CSdpConnection::NewL(iAccepter, iDatabase);
		iConns.AddFirst(*conn);
		iAcceptDelay = KInitialAcceptDelay;
		}
	QueAcceptL();
	}


void CSdpListener::DoCancel()
	{
	LOG_FUNC
	if (iListener.SubSessionHandle())
		{
		iListener.CancelAccept();
		iListener.CancelRecv();
		}
	if(iIsAcceptDelayed)
		{
		iDelayAcceptTimer.Cancel();
		}
	}


TInt CSdpListener::RunError(TInt aError)
/**
	Handle leave from RunL.
	If this is called, the accept socket is assumed to be cleaned up already
**/
	{
	LOG_FUNC
	LOG1(_L("CSdpListener::Error(%d)"), aError);
	// Just try to get an accpet going. If this fails, we'll keep trying using 
	// an exponential timeout, as OOM could cause this to keep failing.
	iListener.Close();
	iAccepter.Close();
	iIsAcceptDelayed = ETrue;

	if(iAcceptDelay<=(KMaxAcceptDelay/2))
		{
		//alter ready for next attempt
		iAcceptDelay*=2;
		}
	else if (aError == KErrHardwareNotAvailable)
		{
/*		the hardware has gone and we've met our retry limit
		we'll wait until a new session is created
		before attempting to get a connection
		this makes sense as the services will have detected hardware off
		and typically closed their sockets and handles to SDP.
		At some point services will try to re-register and this will trigger
		the SDP server to attempt to reaccept connections.
*/
		return KErrNone;
		}

	iDelayAcceptTimer.After(iStatus, iAcceptDelay); 
	SetActive();
	
	return KErrNone;
	}

void CSdpListener::QueAcceptL()
	{
	LOG_FUNC
	User::LeaveIfError(iAccepter.Open(iSockServ));
	if(iIsAcceptDelayed)
		{
		OpenListeningSocketL();
		iIsAcceptDelayed = EFalse;
		}
	iListener.Accept(iAccepter, iStatus);
	SetActive();
	}


