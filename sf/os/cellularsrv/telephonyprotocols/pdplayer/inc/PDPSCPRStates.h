// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// PDPSCPR States
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPSCPRSTATES_H
#define SYMBIAN_PDPSCPRSTATES_H

#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>
#include "PDPMessageDefn.h"
#include "PDPSCPR.h"

//-=========================================================
//
//
//
// States
//
//
//
//-=========================================================
namespace PDPSCprStates
{
const TInt KFakeStart = 1;
const TInt KBlocked = 2;
const TInt KUnblocked = 3;
const TInt KSendErrorRecoveryRequest = 4;
const TInt KContentionTag = 5;
const TInt KUserAuthenticate = 6;

typedef MeshMachine::TNodeContext<CPDPSubConnectionProvider, SCprStates::TContext> TContext;
typedef MeshMachine::TNodeContext<CPDPDefaultSubConnectionProvider, PDPSCprStates::TContext> TDefContext;

//-=========================================================
//Util
//-=========================================================
DECLARE_SMELEMENT_HEADER( TAwaitingPDPFSMMessage, MeshMachine::TState<TContext>, NetStateMachine::MState, PDPSCprStates::TContext )
	TBool Accept(TInt aExtensionId);
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPDPFSMMessage )

DECLARE_SMELEMENT_HEADER( TNoTagOrError, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrError )

DECLARE_SMELEMENT_HEADER( TNoTagOrUserAuthenticateOrAlreadyStarted, MeshMachine::TStateFork<TDefContext>, NetStateMachine::MStateFork, TDefContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrUserAuthenticateOrAlreadyStarted )

DECLARE_SMELEMENT_HEADER( TSendDataClientIdleIfNoSubconnsAndNoClients, MeshMachine::TStateTransition<TDefContext>, NetStateMachine::MStateTransition, TDefContext )
  	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientIdleIfNoSubconnsAndNoClients )

DECLARE_SMELEMENT_HEADER( TNoTagOrSendErrorRecoveryRequestOrError, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrSendErrorRecoveryRequestOrError )

DECLARE_SMELEMENT_HEADER( TNoTagBackwardOrErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagBackwardOrErrorTag )

DECLARE_SMELEMENT_HEADER( TNoTagOrContentionTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrContentionTag )


//-=========================================================
//Provision
//-=========================================================
DECLARE_SMELEMENT_HEADER( TSelfInit, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void SetupProvisionCfgL(ESock::CCommsDatIapView* aIapView);
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSelfInit)

DECLARE_AGGREGATED_TRANSITION2(
   TStoreProvisionAndInitSelf,
   PRStates::TStoreProvision,
   PDPSCprStates::TSelfInit
   )

//-=========================================================
//Creating context
//-=========================================================
class CStartActivity : public MeshMachine::CNodeRetryActivity
    {
public:
    static MeshMachine::CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
    virtual ~CStartActivity();
protected:

	CStartActivity( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

DECLARE_SMELEMENT_HEADER( TCreatePrimaryPDPCtx, MeshMachine::TStateTransition<PDPSCprStates::TDefContext>, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
	virtual void DoL();
    private:
    void SetupSipServerAddrRetrievalL(RPacketContext::TProtocolConfigOptionV2& aPco);
	void SetImsSignallingFlagL(RPacketContext::TProtocolConfigOptionV2& aPco, TBool aImcn);
	TBool IsModeGsmL() const;
	void SetChapInformationL(RPacketContext::TProtocolConfigOptionV2& aPco);
	void CreateChallengeAndResponseForChapL(RPacketContext::TProtocolConfigOptionV2& aPco);
DECLARE_SMELEMENT_FOOTER( TCreatePrimaryPDPCtx)

DECLARE_SMELEMENT_HEADER( TCreateSecondaryOrMbmsPDPCtx, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateSecondaryOrMbmsPDPCtx)

DECLARE_SMELEMENT_HEADER( TOverrideProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TOverrideProvision)

DECLARE_SMELEMENT_HEADER(TSendErrorRecoveryRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TSendErrorRecoveryRequest)

DECLARE_SMELEMENT_HEADER( TAwaitingPDPCtxCreated, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPDPCtxCreated )

DECLARE_SMELEMENT_HEADER( TAwaitingPrimaryPDPCtxCreated, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPrimaryPDPCtxCreated )

DECLARE_AGGREGATED_TRANSITION2(
	TOverrideProvisionAndStartDataClient,
	PDPSCprStates::TOverrideProvision,
	PRStates::TStartDataClients
	)

//-=========================================================
//Activiating context
//-=========================================================
DECLARE_SMELEMENT_HEADER( TActivatePDPContext, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TActivatePDPContext)

DECLARE_SMELEMENT_HEADER( TAwaitingPDPContextActive, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPDPContextActive )

DECLARE_SMELEMENT_HEADER( TModifyActivePDPContext, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TModifyActivePDPContext)

DECLARE_SMELEMENT_HEADER( TAwaitingActivePDPContextModified, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingActivePDPContextModified )

DECLARE_SMELEMENT_HEADER( TGetNegotiatedQoS, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGetNegotiatedQoS)

DECLARE_SMELEMENT_HEADER( TAwaitingNegotiatedQoSRetrieved, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingNegotiatedQoSRetrieved )

DECLARE_SMELEMENT_HEADER( TSendDataClientStarted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientStarted)

DECLARE_SMELEMENT_HEADER( TSendDataClientStopped, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientStopped)

//===========================================================
// Primary GoneDown
//===========================================================
class CPrimaryPDPGoneDownActivity : public MeshMachine::CNodeActivityBase
	{
/*
*/
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CPrimaryPDPGoneDownActivity();
	CPDPSubConnectionProvider* NewDefault();

protected:
	CPrimaryPDPGoneDownActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
private:
    CPDPSubConnectionProvider* iNewDefault;
	Messages::TNodeId iOriginalDataClient;

protected:
    typedef MeshMachine::TNodeContext<CPDPSubConnectionProvider, CoreNetStates::TContext> TContext;

public:
    DECLARE_SMELEMENT_HEADER( TApplyNewDefault, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TApplyNewDefault)

    DECLARE_SMELEMENT_HEADER( TSwitchToNewDefault, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSwitchToNewDefault)

    DECLARE_SMELEMENT_HEADER( TRejoinDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TRejoinDataClient)

	DECLARE_SMELEMENT_HEADER( TStoreOriginalDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TStoreOriginalDataClient)

	DECLARE_SMELEMENT_HEADER( TStopOriginalDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TStopOriginalDataClient)

    DECLARE_SMELEMENT_HEADER( TNoTagOrProviderStopped, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
      	virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrProviderStopped )
	};


//-=========================================================
//Setting Params
//-=========================================================
DECLARE_SMELEMENT_HEADER( TSetQoS, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetQoS)

DECLARE_SMELEMENT_HEADER( TAwaitingQoSSet, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingQoSSet )

DECLARE_SMELEMENT_HEADER( TSetTFT, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetTFT)

DECLARE_SMELEMENT_HEADER( TSetMbmsParameters, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetMbmsParameters)

DECLARE_SMELEMENT_HEADER( TAwaitingTFTSet, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TDefContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingTFTSet )

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_AGGREGATED_TRANSITION2(
	TStoreAndRespondWithCurrentParams,
	PRStates::TStoreAndRespondWithCurrentParams,
	PDPSCprStates::TSetMbmsParameters
	)
#else
DECLARE_AGGREGATED_TRANSITION3(
	TStoreParamsAndRespond,
	SCprStates::TStoreParams,
	SCprStates::TRespondWithCurrentParams,
	PDPSCprStates::TSetMbmsParameters
	)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

//-=========================================================
//Events
//-=========================================================
DECLARE_SMELEMENT_HEADER( TBlockedOrUnblocked, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, PDPSCprStates::TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TBlockedOrUnblocked )

DECLARE_SMELEMENT_HEADER( TMbmsCtxOrPdpCtx, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, PDPSCprStates::TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TMbmsCtxOrPdpCtx )

DECLARE_SMELEMENT_HEADER( TAwaitingParamsChanged, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingParamsChanged )

DECLARE_SMELEMENT_HEADER( TAwaitingContextBlockedOrUnblocked, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingContextBlockedOrUnblocked )

DECLARE_SMELEMENT_HEADER( TForwardContextBlockedOrUnblockedToDC, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TForwardContextBlockedOrUnblockedToDC)


DECLARE_SMELEMENT_HEADER( TSendDataTransferTemporarilyBlocked, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
  	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataTransferTemporarilyBlocked )

DECLARE_SMELEMENT_HEADER( TSendDataTransferUnblocked, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataTransferUnblocked )

DECLARE_AGGREGATED_TRANSITION2(
   TRaiseParamsGrantedAndSendDataClientStarted,
   PDPSCprStates::TFillInGrantedParams,
   PDPSCprStates::TSendDataClientStarted
   )

DECLARE_SMELEMENT_HEADER( TParamsEvent, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
    void FillInEventL(CSubConGenEventParamsGranted& aEvent);
    void FillInGrantedL(RParameterFamily& aFamily, const CSubConGenEventParamsGranted& aEvent);
    void FillInMbmsSessionIdsL(CSubConGenEventParamsGranted& aEvent);
DECLARE_SMELEMENT_FOOTER( TParamsEvent)

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_AGGREGATED_TRANSITION2(
   TRaiseParamsGrantedAndSendApplyResponse,
   PDPSCprStates::TFillInGrantedParams,
   PRStates::TSendApplyResponse
   )
#else
 DECLARE_AGGREGATED_TRANSITION2(
    TRaiseParamsGrantedAndSendApplyResponse,
    PDPSCprStates::TFillInGrantedParams,
	SCprStates::TSendApplyResponse
	)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

DECLARE_SMELEMENT_HEADER( TFillInGrantedParams, PDPSCprStates::TParamsEvent, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TFillInGrantedParams)

DECLARE_SMELEMENT_HEADER( TFillInImsExtParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TFillInImsExtParams)

DECLARE_SMELEMENT_HEADER( TRaiseParamsRejectedL, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRaiseParamsRejectedL)

DECLARE_SMELEMENT_HEADER( TRaiseParamsChanged, PDPSCprStates::TParamsEvent, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRaiseParamsChanged)

DECLARE_SMELEMENT_HEADER( TAwaitingNetworkStatusEvent, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingNetworkStatusEvent )

DECLARE_SMELEMENT_HEADER( TNetworkStatusEventTypeTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNetworkStatusEventTypeTag )

DECLARE_SMELEMENT_HEADER( TSendGoneDown, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendGoneDown)

DECLARE_AGGREGATED_TRANSITION2(
	TFillInGrantedParamsAndImsExtParams,
	PDPSCprStates::TFillInGrantedParams,
	PDPSCprStates::TFillInImsExtParams
	)

//-=========================================================
//Destroying context
//-=========================================================
DECLARE_SMELEMENT_HEADER( TDestroyPDPContext, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDestroyPDPContext)

DECLARE_SMELEMENT_HEADER( TAwaitingPDPContextDestroyed, PDPSCprStates::TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPDPContextDestroyed )

DECLARE_SMELEMENT_HEADER( TAwaitingPDPContextGoneDown, PDPSCprStates::TAwaitingPDPContextDestroyed, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingPDPContextGoneDown )

DECLARE_SMELEMENT_HEADER( TNoTagOrProviderStopped, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrProviderStopped )

DECLARE_SMELEMENT_HEADER( TCleanupFSM, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCleanupFSM)

DECLARE_AGGREGATED_TRANSITION2(
   TCleanupFSMAndDataClients,
   PDPSCprStates::TCleanupFSM,
   PRStates::TDestroyOrphanedDataClients
   )    


//===========================================================
//   Sip Address retrieval
//===========================================================
DECLARE_SMELEMENT_HEADER( TAwaitingIoctlMessage, MeshMachine::TState<TContext>, NetStateMachine::MState, PDPSCprStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingIoctlMessage )

DECLARE_SMELEMENT_HEADER( TRetrieveSipAddr, MeshMachine::TStateTransition<TDefContext>, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRetrieveSipAddr)

//===========================================================
//   Cancel Start or Stop
//===========================================================

DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStopOrCancel, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStopOrCancel )

DECLARE_SMELEMENT_HEADER( TCancelDataClientStartInPDP, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelDataClientStartInPDP )

//===========================================================
//   User Authentication
//===========================================================
DECLARE_SMELEMENT_HEADER(TSendAuthenticate, MeshMachine::TStateTransition<PDPSCprStates::TDefContext>, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TSendAuthenticate)

DECLARE_SMELEMENT_HEADER(TAwaitingAuthenticateComplete, MeshMachine::TState<PDPSCprStates::TDefContext>, NetStateMachine::MState, PDPSCprStates::TDefContext)
    virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingAuthenticateComplete)

}

#endif // SYMBIAN_PDPSCPRSTATES_H
