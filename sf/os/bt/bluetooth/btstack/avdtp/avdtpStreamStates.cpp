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
// Implements the avdtp stream statemachine
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpStream.h"
#include "avdtp.h"
#include "avdtpSignallingChannel.h"
#include "avdtpSignallingSession.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

#ifdef _DEBUG
#define DEBUGPANICINSTATE(aPanic) PanicInState(aPanic)
#else
#define DEBUGPANICINSTATE(aPanic)
#endif

// state ctors

TAVStreamState::TAVStreamState(CAVStreamStateFactory& aFactory)
: iFactory(aFactory)
	{
	STATENAME("BASE");
	}

TAVStreamStateIdle::TAVStreamStateIdle(CAVStreamStateFactory& aFactory)
: TAVStreamState(aFactory)
	{
	STATENAME("Idle");
	}
	
TAVStreamStateOpen::TAVStreamStateOpen(CAVStreamStateFactory& aFactory)
: TAVStreamStateIdle(aFactory)
	{
	STATENAME("Open");
	}

TAVStreamStateReady::TAVStreamStateReady(CAVStreamStateFactory& aFactory)
: TAVStreamStateOpen(aFactory)
	{
	STATENAME("Ready");
	}

TAVStreamStateWaitForSessions::TAVStreamStateWaitForSessions(CAVStreamStateFactory& aFactory)
: TAVStreamStateIdle(aFactory)
	{
	STATENAME("WaitForSessions");
	}

TAVStreamStateWaitForSessionsStartReceived::TAVStreamStateWaitForSessionsStartReceived(CAVStreamStateFactory& aFactory)
: TAVStreamStateWaitForSessions(aFactory)
	{
	STATENAME("WaitForSessionsStartReceived");
	}

TAVStreamStateConfiguring::TAVStreamStateConfiguring(CAVStreamStateFactory& aFactory)
: TAVStreamStateIdle(aFactory)
	{
	STATENAME("Configuring");
	}

TAVStreamStateConfigured::TAVStreamStateConfigured(CAVStreamStateFactory& aFactory)
: TAVStreamStateIdle(aFactory)
	{
	STATENAME("**Abstract** Configure");
	}
	
TAVStreamStateINTConfigured::TAVStreamStateINTConfigured(CAVStreamStateFactory& aFactory)
: TAVStreamStateConfigured(aFactory)
	{
	STATENAME("INTConfigured");
	}
	
TAVStreamStateACPConfigured::TAVStreamStateACPConfigured(CAVStreamStateFactory& aFactory)
: TAVStreamStateConfigured(aFactory)
	{
	STATENAME("ACPConfigured");
	}	

TAVStreamStateOpening::TAVStreamStateOpening(CAVStreamStateFactory& aFactory)
: TAVStreamStateIdle(aFactory)
	{
	STATENAME("Opening");
	}

TAVStreamStateCreatingLogicalChannels::TAVStreamStateCreatingLogicalChannels(CAVStreamStateFactory& aFactory)
: TAVStreamStateOpen(aFactory)
	{
	STATENAME("CreatingLogicalChannels");
	}

TAVStreamStateReleasing::TAVStreamStateReleasing(CAVStreamStateFactory& aFactory)
: TAVStreamStateReady(aFactory)
	{
	STATENAME("Releasing");
	}

TAVStreamStateAborting::TAVStreamStateAborting(CAVStreamStateFactory& aFactory)
: TAVStreamStateReady(aFactory)
	{
	STATENAME("Aborting");
	}

TAVStreamStateStreaming::TAVStreamStateStreaming(CAVStreamStateFactory& aFactory)
: TAVStreamStateReady(aFactory)
	{
	STATENAME("Streaming");
	}

TAVStreamStateStarting::TAVStreamStateStarting(CAVStreamStateFactory& aFactory)
: TAVStreamStateReady(aFactory)
	{
	STATENAME("Starting");
	}

TAVStreamStateSuspending::TAVStreamStateSuspending(CAVStreamStateFactory& aFactory)
: TAVStreamStateReady(aFactory)
	{
	STATENAME("Suspending");
	}

TAVStreamStateWaitingForLogicalChannels::TAVStreamStateWaitingForLogicalChannels(CAVStreamStateFactory& aFactory)
: TAVStreamStateOpen(aFactory)
	{
	STATENAME("WaitingForLogicalChannels");
	}

TAVStreamStateReconfiguring::TAVStreamStateReconfiguring(CAVStreamStateFactory& aFactory)
: TAVStreamStateReady(aFactory)
	{
	STATENAME("Reconfiguring");
	}


// default state actions

void TAVStreamState::Enter(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// do nothing by default
	}
	
void TAVStreamState::Exit(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// do nothing by default
	}
	
void TAVStreamState::Release(CAVStream& aStream) const
	{
	LOG_FUNC
	// it already has been Released
	aStream.Released();
	}

void TAVStreamState::AwaitLogicalChannelsL(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedAwaitLogicalChannelsEvent);
	}

void TAVStreamState::LogicalChannelsAvailable(CAVStream& /*aStream*/, TLogicalChannelFactoryTicket /*aTicket*/, TInt /*aError*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedLogicalChannelsAvailableEvent);
	}

void TAVStreamState::WatchdogFired(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedWatchdogFiredEvent);
	}

void TAVStreamState::Configured(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedConfiguredEvent);
	}

void TAVStreamState::SetConfigurationL(CAVStream& /*aStream*/, 
										RBuf8& /*aPacketBuffer*/, CSignallingChannel& /*aSignallingChannel*/,
										TBool /*aReportingConfigured*/,
								  		TBool /*aRecoveryConfigured*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedSetConfigurationEvent);
	User::Leave(KErrNotReady);
	}

void TAVStreamState::SetConfigConfirm(CAVStream& /*aStream*/, TInt /*aResult*/, 
									TSEID /*aRemoteSEID*/, TAvdtpServiceCategory /*aFailedCategory*/) const
	{
	// throw away
	}

void TAVStreamState::ReconfigConfirm(CAVStream& /*aStream*/, TInt /*aResult*/, 
									TSEID /*aRemoteSEID*/, TAvdtpServiceCategory /*aFailedCategory*/) const
	{
	LOG_FUNC
	// throw away
	}
		
void TAVStreamState::StartConfirm(CAVStream& /*aStream*/, TInt /*aResult*/, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	// throw away
	}
		
void TAVStreamState::AbortConfirm(CAVStream& /*aStream*/, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	// throw away
	}
	
void TAVStreamState::OpenConfirm(CAVStream& /*aStream*/, TInt /*aResult*/, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	// default, throw away weirdo response from remote
	}

void TAVStreamState::ReleaseConfirm(CAVStream& /*aStream*/, TInt /*aResult*/, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	// throw away
	}

void TAVStreamState::SuspendConfirm(CAVStream& /*aStream*/, TInt /*aResult*/, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	// throw away
	}	
	
TInt TAVStreamState::AddSession(CAVStream& /*aStream*/, 
							TAvdtpTransportSessionType /*aType*/,
							CUserPlaneTransportSession& /*aSession*/,
							CTransportChannel*& /*aTransportChannel*/,
							TL2CapConfig::TChannelPriority /*aPriority*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedAddSessionEvent);
	return KErrNotReady;
	}
								
void TAVStreamState::DropSession(CAVStream& /*aStream*/, TAvdtpTransportSessionType /*aType*/,
							 CUserPlaneTransportSession& /*aSession*/) const
	{
	LOG_FUNC
	DEBUGPANICINSTATE(EAvdtpUnexpectedDropSession);
	}

TInt TAVStreamState::StartReceived(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}

TBool TAVStreamState::IsStreaming(const CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return EFalse;
	}

TInt TAVStreamState::Start(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}
	
TInt TAVStreamState::Suspend(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}

void TAVStreamState::Started(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// signalling session should have sanity checked with previous StartReceived()
	DEBUGPANICINSTATE(EAvdtpUnexpectedStartedEvent);
	}
	
void TAVStreamState::Suspended(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// signalling session should have sanity checked with previous IsStreaming
	DEBUGPANICINSTATE(EAvdtpUnexpectedSuspendedEvent);
	}

void TAVStreamState::ChangeState(CAVStream& aContext, CAVStreamStateFactory::TAVStreamStates aState) const
	{
	LOG_FUNC
	aContext.iState->Exit(aContext);

#ifdef __FLOG_ACTIVE
	const TAVStreamState* const newState=&iFactory.GetState(aState);
	LOG2(_L("Stream: State %S -> %S"), &aContext.iState->iName, &newState->iName);
#endif //__FLOG_ACTIVE

	aContext.iState=&iFactory.GetState(aState);
	aContext.iState->Enter(aContext);
	}

void TAVStreamState::PanicInState(TAvdtpPanic aPanic) const
	{
	Panic(aPanic, iFactory.StateIndex(this));
	}


// derived states and state changes

void TAVStreamStateIdle::Configured(CAVStream& aStream) const
	{
	// ah - going ACP
	ChangeState(aStream, CAVStreamStateFactory::EStreamStateACPConfigured);
	}
	
void TAVStreamStateIdle::SetConfigurationL(CAVStream& aStream, 
											RBuf8& aPacketBuffer,
											CSignallingChannel& aSignallingChannel,
										  	TBool aReportingConfigured,
								  			TBool aRecoveryConfigured) const
	{
	LOG_FUNC
// An INT procedure to add in extra options the stack wishes to do, regardless of client
// At present this is limited to mux&frag - which we do if local and remote have
// the *capability* to do so, even if the proposed *configuration* doesnt mention it
	
// AVDTP is daft, so have to arrange for IDs before objects exist if muxing used

// we also need to base this on the number of sessions we know we're going to run	
// see 8.19.8
// 5.4.6 allow fors no reporting session so it's not *obvious* when the 2nd element is
// bit like not obvious what the 2nd L2cap channel will bear

// further, if we're muxing, we need to see what optional sessions are proposed
// in the configuration to see what to configure in the mux capability
	
	TBool canMux;
	TBool canFrag;
	aStream.CanMuxFrag(canMux, canFrag);

	TInt err = KErrNone;
	
	if (canMux)
		{
		// muxing is awkward - must identify the TCs and TSs that are going to be used NOW
		TAvdtpMultiplexingCapability* mux = aStream.CreateMuxCapabilityL(aReportingConfigured, aRecoveryConfigured, aSignallingChannel);

		// ok to create packetstructure - only exchange IDs if do mux
		err = mux->AsProtocol(aPacketBuffer);
		}
		
	if (!err)
		{
		// First check that we are in a state to send a set configuration command as
		// we don't want to send one unless we can commit the pending configuration
		err = aStream.iLocalSEP->CheckConfigurationState(EFalse);
 
 		if (!err)
			{
			err = aSignallingChannel.SendSetConfiguration(aStream, 
								aStream.LocalSEID(), aStream.RemoteSEID(), aPacketBuffer);
			if (!err)
				{
				// tell SEP
				err = aStream.iLocalSEP->SetPendingConfiguration(aPacketBuffer, EFalse); //takes ownership of RBuf
			
				// we should not get an error here as we called CheckConfigurationState above
				__ASSERT_DEBUG(!err, PanicInState(EAvdtpInconsistentPendingConfigurationState));

				if (!err)
					{
					aPacketBuffer.Assign(NULL); // since ownership transfer succeeded
					ChangeState(aStream, CAVStreamStateFactory::EStreamStateConfiguring);
					}
				}
			}
		}

	if (err)
		{
		// clean up and don't bother proceeding
		aStream.DeallocateUnclaimedTransportSessionIDs();
		User::Leave(err);
		}
	}

void TAVStreamStateIdle::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}
		

void TAVStreamStateConfiguring::SetConfigConfirm(CAVStream& aStream, TInt aResult, 
												TSEID /*aRemoteSEID*/, TAvdtpServiceCategory aFailedCategory) const
	{
	LOG_FUNC
	//update our state before telling others
	TRAPD(res, aStream.iLocalSEP->CommitPendingConfigurationL(aResult==KErrNone, EFalse));
	
	if (res!=KErrNone)
		{
		aResult = res;
		}
		
	if (aResult==KErrNone)
		{
		TBool rp = aStream.CheckConfigured(EServiceCategoryReporting);
		TBool rc = aStream.CheckConfigured(EServiceCategoryRecovery);
		aStream.iNumSessionsRequired = AvdtpInternalUtils::NumberOfTransportObjects(rp, rc);
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateINTConfigured);			
		}

	aStream.iStreamNotify.StreamConfigured(aStream.LocalSEID(), aResult, aFailedCategory);

	if (aResult!=KErrNone)
		{
		// remove the stream as the configure failed the stream is invalid
		aStream.Released();	// deletes stream
		}
	}

void TAVStreamStateConfiguring::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateConfigured::Release(CAVStream& aStream) const
	{
	// in this state to Release a stream object (cos it errored say)
	// needs us to send an Abort rather than a Close to remote
	// this is because the remote will need to mark its SEP as not in use,
	// but we haven't yet sent an Open.
	LOG_FUNC
	CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.iRemoteAddress.BTAddr());
	
	if (sigch)
		{
		TInt err = sigch->SendAbort(aStream, aStream.RemoteSEID());
		if (err==KErrNone)
			{
			ChangeState(aStream, CAVStreamStateFactory::EStreamStateAborting);
			}
		else
			{
			// just have to die
			aStream.Released();
			}
		}	
	}

void TAVStreamStateConfigured::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

/**
An INT primitive
A session is adding itself to the stream.
Record its details and find a channel for it to use.
Record the binding between session and channel.
Once all the required sessions are added the stream tries to open the remote
*/
TInt TAVStreamStateINTConfigured::AddSession(CAVStream& aStream,
									TAvdtpTransportSessionType aType,
						   			CUserPlaneTransportSession& aSession,
						   			CTransportChannel*& aChannel,
						   			TL2CapConfig::TChannelPriority aPriority) const
	{
	LOG_FUNC
	TInt ret = KErrNone;
	// check GAVDP isn't being naughty and asking for an unconfigured session
	if ((aType==EReporting && !aStream.CheckConfigured(EServiceCategoryReporting))
		|| (aType==ERecovery && !aStream.CheckConfigured(EServiceCategoryRecovery)))
		{
		// gavdp is trying to use a session it has never configured
		ret = KErrNotSupported;
		}
	
	if (ret == KErrNone)
		{
		TBool useMux;
		TBool useFrag;	//ignored
		aStream.CanMuxFrag(useMux, useFrag);
		
		aChannel = aStream.iProtocol.GetTransportChannel(aStream.iRemoteAddress, useMux);
		
		if (aChannel)
			{
			// bind session to channel
			ret = aChannel->AttachTransportSession(aSession, aType, aPriority);
			if (ret==KErrNone)
				{
				// keep a copy of this binding
				TTransportBinding binding;
				binding.iSession = &aSession;
				binding.iChannel = aChannel;
				
				// keep object bindings
				TTransportBinding* b = NULL;
				
				switch (aType)
					{
					case EMedia:
						b = &aStream.iMediaBinding;
						break;
					case EReporting:
						b = &aStream.iReportingBinding;
						break;
					case ERecovery:
						b = &aStream.iRecoveryBinding;
						break;
					}
		
				__ASSERT_DEBUG(b, PanicInState(EAvdtpBadSessionAttachingToStream));
				// remember binding
				*b = binding;
				}
			TryToOpen(aStream);
			}
		else
			{
			ret = KErrNotReady; // TC went down...
			}
		}
	return ret;
	}

/**
One of the sessions in the stream is dropping from the stream.
At this point the stream hadn't been opened, so don't Release stream
Just cancel oustanding logical channel factory request
*/	
void TAVStreamStateINTConfigured::DropSession(CAVStream& aStream, TAvdtpTransportSessionType /*aType*/, CUserPlaneTransportSession& /*aSession*/) const
	{
	LOG_FUNC
	aStream.iProtocol.LogicalChannelFactory().Cancel(aStream.iFactoryJob);
	}

void TAVStreamStateINTConfigured::TryToOpen(CAVStream& aStream) const
	{
	LOG_FUNC
	// check how many sessions we think we need
	// if all have added to us then we find signalling ch and issue an open
	
	TInt numSessionsBound = (aStream.iMediaBinding.iSession ?1:0) +
							(aStream.iReportingBinding.iSession ?1:0) +
							(aStream.iRecoveryBinding.iSession ?1:0);


	__ASSERT_DEBUG(numSessionsBound>=1 && numSessionsBound<=3, PanicInState(EAVDTPBadSessionCount));
	if (numSessionsBound == aStream.iNumSessionsRequired)
		{
		// GO!
		CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.DeviceAddress());
		TInt err = sigch ? sigch->SendOpenStream(aStream, aStream.RemoteSEID()) : KErrNotReady;
		if (err==KErrNone)
			{
			// all sessions joined - so Open
			ChangeState(aStream, CAVStreamStateFactory::EStreamStateOpening);
			}
		else
			{
			aStream.NotifyUserPlaneTransportSessionsError(NULL, err);
			// no need to release stream - leave gavdp client to decide
			}
		}
	}

void TAVStreamStateINTConfigured::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateINTConfigured::SetConfigurationL(CAVStream& /*aStream*/, 
									RBuf8& /*aPacketBuffer*/,
									CSignallingChannel& /*aSignallingChannel*/,
									TBool /*aReportingConfigured*/,
									TBool /*aRecoveryConfigured*/) const
	{
	// GAVDP spec v1.1 sect4.1.5 forbids this - must be at OPEN state before reconfiguring
	// in our implementation this means we need to be in Ready state (thus have transport channels)
	// yes it is weird...
	User::Leave(KErrAvdtpInvalidStateForReconfigure);
	}

/**
ACP configured, now need to await the Open
*/
	
void TAVStreamStateACPConfigured::AwaitLogicalChannelsL(CAVStream& aStream) const
	{
	LOG_FUNC
	const TBool requireReporting = aStream.CheckConfigured(EServiceCategoryReporting);
	const TBool requireRecovery = aStream.CheckConfigured(EServiceCategoryRecovery);
	
	TAvdtpMultiplexingCapability* const muxCap = static_cast<TAvdtpMultiplexingCapability*>
										(aStream.iLocalSEP->Configuration()[EServiceCategoryMultiplexing]);
	
	if (muxCap)
		{
		TAvdtpMultiplexingCapabilityHelper helper(*muxCap, requireReporting, requireRecovery);
		DoAwaitMuxedChannelsL(aStream, helper, requireReporting, requireRecovery);
		}
	else
		{
		DoAwaitDirectChannelsL(aStream, requireReporting, requireRecovery);
		}
	}
	
	
void TAVStreamStateACPConfigured::DoAwaitMuxedChannelsL(CAVStream& aStream,
												TAvdtpMultiplexingCapabilityHelper& aHelper,
												TBool aRequireReporting,
												TBool aRequireRecovery) const
	{
	LOG_FUNC
	TInt numExtraLogicalChannelsNeeded=0;
	TAvdtpSockAddr address = aStream.iRemoteAddress;
	// get TCIDs for the sessions in the mux record
	// this will indicate the TCIDs that the INT has proposed
	
	// we look for mux channels with any TCIDs proposed
	// and reduce thus reduce the number of TCs we expect
	CTransportChannel** tcPtr = &aStream.iMediaBinding.iChannel;
	
	*tcPtr = aStream.iProtocol.FindMuxChannel(aHelper.MediaCID());
	if (!*tcPtr)
		{
		address.SetSession(EMedia);	
		*tcPtr = aStream.iProtocol.GetTransportChannel(address, ETrue, aHelper.MediaCID());
		User::LeaveIfNull(*tcPtr);
		numExtraLogicalChannelsNeeded++;
		}

	tcPtr = &aStream.iReportingBinding.iChannel;
	if (aRequireReporting)
		{
		__ASSERT_DEBUG(aHelper.ReportingCID() != KInvalidTCID, PanicInState(EAVDTPBadMuxConfiguration));
		*tcPtr = aStream.iProtocol.FindMuxChannel(aHelper.ReportingCID());
		if (!*tcPtr)
			{
			// might need a new channel
			if (aHelper.ReportingCID()!=aHelper.MediaCID())
				{
				address.SetSession(EReporting);	
				*tcPtr = aStream.iProtocol.GetTransportChannel(address, ETrue, aHelper.ReportingCID());
				User::LeaveIfNull(*tcPtr);
				numExtraLogicalChannelsNeeded++;
				}
			// else remote has proposed putting reporting in same channel as media
			}
		}
			
	tcPtr = &aStream.iRecoveryBinding.iChannel;
	if (aRequireRecovery)
		{
		__ASSERT_DEBUG(aHelper.RecoveryCID() != KInvalidTCID, PanicInState(EAVDTPBadMuxConfiguration));
		*tcPtr = aStream.iProtocol.FindMuxChannel(aHelper.RecoveryCID());
		if (!*tcPtr)
			{
			// might need a new channel
			if (aHelper.RecoveryCID()!=aHelper.MediaCID() && aHelper.RecoveryCID()!=aHelper.ReportingCID())
				{
				address.SetSession(ERecovery);	
				*tcPtr = aStream.iProtocol.GetTransportChannel(address, ETrue, aHelper.RecoveryCID());
				User::LeaveIfNull(*tcPtr);
				numExtraLogicalChannelsNeeded++;
				}
			else if (aHelper.RecoveryCID()==aHelper.MediaCID())
				{
				__DEBUGGER();
				User::Leave(KErrCorrupt); // remote has contravened spec
				}
			}
		}

	if (!numExtraLogicalChannelsNeeded)
		{
		// everything we need is already here, kick state machine and wait for sessions to attach
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateWaitForSessions);
		// tell signalling session
		aStream.iStreamNotify.StreamAccepted(aStream.LocalSEID(),
												aStream.RemoteSEID(), aRequireReporting, aRequireRecovery);
		}
	else
		{
		// TCs were created above, now wait for appropriate logical channels	
		aStream.iFactoryJob = aStream.iProtocol.LogicalChannelFactory().
												ExpectSessionLogicalChannelsL(aStream,
												numExtraLogicalChannelsNeeded);
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateWaitingForLogicalChannels);
		}
	}
	
	
void TAVStreamStateACPConfigured::DoAwaitDirectChannelsL(CAVStream& aStream,
												TBool aRequireReporting,
												TBool aRequireRecovery) const
	{
	LOG_FUNC
	// create the necessary Transport Channels ready for the logical channels
	// assume all inbound TCs are mux -our seps always do muxing, and allows for "upgrades"
	// if others SEPs want to add sessions to them later
	
	TInt numExtraLogicalChannelsNeeded = 0;
	TAvdtpSockAddr address = aStream.iRemoteAddress;
	address.SetSession(EMedia);	

#ifdef __SYMBIAN_AVDTP_HIDE_MUX 
    #define CAN_BECOME_MUX EFalse
#else
#pragma message("Not sure if this true - remember morgan, cannot upgrade as dont know which "direct" channel has which TCID")
    #define CAN_BECOME_MUX ETrue
#endif   

	aStream.iMediaBinding.iChannel = aStream.iProtocol.GetTransportChannel(address, CAN_BECOME_MUX);
	User::LeaveIfNull(aStream.iMediaBinding.iChannel);
	numExtraLogicalChannelsNeeded++;

	if (aRequireReporting)
		{
		address.SetSession(EReporting);	
		aStream.iReportingBinding.iChannel = aStream.iProtocol.GetTransportChannel(address, CAN_BECOME_MUX);
		User::LeaveIfNull(aStream.iReportingBinding.iChannel);
		numExtraLogicalChannelsNeeded++;
		}
	
	if (aRequireRecovery)
		{
		address.SetSession(ERecovery);	
		aStream.iRecoveryBinding.iChannel = aStream.iProtocol.GetTransportChannel(address, CAN_BECOME_MUX);
		User::LeaveIfNull(aStream.iRecoveryBinding.iChannel);
		numExtraLogicalChannelsNeeded++;
		}

  	LOG1(_L8("Awaiting %d logical channels"), numExtraLogicalChannelsNeeded);
  
	// wait for appropriate logical channels	
	aStream.iFactoryJob = aStream.iProtocol.LogicalChannelFactory().
												ExpectSessionLogicalChannelsL(aStream,
												numExtraLogicalChannelsNeeded);

	ChangeState(aStream, CAVStreamStateFactory::EStreamStateWaitingForLogicalChannels);
	}
	
TInt TAVStreamStateACPConfigured::AddSession(CAVStream& /*aStream*/, 
							TAvdtpTransportSessionType /*aType*/,
							CUserPlaneTransportSession& /*aSession*/,
							CTransportChannel*& /*aTransportChannel*/,
							TL2CapConfig::TChannelPriority /*aPriority*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}

void TAVStreamStateACPConfigured::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}


void TAVStreamStateOpen::DropSession(CAVStream& aStream, TAvdtpTransportSessionType /*aType*/, CUserPlaneTransportSession& /*aSession*/) const
	{	
	LOG_FUNC
	// if any session in the stream is shutdown
	// we invalidate the stream, and thus send a Release
			
	// we leave the GC to actually shutdown the session saps
	DoRelease(aStream);	
	// leave unbinding transport objects until ReleaseConfirm	
	}

void TAVStreamStateOpen::Release(CAVStream& aStream) const
	{
	LOG_FUNC
	// we need to Release this as the object is going
	DoRelease(aStream);
	}

void TAVStreamStateOpen::DoRelease(CAVStream& aStream) const
	{
	LOG_FUNC
	CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.iRemoteAddress.BTAddr());
	
	if (sigch)
		{
		sigch->SendRelease(aStream, aStream.RemoteSEID());
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateReleasing);
		}
	}

void TAVStreamStateOpen::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

TInt TAVStreamStateReady::AddSession(CAVStream& /*aStream*/, 
		TAvdtpTransportSessionType /*aType*/,
		CUserPlaneTransportSession& /*aSession*/,
		CTransportChannel*& /*aTransportChannel*/,
		TL2CapConfig::TChannelPriority /*aPriority*/) const
{
LOG_FUNC
#ifdef _DEBUG
return KErrPanicAvdtpOpenInBadState;
#else
return KErrNotReady;
#endif
}

void TAVStreamStateReady::SetConfigurationL(CAVStream& aStream, 
									RBuf8& aPacketBuffer,
									CSignallingChannel& aSignallingChannel,
									TBool __DEBUG_ONLY(aReportingConfigured),
									TBool __DEBUG_ONLY(aRecoveryConfigured)) const
	{
	// this is a Reconfigure - see GAVDP spec
	LOG_FUNC
	__ASSERT_DEBUG(!aReportingConfigured && !aRecoveryConfigured, PanicInState(EAvdtpReconfigurationCapabilitiesNotChecked));

	// First check that we are in a state to send a reconfigure command as
	// we don't want to send one unless we can commit the pending configuration
	TInt err = aStream.iLocalSEP->CheckConfigurationState(ETrue);
	
	if (!err)
		{
		err = aSignallingChannel.SendReconfigure(aStream, aStream.RemoteSEID(), aPacketBuffer);
		
		if (!err)
			{
			// tell SEP
			err = aStream.iLocalSEP->SetPendingConfiguration(aPacketBuffer, ETrue); //takes ownership of RBuf

			// we should not get an error here as we called CheckConfigurationState above
			__ASSERT_DEBUG(!err, PanicInState(EAvdtpInconsistentPendingReconfigurationState));

			if (!err)
				{
				aPacketBuffer.Assign(NULL); // since ownership transfer succeeded
				// release configuration buffer as we own it in this state
				ChangeState(aStream, CAVStreamStateFactory::EStreamStateReconfiguring);
				}
			}
		}
		
	User::LeaveIfError(err);
	}

/**
 * Before the OpenStream returns, gavdp might get Cancel()ed. At this point we have an outstanding
 * request on the signalling channel. We can't issue a cancel on the individual request, and we
 * can't close sigch down as it's shared, but there should only be one outstanding for this stream,
 * so we can cancel all transactions for it.
 */
void TAVStreamStateOpening::DropSession(CAVStream& aStream, TAvdtpTransportSessionType /*aType*/,
		 CUserPlaneTransportSession& /*aSession*/) const
	{
		LOG_FUNC
		CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.DeviceAddress());
		__ASSERT_DEBUG(sigch, PanicInState(EAvdtpSignallingChannelShouldExist));
		sigch->CancelTransactions(aStream);
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateIdle);
	}

void TAVStreamStateOpening::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateOpening::OpenConfirm(CAVStream& aStream, TInt aResult, TSEID __DEBUG_ONLY(aRemoteSEID)) const
	{
	LOG_FUNC
	// good!
	__ASSERT_DEBUG(aRemoteSEID == aStream.RemoteSEID(), PanicInState(EAvdtpConfirmAddressedToWrongRequester));
	TInt ret = KErrNone;
	
// now we kick TCs into life
	if (aResult)
		{
		// late reply or connection problem, abort

		CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.iRemoteAddress.BTAddr());
		// should always have sigch here
		__ASSERT_DEBUG(sigch, PanicInState(EAvdtpSignallingChannelShouldExist));
		if (sigch)
			{
			sigch->SendAbort(aStream, aStream.RemoteSEID());
			}
	
		// Opening is performed when the necessary sockets are created and connected in a stream
		aStream.NotifyUserPlaneTransportSessionsError(NULL, aResult);
		}
	else
		{
		// create a job - see what logical channels need creating
		TInt numLogicalChannelsRequired =
				aStream.iMediaBinding.iChannel->IsConnected() ? 0 : 1;
		
		if (aStream.iReportingBinding.iChannel)
			{
			// a reporting transport channel is required - but is it available?
			if (!aStream.iReportingBinding.iChannel->IsConnected())
				{
				// this is required, so add into job
				numLogicalChannelsRequired++;
				}
			}
			
		if (aStream.iRecoveryBinding.iChannel)
			{
			// a recovery transport channel is required - but is it available?
			if (!aStream.iRecoveryBinding.iChannel->IsConnected())
				{
				// this is required, so add into job
				numLogicalChannelsRequired++;
				}
			}
		
		if (numLogicalChannelsRequired)
			{
			// if either required, kick off request...
			ChangeState(aStream, CAVStreamStateFactory::EStreamStateCreatingLogicalChannels);
			TLogicalChannelFactoryTicket ticket;
			TRAP(ret, ticket=aStream.iProtocol.LogicalChannelFactory().CreateSessionLogicalChannelsL(
												aStream.iRemoteAddress.BTAddr(),
										  		aStream, numLogicalChannelsRequired));
			if (ret==KErrNone)
				{
				aStream.iFactoryJob = ticket; // record this, so we can cancel if needed
				}
			else
				{
				// stay in same state
				ChangeState(aStream, CAVStreamStateFactory::EStreamStateOpening);
				}
			}		
		else
			{
			ChangeState(aStream, CAVStreamStateFactory::EStreamStateOpen);
			// all TCs appear to be available without creating new ones
			aStream.TransportChannelsReady();
			}
		}
	}


TInt TAVStreamStateReady::StartReceived(CAVStream& aStream) const
	{
	LOG_FUNC
	
	aStream.ReadyForStartIndication();
	
	return KErrNone;
	}

TInt TAVStreamStateReady::Start(CAVStream& aStream) const
	{
	LOG_FUNC
	TInt retVal = KErrNone;
	CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.iRemoteAddress.BTAddr());
		
	// should always have sigch here - hence function signature
	__ASSERT_DEBUG(sigch, PanicInState(EAvdtpSignallingChannelShouldExist));
	if (sigch)
		{
		retVal = sigch->SendStartStream(aStream, aStream.RemoteSEID());
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateStarting);
		}
	return retVal;
	}

void TAVStreamStateReady::Started(CAVStream& aStream) const
	{
	LOG_FUNC
	ChangeState(aStream, CAVStreamStateFactory::EStreamStateStreaming);
	}
	
void TAVStreamStateReady::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateStarting::Enter(CAVStream& aStream) const
	{
	LOG_FUNC
	// Tell the physical link that encryption keys could be refreshed now, to prevent an auto refresh later
	aStream.TryToAndThenPreventHostEncryptionKeyRefresh();
	}


void TAVStreamStateStarting::StartConfirm(CAVStream& aStream, TInt aResult, TSEID /*aRemoteSEID*/) const
	{
	LOG_FUNC
	// just change state
	if (aResult==KErrNone)
		{
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateStreaming);
		aStream.iStreamNotify.StreamStarted(aStream.LocalSEID());
		}
	else
		{
		// whether suspended or idle, we return to ready 
		// but first release the prevent encryption key refresh token
		aStream.AllowHostEncryptionKeyRefresh();
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateReady);
		aStream.iStreamNotify.StreamInitiatedServiceFailed(aStream, aResult);		
		}
	}
	
TInt TAVStreamStateStarting::StartReceived(CAVStream& /*aStream*/) const
	{
	LOG_FUNC

	// we need to tell the other end 'nope we're busy' because the starts 'crossed in the post'
	// returning KErrInUse will tell the other end to go away (politely of course)
	return KErrInUse;
	}


void TAVStreamStateReleasing::Release(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// we are already releasing so do nothing.
	}
	
void TAVStreamStateReleasing::ReleaseConfirm(CAVStream& aStream, TInt /*aResult*/, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	// ignore error - proceed anyway
	aStream.Released();
	}
	
void TAVStreamStateReleasing::DropSession(CAVStream& /*aStream*/, TAvdtpTransportSessionType /*aType*/, CUserPlaneTransportSession& /*aSession*/) const
	{
	LOG_FUNC
	// to guard against sending multiple Releases per dropped session
	// since we're releasing, we don't need to send a Release, but we just consume
	}
	
void TAVStreamStateReleasing::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}
	

void TAVStreamStateAborting::AbortConfirm(CAVStream& aStream, TSEID /*aSEID*/) const
	{
	LOG_FUNC
	aStream.Released();
	}

void TAVStreamStateAborting::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}
	

void TAVStreamStateAborting::Release(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// we are already releasing so do nothing.
	}
		

void TAVStreamStateCreatingLogicalChannels::LogicalChannelsAvailable(CAVStream& aStream, TLogicalChannelFactoryTicket aTicket, TInt aError) const
	{
	LOG_FUNC
	aStream.iFactoryJob = aTicket;
	aStream.BindLogicalAndTransportChannels(aTicket, aError);
			
	if (aError==KErrNone)
		{
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateReady);
		// tell sessions...
		aStream.TransportChannelsReady();		
		}
	else
		{
		aStream.iStreamNotify.StreamInitiatedServiceFailed(aStream, aError);
		}
	}
	

void TAVStreamStateCreatingLogicalChannels::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateWaitingForLogicalChannels::Enter(CAVStream& aStream) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	
	// start guard timer in case logical channels never arrive for TCs
	aStream.StartWatchdog();
	}

void TAVStreamStateWaitingForLogicalChannels::LogicalChannelsAvailable(CAVStream& aStream, TLogicalChannelFactoryTicket aResponse, TInt aError) const
	{
	LOG_FUNC
	// we created the transport channels in :Await, so now we can bind
	// then the sessions can call :AddSession and scoop up the TCs
	aStream.iFactoryJob = aResponse;
	aStream.BindLogicalAndTransportChannels(aResponse, aError);

	if (aError==KErrNone)
		{	
		const TBool reportingPresent = aStream.iReportingBinding.iChannel ? ETrue : EFalse;
		const TBool recoveryPresent = aStream.iRecoveryBinding.iChannel ? ETrue : EFalse;

		aStream.iNumSessionsRequired = AvdtpInternalUtils::NumberOfTransportObjects(reportingPresent, recoveryPresent);
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateWaitForSessions);
		// tell signalling session		
		aStream.iStreamNotify.StreamAccepted(aStream.LocalSEID(),
												aStream.RemoteSEID(), reportingPresent, recoveryPresent);
		}
	else
		{
		aStream.iStreamNotify.StreamInitiatedServiceFailed(aStream, aError);
		}
	}

void TAVStreamStateWaitingForLogicalChannels::DropSession(CAVStream& aStream, TAvdtpTransportSessionType /*aType*/,
		 CUserPlaneTransportSession& /*aSession*/) const
	{
		LOG_FUNC
		aStream.iProtocol.LogicalChannelFactory().Cancel(aStream.iFactoryJob);
	}

void TAVStreamStateWaitingForLogicalChannels::Exit(CAVStream& aStream) const
	{
	LOG_FUNC
	// start guard timer in case logical channels never arrive for TCs
	aStream.StopWatchdog();
	}

void TAVStreamStateWaitingForLogicalChannels::WatchdogFired(CAVStream& aStream) const
	{
	LOG_FUNC
	// need to tidy TCs, and Abort stream and cancel job
	TLogicalChannelFactoryTicket nullTicket;
	
	aStream.BindLogicalAndTransportChannels(nullTicket, KErrTimedOut);
	
	aStream.iStreamNotify.StreamInitiatedServiceFailed(aStream, KErrTimedOut);
	}


/**
An ACP primitive: the TCs & LCs are ready, now we are awaiting the sessions
to come in and connect to the TCs.  The sessions do so at the behest of GAVDP
*/
TInt TAVStreamStateWaitForSessions::AddSession(CAVStream& aStream,
									TAvdtpTransportSessionType aType,
						   			CUserPlaneTransportSession& aSession,
						   			CTransportChannel*& aChannel,
						   			TL2CapConfig::TChannelPriority aPriority) const
	{
	LOG_FUNC
	TInt ret = KErrNone;

	// bind session to channel - the channel is there, we'd have been told if it had gone
	TTransportBinding* b = NULL;
	
	switch (aType)
		{
		case EMedia:
			b = &aStream.iMediaBinding;
			break;
		case EReporting:
			// check GAVDP isn't being naughty and asking for an unconfigured session
			if (aStream.CheckConfigured(EServiceCategoryReporting))
				{
				b = &aStream.iReportingBinding;
				}
			else
				{
				// GAVDP asking for a bearer that is not configured
				ret = KErrNotSupported;
				}
			break;
		case ERecovery:
			// check GAVDP isn't being naughty and asking for an unconfigured session
			if (aStream.CheckConfigured(EServiceCategoryRecovery))
				{
				b = &aStream.iRecoveryBinding;
				}
			else
				{
				// GAVDP asking for a bearer that is not configured
				ret = KErrNotSupported;
				}
			break;
		default:
			PanicInState(EAvdtpBadSessionAttachingToStream);
		}
	
	if (ret == KErrNone)
		{
		ret = b->iChannel->AttachTransportSession(aSession, aType, aPriority);
		if (ret==KErrNone)
			{
			// stream needs to remember binding
			b->iSession = &aSession;
			// and session need to know channel
			aChannel = b->iChannel;
			// check if all sessions have bound
			if (!--aStream.iNumSessionsRequired)
				{
				ChangeState(aStream, CAVStreamStateFactory::EStreamStateReady);
				aStream.TransportChannelsReady();
				}
			}
		}
	return ret;
	}	

TInt TAVStreamStateWaitForSessions::StartReceived(CAVStream& aStream) const
	{
	LOG_FUNC
	ChangeState(aStream, CAVStreamStateFactory::EStreamStateWaitForSessionsStartReceived);
	return KErrNone;	
	}	

void TAVStreamStateWaitForSessions::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

/**
Overrides the method in TAVStreamStateWaitForSessions. The start has already been received by
CSignallingSession::StartIndication, so after changing the state to EStreamStateReady data
stored in the CAVStream object will be used to pass a start indication to GAVDP.
*/
TInt TAVStreamStateWaitForSessionsStartReceived::AddSession(CAVStream& aStream,
												TAvdtpTransportSessionType aType,
						   						CUserPlaneTransportSession& aSession,
						   						CTransportChannel*& aChannel,
						   						TL2CapConfig::TChannelPriority aPriority) const
	{
	LOG_FUNC
	// Call the function from the parent class to bind the session
	TInt ret = TAVStreamStateWaitForSessions::AddSession(aStream,aType,aSession,aChannel, aPriority);
	
	// if that worked, and all the channels are bound, we can now issue the delayed start
	if((ret == KErrNone) && (!aStream.iNumSessionsRequired))
		{
		aStream.ReadyForStartIndication();
		}

	return ret;
	}
	
TInt TAVStreamStateWaitForSessionsStartReceived::StartReceived(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}	

void TAVStreamStateWaitForSessionsStartReceived::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateSuspending::SuspendConfirm(CAVStream& aStream, TInt aResult, TSEID /*aRemoteSEID*/) const
	{
	LOG_FUNC
	// just change state
	if (aResult==KErrNone)
		{
		// no difference between a "suspended" state, and ready
		// making them the same helps support reconfig in more states
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateReady);
		aStream.iStreamNotify.StreamSuspended(aStream.LocalSEID());
		}
	else
		{
		// stay in streaming state and notify the user of the error
		ChangeState(aStream, CAVStreamStateFactory::EStreamStateStreaming);
		aStream.iStreamNotify.StreamInitiatedServiceFailed(aStream, aResult);
		}
	}

void TAVStreamStateSuspending::Suspended(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	// throw away
	}
	

void TAVStreamStateSuspending::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}

void TAVStreamStateReconfiguring::ReconfigConfirm(CAVStream& aStream, TInt aResult, 
												TSEID /*aRemoteSEID*/, TAvdtpServiceCategory aFailedCategory) const
	{
	LOG_FUNC
	// ok to return to ready on success or failure
	ChangeState(aStream, CAVStreamStateFactory::EStreamStateReady);
	TRAPD(err, aStream.iLocalSEP->CommitPendingConfigurationL(aResult==KErrNone, ETrue));
	if (err!=KErrNone)
		{
		aStream.iStreamNotify.StreamInitiatedServiceFailed(aStream, aResult);
		}
	aStream.iStreamNotify.StreamConfigured(aStream.LocalSEID(), aResult, aFailedCategory);
	}

void TAVStreamStateReconfiguring::Enter(CAVStream& __DEBUG_ONLY(aStream)) const
	{
	LOG_FUNC
	//We should not be preventing the encryption key refresh in this state. Asserting this condition. 
	__ASSERT_DEBUG(!aStream.IsHostEncryptionKeyRefreshPrevented(),Panic(EUnexpectedEncryptionKeyRefreshPrevent));
	}
	

void TAVStreamStateStreaming::Enter(CAVStream& aStream) const
	{                              
	LOG_FUNC
 // experiments with window dragging show not much change if thread priority bumped
 // maybe due to trying to get into esock in the first place
 // Changing the role of the Esock_BT thread in the cmi file, from player to 
 // DealerPlayer allows messages to be passed directly to the Esock_BT thread
 
 	aStream.iOriginalESockBTPriority = RThread().Priority(); //save the original priority of the Esock_BT thread.
 	RThread().SetPriority(EPriorityRealTime);
	
	}

void TAVStreamStateStreaming::Exit(CAVStream& aStream) const
	{
	LOG_FUNC
	RThread().SetPriority(aStream.iOriginalESockBTPriority);//set the priority of the thread back to the original value.
	
	// Tell the physical link that encryption keys could be refreshed now, to prevent an auto refresh later
	aStream.AllowHostEncryptionKeyRefresh();
	}
	
TInt TAVStreamStateStreaming::Suspend(CAVStream& aStream) const
	{
	LOG_FUNC
	TInt retVal = KErrNone;
	CSignallingChannel* sigch = aStream.iProtocol.FindSignallingChannel(aStream.iRemoteAddress.BTAddr());
		
	__ASSERT_DEBUG(sigch, PanicInState(EAvdtpSignallingChannelShouldExist));
	if (sigch)
		{
		retVal = sigch->SendSuspendStream(aStream, aStream.RemoteSEID());
		if (retVal==KErrNone)
			{
			ChangeState(aStream, CAVStreamStateFactory::EStreamStateSuspending);
			}
		}
	return retVal;
	}
	
void TAVStreamStateStreaming::Suspended(CAVStream& aStream) const
	{
	LOG_FUNC
	ChangeState(aStream, CAVStreamStateFactory::EStreamStateReady);
	}

TBool TAVStreamStateStreaming::IsStreaming(const CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return ETrue;
	}
	
TInt TAVStreamStateStreaming::Start(CAVStream& /*aStream*/) const
	{
	// don't want to tell remote to Start when already Streaming, cos it's BadState
	return KErrNotReady;
	}
 
TInt TAVStreamStateStreaming::StartReceived(CAVStream& /*aStream*/) const
	{
	LOG_FUNC
	return KErrNotReady;
	}

