// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ss_std.h>

// Test system includes
#include "SocketTestSection1.h"


_LIT(KDummyProtocol1Name,"Dummy Protocol 1"); // corresponds to KDummyOneName in pdummy

// Test step 1.1
const TDesC& CSocketTest1_1::GetTestName()
	{
	_LIT(ret,"Test1.1");
	return ret;
	}

enum TVerdict CSocketTest1_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Connect and Disconnect"));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	TInt ret = OptimalConnect(ss1);
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Closing Session"));
	CleanupStack::Pop(&ss1);
	ss1.Close();

	Logger().WriteFormat(_L("Re-connecting to socket server"));
    RSocketServ ss;
	ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	CleanupStack::Pop();
	ss.Close();

 	SetTestStepResult(verdict);
	return verdict;
	}


// Test step 1.2
const TDesC& CSocketTest1_2::GetTestName()
	{
	_LIT(ret,"Test1.2");
	return ret;
	}

enum TVerdict CSocketTest1_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Open and Close Sockets"));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	// NumProtocols and GetProtocolInfo
	Logger().WriteFormat(_L("Try NumProtocols"));
//	ss.__DbgMarkHeap();		// in ESOCKMT leak checking is best done by shutting down the server
	TUint numProtocols;
	ret = ss.NumProtocols(numProtocols);
	Logger().WriteFormat(_L("NumProtocols returned %S, number %d"),
			&EpocErrorToText(ret), numProtocols);
	TESTL(KErrNone == ret);

	TProtocolDesc protoInfo;
	TUint aIndex;
	TBool aFound = false;
	RSocket sock1;
	TInt pdummyIndex = -1;
 	Logger().WriteFormat(_L("Trying GetProtocolInfo until Dummy Protocol 1 is found"));
	for (aIndex=1;aIndex<=numProtocols;aIndex++)
		{
		ret = ss.GetProtocolInfo(aIndex, protoInfo);
		Logger().WriteFormat(_L("GetProtocolInfo returned %S"), &EpocErrorToText(ret));
		TESTL(KErrNone == ret || KErrCannotFindProtocol == ret);
		Logger().WriteFormat(_L("Protocol Index %d corresponds to %S"), aIndex, &protoInfo.iName);
		if (protoInfo.iName == KDummyProtocol1Name())
			{
			pdummyIndex = aIndex;
			aFound = true;
			}
		}

	if (!aFound) {
		Logger().WriteFormat(_L("Could not find Dummy Protocol 1"));
		CleanupStack::PopAndDestroy();
		return EFail;
	}

	Logger().WriteFormat(_L("Opening Socket for Family: %d, Sock: %d, protocol: %d"),
			protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);

	ret=sock1.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Opened %S socket OK"), &protoInfo.iName);

	Logger().WriteFormat(_L("Getting protocol info for index %d again"),pdummyIndex);
	ret = ss.GetProtocolInfo(pdummyIndex, protoInfo);
	Logger().WriteFormat(_L("GetProtocolInfo returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);


	Logger().WriteFormat(_L("Opening another socket"));
	RSocket sock2;
	ret = sock2.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Opened %S socket OK"), &protoInfo.iName);

	Logger().WriteFormat(_L("Closing sockets"));
	sock2.Close();
	sock1.Close();

	Logger().WriteFormat(_L("Opening another socket"));
	RSocket sock3;
	ret = sock3.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Opened %S socket OK"), &protoInfo.iName);

	Logger().WriteFormat(_L("Closing socket"));
	sock3.Close();

	Logger().WriteFormat(_L("Opening a socket by name on dummy protocol 1"));
	RSocket sock4;
	ret = sock4.Open(ss, KDummyProtocol1Name());
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Opened %S socket OK"), &protoInfo.iName);

	Logger().WriteFormat(_L("Closing socket"));
	sock4.Close();

	CleanupStack::Pop(&ss);
	ss.Close();

	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 1.3
const TDesC& CSocketTest1_3::GetTestName()
	{
	_LIT(ret,"Test1.3");
	return ret;
	}


enum TVerdict CSocketTest1_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Open Bad Socket Type"));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Attempting to create bad socket type"));
	RSocket sock;
	ret = sock.Open(ss, 0, 0, 0);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrBadName == ret);

	sock.Close();
	CleanupStack::Pop(&ss);
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 1.4
const TDesC& CSocketTest1_4::GetTestName()
	{
	_LIT(ret,"Test1.4");
	return ret;
	}

enum TVerdict CSocketTest1_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Find Protocols"));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Try to find non-existent protocol"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Not a Protocol Name"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);

    Logger().WriteFormat(_L("Try to find dummy protocol 1"));
	ret = ss.FindProtocol(KDummyProtocol1Name(), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	CleanupStack::Pop(&ss);
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 1.5
const TDesC& CSocketTest1_5::GetTestName()
	{
	_LIT(ret,"Test1.5");
	return ret;
	}

enum TVerdict CSocketTest1_5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Tidy Up Sockets on Thread Death"));

	Logger().WriteFormat(_L("Main thread : Creating sub thread"));
	RSemaphore s;
	s.CreateLocal(0);
	RThread t;
	TSocketThreadArg a;
	a.iHandle = this;
	a.iSem=&s;
	a.iNumSockets = KNumTestSockets;
	//Need to share logger across different threads
	Logger().ShareAuto();
	t.Create(_L("SocketThread1"), SocketThread,
			 KDefaultStackSize, KDefaultHeapSize, KDefaultHeapSize, &a);
	t.Resume();
	s.Wait();

	Logger().WriteFormat(_L("Main thread : Killing Sub thread"));
	t.Kill(KErrNone);
	t.Close();

	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 1.6
const TDesC& CSocketTest1_6::GetTestName()
	{
	_LIT(ret,"Test1.6");
	return ret;
	}

enum TVerdict CSocketTest1_6::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Close Uninitialised Socket"));

	Logger().WriteFormat(_L("Attempting to close socket"));
	RSocket s;
	s.Close();
	s.Close();

	return verdict;
	}

// Test step 1.7
const TDesC& CSocketTest1_7::GetTestName()
	{
	_LIT(ret,"Test1.7");
	return ret;
	}

enum TVerdict CSocketTest1_7::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	TInt numExhaustiveSockets = iNormalTest ? KNumExhaustiveSockets : 10;	
	iNormalTest = EFalse; // normal test is only run once, the rest are OOM tests

	Logger().WriteFormat(_L("Test Purpose: Exhaustive Socket Open"));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Finding dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(KDummyProtocol1Name(), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Opening socket"));
	RSocket sock;
	ret = sock.Open(ss,protoInfo.iAddrFamily,protoInfo.iSockType,protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Closing socket"));
	sock.Close();

    Logger().WriteFormat(_L("Creating array of %d sockets"), numExhaustiveSockets);
	RSocket* socks = new (ELeave) RSocket[numExhaustiveSockets];
	CleanupArrayDeletePushL(socks);

	Logger().WriteFormat(_L("Attempting to Open %d sockets"), numExhaustiveSockets);
	TInt sockNum=0;
	ret = KErrNone;
	while (KErrNone==ret && sockNum<numExhaustiveSockets)
		{
		ret = socks[sockNum].Open(ss, protoInfo.iAddrFamily,
								  protoInfo.iSockType, protoInfo.iProtocol);
		sockNum++;
		}
	if (ret != KErrNone)
		{
		sockNum--;
		}
	Logger().WriteFormat(_L("Created %d sockets"), sockNum);

	Logger().WriteFormat(_L("Freeing sockets in creation order"));
	TInt i;
	for (i=0; i<sockNum; i++)
		{
		socks[i].Close();
		}


#if defined (_DEBUG_SOCKET_FUNCTIONS)

	Logger().WriteFormat(_L("Attempting to Open %d sockets"), numExhaustiveSockets);
	TInt sockCount1 = sockNum;
	ret = KErrNone;
	sockNum = 0;
	while (ret==KErrNone && sockNum<numExhaustiveSockets)
		{
		ret=socks[sockNum].Open(ss, protoInfo.iAddrFamily,
								protoInfo.iSockType,protoInfo.iProtocol);
		sockNum++;
		}
	if (ret != KErrNone)
		{
		sockNum--;
		}
	Logger().WriteFormat(_L("Created %d sockets"), sockNum);
	
	// Not sure what use it is to test whether we can open at least as many sockets as previously.
	// We can't assume that once we've freed all the sockets first time around that the ESock heap will
	// go back exactly to where it was before - this makes assumptions about the ESock algorithms.  Why
	// wouldn't ESock legitimately cache objects, for example?
	//TESTL(sockNum >= sockCount1);
	TESTL(sockNum >= sockCount1);

	Logger().WriteFormat(_L("Freeing sockets in creation order"));
	for (i=0; i<sockNum; i++)
		{
		socks[i].Close();
		}

	//
	// NOTE:
	// We do *not* free up the sockets but just close the session.  This is to exercise the subsession
	// cleanup behaviour in ESock with a large number of sockets.  We test that this cleanup operation
	// does not overflow the transport queue, as each subsession cleanup results in a message being sent.
	//
	
#endif	// (_DEBUG) }

	CleanupStack::PopAndDestroy(socks);

	CleanupStack::Pop(&ss);
	Logger().WriteFormat(_L("Now closing socket server session without closing %d opened sockets"), sockNum);
	ss.Close();
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 1.8
const TDesC& CSocketTest1_8::GetTestName()
	{
	_LIT(ret,"Test1.8");
	return ret;
	}

enum TVerdict CSocketTest1_8::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Exhaustively enumerate all the protocols"));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss);
	// NumProtocols and GetProtocolInfo
	Logger().WriteFormat(_L("Try NumProtocols"));
	TUint numProtocols;
	ret = ss.NumProtocols(numProtocols);
	Logger().WriteFormat(_L("NumProtocols returned %S, number %d"),
			&EpocErrorToText(ret), numProtocols);
	TESTL(KErrNone == ret);

	TProtocolDesc protoInfo;
	TUint aIndex;
	TInt pdummyIndex = -1;
 	Logger().WriteFormat(_L("Trying GetProtocolInfo on each protocol"));
	for (aIndex=1;aIndex<=numProtocols;aIndex++)
		{
		ret = ss.GetProtocolInfo(aIndex, protoInfo);
		Logger().WriteFormat(_L("GetProtocolInfo returned %S"), &EpocErrorToText(ret));
		TESTL(KErrNone == ret);
		Logger().WriteFormat(_L("Protocol Index %d corresponds to %S"), aIndex, &protoInfo.iName);
		if (protoInfo.iName ==KDummyProtocol1Name())
			{
			pdummyIndex = aIndex;
			}
		}
	CleanupStack::Pop(&ss);
	ss.Close();
	if (pdummyIndex < 0)
		{
		Logger().WriteFormat(_L("Could not find Dummy Protocol 1"));
		verdict = EFail;
		}
	verdict = EPass;

	SetTestStepResult(verdict);
	return verdict;
	}







// Test step 1.9
const TDesC& CSocketTest1_9::GetTestName()
	{
	_LIT(ret,"Test1.9");
	return ret;
	}


enum TVerdict CSocketTest1_9::InternalDoTestStepL( void )
	{

	// Create socket, shutdown socket then connect
	TVerdict verdict = EPass;
	Logger().WriteFormat(_L("Test Purpose: Connect Socket, then Shutdown"));

	// Connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss);

	// Create and open RConnection
	Logger().WriteFormat(_L("Creating RConnection"));
	RConnection conn;
	ret = conn.Open(ss);
	Logger().WriteFormat(_L("Connection Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(conn);

	// Create and open socket on TCP protocol
	RSocket socket1;


	TProtocolDesc protoInfo;

	TUint numProtocols;
	ret = ss.NumProtocols(numProtocols);
	Logger().WriteFormat(_L("NumProtocols returned %S, number %d"),
			&EpocErrorToText(ret), numProtocols);
	TESTL(KErrNone == ret);


	for (TInt index=1;index<=numProtocols;index++)
		{
		ret = ss.GetProtocolInfo(index, protoInfo);
		Logger().WriteFormat(_L("GetProtocolInfo returned %S"), &EpocErrorToText(ret));
		TESTL(KErrNone == ret || KErrCannotFindProtocol == ret);
		Logger().WriteFormat(_L("Protocol Index %d corresponds to %S"), index, &protoInfo.iName);
		if (protoInfo.iName == KDummyProtocol1Name())
			{
			break;
			}
		}



	Logger().WriteFormat(_L("Opening Socket 1"));
	ret = socket1.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol /*KAfInet, KSockStream, KProtocolInetTcp, conn*/);
	Logger().WriteFormat(_L("Socket 1 Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(socket1);

	// Create Ports for Socket and socket to connect to
	const TInt KSocket1Port = 3441;
	const TInt KSocket2Port = 3881;
	_LIT(KLoopback, "127.0.0.1");
	_LIT(KLocal, "0.0.0.0");

	// Set the local address of Socket1
	TInetAddr socket1Addr;
	socket1Addr.SetPort(KSocket1Port);
	socket1Addr.Input(KLocal);
	ret = socket1.Bind(socket1Addr);

	// Set remote address for socket
	TInetAddr socket2Addr;
	socket2Addr.SetPort(KSocket2Port);
	socket2Addr.Input(KLoopback);

	// Shutdown the Socket
	Logger().WriteFormat(_L("Shutting down Connection on Socket1"));
	TRequestStatus shutdownStatus;
	_LIT8(desOut, "Some test stuff to send to protocol");

	TBuf8<50> desIn;

	socket1.Shutdown(RSocket::EImmediate,desOut,desIn, shutdownStatus);
	User::WaitForRequest(shutdownStatus);
	Logger().WriteFormat(_L("Socket 1 shutdown returned %S"), &EpocErrorToText(shutdownStatus.Int()));

	// Connect socket1 to remote address
	TRequestStatus connectSocket1Status;
	Logger().WriteFormat(_L("Connecting Socket1 "));

	socket1.Connect(socket2Addr, connectSocket1Status);   // used to cause server to panic
	User::WaitForRequest(connectSocket1Status);

	if(connectSocket1Status != KErrBadHandle)
		{
		verdict = EFail;
		}

	CleanupStack::PopAndDestroy(); //socket
	CleanupStack::PopAndDestroy(); //conn
	CleanupStack::PopAndDestroy(); //ss

	return verdict;


	}


// Test step 1.10
const TDesC& CSocketTest1_10::GetTestName()
    {
    _LIT(ret,"Test1.10");
    return ret;
    }
    
    
enum TVerdict CSocketTest1_10::InternalDoTestStepL( void )
    {
    // Create a KAfInet socket specifying KUndefinedProtocol and then close it.

    TVerdict verdict = EPass;
    Logger().WriteFormat(_L("Test Purpose: Open a socket specifying protocol as KUndefinedProtocol"));

    // Connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    TInt ret = ss.Connect();
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    CleanupClosePushL(ss); 

    // Create and open socket (this opens a TCP socket, but we are only interested in seeing that KUndefinedProtocol works correctly). 
    RSocket socket;
    
    Logger().WriteFormat(_L("Opening KUndefinedProtocol Socket"));
    ret = socket.Open(ss, KAfInet, KSockStream, KUndefinedProtocol);
    Logger().WriteFormat(_L("Socket Open returned %S"), &EpocErrorToText(ret)); 
    TESTL(KErrNone == ret);
    CleanupClosePushL(socket); 
    
    CleanupStack::PopAndDestroy(); //socket
    CleanupStack::PopAndDestroy(); //ss

    SetTestStepResult(verdict);
    return verdict;
    }



