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

#include "RequestTimer.h"
#include "TestDummyValues.h"

CRequestTimer::CRequestTimer()
 : CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CRequestTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

CRequestTimer::~CRequestTimer()
	{
	Cancel();
	}

void CRequestTimer::Request(const RMessage2& aMessage, MRequestTimerUser* aSvrObject)
	{
	iMessage = aMessage;
	iSvrObject = aSvrObject;
	After(ASYNC_REQUEST_COMPLETION_TIME_MILLISECS);
	}

void CRequestTimer::CancelMessage()
	{
	Cancel();

	RMessagePtr2 messagePtr(iMessage);

	if (!messagePtr.IsNull())
		{
		iMessage.Complete(KErrCancel);
		}
	}

void CRequestTimer::RunL()
	{
	iSvrObject->TimerCompleted(iMessage);
	}

TInt CRequestTimer::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
