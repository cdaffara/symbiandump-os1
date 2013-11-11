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
// Implements the avdtp stream
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpStream.h"
#include "avdtpsap.h"
#include "avdtp.h"
#include "avdtputil.h"
#include "avdtpSignallingChannel.h"
#include "avdtpSignallingSession.h"
#include "avdtpLocalSEP.h"

#include "avdtputil.h"
#include "btconsts.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

CAVStreamStateFactory* CAVStreamStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CAVStreamStateFactory* ret=new (ELeave) CAVStreamStateFactory();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

void CAVStreamStateFactory::ConstructL()
	{
	LOG_FUNC
	iStates[EStreamStateIdle]=new (ELeave) TAVStreamStateIdle(*this);
	iStates[EStreamStateWaitForSessions]=new (ELeave) TAVStreamStateWaitForSessions(*this);
	iStates[EStreamStateWaitingForLogicalChannels]=new (ELeave) TAVStreamStateWaitingForLogicalChannels(*this);
	iStates[EStreamStateCreatingLogicalChannels]=new (ELeave) TAVStreamStateCreatingLogicalChannels(*this);
	iStates[EStreamStateConfiguring]=new (ELeave) TAVStreamStateConfiguring(*this);
	iStates[EStreamStateINTConfigured]=new (ELeave) TAVStreamStateINTConfigured(*this);
	iStates[EStreamStateACPConfigured]=new (ELeave) TAVStreamStateACPConfigured(*this);
	iStates[EStreamStateOpening]=new (ELeave) TAVStreamStateOpening(*this);
	iStates[EStreamStateOpen]=new (ELeave) TAVStreamStateOpen(*this);
	iStates[EStreamStateReady]=new (ELeave) TAVStreamStateReady(*this);
	iStates[EStreamStateStarting]=new (ELeave) TAVStreamStateStarting(*this);
	iStates[EStreamStateSuspending]=new (ELeave) TAVStreamStateSuspending(*this);
	iStates[EStreamStateReconfiguring]=new (ELeave) TAVStreamStateReconfiguring(*this);
	iStates[EStreamStateReleasing]=new (ELeave) TAVStreamStateReleasing(*this);
	iStates[EStreamStateAborting]=new (ELeave) TAVStreamStateAborting(*this);
	iStates[EStreamStateStreaming]=new (ELeave) TAVStreamStateStreaming(*this);
	iStates[EStreamStateWaitForSessionsStartReceived]=new (ELeave) TAVStreamStateWaitForSessionsStartReceived(*this);
	}

CAVStreamStateFactory::~CAVStreamStateFactory()
	{
	LOG_FUNC
	iStates.DeleteAll();
	}

const TAVStreamState& CAVStreamStateFactory::GetState(CAVStreamStateFactory::TAVStreamStates aState) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aState != EStreamMaxState, Panic(EAvdtpMuxerStateOutOfBounds));
	return *iStates[aState];
	}

TInt CAVStreamStateFactory::StateIndex(const TAVStreamState* aState) const
	{
	TInt state;
	for (state = 0; state < EStreamMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KAvdtpUnknownState;
	}


CAVStream* CAVStream::NewL(const TAvdtpSockAddr& aAddr,
					 		MAvdtpStreamNotify& aStreamNotify,
					 		CAvdtpProtocol& aProtocol,
					 		CLocalSEP& aLocalSEP)
	{
	LOG_STATIC_FUNC
	CAVStream* stream = new (ELeave) CAVStream(aAddr, 
											   aStreamNotify,
											   aProtocol,
											   aLocalSEP);
	CleanupStack::PushL(stream);											   
	stream->ConstructL();
	CleanupStack::Pop();											  
	return stream;
	}

void CAVStream::ConstructL()
	{
	LOG_FUNC
	iWatchdogTimer = CWatchdogTimer::NewL(*this);
	// watchdog actually starts Just-in-time
	}

CAVStream::CAVStream(const TAvdtpSockAddr& aAddr,
					 MAvdtpStreamNotify& aStreamNotify,
					 CAvdtpProtocol& aProtocol,
					 CLocalSEP& aLocalSEP)
: iProtocol(aProtocol), iStreamNotify(aStreamNotify), iLocalSEP(&aLocalSEP), iNotifiedACLStreamActive(EFalse)
	{
	LOG_FUNC
	iRemoteAddress = aAddr;
	__ASSERT_DEBUG(!iRemoteAddress.SEID().IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	__ASSERT_DEBUG(iLocalSEP->SEID().IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));														   		
	
	iState = &iProtocol.StreamStateFactory().GetState(CAVStreamStateFactory::EStreamStateIdle);
	}
	
CAVStream::~CAVStream()
	{
	LOG_FUNC
	
	// We might already have done this, but call again regardless, as
	// the dtor might be called whilst still in the streaming state
	AllowHostEncryptionKeyRefresh();
	
	// cancel outstanding operations on signalling channel
	CSignallingChannel* sigCh = iProtocol.FindSignallingChannel(iRemoteAddress.BTAddr());		

	if (sigCh)
		{
		sigCh->CancelTransactions(*this);
		}

	iProtocol.LogicalChannelFactory().Cancel(iFactoryJob);

	if (iLocalSEP)
		{
		iLocalSEP->StreamDestroyed(*this);
		}
	// remove ourselves from protocol list
	iProtocol.RemoveStream(*this);
	
	// tidyup up any pre-allocated transport session IDs
	DeallocateUnclaimedTransportSessionIDs();

	delete iWatchdogTimer;
	}

void CAVStream::DeallocateUnclaimedTransportSessionIDs()
	{
	LOG_FUNC
	for (TInt i=0; i<=iMuxedTSIDs.Count()-1; i++)
		{
		iMuxedTSIDs[i].Close();
		}
	}

#ifdef _DEBUG
void CAVStream::LocalSEPDestroyed(const CLocalSEP& aTerminator)
#else
void CAVStream::LocalSEPDestroyed(const CLocalSEP& /*aTerminator*/)
#endif
	{
	LOG_FUNC
	__ASSERT_DEBUG(&aTerminator == iLocalSEP, Panic(EAvdtpBadLocalSEPInstructingStreamRelease));
	// release the stream if needed
	iLocalSEP = NULL;
	Release();
	}

void CAVStream::StartWatchdog()
	{
	LOG_FUNC
	iWatchdogTimer->After(KAvdtpLogicalChannelsWatchdogTimeout);
	}
	
void CAVStream::StopWatchdog()
	{
	LOG_FUNC
	iWatchdogTimer->Cancel();
	}
	
void CAVStream::WatchdogFired()
	{
	LOG_FUNC
	iState->WatchdogFired(*this);
	}
	
void CAVStream::LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket aTicket, TInt aError)
	{
	LOG_FUNC
	iState->LogicalChannelsAvailable(*this, aTicket, aError);
	}
	

void CAVStream::OpenConfirm(TInt aResult, TSEID aRemoteSEID)
	{
	LOG_FUNC
	iState->OpenConfirm(*this, aResult, aRemoteSEID);
	}
		
void CAVStream::ReleaseConfirm(TInt aResult, TSEID aRemoteSEID)
	{
	LOG_FUNC
	iState->ReleaseConfirm(*this, aResult, aRemoteSEID);
	}

void CAVStream::AbortConfirm(TSEID aRemoteSEID)
	{
	LOG_FUNC
	iState->AbortConfirm(*this, aRemoteSEID);
	}

void CAVStream::Configured()
	{
	LOG_FUNC
	iState->Configured(*this);
	}

void CAVStream::AwaitLogicalChannelsL()
	{
	LOG_FUNC
	iState->AwaitLogicalChannelsL(*this);
	}


TBool CAVStream::IsStreaming() const
	{
	LOG_FUNC
	return iState->IsStreaming(*this);
	}
	
void CAVStream::Started()
	{
	LOG_FUNC
	iState->Started(*this);
	}

void CAVStream::Suspended()
	{
	LOG_FUNC
	iState->Suspended(*this);
	}

TBool CAVStream::MultiplexingConfigured() const
	{
	return iLocalSEP->MultiplexingConfigured();
	}
	
/**
Call from somewhere that an error needs to be forwarded to the transport sessions
A session can tell us to tell others, but not tell itself
*/
void CAVStream::NotifyUserPlaneTransportSessionsError(CUserPlaneTransportSession* aErroredSession, TInt aError)
	{
	LOG_FUNC
	if (iMediaBinding.iSession && iMediaBinding.iSession != aErroredSession)
		{
		// although all streams must have media sessions, the session may not have bound yet (as when we are ACP)
		iMediaBinding.iSession->StreamError(aError);
		/* this stream error will eventually go through to the SAP, and then the 
		 transportsession will be deleted, this in turn causes a call through to
		 the (public) ClearSession function that will NULL iMediaBinding.iSession 
		 so we don't use it again
		 */
		}
		
	if (iReportingBinding.iSession && iReportingBinding.iSession != aErroredSession)
		{
		iReportingBinding.iSession->StreamError(aError);
		/* this stream error will eventually go through to the SAP, and then the 
		 transportsession will be deleted, this in turn causes a call through to
		 the (public) ClearSession function that will NULL iMediaBinding.iSession 
		 so we don't use it again
		 */
		}
	if (iRecoveryBinding.iSession && iRecoveryBinding.iSession != aErroredSession)
		{
		iRecoveryBinding.iSession->StreamError(aError);
		/* this stream error will eventually go through to the SAP, and then the 
		 transportsession will be deleted, this in turn causes a call through to
		 the (public) ClearSession function that will NULL iMediaBinding.iSession 
		 so we don't use it again
		 */
		}
	}
	
void CAVStream::Released()
	{
	LOG_FUNC
		
	if (iMediaBinding.iSession)
		{
		// the media binding would have detached now if it was actively shutdown thereby
		// leading to this release
		iMediaBinding.iSession->StreamReleased();
		/* this stream release will eventually go through to the SAP, and then the 
		 transportsession will be deleted, this in turn causes a call through to
		 the (public) ClearSession function that will NULL iMediaBinding.iSession 
		 so we don't use it again
		 */
		}
		
	if (iReportingBinding.iSession)
		{
		iReportingBinding.iSession->StreamReleased();
		/* this stream release will eventually go through to the SAP, and then the 
		 transportsession will be deleted, this in turn causes a call through to
		 the (public) ClearSession function that will NULL iMediaBinding.iSession 
		 so we don't use it again
		 */
		}
	if (iRecoveryBinding.iSession)
		{
		iRecoveryBinding.iSession->StreamReleased();
		/* this stream release will eventually go through to the SAP, and then the 
		 transportsession will be deleted, this in turn causes a call through to
		 the (public) ClearSession function that will NULL iMediaBinding.iSession 
		 so we don't use it again
		 */
		}
		
	delete this;
	}

TInt CAVStream::AddSession(TAvdtpTransportSessionType aType,
						   CUserPlaneTransportSession& aSession,
						   CTransportChannel*& aChannel,
						   TL2CapConfig::TChannelPriority aPriority)
	{
	LOG_FUNC
	return iState->AddSession(*this, aType, aSession, aChannel, aPriority);
	}
	
void CAVStream::DropSession(TAvdtpTransportSessionType aType, CUserPlaneTransportSession& aSession)
	{
	LOG_FUNC
	iState->DropSession(*this, aType, aSession);
	}
	
	
/**
Called from states as common helper
*/
void CAVStream::TransportChannelsReady()
	{
	LOG_FUNC
	// notify the sessions - all the TCs are ready. phew.
	TBTDevAddr addr = iRemoteAddress.BTAddr();
	
	iMediaBinding.iSession->Ready(addr);
	
	if (iReportingBinding.iSession)
		{
		iReportingBinding.iSession->Ready(addr);
		}
	if (iRecoveryBinding.iSession)
		{
		iRecoveryBinding.iSession->Ready(addr);
		}
	}

/* this public vsn is used by transport session and avdtpSAP
   when deleting the session we're bound to 
 */
void CAVStream::ClearSession(CUserPlaneTransportSession& aSession)
	{
	LOG_FUNC
	if (iMediaBinding.iSession == &aSession)
		{
		iMediaBinding.iSession = NULL;
		}
	else if (iReportingBinding.iSession == &aSession)
		{
		iReportingBinding.iSession = NULL;
		}
	else if (iRecoveryBinding.iSession == &aSession)
		{
		iRecoveryBinding.iSession = NULL;
		}
	}

TAvdtpMultiplexingCapability* CAVStream::CreateMuxCapabilityL(TBool aRequireReporting,
											TBool aRequireRecovery,
											CSignallingChannel& aIDProvider)
	{
	LOG_FUNC
	// this is called for when we're muxing
	// try to get transport channel objects now (they won't ask for logical channels yet)
	// for transport sessions we have to wait as they MUST be associated with a SAP
	// which is created when user opens an RSocket...
	// so here we just satisfy the weird muxing requirements by allocating the TSID
	// then the session can acquire ownership of that later
	
	// the address used to find a suitable transport channel
	TAvdtpMultiplexingCapability* mux = new(ELeave) TAvdtpMultiplexingCapability;
	CleanupStack::PushL(mux);
	
	TAvdtpMultiplexingCapabilityHelper helper(*mux, aRequireReporting, aRequireRecovery);
	
	TAvdtpSockAddr addr(iRemoteAddress);
	addr.SetSession(EMedia);
	
	iMediaBinding.iChannel = iProtocol.GetTransportChannel(addr, ETrue);
	User::LeaveIfNull(iMediaBinding.iChannel);
	helper.SetMediaCID(iMediaBinding.iChannel->TCID());
	User::LeaveIfError(aIDProvider.TSIDManager().GetTSID(iMuxedTSIDs[EMedia]));
	helper.SetMediaSID(iMuxedTSIDs[EMedia].TSID());

	if (aRequireReporting)
		{
		User::LeaveIfError(aIDProvider.TSIDManager().GetTSID(iMuxedTSIDs[EReporting]));

		addr.SetSession(EReporting);
		iReportingBinding.iChannel = iProtocol.GetTransportChannel(addr, ETrue);
		User::LeaveIfNull(iReportingBinding.iChannel);
		helper.SetReportingCID(iReportingBinding.iChannel->TCID());
		helper.SetReportingSID(iMuxedTSIDs[EReporting].TSID());
		}

	if (aRequireRecovery)
		{
		User::LeaveIfError(aIDProvider.TSIDManager().GetTSID(iMuxedTSIDs[ERecovery]));

		addr.SetSession(ERecovery);
		iRecoveryBinding.iChannel = iProtocol.GetTransportChannel(addr, ETrue);
		User::LeaveIfNull(iRecoveryBinding.iChannel);
		helper.SetRecoveryCID(iRecoveryBinding.iChannel->TCID());
		helper.SetRecoverySID(iMuxedTSIDs[ERecovery].TSID());
		}
	
	CleanupStack::Pop(mux);
	return mux;
	}
	
void CAVStream::GetTSID(RTSID& aTSID, TAvdtpTransportSessionType aSessionType)
	{
	LOG_FUNC
	aTSID.Acquire(iMuxedTSIDs[aSessionType]);
	}
	
void CAVStream::Release()
	{
	LOG_FUNC
	iState->Release(*this);
	}

void CAVStream::SetConfigurationL(RBuf8& aPacketBuffer,
								  CSignallingChannel& aSignallingChannel,
								  TBool aReportingConfigured,
								  TBool aRecoveryConfigured)
	{
	LOG_FUNC
	iState->SetConfigurationL(*this, aPacketBuffer, aSignallingChannel,
								aReportingConfigured, aRecoveryConfigured);
	}
	
void CAVStream::SetConfigConfirm(TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory)
	{
	LOG_FUNC
	iState->SetConfigConfirm(*this, aResult, aRemoteSEID, aFailedCategory);
	}

void CAVStream::ReconfigConfirm(TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory)
	{
	LOG_FUNC
	iState->ReconfigConfirm(*this, aResult, aRemoteSEID, aFailedCategory);
	}

void CAVStream::StartConfirm(TInt aResult, TSEID aRemoteSEID)
	{
	LOG_FUNC
	iState->StartConfirm(*this, aResult, aRemoteSEID);
	}
	
void CAVStream::SuspendConfirm(TInt aResult, TSEID aRemoteSEID)
	{
	LOG_FUNC
	iState->SuspendConfirm(*this, aResult, aRemoteSEID);
	}
	
TInt CAVStream::Start()
	{
	LOG_FUNC
	return iState->Start(*this);
	}
		
TInt CAVStream::Suspend()
	{
	LOG_FUNC
	return iState->Suspend(*this);
	}

void CAVStream::TryToAndThenPreventHostEncryptionKeyRefresh()
	{
	LOG_FUNC
	if (iTryToAndThenPreventHostEncryptionKeyRefreshToken)
		{
		LOG(_L("Already notified physical link of stream starting"));
		}
	else
		{
		iProtocol.ControlPlane().TryToAndThenPreventHostEncryptionKeyRefresh(DeviceAddress(), iTryToAndThenPreventHostEncryptionKeyRefreshToken);
		}
	}

void CAVStream::AllowHostEncryptionKeyRefresh()
	{
	LOG_FUNC
	MBluetoothControlPlaneToken::Release(iTryToAndThenPreventHostEncryptionKeyRefreshToken);
	}

void CAVStream::CanMuxFrag(TBool& aCanMux, TBool& aCanFrag) const
	{
	LOG_FUNC
	// we know our SEPs have this capability...so just check remote
	aCanFrag = EFalse;
#ifdef __SYMBIAN_AVDTP_HIDE_MUX
	aCanMux = EFalse;
#else
	aCanMux = iProtocol.RemoteSEPCache().HasCapability(iRemoteAddress.BTAddr(), RemoteSEID(), EServiceCategoryMultiplexing);
#endif
	}

TBool CAVStream::CheckConfigured(TAvdtpServiceCategory aCategory) const
	{
	LOG_FUNC
	return iLocalSEP->Configuration()[aCategory] ? ETrue : EFalse;
	}

void CAVStream::BindLogicalAndTransportChannels(TLogicalChannelFactoryTicket aTicket, TInt aError)
	{
	LOG_FUNC
	// a record into which to claim the resulting bearers, one at a time
	TLogicalChannelRecord rec;
	
	// check we're muxing
	TBool mux = iLocalSEP->Configuration()[EServiceCategoryMultiplexing]!=NULL;
	
	TLogicalChannelFactoryTicketInspector inspector(aTicket, !!iReportingBinding.iChannel, !!iRecoveryBinding.iChannel, mux);
	
	if (aError==KErrNone)
		{
		rec = inspector.GetLogicalChannel(EMedia);
		}
		
	// bind the possible logical channel into the transport channel for media
	iMediaBinding.iChannel->LogicalChannelComplete(rec, aError);
	rec.Reset();
	
	if (iReportingBinding.iChannel)
		{
		if (aError==KErrNone)
			{
			rec = inspector.GetLogicalChannel(EReporting);
			}
		iReportingBinding.iChannel->LogicalChannelComplete(rec, aError);
		}
		
	rec.Reset();
		
	if (iRecoveryBinding.iChannel)
		{
		if (aError==KErrNone)
			{
			rec = inspector.GetLogicalChannel(ERecovery);
			}
		iRecoveryBinding.iChannel->LogicalChannelComplete(rec, aError);
		}
	}		

/**
A start indication has been received from the remove.
*/
TInt CAVStream::StartIndication(TAvdtpTransactionLabel aLabel, TSEID aSeid)
	{
	// Take a copy of the current start info.  This means that if the 
	// state machine synchronously calls back it will find the correct
	// details, but if it doesn't accept this start indication we
	// can to restore the old info.
	TStartDetails oldDetails = iStartDetails;
	
	iStartDetails.iLabel = aLabel;
	iStartDetails.iSeid = aSeid;
	
	TInt ret = iState->StartReceived(*this);

	if(ret != KErrNone)
		{
		// restore old start info
		iStartDetails = oldDetails;
		}

	return ret;
	}

TBool CAVStream::IsHostEncryptionKeyRefreshPrevented()
	{
	if(iTryToAndThenPreventHostEncryptionKeyRefreshToken)
		return ETrue;
	else
		return EFalse;
	}

/** This is a delayed start indication issued from the state machine.
We now need to pass this on the sig session.
*/
void CAVStream::ReadyForStartIndication()
	{
	LOG_FUNC

	iStreamNotify.StreamReadyForStartIndication(iStartDetails.iLabel, iStartDetails.iSeid);	
	}
	
CWatchdogTimer* CWatchdogTimer::NewL(CAVStream& aObserver)
	{
	LOG_STATIC_FUNC
	CWatchdogTimer* dog = new(ELeave) CWatchdogTimer(aObserver);
	CleanupStack::PushL(dog);
	dog->ConstructL();
	CleanupStack::Pop();
	return dog;
	}
	
void CWatchdogTimer::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	}
	
CWatchdogTimer::CWatchdogTimer(CAVStream& aObserver)
: CTimer(EPriorityStandard), iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}
	
void CWatchdogTimer::RunL()
	{
	LOG_FUNC
	iObserver.WatchdogFired();
	}

