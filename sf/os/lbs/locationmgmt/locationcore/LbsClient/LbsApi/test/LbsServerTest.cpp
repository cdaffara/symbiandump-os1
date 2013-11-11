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
//

#include "unittest.h"
#include "testregistry.h"
#include "test.h"
#include <lbs.h>
#include <lbssatellite.h>
#include <LbsErrors.h>

_LIT(KDirectoryForExesOnTarget, "\\system\\programs\\");
_LIT(KPositionServerImg, "LbsTestSvr.exe");

// Utility function prototypes
void CleanupResetAndDestroy(TAny* aArray);

/*

  All of the tests are individually TRAP'ed, so the code can leave.

  In setting up the tests, User::LeaveIfError is used, so a leave indicates
  abnormal termination of the test. In checking the expected results of
  each test, one of the CHECK.. macros is used, which will potentially set
  the test result (iResult field of TTest) to EFailed.

  Tests can print messages to the screen by using the global CConsoleBase* console
  eg. 	console->Printf(_L("Hello world\n"));

  In real unit tests, the data returned by each API method would need to be
  tested. At present, that is only the case in the RPositioner::NotifyPositionUpdate
  tests.

*/

//
// Basic RPositionServer tests
//

TEST(RPositionServer, Close_Has_No_Side_Effects, "PSOC", 2)
	{
	// Check closing the position server leaves the hardware (or emulator) 
	// in its original state (it should be possible to run another test without
	// having to perform a reset)
	RPositionServer server;
	for (TInt i=0; i<20; i++)
		{
		User::LeaveIfError(server.Connect());
		CHECK( server.Handle() > 0 );
		server.Close();
		CHECK( server.Handle() == 0 );
		}
	}

TEST(RPositionServer, Connect, "PSOC", 3)
	{
	// Try different open methods (from server, from positioner, etc.)
	RPositionServer server;
	CHECK_EQUAL( 0, server.Handle() );
	CHECK_EQUAL( KErrNone, server.Connect() );
	CHECK( server.Handle() > 0 );
	server.Close();
	}

TEST(RPositionServer, Connect_TwoClients, "PSOC", 3)
	{
	RPositionServer server1, server2;
	CleanupClosePushL(server1);
	CleanupClosePushL(server2);

	CHECK_EQUAL( KErrNone, server1.Connect() );
	CHECK_EQUAL( KErrNone, server2.Connect() );

	CleanupStack::PopAndDestroy(2, &server1);
	}

#ifdef __EPOC32__
TEST(RPositionServer, Connect_ServerExeMissing, "PSOC", 3)
	{
	/*
	We cannot run this test under WINS, since the server DLL will
	reside at \epoc32\release\wins\udeb\lbstestsvr.dll. We cannot
	rename that file using an RFs, since it lives outside the Symbian
	OS file system. Even if we set things up to use the DLL from
	\epoc32\release\wins\udeb\z\system\libs, we cannot rename the file
	as it is supposed to be in ROM (ie. the Z drive).
	*/
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	TFindFile exeFinder(fs);
	User::LeaveIfError(exeFinder.FindByDir(KPositionServerImg, KDirectoryForExesOnTarget));

	if(fs.IsFileInRom(exeFinder.File()))
		{
		CleanupStack::PopAndDestroy(&fs);
		return;	//Fail silently is exe is in ROM.
		}
	
	_LIT(KAppendToName, "-1");

	TFileName newName(exeFinder.File());
	newName += KAppendToName;

	User::LeaveIfError(fs.Rename(exeFinder.File(), newName));

	RPositionServer server;
	CleanupClosePushL(server);

	TInt serverConnectResult = server.Connect();

	// We must get the server back for the remaining tests.
	// If we cannot bring the server EXE back, running the rest of the
	// tests is pointless, so assert that the second rename is successful
	__ASSERT_ALWAYS(fs.Rename(newName, exeFinder.File()) == KErrNone, User::Invariant());

	CHECK_EQUAL( KErrNotFound, serverConnectResult );
	CleanupStack::PopAndDestroy(2, &fs); // fs, server
	}
#endif // __EPOC32__

TEST(RPositionServer, Close, "PSOC", 4)
	{
	// Ensure that closing a client sends a message telling the server
	// to close the session.
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CHECK( server.Handle() > 0 );
	server.Close();
	CHECK( server.Handle() == 0 );
	}

TEST(RPositionServer, Version, "PSVI", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TVersion version = server.Version();

	CHECK_EQUAL( version.iMajor, KPosMajorVersionNumber);
	CHECK_EQUAL( version.iMinor, KPosMinorVersionNumber);
	CHECK_EQUAL( version.iBuild, KPosBuildVersionNumber);

	CleanupStack::PopAndDestroy(&server);
	}

//
// RPositionServer PANIC tests
//

TInt ConnectRPositionServerTwice(TAny* /*aPtr*/)
	{
	CTrapCleanup* cs = CTrapCleanup::New();
	if(cs==NULL)
		return KErrNoMemory;
	RPositionServer posServer;
	TInt err = posServer.Connect();
	if(err!=KErrNone)
		{
		delete cs;
		return err;
		}
	err = posServer.Connect();	//This should panic. If it does return, we return the value.
	delete cs;
	return err;
	}

TEST(RPositionServer, Panic_On_Double_Connect, "PSPA", 1)
	{
	DO_PANIC_TEST_L(ConnectRPositionServerTwice, KPosClientFault, 
					EPositionServerHandleNotClosed, KDefaultTestTimeout);
	}

TInt RPositionServerCancelRequestWithoutConnect(TAny* /*aPtr*/)
	{
	RPositionServer posServer;
	posServer.CancelRequest(0);
	return KErrGeneral;
	}

TEST(RPositionServer, Panic_Cancel_With_BadHandle, "PSPA", 2)
	{
	DO_PANIC_TEST_L(RPositionServerCancelRequestWithoutConnect, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetDefaultModuleIdPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositionServer server;
	TPositionModuleId id;
	server.GetDefaultModuleId(id); // This should panic
	return KErrGeneral;
	}

TEST(RPositionServer, GetDefaultModuleIdPanics_NotConnected, "PSPA", 3)
	{
	DO_PANIC_TEST_L(GetDefaultModuleIdPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetNumModulesPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositionServer server;
	TUint num;
	server.GetNumModules(num); // This should panic
	return KErrGeneral;
	}

TEST(RPositionServer, GetNumModulesPanics_NotConnected, "PSPA", 4)
	{
	DO_PANIC_TEST_L(GetNumModulesPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetModuleInfoByIndexPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositionServer server;
	TPositionModuleInfo info;
	server.GetModuleInfoByIndex(0, info); // This should panic
	return KErrGeneral;
	}

TEST(RPositionServer, GetModuleInfoByIndexPanics_NotConnected, "PSPA", 5)
	{
	DO_PANIC_TEST_L(GetModuleInfoByIndexPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetModuleInfoByIdPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositionServer server;
	TPositionModuleInfo info;
	server.GetModuleInfoById(TUid::Uid(0), info); // This should panic
	return KErrGeneral;
	}

TEST(RPositionServer, GetModuleInfoByIdPanics_NotConnected, "PSPA", 6)
	{
	DO_PANIC_TEST_L(GetModuleInfoByIdPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetModuleStatusPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositionServer server;
	TPositionModuleStatus status;
	server.GetModuleStatus(status, TUid::Uid(0)); // This should panic
	return KErrGeneral;
	}

TEST(RPositionServer, GetModuleStatusPanics_NotConnected, "PSPA", 7)
	{
	DO_PANIC_TEST_L(GetModuleStatusPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt NotifyEventWithoutConnect(TAny* /*aPtr*/)
	{
	RPositionServer posServer;
	TPositionModuleStatusEvent event;
	TRequestStatus status;
	posServer.NotifyModuleStatusEvent(event, status);	//This should panic
	return KErrGeneral;
	}

TEST(RPositionServer, Panic_Notify_Event_Not_Connected, "PSPA", 8)
	{
	DO_PANIC_TEST_L(NotifyEventWithoutConnect, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}


//
// Basic RPositioner tests
//

TEST(RPositioner, Close_Has_No_Side_Effects, "POCL", 2)
	{
	// Check closing the position server leaves the hardware (or emulator)
	// in its original state (it should be possible to run another test
	// without having to perform a reset)
	RPositionServer server;
	CleanupClosePushL(server);
	RPositioner positioner;
	CleanupClosePushL(positioner);
	for (TInt i=0; i<20; i++)
		{
		User::LeaveIfError(server.Connect());
		User::LeaveIfError(positioner.Open(server));
		CHECK( positioner.SubSessionHandle() > 0 );
		positioner.Close();
		server.Close();
		CHECK( positioner.SubSessionHandle() == 0 );
		}
	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, Open_NoParameters, "POCL", 3)
	{
	// Try different open methods (from server, from positioner, etc.)
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CHECK_EQUAL( 0, positioner.SubSessionHandle() );
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	CHECK( positioner.SubSessionHandle() > 0 );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, Open_FromModuleId, "POCL", 3)
	{
	// Try different open methods (from server, from positioner, etc.)
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleId moduleId;
	User::LeaveIfError(server.GetDefaultModuleId(moduleId));
	RPositioner positioner;
	CHECK_EQUAL( 0, positioner.SubSessionHandle() );
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server, moduleId));
	CHECK( positioner.SubSessionHandle() > 0 );
	positioner.Close();

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, Open_FromPositionCriteria, "POCL", 3)
	{
	// Try different open methods (from server, from positioner, etc.)
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionCriteria requiredPosCriteria;
	RPositioner positioner;
	CHECK_EQUAL( 0, positioner.SubSessionHandle() );
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server, requiredPosCriteria));
	CHECK( positioner.SubSessionHandle() > 0 );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, Close, "POCL", 4)
	{
	// Ensure that closing a client sends a message telling the server
	// to close the session.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	User::LeaveIfError(positioner.Open(server));

	CHECK( positioner.SubSessionHandle() > 0 );
	positioner.Close();
	CHECK_EQUAL( 0, positioner.SubSessionHandle() );

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositioner, SetRequestor_Single, "PREQ", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	_LIT(KRequestor, "Tom Tom");
	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	CHECK_EQUAL( KErrNone, positioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KRequestor));
	// There is no GetRequestor() to check it got to the server correctly

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, SetRequestor_Multiple, "PREQ", 1)
	{
	// Check all types and combinations of requestors can be set.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	// Note: that putting the RPointerArray on the cleanup stack will
	// only result in Close() being called on it (by PopAndDestroy). The
	// CRequestor objects must be deleted separately (they go on the
	// cleanup stack individually, but ResetAndDestroy() could be called
	// on the RPointerArray... but how to do that from the cleanup stack?)
	static const TInt KNumberOfRequestors = 3;
	_LIT(KName1, "Tom Tom Mapper");
	_LIT(KName2, "+441223719800");
	_LIT(KName3, "http://jbond.com");

	const TDesC* KNames[KNumberOfRequestors] =
		{ &KName1, &KName2, &KName3 };

	const CRequestor::TRequestorType KTypes[KNumberOfRequestors] =
		{ CRequestor::ERequestorService, CRequestor::ERequestorContact, CRequestor::ERequestorContact };

	const CRequestor::TRequestorFormat KFormats[KNumberOfRequestors] =
		{ CRequestor::EFormatApplication, CRequestor::EFormatTelephone, CRequestor::EFormatUrl };

	RRequestorStack requestors;
	CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &requestors));
	for (TInt i = 0; i < KNumberOfRequestors; i++)
		{
		CRequestor* requestor;
		requestor = CRequestor::NewLC(
			KTypes[i],
			KFormats[i],
			*KNames[i]);
		User::LeaveIfError(requestors.Append(requestor));
		CleanupStack::Pop(requestor);
		}

	CHECK_EQUAL( KErrNone, positioner.SetRequestor(requestors));
	// There is no GetRequestorsL() to check the array got to the server correctly

	CleanupStack::PopAndDestroy(3, &server); // requestors, positioner, server
	}

TEST(RPositioner, SetRequestor_BadArgument, "PREQ", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	RRequestorStack requestors;

	CHECK_EQUAL( KErrArgument, positioner.SetRequestor(requestors));
	CleanupStack::PopAndDestroy(2, &server); // positioner, server
	}

TEST(RPositioner, SetRequestor_Multiple_Clients, "PREQ", 4)
	{
	// Check the server completing multiple requests for the same function is
	// handled correctly by the client API.
	// NOTE: Function is not asynchronous, so the general test CTRT2 does
	// not apply here.
	}

TEST(RPositioner, SetRequestor_Cancel, "PREQ", 5)
	{
	// Check request cancellation.
	// NOTE: Function is not asynchronous / there is no cancel, so the
	// general test CTRT3 does not apply here.
	}

TEST(RPositioner, SetUpdateOptions, "PUPD", 2)
	{
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionUpdateOptions posOption;
	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	const TTimeIntervalMicroSeconds KMicroSecondsValue(4000000);
	posOption.SetUpdateInterval(KMicroSecondsValue);
	posOption.SetUpdateTimeOut(KMicroSecondsValue);
	posOption.SetMaxUpdateAge(KMicroSecondsValue);
	posOption.SetAcceptPartialUpdates( ETrue );

	User::LeaveIfError(positioner.SetUpdateOptions(posOption));
	posOption = TPositionUpdateOptions();
	User::LeaveIfError(positioner.GetUpdateOptions(posOption));

	CHECK( posOption.UpdateInterval() == KMicroSecondsValue );
	CHECK( posOption.UpdateTimeOut() == KMicroSecondsValue );
	CHECK( posOption.MaxUpdateAge() == KMicroSecondsValue );
	CHECK_EQUAL( (TBool)ETrue, (TBool)posOption.AcceptPartialUpdates() );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, SetUpdateOptions_Multiple_Clients, "PUPD", 4)
	{
	// Check the server completing multiple requests for the same function
	// is handled correctly by the client API.
	// NOTE: Function is not asynchronous, so the general test CTRT2 does
	// not apply here.
	}

TEST(RPositioner, SetUpdateOptions_Cancel, "PUPD", 5)
	{
	// Check request cancellation.
	// NOTE: Function is not asynchronous / there is no cancel, so the
	// general test CTRT3 does not apply here.
	}

TEST(RPositioner, NotifyPositionUpdate, "PNPU", 5)
	{
	}

//
// RPositioner PANIC tests
//

TInt RPositionerCancelRequestWithoutConnect(TAny* /*aPtr*/)
	{
	RPositioner pos;
	pos.CancelRequest(0);
	return KErrGeneral;
	}

TEST(RPositioner, Panic_Cancel_NotConnected, "PPAN", 1)
	{
	DO_PANIC_TEST_L(RPositionerCancelRequestWithoutConnect, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt OpenRPositionerTwice(TAny* /*aPtr*/)
	{
	CTrapCleanup* cs = CTrapCleanup::New();
	if(cs==NULL)
		return KErrNoMemory;

	RPositionServer ps;
	TInt err = ps.Connect();
	if(err!=KErrNone)
		{
		delete cs;
		return err;
		}
	RPositioner positioner;
	err = positioner.Open(ps, KPositionNullModuleId);
	if(err!=KErrNone)
		{
		delete cs;
		return err;
		}
	err = positioner.Open(ps, KPositionNullModuleId);	//Should panic.
	delete cs;
	return err;
	}

TEST(RPositioner, Panic_OpenTwice, "PPAN", 2)
	{
	DO_PANIC_TEST_L(OpenRPositionerTwice, KPosClientFault, 
					EPositionServerHandleNotClosed, KDefaultTestTimeout);
	}

TInt OpenRPositionerPanicsNotConnected1(TAny* /*aPtr*/)
	{
	RPositionServer server;
	RPositioner pos;
	pos.Open(server); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, OpenRPositionerPanicsNotConnected1, "PPAN", 3)
	{
	DO_PANIC_TEST_L(OpenRPositionerPanicsNotConnected1, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt OpenRPositionerPanicsNotConnected2(TAny* /*aPtr*/)
	{
	RPositionServer server;
	RPositioner pos;
	pos.Open(server, KPositionNullModuleId); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, OpenRPositionerPanicsNotConnected2, "PPAN", 4)
	{
	DO_PANIC_TEST_L(OpenRPositionerPanicsNotConnected2, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt OpenRPositionerPanicsNotConnected3(TAny* /*aPtr*/)
	{
	RPositionServer server;
	RPositioner pos;
	TPositionCriteria crit;
	pos.Open(server, crit); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, OpenRPositionerPanicsNotConnected3, "PPAN", 5)
	{
	DO_PANIC_TEST_L(OpenRPositionerPanicsNotConnected3, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt SetSingleRequestorPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositioner pos;
	pos.SetRequestor(CRequestor::ERequestorUnknown,
	                 CRequestor::EFormatUnknown,
	                 KNullDesC); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, SetSingleRequestorPanics_NotConnected, "PPAN", 6)
	{
	DO_PANIC_TEST_L(SetSingleRequestorPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt SetMultipleRequestorsPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositioner pos;
	RRequestorStack stack;
	pos.SetRequestor(stack); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, SetMultipleRequestorsPanics_NotConnected, "PPAN", 7)
	{
	DO_PANIC_TEST_L(SetMultipleRequestorsPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt SetUpdateOptionsPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositioner pos;
	TPositionUpdateOptions opt;
	pos.SetUpdateOptions(opt); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, SetUpdateOptionsPanics_NotConnected, "PPAN", 8)
	{
	DO_PANIC_TEST_L(SetUpdateOptionsPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetUpdateOptionsPanics_NotConnected(TAny* /*aPtr*/)
	{
	RPositioner pos;
	TPositionUpdateOptions opt;
	pos.GetUpdateOptions(opt); // This should panic
	return KErrGeneral;
	}

TEST(RPositioner, GetUpdateOptionsPanics_NotConnected, "PPAN", 9)
	{
	DO_PANIC_TEST_L(GetUpdateOptionsPanics_NotConnected, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt GetLastKnownPositionPanic(TAny* /*aPtr*/)
	{
	RPositioner positioner;
	TPositionInfo posInfo;
	TRequestStatus status;
	positioner.GetLastKnownPosition(posInfo, status);	//ShouldPanic
	return KErrGeneral;
	}

TEST(RPositioner, Panic_GetLastKnownPosition, "PPAN", 10)
	{
	DO_PANIC_TEST_L(GetLastKnownPositionPanic, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}

TInt NotifyPositionUpdatePanic(TAny* /*aPtr*/)
	{
	RPositioner positioner;
	TPositionInfo posInfo;
	TRequestStatus status;
	positioner.NotifyPositionUpdate(posInfo, status);	//ShouldPanic
	return KErrGeneral;
	}

TEST(RPositioner, Panic_NotifyPositionUpdate, "PPAN", 11)
	{
	DO_PANIC_TEST_L(NotifyPositionUpdatePanic, KPosClientFault, 
					EPositionServerBadHandle, KDefaultTestTimeout);
	}


//
// Utility functions
//

// NOTE: the CHECK macros cannot be used from these functions... //

void CleanupResetAndDestroy(TAny* aArray)
	{
	reinterpret_cast<RRequestorStack*>(aArray)->ResetAndDestroy();
	}

//
// End of file
