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
// ss_connstates.cpp
// 
//

#include "ss_subconn.h"
#include "ss_subconnstates.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_esockactivities.h>
#include "ss_subconn.h"
#include "SS_conn.H"
#include <ss_glob.h>

#include <comms-infras/ss_nodemessages_subconn.h>
#include <comms-infras/ss_nodemessages_rejoiningprovider.h>
#include <comms-infras/ss_nodemessages_dataclient.h>

#include <elements/nm_messages_peer.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksbcnst, "ESockSSocksbcnst");
#endif

using namespace ESock;
using namespace SubSessActivities;
using namespace Messages;
using namespace MeshMachine;

#ifdef _DEBUG
_LIT (KCSubConnectionPanic,"CSubConnectionPanic");
#endif

//-=========================================================
//
//
//States
//
//
//-=========================================================

//-=========================================================
// Build stack
//-=========================================================
DEFINE_SMELEMENT(SubConnStates::TAwaitingBuildStackResponse, NetStateMachine::MState, SubConnStates::TContext)
TBool SubConnStates::TAwaitingBuildStackResponse::Accept()
	{
	return iContext.iMessage.IsMessage<TCFInternalEsock::TBuildStackResponse>();
	}

DEFINE_SMELEMENT(SubConnStates::TSendBuildStack, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendBuildStack::DoL()
	{
    // The flow request parameters are stored within the subconnection node
    RCFParameterFamilyBundleC subConnectionParameters = iContext.Node().iParameterBundle;

	// Send build stack to ourself to kick off a stack building activity
	iContext.iNodeActivity->PostRequestTo( iContext.Node().Id(),
	    TCFInternalEsock::TBuildStackRequest(subConnectionParameters).CRef());
	}

DEFINE_SMELEMENT(SubConnActivities::CBuildStack::TAwaitingBuildStack, NetStateMachine::MState, SubConnActivities::CBuildStack::TContext)
TBool SubConnActivities::CBuildStack::TAwaitingBuildStack::Accept()
	{
	return iContext.iMessage.IsMessage<TCFInternalEsock::TBuildStackRequest>();
	}

DEFINE_SMELEMENT(SubConnActivities::CNoBearer::TStoreFlowParams, NetStateMachine::MStateTransition, SubConnActivities::CNoBearer::TContext)
void SubConnActivities::CNoBearer::TStoreFlowParams::DoL()
	{
	// Store the received flow params in the activity
	TCFControlProvider::TNoBearer& noBearerMessage = message_cast<TCFControlProvider::TNoBearer>(iContext.iMessage);
    SubConnActivities::CNoBearer* noBearerActivity = static_cast<SubConnActivities::CNoBearer*>(iContext.iNodeActivity);
	noBearerActivity->SetFlowRequestParameters(noBearerMessage.iFamilyBundle);
	}

//-=========================================================
//Create
//-=========================================================

DEFINE_SMELEMENT(SubConnStates::TJoinCPR, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TJoinCPR::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    if (iContext.Node().ControlProvider().ServiceProvider() == NULL)
        {
        User::Leave(KErrNotReady);
        }

    iContext.Node().AddClientL(iContext.Node().ControlProvider().ServiceProvider()->RecipientId(), TClientType(TCFClientType::EServProvider));
    iContext.iNodeActivity->PostRequestTo(*iContext.Node().ControlProvider().ServiceProvider(),
        TCFServiceProvider::TJoinRequest(iContext.NodeId(), TCFClientType::ECtrl).CRef());
	}

DEFINE_SMELEMENT(SubConnStates::TRequestServiceProvider, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TRequestServiceProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));

    // The flow request parameters are stored within the activity itself
    SubConnActivities::CNoBearer* noBearerActivity = static_cast<SubConnActivities::CNoBearer*>(iContext.iNodeActivity);
    RCFParameterFamilyBundleC flowParams = noBearerActivity->GetFlowRequestParameters();

	// Send our service provider a comms binder carrying the necessary flow request parameters
    iContext.iNodeActivity->PostRequestTo(
		*iContext.Node().ServiceProvider(),
        TCFServiceProvider::TCommsBinderRequest(
			TSubConnOpen::ECreateNew,
			flowParams
			).CRef()
		);
	}

DEFINE_SMELEMENT(SubConnStates::TLeaveCPR, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TLeaveCPR::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    __ASSERT_DEBUG(iContext.Node().ControlProvider().ServiceProvider() != NULL, User::Panic(KSpecAssert_ESockSSocksbcnst, 1));

#ifdef _DEBUG
    TInt count = iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider), TClientType(0, TCFClientType::EActive));
	// Now the subconn had joined SCPR, and CPR. The relationship between subconn and CPR is temporary and 
	// should be terminate here in this transition. So in here make sure there's ONLY 1 SP inactive and 
	// that's supposed to be the CPR, any other cases are wrong
    __ASSERT_DEBUG(count == 1, User::Panic(KCSubConnectionPanic, KPanicNoServiceProvider));
#endif

	// Find the SP which is not EActive because EActive is not set on the CPR's flag in the first place
    RNodeInterface * cpr = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider), TClientType(0, TCFClientType::EActive));
    iContext.iNodeActivity->PostRequestTo(*cpr, TEPeer::TLeaveRequest().CRef());
    cpr->SetFlags(TCFClientType::ELeaving);
	}

//-=========================================================
//Rejoin
//-=========================================================

DEFINE_SMELEMENT(SubConnStates::TJoinTheOtherOwner, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TJoinTheOtherOwner::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    SubConnActivities::CRejoin* rejoinActivity = static_cast<SubConnActivities::CRejoin*>(iContext.iNodeActivity);
    rejoinActivity->InitialiseL(iContext.Node());

	//MZTODO: activity using this transition - and possibly the whole approach - must be redesigned
	//so that all destinations are safe! (used to be: TCtrlClientJoinRequestUnsafeDst).
    rejoinActivity->SetOldOwnerIntf(iContext.Node().AddClientL(rejoinActivity->TheOtherOwner(), TClientType(TCFClientType::EServProvider)));
	iContext.iNodeActivity->ClearPostedTo();

    RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->TheOtherOwner(),
    	TCFServiceProvider::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl)).CRef());
	}

DEFINE_SMELEMENT(SubConnStates::TLeaveTheOtherOwner, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TLeaveTheOtherOwner::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    SubConnActivities::CRejoin* rejoinActivity = static_cast<SubConnActivities::CRejoin*>(iContext.iNodeActivity);

    RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->TheOtherOwner(),
    	TEPeer::TLeaveRequest().CRef());
		
	rejoinActivity->OldOwnerIntf()->SetFlags(ESock::TCFClientType::ELeaving);
	}

DEFINE_SMELEMENT(SubConnStates::TSendRejoinDataClientRequestToOldOwner, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendRejoinDataClientRequestToOldOwner::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    SubConnActivities::CRejoin* rejoinActivity = static_cast<SubConnActivities::CRejoin*>(iContext.iNodeActivity);

    RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->OldOwner(),
    	TCFRejoiningProvider::TRejoinDataClientRequest(rejoinActivity->Flow(), rejoinActivity->NewOwner()).CRef());
	}

DEFINE_SMELEMENT(SubConnStates::TSendApplyToOldOwner, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendApplyToOldOwner::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    SubConnActivities::CRejoin* rejoinActivity = static_cast<SubConnActivities::CRejoin*>(iContext.iNodeActivity);
    RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->OldOwner(),
    	TCFScpr::TApplyRequest().CRef());
	}

DEFINE_SMELEMENT(SubConnStates::TSendApplyToNewOwner, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendApplyToNewOwner::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    SubConnActivities::CRejoin* rejoinActivity = static_cast<SubConnActivities::CRejoin*>(iContext.iNodeActivity);
    RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->NewOwner(),
    	TCFScpr::TApplyRequest().CRef());
	}

DEFINE_SMELEMENT(SubConnStates::TSendCancelToOldOwner, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendCancelToOldOwner::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    SubConnActivities::CRejoin* rejoinActivity = static_cast<SubConnActivities::CRejoin*>(iContext.iNodeActivity);

    RClientInterface::OpenPostMessageClose(iContext.NodeId(), rejoinActivity->OldOwner(),
    	TEBase::TCancel().CRef());
	}


//-=========================================================
//SetParameters
//-=========================================================
DEFINE_SMELEMENT(SubConnStates::TSendParamsToServiceProvider, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendParamsToServiceProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CSetParameters& setParamsActivity =
	    static_cast<SubConnActivities::CSetParameters&>(*iContext.iNodeActivity);

    if (iContext.Node().SubConnType() == RSubConnection::EAttachToDefault)
      {
      // SubConnections opened with EAttachToDefault require the NetworkControl capability for changing QoS
      setParamsActivity.HasCapabilityL (ECapabilityNetworkControl, "CSubConnection::SetParametersL(): EAttachToDefault requires NetworkControl");
      }
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
	    TCFScpr::TSetParamsRequest(setParamsActivity.GetParameterBundleL()).CRef());
#else
	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
	    TCFScpr::TParamsRequest(setParamsActivity.GetParameterBundleL()).CRef());
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	}

DEFINE_SMELEMENT(SubConnStates::TStoreParams, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TStoreParams::DoL()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	TCFScpr::TSetParamsResponse& paramResponse = message_cast<TCFScpr::TSetParamsResponse>(iContext.iMessage);
#else
	TCFScpr::TParamsResponse& paramResponse = message_cast<TCFScpr::TParamsResponse>(iContext.iMessage);
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if(! iContext.Node().iParameterBundle.IsNull())
    	{
		iContext.Node().iParameterBundle.Close();
    	}
    iContext.Node().iParameterBundle.Open(paramResponse.iFamilyBundle);
	}

DEFINE_SMELEMENT(SubConnStates::TSendParamRequest, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSendParamRequest::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockSSocksbcnst, 2));
    __ASSERT_DEBUG(iContext.Node().ServiceProvider(), User::Panic(KSpecAssert_ESockSSocksbcnst, 3));
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
  	    TCFScpr::TSetParamsRequest(iContext.Node().iParameterBundle).CRef());
#else
	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
  	    TCFScpr::TParamsRequest(iContext.Node().iParameterBundle).CRef());
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    }

DEFINE_SMELEMENT(SubConnStates::TNoTagOrParamsPresent, NetStateMachine::MStateFork, SubConnStates::TContext)
TInt SubConnStates::TNoTagOrParamsPresent::TransitionTag()
    {
    return iContext.Node().iParameterBundle.IsNull() ? MeshMachine::KNoTag : PRStates::KParamsPresent;
    }

DEFINE_SMELEMENT(SubConnStates::TWriteSubConnParamsLength, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TWriteSubConnParamsLength::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	__ASSERT_DEBUG( ! iContext.Node().iParameterBundle.IsNull() , User::Panic(KSpecAssert_ESockSSocksbcnst, 4));
	CESockClientActivityBase& esockActivity =
	    static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);

	//I know!!
	esockActivity.SetError(iContext.Node().iParameterBundle.IsNull() ? KErrNotReady : (TInt) iContext.Node().iParameterBundle.Length() );
	}

DEFINE_SMELEMENT(SubConnStates::TWriteSubConnParams, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TWriteSubConnParams::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	__ASSERT_DEBUG(! iContext.Node().iParameterBundle.IsNull() , User::Panic(KSpecAssert_ESockSSocksbcnst, 5));	
	CESockClientActivityBase& esockActivity =
	    static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);
	RCFParameterFamilyBundleC& pfb = iContext.Node().iParameterBundle;
	pfb.CheckOpenL();

	RBuf8 buffer;
	buffer.CreateL(pfb.Length());
	CleanupClosePushL(buffer);
	User::LeaveIfError(pfb.Store(buffer));
	#ifdef ESOCK_LOGGING_ACTIVE
		if(esockActivity.GetDesMaxLengthL(0) < buffer.Length())
			{
			LOG(ESockLog::Printf(_L("ESock: SubConnStates[%x]\tTWriteSubConnParams: client supplied buffer of size %d is too small to store parameter bundle of size %d"), this, esockActivity.GetDesMaxLengthL(0), buffer.Length()));
			}
	#endif
	esockActivity.WriteL(0, buffer);
	CleanupStack::PopAndDestroy();	// buffer
	}

//-=========================================================
//Events
//-=========================================================

DEFINE_SMELEMENT(SubConnStates::TAwaitingEventNotificationSubscription, NetStateMachine::MState, SubConnStates::TContext)
TBool SubConnStates::TAwaitingEventNotificationSubscription::Accept()
	{
    if (subsessmessage_cast<ESCEventNotificationSetup>(&iContext.iMessage) ||
        subsessmessage_cast<ESCEventAllNotifications>(&iContext.iMessage))
        {
        return ETrue;
        }
    return EFalse;
	}

DEFINE_SMELEMENT(SubConnStates::TNoTagOrAllNotifications, NetStateMachine::MStateFork, SubConnStates::TContext)
TInt SubConnStates::TNoTagOrAllNotifications::TransitionTag()
	{
    if (subsessmessage_cast<ESCEventAllNotifications>(&iContext.iMessage))
        {
        return ESCEventAllNotifications;
        }
    return MeshMachine::KNoTag;
	}

DEFINE_SMELEMENT(SubConnStates::TSetupFilteredEventNotification, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSetupFilteredEventNotification::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CEventNotification& eventNotificationActivity =
	    static_cast<SubConnActivities::CEventNotification&>(*iContext.iNodeActivity);

    eventNotificationActivity.RequestEventNotificationSetupL();
	}

DEFINE_SMELEMENT(SubConnStates::TSetupAllEventNotification, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TSetupAllEventNotification::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CEventNotification& eventNotificationActivity =
	    static_cast<SubConnActivities::CEventNotification&>(*iContext.iNodeActivity);

    eventNotificationActivity.RequestAllEventNotificationsL();
	}

DEFINE_SMELEMENT(SubConnStates::TFillInEvent, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TFillInEvent::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CEventNotification& eventNotificationActivity =
	    static_cast<SubConnActivities::CEventNotification&>(*iContext.iNodeActivity);

	if (iContext.iMessage.IsMessage<TCFSubConnControlClient::TSubConnNotification>())
    	{
        TEnqueueEvent enqueueEvent(iContext);
        enqueueEvent.DoL();
    	}

    CRefCountOwnedSubConNotification* eventRefCountOwner = NULL;
    TBool eventConsumed = EFalse;
    while (iContext.Node().iEventQueue.Deque(eventRefCountOwner))
        {
        __ASSERT_DEBUG(eventRefCountOwner && eventRefCountOwner->Ptr(), User::Panic(KSpecAssert_ESockSSocksbcnst, 6));
        eventConsumed = eventNotificationActivity.FillInEvent(*eventRefCountOwner->Ptr());
        eventRefCountOwner->Close();
        if (eventConsumed)
            {
            break;
            }
        }
	}

DEFINE_SMELEMENT(SubConnStates::TEnqueueEvent, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TEnqueueEvent::CreateOrUpdateBundleL(CSubConGenEventParamsGranted* aEvent)
    {
	RCFParameterFamilyBundleC& myBundle = iContext.Node().GetOrCreateParameterBundleL();

     RParameterFamily family=myBundle.FindFamily(KSubConQoSFamily);
	 if ( family.IsNull() )
        {
       	RCFParameterFamilyBundle newBundle;
		newBundle.CreateL();
		newBundle.Open(myBundle);
		family = newBundle.CreateFamilyL(KSubConQoSFamily);
        //family = myBundle.CreateFamilyL(KSubConQoSFamily); //PJLEFT
        newBundle.Close();
        }

	 family.ClearAllParameters (RParameterFamily::EGranted);

     CSubConGenericParameterSet* origGeneric = const_cast<CSubConGenericParameterSet*>(aEvent->GetGenericSet());
     if (origGeneric)
        {
        // A copy must be used because parameters owned by the event will be destructed
        // along with it.
        CSubConGenericParameterSet* copyGeneric = static_cast<CSubConGenericParameterSet*>(CSubConGenericParameterSet::NewL (origGeneric->GetTypeId()));
        CleanupStack::PushL (copyGeneric);
        copyGeneric->Copy (*origGeneric);
        family.AddParameterSetL (copyGeneric, RParameterFamily::EGranted);
        CleanupStack::Pop (copyGeneric);
        }

     TInt max = aEvent->GetNumExtensionSets();
     for (TInt i=0; i<max; i++)
        {
        CSubConExtensionParameterSet* origExtension = const_cast<CSubConExtensionParameterSet*>(aEvent->GetExtensionSet (i));
        CSubConExtensionParameterSet* copyExtension = static_cast<CSubConExtensionParameterSet*>(CSubConExtensionParameterSet::NewL(origExtension->GetTypeId()));
        CleanupStack::PushL (copyExtension);
        copyExtension->Copy (*origExtension);
        family.AddParameterSetL(copyExtension, RParameterFamily::EGranted);
        CleanupStack::Pop (copyExtension);
        }
    }


void SubConnStates::TEnqueueEvent::DoL()
	{
	TCFSubConnControlClient::TSubConnNotification& eventMsg = message_cast<TCFSubConnControlClient::TSubConnNotification>(iContext.iMessage);
	__ASSERT_DEBUG(eventMsg.iRefCountOwnedSubConNotification, User::Panic(KSpecAssert_ESockSSocksbcnst, 7));
	__ASSERT_DEBUG(eventMsg.iRefCountOwnedSubConNotification->Ptr(), User::Panic(KSpecAssert_ESockSSocksbcnst, 8));
	iContext.Node().iEventQueue.Enque(eventMsg.iRefCountOwnedSubConNotification);
    const CSubConNotificationEvent* event = eventMsg.iRefCountOwnedSubConNotification->Ptr();
	//If param related event - update the params.
    if (event->GroupId() == KSubConnGenericEventsImplUid
      && (event->Id() == KSubConGenericEventParamsGranted
         || event->Id() == KSubConGenericEventParamsChanged))
        {
        CreateOrUpdateBundleL(const_cast<CSubConGenEventParamsGranted*>(static_cast<const CSubConGenEventParamsGranted*>(event)));
        }
	}

DEFINE_SMELEMENT(SubConnStates::TNoTagOrActiveWhenEventEnqued, NetStateMachine::MStateFork, SubConnStates::TContext)
TInt SubConnStates::TNoTagOrActiveWhenEventEnqued::TransitionTag()
	{
	if (!iContext.Node().iEventQueue.IsEmpty())
    	{
    	return KActiveTag;
    	}
    return MeshMachine::KNoTag;
	}


//-=========================================================
//Start/Stop Connection
//-=========================================================
DEFINE_SMELEMENT(SubConnStates::TStartSubConnection, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TStartSubConnection::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    if (iContext.Node().ServiceProvider() == NULL)
        {
        User::Leave(KErrNotReady);
        }
    iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
        TCFServiceProvider::TStart().CRef());
	}

DEFINE_SMELEMENT(SubConnStates::TStopSubConnection, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TStopSubConnection::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
    if (iContext.Node().ServiceProvider() == NULL)
        {
        iContext.iNodeActivity->SetIdle();
        }
    else
        {
        iContext.iNodeActivity->PostRequestTo(*iContext.Node().ServiceProvider(),
            TCFServiceProvider::TStop(KErrCancel).CRef());
        }
	}

//-=========================================================
//Close
//-=========================================================
DEFINE_SMELEMENT(SubConnStates::TDetachControlProvider, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnStates::TDetachControlProvider::DoL()
	{
   	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrlProvider));
	if (iter[0])
    	{
#if defined(__GCCXML__)
    	CConnection& conn = *reinterpret_cast<CConnection*>(&(iter[0]->RecipientId().Node()));
#else
    	CConnection& conn = mcfnode_cast<CConnection>(iter[0]->RecipientId().Node());
#endif
    	iContext.Node().RemoveClient(conn.Id());
    	conn.RemoveClient(iContext.NodeId());
    	}
	}
//-=========================================================
//
//
//Activities
//
//
//-=========================================================
//-=========================================================
//CNoBearer
//-=========================================================
MeshMachine::CNodeActivityBase* SubConnActivities::CNoBearer::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    SubConnActivities::CNoBearer* noBearerActivity = new SubConnActivities::CNoBearer(aActivitySig, aNode);
    if (noBearerActivity == NULL)
        {
    	User::Leave(KErrNoMemory);
        }
    return noBearerActivity;
    }

SubConnActivities::CNoBearer::~CNoBearer()
    {
    //Destroy the subconnection if the ownership hasn't been released!
    iFlowRequestBundle.Close();
	if(Error() != KErrNone)
		{
		PostToOriginators(TEBase::TError(KickOffMessageId(), Error()).CRef());
		}
    }

void SubConnActivities::CNoBearer::SetFlowRequestParameters(const ESock::RCFParameterFamilyBundleC& aParams)
    {
    if(!iFlowRequestBundle.IsNull())
    	{
    	iFlowRequestBundle.Close();
    	}
    iFlowRequestBundle.Open(aParams);
    }

//-=========================================================
//CBuildStack
//-=========================================================
MeshMachine::CNodeActivityBase* SubConnActivities::CBuildStack::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    SubConnActivities::CBuildStack* createActivity = new SubConnActivities::CBuildStack(aActivitySig, aNode);
    if (createActivity == NULL)
        {
    	User::Leave(KErrNoMemory);
        }
    return createActivity;
    }

SubConnActivities::CBuildStack::CBuildStack(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	MeshMachine::CNodeActivityBase(aActivitySig, aNode),
		CoreActivities::ABindingActivity(aNode.Id()),
		TIfStaticFetcherNearestInHierarchy(this)
		{
		}

SubConnActivities::CBuildStack::~CBuildStack()
    {
    }

DEFINE_SMELEMENT(SubConnActivities::CBuildStack::TSendBuildStackResponse, NetStateMachine::MStateTransition, CBuildStack::TContext)
void SubConnActivities::CBuildStack::TSendBuildStackResponse::DoL()
	{
	iContext.iNodeActivity->PostToOriginators(
	    TCFInternalEsock::TBuildStackResponse().CRef());
	}

DEFINE_SMELEMENT(SubConnActivities::CBuildStack::TRequestServiceProviderFromCPR, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnActivities::CBuildStack::TRequestServiceProviderFromCPR::DoL()
	{
	// This transition sends a request for a comms binder to the CPR
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CBuildStack& buildStackActivity =
		static_cast<SubConnActivities::CBuildStack&>(*iContext.iNodeActivity);

	// The activity is a friend of the subconn so lets call on it to do the job
	buildStackActivity.RequestServiceProviderFromCPRL(iContext);
	}

void SubConnActivities::CBuildStack::RequestServiceProviderFromCPRL(TContext& aContext)
	{
    if (aContext.Node().ControlProvider().ServiceProvider() == NULL)
        {
        User::Leave(KErrNotReady);
        }

	// Post the request to our controls provider's service provider (that the top level CPR that manages our connection)
	aContext.iNodeActivity->PostRequestTo(
		*aContext.Node().ControlProvider().ServiceProvider(),
		TCFServiceProvider::TCommsBinderRequest(aContext.Node().iSubConnType,
			aContext.Node().iParameterBundle
			).CRef()
		);
	}


//-=========================================================
//CCreate
//-=========================================================
MeshMachine::CNodeActivityBase* SubConnActivities::CCreate::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    SubConnActivities::CCreate* createActivity = new SubConnActivities::CCreate(aActivitySig, aNode);
    if (createActivity == NULL)
        {
#ifndef __GCCXML__
		RClientInterface::OpenPostMessageClose(aNode.Id(), aNode.Id(), TCFInternalEsock::TSubSess(ESCClose,RMessage2()).CRef());
#endif
    	User::Leave(KErrNoMemory);
        }
    return createActivity;
    }

SubConnActivities::CCreate::~CCreate()
    {
    //Destroy the subconnection if the ownership hasn't been released!
    if (Error() != KErrNone)
    	{
    	RClientInterface::OpenPostMessageClose(iSubConnectionNode, iSubConnectionNode,
    		TCFInternalEsock::TSubSess(ESCClose,RMessage2()).CRef());
    	}
    }

DEFINE_SMELEMENT(SubConnStates::TNoTagOrWaitForIncoming, NetStateMachine::MStateFork, SubConnStates::TContext)
TInt SubConnStates::TNoTagOrWaitForIncoming::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	CESockClientActivityBase& ac = static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);

	TPckgBuf<TSubConnOpen> argPkg;
	ac.ReadL(0,argPkg);
	iContext.Node().iSubConnType = static_cast<RSubConnection::TSubConnType>(argPkg().iType);
	if (iContext.Node().iSubConnType == RSubConnection::EWaitIncoming)
    	{
    	return CoreNetStates::KWaitForIncoming;
    	}
	return MeshMachine::KNoTag;
 	}

DEFINE_SMELEMENT(SubConnStates::TNoTagOrAttachToDefaultOrWaitForIncoming, NetStateMachine::MStateFork, SubConnStates::TContext)
TInt SubConnStates::TNoTagOrAttachToDefaultOrWaitForIncoming::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	CESockClientActivityBase& ac = static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);

	TPckgBuf<TSubConnOpen> argPkg;
	ac.ReadL(0,argPkg);
	iContext.Node().iSubConnType = static_cast<RSubConnection::TSubConnType>(argPkg().iType);
	if (iContext.Node().iSubConnType == RSubConnection::EWaitIncoming)
    	{
    	return CoreNetStates::KWaitForIncoming;
    	}
    else if (iContext.Node().iSubConnType == RSubConnection::EAttachToDefault)
    	{
    	return CoreNetStates::KAttachToDefault;
		}
	return MeshMachine::KNoTag;
 	}

//-=========================================================
//CRejoin
//-=========================================================
MeshMachine::CNodeActivityBase* SubConnActivities::CRejoin::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) SubConnActivities::CRejoin(aActivitySig, aNode);
    }

SubConnActivities::CRejoin::~CRejoin()
    {
    }

void SubConnActivities::CRejoin::InitialiseL(CSubConnection& aSC)
    {
    if (!iNewOwner.IsNull())
        {
        __ASSERT_DEBUG(!iOldOwner.IsNull() && !iFlow.IsNull(), User::Panic(KSpecAssert_ESockSSocksbcnst, 9));
        return;
        }

    if (aSC.ServiceProvider() == NULL)
        {
        User::Leave(KErrNotReady);
        }

    __ASSERT_DEBUG(aSC.Session(), User::Panic(KSpecAssert_ESockSSocksbcnst, 10));
    if (!aSC.Session()->FlowAndSCPRFromSocketHandle(iMessage.Int0(), iFlow, iOldOwner))
    	{
    	User::Leave(KErrNotFound);
    	}

    if (iMessage.Function() == ESCRemoveSocket)
        {
    	RNodeInterface* conn = aSC.ControlProvider().ServiceProvider();
    	__ASSERT_DEBUG(conn, User::Panic(KSpecAssert_ESockSSocksbcnst, 11));
    	User::LeaveIfError(conn? KErrNone : KErrArgument);

#if defined(__GCCXML__)
    	CConnectionProviderBase& p = *reinterpret_cast<CConnectionProviderBase*>(&(conn->RecipientId().Node()));
#else
    	CConnectionProviderBase& p = mcfnode_cast<CConnectionProviderBase>(conn->RecipientId().Node());
#endif
    	RNodeInterface* primarySc = p.DefaultSubConnectionProvider();
    	__ASSERT_DEBUG( primarySc , User::Panic(KSpecAssert_ESockSSocksbcnst, 12));

        iNewOwner = primarySc->RecipientId();
        }
    else if (iMessage.Function() == ESCAddSocket)
        {
        iNewOwner = aSC.ServiceProvider()->RecipientId();
        }
    else
        {
        __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksbcnst, 13));
        }
    if (iNewOwner == iOldOwner)
        {
        User::Leave(KErrAlreadyExists);
        }
    }

//-=========================================================
//CSetParameters
//-=========================================================
MeshMachine::CNodeActivityBase* SubConnActivities::CSetParameters::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) SubConnActivities::CSetParameters(aActivitySig, aNode);
    }

SubConnActivities::CSetParameters::~CSetParameters()
    {
    if (! iParameterBundle.IsNull())
        {
	    iParameterBundle.Close();
        }
    }

RCFParameterFamilyBundleC& SubConnActivities::CSetParameters::GetParameterBundleL()
    {
    if ( ! iParameterBundle.IsNull() )
        {
        return iParameterBundle;
        }
    //Extract the parameter bundle.
	TInt length = iMessage.GetDesLengthL(0);
	RBuf8 buffer;
	buffer.CreateL(length);
	CleanupClosePushL(buffer);
	iMessage.ReadL(0, buffer);

	iParameterBundle.LoadL(buffer);
	CleanupStack::PopAndDestroy(&buffer);
	return iParameterBundle;
    }

void SubConnActivities::CSetParameters::CompleteMessage()
    {
	LOG(ESockLog::Printf(_L("SubConnActivities::CSetParameters::CompleteMessage - (%08X) with %d"), iMessage.Handle(), Error()));
	
	__ASSERT_DEBUG(!iMessage.IsNull(), User::Panic(KSpecAssert_ESockSSocksbcnst, 14));
	iMessage.Complete(Error());
    }

DEFINE_SMELEMENT(SubConnActivities::CSetParameters::TCompleteClient, NetStateMachine::MStateTransition, TContext)
void SubConnActivities::CSetParameters::TCompleteClient::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CSetParameters& setparamActivity = static_cast<SubConnActivities::CSetParameters&>(*iContext.Activity());
    setparamActivity.CompleteMessage();
	}
DEFINE_SMELEMENT(SubConnActivities::CSetParameters::TStoreNewParams, NetStateMachine::MStateTransition, SubConnStates::TContext)
void SubConnActivities::CSetParameters::TStoreNewParams::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCSubConnectionPanic,KPanicNoActivity));
	SubConnActivities::CSetParameters& setParamsActivity =
		static_cast<SubConnActivities::CSetParameters&>(*iContext.iNodeActivity);

	// Store the parameters in the subconnection node
	setParamsActivity.StoreParametersInSubConn(iContext);
	}

void SubConnActivities::CSetParameters::StoreParametersInSubConn(TContext& aContext)
	{
	// Store the parameters in the CSubConnection node
	if (!aContext.Node().iParameterBundle.IsNull())
    	{
		aContext.Node().iParameterBundle.Close();
    	}
    aContext.Node().iParameterBundle.Open(GetParameterBundleL());
    aContext.Node().iParameterBundle.Open();
	}

//-=========================================================
//CEventNotification
//-=========================================================
MeshMachine::CNodeActivityBase* SubConnActivities::CEventNotification::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) SubConnActivities::CEventNotification(aActivitySig, aNode);
    }

SubConnActivities::CEventNotification::~CEventNotification()
    {
    }

void SubConnActivities::CEventNotification::RequestEventNotificationSetupL()
    {
	if (iMessage.Ptr0())
		{
		TPtr8 ptr((TUint8*)iEventUidFilterList, KMaxUidFilterListLen*sizeof(RSubConnection::TEventFilter));
		TInt ret = iMessage.Read(0, ptr, 0);
		iEventUidFilterListLength = ptr.Length() / sizeof(RSubConnection::TEventFilter);

		if (ret != KErrNone)
			{
			// Null filter list
			Mem::FillZ ((void*)iEventUidFilterList, KMaxUidFilterListLen*sizeof(RSubConnection::TEventFilter));
			iEventUidFilterListLength = 0;
			}
		}
	else
		{
		// Null filter list
		Mem::FillZ ((void*)iEventUidFilterList, KMaxUidFilterListLen*sizeof(RSubConnection::TEventFilter));
		iEventUidFilterListLength = 0;
		}
    }

void SubConnActivities::CEventNotification::RequestAllEventNotificationsL()
    {
    RSubConnection::TEventFilter & filter = iEventUidFilterList[0];
    if ( iMessage.Int1() ) //ie request only generic events
        {
        filter.iEventGroupUid = KSubConnGenericEventsImplUid;
        filter.iEventMask = 0xffffffff;
        iEventUidFilterListLength = 1;
        }
    else
        {
        iEventUidFilterListLength = 0;
        }
    }


TBool SubConnActivities::CEventNotification::FillInEvent(const CSubConNotificationEvent& aEvent)
    {
	TBool sendEvent(iEventUidFilterListLength == 0);
	for (TUint i = 0; i < iEventUidFilterListLength; ++i)
		{
		if (iEventUidFilterList[i].iEventGroupUid == aEvent.GroupId() &&
			iEventUidFilterList[i].iEventMask & aEvent.Id())
			{
			sendEvent = ETrue;
			break;
			}
		}
	if (sendEvent)
		{
		TBuf8<KMaxSubConnectionEventSize> eventBuffer;
		TPtr8 ptr((TUint8*)eventBuffer.Ptr(),eventBuffer.MaxLength());
		if (aEvent.Store(ptr) == KErrNone)
			{
			LOG(ESockLog::Printf(_L("ESock: CSubConnection[%x]: Notification Sent.  Event Type %d"), this, aEvent.Id()));
			eventBuffer.SetLength(ptr.Length());
			TInt err = iMessage.Write(0, eventBuffer);
			}
		}
	return sendEvent;
    }



