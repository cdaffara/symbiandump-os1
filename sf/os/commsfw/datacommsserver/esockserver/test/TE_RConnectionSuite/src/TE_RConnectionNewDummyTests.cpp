// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the new test cases that use Dummy Nif
// added to improve test coverage
// 
//
//

/**
 @file
*/

#include "TE_RConnectionNewDummyTests.h"
#include <cs_subconevents.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestTERCnND, "ESockTestTERCnND");
#endif


TE_RConnectionTest800::~TE_RConnectionTest800()
	{
	}

enum TVerdict TE_RConnectionTest800::doTestStepL(void)

// Ensure that Close() does not pull down the interface when there are sockets associated with it

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

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);


	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest800



TE_RConnectionTest801::~TE_RConnectionTest801()
	{
	}

enum TVerdict TE_RConnectionTest801::doTestStepL(void)
/*
 * Ensure that if stop is called on a non existant sub connection that the server deals with this
 */
	{
	SetTestStepResult (EFail);
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// open udp socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// send data using udp socket
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// stop subconnection with invalid sub connection ID
	err = conn.Stop(555, RConnection::EStopNormal);
	if(err == KErrNotReady)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest801



TE_RConnectionTest802::~TE_RConnectionTest802()
	{
	}

enum TVerdict TE_RConnectionTest802::doTestStepL(void)
/*
 * Make a call to Cancel a DataTransferredRequest, then call an actual DataTransferredRequest
 * verify that the previous cancel doesn't affect the new DataTransferredRequest call
 */
	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	SetTestStepResult (EFail);

	// Open Socket Server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Open sub connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// open udp socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// send data using socket
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	TPckg<TUint> down(0);
	TPckg<TUint> up(0);
	TRequestStatus transferredStatus;

	// cancel data transferred call
	conn.DataTransferredCancel();

	// issue data transferred call
	conn.DataTransferredRequest( up, down, transferredStatus);
	User::WaitForRequest(transferredStatus);
	TESTEL(KErrNone == transferredStatus.Int(), transferredStatus.Int());

	// verify that uplink buffer has not been corrupted
	TInt uplink = 0;
	uplink = up();
	if(uplink >= 0)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest802


TE_RConnectionTest803::~TE_RConnectionTest803()
	{
	}

enum TVerdict TE_RConnectionTest803::doTestStepL(void)
/*
 * Make a call to DataTransferredRequest, Connection has not been started
 */
	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	SetTestStepResult (EFail);

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	TPckg<TUint> down(0);
	TPckg<TUint> up(0);
	TRequestStatus transferredStatus;

	// issue data transferred request, conn has NOT been started
	conn.DataTransferredRequest( up, down, transferredStatus);
	User::WaitForRequest(transferredStatus);
	if(KErrNotReady == transferredStatus.Int())
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest803



TE_RConnectionTest804::~TE_RConnectionTest804()
	{
	}

enum TVerdict TE_RConnectionTest804::doTestStepL(void)
/*
 * make a call to DataTransferredRequest on a subconnection ID that does not exist
 */
	{
	TInt err;
	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// open UDP socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// send data with socket
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	TPckg<TUint> down(0);
	TPckg<TUint> up(0);
	TRequestStatus transferredStatus;

	// make a call to data transferred request on subconnection with invalid sub conn ID
	const TUint KInvalidSubConn = 555;
	conn.DataTransferredRequest(KInvalidSubConn, up, down, transferredStatus);
	User::WaitForRequest(transferredStatus);
	TESTEL(KErrArgument == transferredStatus.Int(), transferredStatus.Int());

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest804



TE_RConnectionTest805::~TE_RConnectionTest805()
	{
	}
enum TVerdict TE_RConnectionTest805::doTestStepL(void)
/*
 * Make a call to DataSentNotificationRequest with athreshold being one hundred bytes, threshold is reached
 */
	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	SetTestStepResult (EFail);

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	//	Open UDO socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// make DataSentNotificationRequest after 100 bytes has been transferred
	const TUint KHundredBytes = 100;
	TPckg< TUint > uplinkPkg(0);
	TRequestStatus dataSentStatus;
	conn.DataSentNotificationRequest(KHundredBytes, uplinkPkg, dataSentStatus);

	// send over 100 bytes using udp socket
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// Make sure uplinkPkg has not been corrupted
	User::WaitForRequest(dataSentStatus);
	TInt uplinkResult = 0;
	uplinkResult = uplinkPkg();
	if(uplinkResult >= 0)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest805


TE_RConnectionTest806::~TE_RConnectionTest806()
	{
	}

enum TVerdict TE_RConnectionTest806::doTestStepL(void)
/*
 * Make a call to DataSentNotificationRequest with athreshold being one Thousand bytes
 * Threshold is NOT reached
 */
	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	SetTestStepResult (EFail);

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// open UDP Socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// make DataSentNotificationRequest after 1000 bytes has been transferred
	const TUint KThousandBytes = 1000;
	TPckg< TUint > uplinkPkg(0);
	TRequestStatus dataSentStatus;
	conn.DataSentNotificationRequest(KThousandBytes, uplinkPkg, dataSentStatus);

	// send less than 1000 bytes
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// Should not have completed - wait a short period and check.
	const TInt KPausePeriod = 2;
	User::After(KPausePeriod * 1000000);
	TESTEL(KRequestPending == dataSentStatus.Int(), dataSentStatus.Int());

	// Stop connection and verify data monitoring completion code is KErrCancel from the Stop()
	conn.Stop();
	User::WaitForRequest(dataSentStatus);
	TESTEL(KErrCancel == dataSentStatus.Int(), dataSentStatus.Int());

	// verify uplink result is unchanged
	TUint uplinkResult = uplinkPkg();
	if(uplinkResult == 0)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest806


TE_RConnectionTest807::~TE_RConnectionTest807()
	{
	}
enum TVerdict TE_RConnectionTest807::doTestStepL(void)
/*
 * Make a call to DataSentNotificationRequest with athreshold being one Thousand bytes
 * Threshold is reached, invalid subconnection ID
 */	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);


	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);


	// open UDP socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	 // make DataSentNotificationRequest on subconnection after 100 bytes has been transferred, Sub conn ID is not valid
	const TUint KInvalidSubConnID = 555;
	const TUint KHundredBytes = 100;
	TPckg< TUint > uplinkPkg(0);
	TRequestStatus dataSentStatus;
	conn.DataSentNotificationRequest( KInvalidSubConnID, KHundredBytes, uplinkPkg, dataSentStatus );

	// send data, threshold of 100 is reached
	err = TestUdpDataPathL(sock, iDummyNifSendAddr, 1000);
	TESTEL(KErrNone == err, err);

	// verify call returns
	User::WaitForRequest(dataSentStatus);
	TESTEL(KErrArgument == dataSentStatus.Int(), dataSentStatus.Int());

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest807


TE_RConnectionTest808::~TE_RConnectionTest808()
	{
	}
enum TVerdict TE_RConnectionTest808::doTestStepL(void)
/*
 * Make a call to DataSentNotificationRequest with athreshold being one Thousand bytes
 * Threshold is reached, using VALID subconnection ID
 */
	{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	SetTestStepResult(EPass);

	// open socket server
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// open connection
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start connection
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// open UDP socket
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);


	const TUint KHundredBytes = 100;
	const TUint KThousandBytes = 1000;
	const TUint KDeafaultSubConnID = 1;
	TPckg< TUint > uplinkPkg(0);
	TRequestStatus dataSentStatus;

	// make DataSentNotificationRequest on subconnection after 100 bytes has been transferred, Sub conn ID is valid
	conn.DataSentNotificationRequest(KDeafaultSubConnID, KHundredBytes, uplinkPkg, dataSentStatus );
	err = TestUdpDataPathL(sock, iDummyNifSendAddr, KThousandBytes);
	TESTEL(KErrNone == err, err);

	User::WaitForRequest(dataSentStatus);
	if (KErrNone == dataSentStatus.Int())
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&conn);
	CleanupStack::PopAndDestroy(&ss);

	return TestStepResult();
	} // TE_RConnectionTest808

TE_RConnectionDataMonitorStepBase::~TE_RConnectionDataMonitorStepBase()
	{
	}

void TE_RConnectionDataMonitorStepBase::StartThreeConnectionsAndOpenUdpSocketLC(RSocketServ& aSs, TInt aIap, RSocket& aSocket, RConnection& aConn1, RConnection& aConn2, RConnection& aConn3)
/**
Open an RSocketServ, open three RConnection objects on the RSocketServ, start the three RConnection objects
towards the same IAP, open a UDP socket on the first RConnection.

Helper function for setting up environment for certain data monitoring tests.

@param aSs RSocketServ instance
@param aIap IAP to start
@param aConn1 RConnection 1
@param aConn2 RConnection 2
@param aConn3 RConnection 3
*/
	{
	TInt err;
	TRequestStatus status2;
	TRequestStatus status3;
	TCommDbConnPref pref2;
	TCommDbConnPref pref3;

	// open socket server
	err = OpenSocketServer(aSs);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(aSs);

	// open connections
	err = OpenConnection(aConn1, aSs);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(aConn1);

	err = OpenConnection(aConn2, aSs);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(aConn2);

	err = OpenConnection(aConn3, aSs);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(aConn3);

	// start first connection
	err = StartConnectionWithOverrides(aConn1, aIap);
	TESTEL(KErrNone == err, err);

	// start remaining connections.  Only reason to start them asynchronously is to ensure
	// that the interface started above does not timeout in the time it takes to start the
	// new interfaces.

	StartConnectionWithOverridesAsynchronous(aConn2, pref2, aIap, status2);
	StartConnectionWithOverridesAsynchronous(aConn3, pref3, aIap, status3);

	// open UDP Socket
	err = OpenUdpSocketExplicitL(aSocket, aSs, aConn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(aSocket);

	User::WaitForRequest(status2);
	User::WaitForRequest(status3);

	TESTEL(KErrNone == status2.Int(), status2.Int());
	TESTEL(KErrNone == status3.Int(), status3.Int());
	}

void TE_RConnectionDataMonitorStepBase::StopConnectionAndCleanup(RConnection& aConn)
/**
Cleanup after a call to StartThreeConnectionsAndOpenUdpSocketLC(...)

@param aConn RConnection 1
*/
	{
	// Stop connection
	aConn.Stop();

	CleanupStack::PopAndDestroy();			// sock
	CleanupStack::PopAndDestroy();			// conn3
	CleanupStack::PopAndDestroy();			// conn2
	CleanupStack::PopAndDestroy(&aConn);
	CleanupStack::PopAndDestroy();			// ss
	}

void TE_RConnectionDataMonitorStepBase::SendDataAndCheckCompletionsL(RSocket& aSocket, TInt aCount, TUint aAmountToSend[], TRequestStatus* aCompletionOrder[])
/**
Loop round sending and receiving specified amounts of data on a UDP socket, checking each time that an associated
TRequestStatus has completed as a result.

Helper function for testing data monitoring.

@param aSocket UDP socket to send/receive data over
@param aCount number of loop iterations
@param aAmountToSend array containing the amount of data to send/receive for each loop iteration
@param aCompletionOrder array containing TRequestStatus objects that are supposed to complete for each loop iteration
*/
	{
	for (TInt i = 0 ; i < aCount ; ++i)
		{
		// send/receive data
		TestUdpDataPathL(aSocket, iDummyNifSendAddr, aAmountToSend[i]);

		// check that associated TRequestStatus object has completed
		TRequestStatus* status = aCompletionOrder[i];
		User::WaitForRequest(*status);
		TESTEL(KErrNone == status->Int(), status->Int());

		// check that remaining TRequestStatus objects have *not* completed.
		for (TInt j = i + 1 ; j < aCount ; ++j)
			{
			TESTEL(KRequestPending == aCompletionOrder[j]->Int(), aCompletionOrder[j]->Int());
			}
		}
	}

TE_RConnectionTest809::~TE_RConnectionTest809()
	{
	}

enum TVerdict TE_RConnectionTest809::doTestStepL(void)
/*
 * Check data monitoring uplink requests from several RConnection's towards the same
 * underlying interface work correctly.
 */
	{
	RSocketServ ss;
	RConnection conn1;
	RConnection conn2;
	RConnection conn3;
	RSocket sock;

	SetTestStepResult (EFail);

	StartThreeConnectionsAndOpenUdpSocketLC(ss, iDummyNifIap, sock, conn1, conn2, conn3);

	// Issue data monitoring requests that are successively smaller.  This is testing
	// the proper handling of an underlying delta array of thresholds where the first
	// threshold in the list has to be successively replaced with smaller ones.  Ensure
	// that these values have sufficient seperation to take into account the ~28 bytes
	// of header that are added.

	const TUint KThresh1 = 180;
	const TUint KThresh2 = 110;
	const TUint KThresh3 = 50;

	__ASSERT_DEBUG(KThresh1 > KThresh2 && KThresh2 > KThresh3, User::Panic(KSpecAssert_ESockTestTERCnND, 1));		// sanity check against inappropriate changes to KThresh*

	TPckg< TUint > uplinkPkg1(0);
	TPckg< TUint > uplinkPkg2(0);
	TPckg< TUint > uplinkPkg3(0);

	TRequestStatus status1, status2, status3;
	conn1.DataSentNotificationRequest(KThresh1, uplinkPkg1, status1);
	conn2.DataSentNotificationRequest(KThresh2, uplinkPkg2, status2);
	conn3.DataSentNotificationRequest(KThresh3, uplinkPkg3, status3);

	// Now send enough data to complete the requests one by one and check proper threshold handling

	TUint amountsToSend[] =
		{
		KThresh3,
		KThresh2 - KThresh3,
		KThresh1 - KThresh2
		};

	TRequestStatus* completionOrder[] =
		{
		&status3,
		&status2,
		&status1
		};

	SendDataAndCheckCompletionsL(sock, 3, amountsToSend, completionOrder);

	// sanity check the uplink values returned
	TESTEL(uplinkPkg3() < uplinkPkg2(), uplinkPkg3());
	TESTEL(uplinkPkg2() < uplinkPkg1(), uplinkPkg2());

	SetTestStepResult(EPass);

	StopConnectionAndCleanup(conn1);

	return TestStepResult();
	} // TE_RConnectionTest809

TE_RConnectionTest810::~TE_RConnectionTest810()
	{
	}

enum TVerdict TE_RConnectionTest810::doTestStepL(void)
/*
 * Check data monitoring uplink requests from several RConnection's towards the same
 * underlying interface work correctly.  As 809, but for downlink rather than uplink.
 */
	{
	RSocketServ ss;
	RConnection conn1;
	RConnection conn2;
	RConnection conn3;
	RSocket sock;

	SetTestStepResult (EFail);

	// Start up three RConnection's to the same underlying interface and open a UDP socket on one of them
	StartThreeConnectionsAndOpenUdpSocketLC(ss, iDummyNifIap, sock, conn1, conn2, conn3);

	// Issue data monitoring requests that are successively smaller.  This is testing
	// the proper handling of an underlying delta array of thresholds where the first
	// threshold in the list has to be successively replaced with smaller ones.

	const TUint KThresh1 = 180;
	const TUint KThresh2 = 110;
	const TUint KThresh3 = 50;

	__ASSERT_DEBUG(KThresh1 > KThresh2 && KThresh2 > KThresh3, User::Panic(KSpecAssert_ESockTestTERCnND, 2));		// sanity check against inappropriate changes to KThresh*

	TPckg< TUint > downlinkPkg1(0);
	TPckg< TUint > downlinkPkg2(0);
	TPckg< TUint > downlinkPkg3(0);

	TRequestStatus status1, status2, status3;
	conn1.DataReceivedNotificationRequest(KThresh1, downlinkPkg1, status1);
	conn2.DataReceivedNotificationRequest(KThresh2, downlinkPkg2, status2);
	conn3.DataReceivedNotificationRequest(KThresh3, downlinkPkg3, status3);

	// Now send enough data to complete the requests one by one and check proper threshold handling

	TUint amountsToSend[] =
		{
		KThresh3,
		KThresh2 - KThresh3,
		KThresh1 - KThresh2
		};

	TRequestStatus* completionOrder[] =
		{
		&status3,
		&status2,
		&status1
		};

	SendDataAndCheckCompletionsL(sock, 3, amountsToSend, completionOrder);

	// sanity check the downlink values returned
	TESTEL(downlinkPkg3() < downlinkPkg2(), downlinkPkg3());
	TESTEL(downlinkPkg2() < downlinkPkg1(), downlinkPkg2());

	SetTestStepResult(EPass);

	StopConnectionAndCleanup(conn1);

	return TestStepResult();
	} // TE_RConnectionTest810

TE_RConnectionTest811::~TE_RConnectionTest811()
	{
	}

enum TVerdict TE_RConnectionTest811::doTestStepL(void)
/*
 * Check data monitoring uplink requests from several RConnection's towards the same
 * underlying interface work correctly.  As test 809 but with increasing thresholds.
 */
	{
	RSocketServ ss;
	RConnection conn1;
	RConnection conn2;
	RConnection conn3;
	RSocket sock;

	SetTestStepResult (EFail);

	StartThreeConnectionsAndOpenUdpSocketLC(ss, iDummyNifIap, sock, conn1, conn2, conn3);

	// Issue data monitoring requests that are successively larger.  This is testing
	// the proper handling of an underlying delta array of thresholds where entries
	// are added into the listin different places.  Ensure that these values have
	// sufficient seperation to take into account the ~28 bytes of header that are added.

	const TUint KThresh1 = 50;
	const TUint KThresh2 = 180;
	const TUint KThresh3 = 110;

	__ASSERT_DEBUG(KThresh1 < KThresh3 && KThresh3 < KThresh2, User::Panic(KSpecAssert_ESockTestTERCnND, 3));

	TPckg< TUint > uplinkPkg1(0);
	TPckg< TUint > uplinkPkg2(0);
	TPckg< TUint > uplinkPkg3(0);

	TRequestStatus status1, status2, status3;
	conn1.DataSentNotificationRequest(KThresh1, uplinkPkg1, status1);
	conn2.DataSentNotificationRequest(KThresh2, uplinkPkg2, status2);
	conn3.DataSentNotificationRequest(KThresh3, uplinkPkg3, status3);

	// Now send enough data to complete the requests one by one and check proper threshold handling

	TUint amountsToSend[] =
		{
		KThresh1,
		KThresh3 - KThresh1,
		KThresh2 - KThresh3
		};

	TRequestStatus* completionOrder[] =
		{
		&status1,
		&status3,
		&status2
		};

	SendDataAndCheckCompletionsL(sock, 3, amountsToSend, completionOrder);

	// sanity check the uplink values returned
	TESTEL(uplinkPkg3() < uplinkPkg2(), uplinkPkg3());
	TESTEL(uplinkPkg1() < uplinkPkg3(), uplinkPkg1());

	SetTestStepResult(EPass);

	StopConnectionAndCleanup(conn1);

	return TestStepResult();
	} // TE_RConnectionTest811

TE_RConnectionTest812::~TE_RConnectionTest812()
	{
	}

enum TVerdict TE_RConnectionTest812::doTestStepL(void)
/*
 * Check data monitoring uplink requests from several RConnection's towards the same
 * underlying interface work correctly.  As test 810 but with increasing thresholds.
 */
	{
	RSocketServ ss;
	RConnection conn1;
	RConnection conn2;
	RConnection conn3;
	RSocket sock;

	SetTestStepResult (EFail);

	// Start up three RConnection's to the same underlying interface and open a UDP socket on one of them
	StartThreeConnectionsAndOpenUdpSocketLC(ss, iDummyNifIap, sock, conn1, conn2, conn3);

	// Issue data monitoring requests that are successively smaller.  This is testing
	// the proper handling of an underlying delta array of thresholds where the first
	// threshold in the list has to be successively replaced with smaller ones.

	const TUint KThresh1 = 50;
	const TUint KThresh2 = 180;
	const TUint KThresh3 = 110;

	__ASSERT_DEBUG(KThresh1 < KThresh3 && KThresh3 < KThresh2, User::Panic(KSpecAssert_ESockTestTERCnND, 4));

	TPckg< TUint > downlinkPkg1(0);
	TPckg< TUint > downlinkPkg2(0);
	TPckg< TUint > downlinkPkg3(0);

	TRequestStatus status1, status2, status3;
	conn1.DataReceivedNotificationRequest(KThresh1, downlinkPkg1, status1);
	conn2.DataReceivedNotificationRequest(KThresh2, downlinkPkg2, status2);
	conn3.DataReceivedNotificationRequest(KThresh3, downlinkPkg3, status3);

	// Now send enough data to complete the requests one by one and check proper threshold handling

	TUint amountsToSend[] =
		{
		KThresh1,
		KThresh3 - KThresh1,
		KThresh2 - KThresh3
		};

	TRequestStatus* completionOrder[] =
		{
		&status1,
		&status3,
		&status2
		};

	SendDataAndCheckCompletionsL(sock, 3, amountsToSend, completionOrder);

	// sanity check the uplink values returned
	TESTEL(downlinkPkg3() < downlinkPkg2(), downlinkPkg3());
	TESTEL(downlinkPkg1() < downlinkPkg3(), downlinkPkg1());

	SetTestStepResult(EPass);

	StopConnectionAndCleanup(conn1);

	return TestStepResult();
	} // TE_RConnectionTest812


