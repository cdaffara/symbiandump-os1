// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "btsockettimer.h"

/*static*/ void BTSocketTimer::Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TDeltaTimerEntry& aTimer)
	{
	SBtTls* tls = static_cast<SBtTls*>(Dll::Tls());

	_LIT(sockTimer,"BTSocketTimer");
	__ASSERT_ALWAYS(aTimeInMicroSeconds.Int()>0,User::Panic(sockTimer,0));
	__ASSERT_DEBUG(tls, User::Panic(sockTimer, 1));
	__ASSERT_DEBUG(tls->iTimer, User::Panic(sockTimer, 2));
	
	if(aTimeInMicroSeconds.Int()<100000)
	    aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+KTimerGranularity+(KTimerGranularity>>2);

	tls->iTimer->Queue(aTimeInMicroSeconds, aTimer);
	}


/*static*/ void BTSocketTimer::Remove(TDeltaTimerEntry& aTimer)
	{
	SBtTls& tls = *static_cast<SBtTls*>(Dll::Tls());

	tls.iTimer->Remove(aTimer);
	}


/*static*/ void BTSocketTimer::InitL()
	{
	SBtTls& tls = *static_cast<SBtTls*>(Dll::Tls());

	if (!tls.iTimer)
		{
		tls.iTimer=CDeltaTimer::NewL(KTimerPriority, KTimerGranularity);
		}
	}


/*static*/ void BTSocketTimer::Close()
	{
	SBtTls* tls = static_cast<SBtTls*>(Dll::Tls());

	if (tls)
		{
		delete tls->iTimer;
		tls->iTimer = NULL;
		}
	}
