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
// This contains ESock Test cases from section 5
// 
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <ss_std.h>
#include <test/es_dummy.h>

// Test system includes
#include "SocketTestSection5.h"


// Test step 5.1
const TDesC& CSocketTest5_1::GetTestName()
	{
	_LIT(ret,"Test5.1");
	return ret;
	}

enum TVerdict CSocketTest5_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Memory Leak opening bad socket"));
	
#if defined(_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// open a bad socket
    Logger().WriteFormat(_L("Attempting to create bad socket type with heap checking"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	RSocket sock;
    ret = sock.Open(ss, 0, 0, 0);
	//	ss.__DbgMarkEnd(0);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrBadName == ret);
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}


// Test step 5.2
const TDesC& CSocketTest5_2::GetTestName()
	{
	_LIT(ret,"Test5.2");
	return ret;
	}

enum TVerdict CSocketTest5_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Memory Leak connecting to server and disconnecting from server"));
	
#if defined(_DEBUG_SOCKET_FUNCTIONS)
	// connect to esock
	Logger().WriteFormat(_L("Main thread : Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Main Thread : Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Main thread : Creating semaphore for threads"));
	RSemaphore s;
	s.CreateLocal(0);
	RThread t;
	
	TSocketThreadArg tArg;
	tArg.iHandle = this;
	tArg.iSem  = &s;
	tArg.iNumSockets = 0;
	
	// try connect and close in separate thread
	Logger().WriteFormat(_L("Main thread : Creating sub thread to connect"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	t.Create(_L("socketThread2"), ConnectThread, KDefaultStackSize, KDefaultHeapSize, KDefaultHeapSize, &tArg);
	t.Resume();
	s.Wait();
	t.Close();
	//	ss.__DbgMarkEnd(0);
	Logger().WriteFormat(_L("Main thread : Connect thread completed"));
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 5.3
const TDesC& CSocketTest5_3::GetTestName()
	{
	_LIT(ret,"Test5.3");
	return ret;
	}

enum TVerdict CSocketTest5_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Memory Leak killing thread with open resources"));
	
#if defined(_DEBUG_SOCKET_FUNCTIONS)
	// connect to esock
	Logger().WriteFormat(_L("Main thread : Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Main Thread : Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Main thread : Creating semaphore for threads"));
	RSemaphore s;
	s.CreateLocal(0);
	
	// try create in separate thread and kill it
	Logger().WriteFormat(_L("Main Thread : Creating sub thread to create sockets and resolvers"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TSocketThreadArg a;
	a.iHandle = this;
	a.iSem = &s;
	a.iNumSockets = 1;
	RThread t2;
	t2.Create(_L("socketThread3"), SocketThread, KDefaultStackSize, KDefaultHeapSize, KDefaultHeapSize, &a);
	t2.Resume();
	s.Wait();
	Logger().WriteFormat(_L("Main Thread : Killing Create sub thread"));
	t2.Kill(KErrNone);
	t2.Close();
	
	Logger().WriteFormat(_L("Main Thread : Waiting 100ms for thread to clear up"));
	User::After(100000);
	//	ss.__DbgMarkEnd(0);
	Logger().WriteFormat(_L("Main Thread : Create thread completed"));
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 5.4
const TDesC& CSocketTest5_4::GetTestName()
	{
	_LIT(ret,"Test5.4");
	return ret;
	}

enum TVerdict CSocketTest5_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Memory Leak Getting Protocol Info and Opening Socket"));
	
#if defined(_DEBUG_SOCKET_FUNCTIONS)
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// NumProtocols and GetProtocolInfo
	Logger().WriteFormat(_L("Try NumProtocols"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TUint numProtocols;
	ret = ss.NumProtocols(numProtocols);
	Logger().WriteFormat(_L("NumProtocols returned %S, number %d"), 
		&EpocErrorToText(ret), numProtocols);
	TESTL(KErrNone == ret);
	//	ss.__DbgMarkEnd(0);
	
	Logger().WriteFormat(_L("Trying GetProtocolInfo until Dummy Protocol 1 is found"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TProtocolDesc protoInfo;
	TUint aIndex;
	TBool aFound = false;
	for (aIndex=1;aIndex<=numProtocols;aIndex++) {
		ret = ss.GetProtocolInfo(aIndex, protoInfo);
		TESTL(KErrNone == ret || KErrCannotFindProtocol == ret);
		if (protoInfo.iName ==_L("Dummy Protocol 1")){
			aFound = true;
			break;
			}
		}
	//	ss.__DbgMarkEnd(0);
	if (!aFound) {
		Logger().WriteFormat(_L("Could not find Dummy Protocol 1"));
		return EFail;
		}
	
	Logger().WriteFormat(_L("Trying socket Open to load protocol"));
	RSocket sock;
	ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	sock.Close();
	
	Logger().WriteFormat(_L("Trying socket Open with Heap checking"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	RSocket sock1;
	ret = sock1.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	sock1.Close();
	//	ss.__DbgMarkEnd(0);
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}


// Test step 5.5
const TDesC& CSocketTest5_5::GetTestName()
	{
	_LIT(ret,"Test5.5");
	return ret;
	}

enum TVerdict CSocketTest5_5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Validating leak detection upon shutdown"));
	
#if defined(_DEBUG_SOCKET_FUNCTIONS)
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Open a socket on Dummy Protocol 1"));
	RSocket sock;
	ret = sock.Open(ss,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	sock.SetOpt(KDummyOptionLeakMemory, 4);
	sock.Close();
	
	CleanupStack::PopAndDestroy(&ss);
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}



