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
// The TEFUnit test suite for APNControlList in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyapncontrollistfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyAPNControlListFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNname0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNname0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNname0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNname0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNname0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestEnumerateAPNEntries0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestEnumerateAPNEntries0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestEnumerateAPNEntries0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestEnumerateAPNEntries0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestAppendAPNName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestAppendAPNName0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestAppendAPNName0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestAppendAPNName0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestDeleteAPNName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestDeleteAPNName0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestDeleteAPNName0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestDeleteAPNName0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestNotifyAPNListChanged0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestNotifyAPNListChanged0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestNotifyAPNListChanged0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNControlListServiceStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNControlListServiceStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNControlListServiceStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestGetAPNControlListServiceStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestSetAPNControlListServiceStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestSetAPNControlListServiceStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestSetAPNControlListServiceStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestSetAPNControlListServiceStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestSetAPNControlListServiceStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestNotifyAPNControlListServiceStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestNotifyAPNControlListServiceStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAPNControlListFU, TestNotifyAPNControlListServiceStatusChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-ACL-MGAN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAPNname
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAPNname
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNname0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	// data for RMobilePhone::GetAPNname itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	RMobilePhone::TAPNEntryV3 apnEntry;
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);	
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData(index);
	ltsyData.SerialiseL(dataExpect);
	
	// data for CompleteL
	RMobilePhone::TAPNEntryV3 apnEntryComplete;
	_LIT8(KSameName, "SameName");
	apnEntryComplete.iApn.Copy(KSameName);
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData2(apnEntryComplete);
	ltsyData2.SerialiseL(dataComplete);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetAPNname, dataExpect, KErrNotSupported);
	
	iPhone.GetAPNname(reqStatus, index, pckgEntry);
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetAPNname, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNname, KErrGeneral, dataComplete);
	
	iPhone.GetAPNname(reqStatus, index, pckgEntry);
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetAPNname when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetAPNname, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNname, KErrNone, dataComplete);
	
	iPhone.GetAPNname(reqStatus, index, pckgEntry);
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(apnEntryComplete.iApn, apnEntry.iApn);	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetAPNname
	// from LTSY.
 	//-------------------------------------------------------------------------	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EMobilePhoneGetAPNname, KErrNone, dataComplete);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // dataComplete, expectData, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGAN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetAPNname
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetAPNname
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNname0002L()
	{

// This test should test cancellation of GetAPNname
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	// data for RMobilePhone::GetAPNname itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	RMobilePhone::TAPNEntryV3 apnEntry;
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);	
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData(index);
	ltsyData.SerialiseL(dataExpect);
	
	// data for CompleteL
	RMobilePhone::TAPNEntryV3 apnEntryComplete;
	_LIT8(KSameName, "SameName");
	apnEntryComplete.iApn.Copy(KSameName);
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData2(apnEntryComplete);
	ltsyData2.SerialiseL(dataComplete);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetAPNname
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(EMobilePhoneGetAPNname, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNname, KErrNone, dataComplete, 20);
	
	iPhone.GetAPNname(reqStatus, index, pckgEntry);
	//cancel reguest
	iPhone.CancelAsyncRequest(EMobilePhoneGetAPNname);
	
	User::WaitForRequest(reqStatus);
	
	// This defect was fixed. For more information see defect description    
	//ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 90101);
  	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // dataComplete, dataExpect, this	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGAN-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAPNname with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAPNname with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNname0003L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    TRequestStatus reqStatus;
    TUint32 index = 0;

    //-------------------------------------------------------------------------
    // Test A: Test passing wrong version of parameters to
    // RMobilePhone::GetAPNname
    //-------------------------------------------------------------------------

    // Bad argument
    TInt apnEntry;
    TPckg<TInt> pckgEntry(apnEntry);

    // Call GetAPNname with bad argument
    iPhone.GetAPNname(reqStatus, index, pckgEntry);
    User::WaitForRequest(reqStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, reqStatus.Int());

    CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGAN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetAPNname
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetAPNname
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNname0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	// data for RMobilePhone::GetAPNname itself	
	TRequestStatus reqStatus;	
	TRequestStatus reqStatus2;
	TUint32 index = 0;
	RMobilePhone::TAPNEntryV3 apnEntry;
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);	
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData(index);
	ltsyData.SerialiseL(dataExpect);
	
	// data for CompleteL
	RMobilePhone::TAPNEntryV3 apnEntryComplete;
	_LIT8(KSameName, "SameName");
	apnEntryComplete.iApn.Copy(KSameName);
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData2(apnEntryComplete);
	ltsyData2.SerialiseL(dataComplete);
	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetAPNname
 	//------------------------------------------------------------------------- 	
 	// first client 	
 	iMockLTSY.ExpectL(EMobilePhoneGetAPNname, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNname, KErrNone, dataComplete);	
	iPhone.GetAPNname(reqStatus, index, pckgEntry);
	
	// second client	
	phone2.GetAPNname(reqStatus2, index, pckgEntry);
	
	// wait for the first client	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(apnEntryComplete.iApn, apnEntry.iApn);
	
	// wait for the second client	
	User::WaitForRequest(reqStatus2);   
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, dataComplete, dataExpect, this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGAN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAPNname with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAPNname and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNname0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
		
	// data for RMobilePhone::GetAPNname itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	RMobilePhone::TAPNEntryV3 apnEntry;
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);	
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData(index);
	ltsyData.SerialiseL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetAPNname
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetAPNname, data);
	
	iPhone.GetAPNname(reqStatus, index, pckgEntry);
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MEAE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::EnumerateAPNEntries
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::EnumerateAPNEntries
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestEnumerateAPNEntries0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
	
	// data for EnumerateAPNEntries itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	// data for CompleteL
	TUint32 indexComplete = 5;
	TMockLtsyData1<TUint32> ltsyData(indexComplete);
	ltsyData.SerialiseL(dataComplete);
		
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneEnumerateAPNEntries, KErrNotSupported);
		
	iPhone.EnumerateAPNEntries( reqStatus, index );
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneEnumerateAPNEntries);
	iMockLTSY.CompleteL(EMobilePhoneEnumerateAPNEntries, KErrGeneral, dataComplete);
		
	iPhone.EnumerateAPNEntries( reqStatus, index );
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::EnumerateAPNEntries when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneEnumerateAPNEntries);
	iMockLTSY.CompleteL(EMobilePhoneEnumerateAPNEntries, KErrNone, dataComplete);
		
	iPhone.EnumerateAPNEntries( reqStatus, index );
	User::WaitForRequest(reqStatus);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(indexComplete, index);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::EnumerateAPNEntries
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EMobilePhoneEnumerateAPNEntries, KErrNone, dataComplete);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // dataComplete, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MEAE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::EnumerateAPNEntries
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::EnumerateAPNEntries
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestEnumerateAPNEntries0002L()
	{

// This test should test cancellation of EnumerateAPNEntries
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);
	
	// data for EnumerateAPNEntries itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	// data for CompleteL
	TUint32 indexComplete = 5;
	TMockLtsyData1<TUint32> ltsyData(indexComplete);
	ltsyData.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::EnumerateAPNEntries
 	//-------------------------------------------------------------------------
 	iMockLTSY.ExpectL(EMobilePhoneEnumerateAPNEntries);
	iMockLTSY.CompleteL(EMobilePhoneEnumerateAPNEntries, KErrNone, data, 10);
		
	iPhone.EnumerateAPNEntries( reqStatus, index );	
	//cancel reguest
	iPhone.CancelAsyncRequest(EMobilePhoneEnumerateAPNEntries);
	
	User::WaitForRequest(reqStatus);
	// This defect was fixed. For more information see defect description  
	//ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 90201);
  	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MEAE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::EnumerateAPNEntries
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::EnumerateAPNEntries
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestEnumerateAPNEntries0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	// data for EnumerateAPNEntrieses itself
	TRequestStatus reqStatus;
	TUint32 index = 0;	
	TRequestStatus reqStatus2;
	TUint32 index2 = 0;
	
	// data for CompleteL
	TUint32 indexComplete = 5;
	TMockLtsyData1<TUint32> ltsyData(indexComplete);
	ltsyData.SerialiseL(data);
	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::EnumerateAPNEntries
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneEnumerateAPNEntries);
	iMockLTSY.CompleteL(EMobilePhoneEnumerateAPNEntries, KErrNone, data, 5);
	
	//send first request
	iPhone.EnumerateAPNEntries( reqStatus, index );
	
	//send second request
	phone2.EnumerateAPNEntries( reqStatus2, index2 );	
	
	// wait for the first client	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(indexComplete, index);
	
	// wait for the second client	
	User::WaitForRequest(reqStatus2);
   	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MEAE-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::EnumerateAPNEntries with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::EnumerateAPNEntries and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestEnumerateAPNEntries0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	// data for EnumerateAPNEntries itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::EnumerateAPNEntries
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EMobilePhoneEnumerateAPNEntries);
	iPhone.EnumerateAPNEntries( reqStatus, index );
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(1, this); // this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MAAN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AppendAPNName
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AppendAPNName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestAppendAPNName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

	// data for AppendAPNName itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNEntryV3 apnEntry;
	_LIT8(KSomeName, "SomeName");
	apnEntry.iApn.Copy(KSomeName);
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData( apnEntry );
	ltsyData.SerialiseL(dataExpect);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneAppendAPNName, dataExpect, KErrNotSupported);
			
	iPhone.AppendAPNName(reqStatus, pckgEntry);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneAppendAPNName, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneAppendAPNName, KErrGeneral);
		
	iPhone.AppendAPNName(reqStatus, pckgEntry);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::AppendAPNName when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneAppendAPNName, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneAppendAPNName, KErrNone);
		
	iPhone.AppendAPNName(reqStatus, pckgEntry);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::AppendAPNName
	// from LTSY.
 	//-------------------------------------------------------------------------	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EMobilePhoneAppendAPNName, KErrNone);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this	 
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MAAN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::AppendAPNName
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::AppendAPNName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestAppendAPNName0002L()
	{

	// This test should test cancellation of AppendAPNName
	// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);
	
	// data for AppendAPNName itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNEntryV3 apnEntry;
	_LIT8(KSomeName, "SomeName");
	apnEntry.iApn.Copy(KSomeName);
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData( apnEntry );
	ltsyData.SerialiseL(data);

	//-------------------------------------------------------------------------
	// For coverage increasing call Cancel without calling RMobilePhone::AppendAPNName
 	//------------------------------------------------------------------------- 	 	
 	iPhone.CancelAsyncRequest(EMobilePhoneAppendAPNName);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::AppendAPNName
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(EMobilePhoneAppendAPNName, data);
	iMockLTSY.CompleteL(EMobilePhoneAppendAPNName, KErrNone, 10);
		
	iPhone.AppendAPNName(reqStatus, pckgEntry);
	//cancel reguest
	iPhone.CancelAsyncRequest(EMobilePhoneAppendAPNName);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MAAN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::AppendAPNName
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::AppendAPNName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestAppendAPNName0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	// data for first AppendAPNName()
	TRequestStatus reqStatus;
	RMobilePhone::TAPNEntryV3 apnEntry;
	_LIT8(KSomeName, "SomeName");
	apnEntry.iApn.Copy(KSomeName);
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);
	
	// data for second AppendAPNName()
	TRequestStatus reqStatus2;
	RMobilePhone::TAPNEntryV3 apnEntry2;
	_LIT8(KSomeName2, "SomeName2");
	apnEntry2.iApn.Copy(KSomeName2);
	RMobilePhone::TAPNEntryV3Pckg pckgEntry2(apnEntry2);
	
	// data for first ExpectL
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData( apnEntry );
	ltsyData.SerialiseL(data);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::AppendAPNName
 	//-------------------------------------------------------------------------
 	 	
 	// first request
 	iMockLTSY.ExpectL(EMobilePhoneAppendAPNName, data);
	iMockLTSY.CompleteL(EMobilePhoneAppendAPNName, KErrNone, 10);		
	iPhone.AppendAPNName(reqStatus, pckgEntry);
	
	// second request	
	phone2.AppendAPNName(reqStatus2, pckgEntry2);
	
	// wait for the first answer
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	// wait for the second answer
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());	
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MAAN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::AppendAPNName with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AppendAPNName and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestAppendAPNName0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	// data for AppendAPNName itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNEntryV3 apnEntry;
	_LIT8(KSomeName, "SomeName");
	apnEntry.iApn.Copy(KSomeName);
	RMobilePhone::TAPNEntryV3Pckg pckgEntry(apnEntry);
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> ltsyData( apnEntry );
	ltsyData.SerialiseL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::AppendAPNName
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.ExpectL(EMobilePhoneAppendAPNName, data);		
	iPhone.AppendAPNName(reqStatus, pckgEntry);
	
	User::WaitForRequest(reqStatus);
	
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MDAN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::DeleteAPNName
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::DeleteAPNName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestDeleteAPNName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	// data for DeleteAPNName itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData( index );
	ltsyData.SerialiseL(dataExpect);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneDeleteAPNName, dataExpect, KErrNotSupported);	
	
	iPhone.DeleteAPNName(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneDeleteAPNName, dataExpect);	
	iMockLTSY.CompleteL(EMobilePhoneDeleteAPNName, KErrGeneral);	
	
	iPhone.DeleteAPNName(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::DeleteAPNName when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneDeleteAPNName, dataExpect);	
	iMockLTSY.CompleteL(EMobilePhoneDeleteAPNName, KErrNone);	
	
	iPhone.DeleteAPNName(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::DeleteAPNName
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(EMobilePhoneDeleteAPNName, KErrNone);	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MDAN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::DeleteAPNName
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::DeleteAPNName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestDeleteAPNName0002L()
	{

// This test should test cancellation of DeleteAPNName
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	// data for DeleteAPNName itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData( index );
	ltsyData.SerialiseL(dataExpect);
	
	//-------------------------------------------------------------------------
	// For coverage increasing call Cancel without calling RMobilePhone::DeleteAPNName
 	//------------------------------------------------------------------------- 	 	
 	iPhone.CancelAsyncRequest(EMobilePhoneDeleteAPNName);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::DeleteAPNName
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(EMobilePhoneDeleteAPNName, dataExpect);	
	iMockLTSY.CompleteL(EMobilePhoneDeleteAPNName, KErrNone, 10);	
	
	iPhone.DeleteAPNName(reqStatus, index);
	
	//cancel reguest
	iPhone.CancelAsyncRequest(EMobilePhoneDeleteAPNName);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // dataExpect, this
	
	}

/**
@SYMTestCaseID BA-CTSY-ACL-MDAN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::DeleteAPNName
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::DeleteAPNName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestDeleteAPNName0004L()
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
	
	// data for the first DeleteAPNName itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	// data for the second DeleteAPNName itself
	TRequestStatus reqStatus2;
	TUint32 index2 = 1;
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData( index );
	ltsyData.SerialiseL(data);		

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::DeleteAPNName
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneDeleteAPNName, data);	
	iMockLTSY.CompleteL(EMobilePhoneDeleteAPNName, KErrNone);	
	
	// first request
	iPhone.DeleteAPNName(reqStatus, index);	
	// second request
	phone2.DeleteAPNName(reqStatus2, index2);
		
	// wait for the first answer
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());	
	
	// wait for the second answer
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());	
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MDAN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::DeleteAPNName with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::DeleteAPNName and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestDeleteAPNName0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	// data for DeleteAPNName itself
	TRequestStatus reqStatus;
	TUint32 index = 0;
	
	// data for ExpectL
	TMockLtsyData1<TUint32> ltsyData( index );
	ltsyData.SerialiseL(dataExpect);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::DeleteAPNName
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneDeleteAPNName, dataExpect);		
	
	iPhone.DeleteAPNName(reqStatus, index);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MNALC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyAPNListChanged
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyAPNListChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestNotifyAPNListChanged0001L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus reqStatus;
		
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyAPNListChanged
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNListChanged, KErrNone);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iPhone.NotifyAPNListChanged(reqStatus);		
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNListChanged, KErrGeneral);	
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyAPNListChanged when result is not cached.
 	//-------------------------------------------------------------------------
	iPhone.NotifyAPNListChanged(reqStatus);		
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNListChanged, KErrNone);
		
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(1, this); // this	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MNALC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyAPNListChanged
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyAPNListChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestNotifyAPNListChanged0002L()
	{

// This test should test cancellation of NotifyAPNListChanged
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	TRequestStatus reqStatus;
	
	//-------------------------------------------------------------------------
	// For coverage increasing call Cancel without calling RMobilePhone::NotifyAPNListChanged
 	//------------------------------------------------------------------------- 	 	
 	// It is useless call as there is a checking of active or waiting request in 
 	//CTelObject::GeneralReq case EIsaCancelMessage:
 	//iPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged); 	

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyAPNListChanged
 	//------------------------------------------------------------------------- 	
 	iPhone.NotifyAPNListChanged(reqStatus);		
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNListChanged, KErrNone, 10);
	
	// cancel request
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged);
			
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int()); 	
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // this	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MNALC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyAPNListChanged
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyAPNListChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestNotifyAPNListChanged0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyAPNListChanged
 	//-------------------------------------------------------------------------
 	TRequestStatus reqStatus;
 	TRequestStatus reqStatus2;
 	
 	//send first request
 	iPhone.NotifyAPNListChanged(reqStatus);	
 	
 	//send second request
 	phone2.NotifyAPNListChanged(reqStatus2);
 	
 	// send completion
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNListChanged, KErrNone);
	
	// wait for the first notification
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	
	// wait for the second notification
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int()); 
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGACLSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAPNControlListServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNControlListServiceStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
	
	// data for GetAPNControlListServiceStatus()
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	
	// data for CompleteL
	RMobilePhone::TAPNControlListServiceStatus apnStatusComplete = RMobilePhone::EAPNControlListServiceEnabled; 
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnStatusComplete);
	ltsyData.SerialiseL(dataComplete);	

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetAPNControlListServiceStatus, KErrNotSupported);
		
	iPhone.GetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int()); 
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetAPNControlListServiceStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNControlListServiceStatus, KErrGeneral, dataComplete);
		
	iPhone.GetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int()); 	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetAPNControlListServiceStatus when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetAPNControlListServiceStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNControlListServiceStatus, KErrNone, dataComplete);
		
	iPhone.GetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	ASSERT_EQUALS(apnStatusComplete, apnControlListServiceStatus);	
	AssertMockLtsyStatusL();

  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetAPNControlListServiceStatus
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EMobilePhoneGetAPNControlListServiceStatus, KErrNone, dataComplete);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // dataComplete, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGACLSS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetAPNControlListServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNControlListServiceStatus0002L()
	{

// This test should test cancellation of GetAPNControlListServiceStatus
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
	
	// data for GetAPNControlListServiceStatus()
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	
	// data for CompleteL
	RMobilePhone::TAPNControlListServiceStatus apnStatusComplete = RMobilePhone::EAPNControlListServiceEnabled; 
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnStatusComplete);
	ltsyData.SerialiseL(dataComplete);	

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetAPNControlListServiceStatus
 	//-------------------------------------------------------------------------
 	iMockLTSY.ExpectL(EMobilePhoneGetAPNControlListServiceStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNControlListServiceStatus, KErrNone, dataComplete, 10);
		
	iPhone.GetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	//cancel reguest
	iPhone.CancelAsyncRequest(EMobilePhoneGetAPNControlListServiceStatus);		
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());  	

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // dataComplete, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGACLSS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetAPNControlListServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNControlListServiceStatus0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	// data for the first  GetAPNControlListServiceStatus()
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	
	// data for the second  GetAPNControlListServiceStatus()
	TRequestStatus reqStatus2;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus2;
	
	// data for CompleteL
	RMobilePhone::TAPNControlListServiceStatus apnStatusComplete = RMobilePhone::EAPNControlListServiceEnabled; 
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnStatusComplete);
	ltsyData.SerialiseL(dataComplete);	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetAPNControlListServiceStatus
 	//-------------------------------------------------------------------------
	
	// send first request
	iMockLTSY.ExpectL(EMobilePhoneGetAPNControlListServiceStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetAPNControlListServiceStatus, KErrNone, dataComplete);		
	iPhone.GetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);	
	
	// send second request		
	phone2.GetAPNControlListServiceStatus(reqStatus2, apnControlListServiceStatus2);
	
	// wait for the first answer	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	ASSERT_EQUALS(apnStatusComplete, apnControlListServiceStatus);	
	
	// wait for the second answer	
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int()); 
		
	AssertMockLtsyStatusL();	
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, dataComplete, this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MGACLSS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAPNControlListServiceStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAPNControlListServiceStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestGetAPNControlListServiceStatus0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	// data for GetAPNControlListServiceStatus()
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetAPNControlListServiceStatus
 	//-------------------------------------------------------------------------
 	iMockLTSY.ExpectL(EMobilePhoneGetAPNControlListServiceStatus);
	
	iPhone.GetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);	
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());  
		
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(1, this); // this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MSACLSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetAPNControlListServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestSetAPNControlListServiceStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	// data for SetAPNControlListServiceStatus() itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	apnControlListServiceStatus = RMobilePhone::EAPNControlListServiceEnabled;
	
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnControlListServiceStatus);
	ltsyData.SerialiseL(dataExpect);	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect, KErrNotSupported);
	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int()); 
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneSetAPNControlListServiceStatus, KErrGeneral);
	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetAPNControlListServiceStatus when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneSetAPNControlListServiceStatus, KErrNone);
	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	AssertMockLtsyStatusL();
		
  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetAPNControlListServiceStatus
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EMobilePhoneSetAPNControlListServiceStatus, KErrNone);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MSACLSS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetAPNControlListServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestSetAPNControlListServiceStatus0002L()
	{

// This test should test cancellation of SetAPNControlListServiceStatus
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	// data for SetAPNControlListServiceStatus() itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	apnControlListServiceStatus = RMobilePhone::EAPNControlListServiceEnabled;
	
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnControlListServiceStatus);
	ltsyData.SerialiseL(dataExpect);
	
	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetAPNControlListServiceStatus
 	//-------------------------------------------------------------------------
 	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneSetAPNControlListServiceStatus, KErrNone, 10);
	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	//cancel reguest
	iPhone.CancelAsyncRequest(EMobilePhoneSetAPNControlListServiceStatus);	
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int()); 
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2); // dataExpect, this	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MSACLSS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetAPNControlListServiceStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetAPNControlListServiceStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestSetAPNControlListServiceStatus0003L()
	{

// This test should test sending bad parameter data for SetAPNControlListServiceStatus
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobilePhone::SetAPNControlListServiceStatus
 	//------------------------------------------------------------------------- 	
 	// data for SetAPNControlListServiceStatus() itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;	
	// wrong value
	apnControlListServiceStatus = (RMobilePhone::TAPNControlListServiceStatus) -1;
	
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnControlListServiceStatus);
	ltsyData.SerialiseL(dataExpect);
	
	
	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneSetAPNControlListServiceStatus, KErrNone);
	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	
	User::WaitForRequest(reqStatus);
	// there is no checking of passing parameters in CTSY,
	// but CTSY doesn't use passing parameters, it just sent it to LTSY
	// so, I think, it isn't defect	
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	AssertMockLtsyStatusL();	

	// Done !
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MSACLSS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetAPNControlListServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestSetAPNControlListServiceStatus0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);	

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	// data for the first SetAPNControlListServiceStatus() 
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	apnControlListServiceStatus = RMobilePhone::EAPNControlListServiceEnabled;
	
	// data for the second SetAPNControlListServiceStatus() 
	TRequestStatus reqStatus2;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus2;
	apnControlListServiceStatus2 = RMobilePhone::EAPNControlListServiceDisabled;
	
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnControlListServiceStatus);
	ltsyData.SerialiseL(dataExpect);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetAPNControlListServiceStatus
 	//-------------------------------------------------------------------------
 	
 	// send first request
 	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect);
	iMockLTSY.CompleteL(EMobilePhoneSetAPNControlListServiceStatus, KErrNone);	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	
	
	// send second request
	phone2.SetAPNControlListServiceStatus(reqStatus2, apnControlListServiceStatus2);	
	
	//wait for the first answer
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	
	//wait for the second answer
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int()); 
		
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, dataExpect, this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MSACLSS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetAPNControlListServiceStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetAPNControlListServiceStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestSetAPNControlListServiceStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
	// data for SetAPNControlListServiceStatus() itself
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	apnControlListServiceStatus = RMobilePhone::EAPNControlListServiceEnabled;
	
	// data for ExpectL
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnControlListServiceStatus);
	ltsyData.SerialiseL(dataExpect);	

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetAPNControlListServiceStatus
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneSetAPNControlListServiceStatus, dataExpect);
	
	iPhone.SetAPNControlListServiceStatus(reqStatus, apnControlListServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int()); 
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this

	}


/**
@SYMTestCaseID BA-CTSY-ACL-MNACLSSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyAPNControlListServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyAPNControlListServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestNotifyAPNControlListServiceStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
	
	// data for CompleteL
	RMobilePhone::TAPNControlListServiceStatus apnStatusComplete = RMobilePhone::EAPNControlListServiceEnabled;
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnStatusComplete);
	ltsyData.SerialiseL(dataComplete);	
	
	// data for NotifyAPNControlListServiceStatusChange
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;	
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyAPNControlListServiceStatusChange
	// from LTSY. 
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNControlListServiceStatusChange, KErrNone, dataComplete);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();		
	
 	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iPhone.NotifyAPNControlListServiceStatusChange (reqStatus, apnControlListServiceStatus);
	
	// send completion 
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNControlListServiceStatusChange, KErrGeneral, dataComplete);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int()); 
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyAPNControlListServiceStatusChange when result is not cached.
 	//-------------------------------------------------------------------------
	iPhone.NotifyAPNControlListServiceStatusChange (reqStatus, apnControlListServiceStatus);
	
	// send completion 
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNControlListServiceStatusChange, KErrNone, dataComplete);
		
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	ASSERT_EQUALS(apnStatusComplete, apnControlListServiceStatus); 		

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // dataComplete, this		
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MNACLSSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyAPNControlListServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyAPNControlListServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestNotifyAPNControlListServiceStatusChange0002L()
	{

// This test should test cancellation of NotifyAPNControlListServiceStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
	
	// data for CompleteL
	RMobilePhone::TAPNControlListServiceStatus apnStatusComplete = RMobilePhone::EAPNControlListServiceEnabled;
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnStatusComplete);
	ltsyData.SerialiseL(dataComplete);	
	
	// data for NotifyAPNControlListServiceStatusChange
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;	

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyAPNControlListServiceStatusChange
 	//-------------------------------------------------------------------------
 	iPhone.NotifyAPNControlListServiceStatusChange (reqStatus, apnControlListServiceStatus);	
	// send completion 
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNControlListServiceStatusChange, KErrNone, dataComplete, 10);
		
	// cancel request
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange);
			
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int()); 				
 	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // dataComplete, this	
	}


/**
@SYMTestCaseID BA-CTSY-ACL-MNACLSSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyAPNControlListServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyAPNControlListServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAPNControlListFU::TestNotifyAPNControlListServiceStatusChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);		

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	
	// data for CompleteL
	RMobilePhone::TAPNControlListServiceStatus apnStatusComplete = RMobilePhone::EAPNControlListServiceEnabled;
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> ltsyData(apnStatusComplete);
	ltsyData.SerialiseL(dataComplete);	
	
	// data for the first NotifyAPNControlListServiceStatusChange
	TRequestStatus reqStatus;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;	
	
	// data for the second NotifyAPNControlListServiceStatusChange
	TRequestStatus reqStatus2;
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyAPNControlListServiceStatusChange
 	//-------------------------------------------------------------------------
 	
 	// send first reques
 	iPhone.NotifyAPNControlListServiceStatusChange (reqStatus, apnControlListServiceStatus);
 	
 	// send second reques
 	phone2.NotifyAPNControlListServiceStatusChange (reqStatus2, apnControlListServiceStatus2);
	
	// send completion 
	iMockLTSY.CompleteL(EMobilePhoneNotifyAPNControlListServiceStatusChange, KErrNone, dataComplete);
		
	// wait for the first answer	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	ASSERT_EQUALS(apnStatusComplete, apnControlListServiceStatus); 	
	
	// wait for the second answer	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	ASSERT_EQUALS(apnStatusComplete, apnControlListServiceStatus); 
		

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, dataComplete, this

	}

