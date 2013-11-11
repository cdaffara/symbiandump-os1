// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Core PR State
//
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SS_COREPRSTATES_H
#define SYMBIAN_SS_COREPRSTATES_H

#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_mmcommsprov.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/mm_mutexpolicies.h>
#include <elements/mm_states.h>
#include <comms-infras/ss_activities.h>
#include <comms-infras/ss_common.h>

//-=========================================================
//
//Panics
//
//-=========================================================

namespace CorePanics
{
enum
	{
	KPanicNoActivity = 1,
	KPanicNoServiceProvider = 2,
	KPanicNoControlProvider =3,
	KPanicNoDataClient = 4,
	KPanicNoCSR = 5,
	KPanicNoSelectionPreferences = 6,
	KPanicNoConnection = 7,
	KPanicNoAvailabilityProvider = 8,

	KPanicExpectedNoCSR = 9,
	KPanicExpectedNoServiceProvider = 10,
	KPanicClientsStillPresent =11,

	KPanicIncorrectMessage = 12,
	KOverridenProvisionMessage =13,
	KPanicPeerMessage =14,
	KPanicDataClient = 15,

	KPanicUnexpectedExecutionPath =16,
	KPanicExtInterfaceNotSupported =17,
	KPanicInterfaceNotSupported = 18,

	KPanicIncorrectState = 19,
	KPanicPeerMisbehaving = 20,

	KPanicActivity = 21,
	KPanicNoContext = 22,
	KPanicMultipleDefaultDataClients = 23
	};
}

namespace CoreStates
{

typedef MeshMachine::TNodeContext<ESock::ACFMMNodeBase, MeshMachine::TContext> TContext;

//-=========================================================
//
//Core Transition Ids 1000..1999
//
//-=========================================================

const TInt KNoPeer              = 1000;
const TInt KPeerPresent         = 1001;
const TInt KPeerReady           = 1002;
const TInt KPeerNotReady        = 1003;
const TInt KUseExisting         = 1004;
const TInt KLoopTag             = 1005;
const TInt KSetIdle             = 1006;
const TInt KRecoverableErrorTag = 1007;
const TInt KRetry               = 1008;
const TInt KIgnore              = 1009;
const TInt KPropagate           = 1010;
const TInt KCommit	            = 1011;


//-=========================================================
//
//States
//
//-=========================================================

class ASetErrorState
    {
protected:
	IMPORT_C static TBool Accept(MeshMachine::TNodeContextBase& aContext, TBool aSuperAccept);
	};

template<class TSTATE, class TCONTEXT = CoreStates::TContext>
class TSetErrorState  : public TSTATE, public ASetErrorState
    {
public:
	NETSM_TPL_DECLARE_CTR(TSetErrorState, NetStateMachine::MState, TCONTEXT)

    explicit TSetErrorState(TCONTEXT& aContext)
    :	TSTATE(aContext)
        {
        }

	virtual TBool Accept()
    	{
    	return ASetErrorState::Accept(this->iContext, TSTATE::Accept());
    	}
    };

EXPORT_DECLARE_SMELEMENT_HEADER( TNeverAccept,  MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TNeverAccept )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingJoinComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingJoinComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingClientLeave, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingClientLeave )

//-=========================================================
//
//State Forks
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrRecoverableErrorTagOrErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrRecoverableErrorTagOrErrorTag )

EXPORT_DECLARE_SMELEMENT_HEADER( TPeerPresent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TPeerPresent )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoPeer, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNoPeer )

EXPORT_DECLARE_SMELEMENT_HEADER( TRetryOrIgnoreOrPropagate, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TRetryOrIgnoreOrPropagate )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoClients, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNoClients )

class ACancelOrErrorOrTag
    {
protected:
	IMPORT_C static TInt TransitionTag(MeshMachine::TNodeContextBase& aContext, TInt aTag);
	};

template<TInt TAG>
class TCancelOrErrorOrTag : public MeshMachine::TStateFork<TContext>, public ACancelOrErrorOrTag
    {
public:
	NETSM_TPL_DECLARE_CTR(TCancelOrErrorOrTag, NetStateMachine::MStateFork, TContext)

    TCancelOrErrorOrTag(TContext& aContext) :
        MeshMachine::TStateFork<TContext>(aContext)
        {
        }

	virtual TInt TransitionTag()
    	{
    	return ACancelOrErrorOrTag::TransitionTag(this->iContext, TAG);
    	}
    };

//-=========================================================
//
//Transitions
//
//-=========================================================

//-=========================================================
// CCFActivityBase::SetError handling transitions and states
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TPanic, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPanic )

EXPORT_DECLARE_SMELEMENT_HEADER( TPostToOriginators, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPostToOriginators )

EXPORT_DECLARE_SMELEMENT_HEADER( TSetIdleIfMoreClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetIdleIfMoreClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TAbortAllActivities, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAbortAllActivities )

EXPORT_DECLARE_SMELEMENT_HEADER( TAbortAllActivitiesNodeDeletion, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAbortAllActivitiesNodeDeletion )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagBackwardsOrCancel, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagBackwardsOrCancel )

} //namespace CoreStates


namespace CoreNetStates
{
typedef CoreStates::TContext TContext;

//-=========================================================
//
//Net Core Transition Ids 2000..3999
//
//-=========================================================

const TInt KNoBearer                      = 2000;
const TInt KBearerPresent                 = 2001;
const TInt KBearerReady                   = 2002;
const TInt KNoDataClients                 = 2003;
const TInt KDataClientPresent             = 2004;
const TInt KDataClientReady               = 2005;
const TInt KDataClientNotReady            = 2006;
const TInt KProviderStopped               = 2007;
const TInt KDataClientIdle                = 2008;
const TInt KDataClientNoLongerIdle        = 2009;
const TInt KBearerNoDataClientsToStop     = 2010;
const TInt KNoBearerNoDataClients         = 2011;
const TInt KNonDefault                    = 2012;
const TInt KWaitForIncoming               = 2013;
const TInt KLayerCompleted                = 2014;
const TInt KDataClientsToStop             = 2015;
const TInt KNoDataClientsToStop           = 2016;
const TInt KDataClientsToStart            = 2017;
const TInt KNoDataClientsToStart          = 2018;
const TInt KAlreadyStarted                = 2019;
const TInt KAlreadyStopped                = 2020;
const TInt KBindToComplete                = 2021;
const TInt KNoClients                     = 2022;
const TInt KUnbind                        = 2023;
const TInt KProviderStarted               = 2024;
const TInt KAttachToDefault               = 2025;

//-=========================================================
//
//States
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStart, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStart )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingStart, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStart )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStop, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStop )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingControlClientJoin, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingControlClientJoin )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientJoinRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientJoinRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientJoin, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientJoin )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBinderResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBinderResponse )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBindToComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBindToComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingStarted, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStarted )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingProvision, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingProvision )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingGoneDown, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingGoneDown )

DECLARE_SMELEMENT_HEADER( TAwaitingGoneUp, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingGoneUp )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBindTo, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBindTo )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBindToOrCancel, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBindToOrCancel )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingStopped, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
 	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStopped )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingNoBearer, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingNoBearer )

DECLARE_SMELEMENT_HEADER( TAwaitingCSRCreated, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingCSRCreated )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStarted, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStarted )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientsStarted, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientsStarted )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingRejoinDataClientComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingRejoinDataClientComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientRejoinOrApplyOrCancel, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientRejoinOrApplyOrCancel )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingApplyResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingApplyResponse )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingParamResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingParamResponse )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingSubConEvent, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSubConEvent )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingConEvent, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingConEvent )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientRejoin, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientRejoin )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientIdle, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientIdle )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientActive, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientActive )

//Awaiting one data client stopped
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStopped, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStopped )

//Awaiting all of the data clients stopped
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientsStopped, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientsStopped )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientGoneDown, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientGoneDown )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientGoneDown, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientGoneDown )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBinderRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBinderRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingStop, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStop )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingProviderStatusChangeOrDataClientStatusChange, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingProviderStatusChangeOrDataClientStatusChange )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStatusChange, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStatusChange )

/**
 * State awaiting a message which has derived from TCFSigLegacyRMessage2
 * @publishedPartner
 * @released since tb9.2
 */
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingLegacyRMessage2Ext, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingLegacyRMessage2Ext )

/**
 * State awaiting TLegacyRMessage2Processed message
 * @publishedPartner
 * @released since tb9.2
 */
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingRMessage2Processed, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
    IMPORT_C virtual void Cancel();
DECLARE_SMELEMENT_FOOTER( TAwaitingRMessage2Processed )

//-=========================================================
//
//State Forks
//
//-=========================================================

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByBinderRequest,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityBinderRequest>,
	MeshMachine::TNoTag
	)

DECLARE_SERIALIZABLE_STATE(
	TActiveOrNoTagBlockedByBindTo,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityBindTo>,
	MeshMachine::TActiveOrNoTag<ESock::ECFActivityStartDataClient>
	)

typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityStop, ESock::ECFActivityNoBearer> TActivityStopOrNoBearerMutex;
DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByStopOrNoBearer,
	TActivityStopOrNoBearerMutex,
	MeshMachine::TNoTag
	)

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByStart,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStart>,
	MeshMachine::TNoTag
	)

typedef MeshMachine::TNoClientMutex<Messages::TDefaultClientMatchPolicy, ESock::TCFClientType::EServProvider,ESock::TCFClientType::EStarted> TNoServiceProviderStartedMutex;
DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByNoServiceProviderStarted,
	TNoServiceProviderStartedMutex,
	MeshMachine::TNoTag
	)

typedef MeshMachine::TNoClientMutex<Messages::TDefaultClientMatchPolicy, ESock::TCFClientType::ECtrl|ESock::TCFClientType::EData|ESock::TCFClientType::EAux> TNoClientsMutex;
typedef MeshMachine::TAggregatedMutex_AND<MeshMachine::TActivityIdMutex<ESock::ECFActivityBindTo>, TNoClientsMutex> TBindToAndNoClientsMutex;
DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByBindToAndNoClients,
	TBindToAndNoClientsMutex,
	MeshMachine::TNoTag
	)

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByNoBearer,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityNoBearer>,
	MeshMachine::TNoTag
	)

//[RZ] Please remove this at some point - better use TTag<KBearerPresent>
EXPORT_DECLARE_SMELEMENT_HEADER( TBearerPresent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
     IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TBearerPresent )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrDataClientPresent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrDataClientPresent )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrDataClientsToStop, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrDataClientsToStop )

typedef MeshMachine::TClientMutex<Messages::TDefaultClientMatchPolicy, ESock::TCFClientType::EData,ESock::TCFClientType::EStarting> TDataClientStartingMutex;
DECLARE_SERIALIZABLE_STATE(
	TNoTagOrDataClientsToStopBlockedByStarting,
	TDataClientStartingMutex,
	CoreNetStates::TNoTagOrDataClientsToStop
	)

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoDataClients, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNoDataClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrUnbindOnStop, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrUnbindOnStop )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerPresent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerPresent )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerPresentOrErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerPresentOrErrorTag )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoBearer, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrBearer )

DECLARE_SERIALIZABLE_STATE(
	TNoTagOrNoBearerBlockedByStart,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStart>,
	CoreNetStates::TNoTagOrNoBearer
	)

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNonDefault, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNonDefault )

DECLARE_SERIALIZABLE_STATE(
	TNoTagOrBearerPresentBlockedByStop,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStop>,
	CoreNetStates::TNoTagOrBearerPresent
	)

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagBackwardsOrProviderStopped, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagBackwardsOrProviderStopped )


typedef MeshMachine::TActivityIdMutex<ESock::ECFActivityStart> TActivityStartMutex;
typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityStop, ESock::ECFActivityGoneDown> TActivityStopAndGoneDownMutex;
typedef MeshMachine::TActivityIdMutex<ESock::ECFActivityGoneDown> TActivityGoneDownMutex;

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoDataClientsToStop, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNoDataClientsToStop )

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByStop,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStop>,
	MeshMachine::TNoTag
	)

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByGoneDown,
	TActivityGoneDownMutex,
	MeshMachine::TNoTag
	)


DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByStopOrGoneDown,
	TActivityStopAndGoneDownMutex,
	MeshMachine::TNoTag
	)

DECLARE_SERIALIZABLE_STATE(
	TActiveOrNoTagBlockedByStopOrGoneDown,
	TActivityStopAndGoneDownMutex,
	MeshMachine::TActiveOrNoTag<ESock::ECFActivityStart>
	)

DECLARE_SERIALIZABLE_STATE(
	TActiveOrNoTagBlockedByGoneDown,
	TActivityGoneDownMutex,
	MeshMachine::TActiveOrNoTag<ESock::ECFActivityStart>
	)

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOnEvent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOnEvent )

//-=========================================================
//
//State Transitions
//
//-=========================================================
EXPORT_DECLARE_SMELEMENT_HEADER( TAddAuxClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAddAuxClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TRemoveAuxClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRemoveAuxClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendClientLeavingAndRemoveServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingAndRemoveServiceProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TForwardToControlProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TForwardToControlProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TRequestCommsBinder, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinder )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendControlClientJoinRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendControlClientJoinRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendBindTo, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBindTo )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendBearer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBearer )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendApplyRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendApplyRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendClientLeavingAndRemoveControlProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingAndRemoveControlProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendClientLeavingAndRemoveControlProviderIfNoServiceProviders, TSendClientLeavingAndRemoveControlProvider, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingAndRemoveControlProviderIfNoServiceProviders )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendClientLeavingRequestToServiceProviders, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingRequestToServiceProviders )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendClientLeavingToServiceProviders, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingToServiceProviders )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendClientLeavingRequestToServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingRequestToServiceProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TSetIdleIfNoServiceProviders, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetIdleIfNoServiceProviders )

DECLARE_AGGREGATED_TRANSITION2(
    TLeaveControlProviderAndSetIdleIfNoServiceProviders,
    CoreNetStates::TSendClientLeavingAndRemoveControlProviderIfNoServiceProviders,
    CoreNetStates::TSetIdleIfNoServiceProviders
    )

DECLARE_AGGREGATED_TRANSITION2(
    TLeaveServiceProvidersOrSetIdle,
    CoreNetStates::TSendClientLeavingRequestToServiceProviders,
    CoreNetStates::TSetIdleIfNoServiceProviders
    )

//We need these because of silly macro preprocessing..
typedef MeshMachine::TRetryTransition<CoreNetStates::TRequestCommsBinder, ESock::TCFServiceProvider::TCommsBinderRequest> TRequestCommsBinderRetry;

EXPORT_DECLARE_SMELEMENT_HEADER( TAddControlClientAndSendJoinCompleteIfRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAddControlClientAndSendJoinCompleteIfRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAddDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
	IMPORT_C virtual void Error(TInt aError);
DECLARE_SMELEMENT_FOOTER( TAddDataClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TAddDataClientAndRespond, CoreNetStates::TAddDataClient, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAddDataClientAndRespond )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientIdle, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientIdle )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientActive, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientActive )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientIdleIfNoClients, CoreNetStates::TSendDataClientIdle, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientIdleIfNoClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendLeaveCompleteIfRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendLeaveCompleteIfRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendRejoinComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendRejoinComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( THandleDataClientIdle, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( THandleDataClientIdle )

EXPORT_DECLARE_SMELEMENT_HEADER( TStopDataClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
    void StopDataClient(Messages::RNodeInterface& aDataClient, TInt aStopCode);
DECLARE_SMELEMENT_FOOTER( TStopDataClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TStopSelf, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStopSelf )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendNoBearer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendNoBearer )

EXPORT_DECLARE_SMELEMENT_HEADER( TForwardToControlClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TForwardToControlClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TStartServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartServiceProvider )
typedef MeshMachine::TRetryTransition<CoreNetStates::TStartServiceProvider,ESock::TCFServiceProvider::TStart> TStartServiceProviderRetry;

EXPORT_DECLARE_SMELEMENT_HEADER( TSendStarted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStarted )

DECLARE_SMELEMENT_HEADER( TSendGoneUp, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendGoneUp )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendBindToComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBindToComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TBindSelfToPresentBearer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TBindSelfToPresentBearer )

EXPORT_DECLARE_SMELEMENT_HEADER( TBindSelf, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TBindSelf )

EXPORT_DECLARE_SMELEMENT_HEADER( TStartSelf, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartSelf )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendStop, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStop )

EXPORT_DECLARE_SMELEMENT_HEADER( TCancelDataClientStart, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelDataClientStart )

EXPORT_DECLARE_SMELEMENT_HEADER( TCancelAndCloseZone0ClientExtIfaces, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelAndCloseZone0ClientExtIfaces )

EXPORT_DECLARE_SMELEMENT_HEADER( TProcessOrForwardRMessage2Ext, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessOrForwardRMessage2Ext )

/**
 * Check if ECFActivityStart is running, and if so cancel it
 */
EXPORT_DECLARE_SMELEMENT_HEADER( TCancelStart, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelStart )

DECLARE_AGGREGATED_TRANSITION2(
	TCancelStartAndStopSelf,
    CoreNetStates::TCancelStart,
    CoreNetStates::TStopSelf
    )

EXPORT_DECLARE_SMELEMENT_HEADER( TPassPlaneEventToControlClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPassPlaneEventToControlClients )
} //namespace CoreNetStates



namespace PRStates
{
typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;

//-=========================================================
//
//Core Provider Transition Ids 4000..5999
//
//-=========================================================
const TInt KParamsPresent                      = 4000;
const TInt KOrphans                            = 4001;
const TInt KContinue                           = 4002;

//-=========================================================
//
//States
//
//-=========================================================
EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingParamRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingParamRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingApplyRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
 	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingApplyRequest )

//-=========================================================
//
//State Forks
//
//-=========================================================

DECLARE_SMELEMENT_HEADER( TOrphansOrNoTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
    virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TOrphansOrNoTag )

DECLARE_SMELEMENT_HEADER( TOrphansBackwardsOrNoTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
    virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TOrphansBackwardsOrNoTag )

DECLARE_SMELEMENT_HEADER( TNoTagBackwardsOrNoClients, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
    virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagBackwardsOrNoClients )

DECLARE_SMELEMENT_HEADER( TNonLeavingNoTagOrNoClients, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
    virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNonLeavingNoTagOrNoClients )

//-=========================================================
//
//State Transitions
//
//-=========================================================

EXPORT_DECLARE_SMELEMENT_HEADER( TForwardStateChange, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TForwardStateChange )



EXPORT_DECLARE_SMELEMENT_HEADER( TCreateDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void IssuePeerCreationRequestL(ESock::TCFServiceProvider::TCommsBinderRequest* aBinderRequestMessage = NULL)	;
	void RequestMetaConnPeer(const ESock::RMetaExtensionContainerC& aAccessPointCfg);
	void RequestConnPeer(const ESock::RMetaExtensionContainerC& aAccessPointCfg);
	void RequestSubConnPeer(const ESock::RMetaExtensionContainerC& aAccessPointCfg, ESock::TCFServiceProvider::TCommsBinderRequest* aBinderRequestMessage);
	void RequestDataPeer(const ESock::RMetaExtensionContainerC& aAccessPointCfg, ESock::TCFServiceProvider::TCommsBinderRequest* aBinderRequestMessage);
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateDataClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TDestroyOrphanedDataClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
 	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDestroyOrphanedDataClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TStoreProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
 	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreProvision )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
 	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendProvision )

DECLARE_AGGREGATED_TRANSITION2(
	TProcessDataClientCreation,
	CoreNetStates::TAddDataClient,
	PRStates::TSendProvision
	)

DECLARE_AGGREGATED_TRANSITION3(
	TRemoveClientAndSendLeaveCompleteIfRequest,
	MeshMachine::TRemoveClient,
	PRStates::TDestroyOrphanedDataClients,
	CoreNetStates::TSendLeaveCompleteIfRequest
	)

DECLARE_AGGREGATED_TRANSITION4(
	TProcessClientLeave,
	MeshMachine::TRemoveClient,
	PRStates::TDestroyOrphanedDataClients,
	CoreNetStates::TSendLeaveCompleteIfRequest,
	CoreNetStates::TSendDataClientIdleIfNoClients
	)

DECLARE_AGGREGATED_TRANSITION3(
	TProcessClientLeft,
	MeshMachine::TRemoveClient,
	PRStates::TDestroyOrphanedDataClients,
	CoreNetStates::TSendDataClientIdleIfNoClients
	)

DECLARE_AGGREGATED_TRANSITION4(
	TProcessDestroy,
	CoreStates::TAbortAllActivitiesNodeDeletion,
	CoreNetStates::TSendClientLeavingRequestToServiceProviders,
	CoreNetStates::TSendClientLeavingAndRemoveControlProviderIfNoServiceProviders,
	CoreNetStates::TSetIdleIfNoServiceProviders
	)

EXPORT_DECLARE_SMELEMENT_HEADER( TStartDataClients, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartDataClients )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientStarted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientStarted )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientStopped, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
    TInt iStopCode;
DECLARE_SMELEMENT_FOOTER( TSendDataClientStopped )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendStopped, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStopped )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendStoppedAndGoneDown, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStoppedAndGoneDown )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendGoneDown, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
	TInt iStopCode;
DECLARE_SMELEMENT_FOOTER( TSendGoneDown )

EXPORT_DECLARE_SMELEMENT_HEADER(TProcessDataClientStop, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TProcessDataClientStop)

EXPORT_DECLARE_SMELEMENT_HEADER( TSendDataClientStatusChangeStarted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendDataClientStatusChangeStarted )

EXPORT_DECLARE_SMELEMENT_HEADER( THandleDataClientStatusChange, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( THandleDataClientStatusChange )

DECLARE_AGGREGATED_TRANSITION2(
	THandleDataClientStatusChangeAndDestroyOrphans,
	PRStates::THandleDataClientStatusChange,
	PRStates::TDestroyOrphanedDataClients
	)

EXPORT_DECLARE_SMELEMENT_HEADER( TStoreParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreParams )

EXPORT_DECLARE_SMELEMENT_HEADER( TRespondWithCurrentParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRespondWithCurrentParams )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendApplyResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
   	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendApplyResponse )

EXPORT_DECLARE_SMELEMENT_HEADER( TPassToServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TPassToServiceProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TSendParamsToServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendParamsToServiceProvider )

EXPORT_DECLARE_SMELEMENT_HEADER( TRespondWithRetrievedParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRespondWithRetrievedParams)


DECLARE_AGGREGATED_TRANSITION2(
	TStoreAndRespondWithCurrentParams,
	PRStates::TStoreParams,
	PRStates::TRespondWithCurrentParams
	)

DECLARE_AGGREGATED_TRANSITION2(
	TStoreParamsAndPostToOriginators,
	PRStates::TStoreParams,
	CoreStates::TPostToOriginators
	)

void DestroyFirstClient(const Messages::TClientType& aIncClientType, const Messages::TClientType& aExcClientType = Messages::TClientType::NullType());

DECLARE_SMELEMENT_HEADER( TDestroyFirstOrphan, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDestroyFirstOrphan )

DECLARE_SMELEMENT_HEADER( TDestroyFirstClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDestroyFirstClient )
} //namespace PRStates

#endif //SYMBIAN_SS_COREPRSTATES_H

