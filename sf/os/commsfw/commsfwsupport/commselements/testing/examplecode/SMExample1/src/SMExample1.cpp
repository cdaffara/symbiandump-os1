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
//
//  Traffic lights state machine example
//
//  The example demonstrates transitions and states. There is one state
//  in the example, which is the state of waiting for light to change. This
//  state has a number of ticks associated, which is decrements each time the
//  state is tested.
//
//  There are numerous transitions, which change which lights are on and
//  which lights are off. 
//
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>	
#include <elements/sm_statetriple.h>
#include <elements/sm_core.h>


LOCAL_D CConsoleBase* console; // write all messages to this

/**
 * Declaration of states, transition and tags.
 * The states and transitions are defined further down, just declared here
 * so they can be used in the state machine.
 */ 
// States
class TAwaitingTicks;

// Transitions
class TGoGreen;
class TGoRed;
class TGoAmber;
class TGoRedAndAmber;

// Forks
class TNextTag;
class TStartTag;
class TStartTagBackwards;

// Tags (returned by forks to specify next transition to run)
const TInt KStart = 1;
const TInt KNext = 2;

/**
 * Definition of State Machine. State machine goes from Red->Red/Amber->Green->Amber->Red->...
 */
DEFINE_TRIPLES_TABLE( KTrafficLightStateMachine )
    FIRST_TRIPLE_ENTRY(TAwaitingTicks, TStartTag)
    STATE_TRIPLE_ENTRY(KStart, TGoRed, TAwaitingTicks, TNextTag)
    STATE_TRIPLE_ENTRY(KNext, TGoRedAndAmber, TAwaitingTicks, TNextTag)
    STATE_TRIPLE_ENTRY(KNext, TGoGreen, TAwaitingTicks, TNextTag)
   	STATE_TRIPLE_ENTRY(KNext, TGoAmber, TAwaitingTicks, TStartTagBackwards)
TRIPLES_TABLE_END()

/**
 * Traffic Light class to hold the state of the traffic light.
 */
class CTrafficLight : NetStateMachine::ACore
	{
	friend class TAwaitingTicks;
	friend class TGoRedAndAmber;
	friend class TGoGreen;
	friend class TGoAmber;
	friend class TGoRed;
	
public:
	CTrafficLight();

	/**
	 * Start the state machine running. Runs forever.
	 */
	void GoL();

	/**
	 * Print the current state of the lights.
	 */
	void PrintLights();
	
public:
	/**
	 * Number of ticks before next state change.
	 */
	TInt iTicksLeft;

	/**
	 * The lights. ETrue is on, EFalse is off.
	 */
	TBool iRedLight;
	TBool iAmberLight;
	TBool iGreenLight;
	};

CTrafficLight::CTrafficLight()
	: ACore(), iTicksLeft(0), iRedLight(EFalse), iAmberLight(EFalse), iGreenLight(EFalse)
	{
	}

void CTrafficLight::GoL()
	{
	/**
	   Start the state machines running. This is a call to ACore::Start. This
	   expects a context and the first triple from the state machine. The triples
	   in the state machine count from 1.

	   The context is a TAny* pointer, so it can be anything, so long as the States,
	   Transitions, and StateForks in the statemachine know how to use it. In this
	   case it is a CTrafficLight.
	*/
	Start(this, KTrafficLightStateMachine[1]);
	while (ETrue)
		{
		/**
		   Wait for 1 second and attempt to progress the state machine. The state machine
		   will be in the "Awaiting ticks" state. It will progress through the next transition
		   only if all the ticks have been received.
		*/
		User::After(1000000);
		Next(this);
		}
	}

void CTrafficLight::PrintLights()
	{
	_LIT(KOn, "on ");
	_LIT(KOff, "off");
	console->Printf(_L("Red: %S\tAmber: %S\tGreen: %S\n"), iRedLight ? &KOn : &KOff, iAmberLight ? &KOn : &KOff, iGreenLight ? &KOn : &KOff);
	}

/**
 * The state machine in our example has only one state, TAwaitingTicks. This the state
 * machine waits in this state until all events required to trigger a True response from
 * ::Accept have been received. The events are triggered by a timer in CTrafficLight.
 *
 * Once all these events (ticks) have been received the state machine executes a transition
 * and returns to waiting in this state.
 */
class TAwaitingTicks : public NetStateMachine::MState
	{
public:
	/**
	   The constructor must take a reference to an object of the context type.
	   The context type here is a CTrafficLight.
	*/
	TAwaitingTicks(CTrafficLight& aTrafficLight) : iTrafficLight(aTrafficLight) {}
	virtual TBool Accept();
	virtual void Cancel();

	/**
	   Declare factory method by which the state is created. States and Transitions
	   are created on the stack in preallocated memory, and the memory used for one
	   state is reused for the following transition.

	   The first argument is the constructor to use in the factory method.
	   The second argument is the type of object the factory will return.
	   The last argument is the type of the context object which will be passed
	   as the first and only argument to the constructor. 
	*/
	NETSM_TPL_DECLARE_CTR(TAwaitingTicks, NetStateMachine::MState, CTrafficLight);
private:
	CTrafficLight& iTrafficLight;
	};

/**
 * ::Accept should return true only if the all conditions for the state machine to proceed have
 * been met. In this instance, ::Accept only returns true if ticks have run out.
 */
TBool TAwaitingTicks::Accept()
	{
	TBool ret = (--iTrafficLight.iTicksLeft <= 0);
	
	return ret;
	}

/**
 * When called is called on ACore, ::Cancel is called on the state that the state machine is
 * currenting in.
 */
void TAwaitingTicks::Cancel()
	{
	}

/**
 * Base class for all TGo* classes. Stubs out Error as it's not used in this example.
 */
class TGoColour : public NetStateMachine::MStateTransition
	{
public:
	TGoColour(CTrafficLight& aTrafficLight) : iTrafficLight(aTrafficLight) {}

	virtual void DoL() = 0;
	virtual void Error( TInt aError );

protected:
	CTrafficLight& iTrafficLight;
	};
	
void TGoColour::Error( TInt /*aError*/ )
	{
	/**
	   Do nothing. Normally this would be called if DoL leaves, but in our
	   example, DoL can never leave, so there's no need to handle the error.
	*/
	}

/**
 * Transition to turn on the green light
 */
class TGoGreen : public TGoColour
	{
public:
	TGoGreen(CTrafficLight& aTrafficLight) : TGoColour(aTrafficLight) {}
	virtual void DoL();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TGoGreen, NetStateMachine::MStateTransition, CTrafficLight);
	};

/**
 * ::DoL transitions the state machine from one state to the next.
 *
 * In this case, turn on the green light and set the ticks. The ticks define how long the traffic
 * light should stay in this state before progressing through the next transition.
 */
void TGoGreen::DoL()
	{
	iTrafficLight.iRedLight = EFalse;
	iTrafficLight.iAmberLight = EFalse;
	iTrafficLight.iGreenLight = ETrue;
	iTrafficLight.iTicksLeft = 5;
	iTrafficLight.PrintLights();
	}

/**
 * Transition to turn on the red light
 */
class TGoRed : public TGoColour
	{
public:
	TGoRed(CTrafficLight& aTrafficLight) : TGoColour(aTrafficLight) {}
	virtual void DoL();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TGoRed, NetStateMachine::MStateTransition, CTrafficLight);
	};

void TGoRed::DoL()
	{
	iTrafficLight.iRedLight = ETrue;
	iTrafficLight.iAmberLight = EFalse;
	iTrafficLight.iGreenLight = EFalse;
	iTrafficLight.iTicksLeft = 5;
	iTrafficLight.PrintLights();
	}

/**
 * Transition to turn on the amber light
 */
class TGoAmber : public TGoColour
	{
public:
	TGoAmber(CTrafficLight& aTrafficLight) : TGoColour(aTrafficLight) {}
	virtual void DoL();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TGoAmber, NetStateMachine::MStateTransition, CTrafficLight);
	};

void TGoAmber::DoL()
	{
	iTrafficLight.iRedLight = EFalse;
	iTrafficLight.iAmberLight = ETrue;
	iTrafficLight.iGreenLight = EFalse;
	iTrafficLight.iTicksLeft = 1;
	iTrafficLight.PrintLights();
	}

/**
 * Transition to turn on the red and amber lights
 */
class TGoRedAndAmber : public TGoColour
	{
public:
	TGoRedAndAmber(CTrafficLight& aTrafficLight) : TGoColour(aTrafficLight) {}
	virtual void DoL();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TGoRedAndAmber, NetStateMachine::MStateTransition, CTrafficLight);
	};

void TGoRedAndAmber::DoL()
	{
	iTrafficLight.iRedLight = ETrue;
	iTrafficLight.iAmberLight = ETrue;
	iTrafficLight.iGreenLight = EFalse;
	iTrafficLight.iTicksLeft = 1;
	iTrafficLight.PrintLights();
	}

/**
 * State fork to specify the state machine should move on to the next state triple
 * which has KNext as its tag.
 */
class TNextTag : public NetStateMachine::MStateFork
	{
public:
	TNextTag(CTrafficLight& /*aTrafficLight*/) {}
	virtual TInt TransitionTag();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TNextTag, NetStateMachine::MStateFork, CTrafficLight);
	};

TInt TNextTag::TransitionTag()
	{
	return KNext;
	}

/**
 * State forks are decision points in the state machine. The state machine moves onto
 * the next transition which matches the tag returned by ::TransitionTag.
 *
 * In this case, there is no decision to be made since the state machine proceeds sequentially
 * in a loop, so just move to the next transition with KStart as its tag.
 */
class TStartTag : public NetStateMachine::MStateFork
	{
public:
	TStartTag(CTrafficLight& /*aTrafficLight*/) {}
	virtual TInt TransitionTag();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TStartTag, NetStateMachine::MStateFork, CTrafficLight);
	};

TInt TStartTag::TransitionTag()
	{
	return KStart;
	}

/**
 * State fork to specify the state machine that is should jump back to the last state
 * triple which had KStart as its tag.
 */
class TStartTagBackwards : public NetStateMachine::MStateFork
	{
public:
	TStartTagBackwards(CTrafficLight& /*aTrafficLight*/) {}
	virtual TInt TransitionTag();

	/**
	   See TAwaitingTicks for explanation of this declaration.
	*/
	NETSM_TPL_DECLARE_CTR(TStartTagBackwards, NetStateMachine::MStateFork, CTrafficLight);
	};

/**
 * This ::TransitionTag is slightly different to the previous one, in that it specifies
 * that the search for matching tags should move backwards up the transition list.
 */
TInt TStartTagBackwards::TransitionTag()
	{
	return KStart | NetStateMachine::EBackward;
	}


LOCAL_C void MainL ()
	{
	CTrafficLight* lights = new(ELeave) CTrafficLight();
	lights->GoL();
	}

LOCAL_C void DoStartL ()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL (scheduler);
	CActiveScheduler::Install (scheduler);

	MainL ();

	CleanupStack::PopAndDestroy (scheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(createError, console = Console::NewL(_L("SMExample1"), TSize(KConsFullScreen,KConsFullScreen)));
	if (createError)
		{
		return createError;
		}

	TRAPD(mainError, DoStartL());
	if (mainError)
		{
		console->Printf(_L(" failed, leave code = %d"), mainError);
		}
	console->Printf(_L(" [press any key]\n"));
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

