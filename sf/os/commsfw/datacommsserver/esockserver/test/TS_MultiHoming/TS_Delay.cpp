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
// TS_CheckLinkLocalSame.cpp
// This checks that the link-local addresses on 2 conns
// are the same
// 
//

/**
 @file
*/
 
#include "TS_Delay.h"

/* 
 * 
 */
CTS_Delay::CTS_Delay()
	{
	iTestStepName = KDelay;
	}

CTS_Delay::~CTS_Delay()
	{	
	}

enum TVerdict CTS_Delay::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KDelay,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}


enum TVerdict CTS_Delay::doTestStepL(void)
	{
	TInt aDelay = 0;
	RTimer aTimer;

	
	
	aTimer.CreateLocal();

	GetIntFromConfig(KDelay, KDelayInMs, aDelay);

	TTimeIntervalMicroSeconds32 aInterval = aDelay * 1000;
	TRequestStatus aStatus;

	aTimer.After(aStatus, aInterval);
	User::WaitForRequest(aStatus);
	aTimer.Cancel();
	aTimer.Close();
	return EPass;

	}


