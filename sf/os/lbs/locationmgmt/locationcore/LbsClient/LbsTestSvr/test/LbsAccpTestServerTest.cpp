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
// LbsTestServerTest.cpp
// 
//

// Comment out this line to have more verbose output
//#define VERBOSE 0

#include "unittest.h"
#include "testregistry.h"
#include "test.h"
#include <lbs.h>
#include <lbssatellite.h>

#include <s32mem.h>
#include <lbsptrholder.h>
#include "TestDummyValues.h"
#include "TestDummyExtClasses.h"

// Utility function prototypes
void CleanupResetAndDestroy(TAny* aArray);
void CleanupCloseThreadArray(TAny* aArray);
TInt SetAuthenticRequestor(RPositioner& aPositioner);
void ConnectThreadFuncL();
TInt ConnectThreadFunc(TAny* /*aPtr*/);

void RepeatNotifyModuleStatusEventFuncL();
void RepeatNotifyPositionUpdateFuncL();
void RepeatGetLastKnownPositioneFuncL();

// Pointer to one of the above test functions that leave
typedef void (*TTestThreadFuncL)();

TInt RepeatTestThreadFunc(TAny* aPtr);

// Global constants
const TInt KNumberOfFunctionCallsPerThread = 50;
const TInt KNumberOfFunctionCallsInEnduranceTest = 500;

//
// A macro to reduce the duplication of OOM test code.
//
// Invoke like this:
//	DO_OOM(CServerPositioningPtrHolder* ptrHolder = NULL,
//		   ptrHolder = CServerPositioningPtrHolder::NewL(17,17),
//		   delete ptrHolder);

#define DO_OOM_CUSTOMISED_LOOP(aType, aCreationCode, aDeletionCode, aLoopStartCode, aLoopEndCode)  \
    aType;                                                                   \
    TInt failureRate;                                                        \
    TBool doContinue = ETrue;                                                \
    for (failureRate = 1; doContinue; failureRate++)                         \
        {                                                                    \
		aLoopStartCode;                                                      \
        __UHEAP_SETFAIL(RHeap::EFailNext, failureRate);                      \
        __UHEAP_MARK;                                                        \
                                                                             \
        TRAPD(err, aCreationCode);                                           \
        if (err != KErrNoMemory)                                             \
            {                                                                \
            aDeletionCode;                                                   \
            doContinue = EFalse;                                             \
            }                                                                \
                                                                             \
        __UHEAP_MARKEND;                                                     \
        __UHEAP_RESET;                                                       \
		aLoopEndCode;                                                        \
        }                                                                    \
    CHECK( failureRate > 2 );                                                \
    VERBOSE_OUTPUT3(_L("Needed %d allocations for \"%s\" to succeed.\n"), failureRate-2, L## #aCreationCode)


#define DO_OOM(aType, aCreationCode, aDeletionCode) \
    DO_OOM_CUSTOMISED_LOOP(aType, aCreationCode, aDeletionCode, (void) 0, (void) 0)



/*

  All of the tests are individually TRAP'ed, so the code can leave.

  In setting up the tests, User::LeaveIfError is used, so a leave indicates
  abnormal termination of the test. In checking the expected results of
  each test, one of the CHECK.. macros is used, which will potentially set
  the test result (iResult field of TTest) to EFailed.

  Tests can print messages to the screen by using the global CConsoleBase* console
  eg. 	VERBOSE_OUTPUT2(_L("Hello world\n"));

  In real unit tests, the data returned by each API method would need to be
  tested. At present, that is only the case in the RPositioner::NotifyPositionUpdate
  tests.

*/

//
// Simultaneous Connect to RPositionServer tests
//

TEST(RPositionServer, Connect_NoMemLeak, "ACC1", 1)
	{
	RPositionServer server;
	__UHEAP_MARK;
	CHECK_EQUAL( KErrNone, server.Connect());
	server.Close();
	__UHEAP_MARKEND;
	}

TEST(RPositionServer, Connect_Simultaneous, "ACC1", 1)
	{
	VERBOSE_OUTPUT(_L("\t\tWarning: this may take a minute or two.\n"));

	const TInt KNumThreads = 40;
	TFixedArray<RThread, KNumThreads>* threads = new(ELeave) TFixedArray<RThread, KNumThreads>();
	TArray<RThread> threadArray(threads->Array());
	TFixedArray<TRequestStatus, KNumThreads> reqStatuses;

	CleanupStack::PushL(threads);
	CleanupStack::PushL(TCleanupItem(CleanupCloseThreadArray, &threadArray));

	TInt i;

	VERBOSE_OUTPUT(_L("\nCreating:\n"));
	for (i = 0; i < KNumThreads; i++)
		{
		TBuf<20> name(_L("LbsTestThread_"));
		name.AppendNum(i);
		User::LeaveIfError((*threads)[i].Create(name, ConnectThreadFunc, KDefaultStackSize, KMinHeapSize, 0x10000, NULL));
		VERBOSE_OUTPUT(_L("."));
		}

	VERBOSE_OUTPUT(_L("\nLogging on:\n"));
	for (i = 0; i < KNumThreads; i++)
		{
		(*threads)[i].Logon(reqStatuses[i]);
		VERBOSE_OUTPUT(_L("."));
		}

	VERBOSE_OUTPUT(_L("\nResuming:\n"));
	for (i = 0; i < KNumThreads; i++)
		{
		(*threads)[i].Resume();
		VERBOSE_OUTPUT(_L("."));
		}

	VERBOSE_OUTPUT(_L("\nWaiting:\n"));
	for (i = 0; i < KNumThreads; i++)
		{
		User::WaitForAnyRequest();
		VERBOSE_OUTPUT(_L("."));
		}

	VERBOSE_OUTPUT(_L("\n"));
	for (i = 0; i < KNumThreads; i++)
		CHECK_EQUAL( KErrNone, reqStatuses[i].Int());

	CleanupStack::PopAndDestroy(&threadArray);
	CleanupStack::PopAndDestroy(threads);
	}

TEST(RPositionServer, Connect_Endurance, "AROM", 1)
	{
	VERBOSE_OUTPUT(_L("\t\tWarning: this may take a minute or two.\n"));
	for (TInt i = 0; i < KNumberOfFunctionCallsInEnduranceTest; i++)
		{
		RPositionServer server;
		User::LeaveIfError(server.Connect());
		server.Close();
		}
	}

TEST(RPositioner, Open_Endurance, "AROM", 1)
	{
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);
	for (TInt i = 0; i < KNumberOfFunctionCallsInEnduranceTest; i++)
		{
		RPositioner positioner;
		User::LeaveIfError(positioner.Open(server));
		positioner.Close();
		}
	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, Open_NoMemLeak, "AOC1", 1)
	{
	RPositionServer server;
	RPositioner positioner;
	TPositionModuleId moduleId = TUid::Null();
	TPositionCriteria criteria;

	User::LeaveIfError( server.Connect());

	__UHEAP_MARK;
	CHECK_EQUAL( KErrNone, positioner.Open(server));
	positioner.Close();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	CHECK_EQUAL( KErrNone, positioner.Open(server, moduleId));
	positioner.Close();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	CHECK_EQUAL( KErrNone, positioner.Open(server, criteria));
	positioner.Close();
	__UHEAP_MARKEND;

	server.Close();
	}

//
// OOM Tests
//

TEST(RPositionerServer, Connect_OOM, "ASOM", 1)
	{
	DO_OOM
		(
		RPositionServer server,
		User::LeaveIfError(server.Connect()),
		server.Close()
		);
	}

TEST(RPositioner, Open_OOM, "AROM", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	DO_OOM
		(
		RPositioner positioner,
		User::LeaveIfError(positioner.Open(server)),
		positioner.Close()
		);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositioner, OpenModuleId_OOM, "AROM", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());
	TPositionModuleId modId = TUid::Uid(DUMMY_MODULE_ID1);

	DO_OOM
		(
		RPositioner positioner,
		User::LeaveIfError(positioner.Open(server, modId)),
		positioner.Close()
		);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositioner, OpenCriteria_OOM, "AROM", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());
	TPositionCriteria criteria;

	DO_OOM
		(
		RPositioner positioner,
		User::LeaveIfError(positioner.Open(server, criteria)),
		positioner.Close()
		);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(CRequestor, New_OOM, "ACOM", 1)
	{
	_LIT(KRequestorData, "+441223719800");

	DO_OOM
		(
		CRequestor* requestor = NULL,
		User::LeaveIfNull(requestor = CRequestor::New(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData)),
		delete requestor
		);
	}

TEST(CRequestor, NewL_OOM, "ACOM", 1)
	{
	_LIT(KRequestorData, "+441223719800");

	DO_OOM
		(
		CRequestor* requestor = NULL,
		requestor = CRequestor::NewL(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData),
		delete requestor
		);
	}

TEST(CRequestor, NewLC_OOM, "ACOM", 1)
	{
	_LIT(KRequestorData, "+441223719800");

	DO_OOM
		(
		CRequestor* requestor = NULL,
		requestor = CRequestor::NewLC(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData);
			CleanupStack::Pop(requestor),
		delete requestor
		);
	}

TEST(CRequestor, SetRequestorL_OOM, "ACOM", 1)
	{
	_LIT(KRequestorData, "+441223719800");
	_LIT(KRequestorData2, "+44122371980101");

	// NOTE, this is a special case where the CRequestor is created at the start, then
	// its member data is replaced (which is tested under OOM conditions).
	DO_OOM
		(
		CRequestor* requestor = CRequestor::NewLC(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData),
		requestor->SetRequestorL(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData2),
		CleanupStack::PopAndDestroy(requestor)
		);
	}

TEST(CRequestor, InternalizeL_OOM, "ACOM", 1)
	{
	_LIT(KRequestorData, "+441223719800");
	TInt err = KErrNone;
	CRequestor* requestor = CRequestor::NewLC(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData);
	TBuf8<100> buffer;
	RDesWriteStream writeStrm(buffer);
	CleanupClosePushL(writeStrm);
	TRAP(err, writeStrm << *requestor);
	CleanupStack::PopAndDestroy(&writeStrm);

	DO_OOM_CUSTOMISED_LOOP
		(
		(void) 0,
		strm >> *requestor,
		delete requestor,
		RDesReadStream strm(buffer); CleanupClosePushL(strm),
		CleanupStack::PopAndDestroy(&strm)
		);

	CleanupStack::Pop(requestor);
	}

TEST(RRequestorStack, InternalizeL_OOM, "ASTO", 1)
	{
	_LIT(KRequestorData, "+441223719800");
	TInt err = KErrNone;
	const TInt KNumberOfRequestors = 3;
	RRequestorStack stack;
	CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &stack));
	TInt i;
	for (i = 0; i < KNumberOfRequestors; i++)
		User::LeaveIfError(stack.Append(NULL));
	for (i = 0; i < KNumberOfRequestors; i++)
		stack[i] = CRequestor::NewL(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData);

	TBuf8<100> buffer;
	RDesWriteStream writeStrm(buffer);
	CleanupClosePushL(writeStrm);
	TRAP(err, writeStrm << stack);
	CleanupStack::PopAndDestroy(&writeStrm);

	CleanupStack::PopAndDestroy(&stack);
	CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &stack));

	DO_OOM_CUSTOMISED_LOOP
		(
		(void) 0,
		strm >> stack,
		stack.ResetAndDestroy(),
		RDesReadStream strm(buffer); CleanupClosePushL(strm),
		CleanupStack::PopAndDestroy(&strm)
		);

	CleanupStack::Pop(&stack);
	}

TEST(CPositioningPtrHolder, NewL_OOM, "APHO", 1)
	{
	DO_OOM
		(
		CPositioningPtrHolder* ptrHolder = NULL,
		ptrHolder = CPositioningPtrHolder::NewL(17,17),
		delete ptrHolder
		);
	}

TEST(CServerPositioningPtrHolder, NewL_OOM, "APHO", 1)
	{
	DO_OOM
		(
		CServerPositioningPtrHolder* ptrHolder = NULL,
		ptrHolder = CServerPositioningPtrHolder::NewL(17,17),
		delete ptrHolder
		);
	}

TEST(RPositionServer, ModStatusEvent_LastKnownPos, "ASN1", 1)
	{
	// Check that the notification is produced for every change that
	// should prompt a notification.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	CHECK_EQUAL( KErrNone, SetAuthenticRequestor(positioner));

	// Request #1 - on RPositionServer
	TPositionModuleStatusEvent statusEvent;
	TRequestStatus notifyModuleStatusEventStatus;
	statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
	server.NotifyModuleStatusEvent(statusEvent, notifyModuleStatusEventStatus, TUid::Uid(DUMMY_MODULE_ID1));

	// Request #2 - on RPositioner
	TPositionInfo posInfo;
	TRequestStatus getLastKnownPositionStatus;
	positioner.GetLastKnownPosition(posInfo, getLastKnownPositionStatus);

	// Wait for both to complete
	User::WaitForRequest(getLastKnownPositionStatus, notifyModuleStatusEventStatus);
	User::WaitForRequest(getLastKnownPositionStatus, notifyModuleStatusEventStatus);

	CHECK_EQUAL( KErrNone, getLastKnownPositionStatus.Int() );
	CHECK_EQUAL( KErrNone, notifyModuleStatusEventStatus.Int());

	// Check NotifyModuleStatusEvent request data
	CHECK_EQUAL( DUMMY_STATUS_EVENT_REQ_EVENTS, statusEvent.RequestedEvents());
	CHECK_EQUAL( DUMMY_STATUS_EVENT_OCC_EVENTS, statusEvent.OccurredEvents());
	CHECK_EQUAL( DUMMY_MODULE_ID1, statusEvent.ModuleId().iUid);

	// Check the TPositionModuleStatus part of TPositionModuleStatusEvent
	TPositionModuleStatus moduleStatus;
	statusEvent.GetModuleStatus(moduleStatus);
	CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_STATUS, moduleStatus.DeviceStatus());
	CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_QUAL_STATUS, moduleStatus.DataQualityStatus());

	// Check GetLastKnownPosition request data
	TPosition position;
	posInfo.GetPosition(position);
	CHECK_EQUAL( DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_VERTICAL_ACCURACY, position.VerticalAccuracy());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_ALTITUDE, position.Altitude());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositionServer, ModStatusEvent_LastKnownPos_Threads, "ASN1", 2)
	{
	const TInt KNumThreads = 2;
	TFixedArray<RThread, KNumThreads> threads;
	TArray<RThread> threadArray(threads.Array());
	TFixedArray<TRequestStatus, KNumThreads> reqStatuses;

	CleanupStack::PushL(TCleanupItem(CleanupCloseThreadArray, &threadArray));

	User::LeaveIfError(
		threads[0].Create(_L("LbsThread1"), RepeatTestThreadFunc, KDefaultStackSize, 0x100, 0x10000, (TAny*) (RepeatNotifyModuleStatusEventFuncL))
	);

	User::LeaveIfError(
		threads[1].Create(_L("LbsThread2"), RepeatTestThreadFunc, KDefaultStackSize, 0x100, 0x10000, (TAny*) (RepeatGetLastKnownPositioneFuncL))
	);

	TInt i;
	for (i = 0; i < KNumThreads; i++)
		threads[i].Logon(reqStatuses[i]);

	for (i = 0; i < KNumThreads; i++)
		threads[i].Resume();

	for (i = 0; i < KNumThreads; i++)
		User::WaitForAnyRequest();

	for (i = 0; i < KNumThreads; i++)
		CHECK_EQUAL( KErrNone, reqStatuses[i].Int());

	CleanupStack::PopAndDestroy(&threadArray);
	threads.Reset();
	}

TEST(RPositionServer, ModStatusEvent_LastKnownPos_NotifyPos, "ASN2", 1)
	{
	// Check that the notification is produced for every change that
	// should prompt a notification.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	CHECK_EQUAL( KErrNone, SetAuthenticRequestor(positioner));

	// Request #1 - on RPositionServer
	TPositionModuleStatusEvent statusEvent;
	TRequestStatus notifyModuleStatusEventStatus;
	statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
	server.NotifyModuleStatusEvent(statusEvent, notifyModuleStatusEventStatus, TUid::Uid(DUMMY_MODULE_ID1));

	// Request #2 - on RPositioner
	TPositionInfo getLastKnownPositionInfo;
	TRequestStatus getLastKnownPositionStatus;
	positioner.GetLastKnownPosition(getLastKnownPositionInfo, getLastKnownPositionStatus);

	// Request #3 - on RPositioner
	TPositionInfo notifyPositionUpdateInfo;
	TRequestStatus notifyPositionUpdateStatus;
	positioner.NotifyPositionUpdate(notifyPositionUpdateInfo, notifyPositionUpdateStatus);

	// Wait for all three to complete
	User::WaitForRequest(getLastKnownPositionStatus);
	User::WaitForRequest(notifyModuleStatusEventStatus);
	User::WaitForRequest(notifyPositionUpdateStatus);

	CHECK_EQUAL( KErrNone, getLastKnownPositionStatus.Int() );
	CHECK_EQUAL( KErrNone, notifyModuleStatusEventStatus.Int());
	CHECK_EQUAL( KErrNone, notifyModuleStatusEventStatus.Int());

	// Check NotifyModuleStatusEvent request data
	CHECK_EQUAL( DUMMY_STATUS_EVENT_REQ_EVENTS, statusEvent.RequestedEvents());
	CHECK_EQUAL( DUMMY_STATUS_EVENT_OCC_EVENTS, statusEvent.OccurredEvents());
	CHECK_EQUAL( DUMMY_MODULE_ID1, statusEvent.ModuleId().iUid);

	// Check the TPositionModuleStatus part of TPositionModuleStatusEvent
	TPositionModuleStatus moduleStatus;
	statusEvent.GetModuleStatus(moduleStatus);
	CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_STATUS, moduleStatus.DeviceStatus());
	CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_QUAL_STATUS, moduleStatus.DataQualityStatus());

	// Check GetLastKnownPosition request data
	TPosition position;
	getLastKnownPositionInfo.GetPosition(position);
	CHECK_EQUAL( DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_VERTICAL_ACCURACY, position.VerticalAccuracy());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_ALTITUDE, position.Altitude());

	// Check NotifyPositionUpdate request data
	notifyPositionUpdateInfo.GetPosition(position);
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY, position.VerticalAccuracy());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositionServer, ModStatusEvent_LastKnownPos_NotifyPos_Threads, "ASN2", 2)
	{
	const TInt KNumThreads = 3;
	TFixedArray<RThread, KNumThreads> threads;
	TArray<RThread> threadArray(threads.Array());
	TFixedArray<TRequestStatus, KNumThreads> reqStatuses;

	CleanupStack::PushL(TCleanupItem(CleanupCloseThreadArray, &threadArray));

	User::LeaveIfError(
		threads[0].Create(_L("LbsThread1"), RepeatTestThreadFunc, KDefaultStackSize, 0x100, 0x10000, (TAny*)RepeatNotifyModuleStatusEventFuncL)
	);

	User::LeaveIfError(
		threads[1].Create(_L("LbsThread2"), RepeatTestThreadFunc, KDefaultStackSize, 0x100, 0x10000, (TAny*)RepeatGetLastKnownPositioneFuncL)
	);

	User::LeaveIfError(
		threads[2].Create(_L("LbsThread3"), RepeatTestThreadFunc, KDefaultStackSize, 0x100, 0x10000, (TAny*)RepeatNotifyPositionUpdateFuncL)
	);

	TInt i;
	for (i = 0; i < KNumThreads; i++)
		threads[i].Logon(reqStatuses[i]);

	for (i = 0; i < KNumThreads; i++)
		threads[i].Resume();

	for (i = 0; i < KNumThreads; i++)
		User::WaitForAnyRequest();

	for (i = 0; i < KNumThreads; i++)
		CHECK_EQUAL( KErrNone, reqStatuses[i].Int());

	CleanupStack::PopAndDestroy(&threadArray);
	threads.Reset();
	}


//
// Utility functions
//

// NOTE: the CHECK macros cannot be used from these functions... //

void CleanupResetAndDestroy(TAny* aArray)
	{
	reinterpret_cast<RRequestorStack*>(aArray)->ResetAndDestroy();
	}

void CleanupCloseThreadArray(TAny* aArray)
	{
	TArray<RThread> array(*reinterpret_cast<TArray<RThread>*>(aArray));

	for (TInt count = 0; count < array.Count(); count++)
		{
		RThread thread(array[count]);
		thread.Close();
		}
	}

TInt SetAuthenticRequestor(RPositioner& aPositioner)
	{
	return aPositioner.SetRequestor(DUMMY_REQUESTOR_TYPE_ACCEPTED1, DUMMY_REQUESTOR_FORMAT_ACCEPTED1, DUMMY_REQUESTOR_NAME_ACCEPTED1);
	}

void ConnectThreadFuncL()
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());
	CleanupStack::PopAndDestroy(&server);
	}

TInt ConnectThreadFunc(TAny* /*aPtr*/)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(err, ConnectThreadFuncL());
	delete cleanup;
	return err;
	}

void RepeatNotifyModuleStatusEventFuncL()
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	for (TInt i = 0; i < KNumberOfFunctionCallsPerThread; i++)
		{
		TPositionModuleStatusEvent statusEvent;
		TRequestStatus notifyModuleStatusEventStatus;
		statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
		server.NotifyModuleStatusEvent(statusEvent, notifyModuleStatusEventStatus, TUid::Uid(DUMMY_MODULE_ID1));
		User::WaitForRequest(notifyModuleStatusEventStatus);
		User::LeaveIfError(notifyModuleStatusEventStatus.Int());
		}

	CleanupStack::PopAndDestroy(&server);
	}

void RepeatNotifyPositionUpdateFuncL()
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	for (TInt i = 0; i < KNumberOfFunctionCallsPerThread; i++)
		{
		TPositionInfo posInfo;
		TRequestStatus notifyPositionUpdateStatus;
		positioner.NotifyPositionUpdate(posInfo, notifyPositionUpdateStatus);
		User::WaitForRequest(notifyPositionUpdateStatus);
		User::LeaveIfError(notifyPositionUpdateStatus.Int());
		}

	CleanupStack::PopAndDestroy(2, &server);
	}

void RepeatGetLastKnownPositioneFuncL()
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	for (TInt i = 0; i < KNumberOfFunctionCallsPerThread; i++)
		{
		TPositionInfo posInfo;
		TRequestStatus getLastKnownPositionStatus;
		positioner.GetLastKnownPosition(posInfo, getLastKnownPositionStatus);
		User::WaitForRequest(getLastKnownPositionStatus);
		User::LeaveIfError(getLastKnownPositionStatus.Int());
		}

	CleanupStack::PopAndDestroy(2, &server);
	}

TInt RepeatTestThreadFunc(TAny* aPtr)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	// ANSI C++ forbids reinterpret_cast to a function pointer
	TTestThreadFuncL testFunctionL = (TTestThreadFuncL) (aPtr);
	TRAPD(err, testFunctionL());
	delete cleanup;
	return err;
	}


//
// End of file
