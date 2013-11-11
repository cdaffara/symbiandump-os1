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
// The TEFUnit test suite for SmsMessaging in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysmsmessagingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsySmsMessagingFU::CreateSuiteL(const TDesC& aName)
    {
    SUB_SUITE;

    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifyMoSmsBearerChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifyReceiveModeChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifyReceiveModeChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifyReceiveModeChange0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessage0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessage0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessage0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessage0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessage0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestAckSmsStored0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestAckSmsStored0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestAckSmsStored0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestAckSmsStored0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNackSmsStored0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNackSmsStored0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNackSmsStored0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNackSmsStored0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetMoSmsBearer0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetReceiveMode0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestReceiveMessage0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestReceiveMessage0001AL);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestReceiveMessage0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestReceiveMessage0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestReceiveMessage0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSetMoSmsBearer0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSetMoSmsBearer0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSetMoSmsBearer0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSetReceiveMode0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSetReceiveMode0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSetReceiveMode0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestStoreSmspListL0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestStoreSmspListL0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestStoreSmspListL0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestStoreSmspListL0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestResumeSmsReception0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestResumeSmsReception0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestResumeSmsReception0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestResumeSmsReception0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetMessageStoreInfo0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetMessageStoreInfo0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetMessageStoreInfo0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetMessageStoreInfo0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetMessageStoreInfo0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifySmspListChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifySmspListChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestNotifySmspListChange0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessageNoFdnCheck0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessageNoFdnCheck0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessageNoFdnCheck0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessageNoFdnCheck0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestSendMessageNoFdnCheck0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestEnumerateMessageStores0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetCaps0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsMessagingFU, TestGetCaps0003L);


    END_SUITE;
    }


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SMSM-SNMSBC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::NotifyMoSmsBearerChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::NotifyMoSmsBearerChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifyMoSmsBearerChange0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging::TMobileSmsBearer smsBearer;
    TRequestStatus reqStatus;
        
    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    messaging.NotifyMoSmsBearerChange(reqStatus, smsBearer);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); 
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SNRMC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::NotifyReceiveModeChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::NotifyReceiveModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifyReceiveModeChange0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging::TMobileSmsReceiveMode smsReceiveMode;
    RMobileSmsMessaging::TMobileSmsReceiveMode smsSetReceiveMode(RMobileSmsMessaging::EReceiveUnstoredClientAck);
    TRequestStatus reqStatus;
    TRequestStatus reqSetStatus;

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::NotifyReceiveModeChange
    // from LTSY.
    //-------------------------------------------------------------------------

    messaging.SetReceiveMode(reqSetStatus, smsSetReceiveMode);

    User::WaitForRequest(reqSetStatus);
    ASSERT_EQUALS(KErrNone, reqSetStatus.Int());
    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::NotifyReceiveModeChange when result is not cached.
    //-------------------------------------------------------------------------

    messaging.NotifyReceiveModeChange(reqStatus, smsReceiveMode);

    messaging.SetReceiveMode(reqSetStatus, smsSetReceiveMode);

    User::WaitForRequest(reqSetStatus);
    ASSERT_EQUALS(KErrNone, reqSetStatus.Int());

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(smsSetReceiveMode, smsReceiveMode);


    //-------------------------------------------------------------------------
    // TEST E2: Unsolicited completion of RMobileSmsMessaging::NotifyReceiveModeChange
    // from LTSY.
    //-------------------------------------------------------------------------

    //Request for EMobileSmsMessagingNotifyReceiveModeChange is self reposting. 
    //If it was completed somehow and then SetReceiveMode() was called without call of NotifyReceiveModeChange() - 
    //this result in getting wrong request handle for completion in CTelObject::ReqCompleted() 
    //{updatedReqEntry=PhoneOwner()->FindByTsyHandleAndPlacedRequest(aTsyReqHandle);
    //__ASSERT_ALWAYS(updatedReqEntry!=NULL, Fault(EEtelFaultCompletionReceivedWithNoReqPackage));}

    messaging.SetReceiveMode(reqSetStatus, smsSetReceiveMode);

    User::WaitForRequest(reqSetStatus);
    ASSERT_EQUALS(KErrNone, reqSetStatus.Int());
    

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); // data, this
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNRMC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::NotifyReceiveModeChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::NotifyReceiveModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifyReceiveModeChange0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging::TMobileSmsReceiveMode smsReceiveMode;
    RMobileSmsMessaging::TMobileSmsReceiveMode smsSetReceiveMode(RMobileSmsMessaging::EReceiveUnstoredClientAck);
    TRequestStatus reqStatus;
    TRequestStatus reqSetStatus;

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::NotifyReceiveModeChange
    //-------------------------------------------------------------------------
    
    messaging.NotifyReceiveModeChange(reqStatus, smsReceiveMode);

    messaging.CancelAsyncRequest(EMobileSmsMessagingNotifyReceiveModeChange);

    messaging.SetReceiveMode(reqSetStatus, smsSetReceiveMode);

    User::WaitForRequest(reqSetStatus);
    ASSERT_EQUALS(KErrNone, reqSetStatus.Int());


    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    CleanupStack::PopAndDestroy(2);
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SNRMC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::NotifyReceiveModeChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::NotifyReceiveModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifyReceiveModeChange0004L()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();


    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    RMobileSmsMessaging::TMobileSmsReceiveMode smsReceiveMode;
    RMobileSmsMessaging::TMobileSmsReceiveMode smsSetReceiveMode(RMobileSmsMessaging::EReceiveUnstoredClientAck);
    TRequestStatus reqStatus;
    TRequestStatus reqSetStatus;

    RMobileSmsMessaging::TMobileSmsReceiveMode smsReceiveMode2;
    TRequestStatus reqStatus2;
    TRequestStatus reqSetStatus2;

    messaging.NotifyReceiveModeChange(reqStatus, smsReceiveMode);
    messaging2.NotifyReceiveModeChange(reqStatus2, smsReceiveMode2);

    messaging.SetReceiveMode(reqSetStatus, smsSetReceiveMode);

    User::WaitForRequest(reqSetStatus);
    ASSERT_EQUALS(KErrNone, reqSetStatus.Int());

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());

    ASSERT_EQUALS(smsSetReceiveMode, smsReceiveMode);
    ASSERT_EQUALS(smsSetReceiveMode, smsReceiveMode2);


    CleanupStack::PopAndDestroy(5, this); 

    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessage0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);
        
    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    _LIT8(KMessage, "Merry christmas");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 expectSmsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessage; 
    dataAndAttr.iAttributes = &expectSmsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data, KErrNotSupported);
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B1: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrGeneral, data, 10);
    
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    data.Close();
    expTsyData.SerialiseL(data);
    
    // complete request by error passing to HandleRequest() in ResendSms()
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data, KErrNotSupported);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
    
    
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B2: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    // the API makes 3 attemps to make the EMobileSmsMessagingSendMessage request
    // we complete all of them with KErrGeneral

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrGeneral, data, 10);
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrGeneral, data, 10);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrGeneral, data, 10);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //--------------------------------------------------------------------------
    // TEST B3: failure on completion of pending request from LTSY->CTSY
    //--------------------------------------------------------------------------
    TMockLtsyData1<TSendSmsDataAndAttributes> expData(dataAndAttr);           
    expData.SerialiseL(data);    
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);
   
    data.Close();
    compTsyData.SerialiseL(data);  
    //simulate error code sent by network when SMS msg has an invalid destination number
    TInt compErrorCode = -298123266;
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, compErrorCode, data, 10);
   
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGsmSMSNetworkOutOfOrder, reqStatus.Int());
   
    AssertMockLtsyStatusL();

    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::SendMessage when result is not cached.
    //-------------------------------------------------------------------------

    _LIT8(KSubmit, "Submit");
    _LIT(KNum, "+441632960000");
    
    smsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iGsmServiceCentre.iTelNumber.Copy(KNum);
    smsAttr.iCdmaServiceCategory = 0;
    smsAttr.iCdmaTeleservice = 0;
    smsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    smsAttr.iFlags = 0;
    smsAttr.iMsgRef = 0;
    smsAttr.iSubmitReport = KSubmit;
    smsAttr.iMore = EFalse;
    smsAttr.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iDestination.iTelNumber.Copy(KNum);
    
    expectSmsAttr = smsAttr;
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrNone, data, 10);
    
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iGsmServiceCentre.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iGsmServiceCentre.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iGsmServiceCentre.iTelNumber.Compare(KNum));
    ASSERT_EQUALS(0, smsAttr.iCdmaServiceCategory);
    ASSERT_EQUALS(0, smsAttr.iCdmaTeleservice);
    ASSERT_EQUALS(RMobileSmsMessaging::EFormatGsmTpdu, smsAttr.iDataFormat);
    ASSERT_TRUE((RMobileSmsMessaging::KMessageReference | RMobileSmsMessaging::KGsmSubmitReport)
         == smsAttr.iFlags);
    ASSERT_TRUE(msgRef == smsAttr.iMsgRef);
    ASSERT_EQUALS(0, smsAttr.iSubmitReport.Compare(smsMsg));
    ASSERT_TRUE(EFalse == smsAttr.iMore);
    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iDestination.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iDestination.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iDestination.iTelNumber.Compare(KNum));

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C2: Successful completion request of
    // RMobileSmsMessaging::SendMessage when result is not cached.
    //-------------------------------------------------------------------------

    smsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iGsmServiceCentre.iTelNumber.Copy(KNum);
    smsAttr.iCdmaServiceCategory = 0;
    smsAttr.iCdmaTeleservice = 0;
    smsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    smsAttr.iFlags = 0;
    smsAttr.iMsgRef = 0;
    smsAttr.iSubmitReport = KSubmit;
    smsAttr.iMore = EFalse;
    smsAttr.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iDestination.iTelNumber.Copy(KNum);

    expectSmsAttr = smsAttr;

    tmpName.SetLength(0);
    smsMsg.SetLength(0);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrNone, data, 10);
    
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iGsmServiceCentre.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iGsmServiceCentre.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iGsmServiceCentre.iTelNumber.Compare(KNum));
    ASSERT_EQUALS(0, smsAttr.iCdmaServiceCategory);
    ASSERT_EQUALS(0, smsAttr.iCdmaTeleservice);
    ASSERT_EQUALS(RMobileSmsMessaging::EFormatGsmTpdu, smsAttr.iDataFormat);
    ASSERT_TRUE(RMobileSmsMessaging::KMessageReference == smsAttr.iFlags);
    ASSERT_TRUE(KMsgRef == smsAttr.iMsgRef);
    ASSERT_EQUALS(0, smsAttr.iSubmitReport.Compare(KSubmit));
    ASSERT_TRUE(EFalse == smsAttr.iMore);
    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iDestination.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iDestination.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iDestination.iTelNumber.Compare(KNum));

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::SendMessage
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrNone, data, 10);
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessage0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    _LIT8(KMessage, "Merry christmas");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessage; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::SendMessage
    //-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrNone, data, 10);

    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    messaging.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
    
    User::WaitForRequest(reqStatus);
    // cancel does not work for SendMessage(), this is correct
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3);
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessage with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessage with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessage0003L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    TBuf8<1> tmpName;
    TInt smsAttr(1);
    TPckg<TInt8> badSmsAttrPckg(smsAttr);

    messaging.SendMessage(reqStatus, tmpName, badSmsAttrPckg);
        
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2); // messaging, this
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessage0004L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 data2;
    CleanupClosePushL(data2);

    _LIT8(KMessage, "Merry christmas");
    _LIT8(KMessage2, "Merry christmas2");
    TBuf8<32> tmpName(KMessage);
    TBuf8<32> tmpName2(KMessage2);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TRequestStatus reqStatus2;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr2;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg2(smsAttr2);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessage; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessage, KErrNone, data);

    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    messaging2.SendMessage(reqStatus2, tmpName2, smsAttrPckg2);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(7);

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessage with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessage and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessage0005L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    _LIT8(KMessage, "Merry christmas");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessage; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessage, data);

    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3);

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SASS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::AckSmsStored
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::AckSmsStored
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestAckSmsStored0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    
    _LIT8(KMessage, "Happy New Year");

    TRequestStatus reqStatus;
    TRequestStatus mockLtsyStatus;
    
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);

    TMockLtsyData1<TDesC8*> expTsyData(msgPtr);
    
    TDesC8* zeroPtr(NULL);
    TMockLtsyData1<TDesC8*> expZeroTsyData(zeroPtr);
    
    //-------------------------------------------------------------------------
    // TEST A1: check AckSmsStored() when acknoledgement is not expected
    //-------------------------------------------------------------------------

    expTsyData.SerialiseL(data);
 
    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotReady, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A2: unsuccessful acknoledgement request to the network
    //-------------------------------------------------------------------------
    
    // make the server expect an acknowledgement
    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data, KErrGeneral);

    messaging.AckSmsStored(reqStatus, KMessage, EFalse);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
 
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A3: unsuccessful acknoledgement completion
    //-------------------------------------------------------------------------
    
    // make the server expect an acknowledgement
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);

    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrGeneral);

    messaging.AckSmsStored(reqStatus, KMessage, EFalse);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
 
    AssertMockLtsyStatusL();    
    
    
    //-------------------------------------------------------------------------
    // TEST B1, test AckSmsStored() when there are sms received
    // that have iDeleteAfterClientAck = true
    //-------------------------------------------------------------------------

    // Since Ack failed, the CTSY needs to ask to activate the sms routing again
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    // make the server expect an acknowledgement
    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());

    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);    
    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone);

    messaging.AckSmsStored(reqStatus, KMessage, EFalse);
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();           
    

    //-------------------------------------------------------------------------
    // TEST B2, check AckSmsStored() when there are sms received 
    // but they have iDeleteAfterClientAck = false
    //-------------------------------------------------------------------------

    // make the server expect an acknowledgement
    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);

    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsMsg.Copy(KMessage);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());

    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);    
    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone);

    messaging.AckSmsStored(reqStatus, KMessage, EFalse);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::AckSmsStored
    // from LTSY.
    //-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3, this); // data, messaging, this
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SASS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::AckSmsStored
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::AckSmsStored
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestAckSmsStored0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    _LIT8(KMessage, "Happy New Year");

    // make the server expect an acknowledgement
    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;
    
    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
    
    TRequestStatus reqStatus;
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);

    TMockLtsyData1<TDesC8*> expTsyData(msgPtr);
    
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);
    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone);     

    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    messaging.CancelAsyncRequest(EMobileSmsMessagingAckSmsStored);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    
    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3); 
    
    }

/**
@SYMTestCaseID BA-CTSY-SMSM-SASS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::AckSmsStored
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::AckSmsStored
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestAckSmsStored0004L()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    
    _LIT8(KMessage, "Happy New Year");
    _LIT8(KMessage2, "Happy New Year2");    
    
    // make the server expect an acknowledgement

    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;
    
    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
    
    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;

    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);

    TMockLtsyData1<TDesC8*> expTsyData(msgPtr);
    
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);
    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone, 10);     

    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    messaging2.AckSmsStored(reqStatus2, KMessage2, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SASS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::AckSmsStored with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::AckSmsStored and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestAckSmsStored0005L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    
    _LIT8(KMessage, "Happy New Year");

    // make the server expect an acknowledgement 

    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;
    
    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
    
    TRequestStatus reqStatus;
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);

    TMockLtsyData1<TDesC8*> expTsyData(msgPtr);
    
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);

    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, this); 
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::NackSmsStored
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::NackSmsStored
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNackSmsStored0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    
    _LIT8(KMessage, "Happy New Year");

    TRequestStatus reqStatus;
    TRequestStatus mockLtsyStatus;
    
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    TInt rpCause(0);
    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);

    
    //-------------------------------------------------------------------------
    // TEST A1: check NackSmsStored() when acknoledgement is not expected
    //-------------------------------------------------------------------------

    expTsyData.SerialiseL(data);
 
    messaging.NackSmsStored(reqStatus, KMessage, rpCause);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotReady, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A2: unsuccessful completion
    //-------------------------------------------------------------------------
    
    // make the server expect an acknowledgement
    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);

    iMockLTSY.CompleteL(EMobileSmsMessagingNackSmsStored, KErrGeneral);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
 
    AssertMockLtsyStatusL();
    
    
    //-------------------------------------------------------------------------
    // TEST B1, test NackSmsStored() when there are sms received
    // that have iDeleteAfterClientAck = true
    //-------------------------------------------------------------------------
  
    // Since Nack failed, the CTSY needs to ask to activate the sms routing again  
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    // make the server expect an acknowledgement
    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);

    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());

    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);    
    iMockLTSY.CompleteL(EMobileSmsMessagingNackSmsStored, KErrNone);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();           
    

    //-------------------------------------------------------------------------
    // TEST B2, check NackSmsStored() when there are sms received 
    // but they have iDeleteAfterClientAck = false
    //-------------------------------------------------------------------------

    // make the server expect an acknowledgement
    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);

    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsMsg.Copy(KMessage);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());

    // The server now is expecting the acknowledgement
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);    
    iMockLTSY.CompleteL(EMobileSmsMessagingNackSmsStored, KErrNone);
    
    messaging.NackSmsStored(reqStatus, KMessage, rpCause);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::AckSmsStored
    // from LTSY.
    //-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingNackSmsStored, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3, this); // data, messaging, this
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNSS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::NackSmsStored
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::NackSmsStored
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNackSmsStored0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);


    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    _LIT8(KMessage, "Happy New Year");
    
    TRequestStatus reqStatus;
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    TInt rpCause(0);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::NackSmsStored
    //-------------------------------------------------------------------------

    // make the server expect an acknowledgement
    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement
     
    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);

    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);
    iMockLTSY.CompleteL(EMobileSmsMessagingNackSmsStored, KErrNone);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 
    
    messaging.CancelAsyncRequest(EMobileSmsMessagingNackSmsStored);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());       

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    
    AssertMockLtsyStatusL();
        
    CleanupStack::PopAndDestroy(3); 
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SNSS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::NackSmsStored
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::NackSmsStored
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNackSmsStored0004L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);


    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;
    
    _LIT8(KMessage, "Happy New Year");
    _LIT8(KMessage2, "Happy New Year2");

    // make the server expect an acknowledgement
    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement    
    
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    TInt rpCause(0);

    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);
    
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);
    iMockLTSY.CompleteL(EMobileSmsMessagingNackSmsStored, KErrNone, 10);        

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 

    TInt rpCause2(0);

    messaging2.NackSmsStored(reqStatus2, KMessage2, rpCause2); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNSS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::NackSmsStored with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::NackSmsStored and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNackSmsStored0005L()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    _LIT8(KMessage, "Happy New Year");

    // make the server expect an acknowledgement
    TRequestStatus reqReceiveStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);

    TUint8 smsRoutingStatus ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);    
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    TBuf8<100> forMsg;

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqReceiveStatus, forMsg, receiveAttrPckg);
    User::WaitForRequest(reqReceiveStatus);
    ASSERT_EQUALS(KErrNone, reqReceiveStatus.Int());
    
    // The server now is expecting the acknowledgement        
    
    TRequestStatus reqStatus;
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    TInt rpCause(0);   


    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);

    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
    
    AssertMockLtsyStatusL();    

    CleanupStack::PopAndDestroy(3); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SGMSB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetMoSmsBearer
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetMoSmsBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetMoSmsBearer0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging::TMobileSmsBearer bearer;
    
    err = messaging.GetMoSmsBearer(bearer);
    ASSERT_EQUALS(KErrNotSupported, err);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); 
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SGRM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetReceiveMode
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetReceiveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetReceiveMode0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging::TMobileSmsReceiveMode mode;
    
    err = messaging.GetReceiveMode(mode);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RMobileSmsMessaging::EReceiveUnstoredClientAck, mode);

    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsReceiveMode modeToSet(RMobileSmsMessaging::EReceiveStored);

    messaging.SetReceiveMode(reqStatus, modeToSet);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    err = messaging.GetReceiveMode(mode);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RMobileSmsMessaging::EReceiveUnstoredClientAck, mode);


    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); 
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SRM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestReceiveMessage0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsStore store;

    err = store.Open(messaging, KETelMeSmsStore);
    ASSERT_EQUALS(KErrNotSupported, err);
    
    
    iMockLTSY.ExpectL(EMobilePhoneStoreGetInfo, KErrNone);
    err = store.Open(messaging, KETelIccSmsStore);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(store);
 
    RBuf8 data;
    CleanupClosePushL(data);

    TRequestStatus reqStatus;
    TRequestStatus mockLtsyStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
  
    _LIT8(KMessage, "Happy New Year");
    TBuf8<100> forMsg;
        
    //-------------------------------------------------------------------------
    // TEST A1: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting, KErrNotSupported);

    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);

    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGeneral, data);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::ReceiveMessage
    // from LTSY.
    //-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    TDesC8* zeroPtr(NULL);
    TInt rpCause(KErrGsmSMSUnspecifiedProtocolError);
    TMockLtsyData2<TDesC8*, TInt> expTsyData(zeroPtr, rpCause);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    
    TMockLtsyData1<TDesC8*> expAckTsyData(msgPtr);
    
    data.Close();
    expAckTsyData.SerialiseL(data);

    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compRecTsyData(ind, smsMsgPtr);
    
    data.Close();
    compRecTsyData.SerialiseL(data);

    //-------------------------------------------------------------------------
    // successful completion of RMobileSmsMessaging::ReceiveMessage 
    // in case when iClientStorageFull = false
    //-------------------------------------------------------------------------

    data.Close();
    expAckTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);
    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    //completion with KErrGsmSMSUnspecifiedProtocolError and iClientStorageFull = false
  
    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGsmSMSUnspecifiedProtocolError, data);

    rpCause = KErrGsmSMSUnspecifiedProtocolError;
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
   
    
    //-------------------------------------------------------------------------
    // successful completion of RMobileSmsMessaging::ReceiveMessage 
    // in case when iClientStorageFull = false and iSmsClass2 = EFalse
    //-------------------------------------------------------------------------


    data.Close();
    expAckTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);
    
    TRequestStatus reqAckStatus;
    
    messaging.AckSmsStored(reqAckStatus, KMessage, EFalse);

    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone);

    User::WaitForRequest(reqAckStatus);
    ASSERT_EQUALS(KErrNone, reqAckStatus.Int());

    AssertMockLtsyStatusL();

    smsMsg.iSmsClass2 = EFalse;

    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);
    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);
    messaging.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // Unsolicited completion of RMobileSmsMessaging::ReceiveMessage 
    // in case when (iClientStorageFull && smsClass2)
    //-------------------------------------------------------------------------    

    data.Close();
    expAckTsyData.SerialiseL(data);
    
    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);
    messaging.AckSmsStored(reqStatus, KMessage, ETrue);
    iMockLTSY.CompleteL(EMobileSmsMessagingAckSmsStored, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    smsMsg.iSmsClass2 = ETrue;
    
    data.Close();
    compRecTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    TMockLtsyData1<TDesC8*> expZeroAckTsyData(zeroPtr);
    data.Close();
    expZeroAckTsyData.SerialiseL(data);
    
    iMockLTSY.ExpectL(EMobileSmsMessagingAckSmsStored, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();    
    
    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::ReceiveMessage when result is not cached.
    //-------------------------------------------------------------------------

    // smsStoreArray is not zero, complete ReceiveMessage() from DeliverClass2ToSmsStack()
   
    TRequestStatus reqResumeStatus;
    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    iMockLTSY.CompleteL(EMobileSmsMessagingResumeSmsReception, KErrNone);
    messaging.ResumeSmsReception(reqResumeStatus);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);
    
    User::WaitForRequest(reqResumeStatus);
    ASSERT_EQUALS(KErrNone, reqResumeStatus.Int());
    
    TRequestStatus reqRecStatus;
    messaging.ReceiveMessage(reqRecStatus, forMsg, receiveAttrPckg);    
    
    User::WaitForRequest(reqRecStatus);
    ASSERT_EQUALS(KErrNone, reqRecStatus.Int());

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TUint8 smsRoutingStatus1 ( KSmsRoutingActivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData1(smsRoutingStatus1);
    data.Close();
    compSmsRoutingTsyData1.SerialiseL(data);
    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrNone, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    messaging.AckSmsStored(reqAckStatus, KMessage, ETrue);

    User::WaitForRequest(reqAckStatus);
    ASSERT_EQUALS(KErrNone, reqAckStatus.Int());

    AssertMockLtsyStatusL();

    //completion of receive request with rpError = KErrGsmSMSMemoryCapacityExceeded
    
    
    smsMsg.iSmsClass2 = EFalse;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsMsg.Copy(KMessage);

    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    rpCause = KErrGsmSMSMemoryCapacityExceeded;
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    //completion using KErrGsmSMSUnspecifiedProtocolError

    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGsmSMSUnspecifiedProtocolError, data);

    rpCause = KErrGsmSMSMemoryCapacityExceeded;
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    //completion using KErrGsmSMSMemoryCapacityExceeded and 
    // setting iServerRoutingActivity = ERoutingNotActivated
    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGsmSMSMemoryCapacityExceeded, data);

    rpCause = KErrGsmSMSMemoryCapacityExceeded;
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data, KErrNotSupported);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    AssertMockLtsyStatusL();

    //completion of EMobileSmsMessagingReceiveMessage with KErrGeneral
    ind = ETrue;

    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGeneral, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    
    //completion of EMmTsyActivateSmsRouting with KErrGeneral
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TUint8 smsRoutingStatus ( KSmsRoutingDeactivated );
    TMockLtsyData1<TUint8> compSmsRoutingTsyData(smsRoutingStatus);
    data.Close();
    compSmsRoutingTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMmTsyActivateSmsRouting, KErrGeneral, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    AssertMockLtsyStatusL();   
    
    //Unsolicited completion of EMobileSmsMessagingReceiveMessage with KErrGeneral
    
    data.Close();
    compRecTsyData.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGeneral, data);

    rpCause = KErrGsmSMSMemoryCapacityExceeded; // set to mem capacity exceeded so that base will resend the message later.
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(4, this); 
    
    }

/**
@SYMTestCaseID BA-CTSY-SMSM-SRM-0001A
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for  RMobileSmsMessaging::ReceiveMessage before modem is ready
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestReceiveMessage0001AL()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));


    RTelServer telServer;
    TInt ret = telServer.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer);

    RMobilePhone phone;
    ret = phone.Open(telServer,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(phone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    err=iMockLTSY.Connect();
    ASSERT_EQUALS(KErrNone, err);
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test activation of RMobileSmsMessaging::ReceiveMessage
    //-------------------------------------------------------------------------
    
    TRequestStatus reqStatus;
    TBuf8<100> forMsg;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);


    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    
    CleanupStack::PopAndDestroy(5); 
    
    }

/**
@SYMTestCaseID BA-CTSY-SMSM-SRM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestReceiveMessage0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::ReceiveMessage
    //-------------------------------------------------------------------------
    
    TRequestStatus reqStatus;
    TBuf8<100> forMsg;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);


    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrGeneral, data);

    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);
    
    messaging.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());
    
    // this is used for proper processing of unsolicited complete for EMobileSmsMessagingReceiveMessage
    TDesC8* zeroPtr(NULL);
    TInt rpCause(KErrGsmSMSUnspecifiedProtocolError);
    TMockLtsyData2<TDesC8*, TInt> expTsyData(zeroPtr, rpCause);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingNackSmsStored, data);
    
    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    CleanupStack::PopAndDestroy(3); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SRM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::ReceiveMessage with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::ReceiveMessage with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestReceiveMessage0003L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    //-------------------------------------------------------------------------
    // Failure of completion with bad MsgAttribute
    //-------------------------------------------------------------------------
    
    TRequestStatus status1;
    const TInt KSmsBufLength = 231;
    TBuf8<KSmsBufLength> normalBuf;
    TInt8 tmp;
    TPckg<TInt8> badPckg(tmp);

    messaging.ReceiveMessage(status1, normalBuf, badPckg);
    
    User::WaitForRequest(status1);    
    ASSERT_EQUALS(KErrArgument, status1.Int());
    
    AssertMockLtsyStatusL();    
    
    
    //-------------------------------------------------------------------------
    // Failure of completion with too small data buffer
    //-------------------------------------------------------------------------
   
    TRequestStatus status2;
    RBuf8 data;
    CleanupClosePushL(data);

    TBuf8<1> badBuf;
    
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
    
    _LIT8(KMessage, "Happy New Year");
    
    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = EFalse;
    smsMsg.iSmsMsg.Copy(KMessage);
    
    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    messaging.ReceiveMessage(status2, badBuf, receiveAttrPckg);

    User::WaitForRequest(status2);    
    ASSERT_EQUALS(KErrArgument, status2.Int());    

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3); 

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SRM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestReceiveMessage0004L()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobileSmsMessaging::ReceiveMessage
    //-------------------------------------------------------------------------

    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;
    TBuf8<100> forMsg;
    TBuf8<100> forMsg2;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr2;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg2(receiveAttr2);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);

    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);

    messaging2.ReceiveMessage(reqStatus2, forMsg2, receiveAttrPckg2);


    
    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);



    TSmsMsg smsMsg2;
    TSmsMsg* smsMsgPtr2(&smsMsg2);
    TBool ind2(EFalse);
    
    TMockLtsyData2<TBool, TSmsMsg*> compTsyData2(ind2, smsMsgPtr2);
    data.Close();
    compTsyData2.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingReceiveMessage, KErrNone, data);

    iMockLTSY.ExpectL(EMmTsyActivateSmsRouting);


    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 360502);
    //request for EMobileSmsMessagingReceiveMessage in case of mutiple clients hangs on 
    //User::WaitForRequest() untill testexecute aborts the test with timeout
    
    ASSERT_TRUE(EFalse);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(5, this); 

    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSMSB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SetMoSmsBearer
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SetMoSmsBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSetMoSmsBearer0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsBearer smsBearer;
    

    TMockLtsyData1<RMobileSmsMessaging::TMobileSmsBearer> expTsyData(smsBearer);

    //-------------------------------------------------------------------------
    // TEST A1: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    // -1 means an invalid value, which is out of bounds of enum TMobileSmsBearer
    smsBearer = (RMobileSmsMessaging::TMobileSmsBearer) -1;
    messaging.SetMoSmsBearer(reqStatus, smsBearer);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());


    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    smsBearer = RMobileSmsMessaging::ESmsBearerPacketOnly;
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSetMoSmsBearer, data);
    
    messaging.SetMoSmsBearer(reqStatus, smsBearer);

    iMockLTSY.CompleteL(EMobileSmsMessagingSetMoSmsBearer, KErrGeneral);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::SetMoSmsBearer when result is not cached.
    //-------------------------------------------------------------------------

    smsBearer = RMobileSmsMessaging::ESmsBearerPacketPreferred;

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSetMoSmsBearer, data);
    
    messaging.SetMoSmsBearer(reqStatus, smsBearer);

    iMockLTSY.CompleteL(EMobileSmsMessagingSetMoSmsBearer, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::SetMoSmsBearer
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingSetMoSmsBearer, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A2: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------
    smsBearer = RMobileSmsMessaging::ESmsBearerCircuitOnly;

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSetMoSmsBearer, data, KErrNotSupported);
    
    
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 360601);

    //Test A for EMobileSmsMessagingSetMoSmsBearer (failure to dispatch request to LTSY) 
    //result in unknown exception in request completion in ETel::CompleteAndDestroyReq(), 
    //more concrete - in Deque() function

    ASSERT_TRUE(EFalse);
    
    messaging.SetMoSmsBearer(reqStatus, smsBearer);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); // data, this
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSMSB-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::SetMoSmsBearer
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::SetMoSmsBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSetMoSmsBearer0002L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsBearer smsBearer(RMobileSmsMessaging::ESmsBearerCircuitPreferred);
    

    TMockLtsyData1<RMobileSmsMessaging::TMobileSmsBearer> expTsyData(smsBearer);

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::SetMoSmsBearer
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSetMoSmsBearer, data);
    iMockLTSY.CompleteL(EMobileSmsMessagingSetMoSmsBearer, KErrNone, 10);
    
    messaging.SetMoSmsBearer(reqStatus, smsBearer);
    
    messaging.CancelAsyncRequest(EMobileSmsMessagingSetMoSmsBearer);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3); // data, this
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSMSB-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::SetMoSmsBearer
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::SetMoSmsBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSetMoSmsBearer0004L()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);


    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;
    RMobileSmsMessaging::TMobileSmsBearer smsBearer(RMobileSmsMessaging::ESmsBearerCircuitPreferred);
    RMobileSmsMessaging::TMobileSmsBearer smsBearer2(RMobileSmsMessaging::ESmsBearerPacketOnly);


    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobileSmsMessaging::SetMoSmsBearer
    //-------------------------------------------------------------------------

    TMockLtsyData1<RMobileSmsMessaging::TMobileSmsBearer> expTsyData(smsBearer);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSetMoSmsBearer, data);

    messaging.SetMoSmsBearer(reqStatus, smsBearer);

    messaging2.SetMoSmsBearer(reqStatus2, smsBearer2);

    
    iMockLTSY.CompleteL(EMobileSmsMessagingSetMoSmsBearer, KErrNone);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSRM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SetReceiveMode
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SetReceiveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSetReceiveMode0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsReceiveMode recMode(RMobileSmsMessaging::EReceiveModeUnspecified);
    
    //-------------------------------------------------------------------------
    // TEST A1: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    messaging.SetReceiveMode(reqStatus, recMode);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());


    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::SetReceiveMode when result is not cached.
    //-------------------------------------------------------------------------

    recMode = RMobileSmsMessaging::EReceiveUnstoredClientAck;

    messaging.SetReceiveMode(reqStatus, recMode);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSRM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::SetReceiveMode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::SetReceiveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSetReceiveMode0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsReceiveMode recMode(RMobileSmsMessaging::EReceiveUnstoredClientAck);
    
    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::SetReceiveMode
    //-------------------------------------------------------------------------
    messaging.SetReceiveMode(reqStatus, recMode);
    
    messaging.CancelAsyncRequest(EMobileSmsMessagingSetReceiveMode);

    User::WaitForRequest(reqStatus);

    // Since SetReceiveMode completes without any delays, the cancel will not have any effect.
    // The CMmSmsTsy::SetReceiveModeCancel should be removed from the code in the next cleanup, since it never been called.
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();


    CleanupStack::PopAndDestroy(2); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSRM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::SetReceiveMode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::SetReceiveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSetReceiveMode0004L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);


    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;
    RMobileSmsMessaging::TMobileSmsReceiveMode recMode(RMobileSmsMessaging::EReceiveUnstoredClientAck);
    RMobileSmsMessaging::TMobileSmsReceiveMode recMode2(RMobileSmsMessaging::EReceiveEither);


    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobileSmsMessaging::SetReceiveMode
    //-------------------------------------------------------------------------

    messaging.SetReceiveMode(reqStatus, recMode);

    messaging2.SetReceiveMode(reqStatus2, recMode2);

    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNotSupported, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSSL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::StoreSmspListL
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::StoreSmspListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestStoreSmspListL0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);
    
    //-------------------------------------------------------------------------
    // TEST A0: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------
    
    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    
    smspListPtr->AddEntryL(smspEntry);

    TMockLtsyData1<CMobilePhoneSmspList*> expTsyData(smspListPtr);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data, KErrNotSupported);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrGeneral);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::StoreSmspListL when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::StoreSmspListL
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(4, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSSL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::StoreSmspListL
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::StoreSmspListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestStoreSmspListL0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    
    smspListPtr->AddEntryL(smspEntry);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::StoreSmspListL
    //-------------------------------------------------------------------------

    TMockLtsyData1<CMobilePhoneSmspList*> expTsyData(smspListPtr);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data, KErrNone);
    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrNone, 20);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 
    
    messaging.CancelAsyncRequest(EMobileSmsMessagingStoreSmspList);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    
    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(4);
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSSL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::StoreSmspListL
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::StoreSmspListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestStoreSmspListL0004L()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);

    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;    
    smspListPtr->AddEntryL(smspEntry);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobileSmsMessaging::StoreSmspListL
    //-------------------------------------------------------------------------

    TMockLtsyData1<CMobilePhoneSmspList*> expTsyData(smspListPtr);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data, KErrNone);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 


    messaging2.StoreSmspListL(reqStatus2, smspListPtr); 

    
    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());


    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(7, this); 
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSSL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::StoreSmspListL with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::StoreSmspListL and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestStoreSmspListL0005L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    
    smspListPtr->AddEntryL(smspEntry);

    TMockLtsyData1<CMobilePhoneSmspList*> expTsyData(smspListPtr);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data, KErrNone);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(4);
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SRSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::ResumeSmsReception
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::ResumeSmsReception
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestResumeSmsReception0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;

    
    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception, KErrNotSupported);

    messaging.ResumeSmsReception(reqStatus);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    messaging.ResumeSmsReception(reqStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingResumeSmsReception, KErrGeneral);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::ResumeSmsReception when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    messaging.ResumeSmsReception(reqStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingResumeSmsReception, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::ResumeSmsReception
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingResumeSmsReception, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // additional test
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TRfStateInfo rfInfo = ERfsStateInfoInactive;
    TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
    
    mockData1.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    
    messaging.ResumeSmsReception(reqStatus);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    rfInfo = ERfsStateInfoNormal;
    
    data.Close();
    mockData1.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
    
    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    rfInfo = ERfsStateInfoInactive;    
    data.Close();
    mockData1.SerialiseL(data);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    rfInfo = ERfsStateInfoNormal;    
    data.Close();
    mockData1.SerialiseL(data);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SRSR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::ResumeSmsReception
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::ResumeSmsReception
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestResumeSmsReception0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::ResumeSmsReception
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);
    iMockLTSY.CompleteL(EMobileSmsMessagingResumeSmsReception, KErrNone, 10);

    messaging.ResumeSmsReception(reqStatus);
    
    messaging.CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    
    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(2); 
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SRSR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::ResumeSmsReception
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::ResumeSmsReception
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestResumeSmsReception0004L()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobileSmsMessaging::ResumeSmsReception
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    messaging.ResumeSmsReception(reqStatus);

    messaging2.ResumeSmsReception(reqStatus2);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingResumeSmsReception, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SRSR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::ResumeSmsReception with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::ResumeSmsReception and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestResumeSmsReception0005L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;

    iMockLTSY.ExpectL(EMobileSmsMessagingResumeSmsReception);

    messaging.ResumeSmsReception(reqStatus);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(2);

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SGMSI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetMessageStoreInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetMessageStoreInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetMessageStoreInfo0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo(info);
    TInt index(0);
    
    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo, KErrNotSupported);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    TInt total(0);
    TInt used(0);

    TMockLtsyData2<TInt, TInt> compData(total, used);
    
    compData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingGetMessageStoreInfo, KErrGeneral, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::GetMessageStoreInfo when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    total = 2;
    used = 1;

    data.Close();
    compData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingGetMessageStoreInfo, KErrNone, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(0, info.iName.Compare(KETelIccSmsStore));
    ASSERT_EQUALS(RMobilePhoneStore::EShortMessageStore, info.iType);
    ASSERT_TRUE(total == info.iTotalEntries);
    ASSERT_TRUE(used == info.iUsedEntries);
    ASSERT_EQUALS(KSmsMessageStoreInfoCaps, info.iCaps);

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C2: Successful completion request of
    // RMobileSmsMessaging::GetMessageStoreInfo when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    iMockLTSY.CompleteL(EMobileSmsMessagingGetMessageStoreInfo, KErrNotFound, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(0, info.iName.Compare(KETelIccSmsStore));
    ASSERT_EQUALS(RMobilePhoneStore::EShortMessageStore, info.iType);
    ASSERT_TRUE(total == info.iTotalEntries);
    ASSERT_TRUE(used == info.iUsedEntries);
    ASSERT_EQUALS(KSmsMessageStoreInfoCaps, info.iCaps);

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::GetMessageStoreInfo
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingGetMessageStoreInfo, KErrNone, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SGMSI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::GetMessageStoreInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::GetMessageStoreInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetMessageStoreInfo0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo(info);
    TInt index(0);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::GetMessageStoreInfo
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo);

    TInt total(5);
    TInt used(2);

    TMockLtsyData2<TInt, TInt> compData(total, used);
    
    compData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMobileSmsMessagingGetMessageStoreInfo, KErrNone, data, 10);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    messaging.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SGMSI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetMessageStoreInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetMessageStoreInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetMessageStoreInfo0003L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo(info);
    TInt index(KSmsStoreNumber + 1);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());

    AssertMockLtsyStatusL();

    //    
    index = -1;

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3); // messaging, data, this
    }

/**
@SYMTestCaseID BA-CTSY-SMSM-SGMSI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::GetMessageStoreInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::GetMessageStoreInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetMessageStoreInfo0004L()
    {

                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    TRequestStatus reqStatus;
    TRequestStatus reqStatus2;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info2;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo(info);
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo2(info2);
    TInt index(0);
    TInt index2(0);

    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    messaging2.GetMessageStoreInfo(reqStatus2, index2, pckgInfo2);

    TInt total(5);
    TInt used(2);

    TMockLtsyData2<TInt, TInt> compData(total, used);
    
    compData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingGetMessageStoreInfo, KErrNone, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SGMSI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetMessageStoreInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetMessageStoreInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetMessageStoreInfo0005L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
    RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg pckgInfo(info);
    TInt index(0);

    iMockLTSY.ExpectL(EMobileSmsMessagingGetMessageStoreInfo);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3); 
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNSLC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::NotifySmspListChange
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::NotifySmspListChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifySmspListChange0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);


    RBuf8 data;
    CleanupClosePushL(data);


    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::NotifySmspListChange when result is not cached.
    //-------------------------------------------------------------------------

    TRequestStatus reqNotifyStatus;
    
    messaging.NotifySmspListChange(reqNotifyStatus);


    TRequestStatus reqStatus;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);

    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    
    smspListPtr->AddEntryL(smspEntry);

    TMockLtsyData1<CMobilePhoneSmspList*> expTsyData(smspListPtr);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data, KErrNone);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 
    
    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqNotifyStatus);
    ASSERT_EQUALS(KErrNone, reqNotifyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(4, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNSLC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::NotifySmspListChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::NotifySmspListChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifySmspListChange0002L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::NotifySmspListChange
    //-------------------------------------------------------------------------
    TRequestStatus reqNotifyStatus;
    
    messaging.NotifySmspListChange(reqNotifyStatus);

    messaging.CancelAsyncRequest(EMobileSmsMessagingNotifySmspListChange);

    User::WaitForRequest(reqNotifyStatus);
    ASSERT_EQUALS(KErrCancel, reqNotifyStatus.Int());
    
    CleanupStack::PopAndDestroy(2); // data, this
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SNSLC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::NotifySmspListChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::NotifySmspListChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestNotifySmspListChange0004L()
    {

                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobileSmsMessaging::NotifySmspListChange
    //-------------------------------------------------------------------------
    TRequestStatus reqNotifyStatus;
    TRequestStatus reqNotifyStatus2;
    
    messaging.NotifySmspListChange(reqNotifyStatus);
    messaging2.NotifySmspListChange(reqNotifyStatus2);


    TRequestStatus reqStatus;
    CMobilePhoneSmspList* smspListPtr = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspListPtr);

    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    
    smspListPtr->AddEntryL(smspEntry);

    TMockLtsyData1<CMobilePhoneSmspList*> expTsyData(smspListPtr);
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingStoreSmspList, data, KErrNone);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 
    
    iMockLTSY.CompleteL(EMobileSmsMessagingStoreSmspList, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqNotifyStatus);
    ASSERT_EQUALS(KErrNone, reqNotifyStatus.Int());

    User::WaitForRequest(reqNotifyStatus2);
    ASSERT_EQUALS(KErrNone, reqNotifyStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(7, this);

    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessageNoFdnCheck0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);


    _LIT8(KMessage, "Merry christmas");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessageNoFdnCheck; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    
    expTsyData.SerialiseL(data);
    
    //-------------------------------------------------------------------------
    // TEST B1: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrGeneral, data, 10);
    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    data.Close();
    expTsyData.SerialiseL(data);
    
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data, KErrNotSupported);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B2: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrGeneral, data, 10);
    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrGeneral, data, 10);

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrGeneral, data, 10);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::SendMessage when result is not cached.
    //-------------------------------------------------------------------------

    _LIT8(KSubmit, "Submit");
    _LIT(KNum, "+441632960000");
    
    smsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iGsmServiceCentre.iTelNumber.Copy(KNum);
    smsAttr.iCdmaServiceCategory = 0;
    smsAttr.iCdmaTeleservice = 0;
    smsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    smsAttr.iFlags = 0;
    smsAttr.iMsgRef = 0;
    smsAttr.iSubmitReport = KSubmit;
    smsAttr.iMore = EFalse;
    smsAttr.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iDestination.iTelNumber.Copy(KNum);
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrNone, data, 10);
    
    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iGsmServiceCentre.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iGsmServiceCentre.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iGsmServiceCentre.iTelNumber.Compare(KNum));
    ASSERT_EQUALS(0, smsAttr.iCdmaServiceCategory);
    ASSERT_EQUALS(0, smsAttr.iCdmaTeleservice);
    ASSERT_EQUALS(RMobileSmsMessaging::EFormatGsmTpdu, smsAttr.iDataFormat);
    ASSERT_TRUE((RMobileSmsMessaging::KMessageReference | RMobileSmsMessaging::KGsmSubmitReport)
         == smsAttr.iFlags);
    ASSERT_TRUE(KMsgRef == smsAttr.iMsgRef);
    ASSERT_EQUALS(0, smsAttr.iSubmitReport.Compare(KMessage));
    ASSERT_TRUE(EFalse == smsAttr.iMore);
    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iDestination.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iDestination.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iDestination.iTelNumber.Compare(KNum));

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C2: Successful completion request of
    // RMobileSmsMessaging::SendMessage when result is not cached.
    //-------------------------------------------------------------------------
    
    tmpName.SetLength(0);
    smsMsg.SetLength(0);

    smsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iGsmServiceCentre.iTelNumber.Copy(KNum);
    smsAttr.iCdmaServiceCategory = 0;
    smsAttr.iCdmaTeleservice = 0;
    smsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    smsAttr.iFlags = 0;
    smsAttr.iMsgRef = 0;
    smsAttr.iSubmitReport = KSubmit;
    smsAttr.iMore = EFalse;
    smsAttr.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsAttr.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsAttr.iDestination.iTelNumber.Copy(KNum);
    
    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    data.Close();
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrNone, data, 10);
    
    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iGsmServiceCentre.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iGsmServiceCentre.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iGsmServiceCentre.iTelNumber.Compare(KNum));
    ASSERT_EQUALS(0, smsAttr.iCdmaServiceCategory);
    ASSERT_EQUALS(0, smsAttr.iCdmaTeleservice);
    ASSERT_EQUALS(RMobileSmsMessaging::EFormatGsmTpdu, smsAttr.iDataFormat);
    ASSERT_TRUE(RMobileSmsMessaging::KMessageReference == smsAttr.iFlags);
    ASSERT_TRUE(KMsgRef == smsAttr.iMsgRef);
    ASSERT_EQUALS(0, smsAttr.iSubmitReport.Compare(KSubmit));
    ASSERT_TRUE(EFalse == smsAttr.iMore);
    ASSERT_EQUALS(RMobilePhone::EInternationalNumber, smsAttr.iDestination.iTypeOfNumber);
    ASSERT_EQUALS(RMobilePhone::EIsdnNumberPlan, smsAttr.iDestination.iNumberPlan);
    ASSERT_EQUALS(0, smsAttr.iDestination.iTelNumber.Compare(KNum));

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::SendMessage
    // from LTSY.
    //-------------------------------------------------------------------------

    data.Close();
    compTsyData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrNone, data, 10);
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    data.Close();
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data, KErrNotSupported);
    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3, this); 
    }




/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessageNoFdnCheck0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    _LIT8(KMessage, "Merry christmas");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessageNoFdnCheck; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobileSmsMessaging::SendMessageNoFdnCheck
    //-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrNone, data, 10);

    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    messaging.CancelAsyncRequest(EMobileSmsMessagingSendMessageNoFdnCheck);

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());


    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3);
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessageNoFdnCheck with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessageNoFdnCheck with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessageNoFdnCheck0003L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TRequestStatus reqStatus;
    TBuf8<1> tmpName;
    TInt smsAttr(1);
    TPckg<TInt8> badSmsAttrPckg(smsAttr);

    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, badSmsAttrPckg);
        
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2); // messaging, this
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessageNoFdnCheck0004L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RMobileSmsMessaging messaging2;
    err = messaging2.Open(phone2);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging2);

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 data2;
    CleanupClosePushL(data2);

    _LIT8(KMessage, "Merry christmas");
    _LIT8(KMessage2, "Merry christmas2");
    TBuf8<32> tmpName(KMessage);
    TBuf8<32> tmpName2(KMessage2);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TRequestStatus reqStatus2;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr2;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg2(smsAttr2);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessageNoFdnCheck; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    
    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    messaging2.SendMessageNoFdnCheck(reqStatus2, tmpName2, smsAttrPckg2);


    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    
    iMockLTSY.CompleteL(EMobileSmsMessagingSendMessageNoFdnCheck, KErrNone, data);

   
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(7);

    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessageNoFdnCheck with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessageNoFdnCheck and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestSendMessageNoFdnCheck0005L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RBuf8 data;
    CleanupClosePushL(data);

    _LIT8(KMessage, "Merry christmas");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessageNoFdnCheck; 
    dataAndAttr.iAttributes = &smsAttr; 
    dataAndAttr.iMsgData = &tmpName; 

    TMockLtsyData1<TSendSmsDataAndAttributes> expTsyData(dataAndAttr);           
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMobileSmsMessagingSendMessageNoFdnCheck, data);

    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3);

    }






/**
@SYMTestCaseID BA-CTSY-SMSM-SEMS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::EnumerateMessageStores
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::EnumerateMessageStores
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestEnumerateMessageStores0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TInt count;
    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::EnumerateMessageStores when result is not cached.
    //-------------------------------------------------------------------------

    err = messaging.EnumerateMessageStores(count);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_TRUE(KSmsStoreNumber == count);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); 
    
    }




/**
@SYMTestCaseID BA-CTSY-SMSM-SGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetCaps0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    RMobileSmsMessaging::TMobileSmsCapsV1 caps;
    RMobileSmsMessaging::TMobileSmsCapsV1Pckg pckgCaps(caps);
    

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::GetCaps when result is not cached.
    //-------------------------------------------------------------------------

    err = messaging.GetCaps(pckgCaps);

    ASSERT_EQUALS(KErrNone, err);

    ASSERT_TRUE( caps.iSmsMode == KSmsGsmModeCaps);
    ASSERT_TRUE( caps.iSmsControl == KSmsControlCaps);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this);
    
    }



/**
@SYMTestCaseID BA-CTSY-SMSM-SGC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetCaps with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::GetCaps with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsMessagingFU::TestGetCaps0003L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMobileSmsMessaging messaging;
    TInt err = messaging.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(messaging);

    TInt8 badCaps;
    TPckg<TInt8> badCapsPckg(badCaps);
    
    err = messaging.GetCaps(badCapsPckg);

    ASSERT_EQUALS(KErrArgument, err);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this);

    }




