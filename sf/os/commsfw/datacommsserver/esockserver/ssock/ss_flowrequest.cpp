// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <comms-infras/ss_log.h>
#include <ss_std.h>
#include <ss_glob.h>
#include "SS_conn.H"
#include "ss_subconn.h"

#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include "ss_flowrequest.h"
#include <comms-infras/ss_esockstates.h>
#include "ss_flowrequeststates.h"
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_corepractivities.h>
#include <cs_subconparams.h>

#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <elements/nm_messages_child.h>
#include "ss_internal_activities.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockFlwRq, "ESockSSockFlwRq");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace FlowRequestStates;
using namespace FlowRequestActivities;
using namespace Messages;
using namespace MeshMachine;

namespace FlowRequestDestroyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroy, FlowRequestDestroy, TEChild::TDestroy, FlowRequestActivities::CFlowRequestDestroyActivity::New)
	FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingDestroy, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRemoveRequestor, MeshMachine::TNoTag)
	//TCFSubConnFlowRequest adds subconnection as a control provider so remove if necessary
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TSendClientLeavingAndRemoveControlProvider, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TLeaveServiceProvidersOrSetIdle, MeshMachine::TNoTag)
	//TDestroyAwaitingLeaveCompleteLoop loops back to its own triple if more SPs
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSetIdleIfNoServiceProviders, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreStates::TAbortAllActivities)
NODEACTIVITY_END()
}

namespace ImplicitFlowActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityImplicitFlow, ImplicitFlow, TCFInternalEsock::TFlowRequest, FlowRequestActivities::CFlowRequestActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(FlowRequestStates::TAwaitingImplicitFlowRequest, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CFlowRequestActivity::TStoreFlowParams, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRequestCSRCreation, CoreNetStates::TAwaitingCSRCreated, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TSelectMetaPlane, CFlowRequestActivity::TAwaitingBindTo, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CFlowRequestActivity::TSendBindToComplete,MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TRequestCommsBinder, CoreNetStates::TAwaitingBinderResponse,MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TJoinReceivedSCpr, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRequestCommsBinderFromSCpr, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBindTo, TAcceptErrorState<CoreNetStates::TAwaitingBindToComplete>, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)
	//Cleanup
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRemoveRequestor, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TLeaveServiceProvidersOrSetIdle, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSetIdleIfNoServiceProviders, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace ConnectionFlowActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityConnectionFlow, ConnectionFlow, TCFInternalEsock::TFlowRequest, FlowRequestActivities::CFlowRequestActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(FlowRequestStates::TAwaitingConnFlowRequest, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CFlowRequestActivity::TStoreFlowParams, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TJoinCpr, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TRequestCommsBinder, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TJoinReceivedSCpr, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRequestCommsBinderFromSCpr, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBindTo, TAcceptErrorState<CoreNetStates::TAwaitingBindToComplete>,MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::ABindingActivity::TSendBindToComplete, MeshMachine::TNoTag)
	//Cleanup
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRemoveRequestor, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TLeaveServiceProvidersOrSetIdle, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSetIdleIfNoServiceProviders, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace SubConnectionFlowActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySubConnectionFlow, SubConnectionFlow, TCFInternalEsock::TFlowRequest, FlowRequestActivities::CFlowRequestActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(FlowRequestStates::TAwaitingSubConnFlowRequest, MeshMachine::TNoTag)

	// Send TNoBearer to CSubConnection and wait for TBindTo?
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CFlowRequestActivity::TStoreFlowParams, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TJoinSubConnection, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TSendNoBearer, MeshMachine::TAcceptErrorState<CFlowRequestActivity::TAwaitingBindTo>, MeshMachine::TNoTagOrErrorTag)

        // NoBearer Succeeded
        // Forward TBindTo to the socket itself. After all it is the one to do the binding
        NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TForwardBindToMsgToOriginator, TAcceptErrorState<CoreNetStates::TAwaitingBindToComplete>, MeshMachine::TNoTag)
        THROUGH_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TClearError, MeshMachine::TNoTag) // The socket will error the client. Allow the BindToComplete to finish naturally.
        THROUGH_NODEACTIVITY_ENTRY(KNoTag, CFlowRequestActivity::TSendBindToComplete, MeshMachine::TNoTag)
    
        // Cleanup
        THROUGH_NODEACTIVITY_ENTRY(KNoTag, FlowRequestStates::TRemoveRequestor, MeshMachine::TNoTag)
        NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TLeaveServiceProvidersOrSetIdle, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
        NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSetIdleIfNoServiceProviders, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
        LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)

	// There was an error from the NoBearer request so lets clean up
	NODEACTIVITY_ENTRY(KErrorTag, FlowRequestStates::TLeaveSubConnection, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRaiseAndClearActivityError)
NODEACTIVITY_END()
}

namespace FlowRequestActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(flowRequestActivities)
    ACTIVITY_MAP_ENTRY(CoreErrorActivity, CoreError)
	ACTIVITY_MAP_ENTRY(ImplicitFlowActivity, ImplicitFlow)
	ACTIVITY_MAP_ENTRY(ConnectionFlowActivity, ConnectionFlow)
	ACTIVITY_MAP_ENTRY(SubConnectionFlowActivity, SubConnectionFlow)
	ACTIVITY_MAP_ENTRY(FlowRequestDestroyActivity, FlowRequestDestroy)
ACTIVITY_MAP_END()
}

CFlowRequest::CFlowRequest(TSubSessionUniqueId aSubSessionUniqueId)
:   ACFMMNodeIdBase(FlowRequestActivities::flowRequestActivities::Self()),
	ASubSessionPlatsecApiExt(aSubSessionUniqueId),
	TIfStaticFetcherNearestInHierarchy(this)
	{
	LOG_NODE_CREATE(KESockFlowTag, CFlowRequest);
	}

CFlowRequest::~CFlowRequest()
	{
	// We own the flow parameters sent to us and need to close them but
	// it is possible for the pointer to be null if the CFlowRequest
	// is created then destroyed before it is started.  This will happen
	// if e.g., it is popped and destroyed from the cleanup stack due to
	// the allocation of a second object by the flow requests's creator
	// causing a leave due to OOM before the flow request could be
	// started.
	iFlowParameters.Close();
	LOG_NODE_DESTROY(KESockFlowTag, CFlowRequest);
	__ASSERT_DEBUG(iClients.Count()==0, User::Panic(KSpecAssert_ESockSSockFlwRq, 1)); //Please fix your node.
	}

void CFlowRequest::Received(TNodeContextBase& aContext)
    {
    TNodeSignal::TMessageId noPeerIds[] = {
    	TCFInternalEsock::TFlowRequest::Id(),
    	TNodeSignal::TMessageId()
    	};
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

CFlowRequest* CFlowRequest::NewL(TSubSessionUniqueId aSubSessionUniqueId)
	{
	CFlowRequest* request = new(ELeave) CFlowRequest(aSubSessionUniqueId);

	CleanupStack::PushL(request);
	request->ConstructL();
	CleanupStack::Pop();

	return request;
	}

void CFlowRequest::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    // TODO: TCFMessage::TConnectionGoingDown - If this message is received
    // a new flow request needs to be sent on behalf of the socket, and
    // this one needs to cleanup/destroy itself. This is for the race condition
    // where either the idle timer triggers and begin stopping the connection,
    // or the connection is lost (both resulting in TConnectionGoingDown) while
    // the flow request is in progress.
    if (aMessage.IsMessage<TCFMessage::TStateChange>() ||
        aMessage.IsMessage<TCFControlClient::TGoneDown>() )
        {
        // Note of caution: As a control client of providers the CFlowRequest
        // could be bombarded with messages that we have no interest in.
        // TStateChange is one, but there could be others. Due to the nature
        // of the mesh machine if we don't handle them it will forward them
        // to the CSocket which only expects a limited set of messages from
        // the CFlowRequest. The CSocket wont like the messages and its not
        // great for performance to forward them unnecessarily so try to kill
        // them off here.
        aMessage.ClearMessageId();
        return;
        }

	TNodeContext<CFlowRequest> ctx(*this, aMessage, aSender, aRecipient);
    CFlowRequest::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CFlowRequest::ReturnInterfacePtrL(MPlatsecApiExt*& aInterface)
	{
	aInterface = this;
	}

CImplicitFlowRequest::CImplicitFlowRequest(TSubSessionUniqueId aSubSessionUniqueId, const TNodeId& aTierManagerFC, TUid aTierId)
:	CFlowRequest(aSubSessionUniqueId),
	iTierManagerFC(aTierManagerFC),
	iTierId(aTierId)
	{
	__ASSERT_DEBUG(!iTierManagerFC.IsNull() && iTierId.iUid!=0, User::Panic(KSpecAssert_ESockSSockFlwRq, 2));
	LOG_NODE_CREATE(KESockFlowTag, CImplicitFlowRequest);
	}

CImplicitFlowRequest::~CImplicitFlowRequest()
	{
	LOG_NODE_DESTROY(KESockFlowTag, CImplicitFlowRequest);
	if (!iCSR.IsNull())
		{
		RNodeInterface::OpenPostMessageClose(Id(), iCSR, TEChild::TDestroy().CRef());
		iCSR.SetNull();
		}
	}

CImplicitFlowRequest* CImplicitFlowRequest::NewL(TSubSessionUniqueId aSubSessionUniqueId, const TNodeId& aTierManagerFC, TUid aTierId)
	{
	CImplicitFlowRequest* request = new(ELeave) CImplicitFlowRequest(aSubSessionUniqueId, aTierManagerFC, aTierId);

	CleanupStack::PushL(request);
	request->ConstructL();
	CleanupStack::Pop();

	return request;
	}

//
//TCFFlowRequestBase
TCFFlowRequestBase::TCFFlowRequestBase(TSubSessionUniqueId aSubSessionUniqueId)
:	iSubSessionUniqueId(aSubSessionUniqueId), iFlowParams()
	{
	}

//
//TCFImplicitFlowRequest
void TCFImplicitFlowRequest::StartL(const TNodeId& aSender, const Messages::ANode& aItf)
	{
	TUid tierId = TierManagerUtils::MapTierIdsForLegacyImplicitFlowsL(
		TUid::Uid(iFlowParams.iAddrFamily),
		iFlowParams.iProtocol
		);

	CImplicitFlowRequest* req = CImplicitFlowRequest::NewL(iSubSessionUniqueId, aItf.NodeId(),tierId);
	CleanupStack::PushL(req);
	req->AddClientL(aSender, TClientType(TCFClientType::ECtrl));
	TCFInternalEsock::TFlowRequest msg(iFlowParams);
	req->ReceivedL(aSender, req->Id(), msg); //trigger the activity
	CleanupStack::Pop(req);
	}

void TCFImplicitFlowRequest::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
	{
	const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	__ASSERT_DEBUG(nodeId==SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)), User::Panic(KSpecAssert_ESockSSockFlwRq, 3)); //Implicit socket -> Must be dispatched on the TMF container!
	StartL(address_cast<TNodeId>(aSender), nodeId.Node());
	}

//
//TCFConnFlowRequest
void TCFConnFlowRequest::StartL(const TNodeId& aSender)
	{
	__ASSERT_DEBUG(iSession, User::Panic(KSpecAssert_ESockSSockFlwRq, 4));
    CConnection* cn = iSession->CConnectionFromHandle(iHandle);
    User::LeaveIfError(cn? KErrNone : KErrBadHandle);

	RNodeInterface* cnsp = cn->ServiceProvider();
	User::LeaveIfError(cnsp? KErrNone : KErrNotReady);

	CFlowRequest* req = CFlowRequest::NewL(iSubSessionUniqueId);
	CleanupStack::PushL(req);
	req->AddClientL(aSender, TClientType(TCFClientType::ECtrl));
	req->AddClientL(cnsp->RecipientId(), TClientType(TCFClientType::EServProvider, TCFClientType::EActive));
	TCFInternalEsock::TFlowRequest msg(iFlowParams); //Message to triger flow activity
	req->ReceivedL(aSender, req->Id(), msg);
	CleanupStack::Pop(req);
	}

void TCFConnFlowRequest::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
	{
	const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	__ASSERT_DEBUG(nodeId==SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane)), User::Panic(KSpecAssert_ESockSSockFlwRq, 5)); //Should be dispatched on the Connection Plane container!
	StartL(address_cast<TNodeId>(aSender));
	}

//
//TCFSubConnFlowRequest
void TCFSubConnFlowRequest::StartL(const TNodeId& aSender)
	{
	__ASSERT_DEBUG(iSession, User::Panic(KSpecAssert_ESockSSockFlwRq, 6));
    CSubConnection* scn = iSession->CSubConnectionFromHandle(iHandle);
    User::LeaveIfError(scn ? KErrNone : KErrBadHandle);

	CFlowRequest* req = CFlowRequest::NewL(iSubSessionUniqueId);
	CleanupStack::PushL(req);
	req->AddClientL(aSender, TClientType(TCFClientType::ECtrl));

	// Formerly, the top level SCPR would have been set as the service provider to the flow request
	// Instead we will apply the subconnection itself as the control provider so that we can instead drive it with TNoBearer
	req->AddClientL(scn->Id(), TClientType(TCFClientType::ECtrlProvider, TCFClientType::EDefault));
	TCFInternalEsock::TFlowRequest msg(iFlowParams); //Message to triger flow activity
	req->ReceivedL(aSender, req->Id(), msg);
	CleanupStack::Pop(req);
	}

void TCFSubConnFlowRequest::DispatchL( const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
	{
	const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	__ASSERT_DEBUG(nodeId==SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ESubConnPlane)), User::Panic(KSpecAssert_ESockSSockFlwRq, 7)); //Should be dispatched on the SubConnection Plane container!
	StartL(address_cast<TNodeId>(aSender));
	}


