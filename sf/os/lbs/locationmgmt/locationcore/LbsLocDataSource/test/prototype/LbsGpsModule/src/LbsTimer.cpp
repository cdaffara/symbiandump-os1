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
//


#include "LbsTimer.h"

CLbsTimer* CLbsTimer::NewL(MLbsTimerObserver* aObserver, TInt aTimerId)
	{
	CLbsTimer* self = new(ELeave) CLbsTimer(aObserver, aTimerId);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}


CLbsTimer::CLbsTimer(MLbsTimerObserver* aObserver, TInt aTimerId)
	:
	CTimer(CTimer::EPriorityStandard),
	iObserver(aObserver),
	iTimerId(aTimerId)
	{
	CActiveScheduler::Add(this);
	}


CLbsTimer::~CLbsTimer()
	{
	Cancel();
	}

void CLbsTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CLbsTimer::SetTimer(const TTime& aUtcTargetTime)
	{
	Cancel();
	
	iTargetTime = aUtcTargetTime;

	TTime zeroTime(0);

	if (aUtcTargetTime == zeroTime)
		{
		return;
		}
	
	TTime timeNow;
	timeNow.UniversalTime();
	
	TTimeIntervalMicroSeconds delay(0);
	
	if (timeNow < aUtcTargetTime)
		{
		delay = aUtcTargetTime.MicroSecondsFrom(timeNow);
		}

	TInt delay32 = static_cast<TInt>(I64LOW(delay.Int64()));
	
	if (delay32 < 0)
		{
		delay32 = 0;
		}

	TTimeIntervalMicroSeconds32 time32 = TTimeIntervalMicroSeconds32(delay32);
	
	After(time32);
	}
	
void CLbsTimer::CancelTimer()
	{
	Cancel();
	}
	
void CLbsTimer::RunL()
	{
	if (iObserver)
		{
		iObserver->HandleTimer(iTimerId, iTargetTime);
		}
	}


