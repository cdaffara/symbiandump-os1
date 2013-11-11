// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 4
// 
//

// EPOC includes
#include <e32base.h>
#include <es_mbman.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
//#include <networking/teststep.h>
//#include <networking/testsuite.h>

#include "TestStepRootServer.h"
#include "TestAsynchHandler.h"

#include "RootServerTest.h"
#include "RootServerTestSection4.h"

// Test step 4.1 - SuddenDeathListener
CRootServerTest4_1::CRootServerTest4_1(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
	{
	// store the name of this test case
	SetTestStepName(_L("Test4.1"));
	}


TVerdict CRootServerTest4_1::doTestStepL( void )
	{
										SetTestStepResult(EPass);
	INFO_PRINTF1(_L("Test Purpose: SuddenDeathListener"));

	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
#if defined(SYMBIAN_C32ROOT_API_V3)
	startParams.iParams.iControlFlags = TRSStartModuleParamContainer::KCF_UnstableModule;
#endif
	iniData.iParams.iDeathDelay = isTestServerLifeTime;

	INFO_PRINTF2(_L("02 Loading module: %S"), &startParams.iParams.iName);
	asynchHandler->TryLoad(startParams, iniData);

	CActiveScheduler::Start();
    if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}

	// create a new active object for the sudden death handler
	CTestAsynchHandler* suddenDeathHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	// wait for either sudden death indication or timeout
	INFO_PRINTF2(_L("03 Waiting for sudden death for up to %1.3f seconds"), isTestServerWaitTime / 1000.0);
	asynchHandler->WaitForTimer(isTestServerWaitTime);
	suddenDeathHandler->TryWaitForDeath();
	CActiveScheduler::Start();

	// uninstall active scheduler
	asynchHandler->Cancel();
	suddenDeathHandler->Cancel();

	// see which events have occurred 
	// if root server worked then timer should not have expired
	// and sudden death handler AO should have returned to idle
	if (CTestAsynchHandler::EWaitingforTimer != asynchHandler->GetiState())
		{
		INFO_PRINTF1(_L("Timer expired"));
		SetTestStepResult(EFail);	
		}
	
	if (CTestAsynchHandler::EIdle != suddenDeathHandler->GetiState())
		{
		INFO_PRINTF1(_L("SuddenDeathHandler was not triggered"));
		SetTestStepResult(EFail);	
		}

	// enumerate servers
	INFO_PRINTF1(_L("04 Enumerating modules"));

	TCFModuleName moduleName;
	TRSIter position;
	TInt ret = iRootServerSuite->RootSess().EnumerateModules(position, moduleName);
	INFO_PRINTF2(_L("Enumerate returned %d"), ret);
	if (KErrEof != ret)
		{
		INFO_PRINTF1(_L("Module left alive"));
		SetTestStepResult(EFail);	
		}
	// destroy active objects and scheduler
	CleanupStack::PopAndDestroy(suddenDeathHandler);
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);

	return TestStepResult();
	}

