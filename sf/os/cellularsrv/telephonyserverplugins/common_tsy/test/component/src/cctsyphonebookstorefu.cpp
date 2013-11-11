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
//

/**
 @file The TEFUnit test suite for PhoneBookStore in the Common TSY.
*/

#include "cctsyphonebookstorefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/rmmcustomapi.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

const TInt KOneSecond=1000000;  // Used in a time out function, 1 second (in microSeconds)

CTestSuite* CCTsyPhoneBookStoreFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0004bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00027L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00029L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestRead00030L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00027L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00029L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestWrite00030L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0007L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent0009L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00017L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00019L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00022L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00024L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00027L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00029L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestNotifyStoreEvent00030L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00027L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00029L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDelete00030L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0001L);     //adn
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0001bL);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0004L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0006L);     //fdn
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0006bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0006cL);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0007L);  
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo0009L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00010L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00011L);   //sdn
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00011bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00011cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00015L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00016L);    //vmbx
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00016bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00017L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00020L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00021L);    //bdn
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00021bL);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00024L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00025L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00026L);    //mbdn
    ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00026bL);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00027L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00029L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestGetInfo00030L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0007L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00016L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00017L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00022L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00025L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00026L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00027L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00028L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00029L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestDeleteAll00030L);
	
	// test for coverage increasing
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Read0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Read0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Read0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Read0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Read0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Read0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test3Read0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test3Read0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test3Read0003L);

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Write0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Write0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Write0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Write0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Write0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, Test2Write0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneBookStoreFU, TestOpen0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0001L()
	{
	AuxRead1L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support for SIM refresh register Off in CTSY for RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0001bL()
	{
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));

    // SIM refresh register OFF
    OpenPhone2L(KErrGeneral);

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 data2;
    CleanupClosePushL(data2);
    
    RBuf8 data3;
	CleanupClosePushL(data3);

    // For CustomAPI
    iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
    RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
    currentlyRetrievedCache.iCacheId	= 1;
    currentlyRetrievedCache.iRecordId	= 0;		
    TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
    data.Close();
    ltsyDataCustomAPI.SerialiseL(data);			
    iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);

    // Open CustomAPI
    RMmCustomAPI customAPI;
    customAPI.Open(iPhone);
    CleanupClosePushL(customAPI);
    AssertMockLtsyStatusL();

    // Open ADN phonebook
    TName name(KETelIccAdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);

    TRequestStatus requestStatus;

    TInt index(1);
    TInt numEntries(1);	
    TBuf8<200> buf;			 
    
    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
	CleanupStack::PushL(entryPtr);
	entryPtr->ConstructL();	
	FillPBEntry(entryPtr);
	CleanupStack::Pop(entryPtr);
	
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);

    //-------------------------------------------------------------------------
    // TEST  checking failure of EMmTsyPhoneBookStoreInitIPC
    //-------------------------------------------------------------------------

    // EMmTsyPhoneBookStoreInitIPC
    data.Close();
    TMockLtsyPhoneBookData0 storeInitData(name);
    storeInitData.SerialiseL(data);	

    //storeInit
    CStorageInfoData storageData;       
    SetPhonebookNumOfEntries( storageData, 10 );

    TMockLtsyPhoneBookData1< CStorageInfoData > retStore(name, storageData); 
    retStore.SerialiseL(data2);

    // InitPhonebook return value is ignored in CTSY
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data, KErrNotSupported);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    // EMmTsyPhoneBookStoreCacheIPC            
    cache->AppendL(entryPtr);
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name, cache);
    data3.Close();
    storeCacheData.SerialiseL(data3);	
	data.Close();
	storeInitData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data3, 0);

    // EMmTsyPhoneBookStoreReadIPC
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  readData(name, indexAndEntries);
    data.Close();
    readData.SerialiseL(data);
    	
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrGeneral, data3);

    // EMmTsySimRefreshRegisterIPC
    iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);

    bookStore.Read(requestStatus, index, numEntries, buf); 
           	
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

    	    
    //-------------------------------------------------------------------------
    // TEST B: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    //EMmTsyPhoneBookStoreInitIPC
    TMockLtsyPhoneBookData0 storeInitData2(name);
    data.Close();
    storeInitData2.SerialiseL(data);	

    TMockLtsyPhoneBookData1< CStorageInfoData > retStore2(name, storageData); 
    data2.Close();
    retStore2.SerialiseL(data2);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    //EMmTsyPhoneBookStoreCacheIPC                            
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData2(name, cache);
    data3.Close();
    storeCacheData2.SerialiseL(data3);	
    data.Close();
	storeInitData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data3, 0);

    //EMmTsyPhoneBookStoreReadIPC    
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries >  readData2(name, indexAndEntries);
	data.Close();
	readData2.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrGeneral, data3);

    // EMmTsySimRefreshRegisterIPC
    iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);

    bookStore.Read(requestStatus, index, numEntries, buf); 
           	
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	


    //-------------------------------------------------------------------------
    // TEST C: 
    //-------------------------------------------------------------------------

    //EMmTsyPhoneBookStoreInitIPC
    TMockLtsyPhoneBookData0 storeInitData3(name);
    data.Close();
    storeInitData3.SerialiseL(data);	

    TMockLtsyPhoneBookData1< CStorageInfoData > retStore3(name, storageData); 
    data2.Close();
    retStore3.SerialiseL(data2);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);


    //EMmTsyPhoneBookStoreCacheIPC                            
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData3(name, cache);
    data3.Close();
    storeCacheData3.SerialiseL(data3);	
    data.Close();
	storeInitData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data3, 0);

    //EMmTsyPhoneBookStoreReadIPC
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  readData3(name, indexAndEntries);
	data.Close();
	readData3.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, data3);

    // EMmTsySimRefreshRegisterIPC
    iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);

    bookStore.Read(requestStatus, index, numEntries, buf); 
           	
    User::WaitForRequest(requestStatus);        

    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(7, this); // data, data2, data3, customAPI, bookStore, cache, this

    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support for Read after GetInfo requested before phonebook is ready
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0001cL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhone2L();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 data2;
    CleanupClosePushL(data2);

    TName name(KETelIccSdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    CleanupClosePushL(bookStore);
    
    TMockLtsyPhoneBookData0 storeInitData(name);
    storeInitData.SerialiseL(data);
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);   

    TInt ret = bookStore.Open(iPhone, name);    
    
    ASSERT_EQUALS(KErrNone, ret);   
    
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);
    bookStore.GetInfo(requestStatus, bookPckg);     
    
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());    
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());    
    

    CStorageInfoData storageData;
    SetStorageInfoData(storageData);
    storageData.iSDNNumOfEntries = 20;
    
    TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
    retStoreInitC.SerialiseL(data2);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    TInt usedEntries(12);
    data.Close();
    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());    
    User::After(KOneSecond);
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    TMockLtsyPhoneBookData1< TInt > styData2(name, usedEntries);
    data2.Close();    
    styData2.SerialiseL(data2);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2, 10);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    
    AssertMockLtsyStatusL();
    
    // Read from the SIM
    TInt index = 1;
    TInt numEntries = 1;
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
    data.Close();
    ltsyData.SerialiseL(data);

    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);  

    
    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData2(name, cache);
    data2.Close();
    ArrayData2.SerialiseL(data2);
    
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);  
    
    
    TBuf8<200> buf2;             
    bookStore.Read(requestStatus, index, numEntries, buf2);
    
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, data2, 0);
    
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());   
    

    CleanupStack::PopAndDestroy(5, this); // data, data2, this...   

    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0002L()
	{
	AuxRead2L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with bad parameter data for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read with bad parameter data for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0003L()
	{
    AuxRead3L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc 
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0004L()
	{
    AuxRead4L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0004b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Read for ADN phonebook while SIM refresh register is off
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0004bL()
	{
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));

    // SIM refresh register OFF
    OpenPhone2L(KErrGeneral);
    
    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 data2;
    CleanupClosePushL(data2);

    // For CustomAPI
    iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
    RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
    currentlyRetrievedCache.iCacheId	= 1;
    currentlyRetrievedCache.iRecordId	= 0;		
    TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
    data.Close();
    ltsyDataCustomAPI.SerialiseL(data);			
    iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);

    // Open CustomAPI
    RMmCustomAPI customAPI;
    customAPI.Open(iPhone);
    CleanupClosePushL(customAPI);
    AssertMockLtsyStatusL();

    // Open ADN phonebook 1
    TName name(KETelIccAdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);

    // Open ADN phonebook 2
    RMobilePhoneBookStore bookStore2;   	
    ret = bookStore2.Open(phone2, name);
    ASSERT_EQUALS(KErrNone, ret);	
    CleanupClosePushL(bookStore2);	
    AssertMockLtsyStatusL();     
    	
    TRequestStatus requestStatus;

    TInt index(1);
    TInt numEntries(1);	
    TBuf8<200> buf, buf2;			 
    
    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
	CleanupStack::PushL(entryPtr);
	entryPtr->ConstructL();	
	FillPBEntry(entryPtr);
	CleanupStack::Pop(entryPtr);
  
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
        
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);

    // EMmTsyPhoneBookStoreInitIPC
    data.Close();
    TMockLtsyPhoneBookData0 storeInitData(name);
    storeInitData.SerialiseL(data);	

    //storeInit
    CStorageInfoData storageData;       
    SetPhonebookNumOfEntries( storageData, 10 );

    TMockLtsyPhoneBookData1< CStorageInfoData > retStore(name, storageData); 
    retStore.SerialiseL(data2);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

	// EMmTsySimRefreshRegisterIPC
	//iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
	//iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);	
	
    //EMmTsyPhoneBookStoreCacheIPC         
    cache->AppendL(entryPtr);
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name, cache);
    data2.Close();
    storeCacheData.SerialiseL(data2);
    data.Close();
	storeInitData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data2, 0);

    //EMmTsyPhoneBookStoreReadIPC
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  readData(name, indexAndEntries);
	data.Close();
	readData.SerialiseL(data);

    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, data2);

    // EMmTsySimRefreshRegisterIPC
    iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);

    // First read
    bookStore.Read(requestStatus, index, numEntries, buf); 
    // Second read
    TRequestStatus requestStatus2;
    //User::After(50000); 
    bookStore2.Read(requestStatus2, index, numEntries, buf2);
    
    User::WaitForRequest(requestStatus);        
    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
    
    User::WaitForRequest(requestStatus2);        
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int()); 
    
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(9, this); // this, telServer2, phone2, data, data2, customAPI, bookStore, bookStore2, cache

    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with timeout for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read and tests for timeout for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0005L()
	{
    AuxRead5L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0006L()
	{
    AuxRead1L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Read for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Read for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0007L()
	{
    AuxRead2L(KETelIccFdnPhoneBook);
	}




/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with bad parameter data for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read with bad parameter data for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0008L()
	{
    AuxRead3L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Read for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead0009L()
	{
    AuxRead4L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with timeout for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read and tests for timeout for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00010L()
	{
    AuxRead5L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00011L()
	{
    AuxRead1L(KETelIccSdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00012L()
	{
	AuxRead2L(KETelIccSdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with bad parameter data for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read with bad parameter data for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00013L()
	{
    AuxRead3L(KETelIccSdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00014L()
	{
    AuxRead4L(KETelIccSdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00015
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with timeout for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read and tests for timeout for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00015L()
	{
    AuxRead5L(KETelIccSdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00016
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00016L()
	{
    AuxRead1L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00017
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Read for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Read for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00017L()
	{
	AuxRead2L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00018
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with bad parameter data for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read with bad parameter data for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00018L()
	{
	AuxRead3L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00019
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Read for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00019L()
	{
    AuxRead4L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00020
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with timeout for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read and tests for timeout for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00020L()
	{
    AuxRead5L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00021
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00021L()
	{
    AuxRead1L(KETelIccBdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00022
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Read for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Read for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00022L()
	{
	AuxRead2L(KETelIccBdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00023
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with bad parameter data for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read with bad parameter data for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00023L()
	{
	AuxRead3L(KETelIccBdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00024
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Read for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00024L()
	{
    AuxRead4L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00025
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with timeout for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read and tests for timeout for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00025L()
	{
    AuxRead5L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00026
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00026L()
	{
    AuxRead1L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00027
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Read for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Read for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00027L()
	{
	AuxRead2L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00028
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with bad parameter data for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read with bad parameter data for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00028L()
	{
	AuxRead3L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Read for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Read for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00029L()
	{
    AuxRead4L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR-00030
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Read with timeout for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Read and tests for timeout for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestRead00030L()
	{
    AuxRead5L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0001L()
	{	
	AuxWrite1L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Write for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Write for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0002L()
	{
	AuxWrite2L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with bad parameter data for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write with bad parameter data for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0003L()
	{
	AuxWrite3L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Write for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Write for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0004L()
	{
	AuxWrite4L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with timeout for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write and tests for timeout for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0005L()
	{
	AuxWrite5L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0006L()
	{
	AuxWrite1L(KETelIccFdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Write for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Write for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0007L()
	{
	AuxWrite2L(KETelIccFdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with bad parameter data for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write with bad parameter data for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0008L()
	{
	AuxWrite3L(KETelIccFdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Write for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Write for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite0009L()
	{
	AuxWrite4L(KETelIccFdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with timeout for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write and tests for timeout for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00010L()
	{
	AuxWrite5L(KETelIccFdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00011L()
	{
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhone2L();
    
    TName name(KETelIccSdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);
    
    TRequestStatus requestStatus;
    TInt index(-1); 
    TBuf8<200> buf;

    bookStore.Write(requestStatus, buf, index);
    
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrAccessDenied, requestStatus.Int());

    CleanupStack::PopAndDestroy(2, this);   //bookStore, this
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00016
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00016L()
	{
	AuxWrite1L(KETelIccVoiceMailBox );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00017
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Write for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Write for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00017L()
	{
	AuxWrite2L(KETelIccVoiceMailBox );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00018
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with bad parameter data for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write with bad parameter data for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00018L()
	{
	AuxWrite3L(KETelIccVoiceMailBox );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00019
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Write for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Write for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00019L()
	{
	AuxWrite4L(KETelIccVoiceMailBox );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00020
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with timeout for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write and tests for timeout for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00020L()
	{
	AuxWrite5L(KETelIccVoiceMailBox );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00021
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00021L()
	{
	// This test will fails since the CStorageInfoData dosn't have the BDN store information 
	//	(since this store is not supported by NTSY). So we can't check the size of the text and number, since we don't have max's. 
	// If and when the NTSY will support this store, we will need to add support in the CTSY, and enable the test again.
	ERR_PRINTF2(_L("<font color=Orange>@CTSYKnownFailure: defect id = %d</font>"), 40201);
	AuxWrite1L(KETelIccBdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00022
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Write for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Write for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00022L()
	{
	AuxWrite2L(KETelIccBdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00023
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with bad parameter data for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write with bad parameter data for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00023L()
	{
	AuxWrite3L(KETelIccBdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00024
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Write for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Write for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00024L()
	{
	AuxWrite4L(KETelIccBdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00025
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with timeout for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write and tests for timeout for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00025L()
	{
	AuxWrite5L(KETelIccBdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00026
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00026L()
	{
	AuxWrite1L(KETelIccMbdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00027
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Write for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Write for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00027L()
	{
	AuxWrite2L(KETelIccMbdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00028
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with bad parameter data for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write with bad parameter data for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00028L()
	{
	AuxWrite3L(KETelIccMbdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Write for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Write for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00029L()
	{
	AuxWrite4L(KETelIccMbdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW-00030
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Write with timeout for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Write and tests for timeout for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestWrite00030L()
	{
	AuxWrite5L(KETelIccMbdnPhoneBook );	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::NotifyStoreEvent for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::NotifyStoreEvent for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent0001L()
	{
	AuxNotifyStoreEvent1L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::NotifyStoreEvent for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::NotifyStoreEvent for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent0002L()
	{
	AuxNotifyStoreEvent2L(KETelIccAdnPhoneBook);		
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent0004L()
	{
	AuxNotifyStoreEvent4L(KETelIccAdnPhoneBook);		
	}



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::NotifyStoreEvent for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::NotifyStoreEvent for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent0006L()
	{
	AuxNotifyStoreEvent1L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::NotifyStoreEvent for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::NotifyStoreEvent for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent0007L()
	{
	AuxNotifyStoreEvent2L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent0009L()
	{
	AuxNotifyStoreEvent4L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::NotifyStoreEvent for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::NotifyStoreEvent for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00011L()
	{
	AuxNotifySDNStoreEvent1L();	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::NotifyStoreEvent for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::NotifyStoreEvent for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00012L()
	{
	AuxNotifySDNStoreEvent1L();	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00016
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::NotifyStoreEvent for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::NotifyStoreEvent for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00016L()
	{
	AuxNotifyStoreEvent1L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00017
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::NotifyStoreEvent for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::NotifyStoreEvent for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00017L()
	{
	AuxNotifyStoreEvent2L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00019
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00019L()
	{
	AuxNotifyStoreEvent4L(KETelIccVoiceMailBox);	
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00021
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::NotifyStoreEvent for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::NotifyStoreEvent for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00021L()
	{
	AuxNotifyStoreEvent1L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00022
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::NotifyStoreEvent for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::NotifyStoreEvent for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00022L()
	{
	AuxNotifyStoreEvent2L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00024
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00024L()
	{
	AuxNotifyStoreEvent4L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00026
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::NotifyStoreEvent for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::NotifyStoreEvent for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00026L()
	{
	AuxNotifyStoreEvent1L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00027
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::NotifyStoreEvent for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::NotifyStoreEvent for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00027L()
	{
	AuxNotifyStoreEvent2L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSNSE-00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::NotifyStoreEvent for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestNotifyStoreEvent00029L()
	{
	AuxNotifyStoreEvent4L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0001L()
	{
	AuxDelete1L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Delete for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Delete for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0002L()
	{
	AuxDelete2L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with bad parameter data for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete with bad parameter data for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0003L()
	{
	AuxDelete3L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Delete for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Delete for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0004L()
	{
	AuxDelete4L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with timeout for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete and tests for timeout for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0005L()
	{
	AuxDelete5L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0006L()
	{
	AuxDelete1L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Delete for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Delete for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0007L()
	{
	AuxDelete2L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with bad parameter data for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete with bad parameter data for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0008L()
	{
	AuxDelete3L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Delete for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Delete for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete0009L()
	{
	AuxDelete4L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with timeout for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete and tests for timeout for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00010L()
	{
	AuxDelete5L(KETelIccFdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00011L()
	{
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhone2L();
    
    TName name(KETelIccSdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);
    
    TRequestStatus requestStatus;
    TInt index(0);

    bookStore.Delete(requestStatus, index);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrAccessDenied, requestStatus.Int());

    CleanupStack::PopAndDestroy(2, this);   //bookStore, this
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00016
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00016L()
	{
	AuxDelete1L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00017
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Delete for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Delete for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00017L()
	{
	AuxDelete2L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00018
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with bad parameter data for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete with bad parameter data for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00018L()
	{
	AuxDelete3L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00019
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Delete for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Delete for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00019L()
	{
	AuxDelete4L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00020
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with timeout for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete and tests for timeout for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00020L()
	{
	AuxDelete5L(KETelIccVoiceMailBox);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00021
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00021L()
	{
	AuxDelete1L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00022
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Delete for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Delete for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00022L()
	{
	AuxDelete2L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00023
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with bad parameter data for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete with bad parameter data for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00023L()
	{
	AuxDelete3L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00024
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Delete for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Delete for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00024L()
	{	
	AuxDelete4L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00025
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with timeout for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete and tests for timeout for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00025L()
	{
	AuxDelete5L(KETelIccBdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00026
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00026L()
	{
	AuxDelete1L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00027
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::Delete for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::Delete for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00027L()
	{
	AuxDelete2L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00028
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with bad parameter data for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete with bad parameter data for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00028L()
	{
	AuxDelete3L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::Delete for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::Delete for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00029L()
	{
	AuxDelete4L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSD-00030
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::Delete with timeout for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::Delete and tests for timeout for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDelete00030L()
	{
	AuxDelete5L(KETelIccMbdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0001L()
    {
    AuxGetInfo1L(KETelIccAdnPhoneBook); 
    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for ADN phonebook with a slow SIM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0001bL()
    {
    AuxGetInfo1bL(KETelIccAdnPhoneBook); 
    }



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo with bad parameter data for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo with bad parameter data for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0003L()
	{
    AuxGetInfo3L(KETelIccAdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::GetInfo for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::GetInfo for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0004L()
	{
    AuxGetInfo4L(KETelIccAdnPhoneBook);	    
	}



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0006L()
    {
    AuxGetInfo1L(KETelIccFdnPhoneBook);     
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0006b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for FDN phonebook when ADN is de-activated and ADN phonebook name is returned
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0006bL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhone2L();
    
    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 completeData;
    CleanupClosePushL(data);
    
    TName name(KETelIccFdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);
    
    SetPhonebookNumOfEntriesAndInitPhonebookL( name, 50 );
    
    TPBFdnInfo fdnInfo;
    fdnInfo.iFDNNumOfEntries = 1;
    fdnInfo.iFDNNumberLengthMax = 10;
    fdnInfo.iFDNTextLengthMax = 50;
    
    TName nameAdn(KETelIccAdnPhoneBook);
    TMockLtsyPhoneBookData1<TPBFdnInfo> tsyData(nameAdn, fdnInfo);           
    tsyData.SerialiseL(data);
    
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);        
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreFdnInfoIPC,KErrNone,data,0);    
    User::WaitForRequest(mockLtsyStatus);        
    AssertMockLtsyStatusL(); 
    
    TRequestStatus requestStatus;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);
    
    bookStore.GetInfo(requestStatus, bookPckg); 
    
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());   
    ASSERT_TRUE(0 == bookInfo.iName.Compare(name));   
    ASSERT_EQUALS(0, bookInfo.iUsedEntries);  
    ASSERT_EQUALS(1, bookInfo.iTotalEntries); 
    ASSERT_EQUALS(50, bookInfo.iMaxTextLength);
    ASSERT_EQUALS(10, bookInfo.iMaxNumLength);  
    ASSERT_EQUALS(RMobilePhoneBookStore::ELocationIccMemory, bookInfo.iLocation);   
    ASSERT_EQUALS((TUint16)0, bookInfo.iChangeCounter); 
      
    CleanupStack::PopAndDestroy(4, this); // this, data, completeData, bookStore, cache   
    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0006c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for FDN phonebook with a slow SIM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0006cL()
    {
    AuxGetInfo1bL(KETelIccFdnPhoneBook);     
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo with bad parameter data for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo with bad parameter data for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0008L()
	{	
    AuxGetInfo3L(KETelIccFdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::GetInfo for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::GetInfo for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo0009L()
	{
    AuxGetInfo4L(KETelIccFdnPhoneBook);	    
	}



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00011L()
    {
    AuxGetInfo1L(KETelIccSdnPhoneBook);     
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00011b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for SDN phonebook with a slow SIM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00011bL()
    {
    AuxGetInfo1bL(KETelIccSdnPhoneBook);     
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00011c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for SDN phonebook when multiple phonebooks are opened simultaneously
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00011cL()
    {
    AuxGetInfo1cL();
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support cancelling in CTSY for RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo cancell for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00012L()
	{
    AuxGetInfo2L(KETelIccSdnPhoneBook);	    
	}




/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo with bad parameter data for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo with bad parameter data for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00013L()
	{
    AuxGetInfo3L(KETelIccSdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00014L()
	{
    AuxGetInfo4L(KETelIccSdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00015
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support for timeout in CTSY for RMobilePhoneBookStore::GetInfo for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo timeout for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00015L()
	{
    AuxGetInfo5L(KETelIccSdnPhoneBook);	    
	}





/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00016
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00016L()
    {
    AuxGetInfo1L(KETelIccVoiceMailBox);     
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00016b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for VMBX phonebook with a slow SIM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00016bL()
    {
    AuxGetInfo1bL(KETelIccVoiceMailBox);     
    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00017
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support for cancelling in CTSY for RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancel of RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00017L()
	{
    AuxGetInfo2L(KETelIccVoiceMailBox);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00018
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo with bad parameter data for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo with bad parameter data for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00018L()
	{
    AuxGetInfo3L(KETelIccVoiceMailBox);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00019
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00019L()
	{
    AuxGetInfo4L(KETelIccVoiceMailBox);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00020
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for timeout requests to RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes timeuot of multiple client requests to RMobilePhoneBookStore::GetInfo for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00020L()
	{
    AuxGetInfo5L(KETelIccVoiceMailBox);	    
	}



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00021
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00021L()
    {
    AuxGetInfo1L(KETelIccBdnPhoneBook);     
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00021b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for BDN phonebook with slow SIM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00021bL()
    {
    AuxGetInfo1bL(KETelIccBdnPhoneBook);     
    }


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00023
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo with bad parameter data for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo with bad parameter data for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00023L()
	{
    AuxGetInfo3L(KETelIccBdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00024
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::GetInfo for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::GetInfo for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00024L()
	{
    AuxGetInfo4L(KETelIccBdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00026
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00026L()
    {
    AuxGetInfo1L(KETelIccMbdnPhoneBook);        
    }

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00026b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo for MBDN phonebook with slow SIM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00026bL()
    {
    AuxGetInfo1bL(KETelIccMbdnPhoneBook);        
    }



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00028
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::GetInfo with bad parameter data for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::GetInfo with bad parameter data for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00028L()
	{
    AuxGetInfo3L(KETelIccMbdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSGI-00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::GetInfo for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::GetInfo for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestGetInfo00029L()
	{
    AuxGetInfo4L(KETelIccMbdnPhoneBook);	    
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0001L()
	{
	AuxDeleteAll1L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::DeleteAll for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::DeleteAll for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0002L()
	{
	AuxDeleteAll2L(KETelIccAdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::DeleteAll for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::DeleteAll for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0004L()
	{
	AuxDeleteAll4L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll with timeout for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll and tests for timeout for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0005L()
	{
	AuxDeleteAll5L(KETelIccAdnPhoneBook);	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0006L()
	{
	AuxDeleteAll1L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::DeleteAll for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::DeleteAll for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0007L()
	{
	AuxDeleteAll2L(KETelIccFdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::DeleteAll for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::DeleteAll for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll0009L()
	{
	AuxDeleteAll4L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll with timeout for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll and tests for timeout for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00010L()
	{
	AuxDeleteAll5L(KETelIccFdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00011L()
	{
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhone2L();
    
    TName name(KETelIccSdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);
    
    TRequestStatus requestStatus;

    bookStore.DeleteAll(requestStatus);

    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrAccessDenied, requestStatus.Int());

    CleanupStack::PopAndDestroy(2, this);   //bookStore, this
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00016
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00016L()
	{
	AuxDeleteAll1L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00017
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::DeleteAll for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::DeleteAll for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00017L()
	{
	AuxDeleteAll2L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00019
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::DeleteAll for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::DeleteAll for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00019L()
	{
	AuxDeleteAll4L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00020
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll with timeout for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll and tests for timeout for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00020L()
	{
	AuxDeleteAll5L(KETelIccVoiceMailBox);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00021
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00021L()
	{
	AuxDeleteAll1L(KETelIccBdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00022
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::DeleteAll for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::DeleteAll for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00022L()
	{
	AuxDeleteAll2L(KETelIccBdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00024
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::DeleteAll for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::DeleteAll for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00024L()
	{
	AuxDeleteAll4L(KETelIccBdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00025
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll with timeout for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll and tests for timeout for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00025L()
	{
	AuxDeleteAll5L(KETelIccBdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00026
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00026L()
	{
	AuxDeleteAll1L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00027
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhoneBookStore::DeleteAll for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhoneBookStore::DeleteAll for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00027L()
	{
	AuxDeleteAll2L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhoneBookStore::DeleteAll for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhoneBookStore::DeleteAll for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00029L()
	{
	AuxDeleteAll4L(KETelIccMbdnPhoneBook);
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSDA-00030
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneBookStore::DeleteAll with timeout for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneBookStore::DeleteAll and tests for timeout for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestDeleteAll00030L()
	{
	AuxDeleteAll5L(KETelIccMbdnPhoneBook);
	}


//-------------------------------------------------------------------


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR2-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Read for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Read for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Read0001L()
	{
	Aux2ReadL(KETelIccAdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR2-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Read for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Read for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Read0002L()
	{
	Aux2ReadL(KETelIccFdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR2-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Read for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Read for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Read0003L()
	{
	Aux2ReadL(KETelIccSdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR2-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Read for VBMX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Read for VBMX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Read0004L()
	{
	Aux2ReadL(KETelIccVoiceMailBox);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR2-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Read for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Read for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Read0005L()
	{
	Aux2ReadL(KETelIccBdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR2-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Read for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Read for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Read0006L()
	{
	Aux2ReadL(KETelIccMbdnPhoneBook);
	}



/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW2-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Write for ADN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Write for ADN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Write0001L()
	{	
	Aux2WriteL(KETelIccAdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW2-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Write for FDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Write for FDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Write0002L()
	{	
	Aux2WriteL(KETelIccFdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW2-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Write for SDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Write for SDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Write0003L()
	{	
	Aux2WriteL(KETelIccSdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW2-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Write for VMBX phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Write for VMBX phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Write0004L()
	{	
	Aux2WriteL(KETelIccVoiceMailBox);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW2-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Write for BDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Write for BDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Write0005L()
	{	
	Aux2WriteL(KETelIccBdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSW2-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Write for MBDN phonebook
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Write for MBDN phonebook
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test2Write0006L()
	{	
	Aux2WriteL(KETelIccMbdnPhoneBook);
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSO2-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhoneStore::Open 2 phonebooks of different types
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhoneStore::Open 2 phonebooks of different types
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::TestOpen0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));

    // SIM refresh register OFF
    OpenPhone2L(KErrGeneral);

    // Open ADN phonebook
    TName name1(KETelIccAdnPhoneBook);
    TName name2(KETelIccFdnPhoneBook);
    
    RMobilePhoneBookStore bookStore1;
    RMobilePhoneBookStore bookStore2;
    
    OpenPhoneBookStoreL(bookStore1, name1, iPhone);
    CleanupClosePushL(bookStore1);
    
    TMockLtsyPhoneBookData0 storeInitData(name1);
    TMockLtsyPhoneBookData0 storeInitData2(name2);
    
    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();	
    FillPBEntry(entryPtr);
    CleanupStack::Pop(entryPtr);

    // Open the second phonebook store - this won't need the phonebook be initialized
    // EMmTsyPhoneBookStoreCacheIPC
    
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    RBuf8 data;
    CleanupClosePushL(data);
    RBuf8 data2;
	CleanupClosePushL(data2);
    
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
    cache->AppendL(entryPtr);
    
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name2, cache);
    data2.Close();
    storeCacheData.SerialiseL(data2);
    data.Close();
    storeInitData2.SerialiseL(data);
                  
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data2, 0);
    
    // Open the phonebook store
    TInt ret = bookStore2.Open(iPhone, name2);
    CleanupClosePushL(bookStore2);
    ASSERT_EQUALS(KErrNone, ret);   
    
    User::WaitForRequest(mockLtsyStatus);           
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(6,this);	//this, bookstore1, data, data2, cache, bookstore2    
    }


//-------------------------------------------------------------------



/**
Standard function to open the phone up to the end of the boot sequence
Based on OpenPhoneL() from CCtsyComponentTestBase 
with changes for sim refresh register
*/
void CCTsyPhoneBookStoreFU::OpenPhone2L(TInt aSimError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	
	TRequestStatus modemDetectedStatus;
	RPhone::TModemDetection modemDetection;
	iPhone.NotifyModemDetected(modemDetectedStatus,modemDetection);
	
	err=iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// EMmTsyBootNotifyModemStatusReadyIPC
	iMockLTSY.CompleteL(EMmTsyBootNotifyModemStatusReadyIPC,KErrNone);

	// EMobilePhoneGetNetworkRegistrationStatus
	iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus,KErrNone,0);
	
	// EMmTsyBootNotifySimStatusReadyIPC
	iMockLTSY.ExpectL(EMmTsyBootNotifySimStatusReadyIPC);
	iMockLTSY.CompleteL(EMmTsyBootNotifySimStatusReadyIPC,KErrNone,0);

	// EMobilePhoneGetHomeNetwork
	iMockLTSY.ExpectL(EMobilePhoneGetHomeNetwork);
	RMobilePhone::TMobilePhoneNetworkInfoV5 homeNetwork;
	homeNetwork.iMode = RMobilePhone::ENetworkModeWcdma;
	homeNetwork.iStatus = RMobilePhone::ENetworkStatusCurrent;
	homeNetwork.iBandInfo = RMobilePhone::EBandUnknown;
	homeNetwork.iCountryCode = _L("234");
	homeNetwork.iCdmaSID = _L("");
	homeNetwork.iAnalogSID = _L("");
	homeNetwork.iNetworkId = _L("23499");
	homeNetwork.iDisplayTag = _L("symbian");
	homeNetwork.iShortName = _L("symbian");
	homeNetwork.iLongName = _L("symbian mobile");
	homeNetwork.iAccess = RMobilePhone::ENetworkAccessUtran;
	homeNetwork.iEgprsAvailableIndicator = ETrue;
	homeNetwork.iHsdpaAvailableIndicator = ETrue;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkInfoV5> homeNetworkData(homeNetwork);
	homeNetworkData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork,KErrNone,data,0);

	// EMmTsyPhoneGetPin1DisableSupportedIPC
 	iMockLTSY.ExpectL(EMmTsyPhoneGetPin1DisableSupportedIPC);
	TBool pin1DisableSupport = ETrue;
	TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
	data.Close();
	pin1DisableSupportData.SerialiseL(data);
	iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC,KErrNone,data,0);

	// EMmTsySimRefreshRegisterIPC
	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
	iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, aSimError, 0);		

	// EMobilePhoneGetServiceTable
	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	data.Close();
	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, data);
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
	serviceTableResult.iServices1To8  = 0xFF;
	serviceTableResult.iServices9To16 = 0xFF;
	serviceTableResult.iServices17To24= 0xFF;
	serviceTableResult.iServices25To32= 0xFF;
	serviceTableResult.iServices33To40= 0xFF;
	serviceTableResult.iServices41To48= 0xFF;
	serviceTableResult.iServices49To56= 0xFF;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
	data.Close();
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable,KErrNone,data,0);
	
	// EMobilePhoneGetALSLine
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine);

	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine,KErrNone,data,0);
	
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    
	
	// EMobilePhoneGetIccMessageWaitingIndicators
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);

	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    data.Close();
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);
	
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC);
	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	
	User::WaitForRequest(mockLtsyStatus);

	CleanupStack::PopAndDestroy(); // data
	}


void CCTsyPhoneBookStoreFU::ValidateReadEntryL(TInt aNumEntries, TInt aStartIndex, CPhoneBookBuffer& aPBBuffer, const TDesC& aPBName, CPhoneBookStoreEntry* aEntryPtr)
	{
	TUint8 tagValue;
    CPhoneBookBuffer::TPhBkTagType tagType; 
    TUint16 adnIndex;
    TInt ret (KErrNone);
    TInt index (aStartIndex);
    
	for(TInt i=0; i<aNumEntries; ++i) // aEntriesNum entries are supposed to be read.
		{
		INFO_PRINTF2(_L("Entry %d is being read."), i);
		ret = aPBBuffer.GetTagAndType( tagValue, tagType ); 
		ASSERT_EQUALS(KErrNone, ret);		
		ASSERT_TRUE(RMobilePhoneBookStore::ETagPBNewEntry == tagValue);   
	
		ret = aPBBuffer.GetTagAndType( tagValue, tagType ); 
		ASSERT_EQUALS(KErrNone, ret);	
		ASSERT_TRUE(RMobilePhoneBookStore::ETagPBAdnIndex == tagValue);   
	
		ret = aPBBuffer.GetValue( adnIndex ); 
		ASSERT_EQUALS(KErrNone, ret);	
		ASSERT_TRUE(index++ == adnIndex);	
	
		if(aPBName.CompareF( KETelIccBdnPhoneBook ))   // write of text does not succeed in BDN
			{            
			ret = aPBBuffer.GetTagAndType( tagValue, tagType ); 
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_TRUE(RMobilePhoneBookStore::ETagPBText == tagValue);   
			TPtrC16 textPtr;
			ret = aPBBuffer.GetValue( textPtr );
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_EQUALS(0, textPtr.Compare(*aEntryPtr->iText));
	
			ret = aPBBuffer.GetTagAndType( tagValue, tagType ); 
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_TRUE(RMobilePhoneBookStore::ETagPBNumber == tagValue);   
			ret = aPBBuffer.GetValue( textPtr );
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_EQUALS(0, textPtr.Compare(*aEntryPtr->iNumber));
	
			ret = aPBBuffer.GetTagAndType( tagValue, tagType ); 
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_TRUE(RMobilePhoneBookStore::ETagPBEmailAddress == tagValue);   
			ret = aPBBuffer.GetValue( textPtr );
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_EQUALS(0, textPtr.Compare((*aEntryPtr->iEmail)[0]));
	
			ret = aPBBuffer.GetTagAndType( tagValue, tagType ); 
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_TRUE(RMobilePhoneBookStore::ETagPBSecondName == tagValue);   
			ret = aPBBuffer.GetValue( textPtr );
			ASSERT_EQUALS(KErrNone, ret);	
			ASSERT_EQUALS(0, textPtr.Compare((*aEntryPtr->iSne)[0]));	
			}
		if(aPBName.CompareF( KETelIccAdnPhoneBook ) && aPBName.CompareF( KETelIccFdnPhoneBook ))
			{
			// Terminate the loop as only one entry has been read from SDN, BDN, MBDN and VMBX phonebooks.
			break;
			}
		}
	ret = aPBBuffer.GetTagAndType( tagValue, tagType );    // TON is skipped
	if(KErrNotFound != ret)
		{
		ERR_PRINTF1(_L("An unexpected entry has been returned!"));
		User::Leave(KErrTEFUnitFail);
		}
	}

void CCTsyPhoneBookStoreFU::AuxRead1L(const TDesC& aName)   
    {
    INFO_PRINTF1(_L("Start initialisation"));
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data1;
	CleanupClosePushL(data1);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RBuf8 data;
	CleanupClosePushL(data);
		
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(1);
	TInt numEntries(1);	
	TBuf8<400> buf;	
	  
	SetPhonebookNumOfEntriesAndInitPhonebookL( name, 10 );
	INFO_PRINTF1(_L("Initialisation complete"));
	
	INFO_PRINTF1(_L("TEST A: Failure to dispatch request to LTSY"));
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);  
    
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData1(name, indexAndEntries);
    ltsyData1.SerialiseL(data1);
            

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1, KErrNotSupported);	

    bookStore.Read(requestStatus, index, numEntries, buf); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	INFO_PRINTF1(_L("TEST B: failure on completion of pending request from LTSY->CTSY"));

    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData(name, cache);
    ArrayData.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrGeneral, completeData, 0);
    
    bookStore.Read(requestStatus, index, numEntries, buf); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	
	
	INFO_PRINTF1(_L("TEST E: Unsolicited completion of RMobilePhoneBookStore::Read from LTSY."));

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 0);
    User::WaitForRequest(mockLtsyStatus);        
    
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();

	INFO_PRINTF1(_L("TEST C: Successful completion request of RMobilePhoneBookStore::Read."));
    
    // now read 1entry when there is no data writen
    index = 1;
    indexAndEntries.iIndex = index;
    
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
    data1.Close();
    ltsyData.SerialiseL(data1);
    
    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData2(name, cache);
    completeData.Close();
    ArrayData2.SerialiseL(completeData);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 0);
    
	TBuf8<200> buf2;	 		 
    bookStore.Read(requestStatus, index, numEntries, buf2); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	TUint8 tagValue;
    CPhoneBookBuffer::TPhBkTagType tagType;    
    
    // check received buffer
    CPhoneBookBuffer bookBuffer;    
    bookBuffer.Set(&buf2);
    bookBuffer.StartRead();
    TInt ret = bookBuffer.GetTagAndType( tagValue, tagType ); 
    ASSERT_EQUALS(KErrNotFound, ret); // there is no data in buffer

    // multiple entries 1 ( number of entries in phoneBook set to 10)

    //set data for storeInit
	RBuf8 data2;
	CleanupClosePushL(data2);
    
    CStorageInfoData storageData;       
    
	SetStorageInfoData(storageData);
    SetPhonebookNumOfEntries( storageData, 10 );
           
    storageData.iEmailNumOfEntries = 3;
    storageData.iEmailNumOfEntriesPerEntry = 3;
    storageData.iEmailTextLengthMax = 50;
    storageData.iSNENumOfEntries = 3;
    storageData.iSNENumOfEntriesPerEntry = 3;
    storageData.iSNETextLengthMax = 50;
    
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreAndName(name, storageData); 
	retStoreAndName.SerialiseL(data2);	
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        

    INFO_PRINTF1(_L("Read 2 entries when there is no data writen"));
    numEntries = 2;
    indexAndEntries.iIndex = index;
    indexAndEntries.iNumSlots = numEntries;

    if ( name.CompareF( KETelIccAdnPhoneBook ) && name.CompareF( KETelIccFdnPhoneBook ))
        {        
		TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData2(name, indexAndEntries);
		data1.Close();
		ltsyData2.SerialiseL(data1);
                
        TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData3(name, cache);
        completeData.Close();
        ArrayData3.SerialiseL(completeData);

    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 10);

        bookStore.Read(requestStatus, index, numEntries, buf); 
    	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrNone, requestStatus.Int());   
    	
    	// check recieved buffer
    	bookBuffer.Set(&buf);
    	bookBuffer.StartRead();
    	ret = bookBuffer.GetTagAndType( tagValue, tagType ); 
    	ASSERT_EQUALS(KErrNotFound, ret);    	
        }
    else
        {
        bookStore.Read(requestStatus, index, numEntries, buf); 
    	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());   
        }
            
	AssertMockLtsyStatusL();
	
	//If the Phonebook is SDN type, then we can't write SDN entries into SDN phonebook.
	if (name.CompareF( KETelIccSdnPhoneBook ) != 0)
	    {
        INFO_PRINTF1(_L("Write 3 entries into the phonebook."));
        TRequestStatus writeStatus;
        TInt writeIndex(-1); 
        TBuf8<200> writeBuf;	 		 	    	    
        
        CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
        CleanupStack::PushL(entryPtr);
        entryPtr->ConstructL();
    
        CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
        CleanupStack::PushL(pbBuffer);
    
        pbBuffer->Set( &writeBuf );    
        ret = pbBuffer->AddNewEntryTag();
        TUint16 location(0);
        ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );
    
        _LIT(KPBText, "PBText");
        _LIT(KPBNumber, "PBNumber");
        _LIT(KPBEmailAddress, "PBEmailAddress");
        _LIT(KPBSecondName, "PBSecondName");
        const TUint8 KPBToNpi(0);
    
        if(name.CompareF( KETelIccBdnPhoneBook ))   // write of text does not succeed in BDN
            {            
            entryPtr->iText = HBufC::NewLC( KPBText().Length() );	
            entryPtr->iText->Des() = KPBText;
            CleanupStack::Pop();
            ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBText, KPBText);
    
            entryPtr->iNumber = HBufC::NewLC( KPBNumber().Length() );	
            entryPtr->iNumber->Des() = KPBNumber;
            CleanupStack::Pop();
            ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBNumber, KPBNumber);
    
            HBufC* hbuf = HBufC::NewLC( KPBEmailAddress().Length() );
            hbuf->Des() = KPBEmailAddress;
            CleanupStack::Pop();
            entryPtr->iEmail->AppendL( hbuf->Des() );
            ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBEmailAddress, KPBEmailAddress);
    
            HBufC* hbuf2 = HBufC::NewLC( KPBSecondName().Length() );
            hbuf2->Des() = KPBSecondName;
            CleanupStack::Pop();
            entryPtr->iSne->AppendL( hbuf2->Des() );
            ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBSecondName, KPBSecondName);
            
            ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBTonNpi, KPBToNpi);
            }
        
        
        TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyDataW(name, writeIndex, entryPtr);           
        tsyDataW.SerialiseL(data);
        
        TPBEntryInfo pbEntryInfo = {1, 50};
        TMockLtsyPhoneBookData1< TPBEntryInfo > styDataW2(name, pbEntryInfo);
        styDataW2.SerialiseL(data2);    
        
        // 1st entry    
        iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 4);
    
        bookStore.Write(writeStatus, writeBuf, writeIndex);
        
        User::WaitForRequest(writeStatus); 
        ASSERT_EQUALS(KErrNone, writeStatus.Int());	
    
        // 2nd entry
        pbEntryInfo.iLocation = 2;
        TMockLtsyPhoneBookData1< TPBEntryInfo > styData4(name, pbEntryInfo);
        data2.Close();
        styData4.SerialiseL(data2);
        
        writeIndex = -1;
    
        iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 4);
    
        bookStore.Write(writeStatus, writeBuf, writeIndex);
        
        User::WaitForRequest(writeStatus); 
        ASSERT_EQUALS(KErrNone, writeStatus.Int());	
        
        // 3rd entry
        pbEntryInfo.iLocation = 3;
        TMockLtsyPhoneBookData1< TPBEntryInfo > styData5(name, pbEntryInfo);
        data2.Close();
        styData5.SerialiseL(data2);
        
        writeIndex = -1;
    
        iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 4);
    
        bookStore.Write(writeStatus, writeBuf, writeIndex);
        
        User::WaitForRequest(writeStatus); 
        ASSERT_EQUALS(KErrNone, writeStatus.Int());	
        
        // read 2 entry, there is enough data to read ------------------	
        index = 1;
        numEntries = 2;
        indexAndEntries.iIndex = index;
        indexAndEntries.iNumSlots = numEntries;
    
        buf.Delete(0, buf.Length());
        
        if ( name.CompareF( KETelIccAdnPhoneBook ) && name.CompareF( KETelIccFdnPhoneBook ))
            {
            INFO_PRINTF1(_L("Read a single entry from the phonebook."));
            index = 1;
            numEntries = 1;
            indexAndEntries.iIndex = index;
            indexAndEntries.iNumSlots = numEntries;
            
            TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData5(name, indexAndEntries);
            data1.Close();
            ltsyData5.SerialiseL(data1);
    
            iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
            
            entryPtr->iLocation = index;
            cache->AppendL(entryPtr); 
                    
            TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData5(name, cache);
            completeData.Close();
            ArrayData5.SerialiseL(completeData);
    
            iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 0);
            }
        else
            {
            INFO_PRINTF1(_L("Read 2 entries from the phonebook."));
            }
            
        bookStore.Read(requestStatus, index, numEntries, buf); 
        
        User::WaitForRequest(requestStatus);        
        AssertMockLtsyStatusL();
        ASSERT_EQUALS(KErrNone, requestStatus.Int());   
            
        //check the buffer received
        bookBuffer.Set(&buf);
        bookBuffer.StartRead();
    
        ValidateReadEntryL(numEntries, index, bookBuffer, name, entryPtr);
        
        //Test: Read 2 entries when client does not send sufficient buffer.
        // As the size of entry varies for different phone book so test
        // this condition if the phonebook type is ADN or FDN only.
        TBuf8<50> smallBuf;
        if ( !name.CompareF( KETelIccAdnPhoneBook ) || !name.CompareF( KETelIccFdnPhoneBook ))
            {
            bookStore.Read(requestStatus, index, numEntries, smallBuf);
            User::WaitForRequest(requestStatus);
            AssertMockLtsyStatusL();
            ASSERT_EQUALS(KErrArgument, requestStatus.Int());
            }
    
        // read 1 entry, if the phonebook type is ADN or FDN. 1-entry-read has already been done for other phonebooks.
        if ( !name.CompareF( KETelIccAdnPhoneBook ) || !name.CompareF( KETelIccFdnPhoneBook ))
            {
            INFO_PRINTF1(_L("Read a single entry from the phonebook."));	
            
            index = 1;
            numEntries = 1;
            indexAndEntries.iIndex = index;
            indexAndEntries.iNumSlots = numEntries;
            
            buf.Delete(0, buf.Length());
            
            TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData4(name, indexAndEntries);
            data1.Close();    
            ltsyData4.SerialiseL(data1);
                
            if ( !name.CompareF( KETelIccAdnPhoneBook ) || !name.CompareF( KETelIccFdnPhoneBook ))
                { 
                entryPtr->iLocation = 1;
                cache->AppendL(entryPtr);   //data read from cache in complete
                }
    
            TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData4(name, cache);
            completeData.Close();
            ArrayData4.SerialiseL(completeData);
        
            iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
            iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 0);
                
            bookStore.Read(requestStatus, index, numEntries, buf); 
            
            User::WaitForRequest(requestStatus);        
            AssertMockLtsyStatusL();
            ASSERT_EQUALS(KErrNone, requestStatus.Int());   
            
            //check the buffer received
            bookBuffer.Set(&buf);
            bookBuffer.StartRead();
    
            ValidateReadEntryL(numEntries, index, bookBuffer, name, entryPtr);
    
            //Read 1 entry when client does not send sufficient buffer.
            smallBuf.Delete(0, smallBuf.Length());
            iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);
            iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 0);
            bookStore.Read(requestStatus, index, numEntries, smallBuf);
            User::WaitForRequest(requestStatus);
            AssertMockLtsyStatusL();
            ASSERT_EQUALS(KErrArgument, requestStatus.Int());
            }
        CleanupStack::PopAndDestroy(2); //entryPtr, pbBuffer
	    }

	CleanupStack::PopAndDestroy(7, this); // this, data1, completedata, data, bookstore, cache, data2
    }


    
void CCTsyPhoneBookStoreFU::AuxRead2L(const TDesC& aName)
    {
// This test should test cancellation of Read
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
	CleanupStack::PushL(entryPtr);
	entryPtr->ConstructL();	
	FillPBEntry(entryPtr);
	CleanupStack::Pop(entryPtr);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhoneBookStore::Read
 	//-------------------------------------------------------------------------
 	
    SetPhonebookNumOfEntriesAndInitPhonebookL( name, 10 );
 		
	TRequestStatus requestStatus;
	TInt index(1);
	TInt numEntries(1);	
	TBuf8<200> buf;			 
	
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
    cache->AppendL(entryPtr);
    
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
	ltsyData.SerialiseL(data);
            
    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData(name, cache);
    ArrayData.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);	
    	        
    bookStore.Read(requestStatus, index, numEntries, buf); 
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, data2, 10);

    bookStore.CancelAsyncRequest(EMobilePhoneBookStoreRead);
	
    User::WaitForRequest(requestStatus);        
	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(2); //bookStore, cache
	CleanupStack::PopAndDestroy(3); // data, data2, this	
    }
    
void CCTsyPhoneBookStoreFU::AuxRead3L(const TDesC& aName)
    {

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

    //storeInit
    CStorageInfoData storageData;       
    //bdn num entries set to 0
    storageData.iADNNumOfEntries = 10;
    storageData.iFDNNumOfEntries = 10;
    storageData.iSDNNumOfEntries = 10;
    storageData.iVMBXNumOfEntries = 10;
    storageData.iMBDNNumOfEntries = 10;   
        
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreAndName(name, storageData); 
	data2.Close();    
	retStoreAndName.SerialiseL(data2);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        

	TRequestStatus requestStatus;
	TInt index(-1);
	TInt numEntries(1);	    	        
	
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
	   
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobilePhoneBookStore::Read
 	//------------------------------------------------------------------------- 	
 	// test with index = -1;
    index = -1;
    TBuf8<200> buf1;

    bookStore.Read(requestStatus, index, numEntries, buf1); 
    	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int()); 
    
    // test with index = 0 and numEntries = 1;
    index = 0;
    numEntries = 1;
    
    bookStore.Read(requestStatus, index, numEntries, buf1); 
        
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();    
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());
    
    // test with index = 1 and numEntries = 0;
    index = 1;
    numEntries = 0;
    
    bookStore.Read(requestStatus, index, numEntries, buf1); 
    	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	// test with index = 1 and numEntries = 10000;
    index = 1;
    numEntries = 10000;

    // the size of the BDN phonebook is set to zero in CTSY
    // so there is no check for BDN phonebook size 
    if ( name.CompareF( KETelIccBdnPhoneBook ) )
        {
        bookStore.Read(requestStatus, index, numEntries, buf1); 	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());
        }
 	        
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhoneBookStore::Read
 	//-------------------------------------------------------------------------
	index = -1;
	numEntries = 1;	
	TBuf8<1> buf; // use very small buffer		 
 	        
    bookStore.Read(requestStatus, index, numEntries, buf); 
    	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(5); // cache, bookStore, data, data2, this    
	}
	
    
void CCTsyPhoneBookStoreFU::AuxRead4L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	RMobilePhoneBookStore bookStore2;
	
	ret = bookStore2.Open(phone2, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
	AssertMockLtsyStatusL();      
    
	CleanupClosePushL(bookStore2);
	
	SetPhonebookNumOfEntriesAndInitPhonebookL( name, 10 );
	
	CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
	CleanupStack::PushL(entryPtr);
	entryPtr->ConstructL();	
	FillPBEntry(entryPtr);
	CleanupStack::Pop(entryPtr);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhoneBookStore::Read
 	//-------------------------------------------------------------------------
	TInt index(1);
	TInt numEntries(1);	
	TBuf8<200> buf;			 
	TRequestStatus requestStatus;
	TBuf8<200> buf2;
	TRequestStatus requestStatus2;
	
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);

    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
	ltsyData.SerialiseL(data);
	
	cache->AppendL(entryPtr);
            
    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData(name, cache);
    ArrayData.SerialiseL(data2);
	
	// First Read
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, data2, 10);    	            
    bookStore.Read(requestStatus, index, numEntries, buf); 

	// Second Read
    bookStore2.Read(requestStatus2, index, numEntries, buf2); 
    
    // wait for first answer	
    User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	// wait for second answer
    User::WaitForRequest(requestStatus2);        
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	CleanupStack::PopAndDestroy(cache);
	CleanupStack::PopAndDestroy(&bookStore2);
	CleanupStack::PopAndDestroy(&bookStore);
	CleanupStack::PopAndDestroy(&data2);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(&phone2);
	CleanupStack::PopAndDestroy(&telServer2);
	CleanupStack::PopAndDestroy(this);     
    }
    
    
    
void CCTsyPhoneBookStoreFU::AuxRead5L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhoneBookStore::Read
 	//-------------------------------------------------------------------------
 	
 	SetPhonebookNumOfEntriesAndInitPhonebookL( name, 10 );
 	
	TRequestStatus requestStatus;
	TInt index(1);
	TInt numEntries(1);	
	TBuf8<200> buf;			 

    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);

    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
	ltsyData.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data);	
    
    bookStore.Read(requestStatus, index, numEntries, buf); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(2); //bookStore, cache
	CleanupStack::PopAndDestroy(2, this); // data, this
    }    


void CCTsyPhoneBookStoreFU::AuxWrite1L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(-1); 
	TBuf8<200> buf;	    	    

    //-------------------------------
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );
    
    //-------------------------------

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();
    
    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, index, entryPtr);           
    tsyData.SerialiseL(data);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data, KErrNotSupported);

    bookStore.Write(requestStatus, buf, index);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	


	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrGeneral, data2, 0);
    
    bookStore.Write(requestStatus, buf, index);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	
	

	//-------------------------------------------------------------------------
	// TEST C: successful completion of request
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 0);
    
    bookStore.Write(requestStatus, buf, index);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	


	//-------------------------------------------------------------------------
	// try to write to the place, where there is entry without text
 	//-------------------------------------------------------------------------
 	index = 1;
 	entryPtr->iText = HBufC::NewLC( name.Length() );	
    entryPtr->iText->Des() = name;
    CleanupStack::Pop();

    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBText, name);
 	
 	
    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData3(name, index, entryPtr);           
    data.Close();    
    tsyData3.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 0);
    
    bookStore.Write(requestStatus, buf, index);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// try to write to the place, where there is entry with text
 	//-------------------------------------------------------------------------
 	index = 1;
 	
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 0);
    
    bookStore.Write(requestStatus, buf, index);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhoneBookStore::Write
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        
    
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();



	CleanupStack::PopAndDestroy(1); //pbBuffer
	CleanupStack::PopAndDestroy(1); //entryPtr
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this	
    }


void CCTsyPhoneBookStoreFU::AuxWrite2L(const TDesC& aName)
    {	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(-1); 
	TBuf8<200> buf;	 		 
	    	    

    //-------------------------------
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhoneBookStore::Write
 	//-------------------------------------------------------------------------
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();

    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, index, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 10);

    bookStore.Write(requestStatus, buf, index);
    
	bookStore.CancelAsyncRequest(EMobilePhoneBookStoreWrite);
	                             
	User::WaitForRequest(requestStatus); 
 	
    // These request can't be cancelled, because no
    // functionality for cancelling. So everything
    // to do, is let the request continue and return KErrNone.(CMmPhoneBookStoreTsy::CancelService)
 	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	User::WaitForRequest(mockLtsyStatus);

	AssertMockLtsyStatusL();

    // to check branch with unknown request
    bookStore.CancelAsyncRequest(EMobilePhoneBookStoreWrite);
	 
	CleanupStack::PopAndDestroy(1); //pbBuffer
	CleanupStack::PopAndDestroy(1); //entryPtr
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }


void CCTsyPhoneBookStoreFU::AuxWrite3L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(0); 
	TBuf8<200> buf;	 		 
	    	    
    //--------------------------------------
    //  input bad index = 0
    //--------------------------------------
    
    bookStore.Write(requestStatus, buf, index);
    
	User::WaitForRequest(requestStatus); 
 	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	

	AssertMockLtsyStatusL();

    //--------------------------------------
    //  input not buffer without TVL tags
    //--------------------------------------
    
    index = -2;
    
    bookStore.Write(requestStatus, buf, index);
    
	User::WaitForRequest(requestStatus); 
	 	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	

	AssertMockLtsyStatusL();

    //  small buffer passing
    //-------------------------------------
        
    index = -1;
	TBuf8<1> buf2;
       
    bookStore.Write(requestStatus, buf2, index);
    
	User::WaitForRequest(requestStatus); 
 	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	

	AssertMockLtsyStatusL();


    //---------------------------------------------
	TRequestStatus writeStatus;
	TInt writeIndex(-1); 
	TBuf8<200> writeBuf;	 		 	    	    
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &writeBuf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();

 	entryPtr->iText = HBufC::NewLC( name.Length() );	
    entryPtr->iText->Des() = name;
    CleanupStack::Pop();
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBText, name);

 	entryPtr->iNumber = HBufC::NewLC( name.Length() );	
    entryPtr->iNumber->Des() = name;
    CleanupStack::Pop();
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBNumber, name);

    HBufC* hbuf = HBufC::NewLC( name.Length() );
    hbuf->Des() = name;
    CleanupStack::Pop();
    entryPtr->iEmail->AppendL( hbuf->Des() );
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBEmailAddress, name);

    HBufC* hbuf2 = HBufC::NewLC( name.Length() );
    hbuf2->Des() = name;
    CleanupStack::Pop();
    entryPtr->iSne->AppendL( hbuf2->Des() );
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBSecondName, name);
    
    TUint8 tmp = 0;
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBTonNpi, tmp);

    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, writeIndex, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 10);
    bookStore.Write(writeStatus, writeBuf, writeIndex);
    
	User::WaitForRequest(writeStatus); 
    if ( !name.CompareF( KETelIccAdnPhoneBook ) || 
    		!name.CompareF( KETelIccFdnPhoneBook ) || 
    		!name.CompareF( KETelIccVoiceMailBox ) || 
    		!name.CompareF( KETelIccMbdnPhoneBook ))
        {
         // fail because email text length not set in phoneBookStoreOpen()
    	ASSERT_TRUE(KErrArgument == writeStatus.Int());
        }
    else
        {
        // fail because of textlength is set in phonebookStoreOpen only for Adn, Fdn, Vmbx, Mbdn
	    ASSERT_TRUE(0xFFFFEC48 == writeStatus.Int()); 
        }

	CleanupStack::PopAndDestroy(6, this); // data, data2, this, etc...
    }


void CCTsyPhoneBookStoreFU::AuxWrite4L(const TDesC& aName)
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

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);


	RMobilePhoneBookStore bookStore2;
	
	ret = bookStore2.Open(phone2, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
	AssertMockLtsyStatusL();      
    
	CleanupClosePushL(bookStore2);
	
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	TInt index(-1); 
	TBuf8<200> buf;	 		 

    //-------------------------------
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhoneBookStore::Write
 	//-------------------------------------------------------------------------

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();

    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, index, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 10);

    bookStore.Write(requestStatus, buf, index);

    bookStore2.Write(requestStatus2, buf, index);

	User::WaitForRequest(requestStatus); 
 	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	


	User::WaitForRequest(requestStatus2); 
 	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(9, this); // phone2, telServer2, data, this...
    }


void CCTsyPhoneBookStoreFU::AuxWrite5L(const TDesC& aName)
    {	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(-1); 
	TBuf8<200> buf;	 		 
	    	    

    //-------------------------------
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

 	//-------------------------------------------------------------------------

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();
       
    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, index, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);

    bookStore.Write(requestStatus, buf, index);
    
	User::WaitForRequest(requestStatus); 
 	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	


	AssertMockLtsyStatusL();
	 
	CleanupStack::PopAndDestroy(1); //pbBuffer
	CleanupStack::PopAndDestroy(1); //entryPtr
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this			
    }


void CCTsyPhoneBookStoreFU::AuxNotifyStoreEvent1L(const TDesC& aName)
    {	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	
	TRequestStatus requestStatus;
	TUint32 event;
	TInt index(-1);

    //--------------------------------
    // notify on write
    //--------------------------------
    
	bookStore.NotifyStoreEvent(requestStatus, event, index );

    //--------------------------------
	TRequestStatus writeStatus;
	TInt writeIndex(-1); 
	TBuf8<200> buf;	 		 	    	    
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );
    
    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();
    
    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, writeIndex, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo;
    pbEntryInfo.iLocation = 0;
    pbEntryInfo.iMaxNumLength = 50;
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 0);

    bookStore.Write(writeStatus, buf, writeIndex);

	User::WaitForRequest(writeStatus);	
	ASSERT_EQUALS(KErrNone, writeStatus.Int());
    //--------------------------------

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( RMobilePhoneStore::KStoreEntryAdded == event );
	ASSERT_EQUALS(writeIndex, index);
 
	AssertMockLtsyStatusL();

    //--------------------------------
    // notify on delete
    //--------------------------------
	TRequestStatus deleteStatus;
	TInt deleteIndex(writeIndex); 

	bookStore.NotifyStoreEvent(requestStatus, event, index );

    TMockLtsyPhoneBookData1<TInt> tsyData3(name, deleteIndex);           
    data.Close();    
    tsyData3.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2);
	
    bookStore.Delete(deleteStatus, deleteIndex);
    
	User::WaitForRequest(deleteStatus);	
	ASSERT_EQUALS(KErrNone, deleteStatus.Int());

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//Event is KStoreEmpty because last entry was deleted from storage
	ASSERT_TRUE( RMobilePhoneStore::KStoreEmpty == event );
	ASSERT_EQUALS(deleteIndex, index);

    //--------------------------------
    // notify on deleteAll
    //--------------------------------
	bookStore.NotifyStoreEvent(requestStatus, event, index );
	
    TMockLtsyPhoneBookData0 tsyData4(name);           
    data.Close();    
    tsyData4.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2);

    bookStore.DeleteAll(deleteStatus);
    
	User::WaitForRequest(deleteStatus);	
	ASSERT_EQUALS(KErrNone, deleteStatus.Int());
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( RMobilePhoneStore::KStoreEmpty == event );
	ASSERT_TRUE(-1 == index);
	
	 
	CleanupStack::PopAndDestroy(2); //pbBuffer, entryPtr 
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }


void CCTsyPhoneBookStoreFU::AuxNotifyStoreEvent2L(const TDesC& aName)
    {	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	
	TRequestStatus requestStatus;
	TUint32 event;
	TInt index(-1);


    //--------------------------------
    // cancell notify on write
    //--------------------------------

	bookStore.NotifyStoreEvent(requestStatus, event, index );

    bookStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
    
    //-------------------------------
	TRequestStatus writeStatus;
	TInt writeIndex(-1); 
	TBuf8<200> buf;	 		 	    	    
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );
    
    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();

    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, writeIndex, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 0);

    bookStore.Write(writeStatus, buf, writeIndex);

	User::WaitForRequest(writeStatus);	
	ASSERT_EQUALS(KErrNone, writeStatus.Int());
    //-------------------------------

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	 
	AssertMockLtsyStatusL();
	
    // to check branch with unknown request
    bookStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	 
	CleanupStack::PopAndDestroy(2); //pbBuffer, entryPtr 
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }


void CCTsyPhoneBookStoreFU::AuxNotifyStoreEvent4L(const TDesC& aName)
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

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);


	RMobilePhoneBookStore bookStore2;
	
	ret = bookStore2.Open(phone2, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
	AssertMockLtsyStatusL();      
    
	CleanupClosePushL(bookStore2);
	

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	TUint32 event;
	TUint32 event2;
	TInt index(-1);
	TInt index2(-1);
	
	bookStore.NotifyStoreEvent(requestStatus, event, index );
	bookStore2.NotifyStoreEvent(requestStatus2, event2, index2 );


    //- write ------------------------------
	TRequestStatus writeStatus;
	TInt writeIndex(-1); 
	TBuf8<200> buf;	 		 
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &buf );    
    ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();

    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData(name, writeIndex, entryPtr);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 10);

    bookStore.Write(writeStatus, buf, writeIndex);

	User::WaitForRequest(writeStatus); 
 	
	ASSERT_EQUALS(KErrNone, writeStatus.Int());	
	//------------------------------


	User::WaitForRequest(requestStatus); 
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_TRUE( RMobilePhoneStore::KStoreEntryAdded == event );
	ASSERT_EQUALS(writeIndex, index);

	User::WaitForRequest(requestStatus2); 
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	ASSERT_TRUE( RMobilePhoneStore::KStoreEntryAdded == event2 );
	ASSERT_EQUALS(writeIndex, index2);
	

	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(9, this); // phone2, telServer2, data, this...
    }

void CCTsyPhoneBookStoreFU::AuxNotifySDNStoreEvent1L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    TName name(KETelIccSdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    
    OpenPhoneBookStoreL(bookStore, name, iPhone);
    CleanupClosePushL(bookStore);
    
    
    TRequestStatus requestStatus;
    TUint32 event;
    TInt index(-1);

    // The notification for SDN phonebook will never return.
    // User will get the notification through SAT that SDN phonebook
    // has been changed. Then user will read SDN entries.
    bookStore.NotifyStoreEvent(requestStatus, event, index );

    TRequestStatus timerStatus;
    RTimer timer;
    timer.CreateLocal();
    //Wait for 5 Seconds.
    timer.After(timerStatus, 5*KOneSecond);

    User::WaitForRequest(requestStatus, timerStatus);
    ASSERT_EQUALS(KErrNone, timerStatus.Int());

    bookStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
     
    AssertMockLtsyStatusL();
     
    CleanupStack::PopAndDestroy(2, this); // bookStore, this      
    
    }

void CCTsyPhoneBookStoreFU::AuxDelete1L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(0);
	// Increase PopAndDestroy index (this, data, data2, bookStore)
	TInt popAndDestroyIndex(4);
	    	    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    TMockLtsyPhoneBookData1<TInt> tsyData(name, index);           
    tsyData.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data, KErrNotSupported);
	
    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());


	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrGeneral, data2);

    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhoneBookStore::Delete
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2, 4);
    User::WaitForRequest(mockLtsyStatus);        
    
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhoneBookStore::Delete
 	//-------------------------------------------------------------------------

    // delete of unwriten entry
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2, 4);

    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	// KErrNone cause delete() does not care about entries with number more than exist
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    // Write only in case of ADN or FDN phonebook. This is because CTSY supports 
    // phonebook cache only for ADN and FDN. 
    // With other phonebooks writing does not affect to CTSY's delete functionality.
    if ( !name.CompareF( KETelIccAdnPhoneBook ) || !name.CompareF( KETelIccFdnPhoneBook ))
        {
    	// first write something -----------------------
    	TRequestStatus writeStatus;
    	TInt writeIndex(-1); 
    	TBuf8<200> writeBuf;	 		 	    	    
        
        CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
        CleanupStack::PushL(pbBuffer);
        
        // Increase PopAndDestroy index (pbBuffer)
        popAndDestroyIndex++;
        
        pbBuffer->Set( &writeBuf );    
        TInt ret = pbBuffer->AddNewEntryTag();
        TUint16 location(0);
        ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

        CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
        CleanupStack::PushL(entryPtr);
        entryPtr->ConstructL();
        
        // Increase PopAndDestroy index (entryPtr)
        popAndDestroyIndex++;
        
        _LIT(KPBText, "PBText");
        entryPtr->iText = HBufC::NewLC( KPBText().Length() );	
        entryPtr->iText->Des() = KPBText;
        CleanupStack::Pop();
        ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBText, KPBText);

        TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyDataW(name, writeIndex, entryPtr);           
        tsyDataW.SerialiseL(data);
        
        // Write entry    
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 4);

        bookStore.Write(writeStatus, writeBuf, writeIndex);
        
    	User::WaitForRequest(writeStatus); 
    	ASSERT_EQUALS(KErrNone, writeStatus.Int());
	    index = writeIndex;
        }

	//------------------------------------------------------	
	// Now delete entry
    TMockLtsyPhoneBookData1<TInt> tsyData4(name, index);           
    data.Close();    
    tsyData4.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2);

    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
   
	AssertMockLtsyStatusL();
	
    // this, data, data2, bookStore, pbBuffer, entryPtr 	
	CleanupStack::PopAndDestroy(popAndDestroyIndex);	
    }


void CCTsyPhoneBookStoreFU::AuxDelete2L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	TRequestStatus requestStatus;
	TInt index(0);
	    	    

    TMockLtsyPhoneBookData1<TInt> tsyData(name, index);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2);

    bookStore.Delete(requestStatus, index);
    bookStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
    
	User::WaitForRequest(requestStatus);	
           // These request can't be cancelled, because no
            // functionality for cancelling. So everything
            // to do, is let the request continue and return KErrNone. 
            //(from CMmPhoneBookStoreTsy::CancelService)
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    User::WaitForRequest(mockLtsyStatus);        
	AssertMockLtsyStatusL();


    bookStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }
    
void CCTsyPhoneBookStoreFU::AuxDelete3L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(-1);

    // Increase PopAndDestroy index (this, data, data2, bookStore)
	TInt popAndDestroyIndex(4);
    	    
    //---------------------------------------------
    // check bad index < 0
    //---------------------------------------------
    // If user supplies index which is less than 0, complete with KErrArgument,
    // each record is numbered from 1 to 255 (or maximum number of records).->
    
    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	

    //---------------------------------------------
    // delete when nothing was written
    //---------------------------------------------
	index = 0;	    	    
    
    TMockLtsyPhoneBookData1<TInt> tsyData_(name, index);           
    tsyData_.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);
    
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2, 4);

    bookStore.Delete(requestStatus, index);
    
    //TSY does not take the responsibility, if the location number is
    // greater than total numbers of records in this PBStore.
    // so we won't get error status
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	AssertMockLtsyStatusL();

    // Write only in case of ADN or FDN phonebook. This is because CTSY supports 
    // phonebook cache only for ADN and FDN. 
    // With other phonebooks writing does not affect to CTSY's delete functionality.
    if ( !name.CompareF( KETelIccAdnPhoneBook ) || !name.CompareF( KETelIccFdnPhoneBook ))
        {
    	// first write something -----------------------
    	TRequestStatus writeStatus;
    	TInt writeIndex(-1); 
    	TBuf8<200> writeBuf;	 		 	    	    
        
        CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
        CleanupStack::PushL(pbBuffer);
        
        // Increase PopAndDestroy index (pbBuffer)
        popAndDestroyIndex++;
        
        pbBuffer->Set( &writeBuf );    
        TInt ret = pbBuffer->AddNewEntryTag();
        TUint16 location(0);
        ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

        CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
        CleanupStack::PushL(entryPtr);
        entryPtr->ConstructL();
        
        // Increase PopAndDestroy index (entryPtr)
        popAndDestroyIndex++;
        
        _LIT(KPBText, "PBText");
        entryPtr->iText = HBufC::NewLC( KPBText().Length() );	
        entryPtr->iText->Des() = KPBText;
        CleanupStack::Pop();
        ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBText, KPBText);

        TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData3(name, writeIndex, entryPtr);           
        tsyData3.SerialiseL(data);
        
        // 1 entry    
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 4);

        bookStore.Write(writeStatus, writeBuf, writeIndex);
        
    	User::WaitForRequest(writeStatus); 
    	ASSERT_EQUALS(KErrNone, writeStatus.Int());	

        // 2 entry
        pbEntryInfo.iLocation = 1;
        TMockLtsyPhoneBookData1< TPBEntryInfo > styData4(name, pbEntryInfo);
        data2.Close();    
        styData4.SerialiseL(data2);
        
        writeIndex = -1;

    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 4);

        bookStore.Write(writeStatus, writeBuf, writeIndex);
        
    	User::WaitForRequest(writeStatus); 
    	ASSERT_EQUALS(KErrNone, writeStatus.Int());
        }

	// now delete the writen entry
	index = 0;
	
    TMockLtsyPhoneBookData1<TInt> tsyData4(name, index);           
    data.Close();    
    tsyData4.SerialiseL(data);

    pbEntryInfo.iLocation = 0;
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData5(name, pbEntryInfo);
    data2.Close();    
    styData5.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2, 4);

    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	AssertMockLtsyStatusL();

    // delete it again
	index = 0;
	
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNotFound, data2, 10);

    bookStore.Delete(requestStatus, index);
    
    // here KErrNotFound should be...
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	
    // this, data, data2, bookStore, pbBuffer, entryPtr 	
	CleanupStack::PopAndDestroy(popAndDestroyIndex);	
	
    }
    
    
void CCTsyPhoneBookStoreFU::AuxDelete4L(const TDesC& aName)
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

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 data3;
	CleanupClosePushL(data3);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);


	RMobilePhoneBookStore bookStore2;
	
	ret = bookStore2.Open(phone2, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
	AssertMockLtsyStatusL();      
    
	CleanupClosePushL(bookStore2);
	
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	TInt index(0); 
	TInt index2(1); 

    TMockLtsyPhoneBookData1<TInt> tsyData(name, index);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {1, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data2, 4);


    bookStore.Delete(requestStatus, index);

    TPBEntryInfo pbEntryInfo2 = {1, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData3(name, pbEntryInfo2);
    styData3.SerialiseL(data3);

    TMockLtsyPhoneBookData1<TInt> tsyData4(name, index2);           
    data.Close();    
    tsyData4.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteIPC, KErrNone, data3, 4);

    bookStore2.Delete(requestStatus2, index2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(8, this); // data, data2, this...
    }
    
    
void CCTsyPhoneBookStoreFU::AuxDelete5L(const TDesC& aName)
    {	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	TInt index(0);
	    	    

    TMockLtsyPhoneBookData1<TInt> tsyData(name, index);           
    tsyData.SerialiseL(data);

    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteIPC, data);

    bookStore.Delete(requestStatus, index);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }


void CCTsyPhoneBookStoreFU::AuxGetInfo1L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
   	TRequestStatus mockLtsyStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);

    if ( !name.CompareF( KETelIccSdnPhoneBook ) )
        {
        CStorageInfoData storageData;       
        
		SetStorageInfoData(storageData);
            
    	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
    	retStoreInitC.SerialiseL(data2);
    		
    	iMockLTSY.NotifyTerminated(mockLtsyStatus);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
        User::WaitForRequest(mockLtsyStatus);        
		}

    if ( !name.CompareF( KETelIccSdnPhoneBook ) || !name.CompareF( KETelIccVoiceMailBox ))
        {
        TMockLtsyPhoneBookData0 tsyData(name);           
        tsyData.SerialiseL(data);

        TInt usedEntries(-1);
        TMockLtsyPhoneBookData1< TInt > styData2(name, usedEntries);
        data2.Close();    
        styData2.SerialiseL(data2);

        // test A
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data, KErrNotSupported);
        bookStore.GetInfo(requestStatus, bookPckg); 
    	
        User::WaitForRequest(requestStatus);        
    	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
    	AssertMockLtsyStatusL();

        // test B
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
    	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrGeneral, data2);
    	
        bookStore.GetInfo(requestStatus, bookPckg); 
    	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

        // TEST C, just get info
    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
    	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2);
    	
        bookStore.GetInfo(requestStatus, bookPckg); 
    	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
    	ASSERT_TRUE(0 == bookInfo.iName.Compare(name));   
    	ASSERT_EQUALS(-1, bookInfo.iUsedEntries);	
    	ASSERT_EQUALS(-1, bookInfo.iTotalEntries);	
    	ASSERT_EQUALS(50, bookInfo.iMaxTextLength);	
    	ASSERT_EQUALS(50, bookInfo.iMaxNumLength);	
    	ASSERT_EQUALS(RMobilePhoneBookStore::ELocationIccMemory, bookInfo.iLocation);	
    	ASSERT_EQUALS((TUint16)0, bookInfo.iChangeCounter);	
    	if(!name.CompareF( KETelIccVoiceMailBox ))
    		{
	    	ASSERT_EQUALS(KPBTypeVMBXCaps, bookInfo.iCaps);	
    		}
    	else
    		{
	    	ASSERT_EQUALS((TUint32)RMobilePhoneStore::KCapsReadAccess, bookInfo.iCaps);	
    		}

        }
	else if ( !name.CompareF( KETelIccBdnPhoneBook ) )
        {
        bookStore.GetInfo(requestStatus, bookPckg); 
    	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
    	ASSERT_TRUE(0 == bookInfo.iName.Compare(name));     	
    	ASSERT_EQUALS(0, bookInfo.iUsedEntries);	
    	ASSERT_EQUALS(-1, bookInfo.iTotalEntries);	
    	ASSERT_EQUALS(-1, bookInfo.iMaxTextLength);	
    	ASSERT_EQUALS(-1, bookInfo.iMaxNumLength);	
    	ASSERT_EQUALS(RMobilePhoneBookStore::ELocationUnknown, bookInfo.iLocation);	
    	ASSERT_EQUALS((TUint16)0, bookInfo.iChangeCounter);	
    	ASSERT_EQUALS((TUint32)0, bookInfo.iCaps);	
        }
    else    // ADN FDN
        {
        //TEST C
        bookStore.GetInfo(requestStatus, bookPckg); 
    	
        User::WaitForRequest(requestStatus);        
    	AssertMockLtsyStatusL();
    	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
    	ASSERT_TRUE(0 == bookInfo.iName.Compare(name));     	
    	ASSERT_EQUALS(0, bookInfo.iUsedEntries);	
    	ASSERT_EQUALS(-1, bookInfo.iTotalEntries);	
    	ASSERT_EQUALS(50, bookInfo.iMaxTextLength);	
    	ASSERT_EQUALS(50, bookInfo.iMaxNumLength);	
    	ASSERT_EQUALS(RMobilePhoneBookStore::ELocationIccMemory, bookInfo.iLocation);	
    	ASSERT_EQUALS((TUint16)0, bookInfo.iChangeCounter);	
    	if(!name.CompareF( KETelIccAdnPhoneBook ))
    		{
	    	ASSERT_EQUALS((TUint32)(RMobilePhoneStore::KCapsDeleteAll | 
	    		RMobilePhoneStore::KCapsNotifyEvent |
	    			RMobilePhoneStore::KCapsReadAccess |
	    				RMobilePhoneStore::KCapsWriteAccess),
	    		bookInfo.iCaps);	
    		}
    	else if( !name.CompareF( KETelIccFdnPhoneBook ))
    		{
	    	ASSERT_EQUALS((TUint32)(RMobilePhoneStore::KCapsDeleteAll | 
	    		RMobilePhoneStore::KCapsNotifyEvent |
	    			RMobilePhoneStore::KCapsReadAccess |
	    				RMobilePhoneStore::KCapsWriteAccess |
	    					RMobilePhoneBookStore::KCapsRestrictedWriteAccess),
	    		bookInfo.iCaps);	
    		}
        else if( !name.CompareF( KETelIccVoiceMailBox ))
            {
            ASSERT_EQUALS(KPBTypeVMBXCaps, bookInfo.iCaps);
            }
        else if( !name.CompareF( KETelIccMbdnPhoneBook ))
            {
            ASSERT_EQUALS(KPBTypeMBDNCaps, bookInfo.iCaps);
            }
    	else
    		{
	    	ASSERT_EQUALS((TUint32)0, bookInfo.iCaps);	
    		}

        }

    // change something ----------------------------------------------------
    
    CStorageInfoData storageData;       
    const TInt KMaxLen = 150;
    const TUint32 KCaps
    	= RMobilePhoneStore::KCapsReadAccess
    	| RMobilePhoneStore::KCapsWriteAccess
    	| RMobilePhoneStore::KCapsNotifyEvent;
    
	SetStorageInfoData(storageData, KMaxLen);

    storageData.iVMBXCaps =
     storageData.iMBDNCaps = KCaps;
    
    storageData.iVMBXNumOfEntries = 
     storageData.iMBDNNumOfEntries = 0;
    
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
	data2.Close();    
	retStoreInitC.SerialiseL(data2);
		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        
    

    //now get changes ----------------------------------------    
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo2;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg2(bookInfo2);
	
    bookStore.GetInfo(requestStatus, bookPckg2); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	
	if ( !name.CompareF( KETelIccBdnPhoneBook ) )
    	{
		ASSERT_EQUALS(KErrNone, requestStatus.Int());	
		ASSERT_TRUE(name == bookInfo2.iName); 
		ASSERT_EQUALS(-1, bookInfo2.iMaxNumLength); 
		ASSERT_EQUALS(-1, bookInfo2.iMaxTextLength); 
		ASSERT_TRUE(bookInfo.iLocation == bookInfo2.iLocation); 
		ASSERT_TRUE(bookInfo.iIdentity == bookInfo2.iIdentity); 
		ASSERT_TRUE(bookInfo.iChangeCounter == bookInfo2.iChangeCounter);   // no write or delete was done
        }
    else
        {
    	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
    	ASSERT_TRUE(name == bookInfo2.iName); 
    	ASSERT_EQUALS(KMaxLen, bookInfo2.iMaxNumLength); 
    	ASSERT_EQUALS(KMaxLen, bookInfo2.iMaxTextLength); 
    	ASSERT_TRUE(bookInfo.iLocation == bookInfo2.iLocation); 
    	ASSERT_TRUE(bookInfo.iIdentity == bookInfo2.iIdentity); 
    	ASSERT_TRUE(bookInfo.iChangeCounter == bookInfo2.iChangeCounter);   // no write or delete was done
        if ( !name.CompareF( KETelIccVoiceMailBox ) )
            {
            ASSERT_EQUALS(KPBTypeVMBXCaps, bookInfo2.iCaps);  
            }
        else if ( !name.CompareF( KETelIccMbdnPhoneBook ) )
            {
            ASSERT_EQUALS(KPBTypeMBDNCaps, bookInfo2.iCaps);  
            }
        }

    // get info afer cache reset ----------------------------------------    

	//TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TMockLtsyPhoneBookData0 tsyData4(name);
    data.Close();    
    tsyData4.SerialiseL(data);
	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreResetCacheIPC, KErrNone, data, 2);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	        
	AssertMockLtsyStatusL();

	
    bookStore.GetInfo(requestStatus, bookPckg); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	
    if ( !name.CompareF( KETelIccAdnPhoneBook ) || !name.CompareF( KETelIccFdnPhoneBook ))
        {            
	    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());	
        }
    else
        {
	    ASSERT_EQUALS(KErrNone, requestStatus.Int());	
        }


    // Test E
    TInt usedEntries(-1);
    TMockLtsyPhoneBookData1< TInt > styData2(name, usedEntries);
    data2.Close();    
    styData2.SerialiseL(data2);
    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // data, data2, this...	
    }
    

void CCTsyPhoneBookStoreFU::AuxGetInfo1bL(const TDesC& aName)
    {
	// Since we can not get the phone book info from the SIM if the phone book was not initilized, the 
	// CTSY must make sure that the phone book  was initilize before requesting the info from 
	// the LTSY. This test test if the CTSY knows to wait for the initilize request to complete 
	// before requesting to get the phone book info.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	CleanupClosePushL(bookStore);
	
    TMockLtsyPhoneBookData0 storeInitData(name);
    storeInitData.SerialiseL(data);
  	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	

	TInt ret = bookStore.Open(iPhone, name);	
	
	ASSERT_EQUALS(KErrNone, ret);	
	
	TRequestStatus requestStatus;
   	TRequestStatus mockLtsyStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);
	bookStore.GetInfo(requestStatus, bookPckg); 	
	
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());	
	User::After(KOneSecond);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());	
	

    CStorageInfoData storageData;
	SetStorageInfoData(storageData);
	
    TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
    retStoreInitC.SerialiseL(data2);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    TInt expected = KErrNone;
    TInt usedEntries(12);
    if ( !aName.CompareF( KETelIccAdnPhoneBook ) ||
            !aName.CompareF( KETelIccFdnPhoneBook ))
        {
        data.Close();
        storeInitData.SerialiseL(data);
        iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
        CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
        CleanupStack::PushL(cache);
    
        TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name, cache);
        data2.Close();
        storeCacheData.SerialiseL(data2);   
    
                  
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data2, 0);
        CleanupStack::PopAndDestroy(cache);
        expected = KErrNotReady;
        }
    else if ( ( !aName.CompareF( KETelIccBdnPhoneBook ) ) || (!aName.CompareF( KETelIccMbdnPhoneBook ) ) )
        {
        usedEntries = 0;
        }
    else
        {
        data.Close();
        TMockLtsyPhoneBookData0 tsyData(name);           
        tsyData.SerialiseL(data);

        iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
        ASSERT_EQUALS(KRequestPending, requestStatus.Int());    
        User::After(KOneSecond);
        ASSERT_EQUALS(KRequestPending, requestStatus.Int());
        TMockLtsyPhoneBookData1< TInt > styData2(name, usedEntries);
        data2.Close();    
        styData2.SerialiseL(data2);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2, 10);
        }


    User::WaitForRequest(requestStatus);        
   	AssertMockLtsyStatusL();
   	ASSERT_EQUALS(expected, requestStatus.Int());
   	if ( ( aName.CompareF( KETelIccBdnPhoneBook ) ) && (KErrNone == expected) )
   	    {
        ASSERT_TRUE(0 == bookInfo.iName.Compare(name));
        
        ASSERT_EQUALS(usedEntries, bookInfo.iUsedEntries);	
        ASSERT_EQUALS(-1, bookInfo.iTotalEntries);	
        ASSERT_EQUALS(50, bookInfo.iMaxTextLength);	
        ASSERT_EQUALS(50, bookInfo.iMaxNumLength);	
        ASSERT_EQUALS(RMobilePhoneBookStore::ELocationIccMemory, bookInfo.iLocation);	
        ASSERT_EQUALS((TUint16)0, bookInfo.iChangeCounter);	
        if( !name.CompareF( KETelIccVoiceMailBox ))
            {
            ASSERT_EQUALS(KPBTypeVMBXCaps, bookInfo.iCaps);
            }
        else if( !name.CompareF( KETelIccMbdnPhoneBook ))
            {
            ASSERT_EQUALS(KPBTypeMBDNCaps, bookInfo.iCaps);
            }
        else if( !name.CompareF( KETelIccSdnPhoneBook ))
            {
            ASSERT_EQUALS((TUint32)RMobilePhoneStore::KCapsReadAccess, bookInfo.iCaps);
            }
        else
            {
            ASSERT_EQUALS((TUint32)0, bookInfo.iCaps);  
            }
	
   	    }
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // data, data2, this...	
    }

void CCTsyPhoneBookStoreFU::AuxGetInfo1cL()
    {
	//
	// Open three phonebooks: ADN, FDN, SDN. Check that SDN phonebook is initialised correctly.
	//
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 dataS7;
	CleanupClosePushL(dataS7);
	RBuf8 dataS8;
	CleanupClosePushL(dataS8);
	RBuf8 dataS9;
	CleanupClosePushL(dataS9);	
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName nameS7(KETelIccAdnPhoneBook);
	RMobilePhoneBookStore bookStoreS7;
	CleanupClosePushL(bookStoreS7);

	TName nameS8(KETelIccFdnPhoneBook);
	RMobilePhoneBookStore bookStoreS8;
	CleanupClosePushL(bookStoreS8);

	TName nameS9(KETelIccSdnPhoneBook);
	RMobilePhoneBookStore bookStoreS9;
	CleanupClosePushL(bookStoreS9);
	
	/*
	Test scenario:
		We open three bookstores: S7, S8, S9 (in that order).
		We then call GetInfo on S9.
	
	LTSY must receive (>>) and complete (<<) the following requests in the following sequence:
	>> EMmTsyPhoneBookStoreInitIPC (S7)
	<< EMmTsyPhoneBookStoreInitIPC (S7)
	>> EMmTsyPhoneBookStoreCacheIPC (S7)
	>> EMmTsyPhoneBookStoreCacheIPC (S8)
	>> EMmTsyPhoneBookStoreGetInfoIPC (S9)
	<< EMmTsyPhoneBookStoreCacheIPC (S7)
	<< EMmTsyPhoneBookStoreCacheIPC (S8)
	<< EMmTsyPhoneBookStoreGetInfoIPC (S9) 	
	*/
	
	TMockLtsyPhoneBookData0 storeInitData(nameS7);
    storeInitData.SerialiseL(dataS7);
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, dataS7);	

    // Open all phonebook store sessions. This will cause an Init request being sent down to LTSY,
    // followed by Cache requests for S7 and S8 when initialisation completes.
  	TInt ret = bookStoreS7.Open(iPhone, nameS7);
  	ret = bookStoreS8.Open(iPhone, nameS8);
	ret = bookStoreS9.Open(iPhone, nameS9);	
	 
	ASSERT_EQUALS(KErrNone, ret);	
	
	TRequestStatus requestStatus;
   	TRequestStatus mockLtsyStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);
	bookStoreS9.GetInfo(requestStatus, bookPckg); 	
	
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
	// check that CTSY waits for initialisation to complete 
	// before completing the GetInfo client request
	User::After(KOneSecond);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());	
	
	// prepare initialisation completion data
    CStorageInfoData storageData;
	SetStorageInfoData(storageData);
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(nameS7, storageData);
    retStoreInitC.SerialiseL(data2);

    // complete the Init request. This will trigger cache requests 
    // for S7 and S8 from CTSY to LTSY.
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

	dataS7.Close();
	TMockLtsyPhoneBookData0 tsyDataS7(nameS7);           
	tsyDataS7.SerialiseL(dataS7);        
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, dataS7);
	
	dataS8.Close();
	TMockLtsyPhoneBookData0 tsyDataS8(nameS8);           
	tsyDataS8.SerialiseL(dataS8);
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, dataS8);
	
    // GetInfo request should still be pending
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());

    // Prepare the cache responses, and tell MockLTSY to complete them
	CArrayPtrSeg<CPhoneBookStoreEntry>* cacheS7 = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
	CleanupStack::PushL(cacheS7);

	TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheDataS7(nameS7, cacheS7);
	data2.Close();
	storeCacheDataS7.SerialiseL(data2);   

	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data2, 0);
	CleanupStack::PopAndDestroy(cacheS7);
	
	CArrayPtrSeg<CPhoneBookStoreEntry>* cacheS8 = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
	CleanupStack::PushL(cacheS8);

	TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheDataS8(nameS8, cacheS8);
	data2.Close();
	storeCacheDataS8.SerialiseL(data2);   
			  
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, data2, 0);
	CleanupStack::PopAndDestroy(cacheS8);
	
	dataS9.Close();
	TMockLtsyPhoneBookData0 tsyDataS9(nameS9);           
	tsyDataS9.SerialiseL(dataS9);
	
	// Next to come from CTSY to LTSY will be the GetInfo request for S9
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, dataS9);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());    
	
	// prepare response and complete
	TInt usedEntries(12);
	TMockLtsyPhoneBookData1< TInt > styData2(nameS9, usedEntries);
	data2.Close();    
	styData2.SerialiseL(data2);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2, 10);
   
	User::WaitForRequest(requestStatus); // GetInfo
   	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 
 	// check that returned values are correct
    ASSERT_TRUE(0 == bookInfo.iName.Compare(nameS9));
    ASSERT_EQUALS(usedEntries, bookInfo.iUsedEntries);	
    ASSERT_EQUALS(-1, bookInfo.iTotalEntries);	
    ASSERT_EQUALS(50, bookInfo.iMaxTextLength);	
    ASSERT_EQUALS(50, bookInfo.iMaxNumLength);	
    ASSERT_EQUALS(RMobilePhoneBookStore::ELocationIccMemory, bookInfo.iLocation);	
    ASSERT_EQUALS((TUint16)0, bookInfo.iChangeCounter);	
    ASSERT_EQUALS((TUint32)RMobilePhoneStore::KCapsReadAccess, bookInfo.iCaps);
    
   	
   	// final check and cleanup
   	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); // data, data2, this...
    }

void CCTsyPhoneBookStoreFU::AuxGetInfo2L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);

    // this is only for SDN, VMBX !!!
    
    CStorageInfoData storageData;       
    
	SetStorageInfoData(storageData);
        
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
	retStoreInitC.SerialiseL(data2);
		
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        

    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);

    TInt usedEntries(-1);
    TMockLtsyPhoneBookData1< TInt > styData2(name, usedEntries);
    data2.Close();    
    styData2.SerialiseL(data2);


	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
	
    bookStore.GetInfo(requestStatus, bookPckg); 
    bookStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2, 10);
        	
    User::WaitForRequest(requestStatus);        
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
    
    User::WaitForRequest(mockLtsyStatus);        
	AssertMockLtsyStatusL();
	

	CleanupStack::PopAndDestroy(4, this); // data, data2, boookstore, this
    }


    
void CCTsyPhoneBookStoreFU::AuxGetInfo3L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;

	RMobilePhone::TMobilePhoneLockInfoV1 badBookInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg badBookPckg(badBookInfo);	

    bookStore.GetInfo(requestStatus, badBookPckg); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	

	CleanupStack::PopAndDestroy(2, this);   // bookStore, this
    }
    
    
void CCTsyPhoneBookStoreFU::AuxGetInfo4L(const TDesC& aName)
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

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);


	RMobilePhoneBookStore bookStore2;
	
	ret = bookStore2.Open(phone2, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
	AssertMockLtsyStatusL();      
    
	CleanupClosePushL(bookStore2);
	
	TRequestStatus requestStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);
	
	TRequestStatus requestStatus2;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo2;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg2(bookInfo2);

    if ( !name.CompareF( KETelIccSdnPhoneBook ) || !name.CompareF( KETelIccVoiceMailBox ) )
        {
        CStorageInfoData storageData;       
        
		SetStorageInfoData(storageData);
            
    	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
    	retStoreInitC.SerialiseL(data2);
    		
    	TRequestStatus mockLtsyStatus;
    	iMockLTSY.NotifyTerminated(mockLtsyStatus);
        iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
        User::WaitForRequest(mockLtsyStatus);        

        TMockLtsyPhoneBookData0 tsyData(name);           
        tsyData.SerialiseL(data);

        TInt usedEntries(-1);
        TMockLtsyPhoneBookData1< TInt > styData2(name, usedEntries);
        data2.Close();    
        styData2.SerialiseL(data2);

    	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
    	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreGetInfoIPC, KErrNone, data2);
        }


    bookStore.GetInfo(requestStatus, bookPckg); 

    bookStore2.GetInfo(requestStatus2, bookPckg2); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	


    User::WaitForRequest(requestStatus2);        
	AssertMockLtsyStatusL();
	
    if ( !name.CompareF( KETelIccAdnPhoneBook ) 
        || !name.CompareF( KETelIccFdnPhoneBook )
            || !name.CompareF( KETelIccMbdnPhoneBook ) 
               || !name.CompareF( KETelIccBdnPhoneBook ) )
        {            
	    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	    ASSERT_TRUE(bookInfo.iType == bookInfo2.iType);
	    ASSERT_TRUE(bookInfo.iTotalEntries == bookInfo2.iTotalEntries);
	    ASSERT_TRUE(bookInfo.iUsedEntries == bookInfo2.iUsedEntries);
	    ASSERT_TRUE(bookInfo.iCaps == bookInfo2.iCaps);
	    ASSERT_TRUE(bookInfo.iName == bookInfo2.iName);
	    ASSERT_TRUE(bookInfo.iMaxNumLength == bookInfo2.iMaxNumLength);
	    ASSERT_TRUE(bookInfo.iMaxTextLength == bookInfo2.iMaxTextLength);
	    ASSERT_TRUE(bookInfo.iLocation == bookInfo2.iLocation);
	    ASSERT_TRUE(bookInfo.iChangeCounter == bookInfo2.iChangeCounter);    	
	    ASSERT_TRUE(bookInfo.iIdentity == bookInfo2.iIdentity); 
	    }
    else
        {
	    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	
        }
    
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(7, this); // data, data2, this...	    
    }
    
 
 void CCTsyPhoneBookStoreFU::AuxGetInfo5L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 bookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg bookPckg(bookInfo);

    // this is only for SDN !!
    
    CStorageInfoData storageData;       
    
	SetStorageInfoData(storageData);
        
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
	retStoreInitC.SerialiseL(data);
		
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data, 0);
    User::WaitForRequest(mockLtsyStatus);        

    data.Close();
    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreGetInfoIPC, data);
	
    bookStore.GetInfo(requestStatus, bookPckg); 
        	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	


	CleanupStack::PopAndDestroy(3, this); // bookStore, data, this
    }


 
 
    
    
void CCTsyPhoneBookStoreFU::AuxDeleteAll1L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	    	    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data, KErrNotSupported);
	
    bookStore.DeleteAll(requestStatus);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());


	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrGeneral, data2);

    bookStore.DeleteAll(requestStatus);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhoneBookStore::Delete
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        
    
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhoneBookStore::Delete
 	//-------------------------------------------------------------------------
    
    // delete when there are no entries
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2);

    bookStore.DeleteAll(requestStatus);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    
	// now write something -----------------------
	
	TRequestStatus writeStatus;
	TInt writeIndex(-1); 
	TBuf8<200> writeBuf;	 		 	    	    
    
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);

    pbBuffer->Set( &writeBuf );    
    TInt ret = pbBuffer->AddNewEntryTag();
    TUint16 location(0);
    ret = pbBuffer->PutTagAndValue( RMobilePhoneBookStore::ETagPBAdnIndex, location );

    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();

    TMockLtsyPhoneBookData2<TInt,CPhoneBookStoreEntry*> tsyData3(name, writeIndex, entryPtr);           
    data.Close();    
    tsyData3.SerialiseL(data);
            
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreWriteIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreWriteIPC, KErrNone, data2, 10);

    bookStore.Write(writeStatus, writeBuf, writeIndex);
    
	User::WaitForRequest(writeStatus); 
	ASSERT_EQUALS(KErrNone, writeStatus.Int());	
	//----------------------------------------------------	
	
	// now delete the writen entry
	
    TMockLtsyPhoneBookData0 tsyData4(name);           
    data.Close();    
    tsyData4.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2);

    bookStore.DeleteAll(requestStatus);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(2); //pbBuffer, entryPtr
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }
    
    
void CCTsyPhoneBookStoreFU::AuxDeleteAll2L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	TRequestStatus requestStatus;
	    	    
    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);
 	
    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);

    bookStore.DeleteAll(requestStatus);
    bookStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);

	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2, 10);
    
	User::WaitForRequest(requestStatus);	

    // These request can't be cancelled, because no
    // functionality for cancelling. So everything
    // to do, is let the request continue and return KErrNone.
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);	
    
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this		
    }
    
    
void CCTsyPhoneBookStoreFU::AuxDeleteAll4L(const TDesC& aName)
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

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);


	RMobilePhoneBookStore bookStore2;
	
	ret = bookStore2.Open(phone2, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
	AssertMockLtsyStatusL();      
    
	CleanupClosePushL(bookStore2);
	
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);
 	
    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);


	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2, 4);

    bookStore.DeleteAll(requestStatus);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreDeleteAllIPC, KErrNone, data2, 4);

    bookStore2.DeleteAll(requestStatus2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(7, this); // data, data2, this...
    }
    
    
void CCTsyPhoneBookStoreFU::AuxDeleteAll5L(const TDesC& aName)
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
	    	    
    TMockLtsyPhoneBookData0 tsyData(name);           
    tsyData.SerialiseL(data);
 	
    TPBEntryInfo pbEntryInfo = {0, 50};
    TMockLtsyPhoneBookData1< TPBEntryInfo > styData2(name, pbEntryInfo);
    styData2.SerialiseL(data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreDeleteAllIPC, data);

    bookStore.DeleteAll(requestStatus);
    
	User::WaitForRequest(requestStatus);	

	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
    
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(3, this); // data, data2, this
    }
    
    

void CCTsyPhoneBookStoreFU::Aux2ReadL(const TDesC& aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	SetPhonebookNumOfEntriesAndInitPhonebookL( name, 10 );
	
	TRequestStatus requestStatus;
    RMobileONStore::TMobileONEntryV1 info;
    RMobileONStore::TMobileONEntryV1Pckg infoPckg(info);

 	//-------------------------------------------------------------------------
	// check if supported
 	//-------------------------------------------------------------------------

    bookStore.RMobilePhoneStore::Read(requestStatus, infoPckg); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	


    //-------------------------------------------------
    
    // just to shoot one more branch - ReceivedFdnData function
	RBuf8 data;
	CleanupClosePushL(data);
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	TPBFdnInfo fdnInfo;
	
    TMockLtsyPhoneBookData1<TPBFdnInfo> tsyData(name, fdnInfo);           
    tsyData.SerialiseL(data);

	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreFdnInfoIPC,KErrNone,data,0);    
    
    User::WaitForRequest(mockLtsyStatus);        
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();


	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreFdnInfoIPC,KErrGeneral,data,0);        
    User::WaitForRequest(mockLtsyStatus);        
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();
	
	//--------------------------------------------
		

	CleanupStack::PopAndDestroy(1); //data
	CleanupStack::PopAndDestroy(1); //bookStore
	CleanupStack::PopAndDestroy(1, this); // this	
	}


void CCTsyPhoneBookStoreFU::Aux2WriteL(const TDesC& aName)
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone2L();

	TName name(aName);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);
	
	TRequestStatus requestStatus;
    RMobileONStore::TMobileONEntryV1 info;
    RMobileONStore::TMobileONEntryV1Pckg infoPckg(info);
    
    CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
    CleanupStack::PushL(entryPtr);
    entryPtr->ConstructL();	
    FillPBEntry(entryPtr);
    CleanupStack::Pop(entryPtr);

 	//-------------------------------------------------------------------------
	// check if supported
 	//-------------------------------------------------------------------------

    bookStore.RMobilePhoneStore::Write(requestStatus, infoPckg); 
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	


    // just to check one more branch for cache init--------------------------------
	RBuf8 data;
	CleanupClosePushL(data);

    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
    cache->AppendL(entryPtr);

    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name, cache);
    storeCacheData.SerialiseL(data);
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrGeneral, data, 0);
    
    User::WaitForRequest(mockLtsyStatus);        
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();



    //-for storeInit------------------------------------------
	RBuf8 data2;
	CleanupClosePushL(data2);
	

    CStorageInfoData storageData;       
        
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
	retStoreInitC.SerialiseL(data2);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrGeneral, data2, 0);
    User::WaitForRequest(mockLtsyStatus);        
    
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();
    //----------------------------------------------


	CleanupStack::PopAndDestroy(5, this); // this etc	
	}




/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR3-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CMmPhoneBookStoreTsy::ContinueReadL()
@SYMTestPriority High
@SYMTestActions Invokes CMmPhoneBookStoreTsy::ContinueReadL()
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test3Read0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhone2L(KErrGeneral);   // here SIM refresh register is OFF
	
	RBuf8 data;
	CleanupClosePushL(data);

	// prepare iMockLTSY.ExpectLs for RMmCustomAPI opening
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
	RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
	currentlyRetrievedCache.iCacheId	= 1;
	currentlyRetrievedCache.iRecordId	= 0;		
	TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
	ltsyDataCustomAPI.SerialiseL(data);			
	iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);
		
    RMmCustomAPI customAPI;  
    TInt r = customAPI.Open (iPhone); 
    ASSERT_EQUALS(KErrNone, r);  
    CleanupClosePushL(customAPI);	

	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RBuf8 completeData2;
	CleanupClosePushL(completeData2);
		
	TName name(KETelIccAdnPhoneBook);
	RMobilePhoneBookStore bookStore;
	
	// open store with some changes for EMmTsyPhoneBookStoreInitIPC
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
            
    //EMmTsyPhoneBookStoreInitIPC
    TMockLtsyPhoneBookData0 storeInitData(name);
    data.Close();
    storeInitData.SerialiseL(data);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	

    CStorageInfoData storageData;       
    
	SetStorageInfoData(storageData);

    storageData.iADNNumOfEntries = 10;
    storageData.iFDNNumOfEntries = 10;
    storageData.iSDNNumOfEntries = 10;
    storageData.iVMBXNumOfEntries = 10;
    storageData.iMBDNNumOfEntries = 10; 

	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
	retStoreInitC.SerialiseL(data2);

    // make the store not initialised	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrGeneral, data2, 0);


	//EMmTsyPhoneBookStoreCacheIPC
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
                            
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name, cache);
    data.Close();    
    storeCacheData.SerialiseL(data);
    
	//Open
	TInt ret = bookStore.Open(iPhone, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
    CleanupClosePushL(bookStore);
	
    User::WaitForRequest(mockLtsyStatus);        	
	AssertMockLtsyStatusL();	
	
    // end of open store	
	
	TRequestStatus requestStatus;
	TInt index(0);
	TInt numEntries(1);	
	TBuf8<200> buf;	 		 
	    	    
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };

    TMockLtsyPhoneBookData2< RMobilePhoneBookStore::TPBIndexAndNumEntries, 
        CArrayPtrSeg<CPhoneBookStoreEntry>* >  ltsyData1(name, indexAndEntries, cache);
    ltsyData1.SerialiseL(data1);            


    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData(name, cache);
    ArrayData.SerialiseL(completeData);


 	//-------------------------------------------------------------------------
	// test leads to CMmPhoneBookStoreTsy::ContinueReadL() call
 	//-------------------------------------------------------------------------
	
    TMockLtsyPhoneBookData0 storeInitData2(name);
    data.Close();    
    storeInitData2.SerialiseL(data);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    
    // now read 1entry when there is no data writen
    index = 1;
    indexAndEntries.iIndex = index;
   
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
	data1.Close();    
	ltsyData.SerialiseL(data1);
    
    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData2(name, cache);
    completeData2.Close();    
    ArrayData2.SerialiseL(completeData2);
    completeData.Close();
	storeInitData.SerialiseL(completeData);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, completeData);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, completeData2, 0);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData2, 0);
    
    
	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);	
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrNone, 0);
    
    
	TBuf8<200> buf2;	 		 
    bookStore.Read(requestStatus, index, numEntries, buf2); 

	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	


    TUint8 tagValue;
    CPhoneBookBuffer::TPhBkTagType tagType;
    
    CPhoneBookBuffer bookBuffer;    
    bookBuffer.Set(&buf2);
    bookBuffer.StartRead();

    ret = bookBuffer.GetTagAndType( tagValue, tagType ); 
    ASSERT_EQUALS(KErrNotFound, ret);    

	
	CleanupStack::PopAndDestroy(9, this);		//this, data, customAPI, data1, data2, completeData, completeData2, cache, bookStore 
	
	}


/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR3-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for read without SIM register and caching
@SYMTestPriority High
@SYMTestActions Test support in CTSY for read without SIM register and caching
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test3Read0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhone2L(KErrGeneral);   // here SIM refresh register is OFF

	RBuf8 data;
	CleanupClosePushL(data);

		
	// prepare iMockLTSY.ExpectLs for RMmCustomAPI opening
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
	RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
	currentlyRetrievedCache.iCacheId	= 1;
	currentlyRetrievedCache.iRecordId	= 0;		
	TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
	ltsyDataCustomAPI.SerialiseL(data);			
	iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);
		
    RMmCustomAPI customAPI;  
    TInt r = customAPI.Open (iPhone); 
    ASSERT_EQUALS(KErrNone, r);  
    CleanupClosePushL(customAPI);
        
	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RBuf8 completeData2;
	CleanupClosePushL(completeData2);
		
	TName name(KETelIccAdnPhoneBook);
	RMobilePhoneBookStore bookStore;
	
	// open store with some changes for EMmTsyPhoneBookStoreInitIPC
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
	CleanupStack::PushL(entryPtr);
	entryPtr->ConstructL();	
	FillPBEntry(entryPtr);
	CleanupStack::Pop(entryPtr);
            
    //EMmTsyPhoneBookStoreInitIPC
    TMockLtsyPhoneBookData0 storeInitData(name);
    data.Close();
    storeInitData.SerialiseL(data);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	

    CStorageInfoData storageData;       
    
	SetStorageInfoData(storageData);
	
    storageData.iADNNumOfEntries = 10;
    storageData.iFDNNumOfEntries = 10;
    storageData.iSDNNumOfEntries = 10;
    storageData.iVMBXNumOfEntries = 10;
    storageData.iMBDNNumOfEntries = 10; 
    
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
	retStoreInitC.SerialiseL(data2);

    // make the store not initialised	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrGeneral, data2, 0);

	//EMmTsyPhoneBookStoreCacheIPC    
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache1 = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
	CleanupStack::PushL(cache1);
    
	//Open
	TInt ret = bookStore.Open(iPhone, name);
	ASSERT_EQUALS(KErrNone, ret);	
	
    CleanupClosePushL(bookStore);
	
    User::WaitForRequest(mockLtsyStatus);        	
	AssertMockLtsyStatusL();	
		
    // end of open store	
	
	TRequestStatus requestStatus;
	TInt index(1);
	TInt numEntries(1);	
	TBuf8<200> buf;	 		 
	    	    
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
    
    cache1->AppendL(entryPtr);
    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData(name, cache1);
    ArrayData.SerialiseL(completeData2);
    completeData.Close();
	storeInitData.SerialiseL(completeData);

    // read without caching and SIM //
    
	RBuf8 data4;
	CleanupClosePushL(data4);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TMockLtsyPhoneBookData0 tsyData4(name);
    tsyData4.SerialiseL(data4);
	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreResetCacheIPC, KErrNone, data4, 2);
    User::WaitForRequest(mockLtsyStatus);            
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();
	
    index = 1;
    numEntries = 1;


    TMockLtsyPhoneBookData0 storeInitData2(name);
    data.Close();
    storeInitData2.SerialiseL(data);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, completeData);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrNone, completeData2, 0);
	
	TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
	data1.Close();
	ltsyData.SerialiseL(data1);
    
	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData2, 0);

    bookStore.Read(requestStatus, index, numEntries, buf); 
    	
	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);	
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);
    	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

    ///////////////////////////////////////////////////
	
	CleanupStack::PopAndDestroy(10, this); 		//this, data, customAPI, data1, data2, completeData, completeData2, cache1, bookStore, data4
	
	}

/**
@SYMTestCaseID BA-CTSY-PBSTR-PBSR3-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for SDN entry read without SIM register and caching
@SYMTestPriority High
@SYMTestActions Test support in CTSY for SDN entry read without SIM register and caching
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneBookStoreFU::Test3Read0003L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    
    OpenPhone2L(KErrGeneral);   // here SIM refresh register is OFF

    RBuf8 data;
    CleanupClosePushL(data);

        
    // prepare iMockLTSY.ExpectLs for RMmCustomAPI opening
    iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
    RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
    currentlyRetrievedCache.iCacheId    = 1;
    currentlyRetrievedCache.iRecordId   = 0;        
    TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
    ltsyDataCustomAPI.SerialiseL(data);         
    iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);
        
    RMmCustomAPI customAPI;  
    TInt r = customAPI.Open (iPhone); 
    ASSERT_EQUALS(KErrNone, r);  
    CleanupClosePushL(customAPI);
        
    RBuf8 data1;
    CleanupClosePushL(data1);

    RBuf8 data2;
    CleanupClosePushL(data2);
    
    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName name(KETelIccSdnPhoneBook);
    RMobilePhoneBookStore bookStore;
    
    // open store with some changes for EMmTsyPhoneBookStoreInitIPC
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

            
    //EMmTsyPhoneBookStoreInitIPC
    TMockLtsyPhoneBookData0 storeInitData(name);
    data.Close();
    storeInitData.SerialiseL(data);
    
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);   

    CStorageInfoData storageData;       
    
    SetStorageInfoData(storageData);
    
    storageData.iADNNumOfEntries = 10;
    storageData.iFDNNumOfEntries = 10;
    storageData.iSDNNumOfEntries = 10;
    storageData.iVMBXNumOfEntries = 10;
    storageData.iMBDNNumOfEntries = 10; 
    
    TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(name, storageData); 
    retStoreInitC.SerialiseL(data2);

    // make the store initialised   
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);


    //EMmTsyPhoneBookStoreCacheIPC
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);

    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(name, cache);
    data.Close();    
    storeCacheData.SerialiseL(data);
    
    //Open
    TInt ret = bookStore.Open(iPhone, name);
    ASSERT_EQUALS(KErrNone, ret);   
    
    CleanupClosePushL(bookStore);
    
    User::WaitForRequest(mockLtsyStatus);           
    AssertMockLtsyStatusL();    
        
    // end of open store

    TRequestStatus requestStatus;
    TInt index(1);
    TInt numEntries(1); 
    TBuf8<200> buf;          
                
    RMobilePhoneBookStore::TPBIndexAndNumEntries indexAndEntries = {index, numEntries };
     
    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData1(name, indexAndEntries);
	ltsyData1.SerialiseL(data1);


    TMockLtsyPhoneBookData1< CArrayPtrSeg<CPhoneBookStoreEntry>* > ArrayData(name, cache);
    ArrayData.SerialiseL(completeData);

    index = 1;
    numEntries = 1;

    TMockLtsyPhoneBookData0 storeInitData2(name);
    data.Close();
    storeInitData2.SerialiseL(data);

    TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries>  ltsyData(name, indexAndEntries);
	data1.Close();
	ltsyData.SerialiseL(data1);
    
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreReadIPC, data1);
    iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);

    bookStore.Read(requestStatus, index, numEntries, buf); 
        
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreReadIPC, KErrNone, completeData, 0);
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);
        
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());   

    //
    
    CleanupStack::PopAndDestroy(8, this); 
    
    }

void CCTsyPhoneBookStoreFU::SetPhonebookNumOfEntries( CStorageInfoData& aStorageData, TInt aNumOfEntries )
    {
    //bdn num entries set to 0
    aStorageData.iADNNumOfEntries = aNumOfEntries;
    aStorageData.iFDNNumOfEntries = aNumOfEntries;
    aStorageData.iSDNNumOfEntries = aNumOfEntries;
    aStorageData.iVMBXNumOfEntries = aNumOfEntries;
    aStorageData.iMBDNNumOfEntries = aNumOfEntries; 
    }

void CCTsyPhoneBookStoreFU::SetPhonebookNumOfEntriesAndInitPhonebookL( TName aName, TInt aNumOfEntries )
    {
	RBuf8 data;
 	CleanupClosePushL(data);
    
    //storeInit
    CStorageInfoData storageData;       
    //bdn num entries set to 0
    storageData.iADNNumOfEntries = aNumOfEntries;
    storageData.iFDNNumOfEntries = aNumOfEntries;
    storageData.iSDNNumOfEntries = aNumOfEntries;
    storageData.iVMBXNumOfEntries = aNumOfEntries;
    storageData.iMBDNNumOfEntries = aNumOfEntries;     
        
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreAndName(aName, storageData); 
	data.Close();    
	retStoreAndName.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data, 0);
    User::WaitForRequest(mockLtsyStatus);        

    CleanupStack::PopAndDestroy( &data );
    }

void CCTsyPhoneBookStoreFU::FillPBEntry(CPhoneBookStoreEntry* aEntryPtr)
	{
	_LIT(KPBText, "PBText");
	_LIT(KPBNumber, "PBNumber");
	_LIT(KPBEmailAddress, "PBEmailAddress");
	_LIT(KPBSecondName, "PBSecondName");
	
	aEntryPtr->iLocation = 1;
	
	aEntryPtr->iText = HBufC::NewLC( KPBText().Length() );
	*(aEntryPtr->iText) = KPBText();
	CleanupStack::Pop();

	aEntryPtr->iNumber = HBufC::NewLC( KPBNumber().Length() );	
	*(aEntryPtr->iNumber) = KPBNumber();
	CleanupStack::Pop();

	aEntryPtr->iEmail->AppendL( KPBEmailAddress() );

	aEntryPtr->iSne->AppendL( KPBSecondName() );
	}
