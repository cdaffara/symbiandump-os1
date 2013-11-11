// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file The TEFUnit test suite for PhonebookEnControl in the Common TSY.
*/

#include "cctsyphonebookenfunegative.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchphonebookeninterface.h>
#include "listretrieverao.h"
#include "config.h"


CTestSuite* CCTsyPhonebookEnFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhonebookEnFUNegative, TestUnit0003L);
	

	END_SUITE;
	}


//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileENStore::GetInfo()
@SYMTestPriority High
@SYMTestActions Invokes RMobileENStore::GetInfo()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFUNegative::TestUnit0001L()
	{
	// write into the config file that the IPC is not supported
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileENStore enStore;
	TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(enStore);
	
	TRequestStatus reqStatus;
	
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 enStoreInfoV1;
	TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1> pckgInfoV1(enStoreInfoV1);
	
  
    //-------------------------------------------------------------------------
	// TEST A: The request is not supported
 	//-------------------------------------------------------------------------
    
	enStore.GetInfo(reqStatus, pckgInfoV1);		
    
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();

	config.Reset();
	
	CleanupStack::PopAndDestroy(2, this); // enStore, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-NEGATIVE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileONStore::Read()
@SYMTestPriority High
@SYMTestActions Invokes RMobileONStore::Read()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMobileENStore enStore;
	TInt ret = enStore.Open(iPhone);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(enStore);
	
	TRequestStatus reqStatus;
	
	TInt index = 10;
	
	RMobileENStore::TMobileENEntryV1 readEntry;
	readEntry.iIndex = index;
	TPckg<RMobileENStore::TMobileENEntryV1> pckgReadEntry(readEntry);
  
 	//-------------------------------------------------------------------------
	// TEST A: The request is not supported
 	//-------------------------------------------------------------------------

	enStore.Read(reqStatus, pckgReadEntry);		
    User::WaitForRequest(reqStatus);        
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());	
	
	AssertMockLtsyStatusL();
    
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // onStore, this
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PHONEBOOKEN-NEGATIVE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneENList::Start()
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneENList::Start()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPhonebookEnFUNegative::TestUnit0003L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId, EFalse);
		
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileENStore enStore;
	TInt err = enStore.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(enStore);

	CGetENListAO* getEnListAO = CGetENListAO::NewLC(enStore);

    //-------------------------------------------------------------------------
	// TEST A: The request is not supported
 	//-------------------------------------------------------------------------
  
    getEnListAO->TestRetrieveL();
    ASSERT_EQUALS(KErrNotSupported, getEnListAO->RetrieveLastError());

    AssertMockLtsyStatusL();
    
    config.Reset();
	CleanupStack::PopAndDestroy(3, this); // getEnListAO, enStore, this	
	}	


