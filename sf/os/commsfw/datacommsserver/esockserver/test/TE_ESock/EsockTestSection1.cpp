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
// This contains ESock Test cases from section 1
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection1.h"


// Test step 1.1
const TDesC& CEsockTest1_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.1");
	return ret;
	}

CEsockTest1_1::~CEsockTest1_1()
	{
	}

enum TVerdict CEsockTest1_1::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.1")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d blank socket(s)"), nSockets);
	
	TInt ret = OpenSockets(nSockets);
	TESTEL(KErrNone == ret, ret);
	
	return EPass;
	}


// Test step 1.2
const TDesC& CEsockTest1_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.2");
	
	return ret;
	}

CEsockTest1_2::~CEsockTest1_2()
	{
	}

enum TVerdict CEsockTest1_2::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.2")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d TCP socket(s)"), nSockets);
	TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp, nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		(iEsockSuite->GetSocketHandle(i + 1)).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == 0);
		}
	
	return EPass;
	}


// Test step 1.3
const TDesC& CEsockTest1_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.3");
	
	return ret;
	}

CEsockTest1_3::~CEsockTest1_3()
	{
	}

enum TVerdict CEsockTest1_3::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.3")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d UDP socket(s)"), nSockets);
	TInt ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetUdp, nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<39> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		(iEsockSuite->GetSocketHandle(i + 1)).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == 0);
		}
	
	return EPass;
	}


// Test step 1.4
const TDesC& CEsockTest1_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.4");
	
	return ret;
	}

CEsockTest1_4::~CEsockTest1_4()
	{
	}

enum TVerdict CEsockTest1_4::easyTestStepL()
	{
	// Open n ICMP sockets
	
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.4")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d ICMP socket(s)"), nSockets);
	TInt ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetIcmp, nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		iEsockSuite->GetSocketHandle(i + 1).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == KErrNone);
		}
	
	return EPass;
	}


// Test step 1.5
const TDesC& CEsockTest1_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.5");
	
	return ret;
	}

CEsockTest1_5::~CEsockTest1_5()
	{
	}

enum TVerdict CEsockTest1_5::easyTestStepL()
	{
	// Open n IP sockets
	
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.5")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d IP socket(s)"), nSockets);
	TInt ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetIp, nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		iEsockSuite->GetSocketHandle(i + 1).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == KErrNone);
		}
	
	return EPass;
	}


// Test step 1.6
const TDesC& CEsockTest1_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.6");
	
	return ret;
	}

CEsockTest1_6::~CEsockTest1_6()
	{
	}

enum TVerdict CEsockTest1_6::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.6")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d TCP socket(s) by name"), nSockets);
	TInt ret = OpenSockets(_L("tcp"), nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		iEsockSuite->GetSocketHandle(i + 1).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == KErrNone);
		}
	
	return EPass;
	}


// Test step 1.7
const TDesC& CEsockTest1_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.7");
	
	return ret;
	}

CEsockTest1_7::~CEsockTest1_7()
	{
	}

enum TVerdict CEsockTest1_7::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.7")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d UDP socket(s) by name"), nSockets);
	TInt ret = OpenSockets(_L("udp"), nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		iEsockSuite->GetSocketHandle(i + 1).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == KErrNone); 
		}
	
	return EPass;
	}


// Test step 1.8
const TDesC& CEsockTest1_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.8");
	
	return ret;
	}

CEsockTest1_8::~CEsockTest1_8()
	{
	}

enum TVerdict CEsockTest1_8::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.8")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d ICMP socket(s) by name"), nSockets);
	TInt ret = OpenSockets(_L("icmp"), nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		iEsockSuite->GetSocketHandle(i + 1).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == 0);
		}
	
	return EPass;
	}


// Test step 1.9
const TDesC& CEsockTest1_9::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.9");
	
	return ret;
	}

CEsockTest1_9::~CEsockTest1_9()
	{
	}

enum TVerdict CEsockTest1_9::easyTestStepL()
	{
	// get number of sockets from script
	TInt nSockets; // number of sockets to open
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.9")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open %d IP sockets by name"), nSockets);
	TInt ret = OpenSockets(_L("ip"), nSockets);
	TESTEL(KErrNone == ret, ret);
	
	TInetAddr addr;
	TBuf<32> buf;
	for (TInt i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		iEsockSuite->GetSocketHandle(i + 1).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == KErrNone); 
		}
	
	return EPass;
	}


// Test step 1.10
const TDesC& CEsockTest1_10::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.10");
	
	return ret;
	}

CEsockTest1_10::~CEsockTest1_10()
	{
	}

enum TVerdict CEsockTest1_10::easyTestStepL()
	{
	// Open a TCP socket with invalid socket type
	Logger().WriteFormat(_L("Open socket, of invalid type, for TCP"));
	RSocket sock;
	CleanupClosePushL(sock);
	TInt ret = sock.Open(iEsockSuite->iSocketServer, KAfInet, 0, KProtocolInetTcp);
	TESTEL(ret==KErrBadName, ret);
	
	// Open a UDP socket with invalid socket type
	Logger().WriteFormat(_L("Open socket, of invalid type, for UDP"));
	RSocket sock2;
	CleanupClosePushL(sock2);
	ret = sock2.Open(iEsockSuite->iSocketServer, KAfInet, 12, KProtocolInetUdp);
	TESTEL(ret==KErrBadName, ret);
	
	CleanupStack::PopAndDestroy(2, &sock);
	return EPass;
	}


// Test step 1.11
const TDesC& CEsockTest1_11::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.11");
	
	return ret;
	}

CEsockTest1_11::~CEsockTest1_11()
	{
	}

enum TVerdict CEsockTest1_11::easyTestStepL()
	{
	// open sockets with an invalid protocol
	
	// get protocol id's from script
	TInt protocol1, protocol2;
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.11")), _L("protocolId1"), protocol1));
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.11")), _L("protocolId2"), protocol2));
	
	Logger().WriteFormat(_L("Open stream socket for invalid protocol"));
	RSocket sock;
	CleanupClosePushL(sock);
	TInt ret = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, protocol1);
	TESTEL(ret==KErrBadName, ret);
	
	Logger().WriteFormat(_L("Open datagram socket for invalid protocol"));
	RSocket sock2;
	CleanupClosePushL(sock2);
	ret = sock2.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, protocol2);
	TESTEL(ret==KErrBadName, ret);

	CleanupStack::PopAndDestroy(2, &sock);
	return EPass;
	}


// Test step 1.12
const TDesC& CEsockTest1_12::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.12");
	
	return ret;
	}

CEsockTest1_12::~CEsockTest1_12()
	{
	}

enum TVerdict CEsockTest1_12::easyTestStepL()
	{
	Logger().WriteFormat(_L("Open stream socket for UDP"));

	RSocket sock;
	CleanupClosePushL(sock);
	TInt ret = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetUdp);
	TESTEL(ret==KErrBadName, ret);

	CleanupStack::PopAndDestroy();
	return EPass;
	}


// Test step 1.13
const TDesC& CEsockTest1_13::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.13");
	
	return ret;
	}

CEsockTest1_13::~CEsockTest1_13()
	{
	}

enum TVerdict CEsockTest1_13::easyTestStepL()
	{
	Logger().WriteFormat(_L("Open datagram socket for TCP"));
	
	RSocket sock;
	CleanupClosePushL(sock);
	TInt ret = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetTcp);
	TESTEL(ret==KErrBadName, ret);

	CleanupStack::PopAndDestroy();
	return EPass;
	}


// Test step 1.14
const TDesC& CEsockTest1_14::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.14");
	
	return ret;
	}

CEsockTest1_14::~CEsockTest1_14()
	{
	}

enum TVerdict CEsockTest1_14::easyTestStepL()
	{
	Logger().WriteFormat(_L("Open socket for invalid protocol by name"));

	RSocket sock;
	CleanupClosePushL(sock);
	TInt ret =sock.Open(iEsockSuite->iSocketServer, _L("abc"));
	TESTEL(ret==KErrNotFound, ret);

	CleanupStack::PopAndDestroy();
	return EPass;
	}


// Test step 1.15
const TDesC& CEsockTest1_15::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.15");
	
	return ret;
	}

CEsockTest1_15::~CEsockTest1_15()
	{
	}

enum TVerdict CEsockTest1_15::easyTestStepL()
	{
	CloseSockets(2);
	return EPass;
	}

// Test step 1.16
const TDesC& CEsockTest1_16::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test1.16");
	
	return ret;
	}

CEsockTest1_16::~CEsockTest1_16()
	{
	}

enum TVerdict CEsockTest1_16::easyTestStepPreambleL()
	{
	TInt ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetUdp);
	Logger().WriteFormat(_L("Open a UDP socket, returned %d"), ret);
	if (KErrNone != ret)
		{
		return EFail;
		}
	return EPass;
	}

enum TVerdict CEsockTest1_16::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	if (iEsockSuite->GetSocketListCount() < 1)
		{
		return EInconclusive;
		}
	
    const TInt KBufferLength = 10;
	TBuf8<KBufferLength> buffer;
	buffer.SetMax();
	buffer.FillZ();

	TInetAddr      destAddr;
	TRequestStatus status;
	
	Logger().WriteFormat(_L("Issuing 'SendTo' with invalid destination..."));
	iEsockSuite->GetSocketHandle(1).SendTo(buffer, destAddr, 0, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("...which returned %d\n"), status.Int());	// Should return '-2'
	TESTEL(status == KErrGeneral, EFail);

	// Test 1.15 Close socket
	
    return EPass;
	}

// Test step 1.17
// Open a socket using the RawIp protocol
const TDesC& CEsockTest1_17::GetTestName()
	{
	// store the name of this test case
	_LIT(ret, "Test1.17");
	
	return ret;		
	}

CEsockTest1_17::~CEsockTest1_17()
	{	
	}
		
enum TVerdict CEsockTest1_17::easyTestStepL()
	{			
	Logger().WriteFormat(_L("Open a RawIP socket"));
	
	_LIT(KRawIp, "rawip");
	TInt ret = OpenSockets(KRawIp);
	TESTEL(ret == KErrNone, ret);
	
	return EPass;
	}
	

