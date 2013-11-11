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
// Structures to create a const static (build time) map of states
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file 
 @internalTechnology
*/

#include <e32base.h>

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif


#ifndef _SM_StateTriple_H_
#define _SM_StateTriple_H_

#define DEFINE_TRIPLES_TABLE( name ) \
	const NetStateMachine::TStateTriple name[] = { \
		{ \
		(NetStateMachine::TStateTransitionCtor)NULL, \
		(NetStateMachine::TStateCtor)NULL, \
		(NetStateMachine::TStateForkCtor)NULL, \
		NetStateMachine::KTableBoundaryTag_Internal, \
		NULL \
		},

#define TRIPLES_TABLE_END() \
		{ \
		(NetStateMachine::TStateTransitionCtor)NULL, \
		(NetStateMachine::TStateCtor)NULL, \
		(NetStateMachine::TStateForkCtor)NULL, \
		NetStateMachine::KTableBoundaryTag_Internal, \
		NULL \
		} \
	};

#ifdef SYMBIAN_TRACE_ENABLE

#define FIRST_TRIPLE( firstState, firstStateFork ) \
		{ \
		(NetStateMachine::TStateTransitionCtor)NULL, \
		NetStateMachine::TStateSizeChecker<firstState>::GetVTablePtr, \
		NetStateMachine::TStateForkSizeChecker<firstStateFork>::GetVTablePtr, \
		NetStateMachine::KFirstTripleTag_Internal, \
		_S8("NULL->"#firstState) \
		}

//STATE_TRIPLE can also be used instead of FIRST_TRIPLE to specify a "starting" transition.
//Starting transition is a transition that is executed in case the statemachine is already running
//and an event that would normally cause the statemachine to start occurs.
#define STATE_TRIPLE( transitionTag, stateTransition, state, stateFork ) \
		{ \
		NetStateMachine::TStateTransitionSizeChecker<stateTransition>::GetVTablePtr, \
		NetStateMachine::TStateSizeChecker<state>::GetVTablePtr, \
		NetStateMachine::TStateForkSizeChecker<stateFork>::GetVTablePtr, \
		transitionTag, \
		_S8(#stateTransition "->" #state) \
		}

#define THROUGH_TRIPLE( transitionTag, stateTransition, stateFork ) \
		{ \
		NetStateMachine::TStateTransitionSizeChecker<stateTransition>::GetVTablePtr, \
		(NetStateMachine::TStateCtor)NULL, \
		NetStateMachine::TStateForkSizeChecker<stateFork>::GetVTablePtr, \
		transitionTag, \
		_S8(#stateTransition "->" #stateFork) \
		}

#define LAST_TRIPLE( transitionTag, lastTransition ) \
		{ \
		NetStateMachine::TStateTransitionSizeChecker<lastTransition>::GetVTablePtr, \
		(NetStateMachine::TStateCtor)NULL, \
		(NetStateMachine::TStateForkCtor)NULL, \
		transitionTag, \
		_S8(#lastTransition "->NULL") \
		}
#else

#define FIRST_TRIPLE( firstState, firstStateFork ) \
		{ \
		(NetStateMachine::TStateTransitionCtor)NULL, \
		NetStateMachine::TStateSizeChecker<firstState>::GetVTablePtr, \
		NetStateMachine::TStateForkSizeChecker<firstStateFork>::GetVTablePtr, \
		NetStateMachine::KFirstTripleTag_Internal, \
		NULL \
		}

//STATE_TRIPLE can also be used instead of FIRST_TRIPLE to specify a "starting" transition.
//Starting transition is a transition that is executed in case the statemachine is already running
//and an event that would normally cause the statemachine to start occurs.
#define STATE_TRIPLE( transitionTag, stateTransition, state, stateFork ) \
		{ \
		NetStateMachine::TStateTransitionSizeChecker<stateTransition>::GetVTablePtr, \
		NetStateMachine::TStateSizeChecker<state>::GetVTablePtr, \
		NetStateMachine::TStateForkSizeChecker<stateFork>::GetVTablePtr, \
		transitionTag, \
		NULL \
		}

#define THROUGH_TRIPLE( transitionTag, stateTransition, stateFork ) \
		{ \
		NetStateMachine::TStateTransitionSizeChecker<stateTransition>::GetVTablePtr, \
		(NetStateMachine::TStateCtor)NULL, \
		NetStateMachine::TStateForkSizeChecker<stateFork>::GetVTablePtr, \
		transitionTag, \
		NULL \
		}

#define LAST_TRIPLE( transitionTag, lastTransition ) \
		{ \
		NetStateMachine::TStateTransitionSizeChecker<lastTransition>::GetVTablePtr, \
		(NetStateMachine::TStateCtor)NULL, \
		(NetStateMachine::TStateForkCtor)NULL, \
		transitionTag, \
		NULL \
		}

#endif

#define FIRST_TRIPLE_ENTRY( firstState, firstStateFork ) \
	FIRST_TRIPLE( firstState, firstStateFork ),

#define STATE_TRIPLE_ENTRY( transitionTag, stateTransition, state, stateFork ) \
	STATE_TRIPLE( transitionTag, stateTransition, state, stateFork ),

#define THROUGH_TRIPLE_ENTRY( transitionTag, stateTransition, stateFork ) \
	THROUGH_TRIPLE( transitionTag, stateTransition, stateFork ),

#define LAST_TRIPLE_ENTRY( transitionTag, lastTransition ) \
	LAST_TRIPLE( transitionTag, lastTransition ),

namespace NetStateMachine
{

const TInt KExecuteAlways               = 0x0FFFFFFF;
const TInt KTableBoundaryTag_Internal   = KExecuteAlways - 1; //Should never be used except for the MM itself
const TInt KFirstTripleTag_Internal     = KExecuteAlways - 2; //Should never be used except for the MM itself

//Used as a flag to determine which direction to move 
//in the array of triples for the next transition => the actual transition tag
//is only 31bits
enum KDirection
	{
	EIgnore   = 0x40000000, //do nothing, go nowhere, return (bin 0100...)
	EForward  = 0x00000000,
	EBackward = 0x80000000  //(bin 1000...)
	};

const TInt KMaxStateClassByteSize = 16;

//!!Memory given to MState/MStateTransition derived classes is stack allocated.
//The memory therefore is valid only between the constructor and one and 
//only one subsequent call. No class variables can therefore preserve
//the object state for possible cancel or error call. All such variables
//are to be accessed via the given aContext pointer
//The destructor of the state must not cancel the operation
//There is no guarantie that the destructor of state or transition will be
//called at all
class MStateTransition
	{
	friend class ACore;

protected:
	explicit MStateTransition() 
		{
		};
	virtual ~MStateTransition()
		{
		};

protected:
	virtual void DoL() = 0;
	virtual void Error( TInt aError ) = 0;
	};

class MState
	{
	friend class ACore;

protected:
	explicit MState() 
		{
		};
	virtual ~MState()
		{
		};

protected:
	virtual TBool Accept() = 0;
	virtual void Cancel() = 0;
	};

class MStateFork
	{
	friend class ACore;

protected:
	explicit MStateFork() 
		{
		};
	virtual ~MStateFork()
		{
		};

protected:
	virtual TInt TransitionTag() = 0; //returns a transition tag + direction see KDirection enum
	};


typedef MStateTransition* (*TStateTransitionCtor)(TDesC8& aMem, TAny* aContext);
typedef MState* (*TStateCtor)(TDesC8& aMem, TAny* aContext);
typedef MStateFork* (*TStateForkCtor)(TDesC8& aMem, TAny* aContext);

struct TStateTriple
	{
	TStateTransitionCtor iTCtor;
	TStateCtor iSCtor;
	TStateForkCtor iFCtor;
	TInt iTransitionTag : 30;
	const TText8* iName; //For logging purposes only but present in all configurations to preserve BC. Static const (ROM), 4B.
	};

template <class TSTATETRANSITION>
struct TStateTransitionSizeChecker
    {
    inline static MStateTransition* GetVTablePtr(TDesC8& aMem, TAny* aContext)
    	{
    	__ASSERT_COMPILE(sizeof(TSTATETRANSITION) < KMaxStateClassByteSize); //size of the class is greater than KMaxStateClassByteSize
    	return TSTATETRANSITION::GetVTablePtr(aMem,aContext);
    	}
    };

template <class TSTATE>
struct TStateSizeChecker
    {
    inline static MState* GetVTablePtr(TDesC8& aMem, TAny* aContext)
    	{
    	__ASSERT_COMPILE(sizeof(TSTATE) < KMaxStateClassByteSize); //size of the class is greater than KMaxStateClassByteSize
    	return TSTATE::GetVTablePtr(aMem,aContext);
    	}
    };

template <class TSTATEFORK>
struct TStateForkSizeChecker
    {
    inline static MStateFork* GetVTablePtr(TDesC8& aMem, TAny* aContext)
    	{
    	__ASSERT_COMPILE(sizeof(TSTATEFORK) < KMaxStateClassByteSize); //size of the class is greater than KMaxStateClassByteSize
    	return TSTATEFORK::GetVTablePtr(aMem,aContext);
    	}
    };

} //namespace NetStateMachine

#endif //_SM_StateTriple_H_


