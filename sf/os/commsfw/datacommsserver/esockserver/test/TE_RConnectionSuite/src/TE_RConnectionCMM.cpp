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
// Contains the implementation of test cases testing CMM functionality of RConnection.
// 
//

/**
 @file
*/

#include "TE_RConnectionCMM.h"
#include <ss_std.h>
#include <comms-infras/nifprvar.h>
#include <nifman.h>
#include <inet6err.h>
#include <in6_opt.h>


/**
 * Subconnection Management API extensions
 * 
 * - 400 EnumerateSubConnections(TUint& aCount);
 * - 400 GetSubConnectionInfo(TDes8& aSubConnectionInfo);
 * - 401 GetSubConnectionInfo(TUint aIndex, TDes8& aSubConnectionInfo);
 * - 402 AllSubConnectionNotification(TSubConnectionNotificationBuf& aSubConnectionEvent, TRequestStatus& aStatus);
 * - 403 CancelAllSubConnectionNotification();
 * - 404 DataTransferredRequest(TPckg<TUint>& aUplinkVolume, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
 * - 405 DataTransferredRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TPckg<TUint>& aUplinkVolume, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
 * - 406 DataTransferredCancel();
 * - 407 DataTransferredCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * - 408 DataSentNotificationRequest(TUint aThreshold, TPckg<TUint>& aUplinkVolume, TRequestStatus& aStatus);
 * - 419 DataSentNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TPckg<TUint>& aUplinkVolume, TRequestStatus& aStatus);
 * - 410 DataSentNotificationCancel();
 * - 411 DataSentNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * - 412 DataReceivedNotificationRequest(TUint aThreshold, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
 * - 413 DataReceivedNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
 * - 414 DataReceivedNotificationCancel();
 * - 415 DataReceivedNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * - 416 IsConnectionActiveRequest(TUint aSecs, TPckg<TBool>& aState, TRequestStatus& aStatus);
 * - 417 IsConnectionActiveCancel();
 * - 418 IsSubConnectionActiveRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aSecs, TPckg<TBool>& aState, TRequestStatus& aStatus);
 * - 419 IsSubConnectionActiveCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * - 419b IsSubConnectionActiveCancel(TSubConnectionUniqueId aSubConnectionUniqueId); [Connection Stopped Before Cancel]
 * - 420 Create two connections and enumerate
 * - 421 Multiple outstanding requests on a single interface.
 * - 422 Test erroring of sockets following an EStopNormal
 * - 423 Test erroring of sockets following an EStopAuthoritative
 * - 424 Test stopping subconnections when using the Nifman compatibility layer.
 * - 425 Test DataTransferredRequest() when multiple interfaces are dealing with requests 
 * - 426 Test DataSentNotificationRequest() when multiple interfaces are dealing with requests
 * - 427 Test DataReceivedNotificationRequest() when multiple interfaces are dealing with requests 
 * - 428 Explicitly create a single test NIF connection (using commdb settings) enumerate subconnections and get subconnection info (both with and without subconnection ID).
 * - 429 AllSubConnectionNotification test using the test NIF.
 * - 430 Test of cancellation of AllSubConnectionNotification when using the test NIF
 * - 431 Test of DataTransferredRequest() when using the test NIF.
 * - 432 Test of DataTransferredRequest() for subconnection when using test NIF.
 * - 433 Test of DataSentNotification() request when using the test NIF.
 * - 434 Test of DataSentNotification() request for subconnections when using the test NIF.
 * - 435 Test of DataSentNotification() cancellation when using the test NIF.
 * - 436 Test of DataSentNotification cancellation for subconnections when using the test NIF.
 * - 437 Test of DataReceivedNotification when using the test NIF.
 * - 438 Test of DataReceivedNotification() request for subconnections when using the test NIF.
 * - 439 Test of DataReceivedNotification() cancellation when using the test NIF.
 * - 440 Test of DataReceivedNotification() cancellation for subconnections when using the test NIF.
 * - 441 Test DataTransferredRequest for subconnection with primary and secondary contexts present
 * - 442 Test DataTransferred Request() for subconnection (primary context) with a secondary context present.
 * - 443 Test DataTransferredRequest() for subconnection (secondary context) with another secondary context present.
 * - 444 Test DataTransferredRequest() for complete connection when multiple subconnections are active.
 * - 445 Test for DataTransferred Request on one context then another
 * - 446 Test DataSentNotification() for a secondary context while the primary is also active.
 * - 447 Test DataSentNotification() for primary context while a secondary is also active.
 * - 448 Test DataSentNotification() for complete connection while multiple subconnections are active and passing traffic
 * - 449 Test for DataSentNotification on one context then another
 * - 450 Tests for multiple parallel DataSentNotification requests and cancellation with multiple contexts
 * - 451 Test DataReceivedNotification() for a secondary context while the primary is also active.
 * - 452 Test DataReceivedNotification() for primary context while a secondary is also active.
 * - 453 Test DataReceivedNotification() for complete connection while multiple subconnections are active and passing traffic
 * - 454 Test for DataReceivedNotification on one context then another
 * - 455 Tests for multiple parallel DataReceivedNotification requests and cancellation with multiple contexts
 * - 456 AllSubConnectionNotification test with multiple subconnections.
 * - 457 Simple test with two interfaces - one which supports the new M classes one which doesn't. Try out DataTransferredRequest for each.
 * - 458 Use DataSentNotification while two interfaces (one supporting the M class interface and one not) are active - make request on NIF which does support the extra interface.
 * - 459 Use DataSentNotification while two interfaces (one supporting the M class interface and one not) are active - make request on NIF which does support the extra interface.
 * - 460 Use DataReceivedNotification while two interfaces (one supporting the M class inteface and one not) are active - make request on NIF not supporting extra interface.
 * - 461 Use DataReceivedNotification while two interfaces (one supporting the M class inteface and one not) are active - make request on NIF which does support the interface.
 * - 462 Simple test with two interfaces neither of which support the extended management interface. Try out DataTransferredRequest for  each.
 * - 463 Two interfaces neither of which support extended management interface. Parallel use of DataTransferredRequest
 * - 464 Use DataSentNotification while two interfaces are active (neither of which supports the extended management interface). - make request on first interface created
 * - 465 Use DataSentNotification while two interfaces are active (neither of which supports the extended management interface). - make request on second interface created.
 * - 466 Use DataReceivedNotification while two interfaces are active (neither of which supports the extended management interface). - make request on first interface created.
 * - 467 Use DataReceivedNotification while two interfaces are active (neither of which supports the extended management interface). - make request on second interface created.
 * - 468 Use AllSubConnectionNotification while two interfaces are active (neither of which supports the extended management interface).
 */

#define E_DELAY 1000000
#define E_1SEC  1
#define E_10SEC 10
#define NUM_OF_REPETITIONS 10


enum TVerdict TE_RConnectionTest400::doTestStepL(void)
{
/**
 * Test 400
 * Creates connection and enumerates
 * Tests for EnumerateSubConnections() & GetSubConnectionInfo();
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;
	// Open socket server
	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Open connection
	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF asynchronously
	// Note: Could be any other NIFs 
	TRequestStatus status;
	TCommDbConnPref prefs;
	StartConnectionWithOverridesAsynchronous(conn, prefs, iDummyNifIap, status);
	
	// while it's starting, check that EnumerateSubConnections returns error 
	// DEF021400
	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNotReady == err, err);

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	// check connection counter
	TESTEL(1 == connectionCount, KErrGeneral);

	// TConnectionInfo version 1
	TPckgBuf<TConnectionInfo> connectionInfoV1;
	err = conn.GetConnectionInfo(1, connectionInfoV1);
	TESTEL(KErrNone == err, KErrGeneral);

	TPckgBuf<TConnectionInfoV2> connectionInfo;
	
	// try wrong argument - 0 
	err = conn.GetConnectionInfo(0, connectionInfo);
	TESTEL(KErrArgument == err, err);
	
	// get connection info
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);

	TESTEL(2 == subconnectionCount, subconnectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;

	// for entire connection (index 0)
	err = conn.GetSubConnectionInfo(0, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	// check unique ID
	// unique ID must be 0 
	TSubConnectionUniqueId uniqueID = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueID == 0, KErrGeneral);

	// for subconnection (index 1)
	// for phase 1 we can have only one subconnection 
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	// check unique ID
	// for phase 1 subconnection ID is 1 (...)
	uniqueID = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueID != 0, KErrGeneral);

	TUint count(0);
	
	// check number of connections before start
	err = conn.EnumerateConnections( count );
	TESTEL(KErrNone == err, err);
	TESTEL(1 == count, count);

	// stop connection
	conn.Stop();
	
	// check number of connections after stop
	err = conn.EnumerateConnections( count );
	TESTEL(KErrNone == err, err);
	TESTEL(0 == count, count);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest400


enum TVerdict TE_RConnectionTest401::doTestStepL(void)
{
/**
 * Test 401
 * Get subconnection info
 * Test GetSubConnectionInfo(TSubConnectionInfo& aSubConnectionIfo)
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	TUint connectionCount;

	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	// 1 connection
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfoV2> connectionInfo;

	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;

	// Call GetSubConnectionInfo(TUint aIndex, TSubConnectionInfo& aSubConnectionIfo)
	// to get Unique ID
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	// Check unique ID
	TSubConnectionUniqueId uniqueID = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueID != 0, KErrGeneral);

	// Call GetSubConnectionInfo(TSubConnectionInfo& aSubConnectionIfo)
	err = conn.GetSubConnectionInfo(subconnectionInfo);

	TESTEL(KErrNone == err, err);

	conn.Stop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest401


enum TVerdict TE_RConnectionTest402::doTestStepL(void)
{
/**
 *
 * Test 402
 * void AllSubConnectionNotification(TDes8& aNotification, TRequestStatus& aStatus)
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection connOne;
	
	err = OpenConnection(connOne, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connOne);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(connOne, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// Issue request
	TRequestStatus status;
	TSubConnectionNotificationBuf subconnectionNotification;

	connOne.AllSubConnectionNotification(subconnectionNotification, status);
	User::WaitForRequest(status);

	TSubConnectionEvent& event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TSubConnectionEventType type = event.iEventType;

	// first event from entire connection - ESubConnectionOpened
	TESTEL(ESubConnectionOpened == type, KErrGeneral);

	connOne.AllSubConnectionNotification(subconnectionNotification, status);
	User::WaitForRequest(status);

	// second event from first subconnection - ESubConnectionOpened
	type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, KErrGeneral);

	TUint connectionCount;
	err = connOne.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	// 1 connection
	TESTEL(1 == connectionCount, KErrGeneral);

	// create a socket to allow data to pass
	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, connOne);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// send some test data to give close event something to return
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// start listen for close event
	TSubConnectionClosedEvent& closeEvent = (TSubConnectionClosedEvent&)(*subconnectionNotification.Ptr());
	connOne.AllSubConnectionNotification(subconnectionNotification, status);

    // Use the EStopAuthoritative stop
	connOne.Stop(RConnection::EStopAuthoritative);

	// small delay
	User::After(E_DELAY);

	// wait for completion
	User::WaitForRequest(status);
	TESTEL(KErrNone == err, err);

	// event from first subconnection - ESubConnectionClosed
	TESTEL(ESubConnectionClosed == closeEvent.iEventType, KErrGeneral);
	TESTEL(KTotalUdpTestPacketSize == closeEvent.iTotalUplinkDataVolume, KErrGeneral);
	TESTEL(KTotalUdpTestPacketSize == closeEvent.iTotalDownlinkDataVolume, KErrGeneral);

	err = connOne.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(0 == connectionCount, KErrGeneral);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(connOne);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest402



enum TVerdict TE_RConnectionTest403::doTestStepL(void)
{
/**
 *
 * Test 403
 * void CancelAllSubConnectionNotification();
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection connOne;
	err = OpenConnection(connOne, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connOne);

	// first we'll check that cancel works on failed connection
	// note that iBadNtRasIap will fail after a few secs
	TRequestStatus status;
	TCommDbConnPref prefs;
	StartConnectionWithOverridesAsynchronous(connOne, prefs, iBadNtRasIap, status);

	// Issue request
	TRequestStatus notifStatus;
	TSubConnectionNotificationBuf subconnectionNotification;
	connOne.AllSubConnectionNotification(subconnectionNotification, notifStatus);

	// now wait for StartConnection to fail
	User::WaitForRequest(status);
	TESTEL(KErrNone != status.Int(), status.Int()); // _should_ fail!!

	// cancel notification - pre-DEF023815 this would hang - so need timeout
	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);
	TRequestStatus timerStatus;
	timer.After(timerStatus, 500000); // allow 500ms to process cancel

	connOne.CancelAllSubConnectionNotification();
	User::WaitForRequest(notifStatus, timerStatus);
	User::WaitForAnyRequest();
	timer.Cancel();
	TESTEL(KErrCancel == notifStatus.Int(), notifStatus.Int());

	// now start working case by starting dummynif
	err = StartConnectionWithOverrides(connOne, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// Issue new request
	connOne.AllSubConnectionNotification(subconnectionNotification, status);
	User::WaitForRequest(status);

	TSubConnectionEvent& event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TSubConnectionEventType type = event.iEventType;

	// event from first connection 
	TESTEL(ESubConnectionOpened == type, KErrGeneral);

	// get next event
	connOne.AllSubConnectionNotification(subconnectionNotification, status);
	User::WaitForRequest(status);

	// event from first subconnection 
	type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, KErrGeneral);

	TUint connectionCount;
	err = connOne.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	// 1 connection
	TESTEL(1 == connectionCount, KErrGeneral);

	connOne.AllSubConnectionNotification(subconnectionNotification, status);

	User::After(E_DELAY);
	// cancel request
	connOne.CancelAllSubConnectionNotification();
	// wait for request completion 
	User::WaitForRequest(status);
	// status code - KErrCancel
	TESTEL(KErrCancel == status.Int(), KErrGeneral);

	CleanupStack::PopAndDestroy(&timer);

	connOne.Stop();
	CloseConnection(connOne);
	CleanupStack::Pop(&connOne);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest403


enum TVerdict TE_RConnectionTest404::doTestStepL(void)
{
/**
 *
 * Test 404
 * void DataTransferredRequest(TUint& aUplinkVolume, TUint& aDownlinkVolume, TRequestStatus& aStatus); 
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	TRequestStatus status;
	TCommDbConnPref prefs;
	StartConnectionWithOverridesAsynchronous(conn, prefs, iDummyNifIap, status);

	// check return code is correct before startup
	TUint uplinkVolume(0);
	TUint downlinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TRequestStatus dtstatus;
	conn.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, dtstatus);
	User::WaitForRequest(dtstatus);
	TESTEL(KErrNotReady == dtstatus.Int(), dtstatus.Int());

	// wait for dummynif to complete startup
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	// start socket
	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// send and receive data (512 bytes)
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// short delay
	User::After(E_DELAY);

	// Issue request for entire connection
	conn.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);

	// wait for request completion 
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(uplinkVolume != 0, KErrGeneral);
	TESTEL(downlinkVolume != 0, KErrGeneral);

	// wait for dummynif to timeout
	TNifProgressBuf progress;
	ProgressNotification(conn, status, progress, KLinkLayerClosed);
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	
	//Now, SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY has made this behaviour slightly
	//more robust, as despite the registered shutdown of the connection, we can still
	//serve datamon values until the stack wraps up. It's disputible whether we should
	//be making promises about the state of the stack promptly after registered tear
	//down or especially after an arbitrary 0.03secs. Nontheless, it has been deemed
	//a balanced choice at the time of typing this very comment. 
	User::After(30000);
	// Issue request for entire connection
	conn.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);

	// wait for request completion, with KErrNotReady, as the NIF no longer 
	// exists
	User::WaitForRequest(status);
	TESTEL(KErrNotReady == status.Int(), status.Int());

	conn.Stop();
	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest404



enum TVerdict TE_RConnectionTest405::doTestStepL(void)
{
 /**
 *
 * Test 405
 * void DataTransferredRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint& aUplinkVolume, TUint& aDownlinkVolume, TRequestStatus& aStatus); 
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfoV2> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	// get unique subconnection ID
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TUint uplinkVolume(0);
	TUint downlinkVolume(0);
	TRequestStatus status;

	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);

	// send and receive data (512 bytes)
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// short delay
	User::After(E_DELAY);

	// for specific subconnection
	conn.DataTransferredRequest(subConnectionUniqueId, uplinkVolumeDes, downlinkVolumeDes, status);

	User::WaitForRequest(status);

	TESTEL(KErrNone == err, err);

	TESTEL(uplinkVolume != 0, KErrGeneral);
	TESTEL(downlinkVolume != 0, KErrGeneral);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest405


enum TVerdict TE_RConnectionTest406::doTestStepL(void)
{
/**
 *
 * Test 406
 * void DataTransferredCancel();
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TUint uplinkVolume(0);
	TUint downlinkVolume(0);
	TRequestStatus status;

	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// wait for a period of time
	User::After(E_DELAY);  

	conn.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);

	// cancel request
	conn.DataTransferredCancel();
	User::WaitForRequest(status);

	// check for correct value
	// Note: the request actually completes synchronously in esock, so we
	// can currently never get a status of KErrCancel, so we'll accept 
	// KErrNone as ok
	TESTEL(status.Int() == KErrNone || status.Int() == KErrCancel, status.Int());

	conn.Stop();
	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest406



enum TVerdict TE_RConnectionTest407::doTestStepL(void)
{
/**
 *
 * Test 407
 * void DataTransferredCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;

	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint uplinkVolume(0);
	TUint downlinkVolume(0);
	TRequestStatus status;

	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);


	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// wait for a period of time
	User::After(E_DELAY);  

	conn.DataTransferredRequest(subConnectionUniqueId, uplinkVolumeDes, downlinkVolumeDes, status);

	// cancel request
	conn.CancelAllSubConnectionNotification();
	// wait for request completion
	User::WaitForRequest(status);

	// check for correct value
	// Note: the request actually completes synchronously in esock, so we
	// can currently never get a status of KErrCancel, so we'll accept 
	// KErrNone as ok
	TESTEL(status.Int() == KErrNone || status.Int() == KErrCancel, status.Int());

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest407


enum TVerdict TE_RConnectionTest408::doTestStepL(void)
{
/**
 *
 * Test 408
 * void DataSentNotificationRequest(TUint aThreshold, TUint& aUplinkVolume, TRequestStatus& aStatus);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TUint uplinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TUint threshold(KLowThresholdSize); // ie threshold > KTotalUdpTestPacketSize

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);

	TUint totalVolumeCounter(0);
	TUint i;
	for (i=0; i<3; i++)
		{
		TUint volumeCounter(0);
		TRequestStatus status;
		TRequestStatus timerStatus;

		// for entire connection 
		conn.DataSentNotificationRequest(threshold, uplinkVolumeDes, status);

		while (volumeCounter < threshold)
			{
			// set short timeout as datasentnotification should NOT complete yet
			timer.After(timerStatus, 200000);
			User::WaitForRequest(status, timerStatus);

			// check timer completed first and notification still pending
			TESTEL(KErrNone == timerStatus.Int(), timerStatus.Int());
			TESTEL(KRequestPending == status.Int(), status.Int());

			// send/receive KTotalUdpTestPacketSize bytes
			err = TestUdpDataPathL(sock, iDummyNifSendAddr);
			TESTEL(KErrNone == err, err);
			volumeCounter += KTotalUdpTestPacketSize;

			// wait for a period of time
			User::After(E_DELAY);  
			}
		
		// datasentnotification should complete now
		timer.After(timerStatus, 200000);
		User::WaitForRequest(status, timerStatus);

		// check for correct value
		TESTEL(KErrNone == status.Int(), status.Int());
		totalVolumeCounter += volumeCounter;
		TESTEL(uplinkVolume == totalVolumeCounter, KErrGeneral);

		timer.Cancel();
		User::WaitForAnyRequest();
		}

	CleanupStack::PopAndDestroy(&timer);

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest408


enum TVerdict TE_RConnectionTest409::doTestStepL(void)
{
/**
 *
 * Test 409
 * void DataSentNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TUint& aUplinkVolume, TRequestStatus& aStatus);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TUint uplinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TUint threshold(KLowThresholdSize); // ie threshold > KTotalUdpTestPacketSize

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);

	TUint totalVolumeCounter(0);
	TUint i;
	for (i=0; i<3; i++)
		{
		TUint volumeCounter(0);
		TRequestStatus status;
		TRequestStatus timerStatus;

		// ask for data notification for subconnection
		TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;
		conn.DataSentNotificationRequest(subConnectionUniqueId, threshold, uplinkVolumeDes, status);

		while (volumeCounter < threshold)
			{
			// set short timeout as datasentnotification should NOT complete yet
			timer.After(timerStatus, 200000);
			User::WaitForRequest(status, timerStatus);

			// check timer completed first and notification still pending
			TESTEL(KErrNone == timerStatus.Int(), timerStatus.Int());
			TESTEL(KRequestPending == status.Int(), status.Int());

			// send/receive KTotalUdpTestPacketSize bytes
			err = TestUdpDataPathL(sock, iDummyNifSendAddr);
			TESTEL(KErrNone == err, err);
			volumeCounter += KTotalUdpTestPacketSize;

			// wait for a period of time
			User::After(E_DELAY);  
			}
		
		// datasentnotification should complete now
		timer.After(timerStatus, 200000);
		User::WaitForRequest(status, timerStatus);

		// check for correct value
		TESTEL(KErrNone == status.Int(), status.Int());
		totalVolumeCounter += volumeCounter;
		TESTEL(uplinkVolume == totalVolumeCounter, KErrGeneral);

		timer.Cancel();
		User::WaitForAnyRequest();
		}

	CleanupStack::PopAndDestroy(&timer);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest409


enum TVerdict TE_RConnectionTest410::doTestStepL(void)
{
/**
 *
 * Test 410
 * void DataSentNotificationCancel();
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TUint uplinkVolume(0);
	TUint threshold(512);
	TRequestStatus status;

	TPckg<TUint> uplinkVolumeDes(uplinkVolume);

	conn.DataSentNotificationRequest(threshold, uplinkVolumeDes, status);

	// cancel request
	conn.DataSentNotificationCancel();
	// wait for request completion
	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrCancel, status.Int());

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest410


enum TVerdict TE_RConnectionTest411::doTestStepL(void)
{
/**
 *
 * Test 411
 * void DataSentNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint uplinkVolume(0);
	TUint threshold(512);
	TRequestStatus status;

	TPckg<TUint> uplinkVolumeDes(uplinkVolume);

	conn.DataSentNotificationRequest(subConnectionUniqueId, threshold, uplinkVolumeDes, status);
	conn.DataSentNotificationCancel(subConnectionUniqueId);

	User::WaitForRequest(status);

	// check for correct value
	TESTEL(status.Int() == KErrCancel, status.Int());

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest411


enum TVerdict TE_RConnectionTest412::doTestStepL(void)
{
/**
 *
 * Test 412
 * void DataReceivedNotificationRequest(TUint aThreshold, TUint& aDownlinkVolume, TRequestStatus& aStatus);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TUint downlinkVolume(0);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TUint threshold(KLowThresholdSize); // ie threshold > KTotalUdpTestPacketSize

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);

	TUint totalVolumeCounter(0);
	TUint i;
	for (i=0; i<3; i++)
		{
		TUint volumeCounter(0);
		TRequestStatus status;
		TRequestStatus timerStatus;

		// for entire connection 
		conn.DataReceivedNotificationRequest(threshold, downlinkVolumeDes, status);

		while (volumeCounter < threshold)
			{
			// set short timeout as datareceivednotification should NOT complete yet
			timer.After(timerStatus, 200000);
			User::WaitForRequest(status, timerStatus);

			// check timer completed first and notification still pending
			TESTEL(KErrNone == timerStatus.Int(), timerStatus.Int());
			TESTEL(KRequestPending == status.Int(), status.Int());

			// send/receive KTotalUdpTestPacketSize bytes
			err = TestUdpDataPathL(sock, iDummyNifSendAddr);
			TESTEL(KErrNone == err, err);
			volumeCounter += KTotalUdpTestPacketSize;

			// wait for a period of time
			User::After(E_DELAY);  
			}
		
		// datareceivednotification should complete now
		timer.After(timerStatus, 200000);
		User::WaitForRequest(status, timerStatus);

		// check for correct value
		TESTEL(KErrNone == status.Int(), status.Int());
		totalVolumeCounter += volumeCounter;
		TESTEL(downlinkVolume == totalVolumeCounter, KErrGeneral);

		timer.Cancel();
		User::WaitForAnyRequest();
		}

	CleanupStack::PopAndDestroy(&timer);
	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest412


enum TVerdict TE_RConnectionTest413::doTestStepL(void)
{
 /**
 *
 * Test 413
 * void DataReceivedNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TUint& aDownlinkVolume, TRequestStatus& aStatus);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TUint downlinkVolume(0);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TUint threshold(KLowThresholdSize); // ie threshold > KTotalUdpTestPacketSize

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);

	TUint totalVolumeCounter(0);
	TUint i;
	for (i=0; i<3; i++)
		{
		TUint volumeCounter(0);
		TRequestStatus status;
		TRequestStatus timerStatus;

		// for entire connection 
		TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;
		conn.DataReceivedNotificationRequest(subConnectionUniqueId, threshold, downlinkVolumeDes, status);

		while (volumeCounter < threshold)
			{
			// set short timeout as datasentnotification should NOT complete yet
			timer.After(timerStatus, 200000);
			User::WaitForRequest(status, timerStatus);

			// check timer completed first and notification still pending
			TESTEL(KErrNone == timerStatus.Int(), timerStatus.Int());
			TESTEL(KRequestPending == status.Int(), status.Int());

			// send/receive KTotalUdpTestPacketSize bytes
			err = TestUdpDataPathL(sock, iDummyNifSendAddr);
			TESTEL(KErrNone == err, err);
			volumeCounter += KTotalUdpTestPacketSize;

			// wait for a period of time
			User::After(E_DELAY);  
			}
		
		// datasentnotification should complete now
		timer.After(timerStatus, 200000);
		User::WaitForRequest(status, timerStatus);

		// check for correct value
		TESTEL(KErrNone == status.Int(), status.Int());
		totalVolumeCounter += volumeCounter;
		TESTEL(downlinkVolume == totalVolumeCounter, KErrGeneral);

		timer.Cancel();
		User::WaitForAnyRequest();
		}

	CleanupStack::PopAndDestroy(&timer);

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest413


enum TVerdict TE_RConnectionTest414::doTestStepL(void)
{
/**
 *
 * Test 414
 * void DataReceivedNotificationCancel();
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TUint downlinkVolume(0);
	TUint threshold(512);
	TRequestStatus status;

	TPckg<TUint> downlinkVolumeDes(downlinkVolume);

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);
	TRequestStatus timerStatus;

	// for entire connection
	conn.DataReceivedNotificationRequest(threshold, downlinkVolumeDes, status);

	// give esock 500ms to cancel in case cancel just comes from connection timeout
	timer.After(timerStatus, 500000);
	conn.DataReceivedNotificationCancel();
	User::WaitForRequest(status, timerStatus);

	// check for correct values
	TESTEL(timerStatus.Int() == KRequestPending, timerStatus.Int());
	TESTEL(status.Int() == KErrCancel, status.Int());

	User::WaitForAnyRequest(); // wait for completion (of the timer)

	CleanupStack::PopAndDestroy(&timer);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest414

enum TVerdict TE_RConnectionTest415::doTestStepL(void)
{
/**
 *
 * Test 415
 * void DataReceivedNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint downlinkVolume(0);
	TUint threshold(512);
	TRequestStatus status;

	TPckg<TUint> downlinkVolumeDes(downlinkVolume);

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);
	TRequestStatus timerStatus;

	// for entire connection
	conn.DataReceivedNotificationRequest(subConnectionUniqueId, threshold, downlinkVolumeDes, status);

	// give esock 500ms to cancel in case cancel just comes from connection timeout
	timer.After(timerStatus, 500000);
	conn.DataReceivedNotificationCancel(subConnectionUniqueId);
	User::WaitForRequest(status, timerStatus);

	// check for correct values
	TESTEL(timerStatus.Int() == KRequestPending, timerStatus.Int());
	TESTEL(status.Int() == KErrCancel, status.Int());

	User::WaitForAnyRequest(); // wait for completion (of the timer)

	CleanupStack::PopAndDestroy(&timer);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest415


enum TVerdict TE_RConnectionTest416::doTestStepL(void)
{
/**
 *
 * Test 416
 * void IsConnectionActiveRequest(TUint aSecs, TBool& aState, TRequestStatus& aStatus);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);
	TRequestStatus timerStatus;

	TRequestStatus status;
	const TUint secs(E_1SEC*2);
	const TUint microsecs(secs*1000000);
	const TUint extraMicrosecs(200000); // to account for AO delays etc
	TBool state(EFalse);
	TPckg<TBool> stateDes(state);

	TUint i;

	// first part of the test. Check that if appl believes connection is 
	// currently active, call does not return until it becomes inactive.
	// This is achieved by sending lots of data and confirming call
	// doesn't complete
	state = ETrue;
	conn.IsConnectionActiveRequest(secs, stateDes, status);
	for (i=0; i<NUM_OF_REPETITIONS; i++)
		{
		err = TestUdpDataPathL(sock, iDummyNifSendAddr);
		TESTEL(KErrNone == err, err);

		timer.After(timerStatus, microsecs/2);
		User::WaitForRequest(status, timerStatus);

		// check timer finished first
		TESTEL(timerStatus.Int() == KErrNone, timerStatus.Int());
		TESTEL(KRequestPending == status.Int(), status.Int());
		}

	// now wait for connection to register as inactive - should be within
	// 'secs' seconds

//#define REPRO_IS_CONNECTION_ACTIVE_PANIC	
#ifndef REPRO_IS_CONNECTION_ACTIVE_PANIC
	timer.After(timerStatus, microsecs + extraMicrosecs);
	User::WaitForRequest(status, timerStatus);
	
	// check timer finished last and status values are correct

	if (status.Int() == KRequestPending)
		{
		// NOTE: This cleanup path was temporarily added to avoid a KERN-EXEC 44 which occurs
		// if the IsConnectionActive() request is still pending when the session/subsession
		// is cleaned up should the test fail.  The failure can be reproduced by commenting
		// out this cleanup code path and failing the test case whilst the IsConnectionActive()
		// request is still pending (i.e. define REPRO_IS_CONNECTION_ACTIVE_PANIC).  This should
		// be removed when the KERN-EXEC 44 is fixed.
		conn.IsConnectionActiveCancel();
		User::WaitForRequest(status);
		INFO_PRINTF1(_L("IsConnectionActive() failed to complete in time"));
		TESTEL(EFalse, KErrGeneral);
		}
#else
	TESTEL(EFalse, KErrGeneral);			// deliberately fail leaving pending IsConnectionActive()
	
	// check timer finished last and status values are correct

#endif

	TESTEL(timerStatus.Int() == KRequestPending, timerStatus.Int());
	TESTEL(status.Int() == KErrNone, status.Int());
	TESTEL(state == EFalse, KErrGeneral);

	timer.Cancel();
	User::WaitForAnyRequest();

	// second part of the test. Check that if appl believes connection is 
	// currently inactive, call does not return until it becomes active.
	state = EFalse;
	conn.IsConnectionActiveRequest(secs, stateDes, status);

	// check doesn't return while inactive
	timer.After(timerStatus, microsecs*2);
	User::WaitForRequest(status, timerStatus);

	// check timer returned first
	TESTEL(timerStatus.Int() == KErrNone, timerStatus.Int());
	TESTEL(KRequestPending == status.Int(), status.Int());

	// now send some data and check that notification returns on next tick
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);
	timer.After(timerStatus, microsecs + extraMicrosecs);
	User::WaitForRequest(status, timerStatus);

	// check timer finished last and status values are correct
	TESTEL(timerStatus.Int() == KRequestPending, timerStatus.Int());
	TESTEL(status.Int() == KErrNone, status.Int());
	TESTEL(state != EFalse, KErrGeneral);

	timer.Cancel();
	User::WaitForAnyRequest();

	CleanupStack::PopAndDestroy(&timer);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest416


enum TVerdict TE_RConnectionTest417::doTestStepL(void)
{
/**
 *
 * Test 417
 * void IsConnectionActiveCancel();
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TRequestStatus status;
	TUint secs(E_1SEC);
	TBool state(EFalse);

	TPckg<TBool> stateDes(state);

	conn.IsConnectionActiveRequest(secs, stateDes, status);
	conn.IsConnectionActiveCancel();

	User::WaitForRequest(status);
	// check for correct value	
	TESTEL(status.Int() == KErrCancel, status.Int());

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest417


enum TVerdict TE_RConnectionTest418::doTestStepL(void)
{
/**
 *
 * Test 418
 * void IsSubConnectionActiveRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aSecs, TBool& aState, TRequestStatus& aStatus);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TRequestStatus status;
	TUint secs(E_1SEC);
	TBool state(EFalse);

	TPckg<TBool> stateDes(state);
	conn.IsSubConnectionActiveRequest(subConnectionUniqueId, secs, stateDes, status);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// wait for a period of time
	User::After(E_DELAY/2);  
	
	User::WaitForRequest(status);
	
	// subconnection active
	TESTEL(status.Int() == KErrNone, status.Int());
	TESTEL(state, KErrGeneral);

	conn.IsSubConnectionActiveRequest(subConnectionUniqueId, secs, stateDes, status);

	// wait for a period of time
	User::After(E_DELAY*2);  

	User::WaitForRequest(status);
	// check for correct value	
	TESTEL(status.Int() == KErrNone, status.Int());
	// subconnection not active
	TESTEL(state == EFalse, KErrGeneral);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest418


enum TVerdict TE_RConnectionTest419::doTestStepL(void)
{
/**
 *
 * Test 419
 * void IsSubConnectionActiveCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TRequestStatus status;
	TUint secs(E_1SEC);
	TBool state(EFalse);

	TPckg<TBool> stateDes(state);

	conn.IsSubConnectionActiveRequest(subConnectionUniqueId, secs, stateDes, status);

	User::After(E_DELAY/2);  

	conn.IsSubConnectionActiveCancel(subConnectionUniqueId);

	User::WaitForRequest(status);
	// check for correct value	
	TESTEL(status.Int() == KErrCancel, status.Int());

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	conn.Stop();
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest419


enum TVerdict TE_RConnectionTest419b::doTestStepL(void)
{
/**
 *
 * Test 419b
 * void IsSubConnectionActiveCancel(TSubConnectionUniqueId aSubConnectionUniqueId); [Stop Connection Before Cancel]
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start DUMMYNIF
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);
	
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	// 1 connection with 1 subconnection
	err = conn.EnumerateSubConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TRequestStatus status;
	TUint secs(E_1SEC);
	TBool state(EFalse);
	TPckg<TBool> stateDes(state);

	conn.IsSubConnectionActiveRequest(subConnectionUniqueId, secs, stateDes, status);

	User::After(E_DELAY/2);  

	// In the real world this could happen, but its more likely that the connection might time out
	// and disconnect before the timeout given in the request triggers
	conn.Stop(); 

	conn.IsSubConnectionActiveCancel(subConnectionUniqueId);
	User::WaitForRequest(status);
	// check for correct value	
	TESTEL(status.Int() == KErrCancel, status.Int());

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest419b


enum TVerdict TE_RConnectionTest420::doTestStepL(void)
{
/**
 * Test 420
 * Create two connections and enumerate
 * Tests for EnumerateSubConnections() & GetSubConnectionInfo()
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;
	
	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** first connection ***********

	RConnection connOne;
	err = OpenConnection(connOne, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connOne);

	// start the dummynif (could be any interface, doesn't matter)
	err = StartConnectionWithOverrides(connOne, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	
	// one connection exists
	TUint connectionCount;
	err = connOne.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	// there should only be a single subconnection (representing the whole connection)
	TUint subconnectionCount;
	err = connOne.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);
	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = connOne.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueID = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueID != KNifEntireConnectionSubConnectionId, uniqueID);

	//********** second connection ***********

	RConnection connTwo;
	err = OpenConnection(connTwo, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connTwo);

	// start hungrynif (is important this isn't dummynif again or esock 
	// will just reuse the same interface already open)
	err = StartConnectionWithOverrides(connTwo, iHungryNifIap);
	TESTEL(KErrNone == err, err);
	
	// should have two connections now
	err = connTwo.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	// there should only be a single subconnection on this interface
	err = connTwo.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	// ensure that it *is* representing the complete connection
	err = connTwo.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	uniqueID = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueID != KNifEntireConnectionSubConnectionId, uniqueID);

	//********** clean up ***********

	connTwo.Stop();
	CloseConnection(connTwo);
	CleanupStack::Pop(&connTwo);

	connOne.Stop();
	CloseConnection(connOne);
	CleanupStack::Pop(&connOne);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest420


enum TVerdict TE_RConnectionTest421::doTestStepL(void)
{
/**
 * Test 421
 * Multiple outstanding requests on a single interface.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
	TInt err;
	TRequestStatus status1, status2, activityStatus;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummynif (could be any interface, doesn't matter)
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);
	
	//********** open the socket to be used ***********

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//********** make requests for notification ***********

	TUint uplinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TUint sendThreshold(KLowThresholdSize); // ie sendThreshold > KTotalUdpTestPacketSize

	TUint downlinkVolume(0);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TUint receiveThreshold(KLowThresholdSize*2); // ie receiveThreshold > KTotalUdpTestPacketSize && sendThreshold != receiveThreshold

	conn.DataSentNotificationRequest(sendThreshold, uplinkVolumeDes, status1);	
	conn.DataReceivedNotificationRequest(receiveThreshold, downlinkVolumeDes, status2);

	//********** make request connection activity status ***********

	TBool state(EFalse);
	TPckg<TBool> stateDes(state);
	const TUint secs(E_1SEC*2);

	conn.IsConnectionActiveRequest(secs, stateDes, activityStatus);
	
	//********** send the data to cause the notifications and activity change ***********

	TUint currentVol(0);

	while (currentVol < sendThreshold)
	{
		TESTEL(KRequestPending == status1.Int(), status1.Int());
		TESTEL(KRequestPending == status2.Int(), status2.Int());
		err = TestUdpDataPathL(sock, iDummyNifSendAddr, KSmallBufferLength);
		TESTEL(KErrNone == err, err);
		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// connection should now be active
	User::WaitForRequest(activityStatus);
	TESTEL(KErrNone == activityStatus.Int(), activityStatus.Int());
	TESTEL(state != EFalse, state);

	// send notification should have completed, receive should still be pending
	User::WaitForRequest(status1);
	TESTEL(KErrNone == status1.Int(), status1.Int());
	TESTEL(uplinkVolume >= sendThreshold, uplinkVolume);
	TESTEL(KRequestPending == status2.Int(), status2.Int());

	currentVol = 0;
	while (currentVol < receiveThreshold)
	{
		err = TestUdpDataPathL(sock, iDummyNifSendAddr, KSmallBufferLength);
		TESTEL(KErrNone == err, err);
		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// receive notification should have completed now
	User::WaitForRequest(status2);
	TESTEL(KErrNone == status2.Int(), status2.Int());
	TESTEL(downlinkVolume >= receiveThreshold, downlinkVolume);

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest421



enum TVerdict TE_RConnectionTest422::doTestStepL(void)
/**
 * Test 422
 * Test erroring of sockets following an EStopNormal
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummynif (could be any interface, doesn't matter)
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);
	
	//********** create the socket which will later be errored ***********

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr, KSmallBufferLength);
	TESTEL(KErrNone == err, err);
		
	//********** stop the connection (EStopNormal) ***********

	// this will exercise both this Stop() and the overloaded one
	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	//********** socket should now be errored ***********

	err = TestUdpDataPathL(sock, iDummyNifSendAddr, KSmallBufferLength);
	TESTEL(KErrDisconnected == err, err);

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
	
} // TE_RConnectionTest422


enum TVerdict TE_RConnectionTest423::doTestStepL(void)
/**
 * Test 423
 * Test erroring of sockets following an EStopAuthoritative
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummynif (could be any interface, doesn't matter)
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);
	
	//********** create the socket which will later be errored ***********

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr, KSmallBufferLength);
	TESTEL(KErrNone == err, err);
		
	//********** stop the connection (EStopAuthoritative) ***********

	err = conn.Stop(RConnection::EStopAuthoritative);
	TESTEL(KErrNone == err, err);

	//********** socket should now be errored ***********

	err = TestUdpDataPathL(sock, iDummyNifSendAddr, KSmallBufferLength);
	TESTEL(KErrDisconnected == err, err);

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
	
} // TE_RConnectionTest423


enum TVerdict TE_RConnectionTest424::doTestStepL(void)
/**
 * Test 424
 * Test stopping subconnections when using the Nifman compatibility layer.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummynif (any interface using the compatibility layer will do)
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	//********** find out about the subconnection and try to stop them ***********

	// there should only be a single subconnection (representing the whole connection)
	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueId = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueId!= KNifEntireConnectionSubConnectionId, uniqueId);

	// do a normal stop - should fail
	err = conn.Stop(uniqueId, RConnection::EStopNormal);
	// doesn't fail because CNifAgentRef throws away the fact that the compatibility layer doesn't support this
	TESTEL(KErrNone == err, err);

	// do an authoritative stop - should fail
	err = conn.Stop(uniqueId, RConnection::EStopAuthoritative);
	// doesn't fail because CNifAgentRef throws away the fact that the compatibility layer doesn't support this
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest424


enum TVerdict TE_RConnectionTest425::doTestStepL(void)
/**
 * Test 425
 * Test DataTransferredRequest() when multiple interfaces are dealing with requests 
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	//********** socket activity on first connection ***********

	RSocket sock1;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

	TESTEL(KErrNone == err, err);

	//********** test DataTransferredRequest ***********

	TUint uplinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TUint downlinkVolume(0);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TRequestStatus status;

	// on first connection should have gone up
	conn1.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);
	User::WaitForRequest(status);
	TESTEL(KSmallBufferUdpTestPacketSize == uplinkVolume, uplinkVolume);
	TESTEL(KSmallBufferUdpTestPacketSize == downlinkVolume, downlinkVolume);

	// on second connection should have stayed at zero
	conn2.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);
	User::WaitForRequest(status);
	TESTEL(0 == uplinkVolume, uplinkVolume);
	TESTEL(0 == downlinkVolume, downlinkVolume);

	//********** socket activity on second connection ***********

	RSocket sock2;

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock2);

	TESTEL(KErrNone == err, err);

	//********** test DataTransferredRequest again ***********

	// on first connection should have stayed the same
	conn1.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);
	User::WaitForRequest(status);
	TESTEL(KSmallBufferUdpTestPacketSize == uplinkVolume, uplinkVolume);
	TESTEL(KSmallBufferUdpTestPacketSize == downlinkVolume, downlinkVolume);

	// on second connection should have gone up
	conn2.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);
	User::WaitForRequest(status);
	TESTEL(0 < uplinkVolume, uplinkVolume);
	TESTEL(0 < downlinkVolume, downlinkVolume);

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest425


enum TVerdict TE_RConnectionTest426::doTestStepL(void)
/**
 * Test 426
 * Test DataSentNotificationRequest() when multiple interfaces are dealing with requests
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;
	TRequestStatus status1, status2;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	//********** socket activity ***********

	RSocket sock1;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	//********** make calls to DataSentNotificationRequest ***********

	TUint volume1(0); // for first interface
	TPckg<TUint> volume1Des(volume1);
	TUint threshold1(KLowThresholdSize);

	TUint volume2(0); // for second interface
	TPckg<TUint> volume2Des(volume2);
	TUint threshold2(KLowThresholdSize*2); // not the same as threshold1

	conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);
	conn2.DataSentNotificationRequest(threshold2, volume2Des, status2);

	//********** send data to cause a notification ***********

	TUint currentVol(0);

	while (currentVol < threshold1)
	{
		TESTEL(KRequestPending == status1.Int(), status1.Int());
		TESTEL(KRequestPending == status2.Int(), status2.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// notification on first connection should have completed, notification on second should still be pending
	User::WaitForRequest(status1);
	TESTEL(KErrNone == status1.Int(), status1.Int());
	TESTEL(KRequestPending == status2.Int(), status2.Int());

	//********** cancel the second notification request ***********

	conn2.DataSentNotificationCancel();
	User::WaitForRequest(status2);
	
	TESTEL(KErrCancel == status2.Int(), status2.Int());

	//********** do everything again the other way round ***********

	// should have two interfaces
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	//********** make calls to DataSentNotificationRequest ***********

	volume1 = 0; // for first interface
	volume2 = 0; // for second interface

	conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);
	conn2.DataSentNotificationRequest(threshold2, volume2Des, status2);

	//********** need to check amount of data sent (on connection 2) to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolSent(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolSent = currentVolSent;

	//********** create socket on other connection ***********

	RSocket sock2;

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	//********** send data to cause a notification ***********

	while ((currentVolSent - startVolSent) < threshold2)
	{
		TESTEL(KRequestPending == status1.Int(), status1.Int());
		TESTEL(KRequestPending == status2.Int(), status2.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	// notification on second connection should have completed, notification on first should still be pending
	User::WaitForRequest(status2);
	TESTEL(KRequestPending == status1.Int(), status1.Int());
	TESTEL(KErrNone == status2.Int(), status2.Int());

	//********** cancel the first notification request ***********

	conn1.DataSentNotificationCancel();
	User::WaitForRequest(status1);

	TESTEL(KErrCancel == status1.Int(), status1.Int());

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest426


enum TVerdict TE_RConnectionTest427::doTestStepL(void)
/**
 * Test 427
 * Test DataReceivedNotificationRequest() when multiple interfaces are dealing with requests 
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;
	TRequestStatus status1, status2;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	//********** socket activity ***********

	RSocket sock1;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

	TESTEL(KErrNone == err, err);

	//********** make calls to DataReceivedNotificationRequest ***********

	TUint volume1(0); // for first interface
	TPckg<TUint> volume1Des(volume1);
	TUint threshold1(KLowThresholdSize);

	TUint volume2(0); // for second interface
	TPckg<TUint> volume2Des(volume2);
	TUint threshold2(KLowThresholdSize*2); // not the same as threshold1

	conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);
	conn2.DataReceivedNotificationRequest(threshold2, volume2Des, status2);

	//********** send data to cause a notification ***********

	TUint currentVol(0);

	while (currentVol < threshold1)
	{
		TESTEL(KRequestPending == status1.Int(), status1.Int());
		TESTEL(KRequestPending == status2.Int(), status2.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// notification on first connection should have completed, notification on second should still be pending
	User::WaitForRequest(status1);
	TESTEL(KErrNone == status1.Int(), status1.Int());
	TESTEL(KRequestPending == status2.Int(), status2.Int());

	INFO_PRINTF1(_L("Success: first notification completed, second still pending"));

	//********** cancel the second notification request ***********

	conn2.DataReceivedNotificationCancel();
	User::WaitForRequest(status2);

	TESTEL(KErrCancel == status2.Int(), status2.Int());

	//********** do everything again the other way round ***********

	//********** make calls to DataReceivedNotificationRequest ***********

	volume1 = 0; // for first interface
	volume2 = 0; // for second interface

	conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);
	conn2.DataReceivedNotificationRequest(threshold2, volume2Des, status2);

	//********** need to check amount of data received (on connection 2) to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolRecv(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolRecv = currentVolRecv;

	//********** open the socket on the second connection **********

	RSocket sock2;

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	//********** send data to cause a notification **********

	while ((currentVolRecv - startVolRecv) < threshold2)
	{
		TESTEL(KRequestPending == status1.Int(), status1.Int());
		TESTEL(KRequestPending == status2.Int(), status2.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	// notification on second connection should have completed, notification on first should still be pending
	User::WaitForRequest(status2);
	TESTEL(KRequestPending == status1.Int(), status1.Int());
	TESTEL(KErrNone == status2.Int(), status2.Int());

	INFO_PRINTF1(_L("Success: second notification completed, first still pending"));

	//********** cancel the first notification request ***********

	conn1.DataReceivedNotificationCancel();
	User::WaitForRequest(status1);

	TESTEL(KErrCancel == status1.Int(), status1.Int());

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest427


enum TVerdict TE_RConnectionTest428::doTestStepL(void)
/**
 * Test 428
 * Explicitly create a single test NIF connection (using commdb settings) enumerate subconnections and get subconnection info (both with and without subconnection ID).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif

	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	//********** check we only have one connection ***********

	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	//********** should have two subconnections - a primary and the complete connection ***********

	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	//********** try out getconnectioninfo and getsubconnectioninfo ***********

	TPckgBuf<TConnectionInfo> connectionInfo;
	err = conn.GetConnectionInfo(1, connectionInfo);
	TESTEL(KErrNone == err, err);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;

	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	//********** try out getsubconnectioninfo again ***********

	err = conn.GetSubConnectionInfo(subconnectionInfo); // subconnectionID should still be inside subconnectionInfo
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest428


enum TVerdict TE_RConnectionTest429::doTestStepL(void)
/**
 * Test 429
 * AllSubConnectionNotification test using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	SetTestStepResult(EFail);
	
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// issue request for allsubconnectionnotification
	TRequestStatus status;
	TSubConnectionNotificationBuf subconnectionNotification;
	conn.AllSubConnectionNotification(subconnectionNotification, status);

	//********** we are testing allsubconnectionnotification here ***********

	User::WaitForRequest(status);
	// event should be ESubConnectionOpened
	TSubConnectionEvent& event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TSubConnectionEventType type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);

	// should have one interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	//********** there should be a second subconnection opened event ***********

	// issue request for allsubconnectionnotification
	conn.AllSubConnectionNotification(subconnectionNotification, status);

	User::WaitForRequest(status);
	// event should be ESubConnectionOpened
	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);

	// should have one interface (but two subconnections)
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	//********** issue request ready for connection closure ***********

	// issue request for allsubconnectionnotification
	conn.AllSubConnectionNotification(subconnectionNotification, status);

	//********** stop connection and mop up events ***********

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	// event should be ESubConnectionClosed
	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	type = event.iEventType;
	// should get event for primary before complete connection
	TESTE(event.iSubConnectionUniqueId > KNifEntireConnectionSubConnectionId, event.iSubConnectionUniqueId);
	TESTEL(ESubConnectionClosed == type, type);

	// there should be another one
	conn.AllSubConnectionNotification(subconnectionNotification, status);

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	// event should be ESubConnectionClosed
	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	type = event.iEventType;
	// this should be for the complete connection
	TESTE(event.iSubConnectionUniqueId == KNifEntireConnectionSubConnectionId, event.iSubConnectionUniqueId);
	TESTEL(ESubConnectionClosed == type, type);

	User::After(1000000);

	// should have no interfaces any more
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(0 == connectionCount, connectionCount);

	//********** clean up ***********

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	SetTestStepResult(EPass);
	return TestStepResult();

} // TE_RConnectionTest429


enum TVerdict TE_RConnectionTest430::doTestStepL(void)
/**
 * Test 430
 * Test of cancellation of AllSubConnectionNotification when using the test NIF
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	StartConnectionWithOverrides(conn, iTestNifIap);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** first need to mop up the buffered events ***********

	TRequestStatus status;
	TSubConnectionNotificationBuf subconnectionNotification;

	// should get ESubConnectionOpened
	conn.AllSubConnectionNotification(subconnectionNotification, status);
	User::WaitForRequest(status);
	TSubConnectionEvent& event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TSubConnectionEventType type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);
	INFO_PRINTF1(_L("Received first Notification"));

	// should get another one
	conn.AllSubConnectionNotification(subconnectionNotification, status);
	User::WaitForRequest(status);
	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);
	INFO_PRINTF1(_L("Received second Notification"));

	//********** now issue request again and cancel it ***********

	conn.AllSubConnectionNotification(subconnectionNotification, status);
	conn.CancelAllSubConnectionNotification();
	User::WaitForRequest(status);

	TESTEL(KErrCancel == status.Int(), status.Int());
	INFO_PRINTF1(_L("Received cancel Notification"));

	//********** clean up ***********

	INFO_PRINTF1(_L("Cleanup"));

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest430


enum TVerdict TE_RConnectionTest431::doTestStepL(void)
/**
 * Test 431
 * Test of DataTransferredRequest() when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);
	
	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);

	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Socket connected"));

	//********** test DataTransferredRequest ***********

	TUint uplinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TUint downlinkVolume(0);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TRequestStatus status;

	conn.DataTransferredRequest(uplinkVolumeDes, downlinkVolumeDes, status);
	User::WaitForRequest(status);
	TESTEL(uplinkVolume >= KTotalTcpTestPacketSize, uplinkVolume);
	TESTEL(downlinkVolume >= KTotalTcpTestPacketSize, downlinkVolume);

	//********** clean up ***********

	INFO_PRINTF1(_L("Clean Up"));

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest431


enum TVerdict TE_RConnectionTest432::doTestStepL(void)
/**
 * Test 432
 * Test of DataTransferredRequest() for subconnection when using test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);
	
	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);

	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	// should have one connection
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	// get subconnection info to get the unique id
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	TSubConnectionUniqueId subConnectionUniqueId;

	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	INFO_PRINTF1(_L("Enumerate SubConnections OK"));

	//********** test datatransferredrequest ***********

	TUint uplinkVolume(0);
	TUint downlinkVolume(0);
	TPckg<TUint> uplinkVolumeDes(uplinkVolume);
	TPckg<TUint> downlinkVolumeDes(downlinkVolume);
	TRequestStatus status;

	// for specific subconnection
	conn.DataTransferredRequest(subConnectionUniqueId, uplinkVolumeDes, downlinkVolumeDes, status);
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(uplinkVolume >= KTotalTcpTestPacketSize, uplinkVolume);
	TESTEL(downlinkVolume >= KTotalTcpTestPacketSize, downlinkVolume);

	INFO_PRINTF1(_L("Data Transfer OK"));

	//********** clean up ***********

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest432


enum TVerdict TE_RConnectionTest433::doTestStepL(void)
/**
 * Test 433
 * Test of DataSentNotification() request when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);
	
	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Socket Connected"));

	//********** issue the data sent notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn.DataSentNotificationRequest(threshold, volumeDes, status);

	//********** need to check amount of data sent to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolSent(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolSent = currentVolSent;

	//********** send data to cause a notification **********

	while ((currentVolSent - startVolSent) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock);

		TESTEL(KErrNone == err, err);

		conn.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	//********** check for correct value ***********

	TESTEL(volume >= threshold, volume);

	INFO_PRINTF1(_L("DataSentNotificationRequest completed sucessfully"));

	//********** clean up ***********

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest433


enum TVerdict TE_RConnectionTest434::doTestStepL(void)
/**
 * Test 434
 * Test of DataSentNotification() request for subconnections when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data sent notification request ***********

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn.DataSentNotificationRequest(subConnectionUniqueId, threshold, volumeDes, status);

	//********** need to check amount of data sent to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolSent(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn.DataTransferredRequest(subConnectionUniqueId, currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolSent = currentVolSent;

	//********** send data to cause a notification ***********

	while ((currentVolSent - startVolSent) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock);

		TESTEL(KErrNone == err, err);

		conn.DataTransferredRequest(subConnectionUniqueId, currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	//********** check for correct value ***********

	TESTEL(volume >= threshold, volume);

	INFO_PRINTF1(_L("DataSentNotificationRequest completed sucessfully"));

	//********** clean up ***********

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest434


enum TVerdict TE_RConnectionTest435::doTestStepL(void)
/**
 * Test 435
 * Test of DataSentNotification() cancellation on a connection when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//********** issue the data sent notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn.DataSentNotificationRequest(threshold, volumeDes, status);

	//********** cancel the request ***********

	conn.DataSentNotificationCancel();

	User::WaitForRequest(status);
	TESTEL(KErrCancel == status.Int(), status.Int());

	INFO_PRINTF1(_L("Notification Cancel Complete"));

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest435


enum TVerdict TE_RConnectionTest436::doTestStepL(void)
/**
 * Test 436
 * Test of DataSentNotification cancellation for subconnections when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//********** issue the data sent notification request ***********

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn.DataSentNotificationRequest(subConnectionUniqueId, threshold, volumeDes, status);

	//********** cancel the request ***********

	conn.DataSentNotificationCancel(subConnectionUniqueId);

	User::WaitForRequest(status);
	TESTEL(KErrCancel == status.Int(), status.Int());

	INFO_PRINTF1(_L("Notification Cancel Complete"));

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest436


enum TVerdict TE_RConnectionTest437::doTestStepL(void)
/**
 * Test 437
 * Test of DataReceivedNotification on a connection when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);
	
	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	//********** issue the data received notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn.DataReceivedNotificationRequest(threshold, volumeDes, status);

	//********** need to check amount of data received to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolRecv(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolRecv = currentVolRecv;

	//********** send data to cause a notification ***********

	while ((currentVolRecv - startVolRecv) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock);

		TESTEL(KErrNone == err, err);

		conn.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	//********** check for correct value ***********

	TESTEL(volume >= threshold, volume);

	INFO_PRINTF1(_L("DataReceivedNotification completed successfully"));

	//********** clean up ***********

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest437


enum TVerdict TE_RConnectionTest438::doTestStepL(void)
/**
 * Test 438
 * Test of DataReceivedNotification() request for subconnections when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	//********** issue the data received notification request ***********

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn.DataReceivedNotificationRequest(subConnectionUniqueId, threshold, volumeDes, status);

	//********** need to check amount of data received to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolRecv(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn.DataTransferredRequest(subConnectionUniqueId, currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolRecv = currentVolRecv;

	//********** send data to cause a notification ***********

	while ((currentVolRecv - startVolRecv) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock);

		TESTEL(KErrNone == err, err);

		conn.DataTransferredRequest(subConnectionUniqueId, currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	//********** check for correct value ***********

	TESTEL(volume >= threshold, volume);

	INFO_PRINTF1(_L("DataReceivedNotification completed successfully"));

	//********** clean up ***********

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest438



enum TVerdict TE_RConnectionTest439::doTestStepL(void)
/**
 * Test 439
 * Test of DataReceivedNotification() cancellation on a connection when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// not really necessary as we won't send traffic, but included for completeness
	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//********** issue the data received notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn.DataReceivedNotificationRequest(threshold, volumeDes, status);

	//********** cancel the request ***********

	conn.DataReceivedNotificationCancel();

	User::WaitForRequest(status);
	TESTEL(KErrCancel == status.Int(), status.Int());

	INFO_PRINTF1(_L("Notification Cancel Complete"));

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest439

enum TVerdict TE_RConnectionTest440::doTestStepL(void)
/**
 * Test 440
 * Test of DataReceivedNotification() cancellation for subconnections when using the test NIF.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// not really necessary as we won't send traffic, but included for completeness
	err = ProdQoSL(conn, ss);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********
	
	RSocket sock;

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//********** issue the data received notification request ***********

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn.DataReceivedNotificationRequest(subConnectionUniqueId, threshold, volumeDes, status);

	//********** cancel the request ***********

	conn.DataReceivedNotificationCancel(subConnectionUniqueId);

	User::WaitForRequest(status);
	TESTEL(KErrCancel == status.Int(), status.Int());

	INFO_PRINTF1(_L("Notification Cancel Complete"));

	//********** clean up ***********

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest440



enum TVerdict TE_RConnectionTest441::doTestStepL(void)
/**
 * Test 441
 * Test DataTransferredRequest for subconnection with primary and secondary contexts present
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass);
	return TestStepResult();

} // TE_RConnectionTest441




enum TVerdict TE_RConnectionTest442::doTestStepL(void)
/**
 * Test 442
 * Test DataTransferred Request() for subconnection (primary context) with a secondary context present.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	
	return TestStepResult();

} // TE_RConnectionTest442


enum TVerdict TE_RConnectionTest443::doTestStepL(void)
/**
 * Test 443
 * Test DataTransferredRequest() for subconnection (secondary context) with another secondary context present.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	
	return TestStepResult();

} // TE_RConnectionTest443



enum TVerdict TE_RConnectionTest444::doTestStepL(void)
/**
 * Test 444
 * Test DataTransferredRequest() for complete connection when multiple subconnections are active.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest444


enum TVerdict TE_RConnectionTest445::doTestStepL(void)
/**
 * Test 445
 * Test for DataTransferred Request on one context then another
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest445



enum TVerdict TE_RConnectionTest446::doTestStepL(void)
/**
 * Test 446
 * Test DataSentNotification() for a secondary context while the primary is also active.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest446



enum TVerdict TE_RConnectionTest447::doTestStepL(void)
/**
 * Test 447
 * Test DataSentNotification() for primary context while a secondary is also active.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest447



enum TVerdict TE_RConnectionTest448::doTestStepL(void)
/**
 * Test 448
 * Test DataSentNotification() for complete connection while multiple subconnections are active and passing traffic
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest448



enum TVerdict TE_RConnectionTest449::doTestStepL(void)
/**
 * Test 449
 * Test for DataSentNotification on one context then another
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest449



enum TVerdict TE_RConnectionTest450::doTestStepL(void)
/**
 * Test 450
 * Tests for multiple parallel DataSentNotification requests and cancellation with multiple contexts
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest450



enum TVerdict TE_RConnectionTest451::doTestStepL(void)
/**
 * Test 451
 * Test DataReceivedNotification() for a secondary context while the primary is also active.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest451


enum TVerdict TE_RConnectionTest452::doTestStepL(void)
/**
 * Test 452
 * Test DataReceivedNotification() for primary context while a secondary is also active.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest452


enum TVerdict TE_RConnectionTest453::doTestStepL(void)
/**
 * Test 453
 * Test DataReceivedNotification() for complete connection while multiple subconnections are active and passing traffic
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest453



enum TVerdict TE_RConnectionTest454::doTestStepL(void)
/**
 * Test 454
 * Test for DataReceivedNotification on one context then another
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest454



enum TVerdict TE_RConnectionTest455::doTestStepL(void)
/**
 * Test 455
 * Tests for multiple parallel DataReceivedNotification requests and cancellation with multiple contexts
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	

	return TestStepResult();

} // TE_RConnectionTest455



enum TVerdict TE_RConnectionTest456::doTestStepPreambleL(void)
/**
 * echo daemon will open a connection, so we dont want to stop them all for test456
 */
{
	enum TVerdict result = EPass;
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	if (StartC32WithCMISuppressions(KPhbkSyncCMI) != KErrNone)
    	{
    	return EFail;
    	}
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	/*
	 * Load the config from the .ini file
	 */

	if (KErrNone != ReadIniFile())
		result = EFail;
	
	return result;
}

enum TVerdict TE_RConnectionTest456::doTestStepL(void)
/**
 * Test 456
 * AllSubConnectionNotification test with multiple subconnections.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;
	TRequestStatus status;
	TRequestStatus status2;

	INFO_PRINTF1(_L("Test Setup"));
	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection opening ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//********** now start the interface ***********
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	//********** we want to test allsubconenctionnotification ***********

	TSubConnectionNotificationBuf subconnectionNotification;
	conn.AllSubConnectionNotification(subconnectionNotification, status);

	// should have a single interface
	INFO_PRINTF1(_L("Test Connections"));
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	//********** request for notification should have completed ***********

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	// event should be ESubConnectionOpened
	TSubConnectionEvent& event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TSubConnectionEventType type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);

	//********** there should be a second subconnection opened event ***********

	// issue request for allsubconnectionnotification
	conn.AllSubConnectionNotification(subconnectionNotification, status);

	User::WaitForRequest(status);
	// event should be ESubConnectionOpened
	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);

	//********** issue the request again ***********

	conn.AllSubConnectionNotification(subconnectionNotification, status2);
	TESTEL(KRequestPending == status2.Int(), status2.Int());

	//********** need a connected socket ***********

	RSocket sock1;

	err = OpenTcpSocketExplicit(sock1, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = ConnectTcpSocket(sock1, iEchoServerAddr);
	TESTEL(KErrNone == err, err);
	
	//********** open a QoS channel ***********
	INFO_PRINTF1(_L("Open QoS Channel"));
	TS_QosChannel channel(*this);
	channel.ConstructL();
	err = channel.Open(ss,conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(channel);


	//********** add socket ***********
	INFO_PRINTF1(_L("Add Socket To QoS Flow"));
	err = channel.Add(sock1);
	TESTEL(KErrNone == err, err);
	
	//********** set params ***********
	INFO_PRINTF1(_L("Set QoS Parameters"));
	err = channel.SetQoSParametersL();
	TESTE(KErrNone == err, err);

	// should have 3 subconnections (the whole connection, primary and secondary)
	INFO_PRINTF1(_L("Test Connections"));
	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(3 == subconnectionCount, subconnectionCount);

	//********** request for notification should have completed ***********

	User::WaitForRequest(status2);
	TESTEL(KErrNone == status2.Int(), status2.Int());

	// event should be ESubConnectionOpened
	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	type = event.iEventType;
	TESTEL(ESubConnectionOpened == type, type);

	//********** going to shut down, issue request yet again ***********

	conn.AllSubConnectionNotification(subconnectionNotification, status);
	TESTEL(KRequestPending == status.Int(), status.Int());

	//********** blow away the interface ***********

	err = conn.Stop(RConnection::EStopAuthoritative);
	TESTEL(KErrNone == err, err);

	//********** mop up the events ***********

	User::WaitForRequest(status);

	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TESTEL(ESubConnectionClosed == event.iEventType, event.iEventType);

	// there should be another one, issue request again
	conn.AllSubConnectionNotification(subconnectionNotification, status);
	
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	event = (TSubConnectionEvent&)(*subconnectionNotification.Ptr());
	TESTEL(ESubConnectionClosed == event.iEventType, event.iEventType);

	//********** clean up ***********
	INFO_PRINTF1(_L("Clean Up"));
	
	err = channel.Close();
	TESTEL(KErrNone == err, err);
	CleanupStack::Pop(&channel);	

	err = DestroyTcpSocket(sock1);
	TESTEL(KErrConnectionTerminated == err, err);
	CleanupStack::Pop(&sock1);
	
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
	
} // TE_RConnectionTest456



enum TVerdict TE_RConnectionTest457::doTestStepL(void)
/**
 * Test 457
 * Simple test with two interfaces - one which supports the new M classes one which doesn't. Try out DataTransferredRequest for each.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the test nif
	err = StartConnectionWithOverrides(conn2, iTestNifIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** alternately send packets over each interface and making DataTransferredRequest calls ***********

	TUint uplink1(0);
	TUint uplink1Prev(0);
	TUint downlink1(0);
	TUint downlink1Prev(0);
	TUint uplink2(0);
	TUint uplink2Prev(0);
	TUint downlink2(0);
	TUint downlink2Prev(0);
	TPckg<TUint> uplink1Des(uplink1);
	TPckg<TUint> downlink1Des(downlink1);
	TPckg<TUint> uplink2Des(uplink2);
	TPckg<TUint> downlink2Des(downlink2);
	TRequestStatus status;

	// read the counters first to set the previous values
	conn1.DataTransferredRequest(uplink1Des, downlink1Des, status);
	User::WaitForRequest(status);
	conn2.DataTransferredRequest(uplink2Des, downlink2Des, status);
	User::WaitForRequest(status);
	uplink1Prev = uplink1;
	downlink1Prev = downlink1;
	uplink2Prev = uplink2;
	downlink2Prev = downlink2;

	for (TUint i=0; i<NUM_OF_REPETITIONS; i++)
	{
		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KBufferLength);

		TESTEL(KErrNone == err, err);

		// update the counters
		conn1.DataTransferredRequest(uplink1Des, downlink1Des, status);
		User::WaitForRequest(status);
		conn2.DataTransferredRequest(uplink2Des, downlink2Des, status);
		User::WaitForRequest(status);

		TESTEL(uplink1 == (uplink1Prev + KTotalUdpTestPacketSize), uplink1); // stats for first connection should have increased
		TESTEL(downlink1 == (downlink1Prev + KTotalUdpTestPacketSize), downlink1);
		TESTEL(uplink2 < (uplink2Prev + KTotalUdpTestPacketSize), uplink2); // stats for second connection should be unchanged
		TESTEL(downlink2 < (downlink2Prev + KTotalUdpTestPacketSize), downlink2);

		uplink1Prev = uplink1;
		downlink1Prev = downlink1;

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		User::After(E_DELAY); // pause to allow any retried packets to arrive

		// update the counters
		conn1.DataTransferredRequest(uplink1Des, downlink1Des, status);
		User::WaitForRequest(status);
		conn2.DataTransferredRequest(uplink2Des, downlink2Des, status);
		User::WaitForRequest(status);

		TESTEL(uplink1 == uplink1Prev, uplink1); // stats for first connection should be unchanged
		TESTEL(downlink1 == downlink1Prev, downlink1);
		TESTEL(uplink2 > uplink2Prev, uplink2); // stats for second connection should have increased
		TESTEL(downlink2 > downlink2Prev, downlink2);

		uplink2Prev = uplink2;
		downlink2Prev = downlink2;
	}

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest457


enum TVerdict TE_RConnectionTest458::doTestStepL(void)
/**
 * Test 458
 * Use DataSentNotification while two interfaces (one supporting the M class interface and one not) are active - make request on NIF which does support the extra interface.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the test nif
	err = StartConnectionWithOverrides(conn2, iTestNifIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets connected"));

	//********** issue the data sent notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn1.DataSentNotificationRequest(threshold, volumeDes, status);

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold 
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		// is a send notification request so we can assume these packets actually got sent rather than worrying 
		// about monitoring it more accurately through DataTransferredRequest()
		currentVol += KTotalTcpTestPacketSize;
	}

	INFO_PRINTF2(_L("Success: DataSentNotificationRequest not completed"), currentVol);

	//********** send data over the first connection to complete the request ***********

	currentVol = 0;

	while (currentVol < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume == currentVol, volume);
	
	INFO_PRINTF2(_L("Success: DataSentNotificationRequest completed"), currentVol);

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest458

enum TVerdict TE_RConnectionTest459::doTestStepL(void)
/**
 * Test 459
 * Use DataSentNotification while two interfaces (one supporting the M class interface and one not) are active - make request on NIF which does support the extra interface.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the test nif
	err = StartConnectionWithOverrides(conn2, iTestNifIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);
	
	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data sent notification request on the second interface ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn2.DataSentNotificationRequest(threshold, volumeDes, status);

	//********** need to check amount of data sent (on connection 2) to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolSent(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolSent = currentVolSent;

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	INFO_PRINTF2(_L("Success: DataSentNotificationRequest not completed"), currentVol);

	//********** send data over the second connection now to complete the request ***********

	while ((currentVolSent - startVolSent) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume >= threshold, volume);
	
	INFO_PRINTF2(_L("Success: DataSentNotificationRequest completed"), currentVol);

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnection459


enum TVerdict TE_RConnectionTest460::doTestStepL(void)
/**
 * Test 460
 * Use DataReceivedNotification while two interfaces (one supporting the M class inteface and one not) are active - make request on NIF not supporting extra interface.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the test nif
	err = StartConnectionWithOverrides(conn2, iTestNifIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data received notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn1.DataReceivedNotificationRequest(threshold, volumeDes, status);

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		// is a send notification request so we can assume these packets actually got sent rather than worrying 
		// about monitoring it more accurately through DataTransferredRequest()
		currentVol += KTotalTcpTestPacketSize;
	}

	INFO_PRINTF2(_L("Success: DataReceivedNotificationRequest not completed"), currentVol);

	//********** send data over the first connection to complete the request ***********

	currentVol = 0;

	while (currentVol < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume == currentVol, volume);
	
	INFO_PRINTF2(_L("Success: DataReceivedNotificationRequest completed"), currentVol);

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnection460


enum TVerdict TE_RConnectionTest461::doTestStepL(void)
/**
 * Test 461
 * Use DataReceivedNotification while two interfaces (one supporting the M class inteface and one not) are active - make request on NIF which does support the interface.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the test nif
	err = StartConnectionWithOverrides(conn2, iTestNifIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data received notification request on the second interface ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KLowThresholdSize);
	TRequestStatus status;

	conn2.DataReceivedNotificationRequest(threshold, volumeDes, status);

	//********** need to check amount of data sent (on connection 2) to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolRecv(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolRecv = currentVolRecv;

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	INFO_PRINTF2(_L("Success: DataReceivedNotificationRequest not completed"), currentVol);

	//********** send data over the second connection now to complete the request ***********

	while ((currentVolRecv - startVolRecv) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume >= threshold, volume);
	
	INFO_PRINTF2(_L("Success: DataSentNotificationRequest completed"), currentVol);

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyUdpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnection461


enum TVerdict TE_RConnectionTest462::doTestStepL(void)
/**
 * Test 462
 * Simple test with two interfaces neither of which support the extended management interface. Try out DataTransferredRequest for  each.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** alternately send packets over each interface and making DataTransferredRequest calls ***********

	TUint uplink1(0);
	TUint uplink1Prev(0);
	TUint downlink1(0);
	TUint downlink1Prev(0);
	TUint uplink2(0);
	TUint uplink2Prev(0);
	TUint downlink2(0);
	TUint downlink2Prev(0);
	TPckg<TUint> uplink1Des(uplink1);
	TPckg<TUint> downlink1Des(downlink1);
	TPckg<TUint> uplink2Des(uplink2);
	TPckg<TUint> downlink2Des(downlink2);
	TRequestStatus status;

	for (TUint i=0; i<NUM_OF_REPETITIONS; i++)
	{
		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);
		TESTEL(KErrNone == err, err);

		// update the counters
		conn1.DataTransferredRequest(uplink1Des, downlink1Des, status);
		User::WaitForRequest(status);
		conn2.DataTransferredRequest(uplink2Des, downlink2Des, status);
		User::WaitForRequest(status);

		TESTEL(uplink1 == (uplink1Prev + KSmallBufferUdpTestPacketSize), uplink1); // stats for first connection should have increased
		TESTEL(downlink1 == (downlink1Prev + KSmallBufferUdpTestPacketSize), downlink1);
		TESTEL(uplink2 < (uplink2Prev + KTotalTcpTestPacketSize), uplink2); // stats for second connection should be unchanged

		TESTEL(downlink2 < (downlink2Prev + KTotalTcpTestPacketSize), downlink2);

		uplink1Prev = uplink1;
		downlink1Prev = downlink1;

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		User::After(E_DELAY); // pause to allow any retried packets to arrive

		// update the counters
		conn1.DataTransferredRequest(uplink1Des, downlink1Des, status);
		User::WaitForRequest(status);
		conn2.DataTransferredRequest(uplink2Des, downlink2Des, status);
		User::WaitForRequest(status);

		TESTEL(uplink1 == uplink1Prev, uplink1); // stats for first connection should be unchanged
		TESTEL(downlink1 == downlink1Prev, downlink1);
		TESTEL(uplink2 >= uplink2Prev + KTotalTcpTestPacketSize, uplink2); // stats for second connection should have increased
		TESTEL(downlink2 >= downlink2Prev + KTotalTcpTestPacketSize, downlink2);

		uplink2Prev = uplink2;
		downlink2Prev = downlink2;
	}

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyUdpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest462


enum TVerdict TE_RConnectionTest463::doTestStepL(void)
/**
 * Test 463
 * Two interfaces neither of which support extended management interface. Parallel use of DataTransferredRequest
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** send data over each connection ***********

	// send more data over the second connection than over the first
	err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock2); // this uses packets of size KBufferLength

	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue data transferred requests for each connection ***********

	TUint uplink1(0);
	TUint downlink1(0);
	TUint uplink2(0);
	TUint downlink2(0);
	TPckg<TUint> uplink1Des(uplink1);
	TPckg<TUint> downlink1Des(downlink1);
	TPckg<TUint> uplink2Des(uplink2);
	TPckg<TUint> downlink2Des(downlink2);
	TRequestStatus status1, status2;

	INFO_PRINTF1(_L("issue data transferred requests for each connection"));

	conn1.DataTransferredRequest(uplink1Des, downlink1Des, status1);
	conn2.DataTransferredRequest(uplink2Des, downlink2Des, status2);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	//********** test the stats are reasonable ***********

	TESTEL(uplink1 == KSmallBufferLength+KUdpPacketOverhead, uplink1);
	TESTEL(downlink1 == KSmallBufferLength+KUdpPacketOverhead, downlink1);
	TESTEL(uplink2 >= KTotalTcpTestPacketSize, uplink2);
	TESTEL(downlink2 >= KTotalTcpTestPacketSize, downlink2);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test CleanUp"));
	
	DestroyUdpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest463


enum TVerdict TE_RConnectionTest464::doTestStepL(void)
/**
 * Test 464
 * Use DataSentNotification while two interfaces are active (neither of which supports the extended management interface). - make request on first interface created
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// small delay
	User::After(E_DELAY);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data sent notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn1.DataSentNotificationRequest(threshold, volumeDes, status);

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		// is a send notification request so we can assume these packets actually got sent rather than worrying 
		// about monitoring it more accurately through DataTransferredRequest()
		currentVol += KTotalTcpTestPacketSize;
	}

	INFO_PRINTF2(_L("Success: DataSentNotificationRequest not completed currentVol = %d"), currentVol);

	//********** send data over the first connection to complete the request ***********

	currentVol = 0;

	while (currentVol < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume == currentVol, volume);

	INFO_PRINTF2(_L("Success: DataSentNotificationRequest completed"), currentVol);
	
	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest464


enum TVerdict TE_RConnectionTest465::doTestStepL(void)
/**
 * Test 465
 * Use DataSentNotification while two interfaces are active (neither of which supports the extended management interface). - make request on second interface created.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data sent notification request on second interface ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn2.DataSentNotificationRequest(threshold, volumeDes, status);

	//********** need to check amount of data sent to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolSent(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolSent = currentVolSent;

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	INFO_PRINTF1(_L("Success: DataSentNotificationRequest not completed"));

	//********** send data over the second connection to complete the request ***********

	while ((currentVolSent - startVolSent) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume >= threshold, volume);

	INFO_PRINTF1(_L("Success: DataSentNotificationRequest completed"));
	
	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest465


enum TVerdict TE_RConnectionTest466::doTestStepL(void)
/**
 * Test 466
 * Use DataReceivedNotification while two interfaces are active (neither of which supports the extended management interface). - make request on first interface created.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Setup Complete"));

	// small delay
	User::After(E_DELAY);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data received notification request ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn1.DataReceivedNotificationRequest(threshold, volumeDes, status);

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		// is a send notification request so we can assume these packets actually got sent rather than worrying 
		// about monitoring it more accurately through DataTransferredRequest()
		currentVol += KTotalTcpTestPacketSize;
	}

	INFO_PRINTF1(_L("Success: DataReceivedNotificationRequest not completed"));

	//********** send data over the first connection to complete the request ***********

	currentVol = 0;

	while (currentVol < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume == currentVol, volume);
	
	INFO_PRINTF2(_L("Success: DataReceivedNotificationRequest completed"), currentVol);

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest466


enum TVerdict TE_RConnectionTest467::doTestStepL(void)
/**
 * Test 467
 * Use DataReceivedNotification while two interfaces are active (neither of which supports the extended management interface). - make request on second interface created.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifIap);
	TESTEL(KErrNone == err, err);
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have two interfaces
	TUint connectionCount;
	err = conn1.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket activity ***********

	RSocket sock1, sock2;

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenTcpSocketExplicit(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** issue the data received notification request on second interface ***********

	TUint volume(0);
	TPckg<TUint> volumeDes(volume);
	TUint threshold(KNormalThresholdSize);
	TRequestStatus status;

	conn2.DataReceivedNotificationRequest(threshold, volumeDes, status);

	//********** need to check amount of data sent to know when request should complete ***********

	TUint currentVolSent(0);
	TUint currentVolRecv(0);
	TUint startVolRecv(0);
	TPckg<TUint> currentVolSentDes(currentVolSent);
	TPckg<TUint> currentVolRecvDes(currentVolRecv);
	TRequestStatus statsStatus;

	conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
	User::WaitForRequest(statsStatus);

	startVolRecv = currentVolRecv;

	//********** send data over the other connection (more than the threshold amount) ***********
	
	TUint currentVol = 0;

	while (currentVol < (threshold*2)) // multiplying by 2 makes the final amount of data sent much larger than the actual threshold
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);

		TESTEL(KErrNone == err, err);

		currentVol += KSmallBufferUdpTestPacketSize;
	}

	INFO_PRINTF1(_L("Success: DataReceivedNotificationRequest not completed"));

	//********** send data over the second connection to complete the request ***********

	while ((currentVolRecv - startVolRecv) < threshold)
	{
		TESTEL(KRequestPending == status.Int(), status.Int());

		err = TestTcpDataPathL(sock2);

		TESTEL(KErrNone == err, err);

		conn2.DataTransferredRequest(currentVolSentDes, currentVolRecvDes, statsStatus);
		User::WaitForRequest(statsStatus);
	}

	// notification request should now have completed
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());
	TESTEL(volume >= (currentVolRecv - startVolRecv), volume);
	
	INFO_PRINTF1(_L("Success: DataReceivedNotificationRequest completed"));

	//********** turn the idle timers back on now we are finished ***********

	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyUdpSocket(sock2);
	CleanupStack::Pop(&sock2);

	DestroyUdpSocket(sock1);
	CleanupStack::Pop(&sock1);

	err = conn2.Stop();
	TESTEL(KErrNone == err, err);

	err = conn1.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest467



enum TVerdict TE_RConnectionTest468::doTestStepL(void)
/**
 * Test 468
 * Use AllSubConnectionNotification while two interfaces are active (neither of which supports the extended management interface).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** first connection creation and issuing of allsubconnectionnotification request ***********

	RConnection conn1;
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	// start the dummynif
	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Setup Complete"));

	// issue request for allsubconnectionnotification
	TRequestStatus status1;
	TSubConnectionNotificationBuf subconnectionNotification1;
	conn1.AllSubConnectionNotification(subconnectionNotification1, status1);

	User::WaitForRequest(status1);
	// event should be ESubConnectionOpened
	TSubConnectionEvent& event1 = (TSubConnectionEvent&)(*subconnectionNotification1.Ptr());
	TSubConnectionEventType type1 = event1.iEventType;
	TESTEL(ESubConnectionOpened == type1, type1);

	// there should be another ESubConnectionOpened event left in a queue
	conn1.AllSubConnectionNotification(subconnectionNotification1, status1);

	User::WaitForRequest(status1);
	event1 = (TSubConnectionEvent&)(*subconnectionNotification1.Ptr());
	type1 = event1.iEventType;
	TESTEL(ESubConnectionOpened == type1, type1);

	INFO_PRINTF1(_L("First Notification Complete"));

	//********** second connection creation and issuing of allsubconnectionnotification request ***********

	RConnection conn2;
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// start the ppp nif
	err = StartConnectionWithOverrides(conn2, iDefaultIap);
	TESTEL(KErrNone == err, err);

	// issue request for allsubconnectionnotification
	TRequestStatus status2;
	TSubConnectionNotificationBuf subconnectionNotification2;
	conn2.AllSubConnectionNotification(subconnectionNotification2, status2);

	User::WaitForRequest(status2);
	// event should be ESubConnectionOpened
	TSubConnectionEvent& event2 = (TSubConnectionEvent&)(*subconnectionNotification2.Ptr());
	TSubConnectionEventType type2 = event2.iEventType;
	TESTEL(ESubConnectionOpened == type2, type2);

	// there should be another ESubConnectionOpened event left in a queue
	conn2.AllSubConnectionNotification(subconnectionNotification2, status2);

	User::WaitForRequest(status2);
	event2 = (TSubConnectionEvent&)(*subconnectionNotification2.Ptr());
	type2 = event2.iEventType;
	TESTEL(ESubConnectionOpened == type2, type2);

	INFO_PRINTF1(_L("Second Notification Complete"));

	//********** issue requests to catch the close events ***********

	conn1.AllSubConnectionNotification(subconnectionNotification1, status1);
	conn2.AllSubConnectionNotification(subconnectionNotification2, status2);

	//********** stop the connections and wait for the events ***********

	err = conn1.Stop(RConnection::EStopAuthoritative);
	TESTEL(KErrNone == err, err);

	err = conn2.Stop(RConnection::EStopAuthoritative);
	TESTEL(KErrNone == err, err);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	// check the details for the first connection are ok
	event1 = (TSubConnectionEvent&)(*subconnectionNotification1.Ptr());
	TESTEL(ESubConnectionClosed == event1.iEventType, event1.iEventType);

	// check the details for the second connection are ok
	event2 = (TSubConnectionEvent&)(*subconnectionNotification2.Ptr());
	TESTEL(ESubConnectionClosed == event2.iEventType, event2.iEventType);

	INFO_PRINTF1(_L("Closed Notifications Complete"));

	//********** clean up ***********

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);

	CloseConnection(conn1);
	CleanupStack::Pop(&conn1);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
	
} // TE_RConnectionTest468



enum TVerdict TE_RConnectionTest469::doTestStepL(void)
/**
 * Test 469
 * Normal stop of single existing subconnection (a primary context).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	SetTestStepResult(EFail);
	
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the qos test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket setup ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Sockets Connected"));

	//********** work out the subconnection id of the primary ***********

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;

	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId primarySubConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TESTEL(primarySubConnectionUniqueId != KNifEntireConnectionSubConnectionId, KErrGeneral);

	//********** stop the primary ***********

	err = conn.Stop(primarySubConnectionUniqueId, RConnection::EStopNormal);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF1(_L("Primary Stopped"));

	//********** give qos a chance to remove the context ***********

	User::After(5000000);

	//********** use socket again ***********

	err = TestTcpDataPathL(sock);
	TESTEL(KErrCancel == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	SetTestStepResult(EPass);
	return TestStepResult();
} // TE_RConnectionTest469



enum TVerdict TE_RConnectionTest470::doTestStepL(void)
/**
 * Test 470
 * Normal stop of a secondary context.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	
	return TestStepResult();
	
} // TE_RConnectionTest470


enum TVerdict TE_RConnectionTest471::doTestStepL(void)
/**
 * Test 471
 * Authoritative stop of single existing subconnection (a primary context).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the qos test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** socket setup ***********
	
	RSocket sock;

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Socket Connect"));

	//********** work out the subconnection id of the primary ***********

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;

	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId primarySubConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	TESTEL(primarySubConnectionUniqueId != KNifEntireConnectionSubConnectionId, KErrGeneral);

	//********** stop the primary ***********

	err = conn.Stop(primarySubConnectionUniqueId, RConnection::EStopAuthoritative);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Primary Stopped"));

	//********** give qos a chance to remove the context ***********

	User::After(5000000);

	//********** use socket again ***********

	err = TestTcpDataPathL(sock);
	TESTEL(KErrConnectionTerminated == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest471



enum TVerdict TE_RConnectionTest472::doTestStepL(void)
/**
 * Test 472
 * Authoritative stop of a secondary contsxt.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	// The ideal solution would have been to create two different script, one for 
	// pre 399 world, the other for post 399. However, having two different scripts
	// mean more maintenance headache. Another idea would be to isolate only the
	// EMI tests in a separate script, but also suffers from the same maintenance headaches...
	SetTestStepResult(EPass); 	
	return TestStepResult();

} // TE_RConnectionTest472


enum TVerdict TE_RConnectionTest473::doTestStepL(void)
/**
 * Test 473
 * Test normal stop (while using test NIF) of complete connection passing special subconnection ID for complete connection (should fail).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** stop the complete connection subconnection... ***********

	err = conn.Stop(KNifEntireConnectionSubConnectionId, RConnection::EStopNormal);
	// should really be KErrNotSupported but the call in CNifAgentRef returns void and loses the error code
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	INFO_PRINTF1(_L("Test Cleanup"));

	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest473


enum TVerdict TE_RConnectionTest474::doTestStepL(void)
/**
 * Test 474
 * Test normal stop (while using dummy NIF) of complete connection passing special subconnection ID for complete connection (should fail).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummy nif
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** stop the complete connection subconnection... ***********

	err = conn.Stop(KNifEntireConnectionSubConnectionId, RConnection::EStopNormal);
	// should really be KErrNotSupported but the call in CNifAgentRef returns void and loses the error code
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	INFO_PRINTF1(_L("Test Cleanup"));

	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest474


enum TVerdict TE_RConnectionTest475::doTestStepL(void)
/**
 * Test 475
 * Test authoritative stop (while using test NIF) of complete connection passing special subconnection ID for complete connection (should fail).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** stop the complete connection subconnection... ***********

	err = conn.Stop(KNifEntireConnectionSubConnectionId, RConnection::EStopAuthoritative);
	// should really be KErrNotSupported but the call in CNifAgentRef returns void and loses the error code
	TESTEL(KErrNone == err, err);

	//********** clean up ***********

	INFO_PRINTF1(_L("Test Cleanup"));

	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest475


enum TVerdict TE_RConnectionTest476::doTestStepL(void)
/**
 * Test 476
 * Test authoritative stop (while using dummy NIF) of complete connection passing special subconnection ID for complete connection (should fail).
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummy nif
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	// should have a single interface
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	INFO_PRINTF1(_L("Setup Complete"));

	//********** stop the complete connection subconnection... ***********

	err = conn.Stop(KNifEntireConnectionSubConnectionId, RConnection::EStopAuthoritative);
	// should really be KErrNotSupported but the call in CNifAgentRef returns void and loses the error code
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	INFO_PRINTF1(_L("Test Cleanup"));

	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest476


enum TVerdict TE_RConnectionTest477::doTestStepL (void)
{
/**
 * Test 477
 * Test to verify DEF023826
 * @returns EPass or EFail,EInconclusive,ETestSuiteError,EAbort
 */
	TInt err;
	RSocketServ ss;

	INFO_PRINTF1 (_L ("Opening socket server"));
	
	err = OpenSocketServer (ss);
	TESTEL (KErrNone == err, err);
	CleanupClosePushL (ss);
	
	RConnection conn;

	err = OpenConnection (conn, ss);
	TESTEL (KErrNone == err, err);
	CleanupClosePushL (conn);

	TRequestStatus notifStatus;
	TSubConnectionNotificationBuf subconnectionNotification;

	INFO_PRINTF1 (_L ("Issuing all subconnection notification request"));
	conn.AllSubConnectionNotification (subconnectionNotification, notifStatus);

	TRequestStatus status;

	INFO_PRINTF1 (_L ("Starting connection"));
	
	conn.Start (status);
	User::WaitForRequest (status);
	TESTEL (KErrNone == status.Int (), status.Int ());

	INFO_PRINTF1 (_L ("Stopping connection"));
	
	conn.Stop (RConnection::EStopAuthoritative);

	INFO_PRINTF1 (_L ("Waiting for notification request to complete"));

	User::WaitForRequest (notifStatus);
	TESTEL (KErrNone == notifStatus.Int(), notifStatus.Int());
	
	INFO_PRINTF1(_L("Test Cleanup"));

	CleanupStack::PopAndDestroy (2, &ss);
	
	return TestStepResult();

} // TE_RConnectionTest477



enum TVerdict TE_RConnectionTest478::doTestStepL(void)
{
/**/
	TInt err;
	SetTestStepResult(EFail);
	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummynif (any interface using the compatibility layer will do)
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	//********** find out about the subconnection and try to stop them ***********

	// there should only be a single subconnection (representing the whole connection)
	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);

	TESTEL(2 == subconnectionCount, subconnectionCount);

	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueId = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueId!= KNifEntireConnectionSubConnectionId, uniqueId);

	// do a normal stop - should fail
	err = conn.Stop(uniqueId);
	// doesn't fail because CNifAgentRef throws away the fact that the compatibility layer doesn't support this
	if(err == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	// do an authoritative stop - should fail
	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
}// TE_RConnectionTest478



enum TVerdict TE_RConnectionTest479::doTestStepL(void)
{
/**/	
	RSocketServ ss;
	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the dummynif (any interface using the compatibility layer will do)
	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	//********** find out about the subconnection and try to stop them ***********

	// There should be 2 subconnection, 1 for the whole connection, and 1 for the single
	// subconnection
	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == subconnectionCount, subconnectionCount);

	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueId = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueId!= KNifEntireConnectionSubConnectionId, uniqueId);

	// do a normal stop - should fail...
	err = conn.Stop(uniqueId + 1);
	// ... but doesn't because CNifAgentRef throws away the fact that the compatibility layer doesn't support this

	TESTEL(KErrNotReady == err, err);

	// do an authoritative stop - should fail
	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
} // TE_RConnectionTest479

const TInt KMACLength = 40;
const TInt KLocalLength = 18;
typedef TBuf<KMACLength> TIP6Printable;
_LIT(KLocalFormatString, "%02X-%02X-%02X-%02X-%02X-%02X");
typedef TBuf<KLocalLength> TLocalPrintable;

TIP6Printable PrintAddr(TInetAddr& aAddr)
	{
	TIP6Printable buffer;
	aAddr.Output(buffer);
	return buffer;
	}

TLocalPrintable PrintAddr(TLinkAddr& aAddr)
	{
	TLocalPrintable buffer;
	TPtrC8 bytes(aAddr.Address());
	if (bytes.Length() >= 6)
		{
		buffer.Format(KLocalFormatString, bytes[0], bytes[1],
										  bytes[2], bytes[3],
										  bytes[4], bytes[5]);
		}
	else
		{
		buffer.Format(_L("Unknown hardware address family %d"), aAddr.Family());
		}
	return buffer;
	}

enum TVerdict TE_RConnectionTest480::doTestStepL(void)
/**
 * Test 480
 * Test Establish connection, use TLinkAddr to acquire link address.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
	TInt err;

	INFO_PRINTF1(_L ("Open Socket Server"));
	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

	INFO_PRINTF1 (_L ("Opening connection"));
	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the ppp nif
	INFO_PRINTF1 (_L ("Starting connection"));
	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1 (_L ("Disable Timers"));
	err = conn.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	// should have one interface
	INFO_PRINTF1 (_L ("Enumerate connections"));
	TUint connectionCount;
	err = conn.EnumerateConnections(connectionCount);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == connectionCount, connectionCount);

	//********** socket activity on connection ***********

	RSocket sock;

	INFO_PRINTF1 (_L ("Open Explicit Tcp Socket"));
	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	INFO_PRINTF1 (_L ("Connect Explicit Tcp Socket"));
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1 (_L ("Test Tcp Data Path"));
	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);
	
	//********** routes on connection ***********

	TInt macAddressCount(0);
	if (sock.SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl) == KErrNone)
		{
		TPckgBuf<TSoInetRouteInfo> opt;
		while (sock.GetOpt(KSoInetNextRoute, KSolInetRtCtrl, opt) == KErrNone)
			{
			TInetAddr dst     = opt().iDstAddr;
			TInetAddr gateway = opt().iGateway;
			TInt family = gateway.Family();
			if (family != KAFUnspec)
				{
				INFO_PRINTF2 (_L ("Found Family %d"), family);

				TLinkAddr linkAddr;
				if (opt().GetLinkAddr(linkAddr) == KErrNone)
					{
					TIP6Printable   dstPrint  = PrintAddr(dst);
					TLocalPrintable linkPrint = PrintAddr(linkAddr);
					INFO_PRINTF3 (_L ("Found MAC address %S for %S"), &linkPrint, &dstPrint);
					++macAddressCount;
					}
				else
					{
					TIP6Printable dstPrint  = PrintAddr(dst);
					TIP6Printable gatePrint = PrintAddr(gateway);
					INFO_PRINTF3(_L("Found gateway address %S for %S"), &gatePrint, &dstPrint);
					}
				}				
			}
		}
	INFO_PRINTF2(_L("MAC Address count %d"),macAddressCount);
	TESTEL(macAddressCount > 0, KErrNotFound);

	
	//********** turn the idle timers back on now we are finished ***********

	err = conn.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	//********** clean up ***********
	
	DestroyTcpSocket(sock);
	CleanupStack::Pop(&sock);

	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();

} // TE_RConnectionTest480

/******************************************************************************
 *
 * Test 319
 *
 * To test progress
 *
 *****************************************************************************/

// To test progress
enum TVerdict TE_RConnectionTest319::doTestStepL(void)
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

    TNifProgress tProg;
    conn1.Progress( tProg );
    TESTEL( tProg.iError == KErrNone && tProg.iStage == 0, tProg.iError );
    
    TNifProgressBuf tNifProgressBuf;
    conn1.ProgressNotification( tNifProgressBuf, tStartConn, KConnProgressDefault );

	nErr = StartConnectionWithOverrides(conn1, iDummyNifIap);

    TESTEL( nErr == KErrNone, nErr );
	User::WaitForRequest( tStartConn );
    TESTEL( tStartConn.Int() == KErrNone, tStartConn.Int() );
    TESTEL( tNifProgressBuf().iStage == KStartingSelection, tNifProgressBuf().iStage );

    //close conn1
    conn1.Stop();
	CleanupStack::PopAndDestroy();

    nErr = conn1.Open( ss );
    TESTEL( nErr == KErrNone, nErr );
	CleanupClosePushL(conn1);

    conn1.Progress( tProg );
    TESTEL( tProg.iError == KErrNone && tProg.iStage == 0, tProg.iError );
    //wait for particular guy
    conn1.ProgressNotification( tNifProgressBuf, tStartConn, KFinishedSelection );
	nErr = StartConnectionWithOverrides(conn1, iDummyNifIap);

    TESTEL( nErr == KErrNone, nErr );
	User::WaitForRequest( tStartConn );
    TESTEL( tStartConn.Int() == KErrNone, tStartConn.Int() );
    TESTEL( tNifProgressBuf().iStage == KFinishedSelection, tNifProgressBuf().iStage );

    //close conn1
    conn1.Stop();
	CleanupStack::PopAndDestroy();

    //close ss
	CleanupStack::PopAndDestroy();
    return TestStepResult();
} // TE_RConnectionTest319



enum TVerdict TE_RConnectionTest481::doTestStepL(void)
/**
 * Test 481
 * Test DataSentNotificationRequest() cancellation
 * Added for defect ou1cimx1#305124.
 * Tests that cancellation of the request does indeed cancel the request
 * Defect summary: was the case that the first cancel cancelled the request but left
 * a notification structure in existence. The second cancel would see the structure
 * and think that the cancellation had already been done leaving the request outstanding.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
    TInt err;
    TRequestStatus status1;

    RSocketServ ss;
    err = OpenSocketServer(ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(ss);

    //********** connection creation ***********
    RConnection conn1;
    err = OpenConnection(conn1, ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(conn1);

    // start the dummynif
    err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
    TESTEL(KErrNone == err, err);
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    TESTEL(KErrNone == err, err);

    RSocket sock1;
    err = OpenUdpSocketExplicitL(sock1, ss, conn1);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(sock1);

    INFO_PRINTF1(_L("First DataSentNotificationRequest"));
    // Request DataSentNotificationRequest
    TUint volume1(0); 
    TPckg<TUint> volume1Des(volume1);
    TUint threshold1(KLowThresholdSize);
    conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);

    // Send some data but not enough to trigger the notification 
    TUint currentVol(0);
    while (currentVol < threshold1 / 2)
        {
        err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);
        TESTEL(KErrNone == err, err);
        TESTEL(KRequestPending == status1.Int(), status1.Int());
        currentVol += KSmallBufferUdpTestPacketSize;
        }

    INFO_PRINTF1(_L("First DataSentNotificationRequest still outstanding (as expected) - now cancelling"));

    // Cancel the request and expect the completion
    conn1.DataSentNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());

    INFO_PRINTF1(_L("First DataSentNotificationRequest completed with KErrCancel"));
    
    // The first cancellation has worked, repeat the above steps for the second cancellation
    INFO_PRINTF1(_L("Second DataSentNotificationRequest"));
    conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);

    // NOTE: It is a deliberate that no data is transfered this time.
	//       Transmitting data and crossing the threshold for the first (cancelled) notification
	//       would cause the object to be cleaned up and therefore mask this issue
    
    // Cancel the request and expect the completion
    conn1.DataSentNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());
    INFO_PRINTF1(_L("Second DataSentNotificationRequest completed with KErrCancel"));
    
    // NOTE: Reaching here successfully does NOT mean the test has passed. If the data monitoring structures
    //       have been left behind esock will panic when the connection is stopped  
    
    
    //********** turn the idle timers back on now we are finished ***********
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    TESTEL(KErrNone == err, err);

    //********** clean up ***********
    DestroyUdpSocket(sock1);
    CleanupStack::Pop(&sock1);

    err = conn1.Stop();
    TESTEL(KErrNone == err, err);

    CloseConnection(conn1);
    CleanupStack::Pop(&conn1);

    CloseSocketServer(ss);
    CleanupStack::Pop(&ss);

    return TestStepResult();

} // TE_RConnectionTest481



enum TVerdict TE_RConnectionTest482::doTestStepL(void)
/**
 * Test 482
 * Test DataReceivedNotificationRequest() cancellation
 * Added for defect ou1cimx1#305124.
 * Tests that cancellation of the request does indeed cancel the request
 * Defect summary: was the case that the first cancel cancelled the request but left
 * a notification structure in existence. The second cancel would see the structure
 * and think that the cancellation had already been done leaving the request outstanding.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
    TInt err;
    TRequestStatus status1;

    RSocketServ ss;
    err = OpenSocketServer(ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(ss);

    //********** connection creation ***********
    RConnection conn1;
    err = OpenConnection(conn1, ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(conn1);

    // start the dummynif
    err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
    TESTEL(KErrNone == err, err);
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    TESTEL(KErrNone == err, err);

    RSocket sock1;
    err = OpenUdpSocketExplicitL(sock1, ss, conn1);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(sock1);

    INFO_PRINTF1(_L("First DataReceivedNotificationRequest"));
    // Request DataSentNotificationRequest
    TUint volume1(0); 
    TPckg<TUint> volume1Des(volume1);
    TUint threshold1(KLowThresholdSize);
    conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);

    // Send some data but not enough to trigger the notification 
    TUint currentVol(0);
    while (currentVol < threshold1 / 2)
        {
        err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);
        TESTEL(KErrNone == err, err);
        TESTEL(KRequestPending == status1.Int(), status1.Int());
        currentVol += KSmallBufferUdpTestPacketSize;
        }

    INFO_PRINTF1(_L("First DataReceivedNotificationRequest still outstanding (as expected) - now cancelling"));

    // Cancel the request and expect the completion
    conn1.DataReceivedNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());

    INFO_PRINTF1(_L("First DataReceivedNotificationRequest completed with KErrCancel"));
    
    // The first cancellation has worked, repeat the above steps for the second cancellation
    INFO_PRINTF1(_L("Second DataReceivedNotificationRequest"));
    conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);

    // NOTE: It is a deliberate that no data is transfered this time.
	//       Transmitting data and crossing the threshold for the first (cancelled) notification
	//       would cause the object to be cleaned up and therefore mask this issue
    
    // Cancel the request and expect the completion
    conn1.DataReceivedNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());
    INFO_PRINTF1(_L("Second DataReceivedNotificationRequest completed with KErrCancel"));

    // NOTE: Reaching here successfully does NOT mean the test has passed. If the data monitoring structures
    //       have been left behind esock will panic when the connection is stopped  

    
    //********** turn the idle timers back on now we are finished ***********
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    TESTEL(KErrNone == err, err);

    //********** clean up ***********
    DestroyUdpSocket(sock1);
    CleanupStack::Pop(&sock1);

    err = conn1.Stop();
    TESTEL(KErrNone == err, err);

    CloseConnection(conn1);
    CleanupStack::Pop(&conn1);

    CloseSocketServer(ss);
    CleanupStack::Pop(&ss);

    return TestStepResult();

} // TE_RConnectionTest482



enum TVerdict TE_RConnectionTest483::doTestStepL(void)
/**
 * Test 483
 * Test DataSentNotificationRequest() cancellation
 * Added for defect ou1cimx1#305124.
 * Tests that cancellation of the request does indeed cancel the request
 * Defect summary: If the state described in Test481 occurs, it would have been possible to
 * request more than one DataSentNotificationRequest per RConnection. This should not be possible.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
    TInt err;
    TRequestStatus status1;

    RSocketServ ss;
    err = OpenSocketServer(ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(ss);

    //********** connection creation ***********
    RConnection conn1;
    err = OpenConnection(conn1, ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(conn1);

    // start the dummynif
    err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
    TESTEL(KErrNone == err, err);
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    TESTEL(KErrNone == err, err);

    RSocket sock1;
    err = OpenUdpSocketExplicitL(sock1, ss, conn1);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(sock1);

    INFO_PRINTF1(_L("First DataSentNotificationRequest"));
    // Request DataSentNotificationRequest
    TUint volume1(0); 
    TPckg<TUint> volume1Des(volume1);
    TUint threshold1(KLowThresholdSize);
    conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);

    // Send some data but not enough to trigger the notification 
    TUint currentVol(0);
    while (currentVol < threshold1 / 2)
        {
        err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);
        TESTEL(KErrNone == err, err);
        TESTEL(KRequestPending == status1.Int(), status1.Int());
        currentVol += KSmallBufferUdpTestPacketSize;
        }

    INFO_PRINTF1(_L("First DataSentNotificationRequest still outstanding (as expected) - now cancelling"));

    // Cancel the request and expect the completion
    conn1.DataSentNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());

    INFO_PRINTF1(_L("First DataSentNotificationRequest completed with KErrCancel"));
    
    // The first cancellation has worked, repeat the above steps for the second cancellation
    INFO_PRINTF1(_L("Second DataSentNotificationRequest"));
    conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);

    // NOTE: It is a deliberate that no data is transfered this time.
	//       Transmitting data and crossing the threshold for the first (cancelled) notification
	//       would cause the object to be cleaned up and therefore mask this issue
    
    // Cancel the request and expect the completion
    conn1.DataSentNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());
    INFO_PRINTF1(_L("Second DataSentNotificationRequest completed with KErrCancel"));
    
    
    INFO_PRINTF1(_L("Third DataSentNotificationRequest"));
    conn1.DataSentNotificationRequest(threshold1, volume1Des, status1);
    

    INFO_PRINTF1(_L("Fourth DataSentNotificationRequest"));
    TRequestStatus status2;
    conn1.DataSentNotificationRequest(threshold1, volume1Des, status2);
    TBool requestCompleted = WaitForRequestOrTimeOutL(status2, KOneSecondDelay);
    TESTL(requestCompleted);
    TESTEL(status2.Int() == KErrInUse, status2.Int());
    INFO_PRINTF1(_L("Fourth DataSentNotificationRequest failed with KErrInUse as expected"));
    
    conn1.DataSentNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());
    INFO_PRINTF1(_L("Third DataSentNotificationRequest completed with KErrCancel"));
    
    //********** turn the idle timers back on now we are finished ***********
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    TESTEL(KErrNone == err, err);

    //********** clean up ***********
    DestroyUdpSocket(sock1);
    CleanupStack::Pop(&sock1);

    err = conn1.Stop();
    TESTEL(KErrNone == err, err);

    CloseConnection(conn1);
    CleanupStack::Pop(&conn1);

    CloseSocketServer(ss);
    CleanupStack::Pop(&ss);

    return TestStepResult();

} // TE_RConnectionTest483


enum TVerdict TE_RConnectionTest484::doTestStepL(void)
/**
 * Test 484
 * Test DataReceivedNotificationRequest() cancellation
 * Added for defect ou1cimx1#305124.
 * Tests that cancellation of the request does indeed cancel the request
 * Defect summary: If the state described in Test482 occurs, it would have been possible to
 * request more than one DataReceivedNotificationRequest per RConnection. This should not be possible.
 * @return enum TVerdict Indicates status of test run. See INFO_PRINTF1.h for possible values
 */
{
    TInt err;
    TRequestStatus status1;

    RSocketServ ss;
    err = OpenSocketServer(ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(ss);

    //********** connection creation ***********
    RConnection conn1;
    err = OpenConnection(conn1, ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(conn1);

    // start the dummynif
    err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
    TESTEL(KErrNone == err, err);
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    TESTEL(KErrNone == err, err);

    RSocket sock1;
    err = OpenUdpSocketExplicitL(sock1, ss, conn1);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(sock1);

    INFO_PRINTF1(_L("First DataReceivedNotificationRequest"));
    // Request DataReceivedNotificationRequest
    TUint volume1(0); 
    TPckg<TUint> volume1Des(volume1);
    TUint threshold1(KLowThresholdSize);
    conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);

    // Send some data but not enough to trigger the notification 
    TUint currentVol(0);
    while (currentVol < threshold1 / 2)
        {
        err = TestUdpDataPathL(sock1, iDummyNifSendAddr, KSmallBufferLength);
        TESTEL(KErrNone == err, err);
        TESTEL(KRequestPending == status1.Int(), status1.Int());
        currentVol += KSmallBufferUdpTestPacketSize;
        }

    INFO_PRINTF1(_L("First DataReceivedNotificationRequest still outstanding (as expected) - now cancelling"));

    // Cancel the request and expect the completion
    conn1.DataReceivedNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());

    INFO_PRINTF1(_L("First DataReceivedNotificationRequest completed with KErrCancel"));
    
    // The first cancellation has worked, repeat the above steps for the second cancellation
    INFO_PRINTF1(_L("Second DataReceivedNotificationRequest"));
    conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);

    // NOTE: It is a deliberate that no data is transfered this time.
	//       Transmitting data and crossing the threshold for the first (cancelled) notification
	//       would cause the object to be cleaned up and therefore mask this issue
    
    // Cancel the request and expect the completion
    conn1.DataReceivedNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());
    INFO_PRINTF1(_L("Second DataReceivedNotificationRequest completed with KErrCancel"));
    
    
    INFO_PRINTF1(_L("Third DataReceivedNotificationRequest"));
    conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status1);

    INFO_PRINTF1(_L("Fourth DataReceivedNotificationRequest"));
    TRequestStatus status2;
    conn1.DataReceivedNotificationRequest(threshold1, volume1Des, status2);
    TBool requestCompleted = WaitForRequestOrTimeOutL(status2, KOneSecondDelay);
    TESTL(requestCompleted);
    TESTEL(status2.Int() == KErrInUse, status2.Int());
    INFO_PRINTF1(_L("Fourth DataReceivedNotificationRequest failed with KErrInUse as expected"));
    
    conn1.DataReceivedNotificationCancel();
    User::WaitForRequest(status1);
    TESTEL(status1.Int() == KErrCancel, status1.Int());
    INFO_PRINTF1(_L("Third DataReceivedNotificationRequest completed with KErrCancel"));
    
    //********** turn the idle timers back on now we are finished ***********
    err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    TESTEL(KErrNone == err, err);

    //********** clean up ***********
    DestroyUdpSocket(sock1);
    CleanupStack::Pop(&sock1);

    err = conn1.Stop();
    TESTEL(KErrNone == err, err);

    CloseConnection(conn1);
    CleanupStack::Pop(&conn1);

    CloseSocketServer(ss);
    CleanupStack::Pop(&ss);

    return TestStepResult();

} // TE_RConnectionTest484


// EOF TE_RConnectionCMM.cpp

