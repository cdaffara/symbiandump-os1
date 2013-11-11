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
// The TEFUnit test suite for MMS in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsymmsfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyMMSFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyMMSFU, TestGetMmsConfig0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMMSFU, TestNotifyMmsConfig0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMMSFU, TestNotifyMmsUpdate0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMMSFU, TestSetMmsUserConnParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMMSFU, TestSetMmsUserPreferences0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-MMS-MGMC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMmsConfig
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMmsConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMMSFU::TestGetMmsConfig0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Test API is not supported
	TRequestStatus reqStatus;
	RMobilePhone::TMmsConnParams type = RMobilePhone::EUserPreferences;
	iPhone.GetMmsConfig(reqStatus, type, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MMS-MNMCPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMmsConfig
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMmsConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMMSFU::TestNotifyMmsConfig0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	// Test API is not supported
	TRequestStatus reqStatus;
	RMobilePhone::TMmsConnParams type = RMobilePhone::EUserPreferences;
	iPhone.NotifyMmsConfig(reqStatus, type, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-MMS-MNMU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMmsUpdate
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMmsUpdate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMMSFU::TestNotifyMmsUpdate0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Test API is not supported
	TRequestStatus reqStatus;
	iPhone.NotifyMmsUpdate(reqStatus, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MMS-MSMUCP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetMmsUserConnParams
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetMmsUserConnParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMMSFU::TestSetMmsUserConnParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Test API is not supported
	TRequestStatus reqStatus;
	iPhone.SetMmsUserConnParams(reqStatus, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-MMS-MSMUP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetMmsUserPreferences
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetMmsUserPreferences
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMMSFU::TestSetMmsUserPreferences0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Test API is not supported
	TRequestStatus reqStatus;
	iPhone.SetMmsUserPreferences(reqStatus, data);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



