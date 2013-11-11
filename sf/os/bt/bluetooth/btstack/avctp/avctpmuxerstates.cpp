// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implement the AVCTP Muxer state classes.
// These are CAvctpMuxerStateFactory, TAvctpMuxerState (abstract) and its derived
// classes 
// Together, these classes and the Muxer implement the State pattern
// (GOF).  The states themselves are implemented using the Flyweight
// pattern.  Each state is a Flyweight object, and CAvctpMuxerStateFactory
// is manager of these objects.  As a result of being a flyweight, no
// state object may have state that can't be shared between all
// possible users of the state (i.e. no per-Muxer state)
// 
//

/**
 @file
 @internalComponent
*/
#include <bluetooth/logger.h>

#include <es_prot.h>
#include <es_mbuf.h>

#include "avctpmuxerstates.h" 
#include "bt_subconn_levels.h"
#include "Avctp.h"
#include "avctpmuxer.h"
#include "avctputils.h"
#include "avctpconstants.h"
#include "avctpPacketMgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

using namespace SymbianAvctp;

//
//                                                                                       //
// TAvctpMuxerState Implementation                                                       //
//                                                                                       //
//

/** 
Default constructor.

@internalComponent
@param aFactory The Muxer state factory
*/
TAvctpMuxerState::TAvctpMuxerState(CAvctpMuxerStateFactory& aFactory)
	: iFactory(aFactory)
	{
	LOG_FUNC
	}

/** 
Change a Muxer from one state to another.

We Exit() the first state and then Enter() the second

@internalComponent
@param aTransport The Muxer whose state is to be changed
@param aNewState The index for the new state the Muxer is to transition to
*/
void TAvctpMuxerState::ChangeState(CAvctpTransport& aTransport, CAvctpMuxerStateFactory::TAvctpMuxerStates aNewState) const
	{
	LOG_FUNC

	aTransport.iState->Exit(aTransport);

#ifdef __FLOG_ACTIVE
	TAvctpMuxerState& state = iFactory.GetState(aNewState);
#endif
	LOG3(_L("Muxer 0x%08x : State %S -> %S"), &aTransport, &(aTransport.iState->iName), &(state.iName));
	
	aTransport.iState = &iFactory.GetState(aNewState);
	aTransport.iState->Enter(aTransport);
	}

/**
Calls the appropriate panic function to encode the panic
code with the current state identifier.
@param aPanic The panic code that the state is panicking with.
*/
void TAvctpMuxerState::PanicInState(SymbianAvctp::TPanic aPanic) const
	{
	Panic(aPanic, iFactory.StateIndex(this));
	}
	
/**
 Close the secondary l2cap channel with or without notifying clients.
 @param aTransport the transport the state is related to
 @param aNotifyClient if it's ETrue clients are notified.
 */
void TAvctpMuxerState::ShutDownSecondarySap(CAvctpTransport& aTransport, TBool aNotifyClient) const
	{
	LOG_FUNC
	
	CServProviderBase* sap = aTransport.iChannelSAPs[KAvctpSecondaryChannel];
	if (sap)
		{
		sap->Shutdown(CServProviderBase::EImmediate);
		delete sap;
		sap = NULL;
		aTransport.iChannelSAPs[KAvctpSecondaryChannel] = NULL;
		if (aNotifyClient)
			{
			aTransport.NotifyLinkDown(aTransport.iRemoteAddr, KAvctpSecondaryChannel);
			}
		ChangeState(aTransport, CAvctpMuxerStateFactory::EOpen);
		}
	}

/**
Default state entry function.

@internalComponent
@param aTransport The Muxer whose state is changing
*/
void TAvctpMuxerState::Enter(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	}

/**
Default state exit function.

@internalComponent
@param aTransport The Muxer whose state is changing
*/
void TAvctpMuxerState::Exit(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC	
	}


/**
Default Muxer IsIdle

The muxer is asking its state to determine whether it's idle.

@internalComponent
@param aTransport The Muxer which is idle
*/
TBool TAvctpMuxerState::IsIdle(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	TBool hasData = aTransport.HasDataToSend();
	TBool pidCount = aTransport.ClientCount(); 
	LOG1(_L("Is Idle: %d"), (!hasData && pidCount == 0));
	return (!hasData && pidCount == 0);
	}

/**
Default Muxer IdleTimerExpired

This is a confirmation that the muxer isn't wanted and so should go
to the closed state and die.

  @internalComponent
  @param aTransport The Muxer which is idle
*/
void TAvctpMuxerState::IdleTimerExpired(CAvctpTransport& aTransport) const
	{
	LOG_FUNC

	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);
	}

/**
  Default Muxer State Start (outgoing) function.

  We've been asked to start a connection to the specified address
  The default is to assume that nothing needs to be done.
  
  @internalComponent
  @param aTransport The Muxer which is to start up
  @param aAddr  The remote address to start up
  @return System wide error code
*/
TInt TAvctpMuxerState::Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 /*aClientId*/) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aAddr != TBTDevAddr(0) && aTransport.DevAddr() == aAddr, PanicInState(EWrongBTAddress));
	(void)aAddr;
	(void)aTransport;
	return KErrNone;
	}
	
/**
  Default Muxer State Start (incoming) function.

  We've been asked to start a connection using the given L2CAP sap
  However, this is only valid when we're in the Closed state so 
  panic
  
  @internalComponent
  @param aTransport The Muxer which is to start up
  @param aL2CAPConSAP The existing connection
  @return System wide error code
*/
TInt TAvctpMuxerState::StartIncoming(CAvctpTransport& /*aTransport*/, const TBTDevAddr& /*aAddr*/, CServProviderBase* /*aL2CAPConSAP*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	return KErrNone;
	}

TInt TAvctpMuxerState::AddSecondChannel(CAvctpTransport& /*aTransport*/, CServProviderBase& /*aSAP*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}

void TAvctpMuxerState::RemoveSecondChannel(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	}

TInt TAvctpMuxerState::SecondChannelRemoved(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}

/**
  Default Muxer State CancelConnectRequest function.

  This is called when a control client wants to cancel a connection to a particular PID.
  The connection will actually only be cancelled if there are no remaining control
  clients for this connection.
        
  @internalComponent
  @param aTransport The Muxer which is affected
*/		
TInt TAvctpMuxerState::CancelConnectRequest(CAvctpTransport& /*aTransport*/, TUint16 /*aClientId*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}


/**
Default Muxer Shutdown function.

  @internalComponent
  @param aTransport The Muxer which is to shutdown
*/
void TAvctpMuxerState::Shutdown(CAvctpTransport& /*aTransport*/, TInt /*aError*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	}

/**
Default Muxer Disconnect function.

  @internalComponent
  @param aTransport The Muxer whose L2CAP link has just gone down
*/
void TAvctpMuxerState::Disconnect(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	}

/**
Default Muxer ConnectComplete function.

  @internalComponent
  @param aTransport The Muxer whose L2CAP connection has just come up
*/
void TAvctpMuxerState::ConnectComplete(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	}

/**
Default Muxer Error function

  @internalComponent
  @param aTransport The Muxer which has just received an error from it's channel sap
  @param anError The error
  @param anOperationMask The effected operation
*/
void TAvctpMuxerState::Error(CAvctpTransport& /*aTransport*/, TInt /*aError*/,TUint /*aOperationMask*/, TInt /*aChannel*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	}

/**
Default Muxer new data function

  @internalComponent
  @param aTransport The Muxer which has received the notification of new data.
*/
TInt TAvctpMuxerState::NewData(CAvctpTransport& /*aTransport*/, TUint /*aMtu*/, TInt /*aChannel*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	return KErrNone;
	}

/**
Default Muxer can send function

  @internalComponent
  @param aTransport The Muxer which can now send
*/
void TAvctpMuxerState::CanSend(CAvctpTransport& /*aTransport*/,TInt /*aChannel*/) const
	{
	LOG_FUNC
	PanicInState(EUnexpectedMuxerEvent);
	}

//
//                                                                                       //
// Implementation of TAvctpStateClosed.                                                  //
//                                                                                       //
//

/**
Default constructor

  @internalComponent
  @param aFactory The Muxer state factory
*/
TAvctpStateClosed::TAvctpStateClosed(CAvctpMuxerStateFactory& aFactory)
	: TAvctpMuxerState(aFactory)

	{
	LOG_FUNC

	STATENAME("Closed");
	}
	
/**
Closed state entry function.

This is only called on re-entering the Closed state, so we're not needed so delete ourselves

@internalComponent
@param aTransport The Muxer whose state is changing
*/
void TAvctpStateClosed::Enter(CAvctpTransport& aTransport) const
	{
	LOG_FUNC

	delete &aTransport;
	}

/**
Closed state exit function.

@internalComponent
@param aTransport The Muxer whose state is changing
*/
void TAvctpStateClosed::Exit(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aTransport.DevAddr() != TBTDevAddr(0), PanicInState(ENullTBTDevAddr));
	}

/**
Closed Muxer IdleTimerExpired

The idle timer should only expire in a non closed state so panic
if we get it in this state.

  @internalComponent
  @param aTransport The Muxer which is idle
*/
void TAvctpStateClosed::IdleTimerExpired(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	
	PanicInState(EUnexpectedMuxerEvent);
	}

/**
  Closed Muxer State Start (outgoing) function.

  This function asks us to start a connection to the specified address
  
  @internalComponent
  @param aTransport The Muxer which is to start up
  @param aAddr  The remote address to start up
  @return System wide error code
*/
TInt TAvctpStateClosed::Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 /*aClientId*/) const
	{
	LOG_FUNC

	__ASSERT_DEBUG(aTransport.DevAddr() == TBTDevAddr(0), PanicInState(ETBTDevAddrNotNull));

	TInt ret = KErrNone;
	if (aAddr != TBTDevAddr(0))
		{
		// New this up just in time.
		TRAP(ret, aTransport.iChannelSAPs[KAvctpPrimaryChannel] = aTransport.iProtocol.LowerProtocol()->NewSAPL(KSockSeqPacket));
		if (ret == KErrNone)
			{
			aTransport.AssignToDevice(aAddr);			
			ChangeState(aTransport, CAvctpMuxerStateFactory::ELinkPending);		
			}
		}
	else
		{
		// do nothing - stay closed
		ret = KErrNone;
		}

	if (ret != KErrNone)
		{
		delete &aTransport;
		}
	return ret;
	}
	
/**
  Closed Muxer State Start (incoming) function.

  We've been asked to start a connection using the given L2CAP sap
  
  @internalComponent
  @param aTransport The Muxer which is to start up
  @param aL2CAPConSAP The existing connection
  @return System wide error code
*/
TInt TAvctpStateClosed::StartIncoming(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, CServProviderBase* aL2CAPConSAP) const
	{
	LOG_FUNC

	TInt ret = SymbianAvctp::KErrBadAddress;
	
	// If the following is not true we'll Kern-Exec 3 anyway so assert always
	__ASSERT_ALWAYS(aL2CAPConSAP, PanicInState(ENullLowerProtocolSap));
	__ASSERT_DEBUG(aTransport.DevAddr() == TBTDevAddr(0), PanicInState(ETBTDevAddrNotNull));

	if (aAddr != TBTDevAddr(0))
		{
		aTransport.AssignToDevice(aAddr);		
		aTransport.iChannelSAPs[KAvctpPrimaryChannel] = aL2CAPConSAP;
		aTransport.iChannelSAPs[KAvctpPrimaryChannel]->SetNotify(&aTransport);
		
		aTransport.iChannelSAPs[KAvctpPrimaryChannel]->Start();
		ChangeState(aTransport, CAvctpMuxerStateFactory::EOpen);		
		ret = KErrNone;
		}
	// else covered by ret

	if (ret != KErrNone)
		{
		delete &aTransport;
		}
	return ret;
	}
	
//
//                                                                                       //
// Implementation of void TAvctpStateLinkPending                                         //
//                                                                                       //
// Represents a muxer waiting for the L2CAP link to be created.                          //
//                                                                                       //
//

/**
Default constructor

  @internalComponent
  @param aFactory The Muxer state factory
*/
TAvctpStateLinkPending::TAvctpStateLinkPending(CAvctpMuxerStateFactory& aFactory)
	: TAvctpMuxerState(aFactory)

	{
	LOG_FUNC
	STATENAME("LinkPending");
	}

/**
Link Pending state entry function.

We request the lower protocol SAP to bring up the link.

@internalComponent
@param aTransport The Muxer which is waiting for a link
*/
void TAvctpStateLinkPending::Enter(CAvctpTransport& aTransport) const
	{
	LOG_FUNC

	__ASSERT_ALWAYS(aTransport.iChannelSAPs[KAvctpPrimaryChannel], PanicInState(ENullLowerProtocolSap));
	
	aTransport.DequeIdleTimer();
	// we become the socket for iChannelsSAPs[0]
	aTransport.iChannelSAPs[KAvctpPrimaryChannel]->SetNotify(&aTransport);
	
	TL2CAPSockAddr addr;
	addr.SetBTAddr(aTransport.DevAddr());
	addr.SetPort(KAVCTP);

	// the security settings are:	
	// (though see :Preauthorise() for the authentication exceptions due to avdtp authentication)
	TBTServiceSecurity sec;
	sec.SetAuthentication(KOutboundAuthenticationDefault);
	sec.SetAuthorisation(KOutboundAuthoristationDefault);
	sec.SetEncryption(KOutboundEncryptionDefault);
	sec.SetDenied(EFalse);
	sec.SetUid(KAvctpServiceUid);	
	addr.SetSecurity(sec);
	
	TInt err = aTransport.iChannelSAPs[KAvctpPrimaryChannel]->SetRemName(addr);
	__ASSERT_DEBUG(!err, PanicInState(EErrorSettingAddress));
	
	// Bring up the L2CAP link
	aTransport.iChannelSAPs[KAvctpPrimaryChannel]->ActiveOpen();	
	}

// TAvctpStateLinkPending::Exit - we'd lke to call aTransport.CheckForIdle(0) but cause
// in this state IsIdle always returns EFalse that's not possible. Hence do the
// CheckForIdle in the Enter of the next state currently Open or Closed.




/**
Link Pending Muxer IsIdle

The muxer is asking it's state to determine whether it's idle. Cause the link is pending
we can't be idle whatever happens. 

Note that this means we must call CheckForIdle on aTransport on entering the next state.

  @internalComponent
  @param aTransport The Muxer which is idle
*/
TBool TAvctpStateLinkPending::IsIdle(CAvctpTransport& /*aTransport*/) const
	{
	LOG_FUNC
	return EFalse;
	}
	
/**
  Link Pending Muxer State CancelConnnectRequest function.

  This is called when a control client wants to cancel a connection to a particular PID.
  The connection will actually only be cancelled if there are no remaining control
  clients for this connection.
  
  This function just removes the saplinksmgr on aPid from the list of control
  clients since it no longer is interested in this remote device and shouldn't 
  receive any more events related to it.
    
  @internalComponent
  @param aTransport The Muxer which is affected
  @param aPid The pid to cancel the connection too
*/		
TInt TAvctpStateLinkPending::CancelConnectRequest(CAvctpTransport& /*aTransport*/, TUint16 /*aClientId*/) const
	{
	LOG_FUNC
	return KErrNotSupported;
	}


/**
Link Pending Muxer ConnectComplete function.

This is the successful result of a connection attempt to a remote device. 
(A incoming connection wouldn't have resulted in a BearerConnectComplete on the
protocol instead)

  @internalComponent
  @param aTransport The Muxer whose L2CAP connection has just come up
*/
void TAvctpStateLinkPending::ConnectComplete(CAvctpTransport& aTransport) const
	{
	LOG_FUNC

	ChangeState(aTransport, CAvctpMuxerStateFactory::EOpen);
	aTransport.NotifyLinkUp(aTransport.iRemoteAddr);
	}

/**
Link Pending Muxer Error function

This is called after some kind of error in the lower protocol Sap.

  @internalComponent
  @param aTransport The Muxer which has just received an error from it's iChannelsSAPs[0]
  @param anError The error
  @param anOperationMask The effected operation
*/
void TAvctpStateLinkPending::Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const
	{
	LOG_FUNC

	__ASSERT_DEBUG(aChannel == KAvctpPrimaryChannel, PanicInState(EAvctpInvalidChannel));
	
	if (aOperationMask & (MSocketNotify::EErrorFatal   | 
	  				      MSocketNotify::EErrorConnect | 
	  				      MSocketNotify::EErrorClose   | 
					 	  MSocketNotify::EErrorAllOperations))
		{
		// The connect failed, so we'd better go to the Closed state
		// and tell clients what's happened.
		// Notifications are made before changing the state because the Enter() method of the Close state
		// delete the transport, so we can't call anything after changing the state.

		aTransport.NotifyLinkError(aError, aChannel == KAvctpSecondaryChannel);
		ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);
		}
	else if (aOperationMask & (MSocketNotify::EErrorSend  | 
	  	 	 		       	   MSocketNotify::EErrorRecv  |
	  	 	 		       	   MSocketNotify::EErrorIoctl))
		{
		// Since the connection isn't up, we shouldn't be getting a send or recv error
		// nor do we currently support ioctls in the muxer so:
		PanicInState(EUnexpectedMuxerEvent);
		}		
	}

//
//                                                                                       //
// Implementation of state TAvctpStateOpen                                               //
//                                                                                       //
// This represents a fully connected Muxer that can send and receive data                //
//                                                                                       //
//

/**
Default constructor

  @internalComponent
  @param aFactory The Muxer state factory
*/
TAvctpStateOpen::TAvctpStateOpen(CAvctpMuxerStateFactory& aFactory)
	: TAvctpMuxerState(aFactory)

	{
	LOG_FUNC
	STATENAME("Open");
	}
	
/**
Open state entry function.

In case a Avctp Sap asked to send data we need to signal them that they can now send

@internalComponent
@param aTransport The Muxer whose state is changing
*/
void TAvctpStateOpen::Enter(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	
	aTransport.SetClearToSend(KAvctpPrimaryChannel);	//only first channel is open so far
	aTransport.PacketMgr().CanSend(KAvctpPrimaryChannel);
	
	// for stereo headset usecase, pre-authorise device for AVDTP 
	aTransport.iProtocol.SetPreauthorisation(aTransport.iRemoteAddr, ETrue);
	}

/**
Open state exit function.

@internalComponent
@param aTransport The Muxer whose state is changing
*/
void TAvctpStateOpen::Exit(CAvctpTransport& aTransport) const
	{
	LOG_FUNC

	// for stereo headset usecase, de-pre-authorise device for AVDTP 
	aTransport.iProtocol.SetPreauthorisation(aTransport.iRemoteAddr, EFalse);
	}


/**
  Open Muxer State Start (outgoing) function.

  We've been asked to start a connection to the specified address
  The connection is already up so immediately send a ConnectCfm event
  
  @internalComponent
  @param aTransport The Muxer which is to start up
  @param aAddr  The remote address to start up
  @return System wide error code
*/
TInt TAvctpStateOpen::Start(CAvctpTransport& aTransport, const TBTDevAddr& __DEBUG_ONLY(aAddr), TUint16 aClientId) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aAddr != TBTDevAddr(0) && aTransport.DevAddr() == aAddr, PanicInState(EWrongBTAddress));
 
	aTransport.NotifyAttachConfirm(aClientId, KErrNone, EFalse);
	return KErrNone;
	}
	
	
/**
Open Muxer Shutdown function.

This function allows the muxer to be shutdown irrespective of whether 
it is idle. This is intended to be used to punish a remote device that
has done something naughty.

Notify all clients of the problem and go to the closed state.

  @internalComponent
  @param aTransport The Muxer which is sending data
*/
void TAvctpStateOpen::Shutdown(CAvctpTransport& aTransport, TInt /*aError*/) const
	{
	LOG_FUNC

	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);
	}


/**
Open Muxer Disconnect function.

This is the result of a remote device disconnecting from us.
Let all interested parties know.

  @internalComponent
  @param aTransport The Muxer whose L2CAP link has just gone down
*/
void TAvctpStateOpen::Disconnect(CAvctpTransport& aTransport) const
	{
	LOG_FUNC

	aTransport.iProtocol.PrimaryChannelIncomingRemoteDisconnection(aTransport.iRemoteAddr); 
	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);		
	}

/**
Open Muxer Error function

This is called after some kind of error in the lower protocol Sap.

  @internalComponent
  @param aTransport The Muxer which has just received an error from it's channel sap
  @param anError The error
  @param anOperationMask The affected operation
*/	
void TAvctpStateOpen::Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const
	{	
	LOG_FUNC

	__ASSERT_DEBUG(aChannel == KAvctpPrimaryChannel, PanicInState(EAvctpInvalidChannel));
	
	if (aOperationMask & (MSocketNotify::EErrorRecv |
						  MSocketNotify::EErrorSend))
		{
		// the packet mgr is only interested in data plane errors
		aTransport.PacketMgr().SignalMuxerError(aError, aOperationMask);
		}	

	if (aOperationMask & (MSocketNotify::EErrorFatal   | 
	  				      MSocketNotify::EErrorClose   | 
					 	  MSocketNotify::EErrorAllOperations))
		{
		// The connection has gone down so we inform our control clients of this fact	
		aTransport.NotifyLinkDown(aTransport.iAddress, aChannel, aError);
		}
	else if (aOperationMask & (MSocketNotify::EErrorConnect |
							   MSocketNotify::EErrorIoctl))
		{
		// Since the connection is already up, we shouldn't be getting a connect error
		// nor do we currently support ioctls
		PanicInState(EUnexpectedMuxerEvent);
		}
	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);
	}

TInt TAvctpStateOpen::AddSecondChannel(CAvctpTransport& aTransport, CServProviderBase& aSAP) const
	{
	LOG_FUNC
	// can bind socket to SAP now
	aTransport.BindSecondaryChannelSap(aSAP);
	
	// see if it is connected or not
	TL2CAPSockAddr addr;
	aSAP.RemName(addr);
	
	if (addr.BTAddr()==TBTDevAddr(0))
		{
		addr.SetBTAddr(aTransport.iRemoteAddr);
		addr.SetPort(KAvctpSecondChannelPSM);

		// the security settings are:	
		// (though see :Preauthorise() for the authentication exceptions due to avdtp authentication)
		TBTServiceSecurity sec;
		sec.SetAuthentication(KSecondaryChannelAuthenticationDefault);
		sec.SetAuthorisation(KSecondaryChannelAuthoristationDefault);
		sec.SetEncryption(KOutboundEncryptionDefault);
		sec.SetDenied(EFalse);
		sec.SetUid(KAvctpServiceUid);	
		addr.SetSecurity(sec);
		
		TInt err = aTransport.iChannelSAPs[KAvctpSecondaryChannel]->SetRemName(addr);
		__ASSERT_DEBUG(err == KErrNone, PanicInState(ESetRemNameError));

		const TUint KDefaultMtu = 335;
		
		TPckgBuf<TL2CapConfig> config;
		config().SetMaxTransmitUnitSize(KAvctpSecondaryChannelInboundMTU);
		config().SetMinMTU(KDefaultMtu);
		config().SetMaxReceiveUnitSize(KAvctpSecondaryChannelInboundMTU);
		config().SetMinMRU(KDefaultMtu);
		config().ConfigureReliableChannel(TL2CapConfig::EDefaultRetransmission);
		config().SetLegacyModesDisallowed(ETrue);

		err = aTransport.iChannelSAPs[KAvctpSecondaryChannel]->SetOption(KSolBtL2CAP, KL2CAPUpdateChannelConfig, config); 
		__ASSERT_DEBUG(err == KErrNone, Panic(ESetOptionError));

		aTransport.iChannelSAPs[KAvctpSecondaryChannel]->ActiveOpen();
		ChangeState(aTransport, CAvctpMuxerStateFactory::ESecondLinkPending);
		}
	else
		{
		aTransport.iChannelSAPs[KAvctpSecondaryChannel]->Start();
		aTransport.SetClearToSend(KAvctpSecondaryChannel);
		ChangeState(aTransport, CAvctpMuxerStateFactory::EFullyOpen);
		aTransport.NotifyLinkUp(addr.BTAddr(), ETrue);
		}
	return KErrNone;
	}

/**
Open Muxer new data function

The lower protocol sap has notified the muxer of new data that is 
waiting for retrieval.

This is called once for each new packet of data that L2CAP has notified the muxer of.
We get to the data by calling GetData on the lower protocol SAP 
and give the resulting data to the Avctp Protocol, letting it workout who wants it.

This assumes a packet interface from L2CAP.

  @internalComponent
  @param aTransport The Muxer which has received the notification of new data.
  @param aMtu The length of the data packet to be read. 
*/
TInt TAvctpStateOpen::NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aChannel==KAvctpPrimaryChannel, PanicInState(EUnexpectedMuxerEvent));
	__ASSERT_DEBUG(aTransport.iChannelSAPs[aChannel], PanicInState(ENullLowerProtocolSap));
	
	TInt err = KErrNone;
	// Read data into the buffer
	// the transport feeds us synchronously for first channel
	RMBufChain inboundFragment;
	TInt dataAvailable = aTransport.iChannelSAPs[KAvctpPrimaryChannel]->GetData(inboundFragment, aMtu, 0);
	if (dataAvailable > 0)
		{
		err = aTransport.iPacketMgr->NewData(inboundFragment, aChannel); // ownership xferred;
		
		//NewData() only returns an error in the case where the data header gave a parse error.
		//In this case the muxer is shutdown, so the remaining packets cannot be read.
		//In the case where NewData() failed to pass on onwnership, an attempt should be made
		//to read remaining packets, so no error is returned.
		if(err != KErrNone)
			{
			inboundFragment.Free();
			}
		}
	else if (dataAvailable < KErrNone)	// some error occurred reading the data. shutdown the muxer and report the error
		{
		aTransport.Shutdown(dataAvailable);
		inboundFragment.Free();
		err = KErrMuxerShutDown;
		}
	// otherwise, no data available
	return err;
	}
		
/**
Open Muxer can send function

This is called after a blocked Write on the lower protocol SAP. This is a notification
that the lower protocol SAP is ready to receive more data.

  @internalComponent
  @param aTransport The Muxer which can now send
*/
void TAvctpStateOpen::CanSend(CAvctpTransport& aTransport, TInt aChannel) const
	{
	LOG_FUNC

	aTransport.SetClearToSend(aChannel);	
	aTransport.PacketMgr().CanSend(aChannel);
	}

TAvctpStateFullyOpen::TAvctpStateFullyOpen(CAvctpMuxerStateFactory& aFactory)
:TAvctpStateOpen(aFactory)
	{
	LOG_FUNC
	STATENAME("FullyOpen");
	}

void TAvctpStateFullyOpen::Enter(CAvctpTransport& aTransport) const
	{
	// should check the sap psm really
	aTransport.SetClearToSend(KAvctpSecondaryChannel);
	aTransport.PacketMgr().CanSend(KAvctpSecondaryChannel);
	}

/**
 The method is defined and left empty on purpose (because this class derives from TAvctpStateOpen 
 but we don't wont its method to be called.
 */
void TAvctpStateFullyOpen::Exit(CAvctpTransport& /*aTransport*/) const
	{
	}

void TAvctpStateFullyOpen::Error(CAvctpTransport& aTransport, TInt aError, TUint aErrorMask, TInt aChannel) const
	{
	LOG_FUNC

    if (aErrorMask & (MSocketNotify::EErrorRecv |
                          MSocketNotify::EErrorSend))
        {
        // the packet mgr is only interested in data plane errors
        aTransport.PacketMgr().SignalMuxerError(aError, aErrorMask);
        }   

    if (aErrorMask & (MSocketNotify::EErrorFatal   | 
                          MSocketNotify::EErrorClose   | 
                          MSocketNotify::EErrorAllOperations))
        {
        // The connection has gone down so we inform our control clients of this fact   
        aTransport.NotifyLinkDown(aTransport.iAddress, aChannel, aError);
        }
    else if (aErrorMask & (MSocketNotify::EErrorConnect |
                               MSocketNotify::EErrorIoctl))
        {
        // Since the connection is already up, we shouldn't be getting a connect error
        // nor do we currently support ioctls
        PanicInState(EUnexpectedMuxerEvent);
        }
    ChangeState(aTransport, aChannel == KAvctpSecondaryChannel ? CAvctpMuxerStateFactory::EOpen : CAvctpMuxerStateFactory::EClosed);
	}

void TAvctpStateFullyOpen::Disconnect(CAvctpTransport& aTransport) const
	{
	aTransport.iProtocol.PrimaryChannelIncomingRemoteDisconnection(aTransport.iRemoteAddr); 
	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);		
	}

void TAvctpStateFullyOpen::CanSend(CAvctpTransport& aTransport,TInt aChannel) const
	{
	aTransport.SetClearToSend(aChannel);	
	aTransport.PacketMgr().CanSend(aChannel);
	}

TInt TAvctpStateFullyOpen::NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const
	{
	RMBufChain inboundFragment;
	__ASSERT_DEBUG(aTransport.iChannelSAPs[aChannel], PanicInState(ENullLowerProtocolSap));
	aTransport.iChannelSAPs[aChannel]->GetData(inboundFragment, aMtu, 0);
	
	TInt err = aTransport.iPacketMgr->NewData(inboundFragment, aChannel); // ownership xferred;
	
	if(err != KErrNone)
		{
		inboundFragment.Free();
		}
	return err;
	}

TInt TAvctpStateFullyOpen::Start(CAvctpTransport& aTransport, const TBTDevAddr& /*aAddr*/, TUint16 aClientId) const
	{
	aTransport.NotifyAttachConfirm(aClientId, KErrNone, EFalse);
	return KErrNone;
	}

void TAvctpStateFullyOpen::RemoveSecondChannel(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	// shutdown secondary channel without notifying clients (second param is EFalse)
	ShutDownSecondarySap(aTransport, EFalse);
	}

TInt TAvctpStateFullyOpen::SecondChannelRemoved(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	// shutdown secondary channel notifying clients (second param is ETrue)
	ShutDownSecondarySap(aTransport, ETrue);
	return KErrNone;
	}

void TAvctpStateFullyOpen::Shutdown(CAvctpTransport& aTransport, TInt /*aError*/) const
	{
	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);
	}


/**
 The method is declared and left empty on purpose (because this class derives from TAvctpStateOpen 
 but we don't wont its method to be called.
 */
void TAvctpStateSecondChannelPending::Enter(CAvctpTransport& /*aTransport*/) const
	{
	}

/**
 The method is declared and left empty on purpose (because this class derives from TAvctpStateOpen 
 but we don't wont its method to be called.
 */
void TAvctpStateSecondChannelPending::Exit(CAvctpTransport& /*aTransport*/) const
	{
	}

void TAvctpStateSecondChannelPending::Error(CAvctpTransport& aTransport, TInt aError, TUint /*aErrorMask*/, TInt aChannel) const
	{
	aTransport.NotifyLinkError(aError, aChannel == KAvctpSecondaryChannel);
	ChangeState(aTransport, CAvctpMuxerStateFactory::EOpen);
	}

TAvctpStateSecondChannelPending::TAvctpStateSecondChannelPending(CAvctpMuxerStateFactory& aFactory)
:TAvctpStateOpen(aFactory)
	{
	LOG_FUNC
	STATENAME("SecondLinkPending");
	}

void TAvctpStateSecondChannelPending::ConnectComplete(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	aTransport.NotifyLinkUp(aTransport.iRemoteAddr, ETrue);
	ChangeState(aTransport, CAvctpMuxerStateFactory::EFullyOpen);
	}

void TAvctpStateSecondChannelPending::Disconnect(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	
	// In this state there is at least one outstanding ioctl on the secondary ctrl sap that 
	// we must complete
	
	aTransport.NotifyAttachConfirm(KErrMuxerShutDown, ETrue);
	aTransport.iProtocol.PrimaryChannelIncomingRemoteDisconnection(aTransport.iRemoteAddr); 
	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);		
	}

void TAvctpStateSecondChannelPending::CanSend(CAvctpTransport& aTransport, TInt __DEBUG_ONLY(aChannel)) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aChannel == KAvctpPrimaryChannel, PanicInState(EAvctpInvalidChannel));
	aTransport.SetClearToSend(KAvctpPrimaryChannel);	
	aTransport.PacketMgr().CanSend(KAvctpPrimaryChannel);
	}

TInt TAvctpStateSecondChannelPending::NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aChannel==KAvctpPrimaryChannel, PanicInState(EUnexpectedMuxerEvent));
	__ASSERT_DEBUG(aTransport.iChannelSAPs[aChannel], PanicInState(ENullLowerProtocolSap));
	
	// Read data into the buffer
	// the transport feeds us synchronously for first channel
	RMBufChain inboundFragment;
	aTransport.iChannelSAPs[KAvctpPrimaryChannel]->GetData(inboundFragment, aMtu, 0);

	TInt err = aTransport.iPacketMgr->NewData(inboundFragment, aChannel); // ownership xferred;
	
	//NewData() only returns an error in the case where the data header gave a parse error.
	//In this case the muxer is shutdown, so the remaining packets cannot be read.
	//In the case where NewData() failed to pass on onwnership, an attempt should be made
	//to read remaining packets, so no error is returned.
	if(err != KErrNone)
		{
		inboundFragment.Free();
		}
	return err;
	}

TInt TAvctpStateSecondChannelPending::Start(CAvctpTransport& /*aTransport*/, const TBTDevAddr& /*aAddr*/, TUint16 /*aClientId*/) const
	{
	return KErrNotReady;
	}

void TAvctpStateSecondChannelPending::Shutdown(CAvctpTransport& aTransport, TInt /*aError*/) const
	{
	LOG_FUNC
	ChangeState(aTransport, CAvctpMuxerStateFactory::EClosed);
	}

void TAvctpStateSecondChannelPending::RemoveSecondChannel(CAvctpTransport& aTransport) const
	{
	LOG_FUNC
	// shutdown secondary channel without notifying clients (second param is EFalse)
	ShutDownSecondarySap(aTransport, EFalse);
	}
//
//                                                                                       //
// CAvctpMuxerStateFactory implementation                                                     //
//                                                                                       //
//

/**
Default constructor

  @internalComponent
*/
CAvctpMuxerStateFactory::CAvctpMuxerStateFactory()
	{
	LOG_FUNC
	}

/**
Destructor to free resources.

Delete all our lightweight states

  @internalComponent
*/
CAvctpMuxerStateFactory::~CAvctpMuxerStateFactory()
	{
	LOG_FUNC

	iStates.DeleteAll();
	}

/**
Static factory factory function (think about it... ;-)

Creates an array of lightweight states for Muxers

  @internalComponent
  @leave KErrNoMemory if the CAvctpMuxerStateFactory object could not be created
  @return A pointer to the new state factory
*/
CAvctpMuxerStateFactory* CAvctpMuxerStateFactory::NewL()
	{
	LOG_STATIC_FUNC

	CAvctpMuxerStateFactory* factory = new(ELeave) CAvctpMuxerStateFactory();
	CleanupStack::PushL(factory);

	// Create all the new states
	factory->iStates[EClosed]	  	 = new(ELeave) TAvctpStateClosed(*factory);
	factory->iStates[ELinkPending]	 = new(ELeave) TAvctpStateLinkPending(*factory);
	factory->iStates[EOpen]		  	 = new(ELeave) TAvctpStateOpen(*factory);
	factory->iStates[ESecondLinkPending] = new(ELeave) TAvctpStateSecondChannelPending(*factory);
	factory->iStates[EFullyOpen] = new(ELeave) TAvctpStateFullyOpen(*factory);
	
	CleanupStack::Pop(factory);
	return factory;
	}

/**
Utility function to get a lightweight state

  @internalComponent
  @param aState Index to the state required
  @return A lightweight Muxer state
*/
TAvctpMuxerState& CAvctpMuxerStateFactory::GetState(const TAvctpMuxerStates aState) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aState != EMaxStates, Panic(EAvctpMuxerStateOutOfBounds));
	return *iStates[aState];
	}

TInt CAvctpMuxerStateFactory::StateIndex(const TAvctpMuxerState* aState) const
	{
	TInt state;
	for (state = 0; state < EMaxStates; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}
