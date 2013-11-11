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
#include <bt_sock.h>
#include <btsdp.h>
#include "sdp.h"
#include "sdpnetdb.h"
#include "sdppdu.h"
#include "sdpstackutil.h"
#include "sdpclient.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDP);
#endif

CSdpClient::CSdpClient(CSdpProtocol& aProtocol, CProtocolBase& aL2CAP)
	: iNetDbProviders(_FOFF(CSdpNetDbProvider, iLink)),
	iOutboundQ(_FOFF(CSdpPdu, iLink)),
	iProtocol(aProtocol),
	iL2CAP(aL2CAP)
	{
	TCallBack cb(IdleTimerExpired, this);
	iIdleTimerEntry.Set(cb);
	}

void CSdpClient::ConstructL()
	{
	iBoundSAP=iL2CAP.NewSAPL(KSockSeqPacket);
	iBoundSAP->SetNotify(this);
	iBoundSAP->SecurityCheck(this);
	CheckForIdle(KSDPIdleTimeout*4);// No point hanging about if no one connects
	}

CSdpClient* CSdpClient::NewL(CSdpProtocol& aProt, CProtocolBase& aL2CAP)
	{
	CSdpClient* self = new(ELeave) CSdpClient(aProt, aL2CAP);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpClient::~CSdpClient()
	{
	delete iNextPacket;
	delete iBoundSAP;
	while (!iOutboundQ.IsEmpty())
		{
		delete iOutboundQ.First();
		}
	while (!iNetDbProviders.IsEmpty())
		{// Tell any lingering netdb's that we're gone.
		iNetDbProviders.First()->ClientDown();
		// Note -- this will cause idle timer to be re-queued, 
		// so we must cancel it *after* taking netdbs down.
		}
	CancelIdleTimer();
	}

/****************************************************************************/
/* 
   Commands from protocol
*/

void CSdpClient::Open(TBTDevAddr& aAddr)
/**
    Create lower-layer link to the remote device.
   
	The bound sap will be used to
    create an L2CAP link to the remote end over which the SDP
    PDUs will flow.  The client will then run this channel.
**/
	{
	__ASSERT_DEBUG(!ChannelUp(), Panic(ESdpClientAlreadyConnected));
	iRemoteAddr=aAddr;
	iBoundSAP->AutoBind();
	TL2CAPSockAddr remote;
	remote.SetBTAddr(aAddr);
	remote.SetPort(KSDPPSM);  // Always 0x01
	if(iBoundSAP->SetRemName(remote)!=KErrNone)
		{
		Panic(ESdpErrorSettingAddress);
		}
	TPckgBuf<TBool> noSecurityRequired;
	noSecurityRequired() = ETrue;
	
	if(iBoundSAP->SetOption(KSolBtSAPBase, KBTSetNoSecurityRequired, noSecurityRequired)!=KErrNone)
		{
		Panic(ESdpAgentErrorSettingNoSecurity);
		}
	iBoundSAP->ActiveOpen();  // Signals ConnectComplete eventually!
	}

void CSdpClient::AddNetDbProvider(CSdpNetDbProvider& aNetDbProvider)
	{
	CancelIdleTimer();
	iNetDbProviders.AddFirst(aNetDbProvider);
	if (ChannelUp())
		aNetDbProvider.ClientUp();
	}

void CSdpClient::RemoveNetDbProvider(CSdpNetDbProvider& aNetDbProvider)
	{
	
   	if(!iOutboundQ.IsEmpty())
		{

		CSdpPdu* request = iOutboundQ.First();
		CSdpNetDbProvider* netdb = request->NetDbProvider();
		if(netdb == &aNetDbProvider)
			request->iNetDbProvider = 0;

		TDblQueIter<CSdpPdu> iter(iOutboundQ);
		while(iter)
			{
			request = iter++;
			if(request->NetDbProvider()==&aNetDbProvider)
				//NB 1st one is NULL so won't be deleted
				delete request;
			}
		}

	aNetDbProvider.iLink.Deque();
	if (iNetDbProviders.IsEmpty())
		{
		CheckForIdle(KSDPIdleTimeout);
		}
	}

/****************************************************************************/
/* 
   Commands for netdb provider
*/

inline TUint8 DEHeader(TUint8 aType, TUint8 aSizeIndex)
/**
	Make a data element header from aType and aSizeIndex.
	aType occupies top 5 bits, aSizeIndex the bottom 3 bits.
**/
	{// Move aType 3 bits across.
	return (TUint8)((aType << 3) | (aSizeIndex & 0x7));
	}

void CSdpClient::ServiceSearchRequest(CSdpNetDbProvider& aNetDbProvider,
		TUint16 aMaxCount, const TUUID &aUUID, const TDesC8& aContState)
	{
	if (!ChannelUp())
		{
		aNetDbProvider.Error(KErrSdpClientNotConnected);
		return;
		}
	CSdpPdu* pdu = new CSdpPdu(&aNetDbProvider);
	if (!pdu)
		{
		aNetDbProvider.Error(KErrNoMemory);
		return;
		}

	TPtrC8 uuid(aUUID.ShortestForm());
	TInt len = uuid.Length();
	TUint8 szIndex = 0;
	while (len >>= 1)
		++szIndex;
	__ASSERT_DEBUG(uuid.Length() == (1<<szIndex), Panic(ESdpBadUUID));

	pdu->SetPduId(EServiceSearchRequest);
	pdu->SetTransid(NextTransId());
	pdu->PutByte(DEHeader(6,5));  // Header: DES, 1 byte length
	pdu->PutByte(TUint8(1+uuid.Length()));// Size: Length of one UUID element
	pdu->PutByte(DEHeader(3,szIndex)); //	 Header: 'len' byte UUID
	pdu->PutData(uuid);			  //   Data: The UUID
	pdu->PutBigEndian16(aMaxCount);	// Maximum Service record count
	pdu->PutByte((TUint8)aContState.Length());	// Cont state length
	pdu->PutData(aContState);
	pdu->SetLength();
	EnqueOutboundPdu(*pdu);
	}

void CSdpClient::ServiceAttributeRequest(CSdpNetDbProvider& aNetDbProvider,
		TUint aRecordHandle, TUint16 aMaxLength, TBool aRange,
		TUint aAttribute, const TDesC8& aContState)
	{
	if (!ChannelUp())
		{
		aNetDbProvider.Error(KErrSdpClientNotConnected);
		return;
		}
	CSdpPdu* pdu = new CSdpPdu(&aNetDbProvider);
	if (!pdu)
		{
		aNetDbProvider.Error(KErrNoMemory);
		return;
		}

	pdu->SetPduId(EServiceAttributeRequest);
	pdu->SetTransid(NextTransId());
	pdu->PutBigEndian32(aRecordHandle);
	pdu->PutBigEndian16(aMaxLength);
	pdu->PutByte(DEHeader(6,5));        // Header: DES, 1 byte length
	if (aRange)
		{// Range of attribute IDs. Send 1 4-byte uint.
		pdu->PutByte(5);			    // Size: Length of 16 uint element
		pdu->PutByte(DEHeader(1,2));    //	 Header: 4 byte integer
		pdu->PutBigEndian32(aAttribute);//   Attribute ID range
		}
	else
		{// Single attribute ID. Send 1 2-byte uint.
		pdu->PutByte(3);			    // Size: Length of 32 uint element
		pdu->PutByte(DEHeader(1,1));    //	 Header: 2 byte integer
		pdu->PutBigEndian16(static_cast<TUint16>(aAttribute));//   Single attribute ID
		}
	pdu->PutByte(static_cast<TUint8>(aContState.Length()));	// Cont state length
	pdu->PutData(aContState);
	pdu->SetLength();
	EnqueOutboundPdu(*pdu);
	}

void CSdpClient::EncodedRequest(CSdpNetDbProvider& aNetDbProvider,
								TUint8 aPduRequestId,
								const TDesC8& aEncodedData)
	{
	if (!ChannelUp())
		{
		aNetDbProvider.Error(KErrSdpClientNotConnected);
		return;
		}
	CSdpPdu* pdu = new CSdpPdu(&aNetDbProvider);
	if (!pdu)
		{
		aNetDbProvider.Error(KErrNoMemory);
		return;
		}

	pdu->SetPduId(aPduRequestId);
	pdu->SetTransid(NextTransId());
	pdu->PutData(aEncodedData);
	pdu->SetLength();
	EnqueOutboundPdu(*pdu);
	}


/****************************************************************************/
/* 
   Protocol MSocketNotify functions
   Upcalls from L2CAP
*/
void CSdpClient::NewData(TUint aCount)
/**
	New data arrived on lower SAP.
	Read out the data straight away.
	@param	aCount Number of packets that have arrived.
**/
	{
	LOG1(_L("SDP: New data, count %d"), aCount);

	TPckgBuf<TInt> mru;
	TInt err = iBoundSAP->GetOption(KSolBtL2CAP, KL2CAPInboundMTU, mru);
	if (err == KErrNone && (!iNextPacket || mru() > iNextPacket->Size()))
		{
		delete iNextPacket;
		iNextPacket = HBufC8::New(mru());
		if (iNextPacket == 0)
			{
			err = KErrNoMemory;
			}
		}
	if(err)
		{
		Error(err, EErrorFatal);
		return;
		}

	TPtr8 data = iNextPacket->Des();
	while (aCount)
		{
		data.SetMax();
		iBoundSAP->GetData(data, 0);
		LOGHEXDESC(data)
		ParseNextPacket();
		--aCount;
		}
	}

void CSdpClient::CanSend()
	{
	L2CAPBlocked(EFalse);
	TryToSend();
	}

void CSdpClient::ConnectComplete()
/**
	Right, we're connected.
	Let all the netdbs waiting on us know.
	Should check L2CAP MTU's now, but for now we're just
	trusting that they'll be large enough to hold our very
	small requests and responses, as we can't fragment anyway.
**/
	{
	ChannelUp(ETrue);
	TDblQueIter<CSdpNetDbProvider> iter(iNetDbProviders);
	
	while(iter)
		{
		(iter++)->ClientUp();
		}
	}

void CSdpClient::ConnectComplete(const TDesC8&)
	{
	Panic(ESdpConnectWithData);
	}

void CSdpClient::ConnectComplete(CServProviderBase&)
	{
	Panic(ESdpClientPassiveConnect);
	}

void CSdpClient::ConnectComplete(CServProviderBase&,const TDesC8& )
	{
	Panic(ESdpClientPassiveConnect);
	}

void CSdpClient::CanClose(TDelete /*aDelete*/)
/**
	Line ourselves up ready for deletion.
	The bound SAP calling this upcall will be tided up when we are,
	so there's no point getting carried away in doing it here.
**/
	{
	iRemoteAddr.Reset();
	ChannelUp(EFalse);
	TDblQueIter<CSdpNetDbProvider> iter(iNetDbProviders);
	while(iter)
		{//Tell the netdb's we're down.
		(iter++)->ClientDown();
		}
	CheckForIdle(0);	// Async call back to delete ourselves!
	}

void CSdpClient::CanClose(const TDesC8&, TDelete)
	{
	Panic(ESdpDisconnectWithData);
	}

void CSdpClient::Error(TInt aError,TUint anOperationMask)
/**
   Something's gone wrong.

   We get told whether it's just this operation or all that are
   affected, but we'll just assume that all of them are bust for
   now.
**/
	{
	LOG2(_L("SDP: L2CAP socket error: %d, mask: 0x%x"), aError, anOperationMask);
	if (!(anOperationMask & ~EErrorIoctl))
		return;//Ignore Ioctl errors.
	if (anOperationMask & EErrorFatal)
		ChannelUp(EFalse);
	TDblQueIter<CSdpNetDbProvider> iter(iNetDbProviders);
	CSdpNetDbProvider* netdb;
	
	while(iter)
		{
		netdb=iter++;
		netdb->Error(aError);
		}

	iRemoteAddr.Reset();
	CheckForIdle(0);
	}

void CSdpClient::Disconnect()
/**
	We're no longer connected
**/
	{
	LOG(_L("SDP bearer disconnected. Closing..."))
	CanClose();
	}

void CSdpClient::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	Panic(ESdpDisconnectWithData);
	}

void CSdpClient::IoctlComplete(TDesC8* /*aBuf*/)
	{
	// This must be an internally generated ioctl
	LOG(_L("SDP: Unexpected IoctlComplete upcall!"));
	}

/****************************************************************************/
/* 
   Internal functions
*/

void CSdpClient::ParseNextPacket()
/**
	Parse PDUs out of the L2CAP packet received.
	This is actually a bit general, as we can have at most
	one PDU per packet (SDP v1.1).
**/
	{
	TPtr8 packet(iNextPacket->Des());
	TInt rem = packet.Length();
	TUint8* ptr = rem ? &packet[0] : 0;
	while (rem >= KSdpPduHeaderSize)
		{
		TUint8  pduid   = ptr[KSdpPduIdOffset];
		TUint16 transid = BigEndian::Get16(ptr+KSdpPduTransIdOffset);
		TUint16 paramlen= BigEndian::Get16(ptr+KSdpPduParamLengthOffset);
		if (rem < KSdpPduHeaderSize + paramlen)
			break;
		ptr += KSdpPduHeaderSize;
		TPtrC8 params(ptr, paramlen);
		HandlePDU(pduid, transid, params);
		ptr += paramlen;
		rem -= (KSdpPduHeaderSize+paramlen);
		}
	if (rem)
		{//Partial pdu received. Bin it
		LOG1(_L("SDP: Frame error: %d extra bytes received\n"), rem);
		}
	}

void CSdpClient::HandlePDU(TUint8 aPduId, TUint16 aTransId, const TDesC8& aParams)
	{
 	if(!RequestOutstanding())
 		{// The server is spontaneously sending us data. DROP it
 		return;
 		}

	// There can only be one oustanding pdu,
	// So the first pdu on the list must be the one.
	__ASSERT_DEBUG(!iOutboundQ.IsEmpty(),Panic(ESdpNoRequestPdu));
   	CSdpPdu* request = iOutboundQ.First();
	CSdpNetDbProvider* netdb = request->NetDbProvider();
	TUint16 reqTransId = request->TransId();

	RequestOutstanding(EFalse);
	// Request complete. So delete. (Removes from Q, so we can carry on sending).
	delete request;
	TryToSend();

	if(!netdb)
		return;

	if (reqTransId != aTransId)
   		{//DROP
		LOG2(_L("SDP: Unexpected PDU; pdu id %d, transaction id: %d\n"), aPduId, aTransId);
		netdb->Error(KErrSdpBadResultData);
		return;
		}

	switch (aPduId)
		{
	case EErrorResponse:
		HandleErrorResponse(netdb, aParams);
		break;

	case EServiceSearchResponse:
		HandleServiceSearchResponse(netdb, aParams);
		break;

	case EServiceAttributeResponse:
		HandleServiceAttributeResponse(netdb, aParams);
		break;

	default:
		LOG1(_L("SDP: Unexpected PDU type; pdu id: %d\n"), aPduId);
		netdb->Error(KErrSdpBadResultData);
		};

	}

void CSdpClient::HandleErrorResponse(CSdpNetDbProvider* aNetDbProv, const TDesC8& aParams)
/**
	We've got an error response.

	Parameter format is...
	<pre>
	  Byte  |  0   |  1  |  2   |  3  ....
	        | Error Code | Error info (dependent on code)
	</pre>
**/
	{
	const TInt KMinLength = 2;
	const TInt KErrorCodeOffset = 0;

	TInt err = KErrGeneral; // The code to send up...

	if (aParams.Length() < KMinLength)
		{// Way! Bad error response! drop it..
		err = KErrSdpPeerError;
		}
	else
		{
		TSdpErrorCodes code = (TSdpErrorCodes)BigEndian::Get16(&aParams[KErrorCodeOffset]);
		switch (code)
			{
		case EInvalidSdpVersion:
			err=KErrSdpUnsupportedVersion;
			break;
		case EInvalidServiceRecordHandle:
			err=KErrSdpBadRecordHandle;
			break;
		case EInvalidContinuationState:
			err=KErrSdpBadContinuationState;
			break;
		case EInvalidRequestSyntax:
		case EInvalidPduSize:
		default:
			err=KErrSdpServerRejectedRequest;
			}
		}

	if (!aNetDbProv)
		{
		LOG1(_L("SDP: Error response with no owning aNetDbPr %d\nWill error them all!\n"), err);
		Error(err, EErrorAllOperations);
		return;
		}
	aNetDbProv->Error(err);
	}

void CSdpClient::HandleServiceSearchResponse(CSdpNetDbProvider* aNetDbProv, const TDesC8& aParams)
/**
	Got service search response.
	Pass it right on up to the net db.
**/
	{
	if (!aNetDbProv)
		{
		LOG(_L("SDP: Service search response with no owning aNetDbPr\n"));
		return;
		}
	aNetDbProv->QueryComplete(aParams);
	}

void CSdpClient::HandleServiceAttributeResponse(CSdpNetDbProvider* aNetDbProv, const TDesC8& aParams)
/**
	We've got an attribute request response.
	Pass the params up to the net db. It'll be dealt with client side!

**/
	{
	if (!aNetDbProv)
		{
		LOG(_L("SDP: Attribute response with no owning aNetDbPr\n"));
		return;
		}
	aNetDbProv->QueryComplete(aParams);
	}

CSdpPdu* CSdpClient::NewPdu(CSdpNetDbProvider* aNetDb)
	{
	return new CSdpPdu(aNetDb);
	}

TUint16 CSdpClient::NextTransId()
	{
	return ++iTransId ? iTransId : ++iTransId;
	}

void CSdpClient::EnqueOutboundPdu(CSdpPdu& aPdu)
	{
	iOutboundQ.AddLast(aPdu);
	TryToSend();
	}

void CSdpClient::TryToSend()
/**
	Try to send out a pdu.
	Can do this if we have something to send, L2CAP is not blocked,
	and there is no outstanding request.
	There are 3 different places TryToSend can be called,
	corresponding to invalidation of these three guards.

	Pdu tries to emit for is one. It can make this if we will have something to emit, L2CAP is
	not obstructed, and it does not have no prominent order. It has 3 different places that
	TryToSend can be called, corresponding to invalidation of these three holds. 
**/
	{
	if (RequestOutstanding())
		return;
	if (iOutboundQ.IsEmpty())
		return;
	if (L2CAPBlocked())
		return;

	// To pipeline, we should iterate here, until no more can fit in the packet,
	// then send packet. Should really mark the pdus as sent too.
	CSdpPdu& pdu = *iOutboundQ.First();
	// We just try to write, regardless of MTU! If it doesn't fit, we'll
	// get back an error from remtoe. But our requests generally should fit.
	if (!iBoundSAP->Write(pdu.Data(), 0))
		{
		L2CAPBlocked(ETrue);
		return;
		}
	// PDU will be dequed when it's response returns.
	// This is easy to do, as we're not pipelining requests in L2CAP packets,
	// so at most only one pdu can be outstanding at once.
	RequestOutstanding(ETrue);
	}

void CSdpClient::CheckForIdle(TInt aTimeoutInSeconds)
	{
	CancelIdleTimer();
	iIdleQueued = ETrue;
	// Add 10 to allow aTimeInSeconds to be 0.
    BTSocketTimer::Queue((aTimeoutInSeconds*1000000)+10, iIdleTimerEntry);
	}

void CSdpClient::CancelIdleTimer()
	{
	if (iIdleQueued)
		{
	    BTSocketTimer::Remove(iIdleTimerEntry);
		iIdleQueued = EFalse;
		}
	}

TInt CSdpClient::IdleTimerExpired(TAny* aPtr)
/**
	Static function called whenever idle timer expires.
	Take us down. Gracefully, mind!
**/
	{
	CSdpClient* self = static_cast<CSdpClient*>(aPtr);
	__ASSERT_DEBUG(self->iIdleQueued, Panic(ESdpClientBadIdleState));
	self->iIdleQueued = EFalse;
	if (self->ChannelUp())
		{// Take it down, first.
		// Clear remote address, so we don't accept further netdbs.
		self->iRemoteAddr.Reset();
		self->ChannelUp(EFalse);
		self->iBoundSAP->Shutdown(CServProviderBase::ENormal);
		self->CheckForIdle(KSDPIdleTimeout*2);
		}
	else
		{// Channels down. Say good bye!
		self->iProtocol.ClientDown(*self);	// Who'll promtly delete 'self'
		}
	return FALSE;
	}

TInt CSdpClient::CheckPolicy(const TSecurityPolicy& /*aPolicy*/, const char* /*aDiagnostic*/)
	{
	return KErrNone;
	}
