// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains tests for RMobileSmsMessaging API
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmsms.h"
#include "testdef.h"

CTestSmsMessaging::CTestSmsMessaging()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSmsMessaging"));
	}

enum TVerdict CTestSmsMessaging::doTestStepL()
//
// Test SMS Messaging Functions
//
    {
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile SMS Messaging Functionality"));
//	LOGTEXT(_L8("Test Mobile SMS Messaging Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileSmsMessaging smsMessaging;
	ret=smsMessaging.Open(mmPhone);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;
	
	// Get SMS messaging caps
	RMobileSmsMessaging::TMobileSmsCapsV1 smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);

	TEST(smsMessaging.GetCaps(smsCapsPckg)==KErrNone);
	TEST(smsCaps.iSmsMode==DMMTSY_SMS_MESSAGING_MODE_CAPS);
	TEST(smsCaps.iSmsControl==DMMTSY_SMS_MESSAGING_CONTROL_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::GetCaps (sync) passed"), iTestCount++);
	
//	test.Next(_L("OK: RMobileSmsMessaging's Get SMS caps"));

	// Receive Mode
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;

	TEST(smsMessaging.GetReceiveMode(receiveMode)==KErrNone);
	TEST(receiveMode==DMMTSY_SMS_MESSAGING_RECEIVE_MODE1);
	INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::GetReceiveMode (sync) passed"), iTestCount++);

	smsMessaging.SetReceiveMode(reqStatus, receiveMode);	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	smsMessaging.SetReceiveMode(reqStatus, receiveMode);	
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingSetReceiveMode);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SetReceiveMode (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SetReceiveMode (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	smsMessaging.NotifyReceiveModeChange(reqStatus, receiveMode);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(receiveMode==DMMTSY_SMS_MESSAGING_RECEIVE_MODE2);

	smsMessaging.NotifyReceiveModeChange(reqStatus, receiveMode);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifyReceiveModeChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NotifyReceiveModeChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NotifyReceiveModeChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsMessaging's SMS Receive Mode"));

	// Receive Incoming Message
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);

	smsMessaging.ReceiveMessage(reqStatus, gsmMsg, recAttPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateReceivedSmsMessage(gsmMsg, recAtt);

	smsMessaging.ReceiveMessage(reqStatus, gsmMsg, recAttPckg);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::ReceiveMessage (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::ReceiveMessage (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsMessaging's Receive Message"));

	// AckSmsStored - test no PDU supplied first
	TPtrC8 emptyPdu(NULL,0);

	smsMessaging.AckSmsStored(reqStatus, emptyPdu, DMMTSY_SMS_MESSAGE_FULL);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// AckSmsStored - test PDU supplied second
	TPtrC8 pdu(DMMTSY_SMS_MESSAGE_PDU1);

	smsMessaging.AckSmsStored(reqStatus, pdu, DMMTSY_SMS_MESSAGE_FULL);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	smsMessaging.AckSmsStored(reqStatus, pdu, DMMTSY_SMS_MESSAGE_FULL);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingAckSmsStored);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::AckSmsStored (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::AckSmsStored (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test NackSmsStored

	// asynchronous

	smsMessaging.NackSmsStored(reqStatus, pdu, DMMTSY_SMS_MESSAGE_NACK_CAUSE);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	smsMessaging.NackSmsStored(reqStatus, pdu, DMMTSY_SMS_MESSAGE_NACK_CAUSE);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingNackSmsStored);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NackSmsStored (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NackSmsStored (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test Resume Sms Reception

	smsMessaging.ResumeSmsReception(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	smsMessaging.ResumeSmsReception(reqStatus);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::ResumeSmsReception (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::ResumeSmsReception (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


	// Test Get/Set SMS Bearer
	RMobileSmsMessaging::TMobileSmsBearer bearer;

	TEST(smsMessaging.GetMoSmsBearer(bearer)==KErrNone);
	TEST(bearer==DMMTSY_SMS_MESSAGING_BEARER1);
	INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::GetMoSmsBearer (sync) passed"), iTestCount++);

	smsMessaging.SetMoSmsBearer(reqStatus, bearer);	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	smsMessaging.SetMoSmsBearer(reqStatus, bearer);	
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingSetMoSmsBearer);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SetMoSmsBearer (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SetMoSmsBearer (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	smsMessaging.NotifyMoSmsBearerChange(reqStatus, bearer);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(bearer==DMMTSY_SMS_MESSAGING_BEARER2);

	smsMessaging.NotifyMoSmsBearerChange(reqStatus, bearer);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifyMoSmsBearerChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NotifyMoSmsBearerChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NotifyMoSmsBearerChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsMessaging's Get/Set SMS Bearer"));

	// Test SendMessage
	
	// asynchronous

	RMobileSmsMessaging::TMobileSmsSendAttributesV1 atts;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg attsPckg(atts);

	atts.iFlags = DMMTSY_SMS_MESSAGE_FLAGS2;
	atts.iDataFormat = DMMTSY_SMS_MESSAGE_FORMAT1;
	atts.iDestination.iTelNumber=DMMTSY_PHONE_TEL_NUMBER;
	atts.iMore = DMMTSY_SMS_MESSAGE_MORE;

	TBuf8<32> pduBuf(DMMTSY_SMS_MESSAGE_PDU1);

	smsMessaging.SendMessage(reqStatus, pduBuf, attsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(atts.iMsgRef==DMMTSY_SMS_MESSAGE_MSGREF);
	TEST(atts.iSubmitReport==DMMTSY_SMS_MESSAGE_PDU2);

	// asynchronous & cancel

	smsMessaging.SendMessage(reqStatus, pduBuf, attsPckg);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SendMessage (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(atts.iMsgRef==DMMTSY_SMS_MESSAGE_MSGREF);
		TEST(atts.iSubmitReport==DMMTSY_SMS_MESSAGE_PDU2);
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SendMessage (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test SendMessage - CDMA
	
	// asynchronous

	RMobileSmsMessaging::TMobileSmsCdmaSendAttributesV4 attsV4;
	RMobileSmsMessaging::TMobileSmsCdmaSendAttributesV4Pckg attsV4Pckg(attsV4);

	attsV4.iFlags = DMMTSY_SMS_MESSAGE_CDMA_FLAGS;
	attsV4.iDataFormat = DMMTSY_SMS_MESSAGE_CDMA_FORMAT;
	attsV4.iCdmaTeleservice = DMMTSY_SMS_MESSAGE_CDMA_TELESERVICE;
	attsV4.iCdmaServiceCategory = DMMTSY_SMS_MESSAGE_CDMA_SERVICE_CATEGORY;
	attsV4.iMore = DMMTSY_SMS_MESSAGE_MORE;
	attsV4.iDestination.iTypeOfNumber = DMMTSY_SMS_MESSAGE_TON;
	attsV4.iDestination.iNumberPlan = DMMTSY_SMS_MESSAGE_NP;
	attsV4.iDestination.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
	attsV4.iAddressAuthentication = DMMTSY_SMS_MESSAGE_ADDRESS_AUTHENTICATION;
	
	smsMessaging.SendMessage(reqStatus, pduBuf, attsV4Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(attsV4.iMsgRef == DMMTSY_SMS_MESSAGE_MSGREF);
	TEST(attsV4.iTLAck == DMMTSY_SMS_MESSAGE_TLACK);

//	test.Next(_L("OK: RMobileSmsMessaging's Send Message"));

	// Test SendMessageNoFdnCheck
	
	// asynchronous

	atts.iFlags = DMMTSY_SMS_MESSAGE_FLAGS2;
	atts.iDataFormat = DMMTSY_SMS_MESSAGE_FORMAT1;
	atts.iDestination.iTelNumber=DMMTSY_PHONE_TEL_NUMBER;
	atts.iMore = DMMTSY_SMS_MESSAGE_MORE;

	smsMessaging.SendMessageNoFdnCheck(reqStatus, pduBuf, attsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(atts.iMsgRef==DMMTSY_SMS_MESSAGE_MSGREF);
	TEST(atts.iSubmitReport==DMMTSY_SMS_MESSAGE_PDU2);

	// asynchronous & cancel

	smsMessaging.SendMessageNoFdnCheck(reqStatus, pduBuf, attsPckg);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingSendMessageNoFdnCheck);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SendMessageNoFdnCheck (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(atts.iMsgRef==DMMTSY_SMS_MESSAGE_MSGREF);
		TEST(atts.iSubmitReport==DMMTSY_SMS_MESSAGE_PDU2);
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::SendMessageNoFdnCheck (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsMessaging's Send Message with No Fdn Check"));

	// Test Enumerate Message Stores

	TInt count=0;
	TEST(smsMessaging.EnumerateMessageStores(count)==KErrNone);
	TEST(count==DMMTSY_PHONE_MESSAGE_STORE_COUNT);

	INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::EnumerateMessageStores (sync) passed"), iTestCount++);

	// Get Message Store Info
	
	// asynchronous

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 asyncInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg asyncInfoPckg(asyncInfo);
	smsMessaging.GetMessageStoreInfo(reqStatus, DMMTSY_SMS_STORE_INDEX1, asyncInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateGetSmsMessageStoreInfo(asyncInfo);

	// asynchronous & cancel

	smsMessaging.GetMessageStoreInfo(reqStatus, DMMTSY_SMS_STORE_INDEX1, asyncInfoPckg);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::GetMessageStoreInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateGetSmsMessageStoreInfo(asyncInfo);
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::GetMessageStoreInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileSmsMessaging's Get Message Store Info"));

	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneSmspList* retrieve=CRetrieveMobilePhoneSmspList::NewL(smsMessaging);
 	retrieve->Start(reqStatus);
 	delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Get SMSP list using asynchronous AO version

	CTestGetSmspList* getSmspList=CTestGetSmspList::NewLC(smsMessaging, this);
    getSmspList->Start();
    CActiveScheduler::Start();

	// Lets ensure that the same ao can be used to retrieve more than one list.

	getSmspList->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getSmspList

	// Now test the cancelling

	getSmspList=CTestGetSmspList::NewLC(smsMessaging, this);
    getSmspList->Start();

	CTestCanceller* cancelGetSmspList = getSmspList->RetrieveCancelPtr();
	cancelGetSmspList->Call();

    CActiveScheduler::Start();

	TBool cancelled = (getSmspList->iStatus == KErrCancel);

	// Finally, lets just ensure that the object can be resused even after it has been cancelled

	getSmspList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - Test CRetrieveMobilePhoneSmspList (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - Test CRetrieveMobilePhoneSmspList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getSmspList

//	test.Next(_L("OK: RMobileSmsMessaging's Read SMSP List"));

	// Storing a new version of the list

	RMobileSmsMessaging::TMobileSmspEntryV1 storeEntry;

	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	InitSmspEntry(storeEntry);
	storeEntry.iIndex = DMMTSY_SMSP_STORE_INDEX1;

	smspList->AddEntryL(storeEntry);
	storeEntry.iIndex = DMMTSY_SMSP_STORE_INDEX2;
	smspList->AddEntryL(storeEntry);

	// asynchronous

	smsMessaging.StoreSmspListL(reqStatus, smspList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// aynchronous & cancel

	smsMessaging.StoreSmspListL(reqStatus, smspList);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingStoreSmspList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	cancelled = (reqStatus.Int()==KErrCancel);

	// Ensure that list can be stored after previous store attempt was cancelled.

	smsMessaging.StoreSmspListL(reqStatus, smspList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::StoreSmspListL (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::StoreSmspListL (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	CleanupStack::PopAndDestroy(); // smspList

//	test.Next(_L("OK: RMobileSmsMessaging's Store SMSP List"));

	// Test NotifySmspListChange

	smsMessaging.NotifySmspListChange(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	smsMessaging.NotifySmspListChange(reqStatus);
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifySmspListChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NotifySmspListChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		INFO_PRINTF2(_L("Test %d - RMobileSmsMessaging::NotifySmspListChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	smsMessaging.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

void CTestSmsMessaging::ValidateReceivedSmsMessage(RMobileSmsMessaging::TMobileSmsGsmTpdu& aMsg, RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAtt)
	{
	TEST(aMsg==DMMTSY_SMS_MESSAGE_PDU1);
	TEST(aAtt.iFlags==DMMTSY_SMS_MESSAGE_FLAGS1);
	TEST(aAtt.iGsmServiceCentre.iTelNumber.Compare(DMMTSY_SMS_MESSAGE_SC1)==0);
	TEST(aAtt.iDataFormat==DMMTSY_SMS_MESSAGE_FORMAT1);
	TEST(aAtt.iOriginator.iTelNumber.Compare(DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1)==0);
	TEST(aAtt.iStatus==DMMTSY_SMS_MESSAGE_STATUS1);
	TEST(aAtt.iStore.Compare(DMMTSY_MESSAGE_STORE_NAME)==0);
	TEST(aAtt.iStoreIndex==DMMTSY_SMS_STORE_INDEX1);
	}

void CTestSmsMessaging::ValidateGetSmsMessageStoreInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aSyncInfo)
	{
	TEST(aSyncInfo.iName.Compare(DMMTSY_MESSAGE_STORE_NAME)==0);
	TEST(aSyncInfo.iType==DMMTSY_MESSAGE_STORE_TYPE);
	TEST(aSyncInfo.iTotalEntries==DMMTSY_MESSAGE_STORE_TOTAL_ENTRIES);
	TEST(aSyncInfo.iUsedEntries==DMMTSY_MESSAGE_STORE_USED_ENTRIES);
	TEST(aSyncInfo.iCaps==DMMTSY_MESSAGE_STORE_CAPS);
	}

void CTestSmsMessaging::ValidateSmspEntry(const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
	{
	TEST(aEntry.iText.Compare(DMMTSY_SMSP_STORE_TEXT1)==0);
	TEST(aEntry.iValidParams==DMMTSY_SMSP_STORE_VALID_PARAMS1);
	TEST(aEntry.iDestination.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER)==0);
	TEST(aEntry.iValidityPeriod==DMMTSY_SMSP_STORE_VALIDITY1);
	TEST(aEntry.iServiceCentre.iTelNumber.Compare(DMMTSY_SMSP_STORE_SCA)==0);
	TEST(aEntry.iProtocolId==DMMTSY_SMSP_STORE_PID1);
	TEST(aEntry.iDcs==DMMTSY_SMSP_STORE_DCS1);
	}

void CTestSmsMessaging::InitSmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
	{
	aEntry.iText = DMMTSY_SMSP_STORE_TEXT1;
	aEntry.iValidParams = DMMTSY_SMSP_STORE_VALID_PARAMS1;
	aEntry.iDestination.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
	aEntry.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
	aEntry.iServiceCentre.iTelNumber = DMMTSY_SMSP_STORE_SCA;
	aEntry.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	aEntry.iDcs = DMMTSY_SMSP_STORE_DCS1;
	}

/**************************************************************/
//
// Testing asynchronous retrieve of SMSP list
//
/**************************************************************/

CTestSmsMessaging::CTestGetSmspList* CTestSmsMessaging::CTestGetSmspList::NewLC(RMobileSmsMessaging& aMessaging, CTestSmsMessaging* aTestSmsMessaging)
	{
    CTestGetSmspList* r=new(ELeave) CTestGetSmspList(aMessaging, aTestSmsMessaging);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestSmsMessaging::CTestGetSmspList::CTestGetSmspList(RMobileSmsMessaging& aMessaging, CTestSmsMessaging* aTestSmsMessaging)
    : CActive(EPriorityNormal), iMessaging(aMessaging), iTestSmsMessaging(aTestSmsMessaging)
    {}
      

void CTestSmsMessaging::CTestGetSmspList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneSmspList::NewL(iMessaging);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestSmsMessaging::CTestGetSmspList::~CTestGetSmspList()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestSmsMessaging::CTestGetSmspList::Start()
    {
    iRetrieve->Start(iStatus);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestSmsMessaging->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestSmsMessaging::CTestGetSmspList::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestSmsMessaging::CTestGetSmspList::RunL()
    {
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}

void CTestSmsMessaging::CTestGetSmspList::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestSmsMessaging::CTestGetSmspList::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneSmspList* smspList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, smspList=iRetrieve->RetrieveListL(););
	
	if (smspList)
		{
		iTestSmsMessaging->TEST(smspList->Enumerate()==DMMTSY_SMSP_STORE_COUNT);

		// Get each detected network entry
		RMobileSmsMessaging::TMobileSmspEntryV1 entry;

		for (TInt i=0; i<DMMTSY_SMSP_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=smspList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestSmsMessaging->ValidateSmspEntry(entry);
					iTestSmsMessaging->TEST(entry.iIndex==DMMTSY_SMSP_STORE_INDEX1);
					break;
					}
				case 1:
					{
					iTestSmsMessaging->ValidateSmspEntry(entry);
					iTestSmsMessaging->TEST(entry.iIndex==DMMTSY_SMSP_STORE_INDEX2);
					break;
					}
				default:
					break;
				}
			}
		delete smspList;
		}

   return ret;
   }

