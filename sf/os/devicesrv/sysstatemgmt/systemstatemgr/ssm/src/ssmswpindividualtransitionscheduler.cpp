// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ssmserverpanic.h"
#include "clesessionproxy.h"
#include "ssmswptransitionengine.h"
#include "ssmswptransitionscheduler.h"
#include "ssmswpindividualtransitionscheduler.h"


//
//Construction/Destruction
//


/**
Create a scheduler
*/
CSsmSwpIndividualTransitionScheduler* CSsmSwpIndividualTransitionScheduler::NewLC(TUint aSwpKey, MCleSessionProxy* aCleSession, CSsmSwpTransitionScheduler& aParent)
	{
	CSsmSwpIndividualTransitionScheduler* self = new (ELeave) CSsmSwpIndividualTransitionScheduler(aSwpKey, aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aCleSession);
	return self;
	}

void CSsmSwpIndividualTransitionScheduler::ConstructL(MCleSessionProxy* aCleSession)
	{
	// Need to create an individual CLE session proxy for each transition scheduler
	iCleSession = aCleSession->CreateNewProxyL();
	
	// Need to create an individual swppolicy session for each transition scheduler
	iSsmSwpPolicySession = CSsmSwpPolicyCliSession::NewL();
	}

CSsmSwpIndividualTransitionScheduler::CSsmSwpIndividualTransitionScheduler(TUint aSwpKey, CSsmSwpTransitionScheduler& aParent)
	: 	CActive(CActive::EPriorityStandard),
		iSchedulerState(EIdle),
		iSwpKey(aSwpKey),
		iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

/**
Deactivate
Destroy queue
*/
CSsmSwpIndividualTransitionScheduler::~CSsmSwpIndividualTransitionScheduler()
	{
	CActive::Cancel();
	__ASSERT_DEBUG(iTransitionQueue.Count() == 0, PanicNow(KPanicSysStateMgr,ESwpTransitionSchedulerError1));
	DestroyTransitionQueue();
	if(iCleSession)
		{
		iCleSession->Close();
		iCleSession->ReleaseCle();
		iCleSession = NULL;
		}
	if(iSsmSwpPolicySession)
		{
		delete iSsmSwpPolicySession;
		}
	}

/**
Take ownership of the transition engine and prepare to run it
@param aEngine - the engine to run
*/
void CSsmSwpIndividualTransitionScheduler::SubmitL(CSsmSwpTransitionEngine* aEngine)
	{
	if(!aEngine)
		{
		SSMLOGLEAVE(KErrArgument); //lint !e527 Suppress Unreachable. Lint is just confused by macro, warning goes away when code in macro gets expanded
		}
	// Change aEngine to having the cle proxy from this scheduler
	// This allows multiple SwP transitions
	aEngine->SetCleSessionProxy(iCleSession);

	// Change aEngine to have the SsmSwpPolicySession from this scheduler. i.e, same SsmSwpPolicySession
	// instance will be used for the all the individual Swp transitions.(the same swp policy session would be used).
	// This allows multiple SwP transitions
	aEngine->SetSsmSwpPolicySession(iSsmSwpPolicySession);
	DoSubmitL(aEngine);
	}

void CSsmSwpIndividualTransitionScheduler::Cancel(CSession2* aSession)
	{
	//Cancel all queued requests that originates from aSession
	CancelTransitionQueue(aSession);

	// Cancel iCurrentTransition if it originates from aSession && still possible to Cancel (i.e. not started)
	if(CurrentTransitionOriginatesFrom(aSession) && !iCurrentTransition->InTransition())
		{
		iCancellingSession = aSession;
		CActive::Cancel();
		iCancellingSession = NULL;

		//We cancelled the currently running engine, see if there is anything left in the queue
		iCurrentTransition = GetNextFromQueue();
		if(iCurrentTransition != NULL) // we have more
			{
			ScheduleTransition();
			}
		}
	}

TBool CSsmSwpIndividualTransitionScheduler::CurrentTransitionOriginatesFrom(CSession2* aSession)
	{
	return (iCurrentTransition != NULL) && (iCurrentTransition->OriginatingSesssion() == aSession);
	}

TUint CSsmSwpIndividualTransitionScheduler::SwpKey() const
	{
	return iSwpKey;
	}

TBool CSsmSwpIndividualTransitionScheduler::IsIdle() const
	{
	return iTransitionQueue.Count() == 0 && iCurrentTransition == NULL;
	}

//
// from CActive
//

/**
Cancel running transition if any and destroy any transition in the queue that originates from the cancelling session
*/
void CSsmSwpIndividualTransitionScheduler::DoCancel()
	{
	// Cancel the running transition if it originates from the calling session
	if(CurrentTransitionOriginatesFrom(iCancellingSession))
		{
		iCurrentTransition->Cancel();
		delete iCurrentTransition;
		iCurrentTransition = NULL;
		}
	}

/**
The only valid event at the moment is completion of engine for any reason.
Except few errors (for example, when a transition is not allowed),
most of the unexpected transition errors are identified and dealt in the engine. 
Notes: Tries to start the next engine even if there was an error.
*/
void CSsmSwpIndividualTransitionScheduler::RunL()
	{
	if(KErrNone == iStatus.Int())
		{
		NotifyTransitionCompleted();
		}
	else
		{
		NotifyTransitionFailed();
		}

	// try to start the next transition
	delete iCurrentTransition;
	iCurrentTransition = NULL;
	iCurrentTransition = GetNextFromQueue();
	if(iCurrentTransition != NULL) // we have more
		{
		ScheduleTransition();
		}
	}

/**
Log an error occured

@param aError the error

*/
TInt CSsmSwpIndividualTransitionScheduler::RunError(TInt aError)
	{
	(void)aError;
	DEBUGPRINT2(_L("CSsmSwpIndividualTransitionScheduler leave detected in RunL, error: %d"), aError);
	PanicNow(KPanicSysStateMgr,ESwpTransitionSchedulerError5);
	return KErrNone;
	}

//
// private: methods
//

/**
Start a transition if none is running, otherwise put it into the queue

@param aEngine the transition engine to submit
@leave one of the system wide error codes if insert fails

*/
void CSsmSwpIndividualTransitionScheduler::DoSubmitL(CSsmSwpTransitionEngine* aEngine)
	{
	if(IsActive())
		{
		// add to Queue
		iTransitionQueue.InsertL(aEngine, 0);
		NotifyTransitionQueued();
		}
	else
		{
		// start immediately
		__ASSERT_DEBUG(iCurrentTransition == NULL,  PanicNow(KPanicSysStateMgr,ESwpTransitionSchedulerError2));
		iCurrentTransition = aEngine;
		ScheduleTransition();
		}
	}

/**
Start the transition running and make it current

@param aEngine the transition engine to submit
@leave one of the system wide error codes
*/
void CSsmSwpIndividualTransitionScheduler::ScheduleTransition()
	{
	__ASSERT_DEBUG(!IsActive() && IsAdded(), PanicNow(KPanicSysStateMgr,ESwpTransitionSchedulerError3));
	__ASSERT_DEBUG(iCurrentTransition != NULL, PanicNow(KPanicSysStateMgr,ESwpTransitionSchedulerError4));

	iCurrentTransition->Submit(iStatus);
	NotifyTransitionStarted();
	SetActive();
	}

/**
Retrieve the next transition engine from the queue
return NULL if the queue is empty

@return - the next transition engine or NULL
*/
CSsmSwpTransitionEngine* CSsmSwpIndividualTransitionScheduler::GetNextFromQueue()
	{
	TInt nextPos = iTransitionQueue.Count() - 1;
	if(nextPos < 0) // Q empty
		{
		return NULL;
		}
	CSsmSwpTransitionEngine* ret = iTransitionQueue[nextPos];
	iTransitionQueue.Remove(nextPos);
	return ret;
	}

/**
Remove and delete all queue entries, cancel any pending entries
*/
void CSsmSwpIndividualTransitionScheduler::DestroyTransitionQueue()
	{
	const TInt count = iTransitionQueue.Count();
	for(TInt i=0; i<count; i++)
		{
		iTransitionQueue[i]->CompleteClientMessage(KErrCancel);
		}
	iTransitionQueue.ResetAndDestroy();
	}

/**
Cancel, remove and delete all queue entries originating from aSession
*/
void CSsmSwpIndividualTransitionScheduler::CancelTransitionQueue(CSession2* aSession)
	{
	TInt count = iTransitionQueue.Count();
	while(count--)
		{
		CSsmSwpTransitionEngine* engine = iTransitionQueue[count];
		if( engine && (engine->OriginatingSesssion() == aSession) )
			{
			engine->CompleteClientMessage(KErrCancel);
			iTransitionQueue.Remove(count);
			delete engine;
			}
		}
	}

/**
Notify the monitor (if set) that one of the transition events has occured
*/
void CSsmSwpIndividualTransitionScheduler::NotifyTransitionQueued()
	{
	iParent.NotifyTransitionQueued();
	}

void CSsmSwpIndividualTransitionScheduler::NotifyTransitionStarted()
	{
	iParent.NotifyTransitionStarted();
	}

void CSsmSwpIndividualTransitionScheduler::NotifyTransitionCompleted()
	{
	DEBUGPRINT1(_L("ERequestSwpChange Completed"));
	iParent.NotifyTransitionCompleted();
	}

void CSsmSwpIndividualTransitionScheduler::NotifyTransitionFailed()
	{
	DEBUGPRINT1(_L("Error: ERequestSwpChange Failed"));
	iParent.NotifyTransitionFailed();
	}
