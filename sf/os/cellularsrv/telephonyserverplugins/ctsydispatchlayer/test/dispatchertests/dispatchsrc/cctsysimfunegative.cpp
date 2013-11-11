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

#include "cctsysimfunegative.h"

#include <e32math.h> 
#include <etelmm.h>
#include <etelmmerr.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>

#include "config.h"
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <test/tmockltsydata.h>


/* static */
CTestSuite* CCTsySimFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetServiceTableL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestSetIccMessageWaitingIndicatorsL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetIccMessageWaitingIndicatorsL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetAPNControlListServiceStatusL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestSetApnControlListServiceStatusL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestEnumerateAPNEntriesL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetAPNnameL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestDeleteAPNNameL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestAppendAPNNameL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestChangeSecurityCodeL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetSubscriberIdL);

	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestPowerSimOffL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestSendAPDUReqL);

	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestSimWarmResetL);

	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestReadSimFileL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestActivateSimLockL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestDeActivateSimLockL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetAnswerToResetL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetSimCardReaderStatusL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetSimAuthenticationEapSimDataL);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFUNegative, TestGetSimAuthenticationEapAkaDataL);

	END_SUITE;
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0001
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetServiceTable API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetServiceTableL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneNegativeL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId);
	
	RMobilePhone::TMobilePhoneServiceTableV1 tbData;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg pkg(tbData);
	
	TRequestStatus status;
	iPhone.GetServiceTable(status, RMobilePhone::EUSIMServiceTable, pkg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrMMEtelWrongMode);
	
	AssertMockLtsyStatusL();
		
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config); // this, config
	}


/**
 * @SYMTestCaseID BBA-CTSYD-DIS-SIM-NEGATIVE-UN0002
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the SetIccMessageWaitingIndicators API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestSetIccMessageWaitingIndicatorsL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TMobilePhoneMessageWaitingV1 mwiSet;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mwiSetRetPckg(mwiSet);

	TRequestStatus status;
	iPhone.SetIccMessageWaitingIndicators(status, mwiSetRetPckg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);
	AssertMockLtsyStatusL();
	
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0003
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetIccMessageWaitingIndicators API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetIccMessageWaitingIndicatorsL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneNegativeL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId);

	RMobilePhone::TMobilePhoneMessageWaitingV1 mwi;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mwiRetPckg(mwi);

	TRequestStatus status;
	iPhone.GetIccMessageWaitingIndicators(status, mwiRetPckg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0004
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetAPNControlListServiceStatus API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetAPNControlListServiceStatusL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TAPNControlListServiceStatus statusGet;
	
	TRequestStatus status;
	iPhone.GetAPNControlListServiceStatus(status, statusGet);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);
	AssertMockLtsyStatusL();

	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0005
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the SetApnControlListServiceStatus API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call SetAPNControlListServiceStatus, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestSetApnControlListServiceStatusL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TAPNControlListServiceStatus statusSet = RMobilePhone::EAPNControlListServiceEnabled;
	
	TRequestStatus status;
	iPhone.SetAPNControlListServiceStatus(status, statusSet);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0006
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the EnumerateAPNEntries API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestEnumerateAPNEntriesL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TUint32 enmrtRet;
	TRequestStatus status;
	iPhone.EnumerateAPNEntries(status, enmrtRet);
	User::WaitForRequest(status);	
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config	
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0007
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetAPNname API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetAPNnameL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TAPNEntryV3 apnEntryGet;
	RMobilePhone::TAPNEntryV3Pckg apnEntryGetPckg(apnEntryGet);
	
	TRequestStatus status;
	iPhone.GetAPNname(status, 1, apnEntryGetPckg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config	
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0008
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the DeleteAPNName API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestDeleteAPNNameL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus status;
	iPhone.DeleteAPNName(status, 1);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config	
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0009
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the AppendAPNName API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestAppendAPNNameL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TAPNEntryV3 entryExp;
	RMobilePhone::TAPNEntryV3Pckg entryAppPckg(entryExp);
	
	TRequestStatus status;
	iPhone.AppendAPNName(status, entryAppPckg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config	
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0010
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the ChangeSecurityCode API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestChangeSecurityCodeL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	_LIT(KOldPwd,"oldPswd");
	_LIT(KNewPwd,"newPswd");
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange;
	pwdChange.iNewPassword = KNewPwd;
	pwdChange.iOldPassword = KOldPwd;
	// Choose Phone Device Lock
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;
	
	TRequestStatus status;
	iPhone.ChangeSecurityCode(status, secCode, pwdChange);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config		
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0011
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetSubscriberId API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetSubscriberIdL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhone::TMobilePhoneSubscriberId idGet;
	
	TRequestStatus status;
	iPhone.GetSubscriberId(status, idGet);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, &config);	// this, config	
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0015
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the PowerSimOff API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestPowerSimOffL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TRequestStatus status;
	customApi.PowerSimOff(status);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customApi, this, config		
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0016
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the SendAPDUReq API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestSendAPDUReqL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TBuf8<3> info;
	info.Append(1);
	info.Append(1);
	info.Append(1);
	
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	_LIT8(KApduDataExp,"APDU DATA EXP ");
	dataBuf.CreateL(KApduDataExp);
	
	RMmCustomAPI::TApdu apdu(info,dataBuf);
	
	TRequestStatus status;
	customApi.SendAPDUReq(status, apdu);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, &config);	// dataBuf, customApi, this, config		
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0018
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the SimWarmReset API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestSimWarmResetL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TRequestStatus status;
	customApi.SimWarmReset(status);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customApi, this, config			
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0020
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the ReadSimFile API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestReadSimFileL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	//example Ef(Imsi) path
	RMmCustomAPI::TSimFilePath path;
	path.Append(0x3F);
	path.Append(0x00);
	path.Append(0x7F);
	path.Append(0x00);
	path.Append(0x6F);
	path.Append(0x07);
	path.Append(0x00);
	path.Append(0x00);
	
	const TUint16 offset = 0;
	const TUint16 size = 15;
	
	RMmCustomAPI::TSimFileInfo simFileInfo;
	simFileInfo.iPath = path;
	simFileInfo.iOffSet = offset;
	simFileInfo.iSize = size;
	RMmCustomAPI::TSimFileInfoPckg simFileInfoPckg(simFileInfo);
	
	TBuf8<20> responseBytes;
	
	TRequestStatus status;
	customApi.ReadSimFile(status, simFileInfoPckg, responseBytes);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config	
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0021
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the ActivateSimLock API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned for both async and sync cases
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestActivateSimLockL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	_LIT(KPassword, "12345");
	
	RMmCustomAPI::TSimLockPassword simLockPassword;
	RMmCustomAPI::TLockNumber lockNumber;
	
	lockNumber = RMmCustomAPI::EOperator;
	simLockPassword.Copy(KPassword);
		
	TRequestStatus status;
	// async version
	customApi.ActivateSimLock(status, simLockPassword, lockNumber);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	// synchronous version
	TInt ret = customApi.ActivateSimLock(simLockPassword, lockNumber);
	ASSERT_EQUALS(ret, KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config		
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0022
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the  API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned for both async & sync APIs
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestDeActivateSimLockL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	_LIT(KPassword, "12345");
	
	RMmCustomAPI::TSimLockPassword simLockPassword;
	RMmCustomAPI::TLockNumber lockNumber;
	
	lockNumber = RMmCustomAPI::EOperator;
	simLockPassword.Copy(KPassword);
	
	TRequestStatus status;
	// async version
	customApi.DeActivateSimLock(status, simLockPassword, lockNumber);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	// synchronous version
	TInt ret = customApi.DeActivateSimLock(simLockPassword, lockNumber);
	ASSERT_EQUALS(ret, KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config		
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0023
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetAnswerToReset (ATR) API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetAnswerToResetL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	_LIT8(KAnswerToResetSend, "ExampleAnswerToResetSend");
	TBuf8<100> answerToReset(KAnswerToResetSend);
	
	TRequestStatus status;
	customApi.GetATR(status, answerToReset);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config		
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0024
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetSimCardReaderStatus API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetSimCardReaderStatusL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RMmCustomAPI::TSimCardReaderStatus simCardReaderStatus;
	
	TRequestStatus status;
	customApi.GetSimCardReaderStatus(status, simCardReaderStatus);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config		
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0025
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetSimAuthenticationEapSimData API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetSimAuthenticationEapSimDataL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	// generate a random parameter 
	TUint32 randomValue; 
	TBuf8<16> randomParameter;
	for (TInt i = 0; i< 16; i++)
		{
		randomValue = Math::Random();
		randomParameter.Append(randomValue);
		}
		
	RMmCustomAPI::TSimAuthenticationEapSim eapSim;
	eapSim.iRandomParameters.Copy(randomParameter);
	RMmCustomAPI::TSimDataPckg simDataPckg(eapSim);
	
	TRequestStatus status;
	customApi.GetWlanSimAuthenticationData(status, simDataPckg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config		
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SIM-NEGATIVE-UN0026
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetSimAuthenticationEapAkaData API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySimFUNegative::TestGetSimAuthenticationEapAkaDataL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TUint32 randomValue; 
	TBuf8<16> randomParameter2;
	for (TInt i = 0; i< 16; i++)
		{
		randomValue = Math::Random();
		randomParameter2.Append(randomValue);
		}
	
	_LIT8(KAUTN,"AUTN Value");
	
	RMmCustomAPI::TSimAuthenticationEapAka eapAka;
	eapAka.iRandomParameters.Copy(randomParameter2);
	eapAka.iAUTN.Copy(KAUTN);
	RMmCustomAPI::TAkaDataPckg akaDataPckg(eapAka);
	
	TRequestStatus status;
	customApi.GetWlanSimAuthenticationData(status, akaDataPckg);
	User::WaitForRequest(status);
	ASSERT_EQUALS(status.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, &config);	// customAPI, this, config		
	}

