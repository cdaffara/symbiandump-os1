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
// The TEFUnit test suite for AlternateLineService in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyalternatelineservicefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <centralrepository.h>

const TUid KCRUidCtsyMEAlsLine = { 0x102029A2 };
const TUint KCtsyMEAlsSupported = { 0x00000002 };

CTestSuite* CCTsyAlternateLineServiceFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestGetALSLine0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestGetALSLine0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestSetALSLine0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestSetALSLine0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestSetALSLine0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestNotifyALSLineChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestNotifyALSLineChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternateLineServiceFU, TestNotifyALSLineChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-ALLS-MGAL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetALSLine
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetALSLine
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestGetALSLine0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TInt ret(KErrGeneral);
	
	TRequestStatus requestStatus;
	
	// dont change the tests order in order to preserve the environment state
	// at the beginig of every test
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetALSLine
	// from LTSY.
 	//-------------------------------------------------------------------------

	// create centRep to change repository 
	CRepository* centRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
	CleanupStack::PushL(centRep);

	// setting environment (set KCtsyMEAlsSupported in repository to EFalse)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	// Unsolicited completion with new value EAlternateLineAuxiliary that is not yet cached in CTSY
	iMockLTSY.NotifyTerminated(requestStatus);	    
	RMobilePhone::TMobilePhoneALSLine alsLine(RMobilePhone::EAlternateLineAuxiliary);
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    // Unsolicited completion with the same value EAlternateLineAuxiliary that is cached in CTSY
	iMockLTSY.NotifyTerminated(requestStatus);	
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetALSLine when result is not cached.
 	//-------------------------------------------------------------------------

	// test setting and execute
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	AssertMockLtsyStatusL();
		
	//-------------------------------------------------------------------------
	// TESTs for coverage increasing with different values of alsLine and 
	// of KCtsyMEAlsSupported from repository
 	//-------------------------------------------------------------------------
 	// setting environment (set alsLine to EAlternateLineUnknown)
	iMockLTSY.NotifyTerminated(requestStatus);
	alsLine = RMobilePhone::EAlternateLineUnknown;
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// test setting and execute
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	AssertMockLtsyStatusL();

	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);
    
	// test setting and execute
	alsLine = RMobilePhone::EAlternateLinePrimary;
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);

	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLinePrimary);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetALSLine again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
	
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLinePrimary);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// change alsLine to EAlternateLineUnknown again
	iMockLTSY.NotifyTerminated(requestStatus);
	alsLine = RMobilePhone::EAlternateLineUnknown;
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// test setting and execute
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrGeneral, data);

	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrGeneral, ret);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	
	//change alsLine to EAlternateLineUnknown again
	iMockLTSY.NotifyTerminated(requestStatus);	
	alsLine = RMobilePhone::EAlternateLineUnknown;
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// test setting and execute
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine, KErrNotSupported);
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNotSupported, ret);
	AssertMockLtsyStatusL();

	//Done!
	// return repository KCtsyMEAlsSupported to EFalse (initial state)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // iCentRep
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MGAL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetALSLine with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetALSLine and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestGetALSLine0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TInt ret(-1);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetALSLine
 	//-------------------------------------------------------------------------

	// create centRep to change repository 
	CRepository* centRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
	CleanupStack::PushL(centRep);
    
	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);

	// setting environment (set alsLine to EAlternateLinePrimary)
	iMockLTSY.NotifyTerminated(requestStatus);
	RMobilePhone::TMobilePhoneALSLine alsLine(RMobilePhone::EAlternateLineUnknown);
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	// test setting and execute
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine, KErrNone);
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrTimedOut, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLineNotAvailable);
	AssertMockLtsyStatusL();

	// Done !
	// return repository KCtsyMEAlsSupported to EFalse (initial state)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // iCentRep
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MSL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetALSLine
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetALSLine
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestSetALSLine0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt ret(-1);
	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// create centRep to change repository 
	CRepository* centRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
	CleanupStack::PushL(centRep);

	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);

	// test setting and execute
	RMobilePhone::TMobilePhoneALSLine alsLine(RMobilePhone::EAlternateLinePrimary);
	TMockLtsyData1< RMobilePhone::TMobilePhoneALSLine >
			alsLineData( alsLine );
	alsLineData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSetALSLine, data);
	iMockLTSY.CompleteL(EMobilePhoneSetALSLine, KErrGeneral);

	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetALSLine when result is not cached.
	// Case CtsyMEAlsSupported ETrue
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSetALSLine, data);
	iMockLTSY.CompleteL(EMobilePhoneSetALSLine, KErrNone);
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetALSLine when result is not cached.
	// Case ÑtsyMEAlsSupported EFalse
 	//-------------------------------------------------------------------------

	// setting environment (set KCtsyMEAlsSupported in repository to EFalse)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	// test setting and execute
	alsLine = RMobilePhone::EAlternateLinePrimary;
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// check test results
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLinePrimary); //check
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TESTs for coverage increasing with different values of alsLine
 	//-------------------------------------------------------------------------

	// test execute
	alsLine = RMobilePhone::EAlternateLineAuxiliary;
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// check test results
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLineAuxiliary); //check
	AssertMockLtsyStatusL();

	// test execute
	alsLine = RMobilePhone::EAlternateLineUnknown;
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// check test results
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLineUnknown); //check
	AssertMockLtsyStatusL();

	// test execute
	alsLine = RMobilePhone::EAlternateLineNotAvailable;
	iPhone.SetALSLine(requestStatus, alsLine);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// check test results
	ret = iPhone.GetALSLine(alsLine);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(alsLine, RMobilePhone::EAlternateLineNotAvailable); //check
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetALSLine
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);	
	iMockLTSY.CompleteL(EMobilePhoneSetALSLine, KErrNone);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);

	// test setting and execute
	data.Close();
	alsLineData.SerialiseL(data); // serialise data here because alsLine variable is changed
	iMockLTSY.ExpectL(EMobilePhoneSetALSLine, data, KErrNotSupported);
	iPhone.SetALSLine(requestStatus, alsLine);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	// Done !
	// return repository KCtsyMEAlsSupported to EFalse (initial state)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // iCentRep
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MSL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetALSLine
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetALSLine
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestSetALSLine0004L()
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

	// create centRep to change repository 
	CRepository* centRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
	CleanupStack::PushL(centRep);

	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetALSLine
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	RMobilePhone::TMobilePhoneALSLine alsLine(RMobilePhone::EAlternateLinePrimary);
	TMockLtsyData1< RMobilePhone::TMobilePhoneALSLine >
			alsLineData( alsLine );
	alsLineData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSetALSLine, data);
	iMockLTSY.CompleteL(EMobilePhoneSetALSLine, KErrNone);
	iPhone.SetALSLine(requestStatus, alsLine);
	
	// setting and execute 2nd request
	RMobilePhone::TMobilePhoneALSLine alsLine2(RMobilePhone::EAlternateLineAuxiliary);
	phone2.SetALSLine(requestStatus2, alsLine2);
	
	// wait for completion and check results
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	// setting and execute the test for the case when 
	// KCtsyMEAlsSupported in repository is set to ETrue
	alsLine = RMobilePhone::EAlternateLinePrimary;
	iPhone.SetALSLine(requestStatus, alsLine);
	
	alsLine2 = RMobilePhone::EAlternateLineAuxiliary;
	phone2.SetALSLine(requestStatus2, alsLine2);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	AssertMockLtsyStatusL();

	//Done!
	CleanupStack::PopAndDestroy(1); // iCentRep
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MSL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetALSLine with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetALSLine and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestSetALSLine0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// create centRep to change repository 
	CRepository* centRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
	CleanupStack::PushL(centRep);

	TInt ret(-1);
	// setting environment (set KCtsyMEAlsSupported in repository to ETrue)
	ret = centRep->Set( KCtsyMEAlsSupported, ETrue );
	ASSERT_EQUALS(KErrNone, ret);

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetALSLine
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneALSLine alsLine(RMobilePhone::EAlternateLinePrimary);
	TMockLtsyData1< RMobilePhone::TMobilePhoneALSLine >
			alsLineData( alsLine );
	alsLineData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSetALSLine, data);

	iPhone.SetALSLine(requestStatus, alsLine);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	AssertMockLtsyStatusL();

	// Done !
	// return repository KCtsyMEAlsSupported to EFalse (initial state)
	ret = centRep->Set( KCtsyMEAlsSupported, EFalse );
	ASSERT_EQUALS(KErrNone, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // iCentRep
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MNALC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyALSLineChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyALSLineChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestNotifyALSLineChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyALSLineChange when result is not cached.
 	//-------------------------------------------------------------------------

	// call notify ALSLine change
	RMobilePhone::TMobilePhoneALSLine alsLine;
	iPhone.NotifyALSLineChange(requestStatus, alsLine);

	// change ALSLine
	TRequestStatus          mockLtsyStatus;
	RMobilePhone::TMobilePhoneALSLine       newAlsLine(RMobilePhone::EAlternateLinePrimary);
	iPhone.SetALSLine(      mockLtsyStatus, newAlsLine );
	User::WaitForRequest(   mockLtsyStatus );
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait for completion notify request and check results
	User::WaitForRequest(   requestStatus );
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RMobilePhone::EAlternateLinePrimary, alsLine);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MNALC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyALSLineChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyALSLineChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestNotifyALSLineChange0002L()
	{

// This test should test cancellation of NotifyALSLineChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyALSLineChange
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobilePhoneALSLine alsLine;
	TRequestStatus requestStatus;
	iPhone.NotifyALSLineChange(requestStatus, alsLine);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyALSLineChange);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(1); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALLS-MNALC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyALSLineChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyALSLineChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternateLineServiceFU::TestNotifyALSLineChange0004L()
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
	// Test A: Test multiple clients requesting RMobilePhone::NotifyALSLineChange
 	//-------------------------------------------------------------------------

	// execute 1st request
	RMobilePhone::TMobilePhoneALSLine alsLine;
	TRequestStatus requestStatus;
	iPhone.NotifyALSLineChange(requestStatus, alsLine);

	// execute 2nd request
	RMobilePhone::TMobilePhoneALSLine alsLine2;
	TRequestStatus requestStatus2;
	phone2.NotifyALSLineChange(requestStatus2, alsLine2);

	// change alsLine
	TRequestStatus          mockLtsyStatus;
	RMobilePhone::TMobilePhoneALSLine       newAlsLine(RMobilePhone::EAlternateLinePrimary);
	iPhone.SetALSLine(      mockLtsyStatus, newAlsLine );
	User::WaitForRequest(   mockLtsyStatus );
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait for completion notify requests  and check results
	User::WaitForRequest(   requestStatus );
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RMobilePhone::EAlternateLinePrimary, alsLine);

	User::WaitForRequest(   requestStatus2 );
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(RMobilePhone::EAlternateLinePrimary, alsLine2);

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}




