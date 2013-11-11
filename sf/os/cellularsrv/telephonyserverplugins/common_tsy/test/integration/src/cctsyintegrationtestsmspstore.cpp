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
// Test step definitions for the SMSPStore functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsmspstore.h"
#include "listretrievalhelpers.h"

	
CCTSYIntegrationTestSMSPStoreBase::CCTSYIntegrationTestSMSPStoreBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iSmsMessagingTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSMSPStoreBase::~CCTSYIntegrationTestSMSPStoreBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestSMSPStore0001::CCTSYIntegrationTestSMSPStore0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0001::GetTestStepName());
	}
	
	
TBool CCTSYIntegrationTestSMSPStoreBase::CompareEntries(const RMobileSmsMessaging::TMobileSmspEntryV1 &aEntry1,
		const RMobileSmsMessaging::TMobileSmspEntryV1 &aEntry2)
/*
 * This function copares the previously stored enteries
 *
 * @param aEntry1 and aEntry2 reference the stored entries before and after retrival of them.
 * @return true if the comparison of enteries is successful.
 *
 */
	{
	TBool ret=ETrue;
	
	// Making Comparison with respect to Valid Parameter returned by the board
	if(aEntry2.iValidParams & RMobileSmsMessaging::KSCAIncluded)
		{
		// Check CRetrieveMobilePhoneSmspList::Start returned correct service centre details
		ASSERT_EQUALS_DES16(aEntry1.iServiceCentre.iTelNumber, aEntry2.iServiceCentre.iTelNumber, _L("CRetrieveMobilePhoneSmspList::Start did not return correct service centre telephone number "));
		ASSERT_EQUALS(aEntry1.iServiceCentre.iNumberPlan, aEntry2.iServiceCentre.iNumberPlan, _L("CRetrieveMobilePhoneSmspList::Start did not return correct service centre number plan "));
		ASSERT_EQUALS(aEntry1.iServiceCentre.iTypeOfNumber, aEntry2.iServiceCentre.iTypeOfNumber, _L("CRetrieveMobilePhoneSmspList::Start did not return correct service centre type of number "));
		ASSERT_EQUALS(aEntry1.iIndex, aEntry2.iIndex, _L("Indexes are diffrent "));
		if(aEntry1.iServiceCentre.iTelNumber!=aEntry2.iServiceCentre.iTelNumber
				|| aEntry1.iServiceCentre.iNumberPlan!=aEntry2.iServiceCentre.iNumberPlan
				|| aEntry1.iServiceCentre.iTypeOfNumber!=aEntry2.iServiceCentre.iTypeOfNumber)
				ret=EFalse;	
		}
		
	if(aEntry2.iValidParams & RMobileSmsMessaging::KDestinationIncluded)
		{
		// Check CRetrieveMobilePhoneSmspList::Start returned correct destination details
		ASSERT_EQUALS_DES16(aEntry1.iDestination.iTelNumber, aEntry2.iDestination.iTelNumber, _L("CRetrieveMobilePhoneSmspList::Start did not return correct destination telephone number"));
		ASSERT_EQUALS(aEntry1.iDestination.iNumberPlan, aEntry2.iDestination.iNumberPlan, _L("CRetrieveMobilePhoneSmspList::Start did not return correct destination number plan"));
		ASSERT_EQUALS(aEntry1.iDestination.iTypeOfNumber, aEntry2.iDestination.iTypeOfNumber, _L("CRetrieveMobilePhoneSmspList::Start did not return correct destination type of number"));
		if(aEntry1.iDestination.iTelNumber!=aEntry2.iDestination.iTelNumber
			|| aEntry1.iDestination.iNumberPlan!=aEntry2.iDestination.iNumberPlan
			|| aEntry1.iDestination.iTypeOfNumber!=aEntry2.iDestination.iTypeOfNumber)
			ret=EFalse;	
		}
		
	if(aEntry2.iValidParams & RMobileSmsMessaging::KProtocolIdIncluded)
		{	
		// Check CRetrieveMobilePhoneSmspList::Start returned correct protocolId details
		ASSERT_EQUALS(aEntry1.iProtocolId,aEntry2.iProtocolId, _L("CRetrieveMobilePhoneSmspList::Start did not return correct protocolId"));	
		if(aEntry1.iProtocolId!=aEntry2.iProtocolId)
			ret=EFalse;
		}	
	
	if(aEntry2.iValidParams & RMobileSmsMessaging::KDcsIncluded)
		{
		// Check CRetrieveMobilePhoneSmspList::Start returned correct Dcs
		ASSERT_EQUALS(aEntry1.iDcs,aEntry2.iDcs, _L("CRetrieveMobilePhoneSmspList::Start did not return correct Dcs"));	
		if(aEntry1.iDcs!=aEntry2.iDcs)
			ret=EFalse;
		}
	
	if(aEntry2.iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded)
		{
		// Check CRetrieveMobilePhoneSmspList::Start returned correct Validity Period
		ASSERT_EQUALS(aEntry1.iValidityPeriod, aEntry2.iValidityPeriod, _L("CRetrieveMobilePhoneSmspList::Start did not return correct validity period"));	
		if(aEntry1.iValidityPeriod!=aEntry2.iValidityPeriod)
			ret=EFalse;
		}	
	return ret;
	
	}


CCTSYIntegrationTestSMSPStore0001::~CCTSYIntegrationTestSMSPStore0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0001
 * @SYMFssID BA/CTSY/SMSP-0001
 * @SYMTestCaseDesc Store and retrieve a SMSP list.
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::StoreSmspListL, CRetrieveMobilePhoneSmspList::Start, CMobilePhoneSmspList::NewL, CRetrieveMobilePhoneSmspList::NewL, CMobilePhoneSmspList::AddEntryL, RMobileSmsMessaging::NotifySmspListChange
 * @SYMTestExpectedResults Pass - SMSP list stored and retrieved correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify list can be stored and retrieved list contains correct entries.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get the RMobileSmsMessaging subsession
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);
	
	// Ensure RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of KCapsSetSmspList | KCapsGetSmspList 
	RMobileSmsMessaging::TMobileSmsCapsV1  smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	CHECK_EQUALS_L(smsMessaging.GetCaps(smsCapsPckg),KErrNone, _L("RMobileSmsMessaging::GetCaps returned returned with an error"));
	TUint32 expectedCaps= RMobileSmsMessaging::KCapsSetSmspList | RMobileSmsMessaging::KCapsGetSmspList;
	CHECK_BITS_SET_L(smsCaps.iSmsControl, expectedCaps, KNoUnwantedBits, _L("RMobileSmsMessaging::GetCaps returned with unexpected caps"));
	
	// Take the backup of the SMSP list. 
	TInt listCountAtStart;
	TBackupList backup(*this,smsMessaging);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupList::Backup could not backup"));
	CleanupStack::PushL (backup);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Store SMSP list ===

	// Create a CMobilePhoneSmspList with CMobilePhoneSmspList::NewL 
   	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList); 
	
	// Create a RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;

	     smspEntry.iText = _L("VODAFONE"); 
	 	 
	     smspEntry.iValidParams = RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 


	     smspEntry.iDestination.iTelNumber = _L(""); 
		
	     smspEntry.iDestination.iNumberPlan = RMobilePhone::EUnknownNumberingPlan; 
		
	     smspEntry.iDestination.iTypeOfNumber = RMobilePhone::EUnknownNumber; 
		
	     smspEntry.iValidityPeriod = 183; 
	
	     smspEntry.iServiceCentre.iTelNumber = _L("+441632960000"); 
	
	     smspEntry.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 
	
	     smspEntry.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iProtocolId = 0; 

	     smspEntry.iDcs = 0; 

	     smspEntry.iIndex = 1; 

	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry);
	
	// Post notification for RMobileSmsMessaging::NotifySmspListChange
	TExtEtelRequestStatus notifySmspListChangeStatus(smsMessaging, EMobileSmsMessagingNotifySmspListChange);
	CleanupStack::PushL(notifySmspListChangeStatus);
	smsMessaging.NotifySmspListChange(notifySmspListChangeStatus);

	// This is deliberate action to open a session because the API in Etelmm allocates memory
	// The Session Manager should be used in all other places.
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobileSmsMessaging smsMessagingStore;
	User::LeaveIfError(smsMessagingStore.Open(phone));
	
	// Store the empty SMSP list with RMobileSmsMessaging::StoreSmspListL 
	TExtEtelRequestStatus storeSmspListStatus(smsMessaging, EMobileSmsMessagingStoreSmspList);
	CleanupStack::PushL(storeSmspListStatus);
	smsMessagingStore.StoreSmspListL(storeSmspListStatus,smspList);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeSmspListStatus, ETimeMedium),KErrNone, _L("RMobileSmsMessaging::StoreSmspListL timed out"));
	ASSERT_EQUALS(storeSmspListStatus.Int(), KErrNone, _L("RMobileSmsMessaging::StoreSmspListL returned with an error"));		
	smsMessagingStore.Close();
	
	// Check RMobileSmsMessaging::NotifySmspListChange completes with KErrNone		
	TInt expectedStatus = KErrNone;
	iSmsMessagingTestHelper.WaitForMobileSmsMessagingNotifySmspListChange(notifySmspListChangeStatus,expectedStatus);
		
	// ===  Retrieve list and check it is the same as the stored list ===

	// Use the helper class for CRetrieveMobilePhoneSmspList to retrieve the SMSP list. 
	TInt err=0;
	CRetrieveMobilePhoneSmspListExec* smspListExec = CRetrieveMobilePhoneSmspListHelper::NewL(smsMessaging);	
	CleanupStack::PushL(smspListExec);
	CMobilePhoneSmspList* mobileSmspList = smspListExec->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneSmspList::Start returned with an error"));

 	//	Check CRetrieveMobilePhoneSmspList::Enumerate returns 1 using the helper class function
	TInt listEntries=mobileSmspList->Enumerate();
	if(listCountAtStart==0)
		{
		ASSERT_EQUALS(listEntries, 1, _L("CRetrieveMobilePhoneSmspList::Enumerate did not return one entry as expected"));
		}
   	else
   		{
   		ASSERT_EQUALS(listCountAtStart,listEntries, _L("CRetrieveMobilePhoneSmspList::Enumerate did not return number of entries as expected"));
   		}
  
   	// 	Check the entry in the CMobilePhoneSmspList is the same the entry stored
   	RMobileSmsMessaging::TMobileSmspEntryV1 item;
   	item=mobileSmspList->GetEntryL(0); // zero is the first index in retrieval list
   	ASSERT_TRUE(CompareEntries(item,smspEntry),_L("Comparison of stroed enteries failed"));
	  	 
   	//
	// TEST END
	//
		   	
    StartCleanup();
   
    // Pop :
    // backup
    // smspList
    // notifySmspListChangeStatus
	// storeSmspListStatus
	// smspListExec;
    CleanupStack::PopAndDestroy(5, &backup);
      
    return TestStepResult();
	
	}

TPtrC CCTSYIntegrationTestSMSPStore0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0001");
	}



CCTSYIntegrationTestSMSPStore0002::CCTSYIntegrationTestSMSPStore0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0002::GetTestStepName());
	}

CCTSYIntegrationTestSMSPStore0002::~CCTSYIntegrationTestSMSPStore0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0002
 * @SYMFssID BA/CTSY/SMSP-0002
 * @SYMTestCaseDesc Store an SMSP list with multiple entries.
 * @SYMTestPriority High
 * @SYMTestActions This requires an O2 SIM or anyother, which can store multiple entries in SMSP List RMobileSmsMessaging::StoreSmspListL, CRetrieveMobilePhoneSmspList::RetrieveListL
 * @SYMTestExpectedResults Pass - SMSP list stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify list can be stored and retrieved list contains correct entries.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
		
	// Get the RMobileSmsMessaging subsession
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);
	
	// Ensure RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of KCapsSetSmspList | KCapsGetSmspList 
	RMobileSmsMessaging::TMobileSmsCapsV1  smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	CHECK_EQUALS_L(smsMessaging.GetCaps(smsCapsPckg),KErrNone, _L("RMobileSmsMessaging::GetCaps returned returned with an error"));
	TUint32 expectedCaps= RMobileSmsMessaging::KCapsSetSmspList | RMobileSmsMessaging::KCapsGetSmspList;
	CHECK_BITS_SET_L(smsCaps.iSmsControl, expectedCaps, KNoUnwantedBits, _L("RMobileSmsMessaging::GetCaps returned with unexpected caps"));

	// Take the backup of SMSP list. 
	TInt listCountAtStart;
	TBackupList backup(*this,smsMessaging);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupList::Backup could not backup"));
	CleanupStack::PushL (backup);
	
	//
	// SET UP END
	//
		
	StartTest();
	
	//
	// TEST START
	//
		
	TBuf<130> prompt(_L("You need to insert an O2 SIM before running that test, because O2 SIM can store multiple(3) entries in the list"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	// ===  Store SMSP list ===

	// Create a CMobilePhoneSmspList with CMobilePhoneSmspList::NewL 
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
	
	// Create a RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
	     
	     smspEntry.iText = _L("ABCD"); 

	     smspEntry.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 

	     smspEntry.iDestination.iTelNumber = _L("+1234567890"); 

	     smspEntry.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	     smspEntry.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iValidityPeriod = 0; 

	     smspEntry.iServiceCentre.iTelNumber = _L("+123"); 

	     smspEntry.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	     smspEntry.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iProtocolId = 2; 

	     smspEntry.iDcs = 4; 

	     smspEntry.iIndex = 1; 
	
	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry);

	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry2;
		
		smspEntry2.iText = _L("EFGH");
		
		smspEntry2.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 
	    
	    smspEntry2.iDestination.iTelNumber = _L("+987654"); 

	    smspEntry2.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry2.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry2.iValidityPeriod = 0; 

	  	smspEntry2.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber; 

	  	smspEntry2.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan; 

	  	smspEntry2.iServiceCentre.iTelNumber=_L("+456"); 
	  	
	  	smspEntry2.iProtocolId = 2; 

	    smspEntry2.iDcs = 4; 

	    smspEntry2.iIndex = 2; 

	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry2);
		
	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry3;
	  	
	  	smspEntry3.iText = _L("IJKL"); 

        smspEntry3.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 

        smspEntry3.iDestination.iTelNumber = _L("+12456"); 

	    smspEntry3.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry3.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry3.iValidityPeriod = 0; 

	    smspEntry3.iServiceCentre.iTelNumber = _L("+789"); 

	    smspEntry3.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry3.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry3.iProtocolId = 2; 

	    smspEntry3.iDcs = 4; 

	   	smspEntry3.iIndex = 3; 
	     
	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry3);

	// Post notification for RMobileSmsMessaging::NotifySmspListChange
	TExtEtelRequestStatus notifySmspListChangeStatus(smsMessaging, EMobileSmsMessagingNotifySmspListChange);
	CleanupStack::PushL(notifySmspListChangeStatus);
	smsMessaging.NotifySmspListChange(notifySmspListChangeStatus);
	
	// Store the empty SMSP list with RMobileSmsMessaging::StoreSmspListL 
	TExtEtelRequestStatus storeSmspListStatus(smsMessaging, EMobileSmsMessagingStoreSmspList);
	CleanupStack::PushL(storeSmspListStatus);
	smsMessaging.StoreSmspListL(storeSmspListStatus,smspList);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeSmspListStatus, ETimeMedium),KErrNone, _L("RMobileSmsMessaging::StoreSmspListL timed out"));
	ASSERT_EQUALS(storeSmspListStatus.Int(), KErrNone, _L("RMobileSmsMessaging::StoreSmspListL returned with an error"));		
	
	// Check RMobileSmsMessaging::NotifySmspListChange completes with KErrNone		
	TInt expectedStatus = KErrNone;
	iSmsMessagingTestHelper.WaitForMobileSmsMessagingNotifySmspListChange(notifySmspListChangeStatus,expectedStatus);
	
	// ===  Retrieve list and check it is the same as the stored list ===

	// Use the helper class for CRetrieveMobilePhoneSmspList to retrieve the SMSP list. 
	TInt err=0;
	CRetrieveMobilePhoneSmspListExec* smspListExec = CRetrieveMobilePhoneSmspListHelper::NewL(smsMessaging);	
	CleanupStack::PushL(smspListExec);
	CMobilePhoneSmspList* mobileSmspList = smspListExec->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneSmspList::Start returned with an error"));

	// Check CRetrieveMobilePhoneSmspList::Enumerate returns 3 using the helper class
 	TInt listEnteries=mobileSmspList->Enumerate();
	ASSERT_EQUALS(listEnteries, 3, _L("CRetrieveMobilePhoneSmspList::Enumerate did not return the correct number of enteries"));
	
	// Check the entry in the CMobilePhoneSmspList is the same the entry stored
   	RMobileSmsMessaging::TMobileSmspEntryV1 item;
   	RMobileSmsMessaging::TMobileSmspEntryV1 tempSmspEntry;
	for(TInt i=0;i<listEnteries;i++)
		{
		item=mobileSmspList->GetEntryL(i);
		if(i==0)
		tempSmspEntry=smspEntry;
		else if(i==1)
		tempSmspEntry=smspEntry2;
		else
		tempSmspEntry=smspEntry3;
		ASSERT_TRUE(CompareEntries(tempSmspEntry,item),_L("Comparison of stroed enteries failed"));
		}
	
	//
	// TEST END
	//

    StartCleanup();
    
    // Pop:
    // backup
	// smspListExec;
 	// storeSmspListStatus
	// notifySmspListChangeStatus);	
	// smspList
    CleanupStack::PopAndDestroy(5, &backup);
    	
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSPStore0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0002");
	}

CCTSYIntegrationTestSMSPStore0003::CCTSYIntegrationTestSMSPStore0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0003::GetTestStepName());
	}

CCTSYIntegrationTestSMSPStore0003::~CCTSYIntegrationTestSMSPStore0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0003
 * @SYMFssID BA/CTSY/SMSP-0003
 * @SYMTestCaseDesc Store an empty SMSP list.
 * @SYMTestPriority High
 * @SYMTestActions CMobilePhoneSmspList::NewL, CRetrieveMobilePhoneSmspList::NewL, RMobileSmsMessaging::StoreSmspListL, CRetrieveMobilePhoneSmspList::Start
 * @SYMTestExpectedResults Pass - Empty SMSP list is not stored and retrieved list is not empty.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get the RMobileSmsMessaging subsession
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);

	// Ensure RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of KCapsSetSmspList | KCapsGetSmspList 
	RMobileSmsMessaging::TMobileSmsCapsV1  smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	CHECK_EQUALS_L(smsMessaging.GetCaps(smsCapsPckg),KErrNone, _L("RMobileSmsMessaging::GetCaps returned returned with an error"));
	TUint32 expectedCaps= RMobileSmsMessaging::KCapsSetSmspList | RMobileSmsMessaging::KCapsGetSmspList;
	CHECK_BITS_SET_L(smsCaps.iSmsControl, expectedCaps, KNoUnwantedBits, _L("RMobileSmsMessaging::GetCaps returned with unexpected caps"));
		
	//
	// SET UP END
	//

	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Store empty list ===
	
	// Create a CMobilePhoneSmspList with CMobilePhoneSmspList::NewL 
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
		
	// Post notification for RMobileSmsMessaging::NotifySmspListChange
	TExtEtelRequestStatus notifySmspListChangeStatus(smsMessaging, EMobileSmsMessagingNotifySmspListChange);
	CleanupStack::PushL(notifySmspListChangeStatus);
	smsMessaging.NotifySmspListChange(notifySmspListChangeStatus);

	// This is deliberate action to open a session because the API in Etelmm allocates memory
	// The Session Manager should be used in all other places.
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobileSmsMessaging smsMessagingStore;
 	smsMessagingStore.Open(phone);
 		
	// Store the empty SMSP list with RMobileSmsMessaging::StoreSmspListL cause Error 
	TExtEtelRequestStatus storeSmspListStatus(smsMessaging, EMobileSmsMessagingStoreSmspList);
	CleanupStack::PushL(storeSmspListStatus);
	smsMessaging.StoreSmspListL(storeSmspListStatus,smspList);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeSmspListStatus, ETimeMedium),KErrNone, _L("RMobileSmsMessaging::StoreSmspListL timed out"));
	ASSERT_TRUE(storeSmspListStatus.Int()!= KErrNone, _L("RMobileSmsMessaging::StoreSmspListL did not return an error"))		
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10012);
	smsMessagingStore.Close();
	
	// Check RMobileSmsMessaging::NotifySmspListChange completes with KErrNone
	TInt expectedStatus = KErrNone;
	iSmsMessagingTestHelper.WaitForMobileSmsMessagingNotifySmspListChange(notifySmspListChangeStatus,expectedStatus);
	
	// ===  Retrieve list and check it is the same as the stored list ===

	// Use the helper class for CRetrieveMobilePhoneSmspList to retrieve the SMSP list. 
	TInt err=0;
	CRetrieveMobilePhoneSmspListExec* smspListExec = CRetrieveMobilePhoneSmspListHelper::NewL(smsMessaging);	
	CleanupStack::PushL(smspListExec);
	CMobilePhoneSmspList* mobileSmspList = smspListExec->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneSmspList::Start returned with an error"));

	// Check CMobilePhoneSmspList::Enumerate on the list returned returns value bigger than 0
	TInt listEnteries=mobileSmspList->Enumerate();
	ASSERT_TRUE(listEnteries> 0, _L("CRetrieveMobilePhoneSmspList::Enumerate returned an empty list"))
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop 
	// smspListExec;
 	// storeSmspListStatus
	// notifySmspListChangeStatus);	
	// smspList

    CleanupStack::PopAndDestroy(4, smspList);
   	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSPStore0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0003");
	}



CCTSYIntegrationTestSMSPStore0004::CCTSYIntegrationTestSMSPStore0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0004::GetTestStepName());
	}

CCTSYIntegrationTestSMSPStore0004::~CCTSYIntegrationTestSMSPStore0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0004
 * @SYMFssID BA/CTSY/SMSP-0004
 * @SYMTestCaseDesc Cancel reading SMSP list entries.
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneSmspList::Start, CRetrieveMobilePhoneSmspList::Cancel
 * @SYMTestExpectedResults Pass - Request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get the RMobileSmsMessaging subsession
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);
	
	// Ensure RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of KCapsSetSmspList | KCapsGetSmspList 
	RMobileSmsMessaging::TMobileSmsCapsV1  smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	CHECK_EQUALS_L(smsMessaging.GetCaps(smsCapsPckg),KErrNone, _L("RMobileSmsMessaging::GetCaps returned returned with an error"));
	TUint32 expectedCaps= RMobileSmsMessaging::KCapsSetSmspList | RMobileSmsMessaging::KCapsGetSmspList;
	CHECK_BITS_SET_L(smsCaps.iSmsControl, expectedCaps, KNoUnwantedBits, _L("RMobileSmsMessaging::GetCaps returned with unexpected caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Use helper class for CRetrieveMobilePhoneSmspList to retrieve the SMSP list 
	TInt err=0;
	CRetrieveMobilePhoneSmspListExec* smspListExec = CRetrieveMobilePhoneSmspListHelper::NewL(smsMessaging);
	CleanupStack::PushL(smspListExec);
	
	// Cancel list retrieval  
	CMobilePhoneSmspList* smspList = smspListExec->DoGetList(err,ECancelAfterStart);
	
	// Check retrieval completes with KErrCancel 
	ASSERT_EQUALS(err, KErrCancel, _L("CRetrieveMobilePhoneSmspList::Start Request could not be cancelled properly"));
		
	//
	// TEST END
	//

    StartCleanup();
    CleanupStack::PopAndDestroy(smspListExec);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSPStore0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0004");
	}



CCTSYIntegrationTestSMSPStore0005::CCTSYIntegrationTestSMSPStore0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0005::GetTestStepName());
	}

CCTSYIntegrationTestSMSPStore0005::~CCTSYIntegrationTestSMSPStore0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0005
 * @SYMFssID BA/CTSY/SMSP-0005
 * @SYMTestCaseDesc Cancel writing SMSP list entries.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get the RMobileSmsMessaging subsession
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);
	
	// Ensure RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of KCapsSetSmspList | KCapsGetSmspList 
	RMobileSmsMessaging::TMobileSmsCapsV1  smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	CHECK_EQUALS_L(smsMessaging.GetCaps(smsCapsPckg),KErrNone, _L("RMobileSmsMessaging::GetCaps returned returned with an error"));
	TUint32 expectedCaps= RMobileSmsMessaging::KCapsSetSmspList | RMobileSmsMessaging::KCapsGetSmspList;
	CHECK_BITS_SET_L(smsCaps.iSmsControl, expectedCaps, KNoUnwantedBits, _L("RMobileSmsMessaging::GetCaps returned with unexpected caps"));
	
	// Take the backup of SMSP list
	TInt listCountAtStart;
	TBackupList backup(*this,smsMessaging);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupList::Backup Could not backup"));
	CleanupStack::PushL (backup);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Create a CMobilePhoneSmspList with CMobilePhoneSmspList::NewL 
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
		
	// Find out the network
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	TExtEtelRequestStatus getNetworkStatus (phone,EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeMedium),   KErrNone,
	 			_L("MobilePhone::GetCurrentNetwork timed out"))
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, 
				_L("MobilePhone::GetCurrentNetwork returned an error"))	
				
	// Create a RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
	
	     smspEntry.iText = _L("ABCD"); 

	     smspEntry.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 

	     smspEntry.iDestination.iTelNumber = _L("1234567890"); 

	     smspEntry.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	     smspEntry.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iValidityPeriod = 0; 

	     smspEntry.iServiceCentre.iTelNumber = _L("1234567890"); 

	     smspEntry.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	     smspEntry.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iProtocolId = 2; 

	     smspEntry.iDcs = 4; 

	     smspEntry.iIndex = 1; 
	    	     
	
	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry2;

	  smspEntry2.iValidParams = RMobileSmsMessaging::KSCAIncluded; 

	  smspEntry2.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber; 

	  smspEntry2.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan; 

	  smspEntry2.iServiceCentre.iTelNumber=_L("441632960000"); 
	  
	  smspEntry2.iIndex = 2; 

	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry3;
	  
	  smspEntry3.iValidParams = RMobileSmsMessaging::KSCAIncluded; 

	  smspEntry3.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber; 

	  smspEntry3.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan; 

	  smspEntry3.iServiceCentre.iTelNumber=_L("441632960000"); 

	  smspEntry3.iIndex = 3; 
	
	
	if(info.iShortName.Find(_L("O2")) != KErrNotFound)	//O2
		{
		DEBUG_PRINTF1(_L("O2 can have three entries in the SMSP list"));
		
		// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
		smspList->AddEntryL(smspEntry);
		smspList->AddEntryL(smspEntry2);
		smspList->AddEntryL(smspEntry3);
		}
	else
		{
		smspList->AddEntryL(smspEntry);	
		}
		
	// Store the empty SMSP list with RMobileSmsMessaging::StoreSmspListL
	TExtEtelRequestStatus storeSmspListStatus(smsMessaging, EMobileSmsMessagingStoreSmspList);
	CleanupStack::PushL(storeSmspListStatus);
	smsMessaging.StoreSmspListL(storeSmspListStatus,smspList);
	
	// Cancel request immediately with RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingStoreSmspList) 
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingStoreSmspList);

	// Check store request completes with KErrCancel or KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeSmspListStatus, ETimeShort),KErrNone, _L("RMobileSmsMessaging::StoreSmspListL timed out"));
	ASSERT_TRUE(storeSmspListStatus.Int() == KErrCancel || storeSmspListStatus.Int() == KErrNone, 
				_L("RMobileSmsMessaging:: CancelAsyncRequest could not cancel the request EMobileSmsMessagingStoreSmspList"));
	
	//
	// TEST END
	//

	StartCleanup();
    // Pop:
	// backup
  	// storeSmspListStatus
    // getNetworkStatus
    // smspList
    CleanupStack::PopAndDestroy(4,&backup);	
  
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSPStore0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0005");
	}



CCTSYIntegrationTestSMSPStore0006::CCTSYIntegrationTestSMSPStore0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0006::GetTestStepName());
	}

CCTSYIntegrationTestSMSPStore0006::~CCTSYIntegrationTestSMSPStore0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0006
 * @SYMFssID BA/CTSY/SMSP-0006
 * @SYMTestCaseDesc Cancel notification of change in SMSP list entries.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get the RMobileSmsMessaging subsession 
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Register for notification of SMSP list change with RMobileSmsMessaging::NotifySmspListChange 
	TExtEtelRequestStatus notifySmspListChangeStatus(smsMessaging, EMobileSmsMessagingNotifySmspListChange);
	CleanupStack::PushL(notifySmspListChangeStatus);
	smsMessaging.NotifySmspListChange(notifySmspListChangeStatus);
	
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingNotifySmspListChange) 
 	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingNotifySmspListChange);
 	
 	// Check notification request completes with KErrCancel
 	TInt expectedStatus = KErrCancel;
 	iSmsMessagingTestHelper.WaitForMobileSmsMessagingNotifySmspListChange(notifySmspListChangeStatus,expectedStatus);
	
	//
	// TEST END
	//

    StartCleanup();
	CleanupStack::PopAndDestroy(&notifySmspListChangeStatus);	

	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSPStore0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0006");
	}



CCTSYIntegrationTestSMSPStore0007::CCTSYIntegrationTestSMSPStore0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSMSPStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSMSPStore0007::GetTestStepName());
	}

CCTSYIntegrationTestSMSPStore0007::~CCTSYIntegrationTestSMSPStore0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSMSPStore0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSP-0007
 * @SYMFssID BA/CTSY/SMSP-0007
 * @SYMTestCaseDesc Store an SMSP list that is too long.
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::StoreSmspListL, CRetrieveMobilePhoneSmspList::Start, CMobilePhoneSmspList::NewL, CMobilePhoneSmspList::AddEntryL
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on storing.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get the RMobileSmsMessaging subsession 
	RMobileSmsMessaging& smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer,KMainPhone,KMainSmsMessaging);

	// Ensure RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of KCapsSetSmspList | KCapsGetSmspList 
	RMobileSmsMessaging::TMobileSmsCapsV1  smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	CHECK_EQUALS_L(smsMessaging.GetCaps(smsCapsPckg),KErrNone, _L("RMobileSmsMessaging::GetCaps returned returned with an error"));
	TUint32 expectedCaps= RMobileSmsMessaging::KCapsSetSmspList | RMobileSmsMessaging::KCapsGetSmspList;
	CHECK_BITS_SET_L(smsCaps.iSmsControl, expectedCaps, KNoUnwantedBits, _L("RMobileSmsMessaging::GetCaps returned with unexpected caps"));

	// Take the backup of the SMSP list. 
	TInt listCountAtStart;
	TBackupList backup(*this,smsMessaging);
	CHECK_TRUE_L(backup.BackupL(listCountAtStart), _L("TBackupList::Backup Could not backup"));
	CleanupStack::PushL (backup);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Store SMSP list ===

	// Create a CMobilePhoneSmspList with CMobilePhoneSmspList::NewL 
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
	
	// Create a RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
	     
	     smspEntry.iText = _L("ABCD"); 

	     smspEntry.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 

	     smspEntry.iDestination.iTelNumber = _L("+1234567890"); 

	     smspEntry.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	     smspEntry.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iValidityPeriod = 0; 

	     smspEntry.iServiceCentre.iTelNumber = _L("+123"); 

	     smspEntry.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	     smspEntry.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	     smspEntry.iProtocolId = 2; 

	     smspEntry.iDcs = 4; 

	     smspEntry.iIndex = 1; 
	
	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry);

	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry2;
		
		smspEntry2.iText = _L("EFGH");
		
		smspEntry2.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 
	    
	    smspEntry2.iDestination.iTelNumber = _L("+987654"); 

	    smspEntry2.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry2.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry2.iValidityPeriod = 0; 

	  	smspEntry2.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber; 

	  	smspEntry2.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan; 

	  	smspEntry2.iServiceCentre.iTelNumber=_L("+456"); 
	  	
	  	smspEntry2.iProtocolId = 2; 

	    smspEntry2.iDcs = 4; 

	    smspEntry2.iIndex = 2; 

	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry2);
		
	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry3;
	  	
	  	smspEntry3.iText = _L("IJKL"); 

        smspEntry3.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 

        smspEntry3.iDestination.iTelNumber = _L("+12456"); 

	    smspEntry3.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry3.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry3.iValidityPeriod = 0; 

	    smspEntry3.iServiceCentre.iTelNumber = _L("+789"); 

	    smspEntry3.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry3.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry3.iProtocolId = 2; 

	    smspEntry3.iDcs = 4; 

	   	smspEntry3.iIndex = 3; 
	     
	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry3);
	
	// Create another RMobileSmsMessaging::TMobileSmspEntryV1 entry with the following parameters: 
	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry4;
	  	
	  	smspEntry4.iText = _L("MNOP"); 

        smspEntry4.iValidParams = RMobileSmsMessaging::KDestinationIncluded 

	         | RMobileSmsMessaging::KSCAIncluded 

	         | RMobileSmsMessaging::KProtocolIdIncluded 

	         | RMobileSmsMessaging::KDcsIncluded 

	         | RMobileSmsMessaging::KValidityPeriodIncluded; 

        smspEntry4.iDestination.iTelNumber = _L("+12456"); 

	    smspEntry4.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry4.iDestination.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry4.iValidityPeriod = 0; 

	    smspEntry4.iServiceCentre.iTelNumber = _L("+789"); 

	    smspEntry4.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan; 

	    smspEntry4.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber; 

	    smspEntry4.iProtocolId = 2; 

	    smspEntry4.iDcs = 4; 

	   	smspEntry4.iIndex = 4; 
	     
	// Add the entry to the CMobilePhoneSmspList list with CMobilePhoneSmspList::AddEntryL 
	smspList->AddEntryL(smspEntry4);

	// Store the empty SMSP list with RMobileSmsMessaging::StoreSmspListL 
	TExtEtelRequestStatus storeSmspListStatus(smsMessaging, EMobileSmsMessagingStoreSmspList);
	CleanupStack::PushL(storeSmspListStatus);
	smsMessaging.StoreSmspListL(storeSmspListStatus,smspList);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(storeSmspListStatus, ETimeMedium),KErrNone, _L("RMobileSmsMessaging::StoreSmspListL timed out"));
	
	// Check KErrArgument is returned
	ASSERT_EQUALS(storeSmspListStatus.Int(), KErrArgument, _L("RMobileSmsMessaging::StoreSmspListL did not return with an error"));		
	
	//
	// TEST END
	//

    StartCleanup();
    
	// Pop:
	// backup
    // storeSmspListStatus
    // smspList
	CleanupStack::PopAndDestroy(3,&backup);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSMSPStore0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSMSPStore0007");
	}

TBackupList::TBackupList(CCTSYIntegrationTestSuiteStepBase &aTestStep,
			RMobileSmsMessaging &aSmsMessaging) : TTsyTestHelperBase(aTestStep) ,iSmsMessaging(aSmsMessaging)
/*
 * Constructor
 * @param aBase pointer to the test.
 * @param aSmsMessaging reference to a SMSP session
 *
 */
{
}
TBool TBackupList::BackupL(TInt &aListCount)
/*
 * This function takes backup of the entries of SMSP list and return false if its fails
 * @param aListCount refer to the number of entries in the list at the time of backup
 * @return true if the backup was successful
 *
 */	
	{
	iValid=EFalse;

	// Enumerater the entries of SMSP List
	TInt err=0;
	CRetrieveMobilePhoneSmspListExec* smspListExec = CRetrieveMobilePhoneSmspListHelper::NewL(iSmsMessaging);	
	CleanupStack::PushL(smspListExec);
	CMobilePhoneSmspList* retrievedSmspList = smspListExec->DoGetList(err);
	if(err== KErrNone)
		{
		iValid=ETrue;
		}
	else
		{
		return iValid;
		}
	iListCount=retrievedSmspList->Enumerate();
	aListCount=iListCount;
	
	// Store the values of APN control list, so that they can be restored later
	for(TInt i=0;i<aListCount;i++)
		{
		RMobileSmsMessaging::TMobileSmspEntryV1 item;
	   	item=retrievedSmspList->GetEntryL(i); // zero is the first index in retrieval list
	   	iEntryArray.Append(item);
		}
	CleanupStack::PopAndDestroy(smspListExec);
	
	return iValid;
	}
void TBackupList::DoCleanupL(TAny* aPtr)
/*
 * This function is a static clean up function to be saved in the cleanup stack. Its restore the saved entries.
 *
 * @param aPtr a pointer to a TBackupList instance.
 *
 */	
	{
	TBackupList* me = static_cast<TBackupList*>(aPtr);
	me->CleanupL();
	}
void TBackupList::CleanupL()
/*
 * This function restore the saved entries to the entries in the phone
 *
 */	
	{
	CMobilePhoneSmspList* restoreSmspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(restoreSmspList); 
	
	TExtEtelRequestStatus storeSmspListStatus(iSmsMessaging, EMobileSmsMessagingStoreSmspList);
	CleanupStack::PushL(storeSmspListStatus);
	
	for(int j=0; j<iListCount;j++)
		{
		RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry= iEntryArray[j];
		restoreSmspList->AddEntryL(smspEntry);
		}
	iSmsMessaging.StoreSmspListL(storeSmspListStatus,restoreSmspList);
	
	
	if(iTestStep.WaitForRequestWithTimeOut(storeSmspListStatus, ETimeMedium)==KErrNone)
		{
		if (storeSmspListStatus.Int()!= KErrNone)
			{
			DEBUG_PRINTF1(_L("Error- During CleanupStack RMobileSmsMessaging::StoreSmspListL returned with an error"));
			}
		}
	else
		{
		DEBUG_PRINTF1(_L("Error- During CleanupStack RMobileSmsMessaging::StoreSmspListL timed out"));
		}
	
	// Pop:
	// restoreSmspList
	// storeSmspListStatus
	CleanupStack::PopAndDestroy(2,restoreSmspList);
	
	}
TBackupList::operator TCleanupItem()
/*
 * This operator create a TCleanupItem to save a pointer to the DoCleanup static function and this in the cleanup stack
 *
 */	
	{
	return TCleanupItem(DoCleanupL,this);
	}

