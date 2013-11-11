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

#include "TestDummyValues.h"
#include "TestDummyExtClasses.h"

// Utility function prototypes
void CleanupResetAndDestroy(TAny* aArray);
TInt SetAuthenticRequestor(RPositioner& aPositioner);

_LIT(KDirectoryForExesOnTarget, "\\system\\programs\\");
_LIT(KEposServerImg,"LbsTestSvr.exe");

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

TEST(RPositionServer, Close_Has_No_Side_Effects, "PSOC", 1)
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
		CHECK_EQUAL( 0, server.Handle() );
		}
	}

TEST(RPositionServer, Connect, "PSOC", 2)
	{
	// Try different open methods (from server, from positioner, etc.)
	RPositionServer server;
	CHECK_EQUAL( 0, server.Handle() );
	CHECK_EQUAL( KErrNone, server.Connect() );
	CHECK( server.Handle() > 0 );
	server.Close();
	}

TEST(RPositionServer, Connect_TwoClients, "PSOC", 2)
	{
	RPositionServer server1, server2;
	CleanupClosePushL(server1);
	CleanupClosePushL(server2);

	CHECK_EQUAL( KErrNone, server1.Connect() );
	CHECK_EQUAL( KErrNone, server2.Connect() );

	CleanupStack::PopAndDestroy(2, &server1);
	}

#ifdef __EPOC32__
TEST(RPositionServer, Connect_ServerExeMissing, "PSOC", 2)
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
	User::LeaveIfError(exeFinder.FindByDir(KEposServerImg, KDirectoryForExesOnTarget));

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

TEST(RPositionServer, Close, "PSOC", 3)
	{
	// Ensure that closing a client sends a message telling the server
	// to close the session.
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CHECK( server.Handle() > 0 );
	server.Close();
	CHECK_EQUAL( 0, server.Handle() );
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

TEST(RPositionServer, GetDefaultModuleId, "PSMI", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleId moduleId;
	CHECK_EQUAL( KErrNone, server.GetDefaultModuleId(moduleId));
	CHECK_EQUAL( DUMMY_MODULE_ID_DEF, moduleId.iUid);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, GetNumModules, "PSMI", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TUint numModules;
	CHECK_EQUAL( KErrNone, server.GetNumModules(numModules));
	CHECK_EQUAL( DUMMY_NUM_MODULES, numModules);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, GetModuleInfoByIndex, "PSMI", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleInfo moduleInfo;
	TPositionQuality posQuality;
	TBuf<50> moduleName;

	// Test first module
	CHECK_EQUAL( KErrNone, server.GetModuleInfoByIndex(0, moduleInfo));

	CHECK_EQUAL( DUMMY_MODULE_ID1, moduleInfo.ModuleId().iUid);
	TBool dummyBool = DUMMY_MODULE_INFO_IS_AVAILABLE1;
	if(dummyBool)
		{
		CHECK(moduleInfo.IsAvailable())
		}
	else
		{
		CHECK(!moduleInfo.IsAvailable());
		}
	CHECK_EQUAL( DUMMY_MODULE_INFO_TECHNOLOGY_TYPE1, moduleInfo.TechnologyType());
	CHECK_EQUAL( DUMMY_MODULE_INFO_DEVICE_LOCATION1, moduleInfo.DeviceLocation());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CAPABILITIES1, moduleInfo.Capabilities());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CLASSES_SUPPORTED1, moduleInfo.ClassesSupported(EPositionInfoFamily));
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERSION1.Name(), moduleInfo.Version().Name());

	moduleInfo.GetPositionQuality(posQuality);
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX1, posQuality.TimeToFirstFix());
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX1, posQuality.TimeToNextFix()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY1, posQuality.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERTICAL_ACCURACY1, posQuality.VerticalAccuracy()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_COST_INDICATOR1, posQuality.CostIndicator());
	CHECK_EQUAL( DUMMY_MODULE_INFO_POWER_CONSUMPTION1, posQuality.PowerConsumption());

	moduleInfo.GetModuleName(moduleName);
	CHECK_EQUAL( DUMMY_MODULE_INFO_MOD_NAME1, moduleName);

	// Test second module
	CHECK_EQUAL( KErrNone, server.GetModuleInfoByIndex(1, moduleInfo));

	CHECK_EQUAL( DUMMY_MODULE_ID2, moduleInfo.ModuleId().iUid);
	dummyBool = DUMMY_MODULE_INFO_IS_AVAILABLE2;
	if(dummyBool)
		{
		CHECK(moduleInfo.IsAvailable())
		}
	else
		{
		CHECK(!moduleInfo.IsAvailable());
		}
	CHECK_EQUAL( DUMMY_MODULE_INFO_TECHNOLOGY_TYPE2, moduleInfo.TechnologyType());
	CHECK_EQUAL( DUMMY_MODULE_INFO_DEVICE_LOCATION2, moduleInfo.DeviceLocation());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CAPABILITIES2, moduleInfo.Capabilities());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CLASSES_SUPPORTED2, moduleInfo.ClassesSupported(EPositionInfoFamily));
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERSION2.Name(), moduleInfo.Version().Name());

	moduleInfo.GetPositionQuality(posQuality);
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX2, posQuality.TimeToFirstFix());
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX2, posQuality.TimeToNextFix()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY2, posQuality.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERTICAL_ACCURACY2, posQuality.VerticalAccuracy()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_COST_INDICATOR2, posQuality.CostIndicator());
	CHECK_EQUAL( DUMMY_MODULE_INFO_POWER_CONSUMPTION2, posQuality.PowerConsumption());

	moduleInfo.GetModuleName(moduleName);
	CHECK_EQUAL( DUMMY_MODULE_INFO_MOD_NAME2, moduleName);

	// Test third module
	CHECK_EQUAL( KErrNone, server.GetModuleInfoByIndex(2, moduleInfo));

	CHECK_EQUAL( DUMMY_MODULE_ID3, moduleInfo.ModuleId().iUid);
	dummyBool = DUMMY_MODULE_INFO_IS_AVAILABLE3;
	if(dummyBool)
		{
		CHECK(moduleInfo.IsAvailable())
		}
	else
		{
		CHECK(!moduleInfo.IsAvailable());
		}
	CHECK_EQUAL( DUMMY_MODULE_INFO_TECHNOLOGY_TYPE3, moduleInfo.TechnologyType());
	CHECK_EQUAL( DUMMY_MODULE_INFO_DEVICE_LOCATION3, moduleInfo.DeviceLocation());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CAPABILITIES3, moduleInfo.Capabilities());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CLASSES_SUPPORTED3, moduleInfo.ClassesSupported(EPositionInfoFamily));
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERSION3.Name(), moduleInfo.Version().Name());

	moduleInfo.GetPositionQuality(posQuality);
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX3, posQuality.TimeToFirstFix());
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX3, posQuality.TimeToNextFix()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY3, posQuality.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERTICAL_ACCURACY3, posQuality.VerticalAccuracy()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_COST_INDICATOR3, posQuality.CostIndicator());
	CHECK_EQUAL( DUMMY_MODULE_INFO_POWER_CONSUMPTION3, posQuality.PowerConsumption());

	moduleInfo.GetModuleName(moduleName);
	CHECK_EQUAL( DUMMY_MODULE_INFO_MOD_NAME3, moduleName);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, GetModuleInfoById, "PSMI", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleInfo moduleInfo;
	TPositionQuality posQuality;
	TBuf<50> moduleName;

	CHECK_EQUAL( KErrNone, server.GetModuleInfoById(TUid::Uid(DUMMY_MODULE_ID1), moduleInfo));

	CHECK_EQUAL( DUMMY_MODULE_ID1, moduleInfo.ModuleId().iUid);
	TBool dummyBool = DUMMY_MODULE_INFO_IS_AVAILABLE1;
	if(dummyBool)
		{
		CHECK(moduleInfo.IsAvailable())
		}
	else
		{
		CHECK(!moduleInfo.IsAvailable());
		}
	CHECK_EQUAL( DUMMY_MODULE_INFO_TECHNOLOGY_TYPE1, moduleInfo.TechnologyType());
	CHECK_EQUAL( DUMMY_MODULE_INFO_DEVICE_LOCATION1, moduleInfo.DeviceLocation());
	CHECK_EQUAL( DUMMY_MODULE_INFO_CLASSES_SUPPORTED1, moduleInfo.ClassesSupported(EPositionInfoFamily));
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERSION1.Name(), moduleInfo.Version().Name());

	moduleInfo.GetPositionQuality(posQuality);
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX1, posQuality.TimeToFirstFix());
	CHECK_EQUAL( DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX1, posQuality.TimeToNextFix()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY1, posQuality.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_MODULE_INFO_VERTICAL_ACCURACY1, posQuality.VerticalAccuracy()); 
	CHECK_EQUAL( DUMMY_MODULE_INFO_COST_INDICATOR1, posQuality.CostIndicator());
	CHECK_EQUAL( DUMMY_MODULE_INFO_POWER_CONSUMPTION1, posQuality.PowerConsumption());

	moduleInfo.GetModuleName(moduleName);
	CHECK_EQUAL( DUMMY_MODULE_INFO_MOD_NAME1, moduleName);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, ModuleInfo_Bad_Identifiers, "PSMI", 2)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleInfo moduleInfo;
	CHECK_EQUAL( KErrNotFound, server.GetModuleInfoByIndex(12, moduleInfo));
	CHECK_EQUAL( KErrNotFound, server.GetModuleInfoById(TUid::Uid(DUMMY_MODULE_BAD_ID), moduleInfo));

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, GetModuleStatus, "PSMS", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleStatus posModuleStatus;

	// Check dummy data for known IDs
	CHECK_EQUAL( KErrNone, server.GetModuleStatus(posModuleStatus, TUid::Uid(DUMMY_MODULE_ID1)));
	CHECK_EQUAL( DUMMY_MODULE_STATUS1, posModuleStatus.DeviceStatus());
	CHECK_EQUAL( DUMMY_MODULE_DATA_QUAL_STATUS1, posModuleStatus.DataQualityStatus());

	CHECK_EQUAL( KErrNone, server.GetModuleStatus(posModuleStatus, TUid::Uid(DUMMY_MODULE_ID2)));
	CHECK_EQUAL( DUMMY_MODULE_STATUS2, posModuleStatus.DeviceStatus());
	CHECK_EQUAL( DUMMY_MODULE_DATA_QUAL_STATUS2, posModuleStatus.DataQualityStatus());

	CHECK_EQUAL( KErrNone, server.GetModuleStatus(posModuleStatus, TUid::Uid(DUMMY_MODULE_ID3)));
	CHECK_EQUAL( DUMMY_MODULE_STATUS3, posModuleStatus.DeviceStatus());
	CHECK_EQUAL( DUMMY_MODULE_DATA_QUAL_STATUS3, posModuleStatus.DataQualityStatus());

	// Check bad module IDs
	CHECK_EQUAL( KErrNotFound, server.GetModuleStatus(posModuleStatus, TUid::Uid(DUMMY_MODULE_BAD_ID)));

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, NotifyModuleStatusEvent_Bad_Id, "PSMS", 1)
	{
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleStatusEvent statusEvent;
	TRequestStatus reqStatus;
	statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
	server.NotifyModuleStatusEvent(statusEvent, reqStatus, TUid::Uid(DUMMY_MODULE_BAD_ID));
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNotFound, reqStatus.Int());

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, NotifyModuleStatusEvent, "PSMS", 2)
	{
	// Check that the notification is produced for every change that
	// should prompt a notification.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleId moduleId;
	User::LeaveIfError(server.GetDefaultModuleId(moduleId));

	TPositionModuleStatusEvent statusEvent;
	TRequestStatus reqStatus;
	statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
	server.NotifyModuleStatusEvent(statusEvent, reqStatus, moduleId);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int());

	/*-------------
		Two more generic tests...
	-*-------------
	CHECK( statusEvent.OccurredEvents() & statusEvent.RequestedEvents());
	CHECK( statusEvent.OccurredEvents() | statusEvent.RequestedEvents() == statusEvent.RequestedEvents());
	-*------------*/
	CHECK_EQUAL( DUMMY_STATUS_EVENT_REQ_EVENTS, statusEvent.RequestedEvents());
	CHECK_EQUAL( DUMMY_STATUS_EVENT_OCC_EVENTS, statusEvent.OccurredEvents());
	CHECK_EQUAL( moduleId.iUid, statusEvent.ModuleId().iUid);

	// Check the TPositionModuleStatus part of TPositionModuleStatusEvent
	TPositionModuleStatus moduleStatus;
	statusEvent.GetModuleStatus(moduleStatus);
	CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_STATUS, moduleStatus.DeviceStatus());
	CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_QUAL_STATUS, moduleStatus.DataQualityStatus());

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, NotifyModuleStatusEventCancel, "PSMS", 3)
	{
	// Check notification cancel operations are handled correctly.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionModuleStatusEvent statusEvent;
	TRequestStatus reqStatus;
	statusEvent.SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
	server.NotifyModuleStatusEvent(statusEvent, reqStatus, TUid::Uid(DUMMY_MODULE_ID1));
	server.CancelRequest(EPositionServerNotifyModuleStatusEvent);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrCancel, reqStatus.Int());
	User::After(500000);

	CleanupStack::PopAndDestroy(&server);
	}

TEST(RPositionServer, NotifyModuleStatusEvent_MultipleClients, "PSMS", 4)
	{
	// Check multiple clients registering for notification.
	const TInt KNumClients = 2;
	TInt i;

	RPositionServer servers[KNumClients];
	for (i = 0; i < KNumClients; i++)
		{
		CleanupClosePushL(servers[i]);
		User::LeaveIfError(servers[i].Connect());
		}

	TPositionModuleStatusEvent statusEvent[KNumClients];
	TRequestStatus reqStatus[KNumClients];
	TPositionModuleStatus moduleStatus;
	TPositionModuleId moduleId;

	User::LeaveIfError(servers[0].GetDefaultModuleId(moduleId));

	for (i = 0; i < KNumClients; i++)
		{
		statusEvent[i].SetRequestedEvents(DUMMY_STATUS_EVENT_REQ_EVENTS);
		servers[i].NotifyModuleStatusEvent(statusEvent[i], reqStatus[i], moduleId);
		}

	for (i = 0; i < KNumClients; i++)
		{
		User::WaitForRequest(reqStatus[i]);
		CHECK_EQUAL( KErrNone, reqStatus[i].Int());
		}

	for (i = 0; i < KNumClients; i++)
		{
		CHECK_EQUAL( DUMMY_STATUS_EVENT_REQ_EVENTS, statusEvent[i].RequestedEvents());
		CHECK_EQUAL( DUMMY_STATUS_EVENT_OCC_EVENTS, statusEvent[i].OccurredEvents());
		CHECK_EQUAL( moduleId.iUid, statusEvent[i].ModuleId().iUid);

		statusEvent[i].GetModuleStatus(moduleStatus);
		CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_STATUS, moduleStatus.DeviceStatus());
		CHECK_EQUAL( DUMMY_STATUS_EVENT_DEV_QUAL_STATUS, moduleStatus.DataQualityStatus());
		}

	CleanupStack::PopAndDestroy(KNumClients, &servers[0]);
	}

TEST(RPositionServer, NotifyModuleStatusEvent_DerivedEventClass, "PSMS", 5)
	{
	//Test passes through an object whose class derives from 
	//TPositionModuleStatusEvent. The server does nothing to the object. The test
	//is to check that the reset method on the TPositionModuleStatusEvent derived
	//object is called correctly.

	RPositionServer posServer;
	User::LeaveIfError(posServer.Connect());
	CleanupClosePushL(posServer);

	TExtPosModuleStatusEvent extEvent;
	extEvent.SetRequestedEvents(TExtPosModuleStatusEvent::EEventAll);
	extEvent.SetOccurredEvents(TExtPosModuleStatusEvent::EEventAll);
	TPositionModuleStatus moduleStatus;
	moduleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceStandBy);
	extEvent.SetModuleStatus(moduleStatus);

	TRequestStatus status;

	posServer.NotifyModuleStatusEvent(extEvent, status);

	//Now check everything is as default.

	CHECK(extEvent.PositionClassType() == (EModuleStatusEventTestExtension
										  |EPositionModuleStatusEventClass));
	CHECK_EQUAL(extEvent.PositionClassSize(), sizeof(TExtPosModuleStatusEvent));
	CHECK(extEvent.OccurredEvents()  == TExtPosModuleStatusEvent::EEventNone);
	CHECK(extEvent.RequestedEvents() == TExtPosModuleStatusEvent::EEventAll);

	extEvent.GetModuleStatus(moduleStatus);

	CHECK(moduleStatus.PositionClassType() == EPositionModuleStatusClass);
	CHECK_EQUAL(moduleStatus.PositionClassSize(), sizeof(TPositionModuleStatus));
	CHECK(moduleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceUnknown);

	CleanupStack::PopAndDestroy(&posServer);
	}

//
// Basic RPositioner tests
//

TEST(RPositioner, Close_Has_No_Side_Effects, "POCL", 1)
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
		CHECK_EQUAL( 0, positioner.SubSessionHandle() );
		}
	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, Open_NoParameters, "POCL", 2)
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

TEST(RPositioner, Open_FromModuleId, "POCL", 2)
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

TEST(RPositioner, Open_FromPositionCriteria, "POCL", 2)
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

TEST(RPositioner, Close, "POCL", 3)
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

TEST(RPositioner, SetRequestor_Concurrently, "PREQ", 3)
	{
	// Check that request messages are sent when the particular request-handling
	// object is still processing a previous request.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	// Do not set a valid requestor yet...

	TPositionInfo posInfo;
	TRequestStatus reqStatus;
	positioner.GetLastKnownPosition(posInfo, reqStatus);

	CHECK_EQUAL( KErrNone, SetAuthenticRequestor(positioner));

	// Now that we have not set valid requestor data, the GetLastKnownPosition
	// request should complete with an error.
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int() );

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

TEST(RPositioner, SetUpdateOptions_Range_Tests, "PUPD", 1)
	{
	// Check with all combinations of options for update interval, timeout
	// and information age including zero values and conflicting values.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	TPositionUpdateOptions posOption;
	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	const TInt64 KMicrosecondTestLimit(0x0FFFFFFF00000000);
	const TInt64 KMicrosecondIncrement(0x0100000000000000);
	for (TInt partialUpdates = EFalse; partialUpdates <= ETrue; partialUpdates++)
		{
		for (TInt64 updateInterval = 0; updateInterval < KMicrosecondTestLimit; updateInterval += KMicrosecondIncrement)
			{
			for (TInt64 updateTimeOut = 0; updateTimeOut < KMicrosecondTestLimit; updateTimeOut += KMicrosecondIncrement)
				{
				for (TInt64 maxUpdateAge = 0; maxUpdateAge < KMicrosecondTestLimit; maxUpdateAge += KMicrosecondIncrement)
					{
					posOption.SetUpdateInterval(updateInterval);
					posOption.SetUpdateTimeOut(updateTimeOut);
					posOption.SetMaxUpdateAge(maxUpdateAge);
					posOption.SetAcceptPartialUpdates( partialUpdates );
					User::LeaveIfError(positioner.SetUpdateOptions(posOption));

					TPositionUpdateOptions check;
					User::LeaveIfError(positioner.GetUpdateOptions(check));
					CHECK_EQUAL( posOption.UpdateInterval(), check.UpdateInterval() );
					CHECK_EQUAL( posOption.UpdateTimeOut(), check.UpdateTimeOut() );
					CHECK_EQUAL( posOption.MaxUpdateAge(), check.MaxUpdateAge() );
					CHECK_EQUAL( posOption.AcceptPartialUpdates(), check.AcceptPartialUpdates() );
					}
				}
			}
		}

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, SetUpdateOptions, "PUPD", 2)
	{
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	const TTimeIntervalMicroSeconds KMicroSecondsValue(4000000);
	TPositionUpdateOptions posOption(
		KMicroSecondsValue,
		KMicroSecondsValue,
		KMicroSecondsValue,
		ETrue);

	CHECK_EQUAL( KErrNone, positioner.SetUpdateOptions(posOption));
	posOption = TPositionUpdateOptions();
	CHECK_EQUAL( KErrNone, positioner.GetUpdateOptions(posOption));

	CHECK_EQUAL( posOption.UpdateInterval(), KMicroSecondsValue );
	CHECK_EQUAL( posOption.UpdateTimeOut(), KMicroSecondsValue );
	CHECK_EQUAL( posOption.MaxUpdateAge(), KMicroSecondsValue );
	CHECK_EQUAL( (TBool)ETrue, (TBool)posOption.AcceptPartialUpdates() );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, SetUpdateOptions_Concurrently, "PUPD", 3)
	{
	// Check that request messages are sent when the particular request-handling
	// object is still processing a previous request.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo posInfo;
	TRequestStatus reqStatus;
	positioner.GetLastKnownPosition(posInfo, reqStatus);

	TPositionUpdateOptions posOption(TTimeIntervalMicroSeconds(500000));
	User::LeaveIfError(positioner.SetUpdateOptions(posOption));

	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int() );

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

TEST(RPositioner, GetLastKnownPosition_ClassTypes, "PLKP", 1)
	{
	// Check calling the function with both TPositionInfo classes succeeds
	// and with TPositionInfo-derived classes fails.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	TRequestStatus reqStatus;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo posInfo;
	positioner.GetLastKnownPosition(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int() );

	TPositionCourseInfo courseInfo;
	positioner.GetLastKnownPosition(courseInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrArgument, reqStatus.Int() );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, GetLastKnownPosition, "PLKP", 2)
	{
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo posInfo;
	TRequestStatus reqStatus;
	positioner.GetLastKnownPosition(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int() );

	TPosition position;
	posInfo.GetPosition(position);

	// Dummy server will set the time of the TPosition to the current time.
	// Just check that we are within a second of that time right now.
	TTime now;
	now.UniversalTime();
	CHECK( position.Time() - TTimeIntervalSeconds(1) < now);
	CHECK( position.Time() + TTimeIntervalSeconds(1) > now);
	CHECK_EQUAL( DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_VERTICAL_ACCURACY, position.VerticalAccuracy());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_LAST_KNOWN_ALTITUDE, position.Altitude());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, GetLastKnownPosition_Requestors, "PLKP", 3)
	{
	// Check this using both "authorised" and an "unauthorised" requestor.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	TPositionInfo posInfo;
	TRequestStatus reqStatus;

	// Try an authorised requestor
	User::LeaveIfError(SetAuthenticRequestor(positioner));
	positioner.GetLastKnownPosition(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int() );

	// Try an un-authorised requestor
	positioner.SetRequestor(
		DUMMY_REQUESTOR_TYPE_REJECTED1,
		DUMMY_REQUESTOR_FORMAT_REJECTED1,
		DUMMY_REQUESTOR_NAME_REJECTED1);
	positioner.GetLastKnownPosition(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrAccessDenied, reqStatus.Int() );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, GetLastKnownPosition_Concurrently, "PLKP", 4)
	{
	// Check that request messages are sent when the particular request-handling
	// object is still processing a previous request.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	// Make a Notify request
	TPositionInfo notifyPosInfo;
	TRequestStatus reqNotifyStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqNotifyStatus);

	// Make a GetLastKnownPosition request, check it completes even though
	// there is a Notify request outstanding
	TPositionInfo posInfo;
	TRequestStatus reqGetStatus;
	positioner.GetLastKnownPosition(posInfo, reqGetStatus);
	User::WaitForRequest(reqGetStatus);
	CHECK_EQUAL( KErrNone, reqGetStatus.Int() );

	// Wait for the original (Notify) request
	User::WaitForRequest(reqNotifyStatus);
	CHECK_EQUAL( KErrNone, reqNotifyStatus.Int());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, GetLastKnownPosition_Multiple_Clients, "PLKP", 5)
	{
	// Check the server completing multiple requests for the same function
	// is handled correctly by the client API.

	// Check multiple clients registering for notification.
	const TInt KNumClients = 2;
	TInt i;

	RPositionServer servers[KNumClients];
	RPositioner positioners[KNumClients];
	for (i = 0; i < KNumClients; i++)
		{
		CleanupClosePushL(servers[i]);
		User::LeaveIfError(servers[i].Connect());
		CleanupClosePushL(positioners[i]);
		User::LeaveIfError(positioners[i].Open(servers[i]));
		User::LeaveIfError(SetAuthenticRequestor(positioners[i]));
		}

	TPositionInfo posInfos[KNumClients];
	TRequestStatus reqStatus[KNumClients];

	for (i = 0; i < KNumClients; i++)
		{
		positioners[i].GetLastKnownPosition(posInfos[i], reqStatus[i]);
		}

	for (i = 0; i < KNumClients; i++)
		{
		User::WaitForRequest(reqStatus[i]);
		CHECK_EQUAL( KErrNone, reqStatus[i].Int());
		}

	for (i = 0; i < KNumClients; i++)
		{
		TPosition position;
		posInfos[i].GetPosition(position);

		TTime now;
		now.UniversalTime();
		CHECK( position.Time() - TTimeIntervalSeconds(1) < now);
		CHECK( position.Time() + TTimeIntervalSeconds(1) > now);
		CHECK_EQUAL( DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
		CHECK_EQUAL( DUMMY_LAST_KNOWN_VERTICAL_ACCURACY, position.VerticalAccuracy());
		CHECK_EQUAL( DUMMY_LAST_KNOWN_LATITUDE, position.Latitude());
		CHECK_EQUAL( DUMMY_LAST_KNOWN_LONGITUDE, position.Longitude());
		CHECK_EQUAL( DUMMY_LAST_KNOWN_ALTITUDE, position.Altitude());
		}

	CleanupStack::PopAndDestroy(KNumClients * 2, &servers[0]);
	}

TEST(RPositioner, GetLastKnownPositionCancel, "PLKP", 6)
	{
	// Check request cancellation.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo posInfo;
	TRequestStatus reqStatus;
	positioner.GetLastKnownPosition(posInfo, reqStatus);
	positioner.CancelRequest(EPositionerGetLastKnownPosition);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrCancel, reqStatus.Int() );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, NotifyPositionUpdate_PosInfo, "PNPU", 1)
	{
	// Check the correct message is always sent to the server upon a
	// call to this function and that the return value is always that
	// of the completed message.
	// Test TPositionInfo, a derivative of TPositionInfoBase
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo notifyPosInfo;
	TRequestStatus reqStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int());

	TPosition position;
	notifyPosInfo.GetPosition(position);

	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY, position.VerticalAccuracy());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, NotifyPositionUpdate_CourseInfo, "PNPU", 2)
	{
	// Check with multiple TPositionInfo-derived classes, including ones
	// created specifically to test the client API.
	// Test TPositionCourseInfo, a derivative of TPositionInfoBase
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionCourseInfo notifyPosInfo;
	TRequestStatus reqStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int());

	TCourse course;
	notifyPosInfo.GetCourse(course);

	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_SPEED, course.Speed());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_HEADING, course.Heading());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_SPEED_ACCURACY, course.SpeedAccuracy());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_HEADING_ACCURACY, course.HeadingAccuracy());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, NotifyPositionUpdate_SatelliteInfo, "PNPU", 2)
	{
	// Check with multiple TPositionInfo-derived classes, including ones
	// created specifically to test the client API.
	// Test TPositionSatelliteInfo, a derivative of TPositionInfoBase
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionSatelliteInfo notifyPosInfo;
	TRequestStatus reqStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int());

	TSatelliteData satellite;
	User::LeaveIfError(notifyPosInfo.GetSatelliteData(0, satellite));

	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID1, satellite.SatelliteId());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_AZIMUTH1, satellite.Azimuth());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_ELEVATION1, satellite.Elevation());
	CHECK_EQUAL( (TBool)DUMMY_NOTIFY_POS_UPDATE_IS_USED1, (TBool)satellite.IsUsed());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1, satellite.SignalStrength());

	User::LeaveIfError(notifyPosInfo.GetSatelliteData(1, satellite));
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID2, satellite.SatelliteId());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_AZIMUTH2, satellite.Azimuth());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_ELEVATION2, satellite.Elevation());
	CHECK_EQUAL( (TBool)DUMMY_NOTIFY_POS_UPDATE_IS_USED2, (TBool)satellite.IsUsed());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2, satellite.SignalStrength());

	TPosition position;
	notifyPosInfo.GetPosition(position);
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY, position.VerticalAccuracy());

	CleanupStack::PopAndDestroy(2, &server);
	}


TEST(RPositioner, NotifyPositionUpdate_CustomPosInfo, "PNPU", 2)
	{
	// Check with multiple TPositionInfo-derived classes, including ones
	// created specifically to test the client API.
	// Test TExtPosInfo, a derivative of TPositionInfoBase, created only for
	// this test.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TExtPosInfo notifyPosInfo;
	TRequestStatus reqStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int());

	notifyPosInfo.iGalaxy = DUMMY_NOTIFY_POS_UPDATE_EXT_GALAXY;
	notifyPosInfo.iSolarSystem = DUMMY_NOTIFY_POS_UPDATE_EXT_SOLARSYS;
	notifyPosInfo.iStarDate = DUMMY_NOTIFY_POS_UPDATE_EXT_STARDATE;

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, NotifyPositionUpdate_Requestors, "PNPU", 3)
	{
	// Check this using both "authorised" and an "unauthorised" requestor.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	TPositionInfo posInfo;
	TRequestStatus reqStatus;

	// Try an authorised requestor
	User::LeaveIfError(SetAuthenticRequestor(positioner));
	positioner.NotifyPositionUpdate(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int() );

	// Try an un-authorised requestor
	positioner.SetRequestor(
		DUMMY_REQUESTOR_TYPE_REJECTED1,
		DUMMY_REQUESTOR_FORMAT_REJECTED1,
		DUMMY_REQUESTOR_NAME_REJECTED1);
	positioner.NotifyPositionUpdate(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrAccessDenied, reqStatus.Int() );

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, NotifyPositionUpdate_NoRequestorSet, "PNPU", 4)
	{
	// Check this using both "authorised" and an "unauthorised" requestor.
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));

	TPositionInfo posInfo;
	TRequestStatus reqStatus;

	positioner.NotifyPositionUpdate(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrAccessDenied, reqStatus.Int() );

	CleanupStack::PopAndDestroy(2, &server);
	}


TEST(RPositioner, NotifyPositionUpdate, "PNPU", 6)
	{
	// Check that the notification is produced for every change that
	// should prompt a notification.
	// NOTE: this is a duplicate of PNPU-1
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo notifyPosInfo;
	TRequestStatus reqStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrNone, reqStatus.Int());

	TPosition position;
	notifyPosInfo.GetPosition(position);

	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LATITUDE, position.Latitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_LONGITUDE, position.Longitude());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, position.HorizontalAccuracy());
	CHECK_EQUAL( DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY, position.VerticalAccuracy());

	CleanupStack::PopAndDestroy(2, &server);
	}

TEST(RPositioner, NotifyPositionUpdateCancel, "PNPU", 7)
	{
	// Test TPositionInfo, a derivative of TPositionInfoBase
	RPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RPositioner positioner;
	CleanupClosePushL(positioner);
	User::LeaveIfError(positioner.Open(server));
	User::LeaveIfError(SetAuthenticRequestor(positioner));

	TPositionInfo notifyPosInfo;
	TRequestStatus reqStatus;
	positioner.NotifyPositionUpdate(notifyPosInfo, reqStatus);
	positioner.CancelRequest(EPositionerNotifyPositionUpdate);
	User::WaitForRequest(reqStatus);
	CHECK_EQUAL( KErrCancel, reqStatus.Int());

	CleanupStack::PopAndDestroy(2, &server);
	}

//
// Utility functions
//

// NOTE: the CHECK macros cannot be used from these functions... //

void CleanupResetAndDestroy(TAny* aArray)
	{
	reinterpret_cast<RRequestorStack*>(aArray)->ResetAndDestroy();
	}

TInt SetAuthenticRequestor(RPositioner& aPositioner)
	{
	return aPositioner.SetRequestor(DUMMY_REQUESTOR_TYPE_ACCEPTED1, DUMMY_REQUESTOR_FORMAT_ACCEPTED1, DUMMY_REQUESTOR_NAME_ACCEPTED1);
	}

//
// End of file
