// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIMPHONEINITIALISE_H_
#define __SIMPHONEINITIALISE_H_

/**
 @file
 This file contains timer and callback class for phone initialisation delay
 
 @internalAll
 */

#include <etelmm.h>
#include "csimtimer.h"
#include "CSimPhone.h"

	
// timer and callback classes for phone initialisation delay
NONSHARABLE_CLASS(CSimPhoneInitialise) : CBase, MTimerCallBack
	{
public:
	static CSimPhoneInitialise* NewL(CSimPhone* aPhone);
	~CSimPhoneInitialise();

	void Start(const TTsyReqHandle aHandle);
	void Cancel();

private:
	CSimPhoneInitialise(CSimPhone* aPhone);
	void ConstructL();
	
	void ReadDelayFromConfig();
	void TimerCallBack(TInt aId);

private:
	CSimPhone *iPhone;
	// timer that calls TimerCallBack after initialisation delay
	CSimTimer *iInitialiseDelayTimer;
	// the amount of time to delay
	TInt iPhoneInitialiseDelay;
	// handle of request to complete after delay
	TTsyReqHandle iPhoneInitialiseHandle;
	};

#endif // __SIMPHONEINITIALISE_H_
