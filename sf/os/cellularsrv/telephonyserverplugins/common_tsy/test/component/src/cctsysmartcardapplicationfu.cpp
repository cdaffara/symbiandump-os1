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
// The TEFUnit test suite for SmartCardApplication in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysmartcardapplicationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsySmartCardApplicationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestReadScFile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestUpdateScFile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestGetScFileInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestGetSmartCardApplicationInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestSetSmartCardApplicationStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestEnumerateSmartCardApplications0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySmartCardApplicationFU, TestNotifySmartCardAppInfoChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SMCA-MRSF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ReadScFile
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ReadScFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestReadScFile0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TScFilePathWithAccessOffsets filePathOffsets;
	TBuf8<1> readBuffer;
	iPhone.ReadScFile(reqStatus, filePathOffsets, readBuffer);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SMCA-MUSF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::UpdateScFile
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::UpdateScFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestUpdateScFile0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TScFilePathWithAccessOffsets filePathOffsets;
	TBuf8<1> updateBuffer;
	iPhone.UpdateScFile(reqStatus, filePathOffsets, updateBuffer);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SMCA-MGSFI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetScFileInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetScFileInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestGetScFileInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TScFilePath filePathId;
	TBuf8<1> info;
	iPhone.GetScFileInfo(reqStatus, filePathId, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SMCA-MGSCA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSmartCardApplicationInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSmartCardApplicationInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestGetSmartCardApplicationInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TUint appIndex(0);
	TBuf8<1> info;
	iPhone.GetSmartCardApplicationInfo(reqStatus, appIndex, info);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SMCA-MSSCAS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetSmartCardApplicationStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetSmartCardApplicationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestSetSmartCardApplicationStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TAID aid;
	RMobilePhone::TSmartCardApplicationAction appAction(RMobilePhone::EScApplicationActivate);
	iPhone.SetSmartCardApplicationStatus(reqStatus, aid, appAction);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SMCA-MESCA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::EnumerateSmartCardApplications
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::EnumerateSmartCardApplications
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestEnumerateSmartCardApplications0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TUint count;
	iPhone.EnumerateSmartCardApplications(reqStatus, count);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SMCA-MNSCAIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySmartCardApplicationInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySmartCardApplicationInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySmartCardApplicationFU::TestNotifySmartCardAppInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TAID aid;
	RMobilePhone::TSmartCardApplicationEvent event;
	iPhone.NotifySmartCardApplicationInfoChange(reqStatus, aid, event);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}



