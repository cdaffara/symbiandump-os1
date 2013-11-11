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
// This contains ESock Test cases from section 2
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection2.h"


// Test step 2.1
const TDesC& CEsockTest2_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test2.1");
	
	return ret;
	}

CEsockTest2_1::~CEsockTest2_1()
	{
	}

enum TVerdict CEsockTest2_1::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_2.1"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest2_1::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	TInt nSockets = iEsockSuite->GetSocketListCount();
	// bind to n sockets on port 0
	
	// get ip address
	TInetAddr addr, addr1;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_2.1")), _L("ipAddress"), addr));
	
	// set the port number to 0
	addr.SetPort(0);
	
	// for each open socket
	TInt nRet;
	TUint port;
	for (TInt n = 0; n < nSockets; n++)
		{
		// bind the socket
		nRet = iEsockSuite->GetSocketHandle(n + 1).Bind(addr);
		TESTEL(nRet == KErrNone, nRet);
		
		// get the socket name
		iEsockSuite->GetSocketHandle(n + 1).LocalName(addr1);
		
		// check address has been set correctly
		TESTL(addr.Match(addr1));
		
		// get port number - check it is non-zero
		port = iEsockSuite->GetSocketHandle(n + 1).LocalPort();
		TESTL(port > 0);
		}
	
	return EPass;
	}


// Test step 2.2
const TDesC& CEsockTest2_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test2.2");
	
	return ret;
	}

CEsockTest2_2::~CEsockTest2_2()
	{
	}

enum TVerdict CEsockTest2_2::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_2.2"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest2_2::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	// bind to an illegal port number
	
	// get IP address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_2.2")), _L("ipAddress"), addr));
	
	// set port number to invalid value
	addr.SetPort(65537);
	
	// socket already opened - retrieve
	// socket handle and bind
	TInt nRet = iEsockSuite->GetSocketHandle(1).Bind(addr);
	
	TESTEL(nRet == KErrTooBig, nRet);
	
	return EPass;
	}


// Test step 2.3
const TDesC& CEsockTest2_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test2.3");
	
	return ret;
	
	}

CEsockTest2_3::~CEsockTest2_3()
	{
	}

enum TVerdict CEsockTest2_3::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_2.3"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest2_3::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	// bind to an illegal address
	
	// get IP address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_2.3")), _L("ipAddress"), addr));
	
	// set port number to a valid value
	addr.SetPort(7);
	
	// socket already opened - retrieve
	// socket handle and bind
	TInt nRet = iEsockSuite->GetSocketHandle(1).Bind(addr);
	
	TESTEL(nRet == KErrNotFound, nRet);
	
	return EPass;
	}


// Test step 2.4
const TDesC& CEsockTest2_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test2.4");
	
	return ret;
	}

CEsockTest2_4::~CEsockTest2_4()
	{
	}

enum TVerdict CEsockTest2_4::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_2.4"),KAfInet, KSockStream, KProtocolInetTcp, 2))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest2_4::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	// double bind on different sockets
	
	// get IP address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_2.4")), _L("ipAddress"), addr));
	
	// set port number to a valid value
	addr.SetPort(9);
	
	// socket already opened - retrieve
	// socket handle and bind
	TInt nRet = iEsockSuite->GetSocketHandle(1).Bind(addr);
	
	// bind should succeed
	TESTEL(nRet == KErrNone, nRet);
	
	// socket already opened - retrieve
	// socket handle and bind again to same address
	nRet = iEsockSuite->GetSocketHandle(2).Bind(addr);
	
	// second bind should fail
	TESTEL(nRet == KErrInUse, nRet);
	
	return EPass;
	}


// Test step 2.5
const TDesC& CEsockTest2_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test2.5");
	
	return ret;
	}

CEsockTest2_5::~CEsockTest2_5()
	{
	}

enum TVerdict CEsockTest2_5::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_2.5"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest2_5::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// get port numbers
	TInt port1, port2;
	TESTL(GetIntFromConfig(SectionName(_L("Test_2.5")), _L("port1"), port1));
	TESTL(GetIntFromConfig(SectionName(_L("Test_2.5")), _L("port2"), port2));
	
	// set the local port number to port1
	TInt nRet = iEsockSuite->GetSocketHandle(1).SetLocalPort(port1);
	TESTEL(nRet == KErrNone, nRet);
	
	// check port has been set correctly
	TUint port = iEsockSuite->GetSocketHandle(1).LocalPort();
	TESTL(port == (TUint)port1);
	
	// set the local port number to port2
	// check that this is not successful
	nRet = iEsockSuite->GetSocketHandle(1).SetLocalPort(port2);
	TESTEL(nRet == KErrAlreadyExists, nRet);
	
	// check port has not been changed
	port = iEsockSuite->GetSocketHandle(1).LocalPort();
	TESTL(port == (TUint)port1);
	
	return EPass;
	}

