// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dummy implementation file for an SCPR which will arrange for the building of the sapshim
// layer beneath during its own creation.
// 
//

/**
 @file
 @internalComponent
*/

// Includes
//---------
#include <comms-infras/ss_log.h>
#include <comms-infras/corescpractivities.h>
#include <cs_subconparams.h>
#include "ss_subconn.h"

#include "dummypr_subconnprov.h"

#include <elements/nm_interfaces.h>
#include <elements/sd_mintercept.h>
#include <cs_subconevents.h>
#include <comms-infras/ss_protocolparameterset.h>

#include <ss_glob.h>

#include "ss_nodemessages_factory.h"
#include "ss_nodemessages_dataclient.h"
#include "ss_nodemessages_serviceprovider.h"
#include <comms-infras/ss_nodemessages_internal_esock.h>
#include "ss_internal_activities.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestdmyprxt, "ESockTestdmyprxt");
#endif

// Logging
//--------
#ifdef __CFLOG_ACTIVE
	#define KDummySCPRTag KESockConnectionTag
#endif

#ifdef _DEBUG
_LIT (KDummyProvidersPanic,"DummyProvidersPanic");

#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace PRActivities;
using namespace Messages;
using namespace MeshMachine;

namespace DummyExtendedSCPRStates
{
DEFINE_SMELEMENT(TAwaitingBinderResponse, NetStateMachine::MState, DummyExtendedSCPRStates::TContext)
TBool TAwaitingBinderResponse::Accept()
    {
	const TCFServiceProvider::TCommsBinderResponse* binderResponseMsg = message_cast<TCFServiceProvider::TCommsBinderResponse>(&iContext.iMessage);
	if (binderResponseMsg)
    	{
		__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockTestdmyprxt, 1));
		CDummyBuildStackActivity* act = static_cast<CDummyBuildStackActivity*>(iContext.iNodeActivity);
		act->iBinderSource = iContext.iSender;
        
        return ETrue;
    	}
    return EFalse;

	}

DEFINE_SMELEMENT(TRequestCommsBinderFromCPR, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TRequestCommsBinderFromCPR::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyProvidersPanic, 0));

	RNodeInterface* sp = iContext.Node().ControlProvider();
	__ASSERT_DEBUG(sp != NULL, User::Panic(KDummyProvidersPanic, 0));
	User::LeaveIfError(sp? KErrNone : KErrCorrupt);

	// Construct and send the message
	iContext.iNodeActivity->PostRequestTo(
		*sp,
		TCFServiceProvider::TCommsBinderRequest(
			TSubConnOpen::ECreateNew,
			iContext.Node().iParameterBundle
			).CRef()
		);
	}

DEFINE_SMELEMENT(TRequestCommsBinder, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TRequestCommsBinder::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyProvidersPanic, 0));

	RNodeInterface* sp = iContext.Node().ServiceProvider();
	__ASSERT_DEBUG(sp != NULL, User::Panic(KDummyProvidersPanic, 0));
	User::LeaveIfError(sp ? KErrNone : KErrCorrupt);

	// Fetch the current flow parameters (those that were sent to us) and create a new set for the flow we want
	TFlowParams flowParams = iContext.Node().iFlowParams;
	flowParams.iProtocol = KProtocolInetUdp;
	flowParams.iSocketType = KSockDatagram;

	// Create and load a parameter set with the flow parameters that form our binder request
	CFlowRequestParameters* paramSet = CFlowRequestParameters::NewL();
	CleanupStack::PushL(paramSet);
	paramSet->SetFlowParams(flowParams);
	RCFParameterFamilyBundle flowParamsBundle;
	flowParamsBundle.CreateL();
	RParameterFamily family = flowParamsBundle.CreateFamilyL(KFlowParametersFamily);
	family.AddParameterSetL(paramSet, RParameterFamily::ERequested);

	RCFParameterFamilyBundleC tempBundle;
	tempBundle.Open(flowParamsBundle);
	// Construct and send the message
	iContext.iNodeActivity->PostRequestTo(
		*sp,
		TCFServiceProvider::TCommsBinderRequest(
			TSubConnOpen::ECreateNew,
			tempBundle
			).CRef()
		);

	// We own the parameters so hold on to them until the activity is complete
	CDummyBuildStackActivity* activity = static_cast<CDummyBuildStackActivity*>(iContext.iNodeActivity);
	activity->StoreFlowParameters(flowParamsBundle);
	tempBundle.Close();
	// Cleanup
	CleanupStack::Pop(paramSet);
	}

DEFINE_SMELEMENT(TSendBindToComplete, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TSendBindToComplete::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockTestdmyprxt, 2));
	CDummyBuildStackActivity* act = static_cast<CDummyBuildStackActivity*>(iContext.iNodeActivity);

	RClientInterface::OpenPostMessageClose(iContext.Node().Id(), act->iBinderSource, TCFServiceProvider::TBindToComplete().CRef());
	}

DEFINE_SMELEMENT(TCreateDataClient, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TCreateDataClient::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyProvidersPanic, 0));

	TCFServiceProvider::TCommsBinderRequest* binderReq = message_cast<TCFServiceProvider::TCommsBinderRequest>(&iContext.iMessage);
	__ASSERT_DEBUG(binderReq, User::Panic(KDummyProvidersPanic, 0));

    // Determine what kind of data client to create. That is, the protocol type
	TSubConnOpen::TSubConnType protocolType = TSubConnOpen::EAttachToDefault;
	if(!binderReq->iFamilyBundle.IsNull())
		{
		// Find the flow parameter family of parameters
		RParameterFamily parameterFamily =
			binderReq->iFamilyBundle.FindFamily(KFlowParametersFamily);

		if(!parameterFamily.IsNull())
			{
			STypeId typeId = STypeId::CreateSTypeId(CFlowRequestParameters::EUid, CFlowRequestParameters::EType);
			CFlowRequestParameters* flowParams =	static_cast<CFlowRequestParameters*>(parameterFamily.FindParameterSet(typeId, RParameterFamily::ERequested));
				// The type of flow created by the flow factory is dependent only on the protocol type
				protocolType = (TSubConnOpen::TSubConnType)flowParams->GetFlowParams().iProtocol;
			}
		}

    TNodeId controlProvider = iContext.Node().ControlProvider() ?
    	iContext.Node().ControlProvider()->RecipientId() :
    	TNodeId::NullId();

	TDefaultFlowFactoryQuery query(controlProvider, iContext.Node().Id(), iContext.iMessage.MessageId(), protocolType);	
	// Build the data client request message and post it to the applicable factory container
	TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::EDataPlane, TUid::Uid(0x10285D8F), &query);

	// Send the request to the central object broker
	iContext.iNodeActivity->PostRequestTo(
		SockManGlobals::Get()->iCommsFactoryContainerBroker,
		msg,
		EFalse);
	}

DEFINE_SMELEMENT(TAwaitingBuildStack, NetStateMachine::MState, DummyExtendedSCPRStates::TContext)
TBool TAwaitingBuildStack::Accept()
	{
	return iContext.iMessage.IsMessage<TCFInternalEsock::TBuildStackRequest>();
	}

DEFINE_SMELEMENT(TSendBuildStackResponse, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TSendBuildStackResponse::DoL()
	{
	iContext.iNodeActivity->PostToOriginators(
	    TCFInternalEsock::TBuildStackResponse().CRef());
	}


DEFINE_SMELEMENT(TAwaitingBuildStackResponse, NetStateMachine::MState, DummyExtendedSCPRStates::TContext)
TBool TAwaitingBuildStackResponse::Accept()
	{
	return iContext.iMessage.IsMessage<TCFInternalEsock::TBuildStackResponse>();
	}

DEFINE_SMELEMENT(TSendBuildStack, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TSendBuildStack::DoL()
	{
	// Send build stack to ourself to kick off a stack building activity (we don't need to send any parameters)
	iContext.iNodeActivity->PostRequestTo(
		iContext.Node().Id(),
	    TCFInternalEsock::TBuildStackRequest(RCFParameterFamilyBundleC()).CRef()
		);
	}

DEFINE_SMELEMENT(TSendBindToResponse, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TSendBindToResponse::DoL()
	{
	//This is assumed to be executed as a direct response to TCommsBinderResponse
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KDummyProvidersPanic, 0));

	//Create a binder to send. But send it to our data client (only expecting one) as opposed to originator (which is us anyway)
	TCFServiceProvider::TCommsBinderResponse& binderResponse = message_cast<TCFServiceProvider::TCommsBinderResponse>(iContext.iMessage);
    TCFDataClient::TBindTo msg(binderResponse.iNodeId);

    // Send the message to our default data client
	RNodeInterface* dataClient = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(
		TCFClientType(TCFClientType::EData,	0)
		);

    __ASSERT_DEBUG(dataClient, User::Panic(KSpecAssert_ESockTestdmyprxt, 3));
    iContext.iNodeActivity->PostRequestTo(*dataClient, msg);
    iContext.iNodeActivity->SetPostedTo(TNodeId::NullId());
	}

DEFINE_SMELEMENT(TStoreFlowParams, NetStateMachine::MStateTransition, DummyExtendedSCPRStates::TContext)
void TStoreFlowParams::DoL()
	{
	TCFServiceProvider::TCommsBinderRequest& commsBinder = message_cast<TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);

	// Pull the flow params out of the bundle
	if(!commsBinder.iFamilyBundle.IsNull())
		{
		// Find the flow parameter family of parameters
		RParameterFamily parameterFamily =
			commsBinder.iFamilyBundle.FindFamily(KFlowParametersFamily);

		if(!parameterFamily.IsNull())
			{
			STypeId typeId = STypeId::CreateSTypeId(CFlowRequestParameters::EUid, CFlowRequestParameters::EType);
			CFlowRequestParameters* flowParams =	static_cast<CFlowRequestParameters*>(parameterFamily.FindParameterSet(typeId, RParameterFamily::ERequested));
				iContext.Node().iFlowParams = flowParams->GetFlowParams();
			}
		}
	}
}

CDummyBuildStackActivity::~CDummyBuildStackActivity()
	{
	if(!iFlowParameters.IsNull())
		{
		iFlowParameters.Close();
		}
	}

MeshMachine::CNodeActivityBase* CDummyBuildStackActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CDummyBuildStackActivity(aActivitySig, aNode, c);
    }

CDummyBuildStackActivity::CDummyBuildStackActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount)
	:	CCommsBinderActivity(aActivitySig, aNode, aNextActivityCount),
		iBinderSource(Messages::TNodeCtxId::NullId())
	{
	}

namespace DummyExtendedSCPRBuildStackActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBuildStack, DummyExtSCPRBuildStack, TCFInternalEsock::TBuildStackRequest, CDummyBuildStackActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(DummyExtendedSCPRStates::TAwaitingBuildStack, MeshMachine::TNoTag)

	// Need to request a comms binder from our CPR (as we are a stacking SCPR)
    NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TRequestCommsBinderFromCPR, DummyExtendedSCPRStates::TAwaitingBinderResponse, MeshMachine::TNoTag)

	// Now join the SCPR handed to us and request a comms binder from it too
	// In this comms binder we will send the flow request parameters we expect for the layer below (namely AfInet, UDP, Datagram)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TSendBindToComplete, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TRequestCommsBinder, CoreNetStates::TAwaitingBinderResponse, MeshMachine::TNoTag)

	// Now tell our only data client to join the flow we just received
	NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TSendBindToResponse, CoreNetStates::TAwaitingBindToComplete, MeshMachine::TNoTag)

	// And we are done so respond to the originating activity
	LAST_NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TSendBuildStackResponse)
NODEACTIVITY_END()
}



namespace DummySCPRBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, DummySCPRBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest)

	// Create a flow (a data client of ours)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TStoreFlowParams, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CCommsBinderActivity::TProcessDataClientCreation, MeshMachine::TNoTag)

	// Send a build stack request to ourself to get the half-tier built below us
	NODEACTIVITY_ENTRY(KNoTag, DummyExtendedSCPRStates::TSendBuildStack, DummyExtendedSCPRStates::TAwaitingBuildStackResponse, MeshMachine::TNoTag)

	// We finally need to respond to the flow request that got us started
	NODEACTIVITY_ENTRY(KNoTag, CCommsBinderActivity::TSendBinderResponse, CCommsBinderActivity::TAwaitingBindToComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace DummySCPRParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, DummySCPRSetParams, TCFScpr::TSetParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, TTag<CoreNetStates::KBearerPresent>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, PRStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TStoreAndRespondWithCurrentParams)
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, DummySCPRSetParams, TCFScpr::TParamsRequest)
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingParamRequest, CoreNetStates::TNoTagOrBearerPresent)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, SCprStates::TPassToServiceProvider, CoreNetStates::TAwaitingParamResponse, TTag<CoreNetStates::KBearerPresent>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, SCprStates::TStoreParamsAndPostToOriginators)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TStoreAndRespondWithCurrentParams)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

// Activity Map
namespace DummyExtendedSCPRStates
{
DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(DummySCPRBinderRequestActivity, DummySCPRBinderRequest)
   ACTIVITY_MAP_ENTRY(DummySCPRParamsRequest, DummySCPRSetParams)
   ACTIVITY_MAP_ENTRY(DummyExtendedSCPRBuildStackActivity, DummyExtSCPRBuildStack)
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}

CDummyExtendedSubConnectionProvider* CDummyExtendedSubConnectionProvider::NewL(ESock::CSubConnectionProviderFactoryBase& aFactory)
    {
    CDummyExtendedSubConnectionProvider* self = new (ELeave) CDummyExtendedSubConnectionProvider(aFactory);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


void CDummyExtendedSubConnectionProvider::ConstructL()
	{
	CCoreSubConnectionProvider::ConstructL();
	}

CDummyExtendedSubConnectionProvider::CDummyExtendedSubConnectionProvider(CSubConnectionProviderFactoryBase& aFactory)
:CCoreSubConnectionProvider(aFactory, DummyExtendedSCPRStates::stateMap::Self())
	{
	LOG_NODE_CREATE(KDummySCPRTag, CDummyExtendedSubConnectionProvider);
	}

CDummyExtendedSubConnectionProvider::~CDummyExtendedSubConnectionProvider()
	{
	LOG_NODE_DESTROY(KDummySCPRTag, CDummyExtendedSubConnectionProvider);
	}


void CDummyExtendedSubConnectionProvider::Received(MeshMachine::TNodeContextBase& aContext)
    {
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
        TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
        Messages::TNodeSignal::TMessageId()
        };

    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CDummyExtendedSubConnectionProvider::ReceivedL(
	const Messages::TRuntimeCtxId& aSender,
	const Messages::TNodeId& aRecipient,
	Messages::TSignatureBase& aMessage)
	{
	DummyExtendedSCPRStates::TContext ctx(*this, aMessage, aSender, aRecipient);
    CDummyExtendedSubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


