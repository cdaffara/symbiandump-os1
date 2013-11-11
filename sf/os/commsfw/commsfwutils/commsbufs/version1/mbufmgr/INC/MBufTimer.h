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
//

#if !defined(__MBufTimer_h__)
#define __MBufTimer_h__

/**
@internalTechnology
*/
const TUint KMBufWatchDogTime = 1000000;

//
// MBUF TIMER
//
class MBufTimer
/**
@internalTechnology
*/
	{
public:
	IMPORT_C static void Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TDeltaTimerEntry& aTimer);
	IMPORT_C static void Remove(TDeltaTimerEntry& aTimer);
	};

const TInt KMbufManTimerGranularity = 15500;

enum TMBufMgrActivePriority
	{
	EMBufMgrTimerPriority=50,
	};

#endif
