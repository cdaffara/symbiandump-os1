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
// This contains ESock Test cases from section 7
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "SocketTestSection7.h"


// Test step 7.1
const TDesC& CSocketTest7_1::GetTestName()
	{
	_LIT(ret,"Test7.1");
	return ret;
	}

enum TVerdict CSocketTest7_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Alloc Heaven during socket open"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// get a protocol
	Logger().WriteFormat(_L("Attempting to FindProtocol dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Stretch socket arrays in the  server.
	RSocket *sockets = new (ELeave) RSocket[KNumStretchOpens];
	CleanupArrayDeletePushL(sockets);
	TInt i;
	Logger().WriteFormat(_L("Attempting to Open %d sockets"), KNumStretchOpens);
	for (i=0; i<KNumStretchOpens; i++)
		{
		ret = sockets[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open returned %S for socket %d"), &EpocErrorToText(ret), i);
			TESTL(EFalse);
			}
		}
	
	Logger().WriteFormat(_L("Closing the first %d sockets"), KNumStretchOpens-1);
	for (i=0; i<KNumStretchOpens-1; i++)
		{
		sockets[i].Close();
		}
	
	RSocket sock;
	TInt failure = 0;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Socket Open Loop"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		//		if (ret != KErrNone)
		//			{
		//			ss.__DbgCheckHeap(0);
		//			}
		failure++;
		}
	Logger().WriteFormat(_L("Created socket OK"));
	
	sockets[KNumStretchOpens-1].Close();
	CleanupStack::PopAndDestroy(sockets);
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


// Test step 7.2
const TDesC& CSocketTest7_2::GetTestName()
	{
	_LIT(ret,"Test7.2");
	return ret;
	}

enum TVerdict CSocketTest7_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Alloc Heaven during host resolver open"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// get a protocol
	Logger().WriteFormat(_L("Attempting to FindProtocol dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Stretch host resolver arrays in the  server.
	RHostResolver *hrs = new (ELeave) RHostResolver[KNumStretchOpens];
	CleanupArrayDeletePushL(hrs);
	
	Logger().WriteFormat(_L("Attempting to Open %d host resolvers"), KNumStretchOpens);
	TInt i;
	for (i=0; i<KNumStretchOpens; i++)
		{
		ret = hrs[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open returned %S for host resolver %d"), &EpocErrorToText(ret), i);
			TESTL(EFalse);
			}
		}
	
	Logger().WriteFormat(_L("Closing the first %d host resolvers"), KNumStretchOpens-1);
	for (i=0; i<KNumStretchOpens-1; i++)
		{
		hrs[i].Close();
		}
	
	RHostResolver hr;
	TInt failure = 0;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Host Resolver Open Loop"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = hr.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		//		if (ret != KErrNone)
		//			{
		//			ss.__DbgCheckHeap(0);
		//			}
		failure++;
		}
	Logger().WriteFormat(_L("Created Host Resolver OK"));
	
	hrs[KNumStretchOpens-1].Close();
	CleanupStack::PopAndDestroy(hrs);
	hr.Close();
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

// Test step 7.3
const TDesC& CSocketTest7_3::GetTestName()
	{
	_LIT(ret,"Test7.3");
	return ret;
	}	

enum TVerdict CSocketTest7_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Alloc Heaven for service resolver open"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// get a protocol
	Logger().WriteFormat(_L("Attempting to FindProtocol dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Stretch service resolver arrays in the server.
	RServiceResolver *srs = new (ELeave) RServiceResolver[KNumStretchOpens];
	CleanupArrayDeletePushL(srs);
	TInt i;
	Logger().WriteFormat(_L("Attempting to Open %d service resolvers"), KNumStretchOpens);
	for (i=0; i<KNumStretchOpens; i++)
		{
		ret = srs[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open returned %S for service resolver %d"), &EpocErrorToText(ret), i);
			TESTL(EFalse);
			}
		}
	
	Logger().WriteFormat(_L("Closing the first %d service resolvers"), KNumStretchOpens-1);
	for (i=0; i<KNumStretchOpens-1; i++)
		{
		srs[i].Close();
		}
	
	RServiceResolver sr;
	TInt failure = 0;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Service Resolver Open Loop"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = sr.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
		//		if (ret != KErrNone)
		//			{
		//			ss.__DbgCheckHeap(0);
		//			}
		failure++;
		}
	Logger().WriteFormat(_L("Created service resolver OK"));
	
	srs[KNumStretchOpens-1].Close();
	CleanupStack::PopAndDestroy(srs);
	sr.Close();
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

// Test step 7.4
const TDesC& CSocketTest7_4::GetTestName()
	{
	_LIT(ret,"Test7.4");
	return ret;
	}

enum TVerdict CSocketTest7_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Alloc Heaven for net database open"));
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// get a protocol
	Logger().WriteFormat(_L("Attempting to FindProtocol dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Stretch net database arrays in the server.
	RNetDatabase *ndbs = new (ELeave) RNetDatabase[KNumStretchOpens];
	CleanupArrayDeletePushL(ndbs);
	
	Logger().WriteFormat(_L("Attempting to Open %d net databases"), KNumStretchOpens);
	TInt i;
	for (i=0; i<KNumStretchOpens; i++)
		{
		ret = ndbs[i].Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open returned %S for net database %d"), &EpocErrorToText(ret), i);
			TESTL(EFalse);
			}
		}
	
	Logger().WriteFormat(_L("Closing the first %d net databases"), KNumStretchOpens-1);
	for (i=0; i<KNumStretchOpens-1; i++)
		{
		ndbs[i].Close();
		}
	
	RNetDatabase ndb;
	TInt failure = 0;
	ret = -1;
	Logger().WriteFormat(_L("Starting OOM Net Database Open Loop"));
	//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	while (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Failing after %d allocs"), failure);
		ss.__DbgFailNext(failure);
		ret = ndb.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
		//		if (ret != KErrNone)
		//			{
		//			ss.__DbgCheckHeap(0);
		//			}
		failure++;
		}
	Logger().WriteFormat(_L("Created net database OK"));
	
	ndbs[KNumStretchOpens-1].Close();
	CleanupStack::PopAndDestroy(ndbs);
	ndb.Close();
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

