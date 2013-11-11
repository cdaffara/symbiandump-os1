// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <elements/mm_activities.h>
#include <elements/mm_states.h>

#include "MMExample1states.h"
#include "MMExample1nodes.h"

_LIT8(KLoggingTag, "MMExample1");

enum TMMExample1ActivityIds
	{
	ETimerActivity = 1,
	ETrafficLightActivity = 2,
	ETrafficLightControllerActivity = 3
	};

namespace ClockActivities
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ETimerActivity, Timer, TWait, Clock::CTimerActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingMessageState<TWait>, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(MeshMachine::KNoTag, Clock::TKickOffTimer, MeshMachine::TAwaitingMessageState<TGo>, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(MeshMachine::KNoTag, MeshMachine::TForwardMessageToOriginators)
NODEACTIVITY_END()
}

DECLARE_DEFINE_ACTIVITY_MAP(ClockActivityMap)
    ACTIVITY_MAP_ENTRY(ClockActivities, Timer)
ACTIVITY_MAP_END()

CClock::CClock() : MeshMachine::AMMNodeBase(ClockActivityMap::iSelf, Messages::ANodeId::Id())
	{
	NM_LOG_NODE_CREATE(KLoggingTag, CClock);
	}

void CClock::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage)
	{
	Messages::TNodeSignal::TMessageId noPeerIds[] = { Messages::TNodeSignal::TMessageId() };

	MeshMachine::TNodeContext<CClock> ctx(*this, aCFMessage, aSender, aRecipient);
	
	MeshMachine::AMMNodeBase::Received(noPeerIds, ctx);
	MeshMachine::AMMNodeBase::PostReceived(ctx);
	User::LeaveIfError(ctx.iReturn);
	}
    

namespace TrafficLightActivities
{
DECLARE_DEFINE_NODEACTIVITY(ETrafficLightActivity, TrafficLight, TGoGreen)
    FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingMessageState<TGoGreen>, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(MeshMachine::KNoTag, TrafficLightStates::TGoAmber, MeshMachine::TAwaitingMessageState<TGo>, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(MeshMachine::KNoTag, TrafficLightStates::TGoGreen, MeshMachine::TAwaitingMessageState<TGo>, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(MeshMachine::KNoTag, TrafficLightStates::TGoAmber, MeshMachine::TAwaitingMessageState<TGo>, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(MeshMachine::KNoTag, TrafficLightStates::TGoRedAndSendGoneRed)
NODEACTIVITY_END()
}

DECLARE_DEFINE_ACTIVITY_MAP(TrafficLightActivityMap)
    ACTIVITY_MAP_ENTRY(TrafficLightActivities, TrafficLight)
ACTIVITY_MAP_END()

CTrafficLight::CTrafficLight(CConsoleBase* aConsole, const TDesC& aPositionText) : MeshMachine::AMMNodeBase(TrafficLightActivityMap::iSelf, Messages::ANodeId::Id()), iConsole(aConsole), iPositionText(aPositionText)
	{
	NM_LOG_NODE_CREATE(KLoggingTag, CTrafficLight);
	
	SetColour(ERed);
	}

void CTrafficLight::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::SignatureBase& aCFMessage)
	{
	Messages::TNodeSignal::TMessageId noPeerIds[] = { Messages::TNodeSignal::TMessageId() };

	MeshMachine::TNodeContext<CTrafficLight> ctx(*this, aCFMessage, aSender, aRecipient);
	
	MeshMachine::AMMNodeBase::Received(noPeerIds, ctx);
	MeshMachine::AMMNodeBase::PostReceived(ctx);
	User::LeaveIfError(ctx.iReturn);
	}

void CTrafficLight::SetColour(TColour aColour)
	{
	iColour = aColour;

	switch (aColour)
		{
		case ERed:
			iConsole->Printf(_L("%S set to RED\n"), &iPositionText);
			break;
		case EAmber:
			iConsole->Printf(_L("%S set to AMBER\n"), &iPositionText);
			break;
		case EGreen:
			iConsole->Printf(_L("%S set to GREEN\n"), &iPositionText);
			break;
		}
	}

namespace ControllerActivities
{
DECLARE_DEFINE_NODEACTIVITY(ETrafficLightControllerActivity, TrafficLightController, TGo)
    FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingMessageState<TGo>, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(ControllerStates::KStart, ControllerStates::TEastAndWestGreen, ControllerStates::TAwaitingAllRed, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(MeshMachine::KNoTag, ControllerStates::TNorthAndSouthGreen, ControllerStates::TAwaitingAllRed, MeshMachine::TTag<ControllerStates::KStart|NetStateMachine::EBackward>)
NODEACTIVITY_END()
}

DECLARE_DEFINE_ACTIVITY_MAP(ControllerActivityMap)
    ACTIVITY_MAP_ENTRY(ControllerActivities, TrafficLightController)
ACTIVITY_MAP_END()

CController::CController() : MeshMachine::AMMNodeBase(ControllerActivityMap::iSelf, Messages::ANodeId::Id())
	{
	NM_LOG_NODE_CREATE(KLoggingTag, CController);
	}

void CController::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage)
	{
	Messages::TNodeSignal::TMessageId noPeerIds[] = { TGo::Id(), Messages::TNodeSignal::TMessageId() };

	MeshMachine::TNodeContext<CController> ctx(*this, aCFMessage, aSender, aRecipient);
	
	MeshMachine::AMMNodeBase::Received(noPeerIds, ctx);
	MeshMachine::AMMNodeBase::PostReceived(ctx);
	User::LeaveIfError(ctx.iReturn);
	}

