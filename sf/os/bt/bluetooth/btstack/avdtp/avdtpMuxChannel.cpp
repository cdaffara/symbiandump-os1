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
// Implements the avdtp mux channel
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpMuxChannel.h"
#include "avdtpTransportSession.h"
#include "avdtp.h"
#include "avdtpSignallingChannel.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

/*static*/ CAvdtpOutboundMuxedMessage* CAvdtpOutboundMuxedMessage::NewL()
	{
	LOG_STATIC_FUNC
	return new (ELeave) CAvdtpOutboundMuxedMessage;
	}


TInt CAvdtpOutboundMuxedMessage::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	__DEBUGGER();
	return 0;
	}

CAvdtpOutboundMuxedMessage::TMuxedMessageOperation CAvdtpOutboundMuxedMessage::TryToAddPacket(TTSID aTSID, RMBufChain& aChain)
	{
	LOG_FUNC
	if (Data().Length() + KALHeaderLength + aChain.Length() > KMaxMuxedPacketLength)
		{
		// this would cause a jumbogram - we don't support them, so send
		return EReadyToSendPacketNotConsumed;
		}
	
	TInt r = AddHeader(aTSID, aChain.Length());

	if (!r)
		{
		// add in the payload
		Data().Append(aChain);
		// see whether to send or not - strategy is that if just under 3 slot we wait
		// for callback, otherwise send now
		if (Data().Length() > KMuxedPacketLengthReadyToSend)
			{
			return EReadyToSendPacketAdded;	
			}
		else
			{
			return EPacketAdded;
			}
		
		}
	else
		{
		return ENoMemory;
		}
	}
	
TInt CAvdtpOutboundMuxedMessage::AddHeader(TTSID aTSID, TInt aPacketLen)
	{
	LOG_FUNC
	// grow chain for adding tsid - put in same chain for ease of sending
	TRAPD(r, Data().PrependL(KALHeaderLength));
	if (!r)
		{
		TUint8& ALhdr = Data().First()->Ptr()[0];
		ALhdr = static_cast<TUint8>(aTSID << KALHeaderTSIDOffset);
		// make sure frag = 0
		ALhdr &= ~(1<<KALHeaderFOffset);
		// set LCODE - no support of jumbograms, so only choice of 2 LCODEs
		// i.e. we always set the msb of LCODE then figure out lsb
		ALhdr &= ((1 << KALHeaderLCODE_MSBOffset) | (aPacketLen >= 255 ? 1 : 0));
		}
	return r;	
	}

TInt CAvdtpOutboundMuxedMessage::Reset()
	{
	LOG_FUNC
	// get a packet of 1byte for header, then we jion on payload later
	TRAPD(res, Data().AllocL(KALHeaderLength));
	return res;
	}

#ifdef HAVE_INBOUND_MUX_PACKET
/*static*/ CAvdtpInboundMuxedMessage* CAvdtpInboundMuxedMessage::NewL(CMuxChannel& aMuxChannel)
	{
	LOG_STATIC_FUNC
	return new (ELeave) CAvdtpInboundMuxedMessage(aMuxChannel);
	}

CAvdtpInboundMuxedMessage::CAvdtpInboundMuxedMessage(CMuxChannel& aMuxChannel)
: iMuxChannel(aMuxChannel)
	{
	LOG_FUNC
	}

void CAvdtpInboundMuxedMessage::Reset()
	{
	LOG_FUNC
	// get a packet of 1byte for header, then we jion on payload later
	Data().Free();
	}
	
TInt CAvdtpInboundMuxedMessage::NewData(TUint aCount)
	{
	LOG_FUNC
	LOG1(_L("Count = %d (check for >1 !)"), aCount);	
	
	TInt ret = KErrNone;
	// get the data now?
	// need to parse of the AL header - and give to relevant session
	// as we don't support fragmentation we can "assume" the this l2cap packet
	// is a whole mux packet - of course we need to test the frag bit and moan if set
	while (aCount--)
		{
		RMBufChain frag;
		ret = iMuxChannel.GetData(frag);
		Data().Append(frag);
		}
		
	LOG(_L("Inbound packet built - now parse"))
	
	return ret;
	}

#endif //HAVE_INBOUND_MUX_PACKET

CMuxChannel* CMuxChannel::NewL(CAvdtpProtocol& aProtocol,
								const TBTDevAddr& aRemoteDevice,
								TTCID aRemotelyAssignedTCID/*=KInvalidTCID*/)
	{
	LOG_STATIC_FUNC
	CMuxChannel* s = new (ELeave) CMuxChannel(aProtocol, aRemoteDevice);
	CleanupStack::PushL(s);
	s->ConstructL(aRemotelyAssignedTCID);
	CleanupStack::Pop(s);
	return s;
	}
	
void CMuxChannel::ConstructL(TTCID aRemotelyAssignedTCID)
	{
	LOG_FUNC
//	iHeartbeat = CHeartbeat::NewL()
	iMuxSendTimer = CPeriodic::NewL(EPriorityHigh);
#ifdef HAVE_INBOUND_MUX_PACKET
	iInboundMessage = CAvdtpInboundMuxedMessage::NewL(*this);
#endif
	iOutboundMessage = CAvdtpOutboundMuxedMessage::NewL();
	
	CSignallingChannel* sc = Protocol().FindSignallingChannel(RemoteAddress());
	if (!sc)
		{
		User::Leave(KErrDisconnected);
		}
		
	if (aRemotelyAssignedTCID==KInvalidTCID)
		{
		User::LeaveIfError(sc->TCIDManager().GetTCID(iTCID));
		}
	else
		{
		User::LeaveIfError(sc->TCIDManager().GrabTCID(iTCID, aRemotelyAssignedTCID));
		}
		
	User::LeaveIfError(iSessions.Append(iMediaSessions.Array()));
	User::LeaveIfError(iSessions.Append(iReportingSessions.Array()));
	User::LeaveIfError(iSessions.Append(iRecoverySessions.Array()));
	}

CMuxChannel::CMuxChannel(CAvdtpProtocol& aProtocol,
						 const TBTDevAddr& aRemoteDevice)
: CTransportChannel(aProtocol, aRemoteDevice),
#ifdef SESSION_ITERATOR_CONCRETE
  iIter(iMediaSessions, iReportingSessions, iRecoverySessions)
#else
	iIter(iSessions.Array())
#endif
	{
	LOG_FUNC
	}
	
CMuxChannel::~CMuxChannel()
	{
	LOG_FUNC
	iMuxSendTimer->Cancel();
	iTCID.Close();
	}
	
TTCID CMuxChannel::TCID() const
	{
	LOG_FUNC
	return iTCID.TCID();
	}
	
/*static*/ TInt CMuxChannel::MuxSendIntervalCb(TAny* aCMuxChannel)
	{
	LOG_STATIC_FUNC
	// mux send interval has expired
	// this will send the packet regardless of whether it's full
	// thus guarding against noone filling the packet, and keeping some finite latency
	CMuxChannel& me = *static_cast<CMuxChannel*>(aCMuxChannel);
	(void)me.DoSend();
	return KErrNone;
	}
		
TUint CMuxChannel::SendPacket(TTSID aTSID, RMBufChain& aPacket)
	{
	LOG_FUNC
	// also allows the odd case of "muxing" from one session on one stream!
	// this is useful as it allows the same "fill a baseband packet for 10p" game
	TUint wrote = 0;
	
	// we always know all our bound sessions can send on this channel
	CAvdtpOutboundMuxedMessage::TMuxedMessageOperation result = iOutboundMessage->TryToAddPacket(aTSID, aPacket);
	
	switch (result)
		{
		case CAvdtpOutboundMuxedMessage::ENoMemory:
			Error(KErrNoMemory);
			break;
		case CAvdtpOutboundMuxedMessage::EReadyToSendPacketAdded:
		case CAvdtpOutboundMuxedMessage::EReadyToSendPacketNotConsumed:
			wrote = DoSend();
			break;
		case CAvdtpOutboundMuxedMessage::EPacketAdded:
			{
			// guard against no other write by starting timer
			TCallBack cb(MuxSendIntervalCb, this);

			iMuxSendTimer->Start(KMuxSendInitial,
								 KMuxSendPeriod,
								 cb);
	
			break;
			}
		}
		
	if (result==CAvdtpOutboundMuxedMessage::EReadyToSendPacketNotConsumed && wrote !=0)
		{
		// need to start new muxed packet with aChain as payload
		// old one was sent though, and we notice that l2cap hasn't blocked us
		TInt r = iOutboundMessage->Reset();
		if (!r)
			{
			// recurse
			wrote = SendPacket(aTSID, aPacket);	
			}
		else
			{
			// for *this* write we need to tell them we're blocked
			wrote = 0;
			}
		}
	
	return wrote;
	}
	
TUint CMuxChannel::DoSend()
	{
	LOG_FUNC
	TUint wrote =0;
	if (iLogicalChannel)
		{
		wrote = iLogicalChannel->Write(iOutboundMessage->Data(), 0);
		}
	return wrote;
	}
/*	
TInt CMuxChannel::SessionDataReceived(TTSID aTSID, RMBufChain& aChain)
	{
	// put on pool and notify
	#pragma message("is there n pools for eg media, or just one?")
	}
*/	
TInt CMuxChannel::AttachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType, TL2CapConfig::TChannelPriority aPriority)
/**
Protocol must ensure it has found appropriate muxchannel for
Recovery packets
*/
	{
	LOG_FUNC
	RArray<TUserPlaneTransportSessionState>* sessionArray = NULL;
	
	switch (aType)
		{
		case EMedia:
		sessionArray = &iMediaSessions;
		break;
		
		case EReporting:
		sessionArray = &iReportingSessions;
		break;
		
		case ERecovery:
		sessionArray = &iRecoverySessions;
		break;
		}
	
	__ASSERT_DEBUG(sessionArray, Panic(EAVDTPBadSessionAttachToTransportChannel));
	
	TInt err = sessionArray->Append(TUserPlaneTransportSessionState(aSession, aPriority));
	if(err == KErrNone)
		{
		UpdateChannelPriority();
		}
	return err;
	}

void CMuxChannel::UpdateChannelPriority()
	{
	LOG_FUNC
	
	TL2CapConfig::TChannelPriority maxPriority = TL2CapConfig::ELow;
	
	MaxChannelPriority(maxPriority, iMediaSessions);
	MaxChannelPriority(maxPriority, iReportingSessions);
	MaxChannelPriority(maxPriority, iRecoverySessions);
	
	if(iLogicalChannel)
		{
		TPckgBuf<TL2CapConfig> configBuf;
		configBuf().ConfigureChannelPriority(maxPriority);
		(void)iLogicalChannel->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configBuf);
		}
	}


void CMuxChannel::MaxChannelPriority(TL2CapConfig::TChannelPriority& aMaxPriority, const RArray<TUserPlaneTransportSessionState>& aSessions)
	{
	LOG_STATIC_FUNC
	
	for(TInt i=0; i < aSessions.Count(); ++i)
		{
		TL2CapConfig::TChannelPriority priority = aSessions[i].iChannelPriority;
		if(aMaxPriority < priority)
			{
			aMaxPriority = priority;
			}
		}
	}

/**
Actually activate multiplexing mode in packet domain
- we are either multiplexing for one session, or many and should expect and generate
AL headers in the packets.

This channel may have previously been declared as a multiplexor inbound, as we support
muxing, but remote SEP may not have configured it: another remote SEP (same device)
may then choose to use muxing, which is why we need this "upgradeability"
*/
/*
void CMuxChannel::ActiveMultiplexer()
	{
	iActivatedMultiplexer = ETrue;
	}
*/	
TBool CMuxChannel::CouldAttachSession(const TAvdtpSockAddr& aAddr)
	{
	LOG_FUNC
	// need to see if this is for the same device, and not an incompatible session
	if (aAddr.BTAddr()!=RemoteAddress())
		{
		// can't go to different device!
		return EFalse;
		}
	
	// we know of the different session types by class design
	// so we just want to check that:
	// if this prospective session is Recovery that we dont' have other sessions with same SEID
	// or vice versa
	TInt i=0;
	if (aAddr.Session() == ERecovery)
		{
		for (i=0;i<iMediaSessions.Count();i++)
			{
			if (iMediaSessions[i].iSession.SEID()==aAddr.SEID())
				{
				// got media session for same stream - can't attach
				return EFalse;
				}
			}
		for (i=0;i<iReportingSessions.Count();i++)
			{
			if (iReportingSessions[i].iSession.SEID()==aAddr.SEID())
				{
				// got reporting session for same stream - can't attach
				return EFalse;
				}			
			}
		}
	return ETrue;
	}

	
void CMuxChannel::DetachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType)
	{
	LOG_FUNC
	// find it - for runtime efficiency the session tells us its type
	// we also don't make it virtual to save ram/rom and runtime (can't inline)
	
	// remove it - having asserted that it is found
	RArray<TUserPlaneTransportSessionState>* array = NULL;
		
	switch (aType)
		{
		case EMedia:
			array = &iMediaSessions;
			break;
		case EReporting:
			array = &iReportingSessions;
			break;
		case ERecovery:
			array = &iRecoverySessions;
			break;
		}
	
#ifdef _DEBUG
	TInt found =0;
#endif

	// find in the array
	for (TInt i=0; i<array->Count(); i++)	
		{
		if (&(*array)[i].iSession == &aSession)
			{
			// found it
			array->Remove(i);	// don't delete as don't own
#ifdef _DEBUG
			found++;
#else
			break;
#endif
			}
		}
	__ASSERT_DEBUG(found==1, Panic(EAVDTPBadSessionDetachFromTransportChannel));

	UpdateChannelPriority();
	CheckForClose();
	}

CServProviderBase* CMuxChannel::ObtainSAP()
	{
	__ASSERT_DEBUG(EFalse, Panic(EAvdtpTransferSapCalledForMuxChannel));
	return NULL;
	}
	
// from logical channel
void CMuxChannel::CanSend()
	{
	LOG_FUNC
	iLogicalChannelBlocked = EFalse;
	
	// send muxed packet, and see if we're still unblocked
	TUint r = DoSend();
	
	if (!r)
		{
		//iterate over all sessions to induce them to send
		iIter.Reset();
		
		while(iIter)
			{
			iIter++.iSession.CanSend();
			}		
		}
	}
	
	
void CMuxChannel::NewData(TUint aCount)
/**
Must get out of L2CAP so that we can read the TSID (and check internally the TCID)
*/
	{
	LOG_FUNC
#ifdef HAVE_INBOUND_MUX_PACKET
	iInboundPacket->NewData(aCount);
#else	
	LOG1(_L("Count = %d (check for >1 !)"), aCount);	
	
	// get the data now?
	// need to parse of the AL header - and give to relevant session
	// as we don't support fragmentation we can "assume" the this l2cap packet
	// is a whole mux packet - of course we need to test the frag bit and moan if set
	while (aCount--)
		{
		RMBufChain packet;
		TInt err = iLogicalChannel->GetData(packet,0,0); //returns negative error code or the number of datagrams ( = 1 )
		if (err > 0)
			{
			// without frag one L2CAP datagram is one muxed packet
			LOG(_L("Inbound packet built - now parse"))

			while (packet.Length())
				{
				LOG1(_L("Muxed packet length remaining =%d byte"), packet.Length());
				// iterate over all ALHeaders in muxed packet
				// check for frag - we don't support, so if set drop - remote is bad
				TUint8 ALHeader = *packet.First()->Ptr();
				packet.TrimStart(1);// ALHeader

				LOG1(_L("AL Header = 0x%02x"), ALHeader);
				
				if (!(ALHeader & (1<<KALHeaderFOffset)))
					{
					// F-bit not set, so proceed
					TTSID tsid(ALHeader >> 3);
					TInt lenCode(ALHeader & 0x3);
					TInt len=0;
					
					switch (lenCode)
						{
						case 0x00:
							LOG(_L("no len field"));
							// no length (for last packet in the muxed packet
							len = packet.Length();	
							break;
						case 0x01:
							LOG(_L("16 bit len field"));
							// 16 bit length field
							if (packet.Length()>=2)
								{
								len = *reinterpret_cast<TUint16*>(packet.First()->Ptr());
								packet.TrimStart(2);
								}
							else
								{
								// bad mux packet, try to parse rest lurking in l2cap
								packet.Free();
								continue;							
								}
							break;
						case 0x02:
							LOG(_L("9 bit len field MSB=0"));
							// 9 bit length field = basically 8 bit but don't add 256
							len = *packet.First()->Ptr();
							// drop through
						case 0x03:
							LOG(_L("9 bit len field MSB=1"));
							// 9 bit length field = basically 8 bit but add 256
							len += 256;
							packet.TrimStart(1);
							break;
						} // switch
					LOG1(_L("Session data len = %d bytes"), len);
					// split chain so that we can pass relevant bit to session
					RMBufChain sessionData;
					
					iIter.Reset();
					while (iIter)
						{
						CUserPlaneTransportSession* session = &(iIter++).iSession;
						if (session && session->TSID() == tsid)
							{
							session->NewData(sessionData);
							break;
							}
						}															
					} // end if F bit test			
				} // end while data left in muxed packet
			} // end if no error from l2cap
		} // end aCount while
#endif
	}

/**
Logical Channel has Disconnected - no point our staying around
Error all sessions and die.
*/
void CMuxChannel::Disconnect()
	{
	LOG_FUNC
	iLogicalChannel = NULL;
	Error(KErrDisconnected);
	}
	
/**
Iterate over all sessions and error them.
*/
void CMuxChannel::Error(TInt aError, TUint /*aOperationMask*/)
	{
	LOG_FUNC
	iIter.Reset();
	while (iIter)
		{
		iIter++.iSession.ChannelError(aError);
		}
	}
	
			
void CMuxChannel::CheckForClose()
	{
	LOG_FUNC
	if (!iMediaSessions.Count() &!iReportingSessions.Count() &!iRecoverySessions.Count())
		{
		Protocol().TransportChannelClosing(*this);
		CloseLogicalChannel();
		}
	}
	
/**
Called by mux message to get data from logical channel
*/
TInt CMuxChannel::GetData(RMBufChain& aRxBuffer)
	{
	return iLogicalChannel->GetData(aRxBuffer,0,0);
	}

void CMuxChannel::TransportSessionBlocked(TAvdtpTransportSessionType /*aSession*/, TBool /*aBlocked*/)
	{
	LOG_FUNC
	
	}
	
	
// helper class

template <class T>	
TSessionIterator<T>::TSessionIterator(const TArray< const TArray<T> >& aArrayOfArrays)
: iArrays(aArrayOfArrays)
	{
	Reset();
	}
					 
template <class T>
const T& TSessionIterator<T>::operator++(TInt /*aPostIncrementDummy*/)
	{
	const T& t = iArrays[iArray].operator[](iArrayIndex);
	DoIncrement();
	return t;
	}

template <class T>
void TSessionIterator<T>::DoIncrement()
	{
	// increment index of current array
	iArrayIndex++;
	// if we max out then move onto next array
	if (iArrayIndex == iArrays[iArray].Count())
		{
		// got to the end of the iArray-th array, go onto the next
		iArrayIndex =0;
		iArray++;
		}
	// the next array may be blank, as may the next next etc, so skip them
	do
		{
		// it may be that we get to the very end of all the arrays
		if (iArray == iArrays.Count())
			{
			// got the last array, finish
			iFinished = ETrue;
			}
		if (!iFinished && !iArrays[iArray].Count())
			{
			// skip empty array
			iArray++;
			}
		else
			{
			// this array has stuff in it
			break;
			}
		}
	while (iArray<iArrays.Count());
	if (iArray == iArrays.Count())
		{
		// got the last array, finish
		iFinished = ETrue;
		}	
	}


/*
Conversion operator - 'returns' a Bool. ETrue if not completed
Required to have an iterator of the usual form.
*/
template <class T>	
TSessionIterator<T>::operator TBool()
	{
	return !iFinished;
	}
	
template <class T>	
void TSessionIterator<T>::Reset()
	{
	iFinished = EFalse;
	iArrayIndex =0;
	iArray =0;
	}

