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
// The TEFUnit test suite for CallForwarding in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallforwardingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cctsyactiveretriever.h"
#include <mmretrieve.h>

CTestSuite* CCTsyCallForwardingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestStart0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestStart0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestSetCallForwardingStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestSetCallForwardingStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestSetCallForwardingStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestSetCallForwardingStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestSetCallForwardingStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestGetCallForwardingIndicator0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestNotifyCallForwardingActive0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestNotifyCallForwardingActive0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestNotifyCallForwardingActive0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestNotifyCallForwardingStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestNotifyCallForwardingStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallForwardingFU, TestNotifyCallForwardingStatusChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


void CCTsyCallForwardingFU::AuxTestStart0001L(
		RMobilePhone::TMobileService aServiceGroup,
		RMobilePhone::TMobileInfoLocation aLocation)
	{

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);

	RMobilePhone::TMobilePhoneCFCondition expCondition(condition);
	RMobilePhone::TMobileService expServiceGroup(aServiceGroup);
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobileService>
    		                                          expLtsyData(expCondition, expServiceGroup);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data, KErrNotSupported);

	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, aServiceGroup, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);

    CMobilePhoneCFList* completeCFList( NULL );
	TMockLtsyData1<CMobilePhoneCFList*> completeData(completeCFList);
	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrGeneral, data);

	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, aServiceGroup, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: increase coverage (completion with null pointer)
 	//-------------------------------------------------------------------------

	data.Close();
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);

	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, aServiceGroup, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: increase coverage (completion with empty array)
 	//-------------------------------------------------------------------------

	data.Close();
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);

    completeCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(completeCFList);

	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, aServiceGroup, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneCFList::Start when result is not cached.
 	//-------------------------------------------------------------------------

	data.Close();
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);
	
	RMobilePhone::TMobilePhoneCFInfoEntryV1	entryCFInfoEntry;
	entryCFInfoEntry.iTimeout              = 20;
	entryCFInfoEntry.iCondition            = RMobilePhone::ECallForwardingUnconditional;
	entryCFInfoEntry.iServiceGroup         = RMobilePhone::EVoiceService;
	entryCFInfoEntry.iStatus               = RMobilePhone::ECallForwardingStatusNotProvisioned;
	entryCFInfoEntry.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	entryCFInfoEntry.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	entryCFInfoEntry.iNumber.iTelNumber.Copy(_L("+441632960000"));
	completeCFList->AddEntryL(entryCFInfoEntry);

	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, aServiceGroup, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCFList* list = asyncRetrieveMobilePhoneCFList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_TRUE( TComparator<CMobilePhoneCFList>::IsEqual(*completeCFList, *list) );
	CleanupStack::PopAndDestroy( list );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // completeCFList, activeRetriever, asyncRetrieveMobilePhoneCFList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(&data);

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-RMPCFLN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCFList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCFList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	while ( serviceGroup != RMobilePhone::EFaxService )
		{
		GetNextServiceGroup(serviceGroup);
		
		AuxTestStart0001L(serviceGroup, RMobilePhone::EInfoLocationCachePreferred);
		
		AuxTestStart0001L(serviceGroup, RMobilePhone::EInfoLocationNetwork);
		}

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when information is taken from cache
 	//-------------------------------------------------------------------------

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	// test
	serviceGroup = RMobilePhone::EServiceUnspecified;
	while ( serviceGroup != RMobilePhone::EFaxService )
		{
		GetNextServiceGroup(serviceGroup);
		
		asyncRetrieveMobilePhoneCFList->Start(
				activeRetriever->iStatus,
				RMobilePhone::ECallForwardingUnconditional,
				serviceGroup,
				RMobilePhone::EInfoLocationCache);
		activeRetriever->Activate();
		scheduler.StartScheduler();

		ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
		// Request completes with KErrNotFound because
		// CTSY does not have a cache for CFList
		ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());

		AssertMockLtsyStatusL();
		}

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneCFList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	RBuf8 data;
	CleanupClosePushL(data);

    CMobilePhoneCFList* completeCFList( NULL );
	TMockLtsyData1<CMobilePhoneCFList*> completeData(completeCFList);
	completeData.SerialiseL(data);

	iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // data, activeRetriever, asyncRetrieveMobilePhoneCFList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-RMPCFLN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneCFList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneCFList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestStart0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneCFList::Start
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobileService expServiceGroup(RMobilePhone::EVoiceService);
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobileService>
    		                                          expLtsyData(expCondition, expServiceGroup);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);

	CMobilePhoneCFList* completeCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(completeCFList);
	RMobilePhone::TMobilePhoneCFInfoEntryV1	entryCFInfoEntry;
	entryCFInfoEntry.iTimeout              = 20;
	entryCFInfoEntry.iCondition            = RMobilePhone::ECallForwardingUnconditional;
	entryCFInfoEntry.iServiceGroup         = RMobilePhone::EVoiceService;
	entryCFInfoEntry.iStatus               = RMobilePhone::ECallForwardingStatusNotProvisioned;
	entryCFInfoEntry.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	entryCFInfoEntry.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	entryCFInfoEntry.iNumber.iTelNumber.Copy(_L("+441632960000"));
	completeCFList->AddEntryL(entryCFInfoEntry);

	TMockLtsyData1<CMobilePhoneCFList*> completeData(completeCFList);
	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data, 10);

	RMobilePhone::TMobilePhoneCFCondition condition(expCondition);
	RMobilePhone::TMobileService serviceGroup(expServiceGroup);
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup);
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneCFList::Start
	// this time cancel on 2nd phase
 	//-------------------------------------------------------------------------
	data.Close();
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);
	
	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup);
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // completeCFList, activeRetriever, asyncRetrieveMobilePhoneCFList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-RMPCFLN-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCFList::Start with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCFList::Start with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestStart0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EVoiceService);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// CRetrieveMobilePhoneCFList::Start
 	//-------------------------------------------------------------------------
 	
	condition = RMobilePhone::ECallForwardingAllCases; // not supported
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrArgument, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	condition = RMobilePhone::ECallForwardingAllConditionalCases; // not supported
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrArgument, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // activeRetriever, asyncRetrieveMobilePhoneCFList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-RMPCFLN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneCFList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneCFList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestStart0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList2 =
	                                       CRetrieveMobilePhoneCFList::NewL(phone2);
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList2);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever2 = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList2);
	CleanupStack::PushL(activeRetriever2);
	scheduler.AddRetrieverL(*activeRetriever2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneCFList::Start
 	//-------------------------------------------------------------------------

    // setting and execute 1st request
	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobileService expServiceGroup(RMobilePhone::EVoiceService);
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobileService>
    		                                          expLtsyData(expCondition, expServiceGroup);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);

	CMobilePhoneCFList* completeCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(completeCFList);
	RMobilePhone::TMobilePhoneCFInfoEntryV1	entryCFInfoEntry;
	entryCFInfoEntry.iTimeout              = 20;
	entryCFInfoEntry.iCondition            = RMobilePhone::ECallForwardingUnconditional;
	entryCFInfoEntry.iServiceGroup         = RMobilePhone::EVoiceService;
	entryCFInfoEntry.iStatus               = RMobilePhone::ECallForwardingStatusNotProvisioned;
	entryCFInfoEntry.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	entryCFInfoEntry.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	entryCFInfoEntry.iNumber.iTelNumber.Copy(_L("+441632960000"));
	completeCFList->AddEntryL(entryCFInfoEntry);

	TMockLtsyData1<CMobilePhoneCFList*> completeData(completeCFList);
	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetCallForwardingStatusPhase1, KErrNone, data);

	RMobilePhone::TMobileInfoLocation location(RMobilePhone::EInfoLocationNetwork);
	RMobilePhone::TMobilePhoneCFCondition condition(expCondition);
	RMobilePhone::TMobileService serviceGroup(expServiceGroup);
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup, location);
	activeRetriever->Activate();

    // setting and execute 2nd request
	RMobilePhone::TMobilePhoneCFCondition condition2(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobileService serviceGroup2(RMobilePhone::EVoiceService);
	asyncRetrieveMobilePhoneCFList2->Start(activeRetriever2->iStatus, condition2, serviceGroup2);
	activeRetriever2->Activate();

	// invoke scheduler
	scheduler.StartScheduler();

	// check results
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrServerBusy, activeRetriever2->iStatus.Int());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCFList* list = asyncRetrieveMobilePhoneCFList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_TRUE( TComparator<CMobilePhoneCFList>::IsEqual(*completeCFList, *list) );
	CleanupStack::PopAndDestroy( list );

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // completeCFList, activeRetriever2, asyncRetrieveMobilePhoneCFList2
	CleanupStack::PopAndDestroy(2); // activeRetriever, asyncRetrieveMobilePhoneCFList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-RMPCFLN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCFList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCFList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestStart0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of CRetrieveMobilePhoneCFList::Start
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobileService expServiceGroup(RMobilePhone::EVoiceService);
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobileService>
    		                                          expLtsyData(expCondition, expServiceGroup);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetCallForwardingStatusPhase1, data);

	RMobilePhone::TMobilePhoneCFCondition condition(expCondition);
	RMobilePhone::TMobileService serviceGroup(expServiceGroup);
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // activeRetriever, asyncRetrieveMobilePhoneCFList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MSCFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallForwardingStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallForwardingStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestSetCallForwardingStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 expInfo;
	expInfo.iTimeout              = 20;
	expInfo.iServiceGroup         = RMobilePhone::EVoiceService;
	expInfo.iAction               = RMobilePhone::EServiceActionActivate;
	expInfo.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	expInfo.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	expInfo.iNumber.iTelNumber.Copy(_L("+441632960000"));
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobilePhoneCFChangeV1>
    		                                          expLtsydata(expCondition, expInfo);
	expLtsydata.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallForwardingStatus, data, KErrNotSupported);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 info;
	info.iTimeout              = expInfo.iTimeout;
	info.iServiceGroup         = expInfo.iServiceGroup;
	info.iAction               = expInfo.iAction;
	info.iNumber.iTypeOfNumber = expInfo.iNumber.iTypeOfNumber;
	info.iNumber.iNumberPlan   = expInfo.iNumber.iNumberPlan;
	info.iNumber.iTelNumber.Copy(expInfo.iNumber.iTelNumber);
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	expInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	data.Close();
	expLtsydata.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallForwardingStatus, data);
    iMockLTSY.CompleteL(EMobilePhoneSetCallForwardingStatus, KErrGeneral);

	info.iAction = expInfo.iAction;
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetCallForwardingStatus when result is not cached.
 	//-------------------------------------------------------------------------

	expInfo.iAction = RMobilePhone::EServiceActionRegister;
	data.Close();
	expLtsydata.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallForwardingStatus, data);
    iMockLTSY.CompleteL(EMobilePhoneSetCallForwardingStatus, KErrNone);

	info.iAction = expInfo.iAction;
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetCallForwardingStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobilePhoneSetCallForwardingStatus, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MSCFS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetCallForwardingStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetCallForwardingStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestSetCallForwardingStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetCallForwardingStatus
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 expInfo;
	expInfo.iTimeout              = 20;
	expInfo.iServiceGroup         = RMobilePhone::EVoiceService;
	expInfo.iAction               = RMobilePhone::EServiceActionErase;
	expInfo.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	expInfo.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	expInfo.iNumber.iTelNumber.Copy(_L("+441632960000"));
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobilePhoneCFChangeV1>
    		                                          expLtsydata(expCondition, expInfo);
	expLtsydata.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallForwardingStatus, data);
    iMockLTSY.CompleteL(EMobilePhoneSetCallForwardingStatus, KErrNone, 10);

	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 info;
	info.iTimeout              = expInfo.iTimeout;
	info.iServiceGroup         = expInfo.iServiceGroup;
	info.iAction               = expInfo.iAction;
	info.iNumber.iTypeOfNumber = expInfo.iNumber.iTypeOfNumber;
	info.iNumber.iNumberPlan   = expInfo.iNumber.iNumberPlan;
	info.iNumber.iTelNumber.Copy(expInfo.iNumber.iTelNumber);
    TRequestStatus reqStatus;
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);
	
	iPhone.CancelAsyncRequest(EMobilePhoneSetCallForwardingStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MSCFS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallForwardingStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallForwardingStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestSetCallForwardingStatus0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobilePhone::SetCallForwardingStatus
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 info;
	info.iTimeout              = 20;
	info.iServiceGroup         = RMobilePhone::EVoiceService;
	info.iAction               = RMobilePhone::EServiceActionInvoke; // error argument
	info.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	info.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	info.iNumber.iTelNumber.Copy(_L("+441632960000"));
    TRequestStatus reqStatus;
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	info.iAction               = RMobilePhone::EServiceActionUnspecified; // error argument
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MSCFS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetCallForwardingStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetCallForwardingStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestSetCallForwardingStatus0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetCallForwardingStatus
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 expInfo;
	expInfo.iTimeout              = 20;
	expInfo.iServiceGroup         = RMobilePhone::EVoiceService;
	expInfo.iAction               = RMobilePhone::EServiceActionActivate;
	expInfo.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	expInfo.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	expInfo.iNumber.iTelNumber.Copy(_L("+441632960000"));
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobilePhoneCFChangeV1>
    		                                          expLtsydata(expCondition, expInfo);
	expLtsydata.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallForwardingStatus, data);
    iMockLTSY.CompleteL(EMobilePhoneSetCallForwardingStatus, KErrNone);

	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 info;
	info.iTimeout              = expInfo.iTimeout;
	info.iServiceGroup         = expInfo.iServiceGroup;
	info.iAction               = expInfo.iAction;
	info.iNumber.iTypeOfNumber = expInfo.iNumber.iTypeOfNumber;
	info.iNumber.iNumberPlan   = expInfo.iNumber.iNumberPlan;
	info.iNumber.iTelNumber.Copy(expInfo.iNumber.iTelNumber);
    TRequestStatus reqStatus;
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);

	// setting and execute 2nd request
	RMobilePhone::TMobilePhoneCFCondition condition2(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 info2;
	info2.iTimeout              = 25;
	info2.iServiceGroup         = RMobilePhone::EFaxService;
	info2.iAction               = RMobilePhone::EServiceActionDeactivate;
	info2.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	info2.iNumber.iNumberPlan   = RMobilePhone::EDataNumberPlan;
	info2.iNumber.iTelNumber.Copy(_L("+441632960000"));
    TRequestStatus reqStatus2;
	phone2.SetCallForwardingStatus(reqStatus2, condition2, info2);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MSCFS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallForwardingStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallForwardingStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestSetCallForwardingStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetCallForwardingStatus
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneCFCondition expCondition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 expInfo;
	expInfo.iTimeout              = 20;
	expInfo.iServiceGroup         = RMobilePhone::EVoiceService;
	expInfo.iAction               = RMobilePhone::EServiceActionActivate;
	expInfo.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	expInfo.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	expInfo.iNumber.iTelNumber.Copy(_L("+441632960000"));
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobilePhoneCFChangeV1>
    		                                          expLtsydata(expCondition, expInfo);
	expLtsydata.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallForwardingStatus, data);

	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnconditional);
	RMobilePhone::TMobilePhoneCFChangeV1 info;
	info.iTimeout              = expInfo.iTimeout;
	info.iServiceGroup         = expInfo.iServiceGroup;
	info.iAction               = expInfo.iAction;
	info.iNumber.iTypeOfNumber = expInfo.iNumber.iTypeOfNumber;
	info.iNumber.iNumberPlan   = expInfo.iNumber.iNumberPlan;
	info.iNumber.iTelNumber.Copy(expInfo.iNumber.iTelNumber);
    TRequestStatus reqStatus;
	iPhone.SetCallForwardingStatus(reqStatus, condition, info);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MGCFI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCallForwardingIndicator
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCallForwardingIndicator
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestGetCallForwardingIndicator0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneCFUIndicatorV3 indicator;
	RMobilePhone::TMobilePhoneCFUIndicatorV3Pckg indicatorPckg(indicator);
	TRequestStatus reqStatus;
	iPhone.GetCallForwardingIndicator(reqStatus, indicatorPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MNCFA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCallForwardingActive
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallForwardingActive
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestNotifyCallForwardingActive0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCallForwardingActive when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	RMobilePhone::TMobilePhoneCFActive activeType(RMobilePhone::ECFUnconditionalActive);
	iPhone.NotifyCallForwardingActive(reqStatus, serviceGroup, activeType);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	RMobilePhone::TMobileService completeServiceGroup(RMobilePhone::EVoiceService);
	RMobilePhone::TMobilePhoneCFActive completeActiveType(RMobilePhone::ECFConditionalActive);
	TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCFActive>
	                               completeLtsyData(completeServiceGroup, completeActiveType);
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallForwardingActive, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeServiceGroup, serviceGroup);
	ASSERT_EQUALS(completeActiveType,   activeType);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyCallForwardingActive
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallForwardingActive, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MNCFA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCallForwardingActive
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCallForwardingActive
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestNotifyCallForwardingActive0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCallForwardingActive
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EVoiceService);
	RMobilePhone::TMobilePhoneCFActive activeType(RMobilePhone::ECFConditionalActive);
	iPhone.NotifyCallForwardingActive(reqStatus, serviceGroup, activeType);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingActive);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MNCFA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCallForwardingActive
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCallForwardingActive
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestNotifyCallForwardingActive0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCallForwardingActive
 	//-------------------------------------------------------------------------

	// send 1st request
	TRequestStatus reqStatus;
	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	RMobilePhone::TMobilePhoneCFActive activeType(RMobilePhone::ECFUnconditionalActive);
	iPhone.NotifyCallForwardingActive(reqStatus, serviceGroup, activeType);

	// send 2nd request
	TRequestStatus reqStatus2;
	RMobilePhone::TMobileService serviceGroup2(RMobilePhone::EServiceUnspecified);
	RMobilePhone::TMobilePhoneCFActive activeType2(RMobilePhone::ECFUnconditionalActive);
	phone2.NotifyCallForwardingActive(reqStatus2, serviceGroup2, activeType2);

	// invoke change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	RMobilePhone::TMobileService completeServiceGroup(RMobilePhone::EVoiceService);
	RMobilePhone::TMobilePhoneCFActive completeActiveType(RMobilePhone::ECFConditionalActive);
	TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCFActive>
	                               completeLtsyData(completeServiceGroup, completeActiveType);
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallForwardingActive, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeServiceGroup, serviceGroup);
	ASSERT_EQUALS(completeActiveType,   activeType);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(completeServiceGroup, serviceGroup2);
	ASSERT_EQUALS(completeActiveType,   activeType2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MNCFSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCallForwardingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallForwardingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestNotifyCallForwardingStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCallForwardingStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnspecified);
	iPhone.NotifyCallForwardingStatusChange(reqStatus, condition);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	RMobilePhone::TMobilePhoneCFCondition completeCondition(RMobilePhone::ECallForwardingUnconditional);
	TMockLtsyData1<RMobilePhone::TMobilePhoneCFCondition>
	                               completeLtsyData(completeCondition);
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallForwardingStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeCondition, condition);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyCallForwardingStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallForwardingStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MNCFSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCallForwardingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCallForwardingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestNotifyCallForwardingStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCallForwardingStatusChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnspecified);
	iPhone.NotifyCallForwardingStatusChange(reqStatus, condition);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingStatusChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CFOR-MNCFSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCallForwardingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCallForwardingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallForwardingFU::TestNotifyCallForwardingStatusChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCallForwardingStatusChange
 	//-------------------------------------------------------------------------

	// send 1st request
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCFCondition condition(RMobilePhone::ECallForwardingUnspecified);
	iPhone.NotifyCallForwardingStatusChange(reqStatus, condition);

	// send 2nd request
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneCFCondition condition2(RMobilePhone::ECallForwardingUnspecified);
	phone2.NotifyCallForwardingStatusChange(reqStatus2, condition2);

	// invoke change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	RMobilePhone::TMobilePhoneCFCondition completeCondition(RMobilePhone::ECallForwardingUnconditional);
	TMockLtsyData1<RMobilePhone::TMobilePhoneCFCondition>
	                               completeLtsyData(completeCondition);
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallForwardingStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// chexk results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeCondition, condition);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(completeCondition, condition2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
