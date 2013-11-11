// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 25
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection25.h"


// Test step 25.1
const TDesC& CEsockTest25_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test25.1");
	
	return ret;
	}

CEsockTest25_1::~CEsockTest25_1()
	{
	}

enum TVerdict CEsockTest25_1::easyTestStepL()
	{
	
	TInetAddr addr;
	THostName name;
	TNameRecord hostent;
	
	Logger().WriteFormat(_L("TE_ESock: test 25.1"));
	Logger().WriteFormat(_L("Lookup name in hosts file"));
	
	// Have to lookup a valid or invalid address to establish a csd connection so 
	// Lookup returned address and lookup returned name below can be completed 
	Logger().WriteFormat(_L("Lookup alias: a"));
	GetHostByNameL(hostent, _L("a"));
	
	Logger().WriteFormat(_L("Lookup returned address"));
	name = hostent.iName;
	GetHostByNameL(hostent, name);
	
	Logger().WriteFormat(_L("Lookup returned name"));
	addr = hostent.iAddr;
	GetHostByAddrL(hostent, addr);
	
	return EPass;
	}


// Test step 25.2
const TDesC& CEsockTest25_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test25.2");
	
	return ret;
	}

CEsockTest25_2::~CEsockTest25_2()
	{
	}

enum TVerdict CEsockTest25_2::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 25.2"));
	Logger().WriteFormat(_L("Get Machine Name"));
	
	GetHostNameL();
	
	return EPass;
	}


// Test step 25.3
const TDesC& CEsockTest25_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test25.3");
	
	return ret;
	}

CEsockTest25_3::~CEsockTest25_3()
	{
	}

enum TVerdict CEsockTest25_3::easyTestStepL()
	{
	TPtrC aPtrResult;
	TInt bRet;
	
	Logger().WriteFormat(_L("TE_ESock: test 25.3"));
	Logger().WriteFormat(_L("Get Machine Name"));
	
	bRet = GetStringFromConfig(_L("Test_25.3"), _L("sethostname"), aPtrResult);
	if (!bRet)
		{
		// failed to find ipAddress
		return EFail;
		}
	
	GetHostNameL();
	
	// Synchronous
	SetHostNameL(aPtrResult);
	
	GetHostNameL();
	
	return EPass;
	}


// Test step 25.4
const TDesC& CEsockTest25_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test25.4");
	
	return ret;
	}

CEsockTest25_4::~CEsockTest25_4()
	{
	}

enum TVerdict CEsockTest25_4::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 25.4"));
	Logger().WriteFormat(_L("Lookup name in hosts file and Cancel"));
	
	TNameRecord hostent;
	TSockAddr iAddr;
	
	Logger().WriteFormat(_L("Lookup alias: a"));
	GetHostByNameL(hostent, _L("a"));
	iAddr=hostent.iAddr;
	
	Logger().WriteFormat(_L("Cancel Lookup returned address"));
	GetHostByNameCancelL(hostent, _L("www.yahoo.com"));
	
	Logger().WriteFormat(_L("Lookup returned address"));
	GetHostByNameL(hostent, _L("a") );
	
	TESTL( iAddr == hostent.iAddr );
	
	return EPass;
	
	}


// Test step 25.5
const TDesC& CEsockTest25_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test25.5");
	
	return ret;
	}

CEsockTest25_5::~CEsockTest25_5()
	{
	}

enum TVerdict CEsockTest25_5::easyTestStepL()
	{
	//
	// Out Of Memory Test on open rhostresolver //
	//

	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("TE_ESock: test 25.5"));
	Logger().WriteFormat(_L("RHostResolver Open"));

#if defined (_DEBUG_SOCKET_FUNCTIONS)

	// Open Socket Server
	RSocketServ socketServer;
	CleanupClosePushL(socketServer);
	TInt ret = socketServer.Connect();
	TESTEL(KErrNone == ret, ret);
	
	RHostResolver hr;
	CleanupClosePushL(hr);
	TInt failure=0;

	verdict = EInconclusive;
	TInt prevResult = KErrNoMemory;
	TInt prevOccurs = 0;
	while (EInconclusive == verdict)
		{
		socketServer.__DbgFailNext(failure++);
		ret = hr.Open(socketServer, KAfInet, KProtocolInetUdp);
		
		if ((prevResult != ret) || (++prevOccurs >= 1000))
			{
			Logger().WriteFormat(_L("%d loop(s), open host resolver returned %d"), prevOccurs, prevResult);
			if (KErrNone == ret)
				{
				verdict = EPass;
				}
			else if (KErrServerTerminated == ret)
				{
				verdict = EFail;
				}
			else if (prevResult != ret)
				{
				prevResult = ret;
				prevOccurs = 1;
				}
			else
				{
				prevOccurs = 0;
				}
			}
		};
	
	socketServer.__DbgFailNext(-1);
	CleanupStack::PopAndDestroy(2, &socketServer);

#else
	Logger().WriteFormat(_L("Test disabled on release build."));
#endif

    //-- check esock server's heap balance by shutting down the socket server
	return verdict;
	}

