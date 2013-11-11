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

#include <ssm/ssmswppolicy.h>

#include "ssmdebug.h"
#include "ssmswptransitionengine.h"
#include "ssmswptransitionscheduler.h"
#include "ssmswpindividualtransitionscheduler.h"

//
//Construction/Destruction
//


/**
Create a scheduler
*/
CSsmSwpTransitionScheduler* CSsmSwpTransitionScheduler::NewL()
	{
	CSsmSwpTransitionScheduler* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSsmSwpTransitionScheduler* CSsmSwpTransitionScheduler::NewLC()
	{
	CSsmSwpTransitionScheduler* self = new (ELeave) CSsmSwpTransitionScheduler();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSsmSwpTransitionScheduler::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

LOCAL_C TInt OrderingFunction(const CSsmSwpIndividualTransitionScheduler& aLhs, const CSsmSwpIndividualTransitionScheduler& aRhs)
	{
	return aLhs.SwpKey() - aRhs.SwpKey();
	}

LOCAL_C TInt SearchingFunction(const TUint* aUint, const CSsmSwpIndividualTransitionScheduler& aObject)
	{
	return *aUint - aObject.SwpKey();
	}

CSsmSwpTransitionScheduler::CSsmSwpTransitionScheduler()
: CTimer(CActive::EPriorityStandard),
  iLinearOrder(&OrderingFunction)
	{
	
	}

/**
Deactivate
Destroy queue
*/
CSsmSwpTransitionScheduler::~CSsmSwpTransitionScheduler()
	{
	CTimer::Cancel();
	iIndividualTransitionSchedulers.ResetAndDestroy();
	}

/**
Take ownership of the transition engine and prepare it to run
@param aEngine - the engine to run
*/
void CSsmSwpTransitionScheduler::SubmitL(CSsmSwpTransitionEngine* aEngine)
	{
	if(!aEngine)
		{
		SSMLOGLEAVE(KErrArgument); //lint !e527 Suppress Unreachable. Lint is just confused by macro, warning goes away when code in macro gets expanded
		}
	CSsmSwpIndividualTransitionScheduler* scheduler = FindScheduler(aEngine);
	if(scheduler == NULL)
		{
		// No scheduler already for this SwP, need to create one
		scheduler = CSsmSwpIndividualTransitionScheduler::NewLC(aEngine->TransitionValue().Key(), aEngine->CleSessionProxy(), *this);
		iIndividualTransitionSchedulers.InsertInOrderL(scheduler, iLinearOrder);
		CleanupStack::Pop(scheduler);
		// Submit the request
		scheduler->SubmitL(aEngine);
		}
	else
		{
		// Already have a scheduler for this session so submit it
		scheduler->SubmitL(aEngine);
		}
	}

void CSsmSwpTransitionScheduler::Cancel(CSession2* aSession)
	{
	// Iterate through the array informing all schedulers of the cancel
	TInt count = iIndividualTransitionSchedulers.Count();
	for(TInt i = 0; i < count; ++i)
		{
		iIndividualTransitionSchedulers[i]->Cancel(aSession);
		}
	}

CSsmSwpIndividualTransitionScheduler* CSsmSwpTransitionScheduler::FindScheduler(CSsmSwpTransitionEngine* aEngine)
	{
	TInt offset = iIndividualTransitionSchedulers.FindInOrder(aEngine->TransitionValue().Key(), &SearchingFunction);
	if(offset != KErrNotFound)
		{
		return iIndividualTransitionSchedulers[offset];
		}
	return NULL;
	}

/**
Notify the monitor (if set) that one of the transition events has occured
*/
void CSsmSwpTransitionScheduler::NotifyTransitionQueued()
	{
	if(iTransactionMonitor != NULL)
		{
		iTransactionMonitor->SwpTransitionQueued();
		}
	}

void CSsmSwpTransitionScheduler::NotifyTransitionStarted()
	{
	if(iTransactionMonitor != NULL)
		{
		iTransactionMonitor->SwpTransitionStarted();
		}
	}

// Time to wait after completion before starting lazy cleanup
const TInt KLazyCleanupTimeOut = 5000000;

void CSsmSwpTransitionScheduler::NotifyTransitionCompleted()
	{
	DEBUGPRINT1(_L("ERequestSwpChange Completed"));
	if(iTransactionMonitor != NULL)
		{
		iTransactionMonitor->SwpTransitionCompleted(++iNumberOfCompletedTransitions);
		}
	if(!IsActive())
		{
		After(KLazyCleanupTimeOut);
		}
	}

void CSsmSwpTransitionScheduler::RunL()
	{
	// ignore any error in iStatus as clean-up is a safe operations to perform anytime
	
	// iterate through schedulers, removing them if they're not doing anything
	TInt top = iIndividualTransitionSchedulers.Count() - 1;
	for(TInt i = top; i >= 0; --i)
		{
		if (iIndividualTransitionSchedulers[i]->IsIdle())
			{
			delete iIndividualTransitionSchedulers[i];
			iIndividualTransitionSchedulers[i] = NULL;
			iIndividualTransitionSchedulers.Remove(i);
			}
		}
	// If there are now none left, free the memory
	if(iIndividualTransitionSchedulers.Count() == 0)
		{
		iIndividualTransitionSchedulers.GranularCompress();
		}
	}

TInt CSsmSwpTransitionScheduler::RunError(TInt __DEBUG_ONLY(aError))
	{
	DEBUGPRINT2A("CSsmSwpTransitionScheduler::RunError() run with error %d, ignoring", aError);
	// Ignore any errors with the timer as it is only used for clean up 
	return KErrNone;
	}

void CSsmSwpTransitionScheduler::NotifyTransitionFailed()
	{
	DEBUGPRINT1(_L("Error: ERequestSwpChange Failed"));
	if(iTransactionMonitor != NULL)
		{
		iTransactionMonitor->SwpTransitionFailed(++iNumberOfFailedTransitions);
		}
	}

/**
 * Used for testing purposes
 * Delete all the individual Transition Schedulers stored in the array
 * and free the memory
 */
#ifdef _DEBUG
void CSsmSwpTransitionScheduler::CleanupIndividualSchedulerArray()
	{
	TInt top = iIndividualTransitionSchedulers.Count() - 1;
	for(TInt i = top; i >= 0; --i)
		{
		if (iIndividualTransitionSchedulers[i]->IsIdle())
			{
			delete iIndividualTransitionSchedulers[i];
			iIndividualTransitionSchedulers[i] = NULL;
			iIndividualTransitionSchedulers.Remove(i);
			}
		}
	// If there are now none left, free the memory
	if(iIndividualTransitionSchedulers.Count() == 0)
		{
		iIndividualTransitionSchedulers.GranularCompress();
		}
	}
#endif
