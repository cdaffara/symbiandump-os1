// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Rudimentary timer class that acts as an observer, which is used as a
// way to reduce the time duration from outside the Sim.Tsy.
// 
//

/**
 @file
*/

#include "csimtimer.h"
#include "CSimPhone.h"


const TInt KOneSecond=1000000;	// < Constant multipler to map integer duration to millisecond timer.

/**
Factory function used to create a timer object that can be signalled to
reduce its time.

@return A CSimTime object, which has the functionality to reduce the
        time duration being run.
@param aPhone The object created by this function is an observer which
              will be regitered with this CSimPhone object's
              CSimReduceTimers subject.
*/
CSimTimer* CSimTimer::NewL(CSimPhone* aPhone)
	{
	CSimTimer* self = new(ELeave) CSimTimer();
	CleanupStack::PushL(self);
	self->ConstructL(aPhone);
	CleanupStack::Pop(self);
	return self;
	}

/**
Second-phase construction of the timer.  Obtains the observer subject
from the CSimPhone object to ensure updates are made relative to all
timers.

@param aPhone The new observer will be regitered with this CSimPhone
              object's CSimReduceTimers subject.
*/
void CSimTimer::ConstructL(CSimPhone* aPhone)
	{
	iReduceTimersSubject = aPhone->GetReduceTimersSubject();
	}

/**
Default, first-phase, constructor for the timer (and observer) object.
*/
CSimTimer::CSimTimer()
				: CActive(EPriorityStandard), iRegistered(EFalse)
	{
	CActiveScheduler::Add(this);
	_LIT(KPanic,"CSimTimer::CSimTimer()");
	__ASSERT_ALWAYS(iTimer.CreateLocal() == KErrNone, User::Panic(KPanic,KErrGeneral));
	}

/**
Default destructor for CSimTimer.  Ensures the timer is unregistered as
an observer from its subject and cancels any outstanding requests.
*/
CSimTimer::~CSimTimer()
	{
	// Ensure that this observer is unregistered from its subject.
	if (iRegistered)
		{
		TRAP_IGNORE(iReduceTimersSubject->DetachL(this));
		}
	Cancel();
	iTimer.Close();
	}

/**
Starts the timer.  Waits for aDuration time, passed as a variable to
this function, before completing.

@param aDuration The duration time this timer should wait before
                 completing.
@param aCallback A pointer to the MTimerCallBack object which will be
                 notified when aDuration time passes.
*/
void CSimTimer::Start(TInt aDuration, MTimerCallBack* aCallback)
	{
	Start(aDuration,aCallback,KErrNotFound);
	}

/**
Starts the timer.  Waits for aDuration time, passed as a variable to
this function, before completing.

@param aDuration The duration time this timer should wait before
                 completing.
@param aCallback A pointer to the MTimerCallBack object which will be
                 notified when aDuration time passes.
@param aId An integer that will be passed to the aCallback object when
           the timer completes.  This can be used, by the component
           using this timer, to identify the purpose of the timer.
           This parameter is optional, thus, if not required
           Start(TInt, MTimerCallBack*) can be used.
*/
void CSimTimer::Start(TInt aDuration, MTimerCallBack* aCallback,TInt aId)
	{
	iCallBackId=aId;
	iCallback=aCallback;
	iDuration = aDuration;
	RegisterWithSubjectInCSimPhone();
	iStartTime.UniversalTime();
	iTimer.After(iStatus,aDuration*KOneSecond);
	SetActive();
	}

/**
Stops the timer events.
*/
void CSimTimer::DoCancel()
	{
	iTimer.Cancel();
	}

/**
Calls the MTimerCallBack::TimerCallBack function when the duration time
given to Start() completes.
*/
void CSimTimer::RunL()
	{
	iStartTime = 0;
	UnRegisterFromSubjectInCSimPhone();
	iCallback->TimerCallBack(iCallBackId);
	}

/**
Indicates whether the timer is running or not.

@return Etrue if the timer is running, Efalse otherwise.
*/
TBool CSimTimer::Running() const
	{
	return IsActive();
	}

/**
Function for obtaining the amount of time, in seconds, remaining on
this timer.

@return The number of remaining whole seconds on the duration of this
        timer.  This function can return error in very unlikely cases
        where time remaining is negative (KErrAbort), or if the number
        of elapsed seconds, from when the timer started, is too large
        for 32-bit integer (KErrOverflow).  KErrNotFound is returned if
        the timer is not running; e.g. if the timer has been cancelled.
*/
TInt CSimTimer::GetRemainingSeconds() const
	{
	if (!IsActive())
		{
		return KErrNotFound;
		}

	TTime endAt;
	endAt.UniversalTime();
	TTimeIntervalSeconds secsInterval;

	TInt err = endAt.SecondsFrom(iStartTime,secsInterval);
	if (err != KErrNone)
		{
		return err;
		}

	TInt timeElapsed = secsInterval.Int();
	if (timeElapsed < 0)
		{
		return KErrAbort;
		}

	return iDuration-timeElapsed;
	}

/**
Reduces the remaining time duration, of this timer, by the specified
number of seconds, which must be a number greater than or equal to the
remaining time on this timer, otherwise deduction will not be made.

Note: Although the CSimTimer object is registered on a call to Start,
it may stay registered even if the timer isn't running.  This can
happen when DoCancel() is called from outside this object.  In such a
case, any further Update() calls made to this object are ignored, since
Running() will be EFalse.

@param aSecsOffTimer Number of seconds to deduct off the remaining
                     duration of this timer.  This must be a number of
                     seconds greater than that remaining on the timer,
                     otherwise this method has no effect.
*/
void CSimTimer::Update(TInt aSecsOffTimer)
	{
	if (aSecsOffTimer < 0 || !IsActive())
		{
		return;
		}

	TInt remaining = GetRemainingSeconds();

	// Ensure there is enough time remaining to deduct aSeconds.
	// Also ensures that RemainingSeconds() does not return an error.
	if (remaining >= aSecsOffTimer)
		{
		Cancel();
		TInt newDelay = remaining - aSecsOffTimer;
		iTimer.After(iStatus,newDelay*KOneSecond);
		SetActive();
		}
	}
	
/**
Function for obtaining information about the event assocated with the timer

@return The timer event ID as defined in TTimerEventId in simtsy.h
*/
TInt CSimTimer::GetTimerEventInfo() const
	{
	return iCallBackId;
	}

/**
Registers observer to the subject obtained from the CSimPhone object,
given during construction.
*/
void CSimTimer::RegisterWithSubjectInCSimPhone()
	{
	if (!iRegistered)
		{
		TRAPD(err, iReduceTimersSubject->AttachL(this));
		if (err == KErrNone)
			{
			iRegistered = ETrue;
			}
		}
	}

/**
Unregisters observer from the subject obtained from the CSimPhone
object, given during construction.
*/
void CSimTimer::UnRegisterFromSubjectInCSimPhone()
	{
	if (iRegistered)
		{
		TRAPD(err, iReduceTimersSubject->DetachL(this));
		if (err == KErrNone)
			{
			iRegistered = EFalse;
			}
		}
	}
