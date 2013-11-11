// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dummy implementation file for a SubConnection Provider
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/corescpractivities.h>
#include <cs_subconparams.h>
#include <cs_subconevents.h>

#include "dummypr_subconnprov.h"
#include "dummypr_metaconnprov.h"
#include "activityTest.h"

#include <elements/sd_mintercept.h>

#ifdef __CFLOG_ACTIVE
	#define KDummySCPRTag KESockConnectionTag
#endif

using namespace ESock;
using namespace MeshMachine;

namespace DummySCPRStates
{
DEFINE_SMELEMENT(TRaiseGranted, NetStateMachine::MStateTransition, DummySCPRStates::TContext)
void TRaiseGranted::DoL()
	{
	if (iContext.Node().incomingStatus == TRUE)
		{
		//generate paramsgranted.
		CSubConGenEventParamsGranted* grantedevent = CSubConGenEventParamsGranted::NewL();
	    grantedevent->SetFamily(KSubConQoSFamily);
	    iContext.Node().NotifyClientsL(*grantedevent);
	    iContext.Node().incomingStatus = FALSE;
		}
	}

DEFINE_SMELEMENT(TAwaitingReBindTo, NetStateMachine::MState, DummySCPRStates::TContext)
TBool TAwaitingReBindTo::Accept()
	{
	const TLayerSelectionInfo* selectionInfo = static_cast<const TLayerSelectionInfo*>(
		iContext.Node().AccessPointConfig().FindExtension(TLayerSelectionInfo::TypeId()));
	ASSERT(selectionInfo); // should always be there
	
	TCFDataClient::TBindTo* bindToMsg = Messages::message_cast<TCFDataClient::TBindTo>(&iContext.iMessage);
	if (bindToMsg && iContext.Node().ServiceProvider() && bindToMsg->iNodeId != iContext.Node().ServiceProvider()->RecipientId())
	    {
	    if (TCprConfigModifier::Is(selectionInfo->CprConfig(), TCprConfigModifier::ESCPRHangOnRebind))
	        {
	        return ETrue;
	        }
	    else if (TCprConfigModifier::Is(selectionInfo->CprConfig(), TCprConfigModifier::ESCPRFailOnRebind))
	        {
	        iContext.iMessage.ClearMessageId();
	        Messages::RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.iSender, 
                Messages::TEBase::TError(KErrAbort).CRef());
	        }
	    }
	return EFalse;
	}

DEFINE_SMELEMENT(TAwaitingBrokenStart, NetStateMachine::MState, DummySCPRStates::TContext)
TBool TAwaitingBrokenStart::Accept()
	{
	const TLayerSelectionInfo* selectionInfo = static_cast<const TLayerSelectionInfo*>(
		iContext.Node().AccessPointConfig().FindExtension(TLayerSelectionInfo::TypeId()));
	ASSERT(selectionInfo); // should always be there

	return (iContext.iMessage.IsMessage<TCFDataClient::TStart>() && 
			TCprConfigModifier::Is(selectionInfo->CprConfig(), TCprConfigModifier::ESCPRHangOnStart));
	}

DEFINE_SMELEMENT(TCancelPreviousSelfRequest, NetStateMachine::MStateTransition, DummySCPRStates::TContext)
void TCancelPreviousSelfRequest::DoL()
    {
    iContext.iNodeActivity->PostRequestTo(iContext.Node().Id(), Messages::TEBase::TCancel().CRef());
    }

DEFINE_SMELEMENT(TRebindSelf, NetStateMachine::MStateTransition, DummySCPRStates::TContext)
void TRebindSelf::DoL()
    {
    iContext.iNodeActivity->PostRequestTo(iContext.Node().Id(), TCFDataClient::TBindTo(Messages::TNodeId::NullId()).CRef());
    }




CrazyIdle::~CrazyIdle()
    {
    ASSERT(PostedToNodeId()==Messages::TNodeId::NullId());
    }

MeshMachine::CNodeActivityBase* CrazyIdle::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new(ELeave)CrazyIdle(aActivitySig, aNode);
    }

DEFINE_SMELEMENT(CrazyIdle::TAwaitingCancelled, NetStateMachine::MState, DummySCPRStates::TContext)
TBool CrazyIdle::TAwaitingCancelled::Accept()
    {
    if (iContext.iMessage.IsMessage<Messages::TEBase::TError>())
        {
        ASSERT(iContext.iNodeActivity);
        iContext.iNodeActivity->SetPostedTo(iContext.iNodeActivity->SoleOriginator().Peer());
        }
    return EFalse;
    }
}



namespace DummySCprParamsRequest
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, dummySCprParamRequest, TCFScpr::TSetParamsRequest)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TRespondWithCurrentParams, PRStates::TAwaitingParamRequest,  MeshMachine::TNoTag)
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, dummySCprParamRequest, TCFScpr::TParamsRequest)
	NODEACTIVITY_ENTRY(KNoTag, SCprStates::TRespondWithCurrentParams, SCprStates::TAwaitingParamRequest,  MeshMachine::TNoTag)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

namespace DummyBindToActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityBindTo, dummyBindTo, TCFDataClient::TBindTo)
	FIRST_NODEACTIVITY_ENTRY(DummySCPRStates::TAwaitingReBindTo, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreStates::TNeverAccept,  MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace DummyBreakStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, dummyBreakSCPrStart, TCFDataClient::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(DummySCPRStates::TAwaitingBrokenStart, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CoreStates::TNeverAccept,  MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace DummyStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, dummySCPrStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, MeshMachine::TNoTag)
 	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted, MeshMachine::TNoTag)
 	LAST_NODEACTIVITY_ENTRY(KNoTag, DummySCPRStates::TRaiseGranted)
NODEACTIVITY_END()
}

namespace CrazyIdleActivity
{
//CrazyIdleActivity is used to test yet another angle of the meshmachine cancelling behaviour.
//What it does is:
//(1) responds to TIdle with an attempt to bind the sender, then, without waiting for the bind to complete
//(2) responds to TIdle normally (by deleting the sender)
//(3) cancells the bind request
//What it tests is:
//For two related nodes and upon the teardown of their relation there is an obligation to synchronise the activities
//running on the nodes and awaiting responses from each other. This obligation is put on the requestee, i.e.: a node that perfomes
//an activity on behalf of a leaving node should gracefully terminate that activity (by having the activity respond to the requestor. 
//That obligation therefore doesn't rest on the requestor (someone, but not everyone, needs to do something).
//There's trivial race condition arising from this distribution of responsibility, i.e.: the requestor activity may decide to 
//cancel when the relation is being terminated and TCancel may hit void. Activities must be resilient to this and not send
//TCancels to disappearing nodes. CrazyIdleActivity tests precislely this: it initaites an activity (TBindTo), then tears down the
//relation and cancels the previously initiated activity (TBindTo). CrazyIdleActivity must wrap up gracefully. 
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDataClientIdle, crazyIdle, TCFControlProvider::TIdle, DummySCPRStates::CrazyIdle::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientIdle, MeshMachine::TNoTag)
    //Issue TBindTo to self (this will then bind the data client reporting TIdle). Wait for BindTo activity to start before 
    //telling the dataclient to go away (TDestroy) as otherwise TBindTo won't make it in sending TBindTo to the dataclient.
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, DummySCPRStates::TRebindSelf, DummySCPRStates::TNoTagWaitForBindTo)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::THandleDataClientIdle, MeshMachine::TNoTag)
    //There's an additional test peformed by DummySCPRStates::CrazyIdle::TAwaitingCancelled. The state sets the postedto of CrazyIdle
    //to point at the dataclient and CrazyIdle::~CrazyIdle asserts that that postedto is cleared. Currently PostedTo isn't cleared automatically - some people
    //argued that it shouldn't and that activities should clear postedto whenever it's good for them. Unfortunatelly some activities are
    //a bit lousy doing this and in result they can store a postedto pointing a node that has responded to the request a long time ago
    //and above all a node that may have a long time ago left. A facility has been added to meshmachine to clear postedto when the node pointed
    //at it actually leaves. This facility is tested collectivelly by DummySCPRStates::CrazyIdle::TAwaitingCancelled and CrazyIdle::~CrazyIdle
    NODEACTIVITY_ENTRY(KNoTag, DummySCPRStates::TCancelPreviousSelfRequest, DummySCPRStates::CrazyIdle::TAwaitingCancelled, DummySCPRStates::TNoTagWaitNoDataClients)
    LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}



// Activity Map For test-code ridden scpr
namespace DummySCPRStates
{
DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(DummySCprParamsRequest, dummySCprParamRequest)
   ACTIVITY_MAP_ENTRY(DummyBindToActivity, dummyBindTo)
   ACTIVITY_MAP_ENTRY(DummyBreakStartActivity, dummyBreakSCPrStart)   
   ACTIVITY_MAP_ENTRY(DummyStartActivity, dummySCPrStart)
   ACTIVITY_MAP_ENTRY(CrazyIdleActivity, crazyIdle)     
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}

// Activity Map For vanilla cpr
namespace VanillaDummySCPRStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
        ACTIVITY_MAP_ENTRY(CancelTestBindToActivity, CancelBindTo)   
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}

CDummySubConnectionProvider* CDummySubConnectionProvider::NewL(ESock::CSubConnectionProviderFactoryBase& aFactory)
    {
    CDummySubConnectionProvider* self = new (ELeave) CDummySubConnectionProvider(aFactory, DummySCPRStates::stateMap::Self());
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CDummySubConnectionProvider* CDummySubConnectionProvider::NewVanillaL(ESock::CSubConnectionProviderFactoryBase& aFactory)
    {
    CDummySubConnectionProvider* self = new (ELeave) CDummySubConnectionProvider(aFactory, VanillaDummySCPRStates::stateMap::Self());
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CDummySubConnectionProvider::CDummySubConnectionProvider(CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap)
:CCoreSubConnectionProvider(aFactory, aActivityMap)
	{
	LOG_NODE_CREATE(KDummySCPRTag, CDummySubConnectionProvider);
	}

CDummySubConnectionProvider::~CDummySubConnectionProvider()
	{
	LOG_NODE_DESTROY(KDummySCPRTag, CDummySubConnectionProvider);
	}


void CDummySubConnectionProvider::SetToIncomingL()
	{
 	_LIT(KName,"Test1");
 	TBuf<5> paramName(KName);

 	const int qosParams = 128;

	//RCFParameterFamilyBundle& pfb = CreateParameterBundleL();
	//RParameterFamily family = pfb.CreateFamilyL(KSubConQoSFamily); //PJLEFT
	RCFParameterFamilyBundle newBundle;
	newBundle.CreateL();
	iParameterBundle.Open(newBundle);
	RParameterFamily family = newBundle.CreateFamilyL(KSubConQoSFamily);

	// Create the generic set
	CSubConQosGenericParamSet* isubConQosGenParSet = CSubConQosGenericParamSet::NewL(family, RParameterFamily::EGranted);

    isubConQosGenParSet->SetHeaderMode(0);
    isubConQosGenParSet->SetName(paramName);
    isubConQosGenParSet->SetDownlinkBandwidth(qosParams);
    isubConQosGenParSet->SetDownLinkMaximumBurstSize(qosParams);
    isubConQosGenParSet->SetDownLinkAveragePacketSize(qosParams);
    isubConQosGenParSet->SetDownLinkDelay(qosParams);
    isubConQosGenParSet->SetDownLinkDelayVariation(qosParams);
   	isubConQosGenParSet->SetDownLinkMaximumBurstSize(qosParams);
   	isubConQosGenParSet->SetDownLinkPriority(qosParams);
   	isubConQosGenParSet->SetDownLinkMaximumPacketSize(qosParams);

   	isubConQosGenParSet->SetUpLinkAveragePacketSize(qosParams);
   	isubConQosGenParSet->SetUpLinkDelay(qosParams);
   	isubConQosGenParSet->SetUplinkBandwidth(qosParams);
   	isubConQosGenParSet->SetUpLinkDelayVariation(qosParams);
   	isubConQosGenParSet->SetUpLinkMaximumBurstSize(qosParams);
   	isubConQosGenParSet->SetUpLinkMaximumPacketSize(qosParams);
   	isubConQosGenParSet->SetUpLinkPriority(qosParams);

 	incomingStatus = ETrue;
	}








