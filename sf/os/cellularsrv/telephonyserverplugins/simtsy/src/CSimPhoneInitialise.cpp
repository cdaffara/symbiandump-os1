// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// CSimOta.cpp
// Implements the OTA manipulation code.
// 
//

/**
 @file
*/

#include "utils.h"
#include "CSimPhone.h"
#include <testconfigfileparser.h>
#include "etelmmerr.h"

CSimPhoneInitialise* CSimPhoneInitialise::NewL(CSimPhone* aPhone)
	{
	CSimPhoneInitialise *me = new(ELeave) CSimPhoneInitialise(aPhone);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CSimPhoneInitialise::CSimPhoneInitialise(CSimPhone* aPhone) : iPhone(aPhone)
	{}
	
void CSimPhoneInitialise::ConstructL()
	{
	iInitialiseDelayTimer = CSimTimer::NewL(iPhone);
	ReadDelayFromConfig();
	}
	
CSimPhoneInitialise::~CSimPhoneInitialise()
	{
	if (iInitialiseDelayTimer)
		{
		iInitialiseDelayTimer->Cancel();
		}
	delete iInitialiseDelayTimer;
	}
	
void CSimPhoneInitialise::ReadDelayFromConfig()
	{
	const CTestConfigItem* item = iPhone->CfgFile()->Item(KPhoneInitialiseDelay);
	if(!item)
		return;

	TInt phoneInitialiseDelay;
	TInt ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, phoneInitialiseDelay);
	if (ret == KErrNone)
		{
		iPhoneInitialiseDelay = phoneInitialiseDelay;
		}
	}

void CSimPhoneInitialise::Start(const TTsyReqHandle aHandle)
	{
	iPhoneInitialiseHandle = aHandle;
	iInitialiseDelayTimer->Start(iPhoneInitialiseDelay, this);
	}

void CSimPhoneInitialise::Cancel()
	{
	iInitialiseDelayTimer->Cancel();
	if (iPhoneInitialiseHandle)
		{
		iPhone->ReqCompleted(iPhoneInitialiseHandle, KErrCancel);
		}
	iPhoneInitialiseHandle = NULL;
	}

void CSimPhoneInitialise::TimerCallBack(TInt /*aId*/)
	{
	iPhone->ReqCompleted(iPhoneInitialiseHandle, KErrNone);
	}
