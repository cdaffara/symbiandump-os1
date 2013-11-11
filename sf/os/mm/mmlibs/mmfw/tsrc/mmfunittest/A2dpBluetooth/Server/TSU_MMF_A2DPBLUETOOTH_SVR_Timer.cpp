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

// EPOC includes
#include <e32base.h>
#include "TSU_MMF_A2DPBLUETOOTH_SVR_Timer.h"

CStopActiveSchedulerTimer* CStopActiveSchedulerTimer::NewL()
	{
	CStopActiveSchedulerTimer* self = new(ELeave)CStopActiveSchedulerTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	
	return self;
	}

void CStopActiveSchedulerTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

CStopActiveSchedulerTimer::CStopActiveSchedulerTimer() : CTimer(EPriorityHigh)
	{
	}


void CStopActiveSchedulerTimer::StopAfter(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	Cancel();
	After(aTimeInterval);	
	}


void CStopActiveSchedulerTimer::RunL()
	{
	if (CActiveScheduler::Current()->StackDepth())
		{
		CActiveScheduler::Stop();
		}
	}





 

