// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>
#include "TE_SocketServerTestSections.h"
#include<c32root.h>
#include <es_panic.h>
#include <elements/sd_std.h>
#include <comms-infras/sockmes.h>

// Test step 1
const TDesC& CSocketServerTest1::GetTestName()
	{
	_LIT(cRet,"Test1");
	return cRet;
	}

CSocketServerTest1::CSocketServerTest1()
	{
	}

CSocketServerTest1::~CSocketServerTest1()
	{
	}

enum TVerdict CSocketServerTest1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;

	Logger().WriteFormat(_L("TE_SocketServer Test1"));
	Logger().WriteFormat(_L("Test Purpose: Attempts to connect directly to optimal SocketServer without first connecting to SocketServer"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting direct optimal connection to socket server - should fail and panic"));
    RSocketServ ss1;
	ret = IllegalOptimalConnect(ss1);
	
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrPermissionDenied);
	SetTestStepResult(verdict);
	return verdict;
	}

const TDesC& CSocketServerTest2::GetTestName()
	{
	_LIT(cRet,"Test2");
	return cRet;
	}

// Test step 2
CSocketServerTest2::CSocketServerTest2()
    {
    }

CSocketServerTest2::~CSocketServerTest2()
    {
    }

enum TVerdict CSocketServerTest2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;
	
	Logger().WriteFormat(_L("TE_SocketServer Test2"));
	Logger().WriteFormat(_L("Test Purpose: Request optimal SocketServer but connects directly to it twice"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting optimal connection to socket server"));
    RSocketServ ss1;
	ret = OptimalConnect(ss1);
	Logger().WriteFormat(_L("OptimalConnect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	Logger().WriteFormat(_L("Attempting an illegal direct connection to socket server player"));
	ret = IllegalOptimalConnect(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrPermissionDenied);
	SetTestStepResult(verdict);
	return verdict;
	}


// Test step 3
const TDesC& CSocketServerTest3::GetTestName()
	{
	_LIT(cRet,"Test3");
	return cRet;
	}

CSocketServerTest3::CSocketServerTest3()
	{
	}

CSocketServerTest3::~CSocketServerTest3()
	{
	}

enum TVerdict CSocketServerTest3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("TE_SocketServer Test3"));
	Logger().WriteFormat(_L("Test Purpose: Separate player belonging to optimal SocketServer dies"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting connection to socket server"));
    RSocketServ ss1;
	TInt ret;
	ret = ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	RSocket sock1;
	Logger().WriteFormat(_L("Attempting open a socket to protocol 'Dummy Protocol 1'"));
	ret = sock1.Open(ss1,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to Panic the protocol module"));
	ret = PanicProtocolModule(sock1);
	Logger().WriteFormat(_L("Ioctl returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrServerTerminated || ret == KErrAbort);
	
	sock1.Close();
	
	RSocketServ ss2;
	Logger().WriteFormat(_L("Attempting do normal socket server connection"));
	ret = ss2.Connect();
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	RSocketServ ss3;
	Logger().WriteFormat(_L("Attempting do normal optimal socket server connection"));
	ret = OptimalConnect(ss3);
	CleanupClosePushL(ss3);
	Logger().WriteFormat(_L("Optimal Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	RSocket sock2;
	Logger().WriteFormat(_L("Attempting to connect to 'Dummy Protocol 1'"));
	ret = sock2.Open(ss3,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret != KErrNone);
	
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 5
const TDesC& CSocketServerTest5::GetTestName()
	{
	_LIT(cRet,"Test5");
	return cRet;
	}

CSocketServerTest5::CSocketServerTest5()
	{
	}

CSocketServerTest5::~CSocketServerTest5()
	{
	}

enum TVerdict CSocketServerTest5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("TE_SocketServer Test5"));
	Logger().WriteFormat(_L("Test Purpose: Separate player belonging to optimal SocketServer dies, without completing all messages"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting connection to socket server"));
    RSocketServ ss1;
	TInt ret;
	ret = ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting optimal connection to socket server"));
    RSocketServ ss2;
	ret = OptimalConnect(ss2);
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Optimal Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	RSocket sock1;
	Logger().WriteFormat(_L("Attempting open a socket to protocol 'Dummy Protocol 1'"));
	ret = sock1.Open(ss1,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to Hang the protocol module"));
	TRequestStatus stat1;
	NonCompletingIoclt(sock1,stat1);
	
	RSocket sock2;
	Logger().WriteFormat(_L("Attempting to open a socket to protocol 'Dummy Protocol 1'"));
	ret = sock2.Open(ss2,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	RProperty deathCountProp;
	TRequestStatus deathCountStat;
	
	ret = deathCountProp.Attach(KUidSystemCategory, KUidC32RootModuleDeathKey.iUid);
	deathCountProp.Subscribe(deathCountStat);
	
	Logger().WriteFormat(_L("Attempting to Panic the protocol module"));
	ret = PanicProtocolModule(sock2);
	Logger().WriteFormat(_L("Ioctl returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrServerTerminated || ret == KErrAbort);
	
	Logger().WriteFormat(_L("First Ioctl returned %S"),&EpocErrorToText(ret));
	
	User::WaitForRequest(stat1);
	TESTL(stat1 == KErrServerTerminated || stat1 == KErrAbort);
	
	RSocketServ ss3;
	Logger().WriteFormat(_L("Attempting connection to socket server"));
	ret = ss3.Connect();
	CleanupClosePushL(ss3);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	
	User::WaitForRequest(deathCountStat); //Ensure that all the cleanup has happenned	
	
	Logger().WriteFormat(_L("Checking for  module 'ESock_PDummy1'"));
	CheckForModuleL(_L8("ESock_PDummy1"));
	Logger().WriteFormat(_L("Checking for  module 'ESock_Codealer1'"));
	CheckForModuleL(_L8("ESock_Codealer1"));
	
	
	CleanupStack::PopAndDestroy(&ss3);
	CleanupStack::PopAndDestroy(&ss2);
	CleanupStack::PopAndDestroy(&ss1);
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 7
const TDesC& CSocketServerTest7::GetTestName()
	{
	_LIT(cRet,"Test7");
	return cRet;
	}

CSocketServerTest7::CSocketServerTest7()
	{
	}

CSocketServerTest7::~CSocketServerTest7()
	{
	}

enum TVerdict CSocketServerTest7::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("TE_SocketServer Test7"));
	Logger().WriteFormat(_L("Test Purpose: Optimal SocketServer with separate player dies without the player having completed all messages"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting optimal connection to socket server"));
    RSocketServ ss1;
	TInt ret;
	ret = OptimalConnect(ss1);
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attemting to open connection to 'Dummy Protocol 1'"));
	RSocket sock1;
	ret = sock1.Open(ss1,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attemting to panic the dealer module"));
	ret = PanicDealer(ss1);
	sock1.Close();
	if (ret == KErrNotSupported)
		{
		Logger().WriteFormat(_L("Test Only works for debug versions of epoc"));
		verdict = EInconclusive;
		}
	else
		{
		Logger().WriteFormat(_L("Returned %S"),&EpocErrorToText(ret));
		TESTL(ret == KErrServerTerminated || ret == KErrAbort);
		
		Logger().WriteFormat(_L("Attempting connection to socket server"));
		RSocketServ ss2;
		ret = ss2.Connect();
		CleanupClosePushL(ss2);
		Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
		TESTL(ret == KErrNone);
		
		Logger().WriteFormat(_L("Checking for  module 'ESock_Codealer2'"));
		CheckForModuleL(_L8("ESock_Codealer2"));
		Logger().WriteFormat(_L("Checking for  module 'ESock_Dealer2'"));
		CheckForModuleL(_L8("ESock_Dealer2"));
		
		Logger().WriteFormat(_L("Attemting to open connection to 'Dummy Protocol 1'"));
		
		ret = sock1.Open(ss2,_L("Dummy Protocol 1"));
		Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
		sock1.Close();
		TESTL(ret == KErrNone);
		
		ret = sock1.Open(ss2,_L("Dummy Protocol 2"));
		Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
		sock1.Close();
		TESTL(ret == KErrNotFound);
		CleanupStack::PopAndDestroy(&ss2);
		}
	CleanupStack::PopAndDestroy(&ss1);
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9
const TDesC& CSocketServerTest9::GetTestName()
	{
	_LIT(cRet,"Test9");
	return cRet;
	}
CSocketServerTest9::CSocketServerTest9()
	{
	}

CSocketServerTest9::~CSocketServerTest9()
	{
	}

enum TVerdict CSocketServerTest9::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	int ret;
	
	Logger().WriteFormat(_L("TE_SocketServer Test9"));
	Logger().WriteFormat(_L("Test Purpose: Optimal SocketServer with separate player dies without the player having completed all messages"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	ret =ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to open 'Dummy Protocol 1'"));
	RSocket sock1;
    ret = sock1.Open(ss1,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting optimal connect to socket server"));
    RSocketServ ss2;
	ret = OptimalConnect(ss2);
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to open 'Dummy Protocol 2'"));
	RSocket sock2;
    ret = sock2.Open(ss2,_L("Dummy Protocol 2"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to hang protocol module 2 on socket 2"));
	TRequestStatus stat2;
	NonCompletingIoclt(sock2,stat2);
	
	RProperty deathCountProp;
	TRequestStatus deathCountStat;
	
	ret = deathCountProp.Attach(KUidSystemCategory, KUidC32RootModuleDeathKey.iUid);

	deathCountProp.Subscribe(deathCountStat);
	CleanupClosePushL(deathCountProp);
	Logger().WriteFormat(_L("Attempting to panic dealer on socket 3"));
	ret = PanicDealer(ss2);
	if (ret == KErrNotSupported)
		{
		Logger().WriteFormat(_L("Test Only works for debug versions of epoc"));
		deathCountProp.Cancel();
		User::WaitForRequest(deathCountStat); //Ensure that all the cleanup has happenned	
		verdict = EInconclusive;
		}
	else
		{
		Logger().WriteFormat(_L("Ioctl returned %S on socket 3"),&EpocErrorToText(ret));
		TESTL(ret == KErrServerTerminated || ret == KErrAbort);
		User::WaitForRequest(deathCountStat); //Ensure that all the cleanup has happenned	
		User::WaitForRequest(stat2);
		ret = stat2.Int();
		Logger().WriteFormat(_L("Ioctl returned %S on socket 2"),&EpocErrorToText(ret));
		TESTL(ret == KErrServerTerminated || ret == KErrAbort);
			
		Logger().WriteFormat(_L("Checking for  module 'ESock_Codealer2' and 'ESock_Dealer2'"));
		CheckForModuleL(_L8("ESock_Codealer2"));
		CheckForModuleL(_L8("ESock_Dealer2"));
			
		Logger().WriteFormat(_L("Attempting to open 'Dummy Protocol 1' on optimal socket server"));
		RSocket sock2_2;
	    ret = sock2_2.Open(ss2,_L("Dummy Protocol 1"));
		Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
		TESTL(ret != KErrNone);
			
		Logger().WriteFormat(_L("Attempting to open 'Dummy Protocol 1' on main socket server"));
		RSocket sock1_2;
	    ret = sock1_2.Open(ss1,_L("Dummy Protocol 1"));
		Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
		TESTL(ret == KErrNone);
		}
		
	CleanupStack::PopAndDestroy(&deathCountProp);
	CleanupStack::PopAndDestroy(&ss2);
	CleanupStack::PopAndDestroy(&ss1);
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 16
const TDesC& CSocketServerTest16::GetTestName()
	{
	_LIT(cRet,"Test16");
	return cRet;
	}
CSocketServerTest16::CSocketServerTest16()
	{
	}

CSocketServerTest16::~CSocketServerTest16()
	{
	}

enum TVerdict CSocketServerTest16::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;
	
	Logger().WriteFormat(_L("TE_SocketServer Test16"));
	Logger().WriteFormat(_L("Test Purpose: A large number of IOCTLs cause the separate player channel to be full as player is temporarily unresponsive"));
	
	Logger().WriteFormat(_L("Attempting to connect session 1"));
    RSocketServ ss1;
	ret = ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Making Multiple opens"));
	RSocket sock[KLargeNumSockets];
	OpenMultipleSocketsL(ss1,sock,sizeof(sock)/sizeof(RSocket));
	
	int i;
	Logger().WriteFormat(_L("Making Slow Ioctl calls"));
	TRequestStatus stat[KLargeNumSockets];
	for (i = 0;i < KLargeNumSockets; i++)
		{
		SlowIoctlReturn(sock[i],stat[i],FALSE);
		Logger().WriteFormat(_L("Ioclt call %d returned %S"),i,&EpocErrorToText(ret));
		TESTL(ret == KErrNone || ret == KErrServerBusy );
		}
	
	//Now close	
	for (i = 0;i < KLargeNumSockets; i++)
		{
		sock[i].Close();
		User::WaitForRequest(stat[i]);
		ret = stat[i].Int();
		Logger().WriteFormat(_L("Ioclt TRequestStatus %d returned %S"),i,&EpocErrorToText(ret));
		TESTL(ret == KErrNone || ret == KErrServerBusy );
		}
	
    CleanupStack::PopAndDestroy(&ss1);
    
	SetTestStepResult(verdict);
	return verdict;
	}


// Test step Test
const TDesC& CSocketServerTest17::GetTestName()
	{
	_LIT(cRet,"Test17");
	return cRet;
	}
CSocketServerTest17::CSocketServerTest17()
	{
	}

CSocketServerTest17::~CSocketServerTest17()
	{
	}

enum TVerdict CSocketServerTest17::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	int ret;
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	ret =ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to open 'Dummy Protocol 1'"));
	RSocket sock1;
    ret = sock1.Open(ss1,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"),&EpocErrorToText(ret));
	TESTL(ret == KErrNone);
	
	Logger().WriteFormat(_L("Attempting to hang protocol module 2 on socket 2"));
	TRequestStatus stat1;
	NonCompletingIoclt(sock1,stat1);
	
	CleanupStack::PopAndDestroy(&ss1);


//Ensure that the Ioclt returns;	
	RTimer timer;
	TRequestStatus stat2;
	timer.CreateLocal();
	
	timer.After(stat2,2000000);
	
	User::WaitForRequest(stat1,stat2);
	
	if (stat1.Int() == KRequestPending)
		{
		verdict = EFail;
		}
	if (stat2.Int() == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(stat2);
		}
	
	timer.Close();
	
	//must perform this here because otherwise we get stray events - bad
	
	SetTestStepResult(verdict);
	return verdict;
	}

const TDesC& CSocketServerTest18::GetTestName()
	{
	_LIT(cRet,"Test18");
	return cRet;
	}
CSocketServerTest18::CSocketServerTest18()
	{
	}

CSocketServerTest18::~CSocketServerTest18()
	{
	}

enum TVerdict CSocketServerTest18::InternalDoTestStepL( void )
	{
	SetTestStepResult(EFail);
	TInt ret;
	ret = OpenSocketProtocolStarted();
	if( ret == KErrNone )
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	};

// Test step Test
const TDesC& CSocketServerTest20::GetTestName()
	{
	_LIT(cRet,"Test20");
	return cRet;
	}
CSocketServerTest20::CSocketServerTest20()
	{
	}

CSocketServerTest20::~CSocketServerTest20()
	{
	}

enum TVerdict CSocketServerTest20::InternalDoTestStepL( void )
// Checking DbG functions. Make sure total space is greater than Free Space
	{
	
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	
	TInt totalSpace = ss.__DbgMbufTotalSpace();
	Logger().WriteFormat(_L("Socket Server __DbgMbufTotalSpace returned %S"),&EpocErrorToText(totalSpace));
	TInt freeSpace = ss.__DbgMbufFreeSpace();
	Logger().WriteFormat(_L("Socket Server __DbgMbufFreeSpace returned %S"),&EpocErrorToText(freeSpace));
#ifdef _DEBUG_SOCKET_FUNCTIONS	
	Logger().WriteFormat(_L("_DEBUG_SOCKET_FUNCTIONS has been defined"));
	if(freeSpace <= totalSpace)
		{
		verdict = EPass;
		}
#else
	Logger().WriteFormat(_L("_DEBUG_SOCKET_FUNCTIONS has not been defined"));
	if(totalSpace == KErrNone && freeSpace == KErrNone)
		{
		verdict = EPass;
		}	
#endif
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step Test
const TDesC& CSocketServerTest21::GetTestName()
	{
	_LIT(cRet,"Test21");
	return cRet;
	}
	
CSocketServerTest21::CSocketServerTest21()
	{
	}

CSocketServerTest21::~CSocketServerTest21()
	{
	}

enum TVerdict CSocketServerTest21::InternalDoTestStepL( void )
	//Verify that dbg functions without DEBUG_SOCKET_FUNCTIONS don't cause panic
	{
#ifndef _DEBUG_SOCKET_FUNCTIONS
	Logger().WriteFormat(_L("_DEBUG_SOCKET_FUNCTIONS has not been defined"));
	Logger().WriteFormat(_L("Verifying _Dbg functions do not crash OS"));
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	// fail next mbuf allocation
	err = ss.__DbgFailNext(1);
	Logger().WriteFormat(_L("Socket Server __DbgFailNext returned %S"),&EpocErrorToText(err));
	// Set a heap mark in the socket server of count 0
	TInt err2 = ss.__DbgCheckHeap(0);
	Logger().WriteFormat(_L("Socket Server __DbgCheckHeap returned %S"),&EpocErrorToText(err2));
	if(err == KErrNone && err2 == KErrNone)
		{
		verdict = EPass;
		}
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
#else
	return EPass;
#endif
	}


// Test step Test
const TDesC& CSocketServerTest22::GetTestName()
	{
	_LIT(cRet,"Test22");
	return cRet;
	}
CSocketServerTest22::CSocketServerTest22()
	{
	}

CSocketServerTest22::~CSocketServerTest22()
	{
	}

enum TVerdict CSocketServerTest22::InternalDoTestStepL( void )
/* Verify that dbg functions without DEBUG_SOCKET_FUNCTIONS don't cause panic
 */
	{
#ifndef _DEBUG_SOCKET_FUNCTIONS
	Logger().WriteFormat(_L("_DEBUG_SOCKET_FUNCTIONS has not been defined"));
	Logger().WriteFormat(_L("Verifying _Dbg functions do not crash OS"));
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	// Set a heap mark in the socket server 
	err = ss.__DbgMarkHeap();
	Logger().WriteFormat(_L("Socket Server __DbgMarkHeap returned %S"),&EpocErrorToText(err));
	// End the heap mark in the socket server 
	TInt err2 = ss.__DbgMarkEnd(0);
	Logger().WriteFormat(_L("Socket Server __DbgMarkEnd returned %S"),&EpocErrorToText(err2));
	if(err == KErrNone && err2 == KErrNone)
		{
		verdict = EPass;
		}
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
#else
	return EPass;
#endif
	}


// Test step Test
const TDesC& CSocketServerTest23::GetTestName()
	{
	_LIT(cRet,"Test23");
	return cRet;
	}
CSocketServerTest23::CSocketServerTest23()
	{
	}

CSocketServerTest23::~CSocketServerTest23()
	{
	}

enum TVerdict CSocketServerTest23::InternalDoTestStepL( void )
/* Simple test to verify __HeapChecking Functionality
 */ 
	{
#ifndef _DEBUG_SOCKET_FUNCTIONS
	Logger().WriteFormat(_L("_DEBUG_SOCKET_FUNCTIONS has not been defined"));
	Logger().WriteFormat(_L("Verifying _Dbg functions do not crash OS"));
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	// Get the total space and free space in socket server, both should return KErrNone
	err = ss.__DbgMbufTotalSpace();
	Logger().WriteFormat(_L("Socket Server __DbgMbufTotalSpace returned %S"),&EpocErrorToText(err));
	TInt err2 = ss.__DbgMbufFreeSpace();
	Logger().WriteFormat(_L("Socket Server __DbgMbufTotalSpace returned %S"),&EpocErrorToText(err2));
	if(err == KErrNone && err2 == KErrNone)
		{
		verdict = EPass;
		}
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
#else
	return EPass;
#endif	
	}


// Test step Test
const TDesC& CSocketServerTest24::GetTestName()
	{
	_LIT(cRet,"Test24");
	return cRet;
	}
CSocketServerTest24::CSocketServerTest24()
	{
	}

CSocketServerTest24::~CSocketServerTest24()
	{
	}

enum TVerdict CSocketServerTest24::InternalDoTestStepL( void )
 // Verify that dbg functions without DEBUG_SOCKET_FUNCTIONS don't cause panic
 
	{
#ifndef _DEBUG_SOCKET_FUNCTIONS
	Logger().WriteFormat(_L("_DEBUG_SOCKET_FUNCTIONS has not been defined"));
	Logger().WriteFormat(_L("Verifying _Dbg functions do not crash OS"));
	// Arbitarty pool limit
	const TInt KPoolLimit = 10;
	// const number 0
	const TInt KZero = 0;
	// next MBuf to fail 
	const TInt KFailNextOne = 1;
	TVerdict verdict = EPass;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	err = ss.__DbgSetMbufPoolLimit(KPoolLimit);
	Logger().WriteFormat(_L("Socket Server __DbgSetMbufPoolLimit returned %S"),&EpocErrorToText(err));
	// Fail next MBuf
	TInt err2 = ss.__DbgFailNextMbuf(KFailNextOne);
	Logger().WriteFormat(_L("Socket Server __DbgFailNextMbuf returned %S"),&EpocErrorToText(err2));
	if(err == KErrNone && err2 == KErrNone)
		{
		verdict = EPass;
		}
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
#else
	return EPass;
#endif	
	}


// Test step Test
const TDesC& CSocketServerTest25::GetTestName()
	{
	_LIT(cRet,"Test25");
	return cRet;
	}
CSocketServerTest25::CSocketServerTest25()
	{
	}

CSocketServerTest25::~CSocketServerTest25()
	{
	}

enum TVerdict CSocketServerTest25::InternalDoTestStepL( void )
/* Simple test to verify __HeapChecking Functionality
 */
	{
#ifdef _DEBUG_SOCKET_FUNCTIONS	
	// arbitary pool limit
	const TInt KPoolLimit = 10;
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	err = ss.__DbgSetMbufPoolLimit(KPoolLimit);
	Logger().WriteFormat(_L("Socket Server __DbgSetMbufPoolLimit returned %S"),&EpocErrorToText(err));
	if(err == KErrNone)
		{
		verdict = EPass;
		}
	SetTestStepResult(verdict);
	ss.Close();
	return verdict;
#else
	return EPass;
#endif
	}


// Test step Test
const TDesC& CSocketServerTest26::GetTestName()
	{
	_LIT(cRet,"Test26");
	return cRet;
	}
	
CSocketServerTest26::CSocketServerTest26()
	{
	}

CSocketServerTest26::~CSocketServerTest26()
	{
	}

enum TVerdict CSocketServerTest26::InternalDoTestStepL( void )
/* Simple test to verify __DbgCheckMbuf
 */
	{
	// arbitarty pool limit
	const TInt KPoolLimit = 10;
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
#ifdef _DEBUG_SOCKET_FUNCTIONS
	RSocket sock;
	TInt ret = sock.Open(ss, _L("Dummy Protocol 3"));
	Logger().WriteFormat(_L("Socket Open returned %S"),&EpocErrorToText(err));
	if(ret != KErrNone )
		{
		return EFail;
		}
	// Set the Mbuf pool limit to 0
	err = ss.__DbgCheckMbuf(KPoolLimit);
	Logger().WriteFormat(_L("__DbgCheckMbuf returned %S"),&EpocErrorToText(err));
	if(err == KErrNone)
		{
		verdict = EPass;
		}
	sock.Close();
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
#else
	verdict = ss.__DbgCheckMbuf(KPoolLimit) == KErrNone ? EPass : EFail;
	SetTestStepResult(verdict);
	ss.Close();
	return verdict;
#endif
	}



// Test step Test
const TDesC& CSocketServerTest27::GetTestName()
	{
	_LIT(cRet,"Test27");
	return cRet;
	}
CSocketServerTest27::CSocketServerTest27()
	{
	}

CSocketServerTest27::~CSocketServerTest27()
	{
	}

enum TVerdict CSocketServerTest27::InternalDoTestStepL( void )
/* Simple test to verify __DbgFailNextMbuf
 */
	{
	// The next number of MBufs to fail
	const TInt KMbufFailures = 1;
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	CleanupClosePushL(ss);
#ifdef _DEBUG_SOCKET_FUNCTIONS	
	// Open UDP socket
	RSocket socket;
	TInt ret = socket.Open(ss,KAfInet,KSockDatagram,KProtocolInetUdp);
	Logger().WriteFormat(_L("Socket open returned %S"),&EpocErrorToText(ret));
	if (ret != KErrNone)
		{
		return EFail;
		}
	CleanupClosePushL(socket);
	TInetAddr local;
	// arbitary port numbers
	const TUint KLocalPort = 3442;
	const TUint KDestPort = KLocalPort;
	
	// loop back addresses used for UDP send
	_LIT(KLoopbackAddr, "127.0.0.1");
	_LIT(KLocalAddr, "0.0.0.0");
	local.SetPort(KLocalPort);
	local.Input(KLocalAddr);
	socket.Bind(local);
    
    TInetAddr dstAddr;
    dstAddr.SetPort(KDestPort);
    dstAddr.Input(KLoopbackAddr);
    
    // connect socket to loopback addr
    TRequestStatus connectStatus;		
	socket.Connect(dstAddr, connectStatus);
	User::WaitForRequest(connectStatus);
	Logger().WriteFormat(_L("Socket connect returned %d"),connectStatus.Int());
	if(connectStatus.Int() != KErrNone)
		{
		return EFail;
		}
		
	// send some socket data
	_LIT8(data, "some arbitary send data");
	err = ss.__DbgFailNextMbuf(KMbufFailures);
	Logger().WriteFormat(_L("Socket Server __DbgFailNextMbuf returned %S"),&EpocErrorToText(err));
	if(err == KErrNone)
		{
		return EFail;
		}
	
	// wait for send to return, make sure __DbgFailNextMbuf did not effect outcome
	TRequestStatus sendStatus;
	socket.Send(data, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	Logger().WriteFormat(_L("Socket send returned %S"),&EpocErrorToText(sendStatus.Int()));
	if(sendStatus == KErrNone)
		{
		verdict = EPass;
		}
	
	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&ss);
	SetTestStepResult(verdict);
	return verdict;		
#else
	verdict =  ss.__DbgFailNextMbuf(KMbufFailures) == KErrNone ? EPass : EFail;
	CleanupStack::PopAndDestroy(&ss);
	return verdict;
#endif
	}

// Test step Test
const TDesC& CSocketServerTest28::GetTestName()
	{
	_LIT(cRet,"Test28");
	return cRet;
	}
CSocketServerTest28::CSocketServerTest28()
	{
	}

CSocketServerTest28::~CSocketServerTest28()
	{
	}

enum TVerdict CSocketServerTest28::InternalDoTestStepL( void )
/* Simple test to check InstallExtension API
 */
	{
	Logger().WriteFormat(_L("Attempting to install an ESOCK extension that Doesn't exist"));
	TVerdict verdict = EFail;
	TInt err;
	RSocketServ ss;
	err = ss.Connect();
	Logger().WriteFormat(_L("Socket Server Connect returned %S"),&EpocErrorToText(err));
	if (err != KErrNone)
		{
		return EFail;
		}
	
	// bad extension args to pass to SS
	_LIT(KBadExtensionName, "bad extension");
	_LIT(KBadArguments, "bad Arguments");
	TPtrC badPtr(KBadArguments);
	err = ss.InstallExtension(KBadExtensionName, badPtr);
	// Install Extension is an unsupported API
	if(err == KErrNotSupported)
		{
		verdict = EPass;
		}
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 29
const TDesC& CSocketServerTest29::GetTestName()
    {
    _LIT(cRet,"Test29");
    return cRet;
    }

class RSocketServerAttacker : public RSessionBase
/**
* General session class used to connect to socket server by name and send
* malicious message to it.
*/
    {
public:
    TInt Connect()
        {
        return CreateSession(_L("!SocketServer"),TVersion(0,0,0));
        }
 
    void TryToGetPaniced()
        {
        _LIT8(KDefaultText, "Default");
 
        TIpcArgs args;
        args.Set(0,0); // Set invalid NULL descriptor pointer
        args.Set(1,&KDefaultText);
        args.Set(2,&KDefaultText);
        args.Set(3,&KDefaultText);
        SendReceive(ESSNumProtocols, args);
        }
    };

TInt CSocketServerTest29::PanicTestThread(TAny*)
    {
    RSocketServerAttacker t;
    TInt r = t.Connect();
    if (r == KErrNone)
        t.TryToGetPaniced();
    return r;
    }

enum TVerdict CSocketServerTest29::InternalDoTestStepL( void )
    {
    Logger().Write(_L("Socket Server Test 29"));

    TVerdict verdict = EFail;

    RThread t;
    t.Create(_L("Socket server attacker"),PanicTestThread,KDefaultStackSize,0x1000,0x1000,NULL);
    TRequestStatus s;
    t.Logon(s);
    TBool justInTime = User::JustInTime();
    User::SetJustInTime(EFalse);
    t.Resume();
    User::WaitForRequest(s);

    Logger().WriteFormat(_L("Exit type of thread PanicTestThread is %d"), t.ExitType());
    Logger().WriteFormat(t.ExitCategory());
    Logger().WriteFormat(_L("Exit catagory of thread PanicTestThread is %d"), t.ExitReason());

    if (t.ExitType() == EExitPanic &&
        t.ExitCategory() == _L("Worker client") &&
        t.ExitReason() == Den::ECommonBadDescriptorWrite)
        verdict = EPass;

    t.Close();
    User::SetJustInTime(justInTime);

    SetTestStepResult(verdict);
    return verdict;
    }

