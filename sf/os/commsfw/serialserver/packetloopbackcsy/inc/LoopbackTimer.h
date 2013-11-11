// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for timer class
// 
//

/**
 @file
*/

#ifndef __LOOPBACKTIMER_H__
#define __LOOPBACKTIMER_H__

#include <e32base.h>

class MTimerCallBack;
NONSHARABLE_CLASS(CLoopbackTimer) : public CActive
	{
public:
	static CLoopbackTimer* NewL(TInt aDuration, MTimerCallBack* aCallback);
	~CLoopbackTimer();
	void Start();
	TBool Running() const;

private:
	CLoopbackTimer(TInt aDuration, MTimerCallBack* aCallback);
	void DoCancel();
	void RunL();

private:
    /** Timer used to control when RunL is called */
	RTimer iTimer;
    /** Timer's duration */
	TInt iDuration;
    /** Class with implementation of MTimerCallBack::TimerCallBack that is called when the timer expires */
	MTimerCallBack* iCallback;
    /** True when the timer is running */
	TBool iRunning;
	};

class MTimerCallBack
	{
public:
	virtual void TimerCallBack() = 0;
	};

#endif // __LOOPBACKTIMER_H__

