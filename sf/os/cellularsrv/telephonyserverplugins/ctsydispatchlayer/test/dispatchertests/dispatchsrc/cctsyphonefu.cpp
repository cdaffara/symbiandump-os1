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
 @file The TEFUnit test suite for PhoneControl in the Common TSY.
*/

#include "cctsyphonefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>
#include <etelmmerr.h>
#include "listretrieverao.h"
#include "mockltsyindicatorids.h"
#include <ctsy/rmmcustomapi.h>
#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>

#include "cctsyactiveretriever.h"

CTestSuite* CCTsyPhoneFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUseCase0004L);
  
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0027L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0029L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0030L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0031L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneFU, TestUnit0032L);

	END_SUITE;
	}

//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Use-case: boot phone.  
@SYMTestPriority High
@SYMTestActions Invokes APIs to go through booting phone.
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPhoneFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	//simple boot phone use-case is actually incorporated as the OpenPhoneL() function.
	OpenPhoneL();
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Use-case:  manual network detection and selection
@SYMTestPriority High
@SYMTestActions Invokes APIs to go through detecting and selecting a network to register onto.
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPhoneFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus status;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	
	/********************************Set network selection to manual***********************************/
	
	//set up the notified data
	RMobilePhone::TMobilePhoneNetworkSelectionV1 notifiedPhoneNetworkSelection;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg notifiedPckg(notifiedPhoneNetworkSelection);

	//set up the set data
	RMobilePhone::TMobilePhoneNetworkSelectionV1 phoneNetworkSelection;
	phoneNetworkSelection.iMethod 			= RMobilePhone::ENetworkSelectionManual;
	phoneNetworkSelection.iBandClass		= RMobilePhone::ENetworkBandClassAOnly;
	phoneNetworkSelection.iOperationMode	= RMobilePhone::ENetworkOperationAnalogPreferred;

	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg setPckg(phoneNetworkSelection);

	//set up the Mock LTSY data
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> phoneNetworkSelectionExpData(phoneNetworkSelection);
	phoneNetworkSelectionExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId, data, KErrNone);
	data.Close();
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSelectionMethod> phoneNetworkSelectionCompData(phoneNetworkSelection.iMethod);
	phoneNetworkSelectionCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId, KErrNone, data);
	data.Close();

	//ETel calls
	TRequestStatus notifyStatus;
	iPhone.NotifyNetworkSelectionSettingChange(notifyStatus,notifiedPckg);
	iPhone.SetNetworkSelectionSetting(status,setPckg);
	
	User::WaitForRequest(status);
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(status.Int(),KErrNone);
	ASSERT_EQUALS(notifyStatus.Int(),KErrNone);
	
	//CTSY sets iBandClass and iOperationMode to defults values
	ASSERT_EQUALS(phoneNetworkSelection.iMethod,notifiedPhoneNetworkSelection.iMethod);
	ASSERT_EQUALS(RMobilePhone::ENetworkBandClassUnknown,notifiedPhoneNetworkSelection.iBandClass);
	ASSERT_EQUALS(RMobilePhone::ENetworkOperationUnknown,notifiedPhoneNetworkSelection.iOperationMode);
	AssertMockLtsyStatusL();
	
	/*********************************************************************************************/
	
	/********************************Get Detected Network List************************************/
	
	CGetDetectedNetworksAO* detAO = CGetDetectedNetworksAO::NewLC(iPhone,iMockLTSY);
	
	// --- prepare data for CompleteL ---
 	CMobilePhoneNetworkListV2* networkListV2 = CMobilePhoneNetworkListV2::NewL();
	CleanupStack::PushL(networkListV2);
	
 	RMobilePhone::TMobilePhoneNetworkInfoV2 infoV2;
	
	infoV2.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	infoV2.iMode        = RMobilePhone::ENetworkModeUnknown;
	infoV2.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	infoV2.iBandInfo    = RMobilePhone::EBandUnknown;	
	
	_LIT(KCountryCode, "1234");
	infoV2.iCountryCode = KCountryCode;

	_LIT(KAnalogSID, "12345678");
	infoV2.iAnalogSID   = KAnalogSID;

	_LIT(KNetworkId, "12345678");
	infoV2.iNetworkId   = KNetworkId;
	
	_LIT(KDisplayTag, "1234567890");
	infoV2.iDisplayTag  = KDisplayTag;
	
	_LIT(KShortName, "1234567890");
	infoV2.iShortName   = KShortName;

	_LIT(KLongName, "12345678901234567890");
	infoV2.iLongName    = KLongName;
	
	networkListV2->AddEntryL(infoV2);

	//set up Mock LTSY expect and completes for a retrieve (not cancelled)
    iMockLTSY.ExpectL(MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId, KErrNone);

    //execute the retrieve
	detAO->TestRetrieveL(*networkListV2);

	//check what has been returned 
	ASSERT_EQUALS(detAO->CheckResultsL(),KErrNone);
	
	AssertMockLtsyStatusL();
	
	/*********************************************************************************************/

	/********************************Manual Network Selection************************************/

	//setup ETel required data
	RMobilePhone::TMobilePhoneNetworkManualSelection selection;
	selection.iCountry = KCountryCode;
	selection.iNetwork = KNetworkId;

	//setup MockTSY data
	TBool isManual = ETrue;
	TMockLtsyData2<TBool,RMobilePhone::TMobilePhoneNetworkManualSelection> selectData(isManual,selection);
    selectData.SerialiseL(data);
	
	//set up Mock LTSY expect and completes for a retrieve (not cancelled)
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId, KErrNone);
	data.Close();
	
	//ETel calls
	iPhone.SelectNetwork(status,isManual,selection);
	User::WaitForRequest(status);

	AssertMockLtsyStatusL();
	/*********************************************************************************************/
	
	CleanupStack::PopAndDestroy(networkListV2);
	CleanupStack::PopAndDestroy(detAO);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Use-case: Network notifications at boot-up. 
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::NotifyModeChange, RMobilePhone::NotifyNITZInfoChange, RMobilePhone::NotifySignalStrengthChange  
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPhoneFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus1, requestStatus2, requestStatus3, requestStatus4, requestStatus5;
	TRequestStatus mockLtsyStatus;
	
	//
	// Post all the notifiers.
	//
	
	// RMobilePhone::NotifyCurrentNetworkChange
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoRet;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5> networkInfoRetPckg(networkInfoRet);
	RMobilePhone::TMobilePhoneLocationAreaV1 locationAreaRet;
	iPhone.NotifyCurrentNetworkChange(requestStatus1, networkInfoRetPckg, locationAreaRet);
	
	// RMobilePhone::NotifyNetworkModeChange
	RMobilePhone::TMobilePhoneNetworkMode networkModeRet;
	iPhone.NotifyModeChange(requestStatus2, networkModeRet);
	
	// RMobilePhone::NotifyNitzInfoChange
	RMobilePhone::TMobilePhoneNITZ nitzRet;
	iPhone.NotifyNITZInfoChange(requestStatus3, nitzRet);
	
	// RMobilePhone::NotifySignalStrengthChange
	TInt32 signalStrength(0);
	TInt8 bar(0);
	iPhone.NotifySignalStrengthChange(requestStatus4, signalStrength, bar);
	
	// RMobilePhone::NotifyBatteryInfoChange
	RMobilePhone::TMobilePhoneBatteryInfoV1 battInfoRet;
	iPhone.NotifyBatteryInfoChange(requestStatus5, battInfoRet);
	
	/*
	 * RMobilePhone::NotifyCurrentNetworkChange
	 */

	// Prepare the NotifyCurrentNetworkChange data for CompleteL
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoComplete;
	
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("567");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("890");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("123");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("56700");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian2 mobile");

	// V5
	networkInfoComplete.iEgprsAvailableIndicator = ETrue;
	networkInfoComplete.iHsdpaAvailableIndicator = EFalse;
	// V2
	networkInfoComplete.iAccess = RMobilePhone::ENetworkAccessGsm;
	// V1
	networkInfoComplete.iMode        = RMobilePhone::ENetworkModeCdma2000   ;
	networkInfoComplete.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoComplete.iBandInfo    = RMobilePhone::E1900BandA             ;
	networkInfoComplete.iCountryCode = countryCode;
	networkInfoComplete.iCdmaSID     = cdmaSID    ;
	networkInfoComplete.iAnalogSID   = analogSID  ;
	networkInfoComplete.iNetworkId   = networkId  ;
	networkInfoComplete.iDisplayTag  = displayTag ;
	networkInfoComplete.iShortName   = shortName  ;
	networkInfoComplete.iLongName    = longName   ;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 locationAreaComplete;
	locationAreaComplete.iAreaKnown = ETrue;
	locationAreaComplete.iLocationAreaCode = KLocationAreaCode;
	locationAreaComplete.iCellId = KCellId;
	
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> mockCNCData2(networkInfoComplete, locationAreaComplete);
    
	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockCNCData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    // Check returned data
    ASSERT_EQUALS(networkInfoRet.iMode, networkInfoComplete.iMode);
    ASSERT_EQUALS(networkInfoRet.iStatus, networkInfoComplete.iStatus);
    ASSERT_EQUALS(networkInfoRet.iBandInfo, networkInfoComplete.iBandInfo);
    ASSERT_EQUALS(networkInfoRet.iCountryCode, networkInfoComplete.iCountryCode);
    ASSERT_EQUALS(networkInfoRet.iCdmaSID, networkInfoComplete.iCdmaSID);
    ASSERT_EQUALS(networkInfoRet.iAnalogSID, networkInfoComplete.iAnalogSID);
    ASSERT_EQUALS(networkInfoRet.iNetworkId, networkInfoComplete.iNetworkId);
    ASSERT_EQUALS(networkInfoComplete.iDisplayTag, networkInfoComplete.iDisplayTag);
    ASSERT_EQUALS(networkInfoComplete.iShortName, networkInfoComplete.iShortName);
    ASSERT_EQUALS(networkInfoComplete.iLongName, networkInfoComplete.iLongName);

    ASSERT_EQUALS(locationAreaRet.iAreaKnown, locationAreaComplete.iAreaKnown);
    ASSERT_EQUALS(locationAreaRet.iLocationAreaCode, locationAreaComplete.iLocationAreaCode);
    ASSERT_EQUALS(locationAreaRet.iCellId, locationAreaComplete.iCellId);
    
	/*
	 * RMobilePhone::NotifyNetworkModeChange
	 */
    
	// Prepare the NotifyNetworkModeChange data for CompleteL
    RMobilePhone::TMobilePhoneNetworkMode networkModeComplete;
    
    networkModeComplete = RMobilePhone::ENetworkModeCdma95;

    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkMode> mockNMCData1(networkModeComplete);
    
	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockNMCData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNetworkModeChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    // Check returned data
    ASSERT_EQUALS(networkModeRet, networkModeComplete);
    
	/*
	 * RMobilePhone::NotifyNitzInfoChange
	 */
    
    // Prepare the NotifyNetworkModeChange data for CompleteL
    RMobilePhone::TMobilePhoneNITZ nitzComplete(2007, EOctober, 18, 16,  0,  0, 0);
    
	RMobilePhone::TMobilePhoneNetworkShortName mobilePhoneNetworkShortName = _L("ShortName");
	RMobilePhone::TMobilePhoneNetworkLongName mobilePhoneNetworkLongName = _L("MyLongName");
	TInt32 nitzFieldsUsed = RMobilePhone::KCapsTimeAvailable |
							RMobilePhone::KCapsTimezoneAvailable |
							RMobilePhone::KCapsDSTAvailable |
							RMobilePhone::KCapsShortNameAvailable |
							RMobilePhone::KCapsLongNameAvailable;
	
	TInt timeZone = 10;
	TInt DST = 5;
	
	nitzComplete.iShortNetworkId = mobilePhoneNetworkShortName;
	nitzComplete.iLongNetworkId = mobilePhoneNetworkLongName;
	nitzComplete.iNitzFieldsUsed = nitzFieldsUsed;
	nitzComplete.iTimeZone = timeZone;
	nitzComplete.iDST = DST;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNITZ> mockNitzData1(nitzComplete);
	
	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockNitzData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNitzInfoChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus3);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus3.Int());
    
    // Check returned data
    ASSERT_EQUALS(nitzRet.iNitzFieldsUsed, nitzComplete.iNitzFieldsUsed);
    ASSERT_EQUALS(nitzRet.iTimeZone, nitzComplete.iTimeZone);
    ASSERT_EQUALS(nitzRet.iDST, nitzComplete.iDST);
    // Note: CTSY will overwrite the iShortNetworkId and iLongNetworkId fields in the NITZ data
    // if it all ready holds values for them from previous notifications (such as a NotifyCurrentNetworkChange).
    // We therefore compare with values from a TMobilePhoneNetworkInfoV5 which was received earlier in the test.
    ASSERT_EQUALS(nitzRet.iShortNetworkId, networkInfoComplete.iShortName);
    ASSERT_EQUALS(nitzRet.iLongNetworkId, networkInfoComplete.iLongName);
    
	/*
	 * RMobilePhone::NotifySignalStrengthChange
	 */
    
    // Prepare the NotifySignalStrengthChange data for CompleteL
	TInt32 signalStrengthComplete(70);
	TInt8 barComplete(3);
	
	TMockLtsyData2<TInt32, TInt8> mockSigSthData2(signalStrengthComplete, barComplete);
	
	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockSigSthData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifySignalStrengthChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus4);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus4.Int());
    
    // Check returned data
    ASSERT_EQUALS(signalStrength, signalStrengthComplete);
    ASSERT_EQUALS(bar, barComplete);
    
	/*
	 * RMobilePhone::NotifyBatteryInfoChange
	 */
    
    // Prepare the NotifyBatteryInfoChange data for CompleteL
    RMobilePhone::TMobilePhoneBatteryInfoV1 battInfoComplete;
    battInfoComplete.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
    battInfoComplete.iChargeLevel = 99;
    
    TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> mockBattData1(battInfoComplete);

	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockBattData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyBatteryInfoChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus5);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus5.Int());
    
    // Check returned data
    ASSERT_EQUALS(battInfoRet.iStatus, battInfoComplete.iStatus);
    ASSERT_EQUALS(battInfoRet.iChargeLevel, battInfoComplete.iChargeLevel);
    
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Use-case: Test for retrieval of network information without having to post notifiers. 
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentNetwork, RMobilePhone::GetNITZInfo, RMobilePhone::GetSignalStrength, RMobilePhone::GetCurrentMode
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPhoneFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatusGetCurrentNetwork;
	TRequestStatus reqStatusGetSigStr;
	TRequestStatus reqStatusGetBattInfo;
	TInt ret;
	
	//
	// Invoke get api's 
	//
	
	// RMobilePhone::GetCurrentNetwork
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV5Pckg networkInfoPckg(networkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
	iPhone.GetCurrentNetwork(reqStatusGetCurrentNetwork, networkInfoPckg, locationArea);
	
	// RMobilePhone::GetNITZInfo
	RMobilePhone::TMobilePhoneNITZ nitzRet;
	ret = iPhone.GetNITZInfo(nitzRet);
	ASSERT_EQUALS(ret, KErrNone);

	// RMobilePhone::GetSignalStrength
	TInt32 signalStrength(0);
	TInt8 bar(0);
	iPhone.GetSignalStrength(reqStatusGetSigStr, signalStrength, bar);
	
	// RMobilePhone::GetCurrentMode
	RMobilePhone::TMobilePhoneNetworkMode mode;
	ret = iPhone.GetCurrentMode(mode);
	ASSERT_EQUALS(ret, KErrNone);
	
	// RMobilePhone::GetBatteryInfo
	RMobilePhone::TMobilePhoneBatteryInfoV1 battInfo;
	iPhone.GetBatteryInfo(reqStatusGetBattInfo, battInfo);

	//
	// Check returned values
	//
	
	// Check RMobilePhone::GetCurrentMode returned data against the data that was set up in CCtsyComponentTestBase::OpenPhoneL()
	ASSERT_EQUALS(mode, KNetworkMode);  // Note mode is given different value to that retrieved from GetCurrentNetwork for sake of the test.
	
	// Check RMobilePhone::GetCurrentNetwork returned data against the data that was set up in CCtsyComponentTestBase::OpenPhoneL()
	User::WaitForRequest(reqStatusGetCurrentNetwork);
	ASSERT_EQUALS(reqStatusGetCurrentNetwork.Int(), KErrNone);
	ASSERT_EQUALS(networkInfo.iEgprsAvailableIndicator, KEgprsAvailable);
	ASSERT_EQUALS(networkInfo.iHsdpaAvailableIndicator, KHsdpaAvailable);
	ASSERT_EQUALS(networkInfo.iAccess, KAccess);
	ASSERT_EQUALS(networkInfo.iMode, KMode);
	ASSERT_EQUALS(networkInfo.iStatus, KStatus);
	ASSERT_EQUALS(networkInfo.iBandInfo, KBandInfo);
	ASSERT_EQUALS(networkInfo.iCountryCode, KCountryCode);
	ASSERT_EQUALS(networkInfo.iCdmaSID, KCdmaSID);
	ASSERT_EQUALS(networkInfo.iAnalogSID, KAnalogSID);
	ASSERT_EQUALS(networkInfo.iNetworkId, KNetworkId);
	ASSERT_EQUALS(networkInfo.iDisplayTag, KDisplayTag);
	ASSERT_EQUALS(networkInfo.iShortName, KShortName);
	ASSERT_EQUALS(networkInfo.iLongName, KLongName);
	
	ASSERT_EQUALS(locationArea.iAreaKnown, KAreaKnown);
	ASSERT_EQUALS(locationArea.iLocationAreaCode, KLocationAreaCode);
	ASSERT_EQUALS(locationArea.iCellId, KCellId);
	
	//Do tests for TMobilePhoneNetworkInfoV1 and TMobilePhoneNetworkInfoV2 ?
	
	// Check RMobilePhone::GetNITZInfo returned data against the data that was set up in CCtsyComponentTestBase::OpenPhoneL()
	ASSERT_EQUALS(nitzRet.iShortNetworkId, KShortName);
	ASSERT_EQUALS(nitzRet.iLongNetworkId, KLongName);
	ASSERT_EQUALS(nitzRet.iNitzFieldsUsed, KNitzFieldsUsed);
	ASSERT_EQUALS(nitzRet.iTimeZone, KTimeZone);
	ASSERT_EQUALS(nitzRet.iDST, KDst);
	
	// Check RMobilePhone::GetSignalStrength returned data against the data that was set up in CCtsyComponentTestBase::OpenPhoneL()
	// RMobilePhone::GetSignalStrength
	User::WaitForRequest(reqStatusGetSigStr);
	ASSERT_EQUALS(reqStatusGetSigStr.Int(), KErrNone);
	ASSERT_EQUALS(signalStrength, KSignalStrength);
	ASSERT_EQUALS(bar, KBar);
	
	// Check RMobilePhone::GetBatteryInfo returned data against the data that was set up in CCtsyComponentTestBase::OpenPhoneL()
	User::WaitForRequest(reqStatusGetBattInfo);
	ASSERT_EQUALS(reqStatusGetBattInfo.Int(), KErrNone);
	ASSERT_EQUALS(battInfo.iStatus, KBatteryStatus); 
	ASSERT_EQUALS(battInfo.iChargeLevel, KBatteryChargeLevel);
	
	data.Close();
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}



//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for receiving indication of RF status change.
@SYMTestPriority High
@SYMTestActions Invokes xxxx
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
		
	TRfStateInfo rfstatus = ERfsStateInfoInactive;
	TMockLtsyData1<TRfStateInfo> rfstatdata(rfstatus);
	rfstatdata.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyRfStatusChangeIndId,KErrNone, data, 0);
	
	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for retrieving the Phone Identity
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneId
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	

	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KManufacturer, "Manufacturer XYZ");
	_LIT(KModel, "Model XYZ");	
	_LIT(KRevision, "Revision abc");
	_LIT(KSerialNumber, "SerialNumber 1234");
	//_LIT(KSerialNumberShort15, "SNum 1234");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iManufacturer = KManufacturer;
	phoneId.iModel = KModel;
	phoneId.iRevision = KRevision;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data);
	
	// IPC call
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdentityV1 id;
	iPhone.GetPhoneId(reqStatus, id);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	ASSERT_EQUALS(phoneId.iManufacturer, id.iManufacturer);
	ASSERT_EQUALS(phoneId.iModel, id.iModel);
	ASSERT_EQUALS(phoneId.iRevision, id.iRevision);
	ASSERT_EQUALS(phoneId.iSerialNumber, id.iSerialNumber);

	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for retrieving the service provider name
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceProviderName
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	

	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId);
	
	_LIT(KPLMNField, "plmn123");
	_LIT(KSPName, "spn123");
		
	RMobilePhone::TMobilePhoneServiceProviderNameV2 spnExp;
	spnExp.iDisplayReq = RMobilePhone::KDisplayPLMNRequired | RMobilePhone::KDisplaySPNRequired;
	spnExp.iPLMNField  = KPLMNField;
	spnExp.iSPName = KSPName;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceProviderNameV2> spnData(spnExp);
	data.Close();
	spnData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId, KErrNone, data);
	
	// IPC call
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceProviderNameV2 spn;
	RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg spnPkg(spn);
	
	iPhone.GetServiceProviderName(reqStatus, spnPkg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());	
	ASSERT_EQUALS(spn.iDisplayReq, spnExp.iDisplayReq);
	ASSERT_EQUALS(spn.iPLMNField, spnExp.iPLMNField);
	ASSERT_EQUALS(spn.iSPName, spnExp.iSPName);
		
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling a Get detected network list
@SYMTestPriority High
@SYMTestActions Invokes CGetDetectedNetworksAO::Cancel
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CGetDetectedNetworksAO* detAO = CGetDetectedNetworksAO::NewLC(iPhone,iMockLTSY);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId, KErrNone);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId, KErrNone);

	detAO->TestCancel();
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(detAO);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling for a manual network selection
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::CancelAsyncRequest(EMobilePhoneSelectNetwork)
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0005L()
	{
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
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId, KErrNone);

	TRequestStatus status;
	iPhone.SelectNetwork(status,isManual,selection);
	iPhone.CancelAsyncRequest(EMobilePhoneSelectNetwork);
	User::WaitForRequest(status);	
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY network selection change notification
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//set up the set data
	RMobilePhone::TMobilePhoneSelectionMethod phoneNetworkSelection;
	phoneNetworkSelection = RMobilePhone::ENetworkSelectionManual;

	//set up the Mock LTSY data
	TMockLtsyData1<RMobilePhone::TMobilePhoneSelectionMethod> phoneNetworkSelectionData(phoneNetworkSelection);
	phoneNetworkSelectionData.SerialiseL(data);
	//set up Mock LTSY expect and completes 
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNetworkSelectionSettingChangeIndId, KErrNone, data);
	
	//ETel calls
	TRequestStatus status;
	//set up the notified data
	RMobilePhone::TMobilePhoneNetworkSelectionV1 notifiedPhoneNetworkSelection;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg notifiedPckg(notifiedPhoneNetworkSelection);
	iPhone.NotifyNetworkSelectionSettingChange(status,notifiedPckg);

	User::WaitForRequest(status);
	
	//CTSY sets iBandClass and iOperationMode to defults values
	ASSERT_EQUALS(phoneNetworkSelection,notifiedPhoneNetworkSelection.iMethod);
	ASSERT_EQUALS(RMobilePhone::ENetworkBandClassUnknown,notifiedPhoneNetworkSelection.iBandClass);
	ASSERT_EQUALS(RMobilePhone::ENetworkOperationUnknown,notifiedPhoneNetworkSelection.iOperationMode);
	AssertMockLtsyStatusL();
	
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support for battery information change notification.
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyBatteryInfoChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;
	
	// Post notifier RMobilePhone::NotifyBatteryInfoChange
	RMobilePhone::TMobilePhoneBatteryInfoV1 battInfoRet;
	iPhone.NotifyBatteryInfoChange(requestStatus, battInfoRet);
	
    // Prepare the NotifyBatteryInfoChange data for CompleteL
    RMobilePhone::TMobilePhoneBatteryInfoV1 battInfoComplete;
    battInfoComplete.iStatus = RMobilePhone::EBatteryConnectedButExternallyPowered;
    battInfoComplete.iChargeLevel = 99;
    
    TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> mockBattData1(battInfoComplete);

	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockBattData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyBatteryInfoChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    // Check returned data
    ASSERT_EQUALS(battInfoRet.iStatus, battInfoComplete.iStatus);
    ASSERT_EQUALS(battInfoRet.iChargeLevel, battInfoComplete.iChargeLevel);
	
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support for retrieval battery information without having to post a notification.
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetBatteryInfo
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	RMobilePhone::TMobilePhoneBatteryInfoV1 battInfo;
	iPhone.GetBatteryInfo(reqStatus, battInfo);
	
	// Check RMobilePhone::GetBatteryInfo returned data against the data that was set up in CCtsyComponentTestBase::OpenPhoneL()
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(battInfo.iStatus, KBatteryStatus); 
	ASSERT_EQUALS(battInfo.iChargeLevel, KBatteryChargeLevel);
	
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support for network registration change notification.
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;
	
	// Post notifier RMobilePhone::NotifyNetworkRegistrationStatusChange
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	iPhone.NotifyNetworkRegistrationStatusChange(requestStatus, regStatus);
	
    // Prepare the NotifyNetworkRegistrationStatusChange data for CompleteL
	// The Ctsy expects RMobilePhone::TMobilePhoneRegistrationStatus
	RMobilePhone::TMobilePhoneRegistrationStatus regStatusComplete;
	// Note: the registration status is made different to that set during phone boot-up intentionally
	regStatusComplete = RMobilePhone::ERegisteredRoaming;
    
    TMockLtsyData1<RMobilePhone::TMobilePhoneRegistrationStatus> mockNetRegData1(regStatusComplete);

	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
	mockNetRegData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    // Check completion of api
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    // Check returned data
    ASSERT_EQUALS(regStatus, regStatusComplete);
	
	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of NSPS (No Signal, Power Save) notification and waking
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyNSPSStatus and RMmCustomAPI::NetWakeup
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RMmCustomAPI::TNspsStatus nspsStatus;

	TBool nspsEnabled = ETrue;
	TMockLtsyData1<TBool> nspsStatusData(nspsEnabled);

	
	customApi.NotifyNSPSStatus(reqStatus,nspsStatus);
	
	data.Close();
	nspsStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNspsStatusChangeIndId, KErrNone, data);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(RMmCustomAPI::ENspsOn, nspsStatus);

	AssertMockLtsyStatusL();
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId, KErrNone);
    customApi.NetWakeup(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	customApi.NotifyNSPSStatus(reqStatus,nspsStatus);
	nspsEnabled = EFalse;
	data.Close();
	nspsStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNspsStatusChangeIndId, KErrNone, data);
	data.Close();
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(RMmCustomAPI::ENspsOff, nspsStatus);

	
	AssertMockLtsyStatusL();
		
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of Edge Gprs notification
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	// custom API info
	RMmCustomAPI::TGprsInformation gprsInformation;
	RMmCustomAPI::TGprsInformationPckg gprsInformationPckg(gprsInformation);
	

	// test the case when Edge is not supported
	TBool edgeSupported = EFalse;
	TMockLtsyData1<TBool> egprsInfoData(edgeSupported);
	egprsInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId, KErrNone, data);
	data.Close();
		
	customApi.NotifyEGprsInfoChange(reqStatus,gprsInformationPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(gprsInformation.iGprsInfo,RMmCustomAPI::EGprs);
	AssertMockLtsyStatusL();
	
	// test the case when Edge is supported
	edgeSupported = ETrue;
	egprsInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId, KErrNone, data);
	data.Close();

	customApi.NotifyEGprsInfoChange(reqStatus,gprsInformationPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(gprsInformation.iGprsInfo ,RMmCustomAPI::EEdgeGprs);
	AssertMockLtsyStatusL();

	
	// test the case when Edge is supported and KErrGeneral returned, (error returned not used in the CTSY)
	edgeSupported = ETrue;
	egprsInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId, KErrGeneral, data);
	data.Close();

	customApi.NotifyEGprsInfoChange(reqStatus,gprsInformationPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(gprsInformation.iGprsInfo ,RMmCustomAPI::EEdgeGprs);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&packetService);
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of Getting the Fdn Status
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetFdnStatus() (Sync and Async)
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	OpenPhoneL();
	
	//test the case where the LTSY returns KErrNone - asynchronous call
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId);

	RMobilePhone::TMobilePhoneFdnStatus compFdnStatus = RMobilePhone::EFdnNotActive;
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnStatus> fdnStatusData(compFdnStatus);
	fdnStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId,KErrNone,data);
	data.Close();
	
	RMobilePhone::TMobilePhoneFdnStatus fdnStatus = RMobilePhone::EFdnUnknown;
	iPhone.GetFdnStatus(reqStatus,fdnStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(compFdnStatus,fdnStatus);
	AssertMockLtsyStatusL();

	//test the case where the LTSY returns KErrGeneral - asynchronous call
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId);

	compFdnStatus = RMobilePhone::EFdnNotActive;
	fdnStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId,KErrGeneral,data);
	data.Close();

	fdnStatus = RMobilePhone::EFdnUnknown;
	iPhone.GetFdnStatus(reqStatus,fdnStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);
	AssertMockLtsyStatusL();

	//test the case where the LTSY returns KErrNone - synchronous call
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId);

	compFdnStatus = RMobilePhone::EFdnNotActive;
	fdnStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId,KErrNone,data);
	data.Close();
	
	fdnStatus = RMobilePhone::EFdnUnknown;
	ASSERT_EQUALS(iPhone.GetFdnStatus(fdnStatus), KErrNone);
	ASSERT_EQUALS(compFdnStatus,fdnStatus);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of Set Network Selection Setting error case
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkSelectionSettingChange() RMobilePhone::SetNetworkSelectionSetting()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	//set up the notified data
	RMobilePhone::TMobilePhoneNetworkSelectionV1 notifiedPhoneNetworkSelection;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg notifiedPckg(notifiedPhoneNetworkSelection);

	//set up the set data
	RMobilePhone::TMobilePhoneNetworkSelectionV1 phoneNetworkSelection;
	phoneNetworkSelection.iMethod 			= RMobilePhone::ENetworkSelectionManual;
	phoneNetworkSelection.iBandClass		= RMobilePhone::ENetworkBandClassAOnly;
	phoneNetworkSelection.iOperationMode	= RMobilePhone::ENetworkOperationAnalogPreferred;

	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg setPckg(phoneNetworkSelection);

	//set up the Mock LTSY data
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> phoneNetworkSelectionExpData(phoneNetworkSelection);
	phoneNetworkSelectionExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId, data, KErrNone);
	data.Close();
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneSelectionMethod> phoneNetworkSelectionCompData(phoneNetworkSelection.iMethod);
	phoneNetworkSelectionCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId, KErrGeneral, data);
	data.Close();

	//ETel calls
	TRequestStatus notifyStatus;
	iPhone.NotifyNetworkSelectionSettingChange(notifyStatus,notifiedPckg);
	
	TRequestStatus setNetworkSelectionStatus;
	iPhone.SetNetworkSelectionSetting(setNetworkSelectionStatus,setPckg);
	
	User::WaitForRequest(setNetworkSelectionStatus);
	ASSERT_EQUALS(setNetworkSelectionStatus.Int(),KErrGeneral);
	ASSERT_EQUALS(notifyStatus.Int(),KRequestPending);
		
	//CTSY sets iBandClass and iOperationMode to defults values
	ASSERT_EQUALS(RMobilePhone::ENetworkSelectionUnknown,notifiedPhoneNetworkSelection.iMethod);
	ASSERT_EQUALS(RMobilePhone::ENetworkBandClassUnknown,notifiedPhoneNetworkSelection.iBandClass);
	ASSERT_EQUALS(RMobilePhone::ENetworkOperationUnknown,notifiedPhoneNetworkSelection.iOperationMode);
	AssertMockLtsyStatusL();

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange);
	ASSERT_EQUALS(notifyStatus.Int(),KErrCancel);
		
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    customApi.NotifyNetworkConnectionFailure(requestStatus);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyNetworkConnectionFailureIndId, KErrNone);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::NotifyDtmfEvent
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyDtmfEvent
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0015L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    RMmCustomAPI::TDtmfInfo info;
    customApi.NotifyDtmfEvent(requestStatus, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    
    RMmCustomAPI::TDtmfEventType compType = RMmCustomAPI::EDtmfSequence;
    RMmCustomAPI::TDtmfEvent compEvent = RMmCustomAPI::EDtmfStop;
    TChar compChar = '0';
	TMockLtsyData3<RMmCustomAPI::TDtmfEventType, RMmCustomAPI::TDtmfEvent, TChar>
        modesData(compType, compEvent, compChar);
	data.Close();
	modesData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchCustomPhoneNotifyDtmfEventIndId, KErrNone, data);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(compType, info.iType);
    ASSERT_EQUALS(compEvent, info.iEvent);
    ASSERT_EQUALS(compChar, info.iTone);

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::ResetNetServer
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ResetNetServer
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0016L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	iMockLTSY.ExpectL(MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId, KErrNone);

    TRequestStatus requestStatus;
    customApi.ResetNetServer(requestStatus);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
	iMockLTSY.CompleteL(MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId, KErrNone);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0017
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::SetAlwaysOn
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetAlwaysOn
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0017L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	RBuf8 data;
	CleanupClosePushL(data);

    // Open a packet service, ctsy must have a packet service sub session in order to activate the SetAlwaysOn 
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

    RMmCustomAPI::TSetAlwaysOnMode expData = RMmCustomAPI::EAlwaysModeBoth;
	TMockLtsyData1<RMmCustomAPI::TSetAlwaysOnMode> modesData(expData);
	data.Close();
	modesData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId, data, KErrNone);

    TRequestStatus requestStatus;
    customApi.SetAlwaysOn(requestStatus, expData);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId, KErrNone);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(4, this); // packetService, data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0018
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::SetDriveMode
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetDriveMode
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0018L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	RBuf8 data;
	CleanupClosePushL(data);

    RMmCustomAPI::TSetDriveMode expData = RMmCustomAPI::EActivateDriveMode;
	TMockLtsyData1<RMmCustomAPI::TSetDriveMode> modesData(expData);
	data.Close();
	modesData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId, data, KErrNone);

    TRequestStatus requestStatus;
    customApi.SetDriveMode(requestStatus, RMmCustomAPI::EActivateDriveMode);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId, KErrNone);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0019
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::ReadHSxPAStatus, RMmCustomAPI::WriteHSxPAStatus and RMmCustomAPI::NotifyHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadHSxPAStatus, RMmCustomAPI::WriteHSxPAStatus and RMmCustomAPI::NotifyHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0019L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	RBuf8 data;
	CleanupClosePushL(data);

    // Register Notification
    RMmCustomAPI::THSxPAStatus notifyHspaStatus;
    TRequestStatus notifyRequestStatus;
    customApi.NotifyHSxPAStatus(notifyRequestStatus, notifyHspaStatus);
    ASSERT_EQUALS(KRequestPending, notifyRequestStatus.Int());

    // Expect for setter
    RMmCustomAPI::THSxPAStatus expSetHspaStatus = RMmCustomAPI::EHSxPAEnabled;
	TMockLtsyData1<RMmCustomAPI::THSxPAStatus> expSetData(expSetHspaStatus);
	data.Close();
	expSetData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId, data, KErrNone);

    // Set the data
    TRequestStatus setRequestStatus;
    RMmCustomAPI::THSxPAStatus setHspaStatus = expSetHspaStatus;
    customApi.WriteHSxPAStatus(setRequestStatus, setHspaStatus);
	ASSERT_EQUALS(KRequestPending, setRequestStatus.Int());

    // Complete the set
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId, KErrNone);
	User::WaitForRequest(setRequestStatus);
	ASSERT_EQUALS(KErrNone, setRequestStatus.Int());

    // Expect for getter
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId, KErrNone);

    // get the data
    TRequestStatus getRequestStatus;
    RMmCustomAPI::THSxPAStatus getHspaStatus;
    customApi.ReadHSxPAStatus(getRequestStatus, getHspaStatus);
	ASSERT_EQUALS(KRequestPending, getRequestStatus.Int());

    // Complete the get
    RMmCustomAPI::THSxPAStatus compGetHspaStatus = RMmCustomAPI::EHSxPAEnabled;
	TMockLtsyData1<RMmCustomAPI::THSxPAStatus> compGetData(compGetHspaStatus);
	data.Close();
	compGetData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId, KErrNone, data);
	User::WaitForRequest(getRequestStatus);
	ASSERT_EQUALS(KErrNone, getRequestStatus.Int());
    ASSERT_EQUALS(compGetHspaStatus, getHspaStatus);

    // notify a change
    RMmCustomAPI::THSxPAStatus compNotifyHspaStatus = RMmCustomAPI::EHSxPADisabled;
	TMockLtsyData1<RMmCustomAPI::THSxPAStatus> compNotiftData(compNotifyHspaStatus);
	data.Close();
	compNotiftData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyHspaStatusChangedIndId, KErrNone, data);
	User::WaitForRequest(notifyRequestStatus);
	ASSERT_EQUALS(KErrNone, notifyRequestStatus.Int());
    ASSERT_EQUALS(compNotifyHspaStatus, notifyHspaStatus);
    
    
	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0020
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::GetNetworkProviderName
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetNetworkProviderName
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0020L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 data;
	CleanupClosePushL(data);

    // Checking the synch API version
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId, KErrNone);
	_LIT(KNetworkProviderName, "Provider Name");
    RMobilePhone::TMobilePhoneNetworkLongName completeData(KNetworkProviderName);
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkLongName> nameData(completeData);
	data.Close();
	nameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId, KErrNone, data);
    RMobilePhone::TMobilePhoneNetworkLongName name;
	ASSERT_EQUALS(KErrNone, customApi.GetNetworkProviderName(name));
    ASSERT_EQUALS(name, completeData);

    // Checking the asynch API version
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId, KErrNone);
	_LIT(KNetworkProviderName2, "Provider Name 2");
    RMobilePhone::TMobilePhoneNetworkLongName completeData2(KNetworkProviderName2);
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkLongName> nameData2(completeData2);
	data.Close();
	nameData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId, KErrNone, data);
    TRequestStatus status;
	customApi.GetNetworkProviderName(status, name);
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNone);
    ASSERT_EQUALS(name, completeData2);

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0021
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::GetOperatorName
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetOperatorName
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0021L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId, KErrNone);

    TRequestStatus getRequestStatus;
    RMmCustomAPI::TOperatorNameInfo getNameInfo;
    customApi.GetOperatorName(getRequestStatus, getNameInfo);
	ASSERT_EQUALS(KRequestPending, getRequestStatus.Int());
    
    RMmCustomAPI::TOperatorNameType compType = RMmCustomAPI::EOperatorNameProgrammableLatin;
	_LIT(KNetworkOperatorName, "Network Operator Name");
    TBuf<KMaxOperatorNameLength> compName(KNetworkOperatorName);
	TMockLtsyData2<RMmCustomAPI::TOperatorNameType, TBuf<KMaxOperatorNameLength> > modesData(compType, compName);
	data.Close();
	modesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId, KErrNone, data);

   	User::WaitForRequest(getRequestStatus);
	ASSERT_EQUALS(KErrNone, getRequestStatus.Int());
    ASSERT_EQUALS(compType, getNameInfo.iType);
    ASSERT_EQUALS(compName, getNameInfo.iName);

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0022
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::SetSystemNetworkMode
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkMode
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0022L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	RBuf8 data;
	CleanupClosePushL(data);

    // Check synch version of the API
    RMmCustomAPI::TNetworkModeCaps mode = RMmCustomAPI::KCapsNetworkModeUmts;
	TMockLtsyData1<RMmCustomAPI::TNetworkModeCaps> modeData(mode);
	data.Close();
	modeData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId, KErrNone);
	ASSERT_EQUALS(KErrNone, customApi.SetSystemNetworkMode(mode));

    // Check asynch version of the API
    TRequestStatus status;
    mode = RMmCustomAPI::KCapsNetworkModeDual;
	TMockLtsyData1<RMmCustomAPI::TNetworkModeCaps> modeData2(mode);
	data.Close();
	modeData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId, KErrNone);
    customApi.SetSystemNetworkMode(status, mode);
   	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0023
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0023L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	RBuf8 data;
	CleanupClosePushL(data);

    // Check the synch version of the API
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId, KErrNone);
    TUint32 compModes = RMmCustomAPI::KCapsNetworkModeUmts | RMmCustomAPI::KCapsNetworkModeGsm;
	TMockLtsyData1<TUint32> modesData(compModes);
	data.Close();
	modesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId, KErrNone, data);
    TUint32 modes;
	ASSERT_EQUALS(KErrNone, customApi.GetCurrentSystemNetworkModes(modes));
    ASSERT_EQUALS(modes, compModes);

    // Check the asynch version of the API
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId, KErrNone);
    compModes = RMmCustomAPI::KCapsNetworkModeUmts;
	TMockLtsyData1<TUint32> modesData2(compModes);
	data.Close();
	modesData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId, KErrNone, data);
    TRequestStatus status;
    customApi.GetCurrentSystemNetworkModes(status, modes);
   	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
    ASSERT_EQUALS(modes, compModes);

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0024
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::NotifyRauEvent
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyRauEvent
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0024L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    RMmCustomAPI::TRauEventStatus event;
    customApi.NotifyRauEvent(requestStatus, event);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    
	RMmCustomAPI::TRauEventStatus compEvent = RMmCustomAPI::ERauEventResumed;
	TMockLtsyData1<RMmCustomAPI::TRauEventStatus> modesData(compEvent);
	data.Close();
	modesData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyRauEventIndId, KErrNone, data);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(compEvent, event);

	CleanupStack::PopAndDestroy(3, this); // data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0025
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RMmCustomAPI::NotifyCellInfoChange and RMmCustomAPI::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyCellInfoChange and RMmCustomAPI::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0025L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    RMmCustomAPI::TMmCellInfo* completeInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(completeInfo);
    RMmCustomAPI::TMmCellInfo* notifyInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(notifyInfo);
    RMmCustomAPI::TMmCellInfoPckg *notifyInfoPckg = new(ELeave) RMmCustomAPI::TMmCellInfoPckg(*notifyInfo);
    CleanupStack::PushL(notifyInfoPckg);
    RMmCustomAPI::TMmCellInfo* getInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(getInfo);
    RMmCustomAPI::TMmCellInfoPckg *getInfoPckg = new(ELeave) RMmCustomAPI::TMmCellInfoPckg(*getInfo);
    CleanupStack::PushL(getInfoPckg);
	
    // Register a notifier
    customApi.NotifyCellInfoChange(requestStatus, *notifyInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Indicate cell changed to unknowd cell
    (*completeInfo).iMode = RMmCustomAPI::TMmCellInfo::EUnknown;
	TMockLtsyData1<RMmCustomAPI::TMmCellInfo *> modesData1(completeInfo);
	data.Close();
	modesData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCellInfoChangedIndId, KErrNone, data);

    // Check results
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS((*completeInfo).iMode, (*notifyInfo).iMode);

    // Register a notifier
    customApi.NotifyCellInfoChange(requestStatus, *notifyInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Indicate cell changed to GSM cell with some info
    (*completeInfo).iMode = RMmCustomAPI::TMmCellInfo::EGSM;
    (*completeInfo).iGsmCellInfo.iTA = 17;
    (*completeInfo).iGsmCellInfo.iNmr[13].iARFCN = 4;
    (*completeInfo).iGsmCellInfo.iNmr[8].iBSIC = 44;
    (*completeInfo).iGsmCellInfo.iNmr[11].iRxLEV = 100;
	TMockLtsyData1<RMmCustomAPI::TMmCellInfo> modesData2(*completeInfo);
	data.Close();
	modesData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCellInfoChangedIndId, KErrNone, data);

    // Check results
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS((*completeInfo).iMode, (*notifyInfo).iMode);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iTA, (*notifyInfo).iGsmCellInfo.iTA);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iNmr[13].iARFCN, (*notifyInfo).iGsmCellInfo.iNmr[13].iARFCN);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iNmr[8].iBSIC, (*notifyInfo).iGsmCellInfo.iNmr[8].iBSIC);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iNmr[11].iRxLEV, (*notifyInfo).iGsmCellInfo.iNmr[11].iRxLEV);

    // Register a notifier
    customApi.NotifyCellInfoChange(requestStatus, *notifyInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Indicate cell changed to WCDMA cell with some info
    (*completeInfo).iMode = RMmCustomAPI::TMmCellInfo::EWCDMA;
    (*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddUL = 33;
    (*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddDL = 42;
    (*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iTddNt = 18;
    (*completeInfo).iWcdmaCellInfo.iPrimaryScrambilingCode = 100;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[2].iFrequencyInfo.iFddUL = 311;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[4].iFrequencyInfo.iFddDL = 11;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[5].iCarrierRSSI = 25;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[7].iCellMeasuredResult[3].iCID = 25;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[7].iCellMeasuredResult[30].iTddInfo.iPrimaryCcpchRscp = 5;
	TMockLtsyData1<RMmCustomAPI::TMmCellInfo> modesData3(*completeInfo);
	data.Close();
	modesData3.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCellInfoChangedIndId, KErrNone, data);

    // Check results
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS((*completeInfo).iMode, (*notifyInfo).iMode );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddUL, (*notifyInfo).iWcdmaCellInfo.iFrequencyInfo.iFddUL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddDL, (*notifyInfo).iWcdmaCellInfo.iFrequencyInfo.iFddDL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iTddNt, (*notifyInfo).iWcdmaCellInfo.iFrequencyInfo.iTddNt );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iPrimaryScrambilingCode, (*notifyInfo).iWcdmaCellInfo.iPrimaryScrambilingCode );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[2].iFrequencyInfo.iFddUL, (*notifyInfo).iWcdmaCellInfo.iNwkMeasureReport[2].iFrequencyInfo.iFddUL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[4].iFrequencyInfo.iFddDL, (*notifyInfo).iWcdmaCellInfo.iNwkMeasureReport[4].iFrequencyInfo.iFddDL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[5].iCarrierRSSI, (*notifyInfo).iWcdmaCellInfo.iNwkMeasureReport[5].iCarrierRSSI );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[7].iCellMeasuredResult[3].iCID, (*notifyInfo).iWcdmaCellInfo.iNwkMeasureReport[7].iCellMeasuredResult[3].iCID );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[7].iCellMeasuredResult[30].iTddInfo.iPrimaryCcpchRscp, (*notifyInfo).iWcdmaCellInfo.iNwkMeasureReport[7].iCellMeasuredResult[30].iTddInfo.iPrimaryCcpchRscp );

    // expect a GetCellInfo request
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, KErrNone);

    // execute the GetCellInfo request
    customApi.GetCellInfo(requestStatus, *getInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Complete with unknown cell info
    (*completeInfo).iMode = RMmCustomAPI::TMmCellInfo::EUnknown;
	TMockLtsyData1<RMmCustomAPI::TMmCellInfo> modesData4(*completeInfo);
	data.Close();
	modesData4.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, KErrNone, data);

    // Check results
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS((*completeInfo).iMode, (*getInfo).iMode);

    // expect a GetCellInfo request
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, KErrNone);

    // execute the GetCellInfo request
    customApi.GetCellInfo(requestStatus, *getInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Complete with an GSM cell info
    (*completeInfo).iMode = RMmCustomAPI::TMmCellInfo::EGSM;
    (*completeInfo).iGsmCellInfo.iTA = 1;
    (*completeInfo).iGsmCellInfo.iNmr[1].iARFCN = 2;
    (*completeInfo).iGsmCellInfo.iNmr[2].iBSIC = 3;
    (*completeInfo).iGsmCellInfo.iNmr[7].iRxLEV = 4;
	TMockLtsyData1<RMmCustomAPI::TMmCellInfo> modesData5(*completeInfo);
	data.Close();
	modesData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, KErrNone, data);

    // Check results
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS((*completeInfo).iMode, (*getInfo).iMode);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iTA, (*getInfo).iGsmCellInfo.iTA);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iNmr[1].iARFCN, (*getInfo).iGsmCellInfo.iNmr[1].iARFCN);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iNmr[2].iBSIC, (*getInfo).iGsmCellInfo.iNmr[2].iBSIC);
    ASSERT_EQUALS((*completeInfo).iGsmCellInfo.iNmr[7].iRxLEV, (*getInfo).iGsmCellInfo.iNmr[7].iRxLEV);

    // expect a GetCellInfo request
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, KErrNone);

    // execute the GetCellInfo request
    customApi.GetCellInfo(requestStatus, *getInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Complete with an WCDMA cell info
    (*completeInfo).iMode = RMmCustomAPI::TMmCellInfo::EWCDMA;
    (*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddUL = 11;
    (*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddDL = 12;
    (*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iTddNt = 13;
    (*completeInfo).iWcdmaCellInfo.iPrimaryScrambilingCode = 14;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[1].iFrequencyInfo.iFddUL = 1;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[2].iFrequencyInfo.iFddDL = 2;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[3].iCarrierRSSI = 3;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[4].iCellMeasuredResult[1].iCID = 6;
    (*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[5].iCellMeasuredResult[2].iTddInfo.iPrimaryCcpchRscp = 8;
	TMockLtsyData1<RMmCustomAPI::TMmCellInfo> modesData6(*completeInfo);
	data.Close();
	modesData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId, KErrNone, data);

    // Check results
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS((*completeInfo).iMode, (*getInfo).iMode);
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddUL, (*getInfo).iWcdmaCellInfo.iFrequencyInfo.iFddUL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iFddDL, (*getInfo).iWcdmaCellInfo.iFrequencyInfo.iFddDL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iFrequencyInfo.iTddNt, (*getInfo).iWcdmaCellInfo.iFrequencyInfo.iTddNt );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iPrimaryScrambilingCode, (*getInfo).iWcdmaCellInfo.iPrimaryScrambilingCode );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[1].iFrequencyInfo.iFddUL, (*getInfo).iWcdmaCellInfo.iNwkMeasureReport[1].iFrequencyInfo.iFddUL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[2].iFrequencyInfo.iFddDL, (*getInfo).iWcdmaCellInfo.iNwkMeasureReport[2].iFrequencyInfo.iFddDL );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[3].iCarrierRSSI, (*getInfo).iWcdmaCellInfo.iNwkMeasureReport[3].iCarrierRSSI );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[4].iCellMeasuredResult[1].iCID, (*getInfo).iWcdmaCellInfo.iNwkMeasureReport[4].iCellMeasuredResult[1].iCID );
    ASSERT_EQUALS((*completeInfo).iWcdmaCellInfo.iNwkMeasureReport[5].iCellMeasuredResult[2].iTddInfo.iPrimaryCcpchRscp, (*getInfo).iWcdmaCellInfo.iNwkMeasureReport[5].iCellMeasuredResult[2].iTddInfo.iPrimaryCcpchRscp );

	CleanupStack::PopAndDestroy(8, this); // getInfoPckg, getInfo, notifyInfoPckg, notifyInfo, completeInfo, data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0026
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of RMobilePhone::NotifyCellInfoChange() RMobilePhone::GetCellInfo()
@SYMTestPriority High
@SYMTestActions Invokes NotifyCellInfoChange, GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0026L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	RMobilePhone::TMobilePhoneCellInfoV9 returnCellInfo;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> cellInfoCompData(returnCellInfo);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId, KErrNone);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId);

	returnCellInfo.iMode = RMobilePhone::ECellModeGsm;
	returnCellInfo.iCellId = 1;
	returnCellInfo.iTimingAdvance = 123;
	returnCellInfo.iStatus = RMobilePhone::ECellInfoStatusCurrent;
	cellInfoCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId, KErrNone, data);
	data.Close();
	
	//client side will be notified when a CellInfoChange has occurred, the first time called will result in
	//a NotifyCellInfo Request to inform the LTSY we are interested in these notifications
	TRequestStatus notifyCellInfoChangeReqStatus;
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg cellInfoPckg(cellInfo);
	iPhone.NotifyCellInfoChange(notifyCellInfoChangeReqStatus,cellInfoPckg);
		
	TRequestStatus getCellInfoReqStatus;
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo2;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg cellInfoPckg2(cellInfo2);
	iPhone.GetCellInfo(getCellInfoReqStatus,cellInfoPckg2);
		
	User::WaitForRequest(notifyCellInfoChangeReqStatus);
	User::WaitForRequest(getCellInfoReqStatus);
	ASSERT_EQUALS(notifyCellInfoChangeReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(getCellInfoReqStatus.Int(),KErrNone);
	
	ASSERT_EQUALS(returnCellInfo.iMode, cellInfo.iMode);
	ASSERT_EQUALS(returnCellInfo.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(returnCellInfo.iTimingAdvance, cellInfo.iTimingAdvance);
	ASSERT_EQUALS(returnCellInfo.iStatus, cellInfo.iStatus);
	
	ASSERT_EQUALS(returnCellInfo.iMode, cellInfo2.iMode);
	ASSERT_EQUALS(returnCellInfo.iCellId, cellInfo2.iCellId);
	ASSERT_EQUALS(returnCellInfo.iTimingAdvance, cellInfo2.iTimingAdvance);
	ASSERT_EQUALS(returnCellInfo.iStatus, cellInfo2.iStatus);
	
	AssertMockLtsyStatusL();
		
	//now a second get, the value is in the cach (rest cellInfo first)
	cellInfo.iMode = RMobilePhone::ECellModeUnknown;
	cellInfo.iCellId = 0;
	cellInfo.iTimingAdvance = 0;
	cellInfo.iStatus = RMobilePhone::ECellInfoStatusUnknown;
	iPhone.GetCellInfo(getCellInfoReqStatus,cellInfoPckg);
	User::WaitForRequest(getCellInfoReqStatus);
	ASSERT_EQUALS(getCellInfoReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(returnCellInfo.iMode, cellInfo.iMode);
	ASSERT_EQUALS(returnCellInfo.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(returnCellInfo.iTimingAdvance, cellInfo.iTimingAdvance);
	ASSERT_EQUALS(returnCellInfo.iStatus, cellInfo.iStatus);
	
	AssertMockLtsyStatusL();
	
	// an unsolicited cellInfo change, with KErrNone
	returnCellInfo.iMode = RMobilePhone::ECellModeWcdma;
	returnCellInfo.iCellId = 2;
	returnCellInfo.iTimingAdvance = 124;
	returnCellInfo.iStatus = RMobilePhone::ECellInfoStatusNotCurrent;
	cellInfoCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId, KErrNone, data);
	data.Close();
	
	//reset the return data
	cellInfo.iMode = RMobilePhone::ECellModeUnknown;
	cellInfo.iCellId = 0;
	cellInfo.iTimingAdvance = 0;
	cellInfo.iStatus = RMobilePhone::ECellInfoStatusUnknown;
	iPhone.NotifyCellInfoChange(notifyCellInfoChangeReqStatus,cellInfoPckg);
	User::WaitForRequest(notifyCellInfoChangeReqStatus);
	ASSERT_EQUALS(notifyCellInfoChangeReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(returnCellInfo.iMode, cellInfo.iMode);
	ASSERT_EQUALS(returnCellInfo.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(returnCellInfo.iTimingAdvance, cellInfo.iTimingAdvance);
	ASSERT_EQUALS(returnCellInfo.iStatus, cellInfo.iStatus);
		
	AssertMockLtsyStatusL();
	
	//an unsolicited cellInfo change, with KErrGeneral, the cache is now marked as dirty
	cellInfoCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId, KErrGeneral, data);
	data.Close();

	iPhone.NotifyCellInfoChange(notifyCellInfoChangeReqStatus,cellInfoPckg);
	User::WaitForRequest(notifyCellInfoChangeReqStatus);
	ASSERT_EQUALS(notifyCellInfoChangeReqStatus.Int(),KErrGeneral);
	
	AssertMockLtsyStatusL();
	
	//a client side GetCellInfo call (post the cache being marked dirty), KErrGeneral returned
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId);
	returnCellInfo.iMode = RMobilePhone::ECellModeGsm;
	returnCellInfo.iCellId = 1;
	returnCellInfo.iTimingAdvance = 123;
	returnCellInfo.iStatus = RMobilePhone::ECellInfoStatusCurrent;
	cellInfoCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId, KErrGeneral, data);
	data.Close();
	
	iPhone.GetCellInfo(getCellInfoReqStatus,cellInfoPckg);
	User::WaitForRequest(getCellInfoReqStatus);
	ASSERT_EQUALS(getCellInfoReqStatus.Int(),KErrGeneral);
	
	AssertMockLtsyStatusL(); 
	
	
	//an unsolicited cellInfo change, with KErrNone, the cache is now marked as clean
	cellInfoCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId, KErrNone, data);
	data.Close();

	//reset the return data
	cellInfo.iMode = RMobilePhone::ECellModeUnknown;
	cellInfo.iCellId = 0;
	cellInfo.iTimingAdvance = 0;
	cellInfo.iStatus = RMobilePhone::ECellInfoStatusUnknown;
	iPhone.NotifyCellInfoChange(notifyCellInfoChangeReqStatus,cellInfoPckg);
	User::WaitForRequest(notifyCellInfoChangeReqStatus);
	ASSERT_EQUALS(notifyCellInfoChangeReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(returnCellInfo.iMode, cellInfo.iMode);
	ASSERT_EQUALS(returnCellInfo.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(returnCellInfo.iTimingAdvance, cellInfo.iTimingAdvance);
	ASSERT_EQUALS(returnCellInfo.iStatus, cellInfo.iStatus);
	
	AssertMockLtsyStatusL();

	//finally a cached get.
	//reset the return data
	cellInfo.iMode = RMobilePhone::ECellModeUnknown;
	cellInfo.iCellId = 0;
	cellInfo.iTimingAdvance = 0;
	cellInfo.iStatus = RMobilePhone::ECellInfoStatusUnknown;
	iPhone.GetCellInfo(getCellInfoReqStatus,cellInfoPckg);
	User::WaitForRequest(getCellInfoReqStatus);
	ASSERT_EQUALS(getCellInfoReqStatus.Int(),KErrNone);
	ASSERT_EQUALS(returnCellInfo.iMode, cellInfo.iMode);
	ASSERT_EQUALS(returnCellInfo.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(returnCellInfo.iTimingAdvance, cellInfo.iTimingAdvance);
	ASSERT_EQUALS(returnCellInfo.iStatus, cellInfo.iStatus);

	AssertMockLtsyStatusL();

	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0027
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of RMobilePhone::NotifyCellInfoChange()
@SYMTestPriority High
@SYMTestActions Invokes NotifyCellInfoChange,with a complete error case
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0027L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	//test the complete of NotifyCellInfoChange with an error, this is in a differnet test case
	//as the IPC is only fired the first time a NotifyCellInfoChange is called as the 
	//Cache Ability is set ECacheUnknown
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId, KErrGeneral);

	TRequestStatus notifyCellInfoChangeReqStatus;
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg cellInfoPckg(cellInfo);
	iPhone.NotifyCellInfoChange(notifyCellInfoChangeReqStatus,cellInfoPckg);
	User::WaitForRequest(notifyCellInfoChangeReqStatus);
	ASSERT_EQUALS(notifyCellInfoChangeReqStatus.Int(),KErrGeneral);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0028
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of RMmCustomAPI::GetUSIMServiceSupport()
@SYMTestPriority High
@SYMTestActions Invokes GetUSIMServiceSupport, in KErrNone and KErrGeneral complete cases
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0028L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	//test a Get Usim Service Support with KErrNone completion
	
	TInt applicationNumber = 101;
	TBool supported = ETrue;
	
	TMockLtsyData1<TInt> getUsimServiceSupportExpData(applicationNumber);
	getUsimServiceSupportExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId, data);
	data.Close();
	
	TMockLtsyData2<TInt, TBool> getUsimServiceSupportCompData(applicationNumber, supported);
	getUsimServiceSupportCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId, KErrNone, data);
	data.Close();
	
	RMmCustomAPI::TAppSupport usimSupport;
	usimSupport.iAppNum = applicationNumber; //in
	usimSupport.iSupported = EFalse; //out
	customApi.GetUSIMServiceSupport(reqStatus, usimSupport);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(usimSupport.iAppNum,applicationNumber);
	ASSERT_EQUALS(usimSupport.iSupported,supported);
		
	AssertMockLtsyStatusL();

	//test a Get Usim Service Support with KErrGeneral completion

	applicationNumber = 102;
	supported = EFalse;
	
	getUsimServiceSupportExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId, data);
	data.Close();
	
	getUsimServiceSupportCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId, KErrGeneral, data);
	data.Close();
	
	
	usimSupport.iAppNum = applicationNumber; //in
	usimSupport.iSupported = ETrue; //out
	customApi.GetUSIMServiceSupport(reqStatus, usimSupport);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrGeneral);
	ASSERT_EQUALS(usimSupport.iAppNum,applicationNumber);
	ASSERT_EQUALS(usimSupport.iSupported, static_cast<TBool>(ETrue)); //i.e. not set to EFalse from the complete
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0029
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of RMobilePhone::GetCurrentActiveUSimApplication()
@SYMTestPriority High
@SYMTestActions Invokes GetCurrentActiveUSimApplication, in KErrNone and KErrGeneral complete cases
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0029L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId);
	
	RMobilePhone::TAID comopAid(_L8("AID_DATA"));
	
	TDesC8* aidPtr = &comopAid;
	TMockLtsyData1<TDesC8*> getCurrentActiveUsimAppCompData(aidPtr);
	getCurrentActiveUsimAppCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId, KErrNone, data);
	data.Close();
	
	RMobilePhone::TAID aid;
	iPhone.GetCurrentActiveUSimApplication(reqStatus,aid);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(comopAid, aid);
	AssertMockLtsyStatusL();

	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId);
	
	getCurrentActiveUsimAppCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId, KErrGeneral, data);
	data.Close();

	aid.Zero();
	iPhone.GetCurrentActiveUSimApplication(reqStatus,aid);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);
	ASSERT_EQUALS(comopAid, aid); //aid still copied
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0030
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of RMobilePhone::TerminateAllCalls()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::TerminateAllCalls
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0030L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus requestStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();

	
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	DriverDialCallL(callId, mobileService);
	
	TInt callIdHangUp = 0; //callId is provided if a call is in connecting state; else 0
    TMockLtsyData1<TInt> mockData(callIdHangUp); 
	mockData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId, KErrNone);
	data.Close();

	TInt hangUpCause = KErrGsmCCNormalCallClearing; // Hang up cause for normal hang up
	DriverHangUpCallL(callId, hangUpCause, ETrue);
	

	// Client Side Test

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, mobileService);


	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 

	iPhone.TerminateAllCalls(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ClientHangUpCallL(mobileCall);
	
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	
	AssertMockLtsyStatusL();
	
	mobileCall.Close();
	mobileLine.Close();
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&mobileCall);
	CleanupStack::PopAndDestroy(&mobileLine);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0031
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test correct function of RMmCustomAPI::GetSystemNetworkBand() and RMmCustomAPI::SetSystemNetworkBand()
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkBand(), RMmCustomAPI::GetSystemNetworkBand()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0031L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus requestStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RMmCustomAPI::TBandSelection band=RMmCustomAPI::ENetworkBandAny;
	RMmCustomAPI::TNetworkModeCaps mode=RMmCustomAPI::KCapsNetworkModeUmts;
	
	RMmCustomAPI::TBandSelection bandToGet=RMmCustomAPI::ENetworkBandUmts850;
	RMmCustomAPI::TNetworkModeCaps modeToGet=RMmCustomAPI::KCapsNetworkModeDual;
	
	RMmCustomAPI::TBandSelection bandToSet=RMmCustomAPI::ENetworkBandUmts2100;
	RMmCustomAPI::TNetworkModeCaps modeToSet=RMmCustomAPI::KCapsNetworkModeGsm;
	
	TRequestStatus req=0;

	
	//1. Test GetSystemNetworkBand

	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId);
	
	TMockLtsyData2<RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> getData(bandToGet, modeToGet);
	getData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId, KErrNone, data);
	data.Close();
	
	
	customApi.GetSystemNetworkBand( req, band, mode);
    User::WaitForRequest(req);
    
    ASSERT_EQUALS(KErrNone, req.Int());
    ASSERT_EQUALS(bandToGet, band);
    ASSERT_EQUALS(modeToGet, mode);
    
    AssertMockLtsyStatusL();
    
    
    //2. Test SetSystemNetworkBand

	TMockLtsyData2<RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> setData(bandToSet, modeToSet);
	setData.SerialiseL(data);
    
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId, data);

	iMockLTSY.CompleteL(MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId, KErrNone);
	data.Close();
    
    
    band = bandToSet;
    mode = modeToSet;
    customApi.SetSystemNetworkBand(req, band, mode);
    User::WaitForRequest(req);
    ASSERT_EQUALS(KErrNone, req.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONE-UN0032
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test RPhone::NotifyDtmfEvent
@SYMTestPriority High
@SYMTestActions Invokes RPhone::NotifyDtmfEvent
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPhoneFU::TestUnit0032L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
	
	RMobilePhone::TMobilePhoneDTMFEvent event(RMobilePhone::EStopDtmfTone);	
	RMobilePhone::TMobilePhoneDTMFEvent startEvent(RMobilePhone::EStartDtmfTone);	
	TRequestStatus reqStatus;
	
    TMockLtsyData1< RMobilePhone::TMobilePhoneDTMFEvent > toneEvent( startEvent );
    toneEvent.SerialiseL(data);

	iPhone.NotifyDTMFEvent(reqStatus, event);
	iMockLTSY.CompleteL(KMockLtsyDispatchPhoneNotifyDtmfEventIndId, KErrNone, data);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());  
	ASSERT_EQUALS(RMobilePhone::EStartDtmfTone, event);
	
	
	CleanupStack::PopAndDestroy(2, this); // call, line, data, this	
	}
