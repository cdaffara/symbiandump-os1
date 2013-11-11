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
// Timer for lbs system
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef LBS_TIMER_H
#define LBS_TIMER_H

#include <e32base.h>



class LbsTimerUtils
{
public:

    IMPORT_C static TTime AddUpInt64TimersWithOverflowCheck(TTime timer, TTimeIntervalMicroSeconds interval);
    IMPORT_C static TTimeIntervalMicroSeconds SubtractInt64TimersWithUnderflowCheck(TTime endTime, TTime startTime);
};


//-------------------------------------------------------------------------------	
/** Timer call-back class.

@internalTechnology
@released
@see CLbsCallbackTimer
*/
class MLbsCallbackTimerObserver
	{
public:
	/** Called when the timer expires */
	virtual void OnTimerEventL(TInt aTimerId) = 0;
	/** Called if OnTimerEventL leaves */
	virtual TInt OnTimerError(TInt aTimerId, TInt aError) = 0;
	}; 


//-------------------------------------------------------------------------------
/** Lbs call-back timer.
All timers may complete late if the machine has been turned off for some period of time
after scheduling an event.

@internalTechnology
@released
@see MLbsCallbackTimerObserver
*/
class CLbsCallbackTimer : protected CTimer
	{
public:
	IMPORT_C static CLbsCallbackTimer* NewL(MLbsCallbackTimerObserver& aObserver);
	virtual ~CLbsCallbackTimer();
	
	IMPORT_C void EventAfter(const TTimeIntervalSeconds& aInterval, TInt aEventId);
	IMPORT_C void EventAfter(const TTimeIntervalMicroSeconds& aInterval, TInt aEventId);
	IMPORT_C void EventAfter(const TTimeIntervalMicroSeconds32& aInterval, TInt aEventId);
	IMPORT_C void EventAtUTC(const TTime& aTime, TInt aEventId);
	
	IMPORT_C TBool IsActive() const;
	IMPORT_C void Cancel();
	
protected:
	IMPORT_C CLbsCallbackTimer(MLbsCallbackTimerObserver& aObserver);
	IMPORT_C void ConstructL();
	
	// from CActive
	virtual void RunL();
	virtual TInt RunError(TInt aError);
		
	void StartTimer(TInt64 aInterval);
	
protected:
	TInt iEventId;
	TInt64 iIntervalLeft;
	MLbsCallbackTimerObserver& iObserver;
	}; 

#endif //LBS_TIMER_H
