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

#include <ctsy/ltsy/mltsydispatchsupplementaryservicesinterface.h>
#include "config.h"
#include "cctsyactiveretriever.h"
#include <mmretrieve.h>

#include "cctsysupplementaryservicesfunegative.h"

CTestSuite* CCTsySupplementaryServicesFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFUNegative, TestUnit0013L);
	END_SUITE;
	}

//
// 'Negative' unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSendNetworkServiceRequestNoFdnCheck is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequestNoFdnCheck()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	_LIT(KMsg2,"SendNetworkServiceRequest1.2");
	TName expServiceStringNoFDN(KMsg2);
    	
	iPhone.SendNetworkServiceRequestNoFdnCheck(requestStatus, expServiceStringNoFDN);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetWaitingStatusPhase1 is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCWList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList = CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	RMobilePhone::TMobileInfoLocation location = RMobilePhone::EInfoLocationCachePreferred;	

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, location);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	
	AssertMockLtsyStatusL();	
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // activeRetriever, asyncRetrieveMobilePhoneCWList, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSendNetworkServiceRequest is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequest()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0003L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	_LIT(KMsg1,"SendNetworkServiceRequest1.1");
	TName expServiceString(KMsg1);
    	
	iPhone.SendNetworkServiceRequest(requestStatus, expServiceString);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetBarringStatusPhase1 is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCBList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0004L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	CRetrieveMobilePhoneCBList* phoneCBList = CRetrieveMobilePhoneCBList::NewL(iPhone);
	CleanupStack::PushL(phoneCBList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*phoneCBList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
	
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarAllIncoming;

	phoneCBList->Start(activeRetriever->iStatus, condition);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber())
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int())

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // activeRetriever, iCBList, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSetCallBarringStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallBarringStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0005L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneCBCondition setCondition = RMobilePhone::EBarIncomingRoaming;
	RMobilePhone::TMobilePhoneCBChangeV1 setInfo;
	setInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	setInfo.iServiceGroup = RMobilePhone::EPadAccess;
	_LIT(KPassword,"1234");
	setInfo.iPassword = KPassword;
	
	iPhone.SetCallBarringStatus(requestStatus, setCondition, setInfo);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSetCallForwardingStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallForwardingStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0006L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneCFCondition setCondition = RMobilePhone::ECallForwardingNoReply;
	RMobilePhone::TMobilePhoneCFChangeV1  callForwardinginfo;

	callForwardinginfo.iAction = RMobilePhone::EServiceActionRegister;
	callForwardinginfo.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	callForwardinginfo.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	_LIT(KTelNumber,"+74951000000");
	callForwardinginfo.iNumber.iTelNumber.Copy(KTelNumber);
	callForwardinginfo.iServiceGroup = RMobilePhone::EVoiceService;
	callForwardinginfo.iTimeout = 20;
	
	iPhone.SetCallForwardingStatus(requestStatus, setCondition, callForwardinginfo);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSetCallWaitingStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallWaitingStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0007L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
    RMobilePhone::TMobileService setService = RMobilePhone::EPadAccess;
    RMobilePhone::TMobilePhoneServiceAction setAction = RMobilePhone::EServiceActionActivate;
    	
	iPhone.SetCallWaitingStatus(requestStatus, setService, setAction);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneSetSSPassword is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetSSPassword()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    TRequestStatus requestStatus;
	_LIT(KOldPassword,"oldPswd");
	_LIT(KNewPassword,"newPswd");
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	TUint16 service = 330; // Can be only 0 for all or 330 for Barring 
    	
	iPhone.SetSSPassword(requestStatus,password,service);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobilePhoneGetCallForwardingStatusPhase1 is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCFList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList = CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	RMobilePhone::TMobileInfoLocation location = RMobilePhone::EInfoLocationCachePreferred;
		
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup, location);
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	
	AssertMockLtsyStatusL();	
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // activeRetriever, asyncRetrieveMobilePhoneCFList, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileUssdMessagingSendMessage is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessage()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0010L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobileUssdMessaging ussdMessaging;
	ASSERT_EQUALS(KErrNone, ussdMessaging.Open(iPhone));
	CleanupClosePushL(ussdMessaging);
	
	_LIT8(KMsg1,"SendMessage1.1");
	RMobileUssdMessaging::TGsmUssdMessageData expSendData(KMsg1);
	RMobileUssdMessaging::TMobileUssdAttributesV1 expSendAttr;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttrPckg(expSendAttr);
	
	ussdMessaging.SendMessage(requestStatus, expSendData, sendAttrPckg);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    
    
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // ussdMessaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileUssdMessagingSendMessageNoFdnCheck is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessageNoFdnCheck()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobileUssdMessaging ussdMessaging;
	ASSERT_EQUALS(KErrNone, ussdMessaging.Open(iPhone));
	CleanupClosePushL(ussdMessaging);
	
	_LIT8(KMsg1,"SendMessage1.1");
	RMobileUssdMessaging::TGsmUssdMessageData expSendData(KMsg1);
	RMobileUssdMessaging::TMobileUssdAttributesV1 expSendAttr;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttrPckg(expSendAttr);
	
	ussdMessaging.SendMessageNoFdnCheck(requestStatus, expSendData, sendAttrPckg);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    
    
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // ussdMessaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EMobileUssdMessagingSendRelease is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendRelease()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0012L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RMobileUssdMessaging ussdMessaging;
	ASSERT_EQUALS(KErrNone, ussdMessaging.Open(iPhone));
	CleanupClosePushL(ussdMessaging);

	RMobilePhone::TMobilePhoneSendSSRequestV3 sendSSRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg sendSSRequestPckg(sendSSRequest);
	
	ussdMessaging.SendRelease(requestStatus, sendSSRequestPckg);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    
    
    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // ussdMessaging, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-NEGATIVE-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECustomClearCallBlackListIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ClearCallBlackList()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsySupplementaryServicesFUNegative::TestUnit0013L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
    
	customApi.ClearCallBlackList(requestStatus);
	User::WaitForRequest(requestStatus);
 	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // customApi, this
	}
