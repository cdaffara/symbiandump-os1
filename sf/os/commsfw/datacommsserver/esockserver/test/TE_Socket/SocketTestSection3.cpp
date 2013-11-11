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
// This contains ESock Test cases from section 3
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "SocketTestSection3.h"


// Test step 3.1
const TDesC& CSocketTest3_1::GetTestName()
	{
	_LIT(ret,"Test3.1");
	return ret;
	}

enum TVerdict CSocketTest3_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Basic Service Resolver"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// find dummy protocol 1
	TProtocolDesc protoInfo;
    Logger().WriteFormat(_L("Finding dummy protocol 1"));
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// open a service resolver on it
	Logger().WriteFormat(_L("Open Service resolver"));
	RServiceResolver sr;
	ret = sr.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	CleanupClosePushL(sr);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// try a GetByName
	Logger().WriteFormat(_L("Trying GetByName for DummyName"));
	TPortNum port;
	ret = sr.GetByName(_L("DummyName"), port);
	Logger().WriteFormat(_L("GetByName returned %S, port %d"), &EpocErrorToText(ret), port());
	TESTL(KErrNone == ret);
	TESTL(64 == port());
	
	Logger().WriteFormat(_L("Trying GetByName for Bogus"));
	ret = sr.GetByName(_L("Bogus"), port);
	Logger().WriteFormat(_L("GetByName returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);
	
	// now try Cancel GetByNumber
	TRequestStatus stat;
	TServiceName service;
	Logger().WriteFormat(_L("Trying GetByNumber for 66"));
	sr.GetByNumber(66, service, stat);
	Logger().WriteFormat(_L("Cancelling GetByNumber"));
	sr.Cancel();
	User::WaitForRequest(stat);
    Logger().WriteFormat(_L("Request status is %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrCancel == stat.Int());
	
	Logger().WriteFormat(_L("Trying GetByNumber for 21"));
	ret = sr.GetByNumber(21, service);
	Logger().WriteFormat(_L("GetByNumber returned %S, service '%S'"), 
		&EpocErrorToText(ret), &service);
	TESTL(KErrNone == ret);
	TESTL(_L("DummyService") == service);
	
	Logger().WriteFormat(_L("Trying GetByNumber for 22"));
	ret = sr.GetByNumber(22, service);
	Logger().WriteFormat(_L("GetByNumber returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);
	
	// register and remove service
	Logger().WriteFormat(_L("Trying RegisterService for Simpson"));
	ret = sr.RegisterService(_L("Simpson"),500);
	Logger().WriteFormat(_L("RegisterService returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying RegisterService for Bart"));
	ret = sr.RegisterService(_L("Bart"), 10);
	Logger().WriteFormat(_L("RegisterService returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);
	
	Logger().WriteFormat(_L("Trying to RemoveService Colt"));
	ret = sr.RemoveService(_L("Colt"),45);
	Logger().WriteFormat(_L("RemoveService returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying to RemoveService Usi"));
	ret = sr.RemoveService(_L("Usi"), 9);
	Logger().WriteFormat(_L("RemoveService returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);
	
	Logger().WriteFormat(_L("Close service resolver"));
	CleanupStack::Pop(&sr);
	sr.Close();
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

