/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file TestUtilities.h
*/
#if (!defined __TESTUTILITIES_H__)
#define __TESTUTILITIES_H__


// Delay to allow the queue statuses, etc, to complete - can be very short
// (but not too short; need to allow for debuggers, loggers, slow hardware)
const TUint KCompletionPause = 250 * 1000;			// microseconds

// Timeout before graceful close of channel pair becomes non-graceful
const TInt KGracefulCloseTimeout = 5000 * 1000;		// microseconds

// CBoundedCompletionWait - wait for a bounded time for completion
class CBoundedCompletionWait : public CActive
	{
public:
	static CBoundedCompletionWait* NewLC();
	~CBoundedCompletionWait();
	void WaitForRequestL(TTimeIntervalMicroSeconds32 aInterval);
	void WaitWithoutRequestL(TTimeIntervalMicroSeconds32 aInterval);
	operator TRequestStatus& () { return iStatus; }
	
	enum TWaitResult { EPendingRequest, EPendingTimer, ECompleted, ETimedOut };
	TWaitResult Result() const;
	TInt Status() const;
	void SetStatus(TInt aStatus);
protected:
	void RunL();
	void DoCancel();
	static TInt TimerCallBack(TAny*);
	void TimerCompleted();
private:
	CBoundedCompletionWait();
	CPeriodic* iTimer;
	TWaitResult iResult;
	};


// CSelfPopScheduler - thin extension of CActiveScheduler to give useful installation & removal cleanup-stack behaviour
class CSelfPopScheduler : public CActiveScheduler
	{
public:
	static CSelfPopScheduler* CreateLC();
	operator TCleanupItem();
private:
	static void Cleanup(TAny* aItem);
	};


#endif // __TESTUTILITIES_H__

