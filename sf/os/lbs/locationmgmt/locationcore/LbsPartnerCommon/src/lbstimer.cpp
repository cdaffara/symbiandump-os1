// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// System
#include <e32base.h>

// Lbs
#include "lbsdevloggermacros.h"

// Component
#include "lbstimer.h"

//-----------------------------------------------------------------------------
// CLbsCallbackTimer
//-----------------------------------------------------------------------------
/** Static constructor.
@return A pointer to the new CLbsCallbackTimer object.
@internalTechnology
@released*/
EXPORT_C CLbsCallbackTimer* CLbsCallbackTimer::NewL(MLbsCallbackTimerObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::NewL()\n");
	CLbsCallbackTimer* self = new(ELeave)CLbsCallbackTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Constructor.
@return A pointer to the new CLbsCallbackTimer object.
@internalTechnology
@released*/
EXPORT_C CLbsCallbackTimer::CLbsCallbackTimer(MLbsCallbackTimerObserver& aObserver)
	:CTimer(CActive::EPriorityHigh),
	iEventId(0),
	iObserver(aObserver)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::CLbsCallbackTimer()\n");
	}
	
/** Second phase constructor.
@internalTechnology
@released*/
EXPORT_C void CLbsCallbackTimer::ConstructL() 
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::ConstructL()\n");
	CTimer::ConstructL();
 	CActiveScheduler::Add(this);
	}

/** Destructor
@internalTechnology
@released*/
/*virtual*/CLbsCallbackTimer::~CLbsCallbackTimer()
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::~CLbsCallbackTimer()\n");
	}
	
/** 
@see CActive
@internalTechnology
@released*/
/*virtual*/void CLbsCallbackTimer::RunL()
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::RunL()\n");
	
	if (iIntervalLeft <= 0)
		{
		LBSLOG2(ELogP3, "TIMEOUT: iEventId:%d\n", iEventId);
		iObserver.OnTimerEventL(iEventId);
		}
	else
		{
		LBSLOG(ELogP0, "CLbsCallbackTimer::RunL() - new 32bit loop\n");
		StartTimer(iIntervalLeft);
		}
	}

/**
@see CActive
@internalTechnology
@released
*/
TInt CLbsCallbackTimer::RunError(TInt aError)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::RunError()\n");
	LBSLOG_ERR(ELogP3, "Leave in CLbsCallbackTimer::RunL()\n");
	// the error handler for the RunL above - called in here if the RunL above leaves
	return iObserver.OnTimerError(aError, iEventId);
	}

/** Requests an event after an interval.
The timer may complete late if the machine has been turned off for some period of time
after scheduling an event.

@param aInterval Interval
@param aEventId An event Id to be used in a call-back.
@internalTechnology
@released*/
EXPORT_C void CLbsCallbackTimer::EventAfter(const TTimeIntervalMicroSeconds& aInterval, TInt aEventId)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::EventAfter(micro 64)\n");

	iEventId = aEventId;
	TInt64 interval = aInterval.Int64();
	StartTimer(interval);
	}


/** Requests an event after an interval.
The timer may complete late if the machine has been turned off for some period of time
after scheduling an event.

@param aInterval Interval
@param aEventId An event Id to be used in a call-back.
@internalTechnology
@released*/
EXPORT_C void CLbsCallbackTimer::EventAfter(const TTimeIntervalMicroSeconds32& aInterval, TInt aEventId)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::EventAfter(micro 32)\n");

	iEventId = aEventId;
	TInt64 interval = aInterval.Int();
	StartTimer(interval);	
	}


/** Requests an event after an interval.
The timer may complete late if the machine has been turned off for some period of time
after scheduling an event.

@param aInterval Interval
@param aEventId An event Id to be used in a call-back.
@internalTechnology
@released*/
EXPORT_C void CLbsCallbackTimer::EventAfter(const TTimeIntervalSeconds& aInterval, TInt aEventId)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::EventAfter(seconds)\n");

	iEventId = aEventId;
	TInt64 interval = aInterval.Int()*1000000;
	StartTimer(interval);
	}
	
/** Requests an event at a given UTC time.
The timer may complete late if the machine has been turned off for some period of time
after scheduling an event.

@param aInterval Interval
@param aEventId An event Id to be used in a call-back.
@internalTechnology
@released*/
EXPORT_C void CLbsCallbackTimer::EventAtUTC(const TTime& aTime, TInt aEventId)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::EventAtUTC()\n");

	iEventId = aEventId;
		
	TTime cTime;
	cTime.UniversalTime();
	
	TInt64 interval = aTime.MicroSecondsFrom(cTime).Int64();
	StartTimer(interval);
	}

/** Reports if the timer is running.
@see CActive
@internalTechnology
@released*/
EXPORT_C TBool CLbsCallbackTimer::IsActive() const
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::IsActive()\n");
	return CActive::IsActive();
	}

/** Requests an event at a given UTC time.
The timer may complete late if the machine has been turned off for some period of time
after scheduling an event.

@param aInterval Interval
@param aEventId An event Id to be used in a call-back.
@internalTechnology
@released*/
EXPORT_C void CLbsCallbackTimer::Cancel()
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::Cancel()\n");
	LBSLOG2(ELogP2, "TIMER CANCEL: iEventId:%d\n", iEventId);
	CTimer::Cancel();
	}

/** Requests an event after aInterval microseconds
@param aInterval Interval
@internalTechnology
@released*/
void CLbsCallbackTimer::StartTimer(TInt64 aInterval)
	{
	LBSLOG(ELogP1, "CLbsCallbackTimer::StartTimer()\n");
	LBSLOG4(ELogP2, "TIMER START:  iEventId:%d, interval:%Ldus (%.6fs)\n", iEventId, aInterval, aInterval/1000000.0);
	
	TInt interval;
	if (aInterval < 0) 
		{
		interval = 0;
		iIntervalLeft = 0;
		}
	else if (aInterval <= KMaxTInt)
		{
		interval = aInterval;
		iIntervalLeft = 0;
		}
	else
		{
		interval = KMaxTInt;
		iIntervalLeft = aInterval - interval;
		}
		
	CTimer::HighRes(interval);
	}


//----------------------LbsTTimerUtils-----------------------------------------------

EXPORT_C TTime LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(TTime timer, TTimeIntervalMicroSeconds interval)
{
	if ((timer + interval) > 0)
	{
		return (timer+interval);
	}
	else
	{
		return Time::MaxTTime();
	}
}


EXPORT_C TTimeIntervalMicroSeconds LbsTimerUtils::SubtractInt64TimersWithUnderflowCheck(TTime endTime, TTime startTime)
	{
		TTimeIntervalMicroSeconds result = endTime.Int64() - startTime.Int64();
		
		if (result >= 0)
			{
			return result;
			}
		else
			{
			return static_cast<TTimeIntervalMicroSeconds>(0);
			}
	}
