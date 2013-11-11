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
// This contains ESock Test cases from section 4
// 
//

// EPOC includes
#include <e32base.h>
#include  <es_sock.h>

// Test system includes
#include "SocketTestSection4.h"


// Test step 4.1
const TDesC& CSocketTest4_1::GetTestName()
	{
	_LIT(ret,"Test4.1");
	return ret;
	}

enum TVerdict CSocketTest4_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Basic Net Database"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
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
	
	// open net database
	Logger().WriteFormat(_L("Open net database on dummy protocol 2"));
	RNetDatabase netDb;
	ret = netDb.Open(ss, protoInfo.iAddrFamily,protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotSupported == ret);
	
	// find a different protocol
    Logger().WriteFormat(_L("Finding dummy protocol 1"));
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// open net database
	Logger().WriteFormat(_L("Open net database on dummy protocol 1"));
	RNetDatabase db;
	ret = db.Open(ss, protoInfo.iAddrFamily,protoInfo.iProtocol);
	CleanupClosePushL(db);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// query
	Logger().WriteFormat(_L("Query database"));
	TBuf8<100> response;
	TBuf16<100> buf16;
	ret = db.Query(_L8("Query"), response);
	buf16.Copy(response);
	Logger().WriteFormat(_L("Query returned %S with response '%S'"), 
		&EpocErrorToText(ret), &buf16);
	TESTL(KErrNone == ret);
	TESTL(_L8("Query") == response);
	
	// add
	Logger().WriteFormat(_L("Add Fish to database"));
	ret = db.Add(_L8("Fish"));
	Logger().WriteFormat(_L("Add returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotSupported == ret);
	
	// Cancel Add 
	TRequestStatus stat;
	Logger().WriteFormat(_L("Add Rabbit to database"));
	db.Add(_L8("Rabbit"), stat);
	Logger().WriteFormat(_L("Cancelling Add"));
	db.Cancel();
	User::WaitForRequest(stat);
    Logger().WriteFormat(_L("Request status is %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrCancel == stat.Int());
	
	// remove
	Logger().WriteFormat(_L("Synch Remove Fish from database"));
	ret = db.Remove(_L8("Fish"));
	Logger().WriteFormat(_L("Remove returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);
	
	Logger().WriteFormat(_L("Asynch Remove Rabbit from database"));
	db.Remove(_L8("Rabbit"), stat);
	User::WaitForRequest(stat);
    Logger().WriteFormat(_L("Request status is %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNone == stat.Int());
	
	// close database
	Logger().WriteFormat(_L("Close net database"));
	
	CleanupStack::Pop(&db);
	db.Close();
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

