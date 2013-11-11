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
#include "RootServerTestSection6.h"
#include <cfshared.h>
#include "RootServerTest.h"


// Test step 6.1 - Bind without loading
CRootServerTest6_1::CRootServerTest6_1(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.1"));
}

TVerdict CRootServerTest6_1::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind without loading"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Attempting bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrRSModuleNotLoaded)
		{
		INFO_PRINTF2(_L("LoadProvider returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
	// Test step 6.2 - Bind loaded to unloaded
CRootServerTest6_2::CRootServerTest6_2(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.2"));
}

TVerdict CRootServerTest6_2::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind loaded to unloaded"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 02*************************//
	INFO_PRINTF1(_L("02 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1);

	//*****************step 3************************
	INFO_PRINTF1(_L("03 Attempting bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule2, KSubModule1);
	if (ret != KErrRSModuleNotLoaded)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}

CRootServerTest6_BindWithinModule::CRootServerTest6_BindWithinModule(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
	{
	}

TVerdict CRootServerTest6_BindWithinModule::BindWithinModuleL(TThreadPriority aPriority)
	{
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1, aPriority);

	//*****************step 2***********************
	INFO_PRINTF1(_L("02 Attempting Bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}

// Test step 6.3 - Bind within a module (normal priority)
CRootServerTest6_3::CRootServerTest6_3(CTE_RootServerServer *aSuite) : CRootServerTest6_BindWithinModule(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.3"));
}

TVerdict CRootServerTest6_3::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind within module (normal priority)"));
	return BindWithinModuleL(EPriorityNormal);
	}
	
// Test step 6.4 - Bind within a module (elevated priority)
CRootServerTest6_4::CRootServerTest6_4(CTE_RootServerServer *aSuite) : CRootServerTest6_BindWithinModule(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.4"));
}

TVerdict CRootServerTest6_4::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind within module (elevated priority)"));
	return BindWithinModuleL(EPriorityRealTime);
	}

// Test step 6.5 - Bind submodule to itself
CRootServerTest6_5::CRootServerTest6_5(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.5"));
}

TVerdict CRootServerTest6_5::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind submodule to itself"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1);

	//*****************step 2***********************
	INFO_PRINTF1(_L("02 Attempting Bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule1);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}

// Test step 6.6 - Bind already bound
CRootServerTest6_6::CRootServerTest6_6(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.6"));
}

TVerdict CRootServerTest6_6::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind again"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1);

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Bind internally"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(ret);
		}

	//*****************step 3************************
	INFO_PRINTF1(_L("03 Bind Again"));
	ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrRSAlreadyBound)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(KErrGeneral);
		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
// Test step 6.7 - Enumerate BindInfo
CRootServerTest6_7::CRootServerTest6_7(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.7"));
}

TVerdict CRootServerTest6_7::doTestStepL( void )
	{
	TCFSubModuleAddress fullName1;
	fullName1.SetModule(KModule1());
	fullName1.SetSubModule(KSubModule1());
	TCFSubModuleAddress fullName2;
	fullName2.SetModule(KModule1());
	fullName2.SetSubModule(KSubModule2());	
	
	INFO_PRINTF1(_L("Test Purpose: Enumerate BindInfo"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());
	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1);

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Bind internally"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(ret);
		}

	TRSBindingInfo bindInfo;
	TRSIter position;
	
	//*****************step 2************************
	INFO_PRINTF1(_L("02 Enumerate bindings"));
	ret = iRootServerSuite->RootSess().EnumerateBindings(fullName1, position, bindInfo);

	INFO_PRINTF2(_L("Enumerate returned %d"), ret);

	if (KErrNone != ret)
		{
		SetTestStepResult(EFail);		}
	else
		{		
		if ( (bindInfo.iParams.iAddress1 == fullName1) && (bindInfo.iParams.iAddress2 == fullName2) )
			{
			INFO_PRINTF1(_L("Binding Found") );
			}
		else if ( (bindInfo.iParams.iAddress1 == fullName2) && (bindInfo.iParams.iAddress2 == fullName1) )
			{
			INFO_PRINTF1(_L("Binding Found") );
			}
		else
			{
			INFO_PRINTF1(_L("Binding not found"));
			SetTestStepResult(EFail);			}
		ret = iRootServerSuite->RootSess().EnumerateBindings(fullName1, position, bindInfo);
		INFO_PRINTF2(_L("Follow-up Enumerate returned %d"), ret);
		if(KErrEof != ret)
			{
			INFO_PRINTF1(_L("(should have returned KErrEof"));
			SetTestStepResult(EFail);			}
		ret = iRootServerSuite->RootSess().EnumerateBindings(fullName1, position, bindInfo);
		INFO_PRINTF2(_L("Further Enumerate returned %d"), ret);
		if(KErrEof != ret)
			{
			INFO_PRINTF1(_L("(should have returned KErrEof"));
			SetTestStepResult(EFail);			}
		}
	
	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);

	return TestStepResult();
	}

CRootServerTest6_UnbindWithinModule::CRootServerTest6_UnbindWithinModule(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
	{
	}

TVerdict CRootServerTest6_UnbindWithinModule::UnbindWithinModuleL(TThreadPriority aPriority)
	{
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1, aPriority);

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Bind internally"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(ret);
		}
	//*****************step 3************************
	INFO_PRINTF1(_L("03 Unbind"));
	TRSUnBindingInfo unbindInfo;
	unbindInfo.iParams.iAddress1.SetModule(KModule1());
	unbindInfo.iParams.iAddress1.SetSubModule(KSubModule1());
	unbindInfo.iParams.iAddress2.SetModule(KModule1());
	unbindInfo.iParams.iAddress2.SetSubModule(KSubModule2());

    asynchHandler->TryUnBind( unbindInfo );
	
	CActiveScheduler::Start();
	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Unbind returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}
	
	INFO_PRINTF1(_L("04 Duplicate Unbind"));
    asynchHandler->TryUnBind( unbindInfo );
	
	CActiveScheduler::Start();
	if (asynchHandler->iStatus != KErrRSBindingUnknown)
		{
		INFO_PRINTF2(_L("Duplicate Unbind returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}
	
	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}

// Test step 6.8 - Unbind within Module (normal priority)
CRootServerTest6_8::CRootServerTest6_8(CTE_RootServerServer *aSuite) : CRootServerTest6_UnbindWithinModule(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.8"));
}

TVerdict CRootServerTest6_8::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnBind within Module (elevated priority)"));
	return UnbindWithinModuleL(EPriorityNormal);
	}
	
// Test step 6.9 - Unbind within Module (normal priority)
CRootServerTest6_9::CRootServerTest6_9(CTE_RootServerServer *aSuite) : CRootServerTest6_UnbindWithinModule(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.9"));
}

TVerdict CRootServerTest6_9::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: UnBind within Module (elevated priority)"));
	return UnbindWithinModuleL(EPriorityRealTime);
	}
	

	
// Test step 6.10 - Cancel Bind
CRootServerTest6_10::CRootServerTest6_10(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.10"));
}

TVerdict CRootServerTest6_10::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Cancel bind"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1);

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Attempting bind"));
	TRSBindingInfo bindInfo;
	bindInfo.iParams.iType = EHierarchical;
	bindInfo.iParams.iAddress1.SetModule(KModule1());
	bindInfo.iParams.iAddress1.SetSubModule(KSubModule1());
	bindInfo.iParams.iAddress2.SetModule(KModule1());
	bindInfo.iParams.iAddress2.SetSubModule(KSubModule2());
	bindInfo.iParams.iState1 = KErrNone;
	bindInfo.iParams.iState2 = KErrNone;
	bindInfo.iParams.iForwardQLength = TRSBindingInfo::EMinQueueLength;	
	bindInfo.iParams.iReverseQLength = TRSBindingInfo::EMinQueueLength;	
	
	TRSSubModuleAddress name1(bindInfo.iParams.iAddress1);
	TRSSubModuleAddress name2(bindInfo.iParams.iAddress2);
		
	asynchHandler->TryBind( bindInfo );
	
	//*****************step 3************************
	INFO_PRINTF1(_L("04 Wait for bind to be completed"));
	User::After(1000000);
	//*****************step 4************************
	INFO_PRINTF1(_L("04 Cancelling Bind"));
	iRootServerSuite->RootSess().CancelBind(name1, name2);
	
	CActiveScheduler::Start();

	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
// Test step 6.11 - Cancel UnBind
CRootServerTest6_11::CRootServerTest6_11(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.11"));
}

TVerdict CRootServerTest6_11::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Cancel Unbind"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Test Module"));
	LoadNormalModuleL(asynchHandler, KModule1);

	//*****************step 2************************
	INFO_PRINTF1(_L("02 Bind internally"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(ret);
		}

	//*****************step 3************************
	INFO_PRINTF1(_L("03 Unbind"));
	TRSUnBindingInfo unbindInfo;
	unbindInfo.iParams.iAddress1.SetModule(KModule1());
	unbindInfo.iParams.iAddress1.SetSubModule(KSubModule1());
	unbindInfo.iParams.iAddress2.SetModule(KModule1());
	unbindInfo.iParams.iAddress2.SetSubModule(KSubModule2());
	
	TRSSubModuleAddress name1(unbindInfo.iParams.iAddress1);
	TRSSubModuleAddress name2(unbindInfo.iParams.iAddress2);

    asynchHandler->TryUnBind( unbindInfo );
    
    //*****************step 4************************
	INFO_PRINTF1(_L("04 Wait for unbind to be completed"));
 	User::After(1000000);
	//*****************step 5************************
   INFO_PRINTF1(_L("05 Cancelling UnBind"));
	iRootServerSuite->RootSess().CancelUnbind(name1, name2);
	
	CActiveScheduler::Start();

	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Unbind returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
// Test step 6.12 - Bind sluggish cpm
CRootServerTest6_12::CRootServerTest6_12(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.12"));
}

TVerdict CRootServerTest6_12::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind sluggish cpm"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());
    
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	iniData.iParams.iDiscoveryDelay = 500;
	iniData.iParams.iBindDelay = 500;
	iniData.iParams.iUnbindDelay = 500;
	iniData.iParams.iShutdownDelay = 500;
	asynchHandler->TryLoad(startParams, iniData);

    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}
    //*****************step 02*************************//
	INFO_PRINTF1(_L("02 Attempting bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
// Test step 6.13 - Bind dormant cpm
CRootServerTest6_13::CRootServerTest6_13(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.13"));
}

TVerdict CRootServerTest6_13::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind dormant cpm"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());
    
       //*****************step 02*************************//
	INFO_PRINTF1(_L("02 Load Dormant Test CPM"));
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	iniData.iParams.iDiscoveryDelay = 500;
	iniData.iParams.iBindDelay = 30000;
	iniData.iParams.iUnbindDelay = 30000;
	iniData.iParams.iShutdownDelay = 1000;
	asynchHandler->TryLoad(startParams, iniData);

    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}
    //*****************step 02*************************//
	INFO_PRINTF1(_L("02 Attempting bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrRSRequestTimedOut)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
// Test step 6.14 - UnBind sluggish test cpm
CRootServerTest6_14::CRootServerTest6_14(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.14"));
}

TVerdict CRootServerTest6_14::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Unbind sluggish test cpm"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	iniData.iParams.iDiscoveryDelay = 500;
	iniData.iParams.iBindDelay = 500;
	iniData.iParams.iUnbindDelay = 500;
	iniData.iParams.iShutdownDelay = 500;
	asynchHandler->TryLoad(startParams, iniData);

    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}
    //*****************step 02*************************//
	INFO_PRINTF1(_L("02 Attempting bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(asynchHandler->iStatus.Int());
		}

	INFO_PRINTF1(_L("03 Unbind"));
	TRSUnBindingInfo unbindInfo;
	unbindInfo.iParams.iAddress1.SetModule(KModule1());
	unbindInfo.iParams.iAddress1.SetSubModule(KSubModule1());
	unbindInfo.iParams.iAddress2.SetModule(KModule1());
	unbindInfo.iParams.iAddress2.SetSubModule(KSubModule2());

    asynchHandler->TryUnBind( unbindInfo );
	
	CActiveScheduler::Start();
	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("Unbind returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
	
	
// Test step 6.15 - UnBind dormant test cpm
CRootServerTest6_15::CRootServerTest6_15(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.15"));
}

TVerdict CRootServerTest6_15::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Unbind dormant test cpm"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KModule1;
	iniData.iParams.iDiscoveryDelay = 500;
	iniData.iParams.iBindDelay = 500;
	iniData.iParams.iUnbindDelay = 30000;
	iniData.iParams.iShutdownDelay = 500;
	asynchHandler->TryLoad(startParams, iniData);

    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}
    //*****************step 02*************************//
	INFO_PRINTF1(_L("02 Attempting bind"));
	TInt ret = TryHierachicalBind(asynchHandler, KModule1, KSubModule1, KModule1, KSubModule2);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(asynchHandler->iStatus.Int());
		}

	INFO_PRINTF1(_L("03 Unbind"));
	TRSUnBindingInfo unbindInfo;
	unbindInfo.iParams.iAddress1.SetModule(KModule1());
	unbindInfo.iParams.iAddress1.SetSubModule(KSubModule1());
	unbindInfo.iParams.iAddress2.SetModule(KModule1());
	unbindInfo.iParams.iAddress2.SetSubModule(KSubModule2());

    asynchHandler->TryUnBind( unbindInfo );
	CActiveScheduler::Start();

	if (asynchHandler->iStatus != KErrRSRequestTimedOut)
		{
		INFO_PRINTF2(_L("Unbind returned error: <%d> "), asynchHandler->iStatus.Int());
		SetTestStepResult(EFail);		}
	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}


// Test step 6.16 - Bind between a server and a module
CRootServerTest6_16::CRootServerTest6_16(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test6.16"));
}

TVerdict CRootServerTest6_16::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Bind between a server and a module"));
										SetTestStepResult(EPass);
	// Create scheduler/active object framework 
	CSelfPopScheduler* scheduler = CSelfPopScheduler::CreateLC();
    CTestAsynchHandler* asynchHandler = CTestAsynchHandler::NewLC(&iRootServerSuite->RootSess());

	_LIT8(KPlainModule, "PlainCPM");
	_LIT8(KServerModule, "ServerCPM");

	//*****************step 01*************************//
	INFO_PRINTF1(_L("01 Load Plain Module"));
	LoadNormalModuleL(asynchHandler, KPlainModule);

	//*****************step 02*************************//
	INFO_PRINTF1(_L("02 Load Server Module"));
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = KServerModule;
	startParams.iParams.iIsServer = ETrue;
	asynchHandler->TryLoad(startParams, iniData);

    CActiveScheduler::Start();
   	if (asynchHandler->iStatus != KErrNone)
		{
		INFO_PRINTF2(_L("LoadServer returned error: <%d> "), asynchHandler->iStatus.Int());
		User::Leave(asynchHandler->iStatus.Int());
		}

    //*****************step 03*************************//
	INFO_PRINTF1(_L("03 Attempting bind of plain above server"));
	TInt ret = TryHierachicalBind(asynchHandler, KPlainModule, KSubModule1, KServerModule, KSubModule1);
	if (ret != KErrRSInvalidBinding)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(ret);
		}

	//*****************step 04***********************
	INFO_PRINTF1(_L("04 Attempting bind of server above plain"));
	ret = TryHierachicalBind(asynchHandler, KServerModule, KSubModule1, KPlainModule, KSubModule1);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Bind returned error: <%d> "), ret);
		User::Leave(ret);
		}

	// uninstall and destroy active scheduler
	CleanupStack::PopAndDestroy(asynchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();

	}
