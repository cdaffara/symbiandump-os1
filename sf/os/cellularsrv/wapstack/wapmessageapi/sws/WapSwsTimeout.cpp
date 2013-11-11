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
// Timeout.cpp
// 
//

#include "WapSwsTimeOut.h"

//
// CTimeOutTimer: timer for comms time-outs
//
CTimeOutTimer::CTimeOutTimer(MTimeOutNotify& aTimeOutNotify)
: CTimer(EPriorityStandard), iNotify(aTimeOutNotify)
    {
    }

CTimeOutTimer::~CTimeOutTimer()
    {
	Cancel();
    }

CTimeOutTimer* CTimeOutTimer::NewL(MTimeOutNotify& aTimeOutNotify)
    {
    CTimeOutTimer *p = new (ELeave) CTimeOutTimer(aTimeOutNotify);
    CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop(p);
    return p;
    }

void CTimeOutTimer::ConstructL()
    {
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    }

void CTimeOutTimer::RunL()
    {
	// Timer request has completed, so notify the timer's owner
	iNotify.TimerExpired();
	}
