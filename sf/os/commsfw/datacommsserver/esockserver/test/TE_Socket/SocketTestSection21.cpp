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

// EPOC includes
#include <e32base.h>
#include <es_sock.h>

// Test system includes
#include "SocketTestSection21.h"
#include <test/es_dummy.h>

// Test step 21.1
const TDesC& CSocketTest21_1::GetTestName()
	{
	_LIT(ret,"Test21.1");
	return ret;
	}

enum TVerdict CSocketTest21_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Ioctl call with data"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Open a socket on Dummy Protocol 1"));
	RSocket sock;
	CleanupClosePushL (sock);
	ret = sock.Open(ss,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Issue an Ioctl by passing valid data"));	
	TBuf8<64> ioctlData;
	TRequestStatus status;
	sock.Ioctl (KDummyIoctlCompleteWithData, status, &ioctlData);
	User::WaitForRequest(status);
	TESTL(KErrNone == status.Int());
	TESTL(ioctlData.Compare(KIoctlData()) == 0);
	CleanupStack::PopAndDestroy(); // sock	


	Logger().WriteFormat(_L("Open a socket again on Dummy Protocol 1"));
	RSocket sock2;
	CleanupClosePushL (sock2);
	ret = sock2.Open(ss,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Issue an Ioctl by passing NULL data"));
	TRequestStatus status2;
	sock2.Ioctl (KDummyIoctlCompleteWithData, status2, NULL);
	User::WaitForRequest(status2);
	TESTL(KErrNone == status2.Int());
	CleanupStack::PopAndDestroy(); // sock2		
	
	CleanupStack::PopAndDestroy(); // ss

	return verdict;
	}

// Test step 21.2
const TDesC& CSocketTest21_2::GetTestName()
	{
	_LIT(ret,"Test21.2");
	return ret;
	}

enum TVerdict CSocketTest21_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Connect complete with data"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Open a socket on Dummy Protocol 3"));
	RSocket sock;
	CleanupClosePushL (sock);
	ret = sock.Open(ss, KDummyThreeName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Setting sockopt not to complete connect"));	
	ret = sock.SetOpt(KDummyOptionSetBlockConnectData,28,TRUE); //Block connect
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Perform connect"));
	TRequestStatus status;
	TSockAddr addr;
	TBuf8<64> connectedData;
	sock.Connect (addr, KNullDesC8(), connectedData, status);
	Logger().WriteFormat(_L("<i>Setting sockopt to now complete the connect with data</i>"));
	ret = sock.SetOpt(KDummyOptionSetBlockConnectData,28,FALSE); //Unblock Connect
	TESTL(KErrNone == ret);
	User::WaitForRequest(status);
	TESTL(KErrNone == status.Int());
	TESTL(connectedData.Compare(KConnectCompleteData()) == 0);

	CleanupStack::PopAndDestroy(); // sock
	
	Logger().WriteFormat(_L("Open a socket again on Dummy Protocol 3"));
	RSocket sock2;
	CleanupClosePushL (sock2);
	ret = sock2.Open(ss, KDummyThreeName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Setting sockopt not to complete connect"));	
	ret = sock2.SetOpt(KDummyOptionSetBlockConnectData,28,TRUE); //Block connect
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Perform connect"));
	TRequestStatus status2;
	TSockAddr addr2;
	sock2.Connect (addr2, status2);
	Logger().WriteFormat(_L("<i>Setting sockopt to now complete the connect with data</i>"));
	ret = sock2.SetOpt(KDummyOptionSetBlockConnectData,28,FALSE); //Unblock Connect
	TESTL(KErrNone == ret);
	User::WaitForRequest(status2);
	TESTL(KErrNone == status2.Int());

	CleanupStack::PopAndDestroy(2); // sock, ss

	return verdict;
	}

	

