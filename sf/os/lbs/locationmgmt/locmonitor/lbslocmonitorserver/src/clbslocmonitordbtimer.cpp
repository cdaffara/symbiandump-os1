/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include "clbslocmonitordbtimer.h"
#include "lbslocmonitordbenginedefs.h"
#include "lbsdevloggermacros.h"



CLbsLocMonitorDbTimer* CLbsLocMonitorDbTimer::NewL(MLocMonitorDBCallback& aObserver)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::NewL");
	CLbsLocMonitorDbTimer* self = new(ELeave)CLbsLocMonitorDbTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


void CLbsLocMonitorDbTimer::ConstructL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::ConstructL");
	User::LeaveIfError (iTimer.CreateLocal ());
	}


CLbsLocMonitorDbTimer::CLbsLocMonitorDbTimer(MLocMonitorDBCallback& aObserver):
CActive(EPriorityStandard),
iObserver(aObserver),
iRunning(EFalse)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::CLbsLocMonitorDbTimer");
	CActiveScheduler::Add(this);
	}


CLbsLocMonitorDbTimer::~CLbsLocMonitorDbTimer()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::~CLbsLocMonitorDbTimer");
	Cancel();
	iTimer.Close();
	}


TBool CLbsLocMonitorDbTimer::IsRunning()
    {
    LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::IsRunning");
    return iRunning;
    }


TBool CLbsLocMonitorDbTimer::StartTimer(TTimeIntervalSeconds aTimeInterval)
    {
    LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::StartTimer");
    TBool started = EFalse;
    if(!iRunning)
        {
        iRunning = ETrue;
        TTime targetTime;
        targetTime.UniversalTime(); // target time is now and try for 5 seconds!
        targetTime += aTimeInterval;
        iTimer.At(iStatus, targetTime);
        SetActive();
        started = ETrue;
        }
    return started;
    }


void CLbsLocMonitorDbTimer::RunL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::RunL");
	iRunning = EFalse;
	iObserver.FlushTimerCallback();
	}


void CLbsLocMonitorDbTimer::DoCancel()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbTimer::DoCancel");
	iTimer.Cancel();
	}		
