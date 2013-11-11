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


#include "cctsycellbroadcastfunegative.h"

#include <etelmm.h>
#include "config.h"
#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
#include <test/tmockltsydata.h>
#include "mockltsyindicatorids.h"

/* static */
CTestSuite* CCTsyCellBroadcastFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFUNegative, TestMessagingReceiveMessageL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFUNegative, TestMessagingReceiveMessageCancelL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFUNegative, TestSetBroadcastFilterSettingL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFUNegative, TestStartSimCbTopicBrowsingL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFUNegative, TestDeleteSimCbTopicL);
	END_SUITE;
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-NEGATIVE-UC0001
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in SIM CTSY dispatch when the ActivateBroadcastReceiveMessage API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCellBroadcastFUNegative::TestMessagingReceiveMessageL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
				
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, &config); // messaging, this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-NEGATIVE-UC0002
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in SIM CTSY dispatch when the ReceiveMessageCancel API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCellBroadcastFUNegative::TestMessagingReceiveMessageCancelL()
	{
	TConfig config;
	//config.SetSupportedValue(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, EFalse);
	config.SetSupportedValue(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	// setting environment ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	TRequestStatus status;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(status, msgData, msgAttributes);
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);
				
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();

	config.Reset();
	CleanupStack::PopAndDestroy(3, &config); // messaging, this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-NEGATIVE-UC0003
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in SIM CTSY dispatch when the SetBroadcastFilterSetting API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCellBroadcastFUNegative::TestSetBroadcastFilterSettingL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	// set CMmBroadcastTsy::iCbRoutingActivated to ETrue
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);	
	iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	TRequestStatus status;
	messaging.SetFilterSetting(status, RMobileBroadcastMessaging::EBroadcastAcceptNone);
	ASSERT_EQUALS(KErrNotSupported, status.Int());
	
	AssertMockLtsyStatusL();
	
	config.Reset();	
	CleanupStack::PopAndDestroy(3, &config); // messaging, this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-NEGATIVE-UC0004
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in SIM CTSY dispatch when the StartSimCbTopicBrowsing API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCellBroadcastFUNegative::TestStartSimCbTopicBrowsingL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customAPI;
	// If you call customAPI.Open(iPhone) directly AssertMockLtsyStatusL will fail
	OpenCustomApiLC(customAPI);
		
	TRequestStatus status;

	TInt ret = customAPI.StartSimCbTopicBrowsing();
	ASSERT_EQUALS(KErrNotSupported, ret);
	
	AssertMockLtsyStatusL();
	
	config.Reset();	
	CleanupStack::PopAndDestroy(3, &config); // customAPI, this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-NEGATIVE-UC0005
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in SIM CTSY dispatch when the DeleteSimCbTopic API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsyCellBroadcastFUNegative::TestDeleteSimCbTopicL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId, EFalse);
	config.PushL();
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);
	
	TUint index(0);
	TRequestStatus status;

	customAPI.DeleteSimCbTopic(status, index);;
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNotSupported, status.Int());

	AssertMockLtsyStatusL();
	
	config.Reset();	
	CleanupStack::PopAndDestroy(3, &config); // customAPI, this, config
	}
