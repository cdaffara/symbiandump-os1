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
// The TEFUnit test suite for SignalStrength in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysignalstrengthfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsySignalStrengthFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySignalStrengthFU, TestGetSignalCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySignalStrengthFU, TestGetSignalStrength0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySignalStrengthFU, TestGetSignalStrength0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySignalStrengthFU, TestNotifySignalStrengthChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySignalStrengthFU, TestNotifySignalStrengthChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySignalStrengthFU, TestNotifySignalStrengthChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SIGS-MGSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSignalCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSignalCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySignalStrengthFU::TestGetSignalCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Successful completion request of
	// RMobilePhone::GetSignalCaps.
 	//-------------------------------------------------------------------------

	TUint32 caps;
	TInt res = iPhone.GetSignalCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	// allways returns KMmTsySignalCaps at the moment
	ASSERT_EQUALS(static_cast<TUint32>(KMmTsySignalCaps), caps);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SIGS-MGSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSignalStrength
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSignalStrength
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySignalStrengthFU::TestGetSignalStrength0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetSignalStrength.
 	//-------------------------------------------------------------------------

	// change signal strength
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TInt32 completeSignalStrength = 0;
	TInt8 completeBar = -1;
	TMockLtsyData2<TInt32, TInt8> completeData(completeSignalStrength, completeBar);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySignalStrengthChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	TRequestStatus reqStatus;
	TInt32 signalStrength = 0;
	TInt8 bar = 0;
	iPhone.GetSignalStrength(reqStatus, signalStrength, bar);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());
	AssertMockLtsyStatusL();

	// change signal strength
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	completeSignalStrength = 70;
	completeBar = 3;
	data.Close();
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySignalStrengthChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	iPhone.GetSignalStrength(reqStatus, signalStrength, bar);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(bar, completeBar);
	ASSERT_EQUALS(signalStrength, completeSignalStrength);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-SIGS-MGSS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetSignalStrength
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetSignalStrength
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySignalStrengthFU::TestGetSignalStrength0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// create signal strength
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TInt32 completeSignalStrength(70);
	TInt8 completeBar(3);
	TMockLtsyData2<TInt32, TInt8> completeData(completeSignalStrength, completeBar);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySignalStrengthChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

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
	// Test A: Test multiple clients requesting RMobilePhone::GetSignalStrength
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TInt32 signalStrength(0);
	TInt8 bar(0);
	iPhone.GetSignalStrength(reqStatus, signalStrength, bar);

	TRequestStatus reqStatus2;
	TInt32 signalStrength2(0);
	TInt8 bar2(0);
	phone2.GetSignalStrength(reqStatus2, signalStrength2, bar2);

	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);
	AssertMockLtsyStatusL();

	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(bar, completeBar);
	ASSERT_EQUALS(signalStrength, completeSignalStrength);

	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(bar2, completeBar);
	ASSERT_EQUALS(signalStrength2, completeSignalStrength);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-SIGS-MNSSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySignalStrengthChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySignalStrengthChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySignalStrengthFU::TestNotifySignalStrengthChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifySignalStrengthChange.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TInt32 signalStrength(0);
	TInt8 bar(0);
	iPhone.NotifySignalStrengthChange(reqStatus, signalStrength, bar);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TInt32 completeSignalStrength(70);
	TInt8 completeBar(3);
	TMockLtsyData2<TInt32, TInt8> completeData(completeSignalStrength, completeBar);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySignalStrengthChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(bar, completeBar);
	ASSERT_EQUALS(signalStrength, completeSignalStrength);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifySignalStrengthChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifySignalStrengthChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-SIGS-MNSSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifySignalStrengthChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifySignalStrengthChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySignalStrengthFU::TestNotifySignalStrengthChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifySignalStrengthChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	TInt32 signalStrength(0);
	TInt8 bar(0);
	iPhone.NotifySignalStrengthChange(reqStatus, signalStrength, bar);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SIGS-MNSSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifySignalStrengthChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifySignalStrengthChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySignalStrengthFU::TestNotifySignalStrengthChange0004L()
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
	// Test A: Test multiple clients requesting RMobilePhone::NotifySignalStrengthChange
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TInt32 signalStrength(0);
	TInt8 bar(0);
	iPhone.NotifySignalStrengthChange(reqStatus, signalStrength, bar);
	
	TRequestStatus reqStatus2;
	TInt32 signalStrength2(0);
	TInt8 bar2(0);
	phone2.NotifySignalStrengthChange(reqStatus2, signalStrength2, bar2);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TInt32 completeSignalStrength(70);
	TInt8 completeBar(3);
	TMockLtsyData2<TInt32, TInt8> completeData(completeSignalStrength, completeBar);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifySignalStrengthChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(bar, completeBar);
	ASSERT_EQUALS(signalStrength, completeSignalStrength);
	AssertMockLtsyStatusL();

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(bar2, completeBar);
	ASSERT_EQUALS(signalStrength2, completeSignalStrength);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
