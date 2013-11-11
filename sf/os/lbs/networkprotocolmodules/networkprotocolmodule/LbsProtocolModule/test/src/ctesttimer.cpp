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
// This file provides the implementation of the timer class
// used by tests.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "ctesttimer.h"

//-----------------------------------------------------------------------------
// CTestTimer
//-----------------------------------------------------------------------------

CTestTimer* CTestTimer::NewL(TInt aEventId, MTestTimerObserver& aObserver)
	{
	CTestTimer* self = new(ELeave)CTestTimer(aEventId, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CTestTimer::CTestTimer(TInt aEventId, MTestTimerObserver& aObserver)
	:CTimer(EPriorityStandard),
	iEventId(aEventId),
	iObserver(aObserver)
	{
	}
	
	
void CTestTimer::ConstructL() 
	{
	CTimer::ConstructL();
   	CActiveScheduler::Add(this);
	}


void CTestTimer::RunL()
	{
	if (KErrCancel != iStatus.Int())
		{
		iObserver.OnTimerEvent(iEventId);
		}
	}

void CTestTimer::After(TTimeIntervalMicroSeconds32 aInterval)
	{
	TTime utcTime;
	utcTime.UniversalTime();
	utcTime += aInterval;
	AtUTC(utcTime);
	}

