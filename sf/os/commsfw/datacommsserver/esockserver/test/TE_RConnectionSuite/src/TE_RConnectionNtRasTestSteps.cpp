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
// Contains the implementation of the test cases that predominately use NT RAS
// as the default interface. These test the use of TCP sockets and host resolvers.
// 
//

/**
 @file
*/

#include "TE_RConnectionNtRasTestSteps.h"
#include <in_iface.h>
#include <nifman.h>
#include <simtsy.h>
#include <dns_qry.h>

// From networking/inhook6/inc/dnd_err.h
const TInt KErrDndNameNotFound = -5120;

TE_RConnectionTest101::~TE_RConnectionTest101()
{
}

// Implicitly create a single connection using Connect()
enum TVerdict TE_RConnectionTest101::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr); // this will use Connect()
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest101


TE_RConnectionTest102::~TE_RConnectionTest102()
{
}

// Explicitly associate a TCP socket to an existing connection
enum TVerdict TE_RConnectionTest102::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest102

TE_RConnectionTest103::~TE_RConnectionTest103()
{
}

enum TVerdict TE_RConnectionTest103::doTestStepL(void)
/*
 * Ensure that initially chosen interface with implicit connection creation
 * matches that chosen for later reuses (using TCP sockets)
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	// temporarily create a connection to see how many interfaces there are
	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(1 == numOfConnections, numOfConnections);
	
	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest103


TE_RConnectionTest104::~TE_RConnectionTest104()
{
}

enum TVerdict TE_RConnectionTest104::doTestStepL(void)
/*
 * Create multiple TCP connections simultaneously without waiting for them to connect
 */
{
	TInt err;

	RSocketServ ss;
	RSocket socks[4];
	RConnection conn;
	TUint numOfConnections = 0;

	TInt i;

	/*
	 * Set up the basics we need before starting
	 */

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Open the TCP sockets
	 */

	for (i=0;i<4;i++)
	{
		err = OpenTcpSocket(socks[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(socks[i]);
	}

	/*
	 * Now connect them all at once using this all encompassing helper function
	 */

	err = ConnectFourTcpSockets(socks[0], socks[1], socks[2], socks[3], iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Test the data paths
	 */

	for (i=0;i<4;i++)
	{
		err = TestTcpDataPathL(socks[i]);
		TESTEL(KErrNone == err, err);
	}

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(conn);
	CleanupStack::Pop();

	/*
	 * Tidy up in reverse order
	 */

	for (i=3; i>=0; i--)
	{
		err = DestroyTcpSocket(socks[i]);
		TESTEL(KErrNone == err, err);
		CleanupStack::Pop();
	}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest104

TE_RConnectionTest105::~TE_RConnectionTest105()
{
}

enum TVerdict TE_RConnectionTest105::doTestStepL(void)
/*
 * Send data over a new connection oriented socket (not associated with any existing connection)
 * when an existing connection using non-default parameter values already exists within the same
 * socket server.
 * Even though this is a tcp test it should be run with the db which has the dummy nif as the 
 * default so that any use of the default if will fail to connect.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest105

TE_RConnectionTest106::~TE_RConnectionTest106()
{
}

enum TVerdict TE_RConnectionTest106::doTestStepL(void)
/*
 * Send data over a new connection oriented socket in a new server session when an existing connection exists in 
 * another server session
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1;
	RSocket sock2;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// deliberately using a NIF which doesn't support TCP connections
	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenTcpSocket(sock2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock2);
	TESTEL(KErrNone == err, err);

	err = DestroyTcpSocket(sock2);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();	

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest106

TE_RConnectionTest107::~TE_RConnectionTest107()
{
}

enum TVerdict TE_RConnectionTest107::doTestStepL(void)
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RSocket sock2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenTcpSocketExplicit(sock2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);
	
	err = TestTcpDataPathL(sock2);
	TESTEL(KErrNone == err, err);

	err = DestroyTcpSocket(sock2);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest107

TE_RConnectionTest108::~TE_RConnectionTest108()
{
}

enum TVerdict TE_RConnectionTest108::doTestStepL(void)
/*
 * Create two connections (non-default first) then Connect() a TCP socket implicitly associating 
 * to a connection
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	// deliberately pick an IAP which doesn't support TCP
	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest108


TE_RConnectionTest109::~TE_RConnectionTest109()
{
}

enum TVerdict TE_RConnectionTest109::doTestStepL(void)
/*
 * Test failure of the interface to start
 */
{	
	TInt err;

	RSocketServ ss;
	RConnection conn;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iBadNtRasIap);
	TESTEL(KErrIfAuthenticationFailure == err, err); // should be an error here

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest109

/*
 * Test 110
 * TestTransferSocketL()
 * Transfer socket from in one socket server to another in the same thread
 */

TE_RConnectionTest110::~TE_RConnectionTest110()
{
}

enum TVerdict TE_RConnectionTest110::doTestStepL(void)
{
	TInt err;

	RSocketServ ss1, ss2;
	RSocket sock1, sock2;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	/*
	 * Open TCP socket on session 1 and send data
	 */

	err = OpenTcpSocket(sock1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = ConnectTcpSocket(sock1, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock1);
	TESTEL(KErrNone == err, err);

	/*
	 * Open empty socket on session 2
	 */

	err = OpenNullSocket(sock2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	/*
	 * Transfer socket from session 1 to session 2
	 */


    //-- define the capabilities set for the socket "pulling" process (this one, actually)
    //-- this process must have at least NetworkServices and NetworkControl capabilities
    _LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
    
    //-- enable socket transfer (see lines below) with PlatSec check. Set socket option with receiving process capabilities    
    err = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
    TESTEL(KErrNone == err, err);

    err = TransferSocket(sock1, sock2, ss1);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock2);
	TESTEL(KErrNone == err, err);

	/*
	 * Tidy up
	 */

	err = DestroyTcpSocket(sock2);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	/* can't call the wrapper function here as it will cause a panic (calls shutdown, but 
	   because the connections referred to the same interface will panic) */
	sock1.Close();
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest110

/*
 * Test 111
 * TestKeepInterfaceUpL()
 * Set the socket option to disable the timers and make sure that the interface stays up.
 * Then re-enable the timers and ensure that the interface then goes down.
 */


TE_RConnectionTest111::~TE_RConnectionTest111()
{
}

enum TVerdict TE_RConnectionTest111::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TTimeoutValues timeouts;
	TNifProgressBuf progress;
	TTimeIntervalSeconds timeElapsed;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	// turn off the timers
	// (Should we re-enable timers if this test case leaves before the timers are re-enabled below,
	// perhaps by pushing a TCleanupItem?) 
	conn.SetOpt(KCOLProvider, KConnDisableTimers, 1);

	err = GetTimeoutValues(conn, timeouts);
	TESTEL(KErrNone == err, err);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	TInt maxDelay = timeouts.iLongTimeout * 2;

	for (TInt i=0; i<maxDelay; i++)
	{
		// ensure that we still have a single interface long past when it would normally 
		// be destroyed
		err = EnumerateConnections(conn, numOfConnections);
		TESTEL(KErrNone == err, err);
		TESTEL(1 == numOfConnections, numOfConnections);
		User::After(1000000); // one second
	}

	// check that the nif still works
	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	// turn on the timers
	conn.SetOpt(KCOLProvider, KConnDisableTimers, 0);
	
	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	TRequestStatus status;
	TInt delta;

	ProgressNotification(conn, status, progress, KLinkLayerClosed);

	err = TimeUntilRequestComplete(status, timeElapsed);
	TESTEL(KErrNone == err, err);
	
	delta = timeElapsed.Int() - timeouts.iLongTimeout;
	TESTEL(delta >= -1 && delta <= 1, delta);

	// now use the socket again, should fail
	err = TestTcpDataPathL(sock);
	TESTEL(KErrTimedOut == err, err);

	// calling Destroy() calls Shutdown() resulting in KErrTimedOut
	sock.Close();
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest111



TE_RConnectionTest112::~TE_RConnectionTest112()
{
}

enum TVerdict TE_RConnectionTest112::doTestStepL(void)
/**
 * Test that idle timers takes into account ESock sessions for implicit connections.
 * Tests Long timeout with:
 * - Socket and Host Resolver,
 * - Socket only,
 * - Host Resolver only,
 */
{
// PREQ399 has introduced a behavioral break, where implicit sockets/host resolvers on long timeout.
	return EPass;
}


TE_RConnectionTest113::~TE_RConnectionTest113()
{
}

enum TVerdict TE_RConnectionTest113::doTestStepL(void)
/**
 * Test that idle timers takes into account ESock sessions for implicit connections
 * Tests Short timeout with:
 * - Socket and Host Resolver,
 * - Socket only,
 * - Host Resolver only,
 */
{
	TVerdict verdict;

	verdict = doBaseTestStepL(2, ETrue, ETrue);
	if (verdict != EPass)
		return verdict;

	verdict = doBaseTestStepL(2, ETrue, EFalse);
	if (verdict != EPass)
		return verdict;

	return doBaseTestStepL(2, EFalse, ETrue);
}

TE_RConnectionTestIdle1Base::TE_RConnectionTestIdle1Base() : TE_RConnectionStep()
	{}

enum TVerdict TE_RConnectionTestIdle1Base::doBaseTestStepL(TInt aStep, TBool aTestSocket, TBool aTestHostResolver)
/**
 * Common code for testing that idle timers takes into account ESock sessions for implicit connections
 *
 * @param aStep Test to perform - 1 for Long Timeout, 2 for Short Timeout.
 * @param aTestSocket Whether to include TCP socket in the test
 * @param aTestHostResolver Whether to include Host Resolver in the test.
 */
{
	TInt err;

	TBuf<100> buf;

	if (aStep == 1)
		buf.AppendFormat(_L("Testing Long Timeout with "));
	else
		buf.AppendFormat(_L("Testing Short Timeout with "));

	if (aTestSocket)
		buf.Append(_L("Socket"));

	if (aTestHostResolver)
		{
		if (aTestSocket)
			buf.Append(_L(" and "));
		buf.Append(_L("Host Resolver"));
		}

	INFO_PRINTF1(buf);
	
	RSocketServ ss;
	RSocket sock;
	RHostResolver hr;
	TTimeoutValues timeouts;
	TTimeIntervalSeconds timeElapsed;
	TUint numOfConnections;

	//
	// Test to perform is determined from argument:
	//
	//		aStep == 1 is for testing Long Timeout
	//		aStep == 2 is for testing Short Timeout


	TESTEL(aStep >= 1 && aStep <= 2, aStep);

	// Open Session and RConnection for reading idle timer values and waiting for interface down.
	// Sanity - use a different ESock session to avoid possible interference with the test (even though
	// we are using a Monitor attach).

	RSocketServ ssMon;
	RConnection conn;

	err = OpenSocketServer(ssMon);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ssMon);

	err = OpenConnection(conn, ssMon);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Open ESock session used for main part of test

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Issue name request
	if (aTestHostResolver)
		{
		err = OpenHostResolver(hr, ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(hr);

		err = MakeNameRequest(hr, iTestName); // this will use GetByName()
		TESTEL(KErrNone == err, err);
		}

	if (aTestSocket)
		{
		// Echo data over TCP Socket
		err = OpenTcpSocket(sock, ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(sock);

		// CLeanupStack: ssMon, conn, ss, [hr], sock

		err = ConnectTcpSocket(sock, iEchoServerAddr);
		TESTEL(KErrNone == err, err);

		err = TestTcpDataPathL(sock);
		TESTEL(KErrNone == err, err);
		}

	// Read Idle timer values for interface.

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	TConnectionInfoBuf infoBuf;
	err = GetConnectionInfo(conn, 1, infoBuf);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(conn, infoBuf);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn, timeouts); 
	TESTEL(KErrNone == err, err);

	// Set up to detect when interface comes down
	TRequestStatus status;
	TNifProgressBuf progress;
	ProgressNotification(conn, status, progress, KLinkLayerClosed);

	if (aTestHostResolver)
		{
		DestroyHostResolver(hr);
		CleanupStack::Pop();			// hr
		}

	if (aTestSocket)
		{
		sock.Close();
		CleanupStack::Pop();			// sock
		}

	TInt delta = 1000;			// suitably invalid default value
	TInt deltaError = 0;

	if (aStep == 1)
		{
		// Interface should come down after long timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);
		delta = timeElapsed.Int() - timeouts.iLongTimeout;
		deltaError = 5;
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();			// ss

	if (aStep == 2)
		{
		// Interface should come down after short timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);
		delta = timeElapsed.Int() - timeouts.iShortTimeout;
		deltaError = 4;
		}

	// The reporting of interface down takes longer than idle
	// timeout values, so reflect this in the allowable range.
	// long timeout range up to 5 seconds after timeout,
	// short timeout range up to 4 seconds after timeout
	TESTEL(delta >= -2 && delta <= deltaError, delta);

	CloseConnection(conn);
	CloseSocketServer(ssMon);

	CleanupStack::Pop(2);			// conn, ssMon

	return TestStepResult();
}

TE_RConnectionTest114::~TE_RConnectionTest114()
{
}

enum TVerdict TE_RConnectionTest114::doTestStepL(void)
/**
 * Test that idle timers takes into account ESock sessions for implicit connections
 */
{
// PREQ399 has introduced a behavioral break, where implicit sockets/host resolvers on long timeout.
	return doBaseTestStepL(3);
}


enum TVerdict TE_RConnectionTestIdle2Base::doBaseTestStepL(TInt aStep)
/**
 * Test that idle timers takes into account ESock sessions for implicit connections.
 * Test creates multiple sessions and multiple Sockets and Host Resolvers, and then
 * tests that closing of both sessions results in short timeout, whereas closing
 * of just one does not.
 * @param aStep If 1, test idle timer after closing Sockets/Host Resolvers.  If 2, test idle timer after
 * closing Sockets/Host Resolvers and one of the ESock sessions.  If 3, test idle timer after
 * closing Sockets/Host Resolvers and both ESock sessions.
 */
{
	if (aStep == 3)
		INFO_PRINTF2(_L("Testing Step %d (Short Timeout)"), aStep);
	else
		INFO_PRINTF2(_L("Testing Step %d (Long Timeout)"), aStep);

	TInt err;

	TTimeoutValues timeouts;
	TTimeIntervalSeconds timeElapsed;
	TUint numOfConnections;

	//
	// Test to perform is determined from argument:
	//
	//		aStep == 1 and aStep == 2 are for testing Long Timeout
	//		aStep == 3 is for testing Short Timeout

	TESTEL(aStep >= 1 && aStep <= 3, aStep);

	const TInt KNumSessions = 2;
	RSocketServ ss[KNumSessions];
	RSocket sock[KNumSessions];
	RHostResolver hr[KNumSessions];

	// Open Session and RConnection for reading idle timer values and waiting for interface down.
	// Sanity - use a different ESock session to avoid possible interference with the test (even though
	// we are using a Monitor attach).

	RSocketServ ssMon;
	RConnection conn;

	err = OpenSocketServer(ssMon);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ssMon);

	err = OpenConnection(conn, ssMon);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	TInt i;

	for (i = 0 ; i < KNumSessions ; i++)
		{
		// open ESOCK session, Host Resolver and Socket
		err = OpenSocketServer(ss[i]);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(ss[i]);
		}

	for (i = 0 ; i < KNumSessions ; i++)
		{
		err = OpenHostResolver(hr[i], ss[i]);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(hr[i]);

		err = OpenTcpSocket(sock[i], ss[i]);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(sock[i]);

		}

	// CleanupStack: ssMon, conn, ss[0], ss[1], hr[0], sock[0], hr[1], sock[1]

	// Issue name request on the Host Resolvers
	for (i = 0 ; i < KNumSessions ; i++)
		{
		err = MakeNameRequest(hr[i], iTestName); // this will use GetByName()
		TESTEL(KErrNone == err, err);
		}

	// Read Idle timer values for interface.  Can only do this once the interface is up.

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	TConnectionInfoBuf infoBuf;
	err = GetConnectionInfo(conn, 1, infoBuf);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(conn, infoBuf);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn, timeouts); 
	TESTEL(KErrNone == err, err);

	// Set up to detect when interface comes down
	TRequestStatus status;
	TNifProgressBuf progress;
	ProgressNotification(conn, status, progress, KLinkLayerClosed);

	// Continue with test
	for (i = 0 ; i < KNumSessions ; i++)
		{
		// Echo data over TCP Socket
		err = ConnectTcpSocket(sock[i], iEchoServerAddr);
		TESTEL(KErrNone == err, err);
		}

	for (i = 0 ; i < KNumSessions ; i++)
		{
		err = TestTcpDataPathL(sock[i]);
		TESTEL(KErrNone == err, err);
		}


	for (i = KNumSessions-1 ; i >= 0 ; i--)
		{
		// calling Destroy() calls Shutdown() resulting in KErrTimedOut
		sock[i].Close();
		CleanupStack::Pop();			// sock

		DestroyHostResolver(hr[i]);
		CleanupStack::Pop();			// hr
		}
	
	// CleanupStack: ssMon, conn, ss[0], ss[1]

	TInt delta = 0;
	TInt deltaError = 0;

	if (aStep == 1)
		{
		// Interface should come down after long timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);
		delta = timeElapsed.Int() - timeouts.iLongTimeout;
		deltaError = 5;
		}

	CloseSocketServer(ss[1]);
	CleanupStack::Pop();			// ss[1]

	if (aStep == 2)
		{
		// Interface should come down after long timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);
		delta = timeElapsed.Int() - timeouts.iLongTimeout;
		deltaError = 5;
		}

	CloseSocketServer(ss[0]);
	CleanupStack::Pop();			// ss[0]

	if (aStep == 3)
		{
		// Interface should come down after short timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);
		delta = Abs(timeElapsed.Int() - timeouts.iShortTimeout);
		deltaError = 4;
		}

	// The reporting of interface down longer than idle
	// timeout values for Long timeout and Short timeout.  This is reflected
	// in the allowable range.  The Long timeout delay seems to be due to the
	// TCP FIN/ACK sequence resetting the timer after it has run for 1 or 2 seconds.
	TESTEL(delta <= deltaError, delta);

	CloseConnection(conn);
	CloseSocketServer(ssMon);
	CleanupStack::Pop(2);			// ssMon, conn

	return TestStepResult();
}

TE_RConnectionTest115::~TE_RConnectionTest115()
{
}


enum TVerdict TE_RConnectionTest115::doTestStepL(void)
/**
 * Test Medium and Short idle timers with explicit Host Resolver.
 */
{

	TInt step;
	TVerdict verdict = EInconclusive;

	for (step = 1 ; step <= 3 ; step++)
		{
		INFO_PRINTF2(_L("Testing Step %d with Host Resolver"), step);
		verdict = doBaseTestStepL(step, EFalse, ETrue);
		if (verdict != EPass)
			return verdict;
		}

	return verdict;
}


TE_RConnectionTest116::~TE_RConnectionTest116()
{
}


enum TVerdict TE_RConnectionTest116::doTestStepL(void)
/**
 * Test Long, Medium and Short idle timers with explicit TCP Socket.
 */
{

	TInt step;
	TVerdict verdict = EInconclusive;

	for (step = 1 ; step <= 3 ; step++)
		{
		INFO_PRINTF2(_L("Testing Step %d with Socket"), step);
		verdict = doBaseTestStepL(step, ETrue, EFalse);
		if (verdict != EPass)
			return verdict;
		}


	return verdict;
}


TE_RConnectionSnapNotSupported::~TE_RConnectionSnapNotSupported()
{
}


enum TVerdict TE_RConnectionSnapNotSupported::doTestStepL(void)
/* Previously this test step tested the case when the database is in legaycy mode and the connection
 * is started with a SNAP preference (a tagId of a Network level AP). In this case Esock returned a
 * KErrNotSupported errorCode to indicate the failure.
 * 
 * This is not the case anymore with the new commsdat because the database cannot be in legacy mode.
 * It's always converted into 399 mode with the help of the meshpreface file. So from now on we have
 * 2 possibilities in this case (when somebody starts a connection with SANP preferences):
 * 1.) It's a valid preference and the connection will be started successfully
 * 2.) It's not a valid preference and either CommsDat will leave or Esock will leave.
 * 		Esock leave happens when the SNAP preference seems to be valid (let's say it's a recordid of a Link
 * 		level AP). CommsDat can read it successfully however when processing further the data at one
 * 		point Esock will check the Tier value of the AccessPoint read from the DB. If it's not a Network
 * 		Tier Esock leaves with KErrArgument.
 * 
 * I have modified the test step to use a non-valid SNAP preference (it is a recordID of a link level AP). 
 * So the test step should fail with KErrArgument.
 */
{
	TCommSnapPref pref(23); //this is the recordid of a Link level AP.

	RSocketServ ss;
	RConnection conn;

	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	TRequestStatus status;
	conn.Start(pref,status);
	User::WaitForRequest(status);
	TESTEL(KErrArgument == status.Int(), status.Int());

	CloseConnection(conn);
	CloseSocketServer(ss);
	CleanupStack::Pop(2);			// ss, conn
	return EPass;
}


TE_RConnectionTestIdle3Base::TE_RConnectionTestIdle3Base() : TE_RConnectionStep()
	{}

enum TVerdict TE_RConnectionTestIdle3Base::doBaseTestStepL(TInt aStep, TBool aTestSocket, TBool aTestHostResolver)
/**
 * Test idle timers with Explicit Host Resolver and/or Socket.
 *
 * @param aStep If 1, test idle timer before closing Host Resolver/Socket.  If 2, test idle timer
 * after closing Host Resolver/Socket.  If 3, test idle timer after closing Host Resolver/Socket/RConnection.
 * @param aTestSocket Whether to include Socket in testing.
 * @param aTestHostResolver Whether to include Host Resolver in testing.
 *
 */
	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	RHostResolver hr;
	TTimeoutValues timeouts;
	TTimeIntervalSeconds timeElapsed;
	TUint numOfConnections;

	// Open Session and RConnection for reading idle timer values and waiting for interface down.
	// Sanity - use a different ESock session to avoid possible interference with the test (even though
	// we are using a Monitor attach).

	RSocketServ ssMon;
	RConnection connMon;

	err = OpenSocketServer(ssMon);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ssMon);

	err = OpenConnection(connMon, ssMon);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connMon);

	// Open ESock session used for main part of test

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// Read Idle timer values for interface.

	err = EnumerateConnections(connMon, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	TConnectionInfoBuf infoBuf;
	err = GetConnectionInfo(connMon, 1, infoBuf);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(connMon, infoBuf);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(connMon, timeouts); 
	TESTEL(KErrNone == err, err);

	// Set up to detect when interface comes down
	TRequestStatus status;
	TNifProgressBuf progress;
	ProgressNotification(connMon, status, progress, KLinkLayerClosed);

	// Issue name request
	if (aTestHostResolver)
		{
		err = OpenHostResolverExplicit(hr, ss, conn);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(hr);

		//clear the DNS Cache before calling GetByName()
   		TBuf8<64> hostName;
   		hostName.Copy(iTestName);
   		TDnsQuery query(hostName, KDnsQTypeCacheClear);
        TPckgC<TDnsQuery> querybuf(query);
    	TBuf8<512> result;
	  	TRequestStatus status;
        result.SetLength(result.MaxLength());
        hr.Query(querybuf, result,status);
        User::WaitForRequest(status);
        err = status.Int();
        TESTEL(KErrNone == err, err);

		err = MakeNameRequest(hr, iTestName); // this will use GetByName()
		TESTEL(KErrNone == err, err);
		}

	if (aTestSocket)
		{
		// Echo data over TCP Socket
		err = OpenTcpSocketExplicit(sock, ss, conn);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(sock);


		err = ConnectTcpSocket(sock, iEchoServerAddr);
		TESTEL(KErrNone == err, err);

		err = TestTcpDataPathL(sock);
		TESTEL(KErrNone == err, err);
		}

	// CleanupStack: ssMon, connMon, ss, conn, [hr], [sock]

	TInt delta = 1000;			// suitably invalid default value
	TInt deltaError = 0;

	if (aStep == 1)
		{
		// Interface should come down after medium timeout if using Host Resolver and
		// long timeout if using Socket.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);

		if (aTestSocket)
			{
			delta = timeElapsed.Int() - timeouts.iLongTimeout;
			deltaError = 5;
			}
		else
			{
			delta = timeElapsed.Int() - timeouts.iMediumTimeout;
			deltaError = 4;
			}
		}

	if (aTestSocket)
		{
		// calling Destroy() calls Shutdown() resulting in KErrTimedOut
		sock.Close();
		CleanupStack::Pop();			// sock
		}

	if (aTestHostResolver)
		{
		DestroyHostResolver(hr);
		CleanupStack::Pop();			// hr
		}


	if (aStep == 2)
		{
		// Interface should come down after medium timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);		
		TESTEL(KErrNone == err, err);
		delta = timeElapsed.Int() - timeouts.iMediumTimeout;
		deltaError = 4;
		}

	CloseConnection(conn);
	CleanupStack::Pop();			// conn

	if (aStep == 3)
		{
		// Interface should come down after short timeout.
		err = TimeUntilRequestComplete(status, timeElapsed);
		TESTEL(KErrNone == err, err);
		delta = timeElapsed.Int() - timeouts.iShortTimeout;
		deltaError = 4;
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();			// ss

	// The reporting of interface down seems to take up to 2 seconds longer than idle
	// timeout values, so reflect this in the allowable range.
	TESTEL(delta >= -1 && delta <= deltaError, delta);	

	CloseConnection(connMon);
	CloseSocketServer(ssMon);

	CleanupStack::Pop(2);			// connMon, ssMon

	return TestStepResult();
}



/*
 * HOST RESOLVER DOMINATED TESTS..........
 */

TE_RConnectionTest201::~TE_RConnectionTest201()
{
}

// Implicitly create a single connection using GetByName()
enum TVerdict TE_RConnectionTest201::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName); // this will use GetByName()
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest201


TE_RConnectionTest202::~TE_RConnectionTest202()
{
}

// Implicitly create a single connection using GetByAddress()
enum TVerdict TE_RConnectionTest202::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	// find out the name of some machine using GetByAddress()
	err = MakeAddressRequest(hr, iLookupAddress);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest202


TE_RConnectionTest203::~TE_RConnectionTest203()
{
}

enum TVerdict TE_RConnectionTest203::doTestStepL(void)
/*
 * Implicitly create a single connection using GetByName() - use the synchronous version 
 * this time
 */
{
	TInt err;

	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeSynchronousNameRequest(hr, iTestName); // this will use GetByName()
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest203


TE_RConnectionTest204::~TE_RConnectionTest204()
{
}

enum TVerdict TE_RConnectionTest204::doTestStepL(void)
/*
 * Implicitly create a single connection using GetByName() - use the synchronous version 
 * this time
 */
{
	TInt err;

	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	// find out the name of some machine using GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest204



TE_RConnectionTest205::~TE_RConnectionTest205()
{
}

// Explcitly associate a host resolver with an existing connection
enum TVerdict TE_RConnectionTest205::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolverExplicit(hr, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest205


TE_RConnectionTest206::~TE_RConnectionTest206()
{
}

// Create two connections (separate interfaces) and explcitly associate a separate host resolver 
// with each, checking that requests on each go over the correct interface each time.
enum TVerdict TE_RConnectionTest206::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RHostResolver hr1, hr2;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Set up the connections, both overridden as host resolver won't work 
	// on the default interface

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = StartConnectionWithOverrides(conn1, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnectionWithOverrides(conn2, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	// Set up the host resolvers

	err = OpenHostResolverExplicit(hr1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr1);

	err = OpenHostResolverExplicit(hr2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	// Test the interfaces. Conn1 should be fine (host resolution should work ok)
	// Conn2 should time out (black hole for all traffic again)

	err = MakeNameRequest(hr1, iTestName);
	//INFO_PRINTF1(_L("#1 TE_RConnectionTest206, err=%d"),err);	
	TESTEL(KErrNone == err, err);

    const TInt KNRTrials = 3;
    for(TInt j=0; j<KNRTrials; j++)
    {   //-- sometimes this request can generate KErrTimedOut before KErrDndNameNotFound.
        //-- nothing wrong, actually, this is timeout.
	err = MakeNameRequest(hr2, iTestName);
	    //INFO_PRINTF1(_L("#2 TE_RConnectionTest206, err=%d"),err);	
	    if(KErrDndNameNotFound == err)
	        break;
    }

	TESTEL(KErrDndNameNotFound == err, err);
   

	// Clean up - get rid of the host resolver and connections

	DestroyHostResolver(hr2);
	CleanupStack::Pop();

	DestroyHostResolver(hr1);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();
	
	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest206


TE_RConnectionTest207::~TE_RConnectionTest207()
{
}

enum TVerdict TE_RConnectionTest207::doTestStepL()
/*
 * Ensure that initial (implicit) connection matches that chosen for later uses with host resolvers
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RHostResolver hr;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(1 == numOfConnections, numOfConnections);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(1 == numOfConnections, numOfConnections);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest207


TE_RConnectionTest208::~TE_RConnectionTest208()
{
}

enum TVerdict TE_RConnectionTest208::doTestStepL(void)
/*
 * Test the choice of interface when two non-default interfaces already exist
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock;
	RHostResolver hr;
	TCommDbConnPref pref1, pref2;
	TRequestStatus status1, status2;

	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	StartConnectionWithOverridesAsynchronous(conn2, pref2, iHungryNifLongTimeoutIap, status2);
	StartConnectionWithOverridesAsynchronous(conn1, pref1, iDummyNifLongTimeoutIap, status1);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	err = status1.Int();
	TESTEL(KErrNone == err, err);
	err = status2.Int();
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

    //-- disable connection timers, because they can interfere in the case of delays with name resolution
    conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);


	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

    err = EnumerateConnections(conn1, numOfConnections);
    //INFO_PRINTF1(_L("#a TE_RConnectionTest208, err=%d, nConn=%d "),err, numOfConnections);	


	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

    err = EnumerateConnections(conn1, numOfConnections);
    //INFO_PRINTF1(_L("#b TE_RConnectionTest208, err=%d, nConn=%d "),err, numOfConnections);	

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);

    //INFO_PRINTF1(_L("#1 TE_RConnectionTest208, err=%d, nConn=%d "),err, numOfConnections);	

	TESTEL(KErrNone == err, err);
	TESTEL(3 == numOfConnections, numOfConnections);

    //-- enable connection timers
    conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);


	DestroyHostResolver(hr);
	CleanupStack::Pop();

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest208


TE_RConnectionTest209::~TE_RConnectionTest209()
{
}

enum TVerdict TE_RConnectionTest209::doTestStepL(void)
/*
 * Two host resolvers using the same interface but from different socket servers,
 * explicitly created connections
 */
{	
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RHostResolver hr1, hr2;
	TUint numOfConnections = 0;

	/*
	 * Sort out the first set of connection and host resolver
	 */

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolverExplicit(hr1, ss1, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr1);

	/*
	 * Sort out the second set of connection and host resolver
	 */

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolverExplicit(hr2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	/*
	 * Try making name requests over the two host resolvers
	 */

    //-- disable connection timers, because they can interfere in the case of delays with name resolution
    conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);


	err = MakeNameRequest(hr1, iTestName);
	TESTEL(KErrNone == err, err);

	err = MakeNameRequest(hr2, iTestName);
	TESTEL(KErrNone == err, err);

    //-- enable connection timers.
    conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);
	
	/* 
	 * Tidy up in reverse order
	 */

	DestroyHostResolver(hr2);
	CleanupStack::Pop();
	
	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	DestroyHostResolver(hr1);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnection209


TE_RConnectionTest210::~TE_RConnectionTest210()
{
}

enum TVerdict TE_RConnectionTest210::doTestStepL(void)
/*
 * Two host resolvers using different interfaces from different socket servers,
 * explicitly created connections
 */
	{	
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RHostResolver hr1, hr2;
	TCommDbConnPref pref1, pref2;
	TRequestStatus status1, status2;
	TUint numOfConnections = 0;

	/*
	 * We go to some lengths in this test to ensure that while one interface
	 * is being started up, the other one does not time out in the meantime.
	 * Hence:
	 * - create the RSocketServ and RConnection instances in advance,
	 * - start the two interface asynchronously in parallel.
	 *
	 * This test used to start the NT RAS interface synchronously followed by the
	 * Hungry interface synchronously, but this didn't always work.
	 */

	/*
	 * Create the Socket Server Sessions and RConnection instances.
	 */

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	/*
	 * Start up the interfaces
	 */
	StartConnectionWithOverridesAsynchronous(conn1, pref1, iDefaultIap, status1);
	User::WaitForRequest(status1);
	TESTEL(status1.Int() == err, err);

	err = OpenHostResolverExplicit(hr1, ss1, conn1);
	TESTEL(KErrNone == err, err);
	
	StartConnectionWithOverridesAsynchronous(conn2, pref2, iHungryNifLongTimeoutIap, status2);
	User::WaitForRequest(status2);
	TESTEL(status2.Int() == err, err);
	CleanupClosePushL(conn2);
	
	err = OpenHostResolverExplicit(hr2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	/*
	 * Try making requests using the two host resolvers
	 */

	err = MakeNameRequest(hr1, iTestName);			// NT RAS - should be okay
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	err = MakeNameRequest(hr2, iTestName);			// Hungry - should fail
	TESTEL(KErrDndNameNotFound == err, err);

	/*
	 * Tidy up in reverse order
	 */
	CleanupStack::PopAndDestroy(6);

	return TestStepResult();
	} // TE_RConnectionTest210



TE_RConnectionTest211::~TE_RConnectionTest211()
{
}

enum TVerdict TE_RConnectionTest211::doTestStepL(void)
/*
 * Two host resolvers using same interface from different socket servers,
 * implicitly created connections.
 * This test requires loading a new version of comm db that will have an 
 * interface over which DNS will work as the default interface.
 */
{	
	TInt err;

	RSocketServ ss1, ss2;
	RHostResolver hr1, hr2;
	RConnection conn;
	TUint numOfConnections = 0;

	/*
	 * Sort out the first set of socket server and host resolver
	 */

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenHostResolver(hr1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr1);

	/*
	 * Sort out the second set of socket server and host resolver
	 */

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenHostResolver(hr2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	/*
	 * Make name requests using the two host resolvers (thus implicitly associating 
	 * them with connections).
	 */

	err = MakeNameRequest(hr1, iTestName);
	TESTEL(KErrNone == err, err);

	err = MakeNameRequest(hr2, iTestName);
	TESTEL(KErrNone == err, err);

	/*
	 * See how many interfaces there are, need to open a connection object to do this
	 */

	err = OpenConnection(conn, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(conn);
	CleanupStack::Pop();

	/*
	 * Tidy up in reverse order
	 */

	DestroyHostResolver(hr2);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	DestroyHostResolver(hr1);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest211

TE_RConnectionTest212::~TE_RConnectionTest212()
{
}

enum TVerdict TE_RConnectionTest212::doTestStepL(void)
/*
 * Ensure that Close() does not pull down the interface when there are host resolvers associated with it
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RConnection countingConn;
	RHostResolver hr;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

    //-- disable connection timers, because they can interfere in the case of delays with name resolution
    conn.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	// Use GetByName() to associate it to the connection...
	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

    //-- enable connection timers.
    conn.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);

	CloseConnection(conn);
	/*
	 * conn isn't at top of cleanup stack hence pop 2 then put hr back on
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(hr);

	User::After(iMediumTimeout); // wait how long you would wait for the connection to go if there was an RConnection

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest212


TE_RConnectionTest213::~TE_RConnectionTest213()
{
}

enum TVerdict TE_RConnectionTest213::doTestStepL(void)
/*
 * Ensure that Stop() pulls down the interface when there are host resolvers associated 
 * with it
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn, countingConn;
	RHostResolver hr;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolverExplicit(hr, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn);
	/*
	 * conn is not at the top of the cleanup stack, hence pop 2 then add hr again
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(hr);

	User::After(iShortTimeout);

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(0 == numOfConnections, numOfConnections);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest213

TE_RConnectionTest214::~TE_RConnectionTest214()
{
}

enum TVerdict TE_RConnectionTest214::doTestStepL(void)
/*
 * Ask for name resolution from a new host resolver (not associated with any connection) when an existing 
 * connection using non-default parameter values already exists within the same socket server
 * Although this is a host resolver test it should be run with the db which has the 
 * dummy nif as the default so that any erroneous use of the default will fail.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RHostResolver hr;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest214


TE_RConnectionTest215::~TE_RConnectionTest215()
{
}

enum TVerdict TE_RConnectionTest215::doTestStepL(void)
/*
 * Ask for name resolution from a new host resolver in a new socket session when a connection already 
 * exists in another socket session
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1;
	RHostResolver hr2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);
	
	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// deliberately use an interface that doesn't support host resolution
	err = StartConnectionWithOverrides(conn1, iHungryNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolver(hr2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	err = MakeNameRequest(hr2, iTestName);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	DestroyHostResolver(hr2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTes215


TE_RConnectionTest216::~TE_RConnectionTest216()
{
}

enum TVerdict TE_RConnectionTest216::doTestStepL(void)
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RHostResolver hr2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenHostResolverExplicit(hr2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	err = MakeNameRequest(hr2, iTestName);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr2);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest216


TE_RConnectionTest217::~TE_RConnectionTest217()
{
}

enum TVerdict TE_RConnectionTest217::doTestStepL(void)
/*
 * Create two connections (non-default first) then ask for a name resolution implicitly associating 
 * to one of the connections
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	// deliberately pick an iap which doesn't support host resolution
	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest217


TE_RConnectionTest218::~TE_RConnectionTest218()
{
}

enum TVerdict TE_RConnectionTest218::doTestStepL(void)
/*
 * Ask for name resolution from a new host resolver within the same socket server as a connection using 
 * overridden value already exists.
 * This test relies on the default interface not supporting host resolution (the correct operation is to 
 * reuse the existing interface, not to create another)
 */
{
	TInt err;
	
	RSocketServ ss;
	RConnection conn;
	RHostResolver hr;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest218

/*
 * Test 219
 * TestNonUniqueDefaultConnectionL()
 * Open two default connections and then implicitly associate a socket and a host reolver to it/them.
 */

TE_RConnectionTest219::~TE_RConnectionTest219()
{
}

enum TVerdict TE_RConnectionTest219::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock;
	RHostResolver hr;
	TUint numOfConnections;

	/*
	 * Open the socket server
	 */

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Create two default interface connections
	 */

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	/*
	 * Implicitly assocaite to them with a TCP socket and a host resolver
	 */

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName);
	TESTEL(KErrNone == err, err);

	/*
	 * Check there is only a single interface up
	 */

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	/*
	 * Tidy up
	 */

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest219

/*
 * Test 220
 * TestBadRConnectionSubSessionOpenL()
 * Open a host resolver associated with a non-started connection. This should fail, then try 
 * again with socket server and opened connection from a different socket server - this should 
 * panic.
 */



TE_RConnectionTest220::~TE_RConnectionTest220()
{
}

enum TVerdict TE_RConnectionTest220::doTestStepL(void)
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RHostResolver hr;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// this should error but not panic (connection not started yet)
	err = OpenHostResolverExplicit(hr, ss1, conn1);
	TESTEL(KErrNotReady == err, err);
	
	// this should panic because the connection is in a different socket server from that supplied
	err = OpenHostResolverExplicit(hr, ss1, conn2);

	/*
	 * Tidy up, although should never reach here
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest220




TE_RConnectionTest221::~TE_RConnectionTest221()
{
}

enum TVerdict TE_RConnectionTest221::doTestStepL(void)
{
	RSocketServ             socketServer;
	RSocket                 socket;
	RHostResolver		reslv;
	TRequestStatus		status;
	RConnection		conn;
	TNameEntry		nameEntry;
	TInt			ret;
	
	ret = socketServer.Connect();
	TESTL(ret == KErrNone);
	CleanupClosePushL(socketServer);

	ret = conn.Open(socketServer, KAfInet);
	TESTEL(ret == KErrNone, ret);
	CleanupClosePushL(conn);
  
	ret = StartConnectionWithOverrides(conn, iStaticDnsDynamicAddr);
	TESTEL(ret == KErrNone, ret);

	ret = reslv.Open(socketServer, KAfInet, KProtocolInetUdp, conn);
	TESTEL(ret == KErrNone, ret);
	CleanupClosePushL(reslv);

	ret = reslv.GetByName(iTestName, nameEntry);
	TESTEL(ret == KErrNone, ret);

  	ret = socket.Open(socketServer, 
	             KAfInet, 
	             KSockStream, 
	             KProtocolInetTcp,
		     conn);
	TESTEL(ret == KErrNone, ret);
	CleanupClosePushL(socket);
	
	ret = socket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
	TESTEL(ret == KErrNone, ret);
	TSoInetInterfaceInfo optData;
	TPckg<TSoInetInterfaceInfo> opt(optData);
	TBool cmp;
	do 
		{
		ret = socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, opt);	
		TESTEL(ret == KErrNone, ret);
		if (optData.iName.Length() > iInterfaceName.Length())
			{
			optData.iName.SetLength(iInterfaceName.Length());	
			}
		cmp = optData.iName.Compare(iInterfaceName) == 0;
		} while (!cmp); //No match

	TInetAddr& ns_addr = optData.iNameSer1;
	TESTL(ns_addr.Match(iNameServerAddr));

	TInetAddr &addr = static_cast<TInetAddr&>(nameEntry().iAddr);
	addr.SetPort(80);
	socket.Connect(addr, status);
	User::WaitForRequest(status);
	TESTEL(status == KErrNone, status.Int());
	RBuf8 buf;
	buf.CreateL(1024);
	CleanupClosePushL(buf);
	buf.Copy(_L8("GET / HTTP/1.1\r\nHost: "));
	buf.Append(iTestName);
	buf.Append(_L8("\r\n\r\n"));
	socket.Send(buf,0, status);
	User::WaitForRequest(status);
	buf.Close();
	TESTEL(status == KErrNone, status.Int());
	buf.CreateL(1024);

	TSockXfrLength len;
	socket.RecvOneOrMore(buf,0,status, len);
	User::WaitForRequest(status);
	TESTEL(status == KErrNone, status.Int());

	CleanupStack::Pop(&buf);
	buf.Close();
	CleanupStack::Pop(&socket);
	socket.Close();
	CleanupStack::Pop(&reslv);
	reslv.Close();
	CleanupStack::Pop(&conn);
	conn.Close();
	CleanupStack::Pop(&socketServer);
	socketServer.Close();
	return EPass;
}
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT

TE_RConnectionTest222::~TE_RConnectionTest222()
	{
	
	}

enum TVerdict TE_RConnectionTest222::doTestStepL()
	{
	StartNTRasSimulation();
	
	RSocketServ ss;
	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	RConnection conn;
	OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	const TInt simTsyDialupIapId = 5;
	const TInt KOneSecond = 1000000;	
	TRequestStatus status;	
	TCommDbConnPref pref;
	
	pref.SetIapId(simTsyDialupIapId);
	pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	conn.Start(pref, status);
	
	User::After(KOneSecond*10);
	TInt state = 0;
	err = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyRemoteHangup, state);
	TESTEL(KErrNone == err, err);
	User::WaitForRequest(status);	
	TESTEL(KErrDisconnected == status.Int(), status.Int());	
	
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);
	
	return EPass;
	}

TE_RConnectionTest223::~TE_RConnectionTest223()
	{
	
	}

enum TVerdict TE_RConnectionTest223::doTestStepL()
	{	
	RSocketServ ss;
	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	RConnection conn;
	OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	const TInt csdDialupIapId = 2;
	err = StartConnectionWithOverrides(conn, csdDialupIapId);
	TESTEL(KErrNone == err, err);	
	
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);
	
	return EPass;
	}

TE_RConnectionTest224::~TE_RConnectionTest224()
	{
	
	}

enum TVerdict TE_RConnectionTest224::doTestStepL()
	{	
	RSocketServ ss;
	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	RConnection conn;
	OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	const TInt csdDialupNoScriptIapId = 3;
	err = StartConnectionWithOverrides(conn, csdDialupNoScriptIapId);
	TESTEL(KErrNone == err, err);	
	
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);
	
	return EPass;
	}


#endif // SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT

