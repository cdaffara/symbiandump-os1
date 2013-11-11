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
// This contains ESock Test cases from section 6
// 
//

// EPOC includes
#include <e32base.h>
#include <dns_qry.h>
#include <es_sock.h>

// Test system includes
#include "SocketTestSection6.h"


// Test step 6.1
const TDesC& CSocketTest6_1::GetTestName()
	{
	_LIT(ret,"Test6.1");
	return ret;
	}

enum TVerdict CSocketTest6_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Out of Memory During Connect"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Starting OOM Connect loop"));
	TInt failure = 0;
	RSocketServ ss2;
	ret = -1;
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = OptimalConnect(ss2);
		//		if (ret != KErrNone)
		//			ss.__DbgCheckHeap(0);
		failure++;
		}
	Logger().WriteFormat(_L("Connected OK"));
	
	ss2.Close();
	
	//	ss.__DbgMarkEnd(0);
	ss.__DbgFailNext(-1);
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 6.2
const TDesC& CSocketTest6_2::GetTestName()
	{
	_LIT(ret,"Test6.2");
	return ret;
	}

enum TVerdict CSocketTest6_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Out of Memory Conditions for Socket Open"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying FindProtocol"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	
	// test OOM during socket open
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TInt failure = 0;
	RSocket sock;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Socket Open Loop"));
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		failure++;
		}
	Logger().WriteFormat(_L("Opened socket OK"));
	sock.Close();
	//	ss.__DbgMarkEnd(0);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	CleanupStack::Pop(&ss);
	ss.Close();
	
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 6.3
const TDesC& CSocketTest6_3::GetTestName()
	{
	_LIT(ret,"Test6.3");
	return ret;
	}

enum TVerdict CSocketTest6_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Out of Memory Conditions for Host Resolver"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying FindProtocol"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Host resolvers
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TInt failure = 0;
	RHostResolver hResolv;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Host Resolver Open Loop"));
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = hResolv.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		failure++;
		}
	Logger().WriteFormat(_L("Opened host resolver OK"));
	hResolv.Close();
	//	ss.__DbgMarkEnd(0);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	
	// Test OOM on Query()
    Logger().WriteFormat(_L("Testing OOM handling of RHostResolver::Query()"));
	
	ret = hResolv.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
	TESTL(KErrNone == ret);
	
    TDnsQueryBuf dnsQryBuf;
	TDnsRespABuf dnsRespABuf;
    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(_L8("http://www.sample.net/"));
	
    // create a timer to catch esock hanging
	RTimer timer;
	timer.CreateLocal();
	CleanupClosePushL(timer);
	const TUint oneSec(1000000);
	TRequestStatus timerStatus;
	TRequestStatus status(KErrNoMemory);
	TUint failCount(0);
	
	// slowly increase the fail count until success
	while(KErrNoMemory == status.Int() && failCount < 20)
		{
		Logger().WriteFormat(_L("Checking Query with %d allocs before failure"), failCount);
		ss.__DbgFailNext(failCount++);
		hResolv.Query(dnsQryBuf, dnsRespABuf, status);
		timer.After(timerStatus, oneSec);
		User::WaitForRequest(status, timerStatus);
		timer.Cancel();
		User::WaitForAnyRequest();
		}
	
	// check query eventually finished ok
	ss.__DbgFailNext(-1);
	TESTL(KErrNone == status.Int());
	
	CleanupStack::PopAndDestroy(&timer);
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 6.4
const TDesC& CSocketTest6_4::GetTestName()
	{
	_LIT(ret,"Test6.4");
	return ret;
	}

enum TVerdict CSocketTest6_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Out of Memory Conditions for Service Resolver"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying FindProtocol"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	
	// service resolvers
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TInt failure = 0;
	RServiceResolver sResolv;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Service Resolver Open Loop"));
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = sResolv.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		failure++;
		}
	Logger().WriteFormat(_L("Created service resolver OK"));
	sResolv.Close();
	//	ss.__DbgMarkEnd(0);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 6.5
const TDesC& CSocketTest6_5::GetTestName()
	{
	_LIT(ret,"Test6.5");
	return ret;
	}

enum TVerdict CSocketTest6_5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Out of Memory Conditions for Net Database"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying FindProtocol"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	
	// net database
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TInt failure = 0;
	RNetDatabase netDb;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Net Database Open Loop"));
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = netDb.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		failure++;
		}
	Logger().WriteFormat(_L("Created net database OK"));
	netDb.Close();
	//	ss.__DbgMarkEnd(0);
	
	// Flush any FailNext there might be hanging around.
	ss.__DbgFailNext(-1);
	
	CleanupStack::Pop(&ss);
	ss.Close();
#else
	Logger().WriteFormat(_L("TestDisabled on release build."));
	verdict = EInconclusive;
#endif
	
	SetTestStepResult(verdict);
	return verdict;
	}

