// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Enable Wserv memory leak checks
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TMemLeakMode.h"
#include <w32debug.h>

//
//
CTMemLeakCheckEnable::CTMemLeakCheckEnable(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Enabling memory leak checks"));
	}

void CTMemLeakCheckEnable::ConstructL()
	{
	iIsWsTestMode = !iTest->IsFullRomL();
	if (iIsWsTestMode)
		{
		TheClient->iWs.DebugInfo(EWsDebugSetCheckHeapOnDisconnectMode,EWsCheckHeapOnDisconnectModeAlways);
		}
	}

void CTMemLeakCheckEnable::RunTestCaseL(TInt )
	{
	if (!iIsWsTestMode)
		{
#if defined(__WINS__)
		WARN_PRINTF1(_L("Test skipped as TechView appears to be running\n"));
#else
		INFO_PRINTF1(_L("Test skipped as TechView appears to be running\n"));
#endif
		}
	TestComplete();
	}
	
__WS_CONSTRUCT_STEP__(MemLeakCheckEnable)

//
//

CTMemLeakCheckOneShot::CTMemLeakCheckOneShot(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Triggering one shot memory leak check"));
	}

void CTMemLeakCheckOneShot::ConstructL()
	{
	iIsWsTestMode = !iTest->IsFullRomL();
	if (iIsWsTestMode)
		{
		TheClient->iWs.DebugInfo(EWsDebugSetCheckHeapOnDisconnectMode,EWsCheckHeapOnDisconnectModeOnce);
		}
	}

void CTMemLeakCheckOneShot::RunTestCaseL(TInt )
	{
	if (!iIsWsTestMode)
		{
#if defined(__WINS__)
		WARN_PRINTF1(_L("Test skipped as TechView appears to be running\n"));
#else
		INFO_PRINTF1(_L("Test skipped as TechView appears to be running\n"));
#endif
		}
	TestComplete();
	}

__WS_CONSTRUCT_STEP__(MemLeakCheckOneShot)
