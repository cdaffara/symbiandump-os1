// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Core CPR Activities related to User Prompt Service (UPS)
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalTechnology
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/upscpractivities.h>

#include <e32capability.h>
#include <comms-infras/ss_upsaccesspointconfigext.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockUpsCoreProv, "ESockUpsCoreProv");
#endif

using namespace Messages;
using namespace ESock;
using namespace CoreNetStates;		// TSendDataClientActive
using namespace UpsCprActivities;

//-=========================================================
//
// Activities
//
//-=========================================================

//
// Support for User Prompt Service (UPS)
//

//
// CDeferredCtrlClientJoinActivity methods
//

EXPORT_C MeshMachine::CNodeActivityBase* CDeferredCtrlClientJoinActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TUint c = GetNextActivityCountL(aActivitySig, aNode);

	return new (ELeave) CDeferredCtrlClientJoinActivity(aActivitySig, aNode, c);
	}

EXPORT_C CDeferredCtrlClientJoinActivity::CDeferredCtrlClientJoinActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount)
  : CNodeParallelActivityBase(aActivitySig, aNode, aNextActivityCount)
	{
	}

EXPORT_C CDeferredCtrlClientJoinActivity::~CDeferredCtrlClientJoinActivity()
	{
	// Since this is an join activity, the originator will not be in the peer list.
	// as such, the Originator will not have node peer set, so we should post any errors
	// to the client peer.
	if (Error() && iOriginators.Count() > 0)
		{
		iOriginators[0].PostMessage(TNodeCtxId(ActivityId(), iNode.Id()), TEBase::TError(KickOffMessageId(), Error()).CRef());
		SetError(KErrNone);
		}
	}

void CDeferredCtrlClientJoinActivity::SetClient(const TNodeId& aCommsId)
	{
	__ASSERT_DEBUG(Client() == TNodeId::NullId(), User::Panic(KSpecAssert_ESockUpsCoreProv, 1));
	iClient = aCommsId;
	}

const Messages::TNodeId& CDeferredCtrlClientJoinActivity::Client()
	{
	return iClient;
	}

void CDeferredCtrlClientJoinActivity::SetFlags(TInt aFlags)
	{
	iFlags = aFlags;
	}

TInt CDeferredCtrlClientJoinActivity::Flags()
	{
	return iFlags;
	}

EXPORT_DEFINE_SMELEMENT(CDeferredCtrlClientJoinActivity::TStoreControlClient, NetStateMachine::MStateTransition, CDeferredCtrlClientJoinActivity::TContext)

EXPORT_C void CDeferredCtrlClientJoinActivity::TStoreControlClient::DoL()
/**
Store the control client locally from a TCtrlClientJoinRequest.
*/
	{
	CDeferredCtrlClientJoinActivity* act = iContext.Activity();
	__ASSERT_DEBUG(act->Client() == TNodeId::NullId(), User::Panic(KSpecAssert_ESockUpsCoreProv, 2));
	const TCFControlClient::TJoinRequest& msg = message_cast<TCFControlClient::TJoinRequest>(iContext.iMessage);
	act->SetClient(msg.iNodeId);
    act->SetFlags(msg.iClientType.Flags());
	}

EXPORT_DEFINE_SMELEMENT(CDeferredCtrlClientJoinActivity::TAddControlClientAndSendJoinComplete, NetStateMachine::MStateTransition, CDeferredCtrlClientJoinActivity::TContext)
EXPORT_C void CDeferredCtrlClientJoinActivity::TAddControlClientAndSendJoinComplete::DoL()
/**
Add the stored control client as a client, and send a TJoinComplete to originator(s).
*/
	{
	CDeferredCtrlClientJoinActivity* act = iContext.Activity();
	__ASSERT_DEBUG(act->Client() != TNodeId::NullId(), User::Panic(KSpecAssert_ESockUpsCoreProv, 3));
	
	// Add control client
	iContext.Node().AddClientL(act->Client(), TCFClientType(TCFClientType::ECtrl, act->Flags()));

	// Send TJoinComplete
	RClientInterface::OpenPostMessageClose(iContext.NodeId(), act->Client(), TCFControlClient::TJoinComplete().CRef());
	
	if (iContext.Node().CountClients<TDefaultClientMatchPolicy>(TCFClientType(TCFClientType::ECtrl)) == 1)
    	{
    	TSendDataClientActive dcActive(iContext);
    	dcActive.DoL();
    	}
	}

