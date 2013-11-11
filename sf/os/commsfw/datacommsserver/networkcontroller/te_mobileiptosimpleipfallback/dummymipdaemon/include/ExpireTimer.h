/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface for timer targets and CExpireTimer class.
* 
*
*/



/**
 @file
 @internalComponent
*/

#ifndef __EXPIRETIMER_H__
#define __EXPIRETIMER_H__

#include <e32base.h>


class MExpireTimer
/**
 * A timer notification interface to alert an object that 
 * a timer has expired.
 *
 * @internalComponent
 */
   	{
public:
   	/** The method invoked when the timer expires. */
   	virtual void TimerExpired() = 0;
   	};


class CExpireTimer : public CTimer
/**
 * Implements a simple timer object for the server.
 *
 * @internalComponent
 */
	{
public:
	static CExpireTimer* NewL();
	~CExpireTimer();

   	void After(TTimeIntervalMicroSeconds32 aMcSecs, MExpireTimer& aExpireTimer);
   	void Cancel();

private:
	CExpireTimer();
	void RunL();

private:
	/** The target to notify when the timer expires. */
	MExpireTimer* iExpireTimer;
	};

inline CExpireTimer::CExpireTimer() : 
	CTimer(EPriorityStandard)
/**
 * Constructs the CExpireTimer object.
 *
 * @internalComponent
 */
	{
	}
	
#endif


