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
// Mobility Connection Provider state definitions.
// 
//
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/mobilitycpr.h>
#include "mobilitycprstates.h"
#include <comms-infras/ss_coreprstates.h>

#include <elements/nm_messages_errorrecovery.h>
#include <comms-infras/ss_mcprnodemessages.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockMbCrCPRSta, "ESockMbCrCPRSta");
#endif



using namespace ESock;
using namespace MobilityCprActivities;
using namespace CorePanics;
using namespace Messages;
using namespace MeshMachine;

//
//Panics
#ifdef _DEBUG
_LIT (KCoreMobileCprPanic,"CoreMobileCprPanic");
#endif

/*******************
 Mobility states
********************/

/**
	Generic cancellation behaviour for many of the mobility activity states.
*/
void MobilityCprStates::TStateBase::Cancel()
	{
	// It would be very peculiar to be cancelling if we'd never started
    __ASSERT_DEBUG(iContext.Node().iMobilityActivity == iContext.iNodeActivity && iContext.iNodeActivity, User::Panic(KSpecAssert_ESockMbCrCPRSta, 1));

	// Set this activity idle for politeness.
	iContext.Node().iMobilityActivity->SetIdle();
	iContext.Node().iMobilityActivity = NULL;	// Activity not owned by iMobilityActivity, safe to NULL.
	}

// TAwaitingMobilityStartedOrError
DEFINE_SMELEMENT(MobilityCprStates::TAwaitingMobilityStartedOrError, NetStateMachine::MState, MobilityCprStates::TContext)
TBool MobilityCprStates::TAwaitingMobilityStartedOrError::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMobilityStarted>())
		{
		return ETrue;
		}

	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
	if (msg && iContext.iNodeActivity)
		{
		iContext.iNodeActivity->SetError(msg->iValue);
		return ETrue;
		}

	return EFalse;
	}

TBool AcceptAndRelayToControlProvider(MobilityCprStates::TContext& aContext, 
									  TNodeSignal::TMessageId aMessageId, TBool aConsume)
	{
	if (aContext.iMessage.MessageId() == aMessageId)
		{
		aContext.Node().ControlProvider()->PostMessage(TNodeCtxId(aContext.ActivityId(), aContext.NodeId()), aContext.iMessage);
		if (!aConsume)
			{
			return ETrue;
			}
		aContext.iMessage.ClearMessageId();
		}
	return EFalse;
	}

DEFINE_SMELEMENT(MobilityCprStates::TAwaitingNewCarrierOrRelayAndConsumeCurrentCarrierRejected, NetStateMachine::MState, MobilityCprStates::TContext)
TBool MobilityCprStates::TAwaitingNewCarrierOrRelayAndConsumeCurrentCarrierRejected::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMobilityControlClient::TMigrationNotification>())
		{
		return ETrue;
		}
	return AcceptAndRelayToControlProvider(iContext, TCFMobilityProvider::TMigrationRejected::Id(), ETrue);
	}

// TAwaitingMigrationCompleteOrError
DEFINE_SMELEMENT(MobilityCprStates::TAwaitingMigrationCompleteOrNewCarrierOrError, NetStateMachine::MState, MobilityCprStates::TContext)
TBool MobilityCprStates::TAwaitingMigrationCompleteOrNewCarrierOrError::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationComplete>() ||
	    iContext.iMessage.IsMessage<TCFMobilityControlClient::TMigrationNotification>())
		{
		return ETrue;
		}

	TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
	if (msg && iContext.iNodeActivity)
		{
		iContext.iNodeActivity->SetError(msg->iValue);
		return ETrue;
		}

	return EFalse;
	}

DEFINE_SMELEMENT(MobilityCprStates::TNoTagOrStartHandshakeBackwardsOrError, NetStateMachine::MStateFork, TContext)
TInt MobilityCprStates::TNoTagOrStartHandshakeBackwardsOrError::TransitionTag()
    {
    if (iContext.Activity()->Error() == KErrNone )
        {
        if (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationComplete>())
            {
            return KNoTag | NetStateMachine::EForward;
            }
        else if (iContext.iMessage.IsMessage<TCFMobilityControlClient::TMigrationNotification>())
            {
            return CMobilityActivity::KStartHandshake | NetStateMachine::EBackward;
            }
        }
    return KErrorTag | NetStateMachine::EForward;;
    }

// AwaitingMigrationRequestedOrMigrationRejected
DEFINE_SMELEMENT(MobilityCprStates::TAwaitAndRelayMigrationRequestedOrMigrationRejected, NetStateMachine::MState, MobilityCprStates::TContext)
TBool MobilityCprStates::TAwaitAndRelayMigrationRequestedOrMigrationRejected::Accept()
	{
 	return AcceptAndRelayToControlProvider(iContext, TCFMobilityProvider::TMigrationRequested::Id(), EFalse) || 
 		   AcceptAndRelayToControlProvider(iContext, TCFMobilityProvider::TMigrationRejected::Id(), EFalse);
	}

DEFINE_SMELEMENT(MobilityCprStates::TAwaitAndRelayMigrationAcceptedOrMigrationRejected, NetStateMachine::MState, MobilityCprStates::TContext)
TBool MobilityCprStates::TAwaitAndRelayMigrationAcceptedOrMigrationRejected::Accept()
 	{
 	return AcceptAndRelayToControlProvider(iContext, TCFMobilityProvider::TMigrationAccepted::Id(), EFalse) || 
 		   AcceptAndRelayToControlProvider(iContext, TCFMobilityProvider::TMigrationRejected::Id(), EFalse);
	}

/*******************
 Mobility transitions
********************/

DEFINE_SMELEMENT(MobilityCprStates::TSendStartMobility, NetStateMachine::MStateTransition, TContext)
void MobilityCprStates::TSendStartMobility::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileCprPanic, KPanicNoActivity));

	// Request that the MCPR begin its mobility activity.
	iContext.iNodeActivity->PostRequestTo(*iContext.Node().ControlProvider(),
		TCFMobilityProvider::TStartMobility(NULL).CRef());
	}

DEFINE_SMELEMENT(MobilityCprStates::TSendErrorRecoveryReq, NetStateMachine::MStateTransition, TContext)
void MobilityCprStates::TSendErrorRecoveryReq::DoL()
	{
    //we are started based on the fact the the last message iContext.Node()'s received is TError
    TEBase::TError& errmsg = message_cast<TEBase::TError>(iContext.iMessage);
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreMobileCprPanic, KPanicNoActivity));

    TErrContext ctx(iContext.NodeId(), errmsg.iMsgId, iContext.iNodeActivity->ActivityId(), Elements::TStateChange(0, errmsg.iValue));
    TEErrorRecovery::TErrorRecoveryRequest msg(ctx);

    iContext.iNodeActivity->PostRequestTo(
        *iContext.Node().ControlProvider(),//ControlProvider() verified above
        TCFSafeMessage::TRequestCarrierEast<TEErrorRecovery::TErrorRecoveryRequest>(msg).CRef()
        );
    iContext.iNodeActivity->ClearPostedTo();
	}

DEFINE_SMELEMENT(MobilityCprStates::TSendMigrationRejected, NetStateMachine::MStateTransition, TContext)
void MobilityCprStates::TSendMigrationRejected::DoL()
    {
    iContext.iNodeActivity->PostRequestTo(*iContext.Node().ControlProvider(),
             TCFMobilityProvider::TMigrationRejected(KErrNone).CRef());
    }



