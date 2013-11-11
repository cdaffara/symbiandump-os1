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
// Rudimentary timer class.
// 
//

/**
 @file
*/

#include "LoopbackTimer.h"

const TInt KOneMilliSecond=1000;	// < Constant multipler to map integer duration to millisecond timer.

CLoopbackTimer* CLoopbackTimer::NewL(TInt aDuration, MTimerCallBack* aCallback)
	{
	return new(ELeave) CLoopbackTimer(aDuration, aCallback);
	}

CLoopbackTimer::CLoopbackTimer(TInt aDuration, MTimerCallBack* aCallback)
	: CActive(EPriorityStandard), iDuration(aDuration), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	_LIT(KPanic,"CSimTimer::CSimTimer()");
	__ASSERT_ALWAYS(iTimer.CreateLocal() == KErrNone, User::Panic(KPanic,KErrGeneral));
	}

CLoopbackTimer::~CLoopbackTimer()
	{
	Cancel();
	iTimer.Close();
	}

void CLoopbackTimer::Start()
	{
	if (!iRunning)
		{
		iRunning = ETrue;
		iTimer.After(iStatus, iDuration*KOneMilliSecond);
		SetActive();
		}
	}

void CLoopbackTimer::DoCancel()
	{
	iTimer.Cancel();
	iRunning = EFalse;
	}


void CLoopbackTimer::RunL()
	{
	User::LeaveIfError(KErrNone);	// To prevent LeaveScan warning
	iRunning = EFalse;
	iCallback->TimerCallBack();
	}
	
TBool CLoopbackTimer::Running() const
	{
	return iRunning;
	}

