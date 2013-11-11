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
// cctsysmsfu.cpp
// when support for the various APIs are disabled in the dispatch layer.
//



/**
 @file	The TEFUnit test suite for SmsControl in the Common TSY
*/

#include "cctsysmsfunegative.h"

#include <etelmm.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include <mmretrieve.h>
#include "mockltsyindicatorids.h"

#include "config.h"

#include "cctsyactiveretriever.h"
#include <test/tmockltsydata.h>

CTestSuite* CCTsySmsFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestNackSmsStoredL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestAckSmsStoredL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestSendMessageL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestGetSmsStoreInfoL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestGetSmspListL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestSendMessageNoFdnCheckL);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestResumeSmsReceptionL);

	ADD_TEST_STEP_ISO_CPP(CCTsySmsFUNegative, TestStoreSmspListEntryL);
	END_SUITE;
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0001
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the NackSmsStored API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestNackSmsStoredL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, EFalse);
	config.PushL();

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);
 
    RBuf8 data;
    CleanupClosePushL(data);
    
    TRequestStatus reqStatus;
    TRequestStatus mockLtsyStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
  
    _LIT8(KMessage, "Happy New Year");
    TBuf8<100> forMsg;
  
    // receiving message:
    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId, KErrNone, data);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    TInt rpCause(0);
    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);
    data.Close();
    expTsyData.SerialiseL(data);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(4, &config);    //  messaging, this, data, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0002
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the AckSmsStored API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestAckSmsStoredL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, EFalse);
	config.PushL();

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);
 
    RBuf8 data;
    CleanupClosePushL(data);
	
    TRequestStatus reqStatus;
    TRequestStatus mockLtsyStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
  
    _LIT8(KMessage, "Happy New Year");
    TBuf8<100> forMsg;
  
    // receiving message:
    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId, KErrNone, data);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
 
    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(4, &config);	//  messaging, this, data, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0003
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the SendMessage API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestSendMessageL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, EFalse);
	config.PushL();
	
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    CleanupClosePushL(messaging);
    
    _LIT8(KMessage, "Hello");
    
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);
    
    TRequestStatus status;
	messaging.SendMessage(status, KMessage, smsAttrPckg);
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNotSupported);   
	    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, &config);	//  messaging, this, config	
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0004
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the GetMessageStoreInfo (a.k.a GetSmsStoreInfo) API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestGetSmsStoreInfoL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId, EFalse);
	config.PushL();
	
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    CleanupClosePushL(messaging);
    
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo(info);
    TInt index(0);
    
    TRequestStatus status;
    messaging.GetMessageStoreInfo(status, index, pckgInfo);  
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNotSupported);   
	    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, &config);	//  messaging, this, config	
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0005
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the ReadSmspListPhase1L API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API via the CRetrieveMobilePhoneSmspList AO, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestGetSmspListL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, EFalse);
	config.PushL();
	
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    CleanupClosePushL(messaging);
    
	//CFilteringActiveScheduler does not need to be used, this should be changed
	//to a listretrieverao type class (see "listretrieverao.h")
	CFilteringActiveScheduler* scheduler = new (ELeave) CFilteringActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	CRetrieveMobilePhoneSmspList* asyncRetrieveList
		= CRetrieveMobilePhoneSmspList::NewL(messaging);    
	CleanupStack::PushL(asyncRetrieveList);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveList);
	CleanupStack::PushL(activeRetriever);
	scheduler->AddRetrieverL(*activeRetriever);
	
	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler->StartScheduler();
	
	ASSERT_EQUALS(CActiveRetriever::ResetRequestsNumber(), 0);
	ASSERT_EQUALS(activeRetriever->iStatus.Int(), KErrNotSupported);
	
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(6, &config);	//  activeRetriever, asyncRetrieveList, scheduler, , messaging, this, config			
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0006
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the SendMessageNoFdnCheck API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestSendMessageNoFdnCheckL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId, EFalse);
	config.PushL();
	
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    CleanupClosePushL(messaging);
    
    _LIT8(KMessage, "Hello");

    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);
    
	TRequestStatus status;
    messaging.SendMessageNoFdnCheck(status, KMessage, smsAttrPckg);
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNotSupported);   
	    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, &config);	//  messaging, this, config
	}

/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0007
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the ResumeSmsReception API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestResumeSmsReceptionL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId, EFalse);
	config.PushL();
	
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    CleanupClosePushL(messaging);
	
    TRequestStatus status;
    messaging.ResumeSmsReception(status);  
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNotSupported);   
	    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, &config);	//  messaging, this, config
	}


/**
 * @SYMTestCaseID BA-CTSYD-DIS-SMS-NEGATIVE-UN0009
 * @SYMComponent telephony_ctsy
 * @SYMTestCaseDesc Test handing in CTSY dispatch when the StoreSmspListEntry API is disabled 
 * @SYMTestPriority High
 * @SYMTestActions Disable API, call API, check correct error returned
 * @SYMTestExpectedResults Pass
 * @SYMTestType CT
 */
void CCTsySmsFUNegative::TestStoreSmspListEntryL()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, EFalse);
	config.PushL();
	
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    CleanupClosePushL(messaging);
	
    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);
        
    smspListPtr->AddEntryL(smspEntry);
    
    TRequestStatus status;
    messaging.StoreSmspListL(status, smspListPtr);
	User::WaitForRequest(status);
    ASSERT_EQUALS(status.Int(), KErrNotSupported);   
	    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(4, &config);	//  smspListPtr, messaging, this, config
	}

