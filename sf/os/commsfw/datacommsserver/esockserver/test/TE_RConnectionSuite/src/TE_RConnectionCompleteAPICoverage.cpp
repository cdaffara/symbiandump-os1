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
//


#include "TE_RConnectionCompleteAPICoverage.h"
#include <ss_std.h>
#include <comms-infras/nifprvar.h>
#include <nifman.h>
#include <inet6err.h>

/** Test 700
 *
 */

/** 
@SYMTestCaseID TE_RConnection-700
@SYMTestCaseDesc Test Stop(TSubConnection& aSubConnectionUniqueId) for expected functionality
 */
enum TVerdict TE_RConnectionTest700::doTestStepL(void)
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

	
	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
    // there should only be two subconnection (representing the whole connection and representing the default subconnection)
	TESTEL(2 == subconnectionCount, subconnectionCount);

	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueId = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueId!= KNifEntireConnectionSubConnectionId, uniqueId);

	// do a normal stop - should fail
	// identical to:
	// err = conn.Stop(uniqueId, RConnection::EStopNormal);
	err = conn.Stop(uniqueId);
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
}

/** Test 701
 *
 */

/** 
@SYMTestCaseID TE_RConnection-701
@SYMTestCaseDesc Test Stop(TSubConnection& aSubConnectionUniqueId) with invalid subconnection ID
 */
enum TVerdict TE_RConnectionTest701::doTestStepL(void)
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

	//********** find out about the subconnection and try to stop it ***********


	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
    // there should only be two subconnection (representing the whole connection and representing the default subconnection)
	TESTEL(2 == subconnectionCount, subconnectionCount);

	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueId = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueId!= KNifEntireConnectionSubConnectionId, uniqueId);

	// do a normal stop - should fail as subconnection Id + MaxNumberConnections doesn't exist
	err = conn.Stop(uniqueId + MaxConnectionsNumber);
	
	// make sure an err is reported
	TESTEL(KErrNotReady == err, err);

	//********** clean up ***********
	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
}

/** Test 702
 *
 */

/** 
@SYMTestCaseID TE_RConnection-702
@SYMTestCaseDesc Stop a subconnection when the whole connection has already been stopped
 */
enum TVerdict TE_RConnectionTest702::doTestStepL(void)
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

	TUint subconnectionCount;
	err = conn.EnumerateSubConnections(subconnectionCount);
	TESTEL(KErrNone == err, err);
    // there should only be two subconnection (representing the whole connection and representing the default subconnection)
	TESTEL(2 == subconnectionCount, subconnectionCount);
	// ensure that it *is* representing the complete connection
	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId uniqueId = subconnectionInfo().iSubConnectionUniqueId;
	TESTEL(uniqueId!= KNifEntireConnectionSubConnectionId, uniqueId);

	//********** clean up ***********
	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	// do a normal stop - should fail as whole connection has been stopped already
	// and so subconnection doesn't exist anymore
	err = conn.Stop(uniqueId);
	
	// make sure an err is reported
	TESTEL(KErrNotReady == err, err);

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
}

/** Test 703
 *
 */


/** 
@SYMTestCaseID TE_RConnection-703
@SYMTestCaseDesc Test Progess() call on subconnections, should return KErrNone
but actually do nothing on the server side as this call isn't supported
as this is no longer supported
 */
enum TVerdict TE_RConnectionTest703::doTestStepL()
{
	TInt err;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;
	
	// Get Progress Info
	TNifProgress tProg;
	conn.Progress(subConnectionUniqueId, tProg);
	TESTEL( tProg.iError == KErrNone, tProg.iError );
	
	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 705
 *
 */
 

/**  
@SYMTestCaseID TE_RConnection-705
@SYMTestCaseDesc negative test for non valid subconnectionid being passed to progress()
 */
enum TVerdict TE_RConnectionTest705::doTestStepL()
{
	TInt err;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;
	
	// Get Progress Info
	TNifProgress tProg;
	err = conn.Progress(subConnectionUniqueId + MaxConnectionsNumber, tProg);
	TESTEL( err == KErrNotSupported, err);

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 706
 *
 */
/** 
@SYMTestCaseID TE_RConnection-706
@SYMTestCaseDesc Test ProgressNotification() on subconnection
 */
enum TVerdict TE_RConnectionTest706::doTestStepL()
{
	TInt err;
	TRequestStatus tStatus;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	// Set up ProgressNotification()
	TNifProgressBuf tNifProgressBuf;

	conn.ProgressNotification(subConnectionUniqueId, tNifProgressBuf, tStatus);
  	
  	// Force change in state of subConn  	
	err = RequestSetStausL(conn, ss, subConnectionUniqueId);
	TESTEL(KErrNone == err, err);
	  	
	User::WaitForRequest( tStatus );
	TInt theVal = tNifProgressBuf().iStage;
	TESTEL( tStatus.Int() == KErrNotSupported, tStatus.Int() );

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}


/** 
@SYMTestCaseID TE_RConnection-707
@SYMTestCaseDesc negative testing ProgressNotification should reset any values set in the 
TNifProgressBuff. a raised error value on tProg.iError shouldn't still exist after the progress call
 */
enum TVerdict TE_RConnectionTest707::doTestStepL()
{
	TInt err;
	TRequestStatus tStatus;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	// try to Set up ProgressNotification() passing in a TNifProgressBuf with
	// some erronous values to progressnotification
	TNifProgressBuf tNifProgressBuf;
	tNifProgressBuf().iError = KErrGeneral;
  	conn.ProgressNotification(subConnectionUniqueId, tNifProgressBuf, tStatus);
  	
  	// Force change in state of subConn  	
	err = RequestSetStausL(conn, ss, subConnectionUniqueId);
	TESTEL(KErrNone == err, err);
  	
	User::WaitForRequest( tStatus );
	TInt error = tNifProgressBuf().iError;
	TESTEL(tNifProgressBuf().iError != KErrCancel, tNifProgressBuf().iError);
	TESTEL( tStatus.Int() == KErrNotSupported, tStatus.Int() );
	
	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 708
 *
 */

/** 
@SYMTestCaseID TE_RConnection-708
@SYMTestCaseDesc Test ProgressNotification(TSubConnection& aSubConnectionUniqueId...) with invalid subconnection ID
 */
enum TVerdict TE_RConnectionTest708::doTestStepL()
{
	TInt err;
	TRequestStatus tStatus;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	// try to Set up ProgressNotification() with invalid subconnectionid
	TNifProgressBuf tNifProgressBuf;
	conn.ProgressNotification(subConnectionUniqueId + MaxConnectionsNumber, tNifProgressBuf, tStatus);
  	
	User::WaitForRequest( tStatus );
	TESTEL( tStatus.Int() == KErrNotSupported, tStatus.Int() );

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 709
 *
 */
/**
@SYMTestCaseID TE_RConnection-709
@SYMTestCaseDesc Test CancelProgressNotification()
 */
enum TVerdict TE_RConnectionTest709::doTestStepL()
{
	TInt err;
	TRequestStatus tStatus;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********
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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	// Test ProgressNotification()
	TNifProgressBuf tNifProgressBuf;
  	conn.ProgressNotification(subConnectionUniqueId, tNifProgressBuf, tStatus, KStartingSelection );
    
    // Force change in state of subConn  	
	err = RequestSetStausL(conn, ss, subConnectionUniqueId);
	TESTEL(KErrNone == err, err);
    
    conn.CancelProgressNotification(subConnectionUniqueId);
    
	User::WaitForRequest( tStatus );
	TESTEL(KErrNotSupported == tStatus.Int(), tStatus.Int());

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 710
 *
 */

/** 
@SYMTestCaseID TE_RConnection-710
@SYMTestCaseDesc Test CancelProgressNotification() with invalid subconnectionId
shouldn't panic the system
 */
enum TVerdict TE_RConnectionTest710::doTestStepL()
{
	TInt err;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********
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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;
   
   	// try to cancel invalid subconnectionId
   	// should be a recoverable operation
	conn.CancelProgressNotification(subConnectionUniqueId + MaxConnectionsNumber);
    
   	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 711
 *
 */
/**
@SYMTestCaseID TE_RConnection-711
@SYMTestCaseDesc Negative testing of WaitForIncoming(RSubConnection&) passing in Null pointer
 */
enum TVerdict TE_RConnectionTest711::doTestStepL()
{
	TInt err;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start the test nif
	err = StartConnectionWithOverrides(conn, iTestNifIap);
	TESTEL(KErrNone == err, err);

	// uninstantiated RSubConn reference
	RSubConnection subConn;
	
	// pass in Null subconnection reference to WaitForIncoming
	err = conn.WaitForIncoming(subConn);
	TESTEL(KErrNotSupported == err, err);

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	// Some error was reported so we're safe
	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 712
 *
 */


/** 
@SYMTestCaseID TE_RConnection-712
@SYMTestCaseDesc ProgressNotification requesting notification on an specific
aSelectedProgress stage instead of on any change in subconn status
 */
enum TVerdict TE_RConnectionTest712::doTestStepL()
{
	TInt err;
	TRequestStatus tStatus;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	// Set up ProgressNotification()
	TNifProgressBuf tNifProgressBuf;
  	conn.ProgressNotification(subConnectionUniqueId, tNifProgressBuf, tStatus, KFinishedSelection);
  	
  	// Force change in state of subConn  	
	err = RequestSetStausL(conn, ss, subConnectionUniqueId);
	TESTEL(KErrNone == err, err);
  	
	User::WaitForRequest( tStatus );
	TESTEL( tStatus.Int() == KErrNotSupported, tStatus.Int() );

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}

/** Test 713
 *
 */
/** 
@SYMTestCaseID TE_RConnection-713
@SYMTestCaseDesc Negative testing of ProgressNotification requesting notification on an invalid 
aSelectedProgress stage 
 */
enum TVerdict TE_RConnectionTest713::doTestStepL()
{
TInt err;
	TRequestStatus tStatus;
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** connection creation ***********

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

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	err = conn.GetSubConnectionInfo(1, subconnectionInfo);
	TESTEL(KErrNone == err, err);
	TSubConnectionUniqueId subConnectionUniqueId = subconnectionInfo().iSubConnectionUniqueId;

	// Set up ProgressNotification()
	TNifProgressBuf tNifProgressBuf;
  	conn.ProgressNotification(subConnectionUniqueId, tNifProgressBuf, tStatus, (TUint)-1000);
  	
  	// Force change in state of subConn  	
	err = RequestSetStausL(conn, ss, subConnectionUniqueId);
	TESTEL(KErrNone == err, err);
  	
	User::WaitForRequest( tStatus );
    TESTEL( tStatus.Int() != KErrNone, tStatus.Int() );

	err = conn.Stop();
    TESTEL( err == KErrNone, err );

	CloseConnection(conn);
	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	
    return TestStepResult();
}




