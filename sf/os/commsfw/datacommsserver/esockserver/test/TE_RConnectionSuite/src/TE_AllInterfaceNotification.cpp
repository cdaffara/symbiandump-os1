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
// TS_AllInterfaceNotification.cpp
// Contains the implementation of test cases testing the all interface notification 
// functionality of RConnection.
// 
//

/**
 @file
*/

#include "TE_AllInterfaceNotification.h"
#include <ss_std.h>

#define ListOfSuitableIAPs	iHungryNifIap, iDummyNifIap, iDefaultIap, iSecondHungryNifIap, iThirdHungryNifIap, iSecondDummyNifIap, iThirdDummyNifIap, iFourthDummyNifIap, iFifthDummyNifIap

/******************************************************************************
 *
 * Test 300
 *
 *****************************************************************************/

// Constructor

// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest300::doTestStepL(void)
{
	TInt err;

	RConnection activeConn;
	RConnection conn;
	RSocketServ ss;

	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = OpenConnection(activeConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(activeConn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// issue request
	AllInterfaceNotification(conn, status, info);

	// start another different connection to complete request if it hasn't already done so
	err = StartConnectionWithOverrides(activeConn, 1);
	TESTEL(KErrNone == err, err);

	// wait for completion
	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrInUse, status.Int());

	CloseConnection(activeConn);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest300

/******************************************************************************
 *
 * Test 301
 *
 *****************************************************************************/


// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest301::doTestStepL(void)
{
	TInt err;

	RConnection activeConn;
	RConnection conn;
	RSocketServ ss;

	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = OpenConnection(activeConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(activeConn);

	err = StartConnectionWithOverrides(conn, 1);
	TESTEL(KErrNone == err, err);

	// issue request
	AllInterfaceNotification(conn, status, info);

	// start another different connection to complete request if it hasn't already done so
	err = StartConnectionWithOverrides(activeConn, 6);
	TESTEL(KErrNone == err, err);

	// wait for completion
	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrInUse, status.Int());

	CloseConnection(activeConn);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest301

/******************************************************************************
 *
 * Test 302
 *
 *****************************************************************************/


// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest302::doTestStepL(void)
{
	TInt err;

	RConnection activeConn;
	RConnection conn;
	RConnection conn2;
	RSocketServ ss;
	TUint numOfConnections;
	TPckgBuf<TConnectionInfo> connInfo;

	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = OpenConnection(activeConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(activeConn);

	err = StartConnectionWithOverrides(conn, 1);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = conn.GetConnectionInfo(1, connInfo);
	TESTEL(KErrNone == err, err);

	err = AttachNormal(conn2, connInfo);
	TESTEL(KErrNone == err, err);

	// issue request
	AllInterfaceNotification(conn2, status, info);

	// start another different connection to complete request if it hasn't already done so
	err = StartConnectionWithOverrides(activeConn, 6);
	TESTEL(KErrNone == err, err);

	// wait for completion
	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrInUse, status.Int());

	CloseConnection(activeConn);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest302

/******************************************************************************
 *
 * Test 303
 *
 *****************************************************************************/


// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest303::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RConnection monConn;
	RSocketServ ss;
	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = OpenConnection(monConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(monConn);

	// issue request
	AllInterfaceNotification(monConn, status, info);

	// start another different connection to complete request if it hasn't already done so
	err = StartConnectionWithOverrides(conn, 1);
	TESTEL(KErrNone == err, err);

	// wait for completion - to ensure that monitoring actually occurs
	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrNone, status.Int());

	// issue another request (to be cancelled)
	AllInterfaceNotification(monConn, status, info);

	User::After(iShortTimeout);  // wait for a period of time

	// cancel request
	CancelAllInterfaceNotificationL(monConn);

	// wait for request completion
	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrCancel, status.Int());

	CloseConnection(monConn);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest303

/******************************************************************************
 *
 * Test 304
 *
 *****************************************************************************/


// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest304::doTestStepL(void)
{
	TInt err;

	RSocketServ socketServer;
	RConnection connection;
	RConnection monitor;

	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);

	err = OpenConnection(connection, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connection);

	err = OpenConnection(monitor, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(monitor);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	// start connection to complete request
	err = StartConnection(connection);
	TESTEL(KErrNone == err, err);

	CheckInterfaceNotificationL(status, info, EInterfaceUp, &connection);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	CloseConnection(connection);
	CleanupStack::Pop();

	CheckInterfaceNotificationL(status, info, EInterfaceDown);

	CloseConnection(monitor);
	CleanupStack::Pop();

	CloseSocketServer(socketServer);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest304

/******************************************************************************
 *
 * Test 305
 *
 *****************************************************************************/


// creates a connection and destroys it again
// Implicitly detect a single connection using SendTo()
enum TVerdict TE_RConnectionTest305::doTestStepL(void)
{
	TInt err;

	RConnection monitor;
	RSocketServ ss;
	RSocket sock;

	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(monitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(monitor);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr); // this will use SendTo()
	TESTEL(KErrNone == err, err);

	// check for interface up...
	CheckInterfaceNotificationL(status, info, EInterfaceUp);

	AllInterfaceNotification(monitor, status, info);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	// check for interface down...
	CheckInterfaceNotificationL(status, info, EInterfaceDown);

	CloseConnection(monitor);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest305

/******************************************************************************
 *
 * Test 306
 *
 *****************************************************************************/


// creates a connection and destroys it again
// Implicitly detect a single connection using SendTo()
enum TVerdict TE_RConnectionTest306::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RSocket sock;

	RConnection monitor;
	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(monitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(monitor);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	err = ConnectTcpSocket(sock, iEchoServerAddr); // this will use Connect()
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	// check for interface up...
	CheckInterfaceNotificationL(status, info, EInterfaceUp);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	// check for interface down...
	CheckInterfaceNotificationL(status, info, EInterfaceDown);

	CloseConnection(monitor);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest306

/******************************************************************************
 *
 * Test 307
 *
 *****************************************************************************/

// Implicitly create a single connection using GetByName()
enum TVerdict TE_RConnectionTest307::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RHostResolver hr;

	RConnection monitor;
	TRequestStatus status;
	TInterfaceNotificationBuf info;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(monitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(monitor);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	err = MakeNameRequest(hr, iTestName); // this will use GetByName()
	TESTEL(KErrNone == err, err);

	// check for interface up...
	CheckInterfaceNotificationL(status, info, EInterfaceUp);

	// issue request
	AllInterfaceNotification(monitor, status, info);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	// check for interface down...
	CheckInterfaceNotificationL(status, info, EInterfaceDown);

	CloseConnection(monitor);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest307

/******************************************************************************
 *
 * Test 308
 *
 *****************************************************************************/

const TInt KTS308_NoofMonitors = 2;

// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest308::doTestStepL(void)
{
	TInt err;
	TInt i;

	RSocketServ socketServer;
	RConnection connection;

	RConnection monitor[ KTS308_NoofMonitors ];
	TRequestStatus status[ KTS308_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS308_NoofMonitors ];

	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);

	err = OpenConnection(connection, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connection);

	for( i = 0; i < KTS308_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], socketServer);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	// issue requests
	for( i = 0; i < KTS308_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start connection to complete request
	err = StartConnection(connection);
	TESTEL(KErrNone == err, err);

	// check for interface up...
	for( i = 0; i < KTS308_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection);
		}

	// issue requests
	for( i = 0; i < KTS308_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	CloseConnection(connection);
	CleanupStack::Pop();

	// check for interface down...
	for( i = 0; i < KTS308_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown, NULL);
		}

	// close monitors
	for( i = 0; i < KTS308_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(socketServer);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest308

/******************************************************************************
 *
 * Test 309
 *
 *****************************************************************************/


const TInt KTS309_NoofMonitors = 2;

// creates a connection and destroys it again
// Implicitly detect a single connection using Connect()
enum TVerdict TE_RConnectionTest309::doTestStepL(void)
{
	TInt err;
	TInt i;

	RSocketServ ss;
	RSocket sock;

	RConnection monitor[ KTS309_NoofMonitors ];
	TRequestStatus status[ KTS309_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS309_NoofMonitors ];

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS309_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// issue requests
	for( i = 0; i < KTS309_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	err = ConnectTcpSocket(sock, iEchoServerAddr); // this will use Connect()
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	// check for interface up...
	for( i = 0; i < KTS309_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp);
		}

	// issue requests
	for( i = 0; i < KTS309_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	err = DestroyTcpSocket(sock);
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop();

	// check for interface down...
	for( i = 0; i < KTS309_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown);
		}

	// close monitors
	for( i = 0; i < KTS309_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest309

/******************************************************************************
 *
 * Test 310
 *
 *****************************************************************************/


const TInt KTS310_NoofMonitors = 2;

// creates a connection and destroys it again
// Implicitly detect a single connection using SendTo()
enum TVerdict TE_RConnectionTest310::doTestStepL(void)
{
	TInt err;
	TInt i;

	RSocketServ ss;
	RSocket sock;

	RConnection monitor[ KTS310_NoofMonitors ];
	TRequestStatus status[ KTS310_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS310_NoofMonitors ];

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS310_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// issue requests
	for( i = 0; i < KTS310_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	err = TestUdpDataPathL(sock, iDummyNifSendAddr); // this will use SendTo()
	TESTEL(KErrNone == err, err);

	// check for interface up...
	for( i = 0; i < KTS310_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp);
		}

	// issue requests
	for( i = 0; i < KTS310_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	// check for interface down...
	for( i = 0; i < KTS310_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown);
		}

	// close monitors
	for( i = 0; i < KTS310_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest310

/******************************************************************************
 *
 * Test 311
 *
 *****************************************************************************/

const TInt KTS311_NoofMonitors = 2;

// Implicitly create a single connection using GetByName()
enum TVerdict TE_RConnectionTest311::doTestStepL(void)
{
	TInt err;
	TInt i;

	RSocketServ ss;
	RHostResolver hr;

	RConnection monitor[ KTS311_NoofMonitors ];
	TRequestStatus status[ KTS311_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS311_NoofMonitors ];

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS311_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	// issue requests
	for( i = 0; i < KTS311_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	err = MakeNameRequest(hr, iTestName); // this will use GetByName()
	TESTEL(KErrNone == err, err);

	// check for interface up...
	for( i = 0; i < KTS311_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp);
		}

	// issue requests
	for( i = 0; i < KTS311_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	// check for interface down...
	for( i = 0; i < KTS311_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown);
		}

	// close monitors
	for( i = 0; i < KTS311_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest311

/******************************************************************************
 *
 * Test 312
 *
 *****************************************************************************/

const TInt KTS312_NoofMonitors = 1;		// to make code same for multiple monitors
const TInt KTS312_NoofConnections = 2;

// Implicitly create a single connection using GetByName()
enum TVerdict TE_RConnectionTest312::doTestStepL(void)
{
	TInt err;
	TInt i;
	TInt j;		// used for nested loops

	RSocketServ ss;
	RConnection connection[ KTS312_NoofConnections ];
	TInt iapId[] = {ListOfSuitableIAPs};

	RConnection monitor[ KTS312_NoofMonitors ];
	TRequestStatus status[ KTS312_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS312_NoofMonitors ];

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS312_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	for( i = 0; i < KTS312_NoofConnections; i++ )
		{
		err = OpenConnection(connection[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(connection[i]);
		}

	// issue requests
	for( i = 0; i < KTS312_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start several different connections
	for( i = 0; i < KTS312_NoofConnections; i++ )
		{
		// the first of these should complete the initial request,
		// the remainder should fill the cache
		err = StartConnectionWithOverrides(connection[i], iapId[i]);
		TESTEL(KErrNone == err, err);
		}

	for( j = 0; j < KTS312_NoofConnections; j++ )
		{
		for( i = 0; i < KTS312_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection[j]);
			}

		// re-issue requests, which should complete immediately with cached status
		for( i = 0; i < KTS312_NoofMonitors; i++ )
			{
			AllInterfaceNotification(monitor[i], status[i], info[i]);
			}

		} // for j

	// NOTE: request outstanding at this point...

	// close connections
	for( i = 0; i < KTS312_NoofConnections; i++ )
		{
		CloseConnection(connection[i]);
		CleanupStack::Pop();
		}

	for( j = 0; j < KTS312_NoofConnections; j++ )
		{
		// check for interface down...
		for( i = 0; i < KTS312_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown, NULL);
			}

		// reissue requests, except after last check
		if( j != (KTS312_NoofConnections - 1) )
			{
			// re-issue requests, which should complete immediately with cached status
			for( i = 0; i < KTS312_NoofMonitors; i++ )
				{
				AllInterfaceNotification(monitor[i], status[i], info[i]);
				}
			}

		} // for j

	// close monitors
	for( i = 0; i < KTS312_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest312

/******************************************************************************
 *
 * Test 313
 *
 *****************************************************************************/

const TInt KTS313_NoofMonitors = 2;
const TInt KTS313_NoofConnections = 2;

// Test multiple monitors receive notifications from multiple interfaces
enum TVerdict TE_RConnectionTest313::doTestStepL(void)
{
	TInt err;
	TInt i;
	TInt j;		// used for nested loops

	RSocketServ ss;
	RConnection connection[ KTS313_NoofConnections ];
	TInt iapId[] = {ListOfSuitableIAPs};

	RConnection monitor[ KTS313_NoofMonitors ];
	TRequestStatus status[ KTS313_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS313_NoofMonitors ];

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS313_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	for( i = 0; i < KTS313_NoofConnections; i++ )
		{
		err = OpenConnection(connection[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(connection[i]);
		}

	// issue requests
	for( i = 0; i < KTS313_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start several different connections
	for( i = 0; i < KTS313_NoofConnections; i++ )
		{
		// the first of these should complete the initial request,
		// the remainder should fill the cache
		err = StartConnectionWithOverrides(connection[i], iapId[i]);
		TESTEL(KErrNone == err, err);
		}

	for( j = 0; j < KTS313_NoofConnections; j++ )
		{
		for( i = 0; i < KTS313_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection[j]);

			// re-issue requests, which should complete immediately with cached status
			AllInterfaceNotification(monitor[i], status[i], info[i]);
			}

		} // for j

	// NOTE: request outstanding at this point...

	// close connections
	for( i = 0; i < KTS313_NoofConnections; i++ )
		{
		CloseConnection(connection[i]);
		CleanupStack::Pop();
		}

	for( j = 0; j < KTS313_NoofConnections; j++ )
		{
		// check for interface down...
		for( i = 0; i < KTS313_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown, NULL);

			// except after last check...
			if( j != (KTS313_NoofConnections - 1) )
				{
				// ...re-issue request, which should complete immediately with cached status
				AllInterfaceNotification(monitor[i], status[i], info[i]);
				}
			}


		} // for j

	// close monitors
	for( i = 0; i < KTS313_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest313

#undef TS313_NoofMonitors
#undef TS313_NoofConnections

/******************************************************************************
 *
 * Test 314
 *
 * This code will be the same as for test 316, but with just one monitor.
 *
 *****************************************************************************/

const TInt KTS314_NoofMonitors = 1;		// to make code same for multiple monitors
const TInt KTS314_NoofConnections = ((TInt)KMaxInterfaceNotificationQueueLength+1);

// Test multiple monitors receive notifications from multiple interfaces
enum TVerdict TE_RConnectionTest314::doTestStepL(void)
{
	TInt err;
	TInt i;
	TInt j;		// used for nested loops

	RSocketServ ss;
	RConnection connection[ KTS314_NoofConnections ];

	RConnection monitor[ KTS314_NoofMonitors ];
	TRequestStatus status[ KTS314_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS314_NoofMonitors ];
	TInt iapId[] = {ListOfSuitableIAPs};

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS314_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	for( i = 0; i < KTS314_NoofConnections; i++ )
		{
		err = OpenConnection(connection[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(connection[i]);
		}

	// issue requests
	for( i = 0; i < KTS314_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start several different connections
	for( i = 0; i < KTS314_NoofConnections; i++ )
		{
		// the first of these should complete the initial request,
		// the remainder should fill the cache
		err = StartConnectionWithOverrides(connection[i], iapId[i]);
		if(KErrNone != err)
		    {
        	// Swallow outstanding signals
        	for(i=0;i<KTS314_NoofMonitors;i++)
        		{
        		User::WaitForAnyRequest();
        		}
		    User::Leave(err);
		    }
		}

	for( j = 0; j < KTS314_NoofConnections; j++ )
		{
		for( i = 0; i < KTS314_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection[j]);
			// re-issue requests, which should complete immediately with cached status
			AllInterfaceNotification(monitor[i], status[i], info[i]);
			}

		} // for j

	// NOTE: request outstanding at this point...

	// close connections
	for( i = 0; i < KTS314_NoofConnections; i++ )
		{
		CloseConnection(connection[i]);
		CleanupStack::Pop();
		}

	for( j = 0; j < KTS314_NoofConnections; j++ )
		{
		// check for interface down...
		for( i = 0; i < KTS314_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown, NULL);
			}

		// reissue requests, except after last check
		if( j != (KTS314_NoofConnections - 1) )
			{
			// re-issue requests, which should complete immediately with cached status
			for( i = 0; i < KTS314_NoofMonitors; i++ )
				{
				AllInterfaceNotification(monitor[i], status[i], info[i]);
				}
			}

		} // for j

	// close monitors
	for( i = 0; i < KTS314_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest314

/******************************************************************************
 *
 * Test 315
 *
 * This code will be the same as for test 317, but with just one monitor.
 *
 *****************************************************************************/


const TInt KTS315_NoofMonitors = 1;		// to make code same for multiple monitors
const TInt KTS315_NoofConnections = ((TInt)KMaxInterfaceNotificationQueueLength);

// Test multiple monitors receive notifications from multiple interfaces
enum TVerdict TE_RConnectionTest315::doTestStepL(void)
{
	TInt err;
	TInt i;
	TInt j;		// used for nested loops

	RSocketServ ss;
	RConnection connection[ KTS315_NoofConnections ];

	RConnection monitor[ KTS315_NoofMonitors ];
	TRequestStatus status[ KTS315_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS315_NoofMonitors ];
	TInt iapId[] = {ListOfSuitableIAPs};

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS315_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	for( i = 0; i < KTS315_NoofConnections; i++ )
		{
		err = OpenConnection(connection[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(connection[i]);
		}

	// issue requests
	for( i = 0; i < KTS315_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start several different connections
	for( i = 0; i < KTS315_NoofConnections; i++ )
		{
		// the first of these should complete the initial request,
		// the remainder should fill the cache
		err = StartConnectionWithOverrides(connection[i], iapId[i]);
		if(KErrNone != err)
		    {
        	// Swallow outstanding signals
        	for(i=0;i<KTS315_NoofMonitors;i++)
        		{
        		User::WaitForAnyRequest();
        		}
		    User::Leave(err);
		    }
		}

	// all but one connection status change should be cached
	// the odd one having completed the outstanding request

	// check completed request (uncached status change)
	for( i = 0; i < KTS315_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection[0]);
		}

	// not checking for remainder of "up" values, these will be lost by subsequent "down" values

	// close connections
	for( i = 0; i < KTS315_NoofConnections; i++ )
		{
		CloseConnection(connection[i]);
		CleanupStack::Pop();
		}

	// need to wait for interfaces to be taken down
	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	// closing connection notifications should overwrite all "up" values in cache
	for( j = 0; j < KTS315_NoofConnections; j++ )
		{
		for( i = 0; i < KTS315_NoofMonitors; i++ )
			{
			// re-issue requests, which should complete immediately with cached status
			AllInterfaceNotification(monitor[i], status[i], info[i]);
			// check for interface down...
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown, NULL);
			}
		} // for j

	// close monitors
	for( i = 0; i < KTS315_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest315

/******************************************************************************
 *
 * Test 316
 *
 * This code will be the same as for test 314, but with multiple monitors.
 *
 *****************************************************************************/

const TInt KTS316_NoofMonitors = 2;		// to make code same for multiple monitors
const TInt KTS316_NoofConnections = ((TInt)KMaxInterfaceNotificationQueueLength+1);

// Test multiple monitors receive notifications from multiple interfaces
enum TVerdict TE_RConnectionTest316::doTestStepL(void)
{
	TInt err;
	TInt i;
	TInt j;		// used for nested loops

	RSocketServ ss;
	RConnection connection[ KTS316_NoofConnections ];

	RConnection monitor[ KTS316_NoofMonitors ];
	TRequestStatus status[ KTS316_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS316_NoofMonitors ];
	TInt iapId[] = {ListOfSuitableIAPs};

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS316_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	for( i = 0; i < KTS316_NoofConnections; i++ )
		{
		err = OpenConnection(connection[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(connection[i]);
		}

	// issue requests
	for( i = 0; i < KTS316_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start several different connections
	for( i = 0; i < KTS316_NoofConnections; i++ )
		{
		// the first of these should complete the initial request,
		// the remainder should fill the cache
		err = StartConnectionWithOverrides(connection[i], iapId[i]);
		if(KErrNone != err)
		    {
        	// Swallow outstanding signals
        	for(i=0;i<KTS316_NoofMonitors;i++)
        		{
        		User::WaitForAnyRequest();
        		}
		    User::Leave(err);
		    }
		}

	for( j = 0; j < KTS316_NoofConnections; j++ )
		{
		for( i = 0; i < KTS316_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection[j]);
			}

		// re-issue requests, which should complete immediately with cached status
		for( i = 0; i < KTS316_NoofMonitors; i++ )
			{
			AllInterfaceNotification(monitor[i], status[i], info[i]);
			}

		} // for j

	// NOTE: request outstanding at this point...

	// close connections
	for( i = 0; i < KTS316_NoofConnections; i++ )
		{
		CloseConnection(connection[i]);
		CleanupStack::Pop();
		}

	for( j = 0; j < KTS316_NoofConnections; j++ )
		{
		// check for interface down...
		for( i = 0; i < KTS316_NoofMonitors; i++ )
			{
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown, NULL);
			}

		// reissue requests, except after last check
		if( j != (KTS316_NoofConnections - 1) )
			{
			// re-issue requests, which should complete immediately with cached status
			for( i = 0; i < KTS316_NoofMonitors; i++ )
				{
				AllInterfaceNotification(monitor[i], status[i], info[i]);
				}
			}

		} // for j

	// close monitors
	for( i = 0; i < KTS316_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest316

/******************************************************************************
 *
 * Test 317
 *
 * This code will be the same as for test 315, but with multiple monitors.
 *
 *****************************************************************************/

const TInt KTS317_NoofMonitors = 2;		// to make code same for multiple monitors
const TInt KTS317_NoofConnections = ((TInt)KMaxInterfaceNotificationQueueLength);

// Test multiple monitors receive notifications from multiple interfaces
enum TVerdict TE_RConnectionTest317::doTestStepL(void)
{
	TInt err;
	TInt i;
	TInt j;		// used for nested loops

	RSocketServ ss;
	RConnection connection[ KTS317_NoofConnections ];

	RConnection monitor[ KTS317_NoofMonitors ];
	TRequestStatus status[ KTS317_NoofMonitors ];
	TInterfaceNotificationBuf info[ KTS317_NoofMonitors ];
	TInt iapId[] = {ListOfSuitableIAPs};

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	for( i = 0; i < KTS317_NoofMonitors; i++ )
		{
		err = OpenConnection(monitor[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(monitor[i]);
		}

	for( i = 0; i < KTS317_NoofConnections; i++ )
		{
		err = OpenConnection(connection[i], ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(connection[i]);
		}

	// issue requests
	for( i = 0; i < KTS317_NoofMonitors; i++ )
		{
		AllInterfaceNotification(monitor[i], status[i], info[i]);
		}

	// start several different connections
	for( i = 0; i < KTS317_NoofConnections; i++ )
		{
		// the first of these should complete the initial request,
		// the remainder should fill the cache
		err = StartConnectionWithOverrides(connection[i], iapId[i]);
		if(KErrNone != err)
		    {
        	// Swallow outstanding signals
        	for(i=0;i<KTS317_NoofMonitors;i++)
        		{
        		User::WaitForAnyRequest();
        		}
		    User::Leave(err);
		    }
		}

	// all but one connection status change should be cached
	// the odd one having completed the outstanding request

	// check completed request (uncached status change)
	for( i = 0; i < KTS317_NoofMonitors; i++ )
		{
		CheckInterfaceNotificationL(status[i], info[i], EInterfaceUp, &connection[0]);
		}

	// not checking for remainder of "up" values, these will be lost by subsequent "down" values

	// close connections
	for( i = 0; i < KTS317_NoofConnections; i++ )
		{
		CloseConnection(connection[i]);
		CleanupStack::Pop();
		}

	// need to wait for interfaces to be taken down
	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	// closing connection notifications should overwrite all "up" values in cache
	for( j = 0; j < KTS317_NoofConnections; j++ )
		{
		// check for interface down...
		for( i = 0; i < KTS317_NoofMonitors; i++ )
			{
			// re-issue requests, which should complete immediately with cached status
			AllInterfaceNotification(monitor[i], status[i], info[i]);
			// check for interface down...
			CheckInterfaceNotificationL(status[i], info[i], EInterfaceDown);
			}
		} // for j

	// close monitors
	for( i = 0; i < KTS317_NoofMonitors; i++ )
		{
		CloseConnection(monitor[i]);
		CleanupStack::Pop();
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest317

/******************************************************************************
 *
 * Test 318
 *
 * To test multiple start.
 *
 *****************************************************************************/
// To test multiple start
enum TVerdict TE_RConnectionTest318::doTestStepL(void)
{
	RSocketServ ss;
	RConnection conn1;

	TRequestStatus tStartConn;

	TInt nErr = OpenSocketServer( ss );
	TESTEL(KErrNone == nErr, nErr);
	CleanupClosePushL(ss);
    
    nErr = conn1.Open( ss );
    TESTEL( nErr == KErrNone, nErr );
	CleanupClosePushL(conn1);
    
    conn1.Start( tStartConn );
    nErr = conn1.Start();
    //shouldn't be so quick KErrInUse expected
    TESTEL( nErr == KErrInUse, nErr );
	// wait for completion
	User::WaitForRequest( tStartConn );
    TESTEL( tStartConn.Int() == KErrNone, tStartConn.Int() );

    //close conn1
    conn1.Stop();
	CleanupStack::PopAndDestroy();

    //the other way around
    nErr = conn1.Open( ss );
    TESTEL( nErr == KErrNone, nErr );
	CleanupClosePushL(conn1);

    nErr = conn1.Start();
    TESTEL( nErr == KErrNone, nErr );
    conn1.Start( tStartConn );
	User::WaitForRequest( tStartConn );
    //should get KErrAlreadyExists
    TESTEL( tStartConn.Int() == KErrAlreadyExists, tStartConn.Int() );

    //close conn1
    conn1.Stop();
	CleanupStack::PopAndDestroy();

    //close ss
	CleanupStack::PopAndDestroy();
    return TestStepResult();
} // TE_RConnectionTest318


/******************************************************************************
 *
 * Test TestAllInterfaceNot112
 *
 * Added for def143083
 * 
 * Commsdat configuration should have a default connection that fails to start.
 * Uses dummynif and sets it to fail using the mobility pub/sub.
 * 
 * Start listening for AllInterfaceNotification
 * Start a connection asynchronously
 * On completion of AllInterfaceNotification with EInterfaceUp - Re-request
 * Attach another connection to the interface reported as up
 * Async start completes with error
 * AllInterfaceNotification *SHOULD* complete with EInterfaceDown
 *****************************************************************************/
TVerdict TE_RConnectionTestAllInterfaceNot210::doTestStepL(void)
    {
    TInt err;
    
    TRequestStatus allIfaceStatus;
    TInterfaceNotificationBuf notificationBuf;
    iConnAllIfaceNotify.AllInterfaceNotification(notificationBuf, allIfaceStatus);

    TRequestStatus connStartStatus;
    iConnStart.Start(connStartStatus);
    User::WaitForRequest(connStartStatus, allIfaceStatus);
    
    if (allIfaceStatus == KRequestPending)
        {
        // Fail
        INFO_PRINTF2(_L("Connection start completed with %d before AllInterfaceNotification()"), connStartStatus.Int());
        iConnAllIfaceNotify.CancelAllInterfaceNotification();
        User::WaitForRequest(allIfaceStatus);
        return EFail; // EFail has no effect, default fail result set in pre-amble
        }
    
    // AllInterfaceNotification() completed
    if (notificationBuf().iState != EInterfaceUp)
        {
        // Fail
        INFO_PRINTF1(_L("AllInterfaceNotification() completed with unexpected state [iState != EInterfaceUp]"));
        iConnStart.Stop();
        User::WaitForRequest(connStartStatus);
        return EFail; // EFail has no effect, default fail result set in pre-amble
        }
    
    INFO_PRINTF1(_L("AllInterfaceNotification() completed with [iState == EInterfaceUp]"));
    TConnectionInfoBuf connInfoBuf(notificationBuf().iConnectionInfo);

    // Attach() to the connection
    err = iConnAttach.Attach(connInfoBuf, RConnection::EAttachTypeMonitor);
    if (err != KErrNone)
        {
        // FAIL
        INFO_PRINTF2(_L("Attach() completed with %d"), err);
        iConnStart.Stop();
        User::WaitForRequest(connStartStatus);
        return EFail; // EFail has no effect, default fail result set in pre-amble
        }

    // Re-request
    iConnAllIfaceNotify.AllInterfaceNotification(notificationBuf, allIfaceStatus);

    // Set test time out 
    TRequestStatus timerStatus;
    const TInt KTimerDelay = 20 * 1000 * 1000; // 20 secs should be plenty for the connection start to fail
    iTimer.After(timerStatus, KTimerDelay);
    
    const TInt KRequestCount = 3; 
    TRequestStatus* requests[KRequestCount] = { &connStartStatus, &allIfaceStatus, &timerStatus };
    User::WaitForNRequest(requests, KRequestCount);
    
    if (timerStatus != KRequestPending)
        {
        // Fail
        INFO_PRINTF1(_L("Guard timer elapsed"));
        iConnAllIfaceNotify.CancelAllInterfaceNotification();
        User::WaitForRequest(allIfaceStatus);
        iConnStart.Stop();
        User::WaitForRequest(connStartStatus);
        return EFail; // EFail has no effect, default fail result set in pre-amble
        }
    
    iTimer.Cancel();
    User::WaitForRequest(timerStatus);
    
    if (allIfaceStatus == KRequestPending)
        {
        // Connection start completed
        if (connStartStatus.Int() != KErrNone)
            {
            // Reset timer and wait a little longer
            iTimer.After(timerStatus, KTimerDelay);
            User::WaitForRequest(allIfaceStatus, timerStatus);
            }
        else
            {
            // FAIL - Misconfigured? Connection is expected to fail
            INFO_PRINTF1(_L("Connection started successfully (** Was expected to fail **)"));
            iConnAllIfaceNotify.CancelAllInterfaceNotification();
            User::WaitForRequest(allIfaceStatus);
            return EFail; // EFail has no effect, default fail result set in pre-amble
            }
        
        if (allIfaceStatus == KRequestPending)
            {
            // FAIL
            INFO_PRINTF1(_L("Guard timer elapsed"));
            iConnAllIfaceNotify.CancelAllInterfaceNotification();
            User::WaitForRequest(allIfaceStatus);
            return EFail; // EFail has no effect, default fail result set in pre-amble
            }
        
        iTimer.Cancel();
        User::WaitForRequest(timerStatus);
        }
    else
        {
        // Just wait for the conn start to complete
        User::WaitForRequest(connStartStatus);
        if (connStartStatus.Int() == KErrNone)
            {
            // FAIL - Misconfigured? Connection is expected to fail
            INFO_PRINTF1(_L("Connection started successfully (** Was expected to fail **)"));
            iConnAllIfaceNotify.CancelAllInterfaceNotification();
            User::WaitForRequest(allIfaceStatus);
            return EFail; // EFail has no effect, default fail result set in pre-amble
            }
        }
    
    // Last thing to check - make sure it completed with interface down
    if (notificationBuf().iState != EInterfaceDown)
        {
        // FAIL
        INFO_PRINTF1(_L("AllInterfaceNotification() completed with unexpected state [iState != EInterfaceDown]"));
        return EFail; // EFail has no effect, default fail result set in pre-amble
        }
    
    // PASS
    SetTestStepResult(EPass);
    return EPass;
    }

TVerdict TE_RConnectionTestAllInterfaceNot210::doTestStepPreambleL()
    {
    TInt err;
    
    SetTestStepResult(EFail);

    // Prepare socket server and connections
    err = iSS.Connect();
    TESTEL(err == KErrNone, err);
    
    err = iConnStart.Open(iSS);
    TESTEL(err == KErrNone, err);
    
    err = iConnAttach.Open(iSS);
    TESTEL(err == KErrNone, err);

    err = iConnAllIfaceNotify.Open(iSS);
    TESTEL(err == KErrNone, err);
    
    err = iTimer.CreateLocal();
    TESTEL(err == KErrNone, err);
    
    // Setup the dummynif pub/sub setting
    const TUid KAvailabilityTestingPubSubUid = { 0x10272F42 };
    const TInt KApId = 6; /* see the def143083.xml config */

    RProperty dummyNifEnableProperty;
    err = dummyNifEnableProperty.Define(KAvailabilityTestingPubSubUid, KApId , RProperty::EInt);
    if (err != KErrAlreadyExists)
        {
        TESTEL(err == KErrNone, err);
        }
    
    err = dummyNifEnableProperty.Attach(KAvailabilityTestingPubSubUid, KApId);
    TESTEL(err == KErrNone, err);

    dummyNifEnableProperty.Set(0); // Dummy NIF not available - Start should fail
    dummyNifEnableProperty.Close();
    return EPass;
    }

TVerdict TE_RConnectionTestAllInterfaceNot210::doTestStepPostambleL()
    {
    return EPass;
    }

TE_RConnectionTestAllInterfaceNot210::~TE_RConnectionTestAllInterfaceNot210()
    {
    iConnAllIfaceNotify.Close();
    iConnAttach.Close();
    iConnStart.Close();
    iSS.Close();
    
    iTimer.Close();
    }
