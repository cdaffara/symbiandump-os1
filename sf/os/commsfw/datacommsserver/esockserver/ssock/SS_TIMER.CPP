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

/**
 @file
 @internalTechnology 
*/

#include <ss_pman.h>

EXPORT_C void SocketTimer::Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TDeltaTimerEntry& aTimer)
/**
Queue a timer on the global timer

*/
	{
	_LIT(sockTimer,"SockTimer");
	__ASSERT_ALWAYS(aTimeInMicroSeconds.Int()>0,User::Panic(sockTimer,0));

#ifdef __MARM__
	if(aTimeInMicroSeconds.Int()<100000)
		aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+KTimerGranularity+(KTimerGranularity>>2);
#endif
	SocketServer::GetTimer()->Queue(aTimeInMicroSeconds,aTimer);
	}

EXPORT_C void SocketTimer::Remove(TDeltaTimerEntry& aTimer)
/**
Call cancel on the global timer

*/
	{
	SocketServer::GetTimer()->Remove(aTimer);
	}
