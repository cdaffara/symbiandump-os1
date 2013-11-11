// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// when support for the various APIs are disabled in the dispatch layer.
//



/**
 @file	The TEFUnit test suite for SmsControl in the Common TSY
*/

#include "cctsysatfunegative.h"

#include <etelmm.h>
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/sat_defs.h>
#include <cstktsy/ttlv.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h> //for KLtsyDispatchPhoneGetPhoneIdApiId
#include <ctsy/ltsy/mltsydispatchsmsinterface.h> //for KLtsyDispatchSmsSendSatSmsApiId
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h> //for KLtsyDispatchPhonebookSmsStoreWriteEntryApiId
#include "mockltsyindicatorids.h"
#include "config.h"
#include "testconstants.h"

const TInt KShiftBitsToGetMSB = 8;

CTestSuite* CCTsySatFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsySatFUNegative, TestUnit0014L);

	END_SUITE;
	}

CCTsySatFUNegative::~CCTsySatFUNegative()
	{
	iSat.Close();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0001
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, EFalse);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint8 enable = KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled; 
	
	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = enable & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = enable & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	RSat sat;
	TInt ret = sat.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, &data);
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0002
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, EFalse);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TUint8 enable = KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled; 
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
		
	TBool smsMoControlActivated = enable & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = enable & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = enable & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	RSat sat;
	TInt ret = sat.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, &data);
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0003
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0003L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, EFalse);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TUint8 enable = KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled; 
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
		
	TBool smsMoControlActivated = enable & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	
	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);

	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = enable & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = enable & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	RSat sat;
	TInt ret = sat.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, &data);
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0004
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0004L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, EFalse);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TUint8 enable = KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled; 
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
		
	TBool smsMoControlActivated = enable & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	
	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = enable & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	RSat sat;
	TInt ret = sat.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, &data);
	config.Reset();	
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0005
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0005L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId, EFalse);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TUint8 enable = KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled; 
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
		
	TBool smsMoControlActivated = enable & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	
	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = enable & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = enable & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	RSat sat;
	TInt ret = sat.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, &data);
	config.Reset();	
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0006
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0006L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, EFalse);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TUint8 enable = KSmsMoControlEnabled | KSmsPpDownloadEnabled | KUssdEnabled; 
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
		
	TBool smsMoControlActivated = enable & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	
	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = enable & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	RSat sat;
	TInt ret = sat.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, &data);
	config.Reset();	
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0007
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0007L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
		
	TRequestStatus reqStatus;
	
	const TUint8 KItemIdentifier1 = 77;
	RSat::TMenuSelectionV1 menuSelection;
	RSat::TMenuSelectionV1Pckg menuSelectionPckg(menuSelection);
	menuSelection.iItemId = KItemIdentifier1;
	menuSelection.iHelp = RSat::EHelpRequested;

	iSat.MenuSelection(reqStatus, menuSelectionPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNotSupported);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // data, this
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0008
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId, EFalse);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	
	TRequestStatus reqStatus;
	RSat::TIcon retrievedIconEfImg;
    RSat::TIconId iconId;
    iconId.iIdentifier = 1;
    
	iSat.GetIcon(reqStatus,iconId.iIdentifier,retrievedIconEfImg); 
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNotSupported);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);	
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0009
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId, EFalse);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	RBuf8 getData;
	CleanupClosePushL(getData);
	
	RBuf8 retData;
	CleanupClosePushL(retData);
	
	TRequestStatus reqStatus;

	/******************************************Get Icon*******************************************************/
	
	//Test to get a given Icons EF(Img) record
	
	//re-create the file path
    TBuf8<KFilePathLength> simFilePath(KFilePathLength);
    simFilePath[0] = 0x3F; //MF High byte
    simFilePath[1] = 0x00; //MF Low byte
    simFilePath[2] = 0x7F; //DF(Telecom) High byte
    simFilePath[3] = 0x10; //DF(Telecom) Low byte
    simFilePath[4] = 0x5F; //DF(Graphics) High byte
    simFilePath[5] = 0x50; //DF(Graphics) Low byte
    simFilePath[6] = 0x4F; //EF(Img) High byte
    simFilePath[7] = 0x20; //EF(Img) Low byte
    
    //a EF(Img) record to get
    const TUint8 KRecordNumber = 5;
        
    //create the down expect data
    TUint8 recordNumber = KRecordNumber;
    TDesC8* simFilePathPtr = &simFilePath;
    TMockLtsyData2<TUint8,TDesC8*> getIconData(recordNumber,simFilePathPtr);
    getIconData.SerialiseL(getData);

    //create a EF(Img) record    
    const TUint KNoImagesLength = 1;
    const TUint KEfImgRecordLength = 9;
    
    RBuf8 iconEfRecordInstance;
    CleanupClosePushL(iconEfRecordInstance);
    iconEfRecordInstance.CreateMaxL(KEfImgRecordLength);
    
    const TUint8 KNumberOfImages = 1;
    
    const TUint16 KInstanceNumber = 1;
    const TUint16 KOffset = 1;
    const TUint16 KLength = 1;

    const TUint8 KWidth  = 40;
    const TUint8 KHeight = 40;
    
    iconEfRecordInstance[0] = KWidth; 								//image instance width, expressed in raster image points
    iconEfRecordInstance[1] = KHeight; 								//image instance height, expressed in raster image points
    iconEfRecordInstance[2] = RSat::KBasic;							//image coding scheme that has been used in encoding the image instance 11-basic, 21-colour 
    iconEfRecordInstance[3] = KInstanceNumber >> KShiftBitsToGetMSB; 	//high byte of Image Instance File Identifier
    iconEfRecordInstance[4] = KInstanceNumber & 0xFF; 				//low byte of Image Instance File Identifier.
    iconEfRecordInstance[5] = KOffset >> KShiftBitsToGetMSB;				//high byte of offset into Image Instance File
    iconEfRecordInstance[6] = KOffset & 0xFF; 						//low byte of offset into Image Instance File
    iconEfRecordInstance[7] = KLength >> KShiftBitsToGetMSB;				//high byte of  Image Instance Data length
    iconEfRecordInstance[8] = KLength & 0xFF;						//low byte of Image Instance Data length
                     
    RBuf8 iconEfImgData;
    CleanupClosePushL(iconEfImgData);
    iconEfImgData.CreateL(KNoImagesLength + KEfImgRecordLength);
    iconEfImgData.Append(KNumberOfImages);
    iconEfImgData.Append(iconEfRecordInstance);
    
    
    //create the up complete data
    TDesC8* iconDataPtr = &iconEfImgData;
    TMockLtsyData1<TDesC8*> retIconData(iconDataPtr);
    retIconData.SerialiseL(retData);
    
    //queue the MockLTSY expect completes
	iMockLTSY.ExpectL(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId,getData);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId, KErrNone, retData);
	
	// ETel Sat Calls
	RSat::TIcon retrievedIconEfImg;
    RSat::TIconId iconId;
    iconId.iIdentifier = KRecordNumber;
    
	iSat.GetIcon(reqStatus,iconId.iIdentifier,retrievedIconEfImg); 
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_TRUE(iconEfImgData == retrievedIconEfImg);
	
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();
	
	CleanupStack::PopAndDestroy(&iconEfImgData);
	CleanupStack::PopAndDestroy(&iconEfRecordInstance);
	
	/*************************************************************************************************/
	
	/******************************************Get Image Instance*******************************************************/
	
	//this is used in the CTSY to check against the retrieved icon EF(Img) record
	RSat::TInstanceInfoV3 instanceInfo;
	instanceInfo.iInstanceNumber	= KInstanceNumber;
	instanceInfo.iLength			= KLength;
	instanceInfo.iOffset			= KOffset;
	instanceInfo.iCoding			= RSat::KBasic;
	instanceInfo.iImgWidth			= KWidth;
	instanceInfo.iImgHeight			= KHeight;
	instanceInfo.iFileId			= 0; //not used in the CTSY
 
	RSat::TInstanceInfoV3Pckg instanceInfoPackage1(instanceInfo);
	const TUint KIconDataLength = 2;
	
	RBuf8 retrievedImageinstance;
	CleanupClosePushL(retrievedImageinstance);
	retrievedImageinstance.CreateL(KIconDataLength + (KWidth * KHeight)); //the EF(IIDF)
	
	// ETel Sat Calls
	iSat.GetImageInstance(reqStatus, instanceInfoPackage1, retrievedImageinstance);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNotSupported);
	
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();

	CleanupStack::PopAndDestroy(&retrievedImageinstance);
		
	CleanupStack::PopAndDestroy(&retData);
	CleanupStack::PopAndDestroy(&getData);
	CleanupStack::PopAndDestroy(this);		
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0010
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0010L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	RBuf8 getData;
	CleanupClosePushL(getData);
	
	RBuf8 retData;
	CleanupClosePushL(retData);
	
	TRequestStatus reqStatus;

	/******************************************Get Icon*******************************************************/
	
	//Test to get a given Icons EF(Img) record
	
	//re-create the file path
    TBuf8<KFilePathLength> simFilePath(KFilePathLength);
    simFilePath[0] = 0x3F; //MF High byte
    simFilePath[1] = 0x00; //MF Low byte
    simFilePath[2] = 0x7F; //DF(Telecom) High byte
    simFilePath[3] = 0x10; //DF(Telecom) Low byte
    simFilePath[4] = 0x5F; //DF(Graphics) High byte
    simFilePath[5] = 0x50; //DF(Graphics) Low byte
    simFilePath[6] = 0x4F; //EF(Img) High byte
    simFilePath[7] = 0x20; //EF(Img) Low byte
    
    //a EF(Img) record to get
    const TUint8 KRecordNumber = 5;
        
    //create the down expect data
    TUint8 recordNumber = KRecordNumber;
    TDesC8* simFilePathPtr = &simFilePath;
    TMockLtsyData2<TUint8,TDesC8*> getIconData(recordNumber,simFilePathPtr);
    getIconData.SerialiseL(getData);

    //create a EF(Img) record    
    const TUint KNoImagesLength = 1;
    const TUint KEfImgRecordLength = 9;
    
    RBuf8 iconEfRecordInstance;
    CleanupClosePushL(iconEfRecordInstance);
    iconEfRecordInstance.CreateMaxL(KEfImgRecordLength);
    
    const TUint8 KNumberOfImages = 1;
    
    const TUint16 KInstanceNumber = 1;
    const TUint16 KOffset = 1;
    const TUint16 KLength = 1;

    const TUint8 KWidth  = 40;
    const TUint8 KHeight = 40;
    
    iconEfRecordInstance[0] = KWidth; 								//image instance width, expressed in raster image points
    iconEfRecordInstance[1] = KHeight; 								//image instance height, expressed in raster image points
    iconEfRecordInstance[2] = RSat::KBasic;							//image coding scheme that has been used in encoding the image instance 11-basic, 21-colour 
    iconEfRecordInstance[3] = KInstanceNumber >> KShiftBitsToGetMSB; 	//high byte of Image Instance File Identifier
    iconEfRecordInstance[4] = KInstanceNumber & 0xFF; 				//low byte of Image Instance File Identifier.
    iconEfRecordInstance[5] = KOffset >> KShiftBitsToGetMSB;				//high byte of offset into Image Instance File
    iconEfRecordInstance[6] = KOffset & 0xFF; 						//low byte of offset into Image Instance File
    iconEfRecordInstance[7] = KLength >> KShiftBitsToGetMSB;				//high byte of  Image Instance Data length
    iconEfRecordInstance[8] = KLength & 0xFF;						//low byte of Image Instance Data length
                     
    RBuf8 iconEfImgData;
    CleanupClosePushL(iconEfImgData);
    iconEfImgData.CreateL(KNoImagesLength + KEfImgRecordLength);
    iconEfImgData.Append(KNumberOfImages);
    iconEfImgData.Append(iconEfRecordInstance);
    
    
    //create the up complete data
    TDesC8* iconDataPtr = &iconEfImgData;
    TMockLtsyData1<TDesC8*> retIconData(iconDataPtr);
    retIconData.SerialiseL(retData);
    
    //queue the MockLTSY expect completes
	iMockLTSY.ExpectL(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId,getData);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId, KErrNone, retData);
	
	// ETel Sat Calls
	RSat::TIcon retrievedIconEfImg;
    RSat::TIconId iconId;
    iconId.iIdentifier = KRecordNumber;
    
	iSat.GetIcon(reqStatus,iconId.iIdentifier,retrievedIconEfImg); 
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_TRUE(iconEfImgData == retrievedIconEfImg);
	
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();
	
	CleanupStack::PopAndDestroy(&iconEfImgData);
	CleanupStack::PopAndDestroy(&iconEfRecordInstance);
	
	/*************************************************************************************************/
	
	/******************************************Get Image Instance*******************************************************/

	// Test to get a given instance of an icons EF(IIDF)
	
	// The Client should have read the icon EF before reading the image instance.
	// The contents of the icon EF indicate the size of the image instance,
	// the Client should use this information to allocate a buffer size that it requires
	// and pass a reference to is as a TDes8&
	
    //create the down expect data
	TUint8 imageInstanceNumberInRecord = KNumberOfImages - KInstanceNumber + 1;
	TMockLtsyData2<TUint8,TUint8> getImageInstanceData(recordNumber,imageInstanceNumberInRecord);
	getImageInstanceData.SerialiseL(getData);
	
    RBuf8 imageInstanceData;
    CleanupClosePushL(imageInstanceData);
    
    RBuf8 imageInstanceBody;
    CleanupClosePushL(imageInstanceBody);
	
    //A basic image data
    const TUint KIconDataLength = 2;
    imageInstanceData.CreateMaxL(KIconDataLength);
    imageInstanceData[0] = retrievedIconEfImg[KNoImagesLength + 1]; //i.e. KWidth;
    imageInstanceData[1] = retrievedIconEfImg[KNoImagesLength + 2]; //i.e. KHeight;
    
    //The image body
    imageInstanceBody.CreateMaxL(KWidth * KHeight);
    
    //fill with any data (just for testing)
    for(TInt i = 0; i < imageInstanceBody.Length(); ++i)
    	{
    	imageInstanceBody[i] = i;
    	}
    
    TDesC8* imageInstanceDataPtr = &imageInstanceData;
    TDesC8* imageInstanceBodyPtr = &imageInstanceBody;
    
    TMockLtsyData2<TDesC8*,TDesC8*> retImageInstanceData(imageInstanceDataPtr,imageInstanceBodyPtr);
    retImageInstanceData.SerialiseL(retData);
	
    //queue the MockLTSY expect completes
	iMockLTSY.ExpectL(MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId,getData);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId, KErrNone, retData);
	
	//this is used in the CTSY to check against the retrieved icon EF(Img) record
	RSat::TInstanceInfoV3 instanceInfo;
	instanceInfo.iInstanceNumber	= KInstanceNumber;
	instanceInfo.iLength			= KLength;
	instanceInfo.iOffset			= KOffset;
	instanceInfo.iCoding			= RSat::KBasic;
	instanceInfo.iImgWidth			= KWidth;
	instanceInfo.iImgHeight			= KHeight;
	instanceInfo.iFileId			= 0; //not used in the CTSY
 
	RSat::TInstanceInfoV3Pckg instanceInfoPackage1(instanceInfo);
	
	RBuf8 retrievedImageinstance;
	CleanupClosePushL(retrievedImageinstance);
	retrievedImageinstance.CreateL(KIconDataLength + (KWidth * KHeight)); //the EF(IIDF)
	
	// ETel Sat Calls
	iSat.GetImageInstance(reqStatus, instanceInfoPackage1, retrievedImageinstance);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_TRUE(retrievedImageinstance.Mid(0,KIconDataLength) == imageInstanceData);
	ASSERT_TRUE(retrievedImageinstance.Mid(KIconDataLength) == imageInstanceBody);
	
	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();

	CleanupStack::PopAndDestroy(&retrievedImageinstance);
	CleanupStack::PopAndDestroy(&imageInstanceBody);
	CleanupStack::PopAndDestroy(&imageInstanceData);
	
	/*************************************************************************************************/

	/******************************************Get Image Instance*************************************/


	const TUint KGetClutOffset = 0; //Unused in the CTSY

	// ETel Sat Calls
	const TUint KClutLength = 24;
	TBuf8<KClutLength> retrievedClut;
	iSat.GetClut(reqStatus, KInstanceNumber, KGetClutOffset, retrievedClut);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNotSupported);

	AssertMockLtsyStatusL();
	
	getData.Close();
	retData.Close();
	
	/*************************************************************************************************/
		
	CleanupStack::PopAndDestroy(&retData);
	CleanupStack::PopAndDestroy(&getData);
	CleanupStack::PopAndDestroy(this);	
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0011
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	const TUint8 KPCommandNumber = 123;

	SetupEventListL(KPCommandNumber,KEventUserActivity);
	
	//ETel calls event download
	RSat::TEventDownloadBaseV2      eventDownload;
	RSat::TEventDownloadBaseV2Pckg  eventDownloadPck(eventDownload);
	RSat::TEventList eventList = RSat::KUserActivity;

	TRequestStatus reqStatus;
	iSat.EventDownload(reqStatus, eventList, eventDownloadPck); // No associated data

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	config.Reset();
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0013
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0013L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId, EFalse);

	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL(KSmsMoControlEnabled);

	//setup local info as this is appended to the MO SHORT MESSAGE CONTROL ENVELOPE
	SetupLocalInfoL(KLocationAreaCode,KCellId,KOperatorCode,KLocationStatusNormalService);
	
	
	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	TRequestStatus sendMessageReqStatus;
	TRequestStatus moSmControlRequestReqStatus;

	//send a Sm proactive command
	TUint8 pcmdCode = KSendShortMessage;
	TMockLtsyData1<TUint8> expPCmdNotificationLtsyData(pcmdCode);
	expPCmdNotificationLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	expPCmdNotificationLtsyData.Close();


	const TUint8 KPCmdNumber = 123;
	const TUint8 KCommandQualifier = 0;  //0 = packing not required, 1 = SMS packing by the ME required.
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);

	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSendShortMessage); //ETLV_TypeOfCommand
	tlv.AddByte(KCommandQualifier); // 0 = packing not required, 1 = SMS packing by the ME required.

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);
	tlv.AddByte(KMe);
	
	_LIT8(KAlphaId8,"Text To Display");
	_LIT16(KAlphaId16,"Text To Display");
	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddData(KAlphaId8);
	
	tlv.AddTag(KTlvAddressTag);
	tlv.AddByte(0); //Ton NPI
	tlv.AddData(_L8("Number"));
	

	TUint8 tpduProtocolByte = 0;
		
	tpduProtocolByte |= 0x01 << 0;//TP Message Type Indicator, bit 0-1 - SMS SUBMIT
	tpduProtocolByte |= 0x00 << 2;//TP Reject Duplicates, bit 2 - Instruct the SC to accept an SMS SUBMIT for an SM still held in the
						 		//SC which has the same TP MR and the same TP DA as a previously 	submitted SM from 	the same OA.
	tpduProtocolByte |= 0x00 << 3; //TP Validity Period Format, bit 3-4 - TP VP field not present
	tpduProtocolByte |= 0x00 << 5; //TP Status Report Request, bit 5 - A status report is not requested
	tpduProtocolByte |= 0x00 << 6;	//TP User Data Header Indicator, bit 6 - The TP UD field contains only the short message
	tpduProtocolByte |= 0x00 << 7;	//TP Reply Path, bit 7- TP Reply Path parameter is not set in this SMS SUBMIT/DELIVER

	const TUint8 KProtocolIdentifier = 0x7F; //SmsTpduProtcolIdUSimDdl Parameters(1)+TP-OA(min 2)+TP-PID(1)+DCS(1)+TP-SCTS(7)+TP-UDL(2)+TP-UD(?)
	const TUint8 KDataCodingScheme = 0x02; //TP Data Coding Scheme (TP DCS)

	TTpdu tpdu;
	tpdu.Append(tpduProtocolByte);
	
	_LIT8(KOriginatingAddress,"1234567");
	tpdu.Append(KOriginatingAddress().Length() * 2); //Number of octets
	tpdu.Append(0); //Type of address
	tpdu.Append(KOriginatingAddress());
	tpdu.Append(KProtocolIdentifier); //TP Protocol Identifier 
	tpdu.Append(KDataCodingScheme); //TP Data Coding Scheme (TP DCS)
	tpdu.Append(_L8("1234567")); //TP Service Centre Time Stamp

	_LIT8(KUserData,"User Data");
	tpdu.Append(KUserData().Length()); //TP User Data Length
	tpdu.Append(KUserData()); //TP User Data

	tlv.AddTag(KTlvSmsTpduTag);
	tlv.AddData(tpdu);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(sendSm.PCmdNumber(),KPCmdNumber);
	ASSERT_EQUALS(sendSm.iAlphaId.iStatus,RSat::EAlphaIdProvided);
	ASSERT_TRUE(sendSm.iAlphaId.iAlphaId == KAlphaId16);
	ASSERT_TRUE(sendSm.iSmsTpdu == tpdu);
	
	_LIT8(KServiceCentreNumber,"123456789");
	
	const RSat::TTypeOfNumber KSatTon = RSat::ENetworkSpecificNumber;
	const RSat::TNumberingPlan KSatNpi = RSat::EPrivateNumberPlan;
	
	RSat::TSatSmsV1 satSms;
	RSat::TSatSmsV1Pckg satSmsPckg(satSms);
	satSms.iBuf = sendSm.iSmsTpdu;
	satSms.iServiceCenter.iTelNumber.Copy(KServiceCentreNumber);
	satSms.iServiceCenter.iTypeOfNumber = KSatTon;
	satSms.iServiceCenter.iNumberPlan = KSatNpi;
		
	RSat::TMoSmControlV1 moSmsControl;
	RSat::TMoSmControlV1Pckg moSmsControlPckg(moSmsControl);
	iSat.NotifyMoSmControlRequest(moSmControlRequestReqStatus,moSmsControlPckg);

	
	TUint16 msgRef;
	iSat.SendMessageNoLogging(sendMessageReqStatus,satSmsPckg,msgRef);

	User::WaitForRequest(sendMessageReqStatus);
	ASSERT_EQUALS(sendMessageReqStatus.Int(),KErrNotSupported);
		
	//generate the terminal response to the send sms proactive command.

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);		

	
	config.Reset();
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SAT-NEGATIVE-UN0014
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId IPC is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySatFUNegative::TestUnit0014L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL(KSmsMoControlEnabled);

	//setup local info as this is appended to the MO SHORT MESSAGE CONTROL ENVELOPE
	SetupLocalInfoL(KLocationAreaCode,KCellId,KOperatorCode,KLocationStatusNormalService);
	
	
	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	TRequestStatus sendMessageReqStatus;
	TRequestStatus moSmControlRequestReqStatus;

	//send a Sm proactive command
	TUint8 pcmdCode = KSendShortMessage;
	TMockLtsyData1<TUint8> expPCmdNotificationLtsyData(pcmdCode);
	expPCmdNotificationLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	expPCmdNotificationLtsyData.Close();


	const TUint8 KPCmdNumber = 123;
	const TUint8 KCommandQualifier = 0;  //0 = packing not required, 1 = SMS packing by the ME required.
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);

	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSendShortMessage); //ETLV_TypeOfCommand
	tlv.AddByte(KCommandQualifier); // 0 = packing not required, 1 = SMS packing by the ME required.

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KSim);
	tlv.AddByte(KMe);
	
	_LIT8(KAlphaId8,"Text To Display");
	_LIT16(KAlphaId16,"Text To Display");
	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddData(KAlphaId8);
	
	tlv.AddTag(KTlvAddressTag);
	tlv.AddByte(0); //Ton NPI
	tlv.AddData(_L8("Number"));
	

	TUint8 tpduProtocolByte = 0;
		
	tpduProtocolByte |= 0x01 << 0;//TP Message Type Indicator, bit 0-1 - SMS SUBMIT
	tpduProtocolByte |= 0x00 << 2;//TP Reject Duplicates, bit 2 - Instruct the SC to accept an SMS SUBMIT for an SM still held in the
						 		//SC which has the same TP MR and the same TP DA as a previously 	submitted SM from 	the same OA.
	tpduProtocolByte |= 0x00 << 3; //TP Validity Period Format, bit 3-4 - TP VP field not present
	tpduProtocolByte |= 0x00 << 5; //TP Status Report Request, bit 5 - A status report is not requested
	tpduProtocolByte |= 0x00 << 6;	//TP User Data Header Indicator, bit 6 - The TP UD field contains only the short message
	tpduProtocolByte |= 0x00 << 7;	//TP Reply Path, bit 7- TP Reply Path parameter is not set in this SMS SUBMIT/DELIVER

	const TUint8 KProtocolIdentifier = 0x7F; //SmsTpduProtcolIdUSimDdl Parameters(1)+TP-OA(min 2)+TP-PID(1)+DCS(1)+TP-SCTS(7)+TP-UDL(2)+TP-UD(?)
	const TUint8 KDataCodingScheme = 0x02; //TP Data Coding Scheme (TP DCS)

	TTpdu tpdu;
	tpdu.Append(tpduProtocolByte);
	
	_LIT8(KOriginatingAddress,"1234567");
	tpdu.Append(KOriginatingAddress().Length() * 2); //Number of octets
	tpdu.Append(0); //Type of address
	tpdu.Append(KOriginatingAddress());
	tpdu.Append(KProtocolIdentifier); //TP Protocol Identifier 
	tpdu.Append(KDataCodingScheme); //TP Data Coding Scheme (TP DCS)
	tpdu.Append(_L8("1234567")); //TP Service Centre Time Stamp

	_LIT8(KUserData,"User Data");
	tpdu.Append(KUserData().Length()); //TP User Data Length
	tpdu.Append(KUserData()); //TP User Data

	tlv.AddTag(KTlvSmsTpduTag);
	tlv.AddData(tpdu);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(sendSm.PCmdNumber(),KPCmdNumber);
	ASSERT_EQUALS(sendSm.iAlphaId.iStatus,RSat::EAlphaIdProvided);
	ASSERT_TRUE(sendSm.iAlphaId.iAlphaId == KAlphaId16);
	ASSERT_TRUE(sendSm.iSmsTpdu == tpdu);
	
	_LIT8(KServiceCentreNumber,"123456789");
	
	const RMobilePhone::TMobileTON KMobilePhoneTon = RMobilePhone::ENetworkSpecificNumber;
	const RSat::TTypeOfNumber KSatTon = RSat::ENetworkSpecificNumber;
	const RMobilePhone::TMobileNPI KMobilePhoneNpi = RMobilePhone::EPrivateNumberPlan;
	const RSat::TNumberingPlan KSatNpi = RSat::EPrivateNumberPlan;
	
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsSendAttributes;
	smsSendAttributes.iFlags = (RMobileSmsMessaging::KSmsDataFormat | RMobileSmsMessaging::KGsmServiceCentre | RMobileSmsMessaging::KMoreToSend);
	smsSendAttributes.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
	smsSendAttributes.iGsmServiceCentre.iTelNumber.Copy(KServiceCentreNumber);
	smsSendAttributes.iGsmServiceCentre.iTypeOfNumber = KMobilePhoneTon;
	smsSendAttributes.iGsmServiceCentre.iNumberPlan = KMobilePhoneNpi;
	smsSendAttributes.iMore = EFalse; //set False in CSatTsy::SendMessageNoLogging
	
	TDesC8* tpduPtr = &tpdu;
    TMockLtsyData4< TDesC8*,
                        RMobileSmsMessaging::TMobileSmsDataFormat,
                        RMobilePhone::TMobileAddress,
                        TBool
                       > sendMessagePack(tpduPtr, smsSendAttributes.iDataFormat, smsSendAttributes.iGsmServiceCentre, smsSendAttributes.iMore);
	sendMessagePack.SerialiseL(data);	
	iMockLTSY.ExpectL(MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId,data);
	data.Close();
	
	RSat::TSatSmsV1 satSms;
	RSat::TSatSmsV1Pckg satSmsPckg(satSms);
	satSms.iBuf = sendSm.iSmsTpdu;
	satSms.iServiceCenter.iTelNumber.Copy(KServiceCentreNumber);
	satSms.iServiceCenter.iTypeOfNumber = KSatTon;
	satSms.iServiceCenter.iNumberPlan = KSatNpi;
		
	RSat::TMoSmControlV1 moSmsControl;
	RSat::TMoSmControlV1Pckg moSmsControlPckg(moSmsControl);
	iSat.NotifyMoSmControlRequest(moSmControlRequestReqStatus,moSmsControlPckg);

	
	TUint16 msgRef;
	iSat.SendMessageNoLogging(sendMessageReqStatus,satSmsPckg,msgRef);


	//indicator up from the CTSY to create a Mo Short Message Envelope 
	//LTSY -> CTSY

	//Type of Number (TON) and numbering plan identification (NPI).
	//MSB - 1, Type of Number - 011 (Network Specific Number), Numbering Plan - 1001 (Private numbering plan) 10111001 = B9 Hex
	const TUint8 KTonAndNpi = TonAndNpi(RMobilePhone::ENetworkSpecificNumber,RMobilePhone::EPrivateNumberPlan); 
	
	RBuf8 rpDestinationAddress;
	CleanupClosePushL(rpDestinationAddress);
	rpDestinationAddress.CreateL(KAddrMaxLength);
	rpDestinationAddress.Append(KTonAndNpi);  //TON & NPI
	rpDestinationAddress.Append(_L8("1234567")); //dummy data not EF(ADN) encoded data

	RBuf8 tpDestinationAddress;
	CleanupClosePushL(tpDestinationAddress);
	tpDestinationAddress.CreateL(KAddrMaxLength);
	tpDestinationAddress.Append(KTonAndNpi); //TON & NPI
	tpDestinationAddress.Append(_L8("7654321")); //dummy data not EF(ADN) encoded data

    TDesC8* rpDestinationAddressPtr = &rpDestinationAddress;
    TDesC8* tpDestinationAddressPtr = &tpDestinationAddress;

    TMockLtsyData2<TDesC8*,TDesC8*> createMoSmsControlEnvelopPack(rpDestinationAddressPtr,tpDestinationAddressPtr);

    createMoSmsControlEnvelopPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateMoSmControlEnvelopeIndId, KErrNone, data);
	data.Close();
	
	WaitForMockLTSYTerminated();
	//Response from the SIM
	//LTSY -> CTSY
	
	TTlv moSmsControlResponseTlv;

	TUint8 result = KMoSmCtrlResultAllowed;
	_LIT8(KAlphaIdData8,"Sending an SMS");
	_LIT16(KAlphaIdData16,"Sending an SMS");
	
	moSmsControlResponseTlv.Begin(result);
	
	moSmsControlResponseTlv.AddTag(KTlvAddressTag);
	moSmsControlResponseTlv.AddData(rpDestinationAddress);

	moSmsControlResponseTlv.AddTag(KTlvAddressTag);
	moSmsControlResponseTlv.AddData(tpDestinationAddress);

	moSmsControlResponseTlv.AddTag(KTlvAlphaIdentifierTag);
	moSmsControlResponseTlv.AddData(KAlphaIdData8);
	
	TDesC8* moSmsControlResponseTlvDesPtr = &const_cast<TDesC8&>( moSmsControlResponseTlv.End());
		
	TMockLtsyData1<TDesC8*> moSmsControlResponsePack(moSmsControlResponseTlvDesPtr);
	moSmsControlResponsePack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatMoSmControlEnvelopeResponseIndId,KErrNone,data);
	data.Close();
	
	TMockLtsyData3<TUint8,TDesC8*,TDesC8*> moSmControlResponseDataPack(result,rpDestinationAddressPtr,tpDestinationAddressPtr);
	moSmControlResponseDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId,data);
	data.Close();
	
	User::WaitForRequest(moSmControlRequestReqStatus);
	ASSERT_EQUALS(moSmControlRequestReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(moSmsControl.iResult,RSat::EAllowedNoModification);
	ASSERT_EQUALS(moSmsControl.iAlphaId.iStatus,RSat::EAlphaIdProvided);
	ASSERT_TRUE(moSmsControl.iAlphaId.iAlphaId == KAlphaIdData16);
	
	//LTSY -> CTSY the SMS has been sent
	iMockLTSY.CompleteL(MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId,KErrNone);
	User::WaitForRequest(sendMessageReqStatus);
	ASSERT_EQUALS(sendMessageReqStatus.Int(),KErrNone);
		
	//generate the terminal response to the send sms proactive command.
	RSat::TSendSmRspV1 sendSmRsp;
	sendSmRsp.iGeneralResult = RSat::KSuccess;
	sendSmRsp.iInfoType = RSat::KNoAdditionalInfo;
	sendSmRsp.SetPCmdNumber(KPCmdNumber);
	RSat::TSendSmRspV1Pckg sendSmRspPckg(sendSmRsp);

	GenerateAndWaitForTermRspL(KPCmdNumber,KSendShortMessage,KCommandQualifier,RSat::ESendSm,RSat::KSuccess,sendSmRspPckg,KErrNone);
	
	CleanupStack::PopAndDestroy(&tpDestinationAddress);
	CleanupStack::PopAndDestroy(&rpDestinationAddress);

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);	
		
	config.Reset();
	}

/**
 * Utility function which handles all the Expects/Completes generated by Opening an RSat object.
 */
void CCTsySatFUNegative::OpenSatL(TUint8 aEnabled)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
		
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
	
	TBool smsMoControlActivated = aEnabled & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data);

	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = aEnabled & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = aEnabled & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data);

	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 

	
	ASSERT_EQUALS(KErrNone, iSat.Open(iPhone));

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	CleanupStack::PopAndDestroy(&data); 
	}

void CCTsySatFUNegative::SetupEventListL(TUint8 aPCommandNumber, TUint8 aEvent)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
		
	//setup the expect for Mock LTSY from the RSat::NotifySetUpEventListPCmd call
	//ETel -> CTSY -> LTSY
	TUint8 pcmdCode = KSetUpEventList;
	TMockLtsyData1<TUint8> setUpEventListPCmdExpLtsyData(pcmdCode);
	setUpEventListPCmdExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
	
	//create the Mock LTSY complete TLV data (a proactive command)
	//LTSY -> CTSY
	TTlv proactiveCmdTlv;
	
	proactiveCmdTlv.Begin(KBerTlvProactiveSimCommandTag);

	proactiveCmdTlv.AddTag(KTlvCommandDetailsTag);
	proactiveCmdTlv.AddByte(aPCommandNumber);//ETLV_CommandNumber
	proactiveCmdTlv.AddByte(KSetUpEventList); //ETLV_TypeOfCommand
	proactiveCmdTlv.AddByte(0); //ETLV_CommandQualifier - RFU for Set up event list
	
	proactiveCmdTlv.AddTag(KTlvEventListTag);
	proactiveCmdTlv.AddByte(aEvent); //ETLV_TypeOfCommand

	proactiveCmdTlv.AddTag(KTlvDeviceIdentityTag);
	proactiveCmdTlv.AddByte(KSim);
	proactiveCmdTlv.AddByte(KMe);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(proactiveCmdTlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();
	
	//ETel Sat calls
	RSat::TSetUpEventListV1 setUpEventList;
	RSat::TSetUpEventListV1Pckg setUpEventListPckg(setUpEventList);
	
	iSat.NotifySetUpEventListPCmd(reqStatus,setUpEventListPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(setUpEventList.iType,RSat::EUpdateEventList);
	//Can't be asserted (easily) as some events the TSY is completly responsible for and iEvent is not set in the CTSY
	//ASSERT_TRUE((setUpEventList.iEvents & RSat::KUserActivity) == RSat::KUserActivity)
	ASSERT_EQUALS(setUpEventList.PCmdNumber(),aPCommandNumber);
	
	AssertMockLtsyStatusL();
	
	//The Terminal response
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(aPCommandNumber);

	GenerateAndWaitForTermRspL(aPCommandNumber,KSetUpEventList,0,RSat::ESetUpEventList,RSat::KSuccess,respPck,KErrNone);

	CleanupStack::PopAndDestroy(&data);
	}

void CCTsySatFUNegative::SetupLocalInfoL(TUint16 aAreaCode, TUint16 aCellId, const TDesC8& aOperatorCode, TUint8 aLocationStatus)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	ASSERT_TRUE(aOperatorCode.Length() <= KOperatorCodeLength);
	
	TDesC8* operatorCode = &const_cast<TDesC8&>(aOperatorCode); 
	
	TMockLtsyData4<TUint8, TDesC8*, TUint16, TUint16 >localInfoPack(aLocationStatus,operatorCode,aAreaCode,aCellId);
	localInfoPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatLocationStatusIndId, KErrNone, data);
	
	WaitForMockLTSYTerminated();
	
	CleanupStack::PopAndDestroy(&data);
	}


/**
 * Handles sending a terminal response, generating a "command completed successfully" additional info tlv
 */
void CCTsySatFUNegative::GenerateAndWaitForTermRspL(	TUint8 aPCmdNumber,
											TUint8 aTypeOfCommand, 
											TUint8 aCommandQualifier, 
											RSat::TPCmd aRSatTypeOfCommand,
											RSat::TPCmdResult aGeneralResult,
											const TDesC8& aRsp,
											TInt aCompError)
	{
	GenerateAndWaitForTermRspL(aPCmdNumber, aTypeOfCommand, aCommandQualifier, aRSatTypeOfCommand, aGeneralResult, KNullDesC8, aRsp, KNullDesC8,aCompError);
	}

/**
 * Handles sending a terminal response, and takes as an argument a descriptor containing a tlv. This is to be used when a "command completed successfully" tlv
 * is insufficient.
 */
void CCTsySatFUNegative::GenerateAndWaitForTermRspL(TUint8 aPCmdNumber,
											TUint8 aTypeOfCommand, 
											TUint8 aCommandQualifier,
											RSat::TPCmd aRSatTypeOfCommand,
											RSat::TPCmdResult aGeneralResult,
											const TDesC8& aGeneralResultAddInfo,
											const TDesC8& aRsp,
											const TDesC8& aAdditionalInfo,
											TInt aCompError)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;

	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
	
	GenerateTermRspTlv(terminalRsp, aPCmdNumber, aTypeOfCommand, aCommandQualifier, aGeneralResult, aGeneralResultAddInfo, aAdditionalInfo);
	
	TDesC8* terminalRspPtr = &terminalRsp;
	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);
	
	if(aRsp != KNullDesC8)
		{
		//ETel generated Terminal response
		iSat.TerminalRsp(reqStatus, aRSatTypeOfCommand, aRsp);
		}
	else
		{
		//CTSY generate Terminal response
		iMockLTSY.NotifyTerminated(reqStatus); 
		}
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	iMockLTSY.NotifyTerminated(reqStatus); 
	
	iMockLTSY.CompleteL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, aCompError);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&data);
	}

void CCTsySatFUNegative::GenerateTermRspTlv(	RBuf8& aBuf, 
										TUint8 aPCmdNumber,
										TUint8 aTypeOfCommand, 
										TUint8 aCommandQualifier, 
										RSat::TPCmdResult aGeneralResult, 
										const TDesC8& aGeneralResultAddInfo, 
										const TDesC8& aAdditionalInfo)
	{
	TTlv tlv;
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(aTypeOfCommand); //ETLV_TypeOfCommand
	tlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KMe);
	tlv.AddByte(KSim);
	
	tlv.AddTag(KTlvResultTag);
	tlv.AddByte(aGeneralResult);
	tlv.AddData(aGeneralResultAddInfo);
	
	aBuf.Zero();
	aBuf.Append(tlv.GetDataWithoutTopLevelTag());
	aBuf.Append(aAdditionalInfo);
	}

void CCTsySatFUNegative::RefreshNoTerminalRspL(TUint8 aPCmdNumber, TUint8 aCommandQualifier, TInt aRefreshAllowedCompleteResult)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	//this is an example of how a other interested parties can register for NotifyRefreshPCmd,
	//this would be any other but the main SAT engine.
	RSat satUser;
	CleanupClosePushL(satUser);
	satUser.Open(iPhone);
	
	//Expect a MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId when both
	//notifiers are set (thus needs to be queued before we setup the two notifiers below)
	TUint8 pcmdCode = KRefresh;
	TMockLtsyData1<TUint8> proactiveCmdNotificationsLtsyData(pcmdCode);
	proactiveCmdNotificationsLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
	
	//Refresh Required client side notifier
	//ETel -> CTSY
	RSat::TRefreshV2 refreshRequired;
	RSat::TRefreshV2Pckg refreshRequiredPckg(refreshRequired);
	TRequestStatus refreshRequiredReqStatus;
	iSat.NotifyRefreshRequired(refreshRequiredReqStatus,refreshRequiredPckg);
	
	//Refresh Proactive Command client side notifier 
	//ETel -> CTSY
	//Not both have been set, this will trigger a HandlePCmdNotificationReqL() down to the LTSY
	RSat::TRefreshV2 refreshPCmd;
	RSat::TRefreshV2Pckg refreshPCmdPckg(refreshPCmd);
	TRequestStatus refreshPCmdReqStatus;
	iSat.NotifyRefreshPCmd(refreshPCmdReqStatus,refreshPCmdPckg);
	
	RSat::TRefreshV2 refreshPCmd2;
	RSat::TRefreshV2Pckg refreshPCmdPckg2(refreshPCmd2);
	TRequestStatus refreshPCmdReqStatus2;
	satUser.NotifyRefreshPCmd(refreshPCmdReqStatus2,refreshPCmdPckg2);
	
	//the MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId expect should be gone
	//so assert the LTSY status
	AssertMockLtsyStatusL();
	
	
	TTlv proactiveCmdTlv;
	
	proactiveCmdTlv.Begin(KBerTlvProactiveSimCommandTag);

	proactiveCmdTlv.AddTag(KTlvCommandDetailsTag);
	proactiveCmdTlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	proactiveCmdTlv.AddByte(KRefresh); //ETLV_TypeOfCommand
	proactiveCmdTlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier
	
	proactiveCmdTlv.AddTag(KTlvDeviceIdentityTag);
	proactiveCmdTlv.AddByte(KSim);
	proactiveCmdTlv.AddByte(KMe);

	TUint numberOfFiles = 1;
	proactiveCmdTlv.AddTag(KTlvFileListTag);
	proactiveCmdTlv.AddByte(numberOfFiles);
	//full paths to files (first byte identifies the type of file)
	//master file
	
	const TInt KFilePathLength = 4;
	TBuf8<KFilePathLength> filePath(KFilePathLength);
	filePath[0] = 0x3F;
   	filePath[1] = 0x00;
   	filePath[2] = 0x2F;
   	filePath[3] = 0xE2;
	
   	proactiveCmdTlv.AddData(filePath);

   	_LIT8(KAid,"AID INFO"); //not correct format, just for test check "Application identifier as defined in TS 101 220 [31]"
	proactiveCmdTlv.AddTag(KTlvAIDTag);
   	proactiveCmdTlv.AddData(KAid);
   	
   	
	const TUint8 KTextAttributeTag = 0xC8;
	proactiveCmdTlv.AddTag(KTextAttributeTag);
	//text formatting
	proactiveCmdTlv.AddByte(0x00);
	proactiveCmdTlv.AddByte(0x00);
	proactiveCmdTlv.AddByte(0x00);
	proactiveCmdTlv.AddByte(0x00);

	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(proactiveCmdTlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data);
	data.Close();
	
	User::WaitForRequest(refreshRequiredReqStatus);
	ASSERT_EQUALS(refreshRequiredReqStatus.Int(),KErrNone);
	//ASSERT_EQUALS(refreshRequired.PCmdNumber(),aPCmdNumber); not set in the CTSY
	ASSERT_TRUE(refreshRequired.iAid == KAid);
	ASSERT_EQUALS(refreshRequired.iType,static_cast<RSat::TRefreshType>(aCommandQualifier + 1)); //+1 as RSat::TRefreshType 0x00 is ERefreshTypeNotSet
	ASSERT_EQUALS(filePath[2],static_cast<TUint8>(refreshRequired.iFileList[0] >> KShiftBitsToGetMSB));
	ASSERT_EQUALS(filePath[3],static_cast<TUint8>(refreshRequired.iFileList[0] & 0xFF));
	
	//no IPCs down to the LTSY should have occured, so we can AssertMockLtsyStatusL here.
	AssertMockLtsyStatusL();

	
	//ESatTsyServiceRequest
    TDesC8* aidPtr = const_cast<TDesC8*>(&KAid);
	TDesC8* refreshFileListPtr = &filePath;
	TUint16 cache = 0x01 | 0x02;//ECacheEFSST | ECacheEFCBMID;
	TMockLtsyData3<TDesC8*, TDesC8*, TUint16> refreshAllowedPack(refreshFileListPtr,aidPtr,cache);
	refreshAllowedPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,data,KErrNone);
	data.Close();

	RSat::TRefreshRspV1 refreshAllowedRsp;
	refreshAllowedRsp.iGeneralResult = RSat::KSuccess;
	refreshAllowedRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshAllowedRsp.iAdditionalInfo = KNullDesC;
	RSat::TRefreshRspV1Pckg refreshAllowedRspPckg(refreshAllowedRsp);
	TRequestStatus refreshAllowedReqStatus;
	iSat.RefreshAllowed(refreshAllowedReqStatus,refreshAllowedRspPckg);
	User::WaitForRequest(refreshAllowedReqStatus);
	ASSERT_EQUALS(refreshAllowedReqStatus.Int(),KErrNone);
	
	//the MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId expect should be gone
	//so assert the LTSY status
	AssertMockLtsyStatusL();
	
	
	
	//ESatTsyCompleteRefresh
	//indicator to create complete the notifier
	const TInt KResult = aRefreshAllowedCompleteResult;
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KResult);
	
	User::WaitForRequest(refreshPCmdReqStatus);
	ASSERT_EQUALS(refreshPCmdReqStatus.Int(),KResult);
	//ASSERT_EQUALS(refreshPCmd.PCmdNumber(),aPCmdNumber);  not set in the CTSY
	ASSERT_TRUE(refreshPCmd.iAid == KAid);
	ASSERT_EQUALS(refreshPCmd.iType,static_cast<RSat::TRefreshType>(aCommandQualifier + 1)); //+1 as RSat::TRefreshType 0x00 is ERefreshTypeNotSet
	ASSERT_EQUALS(filePath[2],static_cast<TUint8>(refreshPCmd.iFileList[0] >> KShiftBitsToGetMSB));
	ASSERT_EQUALS(filePath[3],static_cast<TUint8>(refreshPCmd.iFileList[0] & 0xFF));
	
	User::WaitForRequest(refreshPCmdReqStatus2);
	ASSERT_EQUALS(refreshPCmdReqStatus2.Int(),KResult);
	//ASSERT_EQUALS(refreshPCmd2.PCmdNumber(),aPCmdNumber);  not set in the CTSY
	ASSERT_TRUE(refreshPCmd2.iAid == KAid);
	ASSERT_EQUALS(refreshPCmd2.iType,static_cast<RSat::TRefreshType>(aCommandQualifier + 1)); //+1 as RSat::TRefreshType 0x00 is ERefreshTypeNotSet
	ASSERT_EQUALS(filePath[2],static_cast<TUint8>(refreshPCmd2.iFileList[0] >> KShiftBitsToGetMSB));
	ASSERT_EQUALS(filePath[3],static_cast<TUint8>(refreshPCmd2.iFileList[0] & 0xFF));

	CleanupStack::PopAndDestroy(&satUser);
	CleanupStack::PopAndDestroy(&data);
	}

TUint8 CCTsySatFUNegative::TonAndNpi(RMobilePhone::TMobileTON aTon, RMobilePhone::TMobileNPI aNpi)
	{
	return (0x80) | (aTon << 4) | (aNpi);
	}

void CCTsySatFUNegative::DoCleanup()
	{
	
	delete iCallContainer;
	iCallContainer = NULL;
	
	iSat.Close();
	CCtsyComponentTestBase::DoCleanup();
	}

CCTsySatFUNegative::CCallTypeContainer::CCallTypeContainer() :
	iCallControlPckg(iCallControl),
	iSetUpCallPckg(iSetUpCall)
	{
	
	}



