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
 @file The TEFUnit test suite for SmsControl in the Common TSY.
*/

#include "cctsysmsfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include "cctsyactiveretriever.h"
#include <mmretrieve.h>
#include "mockltsyindicatorids.h"

CTestSuite* CCTsySmsFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

    //add use-case tests	
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUseCase0002L);

    //add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0006L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0007L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0008L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0009L);
    ADD_TEST_STEP_ISO_CPP(CCTsySmsFU, TestUnit0010L);	

	END_SUITE;
	}

LOCAL_C void FillInSmsParams(
		TSmsParameters& aSmsParams1,
		TSmsParameters& aSmsParams2)
	{
	aSmsParams1.iLocationNumber = 1;
	aSmsParams1.iParameterIndicator = KDestinationAddress|KServiceCentreAddress;
	aSmsParams1.iProtocolId = 2;
	aSmsParams1.iDataCodingScheme = 3;
	aSmsParams1.iValidityPeriod = 4;
	aSmsParams1.iMobileScTON = RMobilePhone::EInternationalNumber;
	aSmsParams1.iMobileScNPI = RMobilePhone::EIsdnNumberPlan;
	aSmsParams1.iMobileDeTON = RMobilePhone::ENationalNumber;
	aSmsParams1.iMobileDeNPI = RMobilePhone::EDataNumberPlan;
	aSmsParams1.iAlphaTagPresent = ETrue;
	aSmsParams1.iServiceCenterAddress.Copy(_L("asta"));
	aSmsParams1.iDestinationAddress.Copy(_L("la"));
	aSmsParams1.iAlphaTagData.Copy(_L("vista"));

	aSmsParams2.iLocationNumber = 5;
	aSmsParams2.iParameterIndicator = KProtocolID|KDataCodingScheme|KValidityPeriod;
	aSmsParams2.iProtocolId = 6;
	aSmsParams2.iDataCodingScheme = 7;
	aSmsParams2.iValidityPeriod = 8;
	aSmsParams2.iMobileScTON = RMobilePhone::ENetworkSpecificNumber;
	aSmsParams2.iMobileScNPI = RMobilePhone::ETelexNumberPlan;
	aSmsParams2.iMobileDeTON = RMobilePhone::ESubscriberNumber;
	aSmsParams2.iMobileDeNPI = RMobilePhone::ENationalNumberPlan;
	aSmsParams2.iAlphaTagPresent = EFalse;
	aSmsParams2.iServiceCenterAddress.Copy(_L("I'l"));
	aSmsParams2.iDestinationAddress.Copy(_L("be"));
	aSmsParams2.iAlphaTagData.Copy(_L("back"));

	}


//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for ReceiveMessage()
@SYMTestPriority High
@SYMTestActions Invokes ReceiveMessage()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySmsFU::TestUseCase0001L()
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
        
    iMockLTSY.ExpectL(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, KErrNone);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());   
    AssertMockLtsyStatusL();  

    // receiving message:
    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId, KErrNone, data);
    
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();    
    
    
    TDesC8* msgPtr2 = const_cast<TDesC8*>(&KMessage);
    TMockLtsyData1<TDesC8*> expTsyData2(msgPtr2);
    data.Close();
    expTsyData2.SerialiseL(data);
    
    iMockLTSY.ExpectL(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, KErrNone);

    // acknowledgment message receive:
    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    
    // receiving message:
    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);
    data.Close();
    compTsyData.SerialiseL(data);
 
    iMockLTSY.CompleteL(KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId, KErrNone, data);
    
    TDesC8* msgPtr3 = NULL;
    TMockLtsyData1<TDesC8*> expTsyData3(msgPtr3);
    data.Close();
    expTsyData3.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, data);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
           
    CleanupStack::PopAndDestroy(3, this);    
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for successful RMobileSmsMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySmsFU::TestUseCase0002L()
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
    
    
    _LIT8(KMessage, "Hello");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 expectSmsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessage; 
    dataAndAttr.iAttributes = &expectSmsAttr;
    dataAndAttr.iMsgData = &tmpName; 
    
    TDesC8* msgPtr = dataAndAttr.iMsgData;        
    TMockLtsyData5< RMobilePhone::TMobileAddress, 
                        TDesC8*,
                        RMobileSmsMessaging::TMobileSmsDataFormat,
                        RMobilePhone::TMobileAddress,
                        TBool
                       > expTsyData(smsAttr.iDestination, msgPtr, smsAttr.iDataFormat, smsAttr.iGsmServiceCentre, smsAttr.iMore);
    
    _LIT8(KSubmit, "Submit");
    _LIT(KNum, "+7903030303");
    
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
    
    expTsyData.SerialiseL(data);    
    iMockLTSY.ExpectL(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, data);
    data.Close();
    
    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);    
    compTsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, KErrNone, data, 10);
    data.Close();
    /*
     * The fields required will probably be message data format, destination 
     * and possibly a new service centre  address. 
     * On return, for either successful or failed transmission, the message reference number assigned to this 
     * sent message will be placed by the TSY in the iMsgRef field. 
     * Also, if the SMS was sent during GSM or WCDMA mode, then 
     * the SMS-SUBMIT-REPORT will be placed by the TSY in the iSubmitReport field.
     *  
     */    
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
	CleanupStack::PopAndDestroy(3, this); 
	}

//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::AckSmsStored()
@SYMTestPriority High
@SYMTestActions Invokes AckSmsStored()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySmsFU::TestUnit0001L()
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
  
            
    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
  
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
    
    expTsyData.SerialiseL(data);  
    
    iMockLTSY.ExpectL(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, KErrGeneral);

    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST C1, check AckSmsStored() when there is no sms received
    //-------------------------------------------------------------------------

    messaging.AckSmsStored(reqStatus, KMessage, ETrue);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotReady, reqStatus.Int());

    AssertMockLtsyStatusL(); 
    
    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::AckSmsStored
    // from LTSY.
    //-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(3, this); // data, messaging, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::GetMessageStoreInfo()
@SYMTestPriority High
@SYMTestActions Invokes GetMessageStoreInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySmsFU::TestUnit0002L()
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

    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId, KErrNotSupported);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    TInt total(0);
    TInt used(0);

    TMockLtsyData2<TInt, TInt> compData(total, used);
    
    compData.SerialiseL(data);

    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId, KErrGeneral, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::GetMessageStoreInfo when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    total = 2;
    used = 1;

    data.Close();
    compData.SerialiseL(data);

    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId, KErrNone, data);
    
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

    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId);

    messaging.GetMessageStoreInfo(reqStatus, index, pckgInfo);

    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId, KErrNotFound, data);
    
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

    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId, KErrNone, data);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, this);    
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for ReadSmspListPhase1L()
@SYMTestPriority High
@SYMTestActions Invokes ReadSmspListPhase1L()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySmsFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//CFilteringActiveScheduler does not need to be used, this should be changed
	//to a listretrieverao type class
	CFilteringActiveScheduler* scheduler = new (ELeave) CFilteringActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	RMobileSmsMessaging messaging;
	TInt err = messaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging);
	CRetrieveMobilePhoneSmspList* asyncRetrieveList =
	                                       CRetrieveMobilePhoneSmspList::NewL(messaging);    
	CleanupStack::PushL(asyncRetrieveList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveList);
	CleanupStack::PushL(activeRetriever);
	scheduler->AddRetrieverL(*activeRetriever);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrNotSupported);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler->StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	// test for KErrNotFound
    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrNotFound);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler->StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
		
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneSmspList::Start when result is not cached.
 	//-------------------------------------------------------------------------
	
    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId);
    
	TSmsParameters entry;
	TSmsParameters entry1;
	FillInSmsParams(entry, entry1);
	
	TDesC* strPtr1 = &entry.iServiceCenterAddress;
	TDesC* strPtr2 = &entry.iDestinationAddress;
	TDesC* strPtr3 = &entry.iAlphaTagData;
	TBool bMoreToCome = ETrue;
	
	DispatcherSim::TSmsParameters smsParam;
		
	smsParam.iLocationNumber = entry.iLocationNumber;
	smsParam.iParameterIndicator = entry.iParameterIndicator;
	smsParam.iProtocolId = entry.iProtocolId;
	smsParam.iDataCodingScheme = entry.iDataCodingScheme;
	smsParam.iValidityPeriod = entry.iValidityPeriod;
	smsParam.iMobileScTON = entry.iMobileScTON;
	smsParam.iMobileScNPI = entry.iMobileScNPI;
	smsParam.iMobileDeTON = entry.iMobileDeTON;
	smsParam.iMobileDeNPI = entry.iMobileDeNPI;
	smsParam.iAlphaTagPresent = entry.iAlphaTagPresent;
		
	TMockLtsyData5<TDesC* , TDesC*, TDesC*, DispatcherSim::TSmsParameters , TBool> completeLtsyData(strPtr1, strPtr2, strPtr3, smsParam, bMoreToCome);		
	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrNone, data);

    TBool bMoreToCome2 = EFalse;
    TDesC* strPtr21 = &entry1.iServiceCenterAddress;
    TDesC* strPtr22 = &entry1.iDestinationAddress;
    TDesC* strPtr23 = &entry1.iAlphaTagData;
   
    DispatcherSim::TSmsParameters smsParam2;
        
	smsParam2.iLocationNumber = entry1.iLocationNumber;
	smsParam2.iParameterIndicator = entry1.iParameterIndicator;
	smsParam2.iProtocolId = entry1.iProtocolId;
	smsParam2.iDataCodingScheme = entry1.iDataCodingScheme;
	smsParam2.iValidityPeriod = entry1.iValidityPeriod;
	smsParam2.iMobileScTON = entry1.iMobileScTON;
	smsParam2.iMobileScNPI = entry1.iMobileScNPI;
	smsParam2.iMobileDeTON = entry1.iMobileDeTON;
	smsParam2.iMobileDeNPI = entry1.iMobileDeNPI;
	smsParam2.iAlphaTagPresent = entry1.iAlphaTagPresent;
    
	TMockLtsyData5<TDesC* , TDesC*, TDesC*, DispatcherSim::TSmsParameters , TBool> completeLtsyData2(strPtr21, strPtr22, strPtr23, smsParam2, bMoreToCome2);		
	data.Close();
    completeLtsyData2.SerialiseL(data); 
    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrNone, data);
                    
	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler->StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	
	CMobilePhoneSmspList* list = asyncRetrieveList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS(2, list->Enumerate());
	const RMobileSmsMessaging::TMobileSmspEntryV1& smspEntry = list->GetEntryL(0);
	
	TInt completeIndex(entry.iLocationNumber);
	ASSERT_EQUALS(completeIndex, smspEntry.iIndex);
	ASSERT_EQUALS(entry.iAlphaTagData, smspEntry.iText);
	TUint32 completeValidParams(entry.iParameterIndicator);
	ASSERT_EQUALS(completeValidParams, smspEntry.iValidParams);
	ASSERT_EQUALS(entry.iMobileDeTON, smspEntry.iDestination.iTypeOfNumber);
	ASSERT_EQUALS(entry.iMobileDeNPI, smspEntry.iDestination.iNumberPlan);
	ASSERT_TRUE( 0 == entry.iDestinationAddress.Compare(smspEntry.iDestination.iTelNumber) );
	ASSERT_EQUALS(entry.iMobileScTON, smspEntry.iServiceCentre.iTypeOfNumber);
	ASSERT_EQUALS(entry.iMobileScNPI, smspEntry.iServiceCentre.iNumberPlan);
	ASSERT_TRUE( 0 == entry.iServiceCenterAddress.Compare(smspEntry.iServiceCentre.iTelNumber) );
		
	const RMobileSmsMessaging::TMobileSmspEntryV1& smspEntry1 = list->GetEntryL(1);
	
	completeIndex = entry1.iLocationNumber;
	ASSERT_EQUALS(completeIndex, smspEntry1.iIndex);
	ASSERT_EQUALS(0, smspEntry1.iText.Length());
	completeValidParams = entry1.iParameterIndicator;
	ASSERT_EQUALS(completeValidParams, smspEntry1.iValidParams);
	ASSERT_EQUALS(entry1.iProtocolId, smspEntry1.iProtocolId);
	ASSERT_EQUALS(entry1.iDataCodingScheme, smspEntry1.iDcs);
	ASSERT_EQUALS(entry1.iValidityPeriod, smspEntry1.iValidityPeriod);

    CleanupStack::PopAndDestroy(list);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId);
	data.Close();
    completeLtsyData2.SerialiseL(data); 
    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrGeneral, data);    

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler->StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneSmspList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrNone, data);

	data.Close();
    completeLtsyData2.SerialiseL(data); 
    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrNone, data);    
    
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	// increase coverage
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	data.Close();
    completeLtsyData2.SerialiseL(data); 
    iMockLTSY.CompleteL(MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId, KErrGeneral, data);
	    
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3); // activeRetriever, asyncRetrieveList, messaging
	CleanupStack::PopAndDestroy(3, this); // data, scheduler, this	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::NackSmsStored()
@SYMTestPriority High
@SYMTestActions Invokes NackSmsStored()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySmsFU::TestUnit0004L()
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
    
    TRequestStatus mockLtsyStatus;
    TRequestStatus reqStatus;
    TDesC8* msgPtr = const_cast<TDesC8*>(&KMessage);
    TInt rpCause(0);

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    
    TMockLtsyData2<TDesC8*, TInt> expTsyData(msgPtr, rpCause);


    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
  
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
    
    expTsyData.SerialiseL(data);

    iMockLTSY.ExpectL(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, KErrGeneral);

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
    
    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST C1: Unsuccessful completion request of
    // RMobileSmsMessaging::NackSmsStored when there are no sms received.
    //-------------------------------------------------------------------------

    messaging.NackSmsStored(reqStatus, KMessage, rpCause); 

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotReady, reqStatus.Int());
    
    AssertMockLtsyStatusL();  
    
    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::NackSmsStored
    // from LTSY.
    //-------------------------------------------------------------------------
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, this);     
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SMS-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::ReceiveMessage()
@SYMTestPriority High
@SYMTestActions Invokes ReceiveMessage()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySmsFU::TestUnit0005L()
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

    TRequestStatus reqStatus;
    TRequestStatus mockLtsyStatus;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
  
    _LIT8(KMessage, "Happy New Year");
    TBuf8<100> forMsg;
    
    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    messaging.ReceiveMessage(reqStatus, forMsg, receiveAttrPckg);

    TSmsMsg smsMsg;
    TSmsMsg* smsMsgPtr(&smsMsg);
    TBool ind(EFalse);
    
    smsMsg.iSmsClass2 = ETrue;
    smsMsg.iDeleteAfterClientAck = ETrue;
    smsMsg.iSmsMsg.Copy(KMessage);

    TMockLtsyData2<TBool, TSmsMsg*> compTsyData(ind, smsMsgPtr);
    compTsyData.SerialiseL(data);

    iMockLTSY.CompleteL(KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId, KErrGeneral, data);

    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::ReceiveMessage
    // from LTSY.
    //-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId, KErrNone, data);

    TDesC8* zeroPtr(NULL);
    TInt rpCause(KErrGsmSMSUnspecifiedProtocolError);
    TMockLtsyData2<TDesC8*, TInt> expTsyData(zeroPtr, rpCause);

    data.Close();
    expTsyData.SerialiseL(data);    

    iMockLTSY.ExpectL(MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId, data);
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    	
    CleanupStack::PopAndDestroy(3, this);     
	}
 
/**
@SYMTestCaseID BA-CTSY-SMSM-SRSR-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::ResumeSmsReception
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::ResumeSmsReception
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsFU::TestUnit0006L()
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

    iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId, KErrNotSupported);

    messaging.ResumeSmsReception(reqStatus);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

    messaging.ResumeSmsReception(reqStatus);

    iMockLTSY.CompleteL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId, KErrGeneral);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::ResumeSmsReception when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

    messaging.ResumeSmsReception(reqStatus);

    iMockLTSY.CompleteL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMobileSmsMessaging::ResumeSmsReception
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    CleanupStack::PopAndDestroy(3, this);
	}   
    

/**
@SYMTestCaseID BA-CTSY-SMSM-SSM-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsFU::TestUnit0007L()
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

    _LIT8(KMessage, "Hello");
    TBuf8<32> tmpName(KMessage);
    
    TRequestStatus reqStatus;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 expectSmsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsAttr;
    RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttrPckg(smsAttr);

    TSendSmsDataAndAttributes dataAndAttr;
    dataAndAttr.iIpc = EMobileSmsMessagingSendMessage; 
    dataAndAttr.iAttributes = &expectSmsAttr;
    dataAndAttr.iMsgData = &tmpName; 
    
    TDesC8* msgPtr = dataAndAttr.iMsgData;        
    TMockLtsyData5< RMobilePhone::TMobileAddress, 
                        TDesC8*,
                        RMobileSmsMessaging::TMobileSmsDataFormat,
                        RMobilePhone::TMobileAddress,
                        TBool
                       > expTsyData(smsAttr.iDestination, msgPtr, smsAttr.iDataFormat, smsAttr.iGsmServiceCentre, smsAttr.iMore);
    
    expTsyData.SerialiseL(data);    
    iMockLTSY.ExpectL(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, data, KErrNotSupported);
    data.Close();
    
    messaging.SendMessage(reqStatus, tmpName, smsAttrPckg);
    User::WaitForRequest(reqStatus);    
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();


    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::SendMessage when result is not cached.
    //-------------------------------------------------------------------------

    _LIT(KNum, "+7903030303");
    _LIT8(KSubmit, "Submit");
    
    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);    
        
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

    
    expTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, data);
    data.Close();
    
    compTsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, KErrNone, data, 10);
    
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
    // TEST D: Unsolicited completion of RMobileSmsMessaging::SendMessage
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    iMockLTSY.CompleteL(MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId, KErrNone, data, 10);
    data.Close();
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(3, this); 
    
    }


/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes RMobileSmsMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsFU::TestUnit0008L()
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

    TDesC8* msgPtr = dataAndAttr.iMsgData; 
    TMockLtsyData5< RMobilePhone::TMobileAddress, 
                    TDesC8*,
                    RMobileSmsMessaging::TMobileSmsDataFormat,
                    RMobilePhone::TMobileAddress,
                    TBool
                   > expTsyData(smsAttr.iDestination, msgPtr, smsAttr.iDataFormat, smsAttr.iGsmServiceCentre, smsAttr.iMore);
    expTsyData.SerialiseL(data);
    data.Close();
        
    
    //-------------------------------------------------------------------------
    // TEST A: Successful completion request of
    // RMobileSmsMessaging::SendMessage.
    //-------------------------------------------------------------------------

    _LIT8(KSubmit, "Submit");
    _LIT(KNum, "+7903030303");
    
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
    iMockLTSY.ExpectL(MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId, data);    
    
    const TInt16 KMsgRef(25);
    TInt16 msgRef(KMsgRef);
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsMsg(KMessage);
    TMockLtsyData2<TInt16, TBuf8<RMobileSmsMessaging::KGsmTpduSize> > compTsyData(msgRef, smsMsg);
    data.Close();
    compTsyData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId, KErrNone, data, 10);    
    
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
    // TEST B: Unsolicited completion of RMobileSmsMessaging::SendMessage
    // from LTSY.
    //-------------------------------------------------------------------------

    data.Close();
    compTsyData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    iMockLTSY.CompleteL(MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId, KErrNone, data, 10);    
    
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    //-------------------------------------------------------------------------
    // TEST C: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    data.Close();
    expTsyData.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId, data, KErrNotSupported);    
    
    messaging.SendMessageNoFdnCheck(reqStatus, tmpName, smsAttrPckg);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3, this); 
    }

/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileSmsMessaging::SetMoSmsBearer
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::SetMoSmsBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmsFU::TestUnit0009L()
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
    iMockLTSY.ExpectL(MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId, data);
    
    messaging.SetMoSmsBearer(reqStatus, smsBearer);
    messaging2.SetMoSmsBearer(reqStatus2, smsBearer2);

    
    iMockLTSY.CompleteL(MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId, KErrNone);
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(6, this); 
    }

/**
@SYMTestCaseID BA-CTSY-SMSM-SSMNFC-0010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileSmsMessaging::StoreSmspListL
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileSmsMessaging::StoreSmspListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsySmsFU::TestUnit0010L()
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
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------
    
    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;    
    smspListPtr->AddEntryL(smspEntry);

    TMockLtsyData1<RMobileSmsMessaging::TMobileSmspEntryV1> smspEntryData(smspEntry);
    smspEntryData.SerialiseL(data);
    iMockLTSY.ExpectL(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, data, KErrNotSupported);
    
    messaging.StoreSmspListL(reqStatus, smspListPtr); 
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, data);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    iMockLTSY.CompleteL(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, KErrGeneral);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobileSmsMessaging::StoreSmspListL when result is not cached.
    //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, data);

    messaging.StoreSmspListL(reqStatus, smspListPtr); 

    iMockLTSY.CompleteL(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, KErrNone);

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST D: Unsolicited completion of RMobileSmsMessaging::StoreSmspListL
    // from LTSY.
    //-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId, KErrNone);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(4, this); 
    
    }


