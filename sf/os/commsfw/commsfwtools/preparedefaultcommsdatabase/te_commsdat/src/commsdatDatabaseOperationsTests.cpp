// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CCommsdat40Step
// CCommsdat41Step
// CCommsdat42Step
// CCommsdat43Step
// CCommsdat44Step
// CCommsdat45Step
// CCommsdat46Step
// CCommsdat47Step
// CCommsdat48Step
// CCommsdat49Step
// CCommsdat50Step
// CCommsdat51Step
// CCommsdat52Step
// CCommsdat53Step
// CCommsdat54Step
// CCommsdat55Step
// CCommsdat56Step
// CCommsdat57Step
// CCommsdat58Step
// CCommsdat59Step
// CCommsdat60Step
// CCommsdat61Step
// CCommsdat62Step
// CCommsdat63Step
// CCommsdat64Step
// CCommsdat65Step
// CCommsdat66Step
// CCommsdat67Step
// CCommsdat68Step
// CCommsdat70Step
// CCommsdat71Step
// CCommsdat72Step
// CCommsdat73Step
// 
//

/**
 @file commsdatDatabaseOperationsTests.cpp
*/

#include "commsdatDatabaseOperationsTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
#include <e32base.h>
#include <d32comm.h>
#include <commsdattypesv1_1_partner.h>
#include <commsdattypeinfov1_1_internal.h>

CCommsdat40Step::CCommsdat40Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat40Step);
	}

TVerdict CCommsdat40Step::doTestStepL()
/**
 * @return - TVerdict code
 * Setting field values and storing a new record
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Setting and getting field values - CCommsdat40Step::doTestStepL()"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);	 
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);

	iapRecord->SetRecordId(KCDNewRecordRequest);
	TPtrC aValue(_L("TestName"));

    iapRecord->iRecordName.SetMaxLengthL(aValue.Length());
    iapRecord->iRecordName = aValue;

    TPtrC bPtr(static_cast<const TDesC&>(iapRecord->iRecordName));
    INFO_PRINTF2(_L("Set iRecordName to %S"), &bPtr);

	iapRecord->iNetworkWeighting = 10;

    TInt i = iapRecord->iNetworkWeighting;
	INFO_PRINTF2(_L("Set iNetworkWeighting to %d"), i);
	
	cmdbSession->OpenTransactionL();
	TRAPD(storeErr, iapRecord->StoreL(*cmdbSession));
	cmdbSession->CommitTransactionL();
	
	if(storeErr == KErrNone)
		{
		SetTestStepResult(EPass);	
		}
	CleanupStack::PopAndDestroy(iapRecord);
    CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat41Step::CCommsdat41Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat41Step);
	}

TVerdict CCommsdat41Step::doTestStepL()
/**
 * @return - TVerdict code
 * Finding a record; record should be inserted with CED te_commsdat_iap_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat41Step::doTestStepL() in the file commsdat41Step.cpp"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	 
	CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRecord);

	iapRecord->iNetworkWeighting = 10;
    TBool found(EFalse);
	found = iapRecord->FindL(*cmdbSession);
    if (found)
    	{
        SetTestStepResult(EPass);	
    	}

    INFO_PRINTF2(_L("FindL returned %d, should be 1"), found);
    INFO_PRINTF2(_L("test result is %d"), TestStepResult());
    
    CleanupStack::PopAndDestroy(iapRecord);
    CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat42Step::CCommsdat42Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat42Step);
	}


TVerdict CCommsdat42Step::doTestStepL()
/**
 * @return - TVerdict code
 * Storing and finding record
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Storing and finding a record - CCommsdat42Step::doTestStepL() in the file commsdat42Step.cpp"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);

	iapRecord->SetRecordId(KCDNewRecordRequest);
    
    iapRecord->iRecordName.SetMaxLengthL(20);
    iapRecord->iRecordName = _L("TestNameTemplate");
	iapRecord->iNetworkWeighting = 17;
    

	iapRecord->StoreL(*cmdbSession);
    	
    iapRecord->iNetworkWeighting = 17;

    TBool found(EFalse);
    found = iapRecord->FindL(*cmdbSession); // this one should succeed

    INFO_PRINTF2(_L("Find returned %b, should be 1"), found);

	iapRecord->iNetworkWeighting = 13;
	
    found = iapRecord->FindL(*cmdbSession); // this one should fail
    INFO_PRINTF2(_L("Find returned %b, should be 0"), found);

    TRAPD(storeErr, iapRecord->StoreL(*cmdbSession));
    INFO_PRINTF2(_L("Store returned %d, should be -11"), storeErr); // this one should fail 


    CMDBRecordSet<CCDIAPRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
    iapRecord->iNetworkWeighting = 17;
	CleanupStack::Pop(iapRecord);
   	recordSet->iRecords.Append(iapRecord);
	found = recordSet->FindL(*cmdbSession);
    INFO_PRINTF2(_L("RecordSet FindL returned %b, should be 1"), found);
    
	delete recordSet;
	CleanupStack::PopAndDestroy(cmdbSession);
    if(found )
        {
        SetTestStepResult(EPass);	
	    }

    return TestStepResult();
	}


//----------------------------------------------------------------------------------------------------------------

CCommsdat43Step::CCommsdat43Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat43Step);                                                                                           
	}


TVerdict CCommsdat43Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test FindL, StoreL, LoadL for records
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat43Step::doTestStepL() in the file commsdat43Step.cpp"));
    
    
	CMDBSession *cmdbSession = NULL;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
		
	cmdbSession->OpenTransactionL();
		
	CCDIAPRecord *record = NULL;
	record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
		
    record->SetRecordId(1);		
	record->iRecordTag = 13;
	
	TBool found(0);
	found = record->FindL(*cmdbSession);
	
	if(!found)
		{
		INFO_PRINTF2(_L("FindL found no record at id %08x"), record->ElementId());
		record->StoreL(*cmdbSession);
		
		cmdbSession->CommitTransactionL();
		}
			
	cmdbSession->Close();
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
			
	cmdbSession->OpenTransactionL();
	
	record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	record->iRecordTag = 13;
	found = record->FindL(*cmdbSession);
	if(found)
		{
		SetTestStepResult(EPass);
		}
		
	CleanupStack::PopAndDestroy(record);
	cmdbSession->Close();
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat44Step::CCommsdat44Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat44Step);                                                                                           
	}

TVerdict CCommsdat44Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test find, load, modify and store on individual fields of a record; record should be inserted with CED te_commsdat_iap_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat44Step::doTestStepL() and trying to test load, find, store, and modify"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);
	
	TPtrC recordName(_L("TestName"));
	iapRecord->iRecordName.SetMaxLengthL(recordName.Length());
	iapRecord->iRecordName = recordName; 
	TBool found(EFalse);
	found = iapRecord->iRecordName.FindL(*cmdbSession);
	INFO_PRINTF2(_L("FindL for iap record name field returns %d - should be 1"), found);
    if(found)
		{
		// Now a test for loading a field that is not part of a record
        CMDBField<TDesC>* aField = new(ELeave) CMDBField<TDesC>(iapRecord->iRecordName.ElementId());
        CleanupStack::PushL(aField);

        TRAPD(loadErr, aField->LoadL(*cmdbSession));
		INFO_PRINTF2(_L("LoadL for descriptor field returns %d - should be 0"), loadErr);
        
        if(loadErr == KErrNone)
			{
            // Now test for modifying a field
       		TPtrC recordName2(_L("TestName2"));
			iapRecord->iRecordName.SetMaxLengthL(recordName2.Length());
			iapRecord->iRecordName = recordName2;
			TRAPD(modifyErr, iapRecord->iRecordName.ModifyL(*cmdbSession));
		    INFO_PRINTF2(_L("ModifyL for iap record returns %d - should be 0"), modifyErr);
        	if(modifyErr == KErrNone)
				{
				CCDIAPRecord *newIapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
				CleanupStack::PushL(newIapRecord);
				
				newIapRecord->SetRecordId(KCDNewRecordRequest);
                TRAPD(storeErr, newIapRecord->StoreL(*cmdbSession));
                INFO_PRINTF3(_L("StoreL for newIapRecord returns %d - should be 0.  Record id is %08x"), storeErr, newIapRecord->ElementId());
				
				TPtrC recordNameNew(_L("TestNameNew"));
				newIapRecord->iRecordName.SetMaxLengthL(recordNameNew.Length());
				newIapRecord->iRecordName = recordNameNew;
				TRAP(storeErr, newIapRecord->iRecordName.StoreL(*cmdbSession));
                INFO_PRINTF3(_L("StoreL for field in new IAP record returns %d - should be 0.  Record id is %08x"), storeErr, newIapRecord->ElementId());
				if(storeErr == KErrNone)
					{
					SetTestStepResult(EPass);
					}
				
				CleanupStack::PopAndDestroy(newIapRecord);
				}
			}
        CleanupStack::PopAndDestroy(aField);
		}

	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat45Step::CCommsdat45Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat45Step);
	}

TVerdict CCommsdat45Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test find with more than one initialized fields 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing find with multiple fields - CCommsdat45Step::doTestStepL()"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	CCDDefaultWCDMARecord *defaultWCDMARecord1 = (CCDDefaultWCDMARecord*)CCDRecordBase::RecordFactoryL(KCDTIdDefaultWCDMARecord); 
	CleanupStack::PushL(defaultWCDMARecord1);
	
	defaultWCDMARecord1->iUsage = 35;
	TPtrC apn3(_L("APN3"));
	defaultWCDMARecord1->iAPN.SetMaxLengthL(apn3.Length());
	defaultWCDMARecord1->iAPN = apn3;
	defaultWCDMARecord1->iPdpType = 252;
	
	CMDBRecordSet<CCDDefaultWCDMARecord>* recordSet1 = new(ELeave) CMDBRecordSet<CCDDefaultWCDMARecord>(KCDTIdDefaultWCDMARecord);
	CleanupStack::PushL(recordSet1);	
	recordSet1->iRecords.Append(defaultWCDMARecord1);
	TBool res(0);
	TRAPD(findErr1, res = recordSet1->FindL(*cmdbSession));
	INFO_PRINTF2(_L("FindL for record set returns %b"), res);

	CCDDefaultWCDMARecord *defaultWCDMARecord2 = (CCDDefaultWCDMARecord*)CCDRecordBase::RecordFactoryL(KCDTIdDefaultWCDMARecord); 
	CleanupStack::PushL(defaultWCDMARecord2);
	
	defaultWCDMARecord2->iUsage = 35;
	TPtrC apn(_L("APN"));
	defaultWCDMARecord2->iAPN.SetMaxLengthL(apn.Length());
	defaultWCDMARecord2->iAPN = apn;
		
	CMDBRecordSet<CCDDefaultWCDMARecord>* recordSet2 = new(ELeave) CMDBRecordSet<CCDDefaultWCDMARecord>(KCDTIdDefaultWCDMARecord);
	CleanupStack::PushL(recordSet2);	
	recordSet2->iRecords.Append(defaultWCDMARecord2);
	TRAPD(findErr2, res = recordSet2->FindL(*cmdbSession));
	
	CCDDefaultWCDMARecord *defaultWCDMARecord3 = (CCDDefaultWCDMARecord*)CCDRecordBase::RecordFactoryL(KCDTIdDefaultWCDMARecord); 
	CleanupStack::PushL(defaultWCDMARecord3);
	
	defaultWCDMARecord3->iUsage = 35;
			
	CMDBRecordSet<CCDDefaultWCDMARecord>* recordSet3 = new(ELeave) CMDBRecordSet<CCDDefaultWCDMARecord>(KCDTIdDefaultWCDMARecord);
	CleanupStack::PushL(recordSet3);	
	recordSet3->iRecords.Append(defaultWCDMARecord3);
	res = recordSet3->FindL(*cmdbSession);
	
	if(findErr1 == KErrNone && findErr2 == KErrNone && findErr2 == KErrNone)
		{
		if(recordSet1->iRecords.Count() == 1 && recordSet2->iRecords.Count() == 2 && recordSet3->iRecords.Count() == 3)
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(recordSet3);	
	CleanupStack::Pop(defaultWCDMARecord3);	
	CleanupStack::PopAndDestroy(recordSet2);	
	CleanupStack::Pop(defaultWCDMARecord2);	
	CleanupStack::PopAndDestroy(recordSet1);	
	CleanupStack::Pop(defaultWCDMARecord1);	
	CleanupStack::PopAndDestroy(cmdbSession);
    return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat46Step::CCommsdat46Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat46Step);                                                                                           
	}


TVerdict CCommsdat46Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test Load on record set; should return KErrNotFound
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Load within transaction - CCommsdat46Step::doTestStepL() in the file commsdat46Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	cmdbSession->OpenTransactionL();
		
	CMDBRecordSet<CCDDialOutISPRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
	
	TRAPD(loadErr, recordSet->LoadL(*cmdbSession));
		
	if ( loadErr == KErrNotFound )
       	{
        SetTestStepResult(EPass);	
        }

	delete recordSet;
	cmdbSession->Close();
		
	CleanupStack::PopAndDestroy(cmdbSession);
		
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat47Step::CCommsdat47Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat47Step);                                                                                           
	}


TVerdict CCommsdat47Step::doTestStepL()
/**
 * @return - TVerdict code
 * Store a RecordSet within a transaction
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Store RecordSet within transaction - CCommsdat47Step::doTestStepL() in the file commsdat47Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	cmdbSession->OpenTransactionL();
	
 	CMDBRecordSet<CCDDialOutISPRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
	CCDDialOutISPRecord *record1 = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord );
	CCDDialOutISPRecord *record2 = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord );
	CCDDialOutISPRecord *record3 = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord );
	
	record1->SetRecordId(KCDNewRecordRequest);
	record1->iV42MaxLength = 101; //an arbitrary value
	record2->SetRecordId(KCDNewRecordRequest);
	record2->iV42MaxLength = 102; //an arbitrary value
	record3->SetRecordId(KCDNewRecordRequest);
	record3->iV42MaxLength = 103; //an arbitrary value
	
	recordSet->iRecords.Append(record1);
	recordSet->iRecords.Append(record2);
	recordSet->iRecords.Append(record3);

    TRAPD(storeErr, recordSet->StoreL(*cmdbSession));
    INFO_PRINTF2(_L("Store 3 DialOutISP records in a set returned %d.  Should be 0"), storeErr);

	TRAPD(commitErr, cmdbSession->CommitTransactionL());		
    INFO_PRINTF2(_L("CommitTransactionL (storing DialOutISP) returned %d.  Should be 0"), commitErr);

	delete recordSet;
	cmdbSession->Close();
	CleanupStack::PopAndDestroy(cmdbSession);
	
	if(storeErr == KErrNone  &&  commitErr == KErrNone)
		{
    	SetTestStepResult(EPass);	
		}
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat48Step::CCommsdat48Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat48Step);                                                                                           
	}

TVerdict CCommsdat48Step::doTestStepL()
/**
 * @return - TVerdict code
 * Load and Modify in transaction on a recordset;records should be inserted with CED te_commsdat_dialoutisp_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Load and Modify in transaction - CCommsdat48Step::doTestStepL() in the file commsdat48Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	TRAPD(openErr, cmdbSession->OpenTransactionL());
	if(openErr == KErrNone)
		{
		
		CMDBRecordSet<CCDDialOutISPRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
		TRAPD(err,recordSet->LoadL(*cmdbSession));
	
    	INFO_PRINTF2(_L("Load DialOutISP returned %d.  Should be 0"), err);

    	INFO_PRINTF2(_L("Load DialOutISP returned %d records.  should be 3"), recordSet->iRecords.Count());

    	if (err == KErrNone && recordSet->iRecords.Count() == 3)
        	{
        	CCDDialOutISPRecord* dialoutptr = static_cast<CCDDialOutISPRecord *>(recordSet->iRecords[0]);
        	dialoutptr->iV42MaxLength = 201;

        	dialoutptr = static_cast<CCDDialOutISPRecord *>(recordSet->iRecords[1]);
        	dialoutptr->iV42MaxLength = 202;

        	dialoutptr = static_cast<CCDDialOutISPRecord *>(recordSet->iRecords[2]);
        	dialoutptr->iV42MaxLength = 203;
	    
        	INFO_PRINTF1(_L("Modifying record"));

	    	TRAPD(modifyErr, recordSet->ModifyL(*cmdbSession));
		    
		    if(modifyErr == KErrNone)
		    	{
        		INFO_PRINTF1(_L("Committing Transaction..."));
				TRAPD(commitErr, cmdbSession->CommitTransactionL());		
	    		if(commitErr == KErrNone)
	    			{
	    			SetTestStepResult(EPass);	
	    			}
		    	}
		    else
		    	{
		    	INFO_PRINTF1(_L("Rolling transaction back"));
				cmdbSession->RollbackTransactionL();	
		    	}
        	}
		delete recordSet;
		}
	
	cmdbSession->Close();
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat49Step::CCommsdat49Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat49Step);                                                                                           
	}


TVerdict CCommsdat49Step::doTestStepL()
/**
 * @return - TVerdict code
 * ModifyL on record set;records should be inserted with CED te_commsdat_dialoutisp_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat49Step::doTestStepL() in the file commsdat49Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession); 

	cmdbSession->OpenTransactionL();
	
	CMDBRecordSet<CCDDialOutISPRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
	recordSet->LoadL(*cmdbSession);
    
	recordSet->SetMaxLengthL(KCDMaxFieldNameLength);
	*recordSet = _S("DummyTableType");
	
	TRAPD(modifyErr, recordSet->ModifyL(*cmdbSession));
    INFO_PRINTF2(_L("ModifyL recordSet result = %d.  Should be 0."), modifyErr);
		
	cmdbSession->CommitTransactionL();		
	
	delete recordSet;
	CleanupStack::PopAndDestroy(cmdbSession);
	
	if(modifyErr == KErrNone)
		{
		SetTestStepResult(EPass);		
		}
    
	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat50Step::CCommsdat50Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat50Step);                                                                                           
	}

TVerdict CCommsdat50Step::doTestStepL()
/**
 * @return - TVerdict code
 * Set attributes and store them
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat50Step::doTestStepL() and trying to set attributes and store them"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  
		
	CCDVirtualBearerRecord *vbRecord = (CCDVirtualBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord);

	CleanupStack::PushL(vbRecord);
	
    vbRecord->SetRecordId(KCDNewRecordRequest);

    TPtrC fileName(_L("TempFile"));
    vbRecord->iVirtualBearerNifName.SetMaxLengthL(fileName.Length());
    vbRecord->iVirtualBearerNifName = fileName;
	vbRecord->SetAttributes(ECDPrivate);
	if(vbRecord->IsSetAttribute(ECDPrivate))
        {
        INFO_PRINTF1(_L("Attributes are set properly"));

        cmdbSession->SetAttributeMask(ECDPrivate);

	    TRAPD(storeErr, vbRecord->StoreL(*cmdbSession));
        INFO_PRINTF2(_L("vbRecord StoreL result was %d.  Should be 0"), storeErr);
        
	    TUint32 id = vbRecord->iVirtualBearerNifName.ElementId();
	    
        INFO_PRINTF3(_L("Element id of iAgentFileName is %08x, should be %08x"), id & KCDMaskHideRes, KCDTIdAgentFileName | ECDPrivate | (id & KCDMaskShowRecordId));

        if (storeErr == KErrNone  &&  
            (id & KCDMaskShowAttributes) == (ECDPrivate) )
            SetTestStepResult(EPass);
	    }
    else
		INFO_PRINTF1(_L("Attributes set wrongly"));
	
	CleanupStack::PopAndDestroy(vbRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
		
    return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat51Step::CCommsdat51Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat51Step);                                                                                           
	}

TVerdict CCommsdat51Step::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  
		
	CCDVirtualBearerRecord *vbRecord = (CCDVirtualBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord);

	CleanupStack::PushL(vbRecord);
	
    vbRecord->SetRecordId(KCDNewRecordRequest);

    TPtrC fileName(_L("TempFile"));
    vbRecord->iVirtualBearerNifName.SetMaxLengthL(fileName.Length());
    vbRecord->iVirtualBearerNifName = fileName;
	vbRecord->SetAttributes(ECDPrivate);

    cmdbSession->SetAttributeMask(ECDPrivate);

	TRAPD(storeErr, vbRecord->StoreL(*cmdbSession));
    
    if (storeErr == KErrNone)
    	{
    	SetTestStepResult(EPass);
	    }
    
	CleanupStack::PopAndDestroy(vbRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
		
    return TestStepResult(); 
	}


TVerdict CCommsdat51Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify attributes of a record
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat51Step::doTestStepL() and trying to modify attributes"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDVirtualBearerRecord *vbRecord = (CCDVirtualBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord);
	CleanupStack::PushL(vbRecord); 

    vbRecord->SetRecordId(1);

    cmdbSession->SetAttributeMask(ECDPrivate);

	TRAPD(loadErr, vbRecord->LoadL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL vbRecord result was %d.  Should be 0."), loadErr);
    
    if (loadErr == KErrNone)
        {
	    TUint32 id = vbRecord->iVirtualBearerNifName.ElementId();
	    INFO_PRINTF2(_L("Element id of iAgentFileName is %08x"), id);

	    vbRecord->SetAttributes(ECDHidden);
        cmdbSession->SetAttributeMask(ECDHidden);

        TMDBElementId testId = vbRecord->iVirtualBearerNifName.ElementId();

        TRAPD(modifyErr, vbRecord->ModifyL(*cmdbSession));
        INFO_PRINTF2(_L("ModifyL vbRecord result was %d.  Should be 0."), modifyErr);
 
        if( modifyErr == KErrNone)
            {
	        id = vbRecord->iVirtualBearerNifName.ElementId();
            INFO_PRINTF3(_L("Element id of iAgentFileName is %08x.  Should be %08x"), id, testId | ECDHidden);
        
            if (((testId | ECDHidden) & KCDMaskHideRes) == (id & KCDMaskHideRes) )
	            SetTestStepResult(EPass);	
            }
        }
	
	CleanupStack::PopAndDestroy(vbRecord);
	cmdbSession->Close();
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat52Step::CCommsdat52Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat52Step);                                                                                           
	}

TVerdict CCommsdat52Step::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  
		
	CCDVirtualBearerRecord *vbRecord = (CCDVirtualBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord);

	CleanupStack::PushL(vbRecord);
	
    vbRecord->SetRecordId(KCDNewRecordRequest);

    TPtrC fileName(_L("TempFile"));
    vbRecord->iVirtualBearerNifName.SetMaxLengthL(fileName.Length());
    vbRecord->iVirtualBearerNifName = fileName;
	vbRecord->SetAttributes(ECDPrivate | ECDHidden);

    cmdbSession->SetAttributeMask(ECDPrivate | ECDHidden);

	TRAPD(storeErr, vbRecord->StoreL(*cmdbSession));
    
    if (storeErr == KErrNone)
    	{
    	SetTestStepResult(EPass);
	    }
    
	CleanupStack::PopAndDestroy(vbRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
		
    return TestStepResult(); 
	}


TVerdict CCommsdat52Step::doTestStepL()
/**
 * @return - TVerdict code
 * Reach a field by name and modify a descriptor value in db
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat52Step::doTestStepL() and trying to reach a field by name and modify value in db"));
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	CCDVirtualBearerRecord *vbRecord = (CCDVirtualBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord);
	CleanupStack::PushL(vbRecord);
    
    cmdbSession->SetAttributeMask(ECDHidden | ECDPrivate);

    vbRecord->SetRecordId(1);
  
    TRAPD(loadErr, vbRecord->LoadL(*cmdbSession));
	
    INFO_PRINTF2(_L("vbRecord LoadL result was %d.  Should be 0"), loadErr);
  
    CMDBField<TDesC> *afn = NULL;
	TInt temp(0);
	TRAPD(getFieldErr, afn = static_cast<CMDBField<TDesC> *>(vbRecord->GetFieldByNameL(_L("IfName"), temp)));
    TPtrC tempPtr(*afn);
    INFO_PRINTF3(_L("vbRecord GetFieldByName result was %d.  Should be 0.  Value is %S"), getFieldErr, &tempPtr);

    if(getFieldErr == KErrNone)
		{
		TPtrC  agentFriendlyName(_L("temp friendly name"));
        afn->SetMaxLengthL(agentFriendlyName.Length());
		*afn = agentFriendlyName;
		TRAPD(modifyErr,vbRecord->ModifyL(*cmdbSession));
	    INFO_PRINTF2(_L("vbRecord ModifyL result was %d.  Should be 0"), modifyErr);

        if(modifyErr == KErrNone)
			{
			SetTestStepResult(EPass);
			}
		}
		
	TPtrC displayName(vbRecord->iVirtualBearerNifName);
	INFO_PRINTF2(_L("Agent name is %S"), &displayName);
	
	CleanupStack::PopAndDestroy(vbRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult(); 	
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat53Step::CCommsdat53Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat53Step);                                                                                           
	}

TVerdict CCommsdat53Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check ModifyL with multiple changing fields; record should be inserted with CED te_commsdat_lanservice_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat53Step::doTestStepL() and trying to insert a LAN Service Record."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDLANServiceRecord *lanRecord = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	CleanupStack::PushL(lanRecord);

    lanRecord->SetRecordId(1);

	lanRecord->LoadL(*cmdbSession);
      		
    TPtrC displayIpAddr(lanRecord->iIpAddr);
	INFO_PRINTF2(_L("lanRecord ipAddr is %S"), &displayIpAddr);	
	
    TPtrC ipGateway(_L("444.444.444.444"));
	lanRecord->iIpGateway.SetMaxLengthL(ipGateway.Length());
	lanRecord->iIpGateway = ipGateway;

    TPtrC ipAddr(_L("555.555.555.555"));
	lanRecord->iIpAddr.SetMaxLengthL(ipAddr.Length());
	lanRecord->iIpAddr = ipAddr;
	
	TRAPD(modifyErr, lanRecord->ModifyL(*cmdbSession));
    INFO_PRINTF2(_L("lanRecord ModifyL result was %d.  Should be 0"), modifyErr);
 	
	TPtrC displayIpAddr2(lanRecord->iIpAddr);
	INFO_PRINTF2(_L("lanRecord ipAddr is %S"), &displayIpAddr2);	
	
	CleanupStack::PopAndDestroy(lanRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
		
	if(modifyErr == KErrNone)
		{
		SetTestStepResult(EPass);	
		}
    	

    return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat54Step::CCommsdat54Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat54Step);                                                                                           
	}

TVerdict CCommsdat54Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check RefreshL; record should be inserted with CED te_commsdat_lanservice_table2
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat54Step::doTestStepL() and trying to check ModifyL"));
	
	CCDLANServiceRecord *lanRecord1 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	CleanupStack::PushL(lanRecord1);
    lanRecord1->SetRecordId(1);

	CCDLANServiceRecord *lanRecord2 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	CleanupStack::PushL(lanRecord2);
	lanRecord2->SetRecordId(1); // same record id as lanRecord1

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	lanRecord1->LoadL(*cmdbSession);
	lanRecord2->LoadL(*cmdbSession);
	
	
    TPtrC displayIpAddr(lanRecord2->iIpAddr);
	INFO_PRINTF2(_L("LANRecord2 has %S as ip addr"),&displayIpAddr);	
	
	TPtrC displayIpGateway(lanRecord2->iIpGateway);
	INFO_PRINTF2(_L("LANRecord2 has %S as ip gateway"),&displayIpGateway);	
	
	TPtrC newIpGateway(_L("888.888.888.888"));
	TPtrC newIpAddr(_L("999.999.999.999"));
    	
    lanRecord1->iIpGateway = newIpGateway;
	lanRecord2->iIpAddr = newIpAddr;
		
    lanRecord1->ModifyL(*cmdbSession);
	lanRecord2->RefreshL(*cmdbSession);
		
	
	TPtrC newDisplayIpAddr(lanRecord2->iIpAddr);
	INFO_PRINTF3(_L("LANRecord2 now has %S as ip addr. Should be %S"),&newDisplayIpAddr, &newIpAddr);	
	
	TPtrC newDisplayIpGateway(lanRecord2->iIpGateway);
	INFO_PRINTF3(_L("LANRecord2 now has %S as ip gateway.  Should be %S"),&newDisplayIpGateway, &newIpGateway);
	
	TBool intermediateResult = (newDisplayIpAddr == newIpAddr &&  newDisplayIpGateway == newIpGateway);
	
	CleanupStack::PopAndDestroy(cmdbSession) ;
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
    CCDLANServiceRecord *lanRecord3 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	CleanupStack::PushL(lanRecord3);
    lanRecord3->SetRecordId(1);
			
    TRAPD(loadErr, lanRecord3->LoadL(*cmdbSession));	
			
	if(loadErr == KErrNone)
		{
		TPtrC newestDisplayIpAddr(lanRecord3->iIpAddr);
		INFO_PRINTF2(_L("Latest IpAddr is %S"), &newestDisplayIpAddr);	
	
		TPtrC oldIpAddr(_L("555.555.555.555"));
		INFO_PRINTF2(_L("Original IpAddr value was %S"), &oldIpAddr);
	
        TPtrC newestDisplayIpGateway(lanRecord3->iIpGateway);
		INFO_PRINTF2(_L("Latest IpGateway value is %S"), &newestDisplayIpGateway);		
				
		if(intermediateResult && 
           newestDisplayIpAddr == oldIpAddr && 
           newestDisplayIpGateway == newIpGateway)
			{
			SetTestStepResult(EPass);
			}
		}
   	
   	CleanupStack::PopAndDestroy(lanRecord3);
    CleanupStack::PopAndDestroy(cmdbSession);
	CleanupStack::PopAndDestroy(lanRecord2);
	CleanupStack::PopAndDestroy(lanRecord1);
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat55Step::CCommsdat55Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat55Step);                                                                                           
	}

TVerdict CCommsdat55Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test deleting; record should be inserted with CED te_commsdat_lanservice_table3
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat55Step::doTestStepL() and trying to test SetAttributeMask"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	CMDBRecordSet<CCDLANServiceRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDLANServiceRecord>(KCDTIdLANServiceRecord);
	
	TRAPD(loadErr, recordSet->LoadL(*cmdbSession));
    INFO_PRINTF3(_L("lanService recordSet LoadL result was %d.  Should be 0.  returned %d records"), loadErr, recordSet->iRecords.Count());
	
	if(loadErr == KErrNone)
		{
		if(recordSet->iRecords.Count() > 0)
			{
			TRAPD(openErr, cmdbSession->OpenTransactionL());
            INFO_PRINTF2(_L("lanRecord OpenTransactionL result was %d.  Should be 0"), openErr);
				
			if(openErr == KErrNone)
				{
				TRAPD(deleteErr, recordSet->DeleteL(*cmdbSession));
                INFO_PRINTF2(_L("lanRecord DeleteL result was %d.  Should be 0"), deleteErr);

				TRAPD(commitErr, cmdbSession->CommitTransactionL());
                INFO_PRINTF2(_L("lanRecord CommitTransactionL result was %d.  Should be 0"), commitErr);
				}
			}

        CCDLANServiceRecord *lanRecord1 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
		CleanupStack::PushL(lanRecord1);
        lanRecord1->SetRecordId(KCDNewRecordRequest);

		CCDLANServiceRecord *lanRecord2 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
		CleanupStack::PushL(lanRecord2);
		lanRecord2->SetRecordId(KCDNewRecordRequest);

		TPtrC newIpAddr1(_L("111.111.111.111"));
		TPtrC newIpAddr2(_L("222.222.222.222"));
		
        lanRecord1->iIpAddr.SetMaxLengthL(newIpAddr1.Length());				
        lanRecord1->iIpAddr = newIpAddr1;
	 
        lanRecord2->iIpAddr.SetMaxLengthL(newIpAddr2.Length());				
		lanRecord2->iIpAddr = newIpAddr2;
		lanRecord2->SetAttributes(ECDHidden);
		
		TRAPD(openErr, cmdbSession->OpenTransactionL());
		INFO_PRINTF2(_L("lanRecords OpenTransactionL result was %d.  Should be 0"), openErr);
        if(openErr == KErrNone)	
			{
	        CMDBRecordSet<CCDLANServiceRecord>* nestedRecordSet = new(ELeave) CMDBRecordSet<CCDLANServiceRecord>(KCDTIdLANServiceRecord);

            TRAPD(storeErr1, lanRecord1->StoreL(*cmdbSession));
    		INFO_PRINTF2(_L("lanRecord1 StoreL result was %d.  Should be 0"), storeErr1);

            cmdbSession->SetAttributeMask(ECDHidden);

	        TRAPD(storeErr2, lanRecord2->StoreL(*cmdbSession));
            INFO_PRINTF2(_L("lanRecord2 StoreL result was %d.  Should be 0"), storeErr2);
			
            TRAPD(commitErr, cmdbSession->CommitTransactionL());
            INFO_PRINTF2(_L("lanRecords CommitTransactionL result was %d.  Should be 0"), commitErr);

            if(storeErr1 == KErrNone && storeErr2 == KErrNone && commitErr == KErrNone)
				{
				SetTestStepResult(EPass);		
				}
			delete nestedRecordSet;
			}
		CleanupStack::PopAndDestroy(lanRecord2);
		CleanupStack::PopAndDestroy(lanRecord1);
		}
	delete recordSet;
	
	CleanupStack::PopAndDestroy(cmdbSession);
		
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat56Step::CCommsdat56Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat56Step);                                                                                           
	}

TVerdict CCommsdat56Step::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

    CCDLANServiceRecord *lanRecord1 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	CleanupStack::PushL(lanRecord1);
    lanRecord1->SetRecordId(KCDNewRecordRequest);

	CCDLANServiceRecord *lanRecord2 = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	CleanupStack::PushL(lanRecord2);
	lanRecord2->SetRecordId(KCDNewRecordRequest);

	TPtrC newIpAddr1(_L("111.111.111.111"));
	TPtrC newIpAddr2(_L("222.222.222.222"));
		
    lanRecord1->iIpAddr.SetMaxLengthL(newIpAddr1.Length());				
    lanRecord1->iIpAddr = newIpAddr1;
	 
    lanRecord2->iIpAddr.SetMaxLengthL(newIpAddr2.Length());				
	lanRecord2->iIpAddr = newIpAddr2;
	lanRecord2->SetAttributes(ECDHidden);
		
	cmdbSession->OpenTransactionL();
	
    lanRecord1->StoreL(*cmdbSession);
   		
    cmdbSession->SetAttributeMask(ECDHidden);

    lanRecord2->StoreL(*cmdbSession);
        			
    cmdbSession->CommitTransactionL();
        
	CleanupStack::PopAndDestroy(lanRecord2);
	CleanupStack::PopAndDestroy(lanRecord1);
	
	CleanupStack::PopAndDestroy(cmdbSession);
		
	SetTestStepResult(EPass);		
	return TestStepResult(); 
	}


TVerdict CCommsdat56Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test finding records with attributes
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat56Step::doTestStepL() and trying to test SetAttributeMask"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
    CCDLANServiceRecord *lanRecord1 = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
	CleanupStack::PushL(lanRecord1);
	CCDLANServiceRecord *lanRecord2 = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
	CleanupStack::PushL(lanRecord2);
	CCDLANServiceRecord *lanRecord3 = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
	CleanupStack::PushL(lanRecord3);
	CCDLANServiceRecord *lanRecord4 = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
	CleanupStack::PushL(lanRecord4);
	
   	TPtrC newIpAddr1(_L("111.111.111.111"));

    TBool found1(EFalse);
    lanRecord1->iIpAddr.SetMaxLengthL(newIpAddr1.Length());				
	lanRecord1->iIpAddr = newIpAddr1;
	found1 = lanRecord1->FindL(*cmdbSession);
    if(found1)
    	{
    	INFO_PRINTF2(_L("Success.  Found non-hidden record without hidden mask.  ElementId %08x"), lanRecord1->ElementId());
    	}
	 
	TPtrC newIpAddr2(_L("222.222.222.222"));

    TBool found2(EFalse);
    lanRecord2->iIpAddr.SetMaxLengthL(newIpAddr2.Length());				
	lanRecord2->iIpAddr = newIpAddr2;
	found2 = lanRecord2->FindL(*cmdbSession);
    if(!found2)
    	{
    	INFO_PRINTF1(_L("Success.  Could not find hidden record without hidden mask."));
    	}
	else
		{
		INFO_PRINTF3(_L("Result of searching hidden record without mask %b, for element %08x"), found2, lanRecord2->ElementId());
		if(lanRecord2->IsSetAttribute(ECDHidden))
			{
			INFO_PRINTF1(_L("Hidden attribute set though"));	
			}
		}

	cmdbSession->SetAttributeMask(ECDHidden);

    TBool found3(EFalse);
    lanRecord3->iIpAddr.SetMaxLengthL(newIpAddr1.Length());				
	lanRecord3->iIpAddr = newIpAddr1;
	found3 = lanRecord3->FindL(*cmdbSession);
	if(found3)
		{
		INFO_PRINTF2(_L("Success.  Found non-hidden record with hidden mask set.  ElementId %08x"), lanRecord3->ElementId());
		}
    else
    	{
    	INFO_PRINTF1(_L("Failure.  Did not find non-hidden record with hidden mask set."));
    	}
        
    TBool found4(EFalse);
    lanRecord4->iIpAddr.SetMaxLengthL(newIpAddr2.Length());				
	lanRecord4->iIpAddr = newIpAddr2;
	found4 = lanRecord4->FindL(*cmdbSession);
	if(found4)
		{
		INFO_PRINTF2(_L("Success.  Found hidden record with hidden mask set.  ElementId %08x"), lanRecord4->ElementId());
		}
    
        
	CleanupStack::PopAndDestroy(lanRecord4);
	CleanupStack::PopAndDestroy(lanRecord3);
	CleanupStack::PopAndDestroy(lanRecord2);
	CleanupStack::PopAndDestroy(lanRecord1);
	CleanupStack::PopAndDestroy(cmdbSession);
		
	if(found1   &&
	   !found2  &&
	   found3   &&
	   found4 )
		{
		SetTestStepResult(EPass);
		}
		
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat57Step::CCommsdat57Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat57Step);                                                                                           
	}


TVerdict CCommsdat57Step::doTestStepL()
/**
 * @return - TVerdict code
 * test LoadL with record set; record should be added with CED te_commsdat_modembearer_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat57Step::doTestStepL() and trying to test RecordSet LoadL"));

	CMDBRecordSet<CCDModemBearerRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	TRAPD(recordSetLoadErr,recordSet->LoadL(*cmdbSession));
	INFO_PRINTF2(_L("LoadL for record set returns %d"), recordSetLoadErr);
	INFO_PRINTF2(_L("Number of records %d"), recordSet->iRecords.Count());
	
	for(TUint i = 0; i< recordSet->iRecords.Count(); i++)
		{
		INFO_PRINTF2(_L("Record number %d"), i);
		TPtrC displayRecordName((static_cast<CCDModemBearerRecord *>(recordSet->iRecords[i]))->iRecordName);
		INFO_PRINTF2(_L("Record name %S"), &displayRecordName);
		}
	
	if(recordSet->iRecords.Count() > 0)
		{
		SetTestStepResult(EPass);
		}

	delete recordSet;
	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat58Step::CCommsdat58Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat58Step);                                                                                           
	}

TVerdict CCommsdat58Step::doTestStepL()
/**
 * @return - TVerdict code
 * test FindL for record set with a search criteria; record should be added with CED te_commsdat_modembearer_table1
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat58Step::doTestStepL() and trying to test RecordSet FindL"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	CMDBRecordSet<CCDModemBearerRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	CleanupStack::PushL(recordSet);	
	CCDModemBearerRecord *record = static_cast<CCDModemBearerRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(record);
	record->iDataBits = EData8;
	
	recordSet->iRecords.Append(record);
	
    TBool found(0);
	found = recordSet->FindL(*cmdbSession);
	INFO_PRINTF2(_L("FindL for record set returns %d.  Should be 1"), found);
	INFO_PRINTF2(_L("Number of records %d."), recordSet->iRecords.Count());
	
	for(TUint i = 0; i< recordSet->iRecords.Count(); i++)
		{
		INFO_PRINTF2(_L("Record number %d"), i);
		TPtrC displayRecordName(static_cast<CCDModemBearerRecord *>(recordSet->iRecords[i])->iRecordName);
		INFO_PRINTF2(_L("Record name %S"), &displayRecordName);
		}
	
	if(found && recordSet->iRecords.Count() > 0)
		{
		SetTestStepResult(EPass);
		}
	
	recordSet->iRecords.Close();
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(recordSet);	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 

	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat59Step::CCommsdat59Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat59Step);                                                                                           
	}

TVerdict CCommsdat59Step::doTestStepL()
/**
 * @return - TVerdict code
 * test ModifyL for record set; record should be added with CED te_commsdat_modembearer_table2
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat59Step::doTestStepL() and trying to test RecordSet ModifyL"));

	CMDBRecordSet<CCDModemBearerRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	TRAPD(recordSetLoadErr,recordSet->LoadL(*cmdbSession));
	INFO_PRINTF2(_L("LoadL for record set returns %d"), recordSetLoadErr);
	INFO_PRINTF2(_L("Number of records %d"), recordSet->iRecords.Count());

    TInt modifyErr(KErrNotFound);

    if (recordSet->iRecords.Count() == 6)
        {
    	_LIT(KName1, "NAME 1");
		
        CCDModemBearerRecord *record1 = static_cast<CCDModemBearerRecord *>(recordSet->iRecords[0]);
	    record1->iRecordName.SetMaxLengthL(KName1().Length());
	    record1->iRecordName = KName1;
	
    	_LIT(KName5, "NAME 5");

	    CCDModemBearerRecord *record5 = static_cast<CCDModemBearerRecord *>(recordSet->iRecords[4]);
	    record5->iRecordName.SetMaxLengthL(KName5().Length());
	    record5->iRecordName = KName5;

	
	    for(TUint i = 0; i< recordSet->iRecords.Count(); i++)
		    {
		    INFO_PRINTF2(_L("Record 	 %d"), i);
		    TPtrC displayRecordName((static_cast<CCDModemBearerRecord *>(recordSet->iRecords[i]))->iRecordName);
		    INFO_PRINTF2(_L("Record name %S"), &displayRecordName);
		    }
	
    	TRAP(modifyErr, recordSet->ModifyL(*cmdbSession));
        INFO_PRINTF2(_L("ModifyL for recordSet returned %d"), modifyErr);
	    }

	delete recordSet;
    CleanupStack::PopAndDestroy(cmdbSession);
	
	if(modifyErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat60Step::CCommsdat60Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat60Step);                                                                                           
	}

TVerdict CCommsdat60Step::doTestStepL()
/**
 * @return - TVerdict code
 * test RefreshL on record set; record should be added with CED te_commsdat_modembearer_table2 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat60Step::doTestStepL() and trying to test RecordSet RefreshL"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	CMDBRecordSet<CCDModemBearerRecord>* recordSet1 = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	CleanupStack::PushL(recordSet1);	
	recordSet1->LoadL(*cmdbSession);
	
	if (recordSet1->iRecords.Count() > 2)
        {
        INFO_PRINTF2(_L("Number of records %d"), recordSet1->iRecords.Count());
	
	    CMDBRecordSet<CCDModemBearerRecord>* recordSet2 = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
		CleanupStack::PushL(recordSet2);	
	    recordSet2->LoadL(*cmdbSession);
	
        INFO_PRINTF2(_L("Number of records %d"), recordSet2->iRecords.Count());
	        
	    TPtrC name1(_L("NAME 1"));

	    static_cast<CCDModemBearerRecord *>(recordSet1->iRecords[1])->iRecordName = name1;
	        
	    TPtrC name2(_L("NAME 2"));
	    CCDModemBearerRecord *record2 = static_cast<CCDModemBearerRecord *>(recordSet2->iRecords[2]);
	    record2->iRecordName = name2;
		        
	    recordSet1->ModifyL(*cmdbSession);
	    recordSet2->RefreshL(*cmdbSession);
	    
		TPtrC testName1(static_cast<CCDModemBearerRecord *>(recordSet2->iRecords[1])->iRecordName);
		TPtrC testName2(static_cast<CCDModemBearerRecord *>(recordSet2->iRecords[2])->iRecordName);
		if(testName1 == name1 && testName2 == name2)
			{
			SetTestStepResult(EPass);	
			}
		CleanupStack::PopAndDestroy(recordSet2);	
        }    
	CleanupStack::PopAndDestroy(recordSet1);	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat61Step::CCommsdat61Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat61Step);                                                                                           
	}

TVerdict CCommsdat61Step::doTestStepL()
/**
 * @return - TVerdict code
 * test DeleteL for record set; record should be added with CED te_commsdat_modembearer_table2
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat61Step::doTestStepL() and trying to test RecordSet DeleteL"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	CMDBRecordSet<CCDModemBearerRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	CleanupStack::PushL(recordSet);	
	TRAPD(recordSetLoadErr,recordSet->LoadL(*cmdbSession));
	INFO_PRINTF2(_L("LoadL for record set returns %d"), recordSetLoadErr);
	if (recordSetLoadErr == KErrNone && recordSet->iRecords.Count() > 1)
        {
        INFO_PRINTF2(_L("Number of records %d"), recordSet->iRecords.Count());
	
	    TUint32 recordTag = static_cast<CCDModemBearerRecord *>(recordSet->iRecords[1])->iRecordTag;
	    
	    cmdbSession->OpenTransactionL();
	    static_cast<CCDModemBearerRecord *>(recordSet->iRecords[1])->DeleteL(*cmdbSession);
	    cmdbSession->CommitTransactionL();
			    
	    CCDModemBearerRecord *record = static_cast<CCDModemBearerRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
		CleanupStack::PushL(record);
		    
		record->iRecordTag = recordTag;
		TBool res(EFalse);
		res = record->FindL(*cmdbSession);
		if(!res)
		    {
		    SetTestStepResult(EPass);	
		    }
		 
		 CleanupStack::PopAndDestroy(record);
        }
    
	CleanupStack::PopAndDestroy(recordSet);	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat62Step::CCommsdat62Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat62Step);                                                                                           
	}

TVerdict CCommsdat62Step::doTestStepL()
/**
 * @return - TVerdict code
 * test DeleteL for record set; record should be added with CED te_commsdat_modembearer_table2
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat62Step::doTestStepL() and trying to test RecordSet DeleteL"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	CMDBRecordSet<CCDModemBearerRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	recordSet->LoadL(*cmdbSession);
	INFO_PRINTF2(_L("Number of records %d"), recordSet->iRecords.Count());
	
	cmdbSession->OpenTransactionL();
	recordSet->DeleteL(*cmdbSession);
	cmdbSession->CommitTransactionL();
			
	CMDBRecordSet<CCDModemBearerRecord>* recordSetTest = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);	
	TRAPD(recordSetTestLoadErr,recordSetTest->LoadL(*cmdbSession));
		 
	if(recordSetTestLoadErr == KErrNotFound)
		{
		INFO_PRINTF1(_L("Records Deleted correctly"));	
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Err in record deletion %d"), recordSetTestLoadErr);	
		}	
	delete recordSetTest;
		
	delete recordSet;
	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat63Step::CCommsdat63Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat63Step);                                                                                           
	}

TVerdict CCommsdat63Step::doTestStepL()
/**
 * @return - TVerdict code
 * test record set attributes
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat63Step::doTestStepL() and trying to test RecordSet SetAttributes"));
	
	CMDBRecordSet<CCDLocationRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);	
	CleanupStack::PushL(recordSet);	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	TRAPD(recordSetLoadErr,recordSet->LoadL(*cmdbSession));
	INFO_PRINTF2(_L("LoadL for record set returns %d"), recordSetLoadErr);
	INFO_PRINTF2(_L("Number of records %d"), recordSet->iRecords.Count());
	
	recordSet->SetAttributes(ECDNoWriteButDelete);

    cmdbSession->SetAttributeMask(ECDNoWriteButDelete);
	TRAPD(modifyErr, recordSet->ModifyL(*cmdbSession));
	INFO_PRINTF2(_L("ModifyL for record set returns %d"), modifyErr);
	
	if(modifyErr == KErrNone)
		{
		CleanupStack::PopAndDestroy(cmdbSession);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
		CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
		CleanupStack::PushL(cmdbSession);
		
		CCDLocationRecord *record = static_cast<CCDLocationRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));
		CleanupStack::PushL(record);
        record->SetRecordId(22);
		
		TRAPD(loadErr, record->LoadL(*cmdbSession));
    	INFO_PRINTF4(_L("LoadL for record %08x returns %d.  IsSetAttribute(ECDNoWriteButDelete) returns %d"), record->ElementId(), loadErr, record->IsSetAttribute(ECDNoWriteButDelete));
		if(loadErr == KErrNone && record->IsSetAttribute(ECDNoWriteButDelete))
			{
			SetTestStepResult(EPass);	
			}
		CleanupStack::PopAndDestroy(record);
		}
	CleanupStack::PopAndDestroy(cmdbSession);
	CleanupStack::PopAndDestroy(recordSet);	
	
	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat64Step::CCommsdat64Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat64Step);                                                                                           
	}

TVerdict CCommsdat64Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test record set attributes
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat64Step::doTestStepL() and trying to test RecordSet SetAttributes"));
	
	CMDBRecordSet<CCDLocationRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);	
	CleanupStack::PushL(recordSet);	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	TRAPD(recordSetLoadErr,recordSet->LoadL(*cmdbSession));
	INFO_PRINTF2(_L("LoadL for record set returns %d"), recordSetLoadErr);
	INFO_PRINTF2(_L("Number of records %d"), recordSet->iRecords.Count());
	
    if (recordSet->iRecords.Count() > 0)
        {
	    recordSet->iRecords[0]->SetAttributes(ECDNoWriteButDelete);
	    cmdbSession->SetAttributeMask(ECDNoWriteButDelete);

        TRAPD(modifyErr, recordSet->iRecords[0]->ModifyL(*cmdbSession));
        TMDBElementId idWithAttr = recordSet->iRecords[0]->ElementId();
	    INFO_PRINTF3(_L("modify attributes record set err is %d for ElementId %08x"), modifyErr, recordSet->iRecords[0]->ElementId());
	    
	    if(modifyErr == KErrNone)
		    {
		    CleanupStack::PopAndDestroy(cmdbSession);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		    CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
		    CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
		    CleanupStack::PushL(cmdbSession);
		    
		    CMDBRecordSet<CCDLocationRecord>* recordSetTest = new(ELeave) CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);	
			CleanupStack::PushL(recordSetTest);	
		    TRAPD(loadTestErr, recordSetTest->LoadL(*cmdbSession));
		    INFO_PRINTF4(_L("load record set err is %d.  ElementId is %08x.  Should be %08x"), loadTestErr, recordSetTest->iRecords[0]->ElementId(), idWithAttr);
		    
		    if(loadTestErr == KErrNone && recordSetTest->iRecords[0]->IsSetAttribute(ECDNoWriteButDelete))
			    {
			    SetTestStepResult(EPass);	
			    }
			CleanupStack::PopAndDestroy(recordSetTest);	
		    }
        }	    
	CleanupStack::PopAndDestroy(cmdbSession);
	CleanupStack::PopAndDestroy(recordSet);	
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat65Step::CCommsdat65Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat65Step);                                                                                           
	}

TVerdict CCommsdat65Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test storing a single field on a records.
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat65Step::doTestStepL() and trying to test load, find, store, and modify"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);
	
	iapRecord->iNetworkWeighting = 10;
	TBool found = iapRecord->iNetworkWeighting.FindL(*cmdbSession);
   	
	if(found)
		{
		TRAPD(loadErr, iapRecord->iNetworkWeighting.LoadL(*cmdbSession));
	   	INFO_PRINTF2(_L("LoadL for field returns %d - should be 0"), loadErr);

		if(loadErr == KErrNone)
			{
			iapRecord->iNetworkWeighting = 20;
			TRAPD(modifyErr, iapRecord->iNetworkWeighting.ModifyL(*cmdbSession));
    		INFO_PRINTF2(_L("ModifyL for field returns %d - should be 0"), modifyErr);
	        if(modifyErr == KErrNone)
				{
				CCDIAPRecord *newIapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
				CleanupStack::PushL(newIapRecord);
				
                newIapRecord->SetRecordId(72);

				TRAPD(storeErr, newIapRecord->StoreL(*cmdbSession));
                INFO_PRINTF2(_L("StoreL for new IAP record returns %d - should be 0"), storeErr);

                if(storeErr == KErrNone)
					{
					newIapRecord->iNetworkWeighting.SetRecordId(72);
                    newIapRecord->iNetworkWeighting = 21;

				    TRAP(storeErr, newIapRecord->iNetworkWeighting.StoreL(*cmdbSession));
                    INFO_PRINTF2(_L("StoreL for field in new IAP record returns %d - should be 0"), storeErr);
                
                    if(storeErr == KErrNone)
				    	{
				    	SetTestStepResult(EPass);
				    	}
                    }
				
				CleanupStack::PopAndDestroy(newIapRecord);
				}
			}
		}

	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat66Step::CCommsdat66Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat66Step);                                                                                           
	}
TVerdict CCommsdat66Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test linking two records
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat66Step::doTestStepL() and trying to establish a link between an IAP and a Location record"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);

	
	iapRecord->iNetworkWeighting = 21;

    TBool found(EFalse);
    found = iapRecord->FindL(*cmdbSession);
    INFO_PRINTF3(_L("FindL for iap record returns %d, %d - should be 1.  Element Id is %08x"), found, iapRecord->ElementId());

	if(found)
		{
		CCDLocationRecord *locationRecord = (CCDLocationRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord);
		CleanupStack::PushL(locationRecord);
		
		TPtrC recordName(_L("Location 1")); 
	    locationRecord->iRecordName.SetMaxLengthL(recordName.Length());
        locationRecord->iRecordName = recordName;
		
        cmdbSession->SetAttributeMask(ECDNoWriteButDelete | ECDPrivate | ECDHidden);

		found = locationRecord->FindL(*cmdbSession);
        INFO_PRINTF2(_L("FindL for location record returns %d - should be 1"), found);

		if(found)
			{
			iapRecord->iLocation = locationRecord->RecordId();
			TRAPD(modifyErr, iapRecord->ModifyL(*cmdbSession));
            INFO_PRINTF2(_L("ModifyL for iap record returns %d - should be 0"), modifyErr);

			if(modifyErr == KErrNone)
				{
				SetTestStepResult(EPass);
				}
			}
		
		CleanupStack::PopAndDestroy(locationRecord);
		}
	
	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cmdbSession);		
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat67Step::CCommsdat67Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat67Step);                                                                                           
	}

TVerdict CCommsdat67Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test linking two records
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat67Step::doTestStepL() and trying to reach linked records fields"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);
	
	iapRecord->iNetworkWeighting = 21; 
	
    TBool found(0);

    TRAPD(findErr, found = iapRecord->FindL(*cmdbSession));
    INFO_PRINTF4(_L("FindL for iap record returns %d, %d - should be 0, 1.  Element Id is %08x"), findErr, found, iapRecord->ElementId());
	
    if(found)
		{
		CCDLocationRecord *locationRecord = (CCDLocationRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord);
		CleanupStack::PushL(locationRecord);
		
		locationRecord->SetRecordId(TMDBElementId (iapRecord->iLocation));
		
		TRAPD(loadErr, locationRecord->LoadL(*cmdbSession));
		if(loadErr == KErrNone)
			{
			TPtrC recordName(_L("Location 1"));
			TPtrC testName(locationRecord->iRecordName);
			INFO_PRINTF2(_L("Record name is %S"), &testName);
			TInt pauseAfterDialOut(locationRecord->iPauseAfterDialOut);
			INFO_PRINTF2(_L("Pause after dial is %d"), pauseAfterDialOut);
			if( testName == recordName)
				{
				SetTestStepResult(EPass);
				}
			}
		
		CleanupStack::PopAndDestroy(locationRecord);
		}
	
	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cmdbSession);		
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
_LIT(KVeryLongText, 
"A very long text; which is of 1024 characters:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
L"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
L"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
L"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
L"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
L"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
L"GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
L"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
L"IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII"
L"JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ"
L"kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk The end.");

CCommsdat68Step::CCommsdat68Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat68Step);                                                                                           
	}

TVerdict CCommsdat68Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test long texts as field value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing long text as field values in CCommsdat68Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDDialOutISPRecord *record = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
	CleanupStack::PushL(record);
	
	//Create a new dial out ISP record
	record->SetRecordId(KCDNewRecordRequest);
				
	record->iLoginScript.SetL(KVeryLongText);
		
	TRAPD(storeErr, record->StoreL(*cmdbSession));
		
	if(storeErr == KErrNone) 
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
_LIT(KLongTextGreaterThan1024, 
"A very long text; which is of 1024 characters:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
L"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
L"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
L"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
L"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
L"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
L"GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
L"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
L"IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII"
L"JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ"
L"kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk The end.+");

CCommsdat69Step::CCommsdat69Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat69Step);                                                                                           
	}

TVerdict CCommsdat69Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test long texts as field value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing long text as field values in CCommsdat69Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDDialOutISPRecord *record = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
	CleanupStack::PushL(record);
	
	//Create a new dial out ISP record
	record->SetRecordId(KCDNewRecordRequest);
				
	record->iLoginScript.SetL(KLongTextGreaterThan1024);
		
	TRAPD(storeErr, record->StoreL(*cmdbSession));
		
	if(storeErr == KErrOverflow) 
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
_LIT(KMedText255, 
"A medium long text; which is of 255 characters:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
L"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
L"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
L" The end.");

CCommsdat70Step::CCommsdat70Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat70Step);                                                                                           
	}

TVerdict CCommsdat70Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test medium texts as field value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing medium text as field values in CCommsdat70Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDDialOutISPRecord *record = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
	CleanupStack::PushL(record);
	
	//Create a new dial out ISP record
	record->SetRecordId(KCDNewRecordRequest);
				
	record->iLoginName.SetL(KMedText255);
		
	TRAPD(storeErr, record->StoreL(*cmdbSession));
		
	if(storeErr == KErrNone) 
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//--------------------------------------------------------------------------------------------------------------
_LIT(KMedTextGreaterThan255, 
"A medium long text; which is of 256 characters:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
L"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
L"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
L" The end.+");

CCommsdat71Step::CCommsdat71Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat71Step);                                                                                           
	}

TVerdict CCommsdat71Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test medium texts as field value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing medium text as field values in CCommsdat71Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDDialOutISPRecord *record = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
	CleanupStack::PushL(record);
	
	//Create a new dial out ISP record
	record->SetRecordId(KCDNewRecordRequest);
				
	record->iLoginName.SetL(KMedTextGreaterThan255);
		
	TRAPD(storeErr, record->StoreL(*cmdbSession));
	
    INFO_PRINTF2(_L("storeErr = %d"), storeErr);
    
	if(storeErr == KErrOverflow) 
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//--------------------------------------------------------------------------------------------------------------
_LIT(KText50, 
"A text; which is of 50 characters:AAAAAAAAAAAAAAAA");

CCommsdat72Step::CCommsdat72Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat72Step);                                                                                           
	}

TVerdict CCommsdat72Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test medium texts as field value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing text as field values in CCommsdat72Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDDialOutISPRecord *record = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
	CleanupStack::PushL(record);
	
	//Create a new dial out ISP record
	record->SetRecordId(KCDNewRecordRequest);
				
	record->iDescription.SetL(KText50);
		
	TRAPD(storeErr, record->StoreL(*cmdbSession));
		
	if(storeErr == KErrNone) 
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//--------------------------------------------------------------------------------------------------------------
_LIT(KTextGreaterThan50, 
"A text; which is of 51 characters:AAAAAAAAAAAAAAAA+");

CCommsdat73Step::CCommsdat73Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat73Step);                                                                                           
	}

TVerdict CCommsdat73Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test medium texts as field value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing text as field values in CCommsdat73Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDDialOutISPRecord *record = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
	CleanupStack::PushL(record);
	
	//Create a new dial out ISP record
	record->SetRecordId(KCDNewRecordRequest);
				
	record->iDescription.SetL(KTextGreaterThan50);
		
	TRAPD(storeErr, record->StoreL(*cmdbSession));
	
    INFO_PRINTF2(_L("storeErr = %d"), storeErr);
    
	if(storeErr == KErrOverflow) 
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//--------------------------------------------------------------------------------------------------------------
