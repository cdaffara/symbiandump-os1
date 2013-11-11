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
// The TEFUnit test suite for MailboxNumbers in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsymailboxnumbersfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

const TInt KOneSecond=1000000;  // Used in a time out function, 1 second (in microSeconds)

CTestSuite* CCTsyMailboxNumbersFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

    ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0001cL);
    ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0001dL);
	ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestGetMailboxNumbers0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyMailboxNumbersFU, TestNotifyMailboxNumbersChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMailboxNumbers
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMailboxNumbers
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobilePhoneBookStore bookStore;
	TName name(KETelIccVoiceMailBox);
	OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
	expVoicemailIds.iVoice = 1;
	expVoicemailIds.iData = 2;
	expVoicemailIds.iFax = 3;
	expVoicemailIds.iOther = 4;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
	                                             voicemailIdsData( expVoicemailIds );
	voicemailIdsData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data, KErrNotSupported);

	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
	voicemailIds.iVoice = 1;
	voicemailIds.iData = 2;
	voicemailIds.iFax = 3;
	voicemailIds.iOther = 4;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);
	iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data);

	RMobilePhone::TMobilePhoneVoicemailIdsV3 completeVoicemailIds;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
	                             completeVoicemailIdsData( completeVoicemailIds );
	data.Close();
	completeVoicemailIdsData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrGeneral, data);

	iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetMailboxNumbers when result is not cached.
 	//-------------------------------------------------------------------------
	data.Close();
	voicemailIdsData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data);

	completeVoicemailIds.iVoice = 5;
	completeVoicemailIds.iData = 6;
	completeVoicemailIds.iFax = 7;
	completeVoicemailIds.iOther = 8;
	data.Close();
	completeVoicemailIdsData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data);

	iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(completeVoicemailIds.iVoice, voicemailIds.iVoice);
	ASSERT_EQUALS(completeVoicemailIds.iData,  voicemailIds.iData);
	ASSERT_EQUALS(completeVoicemailIds.iFax,   voicemailIds.iFax);
	ASSERT_EQUALS(completeVoicemailIds.iOther, voicemailIds.iOther);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetMailboxNumbers
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(reqStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data);
	// wait for completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // data, bookStore, this
	
	}

/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMailboxNumbers when VMBX storage is not exist
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMailboxNumbers
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0001bL()
    {
	// Since we can not read the mail box number from the SIM if the VMBX storage was not initilized, the 
	// CTSY must make sure that the VMBX storage was initilize before requesting the mail box number from 
	// the LTSY. This test test if the CTSY knows to create a new VMBX and wait for the initilization to complete.
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RBuf8 data;
    CleanupClosePushL(data);

    TRequestStatus reqStatus;

    TName name(KETelIccVoiceMailBox);
    TMockLtsyPhoneBookData0 storeInitData(name);
    storeInitData.SerialiseL(data);
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);   


    RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
    voicemailIds.iVoice = 1;
    voicemailIds.iData = 2;
    voicemailIds.iFax = 3;
    voicemailIds.iOther = 4;
    RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);

    iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());

    CStorageInfoData storageData;
    SetStorageInfoData(storageData);
    
    // only after the book store is ready, and CTSY can send the EMobilePhoneGetMailboxNumbers.
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData);
    data.Close();
    retStoreInitC.SerialiseL(data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data, 0);

    RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
    expVoicemailIds.iVoice = 1;
    expVoicemailIds.iData = 2;
    expVoicemailIds.iFax = 3;
    expVoicemailIds.iOther = 4;
    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
                                                 voicemailIdsData( expVoicemailIds );
    data.Close();
    voicemailIdsData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data, KErrNone);

    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    
    RMobilePhone::TMobilePhoneVoicemailIdsV3 completeVoicemailIds;
    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
                                 completeVoicemailIdsData( completeVoicemailIds );
    completeVoicemailIds.iVoice = 5;
    completeVoicemailIds.iData = 6;
    completeVoicemailIds.iFax = 7;
    completeVoicemailIds.iOther = 8;
    data.Close();
    completeVoicemailIdsData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(completeVoicemailIds.iVoice, voicemailIds.iVoice);
    ASSERT_EQUALS(completeVoicemailIds.iData,  voicemailIds.iData);
    ASSERT_EQUALS(completeVoicemailIds.iFax,   voicemailIds.iFax);
    ASSERT_EQUALS(completeVoicemailIds.iOther, voicemailIds.iOther);
 
    CleanupStack::PopAndDestroy(2, this); // data, this
    
    }

/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc support in CTSY for RMobilePhone::GetMailboxNumbers when VMBX storage is exist but not ready
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMailboxNumbers
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0001cL()
    {
	// Since we can not read the mail box number from the SIM if the VMBX storage was not initilized, the 
	// CTSY must make sure that the VMBX storage was initilize before requesting the mail box number from 
	// the LTSY. This test test if the CTSY knows to wait for an previous initilize request to complete 
	// before requesting to get the mail box number.

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RBuf8 data;
    CleanupClosePushL(data);

    TRequestStatus reqStatus;

    TName name(KETelIccVoiceMailBox);
    TMockLtsyPhoneBookData0 storeInitData(name);
    storeInitData.SerialiseL(data);
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);   

    RMobilePhoneBookStore bookStore;
    CleanupClosePushL(bookStore);
    TInt ret = bookStore.Open(iPhone, name);    
    ASSERT_EQUALS(KErrNone, ret);   
    

    RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
    voicemailIds.iVoice = 1;
    voicemailIds.iData = 2;
    voicemailIds.iFax = 3;
    voicemailIds.iOther = 4;
    RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);

    iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());

    CStorageInfoData storageData;
    SetStorageInfoData(storageData);
    
    TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData);
    data.Close();
    retStoreInitC.SerialiseL(data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data, 0);

    RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
    expVoicemailIds.iVoice = 1;
    expVoicemailIds.iData = 2;
    expVoicemailIds.iFax = 3;
    expVoicemailIds.iOther = 4;
    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
                                                 voicemailIdsData( expVoicemailIds );
    data.Close();
    voicemailIdsData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data, KErrNone);

    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    
    RMobilePhone::TMobilePhoneVoicemailIdsV3 completeVoicemailIds;
    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
                                 completeVoicemailIdsData( completeVoicemailIds );
    completeVoicemailIds.iVoice = 5;
    completeVoicemailIds.iData = 6;
    completeVoicemailIds.iFax = 7;
    completeVoicemailIds.iOther = 8;
    data.Close();
    completeVoicemailIdsData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(completeVoicemailIds.iVoice, voicemailIds.iVoice);
    ASSERT_EQUALS(completeVoicemailIds.iData,  voicemailIds.iData);
    ASSERT_EQUALS(completeVoicemailIds.iFax,   voicemailIds.iFax);
    ASSERT_EQUALS(completeVoicemailIds.iOther, voicemailIds.iOther);
 
    CleanupStack::PopAndDestroy(3, this); // data, bookStore, this
    
    }

/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0001d
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMailboxNumbers when VMBX storage is not exist but another phone book store (ADN) exist
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMailboxNumbers
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0001dL()
    {
	// Since we can not read the mail box number from the SIM if the VMBX storage was not initilized, the 
	// CTSY must make sure that the VMBX storage was initilize before requesting the mail box number from 
	// the LTSY. This test test if the CTSY knows that another phone book store (ADN) initilization would do.

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    TName name(KETelIccAdnPhoneBook);
    RMobilePhoneBookStore adnBookStore;
    OpenPhoneBookStoreL(adnBookStore, name, iPhone);
    CleanupClosePushL(adnBookStore);
    
    RBuf8 data;
    CleanupClosePushL(data);

    TRequestStatus reqStatus;

    RMobilePhoneBookStore bookStore;
    CleanupClosePushL(bookStore);
    TInt ret = bookStore.Open(iPhone, name);    
    ASSERT_EQUALS(KErrNone, ret);   
    

    RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
    voicemailIds.iVoice = 1;
    voicemailIds.iData = 2;
    voicemailIds.iFax = 3;
    voicemailIds.iOther = 4;
    RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);

    RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
    expVoicemailIds.iVoice = 1;
    expVoicemailIds.iData = 2;
    expVoicemailIds.iFax = 3;
    expVoicemailIds.iOther = 4;
    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
                                                 voicemailIdsData( expVoicemailIds );
    data.Close();
    voicemailIdsData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data, KErrNone);
    
    iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, reqStatus.Int());

    RMobilePhone::TMobilePhoneVoicemailIdsV3 completeVoicemailIds;
    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
                                 completeVoicemailIdsData( completeVoicemailIds );
    completeVoicemailIds.iVoice = 5;
    completeVoicemailIds.iData = 6;
    completeVoicemailIds.iFax = 7;
    completeVoicemailIds.iOther = 8;
    data.Close();
    completeVoicemailIdsData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(completeVoicemailIds.iVoice, voicemailIds.iVoice);
    ASSERT_EQUALS(completeVoicemailIds.iData,  voicemailIds.iData);
    ASSERT_EQUALS(completeVoicemailIds.iFax,   voicemailIds.iFax);
    ASSERT_EQUALS(completeVoicemailIds.iOther, voicemailIds.iOther);
 
    CleanupStack::PopAndDestroy(4, this); // data, adnBookStore, bookStore, this
    
    }

/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetMailboxNumbers
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetMailboxNumbers
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0002L()
	{

// This test should test cancellation of GetMailboxNumbers
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobilePhoneBookStore bookStore;
    TName name(KETelIccVoiceMailBox);
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetMailboxNumbers
 	//------------------------------------------------------------------------- 	
	RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3> expVoicemailIdsData( expVoicemailIds );
	expVoicemailIdsData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data);

	RMobilePhone::TMobilePhoneVoicemailIdsV3 completeVoicemailIds;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
	                             completeVoicemailIdsData( completeVoicemailIds );
	data.Close();
	completeVoicemailIdsData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data, 10);

	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);
	iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);

	iPhone.CancelAsyncRequest(EMobilePhoneGetMailboxNumbers);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // data, bookStore, this
	
	}

/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMailboxNumbers with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMailboxNumbers with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetMailboxNumbers
 	//-------------------------------------------------------------------------
	TInt wrongVoicemailIds;
	TPckg<TInt> wrongVoicemailIdsPckg(wrongVoicemailIds);
	iPhone.GetMailboxNumbers(reqStatus, wrongVoicemailIdsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetMailboxNumbers
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetMailboxNumbers
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobilePhoneBookStore bookStore;
    TName name(KETelIccVoiceMailBox);
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);

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
	// Test A: Test multiple clients requesting RMobilePhone::GetMailboxNumbers
 	//-------------------------------------------------------------------------
	// setting and execute 1st request
	RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
	                                      expVoicemailIdsData( expVoicemailIds );
	expVoicemailIdsData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data);

	RMobilePhone::TMobilePhoneVoicemailIdsV3 completeVoicemailIds;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
	                             completeVoicemailIdsData( completeVoicemailIds );
	completeVoicemailIds.iVoice = 1;
	completeVoicemailIds.iData = 2;
	completeVoicemailIds.iFax = 3;
	completeVoicemailIds.iOther = 4;
	data.Close();
	completeVoicemailIdsData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetMailboxNumbers, KErrNone, data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);
	iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);
	
	// setting and execute 2nd request
	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds2;
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg2(voicemailIds2);
	phone2.GetMailboxNumbers(reqStatus2, voicemailIdsPckg2);
	
	// test completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeVoicemailIds.iVoice, voicemailIds.iVoice);
	ASSERT_EQUALS(completeVoicemailIds.iData,  voicemailIds.iData);
	ASSERT_EQUALS(completeVoicemailIds.iFax,   voicemailIds.iFax);
	ASSERT_EQUALS(completeVoicemailIds.iOther, voicemailIds.iOther);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(5, this); // phone2, bookStore, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-MBXN-MGMN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMailboxNumbers with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMailboxNumbers and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestGetMailboxNumbers0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RMobilePhoneBookStore bookStore;
    TName name(KETelIccVoiceMailBox);
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetMailboxNumbers
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneVoicemailIdsV3 expVoicemailIds;
	TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3>
	                                      expVoicemailIdsData( expVoicemailIds );
	expVoicemailIdsData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetMailboxNumbers, data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);
	iPhone.GetMailboxNumbers(reqStatus, voicemailIdsPckg);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 140102);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(3, this); // data, bookStore, this

	}


/**
@SYMTestCaseID BA-CTSY-MBXN-MNMNC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyMailboxNumbersChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyMailboxNumbersChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMailboxNumbersFU::TestNotifyMailboxNumbersChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIds;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsPckg(voicemailIds);
	iPhone.NotifyMailboxNumbersChange(reqStatus, voicemailIdsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}



