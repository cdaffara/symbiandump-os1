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
// IPProto MCPR
//
//

/**
 @file
 @internalComponent
*/


#include <comms-infras/ss_log.h>
#include <comms-infras/coremcprstates.h>
#include <comms-infras/coremcpractivities.h>
#include <elements/nm_messages_errorrecovery.h>
#include <comms-infras/mobilitymcpractivities.h>
#include "dummypr_metaconnprov.h"
#include "dummypr_connprov.h"
#include "dummypr_mcprpubsubsubscriber.h"

#ifdef _DEBUG
#define KDummyMCprTag KESockMetaConnectionTag
_LIT8(KDummyMCprSubTag, "dummymcpr");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace MCprActivities;
using namespace Messages;
using namespace MeshMachine;



DEFINE_SMELEMENT(CDummyMCPRControlClientJoinActivity::TAddControlClient, NetStateMachine::MStateTransition, TContext)
void CDummyMCPRControlClientJoinActivity::TAddControlClient::DoL()
    {
    TCFPeer::TJoinRequest& msg = message_cast<TCFPeer::TJoinRequest>(iContext.iMessage);

    // Client type could be Messages::TClientType::EWorker (Selection Request)
    // or ESock::TCFClientType::ECtrl, possibly others but not ESock::TCFClientType::EData
    // which is handled by another activity
    ASSERT(msg.iClientType.Type() != (TUint32)TCFClientType::EData);

    iContext.Activity()->ReplaceOriginator(
            *iContext.Node().AddClientL(msg.iNodeId, TClientType(TCFClientType::ECtrl)));
    }


DEFINE_SMELEMENT(CDummyMCPRControlClientJoinActivity::TSendJoinComplete, NetStateMachine::MStateTransition, TContext)
void CDummyMCPRControlClientJoinActivity::TSendJoinComplete::DoL()
    {
    ASSERT(iContext.iNodeActivity);
    
    iContext.iNodeActivity->PostToOriginators(TCFPeer::TJoinComplete().CRef());
    }


MeshMachine::CNodeActivityBase* CDummyMCPRControlClientJoinActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
    {
    TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CDummyMCPRControlClientJoinActivity(aActivitySig, aNode, c);
    }

void CDummyMCPRControlClientJoinActivity::ReplaceOriginator(RNodeInterface& aOriginator)
    {
    iOriginators.Remove(0);
    TInt err = KErrNone;
    TRAP(err, iOriginators.AppendL(XNodePeerId(aOriginator.RecipientId(), &aOriginator)));
    ASSERT(err == KErrNone); //This test code is a wee bit lame. If executed in OOM, it may not work.
    }

namespace DummyMCPRControlClientJoinActivity
{
//DummyMCPRControlClientJoin is a special version of the core MCPR join activity that will artificially yield some time (50ms) to schedule the
//test app. This is done to test RConnection::Stop injections at various RConnection::Start stages. Production MCPRs yield a lot reading database or
//consulting other external entities, so the test MCPR must try to be representative. 
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityClientJoin, DummyMCPRControlClientJoin, TCFServiceProvider::TJoinRequest, CDummyMCPRControlClientJoinActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingControlClientJoin, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, CDummyMCPRControlClientJoinActivity::TAddControlClient, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, CDelayTimer::TSetTimerMs<50>, CDelayTimer::TAwaitingTimerExpired, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, MCprStates::TDecrementBlockingDestroy, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, CDummyMCPRControlClientJoinActivity::TSendJoinComplete)   
NODEACTIVITY_END()
}


namespace DummyMCPRActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(DummyMCPRControlClientJoinActivity, DummyMCPRControlClientJoin)
ACTIVITY_MAP_END_BASE(MobilityMCprActivities, mobilityMCprActivities)
}

CDummyMetaConnectionProvider* CDummyMetaConnectionProvider::NewL(CMetaConnectionProviderFactoryBase& aFactory,
                                                                     const TProviderInfo& aProviderInfo)
	{
	CDummyMetaConnectionProvider* self = new (ELeave) CDummyMetaConnectionProvider(aFactory,aProviderInfo,DummyMCPRActivities::stateMap::Self());
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDummyMetaConnectionProvider::CDummyMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory,
                                                               const TProviderInfo& aProviderInfo,
                                                               const MeshMachine::TNodeActivityMap& aActivityMap)
:	CMobilityMetaConnectionProvider(aFactory,aProviderInfo,aActivityMap)
	{
	LOG_NODE_CREATE(KDummyMCprTag, CDummyMetaConnectionProvider);
	}

CDummyMetaConnectionProvider::~CDummyMetaConnectionProvider()
	{
	delete iPubSubAvailability;
	delete iPubSubStopTrigger;
	LOG_NODE_DESTROY(KDummyMCprTag, CDummyMetaConnectionProvider);
	}

void CDummyMetaConnectionProvider::ConstructL()
    {
    CMobilityMetaConnectionProvider::ConstructL();
    iPubSubAvailability = CMCPrPubSubAvailability::NewL(*this, ProviderInfo().APId());
    iPubSubStopTrigger  = CMCPrPubSubStopTrigger::NewL(*this, 0);
    }

void CDummyMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CDummyMetaConnectionProvider %08x:\tReceived() aMessage=%d"),
	   this, aMessage.MessageId().MessageId()));

	TNodeContext<CDummyMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);

    User::LeaveIfError(ctx.iReturn);
	}

void CDummyMetaConnectionProvider::StartAvailabilityMonitoringL(const Messages::TNodeCtxId& aAvailabilityActivity)
    {
    if (!GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider)))
        {
        ASSERT(iPubSubAvailability!=NULL);
        RNodeInterface* peer = AddClientL(iPubSubAvailability->Id(), TClientType(TClientType::ERegistrar, TCFClientType::EAvailabilityProvider));
        iPubSubAvailability->StartAvailabilityMonitoringL(aAvailabilityActivity);
        }
    }

void CDummyMetaConnectionProvider::CancelAvailabilityMonitoring()
    {
    if (!GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider)))
        {
        ASSERT(iPubSubAvailability!=NULL);
        RemoveClient(iPubSubAvailability->Id());
        iPubSubAvailability->CancelAvailabilityMonitoring();
        }
    }

