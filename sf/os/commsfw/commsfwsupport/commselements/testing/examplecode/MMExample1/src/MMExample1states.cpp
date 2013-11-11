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

#include "MMExample1states.h"
#include <elements/nm_interfaces.h>

using namespace Clock;

CTimerCallback* CTimerCallback::NewL(Messages::TNodeCtxId aOriginator)
	{
	CTimerCallback* t = new (ELeave)CTimerCallback(aOriginator);
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop(t);
	return t;
	}

CTimerCallback::CTimerCallback(Messages::TNodeCtxId aOriginator)
	: CTimer(EPriorityStandard), iOriginator(aOriginator)
	{
	}

void CTimerCallback::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CTimerCallback::RunL()
	{
	Messages::TNodeId nodeid = iOriginator;
	Messages::RNodeInterface::OpenPostMessageClose(nodeid, iOriginator, TGo().CRef());
	}

MeshMachine::CNodeActivityBase* CTimerActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	return new (ELeave) CTimerActivity( aActivitySig, aNode, c);
	}

CTimerActivity::CTimerActivity( const MeshMachine::TNodeActivity& aActivitySig,	MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount )
	: CNodeParallelActivityBase(aActivitySig, aNode, aNextActivityCount), iTimer(NULL)
	{
	}

CTimerActivity::~CTimerActivity()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	}

DEFINE_SMELEMENT(Clock::TKickOffTimer, NetStateMachine::MStateTransition, Clock::TContext)
void Clock::TKickOffTimer::DoL()
	{
	ASSERT(iContext.iNodeActivity);
	CTimerActivity* activity = static_cast<CTimerActivity*>(iContext.iNodeActivity);
	
	ASSERT(!activity->iTimer);

	TWait& msg = Messages::message_cast<TWait>(iContext.iMessage);
	activity->iTimer = CTimerCallback::NewL(Messages::TNodeCtxId(iContext.ActivityId(), iContext.NodeId()));
	activity->iTimer->After(msg.iValue * 1000000);
	activity->SetPostedTo(iContext.NodeId());
	}

using namespace TrafficLightStates;

DEFINE_SMELEMENT(TrafficLightStates::TGoGreen, NetStateMachine::MStateTransition, TrafficLightStates::TContext)
void TrafficLightStates::TGoGreen::DoL()
	{
	iContext.Node().SetColour(CTrafficLight::EGreen);
	
	Messages::RNodeInterface* clock = iContext.Node().GetFirstClient<Messages::TDefaultClientMatchPolicy>(Messages::TClientType(EClock));
	iContext.iNodeActivity->PostRequestTo(*clock, TWait(10).CRef());
	}

DEFINE_SMELEMENT(TrafficLightStates::TGoAmber, NetStateMachine::MStateTransition, TrafficLightStates::TContext)
void TrafficLightStates::TGoAmber::DoL()
	{
	iContext.Node().SetColour(CTrafficLight::EAmber);
	
	Messages::RNodeInterface* clock = iContext.Node().GetFirstClient<Messages::TDefaultClientMatchPolicy>(Messages::TClientType(EClock));
	iContext.iNodeActivity->PostRequestTo(*clock, TWait(3).CRef());
	}

DEFINE_SMELEMENT(TrafficLightStates::TGoRedAndSendGoneRed, NetStateMachine::MStateTransition, TrafficLightStates::TContext)
void TrafficLightStates::TGoRedAndSendGoneRed::DoL()
	{
	iContext.Node().SetColour(CTrafficLight::ERed);

	iContext.iNodeActivity->PostToOriginators(TGoneRed());
	}

using namespace ControllerStates;

DEFINE_SMELEMENT(ControllerStates::TAwaitingAllRed, NetStateMachine::MState, ControllerStates::TContext)
TBool ControllerStates::TAwaitingAllRed::Accept()
	{
	if (!iContext.iMessage.IsMessage<TGoneRed>())
		{
		return EFalse;
		}

	if (iContext.iPeer)
		{
		iContext.iPeer->ClearFlags(EGreenFlag);
		}

	if (iContext.Node().CountClients<Messages::TFlagsOnlyClientMatchPolicy>(Messages::TClientType(0, EGreenFlag)))
		{
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
	else
		{
		return ETrue;
		}

	return EFalse;
	}

DEFINE_SMELEMENT(ControllerStates::TEastAndWestGreen, NetStateMachine::MStateTransition, ControllerStates::TContext)
void ControllerStates::TEastAndWestGreen::DoL()
	{
	Messages::TNodeCtxId from(iContext.ActivityId(), iContext.NodeId());
	TGoGreen msg;
	
	iContext.Node().PostToClients<Messages::TDefaultClientMatchPolicy>(from, msg, Messages::TClientType(ETrafficLight, EWest), Messages::TClientType::NullType(), EGreenFlag);
	iContext.Node().PostToClients<Messages::TDefaultClientMatchPolicy>(from, msg, Messages::TClientType(ETrafficLight, EEast), Messages::TClientType::NullType(), EGreenFlag);
	}

DEFINE_SMELEMENT(ControllerStates::TNorthAndSouthGreen, NetStateMachine::MStateTransition, ControllerStates::TContext)
void ControllerStates::TNorthAndSouthGreen::DoL()
	{
	Messages::TNodeCtxId from(iContext.ActivityId(), iContext.NodeId());
	TGoGreen msg;

	ASSERT(!from.IsNull());
	
	iContext.Node().PostToClients<Messages::TDefaultClientMatchPolicy>(from, msg, Messages::TClientType(ETrafficLight, ENorth), Messages::TClientType::NullType(), EGreenFlag);
	iContext.Node().PostToClients<Messages::TDefaultClientMatchPolicy>(from, msg, Messages::TClientType(ETrafficLight, ESouth), Messages::TClientType::NullType(), EGreenFlag);
	}

