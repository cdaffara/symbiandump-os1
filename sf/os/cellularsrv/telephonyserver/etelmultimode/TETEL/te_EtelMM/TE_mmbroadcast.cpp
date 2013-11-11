// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
*/

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmbroadcast.h"
#include "testdef.h"

CTestBroadcastMessaging::CTestBroadcastMessaging()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestBrdcastMessaging"));
	}

/**
Test Broadcast Messaging Functions
This test step tests performs the following tests:

Test 1 - RMobileBroadcastMessaging::GetCaps (sync)
Test 2 - RMobileBroadcastMessaging::ReceiveMessage (test asynchronous call, then test call and cancel request)
Test 3 - RMobileBroadcastMessaging::ReceiveMessage using V2params (test asynchronous call, then test call and cancel request)
Test 4 - RMobileBroadcastMessaging::ReceiveMessage incorrect buffer size (buffer too big) (async call). Tests fix for INC072923.
Test 5 - RMobileBroadcastMessaging::ReceiveMessage incorrect buffer size (buffer too small) (async call). Tests fix for INC072923.
Test 6 - RMobileBroadcastMessaging::GetFilterSetting (sync)
Test 7 - RMobileBroadcastMessaging::NotifyFilterSettingChange (test asynchronous call, then test call and cancel request)
Test 8 - RMobileBroadcastMessaging::SetFilterSetting (test asynchronous call, then test call and cancel request)
Test 9 - RMobileBroadcastMessaging::GetLanguageFilter (test asynchronous call, then test call and cancel request)
Test 10 - RMobileBroadcastMessaging::SetLanguageFilter (test asynchronous call, then test call and cancel request)
Test 11 - RMobileBroadcastMessaging::NotifyLanguageFilterChange (test asynchronous call, then test call and cancel request)
Test 12 - Test CRetrieveMobilePhoneBroadcastIdList (test asynchronous call, then test call and cancel request)
Test 13 - Test StoreBroadcastIdListL (test asynchronous call, then test call and cancel request)
Test 14 - RMobileBroadcastMessaging::NotifyBroadcastIdListChange (test asynchronous call, then test call and cancel request)

@return The verdict of the test step.
*/
enum TVerdict CTestBroadcastMessaging::doTestStepL()
//
// Test Broadcast Messaging Functions
//
    {
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Broadcast Messaging Functionality"));
//	LOGTEXT(_L8("Test Mobile Broadcast Messaging Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileBroadcastMessaging cbsMessaging;
	ret=cbsMessaging.Open(mmPhone);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;

	// Get Broadcast messaging caps
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 cbsCaps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg cbsCapsPckg(cbsCaps);

	TEST(cbsMessaging.GetCaps(cbsCapsPckg)==KErrNone);

	TEST(cbsCaps.iModeCaps==DMMTSY_BROADCAST_MESSAGING_MODE_CAPS);
	TEST(cbsCaps.iFilterCaps==DMMTSY_BROADCAST_MESSAGING_FILTER_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::GetCaps (sync) passed"), iTestCount++);
//	test.Next(_L("OK: RMobileBroadcastMessaging's Get Broadcast caps"));

	// Receiving broadcast messages
	
	RMobileBroadcastMessaging::TBroadcastPageData msgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 msgAtts;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttsPckg(msgAtts);

	cbsMessaging.ReceiveMessage(reqStatus, msgData, msgAttsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(msgAtts.iFormat==DMMTSY_CBS_MESSAGE_PDU_FORMAT);
	TEST(msgAtts.iFlags==DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS);
	TEST(msgData.Compare(DMMTSY_CBS_MESSAGE_PDU) == 0);

	// Test cancel

	cbsMessaging.ReceiveMessage(reqStatus, msgData, msgAttsPckg);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(msgAtts.iFormat==DMMTSY_CBS_MESSAGE_PDU_FORMAT);
		TEST(msgAtts.iFlags==DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS);
		TEST(msgData.Compare(DMMTSY_CBS_MESSAGE_PDU) == 0);
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
//	test.Next(_L("OK: RMobileBroadcastMessaging's Receive Message"));

	// Receiving broadcast messages V2

	RMobileBroadcastMessaging::TBroadcastPageData msgPage;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 msgAttsV2;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg msgAttsV2Pckg(msgAttsV2);

	cbsMessaging.ReceiveMessage(reqStatus, msgPage, msgAttsV2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(msgAttsV2.iFormat==DMMTSY_CBS_MESSAGE_PDU_FORMAT);
	TEST(msgAttsV2.iFlags==DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS);
	TEST(msgPage.Compare(DMMTSY_CBS_MESSAGE_PDU) == 0);

	// Test cancel

	cbsMessaging.ReceiveMessage(reqStatus, msgPage, msgAttsV2Pckg);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingReceiveMessage);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage V2params (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(msgAttsV2.iFormat==DMMTSY_CBS_MESSAGE_PDU_FORMAT);
		TEST(msgAttsV2.iFlags==DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS);
		TEST(msgPage.Compare(DMMTSY_CBS_MESSAGE_PDU) == 0);
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage V2params (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	// Receiving broadcast messages when buffer size incorrect
	
	// Test if client uses buffer of size not equal to 
	// RMobileBroadcastMessaging::TBroadcastPageData to hold the 
	// received message
	const TInt KIncorrectBroadcastPageSizeTooBig = 255; 
	TBuf8<KIncorrectBroadcastPageSizeTooBig> incorrectMsgPageTooBig;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 msgAttributesV1;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg msgAttributesV1Pckg(msgAttributesV1);
	
	cbsMessaging.ReceiveMessage(reqStatus, incorrectMsgPageTooBig, msgAttributesV1Pckg);
	User::WaitForRequest(reqStatus);
	// RMobileBroadcastMessaging::ReceiveMessage should detect 
	// incorrect buffer size and return KErrArgument
	TEST(reqStatus.Int()==KErrArgument);
	if (reqStatus.Int()==KErrArgument)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage incorrect buffer size (too big) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage incorrect buffer size (too big) failed"), iTestCount++);
		INFO_PRINTF3(_L("reqStatus = %d, expected reqStatus = %d"), reqStatus.Int(), KErrArgument);
		}

	const TInt KIncorrectBroadcastPageSizeTooSmall = 35;
	TBuf8<KIncorrectBroadcastPageSizeTooSmall> incorrectMsgPageTooSmall;
	cbsMessaging.ReceiveMessage(reqStatus, incorrectMsgPageTooSmall, msgAttributesV1Pckg);
	User::WaitForRequest(reqStatus);
	// RMobileBroadcastMessaging::ReceiveMessage should detect 
	// incorrect buffer size and return KErrArgument
	TEST(reqStatus.Int()==KErrArgument);
	if (reqStatus.Int()==KErrArgument)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage incorrect buffer size (too small) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::ReceiveMessage incorrect buffer size (too small) failed"), iTestCount++);
		INFO_PRINTF3(_L("reqStatus = %d, expected reqStatus = %d"), reqStatus.Int(), KErrArgument);
		}	
	// Get/Set/Notify broadcast receive setting

	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter setting;

	TEST(cbsMessaging.GetFilterSetting(setting)==KErrNone);
	TEST(setting==DMMTSY_BROADCAST_RECEIVE_SETTING1);
	INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::GetFilterSetting (sync) passed"), iTestCount++);

	// NotifyFilterSettingChange
	
	// Asynchronous

	cbsMessaging.NotifyFilterSettingChange(reqStatus, setting);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(setting==DMMTSY_BROADCAST_RECEIVE_SETTING2);

	// Asynchronous & cancel

	cbsMessaging.NotifyFilterSettingChange(reqStatus, setting);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyFilterSettingChange);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::NotifyFilterSettingChange (async & cancel (Cancelled Request)) passed"), iTestCount++);

		// The cancel function currently alters the value of setting.  This occurs because the Tsy
		// calls completeReq when cancelling requests.  This need to verified to determine whether this
		// behaviour is correct.  So the rest of the script can proceed, reset setting.

		setting=DMMTSY_BROADCAST_RECEIVE_SETTING2;
		}
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::NotifyFilterSettingChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		TEST(setting==DMMTSY_BROADCAST_RECEIVE_SETTING2);
		}

	// set filter setting - async
	cbsMessaging.SetFilterSetting(reqStatus, setting);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	cbsMessaging.SetFilterSetting(reqStatus, setting);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingSetFilterSetting);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::SetFilterSetting (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::SetFilterSetting (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


//	test.Next(_L("OK: RMobileBroadcastMessaging's Broadcast receive setting"));
	
	TBuf16<16> setLangFilter(DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER);
	TBuf16<16> getLangFilter;
	
	// Test Get Language Filter

	// asynchronous

	cbsMessaging.GetLanguageFilter(reqStatus,getLangFilter);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(getLangFilter.Compare(DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER) == 0);

	// asynchronous & cancel

	cbsMessaging.GetLanguageFilter(reqStatus,getLangFilter);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingGetLanguageFilter);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::GetLanguageFilter (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(getLangFilter.Compare(DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER) == 0);
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::GetLanguageFilter (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
	// Test Set Language Filter

	// asynchronous

	cbsMessaging.SetLanguageFilter(reqStatus,setLangFilter);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	cbsMessaging.SetLanguageFilter(reqStatus,setLangFilter);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingSetLanguageFilter);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::SetLanguageFilter (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::SetLanguageFilter (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
	// Test Notify Language Filter Change

	// asynchronous

	cbsMessaging.NotifyLanguageFilterChange(reqStatus,getLangFilter);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(getLangFilter.Compare(DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER) == 0);

	// asynchronous & cancel

	cbsMessaging.NotifyLanguageFilterChange(reqStatus,getLangFilter);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyLanguageFilterChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::NotifyLanguageFilterChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(getLangFilter.Compare(DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER) == 0);
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::NotifyLanguageFilterChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneBroadcastIdList* retrieve=CRetrieveMobilePhoneBroadcastIdList::NewL(cbsMessaging);
 	retrieve->Start(reqStatus,DMMTSY_BROADCAST_ID_TYPE);
 	delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Get CBMI list using asynchronous AO version

	CTestGetCbmiList* getCbmiList=CTestGetCbmiList::NewLC(cbsMessaging, this);
    getCbmiList->Start();
    CActiveScheduler::Start();

	// Lets ensure that the same ao can be used to retrieve more than one list.

	getCbmiList->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getCbmiList

	// Now test the cancelling

	getCbmiList=CTestGetCbmiList::NewLC(cbsMessaging, this);
    getCbmiList->Start();

	CTestCanceller* cancelGetCbmiList = getCbmiList->RetrieveCancelPtr();
	cancelGetCbmiList->Call();

    CActiveScheduler::Start();

	TBool cancelled = (getCbmiList->iStatus == KErrCancel);

	// Finally, lets just ensure that the object can be resused even after it has been cancelled

	getCbmiList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - Test CRetrieveMobilePhoneBroadcastIdList (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - Test CRetrieveMobilePhoneBroadcastIdList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getCbmiList

//	test.Next(_L("OK: RMobileCbmiStore's Read All Entries"));


	// Test StoreCbmiList

	CTestStoreCbmiList *storeList = CTestStoreCbmiList::NewLC(cbsMessaging, this);

	storeList->StartL();
	CActiveScheduler::Start();

	// Lets just perform the operation twice to make sure etelmm handles the cleanup of all
	// internal buffers correctly.

	storeList->StartL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy();

	// Test Cancel

	storeList = CTestStoreCbmiList::NewLC(cbsMessaging, this);

	storeList->StartL();
	storeList->RetrieveCancelPtr()->Call();
	CActiveScheduler::Start();

	if (storeList->iStatus == KErrCancel)
		INFO_PRINTF2(_L("Test %d - Test StoreBroadcastIdListL (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - Test StoreBroadcastIdListL (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	CleanupStack::PopAndDestroy();

	// Test NotifyCbmi List Change

	cbsMessaging.NotifyBroadcastIdListChange(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	cbsMessaging.NotifyBroadcastIdListChange(reqStatus);
	cbsMessaging.CancelAsyncRequest(EMobileBroadcastMessagingNotifyIdListChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::NotifyBroadcastIdListChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		INFO_PRINTF2(_L("Test %d - RMobileBroadcastMessaging::NotifyBroadcastIdListChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
//	test.Next(_L("OK: RMobileBroadcastMessaging Ok"));	

	INFO_PRINTF1(_L(""));
	cbsMessaging.Close();
	mmPhone.Close();

	return TestStepResult();
	}

/**************************************************************/
//
// Testing asynchronous retrieve of CBMI list
//
/**************************************************************/

CTestBroadcastMessaging::CTestGetCbmiList* CTestBroadcastMessaging::CTestGetCbmiList::NewLC(RMobileBroadcastMessaging& aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging)
	{
    CTestGetCbmiList* r=new(ELeave) CTestGetCbmiList(aMessaging, aTestBroadcastMessaging);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestBroadcastMessaging::CTestGetCbmiList::CTestGetCbmiList(RMobileBroadcastMessaging& aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging)
    : CActive(EPriorityNormal), iMessaging(aMessaging), iTestBroadcastMessaging(aTestBroadcastMessaging)
    {
	iIdType=DMMTSY_BROADCAST_ID_TYPE;
	}
      

void CTestBroadcastMessaging::CTestGetCbmiList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneBroadcastIdList::NewL(iMessaging);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestBroadcastMessaging::CTestGetCbmiList::~CTestGetCbmiList()
    {
	Cancel();
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestBroadcastMessaging::CTestGetCbmiList::Start()
    {
    iRetrieve->Start(iStatus,iIdType);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus,iIdType);
	User::WaitForRequest(tmpStatus);
	iTestBroadcastMessaging->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
	}
   
CTestCanceller* CTestBroadcastMessaging::CTestGetCbmiList::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestBroadcastMessaging::CTestGetCbmiList::RunL()
    {
    TInt ret=iStatus.Int();
	TInt count=iTestBroadcastMessaging->iTestCount;
	
	if (!ret)
		ret=CheckResults();

	if(ret!=KErrNone)
		{
		if(!(((count)>=KOOMTestNumberStart)&&(ret==KErrNoMemory)))
			iTestBroadcastMessaging->INFO_PRINTF3(_L("Test %d - TTestBroadcastMessaging::CTestGetCbmiList failed with error %d"), count++, ret);	
		}

    CActiveScheduler::Stop();
	}
   
void CTestBroadcastMessaging::CTestGetCbmiList::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestBroadcastMessaging::CTestGetCbmiList::CheckResults()
    {
	TInt ret=KErrNone;
	CMobilePhoneBroadcastIdList* cbmiList=NULL;

	TInt leaveCode=KErrNone;
	TRAP(leaveCode, cbmiList=iRetrieve->RetrieveListL(););

	if (cbmiList)
		{
		iTestBroadcastMessaging->TEST(cbmiList->Enumerate()==DMMTSY_CBMI_STORE_COUNT);

		// Get each detected network entry
		RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;

		for (TInt i=0; i<DMMTSY_CBMI_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=cbmiList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestBroadcastMessaging->TEST(entry.iId==DMMTSY_CBMI_ID1);
					break;
					}
				case 1:
					{
					iTestBroadcastMessaging->TEST(entry.iId==DMMTSY_CBMI_ID2);
					break;
					}
				case 2:
					{
					iTestBroadcastMessaging->TEST(entry.iId==DMMTSY_CBMI_ID3);
					break;
					}
				default:
					break;
				}
			}
		// now free-up cbmiList
		delete cbmiList;
		}

   return ret;
   }

/**************************************************************/
//
// Testing RMobileBroadcastMessaging::StoreCbmiList
//
/**************************************************************/

CTestBroadcastMessaging::CTestStoreCbmiList* CTestBroadcastMessaging::CTestStoreCbmiList::NewLC(RMobileBroadcastMessaging& aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging)
	{
    CTestStoreCbmiList* r=new(ELeave) CTestStoreCbmiList(aMessaging, aTestBroadcastMessaging);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestBroadcastMessaging::CTestStoreCbmiList::CTestStoreCbmiList(RMobileBroadcastMessaging& aMessaging, CTestBroadcastMessaging* aTestBroadcastMessaging)
    : CActive(EPriorityNormal), iMessaging(aMessaging ), iTestBroadcastMessaging(aTestBroadcastMessaging)
    {
	iIdType=DMMTSY_BROADCAST_ID_TYPE;
	}
      
void CTestBroadcastMessaging::CTestStoreCbmiList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestBroadcastMessaging::CTestStoreCbmiList::~CTestStoreCbmiList()
    {
	delete iCanceller;
	delete iList;
	}
      
void CTestBroadcastMessaging::CTestStoreCbmiList::StartL()
    {
	TInt i;
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;

	delete iList;
	iList = CMobilePhoneBroadcastIdList::NewL();

	for (i = 0; i < DMMTSY_CBMI_STORE_COUNT; i++)
		{
		entry.iId = DMMTSY_CBMI_ID1;
		iList->AddEntryL(entry);
		}
    iMessaging.StoreBroadcastIdListL(iStatus,iList,iIdType);
    SetActive();
    }       
      
CTestCanceller* CTestBroadcastMessaging::CTestStoreCbmiList::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestBroadcastMessaging::CTestStoreCbmiList::RunL()
    {
   	if (iStatus!=KErrNone)
   		{
   		iTestBroadcastMessaging->INFO_PRINTF3(_L("Test %d - TTestBroadcastMessaging::CTestStoreCbmiList failed with error %d"), iTestBroadcastMessaging->iTestCount++, iStatus.Int());	
   		}

    CActiveScheduler::Stop();
	}
      

void CTestBroadcastMessaging::CTestStoreCbmiList::DoCancel()
    {
	iMessaging.CancelAsyncRequest(EMobileBroadcastMessagingStoreIdList);
    }       
