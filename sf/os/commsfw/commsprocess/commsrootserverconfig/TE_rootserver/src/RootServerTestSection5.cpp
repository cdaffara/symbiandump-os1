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
// This contains RootServer Test cases from section 6
// 
//

// EPOC includes
#include <e32base.h>
#include <es_mbman.h>

// Test system includes
//#include <networking/log.h>
//#include <networking/teststep.h>
//#include <networking/testsuite.h>

#include "TestStepRootServer.h"
#include "TestAsynchHandler.h"
#include "RootServerTestSection5.h"
#include "RootServerTest.h"
// Test step 3.5 - Retrieve Module Information
CRootServerTest_GracefulUnload::CRootServerTest_GracefulUnload(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
	{
	}


TVerdict CRootServerTest_GracefulUnload::UnloadGracefullyL(TThreadPriority aPriority)
	{
										SetTestStepResult(EFail);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

    //*****************step 2************************
	INFO_PRINTF1(_L("02 Load test module"));

	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	startParams.iParams.iPriority = aPriority;

	asynchHandler->TryLoad(startParams, iniData);
    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
								SetTestStepResult(EInconclusive);
		}
	else
		{
		//*****************step 4***********************
		INFO_PRINTF1(_L("04 Unload Test Module"));

		asynchHandler->TryUnLoad(startParams.iParams.iName, EGraceful);
		
		CActiveScheduler::Start();

   		if (asynchHandler->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("Unload returned error: <%d> "), asynchHandler->iStatus.Int());
												SetTestStepResult(EInconclusive);
			}
		else
			{
			SetTestStepResult(EPass);			}
		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}


// Test step 5.1 - UnLoad Gracefully (normal priority)
CRootServerTest5_1::CRootServerTest5_1(CTE_RootServerServer *aSuite) : CRootServerTest_GracefulUnload(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test5.1"));
}

TVerdict CRootServerTest5_1::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnLoad Comms Provider Module gracefully (normal priority)"));
	return UnloadGracefullyL(EPriorityNormal);
	}


// Test step 5.2 - UnLoad Gracefully (elevated priority)
CRootServerTest5_2::CRootServerTest5_2(CTE_RootServerServer *aSuite) : CRootServerTest_GracefulUnload(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test5.2"));
}

TVerdict CRootServerTest5_2::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnLoad Comms Provider Module gracefully (elevated priority)"));
	return UnloadGracefullyL(EPriorityRealTime);
	}


// Test step 5.3 -  Cancel UnLoad Gracefully
CRootServerTest5_3::CRootServerTest5_3(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test5.3"));
}

TVerdict CRootServerTest5_3::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Cancel UnLoad Comms Provider Module gracefully"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Load Test Module"));
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;

	asynchHandler->TryLoad(startParams, iniData);
    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
								SetTestStepResult(EInconclusive);
		}
	else
		{
		//*****************step 3***********************
		INFO_PRINTF1(_L("03 Unload Test Module"));
		asynchHandler->TryUnLoad(startParams.iParams.iName, EGraceful);
		
		//*****************step 4***********************
		INFO_PRINTF2(_L("04 Cancelling UnLoad: %S"), &startParams.iParams.iName);
		iRootServerSuite->RootSess().CancelUnloadCpm(startParams.iParams.iName);
		
		CActiveScheduler::Start();

   		if ((asynchHandler->iStatus != KErrNone) && (asynchHandler->iStatus != KErrCancel))
			{
			INFO_PRINTF2(_L("Unload returned error: <%d> "), asynchHandler->iStatus.Int());
			SetTestStepResult(EFail);			}

		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}

// Test step 3.5 - Retrieve Module Information
CRootServerTest_UngracefulUnload::CRootServerTest_UngracefulUnload(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
	{
	}


TVerdict CRootServerTest_UngracefulUnload::UnloadNonGracefullyL(TThreadPriority aPriority)
	{
										SetTestStepResult(EFail);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 3************************
	INFO_PRINTF1(_L("03 Load Test Module"));
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	startParams.iParams.iPriority = aPriority;

	asynchHandler->TryLoad(startParams, iniData);
    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EInconclusive);
		}
	else
		{
		//*****************step 4***********************
		INFO_PRINTF1(_L("04 Unload Test Module"));
		asynchHandler->TryUnLoad(startParams.iParams.iName, EUnGraceful);
		
		CActiveScheduler::Start();

   		if (asynchHandler->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("Unload returned error: <%d> "), asynchHandler->iStatus.Int());
			SetTestStepResult(EInconclusive);
			}
		else
			{
			SetTestStepResult(EPass);	
			}
		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}
	

// Test step 5.4 - UnLoad Ungracefully - normal priority
CRootServerTest5_4::CRootServerTest5_4(CTE_RootServerServer *aSuite) : CRootServerTest_UngracefulUnload(aSuite)
	{
	// store the name of this test case
	SetTestStepName(_L("Test5.4"));
	}

TVerdict CRootServerTest5_4::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnLoad Comms Provider Module Ungracefully (normal priority)"));
	return UnloadNonGracefullyL(EPriorityNormal);
	}

// Test step 5.5 - UnLoad Ungracefully - elevated priority
CRootServerTest5_5::CRootServerTest5_5(CTE_RootServerServer *aSuite) : CRootServerTest_UngracefulUnload(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test5.5"));
}

TVerdict CRootServerTest5_5::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnLoad Comms Provider Module Ungracefully (elevated priority)"));
	return UnloadNonGracefullyL(EPriorityRealTime);
	}


// Test step 5.6 - Cancel UnLoad Ungracefully
CRootServerTest5_6::CRootServerTest5_6(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test5.6"));
}

TVerdict CRootServerTest5_6::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Cancel UnLoad Comms Provider Module Ungracefully"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Load Test Module"));
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;

	asynchHandler->TryLoad(startParams, iniData);
    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Load returned error: <%d> "), asynchHandler->iStatus.Int());
								SetTestStepResult(EInconclusive);
		}
	else
		{
		//*****************step 3***********************
		INFO_PRINTF1(_L("03 Unload Test Module"));
		asynchHandler->TryUnLoad(startParams.iParams.iName, EUnGraceful);
		
		//*****************step 4***********************
		INFO_PRINTF2(_L("04 Cancelling UnLoad: %S"), &startParams.iParams.iName);
		iRootServerSuite->RootSess().CancelUnloadCpm(startParams.iParams.iName);
		
		CActiveScheduler::Start();
		
	    // Unload the CPM anyway in case we were too late to cancel the load.
	    TRequestStatus status;
	    iRootServerSuite->RootSess().UnloadCpm(status, startParams.iParams.iName, EImmediate);
	    User::WaitForRequest(status);

   		if (asynchHandler->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("Unload returned error: <%d> "), asynchHandler->iStatus.Int());
			SetTestStepResult(EFail);	
			}
		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}
	
	
// Test step 5.7 - UnLoad 2 modules sharing the same heap
CRootServerTest5_7::CRootServerTest5_7(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test5.7"));
}

TVerdict CRootServerTest5_7::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnLoad 2 modules sharing the same heap"));
										SetTestStepResult(EFail);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());
    
	//*****************step 3************************
	INFO_PRINTF1(_L("03 Load Test Module_Share1"));
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	_LIT8(KShareModule1, "Test ModuleShare1");
	_LIT8(KShareModule2, "Test ModuleShare2");
	startParams.iParams.iName = KShareModule1;

	asynchHandler->TryLoad(startParams, iniData);
    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}

	//*****************step 3************************
	INFO_PRINTF1(_L("03 Load Test Module"));
	startParams.iParams.iName = KShareModule2;
	startParams.iParams.iHeapType = EShareHeap;
	startParams.iParams.iShareHeapWith = KShareModule1;

	asynchHandler->TryLoad(startParams, iniData);
	CActiveScheduler::Start();

	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}

    //*****************step 5***********************
	INFO_PRINTF1(_L("05 Unload Test Module_Share1"));
	startParams.iParams.iName = KShareModule1;
	asynchHandler->TryUnLoad(startParams.iParams.iName, EGraceful);
	CActiveScheduler::Start();

	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Unload returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}

	//*****************step 6***********************
	INFO_PRINTF1(_L("06 Unload Test Module_Share2"));
	startParams.iParams.iName = KShareModule2;
	asynchHandler->TryUnLoad(startParams.iParams.iName, EGraceful);
	
	CActiveScheduler::Start();

	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Unload returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EInconclusive);
		}
	else
		{
		SetTestStepResult(EPass);	
		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}
    
    
// Test step 5.8 - Unload Sluggish CPM Gracefully with intervening Close of Session
CRootServerTest5_8::CRootServerTest5_8(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
	{
	// store the name of this test case
	SetTestStepName(_L("Test5.8"));
	}

TVerdict CRootServerTest5_8::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Unload Sluggish CPM Gracefully with intervening Close of Session"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());
    
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	iniData.iParams.iShutdownDelay = isTestServerLifeTime;
	INFO_PRINTF2(_L("01 Loading sluggish module: %S"), &startParams.iParams.iName);
	asynchHandler->TryLoad(startParams, iniData);

	CActiveScheduler::Start();
    if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}

	INFO_PRINTF1(_L("02 Unload sluggish Module"));
	asynchHandler->TryUnLoad(startParams.iParams.iName, EGraceful);

	// Brief wait to give unload chance to start
	User::After(250 * 1000);

	// Close the session
	INFO_PRINTF1(_L("03 Closing RRootServ"));
	iRootServerSuite->RootSess().Close();
	// Brief wait for any fireworks
	User::After(250 * 1000);
	
	// Let the request complete
	CActiveScheduler::Start();

	// Reconnect the session
	INFO_PRINTF1(_L("04 Reconnecting RRootServ"));
	TInt ret = iRootServerSuite->RootSess().Connect();
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Reconnect of RRootServ failed with error: <%d> "), ret);
		User::Leave(ret);
		}
	// Brief wait for any fireworks
	User::After(250 * 1000);

	// now try to retrieve information about the module; should be ERunning after session close
	// causes unload handler to be canceled
	INFO_PRINTF1(_L("05 Getting module information"));
	TRSModuleInfo info;
	TCFModuleName moduleName = startParams.iParams.iName;
	ret = iRootServerSuite->RootSess().GetModuleInfo(moduleName, info);
	INFO_PRINTF4(_L("GetModuleInfo for %S returned %d, state %c= ERunning"), &moduleName, ret, info.iParams.iState == EStopping? '=': '!');
	
	if(KErrNone != ret || info.iParams.iState != ERunning)
		{
		CleanupStack::PopAndDestroy(asynchHandler);
		CleanupStack::PopAndDestroy(scheduler);
        SetTestStepResult(EFail);	
        return TestStepResult();
		}

	// Wait for it to finish stopping, query again
	INFO_PRINTF1(_L("06 Getting module information after waiting for stop"));
	User::After((iniData.iParams.iShutdownDelay + 500) * 1000);
	moduleName = startParams.iParams.iName;
	ret = iRootServerSuite->RootSess().GetModuleInfo(moduleName, info);
	INFO_PRINTF3(_L("GetModuleInfo for %S returned %d"), &moduleName, ret);
	
	if(KErrRSModuleUnknown != ret)
		{
		CleanupStack::PopAndDestroy(asynchHandler);
		CleanupStack::PopAndDestroy(scheduler);
        SetTestStepResult(EFail);	
        return TestStepResult();
		}

	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}

