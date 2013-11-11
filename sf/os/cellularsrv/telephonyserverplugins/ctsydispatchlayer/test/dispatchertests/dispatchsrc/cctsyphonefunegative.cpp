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
 @file The TEFUnit test suite for CallControl in the Common TSY.
*/

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>

#include "config.h"
#include "listretrieverao.h"
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include "mockltsyindicatorids.h"
#include "cctsyphonefunegative.h"

CTestSuite* CCTsyPhoneFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0001L);

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0027L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0029L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0030L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFUNegative, TestUnit0031L);	
	
	END_SUITE;
	}
//
// 'Negative' unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetFdnStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetFdnStatus() (Sync and Async)
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
    	
    iPhone.GetFdnStatus(requestStatus, fdnStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    TInt err = iPhone.GetFdnStatus(fdnStatus);
    ASSERT_EQUALS(KErrNotSupported, err);    
	
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomNetWakeupIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NetWakeup()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0007L()
	{
	// 
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    customApi.NetWakeup(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomSetSystemNetworkModeIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::SetSystemNetworkMode() (Sync and Async)
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    RMmCustomAPI::TNetworkModeCaps mode = RMmCustomAPI::KCapsNetworkModeUmts;

    // Check sync version of the API
    TInt err = customApi.SetSystemNetworkMode(mode);
    ASSERT_EQUALS(KErrNotSupported, err);    
	
    // Check async version of the API
	customApi.SetSystemNetworkMode(requestStatus, mode);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetCurrentSystemNetworkModesIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::GetCurrentSystemNetworkModes() (Sync and Async)
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    TUint32 modes;

    // Check sync version of the API
    TInt err = customApi.GetCurrentSystemNetworkModes(modes);
    ASSERT_EQUALS(KErrNotSupported, err);    

    // Check async version of the API
	customApi.GetCurrentSystemNetworkModes(requestStatus, modes);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomResetNetServerIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::ResetNetServer()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0010L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	customApi.ResetNetServer(requestStatus);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomSetAlwaysOnMode is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::SetAlwaysOn()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    // Open a packet service, ctsy must have a packet service sub session in order to activate the SetAlwaysOn 
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
    RMmCustomAPI::TSetAlwaysOnMode expData = RMmCustomAPI::EAlwaysModeBoth;
	
	customApi.SetAlwaysOn(requestStatus, expData);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // customApi, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomSetDriveModeIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::SetDriveMode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0012L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
    customApi.SetDriveMode(requestStatus, RMmCustomAPI::EActivateDriveMode);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomReadHSxPAStatusIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::ReadHSxPAStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0013L()
	{

	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

    RMmCustomAPI::THSxPAStatus hspaStatus;
    
    customApi.ReadHSxPAStatus(requestStatus, hspaStatus);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomWriteHSxPAStatusIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::WriteHSxPAStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0014L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	RMmCustomAPI::THSxPAStatus hspaStatus = RMmCustomAPI::EHSxPAEnabled;
    
    customApi.WriteHSxPAStatus(requestStatus, hspaStatus);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetNetworkProviderNameIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::GetNetworkProviderName() (Sync and Async)
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0015L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
    RMobilePhone::TMobilePhoneNetworkLongName name;

    // Check sync version of the API
    TInt err = customApi.GetNetworkProviderName(name);
    ASSERT_EQUALS(KErrNotSupported, err);  

    // Check async version of the API
	customApi.GetNetworkProviderName(requestStatus, name);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetOperatorNameIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::GetOperatorName()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0016L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	RMmCustomAPI::TOperatorNameInfo getNameInfo;
	    
    customApi.GetOperatorName(requestStatus, getNameInfo);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0017
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetCellInfoIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes customApi::GetCellInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0017L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
    RMmCustomAPI::TMmCellInfo* getInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(getInfo);
    RMmCustomAPI::TMmCellInfoPckg *getInfoPckg = new(ELeave) RMmCustomAPI::TMmCellInfoPckg(*getInfo);
    CleanupStack::PushL(getInfoPckg);
    
    customApi.GetCellInfo(requestStatus, *getInfoPckg);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(4, this); //getInfoPckg, getInfo, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0018
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECtsyPhoneTerminateAllCallsReq is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes 
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0018L()
	{
	// 
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);
	
	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	
	ClientDialCallL(mobileLine, mobileCall, mobileService);

	TRequestStatus requestStatus;
	
	iPhone.TerminateAllCalls(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause, ETrue);
	
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 

	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	mobileCall.Close();
	mobileLine.Close();
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, mobileLine, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0019
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetServiceProviderName is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceProviderName()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0019L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneServiceProviderNameV2 spn;
	RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg spnPkg(spn);
	
	iPhone.GetServiceProviderName(requestStatus, spnPkg);
    User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0020
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetPhoneId is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneId()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0020L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 id;
	
	iPhone.GetPhoneId(requestStatus, id);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0021
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetDetectedNetworksV2Phase1 is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RetrieveMobilePhoneDetectedNetworks::StartV2()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0021L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	CGetDetectedNetworksAO* detAO = CGetDetectedNetworksAO::NewLC(iPhone,iMockLTSY);
	
	detAO->TestRetrieveNegativeL();
	ASSERT_EQUALS(KErrNotSupported, detAO->iStatus.Int());

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // detAO, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0022
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetDetectedNetworksCancel is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes CGetDetectedNetworksAO::Cancel()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0022L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	CGetDetectedNetworksAO* detAO = CGetDetectedNetworksAO::NewLC(iPhone,iMockLTSY);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId, KErrNone);

	detAO->TestCancel();
	ASSERT_EQUALS(KErrCancel, detAO->iStatus.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(detAO);
	CleanupStack::PopAndDestroy(this); // , this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0023
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSelectNetwork is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SelectNetwork()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0023L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneNetworkManualSelection selection;
	TBool isManual = ETrue;
	
	iPhone.SelectNetwork(requestStatus,isManual,selection);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0024
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSelectNetworkCancel is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::CancelAsyncRequest()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0024L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	//setup ETel required data
	_LIT(KCountryCode, "1234");
	_LIT(KNetworkId, "12345678");

	RMobilePhone::TMobilePhoneNetworkManualSelection selection;
	selection.iCountry = KCountryCode;
	selection.iNetwork = KNetworkId;
	
	TBool isManual = ETrue;
	
    TMockLtsyData2<TBool,RMobilePhone::TMobilePhoneNetworkManualSelection> selectData(isManual,selection);

    selectData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId, data, KErrNone);

	TRequestStatus requestStatus;
	iPhone.SelectNetwork(requestStatus,isManual,selection);
	
	iPhone.CancelAsyncRequest(EMobilePhoneSelectNetwork);
	User::WaitForRequest(requestStatus);	
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0025
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSetNetworkSelectionSetting is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetNetworkSelectionSetting()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0025L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneNetworkSelectionV1 phoneNetworkSelection;
	phoneNetworkSelection.iMethod 			= RMobilePhone::ENetworkSelectionManual;
	phoneNetworkSelection.iBandClass		= RMobilePhone::ENetworkBandClassAOnly;
	phoneNetworkSelection.iOperationMode	= RMobilePhone::ENetworkOperationAnalogPreferred;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg setPckg(phoneNetworkSelection);

	iPhone.SetNetworkSelectionSetting(requestStatus,setPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0026
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECtsyPhoneCellInfoIndReq is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCellInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0026L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneNetworkManualSelection selection;
	
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg cellInfoPckg(cellInfo);
	
	iPhone.GetCellInfo(requestStatus,cellInfoPckg);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0027
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECtsyPhoneCellInfoReq is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCellInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0027L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId);
	
	TRequestStatus getCellInfoReqStatus;
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo2;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg cellInfoPckg2(cellInfo2);
	
	// iLtsyDispatchPhoneGetPhoneCellInfo not NULL when disabling the IPC
	iPhone.GetCellInfo(getCellInfoReqStatus,cellInfoPckg2);

	User::WaitForRequest(getCellInfoReqStatus);
	ASSERT_EQUALS(getCellInfoReqStatus.Int(),KErrNotSupported);
		
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0028
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetServiceTableSupportbyApplicationIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetUSIMServiceSupport()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0028L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	TRequestStatus requestStatus;	
	RMmCustomAPI::TAppSupport usimSupport;

	customApi.GetUSIMServiceSupport(requestStatus, usimSupport);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0029
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetCurrentActiveUSimApplication is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCurrentActiveUSimApplication()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0029L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	TRequestStatus requestStatus;	
	RMobilePhone::TAID aid;

	iPhone.GetCurrentActiveUSimApplication(requestStatus, aid);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0030
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomGetBandSelectionIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSystemNetworkBand()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0030L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	TRequestStatus requestStatus;
	RMmCustomAPI::TBandSelection band;
	RMmCustomAPI::TNetworkModeCaps mode;

	customApi.GetSystemNetworkBand(requestStatus, band, mode);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-NEGATIVE-UN0031
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomSetBandSelectionIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkBand()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhoneFUNegative::TestUnit0031L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	TRequestStatus requestStatus;
	RMmCustomAPI::TBandSelection band=RMmCustomAPI::ENetworkBandAny;
	RMmCustomAPI::TNetworkModeCaps mode=RMmCustomAPI::KCapsNetworkModeUmts;

	customApi.SetSystemNetworkBand(requestStatus, band, mode);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}
