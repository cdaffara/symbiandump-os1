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
// This contains ESock Test cases from section 3
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection3.h"

static const TInt KMicrosecondsPerSecond = 1000000;

// Test step 3.1
const TDesC& CEsockTest3_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test3.1");
	
	return ret;
	}

CEsockTest3_1::~CEsockTest3_1()
	{
	}

enum TVerdict CEsockTest3_1::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_3.1"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest3_1::easyTestStepL()
	{
	// Test 1.x Open a socket
	
	// Set the address to loopback and the port to 0, IPv4 or IPv6 address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_3.1")), _L("ipAddress"), addr));
	
	addr.SetPort(0);
	
	TRequestStatus stat;
	for (TInt i = 1; i <= iEsockSuite->GetSocketListCount(); i++)
		{
		// Connect to the loopback address on port 0
		iEsockSuite->GetSocketHandle(i).Connect(addr, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrGeneral, stat.Int());
		}
	
	// Test 1.15 Close socket
	
	return EPass;
	}


// Test step 3.2
const TDesC& CEsockTest3_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test3.2");
	
	return ret;
	}

CEsockTest3_2::~CEsockTest3_2()
	{
	}

enum TVerdict CEsockTest3_2::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_3.2"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest3_2::easyTestStepL()
	{
	// Test 1.x Open a socket
	
	// Set the address to loopback IPv4 or IPv6 address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_3.2")), _L("ipAddress"), addr));
	
	// set the port numnber
	addr.SetPort(65537);
	
	TRequestStatus stat;
	for (TInt i = 1; i <= iEsockSuite->GetSocketListCount(); i++)
		{
		// Connect to the loopback address on port 65537
		iEsockSuite->GetSocketHandle(i).Connect(addr, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrGeneral, stat.Int());
		}
	
	// Test 1.15 Close socket
	
	return EPass;
	}


// Test step 3.3
const TDesC& CEsockTest3_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test3.3");
	
	return ret;
	}

CEsockTest3_3::~CEsockTest3_3()
	{
	}

enum TVerdict CEsockTest3_3::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_3.3"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest3_3::easyTestStepL()
	{
	// Test 1.x Open a socket
	
	// Set the address to loopback, IPv4 or IPv6 address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_3.3")), _L("ipAddress"), addr));
	
	// Set to a non listening port
	TInt port;
	TESTL(GetIntFromConfig(_L("Test_3.3"), _L("port"), port));
	
	addr.SetPort(port);
	
	TRequestStatus stat;
	for (TInt i = 1; i <= iEsockSuite->GetSocketListCount(); i++)
		{
		// Connect to the loopback address on the non listening port n 
		iEsockSuite->GetSocketHandle(i).Connect(addr, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrCouldNotConnect, stat.Int());
		}
	
	// Test 1.15 Close socket
	
	return EPass;
	}

// Test step 3.4
const TDesC& CEsockTest3_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test3.4");
	
	return ret;
	}

CEsockTest3_4::~CEsockTest3_4()
	{
	}

enum TVerdict CEsockTest3_4::easyTestStepL()
	{
	// Test 1.x Open a socket
	
	// Set the address to Remote host, IPv4 or IPv6 address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(SectionName(_L("Test_3.4")), _L("ipAddress"), addr));
	
	// Set to non listening port
	TInt port;
	TESTL(GetIntFromConfig(_L("Test_3.4"), _L("port"), port));
	addr.SetPort(port);
	
	TRequestStatus stat;
	for (TInt i = 1; i <= iEsockSuite->GetSocketListCount(); i++)
		{
		// Connect to the remote host address on the non listening port n 
		iEsockSuite->GetSocketHandle(i).Connect(addr, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrCouldNotConnect, stat.Int());
		}
	
	// Test 1.15 Close socket
	
	return EPass;
	}

// Test step 3.5
const TDesC& CEsockTest3_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test3.5");
	
	return ret;
	}

CEsockTest3_5::~CEsockTest3_5()
	{
	}

enum TVerdict CEsockTest3_5::easyTestStepPreambleL()
	{
	CloseSockets();
	if (KErrNone != OpenMinSockets(_L("Test_3.5"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest3_5::easyTestStepL()
	{
	TRequestStatus stat;
	TInetAddr addr;
	TInt port;
	
	// Test 1.x Open a socket
	
	// Set the address to Remote host, IPv4 or IPv6 address
	TESTL(GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addr));
	
	// Set to echo port
	TESTL(GetIntFromConfig(SectionName(_L("Test_3.5")), _L("port"), port));
	addr.SetPort(port);
	
	// Connect to the remote host address on the echo port 
	iEsockSuite->GetSocketHandle(1).Connect(addr, stat);
	iEsockSuite->GetSocketHandle(1).CancelConnect();
	User::WaitForRequest(stat);
	TESTEL(stat==KErrCancel, stat.Int());
	
	// Test 1.15 Close socket
	
	return EPass;
	}


// Test step 3.6
const TDesC& CEsockTest3_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test3.6");
	
	return ret;
	}

CEsockTest3_6::~CEsockTest3_6()
	{
	}


enum TVerdict CEsockTest3_6::subTest2(RTimer	&aWatchdog,
									  RSocket	&aSocket,
									  TDes8		&aBuffer,
									  TInt		aExpectedStatus)
	{
	TBool finished = EFalse;
	TInt i = 1;
	TRequestStatus sendStatus;
	TRequestStatus recvStatus;
	TRequestStatus timerStatus;
	while ((i <= iNumTries) && !finished)
		{
		aWatchdog.After(timerStatus, 7 * KMicrosecondsPerSecond);
		Logger().Write(_L("Issuing recv..."));
		aSocket.Recv(aBuffer, 0, recvStatus);
		Logger().Write(_L("Sending..."));
        aSocket.SendTo(aBuffer, iAddr, 0, sendStatus);

    	User::WaitForRequest(timerStatus, recvStatus);
        aWatchdog.Cancel();
        aSocket.CancelRecv();
    	User::WaitForAnyRequest();
    	
    	finished = recvStatus == aExpectedStatus && 
        (timerStatus == KRequestPending || timerStatus == KErrNone || timerStatus == KErrCancel);

    	if (!finished)
			{
			User::After(1 * KMicrosecondsPerSecond);
			i++;
			}
		}

	if (!finished)
		{
		return EFail;
		}
	return EPass;
	}


enum TVerdict CEsockTest3_6::easyTestStepPreambleL()
	{
    // Get the ini file data.
	_LIT(KIniTestKey,    "Test_3.6");

	// Set the address to Remote host, IPv4 or IPv6 address
	_LIT(KIniAddressKey, "ipAddress");
	TESTL(GetIpAddressFromConfig(KIniTestKey(), KIniAddressKey(), iAddr));
	
	// Set to non-existent port
	_LIT(KIniErrPortKey, "errorPort");
	TESTL(GetIntFromConfig(KIniTestKey(), KIniErrPortKey, iErrorPort));
	iAddr.SetPort(iErrorPort);

	// Get the valid port
	_LIT(KIniValPortKey, "validPort");
	TESTL(GetIntFromConfig(KIniTestKey(), KIniValPortKey, iValidPort));
	
	// Get the number of times to try the sub-test before giving up.
	_LIT(KIniValTries,   "validTries");
	TESTL(GetIntFromConfig(KIniTestKey(), KIniValTries, iNumTries));

	if (KErrNone != OpenMinSockets(KIniTestKey(), KAfInet, KSockDatagram, KProtocolInetUdp))
		{
		return EFail;
		}
	
   	// Enable notification of ICMP errors.
	RSocket &sock = iEsockSuite->GetSocketHandle(1);
	TInt ret = sock.SetOpt(KSoUdpReceiveICMPError, KSolInetUdp, 1);
	if (KErrNone != ret)
		{
		Logger().WriteFormat(_L("SetOpt to receive ICMP errors returns %d"), ret);
		return EFail;
		}

	return EPass;
	}


enum TVerdict CEsockTest3_6::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	TVerdict verdict = EPass;

    const TInt KStringMaxLen = 40;
    TBuf<KStringMaxLen> addressBuf;
	iAddr.Output(addressBuf);
	Logger().Write(_L(""));
	Logger().WriteFormat(_L("Test3.6 - Testing address = %S, errorPort = %d, validPort = %d"), &addressBuf, iErrorPort, iValidPort);

	RTimer         watchdog;
	CleanupClosePushL(watchdog);
	watchdog.CreateLocal();
	
    const TInt KBufferLength = 10;  
	TBuf8<KBufferLength> buffer;
	buffer.SetMax();
	buffer.FillZ();

	RSocket &sock = iEsockSuite->GetSocketHandle(1);

    // Part 1. Invalid port#, no pending receive
    //
	// First Recv returns -34, second one will pause.
    
	Logger().WriteFormat(_L("Test3.6 Part 1 - Address = %S, errorPort = %d"), &addressBuf, iErrorPort);
	
	TBool finished = EFalse;
    TInt i = 1;
	TRequestStatus status;
	TRequestStatus recvStatus;
	TRequestStatus timerStatus;
	TInt ret;
    while ((i <= iNumTries)  && !finished)
		{
    	status = recvStatus = timerStatus = KRequestPending;

    	sock.SendTo(buffer, iAddr, 0, status);
    	User::WaitForRequest(status);
    	ret = status.Int();
    	Logger().WriteFormat(_L("Send returned %d"), ret);
    	TESTL(KErrNone == ret);
    	
    	// Delay to allow the connection to come up.
    	User::After(5 * KMicrosecondsPerSecond);

		watchdog.After(timerStatus, 5 * KMicrosecondsPerSecond);
		sock.Recv(buffer, 0, recvStatus);
		User::WaitForRequest(timerStatus, recvStatus);
		watchdog.Cancel();
		sock.CancelRecv();
		User::WaitForAnyRequest();
		ret = recvStatus.Int();
		Logger().WriteFormat(_L("1st recv returned %d"), ret);
		
        // This Recv should complete with -34.
    	finished = recvStatus == KErrCouldNotConnect && 
    		(timerStatus == KRequestPending || timerStatus == KErrNone || timerStatus == KErrCancel);

        if (finished)
        	{
        	recvStatus = timerStatus = KRequestPending;

    		watchdog.After(timerStatus, 5 * KMicrosecondsPerSecond);
    		sock.Recv(buffer, 0, recvStatus);
    		User::WaitForRequest(timerStatus, recvStatus);
    		watchdog.Cancel();
    		sock.CancelRecv();
    		User::WaitForAnyRequest();
    		ret = recvStatus.Int();
    		Logger().WriteFormat(_L("2nd recv returned %d"), ret);
    		
    		// This time we expect the watchdog to fire, an immediate Recv completion here means the
    		// test has failed (error not cleared down).
    		
    		finished = timerStatus == KErrNone && 
    			(recvStatus == KRequestPending || recvStatus == KErrNone || recvStatus == KErrCancel);
        	}

        if (!finished)
        	{
        	User::After(1 * KMicrosecondsPerSecond);
        	i++;
        	}
		}

    if (!finished)
    	{
    	verdict = EFail;
    	}
    TESTL(EPass == verdict);

    // Part 2. Invalid port#, with pending receive; should complete with -34
	Logger().WriteFormat(_L("Test3.6 Part 2 - Address = %S, errorPort = %d"), &addressBuf, iErrorPort);
	TVerdict subTestVerdict = subTest2(watchdog, sock, buffer, KErrCouldNotConnect);
	if (verdict == EPass)
		{
		verdict = subTestVerdict;
		}
    TESTL(EPass == verdict);

    // Part 3. Valid port#, with pending receive; should complete with KErrNone
	Logger().WriteFormat(_L("Test3.6 Part 3 - Address = %S, validPort = %d"), &addressBuf, iValidPort);
	iAddr.SetPort(iValidPort);
	subTestVerdict = subTest2(watchdog, sock, buffer, KErrNone);
	if (verdict == EPass)
		{
		verdict = subTestVerdict;
		}
    TESTL(EPass == verdict);

	// Test 1.15 Close socket
	CleanupStack::PopAndDestroy(1, &watchdog);
    return EPass;
	}

