// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the test cases that use real ethernet
// as the default interface. These tests test that connections requiring
// configuration by configuration daemons such as DHCP start successfully
// and that they can be queried using the Ioctl() function.
// 
//

/**
 @file
*/

#include "TE_RConnectionConfigDaemonTestSteps.h"
#include <nifman.h>
#include <comms-infras/es_config.h>


TE_RConnectionTest600::~TE_RConnectionTest600() {}

enum TVerdict TE_RConnectionTest600::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address
  * configuration.  If the connection starts then DHCP and everything
  * else went fine!
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	StopConnection(conn);
	
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest601::~TE_RConnectionTest601() {}

enum TVerdict TE_RConnectionTest601::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address
  * configuration then queries it for the configured address.
  * To check the validity of the address retrieved a socket is
  * bound to it.  If this fails the address returned is not correct
  * with respect to the one configured for the connection by DHCP.
  * The bind is a useful way of checking the address, as it will
  * only succeed if the TCP/IP6 stack has a route for the address.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);
	
	TRequestStatus stat;	
	TConnectionAddrBuf address;
	address().iAddressFamily = KAfInet;
	conn.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, stat, &address);
	User::WaitForRequest(stat);
	TESTEL(stat == KErrNone, stat.Int());
	
	RSocket socket;
	err = socket.Open(socketServer, KAfInet, KSockDatagram, KProtocolInetUdp, conn);
	TESTEL(err == KErrNone, err);
	
	CleanupClosePushL(socket);
	
	TInetAddr* properAddr = (TInetAddr*)&(address().iAddr);	
	TInetAddr addr;
	addr.SetAddress(properAddr->Address());
	
	err = socket.Bind(addr);
	TESTEL(err == KErrNone, err);
	CleanupStack::PopAndDestroy(&socket);

	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest602::~TE_RConnectionTest602() {}

enum TVerdict TE_RConnectionTest602::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address configuration
  * and stops it again.  It then attempts to find out the address that was used 
  * for the connection when it was up.  This will fail, with KErrNotReady, as 
  * the DHCP configuration daemon can no longer be queried for the address of a 
  * connection, as the connection has gone down, therefore the config daemon's 
  * record of the connection has been deleted.
  *
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	StopConnection(conn);
		
	TRequestStatus stat;		
	TConnectionAddrBuf address;
	address().iAddressFamily = KAfInet;
	conn.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, stat, &address);
	User::WaitForRequest(stat);
	TESTEL(stat == KErrNotReady, stat.Int());

	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest603::~TE_RConnectionTest603() {}

enum TVerdict TE_RConnectionTest603::doTestStepL(void)
/**
  * The test attempts to query RConnection for the address of the connection
  * but the connection has not been started.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	TRequestStatus stat;
	TConnectionAddrBuf address;
	address().iAddressFamily = KAfInet;
	conn.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, stat, &address);
	User::WaitForRequest(stat);
	TESTEL(stat == KErrNotReady, stat.Int());

	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest604::~TE_RConnectionTest604() {}

enum TVerdict TE_RConnectionTest604::doTestStepL(void)
/**
  * The test starts an ethernet connection that requires DHCP address configuration
  * and queries for the configured address and stopping the connection before waiting
  * for the query to complete.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);
	
	TRequestStatus stat;
	conn.Ioctl(KCOLConfiguration, KConnAddrRenew, stat);

	StopConnection(conn);
	
	User::WaitForRequest(stat);
	TESTEL(stat == KErrCancel, stat.Int());
	
	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest605::~TE_RConnectionTest605() {}

enum TVerdict TE_RConnectionTest605::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address configuration
  * and querying to renew the lease before calling the query. The 
  * request will complete with KErrCancel and will not be fulfilled.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);
	
	TRequestStatus stat;

	conn.Ioctl(KCOLConfiguration, KConnAddrRenew, stat);
	conn.CancelIoctl();
	User::WaitForRequest(stat);
	TESTEL(stat == KErrCancel, stat.Int());

	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	

TE_RConnectionTest606::~TE_RConnectionTest606() {}

enum TVerdict TE_RConnectionTest606::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address configuration
  * and calling cancelIoctl without ever having an Ioctl request outstanding.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);
	
	conn.CancelIoctl();	// does nothing in this state

	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest607::~TE_RConnectionTest607() {}

enum TVerdict TE_RConnectionTest607::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address configuration
  * but calls canclIoctl() first without having an outstanding ioctl request.
  * It then starts a connection does a proper Ioctl query to check the cancel
  * isn't somehow wrongly cached and then stops the connection just to check 
  * that no functionality is interferred with by the cancel.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	conn.CancelIoctl();	// does nothing in this state
	
	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// now do a query to check cancel isn;t cached somehow...
	TRequestStatus stat;	
	TConnectionAddrBuf address;
	address().iAddressFamily = KAfInet;
	conn.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, stat, &address);
	User::WaitForRequest(stat);
	TESTEL(stat == KErrNone, stat.Int());
	
	RSocket socket;
	err = socket.Open(socketServer, KAfInet, KSockDatagram, KProtocolInetUdp, conn);
	TESTEL(err == KErrNone, err);
	
	CleanupClosePushL(socket);
	
	TInetAddr* properAddr = (TInetAddr*)&(address().iAddr);	
	TInetAddr addr;
	addr.SetAddress(properAddr->Address());

	err = socket.Bind(addr);
	TESTEL(err == KErrNone, err);
	CleanupStack::PopAndDestroy(&socket);
	
	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}
	
TE_RConnectionTest608::~TE_RConnectionTest608() {}

enum TVerdict TE_RConnectionTest608::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address configuration
  * then calls canclIoctl() without having an outstanding ioctl request.
  * It then queries for the configured address to ensure that no functionality has been
  * interferred with by the cancel.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	conn.CancelIoctl();	// does nothing in this state

	TRequestStatus stat;
	TConnectionAddrBuf address;
	address().iAddressFamily = KAfInet;
	conn.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, stat, &address);
	User::WaitForRequest(stat);
	TESTEL(stat == KErrNone, stat.Int());

	RSocket socket;
	err = socket.Open(socketServer, KAfInet, KSockDatagram, KProtocolInetUdp, conn);
	TESTEL(err == KErrNone, err);
	
	CleanupClosePushL(socket);
	
	TInetAddr* properAddr = (TInetAddr*)&(address().iAddr);	
	TInetAddr addr;
	addr.SetAddress(properAddr->Address());

	err = socket.Bind(addr);
	TESTEL(err == KErrNone, err);
	CleanupStack::PopAndDestroy(&socket);

	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}


TE_RConnectionTest609::~TE_RConnectionTest609() {}

enum TVerdict TE_RConnectionTest609::doTestStepL(void)
/**
  * The test starts an ethernet connection requiring DHCP address configuration
  * asynchronously and queries for the configured address before the start completes.
  */
	{
	TInt err;

	RSocketServ socketServer;
	
	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);
	
	RConnection conn;
	
	err = OpenConnection(conn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	TRequestStatus stat1;
	StartConnectionAsynchronous(conn, stat1);
	
	TRequestStatus stat2;
	TConnectionAddrBuf address;
	address().iAddressFamily = KAfInet;
	conn.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, stat2, &address);
	User::WaitForRequest(stat2);
	User::WaitForRequest(stat1);
	
	TESTEL(stat2 == KErrNotReady, stat2.Int());
	TESTEL(stat1 == KErrNone, stat1.Int());

	StopConnection(conn);
	CleanupStack::PopAndDestroy(&conn);

	CleanupStack::PopAndDestroy(&socketServer);	// just sanity check our pops

	return TestStepResult();
	}

