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
// The TEFUnit test suite for SMSPStore in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysmspstorefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "cctsyactiveretriever.h"
#include <mmretrieve.h>

CTestSuite* CCTsySMSPStoreFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySMSPStoreFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySMSPStoreFU, TestStart0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySMSPStoreFU, TestStart0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySMSPStoreFU, TestStart0005L);

	END_SUITE;
	}

/**
To push a CleanupItem ont the cleanupstack to close the phone
*/
void CCTsySMSPStoreFU::Cleanup(TAny* aSelf)
	{
	static_cast<CCTsySMSPStoreFU*>(aSelf)->DoCleanup();
	}
	
/**
Close the Phone a the server connection
*/	
void CCTsySMSPStoreFU::DoCleanup()
	{
	CActiveScheduler::Install(NULL);
	CCtsyComponentTestBase::DoCleanup();
	}


LOCAL_C void FillInSmsParamsAndListL(
		CArrayPtrFlat<TSmsParameters>& aSmsParamsList,
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

	aSmsParamsList.AppendL(&aSmsParams1);
	aSmsParamsList.AppendL(&aSmsParams2);
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SMSP-CRMPSL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmspList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmspList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySMSPStoreFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

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
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1, KErrNotSupported);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	// test for KErrNotFound
    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1, KErrNotFound);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotFound, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrGeneral);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: completion with empty array
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);

	CArrayPtrFlat<TSmsParameters>* completeList = new (ELeave) CArrayPtrFlat<TSmsParameters>(1);
	CleanupStack::PushL(completeList);
	TMockLtsyData1<CArrayPtrFlat<TSmsParameters>*> completeLtsyData(completeList);
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	CMobilePhoneSmspList* list = asyncRetrieveList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_EQUALS(0, list->Enumerate());
    CleanupStack::PopAndDestroy(list);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneSmspList::Start when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);

	TSmsParameters entry;
	TSmsParameters entry1;
	FillInSmsParamsAndListL(*completeList, entry, entry1);
	data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	
	list = asyncRetrieveList->RetrieveListL();
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
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneSmspList::Start
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	completeList->Reset();
    data.Close();
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	// increase coverage
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrGeneral);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(completeList);

	//-------------------------------------------------------------------------
	// TEST: completion with null pointer
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);
	completeList = NULL;
	data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // activeRetriever, asyncRetrieveList, messaging
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-SMSP-CRMPSL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneSmspList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneSmspList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySMSPStoreFU::TestStart0002L()
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
	scheduler.AddRetrieverL(*activeRetriever);

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrieveMobilePhoneSmspList::Start
 	//-------------------------------------------------------------------------
 	
    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);

	CArrayPtrFlat<TSmsParameters>* completeList = new (ELeave) CArrayPtrFlat<TSmsParameters>(1);
	CleanupStack::PushL(completeList);
	TSmsParameters entry;
	TSmsParameters entry1;
	FillInSmsParamsAndListL(*completeList, entry, entry1);

	TMockLtsyData1<CArrayPtrFlat<TSmsParameters>*> completeLtsyData(completeList);
	completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data, 10);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// phase 2 cancel test case
 	//-------------------------------------------------------------------------
	
    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); // completeList, activeRetriever, asyncRetrieveList, messaging
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-SMSP-CRMPSL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneSmspList::Start
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneSmspList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySMSPStoreFU::TestStart0004L()
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

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

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
	scheduler.AddRetrieverL(*activeRetriever);

	RMobileSmsMessaging messaging2;
	err = messaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(messaging2);
	CRetrieveMobilePhoneSmspList* asyncRetrieveList2 =
	                                       CRetrieveMobilePhoneSmspList::NewL(messaging2);
	CleanupStack::PushL(asyncRetrieveList2);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever2 = 
						CActiveRetriever::NewL(*asyncRetrieveList2);
	CleanupStack::PushL(activeRetriever2);
	scheduler.AddRetrieverL(*activeRetriever2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneSmspList::Start
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);

	CArrayPtrFlat<TSmsParameters>* completeList = new (ELeave) CArrayPtrFlat<TSmsParameters>(1);
	CleanupStack::PushL(completeList);
	TSmsParameters entry;
	TSmsParameters entry1;
	FillInSmsParamsAndListL(*completeList, entry, entry1);

	TMockLtsyData1<CArrayPtrFlat<TSmsParameters>*> completeLtsyData(completeList);
    completeLtsyData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobileSmsMessagingGetSmspListPhase1, KErrNone, data, 10);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();

	asyncRetrieveList2->Start(activeRetriever2->iStatus);
	activeRetriever2->Activate();

	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	ASSERT_EQUALS(KErrServerBusy, activeRetriever2->iStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // completeList, activeRetriever2, asyncRetrieveList2, messaging2
	CleanupStack::PopAndDestroy(3); // activeRetriever, asyncRetrieveList, messaging
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-SMSP-CRMPSL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneSmspList::Start with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneSmspList::Start and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySMSPStoreFU::TestStart0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

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
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of CRetrieveMobilePhoneSmspList::Start
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileSmsMessagingGetSmspListPhase1);

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeRetriever->iStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // activeRetriever, asyncRetrieveList, messaging
	CleanupStack::PopAndDestroy(this);

	}
