// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Timer for CMBufManager 
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>
#include "MBufTimer.h"

EXPORT_C void MBufTimer::Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TDeltaTimerEntry& aTimer)
//
// Queue a timer on the global timer
//
	{
	_LIT(mbufTimer,"MBufTimer");
	__ASSERT_ALWAYS(aTimeInMicroSeconds.Int()>0,User::Panic(mbufTimer,0));


	if(aTimeInMicroSeconds.Int()<100000)
		aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+KMbufManTimerGranularity+(KMbufManTimerGranularity>>2);

	CMBufManager::Timer()->Queue(aTimeInMicroSeconds,aTimer);
	}

EXPORT_C void MBufTimer::Remove(TDeltaTimerEntry& aTimer)
//
// Call cancel on the global timer
//
	{
	CMBufManager::Timer()->Remove(aTimer);
	}
