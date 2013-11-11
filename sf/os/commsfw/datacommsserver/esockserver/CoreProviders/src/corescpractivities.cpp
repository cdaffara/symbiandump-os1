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
// Core SCpr Activities
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/

#include "corescpractivities.h"

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_subconn.h>
#include <comms-infras/ss_nodemessages_rejoiningprovider.h>

using namespace ESock;
using namespace SCprActivities;
using namespace NetStateMachine;
using namespace PRActivities;
using namespace CorePanics;
using namespace MeshMachine;


namespace SCprNoBearerActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityNoBearer, SCprNoBearer, TCFControlProvider::TNoBearer, PRActivities::CNoBearer::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, CNoBearer::TNoTagOrBearerPresentBlockedByNoBearer)
	// IF (not already bound):
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendNoBearer, MeshMachine::TAwaitingMessageState<TCFControlProvider::TBearer>, MeshMachine::TNoTagOrErrorTag)

	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRActivities::CNoBearer::TRequestCommsBinderRetry, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TTag<CoreNetStates::KBearerPresent>)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TSendBindTo, CoreNetStates::TAwaitingBindToComplete, MeshMachine::TTag<CoreNetStates::KBearerPresent>)
	THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)

	//Flow asking for a bearer needs to be started (provided KErrNone)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, PRActivities::CNoBearer::TNoTagOrDataClientsToStart)
	NODEACTIVITY_ENTRY(CoreNetStates::KDataClientsToStart, PRActivities::CNoBearer::TStartOriginatingDataClient, CoreNetStates::TAwaitingDataClientStarted, MeshMachine::TNoTag)
	
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendDataClientStatusChangeStarted, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer)
    LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SCprParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, SCprSetParams, TCFScpr::TSetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, TTag<CoreNetStates::KBearerPresent>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TRespondWithCurrentParams)
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, SCprSetParams, TCFScpr::TParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, SCprStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, TTag<CoreNetStates::KBearerPresent>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, SCprStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TRespondWithCurrentParams)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

namespace SCprApplyRequest
{
//This activity is a default behaviour for applying the changes previously made to the node.
//The changes could have been:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
// (1) TSetParamsRequest
#else
// (1) TSubConnParamsRequest
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
// (2) TDataClient[Join/Leaving]Request
//The default behaviour is:
// (1) storing the parameters and passing them to the ServiceProvider (if one exists)
// (2) adding/remving dataclients from the dataclients list.
//If this default behaviour is insuficient, the activty needs to be overriden.
DECLARE_DEFINE_NODEACTIVITY(ECFActivityApplyChanges, SCprApplyReq, TCFScpr::TApplyRequest)
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingApplyRequest, CoreNetStates::TNoTagOrNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TPassToServiceProvider, CoreNetStates::TAwaitingApplyResponse, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreStates::TPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, PRStates::TSendApplyResponse)
#else
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingApplyRequest, CoreNetStates::TNoTagOrNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, SCprStates::TPassToServiceProvider, CoreNetStates::TAwaitingApplyResponse, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreStates::TPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, SCprStates::TSendApplyResponse)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

namespace SCprNotification
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNotification, SCprNotif, TCFSubConnControlClient::TSubConnNotification)
	NODEACTIVITY_ENTRY(KNoTag, SCprStates::TPassEventToControlClients, CoreNetStates::TAwaitingSubConEvent, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace SCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, SCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TCreateDataClient, TAcceptErrorState<CoreNetStates::TAwaitingDataClientJoin>, MeshMachine::TNoTagOrErrorTag)
	// If the originator sends TCancel during TAwaitingDataClientJoin, this results in the activity having its error set.
	// The originator is awaiting a TError response, so arrange for TError to be sent by terminating the activity without
	// clearing down the error.
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
	
	// Below this point we need to modify the error handling approach. If we're getting a TError on TBinderResponse,
	// this means the client requesting the binder couldn't bind to it. As far as the client is concerned, this
	// activity is finished (it has flagged an error). The standard error handling will result in erroring
	// the originator. In this case we shouoldn't error the originator, instead, wrap up quietly.
	
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CCommsBinderActivity::TProcessDataClientCreation, MeshMachine::TTag<CoreStates::KUseExisting>)

	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, CCommsBinderActivity::TSendBinderResponse, CCommsBinderActivity::TAwaitingBindToComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TClearError)
NODEACTIVITY_END()
}

namespace SCprRejoinDataClient
{
//The activity accepts a number of TRejoinDataClientRequests - each subsequent TRejoinDataClientRequests must
//come from the same client and must produce ECFActivityNull to be accepted by the same activity. Each
//sequence of TRejoinDataClientRequests must be commited with a TApply before taking effect. To enable
//concurrent (comming from different clients) TRejoinDataClientRequests the activity is parallel.
//The activity will migrate a group of dataclients belonging to this node to new owners. Each item
//in the group (dataclient and new owner) is communicated with TRejoinDataClientRequests. This activity
//is responsible for removing all subjects (migrated dataclients) from this node, and as such must succeed
//on TApply. Any error is detected at any stage before sending TApply to this activity, this activity needs
//to be sent a TCancel or the client requesting must leave this node to cancel this activity.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityRejoin, SCprRejoin, TCFRejoiningProvider::TRejoinDataClientRequest, PRActivities::CRejoinDataClientActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientRejoin, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag,               PRActivities::CRejoinDataClientActivity::TRejoinDataClient, PRActivities::CRejoinDataClientActivity::TAwaitingJoinComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(CoreStates::KLoopTag, PRActivities::CRejoinDataClientActivity::TRejoinDataClient, PRActivities::CRejoinDataClientActivity::TAwaitingJoinComplete, MeshMachine::TNoTag)

	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendRejoinComplete, CoreNetStates::TAwaitingDataClientRejoinOrApplyOrCancel, PRActivities::CRejoinDataClientActivity::TRejoinLoopTag)
	NODEACTIVITY_ENTRY(KNoTag, PRActivities::CRejoinDataClientActivity::TApplyRejoin, MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingApplyResponse>, MeshMachine::TNoTag)
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendApplyResponse)
#else
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TSendApplyResponse)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

namespace SCprDataClientGoneDownActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientGoneDown, SCprDataClientGoneDown, TCFControlProvider::TDataClientGoneDown)
	// First thing, we clear(unset) the "Flags" of the relative DataClient from "EStart":
	//	this is done in "TAwaitingDataClientGoneDown".
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientGoneDown, CoreNetStates::TNoTagOrNonDefault)
	// If it's not the DefaultDataClient, we simply "DoNothing".
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNonDefault, MeshMachine::TDoNothing)

	// The SCpr Stop Itself...
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TNoTag)
	// then send "GoneDown" to the above ControlClient...
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendGoneDown, MeshMachine::TNoTag)
	// and, in the end, send "DataClientGoneDown" to the ControlProvider
	//	to make it aware of what's going on.
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TSendDataClientGoneDown)
NODEACTIVITY_END()
}

namespace SCprActivities
{
DEFINE_EXPORT_ACTIVITY_MAP(coreSCprActivities)
	ACTIVITY_MAP_ENTRY(SCprNoBearerActivity, SCprNoBearer)
#ifndef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	ACTIVITY_MAP_ENTRY(SCprParamsRequest, SCprSetParams)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	ACTIVITY_MAP_ENTRY(SCprNotification, SCprNotif)
	ACTIVITY_MAP_ENTRY(SCprBinderRequestActivity, SCprBinderRequest)
  	ACTIVITY_MAP_ENTRY(SCprApplyRequest, SCprApplyReq)
  	ACTIVITY_MAP_ENTRY(SCprRejoinDataClient, SCprRejoin)
  	ACTIVITY_MAP_ENTRY(SCprDataClientGoneDownActivity, SCprDataClientGoneDown)
ACTIVITY_MAP_END_BASE(PRActivities, coreActivitiesSCpr)
}

