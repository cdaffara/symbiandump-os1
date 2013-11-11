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
// Protocol Module gateway observer class implementation
// 
//

#include "ctestobserverbase.h"

const TInt KObserverTimerId = 0x10101011;
const TInt KObserverTimerDurationInMicroSec = 1000000;


CTestObserverBase::CTestObserverBase()
	{
	}


CTestObserverBase::~CTestObserverBase()
	{
	delete iTimer;
	delete iScheduler;
	}


void CTestObserverBase::ConstructL()
	{
	iScheduler = (CActiveSchedulerWait*) new CActiveSchedulerWait;
	iTimer = CLbsCallbackTimer::NewL(*this);
	}


void CTestObserverBase::ResetWasObserverCalled()
	{
	iNumberOfCallbacks = 0;
	iCallBackError = ECallBackErrorNone;
	}


void CTestObserverBase::WaitForCallBack()
	{
	if (iNumberOfCallbacks == 0)
		{
		iScheduler->Start();
		}
	}

// Let the test know that a callback has occurred and stop
// the scheduler to allow the test to check what particular
// callback has occurred. Also increase the count of callbacks.
void CTestObserverBase::SignalObserverCallBack(const TCallBackError aResult)
	{
	iTimer->Cancel();

	if (aResult != ECallBackErrorNone)
		{
		iCallBackError = aResult;
		}
	
	if (ECallBackErrorNone == aResult)
		{
		iNumberOfCallbacks ++;
		}
	
	if (iScheduler->IsStarted())
		{
		iScheduler->AsyncStop();
		}
	}

TBool CTestObserverBase::WasObserverCalled() const
	{
	return (iNumberOfCallbacks > 0);
	}

/** Handle a timeout
*/
void CTestObserverBase::OnTimerEventL(TInt aTimerId)
	{
	if (KObserverTimerId == aTimerId)
		{
		SignalObserverCallBack(ECallBackTimeout);
		}	
	}

/** Handle a timer error
*/
TInt CTestObserverBase::OnTimerError(TInt aTimerId, TInt aError)
	{
	(void)aError;
	
	if (KObserverTimerId == aTimerId)
		{
		SignalObserverCallBack(ECallBackTimerError);
		}
	return aError;
	}

TBool CTestObserverBase::IsMoreActivityObserved()
	{
	TBool ret;
	ResetWasObserverCalled();
	iTimer->EventAfter(TTimeIntervalMicroSeconds32(KObserverTimerDurationInMicroSec),KObserverTimerId);
	WaitForCallBack();
	ret = ECallBackTimeout == iCallBackError? EFalse : ETrue;
	ResetWasObserverCalled();
	return ret;
	}

TInt CTestObserverBase::NumberOfCallbacksReceived()
{
	return iNumberOfCallbacks;
}

void CTestObserverBase::CallbackHandled()
{
	// A callback has been handled by the test step
	iNumberOfCallbacks--;
	ASSERT(!(iNumberOfCallbacks < 0));
}
