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
// This contains ESock Test cases from section 2
// 
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>

// Test system includes
#include "SocketTestSection2.h"


// Test step 2.1
const TDesC& CSocketTest2_1::GetTestName()
	{
	_LIT(ret,"Test2.1");
	return ret;
	}

enum TVerdict CSocketTest2_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Basic Host Resolver"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    TInt ret = OptimalConnect(ss);
	//	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// find protocol won't work unless protocol is loaded
	// so force it with a socket open
	Logger().WriteFormat(_L("Open a socket on Dummy Protocol 1"));
	RSocket sock;
	ret = sock.Open(ss,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	sock.Close();
	
	// find a handle to dummy protocol 2
    Logger().WriteFormat(_L("Finding dummy protocol 2"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 2"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// open host resolver
	Logger().WriteFormat(_L("Opening host resolver on dummy protocol 2"));
	RHostResolver hr;
	ret = hr.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotSupported == ret);
	
	// find a different protocol
    Logger().WriteFormat(_L("Finding dummy protocol 1"));
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// and open host resolver on this one
	Logger().WriteFormat(_L("Open host resolver on dummy protocol 1"));
	ret = hr.Open(ss, protoInfo.iAddrFamily, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// now exercise GetByName
	Logger().WriteFormat(_L("Trying GetByName for DummyName"));
	TNameEntry nameEntry;
	ret = hr.GetByName(_L("DummyName"), nameEntry);
	Logger().WriteFormat(_L("GetByName returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	TESTL(nameEntry().iName == _L("Name One"));
	
	Logger().WriteFormat(_L("Getting next name entry"));
	ret = hr.Next(nameEntry);
	Logger().WriteFormat(_L("Next returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	TESTL(nameEntry().iName == _L("Name Two"));
	
	Logger().WriteFormat(_L("Getting next name entry"));
	ret = hr.Next(nameEntry);
	Logger().WriteFormat(_L("Next returned %S"), &EpocErrorToText(ret));
	TESTL(KErrEof == ret);
	
	// and GetByAddress
	Logger().WriteFormat(_L("Trying GetByAddress"));
	TSockAddr a;
	ret = hr.GetByAddress(a, nameEntry);
	Logger().WriteFormat(_L("GetByAddress returned %S, port %d"), 
		&EpocErrorToText(ret), nameEntry().iAddr.Port());
	TESTL(KErrNone == ret);
	TESTL(10 == nameEntry().iAddr.Port());
	
	Logger().WriteFormat(_L("Getting next name entry"));
	ret = hr.Next(nameEntry);
	Logger().WriteFormat(_L("Next returned %S, port %d"), 
		&EpocErrorToText(ret), nameEntry().iAddr.Port());
	TESTL(KErrNone == ret);
	TESTL(11 == nameEntry().iAddr.Port());
	
	Logger().WriteFormat(_L("Getting next name entry"));
	ret = hr.Next(nameEntry);
	Logger().WriteFormat(_L("Next returned %S"), &EpocErrorToText(ret));
	TESTL(KErrEof == ret);
	
	// Test cancelling host resolver request
	TRequestStatus stat;
    Logger().WriteFormat(_L("Trying GetByName"));
	hr.GetByName(_L("DontComplete"), nameEntry, stat);
    Logger().WriteFormat(_L("Cancelling request"));
	hr.Cancel();
	User::WaitForRequest(stat);
    Logger().WriteFormat(_L("Request status is %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrCancel == stat.Int());
	
	// Test SetHostName
    Logger().WriteFormat(_L("Trying SetHostName for Tara"));
	ret = hr.SetHostName(_L("Tara"));
    Logger().WriteFormat(_L("SetHostName returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
    Logger().WriteFormat(_L("Trying SetHostName for Nemesis"));
	ret = hr.SetHostName(_L("Nemesis"));
    Logger().WriteFormat(_L("SetHostName returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotSupported == ret);
	
	// Test SetHostName/GetHostName
    Logger().WriteFormat(_L("Trying GetHostName"));
	TBuf<100> name;
	ret = hr.GetHostName(name);
    Logger().WriteFormat(_L("GetHostName returned %S, name was '%S'"), 
		&EpocErrorToText(ret), &name);
	TESTL(KErrNone == ret);
	TESTL(_L("PDummyHostName") == name);
	
	Logger().WriteFormat(_L("Close host resolver"));
	hr.Close();
	
	CleanupStack::Pop(&ss);
	ss.Close();
	return verdict;
	}
	

