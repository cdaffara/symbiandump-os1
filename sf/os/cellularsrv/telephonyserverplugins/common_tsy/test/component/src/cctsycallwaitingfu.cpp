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
// The TEFUnit test suite for CallWaiting in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallwaitingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cctsyactiveretriever.h"
#include <mmretrieve.h>

CTestSuite* CCTsyCallWaitingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestStart0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestSetCallWaitingStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestSetCallWaitingStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestSetCallWaitingStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestSetCallWaitingStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestSetCallWaitingStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestNotifyCallWaitingStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestNotifyCallWaitingStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestNotifyCallWaitingStatusChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallWaitingFU, TestNotifyCallWaitingStatusChange0004L);

	END_SUITE;
	}


void CCTsyCallWaitingFU::GetNextUnsupportedServiceAction(RMobilePhone::TMobilePhoneServiceAction &aAction)
	{
	switch ( aAction )
		{
		case RMobilePhone::EServiceActionUnspecified:
			aAction = RMobilePhone::EServiceActionRegister;
			break;
		case RMobilePhone::EServiceActionRegister:
			aAction = RMobilePhone::EServiceActionInvoke;
			break;
		case RMobilePhone::EServiceActionInvoke:
			aAction = RMobilePhone::EServiceActionErase;
			break;
		default:
			aAction = RMobilePhone::EServiceActionUnspecified;
			break;
		}
	}


void CCTsyCallWaitingFU::FillInCWInfoEntry(
		RMobilePhone::TMobilePhoneCWInfoEntryV1 &aCWInfoEntry,
		RMobilePhone::TMobileService aServiceGroup,
		RMobilePhone::TMobilePhoneServiceAction aAction)
	{
	aCWInfoEntry.iServiceGroup = aServiceGroup;
	if ( aAction == RMobilePhone::EServiceActionActivate )
		{
		aCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusActive;
		}
	else
		{
		aCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
		}
	}


//
// Actual test cases
//


void CCTsyCallWaitingFU::AuxTestStart0001L(RMobilePhone::TMobileInfoLocation aLocation)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList =
	                                       CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1, KErrNotSupported);

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	CMobilePhoneCWList* completeCWList( NULL );
	TMockLtsyData1<CMobilePhoneCWList*> completeData(completeCWList);
	completeData.SerialiseL(data);
	
	iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrGeneral, data);

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: increase coverage (completion with null pointer)
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// CRetrieveMobilePhoneCWList::Start when LTSY returns empty array
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);

	completeCWList = CMobilePhoneCWList::NewL();
	CleanupStack::PushL(completeCWList);
	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());	
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCWList* list = asyncRetrieveMobilePhoneCWList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_TRUE( TComparator<CMobilePhoneCWList>::IsEqual(*completeCWList, *list) );
	CleanupStack::PopAndDestroy( list );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneCWList::Start.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);

    RMobilePhone::TMobilePhoneCWInfoEntryV1	entryCWInfoEntry;
	entryCWInfoEntry.iServiceGroup = RMobilePhone::EVoiceService;
	entryCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotProvisioned;
	completeCWList->AddEntryL(entryCWInfoEntry);
	
	data.Close();
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, aLocation);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	list = asyncRetrieveMobilePhoneCWList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_TRUE( TComparator<CMobilePhoneCWList>::IsEqual(*completeCWList, *list) );
	CleanupStack::PopAndDestroy( list );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // completeCWList, activeRetriever, asyncRetrieveMobilePhoneCWList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(&data);

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-RMPCWLN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCWList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCWList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	AuxTestStart0001L(RMobilePhone::EInfoLocationCachePreferred);

	AuxTestStart0001L(RMobilePhone::EInfoLocationNetwork);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when information is taken from cache
 	//-------------------------------------------------------------------------

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList =
	                                       CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	// test
	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, RMobilePhone::EInfoLocationCache);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	// Request completes with KErrNotFound because
	// CTSY does not have a cache for CWList
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneCWList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	RBuf8 data;
	CleanupClosePushL(data);

	CMobilePhoneCWList* completeCWList( NULL );
	TMockLtsyData1<CMobilePhoneCWList*> completeData(completeCWList);
	completeData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data);
	
    User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // data, activeRetriever, asyncRetrieveMobilePhoneCWList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CWAT-RMPCWLN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneCWList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneCWList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestStart0002L()
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

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList =
	                                       CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneCWList::Start
 	//-------------------------------------------------------------------------
 	
    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);

	CMobilePhoneCWList* completeCWList = CMobilePhoneCWList::NewL();
	CleanupStack::PushL(completeCWList);
	RMobilePhone::TMobilePhoneCWInfoEntryV1	entryCWInfoEntry;
	entryCWInfoEntry.iServiceGroup = RMobilePhone::EVoiceService;
	entryCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotProvisioned;
	completeCWList->AddEntryL(entryCWInfoEntry);

	TMockLtsyData1<CMobilePhoneCWList*> completeData(completeCWList);
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data, 10);

	RMobilePhone::TMobileInfoLocation location(RMobilePhone::EInfoLocationCachePreferred);
	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, location);
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneCWList::Start
	// this time cancel on 2nd phase
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data);

	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, location);
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // completeCWList, activeRetriever, asyncRetrieveMobilePhoneCWList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CWAT-RMPCWLN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneCWList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneCWList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestStart0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList =
	                                       CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
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

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList2 =
	                                       CRetrieveMobilePhoneCWList::NewL(phone2);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList2);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever2 = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList2);
	CleanupStack::PushL(activeRetriever2);
	scheduler.AddRetrieverL(*activeRetriever2);
 	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneCWList::Start
 	//-------------------------------------------------------------------------

    // setting and execute 1st request
    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);

	CMobilePhoneCWList* completeCWList = CMobilePhoneCWList::NewL();
	CleanupStack::PushL(completeCWList);
	RMobilePhone::TMobilePhoneCWInfoEntryV1	entryCWInfoEntry;
	entryCWInfoEntry.iServiceGroup = RMobilePhone::EVoiceService;
	entryCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotProvisioned;
	completeCWList->AddEntryL(entryCWInfoEntry);

	TMockLtsyData1<CMobilePhoneCWList*> completeData(completeCWList);
	completeData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetWaitingStatusPhase1, KErrNone, data);

	RMobilePhone::TMobileInfoLocation location(RMobilePhone::EInfoLocationNetwork);
	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, location);
	activeRetriever->Activate();

    // setting and execute 2nd request
	RMobilePhone::TMobileInfoLocation location2(RMobilePhone::EInfoLocationCachePreferred);
	asyncRetrieveMobilePhoneCWList2->Start(activeRetriever2->iStatus, location2);
	activeRetriever2->Activate();

	// invoke scheduler
	scheduler.StartScheduler();

	// check results
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrServerBusy, activeRetriever2->iStatus.Int());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCWList* list = asyncRetrieveMobilePhoneCWList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_TRUE( TComparator<CMobilePhoneCWList>::IsEqual(*completeCWList, *list) );
	CleanupStack::PopAndDestroy( list );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // completeCWList, activeRetriever2, asyncRetrieveMobilePhoneCWList2
	CleanupStack::PopAndDestroy(2); // activeRetriever, asyncRetrieveMobilePhoneCWList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-RMPCWLN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneCWList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneCWList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestStart0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList =
	                                       CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of CRetrieveMobilePhoneCWList::Start
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetWaitingStatusPhase1);

	RMobilePhone::TMobileInfoLocation location(RMobilePhone::EInfoLocationCachePreferred);
	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus, location);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // activeRetriever, asyncRetrieveMobilePhoneCWList
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(this);

	}


void CCTsyCallWaitingFU::AuxTestSetCallWaitingStatus0001L(
		RMobilePhone::TMobileService aServiceGroup,
		RMobilePhone::TMobilePhoneServiceAction aAction)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
	//-------------------------------------------------------------------------

	RMobilePhone::TMobileService expServiceGroup = aServiceGroup;
    RMobilePhone::TMobilePhoneServiceAction expAction = aAction;
    TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction>
	                  expLtsyData(expServiceGroup, expAction);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallWaitingStatus, data, KErrNotSupported);

    TRequestStatus reqStatus;
	iPhone.SetCallWaitingStatus(reqStatus, aServiceGroup, aAction);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSetCallWaitingStatus, data);
    iMockLTSY.CompleteL(EMobilePhoneSetCallWaitingStatus, KErrGeneral);

	iPhone.SetCallWaitingStatus(reqStatus, aServiceGroup, aAction);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetCallWaitingStatus when result is not cached.
	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSetCallWaitingStatus, data);

	RMobilePhone::TMobilePhoneCWInfoEntryV1 completeCWInfoEntry;
	FillInCWInfoEntry(completeCWInfoEntry, aServiceGroup, aAction);
	TMockLtsyData1<RMobilePhone::TMobilePhoneCWInfoEntryV1> completeLtsyData(completeCWInfoEntry);
	data.Close();
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallWaitingStatusChange, KErrNone, data);

    iMockLTSY.CompleteL(EMobilePhoneSetCallWaitingStatus, KErrNone);

    iPhone.SetCallWaitingStatus(reqStatus, aServiceGroup, aAction);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&data);
	
	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MSCWS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallWaitingStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallWaitingStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestSetCallWaitingStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	
	while ( serviceGroup != RMobilePhone::EFaxService )
		{
		GetNextServiceGroup(serviceGroup);

		AuxTestSetCallWaitingStatus0001L(serviceGroup, RMobilePhone::EServiceActionActivate);

		AuxTestSetCallWaitingStatus0001L(serviceGroup, RMobilePhone::EServiceActionDeactivate);
		}

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetCallWaitingStatus
	// from LTSY.
	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobilePhoneSetCallWaitingStatus, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


void CCTsyCallWaitingFU::AuxTestSetCallWaitingStatus0002L(
		RMobilePhone::TMobileService aServiceGroup,
		RMobilePhone::TMobilePhoneServiceAction aAction)
	{

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetCallWaitingStatus
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobileService expServiceGroup(aServiceGroup);
    RMobilePhone::TMobilePhoneServiceAction expAction(aAction);
    TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction>
	                  expLtsyData(expServiceGroup, expAction);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallWaitingStatus, data, KErrNone);

    RMobilePhone::TMobilePhoneCWInfoEntryV1 completeCWInfoEntry;
	FillInCWInfoEntry(completeCWInfoEntry, aServiceGroup, aAction);
	TMockLtsyData1<RMobilePhone::TMobilePhoneCWInfoEntryV1> completeLtsyData(completeCWInfoEntry);
	data.Close();
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallWaitingStatusChange, KErrNone, data, 10);

    iMockLTSY.CompleteL(EMobilePhoneSetCallWaitingStatus, KErrNone, 10);

    TRequestStatus reqStatus;
	iPhone.SetCallWaitingStatus(reqStatus, aServiceGroup, aAction);

	iPhone.CancelAsyncRequest(EMobilePhoneSetCallWaitingStatus);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&data);

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MSCWS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetCallWaitingStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetCallWaitingStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestSetCallWaitingStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	
	while ( serviceGroup != RMobilePhone::EFaxService )
		{
		GetNextServiceGroup(serviceGroup);

		AuxTestSetCallWaitingStatus0002L(serviceGroup, RMobilePhone::EServiceActionActivate);

		AuxTestSetCallWaitingStatus0002L(serviceGroup, RMobilePhone::EServiceActionDeactivate);
		}

	CleanupStack::PopAndDestroy(this);
	
	}


void CCTsyCallWaitingFU::AuxTestSetCallWaitingStatus0003L(
		RMobilePhone::TMobileService aServiceGroup,
		RMobilePhone::TMobilePhoneServiceAction aAction)
	{
	TRequestStatus reqStatus;
	iPhone.SetCallWaitingStatus(reqStatus, aServiceGroup, aAction);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();
	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MSCWS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallWaitingStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallWaitingStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestSetCallWaitingStatus0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobilePhone::SetCallWaitingStatus
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EServiceUnspecified);
	
	while ( serviceGroup != RMobilePhone::EFaxService )
		{
		GetNextServiceGroup(serviceGroup);
		RMobilePhone::TMobilePhoneServiceAction action(RMobilePhone::EServiceActionActivate);

		while ( action != RMobilePhone::EServiceActionErase )
			{
			GetNextUnsupportedServiceAction(action);
			
			// action parameter is error argument
			AuxTestSetCallWaitingStatus0003L(serviceGroup, action);
			}
		}

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MSCWS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetCallWaitingStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetCallWaitingStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestSetCallWaitingStatus0004L()
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

	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EVoiceService);
    RMobilePhone::TMobilePhoneServiceAction action(RMobilePhone::EServiceActionDeactivate);

    //-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetCallWaitingStatus
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	RMobilePhone::TMobileService expServiceGroup(serviceGroup);
    RMobilePhone::TMobilePhoneServiceAction expAction(action);
    TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction>
	                  expLtsyData(expServiceGroup, expAction);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallWaitingStatus, data, KErrNone);

    RMobilePhone::TMobilePhoneCWInfoEntryV1 completeCWInfoEntry;
	FillInCWInfoEntry(completeCWInfoEntry, serviceGroup, action);
	TMockLtsyData1<RMobilePhone::TMobilePhoneCWInfoEntryV1> completeLtsyData(completeCWInfoEntry);
	data.Close();
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallWaitingStatusChange, KErrNone, data);

    iMockLTSY.CompleteL(EMobilePhoneSetCallWaitingStatus, KErrNone);

    TRequestStatus reqStatus;
	iPhone.SetCallWaitingStatus(reqStatus, serviceGroup, action);

	// setting and execute 2nd request
	RMobilePhone::TMobileService serviceGroup2(RMobilePhone::EFaxService);
    RMobilePhone::TMobilePhoneServiceAction action2(RMobilePhone::EServiceActionActivate);
    TRequestStatus reqStatus2;
	phone2.SetCallWaitingStatus(reqStatus2, serviceGroup2, action2);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MSCWS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetCallWaitingStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallWaitingStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestSetCallWaitingStatus0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetCallWaitingStatus
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobileService expServiceGroup(RMobilePhone::EVoiceService);
    RMobilePhone::TMobilePhoneServiceAction expAction(RMobilePhone::EServiceActionActivate);
    TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction>
	                  expLtsyData(expServiceGroup, expAction);
	expLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetCallWaitingStatus, data, KErrNone);

	RMobilePhone::TMobileService serviceGroup(RMobilePhone::EVoiceService);
    RMobilePhone::TMobilePhoneServiceAction action(RMobilePhone::EServiceActionActivate);
    TRequestStatus reqStatus;
	iPhone.SetCallWaitingStatus(reqStatus, serviceGroup, action);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MCWSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCallWaitingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallWaitingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestNotifyCallWaitingStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCallWaitingStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfoEntry;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg cwStatus(cwInfoEntry);
	iPhone.NotifyCallWaitingStatusChange(reqStatus, cwStatus);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	
	RMobilePhone::TMobilePhoneCWInfoEntryV1 completeCWInfoEntry;
	completeCWInfoEntry.iServiceGroup = RMobilePhone::EVoiceService;
	completeCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCWInfoEntryV1> completeLtsyData(completeCWInfoEntry);
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallWaitingStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeCWInfoEntry.iServiceGroup, cwInfoEntry.iServiceGroup);
	ASSERT_EQUALS(completeCWInfoEntry.iStatus, cwInfoEntry.iStatus);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyCallWaitingStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallWaitingStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MCWSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCallWaitingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCallWaitingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestNotifyCallWaitingStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCallWaitingStatusChange
	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfoEntry;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg cwStatus(cwInfoEntry);
	iPhone.NotifyCallWaitingStatusChange(reqStatus, cwStatus);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallWaitingStatusChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MCWSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCallWaitingStatusChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallWaitingStatusChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestNotifyCallWaitingStatusChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::NotifyCallWaitingStatusChange
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	RMobilePhone::TMobilePhoneCWInfoEntryV1* cwInfoEntryPtr(NULL);
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg cwStatusPtr(*cwInfoEntryPtr);
	iPhone.NotifyCallWaitingStatusChange(reqStatus, cwStatusPtr);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrBadDescriptor, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::NotifyCallWaitingStatusChange
 	//-------------------------------------------------------------------------

	TBuf8<1> wrongCWStatus;
	iPhone.NotifyCallWaitingStatusChange(reqStatus, wrongCWStatus);
	
	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-CWAT-MCWSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCallWaitingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCallWaitingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallWaitingFU::TestNotifyCallWaitingStatusChange0004L()
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
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCallWaitingStatusChange
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfoEntry;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg cwStatus(cwInfoEntry);
	iPhone.NotifyCallWaitingStatusChange(reqStatus, cwStatus);

	// setting and execute 2nd request
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfoEntry2;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg cwStatus2(cwInfoEntry2);
	phone2.NotifyCallWaitingStatusChange(reqStatus2, cwStatus2);

	// invoke call waiting status change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	
	RMobilePhone::TMobilePhoneCWInfoEntryV1 completeCWInfoEntry;
	completeCWInfoEntry.iServiceGroup = RMobilePhone::EVoiceService;
	completeCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCWInfoEntryV1> completeLtsyData(completeCWInfoEntry);
	completeLtsyData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCallWaitingStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeCWInfoEntry.iServiceGroup, cwInfoEntry.iServiceGroup);
	ASSERT_EQUALS(completeCWInfoEntry.iStatus, cwInfoEntry.iStatus);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(completeCWInfoEntry.iServiceGroup, cwInfoEntry2.iServiceGroup);
	ASSERT_EQUALS(completeCWInfoEntry.iStatus, cwInfoEntry2.iStatus);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
