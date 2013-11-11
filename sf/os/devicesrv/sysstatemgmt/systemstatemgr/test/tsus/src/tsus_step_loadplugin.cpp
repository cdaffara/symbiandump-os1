// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <ssm/ssmsuscli.h>
#include <e32property.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmutility.h>
#include <ssm/ssmsystemwideproperty.h>
#include "ssmdebug.h"
#include "susutilserver.h"
#include "susutilclisrv.h"

#include "tsus_step_loadplugin.h"
#include "tsus_startserver.h"
#include "tsus_ssmsuscli.h"

#include "tsus_swpchangesup.h"


const TUid KSsmSid = {0x2000D75B};
const TUid KPropertyCategory = KSsmSid;

const TUid KIdentityChangeSup = {0x10285A53};
_LIT(KFileNameTestPlugin,"tsus_swpchangesup.dll");



CSusLoadPluginTest::~CSusLoadPluginTest()
	{
	}

CSusLoadPluginTest::CSusLoadPluginTest()
	{
	SetTestStepName(KTSusLoadPluginStep);
	}

TVerdict CSusLoadPluginTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSusLoadPluginTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


/** 
*/
void CSusLoadPluginTest::UnloadPlugins(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Unload the plugins to ensure no memory is left uncleaned on server."));
	TSsmSupInfo info(KFileNameTestPlugin,1, KIdentityChangeSup);
	TInt result = aSession.RequestUnLoadSup(info);
	TESTE(KErrNone==result, result);
	}

/** 
*/
void CSusLoadPluginTest::TestLoadSupL(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Load a valid and working SUP. Ensure that its RunL() gets called."));

	RSsmSystemWideProperty swpNotification;
	TInt err = swpNotification.Connect(KTestSwpLoadSup);
	CleanupClosePushL(swpNotification);
	TEST(err == KErrNone);

	RSemaphore sem;
	sem.CreateGlobal(KTestSwpLoadSupSemaphore, 0);
	CleanupClosePushL(sem);

	//Define a property to a known initial value
	const TInt KSwpInitialValue = -35;
	TSsmSwp swp(KTestSwpLoadSup, KSwpInitialValue);
	RProperty property;
	INFO_PRINTF1(_L("Setting and set initial value for a SWP"));
	err = property.Define(KPropertyCategory, swp.Key(), RProperty::EInt);
	TEST(err == KErrNone);
	err = property.Set(KPropertyCategory, swp.Key(), swp.Value());
	TEST(err == KErrNone);

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(KSwpInitialValue == value, value);
	INFO_PRINTF3(_L("SWP key initial value set to '%d', expected was '%d'"), value, KSwpInitialValue);
	
	//Issue a LoadSup request
	INFO_PRINTF1(_L("Issue a LoadSup request that will request a swp change "));
	TRequestStatus status = KRequestPending;
	TSsmSupInfo info(KFileNameTestPlugin, 1, KIdentityChangeSup);
	TPckgC<TSsmSupInfo>* infoBuf = new (ELeave) TPckgC<TSsmSupInfo>(info);
	CleanupStack::PushL(infoBuf);
	aSession.RequestLoadSup(*infoBuf, status);

	//Wait until SwploadSup signal our semaphore from its RunL (or give up after 5 seconds)
	const TInt timeout = 5000000;
	const TInt result = sem.Wait(timeout);
	TESTE(KErrNone == result, result); // a KTimeout would cause the result to be KErrTimedOut
	
	// Wait for status to be completed
	User::WaitForRequest(status);
	
	//The request to load the sup should already have been completed before the semaphore was signaled
	TESTE(status==KErrNone, status.Int());
	CleanupStack::PopAndDestroy(infoBuf);
	CleanupStack::PopAndDestroy(&sem);

	TRequestStatus status_notif;
	swpNotification.Subscribe(status_notif);
	TEST(status_notif == KRequestPending);
	// Waiting till the swp value is published
	User::WaitForRequest(status_notif);

	INFO_PRINTF2(_L("Subscribe for the swp change notification Completed with %d. "), status_notif.Int());
	TEST(status_notif.Int() == KErrNone);

	swpNotification.GetValue(value);
	INFO_PRINTF3(_L("swp Actual Value %d: Expected Value %d"), value, KTestSwpLoadSupNewValue);
	TEST(value == KTestSwpLoadSupNewValue);

	CleanupStack::PopAndDestroy(&swpNotification);
	//Check swp has a new value
	value = 0;
	err = property.Get(KPropertyCategory, swp.Key(), value);
	TEST(err == KErrNone);
	TEST(KTestSwpLoadSupNewValue == value);
	INFO_PRINTF3(_L("SWP key value has changed to '%d', expected was '%d'."), value, KTestSwpLoadSupNewValue);
	
	// Check that the same SUP can be loaded again from different ordinal
	INFO_PRINTF1(_L("Testing multiple SUP loading"));
	TSsmSupInfo info2(KFileNameTestPlugin, 2);
	TPckgC<TSsmSupInfo> info2Pckg(info2);
	TRequestStatus status2;
	aSession.RequestLoadSup(info2Pckg, status2);
	User::WaitForRequest(status2);
	TEST(status2 == KErrNone);
	
	// Unload the multiple loaded SUP
	INFO_PRINTF1(_L("Unloading multiple SUP loaded"));
	err = aSession.RequestUnLoadSup(info2);
	TEST(err == KErrNone);
	}


/** 
 Old Test CaseID 		APPFWK-SUS-0010
 New Test CaseID 		DEVSRVS-SSMA-SUS-0010
 */

TVerdict CSusLoadPluginTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSusLoadPluginTest tests started...."));

	__UHEAP_MARK;

	INFO_PRINTF1(_L("Creating a test clone for UtilSrv..."));
	RProcess susProcess;
	const TUint32 sid = susProcess.SecureId();
	RThread susThread;
	TESTL(KErrNone == StartServer(susThread, sid));

	//Connect the RSession
	INFO_PRINTF1(_L("Connect the RSsmSusCliTest session"));
	RSsmSusCliTest session;
	TInt ret = session.Connect(KTestServerName);
	TESTE(KErrNone == ret, ret);

	// perform the actual test
	TRAP(ret, TestLoadSupL(session));
	INFO_PRINTF2(_L("TestLoadSupL completed with error '%d'."), ret);

	TEST(ret == KErrNone);	
	
	// Unload the plugins to ensure no memory is left uncleaned on server.
	UnloadPlugins(session);

	//Cleanup
	session.Close();
	susThread.Kill(KErrNone);
	susThread.Close();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSusLoadPluginTest tests completed!"));
	return TestStepResult();
	}

