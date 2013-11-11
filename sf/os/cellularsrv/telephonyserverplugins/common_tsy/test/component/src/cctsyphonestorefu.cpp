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
// The TEFUnit test suite for PhoneStore in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyphonestorefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyPhoneStoreFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneStoreFU, TestGetPhoneStoreInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneStoreFU, TestGetPhoneStoreInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneStoreFU, TestGetPhoneStoreInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneStoreFU, TestGetPhoneStoreInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneStoreFU, TestGetPhoneStoreInfo0005L);

	END_SUITE;
	}


//
// Actual test cases
//
void CCTsyPhoneStoreFU::TestUnsupportedPhoneBookL(const TDesC& aUnsupportedPhbk)
	{
	// result storage	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);

	TRequestStatus status;
	iPhone.GetPhoneStoreInfo(status, infoPckg, aUnsupportedPhbk);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNotSupported, status.Int())
	AssertMockLtsyStatusL();
	}


/**
@SYMTestCaseID BA-CTSY-PSTR-MGPSI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPhoneStoreInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneStoreInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneStoreFU::TestGetPhoneStoreInfo0001L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Test A for this API consists of two tests

	// CASE 1 - client supplies an unsupported phonebook.
	// No need for Expect / Complete, as never reaches LTSY
	// try ALL unsupported phonebooks to improve code coverage
	
	TestUnsupportedPhoneBookL(KETelMeAdnPhoneBook);
	TestUnsupportedPhoneBookL(KETelMeDialledPhoneBook);
	TestUnsupportedPhoneBookL(KETelMeMissedPhoneBook);
	TestUnsupportedPhoneBookL(KETelMeReceivedPhoneBook);
	TestUnsupportedPhoneBookL(KETelCombinedAdnPhoneBook);
	TestUnsupportedPhoneBookL(KETelCombinedSmsStore);
	TestUnsupportedPhoneBookL(KETelMeSmsStore);

	// CASE 2 - client supplies a supported phonebook but
	// LTSY completes with an error immediately

	// prepare "ExpectL" data
	RMobilePhone::TMobileName fdnPbkName(KETelIccFdnPhoneBook);
	TPckg<RMobilePhone::TMobileName> expectData(fdnPbkName);
	
	// prepare "CompleteL" data
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	storeInfo.iType = RMobilePhoneStore::EPhoneBookStore;
	storeInfo.iTotalEntries = 50;
	storeInfo.iUsedEntries = 20;
	storeInfo.iName = fdnPbkName;

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg completeData(storeInfo);

	// result storage	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);

	TRequestStatus status;

	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo, expectData, KErrNotSupported);
	iPhone.GetPhoneStoreInfo(status, infoPckg, fdnPbkName);
	User::WaitForRequest(status);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, status.Int())

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo, expectData);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneStoreInfo,KErrGeneral, completeData);
	iPhone.GetPhoneStoreInfo(status,infoPckg,fdnPbkName);
	User::WaitForRequest(status);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, status.Int())

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetPhoneStoreInfo when result is not cached.
 	//-------------------------------------------------------------------------

	// CASE 1: completion request of RMobilePhone::GetPhoneStoreInfo taking 3 arguments
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo,expectData);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneStoreInfo,KErrNone, completeData);
	iPhone.GetPhoneStoreInfo(status,infoPckg,fdnPbkName);
	User::WaitForRequest(status);

	ASSERT_EQUALS(KErrNone, status.Int())
	ASSERT_EQUALS(storeInfo.iTotalEntries, info.iTotalEntries)
	ASSERT_EQUALS(storeInfo.iUsedEntries, info.iUsedEntries)
	ASSERT_EQUALS(storeInfo.iType, info.iType)
	ASSERT_TRUE(0 == storeInfo.iName.Compare(info.iName))
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetPhoneStoreInfo
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneStoreInfo,KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone,mockLtsyStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetPhoneStoreInfo when result is not cached.
 	//-------------------------------------------------------------------------

	// CASE 2: completion request of RMobilePhone::GetPhoneStoreInfo taking 4 arguments
	iPhone.GetPhoneStoreInfo(status, infoPckg, KETelIccAdnPhoneBook,KEtelGsmPhoneBookType);
	User::WaitForRequest(status);
	
	// Overloaded RMobilePhone::GetPhoneStoreInfo(TRequestStatus &aStatus, 
	// TDes8 &aInfo, const TDesC &aStoreName, const TDesC &aMode) method is not 
	// supported by CTSY.
	ASSERT_EQUALS(KErrNotSupported, status.Int())

	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSY-PSTR-MGPSI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetPhoneStoreInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetPhoneStoreInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneStoreFU::TestGetPhoneStoreInfo0002L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobileName adnPbkName(KETelIccAdnPhoneBook);
	TPckg<RMobilePhone::TMobileName> expectData(adnPbkName);

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	storeInfo.iType = RMobilePhoneStore::EPhoneBookStore;
	storeInfo.iTotalEntries = 50;
	storeInfo.iUsedEntries = 20;
	storeInfo.iName = adnPbkName;

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg completeData(storeInfo);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 

	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo,expectData);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneStoreInfo, KErrNone, completeData, 10);

	TRequestStatus status;

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);

	iPhone.GetPhoneStoreInfo(status,infoPckg,adnPbkName);
	iPhone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo); 
	
	User::WaitForRequest(status);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrCancel,status.Int())
	ASSERT_EQUALS(KErrNone,mockLtsyStatus.Int())
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	}


/**
@SYMTestCaseID BA-CTSY-PSTR-MGPSI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPhoneStoreInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneStoreInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneStoreFU::TestGetPhoneStoreInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobileName adnPbkName(KETelIccAdnPhoneBook);

	TRequestStatus status;
	
	// Try to pass aInfo twice smaller than RMobilePhoneStore::TMobilePhoneStoreInfoV1
	TBuf8<sizeof(RMobilePhoneStore::TMobilePhoneStoreInfoV1) / 2> badParam;

	iPhone.GetPhoneStoreInfo(status,badParam,adnPbkName);
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrArgument,status.Int())

	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSY-PSTR-MGPSI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetPhoneStoreInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetPhoneStoreInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneStoreFU::TestGetPhoneStoreInfo0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret)
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret)
	CleanupClosePushL(phone2);

	// prepare "ExpectL" data 1
	RMobilePhone::TMobileName pbkName1(KETelIccAdnPhoneBook);
	TPckg<RMobilePhone::TMobileName> expectData1(pbkName1);

	// prepare "ExpectL" data 2
	RMobilePhone::TMobileName pbkName2(KETelIccFdnPhoneBook);
	TPckg<RMobilePhone::TMobileName> expectData2(pbkName2);

	// prepare "CompleteL" data 1
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 store1;
	store1.iType = RMobilePhoneStore::EPhoneBookStore;
	store1.iTotalEntries = 50;
	store1.iUsedEntries = 20;
	store1.iCaps = RMobilePhoneStore::KCapsDeleteAll;
	store1.iName = pbkName1;

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg completeData1(store1);

	// prepare "CompleteL" data 2
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 store2;
	store2.iType = RMobilePhoneStore::EPhoneStoreTypeUnknown;
	store2.iTotalEntries = 20;
	store2.iUsedEntries = 10;
	store2.iCaps = RMobilePhoneStore::KCapsWholeStore;
	store2.iName = pbkName2;

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg completeData2(store2);

	// create two EMobilePhoneGetPhoneStoreInfo events
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo,expectData1);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneStoreInfo,KErrNone, completeData1);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo,expectData2);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneStoreInfo,KErrNone, completeData2);

	// issue two simulteneous requests

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo1;
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1> storeInfoPckg1(storeInfo1);

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo2;
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1> storeInfoPckg2(storeInfo2);

	TRequestStatus status1;
	TRequestStatus status2;

	iPhone.GetPhoneStoreInfo(status1,storeInfoPckg1,pbkName1);
	phone2.GetPhoneStoreInfo(status2,storeInfoPckg2,pbkName2);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	ASSERT_EQUALS(KErrNone,status1.Int())
	ASSERT_EQUALS(store1.iType,storeInfo1.iType)
	ASSERT_EQUALS(store1.iTotalEntries,storeInfo1.iTotalEntries)
	ASSERT_EQUALS(store1.iUsedEntries,storeInfo1.iUsedEntries)
	ASSERT_EQUALS(store1.iCaps,storeInfo1.iCaps)
	ASSERT_TRUE(0 == store1.iName.Compare(storeInfo1.iName))

	ASSERT_EQUALS(KErrNone,status2.Int())
	ASSERT_EQUALS(store2.iType,storeInfo2.iType)
	ASSERT_EQUALS(store2.iTotalEntries,storeInfo2.iTotalEntries)
	ASSERT_EQUALS(store2.iUsedEntries,storeInfo2.iUsedEntries)
	ASSERT_EQUALS(store2.iCaps,storeInfo2.iCaps)
	ASSERT_TRUE(0 == store2.iName.Compare(storeInfo2.iName))
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3,this);//phone2, telServer2, this

	}

/**
@SYMTestCaseID BA-CTSY-PSTR-MGPSI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPhoneStoreInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneStoreInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneStoreFU::TestGetPhoneStoreInfo0005L()
	{
	// *************************************************************************************************
	// * Note: script timeout should be set to a value greater than KMmPhoneGetPhoneStoreInfoTimeOut !!!
	// *************************************************************************************************
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetPhoneStoreInfo
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobileName adnPbkName(KETelIccAdnPhoneBook);
	TPckg<RMobilePhone::TMobileName> expectData(adnPbkName);

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);

	TRequestStatus status;

	// Call the Etel API without doing the CompleteL
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneStoreInfo,expectData);	
	iPhone.GetPhoneStoreInfo(status,infoPckg,adnPbkName);
	User::WaitForRequest(status);

	// Check status is KErrTimedOut
	ASSERT_EQUALS(KErrTimedOut, status.Int())

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this); // this
	}



