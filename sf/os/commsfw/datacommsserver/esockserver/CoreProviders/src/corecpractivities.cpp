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
// Core CPR Activities
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
//
//

/**
 @file
 @internalComponent
*/

#define SYMBIAN_NETWORKING_UPS

#include "corecpractivities.h"

#include <comms-infras/ss_log.h>
#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_mcpr.h>
#include "ss_internal_activities.h"
#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/upsmessages.h>
#include <comms-infras/upsprstates.h>
#endif

using namespace ESock;
using namespace CprActivities;
using namespace NetStateMachine;
using namespace PRActivities;
using namespace CoreNetStates;
using namespace MeshMachine;
using namespace Messages;

//-=========================================================
//
//Panics
//
//-=========================================================
_LIT (KCoreCPRActivityPanic,"CoreCPRActivityPanic");
enum
	{
	EPanicNoPrealloc = 1
	};

//-=========================================================
//
// Activities
//
//-=========================================================

namespace CprControlClientJoinActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityClientJoin, CprControlClientJoin, TNodeSignal::TNullMessageId, CControlClientJoinActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingControlClientJoin,  CControlClientJoinActivity::TAddClientOrUpdatePriority)
	THROUGH_NODEACTIVITY_ENTRY(CprStates::KUpdatePriority, MeshMachine::TDoNothing, CControlClientJoinActivity::TNoTagOrSendPriorityToCtrlProvider)
	THROUGH_NODEACTIVITY_ENTRY(CprStates::KAddClient, CprStates::TAddControlClient, CControlClientJoinActivity::TNoTagOrSendPriorityToCtrlProvider)
	NODEACTIVITY_ENTRY(CprStates::KSendPriorityToCtrlProvider, CControlClientJoinActivity::TUpdatePriorityForControlProvider, CoreStates::TAwaitingJoinComplete, CControlClientJoinActivity::TNoTagOrSendPriorityToServProvider)
	NODEACTIVITY_ENTRY(CprStates::KSendPriorityToServProvider, CControlClientJoinActivity::TUpdatePriorityForServiceProviders, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CControlClientJoinActivity::TSendJoinCompleteIfRequest, CControlClientJoinActivity::TNoTagOrSendActive)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(CprStates::KSendActive, CoreNetStates::TSendDataClientActive)
NODEACTIVITY_END()
}

namespace CprNoBearerActivity
{
// Note that there is an issue here with multiple NoBearer messages being received in rapid succession which
// results in several of these activities running in parallel.  This will not be resolved by the
// CoreNetStates::TNoTagOrBearerPresent due to the almost simultaneous reception of the messages.  This
// could possibly be resolved with a custom mutex that checks for a ServiceProvider() as part of its
// IsBlocked() check.

DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityNoBearer, CprNoBearer, TCFControlProvider::TNoBearer, PRActivities::CNoBearer::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRActivities::CNoBearer::TStoreRequestParameters, CNoBearer::TNoTagOrBearerPresentBlockedByNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendNoBearer, MeshMachine::TAwaitingMessageState<TCFControlProvider::TBearer>, CoreNetStates::TNoTagOrBearerPresentOrErrorTag)

	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRActivities::CNoBearer::TRequestCommsBinderRetry, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TTag<CoreNetStates::KBearerPresent>)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TSendBindTo, CoreNetStates::TAwaitingBindToComplete, MeshMachine::TTag<CoreNetStates::KBearerPresent>)
	THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreActivities::ABindingActivity::TSendBindToComplete, CNoBearer::TNoTagOrBearerPresentForAutostart)

	//Autostart on NoBearer.
 	//The philosphy here is that if the local node doesn't have a control client, then there's noone
 	//that could posibly start it. It will hence decide to autostart as the top layer of what looks
 	//like an implicit connection. In the future this autostart behaviour should become a specialty
 	//of someone more concrete (rather than generic function). We are speculating about the implicit
 	//top layer that could acquire this function if it ever comes into being.
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TStartServiceProviderRetry, CoreNetStates::TAwaitingStarted, MeshMachine::TNoTag)

	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer)
    LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace CprBindToActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBindTo, CprBindTo, TCFDataClient::TBindTo, CCprBindToActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBindTo, CCprBindToActivity::TNoTagOrBearerReady)
    //TBindTo can hold:
    //[KNoTag] 		 - a valid serviceProvider cookie that this node isn't bound to;
    //[KBearerReady] - a valid serviceProvider cookie that this node is already bound to;
    //[KBearerReady] - a NULL serviceProvider (this node is at the stack's bottom);

	  //{ JOINING NEW SERVICE PROVIDER
		//a valid serviceProvider supplied, new to this node, let's join it;
	    NODEACTIVITY_ENTRY(KNoTag, CCprBindToActivity::TSendControlClientJoinRequestWithPriority, CoreStates::TAwaitingJoinComplete, TTag<KBearerReady>)
	  //}

	//serviceProvider provisionally joined. Now the activity needs to construct the configuration access points
	THROUGH_NODEACTIVITY_ENTRY(KDataClientReady, MeshMachine::TDoNothing, CprStates::TCreateAdditionalDataClientOrDataClientReady)
		NODEACTIVITY_ENTRY(CprStates::KCreateAdditionalDataClient, CprStates::TCreateAdditionalDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TTag<CprStates::KCreatingAdditionalDataClient>)
		NODEACTIVITY_ENTRY(CprStates::KCreatingAdditionalDataClient, CprStates::TProcessAdditionalDataClientCreationAndBindToPrimary, CoreNetStates::TAwaitingBindToComplete, CprStates::TCreateAdditionalDataClientBackwardOrDataClientReady)

	//Now the activity needs to propagate iteslf (TBindTo) to its dataclients.
	//The dataclients are either present or not. If not this activity will assume this is the layer construction phase
	//and will attempt to construct a default dataclient.
	THROUGH_NODEACTIVITY_ENTRY(KBearerReady, MeshMachine::TDoNothing, CCprBindToActivity::TNoTagOrDataClientReady)

	  //{ DATA CLIENT CREATION
		//No dataclients present, assume this is the layer creation phase. Attempt to create a dataclient.
		NODEACTIVITY_ENTRY(KNoTag, CCprBindToActivity::TCreateDataClient, TAcceptErrorState<CoreNetStates::TAwaitingDataClientJoin>, MeshMachine::TErrorTagOr<CCprBindToActivity::TNoTagOrBindToComplete>)
	    //BindTo activity is the pre-start layer builder, hence it always requests the dataclient from the factory.
	    //The factory (being aware of the phase) may decide to:
	    //1. create a new dataclient          -> process dataclient creation            [KNoTag]
	    //2. return a preexisting dataclient  -> bind the client                        [KDataClientReady]
	    //3. not to create a dataclient       -> send TBindToComplete to the originator [KBindToComplete]
		THROUGH_NODEACTIVITY_ENTRY(KNoTag, PRStates::TProcessDataClientCreation, TTag<KDataClientReady>)
	  //}

    THROUGH_NODEACTIVITY_ENTRY(KDataClientReady, MeshMachine::TDoNothing, CCprBindToActivity::TNoTagOrBearerReadyOrBindToComplete)
      //{ BINDING DATACLIENTS LOOP
	    //Dataclient(s) is/are ready. Depending on whether the node has the lower layer or not,
	    //we will [KNoTag] or will not [KNoBearer] need to request a binder for the dataclient.

		  //{SERVICE PROVIDER PRESENT
			NODEACTIVITY_ENTRY(KNoTag, CCprBindToActivity::TRequestCommsBinder, TAcceptErrorState<CoreNetStates::TAwaitingBinderResponse>, TErrorTagOr<TTag<KBearerReady> >)
	      //}
		NODEACTIVITY_ENTRY(KBearerReady, CCprBindToActivity::TSendBindTo, CCprBindToActivity::TAwaitingBindToCompleteOrError,
																					 TErrorTagOr<TTag<KDataClientReady | NetStateMachine::EBackward> >)
	  //}

	//Binding is finished. If this is not autocommit (see TCFDataClient::TBindTo), the activity will reply TCFDataClient::TBindToComplete
	//to the sender await for the confirmation (TCFDataClient::TCommitBindTo) or cancelation (TBase::TCancel) from the sender.
	//If this is autommit, the activity will skip awaiting for TCFDataClient::TCommitBindTo and commit itself.
	THROUGH_NODEACTIVITY_ENTRY(KBindToComplete, CCprBindToActivity::TSendBindToComplete, CCprBindToActivity::TNoTagOrCommit)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingMessageState<TCFDataClient::TCommitBindTo>, TErrorTagOr<TTag<CoreStates::KCommit> >)

	//commiting (either implicit or explicit).
	NODEACTIVITY_ENTRY(CoreStates::KCommit, CCprBindToActivity::TCommit, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)

	//This is not autocommit and the sender has just explicitly cancelled. Alternativelly this is an error path.
	//Cancelling/processing error entiles sending TCancel to all dataclients awaiting confirmation
	//as well as it entiles leaving the new service provider.
	NODEACTIVITY_ENTRY(KErrorTag, CCprBindToActivity::TCancel, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)

	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace CprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, CprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExistingDefaultBlockedByBinderRequest)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)

	// Below this point we need to modify the error handling approach. If we're getting a TError on TBinderResponse,
	// this means the client requesting the binder couldn't bind to it. As far as the client is concerned, this
	// activity is finished (it has flagged an error). The standard error handling will result in erroring
	// the originator. In this case we shouoldn't error the originator, instead, wrap up quietly.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CCommsBinderActivity::TProcessDataClientCreation, MeshMachine::TTag<CoreStates::KUseExisting>)

	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, CCommsBinderActivity::TSendBinderResponse, CCommsBinderActivity::TAwaitingBindToComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)

	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TClearError)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KWaitForIncoming, MeshMachine::TRaiseError<KErrNotSupported>)
NODEACTIVITY_END()
}

namespace CprDataClientStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStartDataClient, CprDataClientStart, TCFDataClient::TStart, CDataClientStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientStart, CoreNetStates::TNoTagOrNoDataClients)
	NODEACTIVITY_ENTRY(KNoTag, CprDataClientStartActivity::TStartDataClient, MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingDataClientStarted>, MeshMachine::TErrorTagOr<CprDataClientStartActivity::TNoTagOrNoTagBackward>)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendDataClientStarted)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoDataClients, PRStates::TSendDataClientStarted)

	NODEACTIVITY_ENTRY(KErrorTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace CprDataClientIdleActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientIdle, CprDataClientIdle, TCFControlProvider::TIdle)
    NODEACTIVITY_ENTRY(KNoTag, CprStates::THandleDataClientIdle, CoreNetStates::TAwaitingDataClientIdle, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace CprClientLeaveActivity
{
DECLARE_DEFINE_RESERVED_CUSTOM_NODEACTIVITY(ECFActivityClientLeave, CprClientLeave, TCFServiceProvider::TLeaveRequest, CClientLeaveActivity::New)
	FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingMessageState<TCFServiceProvider::TLeaveRequest>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CClientLeaveActivity::TRemoveClientAndDestroyOrphanedDataClients, CClientLeaveActivity::TNoTagOrSendPriorityToCtrlProvider)
	NODEACTIVITY_ENTRY(CprStates::KSendPriorityToCtrlProvider, CClientLeaveActivity::TUpdatePriorityForControlProvider, CoreStates::TAwaitingJoinComplete, CClientLeaveActivity::TNoTagOrSendPriorityToServProvider)
	NODEACTIVITY_ENTRY(CprStates::KSendPriorityToServProvider, CClientLeaveActivity::TUpdatePriorityForServiceProviders, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CClientLeaveActivity::TSendLeaveCompleteAndSendDataClientIdleIfNeeded, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CprStates::TSendDataClientStatusStoppedIfNoControlClient)
NODEACTIVITY_END()
}

namespace CprDataClientGoneDownActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDataClientGoneDown, CprDataClientGoneDown, TCFControlProvider::TDataClientGoneDown)
	// The only thing we do is to clear(unset) the "Flags" of
	//	the relative DataClient from "EStart": this is done
	//	in "TAwaitingDataClientGoneDown".
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientGoneDown, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace CprPolicyCheckRequestActivity
{
#ifdef SYMBIAN_NETWORKING_UPS
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityPolicyCheckRequest, CPrPolicyCheckRequest, UpsMessage::TPolicyCheckRequest, CNodeParallelActivityBase::NewL)
	// TODO: decide whether we should move some of these transitions/forks to the UpsCoreProviders
	FIRST_NODEACTIVITY_ENTRY(UpsStates::TAwaitingPolicyCheckRequest, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CprStates::TPostPolicyCheckResponseToOriginators)
NODEACTIVITY_END()
#endif
}

namespace CprActivities
{
DEFINE_EXPORT_ACTIVITY_MAP(coreCprActivities)
	ACTIVITY_MAP_ENTRY(CprControlClientJoinActivity, CprControlClientJoin)
	ACTIVITY_MAP_ENTRY(CprNoBearerActivity, CprNoBearer)
	ACTIVITY_MAP_ENTRY(CprBinderRequestActivity, CprBinderRequest)
	ACTIVITY_MAP_ENTRY(CprBindToActivity, CprBindTo)
	ACTIVITY_MAP_ENTRY(CprDataClientStartActivity, CprDataClientStart)
	ACTIVITY_MAP_ENTRY(CprDataClientIdleActivity, CprDataClientIdle)
    ACTIVITY_MAP_ENTRY(CprClientLeaveActivity, CprClientLeave)
	ACTIVITY_MAP_ENTRY(CprDataClientGoneDownActivity, CprDataClientGoneDown)
#ifdef SYMBIAN_NETWORKING_UPS
	ACTIVITY_MAP_ENTRY(CprPolicyCheckRequestActivity, CPrPolicyCheckRequest)
#endif
ACTIVITY_MAP_END_BASE(PRActivities, coreActivitiesCpr)
}

namespace CprDataClientStartActivity
{
EXPORT_C CDataClientStartActivity::CDataClientStartActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	: MeshMachine::CNodeActivityBase(aActivitySig, aNode), iClientIter(aNode.GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)))
	{}
}

namespace CprPriorityUpdateActivity
{
CPriorityUpdateActivity::CPriorityUpdateActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
:	MeshMachine::CNodeParallelActivityBase(aActivitySig, aNode, aActivitiesCount)
	{
	}
}

namespace CprControlClientJoinActivity
{
EXPORT_C MeshMachine::CNodeActivityBase* CControlClientJoinActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	return new(ELeave)CControlClientJoinActivity(aActivitySig, aNode, c);
	}

CControlClientJoinActivity::CControlClientJoinActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
:	CprPriorityUpdateActivity::CPriorityUpdateActivity(aActivitySig, aNode, aActivitiesCount)
	{
	}

EXPORT_C CControlClientJoinActivity::~CControlClientJoinActivity()
	{
	}
}

namespace CprClientLeaveActivity
{
EXPORT_C MeshMachine::CNodeActivityBase* CClientLeaveActivity::New(  const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::TNodeContextBase& aContext )
	{
	TUint c = GetNextActivityCount(aActivitySig,aContext.Node());
	
	// Instantiate using memory preallocated and stored in the RCFNodeInterface
	TAny* preallocatedSpace = aContext.iPeer->ClaimPreallocatedSpace(sizeof(CClientLeaveActivity));
	__ASSERT_ALWAYS(preallocatedSpace != NULL,User::Panic(KCoreCPRActivityPanic,EPanicNoPrealloc));

	CClientLeaveActivity* self = new(preallocatedSpace)CClientLeaveActivity(aActivitySig, aContext.Node(), c);
	return self;
	}

CClientLeaveActivity::CClientLeaveActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
:	CprPriorityUpdateActivity::CPriorityUpdateActivity(aActivitySig, aNode, aActivitiesCount)
	{
	}

EXPORT_C CClientLeaveActivity::~CClientLeaveActivity()
	{
	}
}

namespace CprBindToActivity
{
CCprBindToActivity::CCprBindToActivity(const MeshMachine::TNodeActivity& aActivitySig,  MeshMachine::AMMNodeBase& aNode, TInt aNextActivityCount)
:	CBindToActivity(aActivitySig, aNode, aNextActivityCount)
	{
	}
}


