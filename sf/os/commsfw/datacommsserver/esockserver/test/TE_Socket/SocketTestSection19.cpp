// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 19
// 
//



#include <e32base.h>
#include "SocketTestSection19.h"
#include "ES_DUMMY.H"


// Test step 19.1
const TDesC& CSocketTest19_1::GetTestName()
	{
	_LIT(ret,"Test19.1");
	return ret;
	}

enum TVerdict CSocketTest19_1::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Test 19.1 Test connect with connect data"));
	
    Logger().WriteFormat(_L("- connecting to the socket server"));
	RSocketServ ss;
    TInt nRes = OptimalConnect(ss);
    TESTL(nRes == KErrNone);
 	CleanupClosePushL(ss);
	RSocket sock;
	Logger().WriteFormat(_L("Openning a connection to PDummy 3"));
	nRes = sock.Open(ss,KDummyThreeName);
    TESTL(nRes == KErrNone);
    CleanupClosePushL(sock);
	
	TSockAddr addr;
	_LIT8(desOut,"Some test stuff to send to protocol");
	TBuf8<50> desIn;
	TRequestStatus stat;
    Logger().WriteFormat(_L("Performing a connect with connect Data"));
	sock.Connect(addr, desOut, desIn, stat);
	User::WaitForRequest(stat);
	TESTL(stat == KErrNone);
	TESTL(desIn == desOut);
	CleanupStack::PopAndDestroy(&sock); 
    CleanupStack::PopAndDestroy(&ss); 
	SetTestStepResult(EPass);
	return EPass;
	}


// Test step 19.2
const TDesC& CSocketTest19_2::GetTestName()
	{
	_LIT(ret, "Test19.2");
	return ret;
	}

enum TVerdict CSocketTest19_2::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("- connecting to the socket server"));
	RSocketServ ss;
    TInt nRes = OptimalConnect(ss);
    TESTL(nRes == KErrNone);
 	CleanupClosePushL(ss);
	RSocket sock;
	Logger().WriteFormat(_L("Openning a connection to PDummy 3"));
	nRes = sock.Open(ss, KDummyThreeName);
    TESTL(nRes == KErrNone);
    CleanupClosePushL(sock);
    
    RSocket sock2;
    Logger().WriteFormat(_L("Openning a Null socket"));
	nRes = sock2.Open(ss);
    TESTL(nRes == KErrNone);
    CleanupClosePushL(sock2);
	
	TInt err;
	TSockAddr addr;
	Logger().WriteFormat(_L("Performing a bind"));
	err = sock.Bind(addr);
	TESTL(KErrNone == err);
	
	_LIT8(desOut, "Some test stuff to send to protocol");
	TBuf8<50> desIn;
	Logger().WriteFormat(_L("Performing a listen with connect data"));
	err = sock.Listen(5, desOut);
	TESTL(KErrNone == err);
	
	TRequestStatus stat;
    Logger().WriteFormat(_L("Performing an accept with connect Data"));
	sock.Accept(sock2, desIn, stat);
	TESTL(KRequestPending == stat.Int()); // there must be something wrong if it's not pending
	//Now we need to complete the accept
	err = sock.SetOpt(KDummyOptionSetConnectComplete, 3462, 0);
	TESTL(KErrNone == err);
	User::WaitForRequest(stat);
	TESTL(stat == KErrNone);
	TESTL(desIn == desOut);
	CleanupStack::PopAndDestroy(&sock2); 
	CleanupStack::PopAndDestroy(&sock); 
    CleanupStack::PopAndDestroy(&ss); 
	SetTestStepResult(EPass);
	return EPass;
	}


