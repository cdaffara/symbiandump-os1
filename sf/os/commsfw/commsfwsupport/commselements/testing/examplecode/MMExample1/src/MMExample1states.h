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
#ifndef MMEXAMPLE1STATES_H
#define MMEXAMPLE1STATES_H

#include <e32base.h>
#include <elements/nm_address.h>
#include <elements/mm_activities.h>
#include <elements/mm_states.h>

#include "MMExample1nodes.h"

/**
 * Definition of constants for whisper message.
 */
const TInt KJunctionMessageRealm = 0x2002D4B3; // used to group messages, UID allocated from symbian signed

enum
	{
	EGoGreen = 1,
	EGoneRed = 2,
	EWait = 3,
	EGo = 4
	};

/**
 * Defines the actual concrete message used in the example.
 */
typedef Messages::TMessageSigVoid<EGoGreen, KJunctionMessageRealm> TGoGreen;
typedef Messages::TMessageSigVoid<EGoneRed, KJunctionMessageRealm> TGoneRed;
typedef Messages::TMessageSigNumber<EWait, KJunctionMessageRealm> TWait;
typedef Messages::TMessageSigVoid<EGo, KJunctionMessageRealm> TGo;

enum TClientTypes
	{
	EClock =              0x00000100,
	EController =         0x00000200,
	ETrafficLight =       0x00000400,
	};

enum TClientFlags
	{
	EGreenFlag          = 0x80000000,
	ENorth              = 0x40000000,
	ESouth              = 0x20000000,
	EEast               = 0x10000000,
	EWest               = 0x08000000
	};
	
namespace Clock
{
typedef MeshMachine::TNodeContext<CClock> TContext;

class CTimerCallback : public CTimer
	{
public:
	static CTimerCallback* NewL(Messages::TNodeCtxId aOriginator);
	void ConstructL();

	void RunL();

private:
	CTimerCallback(Messages::TNodeCtxId aOriginator);
	
	Messages::TNodeCtxId iOriginator;
	};
	
class CTimerActivity : public MeshMachine::CNodeParallelActivityBase
	{
public:
	static CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
	~CTimerActivity();

protected:
	CTimerActivity( const MeshMachine::TNodeActivity& aActivitySig,	MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount );

public:
	CTimerCallback* iTimer;
	};

DECLARE_SMELEMENT_HEADER( TKickOffTimer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TKickOffTimer )
}

namespace TrafficLightStates
{
typedef MeshMachine::TNodeContext<CTrafficLight> TContext;

DECLARE_SMELEMENT_HEADER( TGoAmber, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGoAmber )

DECLARE_SMELEMENT_HEADER( TGoGreen, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGoGreen )

DECLARE_SMELEMENT_HEADER( TGoRedAndSendGoneRed, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGoRedAndSendGoneRed )

}

namespace ControllerStates
{
const TInt KStart = 1;

typedef MeshMachine::TNodeContext<CController> TContext;

DECLARE_SMELEMENT_HEADER( TAwaitingAllRed, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingAllRed )

DECLARE_SMELEMENT_HEADER( TEastAndWestGreen, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TEastAndWestGreen )

DECLARE_SMELEMENT_HEADER( TNorthAndSouthGreen, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TNorthAndSouthGreen )
}

#endif
