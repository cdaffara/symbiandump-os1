// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "subtitletimer.h"


CSubtitleTimer::CSubtitleTimer(MMMFDevSubtitleCallback& aCallback) : 
	CTimer(EPriorityHigh),
	iMMFDevSubtitleCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CSubtitleTimer* CSubtitleTimer::NewL(MMMFDevSubtitleCallback& aCallback)
	{
	CSubtitleTimer* instance = new (ELeave) CSubtitleTimer(aCallback);
	CleanupStack::PushL(instance);
	
	// Call CTimers contructL
	instance->ConstructL();
	CleanupStack::Pop(instance);
	return instance;
	}

void CSubtitleTimer::StartTimer(const TTimeIntervalMicroSeconds32& aTime)
	{
	HighRes(aTime);
	}
	
void CSubtitleTimer::RunL()
	{
	if (iStatus == KErrNone)
		{
		// Inform devsubtitle of the timers experation
		iMMFDevSubtitleCallback.SubtitleTimerExpired();
		}
	}
