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
 @file The TEFUnit test suite for CellBroadcastControl in the Common TSY.
*/

#include "cctsycellbroadcastfu.h"
#include <ctsy/rmmcustomapi.h>
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
#include "mockltsyindicatorids.h"

CTestSuite* CCTsyCellBroadcastFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFU, TestUseCase0002L);		
	  
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCellBroadcastFU, TestUnit0003L);	

	END_SUITE;
	}

void CCTsyCellBroadcastFU::InitSimCbTopicsL(RMmCustomAPI& aCustomApi, const TDesC& aTopic, TInt aNum)
	{

	RBuf8 compData;
	CleanupClosePushL(compData);

	CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simTopicArray =  new ( ELeave ) CArrayFixFlat
                            <RMmCustomAPI::TSimCbTopic>(1);	
	CleanupStack::PushL(simTopicArray);
	
	RMmCustomAPI::TSimCbTopic topic;
	topic.iNumber = aNum;
	topic.iName.Copy(aTopic);

	simTopicArray->AppendL( topic );
	
	TMockLtsyData1< CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* > ltsyData(simTopicArray);	
    ltsyData.SerialiseL(compData); 

	iMockLTSY.ExpectL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId);    
	iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId, KErrNone, compData);    
	TInt ret = aCustomApi.StartSimCbTopicBrowsing();
	ASSERT_EQUALS(KErrNone, ret);

	CleanupStack::PopAndDestroy(2);	//simTopicArray, compData
	}

//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for ReceiveMessage(), CompleteReceiveMessageGsmCbs(), SetFilterSettingL(), GetFilterSetting()
@SYMTestPriority High
@SYMTestActions Invokes ReceiveMessageL(), CompleteReceiveMessageGsmCbs(), SetFilterSettingL(), GetFilterSetting()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyCellBroadcastFU::TestUseCase0001L()
	{		
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
		
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter expectData = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	
	TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data);     
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, data); 
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);

	_LIT8(KMessage,"CBSMESSAGE");
	TDesC8* cbsMessage = const_cast<TDesC8*>(&KMessage);
	TMockLtsyData1<TDesC8*> completeLtsyData(cbsMessage);
    data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(KMockLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId, KErrNone, data);
        
	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);
			
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// iCbRoutingActivated == ETrue now: we can test SetFilterSetting()
	
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptAll, setting);
	AssertMockLtsyStatusL();
	
	// mockltsy setup:
	expectData = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	
	TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> expectLtsyData2(expectData);
	data.Close();
    expectLtsyData2.SerialiseL(data);     
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId, data);    
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId, KErrNone);
	
	// change setting	
	messaging.SetFilterSetting(reqStatus, RMobileBroadcastMessaging::EBroadcastAcceptNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	err = messaging.GetFilterSetting(setting);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(RMobileBroadcastMessaging::EBroadcastAcceptNone, setting);
	AssertMockLtsyStatusL();		
	
	CleanupStack::PopAndDestroy(3, this); // messaging, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CompleteReceiveMessageWcdmaCbs()
@SYMTestPriority High
@SYMTestActions Invokes CompleteReceiveMessageWcdmaCbs()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyCellBroadcastFU::TestUseCase0002L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter expectData = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	
	TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data);     
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);		    
	
	TUint32 expectedFlags(RMobileBroadcastMessaging::KBroadcastDataFormat);
	RMobileBroadcastMessaging::TMobileBroadcastDataFormat
	                      expectedFormat(RMobileBroadcastMessaging::EFormatGsmTpdu);
		
    _LIT8(KMessage, "Hello0");
    TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize> tmpName(KMessage);
	TInt wcdmaPageNumber(1);
    DispatcherCellBroadcast::TWcdmaCbsMsgBase wcdmaCbsMsg0;
	wcdmaCbsMsg0.iNumberOfPages = 2;
	wcdmaCbsMsg0.iMessageType = 3;
	wcdmaCbsMsg0.iMessageId = 4;
	wcdmaCbsMsg0.iSerialNum = 5;
	wcdmaCbsMsg0.iDCS = 6;
	wcdmaCbsMsg0.iInfoLength = 5;
    	
	TBool moreToCome = EFalse;
	TDesC8* strPtr = &tmpName;
	TMockLtsyData3<TDesC8* , DispatcherCellBroadcast::TWcdmaCbsMsgBase , TBool > completeWcdmaLtsyData(strPtr, wcdmaCbsMsg0, moreToCome);	
	data.Close();
	completeWcdmaLtsyData.SerialiseL(data);
		
	iMockLTSY.CompleteL(KMockLtsyDispatchCellBroadcastWcdmaBroadcastMessageReceivedIndId, KErrNone, data);
	
	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 wcdmaAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg wcdmaMsgAttributes(wcdmaAttributes);
		
	messaging.ReceiveMessage(reqStatus, msgData, wcdmaMsgAttributes);	

	User::WaitForRequest(reqStatus);
	
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	
	// generate a message from the original message with the page number at the end
	TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize>  messageWithPageNumber;
	messageWithPageNumber.Copy(tmpName);
	_LIT8(KFormat, "%c");
	TBuf8<2> abc(KFormat);
	// Append pagenumber to end of CBS message     
	messageWithPageNumber.AppendFormat( abc, wcdmaPageNumber);
	
	ASSERT_EQUALS(messageWithPageNumber,  msgData);	

	ASSERT_EQUALS(expectedFlags,               wcdmaAttributes.iFlags);
	expectedFormat = RMobileBroadcastMessaging::EFormatWcdmaTpdu;
	ASSERT_EQUALS(expectedFormat,              wcdmaAttributes.iFormat);
	ASSERT_EQUALS(wcdmaCbsMsg0.iNumberOfPages, wcdmaAttributes.iNumberOfPages);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageType,   wcdmaAttributes.iMessageType);
	ASSERT_EQUALS(wcdmaCbsMsg0.iMessageId,     wcdmaAttributes.iMessageId);
	ASSERT_EQUALS(wcdmaCbsMsg0.iSerialNum,     wcdmaAttributes.iSerialNum);
	ASSERT_EQUALS(wcdmaCbsMsg0.iDCS,           wcdmaAttributes.iDCS);

	AssertMockLtsyStatusL();

	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter expectData2 = RMobileBroadcastMessaging::EBroadcastAcceptNone;	
	TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> expectLtsyData2(expectData2);    
    data.Close();
    expectLtsyData2.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, data);
 
    CleanupStack::PopAndDestroy(1); // messaging    
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
 	CleanupStack::PopAndDestroy(2, this); // data, this	
	}
	
//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for StartSimCbTopicBrowsing
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::StartSimCbTopicBrowsing()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyCellBroadcastFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);
    
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	RBuf8 compData;
	CleanupClosePushL(compData);

	CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simTopicArray =  new ( ELeave ) CArrayFixFlat
                            <RMmCustomAPI::TSimCbTopic>(1);	
	CleanupStack::PushL(simTopicArray);
	
	_LIT(KTopic, "Topic");
	RMmCustomAPI::TSimCbTopic topic;
	topic.iNumber = 0;
	topic.iName.Copy(KTopic);

	simTopicArray->AppendL( topic );
	
	TMockLtsyData1< CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* > ltsyData(simTopicArray);	
    ltsyData.SerialiseL(compData); 

	iMockLTSY.ExpectL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId);    
	iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId, KErrGeneral, compData);    
	
	TInt ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrGeneral, ret);
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::StartSimCbTopicBrowsing when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId);
    iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId, KErrNone, compData);

    ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrNone, ret);
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::StartSimCbTopicBrowsing
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(4, this);		
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for DeleteSimCbTopic
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeleteSimCbTopic()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyCellBroadcastFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomApiLC(customAPI);

    TRequestStatus requestStatus;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);
 	
 	InitSimCbTopicsL(customAPI, _L("Topic"), 0);
	
	TUint index(0);
	TUint expIndex(index);
	TBool deleteFlag(ETrue);

	TMockLtsyData2< TUint, TBool > ltsyData2(expIndex, deleteFlag);	
	ltsyData2.SerialiseL(expectData);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId, expectData);    
    iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId, KErrGeneral);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::DeleteSimCbTopic
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::DeleteSimCbTopic when result is not cached.
 	//-------------------------------------------------------------------------

 	index = 0;
 	expIndex  = index;
	deleteFlag = ETrue;
	expectData.Close();
	ltsyData2.SerialiseL(expectData);

    iMockLTSY.ExpectL( MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId, expectData);
    iMockLTSY.CompleteL( MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId, KErrNone);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 	
		
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-CELLBROADCAST-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CancelAsyncRequest()
@SYMTestPriority High
@SYMTestActions Invokes CancelAsyncRequest()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyCellBroadcastFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileBroadcastMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileBroadcastMessaging::ReceiveMessage
 	//-------------------------------------------------------------------------	
	
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter expectData = RMobileBroadcastMessaging::EBroadcastAcceptAll;

	TMockLtsyData1<RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter> expectLtsyData(expectData);
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, data);
    
	_LIT8(KMessage,"CBSMESSAGE");
	TDesC8* cbsMessage = const_cast<TDesC8*>(&KMessage);
	TMockLtsyData1<TDesC8*> completeLtsyData(cbsMessage);
    data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(KMockLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId, KErrNone, data);
    
	TRequestStatus reqStatus;
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 attributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributes(attributes);
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();	
		
 	//-------------------------------------------------------------------------
	// Test cancelling, increase coverage
 	//-------------------------------------------------------------------------

	// setting environment ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// execute request
	messaging.ReceiveMessage(reqStatus, msgData , msgAttributes);
	
	// setting cancel request
	expectData = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, data, KErrNotSupported);
    
    messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);    

	// ReceiveMessage completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
		
 	//-------------------------------------------------------------------------
	// Test cancelling, increase coverage
 	//-------------------------------------------------------------------------

	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// execute request
	messaging.ReceiveMessage(reqStatus, msgData , msgAttributes);
	
	// setting cancel request
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, KErrGeneral);
	
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);

	// ReceiveMessage completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// Test cancelling, increase coverage
 	//-------------------------------------------------------------------------
	
	// setting environement ( CMmBroadcastTsy::iCbRoutingActivated to ETrue)
	iMockLTSY.NotifyTerminated(reqStatus);	
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// execute request
	messaging.ReceiveMessage(reqStatus, msgData, msgAttributes);

	// setting cancel request
	data.Close();
    expectLtsyData.SerialiseL(data); 
    iMockLTSY.ExpectL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId, KErrNone);
  
	messaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);

	// ReceiveMessage completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); // messaging, data, this	
	}
