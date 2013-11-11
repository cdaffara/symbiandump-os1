// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: A timer object to simulate the delay in the processing of a request
// for reference location from the SuplProxyProtocolModule.
//

#include "te_delaytimer.h"


CTe_DelayTimer* CTe_DelayTimer::NewL(MTimerCallback& aObserver)
    {
    CTe_DelayTimer* delayTimer = new (ELeave) CTe_DelayTimer(aObserver);
    CleanupStack::PushL(delayTimer);
    delayTimer->ConstructL();
    CleanupStack::Pop(delayTimer);
    return delayTimer;
    }

CTe_DelayTimer::CTe_DelayTimer(MTimerCallback& aObserver):
    CTimer(EPriorityStandard),iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

void CTe_DelayTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

CTe_DelayTimer::~CTe_DelayTimer()
    {
    Cancel();
    }

void CTe_DelayTimer::RunL()
    {
    iObserver.CompleteCallback();
    }

TInt CTe_DelayTimer::RunError(TInt aError)
    {
    return aError;
    }

void CTe_DelayTimer::Start()
    {
    if (!IsActive())
        {
         After(iDelay);
        }
    else
        {
        Cancel();
        After(iDelay);
        }
    }

//Utility method to set the value of delay
void CTe_DelayTimer::SetDelay(const TTimeIntervalMicroSeconds32 aDelay)
    {
    //Set the timer delay value
    iDelay = aDelay;
    }
