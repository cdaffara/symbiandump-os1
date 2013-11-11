// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIMTIMER_H__
#define __SIMTIMER_H__

/**
 * @file
 *
 * Header for timer class with time decrease mechanism using an
 * observer pattern.
 *
 * @internalAll
 */

#include <e32base.h>
#include <simtsy.h>
#include "CSimReduceTimers.h"


enum TDirection
	{
	ESmsRx,
	ESmsTx
	};


class MTimerCallBack;
class CSimPhone;
/**
Definition for a timer with observer functionality, which can be used
retrieve and update the time duration at any point during the
execution.
*/
class CSimTimer : public CActive, MSimTimerUpdateObserver
	{
public:
	static CSimTimer* NewL(CSimPhone* aPhone);
	~CSimTimer();
	void Start(TInt aDuration, MTimerCallBack* aCallback);
	void Start(TInt aDuration, MTimerCallBack* aCallback,TInt aId);
	void DoCancel();
	TBool Running() const;
	void Update(TInt aSecsOffTimer);
	TInt GetRemainingSeconds() const;
	TInt GetTimerEventInfo() const;

private:
	CSimTimer();
	void ConstructL(CSimPhone* aPhone);
	void RunL();
	void RegisterWithSubjectInCSimPhone();
	void UnRegisterFromSubjectInCSimPhone();

private:
	/** The RTimer handle used to request a specific delay before the
	    RunL() function is called by the active scheduler..*/
	RTimer iTimer;
	/** The duration requested of the timer, when a call is made to
	    Start().*/
	TInt iDuration;
	/** The system time when the timer started.*/
	TTime iStartTime;
	/** Optional value; can be used to identify the purpose of the
	    timer to the iCallback object.*/
	TInt iCallBackId;
	/** The object to send a call-back to when the timer finishes.*/
	MTimerCallBack* iCallback;
	/**	The subject with which this observer is registered.*/
	CSimReduceTimers* iReduceTimersSubject;
	/** States whether this observer has successfully attached to
	    the subject.*/
	TBool iRegistered;
	};


/**
Abstract definition of the object required for the CSimTimer to pass
a message back to, thus indicating that the timer duration has expired.
*/
class MTimerCallBack
	{
public:
	/**
	A call-back function for when the given timer duration has ended.

	@param aId
	*/
	virtual void TimerCallBack(TInt aId)=0;
	};

#endif
