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
// core state execution
// 
//

/**
 @file
*/

#include "sm_core.h"

#include <elements/sd_log.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemStateMachCrC, "ElemStateMachCrC");
#endif

using namespace NetStateMachine;

EXPORT_C TBool ACore::Accept(const TStateTriple& aFirst, TAny* aContext, TInt aTransitionTag)
	{
	TBuf8<KMaxStateClassByteSize> mem;
	__ASSERT_DEBUG(aFirst.iSCtor, User::Panic(KSpecAssert_ElemStateMachCrC, 1)); //StateConstructor must always be set
	MState* state = aFirst.iSCtor( mem, aContext );
	return (aFirst.iTransitionTag == aTransitionTag || aTransitionTag == KExecuteAlways) && state->Accept();
	}

EXPORT_C void ACore::Start(TAny* aContext, const TStateTriple& aFirst)
    {
	iCurrent = &aFirst;
	TBuf8<KMaxStateClassByteSize> mem;
	__ASSERT_DEBUG(iCurrent->iFCtor, User::Panic(KSpecAssert_ElemStateMachCrC, 2)); //StateFork must always be set
	MStateFork* fork = iCurrent->iFCtor(mem, aContext);
	DoTransition(*fork, aContext, mem);
    }

EXPORT_C TBool ACore::Next(TAny* aContext)
    {
    if (IsIdle())
    	{
    	return EFalse;
    	}

	//Normal triple must accept first
	TBuf8<KMaxStateClassByteSize> mem;
	if (iCurrent->iSCtor)
        {
    	MState* state = iCurrent->iSCtor(mem, aContext);
    	if (!state->Accept())
    	    {
    	    return EFalse;
    	    }
    	//[RZ] Here it was, wasn't it. The problem with ACore not allowing to inject any
    	//     action between state::accept and transition::DoL. And we do we want?
    	//     reset iPostedTo. iPostedTo must be reset after accepting the message
    	//     and this shouldn't be left to the activity implementor.

    	//[MZ] This is by design. The MM may never reset iPostedTo. This is properly
    	//     left to the activity implementation (same as setting it for that matter).
    	//     Activity implementations can receive multiple responses and there is
    	//     no reason whatsoever to disallow this behaviour.
    	//     One obvious example here is the TSelectComplete sent N+1 times where N
    	//     is the count of the selected providers.
    	}

    //Through triple or after accept
    __ASSERT_DEBUG(iCurrent->iFCtor, User::Panic(KSpecAssert_ElemStateMachCrC, 3)); //StateFork must always be set
    MStateFork* fork = iCurrent->iFCtor(mem, aContext);
    return DoTransition(*fork, aContext, mem);
    }

TBool ACore::DoTransition(MStateFork& aStateFork, TAny* aContext, TDes8& aMem)
    {
    TInt aTransitionTag = aStateFork.TransitionTag();

	//SM can choose to ignore the current "Do" and continue to wait in the current tripple.
	if (aTransitionTag & EIgnore)
		{
		return EFalse;
		}

	//Inside of this block the EIgnore flag is cleared.
	//There is no risk when matching in ACore::FindNext
	//(and we can safely do aTransitionTag & ~EBackward)
	iCurrent = ACore::FindNext(*iCurrent, aTransitionTag);

	//Diagnostic, debug only panic.
	//A transition marked by the specified tag has not been found within the state machine.
	//We do not allow this behaviour to make finding of potential mistates easier.
	//If you see this panic you will need to redesign your SM so that it
	//always finds a transition it has selected for execution.
	__ASSERT_DEBUG(iCurrent, User::Panic(KSpecAssert_ElemStateMachCrC, 4));

	//Only progress if there was a match with the tag found (iCurrent!=NULL)
	//as well as this is any but the first triple (iCurrent->iTCtor!=NULL)
    if (iCurrent!=NULL && iCurrent->iTCtor!=NULL)
        {
    	__ASSERT_DEBUG( (aTransitionTag & ~EBackward)==iCurrent->iTransitionTag || aTransitionTag==KExecuteAlways, User::Panic(KSpecAssert_ElemStateMachCrC, 5));
		TInt err = Do(*iCurrent, aContext, aMem);
  		//If there was no error (err == KErrNone), the SM didn't set itself to idle (iCurrent!=NULL),
  		//this isn't the last triple (iCurrent->iFCtor!=NULL) and our state consists only from the fork,
  		//proceed to the next triple.
  		if (err == KErrNone && iCurrent!=NULL && iCurrent->iSCtor==NULL && iCurrent->iFCtor!=NULL)
			{
			MStateFork* fork = iCurrent->iFCtor(aMem, aContext);
			return DoTransition(*fork, aContext, aMem);
			}
        }

	//The activity may have finished
	return ETrue;
    }

// Runs the transition in the given state triple
EXPORT_C TInt ACore::Do(const TStateTriple& aTriple,TAny* aContext, TDes8& aMem)
	{
	MStateTransition* transition = aTriple.iTCtor(aMem, aContext);
	TRAPD(err, transition->DoL());
	if (err != KErrNone)
		{
		//COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KESockStateMachine, _L8("ERROR: ACore::Do - Transition failed with error %d"), err));

		transition->Error(err);
		}
	return err;
	}

EXPORT_C void ACore::SetIdle()
	{
	iCurrent = NULL;
	}

EXPORT_C TBool ACore::IsIdle() const
	{
	return iCurrent == NULL || iCurrent->iFCtor == NULL;
	}

EXPORT_C void ACore::DoCurrent(TAny* aContext)
    {
   	TBuf8<KMaxStateClassByteSize> mem;
   	__ASSERT_DEBUG(!IsIdle(), User::Panic(KSpecAssert_ElemStateMachCrC, 6));
	Do(*iCurrent,aContext,mem);
    }

EXPORT_C void ACore::Cancel(TAny* aContext)
	{
	if (!IsIdle())
		{
		__ASSERT_DEBUG(iCurrent, User::Panic(KSpecAssert_ElemStateMachCrC, 7)); //This should always be != NULL when we are not idle
		//Trying to Cancel a SM waiting in the last triple with no state?
		if (iCurrent->iSCtor)
			{
			//Give the state a chance to cancel related actions
			TBuf8<KMaxStateClassByteSize> mem;
			MState* state = iCurrent->iSCtor( mem, aContext );
			state->Cancel();
			}
		SetIdle();
		}
	}

EXPORT_C const TStateTriple* ACore::FindNext(const TStateTriple& aCurrent, TInt aTransitionTag)
    {
    //For the following comparisons it is crutial that the EIgnore is cleared.
    __ASSERT_DEBUG(0 == (aTransitionTag & EIgnore), User::Panic(KSpecAssert_ElemStateMachCrC, 8));

    //Include current triple in the search if direction is backwards.
    const TStateTriple* current = &aCurrent;

    if (0 != (aTransitionTag & EBackward))
        {
    	//We now need to clear the EBackward for the following comparisons to work.
        aTransitionTag &= ~EBackward;

        //1) "current" can be anywhere in the SM but in the last tripple (because there is no state there).
        //2) current->iTCtor check is here since the very first triple doesn't need to have a transition
    	while (current->iFCtor && current->iTCtor &&
    		aTransitionTag!=KExecuteAlways && aTransitionTag!=current->iTransitionTag)
    		{
    		--current;
    		}
        }
    else
        {
        ++current;
		//"current" can be anywhere in the SM but in the first tripple (preincrement).
    	while (current->iTCtor &&
    		aTransitionTag!=KExecuteAlways && aTransitionTag!=current->iTransitionTag)
    		{
    		++current;
    		}
        }

    return (aTransitionTag==KExecuteAlways || aTransitionTag==current->iTransitionTag)?
    	current : NULL;
    }

