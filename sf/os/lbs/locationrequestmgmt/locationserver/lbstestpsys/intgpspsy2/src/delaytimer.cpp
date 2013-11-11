// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "delaytimer.h"

CDelayTimer::CDelayTimer()
 : CTimer(EPriorityStandard)
{
    iSvrObject = NULL;
	CActiveScheduler::Add(this);
}

void CDelayTimer::ConstructL()
{
	CTimer::ConstructL();
}

CDelayTimer::~CDelayTimer()
{
	Cancel();
}

void CDelayTimer::Start(
        const TInt aTimerId, 
        const TTimeIntervalMicroSeconds32 aDelay,
        MDelayTimerUser* aSvrObject)
{
    __ASSERT_ALWAYS(aSvrObject != NULL, User::Panic(_L("DelayTimer"), KErrArgument));

	iTimerId = aTimerId;
	iSvrObject = aSvrObject;
	After(aDelay);
}

void CDelayTimer::RunL()
{
	if (iSvrObject) iSvrObject->TimerCompleted(iTimerId);
}

TInt CDelayTimer::RunError(TInt /*aError*/)
{
    return KErrNone;
}
