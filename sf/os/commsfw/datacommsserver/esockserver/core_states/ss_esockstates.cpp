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
// Sub Session States
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/

#include "ss_esockstates.h"


#include <comms-infras/ss_log.h>
#include <ss_glob.h>

#include <comms-infras/ss_nodeinterfaces.h>
#include <ss_std.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_esockactivities.h>

#include <comms-infras/ss_nodemessages_subconn.h>
#include "ss_apiext_messages.h"
#include <elements/nm_messages_base.h>

// Remove the "#if 0" if KESockStatesTag/KESockStatesSubTag are required
#if 0
#ifdef __CFLOG_ACTIVE
	#define KESockStatesTag KESockCoreProviderTag
	_LIT8(KESockStatesSubTag, "esockstate");
#endif
#endif

using namespace SubSessActivities;
using namespace SubSessStates;
using namespace NetStateMachine;
using namespace ESock;
using namespace CorePanics;
using namespace Messages;

//
//Panics
#ifdef _DEBUG
_LIT (KESockStatePanic,"ESockStatePanic");
#endif

TBool AECABState::Accept(MeshMachine::TNodeContextBase& aContext, TBool aSuperAccept)
	{
	TEBase::TError* msg = message_cast<TEBase::TError>(&aContext.iMessage);
	if(msg && aSuperAccept == EFalse && aContext.iNodeActivity != NULL)
    	{
    	aContext.iNodeActivity->SetError(msg->iValue);
        aContext.iNodeActivity->SetIdle();
    	aContext.iMessage.ClearMessageId();
    	return EFalse;
		}
	return aSuperAccept;
	}


DEFINE_SMELEMENT(SubSessStates::TAcquireMessageOwnership, NetStateMachine::MStateTransition, SubSessStates::TContext)
void SubSessStates::TAcquireMessageOwnership::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KESockStatePanic,KPanicNoActivity));
    SubSessActivities::CESockClientActivityBase& ac = static_cast<SubSessActivities::CESockClientActivityBase&>(*iContext.iNodeActivity);

    //Take ownership of the clients message
    ac.AcquireOwnership(message_cast<ESock::TCFInternalEsock::TSubSess>(iContext.iMessage).iMessage, iContext.Node());
	}


DEFINE_SMELEMENT(SubSessStates::TBinEvent, NetStateMachine::MStateTransition, SubSessStates::TContext)
void SubSessStates::TBinEvent::DoL()
	{
	TCFSubConnControlClient::TSubConnNotification& eventMsg = message_cast<TCFSubConnControlClient::TSubConnNotification>(iContext.iMessage);
    eventMsg.iRefCountOwnedSubConNotification->Close();
	}

DEFINE_SMELEMENT(SubSessStates::TCancelAndCloseClientExtIfaces, NetStateMachine::MStateTransition, SubSessStates::TContext)
void TCancelAndCloseClientExtIfaces::DoL()
	{
	TCancelAndCloseAllClientExtItf msg(iContext.Node().UniqueId());
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(iContext.NodeId(), msg,
		TClientType(TCFClientType::EServProvider), TClientType(0, TCFClientType::ELeaving));
	}

