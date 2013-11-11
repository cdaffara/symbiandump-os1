// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

//This test will be able to do what it is planned to do only if the shutdown client and server
//components are built with SYSLIBS_TEST macro defined.
#include <e32test.h>        //RTest
#include <e32svr.h>         //RDebug
#include <savenotf.h>       //RSaveSession, MSaveObserver, CSaveNotifier
#include <e32consf.h>		//for logging to file
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <shutdownsrv.h>    //CServShutdownServer
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsess.h" //CServShutdownServer
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS


static RTest TheTest(_L("t_pwroffevents"));

//
//
//Test macroses and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//
//Test classes & objects

// Stub Power API implementation
static TPowerState iCurrentState = EPwLimit;
static TPowerState iTargetState = EPwLimit;	

// Sets the target state for the current test
void SetTargetState(TPowerState aState)
	{
	iTargetState = aState;
	}
	
TInt Power::EnableWakeupEvents(TPowerState aState)
	{
	iCurrentState = aState;
	return KErrNone;
	}
	
void Power::RequestWakeupEventNotification(TRequestStatus& aStatus)
	{
	TRequestStatus* ptrStatus = &aStatus;
	User::RequestComplete(ptrStatus, KErrNone);
	}
	
TInt Power::PowerDown()
	{
	TEST2(iCurrentState, iTargetState);
	return KErrNone;	
	}
	
// Type definition for pointer to member function.
typedef void (*ClassFuncPtrL) (void);
/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);

	TInt err=KErrNone;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;


		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
		
		TRAP(err, (*testFuncL)());
			
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		
		if (err!=KErrNoMemory)
			TEST2(err, KErrNone);
		
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST2(startProcessHandleCount, endProcessHandleCount);
		TEST2(startThreadHandleCount, endThreadHandleCount);
		
		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TEST2(err, KErrNone);
	TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3687
@SYMTestCaseDesc 		Device Standby (default) Test
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To ensure that the existing default behaviour of going to standby if no power state is passed to HandleShutdownEventL() is maintained.
@SYMTestActions			Call two parameter version of HandleShutdownEventL() with aPowerOff = ETrue. 
@SYMTestExpectedResults	When Power::PowerDown() is called, the target power state should be EPwStandby. 
*/
void StandbyTestL()
    {	
    SetTargetState(EPwStandby);
    
    CServShutdownServer* serv = CServShutdownServer::NewL();
    
    serv->HandleShutdownEventL(MSaveObserver::ESaveAll,ETrue);
    
    delete serv;

    SetTargetState(EPwLimit); // To reset the target state
    }

/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3688
@SYMTestCaseDesc 		Device Standby with Parameter Test
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To ensure that the device goes into standby mode by passing the standby power state to HandleShutdownEventL().
@SYMTestActions			Call three parameter version of HandleShutdownEventL() with aPowerOff = ETrue and aEvent = EPwStandby.
@SYMTestExpectedResults	When Power::PowerDown() is called, the target power state should be EPwStandby.
*/
void StandbyWithParameterTestL()
    {	
    SetTargetState(EPwStandby);
    
    CServShutdownServer* serv = CServShutdownServer::NewL();
    
    serv->HandleShutdownEventL(MSaveObserver::ESaveAll,ETrue,EPwStandby);
    
    delete serv;

    SetTargetState(EPwLimit); // To reset the target state
    }

/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3689
@SYMTestCaseDesc 		Device Power-off Test
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To ensure that the device goes into power-off mode by passing the power off power state to HandleShutdownEventL().
@SYMTestActions			Call three parameter version of HandleShutdownEventL() with aPowerOff = ETrue and aEvent = EPwOff.
@SYMTestExpectedResults	When Power::PowerDown() is called, the target power state should be EPwOff.
*/
void PowerOffTestL()
    {	
    SetTargetState(EPwOff);
    
    CServShutdownServer* serv = CServShutdownServer::NewL();
    
    serv->HandleShutdownEventL(MSaveObserver::ESaveAll,ETrue,EPwOff);
    
    delete serv;

    SetTargetState(EPwLimit); // To reset the target state
    }

/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3690
@SYMTestCaseDesc 		Device Restart Test
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To ensure that the device goes into restart mode by passing the restart power state to HandleShutdownEventL().
@SYMTestActions			Call three parameter version of HandleShutdownEventL() with aPowerOff = ETrue and aEvent = EPwRestart. 
@SYMTestExpectedResults	When Power::PowerDown() is called, the target power state should be EPwRestart.
*/
void RestartTestL()
    {	
    SetTargetState(EPwRestart);
    
    CServShutdownServer* serv = CServShutdownServer::NewL();
    
    serv->HandleShutdownEventL(MSaveObserver::ESaveAll,ETrue,EPwRestart);
    
    delete serv;

    SetTargetState(EPwLimit); // To reset the target state
    }
    
/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3691
@SYMTestCaseDesc 		Invalid Value Test
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To ensure that invalid values do not cause the system to change power state.
@SYMTestActions			Call HandleShutdownEventL() with aPowerOff = ETrue and EPwActive, EPwLimit as power state values. 
@SYMTestExpectedResults	Function should panic with KErrNotSupported.
*/
void InvalidValueTestL()
    {	
    CServShutdownServer* serv = CServShutdownServer::NewL();
    
    TRAPD(err, serv->HandleShutdownEventL(MSaveObserver::ESaveAll,ETrue,EPwActive));
    TEST2(err, KErrNotSupported);

    TRAPD(err2, serv->HandleShutdownEventL(MSaveObserver::ESaveAll,ETrue,EPwLimit));
    TEST2(err2, KErrNotSupported);
    
    delete serv;

    SetTargetState(EPwLimit); // To reset the target state
    }

/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3692
@SYMTestCaseDesc 		Out of memory Test
@SYMPREQ				PREQ1089
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To ensure that any OOM failure in the new code path is handled gracefully.
@SYMTestActions			SYSLIB-PWRLCLI-UT-1000 to SYSLIB-PWRLCLI-UT-1005 will be run under standard RTest OOM Harness.
@SYMTestExpectedResults	The existing tests should succeed when executed under OOM conditions.
*/
void OOMTestL()
    {
	DoOOMTestL(&StandbyTestL,_L("OOM Device Standby (default) Test"));
	DoOOMTestL(&StandbyWithParameterTestL,_L("OOM Device Standby with Parameter Test"));
	DoOOMTestL(&PowerOffTestL,_L("OOM Device Power-off Test"));
	DoOOMTestL(&RestartTestL,_L("OOM Device Restart Test"));
	DoOOMTestL(&InvalidValueTestL,_L("OOM Invalid Value Test"));				
    }

//
//
//Tests

static void DoRunL()
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3687 Device Standby (default) Test "));
    ::StandbyTestL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3688 Device Standby with Parameter Test "));
    ::StandbyWithParameterTestL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3689 Device Power-off Test "));
    ::PowerOffTestL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3690 Device Restart Test "));
    ::RestartTestL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3691 Invalid Value Test "));
    ::InvalidValueTestL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3692 Out of memory Test "));
    ::OOMTestL();
    }

TInt E32Main()
	{
	LogRTestToFile(TheTest);
    TheTest.Title();
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
    TEST(tc != NULL);

	TheTest.Start(_L("Power shutdown tests"));

    TRAPD(err, ::DoRunL());
	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();
	
	delete tc;

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
