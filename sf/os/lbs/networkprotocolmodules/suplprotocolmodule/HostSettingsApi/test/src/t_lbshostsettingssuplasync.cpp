// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Asynchronous unit tests for LBS SUPL API to check functionality of the
// settings store observer.
// 
//

#include <test/tefunit.h>
#include <test/ctefunitserver.h>
#include <lbs/lbshostsettings.h>
#include <centralrepository.h>
#include <lbs/lbshostsettingsclasstypes.h>

#include "t_lbshostsettingssuplutils.h"
#include "t_activewaiter.h"

// Name of observer test application for test 2
_LIT(KObserverApp, "t_lbshostsettingssuplobs");

class CTestAsyncSuplLbsApi;

class CTestLbsHostSettingsStoreObserver : public CBase, public MLbsHostSettingsStoreObserver
	{
public:
	friend class CTestAsyncSuplLbsApi;
	
	IMPORT_C static CTestLbsHostSettingsStoreObserver* NewL(CTestAsyncSuplLbsApi* aTester);
	IMPORT_C virtual void LbsHostSettingsUpdated(TInt aError, TLbsHostStoreId aStoreId, TLbsHostSettingsId aSettingsId) const;
	~CTestLbsHostSettingsStoreObserver();
	
private:
	CTestLbsHostSettingsStoreObserver(CTestAsyncSuplLbsApi* aTester);

private:
	CTestAsyncSuplLbsApi* iTester;
	};

class CTestAsyncSuplLbsApi : public CActiveTestFixture
	{
public:
	friend class CTestLbsHostSettingsStoreObserver;

	// SetUp and TearDown code
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestObserverLocalL();
	void TestObserverMultiAppL();
	
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	CLbsHostSettingsStore *iLBSHostStore1;
	CLbsHostSettingsStore *iLBSHostStore2;
	CTestLbsHostSettingsStoreObserver* iObserver1;
	CTestLbsHostSettingsStoreObserver* iObserver2;
	CActiveWaiter* iWaiter;
	TInt iObservedError;
	TLbsHostStoreId iObservedStoreId;
	TLbsHostSettingsId iObservedSettingsId;
	TBool iObserver1Notified;
	};
	
const TVersion KObserverVersion = TVersion(0, 0, 0);
//-------------------------------------------------------------------------------------------------

_LIT( KServerName, "t_lbshostsettingssuplasync" );

GLDEF_C const TTestName ServerName()
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE
	ADD_TEST_SUITE( CTestAsyncSuplLbsApi )
	END_SUITE
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	CTestStep* testStep = NULL;
	return testStep;
	}

CTestSuite* CTestAsyncSuplLbsApi::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	_LIT( KTestObserverLocalL, "TestObserverLocalL");
	AddTestCase(lTestSuite, KTestObserverLocalL, &CTestAsyncSuplLbsApi::TestObserverLocalL);
	        
	_LIT( KTestObserverMultiAppL, "TestObserverMultiAppL");
	AddTestCase(lTestSuite, KTestObserverMultiAppL, &CTestAsyncSuplLbsApi::TestObserverMultiAppL);

    END_SUITE;	
	}

/**
Method called just before a TEF Unit Test is executed.
@see	CActiveTestFixture::SetupL
*/
void CTestAsyncSuplLbsApi::SetupL()
	{
 	//Wipe the CenRep
	CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(rep);
	User::LeaveIfError(rep->Reset());
	CleanupStack::PopAndDestroy(rep);
 	}

/**
Method called after a test is executed. 
@see	CActiveTestFixture::TearDownL
*/
void CTestAsyncSuplLbsApi::TearDownL()
	{
	delete iLBSHostStore1;
	iLBSHostStore1 = NULL;
	delete iLBSHostStore2;
	iLBSHostStore2 = NULL;
	delete iObserver1;
	iObserver1 = NULL;
	delete iObserver2;
	iObserver2 = NULL;
	delete iWaiter;
	iWaiter = NULL;
	}

//-------------------------------------------------------------------------------------------------

void CTestAsyncSuplLbsApi::TestObserverLocalL()
//
// Test the use of an MLbsHostSettingsStoreObserver implementation within
// the process that is modifying the settings store.
//
// This test creates two instances of CLbsHostSettingsStore and an observer
// instance for each one. Only the first store instance is used for settings
// modifications. The API specifies that an application will not receive
// notifications for changes it itself has made via the same instance of
// CLbsHostSettingsStore, so only observer 2 should receive notifications.
//
// 1. Create two observers
// 2. Create two store instances and start notifiers
// 3. Create waiter
// 4. Read the initial host settings in from the ini file
// 5. Create a host settings entry using the initial settings
// 6. Retrieve & validate
// 7. Wait for notifier to call LbsHostSettingsUpdated()
// 8. Get new host settings
// 9. Update the previously added entry
// 10. Retrieve and validate
// 11. Wait for notifier to call LbsHostSettingsUpdated()
// 12. Create another settings entry
// 13. Wait for notifier to call LbsHostSettingsUpdated()
// 14. Set second entry as default
// 15. Update second entry
// 16. Wait for notifier to call LbsHostSettingsUpdated()
// 17. Delete first entry
// 18. Wait for notifier to call LbsHostSettingsUpdated()
// 19. Try to update a non-existent entry
// 20. Update a valid entry
// 21. Wait for notifier to call LbsHostSettingsUpdated()
// 22. Try to delete a non-existent entry
// 23. Delete a valid entry
// 24. Wait for notifier to call LbsHostSettingsUpdated()
// 25. Retrieve the extended interface
// 26. Check the version number
//
// Note that for a given observer, the calls to WaitActive() catch only the
// *first* notification since the previous call to that function. Any others
// will be lost since the CenRep notification request is not renewed until
// CLbsHostSettingsNotifier::RunL() gets invoked. This should not matter since
// we expect only one notification per modified settings entry, but it doesn't
// prove that we aren't getting multiple notifications. (TestObserverMultiAppL()
// does not suffer from this limitation.)
//
	{
	INFO_PRINTF1(_L("CTestAsyncSuplLbsApi::TestObserverLocalL"));
	
	// Create active scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler); 
	CActiveScheduler::Install(scheduler);

	// 1. Create two observers
	iObserver1 = CTestLbsHostSettingsStoreObserver::NewL(this);
	iObserver2 = CTestLbsHostSettingsStoreObserver::NewL(this);

	// 2. Create two store instances with the corresponding observers,
	// and start notifiers
	iLBSHostStore1 = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId, *iObserver1);
	iLBSHostStore2 = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId, *iObserver2);

	// 3. Create waiter. Note: waiter must be created after notifiers are
	// started otherwise a panic E32USER-CBase 46 (stray signal) results.
    iWaiter = new(ELeave) CActiveWaiter;

	// 4. Read the initial host settings in from the ini file
	TLbsHostSettingsSupl settings;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, settings, _L("HostSetting_Default")));
			
	// 5. Create a host settings entry using the initial settings
	TLbsHostSettingsId newSettingId1;
	TInt err = iLBSHostStore1->CreateHostSettings(settings, KLbsHostSettingsTestCreatorId, newSettingId1);	
	ASSERT_EQUALS(KErrNone, err);
	
	// 6. Retrieve & validate
	TLbsHostSettingsSupl settingResult;
	err = iLBSHostStore1->GetHostSettings(newSettingId1, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(settings, settingResult));

	// 7. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId1);
	ASSERT_FALSE(iObserver1Notified);

	// 8. Get new host settings
	TLbsHostSettingsSupl settings2;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, settings2, _L("HostSetting_Two")));
	
	// 9. Update the previously added entry
	err = iLBSHostStore1->UpdateHostSettings(newSettingId1, settings2);	
	ASSERT_EQUALS(err, KErrNone);
	
	// 10. Retrieve and validate
	err = iLBSHostStore1->GetHostSettings(newSettingId1, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(settings2, settingResult));

	// 11. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId1);
	ASSERT_FALSE(iObserver1Notified);

	// 12. Create another settings entry
	TLbsHostSettingsId newSettingId2;
	err = iLBSHostStore1->CreateHostSettings(settings, KLbsHostSettingsTestCreatorId, newSettingId2);	
	ASSERT_EQUALS(KErrNone, err);

	// 13. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId2);
	ASSERT_FALSE(iObserver1Notified);

	// 14. Set second entry as default. This should not result in any
	// notification since the default host key is part of the metadata space.
	// (If it did then the test under step 16 below would fail since
	// LbsHostSettingsUpdated() would be called with settings
	// ID = 2 [EDefaultHost] but newSettingsId2 = 1.)
	err = iLBSHostStore1->SetDefaultHostSettings(newSettingId2);	
	ASSERT_EQUALS(err, KErrNone);

	// 15. Update second entry
	err = iLBSHostStore1->UpdateHostSettings(newSettingId2, settings2);	
	ASSERT_EQUALS(err, KErrNone);

	// 16. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId2);
	ASSERT_FALSE(iObserver1Notified);

	// 17. Delete first entry
	err = iLBSHostStore1->DeleteHostSettings(newSettingId1);
	ASSERT_EQUALS(err, KErrNone);

	// 18. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId1);
	ASSERT_FALSE(iObserver1Notified);

	// 19. Try to update a non-existent entry. This shouldn't result in any
	// notification. (If it did then the test under step 21 below would
	// fail since LbsHostSettingsUpdated() would be called with settings
	// ID = newSettingsId1.)
	err = iLBSHostStore1->UpdateHostSettings(newSettingId1, settings);	
	ASSERT_EQUALS(err, KErrNotFound);

	// 20. Update a valid entry
	err = iLBSHostStore1->UpdateHostSettings(newSettingId2, settings);	
	ASSERT_EQUALS(err, KErrNone);

	// 21. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId2);
	ASSERT_FALSE(iObserver1Notified);

	// 22. Try to delete a non-existent entry. This shouldn't result in any
	// notification. (If it did then the test under step 24 below would
	// fail since LbsHostSettingsUpdated() would be called with settings
	// ID = newSettingsId1.)
	err = iLBSHostStore1->DeleteHostSettings(newSettingId1);	
	ASSERT_EQUALS(err, KErrNotFound);

	// 23. Delete a valid entry
	err = iLBSHostStore1->DeleteHostSettings(newSettingId2);
	ASSERT_EQUALS(err, KErrNone);

	// 24. Wait for notifier to call LbsHostSettingsUpdated()
    iWaiter->WaitActive();
	ASSERT_EQUALS(iObservedError, KErrNone);
	ASSERT_EQUALS(iObservedStoreId, KLbsHostSettingsSuplStoreId);
	ASSERT_EQUALS(iObservedSettingsId, newSettingId2);
	ASSERT_FALSE(iObserver1Notified);

	// 25. Retrieve the extended interface
	TAny* ei = iObserver1->ExtendedInterface(0, NULL, NULL);
	ASSERT_TRUE(ei == NULL);
	
	// 26. Check the version number
	TVersion ver = iObserver1->Version();
	ASSERT_TRUE(
			(ver.iMajor == KObserverVersion.iMajor) &&
			(ver.iMinor == KObserverVersion.iMinor) &&
			(ver.iBuild == KObserverVersion.iBuild)
	);
	CleanupStack::PopAndDestroy(scheduler);	
	}

void CTestAsyncSuplLbsApi::TestObserverMultiAppL()
//
// Test the use of MLbsHostSettingsStoreObserver implementations with
// multiple applications/processes.
//
// This test spawns two instances of a separate observer application, each
// of which requests notification of changes to the settings store that
// is modified here. When the notifier calls LbsHostSettingsUpdated() in
// either of the observer apps, they will attempt to rendezvous with this
// process and pass back the notification details for verification.
//
// 1. Spawn observer applications
// 2. Make rendezvous requests to each observer app
// 3. Create store
// 4. Read the initial host settings in from the ini file
// 5. Create a host settings entry using the initial settings
// 6. Retrieve & validate
// 7. Wait for observer apps to rendezvous
// 8. Get new host settings
// 9. Update the previously added entry
// 10. Retrieve and validate
// 11. Wait for observer apps to rendezvous
// 12. Create another settings entry
// 13. Wait for observer apps to rendezvous
// 14. Set second entry as default
// 15. Update second entry
// 16. Wait for observer apps to rendezvous
// 17. Delete first entry
// 18. Wait for observer apps to rendezvous
// 19. Try to update a non-existent entry
// 20. Update a valid entry
// 21. Wait for observer apps to rendezvous
// 22. Try to delete a non-existent entry
// 23. Delete a valid entry
// 24. Wait for observer apps to rendezvous
// 25. Kill observer apps
//
	{
	INFO_PRINTF1(_L("CTestAsyncSuplLbsApi::TestObserverMultiAppL"));

	// 1. Spawn observer applications
	RProcess app1, app2;
	TInt error = app1.Create(KObserverApp, KNullDesC);
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Cannot create observer app 1 - error = %d"), error);
		User::Leave(error);
		}

	CleanupClosePushL(app1);

	error = app2.Create(KObserverApp, KNullDesC);
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Cannot create observer app 2 - error = %d"), error);
		User::Leave(error);
		}

	CleanupClosePushL(app2);

	// 2. Make rendezvous requests to each observer app
	TRequestStatus status1, status2;
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);
	app1.Resume();
	app2.Resume();

	// 3. Create store
	iLBSHostStore1 = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);

	// 4. Read the initial host settings in from the ini file
	TLbsHostSettingsSupl settings;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, settings, _L("HostSetting_Default")));
			
	// 5. Create a host settings entry using the initial settings
	TLbsHostSettingsId newSettingId1;
	TInt err = iLBSHostStore1->CreateHostSettings(settings, KLbsHostSettingsTestCreatorId, newSettingId1);	
	ASSERT_EQUALS(KErrNone, err);
	
	// 6. Retrieve & validate
	TLbsHostSettingsSupl settingResult;
	err = iLBSHostStore1->GetHostSettings(newSettingId1, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(settings, settingResult));

	// 7. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId1.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId1.iUid));
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);

	// 8. Get new host settings
	TLbsHostSettingsSupl settings2;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, settings2, _L("HostSetting_Two")));
	
	// 9. Update the previously added entry
	err = iLBSHostStore1->UpdateHostSettings(newSettingId1, settings2);	
	ASSERT_EQUALS(err, KErrNone);
	
	// 10. Retrieve and validate
	err = iLBSHostStore1->GetHostSettings(newSettingId1, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(settings2, settingResult));

	// 11. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId1.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId1.iUid));
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);

	// 12. Create another settings entry
	TLbsHostSettingsId newSettingId2;
	err = iLBSHostStore1->CreateHostSettings(settings, KLbsHostSettingsTestCreatorId, newSettingId2);	
	ASSERT_EQUALS(KErrNone, err);

	// 13. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId2.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId2.iUid));
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);

	// 14. Set second entry as default. This should not result in any
	// notification since the default host key is part of the metadata space.
	// (If it did then the test under step 16 below would fail since
	// LbsHostSettingsUpdated() would be called with settings
	// ID = 2 [EDefaultHost] but newSettingsId2 = 1.)
	err = iLBSHostStore1->SetDefaultHostSettings(newSettingId2);	
	ASSERT_EQUALS(err, KErrNone);

	// 15. Update second entry
	err = iLBSHostStore1->UpdateHostSettings(newSettingId2, settings2);	
	ASSERT_EQUALS(err, KErrNone);

	// 16. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId2.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId2.iUid));
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);

	// 17. Delete first entry
	err = iLBSHostStore1->DeleteHostSettings(newSettingId1);
	ASSERT_EQUALS(err, KErrNone);

	// 18. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId1.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId1.iUid));
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);

	// 19. Try to update a non-existent entry. This shouldn't result in any
	// notification. (If it did then the test under step 21 below would
	// fail since LbsHostSettingsUpdated() would be called with settings
	// ID = newSettingsId1.)
	err = iLBSHostStore1->UpdateHostSettings(newSettingId1, settings);	
	ASSERT_EQUALS(err, KErrNotFound);

	// 20. Update a valid entry
	err = iLBSHostStore1->UpdateHostSettings(newSettingId2, settings);	
	ASSERT_EQUALS(err, KErrNone);

	// 21. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId2.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId2.iUid));
	app1.Rendezvous(status1);
	app2.Rendezvous(status2);

	// 22. Try to delete a non-existent entry. This shouldn't result in any
	// notification. (If it did then the test under step 24 below would
	// fail since LbsHostSettingsUpdated() would be called with settings
	// ID = newSettingsId1.)
	err = iLBSHostStore1->DeleteHostSettings(newSettingId1);	
	ASSERT_EQUALS(err, KErrNotFound);

	// 23. Delete a valid entry
	err = iLBSHostStore1->DeleteHostSettings(newSettingId2);
	ASSERT_EQUALS(err, KErrNone);

	// 24. Wait for observer apps to rendezvous
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	ASSERT_EQUALS(status1.Int(), static_cast<TInt>(newSettingId2.iUid));
	ASSERT_EQUALS(status2.Int(), static_cast<TInt>(newSettingId2.iUid));

	// 25. Kill observer apps
	app1.Kill(KErrNone);
	app2.Kill(KErrNone);

	CleanupStack::PopAndDestroy(&app2);	
	CleanupStack::PopAndDestroy(&app1);	
	}

EXPORT_C CTestLbsHostSettingsStoreObserver* CTestLbsHostSettingsStoreObserver::NewL(CTestAsyncSuplLbsApi* aTester)
	{
	CTestLbsHostSettingsStoreObserver* self = new(ELeave) CTestLbsHostSettingsStoreObserver(aTester);
	return self;
	}

EXPORT_C void CTestLbsHostSettingsStoreObserver::LbsHostSettingsUpdated(TInt aError, TLbsHostStoreId aStoreId, TLbsHostSettingsId aSettingsId) const
	{
	if (this == iTester->iObserver2)
		{
		iTester->iObservedError = aError;
		iTester->iObservedStoreId = aStoreId;
		iTester->iObservedSettingsId = aSettingsId;

		TRequestStatus* status = &iTester->iWaiter->iStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		// Observer 1 should not receive any change notifications since the
		// corresponding instance of CLbsHostSettingsStore is making the
		// changes. Ensure that TestObserverLocalL() will raise an error if it
		// does.
		// Note that User::RequestComplete() is not called here since it would
		// cause a stray signal. CLbsHostSettingsNotifier::RunL() will always
		// be run for each of the two observers before WaitActive() returns.
		iTester->iObserver1Notified = TRUE;
		}
	}

CTestLbsHostSettingsStoreObserver::CTestLbsHostSettingsStoreObserver(CTestAsyncSuplLbsApi* aTester) : iTester(aTester)
	{
	}

CTestLbsHostSettingsStoreObserver::~CTestLbsHostSettingsStoreObserver()
	{
	}

