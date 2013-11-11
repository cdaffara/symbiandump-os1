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
// TestDummy implementation file for a Connection Provider
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/mobilitycpractivities.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <es_sock.h>

#include "dummypr_metaconnprov.h"
#include "dummypr_connprov.h"
#include "dummypr_subconnprov.h"
#include "activityTest.h"

#include <elements/sd_mintercept.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestdmyprcn, "ESockTestdmyprcn");
#endif

#ifdef _DEBUG
	#define KDummyCprTag KESockConnectionTag
#endif

using namespace Messages;
using namespace ESock;
using namespace MeshMachine;
using namespace PRActivities;

static const TUint KDestroyDelay = 3000;
//-================================================
//
// States and Transitions
//
//-================================================
CDelayTimer* CDelayTimer::NewL( const Messages::TNodeId& aSender, const Messages::TNodeId& aRecipient, const Messages::TNodeSignal::TMessageId& aMessageId )
    {
    CDelayTimer* timer = new(ELeave) CDelayTimer( aSender, aRecipient, aMessageId );
    CleanupStack::PushL( timer );
    timer->ConstructL();
    CleanupStack::Pop();
    return timer;
    }

CDelayTimer::~CDelayTimer()
    { 
    Cancel();
    }
    
CDelayTimer::CDelayTimer( const Messages::TNodeId& aSender, const Messages::TNodeId& aRecipient, const Messages::TNodeSignal::TMessageId& aMessageId ) :
    CTimer( EPriorityStandard ),
    iSender(aSender),
    iRecipient(aRecipient),
    iMessageId(aMessageId)
    {
    CActiveScheduler::Add( this );
    }
    
void CDelayTimer::ConstructL()
    {
    CTimer::ConstructL();
    }
    
void CDelayTimer::RunL()
    {
    CDelayTimer::TDelayMessage msg(iMessageId);
    Messages::RClientInterface::OpenPostMessageClose(iSender, iRecipient, msg );
    delete this;
    }

void CDelayTimer::Start( TInt aIntervalInMSecs )
    {
    After( TTimeIntervalMicroSeconds32( aIntervalInMSecs * 1000 ) );
    }

CDelayTimer::TDelayMessage::TDelayMessage()
    {
    }

CDelayTimer::TDelayMessage::TDelayMessage(const TNodeSignal::TMessageId& aMessageId)
:   TSignatureBase(aMessageId)
    {
    }


DEFINE_SMELEMENT(CDelayTimer::TAwaitingTimerExpired, NetStateMachine::MState, CDelayTimer::TContext)
TBool CDelayTimer::TAwaitingTimerExpired::Accept()
    {
    return iContext.iMessage.IsMessage<Messages::TEBase::TNull>();
    }

namespace DummyCPRStates
{
DEFINE_SMELEMENT(TSetClientAsIncoming, NetStateMachine::MStateTransition, DummyCPRStates::TContext)
void TSetClientAsIncoming::DoL()
	{
    if (iContext.Node().iConnStatus == CDummyConnectionProvider::EConnIncoming)
        {
		TCFFactory::TPeerFoundOrCreated& dcJoined = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);
        Messages::ANode& incomingDCMCFNode = dcJoined.iNodeId.Node();
        CDummySubConnectionProvider& incomingDC = mcfnode_cast<CDummySubConnectionProvider>(incomingDCMCFNode);
        incomingDC.SetToIncomingL();
        }
	}

DEFINE_SMELEMENT(TCreateIncomingSCPR, NetStateMachine::MStateTransition, DummyCPRStates::TContext)
void TCreateIncomingSCPR::DoL()
	{
    if (iContext.Node().iConnStatus != CDummyConnectionProvider::EConnIncoming)
        {
        //wait forever.
        return;
        }
	TCFServiceProvider::TCommsBinderRequest& commsBinderReq = message_cast<TCFServiceProvider::TCommsBinderRequest>(iContext.iMessage);
	__ASSERT_DEBUG(commsBinderReq.iValue == TSubConnOpen::EWaitForIncoming, User::Panic(KSpecAssert_ESockTestdmyprcn, 1));
	commsBinderReq.iValue = TSubConnOpen::ECreateNew;
	PRStates::TCreateDataClient createSCPR(iContext);
	createSCPR.DoL();
	}

DEFINE_SMELEMENT(TThreeSecDelayAndPostToSelf, NetStateMachine::MStateTransition, DummyCPRStates::TContext)
void TThreeSecDelayAndPostToSelf::DoL()
    {
    CDelayTimer* delay = CDelayTimer::NewL(iContext.Node().ControlProvider()->RecipientId(), iContext.NodeId(), iContext.iMessage.MessageId() );
    delay->Start(KDestroyDelay);
    }

DEFINE_SMELEMENT(TAwaitingStart, NetStateMachine::MState, DummyCPRStates::TContext)
TBool TAwaitingStart::Accept()
	{
	const TLayerSelectionInfo* selectionInfo = static_cast<const TLayerSelectionInfo*>(
		iContext.Node().AccessPointConfig().FindExtension(TLayerSelectionInfo::TypeId()));
	ASSERT(selectionInfo); // should always be there

    if (iContext.iMessage.IsMessage<TCFServiceProvider::TStart>() && 
    	TCprConfigModifier::Is(selectionInfo->CprConfig(), TCprConfigModifier::EDontStart))
    	{
    	return ETrue;
    	}
    return EFalse;
	}

DEFINE_SMELEMENT(TAwaitingDestroy, NetStateMachine::MState, DummyCPRStates::TContext)
TBool TAwaitingDestroy::Accept()
    {
    const TLayerSelectionInfo* selectionInfo = static_cast<const TLayerSelectionInfo*>(
        iContext.Node().AccessPointConfig().FindExtension(TLayerSelectionInfo::TypeId()));
    ASSERT(selectionInfo); // should always be there

    if (iContext.iMessage.IsMessage<TEChild::TDestroy>() && 
            TCprConfigModifier::Is(selectionInfo->CprConfig(), TCprConfigModifier::ECPRWaitOnThreeSecDestroy)
            && iContext.iNode.CountActivities(ECFActivityDummyCprDestroy) == 0 ) // only accept the first destroy which will subsequently be reposted to self after 3 second
        {
        static_cast<ESock::CMMCommsProviderBase&>(iContext.iNode).MarkMeForDeletion();
        return ETrue;
        }
    return EFalse;
    }

}

//-================================================
//
// Activities
//
//-================================================
namespace DummyCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityBinderRequest, DummyCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExistingDefaultBlockedByBinderRequest)
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TCreateDataClient, CoreNetStates::TAwaitingDataClientJoin, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(CoreNetStates::KWaitForIncoming, DummyCPRStates::TCreateIncomingSCPR, MeshMachine::TAwaitingMessageState<TCFFactory::TPeerFoundOrCreated>, MeshMachine::TNoTag)

	// Below this point we need to modify the error handling approach. If we're getting a TError on TBinderResponse,
	// this means the client requesting the binder couldn't bind to it. As far as the client is concerned, this
	// activity is finished (it has flagged an error). The standard error handling will result in erroring
	// the originator. In this case we shouoldn't error the originator, instead, wrap up quietly.
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CCommsBinderActivity::TProcessDataClientCreation, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, DummyCPRStates::TSetClientAsIncoming, MeshMachine::TTag<CoreStates::KUseExisting>)

	NODEACTIVITY_ENTRY(CoreStates::KUseExisting, CCommsBinderActivity::TSendBinderResponse, CCommsBinderActivity::TAwaitingBindToComplete, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TClearError)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KWaitForIncoming, MeshMachine::TRaiseError<KErrNotSupported>)
NODEACTIVITY_END()
}

namespace DummyCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, DummyCPrStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(DummyCPRStates::TAwaitingStart, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TRaiseError<KErrAbort>)
NODEACTIVITY_END()
}

// Activity Map For test-code ridden cpr
namespace DummyCprDestroyActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityDummyCprDestroy, DummyCprDestroy, TEChild::TDestroy)
    FIRST_NODEACTIVITY_ENTRY(DummyCPRStates::TAwaitingDestroy, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, DummyCPRStates::TThreeSecDelayAndPostToSelf, MeshMachine::TAwaitingMessageState<Messages::TEChild::TLeft>, MeshMachine::TTag<PRStates::KContinue>)
    LAST_NODEACTIVITY_ENTRY(PRStates::KContinue, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

// Activity Map
namespace DummyCPRStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(DummyCprBinderRequestActivity, DummyCprBinderRequest)
   ACTIVITY_MAP_ENTRY(DummyCprStartActivity, DummyCPrStart)
   ACTIVITY_MAP_ENTRY(DummyCprDestroyActivity, DummyCprDestroy)
ACTIVITY_MAP_END_BASE(MobilityCprActivities, mobilityCprActivities)
}


// Activity Map For vanilla cpr
namespace VanillaDummyCPRStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(CancelTestBindToActivity, CancelBindTo)   
ACTIVITY_MAP_END_BASE(MobilityCprActivities, mobilityCprActivities)
}

CDummyConnectionProvider* CDummyConnectionProvider::NewL(ESock::CConnectionProviderFactoryBase& aFactory, TConnType aConnStatus)
    {
    CDummyConnectionProvider* self = new (ELeave) CDummyConnectionProvider(aFactory, DummyCPRStates::stateMap::Self(), aConnStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CDummyConnectionProvider* CDummyConnectionProvider::NewVanillaL(ESock::CConnectionProviderFactoryBase& aFactory)
    {
    CDummyConnectionProvider* self = new (ELeave) CDummyConnectionProvider(aFactory, VanillaDummyCPRStates::stateMap::Self(), CDummyConnectionProvider::EConnNoIncoming);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CDummyConnectionProvider::CDummyConnectionProvider(CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap, TConnType aConnStatus)
:	CMobilityConnectionProvider(aFactory, aActivityMap),
	TIfStaticFetcherNearestInHierarchy(this),
    iConnStatus(aConnStatus)
	{
	LOG_NODE_CREATE(KDummyCprTag, CDummyConnectionProvider);
	}

CDummyConnectionProvider::~CDummyConnectionProvider()
	{
    LOG_NODE_DESTROY(KDummyCprTag, CDummyConnectionProvider);
	}


TInt CDummyConnectionProvider::ControlL(TUint aOptionLevel, TUint /*aOptionName*/, TDes8& /*aOption*/, ESock::MPlatsecApiExt* /*aPlatsecIfc*/)
    {
    if (aOptionLevel == EConnIncoming)
        {
        iConnStatus = EConnIncoming;
        }
    else if (aOptionLevel == EConnNoIncoming)
        {
        iConnStatus = EConnNoIncoming;
        }
    return KErrNone;
    }

void CDummyConnectionProvider::ReturnInterfacePtrL(ESock::MLegacyControlApiExt*& aInterface)
    {
    aInterface = this;
    }





