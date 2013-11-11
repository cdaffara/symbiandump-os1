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
// Core CPR States
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
//
//
//

/**
 @file
 @internalComponent
*/

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_subconnprov.h>
#include <ss_glob.h>
#include <comms-infras/ss_metaconnprov_internal.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/corecpractivities.h>
#include <elements/nm_messages_child.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_mcpr.h>
#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/upsmessages.h>
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCoreProvcrc, "ESockCrPrvcrc");
#endif

using namespace CprStates;
using namespace NetStateMachine;
using namespace ESock;
using namespace CorePanics;
using namespace Messages;

#ifdef SYMBIAN_NETWORKING_UPS
_LIT_SECURITY_POLICY_C1(KCprStartSecurityPolicy, ECapabilityNetworkServices);
#endif

//-=========================================================
//
// Panics
//
//-=========================================================
#ifdef _DEBUG
_LIT (KCoreCprPanic,"CoreCprPanic");
void CprPanic(TInt aCode)
	{
	User::Panic(KCoreCprPanic, aCode);
	}
#endif

//-=========================================================
//
//States
//
//-=========================================================

//-=========================================================
//
//State Forks
//
//-=========================================================
EXPORT_DEFINE_SMELEMENT(TCreateAdditionalDataClientOrDataClientReady, NetStateMachine::MStateFork, CprStates::TContext)
EXPORT_C TInt TCreateAdditionalDataClientOrDataClientReady::TransitionTag()
	{
 	CCoreConnectionProvider& node = iContext.Node();

 	const CConfigAccessPointConfig *capc = static_cast<const CConfigAccessPointConfig*>
 		(node.AccessPointConfig().FindExtension(
 			STypeId::CreateSTypeId(CConfigAccessPointConfig::EUid,CConfigAccessPointConfig::ETypeId)));

 	if (capc == NULL)
 		return CoreNetStates::KDataClientReady;

	TInt clientcount = iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EConfigAccessPoint));

 	if (clientcount < capc->SCprs().Count())
 		return KCreateAdditionalDataClient;

	return CoreNetStates::KDataClientReady;
	}

EXPORT_DEFINE_SMELEMENT(TCreateAdditionalDataClientBackwardOrDataClientReady, NetStateMachine::MStateFork, CprStates::TContext)
EXPORT_C TInt TCreateAdditionalDataClientBackwardOrDataClientReady::TransitionTag()
 	{
  	TInt ret = TCreateAdditionalDataClientOrDataClientReady::TransitionTag();
  	if (ret == KCreateAdditionalDataClient)
  		return ret | NetStateMachine::EBackward;
	else
 		return ret;
 	}

//-=========================================================
//
//Transitions
//
//-=========================================================
/**
   If we have no control clients, we have no way of being stopped
   so report selves as stopped, and let mcpr destroy us when we go idle */
EXPORT_DEFINE_SMELEMENT(TSendDataClientStatusStoppedIfNoControlClient, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void TSendDataClientStatusStoppedIfNoControlClient::DoL()
	{
	TInt controlclients = iContext.Node().CountClients<TDefaultClientMatchPolicy>(
			TClientType(TCFClientType::ECtrl)
			);

	if (controlclients == 0)
		{
		/* Notify our control provider that we are also stopped */
		iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
				TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
				TCFControlProvider::TDataClientStatusChange(TCFControlProvider::TDataClientStatusChange::EStopped).CRef(),
				TClientType(TCFClientType::ECtrlProvider)
			);
		}
	}

EXPORT_DEFINE_SMELEMENT(TAddControlClient, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TAddControlClient::DoL()
	{
	if (iContext.iMessage.IsMessage<TCFPeer::TJoinRequest>())
		{
		TCFPeer::TJoinRequest& msg = message_cast<TCFPeer::TJoinRequest>(iContext.iMessage);

		// Client type could be Messages::TClientType::EWorker (Selection Request)
		// or ESock::TCFClientType::ECtrl, possibly others but not ESock::TCFClientType::EData
		// which is handled by another activity
		__ASSERT_DEBUG(msg.iClientType.Type() != (TUint32)TCFClientType::EData, User::Panic(KSpecAssert_ESockCoreProvcrc, 1));
		if (msg.iValue != 0)
			{
			iContext.Node().AddClientL(msg.iNodeId, TClientType(TCFClientType::ECtrl), &msg.iValue);
			}
		else
			{
			iContext.Node().AddClientL(msg.iNodeId, TClientType(TCFClientType::ECtrl));
			}
		}
	else
		{
	 	TCFFactory::TPeerFoundOrCreated& msg = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);
		iContext.Node().AddClientL(address_cast<TNodeId>(iContext.iSender), TClientType(TCFClientType::ECtrl, msg.iValue));
		}
	}

namespace CprPriorityUpdateActivity
{

EXPORT_DEFINE_SMELEMENT(CPriorityUpdateActivity::TNoTagOrSendPriorityToCtrlProvider, NetStateMachine::MStateFork, CprStates::TContext)
EXPORT_C TInt CPriorityUpdateActivity::TNoTagOrSendPriorityToCtrlProvider::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CPriorityUpdateActivity& activity = static_cast<CPriorityUpdateActivity&>(*iContext.iNodeActivity);

	TUint prevPriority = activity.Priority();
	activity.SetPriority(iContext.Node().Priority());

	if (prevPriority != activity.Priority())
		{
		return KSendPriorityToCtrlProvider;
		}
	else
		{
		return MeshMachine::KNoTag;
		}
	}

EXPORT_DEFINE_SMELEMENT(CPriorityUpdateActivity::TNoTagOrSendPriorityToServProvider, NetStateMachine::MStateFork, CprStates::TContext)
EXPORT_C TInt CPriorityUpdateActivity::TNoTagOrSendPriorityToServProvider::TransitionTag()
	{
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider), TClientType(0, TCFClientType::ELeaving));
	if (iter[0])
		{
		return KSendPriorityToServProvider;
		}
	else
		{
		return MeshMachine::KNoTag;
		}
	}
EXPORT_DEFINE_SMELEMENT(CPriorityUpdateActivity::TUpdatePriorityForControlProvider, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void CPriorityUpdateActivity::TUpdatePriorityForControlProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CPriorityUpdateActivity& activity = static_cast<CPriorityUpdateActivity&>(*iContext.iNodeActivity);
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrlProvider));
	RNodeInterface* controlProvider = iter[0];
	if (controlProvider)
		{
		__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
		controlProvider->PostMessage(iContext.NodeId(), TCFControlProvider::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::EData), activity.Priority()).CRef());
		}
	}

EXPORT_DEFINE_SMELEMENT(CPriorityUpdateActivity::TUpdatePriorityForServiceProviders, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void CPriorityUpdateActivity::TUpdatePriorityForServiceProviders::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CPriorityUpdateActivity& activity = static_cast<CPriorityUpdateActivity&>(*iContext.iNodeActivity);
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
		iContext.NodeId(),
		TCFPeer::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl), activity.Priority()).CRef(),
			TClientType(TCFClientType::EServProvider)
		);
	}

}

namespace CprControlClientJoinActivity
{

EXPORT_DEFINE_SMELEMENT(CControlClientJoinActivity::TAddClientOrUpdatePriority, NetStateMachine::MStateFork, TContext)
EXPORT_C TInt CControlClientJoinActivity::TAddClientOrUpdatePriority::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CControlClientJoinActivity& activity = static_cast<CControlClientJoinActivity&>(*iContext.iNodeActivity);
	activity.SaveJoinMessageData(iContext);

	TInt count = iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl, 0));
	if (count == 0)
		{
		activity.SetPriority(KMaxTUint);
		}
	else
		{
		activity.SetPriority(iContext.Node().Priority());
		}

	RCFNodePriorityInterface* client = static_cast<RCFNodePriorityInterface*>(iContext.Node().FindClient(iContext.iSender));
	TCFPeer::TJoinRequest* msg = message_cast<TCFPeer::TJoinRequest>(&iContext.iMessage);
	if (client && msg)
		{
		client->SetPriority(msg->iValue);
		}

	if (activity.UpdatePriority())
		{
		return KUpdatePriority;
		}
	else
		{
		return KAddClient;
		}
	}

EXPORT_DEFINE_SMELEMENT(CControlClientJoinActivity::TNoTagOrSendActive, NetStateMachine::MStateFork, TContext)
EXPORT_C TInt CControlClientJoinActivity::TNoTagOrSendActive::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CControlClientJoinActivity& activity = static_cast<CControlClientJoinActivity&>(*iContext.iNodeActivity);
	if (activity.UpdatePriority())
		{
		return MeshMachine::KNoTag;
		}
	else
		{
		return KSendActive;
		}
	}


EXPORT_DEFINE_SMELEMENT(CControlClientJoinActivity::TSendJoinCompleteIfRequest, NetStateMachine::MStateTransition, TContext)
EXPORT_C void CControlClientJoinActivity::TSendJoinCompleteIfRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CControlClientJoinActivity& activity = static_cast<CControlClientJoinActivity&>(*iContext.iNodeActivity);
	// Send confirmation
	if (activity.NeedToSendJoinComplete())
		{
		Messages::TNodePeerId& peerId = activity.iOriginators[0];
		RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), peerId.RecipientId(), TCFPeer::TJoinComplete().CRef());
		}

	}
} //CprControlClientJoinActivity

namespace CprClientLeaveActivity
{
EXPORT_DEFINE_SMELEMENT(CClientLeaveActivity::TSendLeaveCompleteIfRequest, NetStateMachine::MStateTransition, CClientLeaveActivity::TContext)
EXPORT_C void CClientLeaveActivity::TSendLeaveCompleteIfRequest::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CClientLeaveActivity& activity = static_cast<CClientLeaveActivity&>(*iContext.iNodeActivity);
	if (activity.NeedToSendLeaveComplete())
		{
		Messages::TNodePeerId& peerId = activity.iOriginators[0];
		RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), peerId.RecipientId(), TCFPeer::TLeaveComplete().CRef());
		}

	}

EXPORT_DEFINE_SMELEMENT(CClientLeaveActivity::TSaveLeaveMessageData, NetStateMachine::MStateTransition, CClientLeaveActivity::TContext)
EXPORT_C void CClientLeaveActivity::TSaveLeaveMessageData::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CClientLeaveActivity& activity = static_cast<CClientLeaveActivity&>(*iContext.iNodeActivity);
	activity.SaveLeaveMessageData(iContext);
	activity.SetPriority(iContext.Node().Priority());
	}

} //CprClientLeaveActivity

namespace CprBindToActivity
{
EXPORT_DEFINE_SMELEMENT(CCprBindToActivity::TSendControlClientJoinRequestWithPriority, NetStateMachine::MStateTransition, CCprBindToActivity::TContext)
void CCprBindToActivity::TSendControlClientJoinRequestWithPriority::DoL()
    {
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreCprPanic, KPanicNoActivity));
	CprBindToActivity::CCprBindToActivity& activity = static_cast<CprBindToActivity::CCprBindToActivity&>(*iContext.iNodeActivity);
	const TCFDataClient::TBindTo& bindToMsg(message_cast<const TCFDataClient::TBindTo>(iContext.iMessage));

	__ASSERT_DEBUG(!bindToMsg.iNodeId.IsNull(), User::Panic(KCoreCprPanic, KPanicNoServiceProvider));

    RNodeInterface* newServiceProvider = iContext.Node().AddClientL(bindToMsg.iNodeId,
                            TClientType(TCFClientType::EServProvider, TCFClientType::EActivating));
    __ASSERT_DEBUG(newServiceProvider, User::Panic(KCoreCprPanic, KPanicNoServiceProvider));
    activity.iNewServiceProvider = bindToMsg.iNodeId;
    //Join the new service provider
    activity.PostRequestTo(*newServiceProvider,
                            TCFControlClient::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::ECtrl), iContext.Node().Priority()).CRef());
    }
}

EXPORT_DEFINE_SMELEMENT(THandleDataClientIdle, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void THandleDataClientIdle::DoL()
	{
	/** if we've lost our control client, and still have idle data clients to which are started
		we need to stop them. otherwise they'll be left hanging */
	TInt controlclients = iContext.Node().CountClients<TDefaultClientMatchPolicy>(
			TClientType(TCFClientType::ECtrl)
			);
	if (controlclients)
		{
		CoreNetStates::THandleDataClientIdle(iContext).DoL();
		}
	else
		{
		/* send destroy to all non-active dataclients */
		TInt activedataclients = iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EActive))
 			+ iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EActivating));

		__ASSERT_DEBUG(activedataclients >= 0, User::Panic(KSpecAssert_ESockCoreProvcrc, 2));
		if (activedataclients == 0)
			{
			/* destroy all data clients, which haven't already been destroyed */
			TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(
					TClientType(TCFClientType::EData),
					TClientType(0, TClientType::ELeaving)
					);
			for (TInt i = 0; iter[i]; i++)
				{
				iter[i]->PostMessage(
					iContext.NodeId(),
					TEChild::TDestroy().CRef()
					);
				iter[i]->SetFlags(TClientType::ELeaving);
				}
			}

		/* Notify our control provider that we are also stopped */
		iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
				TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
				TCFControlProvider::TDataClientStatusChange(TCFControlProvider::TDataClientStatusChange::EStopped).CRef(),
				TClientType(TCFClientType::ECtrlProvider)
				);
		}
	}

EXPORT_DEFINE_SMELEMENT(TCreateAdditionalDataClient, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void TCreateAdditionalDataClient::DoL()
 	{
 	__ASSERT_DEBUG(iContext.iNodeActivity, CprPanic(KPanicNoActivity));

 	const ESock::CConfigAccessPointConfig* capc = static_cast<const ESock::CConfigAccessPointConfig*>(
 		iContext.Node().AccessPointConfig().FindExtension(
 			STypeId::CreateSTypeId(CConfigAccessPointConfig::EUid,CConfigAccessPointConfig::ETypeId)));

	// Number of additional SCPRs started should always be at least 1 less than there is to be started
 	const RArray<TUid>& scprs = capc->SCprs();
	TInt clientcount = iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EConfigAccessPoint));
 	__ASSERT_DEBUG(clientcount < scprs.Count(), User::Panic(KSpecAssert_ESockCoreProvcrc, 3));

 	const TUid scpruid = scprs[ clientcount ];

	TDefaultSCPRFactoryQuery query(iContext.Node().Id(), TSubConnOpen::ECreateNew);

	TCFFactory::TFindOrCreatePeer msg(TCFPlayerRole::ESubConnPlane, scpruid, &query);
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->iCommsFactoryContainerBroker, msg, EFalse);
	}

EXPORT_DEFINE_SMELEMENT(TBindToPrimaryDataClient, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void TBindToPrimaryDataClient::DoL()
 	{
 	TCFFactory::TPeerFoundOrCreated* joinMsg = message_cast<TCFFactory::TPeerFoundOrCreated>(&iContext.iMessage);

 	Messages::TNodeId id = joinMsg->iNodeId;
	RNodeInterface* client = iContext.Node().FindClient(id);
	__ASSERT_DEBUG(client, User::Panic(KSpecAssert_ESockCoreProvcrc, 4));
	client->SetFlags(TCFClientType::EConfigAccessPoint);
	RNodeInterface* dc = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EDefault));
	__ASSERT_DEBUG(dc, User::Panic(KSpecAssert_ESockCoreProvcrc, 5)); // primary dataclient *must* already exist

	TNodeId primaryid = dc->RecipientId();

 	iContext.iNodeActivity->PostRequestTo(id, TCFDataClient::TBindTo(primaryid).CRef());
 	}


EXPORT_DEFINE_SMELEMENT(CprDataClientStartActivity::TNoTagOrNoTagBackward, NetStateMachine::MStateFork, CprDataClientStartActivity::TContext)
EXPORT_C TInt CprDataClientStartActivity::TNoTagOrNoTagBackward::TransitionTag()
	{
	CprDataClientStartActivity::CDataClientStartActivity* const act
		= static_cast<CprDataClientStartActivity::CDataClientStartActivity*>(iContext.iNodeActivity);

	if (*act->iClientIter)
		return MeshMachine::KNoTag | NetStateMachine::EBackward;
	else
		return MeshMachine::KNoTag;
	}

EXPORT_DEFINE_SMELEMENT(CprDataClientStartActivity::TStartDataClient, NetStateMachine::MStateTransition, CprStates::TContext)
EXPORT_C void CprDataClientStartActivity::TStartDataClient::DoL()
 	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCoreProvcrc, 6));
	CprDataClientStartActivity::CDataClientStartActivity* const act
		= static_cast<CprDataClientStartActivity::CDataClientStartActivity*>(iContext.iNodeActivity);

	RNodeInterface* const client = *act->iClientIter;
	iContext.iNodeActivity->PostRequestTo(*client, TCFDataClient::TStart().CRef());
	client->SetFlags(TCFClientType::EStarting);
	act->iClientIter++;
	}

#ifdef SYMBIAN_NETWORKING_UPS
EXPORT_DEFINE_SMELEMENT(TPostPolicyCheckResponseToOriginators, NetStateMachine::MStateTransition, TContext)
EXPORT_C void TPostPolicyCheckResponseToOriginators::DoL()
	{
	MPlatsecApiExt* const platsec = reinterpret_cast<MPlatsecApiExt*>(address_cast<TNodeId>(iContext.iSender).Node().FetchNodeInterfaceL(MPlatsecApiExt::KInterfaceId));
	__ASSERT_DEBUG(platsec, User::Panic(KSpecAssert_ESockCoreProvcrc, 7));

		TInt result = platsec->CheckPolicy(KCprStartSecurityPolicy);
		if (result != KErrNone && result != KErrPermissionDenied && result != KErrCompletion)
			{
			User::Leave(result);
			}

	iContext.iNodeActivity->PostToOriginators(UpsMessage::TPolicyCheckResponse(result));
	}
#endif




