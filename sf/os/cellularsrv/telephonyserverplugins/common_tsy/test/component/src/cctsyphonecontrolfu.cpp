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
// The TEFUnit test suite for PhoneControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyphonecontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "MmTsy_conf.h"

CTestSuite* CCTsyPhoneControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestNotifyModemDetected0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestGetPhoneInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestGetPhoneInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestIsSupportedByModule0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestIsSupportedByModule0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestGetCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestGetInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneControlFU, TestGetStatus0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PCON-PNMD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::NotifyModemDetected
@SYMTestPriority High
@SYMTestActions Invokes RPhone::NotifyModemDetected
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestNotifyModemDetected0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPhone::NotifyModemDetected.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RPhone::TModemDetection detection(RPhone::EDetectedPresent);
	iPhone.NotifyModemDetected(reqStatus, detection);
	
	TRequestStatus mockLtsyStatus; 
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	RPhone::TStatus status;
	status.iModemDetected = RPhone::EDetectedNotPresent;
	status.iMode = RPhone::EModeUnknown;
	TMockLtsyData1<RPhone::TStatus> completeData(status);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelPhoneNotifyModemDetected, KErrNone, data);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(detection, status.iModemDetected);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPhone::NotifyModemDetected
	// from LTSY.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EEtelPhoneNotifyModemDetected, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PCON-TGPI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::GetPhoneInfo
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::GetPhoneInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestGetPhoneInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

 	//-------------------------------------------------------------------------
	// Successful completion request of
	// RTelServer::GetPhoneInfo.
 	//-------------------------------------------------------------------------

	RTelServer::TPhoneInfo info;
	TInt res = iTelServer.GetPhoneInfo(0, info);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(KMmTsyNetworkType, info.iNetworkType);
	ASSERT_TRUE( 0 == KMmTsyPhoneName().Compare(info.iName) );
	ASSERT_EQUALS(static_cast<TUint>(KMmTsyNumOfSupportedLines), info.iNumberOfLines);
	ASSERT_EQUALS(static_cast<TUint>(KETelExtMultimodeV1), info.iExtensions);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCON-TGPI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::GetPhoneInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::GetPhoneInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestGetPhoneInfo0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RTelServer::GetPhoneInfo
 	//-------------------------------------------------------------------------
 	
	RTelServer::TPhoneInfo info;
	TInt res = iTelServer.GetPhoneInfo(1, info);
	ASSERT_EQUALS(KErrNotFound, res);

	// Done !
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-PCON-TISBM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::IsSupportedByModule
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::IsSupportedByModule
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestIsSupportedByModule0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

 	//-------------------------------------------------------------------------
	// Successful completion request of
	// RTelServer::IsSupportedByModule.
 	//-------------------------------------------------------------------------

	TBool supported(EFalse);
	TInt res = iTelServer.IsSupportedByModule(KMmTsyModuleName, KETelExtMultimodeV1, supported);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(static_cast<TBool>(ETrue), supported);

	res = iTelServer.IsSupportedByModule(KMmTsyModuleName, 0, supported);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(static_cast<TBool>(EFalse), supported);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCON-TISBM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::IsSupportedByModule with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::IsSupportedByModule with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestIsSupportedByModule0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RTelServer::IsSupportedByModule
 	//-------------------------------------------------------------------------
 	
	TBool supported(EFalse);
	TInt res = iTelServer.IsSupportedByModule(KNullDesC, 0, supported);
	ASSERT_EQUALS(KErrNotFound, res);

	// Done !
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-PCON-PGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::GetCaps
@SYMTestPriority High
@SYMTestActions Invokes RPhone::GetCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestGetCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPhone::GetCaps when result is not cached.
 	//-------------------------------------------------------------------------

	RPhone::TCaps caps;
	// function always returns KPhoneCoreCaps at the moment
	TInt res = iPhone.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(KPhoneCoreCaps, caps.iFlags);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCON-PGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::GetInfo
@SYMTestPriority High
@SYMTestActions Invokes RPhone::GetInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestGetInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Successful completion request of
	// RPhone::GetInfo.
 	//-------------------------------------------------------------------------

	RPhone::TPhoneInfo phoneInfo;
	TInt res = iPhone.GetInfo(phoneInfo);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(RPhone::EDetectedPresent, phoneInfo.iDetection);
	AssertMockLtsyStatusL();

	// invoke phone info change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RPhone::TStatus status;
	status.iModemDetected = RPhone::EDetectedNotPresent;
	status.iMode = RPhone::EModeUnknown;
	TMockLtsyData1<RPhone::TStatus> completeData(status);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelPhoneNotifyModemDetected, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	res = iPhone.GetInfo(phoneInfo);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(status.iModemDetected, phoneInfo.iDetection);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PCON-PGS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::GetStatus
@SYMTestPriority High
@SYMTestActions Invokes RPhone::GetStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneControlFU::TestGetStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Successful completion request of
	// RPhone::GetStatus.
 	//-------------------------------------------------------------------------

	RPhone::TStatus status;
	TInt res = iPhone.GetStatus(status);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(RPhone::EDetectedPresent, status.iModemDetected);
	ASSERT_EQUALS(RPhone::EModeIdle, status.iMode);
	AssertMockLtsyStatusL();

	// invoke phone info change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RPhone::TStatus completeStatus;
	completeStatus.iModemDetected = RPhone::EDetectedNotPresent;
	completeStatus.iMode = RPhone::EModeUnknown;
	TMockLtsyData1<RPhone::TStatus> completeData(completeStatus);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelPhoneNotifyModemDetected, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	res = iPhone.GetStatus(status);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(status.iModemDetected, completeStatus.iModemDetected);
	ASSERT_EQUALS(status.iMode, completeStatus.iMode);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}
