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
#include <comms-infras/ss_coreprstates.h>
#include "ss_flowrequest.h"
#include "ss_flowrequeststates.h"
#include <comms-infras/ss_mcprnodemessages.h>
#include <cs_subconparams.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_selector.h>
#include <elements/nm_messages_peer.h>

#include <comms-infras/ss_protocolparameterset.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockFlwRqS, "ESockSSockFlwRqS");
#endif

using namespace ESock;
using namespace CorePanics;
using namespace FlowRequestStates;
using namespace Messages;
using namespace MeshMachine;

#ifdef _DEBUG
_LIT (KCFlowRequestPanic,"FlowRequestPanic");
#endif

//
//Implicit and common
DEFINE_SMELEMENT(FlowRequestStates::TAwaitingImplicitFlowRequest, NetStateMachine::MState, FlowRequestStates::TContext)
TBool FlowRequestStates::TAwaitingImplicitFlowRequest::Accept()
	{
	TCFInternalEsock::TFlowRequest* flowReqMsg = message_cast<TCFInternalEsock::TFlowRequest>(&iContext.iMessage);
	return flowReqMsg && flowReqMsg->iFlowParams.iFlowRequestType == TFlowParams::EImplicit;
	}

DEFINE_SMELEMENT(FlowRequestStates::TAwaitingConnFlowRequest, NetStateMachine::MState, FlowRequestStates::TContext)
TBool FlowRequestStates::TAwaitingConnFlowRequest::Accept()
	{
	TCFInternalEsock::TFlowRequest* flowReqMsg = message_cast<TCFInternalEsock::TFlowRequest>(&iContext.iMessage);
	return flowReqMsg && flowReqMsg->iFlowParams.iFlowRequestType == TFlowParams::EExplicitConnection;
	}

DEFINE_SMELEMENT(FlowRequestStates::TAwaitingSubConnFlowRequest, NetStateMachine::MState, FlowRequestStates::TContext)
TBool FlowRequestStates::TAwaitingSubConnFlowRequest::Accept()
	{
	TCFInternalEsock::TFlowRequest* flowReqMsg = message_cast<TCFInternalEsock::TFlowRequest>(&iContext.iMessage);
	return flowReqMsg && flowReqMsg->iFlowParams.iFlowRequestType == TFlowParams::EExplicitSubConnection;
	}

DEFINE_SMELEMENT(FlowRequestStates::TRequestCSRCreation, NetStateMachine::MStateTransition, FlowRequestStates::TImplicitContext)
void FlowRequestStates::TRequestCSRCreation::DoL()
	{
	//const TImplicitContext& ctx = iContext;
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	__ASSERT_DEBUG(iContext.Node().iCSR.IsNull(), User::Panic(KCFlowRequestPanic, KPanicExpectedNoCSR));
	iContext.iNodeActivity->PostRequestTo(iContext.Node().iTierManagerFC,
		TCFInternalEsock::TCreateCSR(iContext.Node().iTierId).CRef(), EFalse);
	}

DEFINE_SMELEMENT(FlowRequestStates::TSelectMetaPlane, NetStateMachine::MStateTransition, FlowRequestStates::TImplicitContext)
void FlowRequestStates::TSelectMetaPlane::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	__ASSERT_DEBUG(iContext.Node().iCSR.IsNull(), User::Panic(KCFlowRequestPanic, KPanicExpectedNoCSR));
	iContext.Node().iCSR = message_cast<TCFInternalEsock::TCSRCreated>(iContext.iMessage).iNodeId;
	__ASSERT_DEBUG(!iContext.Node().iCSR.IsNull(), User::Panic(KCFlowRequestPanic, KPanicNoCSR));
    TSelectionPrefs prefs(TSelectionPrefs::ERequestCommsBinder); //Always get the same default connection (legacy -> on IpProto layer)
	iContext.iNodeActivity->PostRequestTo(iContext.Node().iCSR, TCFSelector::TSimpleSelect(prefs).CRef());
	}

DEFINE_SMELEMENT(FlowRequestStates::TJoinReceivedSCpr, NetStateMachine::MStateTransition, FlowRequestStates::TImplicitContext)
void FlowRequestStates::TJoinReceivedSCpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	__ASSERT_DEBUG(iContext.iPeer == iContext.Node().ServiceProvider(), User::Panic(KCFlowRequestPanic, KPanicExpectedNoServiceProvider));

	TCFServiceProvider::TCommsBinderResponse& br = message_cast<TCFServiceProvider::TCommsBinderResponse>(iContext.iMessage);

	RNodeInterface* brPeer = iContext.Node().AddClientL(br.iNodeId, TClientType(TCFClientType::EServProvider, TCFClientType::EDefault));
    iContext.iNodeActivity->PostRequestTo(*brPeer, TCFServiceProvider::TJoinRequest(iContext.NodeId(),
		TClientType(TCFClientType::ECtrl)).CRef());
	}

DEFINE_SMELEMENT(FlowRequestStates::TRequestCommsBinderFromSCpr, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TRequestCommsBinderFromSCpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	RNodeInterface* scpr = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EDefault));

	__ASSERT_DEBUG(scpr, User::Panic(KCFlowRequestPanic, KPanicNoServiceProvider));
  	iContext.iNodeActivity->PostRequestTo(*scpr, TCFServiceProvider::TCommsBinderRequest(TSubConnOpen::ECreateNew, iContext.Node().iFlowParameters).CRef());
	}

DEFINE_SMELEMENT(FlowRequestStates::TRemoveRequestor, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TRemoveRequestor::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	iContext.iNodeActivity->RemoveOriginator(0);
	//Remove the orignator client added to the list
	RNodeInterface* requestor = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
	if(requestor)
		{
//TODO bug fix 1245412 DEF122892 cant be applied - investigate 1221893 & 1245412 & te_rsubconnectionsuite.script crashes		TInt atFound = iContext.Activity()->FindOriginator(*requestor);
		iContext.Node().RemoveClient(requestor->RecipientId(), iContext);
//TODO bug fix cant be applied - investigate		if(atFound != KErrNotFound)
//			{
//			iContext.Activity()->RemoveOriginator(atFound);
//			}
		}
	}

DEFINE_SMELEMENT(FlowRequestActivities::CFlowRequestActivity::TStoreFlowParams, NetStateMachine::MStateTransition, TContext)
void FlowRequestActivities::CFlowRequestActivity::TStoreFlowParams::DoL()
	{
	// Make sure we are being executed in the context of a node that has an activity running
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));

	// Fetch our message and create a bundle of flow parameters from it
	TCFInternalEsock::TFlowRequest& flowRequestMessage = message_cast<TCFInternalEsock::TFlowRequest>(iContext.iMessage);

	CFlowRequestParameters* params = CFlowRequestParameters::NewL();
	CleanupStack::PushL(params);
	params->SetFlowParams(flowRequestMessage.iFlowParams);

	RCFParameterFamilyBundle bundle;
	bundle.CreateL();
	CleanupClosePushL(bundle);
    RParameterFamily family = bundle.CreateFamilyL(KFlowParametersFamily);

	family.AddParameterSetL (params, RParameterFamily::ERequested);

	// Save the flow parameter bundle in the activity
	__ASSERT_DEBUG(iContext.Node().iFlowParameters.IsNull(), User::Panic(KSpecAssert_ESockSSockFlwRqS, 1));
	iContext.Node().iFlowParameters.Open(bundle);

	// Cleanup
	CleanupStack::Pop(&bundle);
	CleanupStack::Pop(params);
	}

DEFINE_SMELEMENT(FlowRequestActivities::CFlowRequestActivity::TSendBindToComplete, NetStateMachine::MStateTransition, TContext)
void FlowRequestActivities::CFlowRequestActivity::TSendBindToComplete::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
    CFlowRequestActivity& activity = static_cast<CFlowRequestActivity&>(*iContext.iNodeActivity);
    __ASSERT_DEBUG(!activity.iBindToSender.IsNull(), User::Panic(KSpecAssert_ESockSSockFlwRqS, 2)); 
    RClientInterface::OpenPostMessageClose(iContext.Node().Id(), activity.iBindToSender, TCFDataClient::TBindToComplete().CRef());
    activity.iBindToSender.SetNull();
    }

DEFINE_SMELEMENT(FlowRequestActivities::CFlowRequestActivity::TAwaitingBindTo, NetStateMachine::MState, TContext)
TBool FlowRequestActivities::CFlowRequestActivity::TAwaitingBindTo::Accept()
    {
    TBool accept(EFalse);     
    if(iContext.iMessage.IsMessage<TCFDataClient::TBindTo>()) 
        { 
        // store the node to which we send the response later on.
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
        CFlowRequestActivity& activity = static_cast<CFlowRequestActivity&>(*iContext.iNodeActivity);
        __ASSERT_DEBUG(activity.iBindToSender.IsNull(), User::Panic(KSpecAssert_ESockSSockFlwRqS, 3));
        activity.iBindToSender = iContext.iSender;
        accept = ETrue;
        } 
    
    return accept;
    }

DEFINE_SMELEMENT(FlowRequestStates::TSendNoBearer, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TSendNoBearer::DoL()
	{
	// Make sure we are being executed in the context of a node that has an activity running
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));

	// Fetch the subconnection. It has been designated as our control provider
	RNodeInterface* subconn = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(
		TClientType(TCFClientType::ECtrlProvider, TCFClientType::EDefault));

	__ASSERT_DEBUG(subconn != NULL, User::Panic(KCFlowRequestPanic, KPanicNoControlProvider));

	// Send TNoBearer to the subconnection
	iContext.iNodeActivity->PostRequestTo(*subconn,
		TCFControlProvider::TNoBearer(iContext.Node().iFlowParameters).CRef());
	}


DEFINE_SMELEMENT(FlowRequestStates::TJoinSubConnection, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TJoinSubConnection::DoL()
	{
	// Make sure we are being executed in the context of a node that has an activity running
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));

	// Fetch the subconnection. It has been designated as our control provider
	RNodeInterface* subconn = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(
		TClientType(TCFClientType::ECtrlProvider, TCFClientType::EDefault));

	__ASSERT_DEBUG(subconn != NULL, User::Panic(KCFlowRequestPanic, KPanicNoControlProvider));

	// Join the subconnection
	iContext.iNodeActivity->PostRequestTo(*subconn,
		TCFPeer::TJoinRequest(
			iContext.Node().Id(), TClientType(TCFClientType::EData)).CRef());
	}

DEFINE_SMELEMENT(FlowRequestStates::TLeaveSubConnection, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TLeaveSubConnection::DoL()
	{
	// Make sure we are being executed in the context of a node that has an activity running
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));

	// Fetch the subconnection. Its our control provider and must be present
	RNodeInterface* subconn = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(
		TClientType(TCFClientType::ECtrlProvider, TCFClientType::EDefault));
	__ASSERT_DEBUG(subconn != NULL, User::Panic(KCFlowRequestPanic, KPanicNoControlProvider));

	// Send the leave request to our subconnection
	iContext.iNodeActivity->PostRequestTo(*subconn, TEPeer::TLeaveRequest().CRef());
	subconn->SetFlags(TCFClientType::ELeaving);
	}


DEFINE_SMELEMENT(FlowRequestStates::TForwardBindToMsgToOriginator, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TForwardBindToMsgToOriginator::DoL()
	{
	//This is assumed to be executed as a direct response to TCommsBinderResponse
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));

	// Create and send the comms binder from the incoming one
	TCFDataClient::TBindTo& incomingBindTo = message_cast<TCFDataClient::TBindTo>(iContext.iMessage);
    iContext.iNodeActivity->PostToOriginators(TCFDataClient::TBindTo(incomingBindTo.iNodeId).CRef());
    iContext.iNodeActivity->ClearPostedTo();
	}

//
//Cpr specific
DEFINE_SMELEMENT(FlowRequestStates::TJoinCpr, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TJoinCpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	RNodeInterface* cpr = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));

	//The sc has been already added
	__ASSERT_DEBUG(cpr != NULL, User::Panic(KCFlowRequestPanic, KPanicNoServiceProvider));

	//Join the service provider
	iContext.iNodeActivity->PostRequestTo(*cpr, TCFServiceProvider::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl)).CRef());
	}

//
//SCpr specific
DEFINE_SMELEMENT(FlowRequestStates::TJoinSCpr, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TJoinSCpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
	RNodeInterface* scpr = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EDefault));

	//The sc has been already added
	__ASSERT_DEBUG(scpr != NULL, User::Panic(KCFlowRequestPanic, KPanicNoServiceProvider));

	//Join the service provider
	iContext.iNodeActivity->PostRequestTo(*scpr, TCFServiceProvider::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl)).CRef());
	}



DEFINE_SMELEMENT(FlowRequestStates::TSendClientLeavingAndRemoveControlProvider, NetStateMachine::MStateTransition, FlowRequestStates::TContext)
void FlowRequestStates::TSendClientLeavingAndRemoveControlProvider::DoL()
	{
   	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrlProvider));
   	RNodeInterface* cl = iter[0];
   	//It is perfectly possible that there is no Control Provider at all.
   	if (cl)
   		{
		__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCFlowRequestPanic, KPanicNoActivity));
		cl->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TEChild::TLeft().CRef());
   		iContext.Node().RemoveClient(cl->RecipientId(),iContext);
   		__ASSERT_DEBUG(iter[1] == NULL, User::Panic(KCFlowRequestPanic, KPanicNoControlProvider)); //But it is not possible to have two Control Providers!
   		}
	}
