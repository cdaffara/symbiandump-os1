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
// Test step definitions for the BroadcastMessaging functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestbroadcastmessaging.h"
#include "listretrievalhelpers.h"


	
CCTSYIntegrationTestBroadcastMessagingBase::CCTSYIntegrationTestBroadcastMessagingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iBroadcastMessagingTestHelper(*this), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestBroadcastMessagingBase::~CCTSYIntegrationTestBroadcastMessagingBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestBroadcastMessaging0001::CCTSYIntegrationTestBroadcastMessaging0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0001::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0001::~CCTSYIntegrationTestBroadcastMessaging0001()
/**
 * Destructor.
 */
	{
	}

TBool CCTSYIntegrationTestBroadcastMessagingBase::AssertValidBroadcastMessage(const RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 &aBraodcastAttributes)
/*
 * This function checks the validity of received Broadcastmessage
 *
 * @param aBraodcastAttributes Reference to attributes of received messages.
 * @return true if the received message is not valid.
 */
	{
	TBool ret=EFalse;
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgAttributes.iFlags in set of KBroadcastDataFormat
	ASSERT_BITS_SET(aBraodcastAttributes.iFlags, RMobileBroadcastMessaging::KBroadcastDataFormat ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::ReceiveMessage did not return desired Message Flags"));
		
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgAttributes.iFormat of EFormatGsmTpdu
	ASSERT_EQUALS(aBraodcastAttributes.iFormat, RMobileBroadcastMessaging::EFormatGsmTpdu, _L("RMobileBroadcastMessaging::ReceiveMessage did not return desired Message Format"));

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgAttributes.iDCS > 0
	ASSERT_TRUE(aBraodcastAttributes.iDCS > 0, _L("RMobileBroadcastMessaging::ReceiveMessage did not return DCS > 0"));

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgAttributes.iMessageId > 0
	ASSERT_TRUE(aBraodcastAttributes.iMessageId> 0, _L("RMobileBroadcastMessaging::ReceiveMessage did not return MessageId > 0"));

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgAttributes.iMessageType of EGsmBroadcastId
	ASSERT_EQUALS((int)aBraodcastAttributes.iMessageType , (int)RMobileBroadcastMessaging::EGsmBroadcastId, _L("RMobileBroadcastMessaging::ReceiveMessage did not return desired MessageType"));

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgAttributes.iNumberOfPages > 0
	ASSERT_TRUE(aBraodcastAttributes.iNumberOfPages > 0, _L("RMobileBroadcastMessaging::ReceiveMessage did not return NumberOfPages > 0"));

	if((aBraodcastAttributes.iFlags && RMobileBroadcastMessaging::KBroadcastDataFormat)	
			&& (aBraodcastAttributes.iFormat== RMobileBroadcastMessaging::EFormatGsmTpdu)
			&& (aBraodcastAttributes.iDCS>0) && (aBraodcastAttributes.iMessageId> 0)	
			&& ((int)aBraodcastAttributes.iMessageType ==(int)RMobileBroadcastMessaging::EGsmBroadcastId)
			&& (aBraodcastAttributes.iNumberOfPages > 0) )
		{
		ret= ETrue;
		}
	
	return ret;
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0001
 * @SYMFssID BA/CTSY/BRMSG-0001
 * @SYMTestCaseDesc Set the broadcast messaging simple filter.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::NotifyFilterSettingChange,RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting
 * @SYMTestExpectedResults Pass - Simple filter notifications complete and are set correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify filter change notification completes and correct filter has been set.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps  = RMobileBroadcastMessaging::KCapsSimpleFilter;

	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Set the simple filter setting to accept none ===

	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptNone 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptNone
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );

	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptNone
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptNone, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))
	
	// ===  Set the simple filter setting to accept all ===
	
	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10026);		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10027);		
	CHECK_EQUALS_L(1, 2, _L("This test has been made to leave intentially.The purpose of this leave is to avoid the board crashing"));
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptAll 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting2 = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptAll
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting2,
			KErrNone );
	
	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptAll
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptAll, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptAll"))
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
    // notifyFilterSettingChangeStatus
    // setFilterSettingStatus
    CleanupStack::PopAndDestroy(2,&notifyFilterSettingChangeStatus);
	
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0001");
	}



CCTSYIntegrationTestBroadcastMessaging0002::CCTSYIntegrationTestBroadcastMessaging0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0002::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0002::~CCTSYIntegrationTestBroadcastMessaging0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0002
 * @SYMFssID BA/CTSY/BRMSG-0002
 * @SYMTestCaseDesc Set the broadcast messaging ID filter
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneBroadcastIdList::Start, RMobileBroadcastMessaging::NotifyFilterSettingChange, CRetrieveMobilePhoneBroadcastIdList::RetrieveListL, RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, CMobilePhoneBroadcastIdList::Enumerate, RMobileBroadcastMessaging::NotifyBroadcastIdListChange, RMobileBroadcastMessaging::StoreBroadcastIdListL, CMobilePhoneBroadcastIdList::AddRangeEntryL
 * @SYMTestExpectedResults Pass - Filter setting and notification returned KErrNotSuppoerted and no value can be retrieved .
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify filter change notification completes with KErrNotSupported and no filter has been set.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
		
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
				
	// ===  Set broadcast ID filter ===

	// Create a CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);

	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=100 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 100; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 
	
	// This is deliberate action to open a session because the API in Etelmm allocates memory
	// The Session Manager should be used in all other places.
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobileBroadcastMessaging broadcastMessagingStore;
	broadcastMessagingStore.Open(phone);
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with KErrNotSupported having aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessagingStore.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d </font>"), 10028);		
    broadcastMessagingStore.Close();
    
    // Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KErrNotSupported
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KErrNotSupported);
	
	// ===  Get the broadcast ID filter ===

	// Use helper class for CRetrieveMobilePhoneBroadcastIdList to retrieve the broadcast ID list. 
	TInt err;
	CRetrieveMobilePhoneBroadcastIdListExec* broadcastIdListExec  = CRetrieveMobilePhoneBroadcastIdListHelper::NewL(broadcastMessaging,idType);	
	CleanupStack::PushL(broadcastIdListExec);
	
	// Get the CMobilePhoneBroadcastIdList list using the helper class function which will call CRetrieveMobilePhoneBroadcastIdList::RetrieveListL 
	CMobilePhoneBroadcastIdList* broadcastIdList2 = broadcastIdListExec ->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneBroadcastIdList::RetrieveListL  is not able to get the list"))
	
	// Check CMobilePhoneBroadcastIdList::Enumerate returns 0
	ASSERT_EQUALS(broadcastIdList2->Enumerate(),0 , _L("CMobilePhoneBroadcastIdList::Enumerate did not return an 99 as expected"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
    // storeBroadcastIdList
    // broadcastIdListExec
	CleanupStack::PopAndDestroy(4, broadcastIdList);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0002");
	}



CCTSYIntegrationTestBroadcastMessaging0003::CCTSYIntegrationTestBroadcastMessaging0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0003::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0003::~CCTSYIntegrationTestBroadcastMessaging0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0003
 * @SYMFssID BA/CTSY/BRMSG-0003
 * @SYMTestCaseDesc Receive cell broadcast with different filter settings.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::NotifyFilterSettingChange, RMobileBroadcastMessaging::SetLanguageFilter, RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, RMobileBroadcastMessaging::GetLanguageFilter, RMobileBroadcastMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - Broadcast messages that should not be filtered received. Messages that should be filtered not received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify cell broadcast message is received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10072);
	User::Leave(KErrNone);	
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;

	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter 
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
	
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	


	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Set the simple filter setting to accept all and receive a message ===
	
	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptAll 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptAll
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );

	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptAll
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptAll, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	RMobileBroadcastMessaging::TGsmBroadcastMessageData rxMsgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 rxMsgAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg rxMsgAttributePackage(rxMsgAttributes);
	TExtEtelRequestStatus receiveMessageStatus(broadcastMessaging, EMobileBroadcastMessagingReceiveMessage);
	CleanupStack::PushL(receiveMessageStatus);
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);

	// Simulated network sends a GSM formatted broadcast message. 
	TBuf<120> prompt(_L("Waiting for the CBM, notification of a directly delivered cell broadcast message from the network operator"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));		

	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));

	// ===  Set the simple filter setting to accept none and receive a message ===

	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10027);		
	CHECK_EQUALS_L(1, 2, _L("This test has been made to leave intentially. The purpose of this leave is to avoid the board crashing"));
		
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptNone 
	filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		
	
	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptNone
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );
	
	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptNone
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptNone, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulated network sends a GSM formatted broadcast message. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
    // notifyFilterSettingChangeStatus
	// setFilterSettingStatus
	// receiveMessageStatus
	    
    CleanupStack::PopAndDestroy(3,&notifyFilterSettingChangeStatus);
   
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0003");
	}



CCTSYIntegrationTestBroadcastMessaging0004::CCTSYIntegrationTestBroadcastMessaging0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0004::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0004::~CCTSYIntegrationTestBroadcastMessaging0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0004
 * @SYMFssID BA/CTSY/BRMSG-0004
 * @SYMTestCaseDesc Receive broadcast messages when ID filter has been defined but filter is set to accept all messages.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, RMobileBroadcastMessaging::ReceiveMessage, RMobileBroadcastMessaging::NotifyBroadcastIdListChange, RMobileBroadcastMessaging::StoreBroadcastIdListL, CMobilePhoneBroadcastIdList::AddRangeEntryL
 * @SYMTestExpectedResults Pass - All broadcast messages received when filter is set to accept all.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify all messages received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10073);
	User::Leave(KErrNone);	


	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter 
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
	
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat 
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Set broadcast ID filter ===

	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);

	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=100 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 100; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 

	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
	// Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KRequestPending 
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KRequestPending );
	
	// ===  Set filter to accept all messages and receive a message within and out of the ID range ===

	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptAll 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptAll;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptAll
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );

	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptAll
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptAll, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))

	// Call RMobileBroadcastMessaging::ReceiveMessage
	RMobileBroadcastMessaging::TGsmBroadcastMessageData rxMsgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 rxMsgAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg rxMsgAttributePackage(rxMsgAttributes);
	TExtEtelRequestStatus receiveMessageStatus(broadcastMessaging, EMobileBroadcastMessagingReceiveMessage);
	CleanupStack::PushL(receiveMessageStatus);
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);

	// Simulator sends a broadcast message with an ID = 1 
	TBuf<110> prompt(_L("Waiting for the CBM, notification of a directly delivered cell broadcast message from the network operator"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));		

	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 50 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));

	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 99 
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));		

	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	// ===  Should also receive messages outside of the range ===

	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 100. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));	

	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
    // storeBroadcastIdList
    // notifyFilterSettingChangeStatus
    // setFilterSettingStatus
    // receiveMessageStatus
    CleanupStack::PopAndDestroy(6,broadcastIdList);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0004");
	}



CCTSYIntegrationTestBroadcastMessaging0005::CCTSYIntegrationTestBroadcastMessaging0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0005::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0005::~CCTSYIntegrationTestBroadcastMessaging0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0005
 * @SYMFssID BA/CTSY/BRMSG-0005
 * @SYMTestCaseDesc Receive broadcast messages when ID filter has been defined and filter is set to accept filter.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, RMobileBroadcastMessaging::ReceiveMessage, RMobileBroadcastMessaging::NotifyBroadcastIdListChange, RMobileBroadcastMessaging::StoreBroadcastIdListL, CMobilePhoneBroadcastIdList::AddRangeEntryL
 * @SYMTestExpectedResults Pass - Only messages within the filter range are accepted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify messages with an ID within the filter range are accepted.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10074);
	User::Leave(KErrNone);	

	
	
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter 
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
	
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Set broadcast ID filter ===

	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=100 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 100; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
	// Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KRequestPending 
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KRequestPending );

	// ===  Set filter to accept filter settings ===

	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptFilter 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptFilter;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptFilter
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );

	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptFilter
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptFilter, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))

	// ===  Check messages with IDs in range are received ===
		
	//Call RMobileBroadcastMessaging::ReceiveMessage
	RMobileBroadcastMessaging::TGsmBroadcastMessageData rxMsgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 rxMsgAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg rxMsgAttributePackage(rxMsgAttributes);
	TExtEtelRequestStatus receiveMessageStatus(broadcastMessaging, EMobileBroadcastMessagingReceiveMessage);
	CleanupStack::PushL(receiveMessageStatus);
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 1 
	TBuf<110> prompt(_L("Waiting for the CBM, notification of a directly delivered cell broadcast message from the network operator"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));			
	
	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 50 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));
	
	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage 
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 99 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));	
	
	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	// ===  Check messages not in range are not received ===
	
	//Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 100. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));

	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
    // storeBroadcastIdList
    // notifyFilterSettingChangeStatus
    // setFilterSettingStatus
    // receiveMessageStatus
    
    CleanupStack::PopAndDestroy(6, broadcastIdList);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0005");
	}



CCTSYIntegrationTestBroadcastMessaging0006::CCTSYIntegrationTestBroadcastMessaging0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0006::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0006::~CCTSYIntegrationTestBroadcastMessaging0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0006
 * @SYMFssID BA/CTSY/BRMSG-0006
 * @SYMTestCaseDesc Receive broadcast messages when ID filter has been defined and filter is set to reject filter.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, RMobileBroadcastMessaging::ReceiveMessage, RMobileBroadcastMessaging::NotifyBroadcastIdListChange, RMobileBroadcastMessaging::StoreBroadcastIdListL, CMobilePhoneBroadcastIdList::AddRangeEntryL
 * @SYMTestExpectedResults Pass - Messages within ID range are not received, messages outside of the range are received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify messages within the ID range are not received and messages outside the range are.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter 
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
	
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Set broadcast ID filter ===

	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=100 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 100; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
    // Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KRequestPending 
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KRequestPending );

	// ===  Set filter to reject filter settings ===
	
	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastRejectFilter 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastRejectFilter;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastRejectFilter
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );

	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastRejectFilter
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastRejectFilter, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))

	// ===  Check messages with IDs in range are not received ===

	// Call RMobileBroadcastMessaging::ReceiveMessage
	RMobileBroadcastMessaging::TGsmBroadcastMessageData rxMsgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 rxMsgAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg rxMsgAttributePackage(rxMsgAttributes);
	TExtEtelRequestStatus receiveMessageStatus(broadcastMessaging, EMobileBroadcastMessagingReceiveMessage);
	CleanupStack::PushL(receiveMessageStatus);
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 1 
	TBuf<110> prompt(_L("Waiting for the CBM, notification of a directly delivered cell broadcast message from the network operator"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 50 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 99 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));

	// ===  Check messages not in range are  received ===
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 100. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));
	
	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 0. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage completes with aMsgData with length > 0 characters
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage timed out"));
	ASSERT_EQUALS(receiveMessageStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::ReceiveMessage returned with an error"));		
	ASSERT_TRUE(rxMsgData.Length()>0, _L("RMobileBroadcastMessaging::ReceiveMessage did not complete with aMsgData with length > 0 characters"));
	
	ASSERT_TRUE(AssertValidBroadcastMessage(rxMsgAttributes), _L("AssertValidBroadcastMessage returned with failure"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
    // storeBroadcastIdList
    // notifyFilterSettingChangeStatus
    // setFilterSettingStatus
    // receiveMessageStatus
    CleanupStack::PopAndDestroy(6,broadcastIdList);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0006");
	}



CCTSYIntegrationTestBroadcastMessaging0007::CCTSYIntegrationTestBroadcastMessaging0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0007::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0007::~CCTSYIntegrationTestBroadcastMessaging0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0007
 * @SYMFssID BA/CTSY/BRMSG-0007
 * @SYMTestCaseDesc Receive broadcast messages when ID filter has been defined and fliter is set to reject all messages.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, RMobileBroadcastMessaging::ReceiveMessage, RMobileBroadcastMessaging::NotifyBroadcastIdListChange, RMobileBroadcastMessaging::StoreBroadcastIdListL, CMobilePhoneBroadcastIdList::AddRangeEntryL
 * @SYMTestExpectedResults Pass - No messages received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify no messages are received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter 
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps = RMobileBroadcastMessaging::KCapsSimpleFilter;
	
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat 
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Set broadcast ID filter ===
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=100 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 100; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
    // Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KRequestPending 
    iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KRequestPending );

	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptNone 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptNone;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptNone
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );
	
	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptNone
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptNone, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))

	// ===  Check messages with IDs in range are not received ===
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	RMobileBroadcastMessaging::TGsmBroadcastMessageData rxMsgData;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 rxMsgAttributes;
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg rxMsgAttributePackage(rxMsgAttributes);
	TExtEtelRequestStatus receiveMessageStatus(broadcastMessaging, EMobileBroadcastMessagingReceiveMessage);
	CleanupStack::PushL(receiveMessageStatus);
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);

	// Simulator sends a broadcast message with an ID = 1 
	TBuf<110> prompt(_L("Waiting for the CBM, notification of a directly delivered cell broadcast message from the network operator"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 50 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));
	
	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 99 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));


	// ===  Check messages not in range are not received ===

	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 100. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));

	// Call RMobileBroadcastMessaging::ReceiveMessage
	broadcastMessaging.ReceiveMessage(receiveMessageStatus, rxMsgData, rxMsgAttributePackage);
	
	// Simulator sends a broadcast message with an ID = 0. 
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Check RMobileBroadcastMessaging::ReceiveMessage is still pending 2 minutes after the message was sent
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveMessageStatus, ETimeVeryLong),KErrTimedOut, _L("RMobileBroadcastMessaging::ReceiveMessage did not pend 2 mins after the message was sent"));

	
	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
    // storeBroadcastIdList
    // notifyFilterSettingChangeStatus
    // setFilterSettingStatus
    // receiveMessageStatus
    CleanupStack::PopAndDestroy(6,broadcastIdList);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0007");
	}



CCTSYIntegrationTestBroadcastMessaging0008::CCTSYIntegrationTestBroadcastMessaging0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0008::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0008::~CCTSYIntegrationTestBroadcastMessaging0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0008
 * @SYMFssID BA/CTSY/BRMSG-0008
 * @SYMTestCaseDesc Store a broadcast ID list with no IDs
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::StoreBroadcastIdListL
 * @SYMTestExpectedResults Pass - No entry is stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify list returns KErrNotSupported when stored.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	
	// Check RMobileBroadcastMessaging::StoreBroadcastIdListL returns KErrNotSupported
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		

	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	// Pop:
    // broadcastIdList
    // storeBroadcastIdList
  
    CleanupStack::PopAndDestroy(2,broadcastIdList);
   	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0008");
	}



CCTSYIntegrationTestBroadcastMessaging0009::CCTSYIntegrationTestBroadcastMessaging0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0009::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0009::~CCTSYIntegrationTestBroadcastMessaging0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0009
 * @SYMFssID BA/CTSY/BRMSG-0009
 * @SYMTestCaseDesc Store 999 IDs with broadcast ID list
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::StoreBroadcastIdListL
 * @SYMTestExpectedResults Pass - List is not stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify list can not be stored.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=1000 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 1000; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
    // Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KErrNotSupported
    iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KErrNotSupported);

	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
	// storeBroadcastIdList
    
    CleanupStack::PopAndDestroy(3,broadcastIdList);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0009");
	}



CCTSYIntegrationTestBroadcastMessaging0010::CCTSYIntegrationTestBroadcastMessaging0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0010::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0010::~CCTSYIntegrationTestBroadcastMessaging0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0010
 * @SYMFssID BA/CTSY/BRMSG-0010
 * @SYMTestCaseDesc Store maximum number of broadcast IDs for the phone.
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::StoreBroadcastIdListL
 * @SYMTestExpectedResults Pass - List is stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify list can be stored.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 

	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=X where X=maximum number of entries supported 

	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 

	// Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KErrNone

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0010");
	}



CCTSYIntegrationTestBroadcastMessaging0011::CCTSYIntegrationTestBroadcastMessaging0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0011::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0011::~CCTSYIntegrationTestBroadcastMessaging0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0011
 * @SYMFssID BA/CTSY/BRMSG-0011
 * @SYMTestCaseDesc Store broadcast ID list with duplicate IDs
 * @SYMTestPriority High
 * @SYMTestActions RMobileBroadcastMessaging::StoreBroadcastIdListL
 * @SYMTestExpectedResults Pass - List is not stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify list can not be stored.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=3 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 3; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 
	
	// Add another entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=3 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 

	// Post a notification RMobileBroadcastMessaging::NotifyBroadcastIdListChange
	TExtEtelRequestStatus notifyBroadcastIdListChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyIdListChange);
	CleanupStack::PushL(notifyBroadcastIdListChangeStatus);
	broadcastMessaging.NotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus); 
	
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
	RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
	TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
	CleanupStack::PushL(storeBroadcastIdList);
	broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
	ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
    // Check RMobileBroadcastMessaging::NotifyBroadcastIdListChange completes with KErrNotSupported
    iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange(notifyBroadcastIdListChangeStatus, KErrNotSupported);

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
    // broadcastIdList
    // notifyBroadcastIdListChangeStatus
    // storeBroadcastIdList
    CleanupStack::PopAndDestroy(3, broadcastIdList);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0011");
	}



CCTSYIntegrationTestBroadcastMessaging0012::CCTSYIntegrationTestBroadcastMessaging0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0012::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0012::~CCTSYIntegrationTestBroadcastMessaging0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0012
 * @SYMFssID BA/CTSY/BRMSG-0012
 * @SYMTestCaseDesc Store and retrieve broadcast ID list.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - List retrieved is empty.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify store returned KErrNotSupported and retrieved an empty list.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging);
		
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// Add an entry to the list with CMobilePhoneBroadcastIdList::AddRangeEntryL with aStart.iId=1 and aEnd.iId=4 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry; 
	idEntry.iId = 1; 
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 idEntry2; 
    idEntry2.iId = 4; 
	broadcastIdList->AddRangeEntryL(idEntry, idEntry2); 
    
	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
    RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
    TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
    CleanupStack::PushL(storeBroadcastIdList);
    broadcastMessaging.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
    
    // Check store returned KErrNotSupported
    ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    
	// Use Retrieval helper class to retrieve broadcast Id entries from the list
    TInt err;
    CRetrieveMobilePhoneBroadcastIdListExec* broadcastIdListExec  = CRetrieveMobilePhoneBroadcastIdListHelper::NewL(broadcastMessaging,idType);	
	CleanupStack::PushL(broadcastIdListExec);
	CMobilePhoneBroadcastIdList* broadcastIdList2 = broadcastIdListExec ->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneBroadcastIdList::RetrieveListL  is not able to get the list"))

    // Check CMobilePhoneBroadcastIdList::Enumerate returns 0 entry
	ASSERT_EQUALS(broadcastIdList2->Enumerate(), 0, _L("CMobilePhoneBroadcastIdList::Enumerate did not return an empty list"));
				
	//
	// TEST END
	//

    StartCleanup();
	
    // Pop:
    // broadcastIdList
	// storeBroadcastIdList
    // broadcastIdListExec
    CleanupStack::PopAndDestroy(3, broadcastIdList);
    
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0012");
	}



CCTSYIntegrationTestBroadcastMessaging0013::CCTSYIntegrationTestBroadcastMessaging0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0013::GetTestStepName());
	}

CCTSYIntegrationTestBroadcastMessaging0013::~CCTSYIntegrationTestBroadcastMessaging0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestBroadcastMessaging0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-BRMSG-0013
 * @SYMFssID BA/CTSY/BRMSG-0013
 * @SYMTestCaseDesc Store and retrieve an empty broadcast ID list.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - List retrieved is same as that stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify an empty list is retrieved.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging);
		
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Create a new CMobilePhoneBroadcastIdList with CMobilePhoneBroadcastIdList ::NewL 
	CMobilePhoneBroadcastIdList* broadcastIdList= CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(broadcastIdList);
	
	// This is deliberate action to open a session because the API in Etelmm allocates memory
	// The Session Manager should be used in all other places.
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobileBroadcastMessaging broadcastMessagingStore;
	broadcastMessagingStore.Open(phone);

	// Store the broadcast ID list with RMobileBroadcastMessaging::StoreBroadcastIdListL with aIdType=EGsmBroadcastId 
    RMobileBroadcastMessaging::TMobileBroadcastIdType idType=RMobileBroadcastMessaging::EGsmBroadcastId;
    TExtEtelRequestStatus storeBroadcastIdList(broadcastMessaging, EMobileBroadcastMessagingStoreIdList);
    CleanupStack::PushL(storeBroadcastIdList);
    broadcastMessagingStore.StoreBroadcastIdListL(storeBroadcastIdList,broadcastIdList,idType);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(storeBroadcastIdList, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL timed out"));
    ASSERT_EQUALS(storeBroadcastIdList.Int(), KErrNotSupported, _L("RMobileBroadcastMessaging::StoreBroadcastIdListL did not return KErrNotSupported"));		
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10028);		
    broadcastMessagingStore.Close();
   
    // Use Retrieval helper class to retrieve broadcast Id entries from the list
    TInt err;
    CRetrieveMobilePhoneBroadcastIdListExec* broadcastIdListExec  = CRetrieveMobilePhoneBroadcastIdListHelper::NewL(broadcastMessaging,idType);	
	CleanupStack::PushL(broadcastIdListExec);
	CMobilePhoneBroadcastIdList* broadcastIdList2 = broadcastIdListExec ->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneBroadcastIdList::RetrieveListL  is not able to get the list"))

	// Check CMobilePhoneBroadcastIdList::Enumerate returns 0 entry
	ASSERT_EQUALS(broadcastIdList2->Enumerate(), 0, _L("CMobilePhoneBroadcastIdList::Enumerate did not return an empty list"));

	//
	// TEST END
	//

    StartCleanup();
    // Pop:
    // broadcastIdList
	// storeBroadcastIdList
    // broadcastIdListExec
    CleanupStack::PopAndDestroy(3, broadcastIdList);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0013");
	}

CCTSYIntegrationTestBroadcastMessaging0014::CCTSYIntegrationTestBroadcastMessaging0014(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestBroadcastMessagingBase(aEtelSessionMgr)
/**
* Constructor.
*/
{
SetTestStepName(CCTSYIntegrationTestBroadcastMessaging0014::GetTestStepName());
}

CCTSYIntegrationTestBroadcastMessaging0014::~CCTSYIntegrationTestBroadcastMessaging0014()
/**
* Destructor.
*/
{
}

TVerdict CCTSYIntegrationTestBroadcastMessaging0014::doTestStepL()
/**
* @SYMTestCaseID BA-CTSY-INT-BRMSG-0014
* @SYMFssID BA/CTSY/BRMSG-0014
* @SYMTestCaseDesc Set the broadcast messaging Language filter.
* @SYMTestPriority High
* @SYMTestActions RMobileBroadcastMessaging::NotifyFilterSettingChange,RMobileBroadcastMessaging::GetFilterSetting, RMobileBroadcastMessaging::SetFilterSetting, RMobileBroadcastMessaging::SetLanguageFilter, RMobileBroadcastMessaging::GetLanguageFilter
* @SYMTestExpectedResults Pass - Language filter notifications complete and are set correctly.
* @SYMTestType CIT
* @SYMTestCaseDependencies live/automatic
*
* Reason for test: Verify filter change notification completes and correct filter has been set.
*
* @return - TVerdict code
*/
	{

	//
	// SET UP
	//
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	RMobileBroadcastMessaging& broadcastMessaging = iEtelSessionMgr.GetBroadcastMessagingL(KMainServer, KMainPhone, KMainBroadcastMessaging) ;
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iFilterCaps in set of KCapsSimpleFilter
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1 caps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg capPkg(caps);
	TUint32 expectedCaps  = RMobileBroadcastMessaging::KCapsSimpleFilter;
	
	CHECK_EQUALS_L(broadcastMessaging.GetCaps(capPkg), KErrNone, _L("RMobileBroadcastMessaging::GetCaps returned with an error"));
	CHECK_BITS_SET_L(caps.iFilterCaps,expectedCaps ,KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Filter caps"));
	
	// Ensure RMobileBroadcastMessaging::GetCaps returns aCaps.iModeCaps in set of KCapsGsmTpduFormat
	CHECK_BITS_SET_L(caps.iModeCaps, RMobileBroadcastMessaging::KCapsGsmTpduFormat, KNoUnwantedBits, _L("RMobileBroadcastMessaging::GetCaps did not return desired Mode caps"));
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Check no language filter is set, then set language filter for one language ===
	
	// Check RMobileBroadcastMessaging::GetLanguageFilter completes with KErrNotFound
	TBuf<10> langFilter;   
	TExtEtelRequestStatus getLanguageFilterStatus(broadcastMessaging, EMobileBroadcastMessagingGetLanguageFilter);
	CleanupStack::PushL(getLanguageFilterStatus);
	broadcastMessaging.GetLanguageFilter(getLanguageFilterStatus, langFilter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::GetLanguageFilter timed out"));
	ASSERT_EQUALS(getLanguageFilterStatus.Int(), KErrNotFound, _L("RMobileBroadcastMessaging::GetLanguageFilter returned with a language filtered unexpectedly"));		
	
	// Post a notification RMobileBroadcastMessaging::NotifyLanguageFilterChange
	TBuf<1> langFilterSetting;
	TExtEtelRequestStatus notifyLanguageFilterChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyLanguageFilterChange);
	CleanupStack::PushL(notifyLanguageFilterChangeStatus);
	broadcastMessaging.NotifyLanguageFilterChange(notifyLanguageFilterChangeStatus, langFilterSetting); 
		
	// Set the language filter with RMobileBroadcastMessaging::SetLanguageFilter and aLangFilter="\1" (English, see spec 03.38) 
	_LIT(KEnglish, "\1");
	TBuf<1> filter(KEnglish);  
	TExtEtelRequestStatus setLanguageFilterStatus(broadcastMessaging, EMobileBroadcastMessagingSetLanguageFilter);
	CleanupStack::PushL(setLanguageFilterStatus);
	broadcastMessaging.SetLanguageFilter(setLanguageFilterStatus,filter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetLanguageFilter timed out"));
	ASSERT_EQUALS(setLanguageFilterStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetLanguageFilter returned with an error"));		
		
	// Check RMobileBroadcastMessaging::NotifyLanguageFilterChange completes with aLangFilter="\1"
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyLanguageFilterChange( 
				broadcastMessaging,
				notifyLanguageFilterChangeStatus,
				filter, 
				langFilterSetting,
				KErrNone );
	
	// Check RMobileBroadcastMessaging::GetLanguageFilter completes with aLangFilter="\1"
	broadcastMessaging.GetLanguageFilter(getLanguageFilterStatus, langFilter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::GetLanguageFilter timed out"));
	ASSERT_EQUALS(getLanguageFilterStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::GetLanguageFilter returned with a language filtered unexpectedly"));		
	ASSERT_EQUALS_DES16(langFilter, filter, _L("RMobileBroadcastMessaging::GetLanguageFilter did not complete with a aLangFilter=KEnglish"));		
	
	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting;
	TExtEtelRequestStatus notifyFilterSettingChangeStatus(broadcastMessaging, EMobileBroadcastMessagingNotifyFilterSettingChange);
	CleanupStack::PushL(notifyFilterSettingChangeStatus);
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptFilter 
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSetting1 = RMobileBroadcastMessaging::EBroadcastAcceptFilter;
	TExtEtelRequestStatus setFilterSettingStatus(broadcastMessaging, EMobileBroadcastMessagingSetFilterSetting);
	CleanupStack::PushL(setFilterSettingStatus);
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		
		
	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptFilter
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );

	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptFilter
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter filterSettingObtained;
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptFilter, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))
	
	// ===  Set language filter to filter for no languages ===
	
	// Post a notification RMobileBroadcastMessaging::NotifyLanguageFilterChange
	broadcastMessaging.NotifyLanguageFilterChange(notifyLanguageFilterChangeStatus, langFilterSetting); 
		
	// Set the language filter with RMobileBroadcastMessaging::SetLanguageFilter and aLangFilter=""  
	_LIT(KNone, "");
	filter=KNone;  
	broadcastMessaging.SetLanguageFilter(setLanguageFilterStatus,filter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetLanguageFilter timed out"));
	ASSERT_EQUALS(setLanguageFilterStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetLanguageFilter returned with an error"));		
		
	// Check RMobileBroadcastMessaging::NotifyLanguageFilterChange completes with aLangFilter=""
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyLanguageFilterChange( 
				broadcastMessaging,
				notifyLanguageFilterChangeStatus,
				filter, 
				langFilterSetting,
				KErrNone );
	
	// Check RMobileBroadcastMessaging::GetLanguageFilter completes with KErrNotFound
	broadcastMessaging.GetLanguageFilter(getLanguageFilterStatus, langFilter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::GetLanguageFilter timed out"));
	ASSERT_EQUALS(getLanguageFilterStatus.Int(), KErrNotFound, _L("RMobileBroadcastMessaging::GetLanguageFilter returned with a language filtered unexpectedly"));		
	
	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
		
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptFilter 
	filterSetting1= RMobileBroadcastMessaging::EBroadcastAcceptFilter;
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptFilter
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );
	
	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptFilter
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptFilter, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))
	
	// ===  Check no language filter is set, then set language filter for more than one language ===
	
	// Post a notification RMobileBroadcastMessaging::NotifyLanguageFilterChange
	broadcastMessaging.NotifyLanguageFilterChange(notifyLanguageFilterChangeStatus, langFilterSetting); 
		
	// Set the language filter with RMobileBroadcastMessaging::SetLanguageFilter and aLangFilter="\7\1" (Danish + English, see spec 03.38) 
	_LIT(KMultipleLang, "\7\1");
	filter=KMultipleLang;  
	broadcastMessaging.SetLanguageFilter(setLanguageFilterStatus,filter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetLanguageFilter timed out"));
	ASSERT_EQUALS(setLanguageFilterStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetLanguageFilter returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyLanguageFilterChange completes with aLangFilter="\7\1"
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyLanguageFilterChange( 
			broadcastMessaging,
			notifyLanguageFilterChangeStatus,
			filter, 
			langFilterSetting,
			KErrNone );
	
	// Check RMobileBroadcastMessaging::GetLanguageFilter completes with aLangFilter="\7\1"
	broadcastMessaging.GetLanguageFilter(getLanguageFilterStatus, langFilter);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLanguageFilterStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::GetLanguageFilter timed out"));
	ASSERT_EQUALS(getLanguageFilterStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::GetLanguageFilter returned with a language filtered unexpectedly"));		
	ASSERT_EQUALS_DES16(langFilter, filter, _L("RMobileBroadcastMessaging::GetLanguageFilter did not complete with a aLangFilter=KEnglish"));		
	
	// Post a notification RMobileBroadcastMessaging::NotifyFilterSettingChange
	broadcastMessaging.NotifyFilterSettingChange(notifyFilterSettingChangeStatus, filterSetting); 
	
	// Set the filter with RMobileBroadcastMessaging::SetFilterSetting and aSetting=EBroadcastAcceptFilter 
	filterSetting1= RMobileBroadcastMessaging::EBroadcastAcceptFilter;
	broadcastMessaging.SetFilterSetting(setFilterSettingStatus, filterSetting1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setFilterSettingStatus, ETimeMedium),KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting timed out"));
	ASSERT_EQUALS(setFilterSettingStatus.Int(), KErrNone, _L("RMobileBroadcastMessaging::SetFilterSetting returned with an error"));		

	// Check RMobileBroadcastMessaging::NotifyFilterSettingChange completes with aSetting=EBroadcastAcceptFilter
	iBroadcastMessagingTestHelper.WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
			broadcastMessaging,
			notifyFilterSettingChangeStatus,
			filterSetting, 
			filterSetting1,
			KErrNone );
	
	// Check RMobileBroadcastMessaging::GetFilterSetting returns aSetting=EBroadcastAcceptFilter
	ASSERT_EQUALS(broadcastMessaging.GetFilterSetting(filterSettingObtained), KErrNone, _L("RMobileBroadcastMessaging::GetFilterSetting returned with an error"));
	ASSERT_EQUALS(filterSettingObtained, RMobileBroadcastMessaging::EBroadcastAcceptFilter, _L("RMobileBroadcastMessaging::GetFilterSetting did not return EBroadcastAcceptNone"))
	
	//
	// TEST END
	//
	
	StartCleanup();
	
	// Pop :
	// getLanguageFilterStatus
	// notifyLanguageFilterChangeStatus
	// setLanguageFilterStatus
	// notifyFilterSettingChangeStatus
	// setFilterSettingStatus
	CleanupStack::PopAndDestroy(5, &getLanguageFilterStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestBroadcastMessaging0014::GetTestStepName()
/**
* @return The test step name.
*/
	{
	return _L("CCTSYIntegrationTestBroadcastMessaging0014");
	}



