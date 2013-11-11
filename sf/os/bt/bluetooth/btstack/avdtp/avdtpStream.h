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
// Defines the avdtp stream.  The stream appears when the SEPs have been configured
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPSTREAM_H
#define AVDTPSTREAM_H

#include <bluetoothav.h>

#include "bt.h"
#include "avdtpTransportSession.h"
#include "avdtpTransportChannel.h"
#include "avdtpLogicalChannelFactory.h"
#include "avdtpServiceInterface.h"
#include "avdtpSEPCache.h"
#include "avdtpAllocators.h"
#include "avdtputil.h"

// when rationalising logging, will not need this to get FLOG_ACTIVE MACRO in this header
#include <comms-infras/commsdebugutility.h>

class CAVStream;
class CSignallingSession;
class CTransportSession;
class CUserPlaneTransportSession;
class CAvdtpSAP;
class CAvdtpProtocol;
class TAVStreamState;
class CAVStreamStateFactory;
class CLocalSEP;
class CWatchdogTimer;

#ifdef _OOM_TEST
const TInt KAvdtpLogicalChannelsWatchdogTimeout = 50000; 
#else
const TInt KAvdtpLogicalChannelsWatchdogTimeout = 30000000; //30s	
#endif

/**
Interface for upcalls from stream to signalling session
*/
class MAvdtpStreamNotify
	{
public:
	virtual void StreamStarted(TSEID aLocalSEID) =0;
	virtual void StreamSuspended(TSEID aLocalSEID) =0;
	virtual void StreamAccepted(TSEID aLocalSEID, TSEID aRemoteSEID, TBool aWithReporting, TBool aWithRecovery) =0;
	virtual void StreamConfigured(TSEID aLocalSEID, TInt aError, TAvdtpServiceCategory aFailedCategory) =0;
	virtual void StreamInitiatedServiceFailed(const CAVStream& aStream, TInt aError) =0;
	virtual void StreamReadyForStartIndication(const TAvdtpTransactionLabel aLabel, const TSEID aSeid) =0;
	};

#ifdef __FLOG_ACTIVE
#define STATENAME(statename) iName = _L(statename);
#else
#define STATENAME(statename)
#endif

NONSHARABLE_CLASS(TPendingStreamDetails)
	{
public:
	TPendingStreamDetails()
		{
		Reset();
		};
		
	void Reset() 
		{
		iLocalSEP = NULL;
		iStreamNotify = NULL;
		iRemoteSEID.Reset();
		};
		
	CLocalSEP* iLocalSEP; //non-owned
	MAvdtpStreamNotify* iStreamNotify; // non-owned
	TSEID iRemoteSEID;
	TBool iRequireReporting;
	TBool iRequireRecovery;
	};

/**
	This class knows which SAPs constitute 1 stream
	SAPs can therefore use this to propagate important matters
	e.g. Coordinating sends to MUX? Flushing?
	
	Instances of this class appear during the signalling phase
	When endpoints have been discovered, the GC will Select a SEP
	which will cause a CAVStream to be instantiated.
	
	The stream then needs to be told of the capabilities that have been selected
	If remote supports MUX then we always choose to use that
	
	Once the stream is told it knows how many sessions will be required.
	
*/

NONSHARABLE_CLASS(CAVStream) : public CBase, public XLogicalChannelFactoryClient,
								public XAvdtpSignalReceiver
	{
friend class CAvdtpProtocol;
friend class CWatchdogTimer;
friend class CSignallingSession;
friend class TAVStreamState;
friend class TAVStreamStateIdle;
friend class TAVStreamStateOpening;
friend class TAVStreamStateReleasing;
friend class TAVStreamStateAborting;
friend class TAVStreamStateCreatingLogicalChannels;
friend class TAVStreamStateWaitingForLogicalChannels;
friend class TAVStreamStateOpen;
friend class TAVStreamStateReady;
friend class TAVStreamStateWaitForSessions;
friend class TAVStreamStateStreaming;
friend class TAVStreamStateConfiguring;
friend class TAVStreamStateConfigured;
friend class TAVStreamStateINTConfigured;
friend class TAVStreamStateACPConfigured;
friend class TAVStreamStateReconfiguring;
friend class TAVStreamStateStarting;
friend class TAVStreamStateSuspending;
friend class TAVStreamStateWaitForSessionsStartReceived;

public:
	static CAVStream* NewL(const TAvdtpSockAddr& aAddr,
					 MAvdtpStreamNotify& aStreamNotify,
					 CAvdtpProtocol& aProtocol,
					 CLocalSEP& aLocalSEP);
	~CAVStream();
	void NotifyUserPlaneTransportSessionsError(CUserPlaneTransportSession* aErroredSession, TInt aError);
	void LocalSEPDestroyed(const CLocalSEP& aTerminator);
	
	TInt AddSession(TAvdtpTransportSessionType aType,
					CUserPlaneTransportSession& aSession,
					CTransportChannel*& aTransportChannel,
					TL2CapConfig::TChannelPriority aPriority = TL2CapConfig::ELow);
	void DropSession(TAvdtpTransportSessionType aType, CUserPlaneTransportSession& aSession);
	void ClearSession(CUserPlaneTransportSession& aSession);

	inline TSEID LocalSEID() const;
	inline TSEID RemoteSEID() const;
	inline TBTDevAddr DeviceAddress() const;
	void CanMuxFrag(TBool& aCanMux, TBool& aCanFrag) const;	
	TBool CheckConfigured(TAvdtpServiceCategory aCategory) const;
	void GetTSID(RTSID& aTSID, TAvdtpTransportSessionType aSessionType);
	TBool MultiplexingConfigured() const;
	
// INT stuff
	void SetConfigurationL(RBuf8& aPacketBuffer,
							CSignallingChannel& aSignallingChannel,
							TBool aReportingConfigured,
							TBool aRecoveryConfigured);
	TInt Start();
	TInt Suspend();
	
// ACP stuff
	void Configured();
	void AwaitLogicalChannelsL();
	void Started();
	void Suspended();
	TBool IsStreaming() const;
	TInt StartIndication(TAvdtpTransactionLabel aLabel, TSEID aSeid);
	TBool IsHostEncryptionKeyRefreshPrevented();
	
private:
	CAVStream(const TAvdtpSockAddr& aAddr,
				MAvdtpStreamNotify& aStreamNotify,
				CAvdtpProtocol& aProtocol,
				CLocalSEP& aLocalSEP);
	void ConstructL();

private:
	void LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket aTicket, TInt aResult);
	void BindLogicalAndTransportChannels(TLogicalChannelFactoryTicket aTicket, TInt aError);
	void TransportChannelsReady();
	TAvdtpMultiplexingCapability* CreateMuxCapabilityL(TBool aRequireReporting, TBool aRequireRecovery, CSignallingChannel& aIDProvider);
	void DeallocateUnclaimedTransportSessionIDs();
// signalling channel callbacks
	virtual void OpenConfirm(TInt aResult, TSEID aSEID);
	virtual void ReleaseConfirm(TInt aResult, TSEID aSEID);
	virtual void StartConfirm(TInt aResult, TSEID aRemoteSEID);
	virtual void SuspendConfirm(TInt aResult, TSEID aRemoteSEID);
	virtual void SetConfigConfirm(TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory);
	virtual void ReconfigConfirm(TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory);
	virtual void AbortConfirm(TSEID aRemoteSEID);
	void ReadyForStartIndication();
	void Release();		// initiate release proceedings, eg SEP has been destroyed
	void Released();	// release proceedings completed
	// the watchdog guards against failed creation/reception of transport channels
	void StartWatchdog();
	void StopWatchdog();
	void WatchdogFired();
	void TryToAndThenPreventHostEncryptionKeyRefresh();
	void AllowHostEncryptionKeyRefresh();
	
private:
	NONSHARABLE_STRUCT(TStartDetails)
		{
	public:
		TAvdtpTransactionLabel 	iLabel;
		TSEID iSeid;
		};

private:
	CAvdtpProtocol&		iProtocol;

	MAvdtpStreamNotify& iStreamNotify;

	TTransportBinding	iMediaBinding;
	TTransportBinding	iReportingBinding;
	TTransportBinding	iRecoveryBinding;
	
	TFixedArray<RTSID, 3>	iMuxedTSIDs; // fixed to make easier to access by sessiontype

	TAvdtpSockAddr		iRemoteAddress;	// devaddr and RemoteSEID
	CLocalSEP*			iLocalSEP;		// NULL if the SEP is destroyed and we need to Release

	TInt				iNumSessionsRequired; // the number that constitute this stream
	// iNumSessions is used to determine when transportchannels should be activated/shutdown
	const TAVStreamState*		iState;		// non-owned

	TLogicalChannelFactoryTicket iFactoryJob;
	
	CWatchdogTimer*		iWatchdogTimer;
	TStartDetails	iStartDetails;
	TThreadPriority iOriginalESockBTPriority;
	TBool			iNotifiedACLStreamActive;
	TDblQueLink			iProtocolQLink;
	//First try to refresh the encrption key and then prevent it from refreshing in future untill
	//we allow it later. This is done so that we do not (or forced by controller) refresh the encryption key 
	//in the middle of streaming audio data. 
	MBluetoothControlPlaneToken*	iTryToAndThenPreventHostEncryptionKeyRefreshToken;
	};
	

NONSHARABLE_CLASS(CWatchdogTimer) : public CTimer
	{
public:
	static CWatchdogTimer* NewL(CAVStream& aObserver);
private:
	CWatchdogTimer(CAVStream& aObserver);
	void RunL();
	void ConstructL();
private:
	CAVStream& iObserver;
	};

NONSHARABLE_CLASS(CAVStreamStateFactory) : public CBase
	{
public:
	static CAVStreamStateFactory* NewL();
	enum TAVStreamStates
		{
		EStreamStateIdle,
		EStreamStateConfiguring,
		EStreamStateINTConfigured,
		EStreamStateACPConfigured,
		EStreamStateOpening,
		EStreamStateWaitForSessions,
		EStreamStateOpen, // AVDTP Open transaction performed
		EStreamStateReady, // all logical channels in place
		EStreamStateReleasing,
		EStreamStateAborting,
		EStreamStateStreaming,
		EStreamStateWaitingForLogicalChannels,
		EStreamStateCreatingLogicalChannels,
		EStreamStateSuspending,
		EStreamStateStarting,
		EStreamStateReconfiguring,
		EStreamStateWaitForSessionsStartReceived,
	// *** keep next one last ***
		EStreamMaxState,
		};
	const TAVStreamState& GetState(TAVStreamStates aState) const;
	TInt StateIndex(const TAVStreamState* aState) const;
	~CAVStreamStateFactory();
private:
	void ConstructL();
	TFixedArray<TAVStreamState*, EStreamMaxState> iStates;
	};
	
NONSHARABLE_CLASS(TAVStreamState)
	{
public:
	virtual void Release(CAVStream& aStream) const;
	virtual void LogicalChannelsAvailable(CAVStream& aStream, TLogicalChannelFactoryTicket aTicket, TInt aErr) const;
	virtual TInt AddSession(CAVStream& aStream, 
							TAvdtpTransportSessionType aType,
							CUserPlaneTransportSession& aSession,
							CTransportChannel*& aTransportChannel,
							TL2CapConfig::TChannelPriority aPriority) const;
	virtual void DropSession(CAVStream& aStream, TAvdtpTransportSessionType aType,
							 CUserPlaneTransportSession& aSession) const;
	
// INT stuff
	virtual void SetConfigurationL(CAVStream& aStream, 
									RBuf8& aPacketBuffer,
									CSignallingChannel& aSignallingChannel,
									TBool aReportingConfigured,
									TBool aRecoveryConfigured) const;

	// forwarded from service_interface
	virtual void SetConfigConfirm(CAVStream& aStream, TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory) const;
	virtual void ReconfigConfirm(CAVStream& aStream, TInt aResult, 
									TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory) const;
	virtual void StartConfirm(CAVStream& aStream, TInt aResult, TSEID aRemoteSEID) const;
	virtual void SuspendConfirm(CAVStream& aStream, TInt aResult, TSEID aRemoteSEID) const;
	virtual void OpenConfirm(CAVStream& aStream, TInt aResult, TSEID aSEID) const;
	virtual void ReleaseConfirm(CAVStream& aStream, TInt aResult, TSEID aSEID) const;
	virtual void AbortConfirm(CAVStream& aStream, TSEID aRemoteSEID) const;
	virtual TInt Start(CAVStream& aStream) const;
	virtual TInt Suspend(CAVStream& aStream) const;

// ACP stuff
	virtual void Configured(CAVStream& aStream) const;
	virtual void AwaitLogicalChannelsL(CAVStream& aStream) const;
	virtual TBool IsStreaming(const CAVStream& aStream) const;
	virtual TInt StartReceived(CAVStream& aStream) const;
	virtual void Started(CAVStream& aStream) const;
	virtual void Suspended(CAVStream& aStream) const;

	virtual void Enter(CAVStream& aStream) const;
	virtual void Exit(CAVStream& aStream) const;
	virtual void WatchdogFired(CAVStream& aStream) const;

protected:
	TAVStreamState(CAVStreamStateFactory& aFactory);
	void ChangeState(CAVStream& aContext, CAVStreamStateFactory::TAVStreamStates aState) const;
	void PanicInState(TAvdtpPanic aPanic) const;
protected:
	CAVStreamStateFactory&	iFactory;
#ifdef __FLOG_ACTIVE
	TBuf<32>				iName;
#endif
	};

/**
State when:
Stream object initially created (either INT or ACP)
Or after configured: reason is that the remote may have got a GetConfig
and been happy with configuration, so no need for it to configure
*/ 
NONSHARABLE_CLASS(TAVStreamStateIdle) : public TAVStreamState
	{
public:
	TAVStreamStateIdle(CAVStreamStateFactory& aFactory);
	
	virtual void SetConfigurationL(CAVStream& aStream, 
							RBuf8& aPacketBuffer,
							CSignallingChannel& aSignallingChannel,
							TBool aReportingConfigured,
							TBool aRecoveryConfigured) const;
	virtual void Configured(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
private:
	};

NONSHARABLE_CLASS(TAVStreamStateConfiguring) : public TAVStreamStateIdle
	{
public:
	TAVStreamStateConfiguring(CAVStreamStateFactory& aFactory);
	virtual void SetConfigConfirm(CAVStream& aStram, TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory) const;
	virtual void Enter(CAVStream& aStream) const;
	};

/*
Abstract state to get common Release event for INT and ACPConfigured states
Since abstract, this isn't added as a state in the state factory
*/
NONSHARABLE_CLASS(TAVStreamStateConfigured) : public TAVStreamStateIdle
	{
public:
	virtual void Release(CAVStream& aStream) const;	
	virtual void Enter(CAVStream& aStream) const;
protected:
	TAVStreamStateConfigured(CAVStreamStateFactory& aFactory);	
	};
	
NONSHARABLE_CLASS(TAVStreamStateINTConfigured) : public TAVStreamStateConfigured
	{
public:
	TAVStreamStateINTConfigured(CAVStreamStateFactory& aFactory);
	virtual TInt AddSession(CAVStream& aStream,
					TAvdtpTransportSessionType aType,
					CUserPlaneTransportSession& aSession,
					CTransportChannel*& aChannel,
					TL2CapConfig::TChannelPriority aPriority) const;
	virtual void DropSession(CAVStream& aStream, TAvdtpTransportSessionType aType,
					 CUserPlaneTransportSession& aSession) const;
	virtual void SetConfigurationL(CAVStream& aStream, 
									RBuf8& aPacketBuffer,
									CSignallingChannel& aSignallingChannel,
									TBool aReportingConfigured,
									TBool aRecoveryConfigured) const;
	virtual void Enter(CAVStream& aStream) const;
private:
	void TryToOpen(CAVStream& aStream) const;
	};

NONSHARABLE_CLASS(TAVStreamStateACPConfigured) : public TAVStreamStateConfigured
	{
public:
	TAVStreamStateACPConfigured(CAVStreamStateFactory& aFactory);
	virtual void AwaitLogicalChannelsL(CAVStream& aStream) const;
	TInt AddSession(CAVStream& aStream, 
							TAvdtpTransportSessionType aType,
							CUserPlaneTransportSession& aSession,
							CTransportChannel*& aTransportChannel,
							TL2CapConfig::TChannelPriority aPriority) const;
	virtual void Enter(CAVStream& aStream) const;
private:
	void TryToOpen(CAVStream& aStream) const;
	void DoAwaitDirectChannelsL(CAVStream& aStream,
								TBool aRequireReporting,
								TBool aRequireRecovery) const;
	void DoAwaitMuxedChannelsL(CAVStream& aStream,
							    TAvdtpMultiplexingCapabilityHelper& aHelper,
								TBool aRequireReporting,
								TBool aRequireRecovery) const;
	};
	
NONSHARABLE_CLASS(TAVStreamStateWaitForSessions) : public TAVStreamStateIdle
	{
public:
	TAVStreamStateWaitForSessions(CAVStreamStateFactory& aFactory);
	
	virtual TInt AddSession(CAVStream& aStream,
					TAvdtpTransportSessionType aType,
					CUserPlaneTransportSession& aSession,
					CTransportChannel*& aChannel,
					TL2CapConfig::TChannelPriority aPriority) const;
				
	virtual TInt StartReceived(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
	};

NONSHARABLE_CLASS(TAVStreamStateWaitForSessionsStartReceived) : public TAVStreamStateWaitForSessions
	{
public:
	TAVStreamStateWaitForSessionsStartReceived(CAVStreamStateFactory& aFactory);
	
	virtual TInt AddSession(CAVStream& aStream,
					TAvdtpTransportSessionType aType,
					CUserPlaneTransportSession& aSession,
					CTransportChannel*& aChannel,
					TL2CapConfig::TChannelPriority aPriority) const;
	
	virtual TInt StartReceived(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
	};
	
NONSHARABLE_CLASS(TAVStreamStateOpening) : public TAVStreamStateIdle
	{
public:
	TAVStreamStateOpening(CAVStreamStateFactory& aFactory);
	virtual void OpenConfirm(CAVStream& aStream, TInt aResult, TSEID aRemoteSEID) const;
	virtual void DropSession(CAVStream& aStream, TAvdtpTransportSessionType aType, CUserPlaneTransportSession& aSession) const;
	virtual void Enter(CAVStream& aStream) const;
	};
	
	
NONSHARABLE_CLASS(TAVStreamStateOpen) : public TAVStreamStateIdle
	{
public:
	TAVStreamStateOpen(CAVStreamStateFactory& aFactory);
	virtual void Release(CAVStream& aStream) const;
	virtual void DropSession(CAVStream& aStream, TAvdtpTransportSessionType aType, CUserPlaneTransportSession& aSession) const;
	virtual void Enter(CAVStream& aStream) const;
private:
	void DoRelease(CAVStream& aStream) const;	
	};

/**
Ready state is 'GAVDP Open' + Transport channels connected.
This state is required to make up for the big difference between having got an error-free
AVDTP Open response (thus "open") and the successful subsequent transport channel connection, and session attachment
which once complete means we are open, ready for moving to streaming, or reconfiguring
*/
NONSHARABLE_CLASS(TAVStreamStateReady) : public TAVStreamStateOpen
	{
public:
	TAVStreamStateReady(CAVStreamStateFactory& aFactory);
	virtual TInt AddSession(CAVStream& aStream,
			TAvdtpTransportSessionType aType,
			CUserPlaneTransportSession& aSession,
			CTransportChannel*& aChannel,
			TL2CapConfig::TChannelPriority aPriority) const;
	// next pertains to reconfiguration
	void SetConfigurationL(CAVStream& aStream, 
									RBuf8& aPacketBuffer,
									CSignallingChannel& aSignallingChannel,
									TBool aReportingConfigured,
									TBool aRecoveryConfigured) const;
	virtual TInt StartReceived(CAVStream& aStream) const;
	virtual TInt Start(CAVStream& aStream) const;
	virtual void Started(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
	};

/**
Waiting for Logical Channels happens after we have got a AVDTP Open and sent Response
*/
NONSHARABLE_CLASS(TAVStreamStateWaitingForLogicalChannels) : public TAVStreamStateOpen
	{
public:
	TAVStreamStateWaitingForLogicalChannels(CAVStreamStateFactory& aFactory);
	virtual void Enter(CAVStream& aStream) const;
	virtual void LogicalChannelsAvailable(CAVStream& aStream, TLogicalChannelFactoryTicket aTicket, TInt aErr) const;
	virtual void Exit(CAVStream& aStream) const;
	virtual void WatchdogFired(CAVStream& aStream) const;
	virtual void DropSession(CAVStream& aStream, TAvdtpTransportSessionType aType, CUserPlaneTransportSession& aSession) const;
	};
	
/**
Creating Logical Channels happens after we have got a AVDTP Open Response back
*/
NONSHARABLE_CLASS(TAVStreamStateCreatingLogicalChannels) : public TAVStreamStateOpen
	{
public:
	TAVStreamStateCreatingLogicalChannels(CAVStreamStateFactory& aFactory);
	virtual void LogicalChannelsAvailable(CAVStream& aStream, TLogicalChannelFactoryTicket aTicket, TInt aErr) const;
	virtual void Enter(CAVStream& aStream) const;
	};

NONSHARABLE_CLASS(TAVStreamStateReconfiguring) : public TAVStreamStateReady
	{
public:
	TAVStreamStateReconfiguring(CAVStreamStateFactory& aFactory);
	virtual void ReconfigConfirm(CAVStream& aStram, TInt aResult, TSEID aRemoteSEID, TAvdtpServiceCategory aFailedCategory) const;
	virtual void Enter(CAVStream& aStream) const;
	};

NONSHARABLE_CLASS(TAVStreamStateReleasing) : public TAVStreamStateReady
	{
public:
	TAVStreamStateReleasing(CAVStreamStateFactory& aFactory);
	virtual void Release(CAVStream& aStream) const;
	virtual void ReleaseConfirm(CAVStream& aStream, TInt aResult, TSEID aSEID) const;
	virtual void DropSession(CAVStream& aStream, TAvdtpTransportSessionType aType, CUserPlaneTransportSession& aSession) const;
	virtual void Enter(CAVStream& aStream) const;
	};

/*
Rarely entered state - required to gracefully release a Configured, but UnOpened, stream
**/
NONSHARABLE_CLASS(TAVStreamStateAborting) : public TAVStreamStateReady
	{
public:
	TAVStreamStateAborting(CAVStreamStateFactory& aFactory);
	virtual void Release(CAVStream& aStream) const;
	virtual void AbortConfirm(CAVStream& aStream, TSEID aRemoteSEID) const;
	virtual void Enter(CAVStream& aStream) const;
	};

	
NONSHARABLE_CLASS(TAVStreamStateStarting) : public TAVStreamStateReady
	{
public:
	TAVStreamStateStarting(CAVStreamStateFactory& aFactory);
	virtual void StartConfirm(CAVStream& aStream, TInt aResult, TSEID aSEID) const;
	virtual TInt StartReceived(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
	};
	
NONSHARABLE_CLASS(TAVStreamStateStreaming) : public TAVStreamStateReady
	{
public:
	TAVStreamStateStreaming(CAVStreamStateFactory& aFactory);
	virtual TInt Suspend(CAVStream& aStream) const;
	virtual TBool IsStreaming(const CAVStream& aStream) const;
	virtual void Suspended(CAVStream& aStream) const;
	virtual TInt Start(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
	virtual void Exit(CAVStream& aStream) const;
	virtual TInt StartReceived(CAVStream& aStream) const;
	};

NONSHARABLE_CLASS(TAVStreamStateSuspending) : public TAVStreamStateReady
	{
public:
	TAVStreamStateSuspending(CAVStreamStateFactory& aFactory);
	virtual void SuspendConfirm(CAVStream& aStream, TInt aResult, TSEID aRemoteSEID) const;
	virtual void Suspended(CAVStream& aStream) const;
	virtual void Enter(CAVStream& aStream) const;
	};

inline TBTDevAddr CAVStream::DeviceAddress() const
	{
	return iRemoteAddress.BTAddr();
	}
	
inline TSEID CAVStream::RemoteSEID() const
	{
#ifdef _DEBUG
	TSEID seid = iRemoteAddress.SEID();
	__ASSERT_DEBUG(!seid.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	return seid;
#else
	return iRemoteAddress.SEID();
#endif
	}

#include "avdtpLocalSEP.h"
		
inline TSEID CAVStream::LocalSEID() const
	{
#ifdef _DEBUG
	TSEID seid = iLocalSEP->SEID();
	__ASSERT_DEBUG(seid.IsLocal(), Panic(EAvdtpSEIDHasWrongDomain));
	return seid;
#else
	return iLocalSEP->SEID();
#endif
	}

#endif //AVDTPSTREAM_H
