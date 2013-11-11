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
// The TEFUnit test suite for IMS in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyimsfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyIMSFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyIMSFU, TestImsAuthenticate0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIMSFU, TestImsAuthenticate0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIMSFU, TestImsAuthenticate0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIMSFU, TestImsAuthenticate0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIMSFU, TestImsAuthenticate0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyIMSFU, TestNotifyImsAuthorizationInfoChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-IMS-MIA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ImsAuthenticate
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ImsAuthenticate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIMSFU::TestImsAuthenticate0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RMobilePhone::TImsAuthenticateDataV5 expAuthenticateData;
	expAuthenticateData.iAUTN.Copy(_L8("11"));
	expAuthenticateData.iRAND.Copy(_L8("22"));
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							expAuthenticateLtsyData( expAuthenticateData );
	expAuthenticateLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data, KErrNotSupported);

	TRequestStatus reqStatus;
	RMobilePhone::TImsAuthenticateDataV5 authenticateData;
	authenticateData.iAUTN.Copy(_L8("11"));
	authenticateData.iRAND.Copy(_L8("22"));
	RMobilePhone::TImsAuthenticateDataV5Pckg authenticateDataPckg(authenticateData);
	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	RMobilePhone::TImsAuthenticateDataV5 completeAuthenticateData;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							completeAuthenticateLtsyData(completeAuthenticateData);
	completeAuthenticateData.iAUTS.Copy(_L8("88"));
    data.Close();
    completeAuthenticateLtsyData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrGeneral, data);

	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(completeAuthenticateData.iAUTS, authenticateData.iAUTS);
	
	//-------------------------------------------------------------------------
	// TEST: increase coverage
 	//-------------------------------------------------------------------------
	
	data.Close();
	expAuthenticateLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	// pack special constant _L8("17796")
	// for recognize null pointer packing test case in CMockPhoneMessHandler::Complete
	completeAuthenticateData.iAUTS.Copy(_L8("17796"));
    data.Close();
    completeAuthenticateLtsyData.SerialiseL(data); 
	completeAuthenticateData.iAUTS.Copy(_L8("88"));
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrGeneral, data);

	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::ImsAuthenticate when result is not cached.
 	//-------------------------------------------------------------------------

	data.Close();
	expAuthenticateLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	completeAuthenticateData.iRES.Copy(_L8("66"));
	completeAuthenticateData.iIK.Copy(_L8("77"));
	completeAuthenticateData.iCK.Copy(_L8("88"));
    data.Close();
    completeAuthenticateLtsyData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrNone, data);

	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	// check only fields which are used for returned data
	ASSERT_EQUALS(completeAuthenticateData.iRES, authenticateData.iRES);
	ASSERT_EQUALS(completeAuthenticateData.iIK,  authenticateData.iIK);
	ASSERT_EQUALS(completeAuthenticateData.iCK,  authenticateData.iCK);
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::ImsAuthenticate
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(reqStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrNone, data);
	// wait for completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	iMockLTSY.NotifyTerminated(reqStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrGeneral, data);
	// wait for completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-IMS-MIA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::ImsAuthenticate
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::ImsAuthenticate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIMSFU::TestImsAuthenticate0002L()
	{

// This test should test cancellation of ImsAuthenticate
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::ImsAuthenticate
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TImsAuthenticateDataV5 expAuthenticateData;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							expAuthenticateLtsyData( expAuthenticateData );
	expAuthenticateLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	RMobilePhone::TImsAuthenticateDataV5 completeAuthenticateData;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							completeAuthenticateLtsyData(completeAuthenticateData);
    data.Close();
    completeAuthenticateLtsyData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrNone, data, 10);

	TRequestStatus reqStatus;
	RMobilePhone::TImsAuthenticateDataV5 authenticateData;
	RMobilePhone::TImsAuthenticateDataV5Pckg authenticateDataPckg(authenticateData);
	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	
	iPhone.CancelAsyncRequest(EMobilePhoneIMSAuthenticate);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-IMS-MIA-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ImsAuthenticate with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ImsAuthenticate with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIMSFU::TestImsAuthenticate0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::ImsAuthenticate
 	//-------------------------------------------------------------------------

	RMobilePhone::TImsAuthenticateDataV5 expAuthenticateData;
	TInt wrongAuthenticateData;
	TPckg<TInt> wrongAuthenticateDataPckg(wrongAuthenticateData);
	memcpy(&wrongAuthenticateData, &expAuthenticateData, sizeof(wrongAuthenticateData));
	
	iPhone.ImsAuthenticate(reqStatus, wrongAuthenticateDataPckg);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-IMS-MIA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::ImsAuthenticate
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::ImsAuthenticate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIMSFU::TestImsAuthenticate0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::ImsAuthenticate
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	RMobilePhone::TImsAuthenticateDataV5 expAuthenticateData;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
	                             expAuthenticateLtsyData( expAuthenticateData );
	expAuthenticateLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	RMobilePhone::TImsAuthenticateDataV5 completeAuthenticateData;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
	                       completeAuthenticateLtsyData(completeAuthenticateData);
	completeAuthenticateData.iRES.Copy(_L8("33"));
	completeAuthenticateData.iIK.Copy(_L8("44"));
	completeAuthenticateData.iCK.Copy(_L8("55"));
    data.Close();
    completeAuthenticateLtsyData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrNone, data, 10);

	TRequestStatus reqStatus;
	RMobilePhone::TImsAuthenticateDataV5 authenticateData;
	RMobilePhone::TImsAuthenticateDataV5Pckg authenticateDataPckg(authenticateData);
	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	
	// setting and execute 2nd request
	RMobilePhone::TImsAuthenticateDataV5 expAuthenticateData2;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							expAuthenticateLtsyData2( expAuthenticateData2 );
	data.Close();
	expAuthenticateLtsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	RMobilePhone::TImsAuthenticateDataV5 completeAuthenticateData2;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							completeAuthenticateLtsyData2(completeAuthenticateData2);
	completeAuthenticateData2.iRES.Copy(_L8("66"));
	completeAuthenticateData2.iIK.Copy(_L8("77"));
	completeAuthenticateData2.iCK.Copy(_L8("88"));
    data.Close();
    completeAuthenticateLtsyData2.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneIMSAuthenticate, KErrNone, data);

	TRequestStatus reqStatus2;
	RMobilePhone::TImsAuthenticateDataV5 authenticateData2;
	RMobilePhone::TImsAuthenticateDataV5Pckg authenticateDataPckg2(authenticateData2);
	phone2.ImsAuthenticate(reqStatus2, authenticateDataPckg2);
	
	// test completion
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	// check only fields which are used for returned data
	ASSERT_EQUALS(completeAuthenticateData2.iRES, authenticateData2.iRES);
	ASSERT_EQUALS(completeAuthenticateData2.iIK,  authenticateData2.iIK);
	ASSERT_EQUALS(completeAuthenticateData2.iCK,  authenticateData2.iCK);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	// check only fields which are used for returned data
	ASSERT_EQUALS(completeAuthenticateData.iRES, authenticateData.iRES);
	ASSERT_EQUALS(completeAuthenticateData.iIK,  authenticateData.iIK);
	ASSERT_EQUALS(completeAuthenticateData.iCK,  authenticateData.iCK);

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-IMS-MIA-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ImsAuthenticate with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ImsAuthenticate and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIMSFU::TestImsAuthenticate0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::ImsAuthenticate
 	//-------------------------------------------------------------------------

	RMobilePhone::TImsAuthenticateDataV5 expAuthenticateData;
	TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5>
							expAuthenticateLtsyData( expAuthenticateData );
	expAuthenticateLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneIMSAuthenticate, data);

	TRequestStatus reqStatus;
	RMobilePhone::TImsAuthenticateDataV5 authenticateData;
	RMobilePhone::TImsAuthenticateDataV5Pckg authenticateDataPckg(authenticateData);
	iPhone.ImsAuthenticate(reqStatus, authenticateDataPckg);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-IMS-MNIAIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyImsAuthorizationInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyImsAuthorizationInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyIMSFU::TestNotifyImsAuthorizationInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	iPhone.NotifyImsAuthorizationInfoChange(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



