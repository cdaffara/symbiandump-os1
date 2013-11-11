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
// Implements the avdtp protocol object
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <e32def.h>
#include <bluetoothav.h>
#include <bluetooth/avctptypes.h> // for PID used for pre-authorisation
#include "IncomingConnListener.h"
#include "avdtp.h"
#include "avdtpsap.h"
#include "avdtpTransportChannel.h"
#include "avdtpTransportSession.h"
#include "avdtpSignallingChannel.h"
#include "avdtpStream.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

/**
Protocol object constructor.

@internalComponent
@param aSecMan The bluetooth security manager
@see CBTSecMan
@return A pointer to the AVDTP protocol object
*/
CAvdtpProtocol::CAvdtpProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
:	CBluetoothProtocolBase(aSecMan, aControlPlane, aCodMan),
	iTransportChannels(_FOFF(CTransportChannel, iProtocolQLink)),
	iClosingTransportChannels(_FOFF(CTransportChannel, iProtocolQLink)),
	iSignallingChannels(_FOFF(CSignallingChannel, iProtocolQLink)),
	iStreams(_FOFF(CAVStream, iProtocolQLink)),
	iSecondarySAPs(_FOFF(CAvdtpSAP, iLink))
	{
	CONNECT_LOGGER
	LOG_FUNC
	TCallBack cb(TryToClose, this);
	iIdleTimerEntry.Set(cb);
	}

/**
Protocol object destructor.
We don't clear up the SAPs as they are owned by ESock and it is up to it to destroy them.

@internalComponent
*/
CAvdtpProtocol::~CAvdtpProtocol()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iIdleEntryQueued, Panic(EAvdtpProtocolDeletionWhilstIdling));
	RemoveIdleTimerEntry();

	if (iLowerProtocol)
		iLowerProtocol->Close();  // Matches the bind

	delete iLogicalChannelFactory;
	delete iStreamStateFactory;
	delete iRemoteSEPCache;
	
	__ASSERT_ALWAYS(iStreams.IsEmpty(), Panic(EAvdtpProtocolDyingWithStreamsOnQueue));
	__ASSERT_ALWAYS(iSignallingChannels.IsEmpty(), Panic(EAvdtpProtocolDyingWithSignallingChannelsOnQueue));
	__ASSERT_ALWAYS(iTransportChannels.IsEmpty(), Panic(EAvdtpProtocolDyingWithTransportChannelsOnQueue));
	__ASSERT_ALWAYS(iClosingTransportChannels.IsEmpty(), Panic(EAvdtpProtocolDyingWithTransportChannelsOnQueue));

	CLOSE_LOGGER
}

/**
Static protocol object factory function.
Leaves the protocol object on the cleanup stack.

@internalComponent
@see CBTSecMan
@leave KErrNoMemory if the protocol object could not be created
@param aSecMan The bluetooth security manager

@return A pointer to the AVDTP protocol object
*/
CAvdtpProtocol* CAvdtpProtocol::NewLC(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
	LOG_STATIC_FUNC	
	CAvdtpProtocol* self = new(ELeave) CAvdtpProtocol(aSecMan, aControlPlane, aCodMan);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Static protocol object factory function.

@internalComponent
@see CBTSecMan
@leave KErrNoMemory if the protocol object could not be created
@param aSecMan The bluetooth security manager
@return A pointer to the AVDTP protocol object
*/
CAvdtpProtocol* CAvdtpProtocol::NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
	LOG_STATIC_FUNC
	CAvdtpProtocol* self = CAvdtpProtocol::NewLC(aSecMan, aControlPlane, aCodMan);
	CleanupStack::Pop(self);
	return self;
	}

/**
Protocol object second-phase construction

@internalComponent
*/
void CAvdtpProtocol::ConstructL()
	{
	LOG_FUNC
	iStreamStateFactory = CAVStreamStateFactory::NewL();
	iRemoteSEPCache = CRemoteSEPCache::NewL();
	}

/**
Pre-binding initialise.

Alloc any stuff we need. This will only ever be called once during the lifetime of this protocol.

@internalComponent
@leave KErrNoMemory if the state factory cannot be created
@param aTag The string identifier for the protocol from the ESock ini file
*/
void CAvdtpProtocol::InitL(TDesC& /*aTag*/)
	{
	LOG_FUNC
	}

/**
Binding complete.

@internalComponent
*/
void CAvdtpProtocol::StartL()
	{
	LOG_FUNC
	// Should check that we're bound now.
	if (!iLowerProtocol)
		{
		User::Leave(KErrAvdtpNotBound);
		}
	
	iLogicalChannelFactory = CLogicalChannelFactory::NewL(*this, *iLowerProtocol);
	}

// From higher protocol
void CAvdtpProtocol::BindL(CProtocolBase* /*protocol*/, TUint /* id*/)
	{
	LOG_FUNC
	// Could register the protocol that's bound to us
	// (e.g. "see" whether we support RTCP/RTP-FEC (if that
	// protocol suite becomes ESOCK-side!)
	}

/**
Request by Protocol Mgr to bind to the specified protocol. We can only be bound
to one lower layer protocol, so the function leaves if we are already bound.

@internalComponent
@leave KErrAvdtpAlreadyBound if we are already bound, and any other leave from
     the lower protocol's BindL()
@param aProtocol The protocol we need to bind to.
*/
void CAvdtpProtocol::BindToL(CProtocolBase* aProtocol)
	{
	LOG_FUNC
	if(!iLowerProtocol)
		{
#ifdef _DEBUG
		TServerProtocolDesc prtDesc;
		aProtocol->Identify(&prtDesc);

		if(prtDesc.iAddrFamily!=KBTAddrFamily ||
		   prtDesc.iProtocol!=KL2CAP)
			{
			User::Leave(KErrBtEskError);
			}
#endif

		iLowerProtocol=static_cast<CBluetoothProtocolBase*>(aProtocol);
		iLowerProtocol->BindL(this, 0); // id not used
		iLowerProtocol->Open();
		}
	else
		{
	    User::Leave(KErrAvdtpAlreadyBound);
		}
	}

void CAvdtpProtocol::Close()
	{
	LOG_FUNC
	CProtocolBase::Close();
	}


/** 
Create a new SAP: all SAPs are datagram.

The SAP will eventually construct a session, to which the SAP delegates
most of its work.  This is reminiscent of the Adaptor pattern.

The delegate represents a session as per the AVDTP specification.

Note that this implmentation adds to the available session types with
the signalling session.  This allows for multiplexed use of the signalling
channel.

The SAP returned is owned by the caller -- this protocol will not clean it up.
ESock uses this function to create a new SAP, and ESock will delete when it is
finished with it.

In future this could take care of secondary saps - make them !datagram

@internalComponent
@leave KErrNotSupported if aSockType is not datagram

@param aSockType The socket type for the SAP:
@return A pointer to a new SAP
*/
CServProviderBase* CAvdtpProtocol::NewSAPL(TUint aSockType)
	{
	LOG_FUNC
	CAvdtpSAP* sap = NULL;
	
	switch(aSockType)
		{		
		case KSockDatagram:
			sap = CAvdtpSAP::NewL(*this);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	return sap;
	}

/**
Identify the protocol.

The descriptor is filled in to identify the protocol to ESock.

@internalComponent
@param aDesc A pointer to the descriptor to be filled in
*/
void CAvdtpProtocol::Identify(TServerProtocolDesc* aDesc) const
	{
	LOG_FUNC
	ProtocolIdentity(aDesc);
	}

/**
Fill in the protocol descriptor.

This is a static utility function to fill in the protocol details.
Called from family too.

@internalComponent
@param aDesc A pointer to the descriptor to be filled in
*/
void CAvdtpProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc)
	{
	LOG_STATIC_FUNC
	aDesc->iName			= KAVDTPProtocolName;
	aDesc->iAddrFamily		= KBTAddrFamily;
	aDesc->iSockType		= KSockDatagram; // because of bearing rtp
	aDesc->iProtocol		= KAVDTP;
	aDesc->iVersion			= TVersion(KBTMajor,KBTMinor,KBTBuild);
	aDesc->iByteOrder		= ELittleEndian;
	aDesc->iServiceInfo		= KAvdtpServiceInfo;
	aDesc->iNamingServices	= NULL;
	aDesc->iSecurity		= KSocketNoSecurity;
	aDesc->iMessageSize		= KSocketMessageSizeUndefined; // whatever l2cap dynamically. not static knowledge of max
	aDesc->iServiceTypeInfo	= ESocketSupport | ECantProcessMBufChains |
							  EPreferDescriptors | EUseCanSend;
	aDesc->iNumSockets		= 100;
	}


/**
Our reference is now zero, so start to close.

We don't actually close, merely Q a timer for a later close down.
This close can be prempted by another open.

@internalComponent
*/
void CAvdtpProtocol::CloseNow()
	{
	LOG_FUNC
	iClosePending = ETrue;
	QueIdleTimerEntry();
	}

void CAvdtpProtocol::Open()
/**
Request to open the protocol.

The protocol may be repeatedly opened and closed.  The order of calls is
InitL, [Open *n , Close * n,  CloseNow] * m etc.

@internalComponent
*/
	{
	LOG_FUNC
	iClosePending = EFalse;
	RemoveIdleTimerEntry();
	CProtocolBase::Open();
	}


/**
Create a signalling channel for listening - this has the special address of 0
*/
CSignallingChannel* CAvdtpProtocol::CreateSignallingChannelForListening()
	{
// the signalling channel with addr=0 *is* the listener
	LOG_FUNC
	return GetSignallingChannel(TBTDevAddr(0));
	}

/**
For an open platform we need to support shared use of the Signalling Channel
Signalling sessions ask for this then and attach themselves to it
*/
CSignallingChannel* CAvdtpProtocol::FindSignallingChannel(const TBTDevAddr& aRemoteAddr)
	{
	LOG_FUNC
	CSignallingChannel* c = NULL;
	TDblQueIter<CSignallingChannel> iter(iSignallingChannels);
	
	while (iter)
		{
		c = iter++;
		if (c->iRemoteAddress==aRemoteAddr)
			{			
			return c;
			}
		}
	return NULL;	
	}
	
CSignallingChannel* CAvdtpProtocol::FindListeningSignallingChannel()
	{
	LOG_FUNC
	CSignallingChannel* c = NULL;
	TDblQueIter<CSignallingChannel> iter(iSignallingChannels);
	
	while (iter)
		{
		c = iter++;
		if (c->IsListening())
			{			
			return c;
			}
		}
	return NULL;	
	}
	
CSignallingChannel* CAvdtpProtocol::GetSignallingChannel(const TBTDevAddr& aRemoteAddr) 
	{
	LOG_FUNC
	// if not found, create one else return it
	CSignallingChannel* ch = FindSignallingChannel(aRemoteAddr);
	if (!ch)
		{
		TRAP_IGNORE(ch = CSignallingChannel::NewL(*this, *iLogicalChannelFactory, aRemoteAddr));
		if (ch)
			{
			iSignallingChannels.AddFirst(*ch);
			}
		}
	return ch;
	}

/**
This function transfers sessions attached to a listening channel to the connected signalling
channel. It is only effective if the connected channel is the result of an active open rather
than an incoming connection. This function does nothing in the latter case.
*/
void CAvdtpProtocol::ConnectSignallingListeners(CSignallingChannel& aConnectedSignallingChannel)
	{
	LOG_FUNC
	CSignallingChannel* listeningChannel = FindListeningSignallingChannel();
	
	if (listeningChannel)
		{
		// take off all the sessions, and re-attach to the connected signalling channel
		TDblQueIter<XAvdtpSignalReceiver> listeningIter(const_cast<TDblQue<XAvdtpSignalReceiver>&>
											(listeningChannel->Users()));
		XAvdtpSignalReceiver* user;
		
		while ((user = listeningIter++) != NULL)
			{
			listeningChannel->DetachSignallingUser(*user);
			aConnectedSignallingChannel.AttachSignallingUser(*user);
			}	
		}
	// else do nothing
	}
	
/**
Find a transport channel for a transport session to bind to, allocate if needed
Ownership is NOT returned - channels own themselves
The protocol keeps a reference to the channel in a queue
so that it can hand out further references (in the form of pointers) as needed

Algorithm is thus:

1) Deduce session type this channel would be for: s
2) Deduce device address this channel would be for: d
3) Deduce Stream this channel would be for: S
4) Find the stream container object for S - this say if we can mux
5) If mux then find a TC with different S if s = Recovery
6) elseif !mux then create new TC
*/
CTransportChannel* CAvdtpProtocol::GetTransportChannel(const TAvdtpSockAddr& aRemoteAddr,
													 	TBool aUseMux,
													 	TTCID aRemotelyAssignedTTCID/*=KInvalidTCID*/)
	{
	LOG_FUNC
	
	CTransportChannel* tc = NULL;

	if (aUseMux)
		{
		// try to find a TC we already have on which this session can go
		TDblQueIter<CTransportChannel> iter(iTransportChannels);
		
		while (iter)
			{
			CTransportChannel* possibleChannel = iter++;
			if (possibleChannel->CouldAttachSession(aRemoteAddr))
				{
				// found one
				tc = possibleChannel;
				break;
				}
			}
		}
		
	if (!tc)
		{
		// we now know we do actually need a new TC
		// ***but don't connect it yet***
		__ASSERT_DEBUG(FindSignallingChannel(aRemoteAddr.BTAddr()), Panic(EAvdtpSignallingChannelShouldExist));
		// spec is unclear but it is the case that only mux channels get assigned TCIDs
		TInt err = KErrNone;
		if (aUseMux)
			{
			TRAP(err, tc = CTransportChannel::NewMuxChannelL(*this, 
											aRemoteAddr.BTAddr(),
								 			aRemotelyAssignedTTCID));
			}
		else
			{
			TRAP(err, tc = CTransportChannel::NewDirectChannelL(*this, 
											aRemoteAddr.BTAddr()));
			}
		if (err==KErrNone)
			{
			iTransportChannels.AddFirst(*tc);
			}
		}
	
	return tc;
	}
	
	
/**
Find transport channel with TCID==aTCID
If not found return NULL - do not create (like the Get method)
There is no need to search for Direct Channels so this is asserted against
*/
CTransportChannel* CAvdtpProtocol::FindMuxChannel(TTCID aTCID)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aTCID!=KDirectChannelTCID, Panic(EAvdtpSearchingForDirectChannel));

	TDblQueIter<CTransportChannel> iter(iTransportChannels);
	CTransportChannel* tc = NULL;
	
	while (iter)
		{
		tc = iter++;
		if (tc->TCID() == aTCID)
			{
			// found it
			__ASSERT_DEBUG(tc->TCID()!=KDirectChannelTCID, Panic(EAvdtpIncorrectlyFoundDirectChannel));
			break;
			}
		}
	
	return tc;
	}

/**
Called by the transport channel when it's dead
eg when link lost, or graceful close completed

@internalComponent
@param aTransportChannel The channel that's down
*/
void CAvdtpProtocol::TransportChannelDown(CTransportChannel& aTransportChannel)
	{
	LOG_FUNC
#ifdef _DEBUG
	TDblQueIter<CTransportChannel> openiter(iTransportChannels);
	TBool found = EFalse;
	while (openiter)
		{
		// look on open queue
		if (openiter++ == &aTransportChannel)
			{
			found = ETrue;
			break;
			}
		}
	if (!found)
		{
		// look on closing queue
		TDblQueIter<CTransportChannel> closingiter(iClosingTransportChannels);
		while (closingiter)
			{
			// look on open queue
			if (closingiter++ == &aTransportChannel)
				{
				found = ETrue;
				break;
				}
			}
		}
	if (!found) Panic(EAvdtpProtocolToldToRemoveUnknownTransportChannel);
#endif
	// take off our queue, and leave the channel to delete itself
	
	aTransportChannel.iProtocolQLink.Deque();

	if (ShouldClose())
		{
		QueIdleTimerEntry();
		}
	}

/**
Called by transport channel when it is no longer able to be used, but is closing
*/
void CAvdtpProtocol::TransportChannelClosing(CTransportChannel& aTransportChannel)
	{
	LOG_FUNC
#ifdef _DEBUG
	TDblQueIter<CTransportChannel> iter(iTransportChannels);
	TBool found = EFalse;
	while (iter)
		{
		if (iter++ == &aTransportChannel)
			{
			found = ETrue;
			break;
			}
		}
	if (!found) Panic(EAvdtpProtocolToldToRemoveUnknownTransportChannel);
#endif
	// take off our queue, and leave the channel to delete itself
	
	aTransportChannel.iProtocolQLink.Deque();
	iClosingTransportChannels.AddFirst(aTransportChannel);

	// don't start looking to see if idle yet
	}
	
/**
Called by the signalling channel when it's dead.

@internalComponent
@param aSignallingChannel The channel that's down
*/
void CAvdtpProtocol::SignallingChannelDown(CSignallingChannel& aSignallingChannel)
	{
	LOG_FUNC
#ifdef _DEBUG
	TDblQueIter<CSignallingChannel> iter(iSignallingChannels);
	CSignallingChannel* sigch = NULL;
	while (iter)
		{
		sigch = iter++;
		if (sigch==&aSignallingChannel)
			{
			break;
			}
		}
#ifndef _OOM_TEST
	__ASSERT_DEBUG(sigch, Panic(EAvdtpSignallingChannelShouldExist));
#endif
#endif
	
	aSignallingChannel.iProtocolQLink.Deque();

	if (ShouldClose())
		{
		QueIdleTimerEntry();
		}
	}
	
/**
Asynchronous callback function.toe check if should close

@internalComponent
@param aProtocol The protocol object
@return EFalse to indicate the callback does not need to be reissued
*/
TInt CAvdtpProtocol::TryToClose(TAny* aProtocol)
	{
	LOG_STATIC_FUNC
	CAvdtpProtocol* protocol	= static_cast<CAvdtpProtocol*>(aProtocol);
	protocol->iIdleEntryQueued	= EFalse;

	if (protocol->ShouldClose())
		{
		protocol->CanClose();
		}

	return EFalse; // don't try to callback again
	}
   
void CAvdtpProtocol::RemoveIdleTimerEntry()
/**
Takes us off the idle timer Q if we're on it.

@internalComponent
*/
	{
	LOG_FUNC
	if (iIdleEntryQueued)
		{
		BTSocketTimer::Remove(iIdleTimerEntry);
		iIdleEntryQueued = EFalse;
		}
	}

/**
Q a timer to delete us. If already running, not reset.

@internalComponent
*/
void CAvdtpProtocol::QueIdleTimerEntry()
	{
	if (!iIdleEntryQueued)
		{
		BTSocketTimer::Queue(KAvdtpIdleTimeout, iIdleTimerEntry);
		iIdleEntryQueued = ETrue;
		}
	}

/**
Called to check whether we can close down.

@internalComponent
@return ETrue if the protocol can close, EFalse if not
*/
TBool CAvdtpProtocol::ShouldClose()
	{
	LOG_FUNC
	return (iClosePending &&
			iTransportChannels.IsEmpty() &&
			iClosingTransportChannels.IsEmpty() &&
			iSignallingChannels.IsEmpty());
	}
	
/**
Called from logical channel factory when bearer listening is required
@internalComponent
*/
TInt CAvdtpProtocol::StartProtocolListening()
	{
	LOG_FUNC
	// because we do non-default security we override the base class
	// and new the incoming listener ourselves
	TRAPD(err, DoStartAvdtpListeningL());
	return err;
	}
	
/**
Helper to actually start listening. This protocol doesn't use the base class
implementation as it needs to do a bit more.
@internalComponent
*/
void CAvdtpProtocol::DoStartAvdtpListeningL()
	{
	LOG_FUNC
	// Check that we haven't already got an iListener.
	// NOTE: in production code we will leak an iListener
	// these are fairly small so not too severe.
	__ASSERT_DEBUG(iListener == NULL, Panic(EAvdtpStartedListeningAgain));
	CServProviderBase* sap = iLowerProtocol->NewSAPL(KSockSeqPacket);
	
	// We listen with streaming mode by default.  This allows fallback to eRTM
	// or basic mode if the remote requests it.  We have to do it this way round
	// and assume the remote will correctly request a reliable channel for signalling
	// as if we listen expecting a reliable channel we can't fall back to unreliable.
	
	TPckgBuf<TL2CapConfig> configBuf;
	sap->GetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configBuf);
	
	configBuf().ConfigureUnreliableChannel(500);
	configBuf().ConfigureUnreliableDesiredChannel(500, TL2CapConfig::EDefaultRetransmission);
	sap->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, configBuf);

	TBTSockAddr localAddr;
	localAddr.SetPort(0x19);

	// the security settings are:	
	// for signalling channel, authenticate, authorise
	// (though see CIncomingConnectionListner:Preauthorise())
	TBTServiceSecurity sec;
	sec.SetAuthentication(EMitmDesired);
	sec.SetAuthorisation(ETrue);
	sec.SetEncryption(ETrue);
	sec.SetDenied(EFalse);

	sec.SetUid(KAvdtpUID);
	
	// as logical channels come in these settings are changed to stop
	// multiple authorises for the stream bearers

	localAddr.SetSecurity(sec);

// set the MTU into SAP here - for now we put the same on all logical channels - chosen with 5 slot packets in mind
//	TPckgBuf<TUint16> mtu;
//	mtu() = 339;

	iListener=CIncomingConnectionListener::NewL(*this, sap, localAddr, 3);										
	}
	
/**
AVDTP being a weird protocol has to to use the Logical Channel Factory for bearer listening
So just forward to it...

@param aSAP The SAP for the new lower layer connection
@internalComponent
*/
TInt CAvdtpProtocol::BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP)
	{
	LOG_FUNC
	return LogicalChannelFactory().BearerConnectComplete(aAddr, aSAP);
	}

/**
A stream has been created and is being made known to the protocol
@param aStream the stream being created
@panic if protocol already knows about stream
@internalComponent
*/
void CAvdtpProtocol::StreamCreated(CAVStream& aStream)
	{
	LOG_FUNC
#ifdef _DEBUG
	// check not already added
	TDblQueIter<CAVStream> iter(iStreams);
	while (iter)
		{
		__ASSERT_DEBUG(iter!=&aStream, Panic(EAvdtpStreamAlreadyExists));
		iter++;
		}
#endif	
	iStreams.AddFirst(aStream);
	}

/**
Find a stream based on it's remote address
@panic if the SEID in the address is Local
@param aRemoteAddr the remote address of the stream to find. The BTAddr and SEID are used
@internalComponent
*/	
CAVStream* CAvdtpProtocol::FindStream(const TAvdtpSockAddr& aRemoteAddr)
	{
	LOG_FUNC
	CAVStream* s = NULL;
	TDblQueIter<CAVStream> iter(iStreams);
	__ASSERT_DEBUG(!aRemoteAddr.SEID().IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));

	CAVStream* i;
	while (iter)
		{
		i = iter++;
		if (i->RemoteSEID()==aRemoteAddr.SEID() &&
				 i->DeviceAddress()==aRemoteAddr.BTAddr())
			{
			s = i;
			break;
			}
		}

	return s;
	}


/**
A stream going is going down, and wishes for the protocol to remove knowledge of it
@param aStream the stream going down
@panic debug panic if the stream wasn't known originally
@internalComponent
*/
void CAvdtpProtocol::RemoveStream(CAVStream& aStream)
	{
	LOG_FUNC
	CAVStream* s = NULL;
	TDblQueIter<CAVStream> iter(iStreams);
	
	while (iter)
		{
		s = iter++;
		if (s == &aStream)
			{
			s->iProtocolQLink.Deque();
			break;
			}
		}
	__ASSERT_DEBUG(s!=NULL, Panic(EAVDTPBadRemoveStream));
	}
	

/*
A secondary SAP has been created - we own it until a Primary SAP claims one
@internalComponent
*/	
void CAvdtpProtocol::AddSecondarySAP(CAvdtpSAP& aSecondarySAP)
	{
	LOG_FUNC
	iSecondarySAPs.AddFirst(aSecondarySAP);
	}

/*
Tidy up unclaimed secondary SAPs
@internalComponent
*/	
void CAvdtpProtocol::DestroySecondarySAPs()
	{
	LOG_FUNC
	while (!iSecondarySAPs.IsEmpty())
		{
		CAvdtpSAP* secondarySAP = iSecondarySAPs.First();
		iSecondarySAPs.Remove(*secondarySAP);
		delete secondarySAP;
		}
	}
	

/*
Primary SAP seeking (good-looking) Secondary SAP for protocol enjoyment
just give it the first one	
ownership will remain with the socket
@internalComponent
*/
CAvdtpSAP* CAvdtpProtocol::GetSecondarySAP()
	{
	LOG_FUNC
	CAvdtpSAP* secondarySAP = NULL;
	if (!iSecondarySAPs.IsEmpty())
		{
		secondarySAP = iSecondarySAPs.First();
		iSecondarySAPs.Remove(*secondarySAP);
		}
	__ASSERT_DEBUG(secondarySAP, Panic(EAvdtpProtocolAskedForSecondarySAPWhenNonExist));
	return secondarySAP;
	}
	

/**
Control plane message delivery system between protocols
@return error as a result of processing or not consuming the control message
@param aMessage the message
@param aParam arbitrary data for message - knowledge of aMessage allows casting
@see CBTProtocolFamily
@internalComponent
*/
TInt CAvdtpProtocol::ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam)
	{
	LOG_FUNC
	// only ones applicable to this protocol at present are 
	// for preauthorising a device - must have come from AVCTP, with PID=RCP
	TInt ret = KErrNotSupported;
	
	switch (aMessage)
		{
		case EPreauthoriseDevice:
			{
			__ASSERT_DEBUG(aParam, Panic(EAvdtpProtocolReceivingBadlyFormedControlMessage));
			const TOverrideAuthorise& override = *reinterpret_cast<const TOverrideAuthorise*>(aParam);
			__ASSERT_DEBUG(override.iAuthorisingProtocol == KAVCTP, Panic(EAvdtpProtocolReceivingControlFromUnexpectedProtocol));
			__ASSERT_DEBUG(override.iAuthorisingPort == SymbianAvctp::KAvrcpPid, Panic(EAvdtpProtocolReceivingControlFromUnexpectedPort)); //magic
			
			SetPreauthorisation(override.iPreauthorisedRemoteAddress,
								override.iPreauthorise);
			ret = KErrNone;							
			break;
			}
		default:
			__ASSERT_DEBUG(aParam, Panic(EAvdtpProtocolReceivingBadlyFormedControlMessage));
		}
	return ret;
	}


/**
Helper to hide the need to supply the socket level to which preauthorisation pertains
@param aPreauthorisedAddress the address of the device to preauthorise
@param aPreauthorise ETrue if the device is allowed to be authorised for other AVDTP/AVCTP connection, EFalse to cancel
@internalComponent
*/
TInt CAvdtpProtocol::SetPreauthorisation(const TBTDevAddr& aPreauthoriseAddress, TBool aSetPreauthorisation)
	{
	LOG_FUNC
	TInt ret = KErrNone;
	if (IsListening())
		{
		if (aSetPreauthorisation && !Listener().IsPreauthorised(KSolBtL2CAP, aPreauthoriseAddress))
			{
			Listener().SetPreauthorisation(KSolBtL2CAP, aPreauthoriseAddress, ETrue);
			
			TOverrideAuthorise override;
			override.iAuthorisingProtocol = KAVDTP;
			override.iPreauthorise = aSetPreauthorisation;
			override.iPreauthorisedRemoteAddress = aPreauthoriseAddress;
			
			ControlPlane().Preauthorise(KAVCTP, override);
			}
		else if (!aSetPreauthorisation)
			{
			Listener().SetPreauthorisation(KSolBtL2CAP, aPreauthoriseAddress, EFalse);			
			}
		// else do nothing
		}
	else
		{
		ret = KErrNotReady;
		}
		
	return ret;
	}
