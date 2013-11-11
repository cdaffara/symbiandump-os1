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
//

#include "t_activewaiter.h"

CActiveWaiter::CActiveWaiter()
	: CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}
	
CActiveWaiter::~CActiveWaiter()
	{
	Cancel();
	}

void CActiveWaiter::SetActiveDontWait()
	{
	SetActive();
	}

void CActiveWaiter::WaitActive()
	{ 
	ASSERT(!iScheduler.IsStarted());
	SetActive();
	
	iScheduler.Start();
	}

void CActiveWaiter::RunL()
	{
	if (iScheduler.IsStarted())
		{
		ASSERT(iScheduler.CanStopNow());
		iScheduler.AsyncStop();
		}
	}

void CActiveWaiter::DoCancel()
	{
	if (iScheduler.IsStarted())
		{
		ASSERT(iScheduler.CanStopNow());
		iScheduler.AsyncStop();
		}
	}
