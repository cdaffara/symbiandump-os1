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
// COREPRACTIVITIES.CPP
// Core PR Activities
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
//
//

/**
 @file
 @internalComponent
*/

#define SYMBIAN_NETWORKING_UPS

#include "ss_corepractivities.h"


#include <comms-infras/ss_log.h>
#include "ss_internal_activities.h"
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_mcprnodemessages.h>

#include <comms-infras/ss_protocolparameterset.h>
#include <ss_glob.h>


#include <elements/nm_messages_child.h>
#include <elements/nm_messages_peer.h>
#include <elements/nm_messages_errorrecovery.h>
#include "ss_nodemessages_dataclient.h"
#include "ss_nodemessages_serviceprovider.h"
#include <comms-infras/ss_nodemessages_rejoiningprovider.h>
#include <comms-infras/ss_nodemessages_flow.h>
#include "ss_nodemessages_factory.h"
#include <comms-infras/ss_nodemessages_internal_esock.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrStaCPRAC, "ESockCrStaCPRAC");
#endif

#if defined __CFLOG_ACTIVE || defined ESOCK_EXTLOG_ACTIVE
	#define KCoreProviderStatesTag KESockCoreProviderTag
	_LIT8(KCoreProviderStatesSubTag, "coreprovstate");
#endif

using namespace NetStateMachine;
using namespace CoreStates;
using namespace CoreNetStates;
using namespace PRStates;
using namespace PRActivities;
using namespace CoreActivities;
using namespace ESock;
using namespace CorePanics;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;
using namespace Factories;


#ifdef _DEBUG
_LIT (KCorePrPanic,"CorePrPanic");
#endif

namespace CoreErrorActivity
{ //Special parallel activity, must be started as the last one
DEFINE_EXPORT_CUSTOM_NODEACTIVITY(ECFActivityError, CoreError, TEBase::TError, CErrorActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CErrorActivity::TCFAwaitingError, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CErrorActivity::TSendErrorRecoveryReq, MeshMachine::TAwaitingMessageState<TEErrorRecovery::TErrorRecoveryResponse>, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CErrorActivity::TDoErrorRecovery)
NODEACTIVITY_END()
}

namespace PRProvisionActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStoreProvision, PrProvision, TCFDataClient::TProvisionConfig)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TStoreProvision, CoreNetStates::TAwaitingProvision, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRControlClientJoinActivity
{
DEFINE_EXPORT_NODEACTIVITY(ECFActivityClientJoin, PRControlClientJoin, TNodeSignal::TNullMessageId) //May be waiting for both messages
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TAddControlClientAndSendJoinCompleteIfRequest, CoreNetStates::TAwaitingControlClientJoin, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRDataClientJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL, so no point converting)
DEFINE_EXPORT_NODEACTIVITY(ECFActivityDataClientJoin, PRDataClientJoin, TCFPeer::TJoinRequest)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientJoinRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TAddDataClientAndRespond)
NODEACTIVITY_END()
}


namespace PRDestroyOrphansActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroyOrphans, PRDestroyOrphans, TCFMessage::TDestroyOrphans, CoreActivities::CDestroyOrphansActivity::New)
    // Destroy non-Default Data clients first (as there can be references from non-Default Data clients to the Default Data client)
    FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingMessageState<TCFMessage::TDestroyOrphans>, PRStates::TOrphansOrNoTag)
    NODEACTIVITY_ENTRY(KOrphans, PRStates::TDestroyFirstOrphan, MeshMachine::TAwaitingMessageState<Messages::TEChild::TLeft>, MeshMachine::TTag<KContinue>)
    THROUGH_NODEACTIVITY_ENTRY(KContinue, PRStates::TProcessClientLeft, PRStates::TOrphansBackwardsOrNoTag)
    ROUTING_NODEACTIVITY_ENTRY(KNoTag, CDestroyOrphansActivity::TNoTagOrNoClients)
    LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)

    // At this point, there are no Data or Control clients, so leave the Service Providers (if any) and destroy the node.
    // (Can we just modify the above tuples so that they do not accept the very last TLeft, and thus leave it
    // to MCPrDestroyActivity or PRClientLeftActivity to accept and do the destruction for us?).
    
    // If Control Provider is present, send a TIdle message.
    THROUGH_NODEACTIVITY_ENTRY(KNoClients, CoreNetStates::TSendDataClientIdle, TNoTag)
    // If Control Provider is present, terminate the activity (KNoTag), as
    // Control Provider will send us a TDestroy in response to the TIdle message. 
    ROUTING_NODEACTIVITY_ENTRY(KNoTag, CDestroyOrphansActivity::TControlProviderNoTagOrNoClients)
    LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)

    // No Control Provider - leave Service Providers (if any) and destroy the node.
    ROUTING_NODEACTIVITY_ENTRY(KNoClients, CoreNetStates::TNoTagOrNoBearer)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingRequestToServiceProviders, TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, CDestroyOrphansActivity::TNoTagOrNoTagBackwards)

    ROUTING_NODEACTIVITY_ENTRY(KNoBearer, TNoTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, CDestroyOrphansActivity::TMarkNodeForDestruction)
    // Node will be destroyed in CDestroyOrphansActivity::Destroy() 
NODEACTIVITY_END()
}

namespace PRClientLeaveActivity
{//This activity will wait for ECFActivityBinderRequest to complete
DEFINE_EXPORT_NODEACTIVITY(ECFActivityClientLeave, PRClientLeave, Messages::TEPeer::TLeaveRequest) 
    NODEACTIVITY_ENTRY(KNoTag, PRStates::TProcessClientLeave, MeshMachine::TAwaitingMessageState<TEPeer::TLeaveRequest>, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRClientLeftActivity
{
//This activity waits for TLeft which is a response to destroy, shouldn't really be needed
DEFINE_EXPORT_NODEACTIVITY(ECFActivityClientLeft, PRClientLeft, Messages::TEChild::TLeft)
    NODEACTIVITY_ENTRY(KNoTag, PRStates::TProcessClientLeft, MeshMachine::TAwaitingMessageState<Messages::TEChild::TLeft>, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRDataClientIdleActivity
{
DEFINE_EXPORT_NODEACTIVITY(ECFActivityDataClientIdle, PRDataClientIdle, TCFControlProvider::TIdle)
    NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::THandleDataClientIdle, CoreNetStates::TAwaitingDataClientIdle, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRDataClientActiveActivity
{
DEFINE_EXPORT_NODEACTIVITY(ECFActivityDataClientActive, PRDataClientActive, TCFControlProvider::TActive)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TAwaitingDataClientActive, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRDestroyActivity
{
//The generic Destroy activity. Carries out the node's goodbye handshake.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroy, PRDestroy, TEChild::TDestroy, CoreActivities::CDestroyActivity::New)
	FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingDestroy, CoreActivities::CDestroyActivity::TNoTagBlockedByActivitiesOrLeavingDataClient)

    ROUTING_NODEACTIVITY_ENTRY(KNoTag, PRStates::TNonLeavingNoTagOrNoClients)
    NODEACTIVITY_ENTRY(KNoTag, PRStates::TDestroyFirstClient, MeshMachine::TAwaitingMessageState<Messages::TEChild::TLeft>, MeshMachine::TTag<KContinue>)
    THROUGH_NODEACTIVITY_ENTRY(KContinue, PRStates::TProcessClientLeft, PRStates::TNoTagBackwardsOrNoClients)

 	THROUGH_NODEACTIVITY_ENTRY(KNoClients, PRStates::TProcessDestroy, MeshMachine::TNoTag)
 	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
 	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider)
NODEACTIVITY_END()
}

namespace PRSetParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DEFINE_EXPORT_NODEACTIVITY(ECFActivityParamRequest, PRSetParams, TCFScpr::TSetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, MeshMachine::TTag<CoreNetStates::KBearerPresent>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TStoreAndRespondWithCurrentParams)
NODEACTIVITY_END()
#else
DEFINE_EXPORT_NODEACTIVITY(ECFActivityParamRequest, PRSetParams, TNodeSignal::TNullMessageId)
NODEACTIVITY_END()
#endif
}

// no Store in case of GetParamsRequest
namespace PRGetParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DEFINE_EXPORT_NODEACTIVITY(ECFActivityParamRequest, PRGetParams, TCFScpr::TGetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, MeshMachine::TTag<CoreNetStates::KBearerPresent>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreStates::TPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TRespondWithRetrievedParams)
NODEACTIVITY_END()
#else
DEFINE_EXPORT_NODEACTIVITY(ECFActivityParamRequest, PRGetParams, TNodeSignal::TNullMessageId)
NODEACTIVITY_END()
#endif
}

namespace PRBindToActivity
{
//PRBindToActivity is responsible for handling TCFDataClient::TBindTo;
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBindTo, PRBindTo, TCFDataClient::TBindTo, CBindToActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBindTo, CBindToActivity::TNoTagOrBearerReady)
    //TBindTo can hold:
    //[KNoTag] 		 - a valid serviceProvider cookie that this node isn't bound to;
    //[KBearerReady] - a valid serviceProvider cookie that this node is already bound to;
    //[KBearerReady] - a NULL serviceProvider (this node is at the stack's bottom);

	  //{ JOINING NEW SERVICE PROVIDER
		//a valid serviceProvider supplied, new to this node, let's join it;
	    NODEACTIVITY_ENTRY(KNoTag, CBindToActivity::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, TTag<KBearerReady>)
	  //}

	//serviceProvider provisionally joined. Now the activity needs to propagate iteslf (TBindTo) to its dataclients.
	//The dataclients are either present or not. If not this activity will assume this is the layer construction phase
	//and will attempt to construct a default dataclient.
	THROUGH_NODEACTIVITY_ENTRY(KBearerReady, MeshMachine::TDoNothing, CBindToActivity::TNoTagOrDataClientReady)

	  //{ DATA CLIENT CREATION
		//No dataclients present, assume this is the layer creation phase. Attempt to create a dataclient.
		NODEACTIVITY_ENTRY(KNoTag, CBindToActivity::TCreateDataClient, TAcceptErrorState<CoreNetStates::TAwaitingDataClientJoin>, MeshMachine::TErrorTagOr<CBindToActivity::TNoTagOrBindToComplete>)
	    //BindTo activity is the pre-start layer builder, hence it always requests the dataclient from the factory.
	    //The factory (being aware of the phase) may decide to:
	    //1. create a new dataclient          -> process dataclient creation            [KNoTag]
	    //2. return a preexisting dataclient  -> bind the client                        [KDataClientReady]
	    //3. not to create a dataclient       -> send TBindToComplete to the originator [KBindToComplete]
		THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRStates::TProcessDataClientCreation, TTag<KDataClientReady>)
	  //}

    THROUGH_NODEACTIVITY_ENTRY(KDataClientReady, MeshMachine::TDoNothing, CBindToActivity::TNoTagOrBearerReadyOrBindToComplete)
      //{ BINDING DATACLIENTS LOOP
	    //Dataclient(s) is/are ready. Depending on whether the node has the lower layer or not,
	    //we will [KNoTag] or will not [KNoBearer] need to request a binder for the dataclient.

		  //{SERVICE PROVIDER PRESENT
			NODEACTIVITY_ENTRY(KNoTag, CBindToActivity::TRequestCommsBinder, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, TErrorTagOr<TTag<KBearerReady> >)
	      //}
		NODEACTIVITY_ENTRY(KBearerReady, PRActivities::CBindToActivity::TSendBindTo, CBindToActivity::TAwaitingBindToCompleteOrError,
																					 TErrorTagOr<TTag<KDataClientReady | NetStateMachine::EBackward> >)
	  //}

	//Binding is finished. If this is not autocommit (see TCFDataClient::TBindTo), the activity will reply TCFDataClient::TBindToComplete
	//to the sender await for the confirmation (TCFDataClient::TCommitBindTo) or cancelation (TBase::TCancel) from the sender.
	//If this is autommit, the activity will skip awaiting for TCFDataClient::TCommitBindTo and commit itself.
	THROUGH_NODEACTIVITY_ENTRY(KBindToComplete, CBindToActivity::TSendBindToComplete, CBindToActivity::TNoTagOrCommit)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingMessageState<TCFDataClient::TCommitBindTo>, TErrorTagOr<TTag<KCommit> >)

	//commiting (either implicit or explicit).
	NODEACTIVITY_ENTRY(KCommit, CBindToActivity::TCommit, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)

	//This is not autocommit and the sender has just explicitly cancelled. Alternativelly this is an error path.
	//Cancelling/processing error entiles sending TCancel to all dataclients awaiting confirmation
	//as well as it entiles leaving the new service provider.
	NODEACTIVITY_ENTRY(KErrorTag, CBindToActivity::TCancel, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)

	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace PRStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, PRStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, CoreNetStates::TNoTagOrBearerPresentBlockedByStop)
	NODEACTIVITY_ENTRY(KBearerPresent, CoreNetStates::TBindSelfToPresentBearer, CoreNetStates::TAwaitingBindToComplete, TTag<KBearerPresent>)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendNoBearer, MeshMachine::TAwaitingMessageState<TCFControlProvider::TBearer>, CoreNetStates::TNoTagOrBearerPresentOrErrorTag)

	//Start the service provider, use the default cancellation.
	//Forward TCancel to the service provider, wait for TStarted or TError (via the Error Activity)
	//When TStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(KBearerPresent, CoreNetStates::TStartServiceProviderRetry, CoreNetStates::TAwaitingStarted, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
	//Start data clients, use the default cancellation.
	//Forward TCancel to the self, wait for TCFDataClient::TStarted or TError (via the Error Activity)
	//When TCFDataClient::TStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStartSelf, CoreNetStates::TAwaitingDataClientStarted, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KErrorTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)
NODEACTIVITY_END()
}

namespace PRStopActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStop, PRStop, TCFServiceProvider::TStop, MeshMachine::CNodeRetryActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStop, TActiveOrNoTagBlockedByBindTo)
	THROUGH_NODEACTIVITY_ENTRY(KActiveTag, CoreNetStates::TCancelDataClientStart, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, CoreNetStates::TNoTagOrNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStop, CoreNetStates::TAwaitingStopped, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingRequestToServiceProvider, MeshMachine::TAwaitingLeaveComplete, TTag<CoreNetStates::KNoBearer>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoBearer, PRStates::TSendStoppedAndGoneDown)
NODEACTIVITY_END()
}

namespace PRDataClientStartActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStartDataClient, PRDataClientStart, TCFDataClient::TStart)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientStart, CoreNetStates::TNoTagOrNoDataClients)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TStartDataClients, TAcceptErrorState<CoreNetStates::TAwaitingDataClientsStarted>, MeshMachine::TErrorTagOr<MeshMachine::TTag<CoreNetStates::KNoDataClients> >)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoDataClients, PRStates::TSendDataClientStarted)

	NODEACTIVITY_ENTRY(KErrorTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientsStopped, MeshMachine::TErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TRaiseAndClearActivityError)
NODEACTIVITY_END()
}

namespace PRDataClientStopActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStopDataClient, PRDataClientStop, TCFDataClient::TStop, MeshMachine::CPreallocatedNodeRetryActivity::New)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientStop, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRStates::TProcessDataClientStop, CoreNetStates::TNoTagOrDataClientsToStopBlockedByStarting)

	NODEACTIVITY_ENTRY(CoreNetStates::KDataClientsToStop, CoreNetStates::TStopDataClients, CoreNetStates::TAwaitingDataClientsStopped, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreNetStates::TNoTagOrUnbindOnStop)

	NODEACTIVITY_ENTRY(CoreNetStates::KUnbind, CoreNetStates::TSendClientLeavingRequestToServiceProvider, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	// Note that if CMMCommsProviderBase::DestroyOrphanedDataClients() finds this activity running, it
	// will do nothing and assume that destruction of orphans will be initiated below.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRStates::TDestroyOrphanedDataClients, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendDataClientStopped)
NODEACTIVITY_END()
}


namespace PRForwardStateChangeActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityForwardStateChange, PRForwardStateChange, TCFMessage::TStateChange)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TForwardStateChange, MeshMachine::TAwaitingMessageState<TCFMessage::TStateChange>, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRDataClientStatusChangeActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientStatusChange, PRDataClientStatusChange, TCFControlProvider::TDataClientStatusChange)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::THandleDataClientStatusChangeAndDestroyOrphans, CoreNetStates::TAwaitingDataClientStatusChange, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PRGoneDownActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityGoneDown, PRGoneDown, TCFControlClient::TGoneDown, CGoneDownActivity::New)
	// Our Service Provider has gone down unexpectedly (we haven't issued a TStop)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingGoneDown, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TCancelAndCloseZone0ClientExtIfaces, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingRequestToServiceProvider, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CGoneDownActivity::TSendErrorRecoveryReq, MeshMachine::TAwaitingErrorRecoveryResponseOrError, CoreStates::TRetryOrIgnoreOrPropagate)
	THROUGH_NODEACTIVITY_ENTRY(CoreStates::KRetry, MeshMachine::TDoNothing, CGoneDownActivity::TIgnoreOrPropagate)
	LAST_NODEACTIVITY_ENTRY(CoreStates::KIgnore, MeshMachine::TDoNothing)
	NODEACTIVITY_ENTRY(CoreStates::KPropagate, CoreNetStates::TCancelStartAndStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendGoneDown)
NODEACTIVITY_END()
}


namespace PRGoneUpActivity
{
// This Activity forward the TGoneUp event to the Control Clients nodes that are
// not in the originator lis

DECLARE_DEFINE_NODEACTIVITY(ECFActivityGoneUp, PRGoneUp, TCFControlClient::TGoneUp)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingGoneUp, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendGoneUp)
NODEACTIVITY_END()
}

namespace PRLegacyRMessage2HandlerActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityLegacyRMessage2Handler, PRLegacyRMessage2Handler, TNodeSignal::TNullMessageId, MeshMachine::CNodeParallelMessageStoreActivityBase::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingLegacyRMessage2Ext, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TProcessOrForwardRMessage2Ext, CoreNetStates::TAwaitingRMessage2Processed, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, CoreStates::TPostToOriginators)
NODEACTIVITY_END()
}


namespace CoreActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(coreActivitiesAll)
	ACTIVITY_MAP_ENTRY(CoreErrorActivity, CoreError) //Must be first in the table
ACTIVITY_MAP_END()

//-=========================================================
//
//Error Activity
//
//-=========================================================
MeshMachine::CNodeActivityBase* CErrorActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
    {
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CErrorActivity(aActivitySig, aNode, c);
    }

CErrorActivity::CErrorActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
	:	MeshMachine::CNodeParallelActivityBase(aActivitySig, aNode, aActivitiesCount), iErroredActivityId(MeshMachine::KActivityNull)
	{
	}

CErrorActivity::~CErrorActivity()
	{
	if (Error() != KErrNone)
		{
		CNodeActivityBase* a = iNode.FindActivityById(iErroredActivityId);
		if (a)
			{
			a->SetError(Error());
			a->SetIdle();
			}
    	}
    SetError(KErrNone);
    }

TBool CErrorActivity::IsIdle() const
	{
	return NetStateMachine::ACore::IsIdle();
	}

void CErrorActivity::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& /*aOriginator*/, const TStateTriple& aFirst)
	{
	__ASSERT_DEBUG(IsIdle(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 1));
  	MESH_LOG_ACTIVITY_EXT(KESockMeshMachine, this, &aContext, (_L8("CErrorActivity %08x:\tStartL->starting activity"), this));

	NetStateMachine::ACore::Start(&aContext, aFirst);

    MESH_LOG_ACTIVITY_EXT(KESockMeshMachine, this, &aContext, (_L8("CErrorActivity %08x:\tStartL->activity started"),this));
	}

TBool CErrorActivity::Next(TNodeContextBase& aContext)
	{
	TBool ret = EFalse;


	if (aContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		CNodeActivityBase* a = iNode.FindActivityById(iErroredActivityId);
		//Special handling for TCancel.
		if (a && a->FindOriginator(aContext.iSender) != KErrNotFound)
			{
			ret = ETrue;
			//iActiviy could have handled it, but chose not to.
			Cancel(aContext);
			//consume the message, otherwise the iActivity will get idled and iActivity reference
			//will become invalid.
			aContext.iMessage.ClearMessageId();
			}
		}
	else
		{
		//If we have sent TErrorRecoveryRequest, we had to have set PostedToId() to the recipient (ControlProvider)
		//as otherwise there would be no path for propagating the potential TCancel.
		//The response however (TErrorRecoveryResponse or TError) will not necesserily come from that recipient (CP)
		//(for example it could be coming from the MCpr and we could be the SCpr).
		//Normally this would be a problem because any unrelated TError message arriving to the node
		//and presented to the awaiting state would easily be confused with the response (TError == no recovery on the MCpr == EPropagate).
		//We avoid the problem by checking all arriving  messages if they are adressed to our activity (but we don't
		//check the sender.
	    const TNodeCtxId* recipient = address_cast<const TNodeCtxId>(&aContext.iRecipient);
		if (recipient && (ActivityId() == recipient->NodeCtx()))
			{
			ret = ACore::Next(&aContext);
			if(ret)
				{
		    	MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext, (_L8("CNodeActivityBase %08x:\tNext->transition"), this));
				}
			}
		}
	return ret;
  	}

EXPORT_DEFINE_SMELEMENT(CErrorActivity::TAwaitingError, NetStateMachine::MState, CErrorActivity::TContext)
EXPORT_C TBool CErrorActivity::TAwaitingError::Accept()
    {
	if (! iContext.iMessage.IsMessage<TEBase::TError>())
		{
		return EFalse;
		}

	TEBase::TError& errorMessage = message_cast<TEBase::TError>(iContext.iMessage);

	//Diagnostic panic only. TError message should not be travelling around with KErrNone.
	//If you see this panic, please send a proper error code.
	__ASSERT_DEBUG(errorMessage.iValue != KErrNone, User::Panic(KSpecAssert_ESockCrStaCPRAC, 2));

    MeshMachine::CNodeActivityBase* aa = iContext.Node().FindAddressedActivity(iContext);

    //TError is always a response. If there is no activity addressed by the TError,
    //then we assume the activity hasn't bothered waiting for the result.
    //We hence ignore the error.
    if (aa)
    	{
        aa->SetError(errorMessage.iValue);
    	aa->SetIdle();
    	}
    errorMessage.ClearMessageId();
    return EFalse;
    }

DEFINE_SMELEMENT(CErrorActivity::TCFAwaitingError, NetStateMachine::MState, CErrorActivity::TContext)
EXPORT_C TBool CErrorActivity::TCFAwaitingError::Accept()
    {
	if (! iContext.iMessage.IsMessage<TEBase::TError>())
		{
		return EFalse;
		}

	TEBase::TError& errorMessage = message_cast<TEBase::TError>(iContext.iMessage);

	//Diagnostic panic only. TError message should not be travelling around with KErrNone.
	//If you see this panic, please send a proper error code.
	__ASSERT_DEBUG(errorMessage.iValue != KErrNone, User::Panic(KSpecAssert_ESockCrStaCPRAC, 3));

	RNodeInterface* client = iContext.Node().FindClient(iContext.iSender);
	if (client &&
		client->Type() & TCFClientType::EServProvider &&
		iContext.Node().ControlProvider() != NULL &&
		iContext.Node().CountActivities(ECFActivityDestroy) == 0)
		{
		//this is the only way out into the activity and into error recovery steps
		//that error activity is responsible for doing.
		return ETrue;
		}

    return CErrorActivity::TAwaitingError::Accept();
    }

//Simply leaving from this DoL will NOT have the effect of sending TError to originators
//of the Errored activity! iContext.iNodeActivity is the Error activity.
//Reassign iContext.iNodeActivity before leaving or handle the error.
EXPORT_DEFINE_SMELEMENT(CErrorActivity::TSendErrorRecoveryReq, NetStateMachine::MStateTransition, CErrorActivity::TContext)
EXPORT_C void CErrorActivity::TSendErrorRecoveryReq::DoL()
    {
	//Find matching activity, if any
    MeshMachine::CNodeActivityBase* aa = iContext.Node().FindAddressedActivity(iContext);
    //we are started based on the fact the the last message iContext.Node()'s received is TError
    TEBase::TError& errmsg = message_cast<TEBase::TError>(iContext.iMessage);

	//The error comes from someone else than our Data Client (or we wouldn't be here).
	//It may be our Service Provider or it can be some other node which has originated
	//an activity on us (in which case the errored activity must be present).
    if (aa==NULL)
    	{
        __CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag, _L8("ASendErrorRecoveryReq::DoL - TError but no addressed activity - ignoring")));
#ifdef SYMBIAN_NETWORKING_UPS
		// Terminate the error activity (not UPS specific).
        iContext.iNodeActivity->SetIdle();
#endif //SYMBIAN_NETWORKING_UPS
        return;
    	}

	 AContextStore* intf = NULL;
     if (aa->SupportsExtInterface(AContextStore::KInterfaceId))
    	{
    	//FetchExtInterfaceL below can never leave because it is being checked few lines above in the "if"
	    intf = reinterpret_cast<AContextStore*>(aa->FetchExtInterfaceL(AContextStore::KInterfaceId));
    	}

    //Check if there is any point in sending Error Recovery Request
    if (intf==NULL || !intf->IsStored() || !iContext.Node().ControlProvider())
        {
        __CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag, _L8("ERROR: ASendErrorRecoveryReq::DoL - KErrNotSupported")));
        iContext.iNodeActivity->SetIdle();
        aa->SetError(errmsg.iValue);
    	aa->SetIdle();
    	return;
        }

    //Determine who TErrorRecoveryRequest should be sent to.
    //If there is no ControlProvider we send a RecoveryRequest to ourselves to recover from the error,
    //otherwise we sned the RecoveryRequest up to our ControlProvider.
    //MCPrs typically put all of the error recovery function in a single error recovery activity therefore
    //it makes sense even for MCPrs to send TErrorRecoveryRequest to their error recovery function. By
    //posting a TErrorRecoveryRequest sub-classes of the MCPrs get a chance to override the default error
    //recovery.
    RNodeInterface*  errorRecoverer = iContext.Node().ControlProvider() ? iContext.Node().ControlProvider() : &iContext.Node().SelfInterface();

	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
    CoreActivities::CErrorActivity& activity = static_cast<CoreActivities::CErrorActivity&>(*iContext.iNodeActivity);
    __ASSERT_DEBUG(activity.iErroredActivityId==MeshMachine::KActivityNull, User::Panic(KSpecAssert_ESockCrStaCPRAC, 4));
	// Save a reference to the activity and a point
	// Each time we use the activity, query it from the node, because the activity may have gone away (originator left node etc)
	activity.iErroredActivityId = aa->ActivityId();

    activity.SetError(errmsg.iValue);
    activity.iMessageId = errmsg.iMsgId;

    TErrContext ctx(iContext.NodeId(), errmsg.iMsgId, aa->ActivitySigId(), TStateChange(0, errmsg.iValue));
	TEErrorRecovery::TErrorRecoveryRequest msg(ctx);

    activity.PostRequestTo(
    	*errorRecoverer,//ControlProvider() verified above
    	TCFSafeMessage::TRequestCarrierEast<TEErrorRecovery::TErrorRecoveryRequest>(msg).CRef()
    	);

    //The original activiy might have set 'sent to', but that's surely
    //not meaningful anymore (we've just received a response from that 'sent to').
    //We could have pretended that the orginal activity knows it's sent
    //error recovery to the control provider but it's best just to clear
    //'sent to' (and handle TCancel from here (CErrorActivity::Next()).
    aa->ClearPostedTo();
    }

EXPORT_DEFINE_SMELEMENT(CErrorActivity::TDoErrorRecovery, NetStateMachine::MStateTransition, CErrorActivity::TContext)
EXPORT_C void CErrorActivity::TDoErrorRecovery::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
    CoreActivities::CErrorActivity* act = static_cast<CoreActivities::CErrorActivity*>(iContext.iNodeActivity);
	CNodeActivityBase* a = iContext.Node().FindActivityById(act->iErroredActivityId);
    if (a == NULL)
		{
		iContext.iNodeActivity->SetIdle();
		return;
		}

	if (iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>())
		{
		TErrResponse& resp = message_cast<TEErrorRecovery::TErrorRecoveryResponse>(iContext.iMessage).iErrResponse;
		if (resp.iAction == TErrResponse::ERetry)
			{ //rerun current transition
			__CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag, _L8("ADoErrorRecovery::DoL - instructed to retry")));
			__ASSERT_DEBUG(a->SupportsExtInterface(AContextStore::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));

			AContextStore* intf = reinterpret_cast<AContextStore*>(a->FetchExtInterfaceL(AContextStore::KInterfaceId));
			__ASSERT_DEBUG(intf->IsStored(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 5));
			intf->Retry(*a,iContext);
			if (iContext.iReturn == KErrNone)
				{ //retry succeded
				act->SetError(KErrNone);
				}
			//if Retry had failed, then the d'tor of act will raise the error on
			//and terminate act->iActivity,
			}
		else if (resp.iAction == TErrResponse::EPropagate)
			{ //set new error values and fall through
			__CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag, _L8("ADoErrorRecovery::DoL - instructed to propagate the error")));
			act->iMessageId = resp.iMessageId;
			a->SetError(resp.iError);
			}
		else //if (resp.iAction == TErrResponse::EIgnore)
			{
			__CFLOG_VAR((KCoreProviderStatesTag, KCoreProviderStatesSubTag,
			_L8("WARNING: ADoErrorRecovery::DoL() - instructed to ignore the error!")));
			}
		}
	}


//-=========================================================
//
//Destroy Activity - will delete the node when destructed
//
//-=========================================================

EXPORT_C MeshMachine::CNodeActivityBase* CDestroyActivity::New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CDestroyActivity));
	CDestroyActivity* self = new (space) CDestroyActivity(aActivitySig, aNode);
	self->InsertPreallocatedDestroyActivity(); //Destructing preallocated activity
	return self;
	}

CDestroyActivity::CDestroyActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	CNodeRetryActivity(aActivitySig, aNode),
	APreallocatedOriginators<1>(iOriginators)
	{
    //Mark the provider for deletion, so that it's not served by the factory from now on.
    static_cast<ESock::CMMCommsProviderBase&>(iNode).MarkMeForDeletion();
	}

void CDestroyActivity::Destroy()
	{
	ReturnPreallocatedSpace(this);
	this->~CDestroyActivity(); //Run the destructor

	//Delete the provider.
	static_cast<ESock::CMMCommsProviderBase&>(iNode).DeleteMeNow();
	}

EXPORT_DEFINE_SMELEMENT(CDestroyActivity::TNoTagOrNoTagBackwards, NetStateMachine::MStateFork, PRStates::TContext)
EXPORT_C TInt CDestroyActivity::TNoTagOrNoTagBackwards::TransitionTag()
    {
	if (iContext.iMessage.IsMessage<TEChild::TLeft>())
		{
		TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData|TCFClientType::ECtrl));
		__ASSERT_DEBUG(iter[0], User::Panic(KSpecAssert_ESockCrStaCPRAC, 7)); //One leaving client must still be there.
		return iter[1] == NULL ?  MeshMachine::KNoTag : MeshMachine::KNoTag | NetStateMachine::EBackward;
		}
	else if (iContext.iMessage.IsMessage<TEPeer::TLeaveComplete>())
		{
		__ASSERT_DEBUG(iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl|TCFClientType::EData))==NULL,
		User::Panic(KCorePrPanic, KPanicClientsStillPresent));
		if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider))==NULL)
			{
			return NetStateMachine::EForward | MeshMachine::KNoTag;
			}
		return NetStateMachine::EBackward | MeshMachine::KNoTag; //Loop back to the same triple (& remove the peer)
		}
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCrStaCPRAC, 8));
	return KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(CDestroyActivity::TMakeClientsLeaveOrProcessClientLeave, NetStateMachine::MStateTransition, PRStates::TContext)
EXPORT_C void CDestroyActivity::TMakeClientsLeaveOrProcessClientLeave::DoL()
	{
	if (iContext.iMessage.IsMessage<TEChild::TLeft>())
		{
		ProcessClientLeaveL();
		}
	else
		{
		MakeClientsLeaveL();
		}
	}

void CDestroyActivity::TMakeClientsLeaveOrProcessClientLeave::MakeClientsLeaveL()
    {
    __ASSERT_DEBUG(iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl))==NULL,
    	User::Panic(KCorePrPanic, KPanicClientsStillPresent));
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	iContext.iNodeActivity->ClearPostedTo();

	//MZTODO: this asserion may need to be changed since TDestroy can
	//come while data client is active etc.
    __ASSERT_DEBUG(iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EActive|TCFClientType::EActivating|TCFClientType::EStarting|TCFClientType::EStarted))==NULL,
    	User::Panic(KCorePrPanic, KPanicClientsStillPresent));

	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(),
		TEChild::TDestroy().CRef(),
		TClientType(TCFClientType::EData),
		TClientType(0, TCFClientType::ELeaving|TCFClientType::EDefault),
		TClientType::ELeaving
		);
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(),
		TEChild::TDestroy().CRef(),
		TClientType(TCFClientType::EData),
		TClientType(0, TCFClientType::ELeaving),
		TClientType::ELeaving
		);
	}

void CDestroyActivity::TMakeClientsLeaveOrProcessClientLeave::ProcessClientLeaveL()
    {
    CDestroyActivity::TProcessClientLeave processClientLeave(iContext);
    processClientLeave.DoL();
    }

//-=========================================================
//
// Destroy Orphan Activity - will delete the node when destructed
// if TMarkNodeForDestruction tuple has been called.
//
//-=========================================================

CNodeActivityBase* CDestroyOrphansActivity::New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CDestroyOrphansActivity));
    CDestroyOrphansActivity* self = new (space) CDestroyOrphansActivity(aActivitySig, aNode);
    self->InsertPreallocatedDestroyActivity(); //Destructing preallocated activity
    return self;
    }

CDestroyOrphansActivity::CDestroyOrphansActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
  : MeshMachine::CNodeActivityBase(aActivitySig, aNode),
          MeshMachine::APreallocatedOriginators<1>(iOriginators)
    {
    }

CDestroyOrphansActivity::~CDestroyOrphansActivity() 
    {
    }

void CDestroyOrphansActivity::Destroy() 
    {
    ReturnPreallocatedSpace(this);
    if (DestroyFlag() && iNode.CountActivities(ECFActivityDestroy) == 0)
        {
        static_cast<ESock::CMMCommsProviderBase&>(iNode).MarkMeForDeletion();
        this->~CDestroyOrphansActivity(); //Run the destructor
        static_cast<ESock::CMMCommsProviderBase&>(iNode).DeleteMeNow();
        }
    else
        {
        this->~CDestroyOrphansActivity(); //Run the destructor
        }
    }

void CDestroyOrphansActivity::SetDestroyFlag()
    {
    iDestroyFlag = ETrue;
    }

TBool CDestroyOrphansActivity::DestroyFlag() const
    {
    return iDestroyFlag;
    }

DEFINE_SMELEMENT(CDestroyOrphansActivity::TMarkNodeForDestruction, NetStateMachine::MStateTransition, CDestroyOrphansActivity::TContext)
void CDestroyOrphansActivity::TMarkNodeForDestruction::DoL()
/**
Flag that the node should be destroyed when the activity completes.
*/
    {
    CDestroyOrphansActivity* act = static_cast<CDestroyOrphansActivity*>(iContext.Activity());
    ASSERT(act);
    act->SetDestroyFlag();
    }

// This is a copy from CDestroyActivity.
DEFINE_SMELEMENT(CDestroyOrphansActivity::TNoTagOrNoTagBackwards, NetStateMachine::MStateFork, CDestroyOrphansActivity::TContext)
TInt CDestroyOrphansActivity::TNoTagOrNoTagBackwards::TransitionTag()
    {
    if (iContext.iMessage.IsMessage<TEChild::TLeft>())
        {
        TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData|TCFClientType::ECtrl));
        __ASSERT_DEBUG(iter[0], User::Panic(KSpecAssert_ESockCrStaCPRAC, 7)); //One leaving client must still be there.
        return iter[1] == NULL ?  MeshMachine::KNoTag : MeshMachine::KNoTag | NetStateMachine::EBackward;
        }
    else if (iContext.iMessage.IsMessage<TEPeer::TLeaveComplete>())
        {
        __ASSERT_DEBUG(iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl|TCFClientType::EData))==NULL,
        User::Panic(KCorePrPanic, KPanicClientsStillPresent));
        if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider))==NULL)
            {
            return NetStateMachine::EForward | MeshMachine::KNoTag;
            }
        return NetStateMachine::EBackward | MeshMachine::KNoTag; //Loop back to the same triple (& remove the peer)
        }
    __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCrStaCPRAC, 8));
    return KNoTag;
    }

DEFINE_SMELEMENT(CDestroyOrphansActivity::TNoTagOrNoClients, NetStateMachine::MStateFork, CDestroyOrphansActivity::TContext)
TInt CDestroyOrphansActivity::TNoTagOrNoClients::TransitionTag()
/**
If the Destroy activity is not running and there are no data/control clients, return KNoClients, else return KNoTag.
*/
    {
    if (iContext.Node().CountActivities(ECFActivityDestroy))
        {
        return KNoTag;
        }
    CoreStates::TNoTagOrNoClients fork(iContext);
    return fork.TransitionTag();
    }


DEFINE_SMELEMENT(CDestroyOrphansActivity::TControlProviderNoTagOrNoClients, NetStateMachine::MStateFork, CDestroyOrphansActivity::TContext)
TInt CDestroyOrphansActivity::TControlProviderNoTagOrNoClients::TransitionTag()
/**
If there is a Control Provider, return KNoTag, else return KNoClients
*/
    {
    return iContext.Node().ControlProvider() ? KNoTag : KNoClients;
    }

//-=========================================================
//
//Loppin Activity
//
//-=========================================================
EXPORT_C ACountLoopActivity::~ACountLoopActivity()
    {
    }


EXPORT_C MeshMachine::CNodeActivityBase* CCountLoopActivityBase::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) CCountLoopActivityBase(aActivitySig, aNode);
    }

EXPORT_C CCountLoopActivityBase::CCountLoopActivityBase(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	MeshMachine::CNodeActivityBase(aActivitySig, aNode),
    TIfStaticFetcherNearestInHierarchy(this)
    {
	}

EXPORT_C CCountLoopActivityBase::~CCountLoopActivityBase()
    {
    }

EXPORT_DEFINE_SMELEMENT(ACountLoopActivity::TNoTagOrNoTagBackwards, NetStateMachine::MStateFork, PRStates::TContext)
EXPORT_C TInt ACountLoopActivity::TNoTagOrNoTagBackwards::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(ACountLoopActivity::KInterfaceId),User::Panic(KCorePrPanic, KPanicInterfaceNotSupported));
	ACountLoopActivity* countLoopActivity = static_cast<ACountLoopActivity*>(iContext.iNodeActivity->FetchExtInterface(ACountLoopActivity::KInterfaceId));
 	if (countLoopActivity->DecCount() > 0)
 		{
        return KNoTag | NetStateMachine::EBackward;
     	}
 	return KNoTag | NetStateMachine::EForward;
 	}





//-=========================================================
//
//
//Binding Activity
//
//
//-=========================================================
EXPORT_C ABindingActivity::~ABindingActivity()
	{
    //Handle premature termination of the ABindingActivity object
    //by responding to the originator
	//If the originator has not been replied to yet, reply now with an error code
	if (IsBinding())
		{
		ReplyToOriginator(KErrAbort);
		}
	}

EXPORT_C void ABindingActivity::StoreOriginator(const TRuntimeCtxId& aNodeCtxId)
    {
    //Check if the originator wasn't set before. If it was, it must be replied to before storing ths new one.
    __ASSERT_DEBUG(iOriginator.IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 9));
    iOriginator = aNodeCtxId;
    }

EXPORT_C void ABindingActivity::ReplyToOriginator(TInt aError)
    {
    //NOTE: Please do not make this diagnostic panic conditional.
    //Please allow it to server everyone equally.
    //Please handle the error conditions properly so that you obey this API's semantics.
    //If you are not providing a clean error handling solution for your activity,
    //please use IsBinding() before calling this API!
    __ASSERT_DEBUG(!iOriginator.IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 10)); //The iOriginator must be set.
    
    RClientInterface::OpenPostMessageClose(iOurNode, iOriginator, TCFServiceProvider::TBindToComplete(aError).CRef());

     iOriginator.SetNull();
    }

EXPORT_DEFINE_SMELEMENT(ABindingActivity::TSendBindToComplete, NetStateMachine::MStateTransition, CoreStates::TContext)
EXPORT_C void ABindingActivity::TSendBindToComplete::DoL()
	{
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
    __ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(ABindingActivity::KInterfaceId), User::Panic(KSpecAssert_ESockCrStaCPRAC, 11));
    ABindingActivity* bindingActivity = reinterpret_cast<ABindingActivity*>(iContext.iNodeActivity->FetchExtInterfaceL(ABindingActivity::KInterfaceId));
    bindingActivity->ReplyToOriginator(iContext.iNodeActivity->Error());
	}

void ABindingActivity::FinalReplyToOriginator(TInt aError)
/**
Intended to be called from derived class destructors to arrange for a TBindToComplete reply to be sent
before any other messages in those derived class destructors (for example TDestroy).
*/
	{
	if (IsBinding())
		{
		ReplyToOriginator(aError);
		// Ensure that we don't send another reply in ~ABindingActivity.
		iOriginator = Messages::TNodeCtxId();
		}
	}

} // CoreActivities


namespace PRActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(coreActivitiesPR)
	ACTIVITY_MAP_ENTRY(PRDataClientJoinActivity, PRDataClientJoin)
	ACTIVITY_MAP_ENTRY(PRControlClientJoinActivity, PRControlClientJoin)
	ACTIVITY_MAP_ENTRY(PRClientLeftActivity, PRClientLeft)
    ACTIVITY_MAP_ENTRY(PRClientLeaveActivity, PRClientLeave)
	ACTIVITY_MAP_ENTRY(PRForwardStateChangeActivity, PRForwardStateChange)
	ACTIVITY_MAP_ENTRY(PRBindToActivity, PRBindTo)
	ACTIVITY_MAP_ENTRY(PRDataClientStartActivity, PRDataClientStart)
	ACTIVITY_MAP_ENTRY(PRDataClientStopActivity, PRDataClientStop)
	ACTIVITY_MAP_ENTRY(PRDestroyOrphansActivity, PRDestroyOrphans)
ACTIVITY_MAP_END_BASE(CoreActivities,coreActivitiesAll)

//Activity Map provided by CorePr to be used by SCprs.
//(it is further extended in CoreSCpr).
DEFINE_EXPORT_ACTIVITY_MAP(coreActivitiesSCpr)
	ACTIVITY_MAP_ENTRY(PRProvisionActivity, PrProvision)
    ACTIVITY_MAP_ENTRY(PRStartActivity, PRStart)
    ACTIVITY_MAP_ENTRY(PRStopActivity, PRStop)
	ACTIVITY_MAP_ENTRY(PRDataClientIdleActivity, PRDataClientIdle)
	ACTIVITY_MAP_ENTRY(PRDataClientActiveActivity, PRDataClientActive)
	ACTIVITY_MAP_ENTRY(PRDestroyActivity, PRDestroy)
	ACTIVITY_MAP_ENTRY(PRGoneDownActivity, PRGoneDown)
	ACTIVITY_MAP_ENTRY(PRGoneUpActivity, PRGoneUp) //robertomaro
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	ACTIVITY_MAP_ENTRY(PRSetParamsRequest, PRSetParams)
	ACTIVITY_MAP_ENTRY(PRGetParamsRequest, PRGetParams)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    ACTIVITY_MAP_ENTRY(PRLegacyRMessage2HandlerActivity, PRLegacyRMessage2Handler)
ACTIVITY_MAP_END_BASE(PRActivities,coreActivitiesPR)

//Activity Map provided by CorePr to be used by Cprs.
//(it is further extended in CoreCpr).
DEFINE_EXPORT_ACTIVITY_MAP(coreActivitiesCpr)
	ACTIVITY_MAP_ENTRY(PRProvisionActivity, PrProvision)
    ACTIVITY_MAP_ENTRY(PRStartActivity, PRStart)
    ACTIVITY_MAP_ENTRY(PRStopActivity, PRStop)
	ACTIVITY_MAP_ENTRY(PRDataClientIdleActivity, PRDataClientIdle)
	ACTIVITY_MAP_ENTRY(PRDataClientActiveActivity, PRDataClientActive)
	ACTIVITY_MAP_ENTRY(PRDestroyActivity, PRDestroy)
	ACTIVITY_MAP_ENTRY(PRGoneDownActivity, PRGoneDown)
	ACTIVITY_MAP_ENTRY(PRGoneUpActivity, PRGoneUp) //robertomaro
    ACTIVITY_MAP_ENTRY(PRDataClientStatusChangeActivity, PRDataClientStatusChange)
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    ACTIVITY_MAP_ENTRY(PRSetParamsRequest, PRSetParams)
    ACTIVITY_MAP_ENTRY(PRGetParamsRequest, PRGetParams)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    ACTIVITY_MAP_ENTRY(PRLegacyRMessage2HandlerActivity, PRLegacyRMessage2Handler)
ACTIVITY_MAP_END_BASE(PRActivities,coreActivitiesPR)

//Activity Map provided by CorePr to be used by MCprs.
//(it is further extended in CoreMCpr).
DEFINE_EXPORT_ACTIVITY_MAP(coreActivitiesMCpr)
	ACTIVITY_MAP_ENTRY(PRDataClientIdleActivity, PRDataClientIdle)
	ACTIVITY_MAP_ENTRY(PRDataClientActiveActivity, PRDataClientActive)
    ACTIVITY_MAP_ENTRY(PRDataClientStatusChangeActivity, PRDataClientStatusChange)
    ACTIVITY_MAP_ENTRY(PRLegacyRMessage2HandlerActivity, PRLegacyRMessage2Handler)
    ACTIVITY_MAP_ENTRY(PRDestroyActivity, PRDestroy)
ACTIVITY_MAP_END_BASE(PRActivities,coreActivitiesPR)

//Activity Map provided by CorePr to be used by TMs.
//(it is further extended in CoreTM).
DEFINE_EXPORT_ACTIVITY_MAP(coreActivitiesTM)
ACTIVITY_MAP_END_BASE(PRActivities,coreActivitiesPR)




//-=========================================================
//
//CBindToActivity
//
//-=========================================================
EXPORT_C CBindToActivity::CBindToActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TInt aNextActivityCount)
	:	MeshMachine::CNodeParallelActivityBase(aActivitySig, aNode, aNextActivityCount),
		CoreActivities::ABindingActivity(aNode.Id()),
	    TIfStaticFetcherNearestInHierarchy(this)
		{
		}

EXPORT_C MeshMachine::CNodeActivityBase* CBindToActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
     //there can be only one BindTo activity running at a time.
    TUint c = GetNextActivityCountL(aActivitySig,aNode);
	__ASSERT_DEBUG(c == 1,User::Panic(KCorePrPanic, KPanicPeerMisbehaving));
    if (c > 1)
        {
        User::Leave(KErrInUse);
        }
    return new (ELeave) CBindToActivity(aActivitySig, aNode, c);
    }

EXPORT_C CBindToActivity::~CBindToActivity()
    {
    //If the activity is aborted then no cleanup will be performed (the destroy activity will do
    //that for us) so we should not assert that iSuccessfulDataClients is empty or iNewServiceProvider
    //is null.

    //CBindToActivity::TCommit or CBindToActivity::TCancel should have been executed
    //and rendered iSuccessfulDataClients empty.
	//__ASSERT_DEBUG(iSuccessfulDataClients.Count() == 0, User::Panic(KCorePrPanic, KPanicIncorrectState));

	//CBindToActivity::iNewServiceProvider should have been cleared by CBindToActivity::TCommit or CBindToActivity::TCancel
	//__ASSERT_DEBUG(iNewServiceProvider == NULL, User::Panic(KCorePrPanic, KPanicIncorrectState));

    iSuccessfulDataClients.Close();
    }

Messages::RNodeInterface* CBindToActivity::NextDataClient()
	{
	TClientIter<TDefaultClientMatchPolicy> iter = iNode.GetClientIter<TDefaultClientMatchPolicy>(
												/*include*/TClientType(TCFClientType::EData),
												/*exclude*/TClientType(0, TCFClientType::ELeaving | TCFClientType::EConfigAccessPoint));
    iCurrentDataClient = iter++;
    while (iCurrentDataClient && (iSuccessfulDataClients.Find(iCurrentDataClient) != KErrNotFound))
    	{
        iCurrentDataClient = iter++;
        };
    return iCurrentDataClient;
	}

TBool CBindToActivity::DataClientsAutocommit()
 	{
 	TInt nonLeavingDataClients = iNode.CountClients<TDefaultClientMatchPolicy>(
 								/*include*/TClientType(TCFClientType::EData),
 								/*exclude*/TClientType(0, TCFClientType::ELeaving | TCFClientType::EConfigAccessPoint));
 	return nonLeavingDataClients <=1 && IsAutocommit();
 	}


void CBindToActivity::AddClientAsSuccessfulL(Messages::RNodeInterface* aDataClient)
	{
	__ASSERT_DEBUG(aDataClient, User::Panic(KCorePrPanic, KPanicDataClient));
	__ASSERT_DEBUG(iSuccessfulDataClients.Find(aDataClient) == KErrNotFound, User::Panic(KCorePrPanic, KPanicIncorrectState));
	iSuccessfulDataClients.AppendL(aDataClient);
	}

void CBindToActivity::RemoveClientFromSuccessful(Messages::RNodeInterface* aDataClient)
	{
	TInt index = iSuccessfulDataClients.Find(aDataClient);
	__ASSERT_DEBUG(index >= 0, User::Panic(KCorePrPanic, KPanicDataClient));
	iSuccessfulDataClients.Remove(index);
	}

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TSendControlClientJoinRequest, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TSendControlClientJoinRequest::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
    CBindToActivity& activity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);
	const TCFDataClient::TBindTo& bindToMsg(message_cast<const TCFDataClient::TBindTo>(iContext.iMessage));

	__ASSERT_DEBUG(!bindToMsg.iNodeId.IsNull(), User::Panic(KCorePrPanic, KPanicNoServiceProvider));
    RNodeInterface* newServiceProvider = iContext.Node().AddClientL(bindToMsg.iNodeId,
                            TClientType(TCFClientType::EServProvider, TCFClientType::EActivating));
    __ASSERT_DEBUG(newServiceProvider, User::Panic(KCorePrPanic, KPanicNoServiceProvider));
    activity.iNewServiceProvider = bindToMsg.iNodeId;
    //Join the new service provider
    iContext.Activity()->PostRequestTo(*newServiceProvider,
                            TCFControlClient::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl)).CRef());
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TAwaitingBindToCompleteOrError, NetStateMachine::MState, CRejoinDataClientActivity::TContext)
TBool CBindToActivity::TAwaitingBindToCompleteOrError::Accept()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
    CBindToActivity& activity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);
    if (CoreNetStates::TAwaitingBindToComplete(iContext).Accept())
	    {
	    if (activity.IsBinding())
	    	{
	    	//Activity is binding (i.e.: we havent received TBindToComplete as a response
	    	//to null-carrying TBindTo.
		    activity.ReplyToOriginator(KErrNone);
	    	}
	    activity.ResetCurrentDataClient();
	    return ETrue;
	    }
    else if (iContext.iMessage.IsMessage<TEBase::TError>())
    	{
	    activity.ReplyToOriginator(message_cast<TEBase::TError>(iContext.iMessage).iValue);
    	activity.RemoveClientFromSuccessful(activity.CurrentDataClient());
    	activity.ResetCurrentDataClient();
    	return ETrue;
    	}
	return EFalse;
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TNoTagOrDataClientReady, NetStateMachine::MStateFork, CBindToActivity::TContext)
TInt CBindToActivity::TNoTagOrDataClientReady::TransitionTag()
    {
    //Any non-leaving dataclients already present?
    if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(
    	/*include*/TClientType(TCFClientType::EData),
    	/*exclude*/TClientType(0, TCFClientType::ELeaving | TCFClientType::EConfigAccessPoint)))
	    {
	    return KDataClientReady;
	    }
	return KNoTag;
    }


EXPORT_DEFINE_SMELEMENT(CBindToActivity::TRequestCommsBinder, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TRequestCommsBinder::DoL()
    {
	__ASSERT_DEBUG(iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)),User::Panic(KCorePrPanic, KPanicDataClient));
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
  	CBindToActivity& activity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);

  	//The service provider has been joined already and must be found here
    __ASSERT_DEBUG(!activity.iNewServiceProvider.IsNull(), User::Panic(KCorePrPanic, KPanicNoServiceProvider));
    RNodeInterface* newServiceProvider = iContext.Node().FindClient(activity.iNewServiceProvider);
    __ASSERT_DEBUG(newServiceProvider, User::Panic(KCorePrPanic, KPanicNoServiceProvider));
    //We must not be in this transition if dc was not found
    __ASSERT_DEBUG(activity.CurrentDataClient(), User::Panic(KCorePrPanic, KPanicDataClient));

    // Also if it is not so common to have multiple DataClient (except
    // 	for the relation "SCPR<-CPR"), there are situation where it happens
    // 	and where only one of them is marked as Default.
    // So, we check if this is the case and, in case the
    //	Originator of this activity is NOT a Default one, send ECreateNew.

    // If the Current DC is marked as Default, we ask to the lower layer
    //	to "AttachToDefault". If it is not, we ask "CreateNew".
    TInt subConnOpenType = (activity.CurrentDataClient()->Flags() & TCFClientType::EDefault) ?
   			TSubConnOpen::EAttachToDefault : TSubConnOpen::ECreateNew;

    // --- WORKAROUND START ---
    // [399TODO] There are situation were we don't have any DataClient marked
    //	as Default, but, asking for ECreateNew, the lower layer doesn't
    //	manage the request very well.
    //	This is a WORKAROUND and need fixing (see DEF113154).
    //	In the meanwhile, we count the number of "DefaultDataClient" and,
    //	if the result is "0", we ask the lower layer "AttachToDefault"
    TInt numberOfDefaultDataClient = iContext.Node().CountClients<TDefaultClientMatchPolicy>(
    		TClientType(TCFClientType::EData, TCFClientType::EDefault)
    		);
    if (numberOfDefaultDataClient == 0)
    	{
    	subConnOpenType = TSubConnOpen::EAttachToDefault;
    	}
    // --- WORKAROUND END ---

    // Send "TCommsBinderRequest" to the Current ServiceProvider
   	activity.PostRequestTo(
   			*newServiceProvider,
   			TCFServiceProvider::TCommsBinderRequest(subConnOpenType).CRef()
   			);
    }


EXPORT_DEFINE_SMELEMENT(CBindToActivity::TCreateDataClient, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TCreateDataClient::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));

	IssuePeerCreationRequestL ();
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TSendBindTo, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TSendBindTo::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CBindToActivity& bindToActivity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);

	//Provisionally stash the current dataclient as successful. If it fails to bind, we'll unstash it
	//We're doing this now, as stashing may fail and we don't want that to be the sole reason for
	//unrolling.
	bindToActivity.AddClientAsSuccessfulL(bindToActivity.CurrentDataClient());

	TCFServiceProvider::TCommsBinderResponse* commsBinderResponse = message_cast<TCFServiceProvider::TCommsBinderResponse>(&iContext.iMessage);
	Messages::TNodeId commsBinder = commsBinderResponse ? commsBinderResponse->iNodeId : Messages::TNodeId::NullId();

	bindToActivity.PostRequestTo(*bindToActivity.CurrentDataClient(),
			TCFDataClient::TBindTo(commsBinder, !bindToActivity.DataClientsAutocommit()).CRef());
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TSendBindToComplete, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TSendBindToComplete::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	iContext.iNodeActivity->PostToOriginators(TCFDataClient::TBindToComplete().CRef());
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TNoTagOrCommit, NetStateMachine::MStateFork, CBindToActivity::TContext)
TInt CBindToActivity::TNoTagOrCommit::TransitionTag()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CBindToActivity& bindToActivity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);
	if (bindToActivity.IsAutocommit())
		{
		return KCommit;
		}
	return KNoTag;
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TNoTagOrBindToComplete, NetStateMachine::MStateFork, CBindToActivity::TContext)
EXPORT_C TInt CBindToActivity::TNoTagOrBindToComplete::TransitionTag()
    {
    TCFFactory::TPeerFoundOrCreated& dcJoined = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);

    if (dcJoined.iNodeId.IsNull())
    	{
    	//Factory decided not to create a dataclient;
    	return KBindToComplete;
    	}

    //factory created a new citizen.
    return KNoTag;
    }


EXPORT_DEFINE_SMELEMENT(CBindToActivity::TNoTagOrBearerReady, NetStateMachine::MStateFork, CBindToActivity::TContext)
TInt CBindToActivity::TNoTagOrBearerReady::TransitionTag()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CBindToActivity& bindToActivity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);
    TCFDataClient::TBindTo& bindToReq = message_cast<TCFDataClient::TBindTo>(iContext.iMessage);
    bindToActivity.SetAutocommit(!bindToReq.iValue);

    if (bindToReq.iNodeId.IsNull())
    	//received a null service provider, the node is at the stack's bottom.
        {
        return KBearerReady;
        }

    RNodeInterface* sp = iContext.Node().ServiceProvider();
    if (sp && bindToReq.iNodeId == sp->RecipientId())
        {
        //received the same service provider, it's already bound to.
        bindToActivity.iNewServiceProvider = sp->RecipientId();
		return KBearerReady;
		}
    RNodeInterface* newServiceProvider = iContext.Node().FindClient(bindToReq.iNodeId);
	if (newServiceProvider)
		{
		__ASSERT_DEBUG(newServiceProvider->Type() == TCFClientType::EServProvider, User::Panic(KCorePrPanic, KPanicIncorrectState));
		//Ok, we've received a TBindTo holding a service provider that we already know of and that is not
		//our current service provider. We're going to assume this node tolerates multiple service providers (like MCPRs do).
		//the current service provider will be swapped, but won't be dropped.
		bindToActivity.iNewServiceProvider = bindToReq.iNodeId;
		newServiceProvider->SetFlags(TCFClientType::EActivating);
		bindToActivity.SetDontLeaveServiceProvider();
		return KBearerReady;
		}
	else
	    {
        bindToActivity.iNewServiceProvider = TNodeId::NullId();
	    }
	//The node received a new service provider...
	return KNoTag;
    }


EXPORT_DEFINE_SMELEMENT(CBindToActivity::TNoTagOrBearerReadyOrBindToComplete, NetStateMachine::MStateFork, CBindToActivity::TContext)
TInt CBindToActivity::TNoTagOrBearerReadyOrBindToComplete::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CBindToActivity& bindToActivity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);

    if (!bindToActivity.NextDataClient())
    	{
    	//No more dataclients to bind
    	return KBindToComplete;
    	}

    if (bindToActivity.iNewServiceProvider.IsNull())
    	{
    	//There is no service provider (new or old) below us.
    	return KBearerReady;
    	}

    return KNoTag;
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TCommit, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TCommit::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CBindToActivity& bindToActivity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);

	TInt count = bindToActivity.DataClientsAutocommit() ? 0 : bindToActivity.iSuccessfulDataClients.Count();
    while (count )
    	{
    	bindToActivity.PostRequestTo(*bindToActivity.iSuccessfulDataClients[--count], TCFDataClient::TCommitBindTo().CRef());
        };
    bindToActivity.iSuccessfulDataClients.Reset();
    RNodeInterface* sp = iContext.Node().ServiceProvider();

    if (sp && sp->RecipientId() != bindToActivity.iNewServiceProvider)
    	{
    	if (bindToActivity.ShouldLeaveServiceProvider())
    		{
	    	bindToActivity.PostRequestTo(*sp, TEPeer::TLeaveRequest().CRef());
	    	sp->SetFlags(TCFClientType::ELeaving);
    		}
        else
        	{
        	//Didn't leave, no need for waiting for TLeaveComplete;
        	bindToActivity.SetIdle();
        	}
    	sp->ClearFlags(TCFClientType::EActive);
    	}
    else
    	{
    	//Didn't leave, no need for waiting for TLeaveComplete;
    	bindToActivity.SetIdle();
    	}

    if (!bindToActivity.iNewServiceProvider.IsNull() && (sp == NULL || sp->RecipientId() != bindToActivity.iNewServiceProvider))
        {
        RNodeInterface* newServiceProvider = iContext.Node().FindClient(bindToActivity.iNewServiceProvider);
        if (newServiceProvider)
            {
            __ASSERT_DEBUG(newServiceProvider->Flags() & TCFClientType::EActivating, User::Panic(KCorePrPanic, KPanicIncorrectState));
            newServiceProvider->ClearFlags(TCFClientType::EActivating);
            newServiceProvider->SetFlags(TCFClientType::EActive);
            // Note: iContext.Node().ServiceProvider() must be re-evaluated in the ASSERT below (i.e. don't use any previously cached value).
            __ASSERT_DEBUG(iContext.Node().ServiceProvider() == newServiceProvider, User::Panic(KCorePrPanic, KPanicIncorrectState));
            }
        }
    bindToActivity.iNewServiceProvider = TNodeId::NullId();
    }

EXPORT_DEFINE_SMELEMENT(CBindToActivity::TCancel, NetStateMachine::MStateTransition, CBindToActivity::TContext)
void CBindToActivity::TCancel::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CBindToActivity& bindToActivity = static_cast<CBindToActivity&>(*iContext.iNodeActivity);
	TInt count = bindToActivity.iSuccessfulDataClients.Count();
    while (count )
    	{
    	bindToActivity.PostRequestTo(*bindToActivity.iSuccessfulDataClients[--count], TEBase::TCancel().CRef());
        };
    bindToActivity.iSuccessfulDataClients.Reset();

	TBool setIdle = ETrue;
    if (!bindToActivity.iNewServiceProvider.IsNull())
        {
        RNodeInterface* newServiceProvider = iContext.Node().FindClient(bindToActivity.iNewServiceProvider);
        if (newServiceProvider && newServiceProvider != iContext.Node().ServiceProvider())
            {
            __ASSERT_DEBUG(newServiceProvider->Flags() & TCFClientType::EActivating, User::Panic(KCorePrPanic, KPanicIncorrectState));
            newServiceProvider->ClearFlags(TCFClientType::EActivating);
            if (bindToActivity.ShouldLeaveServiceProvider())
                {
                bindToActivity.PostRequestTo(*newServiceProvider, TEPeer::TLeaveRequest().CRef());
                newServiceProvider->SetFlags(TCFClientType::ELeaving);
                setIdle = EFalse;
                }
            }
        }

    if (setIdle)
        {
        bindToActivity.SetIdle();
        }
    bindToActivity.iNewServiceProvider = TNodeId::NullId();
    }


//-=========================================================
//
//Rejoin DataClient Activity
//
//-=========================================================
EXPORT_C MeshMachine::CNodeActivityBase* CRejoinDataClientActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CRejoinDataClientActivity(aActivitySig, c, aNode);
    }

CRejoinDataClientActivity::~CRejoinDataClientActivity()
    {
    if ( iDataClients.Count() )
        {
        if (Error() != KErrNone)
            {
            //An error has occured when processing rejoin as the activity hasn't been properly wrapped
            //up. This error must have happened before any of the clients have been added to the new
            //owner as such addition must either collectively succeed or collectively fail. Reinstall
            //clients in at the current owner.
            for (TInt i = 0; i < iDataClients.Count(); i++)
                {
                Messages::RNodeInterface& dataClient = iDataClients[i].iDataClient; 
                if (!(dataClient.Flags() & TCFClientType::EActive))
                    {
#ifndef __GCCXML__
                    //If the dataclient managed to report idle in the mean time, have him destroyed
                    RClientInterface::OpenPostMessageClose(iNode.Id(), dataClient.RecipientId(), TEChild::TDestroy().CRef());
                    dataClient.SetFlags(TCFClientType::ELeaving);
#endif
                    }
                dataClient.ClearFlags(TCFClientType::EActivating);
#ifndef __GCCXML__
                //Simulate client leaving on the new owner.
                RClientInterface::OpenPostMessageClose(dataClient.RecipientId(), iDataClients[i].iNewOwner,
                        TEChild::TLeft().CRef());
#endif
                }
            }
        else
            {
            //All clear. Remove the clients for good.
            for (TInt i = 0; i < iDataClients.Count(); i++)
                {
                iNode.RemoveClient(iDataClients[i].iDataClient.RecipientId());
                }
            }
        }
    iDataClients.Reset();
    }


void CRejoinDataClientActivity::TCFDataClientJoiningRequest::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	MeshMachine::AMMNodeBase* nodeBase = reinterpret_cast<MeshMachine::AMMNodeBase*>(nodeId.Node().FetchNodeInterfaceL(AMMNodeBase::KInterfaceId));
    RNodeInterface* client = NULL;
    client = nodeBase->AddClientL(iDataClient, iDataClientType);
    client->SetFlags(TCFClientType::EJoining|TCFClientType::EStarted);
    RClientInterface::OpenPostMessageClose(nodeId, aSender, TCFPeer::TJoinComplete().CRef());
    }


EXPORT_DEFINE_SMELEMENT(CRejoinDataClientActivity::TAwaitingJoinComplete, NetStateMachine::MState, CRejoinDataClientActivity::TContext)
TBool CRejoinDataClientActivity::TAwaitingJoinComplete::Accept()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 13));
	if (iContext.iMessage.IsMessage<TCFPeer::TJoinComplete>())
    	{
    	return ETrue;
    	}
    else if (iContext.iMessage.IsMessage<TEBase::TError>())
        {
	    iContext.iNodeActivity->SetError(message_cast<TEBase::TError>(iContext.iMessage).iValue);
	    iContext.iNodeActivity->SetIdle();
	    iContext.iMessage.ClearMessageId();
        }
	return EFalse;
    }


EXPORT_DEFINE_SMELEMENT(CRejoinDataClientActivity::TRejoinDataClient, NetStateMachine::MStateTransition, CRejoinDataClientActivity::TContext)
EXPORT_C void CRejoinDataClientActivity::TRejoinDataClient::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 14));
	CRejoinDataClientActivity* rejoinActivity = static_cast<CRejoinDataClientActivity*>(iContext.iNodeActivity);

	TCFRejoiningProvider::TRejoinDataClientRequest& rejoinDCMsg = message_cast<TCFRejoiningProvider::TRejoinDataClientRequest>(iContext.iMessage);
	RNodeInterface* subject = iContext.Node().FindClient(rejoinDCMsg.iNodeId1);
	if (NULL == subject //client not found
	    || subject->Flags() & TCFClientType::ELeaving //client leaving/gone
	    || !(subject->Flags() & TCFClientType::EActive)) //client reported idle or never bound to
    	{
    	User::Leave(KErrNotFound);
    	}
    if (subject->Flags() & TCFClientType::EActivating) //client requested by someone
        {
    	User::Leave(KErrInUse);
        }

    rejoinActivity->iDataClients.AppendL(TMigrationPairs(*subject, rejoinDCMsg.iNodeId2));

	CRejoinDataClientActivity::TCFDataClientJoiningRequest msg(subject->RecipientId(), subject->ClientType());

    RClientInterface::OpenPostMessageClose(
    	iContext.NodeId(),
    	rejoinDCMsg.iNodeId2,
    	msg);

    //Set Client being migrated to a new owner.. reference handed over. secure lifetime.
    subject->SetFlags(TCFClientType::EActivating);
    }

EXPORT_DEFINE_SMELEMENT(CRejoinDataClientActivity::TApplyRejoin, NetStateMachine::MStateTransition, CRejoinDataClientActivity::TContext)
void CRejoinDataClientActivity::TApplyRejoin::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 15));
	CRejoinDataClientActivity* rejoinActivity = static_cast<CRejoinDataClientActivity*>(iContext.iNodeActivity);
	__ASSERT_DEBUG(rejoinActivity->iDataClients.Count(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 16));
    for (TInt i = 0; i < rejoinActivity->iDataClients.Count(); i++)
        {
        RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->iDataClients[i].iDataClient.RecipientId(),
        	TCFFlow::TRejoin(rejoinActivity->iDataClients[i].iNewOwner).CRef());
        rejoinActivity->iDataClients[i].iDataClient.SetFlags(TCFClientType::ELeaving);
        }
    rejoinActivity->PostRequestTo(iContext.NodeId(), TCFScpr::TApplyRequest().CRef());
    }

EXPORT_DEFINE_SMELEMENT(CRejoinDataClientActivity::TRejoinLoopTag,NetStateMachine::MStateFork, CRejoinDataClientActivity::TContext)
EXPORT_C TInt CRejoinDataClientActivity::TRejoinLoopTag::TransitionTag()
	{
	if (iContext.iMessage.IsMessage<TCFRejoiningProvider::TRejoinDataClientRequest>())
    	{
    	return CoreStates::KLoopTag | NetStateMachine::EBackward;
    	}
	return MeshMachine::KNoTag | NetStateMachine::EForward;
	}

//-=========================================================
//
// CommsBinderActivity (parallel)
//
//-=========================================================

EXPORT_C MeshMachine::CNodeActivityBase* CCommsBinderActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
    {
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CCommsBinderActivity(aActivitySig, aNode, c);
    }

EXPORT_C CCommsBinderActivity::CCommsBinderActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount)
:	MeshMachine::CNodeRetryParallelActivity(aActivitySig, aNode, aNextActivityCount),
	TIfStaticFetcherNearestInHierarchy(this)
	{
	}

//-=========================================================
//
// CCommsBinderActivity
//
// Aggregate class containing common functionality for
// CommsBinderActivity and CCommsBinderCombiningActivity.
//
//-=========================================================
// When I node need to create the data client, this mutex block if the dataclient is leaving.
// A scenario can arise when a connection is being started, stopped and started again in succession.
// the stop might interfere with the second start and ccause the second start to fail.
// this mutex will ensure that the leaving DC is completely destroyed, and then the second start is continued
// - when any of the data clients is being destroyed 
// - when a non-leaving data client is present or the activity is already running
//
EXPORT_C TBool CCommsBinderActivity::TDataClientMutex::IsBlocked(MeshMachine::TNodeContextBase& aContext)
  	{
  	TInt c = aContext.Node().CountActivities(aContext.iNodeActivity->ActivitySigId());
  	__ASSERT_DEBUG(c>0, User::Panic(KSpecAssert_ESockCrStaCPRAC, 17)); //Diagnostic
    TInt numOfLeavingDataClients = aContext.Node().CountClients<TDefaultClientMatchPolicy>(
                                   /*include*/TClientType(TCFClientType::EData, TCFClientType::ELeaving));
  	if (numOfLeavingDataClients == 0 && (c == 1 || CCommsBinderActivity::IsDataClientPresent(aContext)))
  		{
  		return EFalse;
  		}
	return ETrue;
   	}

EXPORT_C TBool CCommsBinderActivity::TDefaultDataClientMutex::IsBlocked(MeshMachine::TNodeContextBase& aContext)
  	{
  	TInt c = aContext.Node().CountActivities(aContext.iNodeActivity->ActivitySigId());
  	__ASSERT_DEBUG(c>0, User::Panic(KSpecAssert_ESockCrStaCPRAC, 18)); //Diagnostic
    TInt numOfLeavingDataClients = aContext.Node().CountClients<TDefaultClientMatchPolicy>(
                                   /*include*/TClientType(TCFClientType::EData, TCFClientType::ELeaving));
  	if (numOfLeavingDataClients == 0 && (c == 1 || CCommsBinderActivity::IsDataClientPresent(aContext, TCFClientType::EDefault)))
  		{
  		return EFalse;
  		}
	return ETrue;
   	}

EXPORT_C /*virtual*/ CCommsBinderActivity::~CCommsBinderActivity()
	{
	if(!iBinderRequestParameters.IsNull())
		{
		iBinderRequestParameters.Close();
		}
	}

EXPORT_C void CCommsBinderActivity::StoreBinder(RNodeInterface* aDataClient)
    {
    __ASSERT_DEBUG(iPendingBinder == aDataClient || iPendingBinder == NULL, User::Panic(KSpecAssert_ESockCrStaCPRAC, 19));
	iPendingBinder = aDataClient;
    }

EXPORT_C RNodeInterface* CCommsBinderActivity::Binder() const
    {
	return iPendingBinder;
    }

EXPORT_C void CCommsBinderActivity::StoreBinderRequestParameters(const RCFParameterFamilyBundleC& aBinderRequestParameters)
	{
	__ASSERT_DEBUG(iBinderRequestParameters.IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 20));
	if(! aBinderRequestParameters.IsNull())
		{
		iBinderRequestParameters.Open(aBinderRequestParameters);
		}
	}

void CCommsBinderActivity::SendCustomFlowProvision()
/**
Send a custom message that provisions a flow with flow parameters
*/
	{
    __ASSERT_DEBUG(iPendingBinder, User::Panic(KSpecAssert_ESockCrStaCPRAC, 21));

	// Only send the message if we have parameters to send
	if(!iBinderRequestParameters.IsNull())
		{
		RParameterFamily parameterFamily =
			iBinderRequestParameters.FindFamily(KFlowParametersFamily);

		if(!parameterFamily.IsNull())
			{
			STypeId typeId = STypeId::CreateSTypeId(CFlowRequestParameters::EUid, CFlowRequestParameters::EType);
			CFlowRequestParameters* flowParams =	static_cast<CFlowRequestParameters*>(parameterFamily.FindParameterSet(typeId, RParameterFamily::ERequested));

				iPendingBinder->PostMessage(
					iNode.Id(),
					TCFInternalEsock::TFlowProvision(
						flowParams->GetFlowParams()
						).CRef()
					);
			}
		}
	}


void CCommsBinderActivity::SendBinderResponseToOriginator()
/**
Send out CommsBinderResponse to all originators.

We send to all originators that have joined up until this point, and store this count
in iOriginatorsCountSnapshot.  See comment in ProcessBindToComplete().
*/
    {
    __ASSERT_DEBUG(iPendingBinder, User::Panic(KSpecAssert_ESockCrStaCPRAC, 22));

    PostRequestTo(
        SoleOriginator().Peer(),
        TCFServiceProvider::TCommsBinderResponse(iPendingBinder->RecipientId()).CRef());

	iPendingBinder->SetFlags(TCFClientType::EActivating);
    }


EXPORT_C void CCommsBinderActivity::Cancel(TNodeContextBase& aContext)
    {
    if (iPendingBinder)
        {
        //iPendingBinder is set when CCommsBinderActivity decides which of its dataclients it 
        //wishes to send along with its TCommsBinderResponse and promptly (on the same stack) as
        //this response is sent. After TCommsBinderResponse, the error handling of CCommsBinderActivity
        //changes in that it should not flag any errors back to the recpient of TCommsBinderResponse.
        //Such response may be when:
        //(1) the recipient responds to TCommsBinderResponse with TError - the activity is finished, the recipients doesn't wait for anything (certainly not for an error)
        //(2) the recipients explicitly cancels (or implicitly cancells by leaving the local node) - the activity is finished, the recipient doesn't wait for anything (certainly not for an error) 
        SetError(KErrNone);
        BindToComplete();
        SetIdle();
        }
    else
        {
        CNodeRetryParallelActivity::Cancel(aContext);
        }
    }

void CCommsBinderActivity::BindToComplete()
    {
    __ASSERT_DEBUG(iPendingBinder, User::Panic(KSpecAssert_ESockCrStaCPRAC, 23));
	TUint c = iNode.CountActivities(ActivitySigId());
    // Note: this routine can be used with parallel and non-parallel binder activities.  In the
    // former case we start with multiple activities and eventually end up with a single one.  In
    // the latter case we have a single activity throughout.
	if(c == 1)
		{
		iPendingBinder->ClearFlags(TCFClientType::EActivating);
		}
	iPendingBinder = NULL;
    }

//
// CCommsBinderActivity methods that are embedded transitions/states/stateforks
//

EXPORT_C RNodeInterface* CCommsBinderActivity::IsDataClientPresent(TNodeContextBase& aContext, TUint aClientFlags)
/**
Check if we have a data client and, if so, store it as the binder for this activity.

@param aContext Node context
@param aClientFlags client flags to use in iterator check
@return ETrue if data client present, else EFalse.
*/
	{
	// Be careful not to use a client to which we have already sent TDestroy previously.
	// Find the first data client that does not have the ELeaving flag set.  If all are
	// marked ELeaving then return EFalse - they are on the way out and will disappear
	// eventually.  The effect will be that a new data client will be created.
	RNodeInterface* dataClient = aContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, aClientFlags), TClientType(0, TCFClientType::ELeaving));
	return dataClient;
	}

EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TNoTagOrUseExisting, NetStateMachine::MStateFork, CCommsBinderActivity::TContext)
EXPORT_C TInt CCommsBinderActivity::TNoTagOrUseExisting::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 24));
	TCFServiceProvider::TCommsBinderRequest& msg = message_cast<TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);

	// Save away the parameters sent to us so that they are accessible during the activity
	__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
	CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));
	intf->StoreBinderRequestParameters(msg.iFamilyBundle);

	if(msg.iValue == TSubConnOpen::EAttachToDefault)
		{
		RNodeInterface* dc = CCommsBinderActivity::IsDataClientPresent(iContext);
		if (dc)
			{
			__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
			CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));

			intf->StoreBinder(dc);
			return CoreStates::KUseExisting;
			}
		}
	return MeshMachine::KNoTag;
	}

//MZTODO - logic of:
//TNoTagOrWaitForIncomingOrUseExisting
//&
//TNoTagOrWaitForIncomingOrUseExistingDefault
//has been copied but does not seem right, as both use EDefault?
//Why?

//[401TODO] DL : Only one of TNoTagOrWaitForIncomingOrUseExisting(Default) is used, is it ok to nuke one?

EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExisting, NetStateMachine::MStateFork, CCommsBinderActivity::TContext)
EXPORT_C TInt CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExisting::TransitionTag()
	{
	TCFServiceProvider::TCommsBinderRequest& msg = message_cast<TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 25));

    if (msg.iValue == TSubConnOpen::EWaitForIncoming)
    	{
    	return CoreNetStates::KWaitForIncoming;
    	}

	else
		{
		RNodeInterface* dc = CCommsBinderActivity::IsDataClientPresent(iContext, TCFClientType::EDefault);
		if (dc)
			{
			__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
			CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));

			intf->StoreBinder(dc);
			return CoreStates::KUseExisting;
			}
		}

	return MeshMachine::KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExistingDefault, NetStateMachine::MStateFork, CCommsBinderActivity::TContext)
EXPORT_C TInt CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExistingDefault::TransitionTag()
	{
	TCFServiceProvider::TCommsBinderRequest& msg = message_cast<TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 26));

	if(msg.iValue == TSubConnOpen::EAttachToDefault)
		{
		RNodeInterface* dc = CCommsBinderActivity::IsDataClientPresent(iContext, TCFClientType::EDefault);
		if (dc)
			{
			__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
			CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));

			intf->StoreBinder(dc);
			return CoreStates::KUseExisting;
			}
		}
	else if (msg.iValue == TSubConnOpen::EWaitForIncoming)
    	{
    	return CoreNetStates::KWaitForIncoming;
    	}
	return MeshMachine::KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TStorePendingBinder, NetStateMachine::MStateTransition, CCommsBinderActivity::TContext)
EXPORT_C void CCommsBinderActivity::TStorePendingBinder::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 27));
	message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);

	__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
	CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));

    intf->StoreBinder(iContext.iPeer);
	}

EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TSendBinderResponse, NetStateMachine::MStateTransition, CCommsBinderActivity::TContext)
EXPORT_C void CCommsBinderActivity::TSendBinderResponse::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 28));

	__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
	CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));

    intf->SendBinderResponseToOriginator();
	}

EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TSendCustomFlowProvision, NetStateMachine::MStateTransition, CCommsBinderActivity::TContext)
EXPORT_C void CCommsBinderActivity::TSendCustomFlowProvision::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 29));

	__ASSERT_DEBUG(iContext.iNodeActivity->SupportsExtInterface(CCommsBinderActivity::KInterfaceId), User::Panic(KCorePrPanic, KPanicExtInterfaceNotSupported));
	CCommsBinderActivity* intf = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));

    intf->SendCustomFlowProvision();
	}
    
EXPORT_DEFINE_SMELEMENT(CCommsBinderActivity::TAwaitingBindToComplete, NetStateMachine::MState, PRStates::TContext)
EXPORT_C TBool CCommsBinderActivity::TAwaitingBindToComplete::Accept()
	{   
	TCFServiceProvider::TBindToComplete* bindToComplete = message_cast<TCFServiceProvider::TBindToComplete>(&iContext.iMessage); 
	if (bindToComplete) 
	    { 
	    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity)); 
	    iContext.iNodeActivity->SetError(bindToComplete->iValue); 
	    
	    CCommsBinderActivity* binderActivity = reinterpret_cast<CCommsBinderActivity*>(iContext.iNodeActivity->FetchExtInterface(CCommsBinderActivity::KInterfaceId));
	    __ASSERT_DEBUG(binderActivity, User::Panic(KSpecAssert_ESockCrStaCPRAC, 30));
	    binderActivity->BindToComplete();
	    iContext.Node().DestroyOrphanedDataClients();
	    return ETrue; 
	    }

	return EFalse;
	}

//-=========================================================
//
//CNoBearer Activity
//
//-=========================================================

CNoBearer::CNoBearer(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
:	MeshMachine::CNodeRetryParallelActivity(aActivitySig, aNode, aActivitiesCount),
	ABindingActivity(aNode.Id()),
    TIfStaticFetcherNearestInHierarchy(this)
	{
	}

/*virtual*/ CNoBearer::~CNoBearer()
	{
	iNoBearerParameters.Close();
	}

EXPORT_C MeshMachine::CNodeActivityBase* CNoBearer::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CNoBearer(aActivitySig, aNode, c);
	}

EXPORT_C TNodePeerId& CNoBearer::GetOriginator()
    {
    __ASSERT_DEBUG(iOriginators.Count() == 1, User::Panic(KSpecAssert_ESockCrStaCPRAC, 31));
    return iOriginators[0];
    }

EXPORT_C void CNoBearer::ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
	{
	aInterface = this;
	}

EXPORT_C TBool CNoBearer::TServiceProviderMutex::IsBlocked(MeshMachine::TNodeContextBase& aContext)
	{
	TInt c = aContext.Node().CountActivities(aContext.iNodeActivity->ActivitySigId());
  	__ASSERT_DEBUG(c>0, User::Panic(KSpecAssert_ESockCrStaCPRAC, 32)); //Diagnostic
  	if (c == 1 || aContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider)) != 0)
  		{
  		return EFalse;
  		}
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(CNoBearer::TRequestCommsBinder, NetStateMachine::MStateTransition, CNoBearer::TContext)
EXPORT_C void CNoBearer::TRequestCommsBinder::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
  	CNoBearer& noBearer = static_cast<CNoBearer&>(*iContext.iNodeActivity);

	RNodeInterface* sp = iContext.Node().ServiceProvider();
	__ASSERT_DEBUG(sp, User::Panic(KCorePrPanic, KPanicNoServiceProvider));

	TSubConnOpen::TSubConnType type = TSubConnOpen::EAttachToDefault;
	if (!(noBearer.GetOriginator().Flags() & TCFClientType::EDefault))
    	{
    	type = TSubConnOpen::ECreateNew;
    	}

	noBearer.PostRequestTo(
			*sp,
			TCFServiceProvider::TCommsBinderRequest(
					type, noBearer.iNoBearerParameters
					).CRef()
			);
	}

EXPORT_DEFINE_SMELEMENT(CNoBearer::TStoreRequestParameters, NetStateMachine::MStateTransition, CNoBearer::TContext)
EXPORT_C void CNoBearer::TStoreRequestParameters::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
  	CNoBearer& noBearer = static_cast<CNoBearer&>(*iContext.iNodeActivity);
	const TCFControlProvider::TNoBearer& noBearerMessage = message_cast<TCFControlProvider::TNoBearer>(iContext.iMessage);
	__ASSERT_DEBUG(noBearer.iNoBearerParameters.IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 33)); // handle should be empty when this fn is called
	if(noBearerMessage.iFamilyBundle.IsNull())
		{
		noBearer.iNoBearerParameters.Open();
		}
		else
		{
		noBearer.iNoBearerParameters.Open(noBearerMessage.iFamilyBundle);
		}
	}

EXPORT_DEFINE_SMELEMENT(CNoBearer::TNoTagOrDataClientsToStart, NetStateMachine::MStateFork, CNoBearer::TContext)
TInt CNoBearer::TNoTagOrDataClientsToStart::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
  	CNoBearer& activity = static_cast<CNoBearer&>(*iContext.iNodeActivity);

	const TNodePeerId& originator = activity.GetOriginator();
	__ASSERT_DEBUG(originator.Type() & TCFClientType::EData, User::Panic(KSpecAssert_ESockCrStaCPRAC, 34));

	if (activity.Error() == KErrNone && (originator.Flags() & (TCFClientType::EStarting|TCFClientType::EStarted|TCFClientType::ELeaving)) == 0)
		{
		//Start dc
		return CoreNetStates::KDataClientsToStart;
		}
	//Finish the activity
	return MeshMachine::KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(CNoBearer::TStartOriginatingDataClient, NetStateMachine::MStateTransition, CNoBearer::TContext)
void CNoBearer::TStartOriginatingDataClient::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
  	CNoBearer& activity = static_cast<CNoBearer&>(*iContext.iNodeActivity);
	TNodePeerId& originator = activity.GetOriginator();
	__ASSERT_DEBUG(originator.Type() & TCFClientType::EData, User::Panic(KSpecAssert_ESockCrStaCPRAC, 35));
	activity.PostRequestTo(originator.Peer(), TCFDataClient::TStart().CRef());
	originator.SetFlags(TCFClientType::EStarting);
    }

EXPORT_DEFINE_SMELEMENT(CNoBearer::TNoTagOrBearerPresent, NetStateMachine::MStateFork, CNoBearer::TContext)
EXPORT_C TInt CNoBearer::TNoTagOrBearerPresent::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CNoBearer& noBearer = static_cast<CNoBearer&>(*iContext.iNodeActivity);
	const TCFControlProvider::TNoBearer& noBearerMessage = message_cast<TCFControlProvider::TNoBearer>(iContext.iMessage);
	__ASSERT_DEBUG(noBearer.iNoBearerParameters.IsNull(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 36)); // should not yet be set when this fn is called
	noBearer.iNoBearerParameters.Open(noBearerMessage.iFamilyBundle);

	CoreNetStates::TNoTagOrBearerPresent fork(iContext);
	return fork.TransitionTag();
	}

EXPORT_DEFINE_SMELEMENT(CNoBearer::TNoTagOrBearerPresentForAutostart, NetStateMachine::MStateFork, CNoBearer::TContext)
EXPORT_C TInt CNoBearer::TNoTagOrBearerPresentForAutostart::TransitionTag()
	{
 	TInt cntrlClients = iContext.Node().CountClients<TDefaultClientMatchPolicy>(
 								/*include*/TClientType(TCFClientType::ECtrl));
 	if (cntrlClients > 0 &&
 		iContext.Node().ServiceProvider() &&
 		!(iContext.Node().ServiceProvider()->Flags() & TCFClientType::EStarted))
 		{
 		//This fork calculates if the NoBearer activity (which this fork has been implemented for)
 		//should attempt to autostart the service provider when returning it to the sender of TNoBearer.
 		//The philosphy here is that if the local node doesn't have a control client, then there's noone
 		//that could posibly start it. It will hence decide to autostart as the top layer of what looks
 		//like an implicit connection. In the future this autostart behaviour should become a specialty
 		//of someone more concrete (rather than generic function). We are speculating about the implicit
 		//top layer that could acquire this function if it ever comes into being.
 		return CoreNetStates::KBearerPresent;
 		}
 	return KNoTag;
	}


//-=========================================================
//
//CStartActivity Activity
//
//-=========================================================
CStartActivity::CStartActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	MeshMachine::CNodeRetryActivity(aActivitySig, aNode),
		CoreActivities::ABindingActivity(aNode.Id()),
		TIfStaticFetcherNearestInHierarchy(this)
		{
		}

EXPORT_C CStartActivity::~CStartActivity()
	{
    CMMCommsProviderBase& node(static_cast<CMMCommsProviderBase&>(iNode));
    const TProviderInfoExt* providerInfoExt = static_cast<const TProviderInfoExt*>(node.AccessPointConfig().FindExtension(
            STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId)));

    __ASSERT_DEBUG(providerInfoExt, User::Panic(KSpecAssert_ESockCrStaCPRAC, 40));

    if (Error() != KErrNone)
        {
		CNodeActivityBase* stopActivity = iNode.FindActivityById(ECFActivityStop);

		// If the Stop activity is running we skip sending the GoneDown message. This is because the Stop
		// activity will send GoneDown too.
		if (!stopActivity)
		    {
            TCFControlClient::TGoneDown goneDown(Error(), providerInfoExt->iProviderInfo.APId());

            TClientIter<TDefaultClientMatchPolicy> iter = iNode.GetClientIter<TDefaultClientMatchPolicy>(
                    TClientType(TCFClientType::ECtrl), TClientType(0, TCFClientType::ELeaving));
            RNodeInterface* ctrlClient = NULL;

            while ( (ctrlClient = iter++) != NULL )
                {
                // Let control clients know the node has gone down, other than those that originated Start (they will be errored by ~CNodeActivityBase)...
                if (FindOriginator(*ctrlClient) >= 0)
                    {
                    continue; // ControlClient is a Start originator
                    }

                TNodeCtxId ctxId(ActivityId(), iNode.Id());
                ctrlClient->PostMessage(ctxId, goneDown.CRef());
                }
		    }
        }
	}

EXPORT_C MeshMachine::CNodeActivityBase* CStartActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) CStartActivity(aActivitySig, aNode);
    }

//-=========================================================
//
//Gone Down Activity
//
//-=========================================================

EXPORT_C MeshMachine::CNodeActivityBase* CGoneDownActivity::New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    	{
   		TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CGoneDownActivity));
		CGoneDownActivity* self = new (space) CGoneDownActivity(aActivitySig, aNode);
		self->AppendPreallocatedActivity();
		return self;
    	}

CGoneDownActivity::CGoneDownActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CNodeRetryActivity(aActivitySig, aNode),
		APreallocatedOriginators<2>(iOriginators)
		{
		}

void CGoneDownActivity::Destroy()
		{
		this->~CGoneDownActivity(); //Run the destructor
		ReturnPreallocatedSpace(this);
		}

CGoneDownActivity::~CGoneDownActivity()
    {
    //This is a gone down activity. Error mode is its only/natural state
    //CGoneDownActivity inherits ultimatelly from CNodeActivityBase, which
    //will attempt to interpret the error mode as a failure to execute (and
    //auto respond to orignators), which we don't want. Hence clearing
    //the error and allowing 'this' to die peacefully.
    SetError(KErrNone);
    }

void CGoneDownActivity::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const TStateTriple& aFirst)
	{
	//399TODO: for the moment ignore duplicated TGoneDown messages
	//but this needs to be fixed (someone is sending TGoneDown from TSendDataClientStopped
	//hence the problem)

	//ASSERT(IsIdle());
	if (!IsIdle())
		{
		return;
		}

	//This activity provides service for only one single requestor at a time.
	__ASSERT_DEBUG(iOriginators.Count()==0, User::Panic(KSpecAssert_ESockCrStaCPRAC, 37)); //Diagnostic panic

    TCFControlClient::TGoneDown& msg = message_cast<TCFControlClient::TGoneDown>(aContext.iMessage);
    SetError(msg.iValue1);
    iGoneDownApId = msg.iValue2;

	MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext, (_L8("CGoneDownActivity %08x:\tStartL->starting activity"), this));
	NetStateMachine::ACore::Start(&aContext, aFirst);

	MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext, (_L8("CGoneDownActivity %08x:\tStartL->activity started"), this));
    (void)aOriginator;
	}

//Find next DC to rebind
TBool CGoneDownActivity::IsIdle() const
	{
	return NetStateMachine::ACore::IsIdle();
	}

EXPORT_DEFINE_SMELEMENT(CGoneDownActivity::TSendErrorRecoveryReq, NetStateMachine::MStateTransition, CGoneDownActivity::TContext)
void CGoneDownActivity::TSendErrorRecoveryReq::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	CGoneDownActivity& activity = static_cast<CGoneDownActivity&>(*iContext.iNodeActivity);

	//TInt error = ExtractErrorCode(iContext.iMessage);
    TErrContext ctx(iContext.NodeId(), TCFControlClient::TGoneDown::Id(), activity.ActivitySigId(), TStateChange(0, activity.Error()));
    ctx.iInfo = (TAny*)activity.iGoneDownApId;
	TEErrorRecovery::TErrorRecoveryRequest msg(ctx);

    //We can not set SetSentTo() to Control Provider, because the response may not be coming from it
    //(for example it could be coming from the MCpr and we could be the SCpr).
    //Normally this would be a problem because any unrelated TError message arriving to the node
    //and presented to the awaiting state would easily be confused with the response (TError ==
    //no recovery on the MCpr == EPropagate).
    //We avoid the problem by checking all arriving TError messages if they are adressed to our activity.
    __ASSERT_DEBUG(iContext.Node().ControlProvider(), User::Panic(KCorePrPanic, KPanicNoControlProvider));
    activity.PostRequestTo(*iContext.Node().ControlProvider(),
    	TCFSafeMessage::TRequestCarrierEast<TEErrorRecovery::TErrorRecoveryRequest>(msg).CRef());
    activity.ClearPostedTo();
    }

EXPORT_DEFINE_SMELEMENT(CGoneDownActivity::TIgnoreOrPropagate, NetStateMachine::MStateFork, CGoneDownActivity::TContext)
TInt CGoneDownActivity::TIgnoreOrPropagate::TransitionTag()
    {
    __ASSERT_DEBUG(iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>(), User::Panic(KSpecAssert_ESockCrStaCPRAC, 38));
    RNodeInterface* sp = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EStarted));
    if (sp)
        {
        return CoreStates::KIgnore | NetStateMachine::EForward;
        }
    //There is no started service provider, the reconnection, mobility, etc must have failed,
    //continue with tearing this layer down.
    CGoneDownActivity& activity = static_cast<CGoneDownActivity&>(*iContext.iNodeActivity);
    TErrResponse& resp = message_cast<TEErrorRecovery::TErrorRecoveryResponse>(iContext.iMessage).iErrResponse;
    resp.iAction=TErrResponse::EPropagate;
    __ASSERT_DEBUG(activity.Error()!=KErrNone, User::Panic(KSpecAssert_ESockCrStaCPRAC, 39));
    resp.iError = activity.Error();
    return CoreStates::KPropagate | NetStateMachine::EForward;
	}

//-=========================================================
//
//Stop Activity
//
//-=========================================================
/*
MeshMachine::CNodeActivityBase* CStopActivity::New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CStopActivity));
	CStopActivity* self = new (space) CStopActivity(aActivitySig, aNode);
	self->AppendPreallocatedActivity();
	return self;
	}

CStopActivity::CStopActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CNodeRetryActivity(aActivitySig, aNode),
		APreallocatedOriginators<10>(iOriginators)
	{
	}

void CStopActivity::Destroy()
	{
	this->~CStopActivity(); //Run the destructor
	ReturnPreallocatedSpace(this);
	}

CStopActivity::~CStopActivity()
    {
    //This is a gone down activity. Error mode is its only/natural state
    //CGoneDownActivity inherits ultimatelly from CNodeActivityBase, which
    //will attempt to interpret the error mode as a failure to execute (and
    //auto respond to orignators), which we don't want. Hence clearing
    //the error and allowing 'this' to die peacefully.
    SetError(KErrNone);
    }
*/
EXPORT_DEFINE_SMELEMENT(CStartActivity::TAwaitingBindToCompleteOrCancel, NetStateMachine::MState, CStartActivity::TContext)
TBool CStartActivity::TAwaitingBindToCompleteOrCancel::Accept()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCorePrPanic, KPanicNoActivity));
	if (iContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		iContext.iNodeActivity->SetError(KErrCancel);
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
	else
		{
		CoreNetStates::TAwaitingBindToComplete state(iContext);
		return state.Accept();
		}
	}
} //PRActivities

EXPORT_DEFINE_SMELEMENT(PRDataClientStopActivity::TNoTagOrProviderStopped, NetStateMachine::MStateFork, PRDataClientStopActivity::TContext)
EXPORT_C TInt PRDataClientStopActivity::TNoTagOrProviderStopped::TransitionTag()
	{
	iContext.iNodeActivity->SetError(message_cast<TCFDataClient::TStop>(iContext.iMessage).iValue);
    if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(
	    TClientType(TCFClientType::EData, TCFClientType::EStarted)) != NULL)
		{
		// At least one started data client
        return MeshMachine::KNoTag;
		}
	return CoreNetStates::KProviderStopped;
    }
