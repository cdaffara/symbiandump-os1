// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code  
*/
#include <w32std.h>
#include <e32property.h>
#include <e32power.h>
#include <startupdomaindefs.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstatemanager.h>
#include <hal.h>
#include <ssm/ssmpatchableconstants.h>
#include "lafshutdowneventobserveradaptor.h"
#include "tunitutilityplugin_step.h"
#include <shutdownsrv.h>    //CServShutdownServer

CUnitUtilityPluginTest::CUnitUtilityPluginTest(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

CUnitUtilityPluginTest::~CUnitUtilityPluginTest()
	{
	delete iScheduler;
	}

TVerdict CUnitUtilityPluginTest::doTestStepPreambleL()
	{
	iScheduler = new CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CUnitUtilityPluginTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CUnitUtilityPluginTest::doTestStepL()
	{
	if (IsSsmGracefulShutdown())
		{
		__UHEAP_MARK;
		TRAPD(err, TestGetShutdownStateL(););
		TEST(err == KErrNone);
		__UHEAP_MARKEND;
		
		__UHEAP_MARK;
		TRAP(err, TestClientArrayL(););
		TEST(err == KErrNone);
		__UHEAP_MARKEND;
		
		__UHEAP_MARK;
		TRAP(err, TestIsClientHungL(););
		TEST(err == KErrNone);
		__UHEAP_MARKEND;
		}
	else
		{
		INFO_PRINTF2(_L("SSM is not used for shutdown, IsSsmGracefulShutdown() returned : [%d]"), IsSsmGracefulShutdown());
		}

	return TestStepResult();
	}

/*
@SYMTestCaseID				DS-UTILITYPLUGINS-0011
@SYMPREQ					PREQ2326
@SYMREQ						REQ10976 REQ9110 REQ9120 REQ9155 REQ9158
@SYMTestType				UT
@SYMTestPriority			Critical
@SYMTestStatus 				Implemented
@SYMTestCaseDesc			Tests behaviour of CLafShutdownEventObserverAdaptor
@SYMTestActions				Performs following actions,
							- Create an instance of CLafShutdownEventObserverAdaptor
							- Initialize and start
							- Call On GetShutdownState to get the shutdown state(powerOff and allSessionsHavePendingRequest).
@SYMTestExpectedResults		allSessionsHavePendingRequest and powerOff state should be EFalse
*/
void CUnitUtilityPluginTest::TestGetShutdownStateL()
	{	
	INFO_PRINTF1(_L("*** Starting TestGetShutdownStateL test"));
	CLafShutdownEventObserverAdaptor* observer = reinterpret_cast<CLafShutdownEventObserverAdaptor*>(CLafShutdownEventObserverAdaptor::NewL());
	CleanupStack::PushL(observer);
	TRAPD(err,observer->InitializeL());
	TEST(err == KErrNone || err == KErrAlreadyExists);
	observer->StartL();
	TBool powerOff = ETrue;
	TBool allSessionsHavePendingRequest = ETrue;
	observer->GetShutdownState(powerOff, allSessionsHavePendingRequest);
	TEST(!powerOff);
	TEST(!allSessionsHavePendingRequest);
	observer->Release();
	CleanupStack::PopAndDestroy(observer);
	INFO_PRINTF1(_L("*** TestGetShutdownStateL test completed"));
	}

/*
@SYMTestCaseID				DS-UTILITYPLUGINS-0012
@SYMPREQ					PREQ2326
@SYMREQ						REQ10976 REQ9110 REQ9120 REQ9155 REQ9158
@SYMTestType				UT
@SYMTestPriority			Critical
@SYMTestStatus 				Implemented
@SYMTestCaseDesc			Tests behaviour of CLafShutdownEventObserverAdaptor
@SYMTestActions				Performs following actions,
							- Create an instance of CLafShutdownEventObserverAdaptor
							- Initialize and start
							- Call On ClientArrayLC 
@SYMTestExpectedResults		Test that return a array of registered client thread IDs
*/
void CUnitUtilityPluginTest::TestClientArrayL()
	{	
	INFO_PRINTF1(_L("*** Starting TestClientArrayL test"));
	CLafShutdownEventObserverAdaptor* observer = reinterpret_cast<CLafShutdownEventObserverAdaptor*>(CLafShutdownEventObserverAdaptor::NewL());
	CleanupStack::PushL(observer);
	TRAPD(err,observer->InitializeL());
	TEST(err == KErrNone || err == KErrAlreadyExists);
	observer->StartL();
	CArrayFix<TThreadId>* array = observer->ClientArrayLC();
	INFO_PRINTF2(_L("*** TestClientArrayL : Array count %d"),array->Count());
	TEST(array->Count());
	CleanupStack::PopAndDestroy(array);
	observer->Release();
	CleanupStack::PopAndDestroy(observer);
	INFO_PRINTF1(_L("*** TestClientArrayL test completed"));
	}
	
/*
@SYMTestCaseID				DS-UTILITYPLUGINS-0013
@SYMPREQ					PREQ2326
@SYMREQ						REQ10976 REQ9110 REQ9120 REQ9155 REQ9158
@SYMTestType				UT
@SYMTestPriority			Critical
@SYMTestStatus 				Implemented
@SYMTestCaseDesc			Tests behaviour of CLafShutdownEventObserverAdaptor
@SYMTestActions				Performs following actions,
							- Create an instance of CLafShutdownEventObserverAdaptor
							- Initialize and start
							- Get all registered client thread IDs
							- Call On IsClientHung
@SYMTestExpectedResults		Test that all registered client thread id are not hung.
*/	
void CUnitUtilityPluginTest::TestIsClientHungL()
	{	
	INFO_PRINTF1(_L("*** Starting TestIsClientHungL test"));
	CLafShutdownEventObserverAdaptor* observer = reinterpret_cast<CLafShutdownEventObserverAdaptor*>(CLafShutdownEventObserverAdaptor::NewL());
	CleanupStack::PushL(observer);
	TRAPD(err,observer->InitializeL());
	TEST(err == KErrNone || err == KErrAlreadyExists);
	observer->StartL();
	CArrayFix<TThreadId>* array = observer->ClientArrayLC();
	for(TInt i=0; i<array->Count(); i++)
		{
		TEST(!observer->IsClientHung(array->At(i)));	
		}
	CleanupStack::PopAndDestroy(array);
	observer->Release();
	CleanupStack::PopAndDestroy(observer);
	INFO_PRINTF1(_L("*** TestIsClientHungL test completed"));
	}
