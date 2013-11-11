// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TC_TIMER_H
#define __TC_TIMER_H

#include "tc_timer.h"
#include "td_privfw.h"

CTcTimer::CTcTimer(CTdPrivFW& aObserver) :
	CTimer(EPriorityStandard),
	iObserver(aObserver)
	{
	}
	
CTcTimer::~CTcTimer()
	{
	Cancel();
	}

void CTcTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CTcTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

void CTcTimer::RunL()
	{
	iObserver.StopWaiting();
	delete this;
	}

#endif
