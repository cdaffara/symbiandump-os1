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
//

#include <test/tefunit.h>
#include <test/ctefunitserver.h>
#include <lbs/lbshostsettings.h>
#include <centralrepository.h>
#include <lbs/lbshostsettingsclasstypes.h>

#include "lbshostsettingsconsts.h"

#include "t_lbshostsettingssuplutils.h"

class CTestSuplLbsApi : public CTestFixture
	{
public:
	// SetUp and TearDown code
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestCreateAndDefaultL();
	void TestGetHostSettingsL();
	void TestUpdateHostSettingsL();
	void TestRewindGetNextByCreatorL();
	void TestDeleteHostSettingsL();
	void TestGetNextHostSettingL();
	void TestTimestampsL();
	void TestPortNumberNotSetL();
	
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	CLbsHostSettingsStore *iLBSHostStore;
	};
	
const TUid devProvHostSettingsUid = TUid::Uid(0x10285AA8);
const TUid KUidInvalidKey = {0x0FFFFFFF};

const TUint32 KRootKeyMask = 0xFF000000;	// don't care what the last 6 digits are

//-------------------------------------------------------------------------------------------------

_LIT( KServerName, "t_lbshostsettingssupl" );

GLDEF_C const TTestName ServerName()
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE
	ADD_TEST_SUITE( CTestSuplLbsApi )
	END_SUITE
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	CTestStep* testStep = NULL;
	return testStep;
	}

CTestSuite* CTestSuplLbsApi::CreateSuiteL(const TDesC& aName)
	{
    SUB_SUITE;
    
    _LIT( KTestCreateAndDefaultL, "TestCreateAndDefaultL");
    AddTestCase(lTestSuite, KTestCreateAndDefaultL, &CTestSuplLbsApi::TestCreateAndDefaultL);
        
    _LIT( KTestGetHostSettingsL, "TestGetHostSettingsL");
    AddTestCase(lTestSuite, KTestGetHostSettingsL, &CTestSuplLbsApi::TestGetHostSettingsL);
            
    _LIT( KTestUpdateHostSettingsL, "TestUpdateHostSettingsL");
    AddTestCase(lTestSuite, KTestUpdateHostSettingsL, &CTestSuplLbsApi::TestUpdateHostSettingsL);
            
    _LIT( KTestRewindGetNextByCreatorL, "TestRewindGetNextByCreatorL");
    AddTestCase(lTestSuite, KTestRewindGetNextByCreatorL, &CTestSuplLbsApi::TestRewindGetNextByCreatorL);
            
    _LIT( KTestDeleteHostSettingsL, "TestDeleteHostSettingsL");
    AddTestCase(lTestSuite, KTestDeleteHostSettingsL, &CTestSuplLbsApi::TestDeleteHostSettingsL);
            
    _LIT( KTestGetNextHostSettingL, "TestGetNextHostSettingL");
    AddTestCase(lTestSuite, KTestGetNextHostSettingL, &CTestSuplLbsApi::TestGetNextHostSettingL);
            
    _LIT( KTestTimestampsL, "TestTimestampsL");
    AddTestCase(lTestSuite, KTestTimestampsL, &CTestSuplLbsApi::TestTimestampsL);
            
    _LIT( KTestPortNumberNotSetL, "TestPortNumberNotSetL");
    AddTestCase(lTestSuite, KTestPortNumberNotSetL, &CTestSuplLbsApi::TestPortNumberNotSetL);
    
    END_SUITE;	
	}

/**
Method called just before a TEF Unit Test is executed.
@see	CTestFixture::SetupL
*/
void CTestSuplLbsApi::SetupL()
	{
 	//Wipe the CenRep
	CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(rep);
	User::LeaveIfError(rep->Reset());
	// delete everything
	TUint32 errorKey = 0;
	rep->Delete((ECreatorId << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EConnectionId << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EConnectionIdType << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EHostAddress << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EReadOnly << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EHostPort << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EHostName << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EProviderId << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((ETimestamp << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EAuthMoLr << KRecordTypeShift), KRootKeyMask, errorKey);
	rep->Delete((EAuthMtLr << KRecordTypeShift), KRootKeyMask, errorKey);
	CleanupStack::PopAndDestroy(rep);
	//create & wipe the store
 	iLBSHostStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
 	}

/**
Method called after a test is executed. 
@see	CTestFixture::TearDownL
*/
void CTestSuplLbsApi::TearDownL()
	{
	//delete iLBSHostStore;
	iLBSHostStore = NULL;
	}

//-------------------------------------------------------------------------------------------------
void CTestSuplLbsApi::TestCreateAndDefaultL()
//
// Tests GetDefaultHostSettings/SetDefaultHostSettings, CreateHostSettings functionalities
// 1. Read the initial host setting in from the ini file
// 2. Attempt to get the default host settings - this should fail with KErrNotFound
// 3. Create a setting
// 4. Make the newly added account the default host settings
// 5. Extra added: retrieve the setting using the base class. This should fail with 
//    with KErrArgument 
// 6. Get the default host settings & verify that they are the same as the account we added

	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestCreateAndDefaultL"));
	ASSERT_NOT_NULL(iLBSHostStore);
	
	// 1. Read the initial host setting in from the ini file
	TLbsHostSettingsSupl defSetting;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, defSetting, _L("HostSetting_Default")));
		
	// 2. Get the default host settings - this should fail with KErrNotFound
	TLbsHostSettingsSupl settings;
	TLbsHostSettingsId settingsId;
	TInt err = iLBSHostStore->GetDefaultHostSettings(settings, settingsId);
	ASSERT_EQUALS(KErrNotFound, err);

	// 3. Preset the default host settings
	TLbsHostSettingsId newSettingId;
	err = iLBSHostStore->CreateHostSettings(defSetting, KLbsHostSettingsTestCreatorId, newSettingId);
	ASSERT_EQUALS(KErrNone, err);
	
	// Make it default
	err = iLBSHostStore->SetDefaultHostSettings(newSettingId);
	ASSERT_EQUALS(err, KErrNone);
		
	// 4. Get the default host settings.
	// First try using the base LBS host settings class- this should fail with KErrArgument
	TLbsHostSettingsBase settingsBase;
	err = iLBSHostStore->GetDefaultHostSettings(settingsBase, settingsId);
	ASSERT_EQUALS(KErrArgument, err);
	
	//Now try one that should succeed.
	err = iLBSHostStore->GetDefaultHostSettings(settings, settingsId);
	ASSERT_EQUALS(KErrNone, err);

	// 5. Validate against the ini file settings
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(defSetting, settings));
	
	// Validate the settingsId
	ASSERT_EQUALS(newSettingId, settingsId);
	}

void CTestSuplLbsApi::TestGetHostSettingsL()
//
// Tests GetHostSettings functionality
// 1. Create a new host setting using the incorrect LBS host settings class type- this should fail with KErrArgument
// 2. Create a new host setting
// 3. Retrieve a setting using the incorrect LBS host settings class type- this should fail with KErrArgument
// 4. Retrieve a setting using an invalid setting key- this should fail with KErrNotFound
// 5. Retrieve and validate said setting
 
	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestGetHostSettingsL"));
	ASSERT_NOT_NULL(iLBSHostStore);
	
	// 0. Get new host setting value from ini file
	TLbsHostSettingsSupl setting;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting, _L("HostSetting_One")));
	
	// 2. Create a new host setting
	TLbsHostSettingsId newSettingId;
	TLbsHostSettingsBase settingBase;
	TInt err = iLBSHostStore->CreateHostSettings(settingBase, KLbsHostSettingsTestCreatorId, newSettingId);
	ASSERT_EQUALS(err, KErrArgument);
	
	// 2. Create a new host setting
	err = iLBSHostStore->CreateHostSettings(setting, KLbsHostSettingsTestCreatorId, newSettingId);
	ASSERT_EQUALS(err, KErrNone);
	
	// 3. Retrieve a setting using the incorrect LBS host settings class type- this should fail with KErrArgument
	TLbsHostSettingsBase settingResultBase;
	err = iLBSHostStore->GetHostSettings(newSettingId, settingResultBase);	
	ASSERT_EQUALS(err, KErrArgument);
	
	// 4. Retrieve a setting using an invalid setting key- this should fail with KErrNotFound
	TLbsHostSettingsSupl settingResult;
	err = iLBSHostStore->GetHostSettings(KUidInvalidKey, settingResult);	
	ASSERT_EQUALS(err, KErrNotFound);
	
	// 5. Retrieve and validate said setting
	err = iLBSHostStore->GetHostSettings(newSettingId, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting, settingResult));
	}

void CTestSuplLbsApi::TestUpdateHostSettingsL()
//
// Tests UpdateHostSettings functionality
// 1. Create a new host setting
// 2. Verify that setting was well created
// 3. Update setting with config read from .ini file
// 4. Verify that setting was updated
// 
	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestUpdateHostSettingsL"));
	ASSERT_NOT_NULL(iLBSHostStore);
	
	// 1   Create a new host setting
	// 1.1 Get new host setting values from ini file
	TLbsHostSettingsSupl setting1;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting1, _L("HostSetting_Two")));
	
	// 1.2 Create it in the store
	TLbsHostSettingsId newSettingId;
	TInt err = iLBSHostStore->CreateHostSettings(setting1, KLbsHostSettingsTestCreatorId, newSettingId);
	ASSERT_EQUALS(err, KErrNone);
	
	// 1.3 Retrieve and validate new setting
	TLbsHostSettingsSupl settingResult;
	err = iLBSHostStore->GetHostSettings(newSettingId, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting1, settingResult));

	// 2   Update and validate setting
	// 2.1 Get setting from ini file
	TLbsHostSettingsSupl setting2;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting2, _L("HostSetting_Three")));
	
	//2.2 Update exisiting setting but use the base LBS host setting class type- should fail with KErrArgument
	TLbsHostSettingsBase settingBase;
	err = iLBSHostStore->UpdateHostSettings(newSettingId, settingBase);	
	ASSERT_EQUALS(err, KErrArgument);
	
	//2.3 Update exisiting setting but use an invalid setting key- should fail with KErrNotFound
	err = iLBSHostStore->UpdateHostSettings(KUidInvalidKey, setting2);	
	ASSERT_EQUALS(err, KErrNotFound);
	
	// 2.4 Update existing (new) setting
	err = iLBSHostStore->UpdateHostSettings(newSettingId, setting2);	
	ASSERT_EQUALS(err, KErrNone);
	
	// 2.5 Retrieve & validate
	err = iLBSHostStore->GetHostSettings(newSettingId, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting2, settingResult));
	}

void CTestSuplLbsApi::TestRewindGetNextByCreatorL()
//
// // Tests GetNextHostSettingsByCreator, RewindHostSettings functionalities
// 1. Add 3 settings, the second having a different CreatorId
// 2. Call RewindHostSettings to initiliaze setting snapshot
// 3. Call GetNextHostSettingsByCreator three times with the same CreatorId 
//    to retrieve settings. The thrid call should fail with KErrNotFound. 

	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestRewindGetNextByCreatorL"));
	ASSERT_NOT_NULL(iLBSHostStore);
	
    TLbsHostSettingsSupl setting_1, setting_2, setting_3;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting_1, _L("HostSetting_One")));
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting_2, _L("HostSetting_Two")));
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting_3, _L("HostSetting_Three")));
			
	TLbsHostSettingsId newSettingId_1, newSettingId_2, newSettingId_3;
	TInt err = KErrNone;
	const TLbsHostCreatorId KUidLbsCreatorId2 = {0x00000004};
	
	err = iLBSHostStore->CreateHostSettings(setting_1, KLbsHostSettingsTestCreatorId, newSettingId_1);
	ASSERT_EQUALS(err, KErrNone);
		
	err = iLBSHostStore->CreateHostSettings(setting_2, KUidLbsCreatorId2, newSettingId_2);
	ASSERT_EQUALS(err, KErrNone);
	
	err = iLBSHostStore->CreateHostSettings(setting_3, KLbsHostSettingsTestCreatorId, newSettingId_3);
	ASSERT_EQUALS(err, KErrNone);
	
	// Iterate over them and check results...
	err = iLBSHostStore->RewindHostSettings();
	ASSERT_EQUALS(err, KErrNone);
	
	// Retrieve settings using the incorrect LBS host settings class type- this should fail with KErrArgument
	TLbsHostSettingsBase settingResultBase;
	TLbsHostSettingsId settingResultBaseId;
	err = iLBSHostStore->GetNextHostSettingsByCreator(KLbsHostSettingsTestCreatorId, settingResultBase, settingResultBaseId);	
	ASSERT_EQUALS(err, KErrArgument);
	
	
	TLbsHostSettingsSupl got_setting_1, got_setting_2, got_setting_3;
	TLbsHostSettingsId gotSettingId_1, gotSettingId_2, gotSettingId_3;	
	
	err = iLBSHostStore->GetNextHostSettingsByCreator(KLbsHostSettingsTestCreatorId, got_setting_1, gotSettingId_1);
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting_1, got_setting_1));
	ASSERT_EQUALS(gotSettingId_1, newSettingId_1);
	
	err = iLBSHostStore->GetNextHostSettingsByCreator(KLbsHostSettingsTestCreatorId, got_setting_2, gotSettingId_2);
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting_3, got_setting_2));
	ASSERT_EQUALS(gotSettingId_2, newSettingId_3);
						
	err = iLBSHostStore->GetNextHostSettingsByCreator(KLbsHostSettingsTestCreatorId, got_setting_3, gotSettingId_3);
	ASSERT_EQUALS(err, KErrNotFound);
	
	}


void CTestSuplLbsApi::TestDeleteHostSettingsL()
//
// Tests DeleteHostSettings functionality
// 1. Create a new setting
// 2. Verify that setting has been well created
// 3. Delete setting just created
// 4. Fail to retrieve setting - since it's deleted
// 5. Fail to delete setting again - since it's already deleted
// 6. Attempt to update the deleted setting- should fail
// 
	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestDeleteHostSettingsL"));
	ASSERT_NOT_NULL(iLBSHostStore);
		
	// 1   Create a new host setting
	// 1.1 Get new host setting values from ini file
	TLbsHostSettingsSupl setting1;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting1, _L("HostSetting_Five")));
		
	// 1.2 Create it in the store
	TLbsHostSettingsId newSettingId;
	TInt err = iLBSHostStore->CreateHostSettings(setting1, KLbsHostSettingsTestCreatorId, newSettingId);
	ASSERT_EQUALS(err, KErrNone);
		
	// 2 Retrieve and validate new setting
	TLbsHostSettingsSupl settingResult;
	err = iLBSHostStore->GetHostSettings(newSettingId, settingResult);	
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting1, settingResult));
		
	// 3. Delete setting
	ASSERT_EQUALS(KErrNone, iLBSHostStore->DeleteHostSettings(newSettingId));
	
	// 4. Fail to retrieve setting - since it's deleted
	err = iLBSHostStore->GetHostSettings(newSettingId, settingResult);	
	ASSERT_EQUALS(err, KErrNotFound);
	
	// 5. Fail to delete setting again - since it's already deleted
	err = iLBSHostStore->DeleteHostSettings(newSettingId);
	ASSERT_EQUALS(err, KErrNotFound);
	
	//6. Attempt to update the deleted setting- should fail
	err = iLBSHostStore->UpdateHostSettings(newSettingId, setting1);
	ASSERT_EQUALS(err, KErrNotFound);
	}

void CTestSuplLbsApi::TestGetNextHostSettingL()
//
// Tests GetNextHostSettings functionality 
// 1. Create three settings
// 2. Call RewindHostSettings to initiliaze setting snapshot
// 3. Call GetNextHostSettings four times to retrieve settings 
//    The fourth call should fail with KErrNotFound 
//
	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestGetNextHostSettingL"));
	
	TLbsHostSettingsSupl setting_1, setting_2, setting_3;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting_1, _L("HostSetting_One")));
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting_2, _L("HostSetting_Two")));
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting_3, _L("HostSetting_Three")));
			
	TLbsHostSettingsId newSettingId_1, newSettingId_2, newSettingId_3;
	TInt err = KErrNone;
	
	err = iLBSHostStore->CreateHostSettings(setting_1, KLbsHostSettingsTestCreatorId, newSettingId_1);
	ASSERT_EQUALS(err, KErrNone);
		
	err = iLBSHostStore->CreateHostSettings(setting_2, KLbsHostSettingsTestCreatorId, newSettingId_2);
	ASSERT_EQUALS(err, KErrNone);
	
	err = iLBSHostStore->CreateHostSettings(setting_3, KLbsHostSettingsTestCreatorId, newSettingId_3);
	ASSERT_EQUALS(err, KErrNone);
	
	//Try retrieving settings without calling RewindHostSettings first- should fail
	TLbsHostSettingsSupl settingResult;
	TLbsHostSettingsId settingResultId;
	err = iLBSHostStore->GetNextHostSettings(settingResult, settingResultId);	
	ASSERT_EQUALS(err, KErrNotFound);
		
	// Iterate over them and check results...
	err = iLBSHostStore->RewindHostSettings();
	ASSERT_EQUALS(err, KErrNone);
	
	// Retrieve settings using the incorrect LBS host settings class type- this should fail with KErrArgument
	TLbsHostSettingsBase settingResultBase;
	TLbsHostSettingsId settingResultBaseId;
	err = iLBSHostStore->GetNextHostSettings(settingResultBase, settingResultBaseId);	
	ASSERT_EQUALS(err, KErrArgument);
	
	
	TLbsHostSettingsSupl got_setting_1, got_setting_2, got_setting_3;
	TLbsHostSettingsId gotSettingId_1, gotSettingId_2, gotSettingId_3;
		
	err = iLBSHostStore->GetNextHostSettings(got_setting_1, gotSettingId_1);
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting_1, got_setting_1));
	ASSERT_EQUALS(gotSettingId_1, newSettingId_1);
	
	err = iLBSHostStore->GetNextHostSettings(got_setting_2, gotSettingId_2);
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting_2, got_setting_2));
	ASSERT_EQUALS(gotSettingId_2, newSettingId_2);
				
	err = iLBSHostStore->GetNextHostSettings(got_setting_3, gotSettingId_3);
	ASSERT_EQUALS(err, KErrNone);
	ASSERT_TRUE(CTestSuplUtils::AreSettingsEqual(setting_3, got_setting_3));
	ASSERT_EQUALS(gotSettingId_3, newSettingId_3);
	
	// re-using got_setting_3
	// should return KErrNotFound 

	err = iLBSHostStore->GetNextHostSettings(got_setting_3, gotSettingId_3);
	ASSERT_EQUALS(err, KErrNotFound);
	
	}

void CTestSuplLbsApi::TestTimestampsL()
//
// Test the use of timestamps
// 1. Create a new host settings
// 2. Retrieve the last modified timestamp 
// 3. Update the previously added host settings
// 4. Retrieve the last modified timestamp and check that it is greater than the last timestamp
	{
	INFO_PRINTF1(_L("CTestSuplLbsApi::TestTimestampsL"));
	
	TLbsHostSettingsSupl setting;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting, _L("HostSetting_One")));
		
	// 1. Create a new host setting
	TLbsHostSettingsId newSettingId;
	TInt err = iLBSHostStore->CreateHostSettings(setting, KLbsHostSettingsTestCreatorId, newSettingId);
	ASSERT_EQUALS(err, KErrNone);
			
	// 2. Retrieve just created host and extract the timestamp value 	
	TLbsHostSettingsSupl settingStored;
	err = iLBSHostStore->GetHostSettings(newSettingId, settingStored);	
	ASSERT_EQUALS(err, KErrNone);
		
	TTime t1 = settingStored.LastModified();
	
	// wait a few seconds.
	User::After(3*1000*1000);
	
	// 3 Update the previously added host settings
	TLbsHostSettingsSupl setting2;
	ASSERT_EQUALS(KErrNone, CTestSuplUtils::GetHostSettingFromConfig(iConfig, setting2, _L("HostSetting_Two")));
	err = iLBSHostStore->UpdateHostSettings(newSettingId, setting2);	
	ASSERT_EQUALS(err, KErrNone);
	
	// 4 Reads setting2 back
	
	TLbsHostSettingsSupl settingStored2;
	err = iLBSHostStore->GetHostSettings(newSettingId, settingStored2);	
	ASSERT_EQUALS(err, KErrNone);
		
	// 5. Retrieve the last modified timestamp and check that it is greater than the last timestamp
	TTime t2 = settingStored2.LastModified();
	ASSERT_TRUE(t1 < t2);
	}

void CTestSuplLbsApi::TestPortNumberNotSetL()
//
// Test that if no port number is specified, the OMA default of 7275 will be set by default.
// 1. Create a new host settings
// 2. Initialise it with typical loopback values
// 3. DO NOT SET PORT NUMBER
// 4. Attempt to write settings to the store
// 5. Attempt to retrieve the entry without error.
// 6. Attempt to read port number from retrieved entry
// 7. Test that it is as expected.
	{
	INFO_PRINTF1(_L("--- TestPortNumberNotSetL >>"));
	CLbsHostSettingsStore* commonSettingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(commonSettingsStore);
	
	//	Go to start...
	commonSettingsStore->RewindHostSettings();

	// 1. Create a new host settings using normal construction, and set all values accept port number.
	TLbsHostSettingsSupl hostSettings;
	TLbsHostSettingsSupl readHostSettings;
	
	// 2. Initialise it with typical loopback values
	_LIT8(KLoopBackIp, "127.0.0.1");
	_LIT8(KSymbian, "Symbian");
	const TLbsHostNameAddress loopbackip(KLoopBackIp);
	const TLbsHostNameAddress symbian(KSymbian);

	hostSettings.SetHostNameAddress(loopbackip); 
	hostSettings.SetConnectionPoint(0, TLbsHostSettingsSupl::ELbsConnectionTypeNap);
	hostSettings.SetReadOnly(EFalse);

	// 3. DO NOT SET PORT NUMBER
	//	Note how there is no port number specified, the write bethod should
	//	default this to 7275 if the constructor has not already.
	//	hostSettings.SetPortId(7275);
	hostSettings.SetName(symbian);
	hostSettings.SetProviderId(symbian);
	hostSettings.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthNone);
	hostSettings.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthNone);

	// 4. Attempt to write settings to the store
	TUid uid;

	TInt err = commonSettingsStore->CreateHostSettings(hostSettings, devProvHostSettingsUid, uid);
	INFO_PRINTF2(_L("--- Prior to assert: CreateHostSettings error return was %d, should be KErrNone."), err);
	ASSERT_EQUALS(err, KErrNone);
	
	// 5. Attempt to retrieve the entry without error.
	err = commonSettingsStore->GetHostSettings(uid, readHostSettings);
	INFO_PRINTF2(_L("--- Prior to assert: GetHostSettings error return was %d, should be KErrNone."), err);
	ASSERT_EQUALS(err, KErrNone);
	
	// 6. Attempt to read port number from retrieved entry	
	TInt readPort;
	readHostSettings.GetPortId(readPort);
	INFO_PRINTF2(_L("--- Prior to assert: retrievedPort value was %d, should be 7275."), readPort);

	// 7. Test that it is as expected.
	ASSERT_EQUALS(7275, readPort)
	
	CleanupStack::PopAndDestroy(commonSettingsStore);
	INFO_PRINTF1(_L("--- TestPortNumberNotSetL <<"));
	}
