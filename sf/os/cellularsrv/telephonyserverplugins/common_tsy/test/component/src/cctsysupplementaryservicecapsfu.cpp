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
// The TEFUnit test suite for SupplementaryServiceCaps in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysupplementaryservicecapsfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsySupplementaryServiceCapsFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServiceCapsFU, TestGetCallServiceCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServiceCapsFU, TestNotifyCallServiceCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServiceCapsFU, TestNotifyCallServiceCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServiceCapsFU, TestNotifyCallServiceCapsChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SUSC-MGCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCallServiceCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCallServiceCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySupplementaryServiceCapsFU::TestGetCallServiceCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCallServiceCaps.
 	//-------------------------------------------------------------------------

	TUint32 caps;
	TInt res = iPhone.GetCallServiceCaps(caps); 
	ASSERT_EQUALS(KErrNone, res);
	
	// only return value at this moment is KMultimodeTsyGsmCallServiceCaps
	ASSERT_EQUALS(KMultimodeTsyGsmCallServiceCaps, caps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUSC-MCSCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCallServiceCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallServiceCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySupplementaryServiceCapsFU::TestNotifyCallServiceCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint32 completeCaps(100);
	TPckg<TUint32> pckgCaps(completeCaps);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCallServiceCapsChange when result is not cached.
 	//-------------------------------------------------------------------------
	
	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyCallServiceCapsChange(reqStatus, caps);
	
	iMockLTSY.CompleteL(EMobilePhoneNotifyCallServiceCapsChange, KErrNone, pckgCaps);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	ASSERT_EQUALS( completeCaps, caps );
	
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUSC-MCSCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCallServiceCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCallServiceCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySupplementaryServiceCapsFU::TestNotifyCallServiceCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCallServiceCapsChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyCallServiceCapsChange(reqStatus, caps);
	
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallServiceCapsChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUSC-MCSCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCallServiceCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCallServiceCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySupplementaryServiceCapsFU::TestNotifyCallServiceCapsChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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

	TUint32 completeCaps(100);
	TPckg<TUint32> pckgCaps(completeCaps);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCallServiceCapsChange
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyCallServiceCapsChange(reqStatus, caps);
	
	TRequestStatus reqStatus2;
	TUint32 caps2(0);
	phone2.NotifyCallServiceCapsChange(reqStatus2, caps2);
	
	iMockLTSY.CompleteL(EMobilePhoneNotifyCallServiceCapsChange, KErrNone, pckgCaps);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	AssertMockLtsyStatusL();

	ASSERT_EQUALS( completeCaps, caps );
	ASSERT_EQUALS( completeCaps, caps2 );	
	
	// Done !
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}
