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
// CCommsdat100Step
// CCommsdat101Step
// CCommsdat102Step
// CCommsdat103Step
// CCommsdat104Step
// CCommsdat105Step
// 
//

/**
 @file commsdatValidationTests.cpp
*/
#include "commsdatValidationTests.h"
#include "CommsDatInternalDefs.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
#include <commsdat_partner.h>
#include <commsdattypesv1_1_partner.h>
#include <commsdattypeinfov1_1_internal.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

namespace
{
TBool CheckConnPrefValidation(CommsDat::CMDBSession* aCDSession_v11, 
							  CommsDat::CMDBSession* aCDSession_vLatest)
	{
	TBool result = EFalse;
	
	CCDConnectionPrefsRecord* connPrefRec1 = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec1);
    
    //for the mapped APPriorityselPol field
    CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolField = new(ELeave)CMDBRecordLink<CCDAccessPointRecord>();
    CleanupStack::PushL(apPrioritySelPolField);
    //the connPref records are mapped in the last APPrioritySelPol record (from the meshpreface.cfg)
    apPrioritySelPolField->SetRecordId(254);
    
    connPrefRec1->SetRecordId(1);
    connPrefRec1->LoadL(*aCDSession_v11);
    
    TInt rank = connPrefRec1->iRanking; 
    
    switch (rank)
    	{
    	case 1:
    		{
    		//rank 1 is mapped into the iAP1 field
    		apPrioritySelPolField->SetElementId(apPrioritySelPolField->ElementId() | KCDTIdAp1);
    		break;
    		}
    	case 2:
    		{
    		//rank 2 is mapped into the iAP2 field
    		apPrioritySelPolField->SetElementId(apPrioritySelPolField->ElementId() | KCDTIdAp2);
    		break;
    		}
    	default:
    		User::Leave(KErrNotFound);
    	}
    
    apPrioritySelPolField->LoadL(*aCDSession_vLatest);
    
    //at this point we know the elementId of the generated IPProto level AP which links aganst
    //the link level AP through its' CustomSelectionPolicy field. The value of this field in the
    //given IPProto AP must be the same as the recordID of the linked IAP record from the ConnPref
    //record.
    
    //so let's read the customSelectionPolicy field from the IPProto AP
    CMDBField<TInt>* custSelPolField = new(ELeave)CMDBField<TInt>(KCDTIdCustomSelectionPolicy);
    CleanupStack::PushL(custSelPolField);
    
    custSelPolField->SetRecordId((*apPrioritySelPolField & KCDMaskShowRecordId) >> 8);
    
    custSelPolField->LoadL(*aCDSession_vLatest);
    
    if (connPrefRec1->iDefaultIAP == *custSelPolField)
    	{
    	result = ETrue;
    	}
    
    CleanupStack::PopAndDestroy(custSelPolField);
    CleanupStack::PopAndDestroy(apPrioritySelPolField);
    CleanupStack::PopAndDestroy(connPrefRec1);
    
    return result;
	}

TInt GetAPNumberL(const TInt aTierValaue, CommsDat::CMDBSession* aCDSession_vLatest)
	{
	CMDBRecordSet<CCDAccessPointRecord>* ipprotoAPs = new(ELeave)CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
	CleanupStack::PushL(ipprotoAPs);
	
	CCDAccessPointRecord* primedIPProtoAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(primedIPProtoAP);
	
	primedIPProtoAP->iTier = aTierValaue;
	
	ipprotoAPs->iRecords.AppendL(primedIPProtoAP);
	//the ownership of the primed record is now at the recordset
	CleanupStack::Pop(primedIPProtoAP);
	
	ipprotoAPs->FindL(*aCDSession_vLatest);
	
	TInt apNum = ipprotoAPs->iRecords.Count();
	
	CleanupStack::PopAndDestroy(ipprotoAPs);
	
	return apNum;
	}

}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

void CConnectionPrefValidationTestBase::deleteAllL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CMDBRecordSet<CCDConnectionPrefsRecord> *recordSet1 = new (ELeave) CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(recordSet1);
	TRAPD(loadErr, recordSet1->LoadL(*cmdbSession));
	if(loadErr == KErrNone)
		{
		recordSet1->DeleteL(*cmdbSession);
		}
	CleanupStack::PopAndDestroy(recordSet1);
		
	CMDBRecordSet<CCDIAPRecord> *recordSet2 = new(ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(recordSet2);
	
	TRAP(loadErr, recordSet2->LoadL(*cmdbSession));
	if(loadErr == KErrNone)
		{
		recordSet2->DeleteL(*cmdbSession);
		}
		
	CleanupStack::PopAndDestroy(recordSet2);
	CleanupStack::PopAndDestroy(cmdbSession);
	}
	
TVerdict CConnectionPrefValidationTestBase::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in doTestStepPreambleL() and deleting all connection preferences record"));
	
	deleteAllL();
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CConnectionPrefValidationTestBase::doTestStepPostambleL()
	{
	deleteAllL();
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat100Step::CCommsdat100Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat100Step);                                                                                           
	}

TVerdict CCommsdat100Step::doTestStepL()
/**
 * @return - TVerdict code
 * The IAP record, conn pref record refers to, has to have a valid id, hence less 
 * than KCDMaxRecords.
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat100Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(KConnPrefRecordId );
	cpRecord->iDefaultIAP = KCDMaxRecords;
	TRAPD(storeErr, cpRecord->StoreL(*cmdbSession));
	
	if(storeErr == KErrNotFound)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cpRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
        
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat101Step::CCommsdat101Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat101Step);                                                                                           
	}

TVerdict CCommsdat101Step::doTestStepL()
/**
 * @return - TVerdict code
 * The ranking of conn pref record must be eiher 1 or 2, testing with an invalid value
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat101Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(KConnPrefRecordId);
	cpRecord->iDefaultIAP = 1; // An arbitrary but still valid IAP record id
	cpRecord->iRanking = 3;    // Invalid value, should cause KErrOverflow
	TRAPD(storeErr, cpRecord->StoreL(*cmdbSession));
	INFO_PRINTF2(_L("store returns %d"), storeErr);
	
	if(storeErr == KErrOverflow)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cpRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat102Step::CCommsdat102Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat102Step);                                                                                           
	}

TVerdict CCommsdat102Step::doTestStepL()
/**
 * @return - TVerdict code
 * Conn pref records with a rank of 2 are not permitted to be incoming
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat102Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(KConnPrefRecordId);
	cpRecord->iDefaultIAP = 1;  // An arbitrary, but valiad IAP record id
	cpRecord->iRanking = 2;     // A invalid ranking for incoming connection
	cpRecord->iDirection = ECommDbConnectionDirectionIncoming;
	TRAPD(storeErr, cpRecord->StoreL(*cmdbSession));
	
	if(storeErr == KErrNotSupported)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cpRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat103Step::CCommsdat103Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat103Step);                                                                                           
	}

const TInt KRanking = 1; 

TVerdict CCommsdat103Step::doTestStepL()
/**
 * @return - TVerdict code
 * Two conn pref records with the same ranking are not allowed
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat103Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord); 
	
	//Insert an IAP record; which will be referred by conn pref records
	iapRecord->SetRecordId(KIAPRecordId); 
	iapRecord->iServiceType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iServiceType= TPtrC(DIAL_OUT_ISP);
	iapRecord->iService=1;
	iapRecord->iBearerType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iBearerType=TPtrC(LAN_BEARER);
	iapRecord->iBearer=1;
    
	iapRecord->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(iapRecord);
	
	CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord1);
	
	cpRecord1->SetRecordId(KConnPrefRecordId);
	cpRecord1->iDefaultIAP = KIAPRecordId;  //A valid IAP record id 
	cpRecord1->iRanking = KRanking;         //A valid ranking
	cpRecord1->iDirection = ECommDbConnectionDirectionOutgoing;  //A valid direction
	TRAPD(storeErr1, cpRecord1->StoreL(*cmdbSession));  // Should raise KErrNone
	INFO_PRINTF2(_L("first store returns %d"), storeErr1);
	
	CCDConnectionPrefsRecord *cpRecord2 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord2);
	
	cpRecord2->SetRecordId(KConnPrefRecordId + 1);//Just another conn pref record
	cpRecord2->iDefaultIAP = KIAPRecordId ;       //A valid IAP record id
	cpRecord2->iRanking = KRanking;               //Same ranking with above one; should cause KErrAlreadyExists
	cpRecord2->iDirection = ECommDbConnectionDirectionOutgoing; // A valid direction
	TRAPD(storeErr2, cpRecord2->StoreL(*cmdbSession));// Should raise KErrAlreadyExists
	INFO_PRINTF2(_L("second store returns %d"), storeErr2);
	
	if(storeErr1 == KErrNone && storeErr2 == KErrAlreadyExists)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cpRecord2);
	CleanupStack::PopAndDestroy(cpRecord1);
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat104Step::CCommsdat104Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat104Step);                                                                                           
	}

TVerdict CCommsdat104Step::doTestStepL()
/**
 * @return - TVerdict code
 * An outgoing IAP; cannot be referred as incoming
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat104Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord); 
	
	//Insert an outgoing IAP record
	iapRecord->SetRecordId(KIAPRecordId);
	iapRecord->iServiceType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iServiceType= TPtrC(DIAL_OUT_ISP);  // Outgoing IAP
	TRAPD(storeErr1, iapRecord->StoreL(*cmdbSession)); //Should return KErrNone
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(KConnPrefRecordId);
	cpRecord->iRanking = 1;    //A valid ranking
	cpRecord->iDirection = ECommDbConnectionDirectionIncoming; //Should cause KErrArgument
	cpRecord->iDefaultIAP = KIAPRecordId;  //The IAP record inserted above
	cpRecord->iBearerSet = KCommDbBearerCSD;  //Type of bearer
	TRAPD(storeErr2, cpRecord->StoreL(*cmdbSession)); //Should return KErrArgument
	
	if(storeErr1 == KErrNone && storeErr2 == KErrArgument)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cpRecord);
	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat105Step::CCommsdat105Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat105Step);                                                                                           
	}

TVerdict CCommsdat105Step::doTestStepL()
/**
 * @return - TVerdict code
 * A valid combination: an outgoing IAP is referred with an outgoing direction 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat105Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);
	
	// An outgoing IAP record is inserted
	iapRecord->SetRecordId(KIAPRecordId);
	iapRecord->iServiceType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iServiceType= TPtrC(DIAL_OUT_ISP);
	iapRecord->iService=1;
	iapRecord->iBearerType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iBearerType=TPtrC(LAN_BEARER);
	iapRecord->iBearer=1;
	TRAPD(storeErr1, iapRecord->StoreL(*cmdbSession)); //Should return KErrNone
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(KConnPrefRecordId);
	cpRecord->iRanking = 1;     // A valid ranking
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;  //A valid direction
	cpRecord->iDefaultIAP = KIAPRecordId;  //IAP record inserted above
	cpRecord->iBearerSet = KCommDbBearerLAN; // A valid bearer type
	TRAPD(storeErr2, cpRecord->StoreL(*cmdbSession)); //Should return KErrNone
	
	if(storeErr1 == KErrNone && storeErr2 == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cpRecord);
	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat106Step::CCommsdat106Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat106Step);                                                                                           
	}

TVerdict CCommsdat106Step::doTestStepL()
/**
 * @return - TVerdict code
 Test whether iDefaultIAP accepts zero as a valid value, since phones get shipped without any predefined IAPs
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat106Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	//Even if the defaultIAP field won't contain anything an IAP has to be inserted because this will
	//be the base of the link layer AP generation - CommsDat will search for an IAP based on the
	//serviceType and Direction settings in the ConnPref record.
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord);
	
	// An outgoing IAP record is inserted which satisfies the settings of the ConnPref record
	iapRecord->SetRecordId(KIAPRecordId);
	iapRecord->iServiceType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iServiceType= TPtrC(DIAL_OUT_ISP);
	iapRecord->iService=1;
	iapRecord->iBearerType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iBearerType=TPtrC(LAN_BEARER);
	iapRecord->iBearer=1;
	TRAPD(storeErr, iapRecord->StoreL(*cmdbSession)); //Should return KErrNone
	
	if (storeErr == KErrNone)
		{
		cpRecord->SetRecordId(KConnPrefRecordId );
		cpRecord->iDefaultIAP = 0;
		cpRecord->iDirection.SetL(ECommDbConnectionDirectionOutgoing);
		cpRecord->iBearerSet.SetL(KCommDbBearerCSD);
		cpRecord->iRanking = 1;     // A valid ranking
				
		TRAP(storeErr, cpRecord->StoreL(*cmdbSession));
		
		if(storeErr == KErrNone)
			{
			SetTestStepResult(EPass);
			}
		}
	CleanupStack::PopAndDestroy(iapRecord);
	CleanupStack::PopAndDestroy(cpRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
        
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat107Step::CCommsdat107Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat107Step);
	}

TVerdict CCommsdat107Step::doTestStepL()
/**
 * @return - TVerdict code
 Test whether iDefaultIAP accepts zero as a valid value, since phones get shipped without any predefined IAPs
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Modifying a link value to 0 in CCommsdat107Step::doTestStepL() "));
	
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDVPNServiceRecord* vpnRec = static_cast<CCDVPNServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVPNServiceRecord));
    CleanupStack::PushL(vpnRec);
    
    vpnRec->SetRecordId(2);
    vpnRec->LoadL(*cmdbSession);
	
    vpnRec->iServiceIAP = 0;
    
    vpnRec->ModifyL(*cmdbSession);
    
    vpnRec->LoadL(*cmdbSession);
    
    //this record is stored by ced without the homeIAP field.
    CCDVPNServiceRecord* vpnRec01 = static_cast<CCDVPNServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVPNServiceRecord));
    CleanupStack::PushL(vpnRec01);
    
    vpnRec01->SetRecordId(4);
    vpnRec01->LoadL(*cmdbSession);
    
    //at this point the stored and the modified vpn service records should have and initial
    //0 value in their homeIAP fields. The NotZero flag of the fields should be false when 
    //reading from the DB.
    
    if (NULL == vpnRec01->iServiceIAP &&
    	NULL == vpnRec->iServiceIAP &&
    	NULL == (vpnRec01->ElementId() & KCDNotNullFlag) &&
    	NULL == (vpnRec->ElementId() & KCDNotNullFlag) )
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(vpnRec01);
    CleanupStack::PopAndDestroy(vpnRec);
	CleanupStack::PopAndDestroy(cmdbSession);
        
	return TestStepResult();
	}



#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat332Step::CCommsdat332Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat332Step);                                                                                           
	}

CCommsdat332Step::~CCommsdat332Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat332Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat332Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat332Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat332Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the name of the 1st ConnPref record. ModifyL is called through a 
 * field container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    _LIT(KConnPrefName, "Modified ConnPref Name");
    
    CMDBField<TDesC>* connPrefName = new(ELeave)CMDBField<TDesC>(KCDTIdConnectionPrefsRecord | KCDTIdRecordName);
    CleanupStack::PushL(connPrefName);
    connPrefName->SetRecordId(1);
    
    connPrefName->SetMaxLengthL(KConnPrefName().Length());
    connPrefName->SetL(KConnPrefName);
    
    connPrefName->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(connPrefName);
    
    CMDBField<TDesC>* connPrefName1 = new(ELeave)CMDBField<TDesC>(KCDTIdConnectionPrefsRecord | KCDTIdRecordName);
    CleanupStack::PushL(connPrefName1);
    connPrefName1->SetRecordId(1);
    
    connPrefName1->LoadL(*iCDSession_v11);
	
    TPtrC nameField(*connPrefName1);
    
    if (0 == nameField.Compare(KConnPrefName))
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(connPrefName1);
    
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat333Step::CCommsdat333Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat333Step);                                                                                           
	}

CCommsdat333Step::~CCommsdat333Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat333Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat333Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat333Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat333Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the name of the 1st ConnPref record. ModifyL is called through a 
 * record container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    _LIT(KConnPrefName, "DefaultRecordName-1");
    
    CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec);
    
    connPrefRec->SetRecordId(1);
    
    connPrefRec->iRecordName.SetMaxLengthL(KConnPrefName().Length());
    connPrefRec->iRecordName.SetL(KConnPrefName);
    
    connPrefRec->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefRec);
    
    CMDBField<TDesC>* connPrefName1 = new(ELeave)CMDBField<TDesC>(KCDTIdConnectionPrefsRecord | KCDTIdRecordName);
    CleanupStack::PushL(connPrefName1);
    connPrefName1->SetRecordId(1);
    
    connPrefName1->LoadL(*iCDSession_v11);
	
    TPtrC nameField(*connPrefName1);
    
    if (0 == nameField.Compare(KConnPrefName))
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(connPrefName1);
    
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat334Step::CCommsdat334Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat334Step);                                                                                           
	}

CCommsdat334Step::~CCommsdat334Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat334Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat334Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat334Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat334Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the DialogPref field of the 1st ConnPref record. 
 * As a result _NOT_ the prompt field of the ipproto Tier record has to be modified
 * but instead the full TMDBElementId of the connpref record should be stored
 * in the cprConfig field of the generated ipproto AP. 
 * 
 * ModifyL is called through a record container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec);
    
    connPrefRec->SetRecordId(1);
    connPrefRec->LoadL(*iCDSession_v11);
    
    connPrefRec->iDialogPref = ECommDbDialogPrefPrompt;
    connPrefRec->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefRec);
    
    
    CCDConnectionPrefsRecord* connPrefRec1 = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec1);
    
    connPrefRec1->SetRecordId(1);
    connPrefRec1->LoadL(*iCDSession_v11);
    
    CMDBField<TUint32>* connPrefPromptField = &(connPrefRec1->iDialogPref);
   
    CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(apRec);
    
    apRec->iCprConfig = connPrefRec1->ElementId();
    
    if ( (ECommDbDialogPrefPrompt == *connPrefPromptField) &&
    		apRec->FindL(*iCDSession_vLatest) )
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(apRec);
    CleanupStack::PopAndDestroy(connPrefRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat335Step::CCommsdat335Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat335Step);                                                                                           
	}

CCommsdat335Step::~CCommsdat335Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat335Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat335Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat335Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat335Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the DialogPref field of the 1st ConnPref record. ModifyL is called through a 
 * field container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TUint32>* connPrefPromptField = new(ELeave)CMDBField<TUint32>(KCDTIdDialogPref);
    CleanupStack::PushL(connPrefPromptField);
    
    connPrefPromptField->SetRecordId(1);
    connPrefPromptField->LoadL(*iCDSession_v11);
    
    *connPrefPromptField = ECommDbDialogPrefDoNotPrompt;
    connPrefPromptField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefPromptField);
    
    
    CCDConnectionPrefsRecord* connPrefRec1 = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec1);
    
    connPrefRec1->SetRecordId(1);
    connPrefRec1->LoadL(*iCDSession_v11);
    
    CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(apRec);
    
    apRec->iCprConfig.SetL(connPrefRec1->iDefaultIAP);
    apRec->FindL(*iCDSession_vLatest);
    
    CMDBField<TUint32>* connPrefPromptField1 = &(connPrefRec1->iDialogPref);
    
    if ( (ECommDbDialogPrefDoNotPrompt == *connPrefPromptField1) &&
    		apRec->iCprConfig == apRec->iCustomSelectionPolicy)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(apRec);
    CleanupStack::PopAndDestroy(connPrefRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat336Step::CCommsdat336Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat336Step);                                                                                           
	}

CCommsdat336Step::~CCommsdat336Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat336Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat336Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat336Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat336Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the BearerSet field of the 1st ConnPref record. ModifyL is called through a 
 * record container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec);
    
    connPrefRec->SetRecordId(1);
    connPrefRec->LoadL(*iCDSession_v11);
    
    connPrefRec->iBearerSet = KCommDbBearerWLAN;
    
    connPrefRec->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefRec);
    
    
    CCDConnectionPrefsRecord* connPrefRec1 = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec1);
    
    connPrefRec1->SetRecordId(1);
    connPrefRec1->LoadL(*iCDSession_v11);
    
    if (KCommDbBearerWLAN == connPrefRec1->iBearerSet)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(connPrefRec1);    
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat337Step::CCommsdat337Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat337Step);                                                                                           
	}

CCommsdat337Step::~CCommsdat337Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat337Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat337Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat337Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat337Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the BearerSet field of the 1st ConnPref record. ModifyL is called through a 
 * field container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TUint32>* connPrefBearerField = new(ELeave)CMDBField<TUint32>(KCDTIdBearerSet);
    CleanupStack::PushL(connPrefBearerField);
    
    connPrefBearerField->SetRecordId(1);    
    *connPrefBearerField = KCommDbBearerPAN;
    
    connPrefBearerField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefBearerField);
    
    
    CMDBField<TUint32>* connPrefBearerField1 = new(ELeave)CMDBField<TUint32>(KCDTIdBearerSet);
    CleanupStack::PushL(connPrefBearerField1);
    
    connPrefBearerField1->SetRecordId(1);
    connPrefBearerField1->LoadL(*iCDSession_v11);
    
    if (KCommDbBearerPAN == *connPrefBearerField1)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(connPrefBearerField1);    
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
//changing the default IAP field in the ConnPref rec
CCommsdat338Step::CCommsdat338Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat338Step);
	}

CCommsdat338Step::~CCommsdat338Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat338Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat338Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat338Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat338Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the IAP field of the 1st ConnPref record to a valid IAP. ModifyL is called through a 
 * record container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec);
    
    connPrefRec->SetRecordId(1);
    connPrefRec->LoadL(*iCDSession_v11);
    
    connPrefRec->iDefaultIAP = 1;
    
    connPrefRec->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefRec);  
    
    if (CheckConnPrefValidation(iCDSession_v11, iCDSession_vLatest))
    	{
    	SetTestStepResult(EPass);
    	}
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
//changing the default IAP field in the ConnPref rec
CCommsdat339Step::CCommsdat339Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat339Step);
	}

CCommsdat339Step::~CCommsdat339Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat339Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat339Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat339Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat339Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the IAP field of the 1st ConnPref record to a valid IAP. ModifyL is called through a 
 * field container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* defIAPField = new(ELeave)CMDBField<TInt>(KCDTIdIAPLink);
    CleanupStack::PushL(defIAPField);
    
    defIAPField->SetRecordId(1);
    *defIAPField = 3;
    
    defIAPField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(defIAPField);  
    
    if (CheckConnPrefValidation(iCDSession_v11, iCDSession_vLatest))
    	{
    	SetTestStepResult(EPass);
    	}
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
//changing the default IAP field in the ConnPref rec
CCommsdat340Step::CCommsdat340Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat340Step);
	}

CCommsdat340Step::~CCommsdat340Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat340Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat340Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat340Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    //I know it's a 'C' object so it's initialized to 0 but...
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat340Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the IAP field of the 1st ConnPref record to a _NON_ valid IAP. ModifyL is called through a 
 * record container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec);
    
    connPrefRec->SetRecordId(1);
    connPrefRec->LoadL(*iCDSession_v11);
    
    connPrefRec->iDefaultIAP = 30;
    
    TRAPD(err, connPrefRec->ModifyL(*iCDSession_v11));
    
    if (KErrNotFound == err)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(connPrefRec);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
//changing the default IAP field in the ConnPref rec
CCommsdat341Step::CCommsdat341Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat341Step);
	}

CCommsdat341Step::~CCommsdat341Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat341Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat341Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat341Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    //I know it's a 'C' object so it's initialized to 0 but...
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat341Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the IAP field of the 1st ConnPref record to a _NON_ valid IAP. ModifyL is called through a 
 * field container.
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* defaultIAP = new(ELeave)CMDBField<TInt>(KCDTIdIAPLink);
    CleanupStack::PushL(defaultIAP);
    
    defaultIAP->SetRecordId(1);    
    *defaultIAP = 30;
    
    TRAPD(err, defaultIAP->ModifyL(*iCDSession_v11));
    
    if (KErrNotFound == err)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(defaultIAP);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat342Step::CCommsdat342Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat342Step);
	}

CCommsdat342Step::~CCommsdat342Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat342Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat342Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat342Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat342Step::doTestStepL()
/**
 * @return - TVerdict code
 * Storing a new connpref record with rank2 and delete the connpref record with rank1. 
 * 
 * Consecvencies:
 * 1. There will be a default AP generated because the rank1 record is deleted.
 *    (customselpol record will be CCDAccessPointRecord::KNoPolicy of it)
 * 2. This record will be linked form the APPrioritySelPol record (from the AP1 field)
 * 3. Because the rank2 connpref record is a prompting record the generated IPProto AP
 *    will be a prompting AP too. Meaning that the full TMDBElementId of the connpref
 *    record (record2) will be stored in the CprConfig field of the generated IPProto AP.
 * 
 * Delete is called on a record container 
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRec);
	
	iapRec->SetRecordId(10);
	iapRec->LoadL(*iCDSession_v11);
	
	cpRecord->SetRecordId(2);
	cpRecord->iDefaultIAP = iapRec->RecordId();
	cpRecord->iRanking = 2;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt;
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iapRec);
	CleanupStack::PopAndDestroy(cpRecord);
	
	//and now delete the 1st connpref record causing a hole in the mapped
	//APPrioritySelPol record (AP1 link will be 0 but the APCount field of the rec
	//should be still 2)
	
	CCDConnectionPrefsRecord *cpRec = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRec);
	
	cpRec->SetRecordId(1);
	cpRec->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRec);
	
	
	CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord1);
	
	cpRecord1->SetRecordId(1);
	TRAPD(err, cpRecord1->LoadL(*iCDSession_v11));
	
	if (KErrNotFound == err)
		{
		//ok, the connPref record is deleted
		CCDAPPrioritySelectionPolicyRecord* apPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(apPrioritySelPolRec);
		//The last record is used for this mapping
		apPrioritySelPolRec->SetRecordId(254);
		
		apPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL != apPrioritySelPolRec->iAp1 &&
			 2 == apPrioritySelPolRec->iApCount )
			{
			//1st ipprotoAP should contain CCDAccessPointRecord::KNoPolicy in its' customSelPol field
			//2nd ipprotoAP should contain the full TMDBElementID of the 2nd connpref record
			CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(apRec);
			
			apRec->SetElementId(apPrioritySelPolRec->iAp1);
			apRec->LoadL(*iCDSession_vLatest);
			
			TPtrC recName(apRec->iRecordName);
			_LIT(KDefIPProtoRecName, "IPProtoDefault");
			
			if (CCDAccessPointRecord::KNoPolicy == apRec->iCustomSelectionPolicy &&
				0 == recName.Compare(KDefIPProtoRecName))
				{
				//ok, the 1st ipproto AP is fine
				apRec->SetElementId(apPrioritySelPolRec->iAp2);
				apRec->LoadL(*iCDSession_vLatest);
				
				CCDConnectionPrefsRecord *cpRecord2 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
				CleanupStack::PushL(cpRecord2);
				
				cpRecord2->SetRecordId(2);
				cpRecord2->LoadL(*iCDSession_v11);
				
				if (apRec->iCprConfig == cpRecord2->ElementId())
					{
					SetTestStepResult(EPass);
					}
				
				CleanupStack::PopAndDestroy(cpRecord2);
				}
			
			CleanupStack::PopAndDestroy(apRec);
			}
		
		CleanupStack::PopAndDestroy(apPrioritySelPolRec);
		}

	CleanupStack::PopAndDestroy(cpRecord1);
	
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat343Step::CCommsdat343Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat343Step);
	}

CCommsdat343Step::~CCommsdat343Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat343Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat343Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat343Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat343Step::doTestStepL()
/**
 * @return - TVerdict code
 * Storing a new connpref record with rank1 and delete the connpref record with rank2. Delete 
 * is called on a record container 
 * This test step is dependent on CCommsdat342Step
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRec);
	
	iapRec->SetRecordId(3);
	iapRec->LoadL(*iCDSession_v11);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = iapRec->RecordId();
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt;
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iapRec);
	CleanupStack::PopAndDestroy(cpRecord);
	
	//and now delete the 2nd connpref record
	
	CCDConnectionPrefsRecord *cpRec = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRec);
	
	cpRec->SetRecordId(2);
	cpRec->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRec);
	
	CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord1);
	
	cpRecord1->SetRecordId(2);
	TRAPD(err, cpRecord1->LoadL(*iCDSession_v11));
	
	if (KErrNotFound == err)
		{
		//ok, the connPref record is deleted
		CCDAPPrioritySelectionPolicyRecord* apPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(apPrioritySelPolRec);
		//The last record is used for this mapping
		apPrioritySelPolRec->SetRecordId(254);
		
		apPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL == apPrioritySelPolRec->iAp2 &&
			 1 == apPrioritySelPolRec->iApCount )
			{
			//ok, we have only 1 valid link in the mapped APPrioritySelPol record (the 1st one)
			CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
			CleanupStack::PushL(tierRec);
			
			//we just know that the IPProto tier record is the 2nd
			tierRec->SetRecordId(2);
			tierRec->LoadL(*iCDSession_vLatest);
			
			CMDBRecordLink<CCDAccessPointRecord>* apPriorityVal = &(apPrioritySelPolRec->iAp1);
			
			TInt apPriority = apPrioritySelPolRec->iAp1;
			TInt tierDefAP = tierRec->iDefaultAccessPoint;
			
			if (apPriority == tierDefAP)
				{
				//ok, the 2 links are the same
				CCDAccessPointRecord* ipProtoAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
				CleanupStack::PushL(ipProtoAPRec);
				
				ipProtoAPRec->SetElementId(*apPriorityVal);
				ipProtoAPRec->LoadL(*iCDSession_vLatest);
				
				if (3 == ipProtoAPRec->iCustomSelectionPolicy)
					{
					//remember, when we created the the connpref rec with rank1 the IAP which is
					//linked was the 3rd
					SetTestStepResult(EPass);
					}
				
				CleanupStack::PopAndDestroy(ipProtoAPRec);
				}
			CleanupStack::PopAndDestroy(tierRec);
			}
		
		CleanupStack::PopAndDestroy(apPrioritySelPolRec);
		}

	CleanupStack::PopAndDestroy(cpRecord1);
	
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat344Step::CCommsdat344Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat344Step);
	}

CCommsdat344Step::~CCommsdat344Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat344Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat344Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat344Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat344Step::doTestStepL()
/**
 * @return - TVerdict code
 * deleting the defaultIAP field from the connpref record with rank1
 * Delete is called through a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
    
	cpRecord->iRanking = 1;
	cpRecord->FindL(*iCDSession_v11);
    
    CMDBField<TInt>* connPrefDefIAPField = new(ELeave)CMDBField<TInt>(KCDTIdIAPLink);
    CleanupStack::PushL(connPrefDefIAPField);
    
    connPrefDefIAPField->SetRecordId(cpRecord->RecordId());
    connPrefDefIAPField->DeleteL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefDefIAPField);
    CleanupStack::PopAndDestroy(cpRecord);
    
    
	CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord1);
	
	cpRecord1->SetRecordId(1);
	cpRecord1->LoadL(*iCDSession_v11);
	
	if (NULL == cpRecord1->iDefaultIAP)
		{
		//ok, the defaultIAP link is 0
		CCDAPPrioritySelectionPolicyRecord* apPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(apPrioritySelPolRec);
		//The last record is used for this mapping
		apPrioritySelPolRec->SetRecordId(254);
		
		apPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL != apPrioritySelPolRec->iAp1 &&
			 1 == apPrioritySelPolRec->iApCount )
			{
			//1st ipprotoAP should contain CCDAccessPointRecord::KNoPolicy in its' customSelPol field
			CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(apRec);
			
			apRec->SetElementId(apPrioritySelPolRec->iAp1);
			apRec->LoadL(*iCDSession_vLatest);
			
			TPtrC recName(apRec->iRecordName);
			_LIT(KDefIPProtoRecName, "IPProtoDefault");
			
			if (CCDAccessPointRecord::KNoPolicy == apRec->iCustomSelectionPolicy &&
				0 == recName.Compare(KDefIPProtoRecName) &&
				0 == apRec->iCprConfig)
				{
				CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
				CleanupStack::PushL(tierRec);
				
				//we just know that the IPProto tier record is the 2nd
				tierRec->SetRecordId(2);
				tierRec->LoadL(*iCDSession_vLatest);
				
				TInt tierLink = tierRec->iDefaultAccessPoint;
				TInt apPrioritySelPolLink = apPrioritySelPolRec->iAp1;
				
				if ( tierRec->iDefaultAccessPoint == (apRec->ElementId() & ~KCDMaskShowFieldType) &&
					 tierLink == apPrioritySelPolLink )
					{
					SetTestStepResult(EPass);
					}
				
				CleanupStack::PopAndDestroy(tierRec);
				}
			
			CleanupStack::PopAndDestroy(apRec);
			}
		
		CleanupStack::PopAndDestroy(apPrioritySelPolRec);
		}
	
	CleanupStack::PopAndDestroy(cpRecord1);
	
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat345Step::CCommsdat345Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat345Step);
	}

CCommsdat345Step::~CCommsdat345Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat345Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat345Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat345Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat345Step::doTestStepL()
/**
 * @return - TVerdict code
 * deleting the defaultIAP field from the connpref record with rank2
 * Delete is called through a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
	CCDConnectionPrefsRecord *cpRec = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRec);
	
	CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRec);
	
	iapRec->SetRecordId(3);
	iapRec->LoadL(*iCDSession_v11);
	
	//1st inserting a new connpref record with rank2
	cpRec->SetRecordId(2);
	cpRec->iDefaultIAP = iapRec->RecordId();
	cpRec->iRanking = 2;
	cpRec->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRec->iBearerSet = KCommDbBearerCSD;
	cpRec->iDialogPref = ECommDbDialogPrefPrompt;
	
	cpRec->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iapRec);
	CleanupStack::PopAndDestroy(cpRec);
    
	//delete the linked IAP record from the DB
	CCDIAPRecord* iapRecToDelete = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRecToDelete);
	
	iapRecToDelete->SetRecordId(3);
	iapRecToDelete->DeleteL(*iCDSession_vLatest);
	CleanupStack::PopAndDestroy(iapRecToDelete);
	
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
    
	cpRecord->iRanking = 2;
	cpRecord->FindL(*iCDSession_v11);
    
    CMDBField<TInt>* connPrefDefIAPField = new(ELeave)CMDBField<TInt>(KCDTIdIAPLink);
    CleanupStack::PushL(connPrefDefIAPField);
    
    connPrefDefIAPField->SetRecordId(cpRecord->RecordId());
    connPrefDefIAPField->DeleteL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefDefIAPField);
    CleanupStack::PopAndDestroy(cpRecord);
    
    
	CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord1);
	
	cpRecord1->SetRecordId(2);
	cpRecord1->LoadL(*iCDSession_v11);
	
	if (NULL == cpRecord1->iDefaultIAP)
		{
		//ok, the defaultIAP link is 0
		CCDAPPrioritySelectionPolicyRecord* apPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(apPrioritySelPolRec);
		//The last record is used for this mapping
		apPrioritySelPolRec->SetRecordId(254);
		
		apPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL == apPrioritySelPolRec->iAp2 &&
			 1 == apPrioritySelPolRec->iApCount )
			{
			//ok, we have only 1 valid link in the mapped APPrioritySelPol record (the 1st one)
			CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
			CleanupStack::PushL(tierRec);
			
			//we just know that the IPProto tier record is the 2nd
			tierRec->SetRecordId(2);
			tierRec->LoadL(*iCDSession_vLatest);
			
			TInt apPriority = apPrioritySelPolRec->iAp1;
			TInt tierDefAP = tierRec->iDefaultAccessPoint;
			
			if (apPriority == tierDefAP)
				{
				SetTestStepResult(EPass);
				}
			CleanupStack::PopAndDestroy(tierRec);
			}
		
		CleanupStack::PopAndDestroy(apPrioritySelPolRec);
		}
	
	CleanupStack::PopAndDestroy(cpRecord1);
	
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat346Step::CCommsdat346Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat346Step);
	}

CCommsdat346Step::~CCommsdat346Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat346Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat346Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat346Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat346Step::doTestStepL()
/**
 * @return - TVerdict code
 * storing an IAP record and setting the defaultIAP link in the connpref record with ranking1
 * The config file used for this test step doesn't contain any IAP records and the defaultIAP
 * link in the connpref record is 0. In this case CommsDat doesn't generate any 399 path.
 * The modify is called through a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    //insert a new IAP record
	CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecord); 
	
	//Insert an IAP record; which will be referred by conn pref records
	iapRecord->SetRecordId(KCDNewRecordRequest); 
	iapRecord->iServiceType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iServiceType= TPtrC(DIAL_OUT_ISP);
	iapRecord->iService=2;
	iapRecord->iBearerType.SetMaxLengthL(KMaxTextLength);
	iapRecord->iBearerType=TPtrC(MODEM_BEARER);
	iapRecord->iBearer=3;
    
	iapRecord->StoreL(*iCDSession_v11);
	
	CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connPrefRec);
	
	connPrefRec->SetRecordId(1);
	connPrefRec->LoadL(*iCDSession_v11);
	
	connPrefRec->iDefaultIAP = iapRecord->RecordId();
	
	connPrefRec->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(connPrefRec);
	
	CCDConnectionPrefsRecord* connPrefRec1 = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connPrefRec1);
	
	connPrefRec1->SetRecordId(1);
	connPrefRec1->LoadL(*iCDSession_v11);
	
	if (iapRecord->RecordId() == connPrefRec1->iDefaultIAP)
		{
		CCDAPPrioritySelectionPolicyRecord* apPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(apPrioritySelPolRec);
		//The last record is used for this mapping
		apPrioritySelPolRec->SetRecordId(254);
		
		apPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL != apPrioritySelPolRec->iAp1 &&
			 1 == apPrioritySelPolRec->iApCount )
			{
			//ok, we have only 1 valid link in the mapped APPrioritySelPol record (the 1st one)
			CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
			CleanupStack::PushL(tierRec);
			
			//we just know that the IPProto tier record is the 2nd
			tierRec->SetRecordId(2);
			tierRec->LoadL(*iCDSession_vLatest);
			
			TInt apPriority = apPrioritySelPolRec->iAp1;
			TInt tierDefAP = tierRec->iDefaultAccessPoint;
			
			if (apPriority == tierDefAP)
				{
				SetTestStepResult(EPass);
				}
			CleanupStack::PopAndDestroy(tierRec);
			}
		
		CleanupStack::PopAndDestroy(apPrioritySelPolRec);
		}
	
	CleanupStack::PopAndDestroy(connPrefRec1);
	CleanupStack::PopAndDestroy(iapRecord);
	
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat347Step::CCommsdat347Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat347Step);
	}

CCommsdat347Step::~CCommsdat347Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat347Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat347Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat347Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat347Step::doTestStepL()
/**
 * @return - TVerdict code
 * modify the defaultIAP field in the connpref rec with rank1 to 0. In this case
 * although it's a modification commsdat has to delete the 399 path...
 * The modify is called through a record container
 * The expected behaviour in this case is that commsDat sets the 0 for the defaultIAP link
 * but leaves the already constructed default path as it is. So nothing is deleted...
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
	CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connPrefRec);
	
	connPrefRec->SetRecordId(1);
	
	connPrefRec->LoadL(*iCDSession_v11);
	
	connPrefRec->iDefaultIAP = 0;
	connPrefRec->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(connPrefRec);
	
	
	CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord1);
	
	cpRecord1->SetRecordId(1);
	cpRecord1->LoadL(*iCDSession_v11);
	
	if (NULL == cpRecord1->iDefaultIAP)
		{
		//ok, the defaultIAP link is 0
		CCDAPPrioritySelectionPolicyRecord* apPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(apPrioritySelPolRec);
		//The last record is used for this mapping
		apPrioritySelPolRec->SetRecordId(254);
		
		apPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL != apPrioritySelPolRec->iAp1 &&
			 1 == apPrioritySelPolRec->iApCount )
			{
			//ok, we have only 1 valid link in the mapped APPrioritySelPol record (the 1st one)
			CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
			CleanupStack::PushL(tierRec);
			
			//we just know that the IPProto tier record is the 2nd
			tierRec->SetRecordId(2);
			tierRec->LoadL(*iCDSession_vLatest);
			
			TInt apPriority = apPrioritySelPolRec->iAp1;
			TInt tierDefAP = tierRec->iDefaultAccessPoint;
			
			if (apPriority == tierDefAP)
				{
				SetTestStepResult(EPass);
				}
			CleanupStack::PopAndDestroy(tierRec);
			}
		
		CleanupStack::PopAndDestroy(apPrioritySelPolRec);
		}
	
	CleanupStack::PopAndDestroy(cpRecord1);
	
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat348Step::CCommsdat348Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat348Step);
	}

CCommsdat348Step::~CCommsdat348Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat348Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat348Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat348Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat348Step::doTestStepL()
/**
 * @return - TVerdict code
 * Deleting an IAP record -> the 'mapped' AP record should be deleted too
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CMDBField<TInt>* mappedLinkLevelAPTagId = new(ELeave)CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
    CleanupStack::PushL(mappedLinkLevelAPTagId);
    
    *mappedLinkLevelAPTagId = 3;
    
    if (mappedLinkLevelAPTagId->FindL(*iCDSession_vLatest))
    	{
    	//ok, at this point we have the 'mapped' link level AP
	    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	    CleanupStack::PushL(iapRec);
	    
	    iapRec->SetRecordId(3);
	    iapRec->DeleteL(*iCDSession_v11);
	    
	    CleanupStack::PopAndDestroy(iapRec);
	    
	    if (! (mappedLinkLevelAPTagId->FindL(*iCDSession_vLatest)) )
	    	{
	    	//this time the mapped link level AP shouldn't be in the db anymore
	    	SetTestStepResult(EPass);
	    	}
    	}
    
    CleanupStack::PopAndDestroy(mappedLinkLevelAPTagId);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat349Step::CCommsdat349Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat349Step);
	}

CCommsdat349Step::~CCommsdat349Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat349Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat349Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat349Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat349Step::doTestStepL()
/**
 * @return - TVerdict code
 * Try to load an IAP record with recordId 255, and 256. (this was a Nokia defect in 9.4)
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec);
    iapRec->SetRecordId(256);
    
    TRAPD(err, iapRec->LoadL(*iCDSession_v11))
    if (KErrNotFound == err)
    	{
        SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(iapRec);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat350Step::CCommsdat350Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat350Step);
	}

CCommsdat350Step::~CCommsdat350Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat350Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat350Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat350Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat350Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the bearertype and bearer from modmbearer to lanbearer.
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec);
    iapRec->SetRecordId(1);
    
    iapRec->LoadL(*iCDSession_v11);
    iapRec->iBearerType.SetMaxLengthL(KMaxTextLength);
	iapRec->iBearerType=TPtrC(LAN_BEARER);
	iapRec->iBearer=1;
    
	iapRec->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapRec);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(1);
    associatedLinkAP->iRecordTag = 1;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC bearerType(iapRec1->iBearerType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	_LIT(KExpectedAPName, "ethintMappedFromIAP1");
    	
    	if ( 0 == bearerType.Compare(TPtrC(LAN_BEARER)) &&
    		 1 == iapRec1->iBearer &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000400 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000300 == associatedLinkAP->iSCpr &&
    		 0x13800400 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat351Step::CCommsdat351Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat351Step);
	}

CCommsdat351Step::~CCommsdat351Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat351Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat351Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat351Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat351Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the bearertype from lanbearer to modmbearer 
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CMDBField<TDesC>* bearerType = new(ELeave)CMDBField<TDesC>(KCDTIdIAPBearerType);
    CleanupStack::PushL(bearerType);
    bearerType->SetRecordId(2);
    
    bearerType->SetMaxLengthL(KMaxTextLength);
    bearerType->SetL(TPtrC(MODEM_BEARER));
    
    bearerType->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(bearerType);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(2);
    associatedLinkAP->iRecordTag = 2;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC bearerTypeName(iapRec1->iBearerType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	_LIT(KExpectedAPName, "pppMappedFromIAP2");
    	
    	if ( 0 == bearerTypeName.Compare(TPtrC(MODEM_BEARER)) &&
    		 2 == iapRec1->iBearer &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat352Step::CCommsdat352Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat352Step);
	}

CCommsdat352Step::~CCommsdat352Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat352Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat352Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat352Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat352Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the bearer from 3 to 2 
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CMDBField<TInt>* bearer = new(ELeave)CMDBField<TInt>(KCDTIdIAPBearer);
    CleanupStack::PushL(bearer);
    bearer->SetRecordId(3);
    
    *bearer = 2;
    
    bearer->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(bearer);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(3);
    associatedLinkAP->iRecordTag = 3;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC bearerTypeName(iapRec1->iBearerType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	_LIT(KExpectedAPName, "pppMappedFromIAP3");
    	
    	if ( 0 == bearerTypeName.Compare(TPtrC(MODEM_BEARER)) &&
    		 2 == iapRec1->iBearer &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat353Step::CCommsdat353Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat353Step);
	}

CCommsdat353Step::~CCommsdat353Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat353Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat353Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat353Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat353Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the servicetype from dialout to lanservice and the service from 3 to 2
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec);
    iapRec->SetRecordId(4);
    
    iapRec->LoadL(*iCDSession_v11);
    iapRec->iServiceType.SetMaxLengthL(KMaxTextLength);
	iapRec->iServiceType=TPtrC(LAN_SERVICE);
	iapRec->iService=2;
    
	iapRec->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapRec);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(4);
    associatedLinkAP->iRecordTag = 4;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	_LIT(KExpectedAPName, "pppMappedFromIAP4");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(LAN_SERVICE)) &&
    		 2 == iapRec1->iService &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat354Step::CCommsdat354Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat354Step);
	}

CCommsdat354Step::~CCommsdat354Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat354Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat354Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat354Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat354Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the name of the iap record
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec);
    iapRec->SetRecordId(5);
    
    _LIT(KNewIAPName, "New IAP record name");
    
    iapRec->LoadL(*iCDSession_v11);
    iapRec->iRecordName.SetMaxLengthL(KNewIAPName().Length());
	iapRec->iRecordName.SetL(KNewIAPName);
    
	iapRec->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapRec);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(5);
    associatedLinkAP->iRecordTag = 5;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	TPtrC iapRecName(iapRec1->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP5");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 4 == iapRec1->iService &&
    		 0 == iapRecName.Compare(KNewIAPName) &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat355Step::CCommsdat355Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat355Step);
	}

CCommsdat355Step::~CCommsdat355Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat355Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat355Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat355Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat355Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the name of the iap record
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CMDBField<TDesC>* iapNameField = new(ELeave)CMDBField<TDesC>(KCDTIdIAPRecord | KCDTIdRecordName);
    CleanupStack::PushL(iapNameField);
    iapNameField->SetRecordId(6);
    
    _LIT(KNewIAPName, "New IAP record name 02");
    
    iapNameField->SetMaxLengthL(KNewIAPName().Length());
    iapNameField->SetL(KNewIAPName);
    
    iapNameField->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapNameField);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(6);
    associatedLinkAP->iRecordTag = 6;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	TPtrC iapRecName(iapRec1->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP6");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 2 == iapRec1->iService &&
    		 0 == iapRecName.Compare(KNewIAPName) &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat356Step::CCommsdat356Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat356Step);
	}

CCommsdat356Step::~CCommsdat356Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat356Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat356Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat356Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat356Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the network weighting field of the iap record
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec);
    iapRec->SetRecordId(7);
    
    iapRec->LoadL(*iCDSession_v11);
    iapRec->iNetworkWeighting = 4;
    
	iapRec->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapRec);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(7);
    associatedLinkAP->iRecordTag = 7;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP7");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 5 == iapRec1->iService &&
    		 4 == iapRec1->iNetworkWeighting &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat357Step::CCommsdat357Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat357Step);
	}

CCommsdat357Step::~CCommsdat357Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat357Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat357Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat357Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat357Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the network weighting field of the iap record
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    CMDBField<TInt>* iapNetworkWeitghtingField = new(ELeave)CMDBField<TInt>(KCDTIdIAPNetworkWeighting);
    CleanupStack::PushL(iapNetworkWeitghtingField);
    iapNetworkWeitghtingField->SetRecordId(8);
    
    *iapNetworkWeitghtingField = 5;
    
    iapNetworkWeitghtingField->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapNetworkWeitghtingField);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(8);
    associatedLinkAP->iRecordTag = 8;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP8");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 3 == iapRec1->iService &&
    		 5 == iapRec1->iNetworkWeighting &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat358Step::CCommsdat358Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat358Step);
	}

CCommsdat358Step::~CCommsdat358Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat358Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat358Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat358Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat358Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the sid  field of the iap record
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    const TInt KDummySid = 0x4324567; 
    
    CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec);
    iapRec->SetRecordId(9);
    
    iapRec->LoadL(*iCDSession_v11);
    iapRec->iAppSid = KDummySid;
    
	iapRec->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapRec);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(9);
    associatedLinkAP->iRecordTag = 9;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "ethintMappedFromIAP9");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(LAN_SERVICE)) &&
    		 2 == iapRec1->iService &&
    		 KDummySid == iapRec1->iAppSid &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000400 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000300 == associatedLinkAP->iSCpr &&
    		 0x13800400 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 KDummySid == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat359Step::CCommsdat359Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat359Step);
	}

CCommsdat359Step::~CCommsdat359Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat359Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat359Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat359Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat359Step::doTestStepL()
/**
 * @return - TVerdict code
 * change the sid  field of the iap record
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    const TInt KDummySid = 0x4324567; 
    
    CMDBField<TInt>* iapSidField = new(ELeave)CMDBField<TInt>(KCDTIdIAPAppSid);
    CleanupStack::PushL(iapSidField);
    iapSidField->SetRecordId(10);
    
    *iapSidField = KDummySid;
    
    iapSidField->ModifyL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapSidField);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(10);
    associatedLinkAP->iRecordTag = 10;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP10");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 6 == iapRec1->iService &&
    		 KDummySid == iapRec1->iAppSid &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 KDummySid == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat360Step::CCommsdat360Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat360Step);
	}

CCommsdat360Step::~CCommsdat360Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat360Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat360Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat360Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat360Step::doTestStepL()
/**
 * @return - TVerdict code
 * delete the sid  field of the iap record
 * delete is called on a field container
 * This test step is dependent on the CCommsdat359Step
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* iapSidField = new(ELeave)CMDBField<TInt>(KCDTIdIAPAppSid);
    CleanupStack::PushL(iapSidField);
    iapSidField->SetRecordId(10);
    
    iapSidField->DeleteL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapSidField);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(10);
    associatedLinkAP->iRecordTag = 10;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP10");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 6 == iapRec1->iService &&
    		 0 == iapRec1->iAppSid &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat361Step::CCommsdat361Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat361Step);
	}

CCommsdat361Step::~CCommsdat361Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat361Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat361Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat361Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat361Step::doTestStepL()
/**
 * @return - TVerdict code
 * delete the network weigthing  field of the iap record
 * delete is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* iapnetworkWeightingField = new(ELeave)CMDBField<TInt>(KCDTIdIAPNetworkWeighting);
    CleanupStack::PushL(iapnetworkWeightingField);
    iapnetworkWeightingField->SetRecordId(11);
    
    iapnetworkWeightingField->DeleteL(*iCDSession_v11);
	
    CleanupStack::PopAndDestroy(iapnetworkWeightingField);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(11);
    associatedLinkAP->iRecordTag = 11;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP11");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(DIAL_OUT_ISP)) &&
    		 2 == iapRec1->iService &&
    		 0 == iapRec1->iNetworkWeighting &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat362Step::CCommsdat362Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat362Step);
	}

CCommsdat362Step::~CCommsdat362Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat362Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat362Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat362Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat362Step::doTestStepL()
/**
 * @return - TVerdict code
 * delete the bearer field of the iap record
 * delete is called on a field container
 * 
 * NOTE: This case shouldn't be allowed since a linked field is set to 0. But because of BC reasons we have to allow
 * this. There are test steps in the CommdbShim (24_03, 24_04, 24_07, 24_08) where a linked field in the GlobalSettings
 * record is set to 0 explicitly!!!!
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* iapBearerField = new(ELeave)CMDBField<TInt>(KCDTIdIAPBearer);
    CleanupStack::PushL(iapBearerField);
    iapBearerField->SetRecordId(12);
    
    iapBearerField->DeleteL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(iapBearerField);
    
    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(12);
    associatedLinkAP->iRecordTag = 12;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC bearerTypeName(iapRec1->iBearerType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "pppMappedFromIAP12");
    	
    	if ( 0 == bearerTypeName.Compare(TPtrC(MODEM_BEARER)) &&
    		 0 == iapRec1->iBearer &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000300 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000500 == associatedLinkAP->iSCpr &&
    		 0x13800300 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat363Step::CCommsdat363Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat363Step);
	}

CCommsdat363Step::~CCommsdat363Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat363Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat363Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat363Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat363Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the lanserviceextension recordId in the lanservice record
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CCDLANServiceRecord* lanServiceRec = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
    CleanupStack::PushL(lanServiceRec);
    lanServiceRec->SetRecordId(2);
    
    lanServiceRec->LoadL(*iCDSession_v11);
    
    lanServiceRec->iServiceExtensionTableRecordId = 2;
    
    lanServiceRec->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(lanServiceRec);

    CCDLANServiceRecord* lanServiceRec1 = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
    CleanupStack::PushL(lanServiceRec1);
    lanServiceRec1->SetRecordId(2);
    
    lanServiceRec1->LoadL(*iCDSession_v11);
    
    if (2 == lanServiceRec1->iServiceExtensionTableRecordId)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(lanServiceRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat364Step::CCommsdat364Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat364Step);
	}

CCommsdat364Step::~CCommsdat364Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat364Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat364Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat364Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat364Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the lanserviceextension recordId in the lanservice record
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* lanServiceRecIdField = new(ELeave)CMDBField<TInt>(KCDTIdLANServiceExtensionTableRecordId);
    CleanupStack::PushL(lanServiceRecIdField);
    lanServiceRecIdField->SetRecordId(2);
    
    *lanServiceRecIdField = 2;
    
    lanServiceRecIdField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(lanServiceRecIdField);

    CCDLANServiceRecord* lanServiceRec1 = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
    CleanupStack::PushL(lanServiceRec1);
    lanServiceRec1->SetRecordId(2);
    
    lanServiceRec1->LoadL(*iCDSession_v11);
    
    if (2 == lanServiceRec1->iServiceExtensionTableRecordId)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(lanServiceRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat365Step::CCommsdat365Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat365Step);
	}

CCommsdat365Step::~CCommsdat365Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat365Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat365Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat365Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat365Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the ISP recordId in the proxies record
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CCDProxiesRecord* proxiesRec = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    CleanupStack::PushL(proxiesRec);
    proxiesRec->SetRecordId(1);
    
    proxiesRec->LoadL(*iCDSession_v11);
    
    proxiesRec->iService = 2;
    
    proxiesRec->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(proxiesRec);

    CCDProxiesRecord* proxiesRec1 = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    CleanupStack::PushL(proxiesRec1);
    proxiesRec1->SetRecordId(1);
    
    proxiesRec1->LoadL(*iCDSession_v11);
    
    if (2 == proxiesRec1->iService)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(proxiesRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat366Step::CCommsdat366Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat366Step);
	}

CCommsdat366Step::~CCommsdat366Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat366Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat366Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat366Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;
	}


TVerdict CCommsdat366Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the ISP recordId in the proxies record
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* proxiesISPField = new(ELeave)CMDBField<TInt>(KCDTIdISP);
    CleanupStack::PushL(proxiesISPField);
    proxiesISPField->SetRecordId(1);
    
    *proxiesISPField = 3;
    
    proxiesISPField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(proxiesISPField);

    CCDProxiesRecord* proxiesRec1 = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    CleanupStack::PushL(proxiesRec1);
    proxiesRec1->SetRecordId(1);
    
    proxiesRec1->LoadL(*iCDSession_v11);
    
    if (3 == proxiesRec1->iService)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(proxiesRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat367Step::CCommsdat367Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat367Step);
	}

CCommsdat367Step::~CCommsdat367Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat367Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat367Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat367Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat367Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change bearer in the IAP record
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* IAPBearerField = new(ELeave)CMDBField<TInt>(KCDTIdIAPBearer);
    CleanupStack::PushL(IAPBearerField);
    IAPBearerField->SetRecordId(1);
    
    *IAPBearerField = 11;
    
    IAPBearerField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(IAPBearerField);

    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(1);
    associatedLinkAP->iRecordTag = 1;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC bearerTypeName(iapRec1->iBearerType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "rawipMappedFromIAP1");
    	
    	if ( 0 == bearerTypeName.Compare(TPtrC(MODEM_BEARER)) &&
    		 11 == iapRec1->iBearer &&
    		 0 == iapRec1->iNetworkWeighting &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000500 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000300 == associatedLinkAP->iSCpr &&
    		 0x13800500 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat368Step::CCommsdat368Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat368Step);
	}

CCommsdat368Step::~CCommsdat368Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat368Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat368Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat368Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat368Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change service in the IAP record
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    CMDBField<TInt>* IAPServiceField = new(ELeave)CMDBField<TInt>(KCDTIdIAPService);
    CleanupStack::PushL(IAPServiceField);
    IAPServiceField->SetRecordId(2);
    
    *IAPServiceField = 2;
    
    IAPServiceField->ModifyL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(IAPServiceField);

    CCDIAPRecord* iapRec1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRec1);
    
    CCDAccessPointRecord* associatedLinkAP = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(associatedLinkAP);
    
    iapRec1->SetRecordId(2);
    associatedLinkAP->iRecordTag = 2;
    
    iapRec1->LoadL(*iCDSession_v11);
    
    if (associatedLinkAP->FindL(*iCDSession_vLatest))
    	{
    	TPtrC serviceTypeName(iapRec1->iServiceType);
    	TPtrC apRecName(associatedLinkAP->iRecordName);
    	
    	_LIT(KExpectedAPName, "ethintMappedFromIAP2");
    	
    	if ( 0 == serviceTypeName.Compare(TPtrC(LAN_SERVICE)) &&
    		 2 == iapRec1->iService &&
    		 0 == iapRec1->iNetworkWeighting &&
    		 0 == apRecName.Compare(KExpectedAPName) &&
    		 0 == associatedLinkAP->iSelectionPolicy &&
    		 0x11800300 == associatedLinkAP->iTier &&
    		 0x12000400 == associatedLinkAP->iMCpr &&
    		 0x12800300 == associatedLinkAP->iCpr &&
    		 0x13000300 == associatedLinkAP->iSCpr &&
    		 0x13800400 == associatedLinkAP->iProtocol &&
    		 0 == associatedLinkAP->iCprConfig &&
    		 0 == associatedLinkAP->iAppSID &&
    		 0 == associatedLinkAP->iCustomSelectionPolicy )
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    
    CleanupStack::PopAndDestroy(associatedLinkAP);
    CleanupStack::PopAndDestroy(iapRec1);
    
    return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat369Step::CCommsdat369Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat369Step);
	}

CCommsdat369Step::~CCommsdat369Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat369Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat369Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat369Step::CreateSessionsL()
	{
    iCDSession_v11 = NULL;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat369Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the TierThredName field in the Tier record (valid name)
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    _LIT(KNewTierName, "New Tier Name");
    
    CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
    CleanupStack::PushL(tierRec);
    
    tierRec->SetRecordId(1);
    tierRec->LoadL(*iCDSession_vLatest);
    
    tierRec->iTierThreadName.SetMaxLengthL(KNewTierName().Length());
    tierRec->iTierThreadName.SetL(KNewTierName);
    
    tierRec->ModifyL(*iCDSession_vLatest);
    
    CleanupStack::PopAndDestroy(tierRec);
    
    CCDTierRecord* tierRec1 = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
    CleanupStack::PushL(tierRec1);
    
    tierRec1->SetRecordId(1);
    tierRec1->LoadL(*iCDSession_vLatest);
    
    TPtrC tierThreadName(tierRec1->iTierThreadName);
    if (0 == tierThreadName.Compare(KNewTierName))
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(tierRec1);
    
    return TestStepResult();
	}


//----------------------------------------------------------------------------------------------------------------
CCommsdat370Step::CCommsdat370Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat370Step);
	}

CCommsdat370Step::~CCommsdat370Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat370Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat370Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat370Step::CreateSessionsL()
	{
    iCDSession_v11 = NULL;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat370Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the TierThredName field in the Tier record (invalid name)
 * Modify is called on a record container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    _LIT(KNewTierName, "New Tier Name which is longer than the permitted");
    
    CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
    CleanupStack::PushL(tierRec);
    
    tierRec->SetRecordId(1);
    tierRec->LoadL(*iCDSession_vLatest);
    
    tierRec->iTierThreadName.SetMaxLengthL(KNewTierName().Length());
    tierRec->iTierThreadName.SetL(KNewTierName);
    
    TRAPD(err, tierRec->ModifyL(*iCDSession_vLatest));
    
    if (KErrTooBig == err)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(tierRec);
    
    return TestStepResult();
    }

//----------------------------------------------------------------------------------------------------------------
CCommsdat371Step::CCommsdat371Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat371Step);
	}

CCommsdat371Step::~CCommsdat371Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat371Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat371Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat371Step::CreateSessionsL()
	{
    iCDSession_v11 = NULL;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat371Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the TierThredName field in the Tier record (valid name)
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    _LIT(KNewTierName, "New Tier Name2");
    
    CMDBField<TDesC>* tierNameField = new(ELeave)CMDBField<TDesC>(KCDTIdTierThreadName);
    CleanupStack::PushL(tierNameField);
    
    tierNameField->SetRecordId(2);
    tierNameField->SetMaxLengthL(KNewTierName().Length());
    tierNameField->SetL(KNewTierName);

    tierNameField->ModifyL(*iCDSession_vLatest);
    
    CleanupStack::PopAndDestroy(tierNameField);
    
    CCDTierRecord* tierRec1 = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
    CleanupStack::PushL(tierRec1);
    
    tierRec1->SetRecordId(2);
    tierRec1->LoadL(*iCDSession_vLatest);
    
    TPtrC tierThreadName(tierRec1->iTierThreadName);
    if (0 == tierThreadName.Compare(KNewTierName))
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(tierRec1);
    
    return TestStepResult();
    }

//----------------------------------------------------------------------------------------------------------------
CCommsdat372Step::CCommsdat372Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat372Step);
	}

CCommsdat372Step::~CCommsdat372Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat372Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat372Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat372Step::CreateSessionsL()
	{
    iCDSession_v11 = NULL;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}


TVerdict CCommsdat372Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the TierThredName field in the Tier record (valid name)
 * Modify is called on a field container
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    _LIT(KNewTierName, "New Tier Name2 which is longer than the permitted");
    
    CMDBField<TDesC>* tierNameField = new(ELeave)CMDBField<TDesC>(KCDTIdTierThreadName);
    CleanupStack::PushL(tierNameField);
    
    tierNameField->SetRecordId(2);
    tierNameField->SetMaxLengthL(KNewTierName().Length());
    tierNameField->SetL(KNewTierName);

    TRAPD(err, tierNameField->ModifyL(*iCDSession_vLatest));
        
    if (KErrTooBig == err)
    	{
    	SetTestStepResult(EPass);
    	}
    
    CleanupStack::PopAndDestroy(tierNameField);
    
    return TestStepResult();
    }

//----------------------------------------------------------------------------------------------------------------
CCommsdat380Step::CCommsdat380Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat380Step);
	}

CCommsdat380Step::~CCommsdat380Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat380Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat380Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat380Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

TVerdict CCommsdat380Step::doTestStepL()
/**
 * @return - TVerdict code
 * This teststep inserts a rank1 connpref record with prompting on. There is IAP
 * record in the database.
 * 
 * Store is made on a record container
 * 
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    const TInt KIPProtoTierLink = 0x11800200;
    
    CCDConnectionPrefsRecord* connPrefRec = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRec);
    
    connPrefRec->SetRecordId(1);
    connPrefRec->DeleteL(*iCDSession_v11);
    
    CleanupStack::PopAndDestroy(connPrefRec);
    
    //Here the default IPProto AP should be linked from the APPrioritySelPol and
    //IPProto Tier records.
    
	CCDAPPrioritySelectionPolicyRecord* apSelPolRec1 = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
	CleanupStack::PushL(apSelPolRec1);
	
	apSelPolRec1->SetRecordId(254);
	apSelPolRec1->LoadL(*iCDSession_vLatest);
	
	CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(ipprotoApRec);
	
	ipprotoApRec->SetElementId(apSelPolRec1->iAp1);
	ipprotoApRec->LoadL(*iCDSession_vLatest);
    
	if (1 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest) &&
		CCDAccessPointRecord::KNoPolicy == ipprotoApRec->iCustomSelectionPolicy &&
		0 == ipprotoApRec->iCprConfig)
		{
	    //now insert an IAP record
		CCDIAPRecord *iapRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
		CleanupStack::PushL(iapRecord); 
		
		//Insert an IAP record; which will be referred by conn pref records
		iapRecord->SetRecordId(KIAPRecordId); 
		iapRecord->iServiceType.SetMaxLengthL(KMaxTextLength);
		iapRecord->iServiceType= TPtrC(DIAL_OUT_ISP);
		iapRecord->iService=2;
		iapRecord->iBearerType.SetMaxLengthL(KMaxTextLength);
		iapRecord->iBearerType=TPtrC(MODEM_BEARER);
		iapRecord->iBearer=3;
	    
		iapRecord->StoreL(*iCDSession_v11);
		
		CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord);
		
		cpRecord->SetRecordId(2);
		cpRecord->iDefaultIAP = 0;
		cpRecord->iRanking = 1;
		cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
		cpRecord->iBearerSet = KCommDbBearerCSD;
		cpRecord->iDialogPref = ECommDbDialogPrefPrompt;
		
		cpRecord->StoreL(*iCDSession_v11);
		
		CleanupStack::PopAndDestroy(cpRecord);
		CleanupStack::PopAndDestroy(iapRecord);
	    
		if (1 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
			{
			CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
			CleanupStack::PushL(cpRecord1);
			
			cpRecord1->SetRecordId(2);
			cpRecord1->LoadL(*iCDSession_v11);
			
			CCDAPPrioritySelectionPolicyRecord* apSelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
			CleanupStack::PushL(apSelPolRec);
			
			apSelPolRec->SetRecordId(254);
			apSelPolRec->LoadL(*iCDSession_vLatest);
			
			CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(apRec);
			
			apRec->SetElementId(apSelPolRec->iAp1);
			apRec->LoadL(*iCDSession_vLatest);
			
			//the customSelPol is the calculated IAP and the CprConfig field is the 
			//elementID of the connpref record as it's a prompting record
			if ( KIAPRecordId == apRec->iCustomSelectionPolicy &&
			     apRec->iCprConfig == cpRecord1->ElementId() )
				{
				SetTestStepResult(EPass);
				}
			
			CleanupStack::PopAndDestroy(apRec);
			CleanupStack::PopAndDestroy(apSelPolRec);
			CleanupStack::PopAndDestroy(cpRecord1);
			}
		}
	
	CleanupStack::PopAndDestroy(ipprotoApRec);
	CleanupStack::PopAndDestroy(apSelPolRec1);
	
    return TestStepResult();
    }


//----------------------------------------------------------------------------------------------------------------
CCommsdat381Step::CCommsdat381Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat381Step);
	}

CCommsdat381Step::~CCommsdat381Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat381Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat381Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat381Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

TVerdict CCommsdat381Step::doTestStepL()
/**
 * @return - TVerdict code
 * This teststep inserts a rank1 connpref record with prompting on. There is IAP
 * record in the database.
 * 
 * Store is made on a record container
 * 
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    TBool goOn = EFalse;
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRecord); 
    
    iapRecord->iServiceType.SetMaxLengthL((TPtrC(KCDTypeNameLANService)).Length());
    iapRecord->iServiceType.SetL( TPtrC( KCDTypeNameLANService ) );
    iapRecord->iService=2;
    
    if ( (iapRecord->FindL(*iCDSession_v11)) &&
    	 (9 == iapRecord->RecordId()) )
    	{
    	goOn = ETrue;
    	}
    
    CleanupStack::PopAndDestroy(iapRecord);
    
    if (goOn)
    	{
    	iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	    CleanupStack::PushL(iapRecord); 
	    
	    iapRecord->iService=2;
	    
	    if ( (iapRecord->FindL(*iCDSession_v11)) &&
	    	 (1 == iapRecord->RecordId()) )
	    	{
	    	SetTestStepResult(EPass);
	    	}
	    
	    CleanupStack::PopAndDestroy(iapRecord);
    	}
    
    return TestStepResult();
    }

//----------------------------------------------------------------------------------------------------------------
CCommsdat382Step::CCommsdat382Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat382Step);
	}

CCommsdat382Step::~CCommsdat382Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat382Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat382Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat382Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

TVerdict CCommsdat382Step::doTestStepL()
/**
 * @return - TVerdict code
 * This teststep inserts a rank1 connpref record with prompting on. There is IAP
 * record in the database.
 * 
 * Store is made on a record container
 * 
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
	
    TBool goOn = EFalse;
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRecord); 
    
    iapRecord->iBearerType.SetMaxLengthL((TPtrC(KCDTypeNameLANBearer)).Length());
    iapRecord->iBearerType.SetL( TPtrC( KCDTypeNameLANBearer ) );
    iapRecord->iBearer=2;
    
    if ( (iapRecord->FindL(*iCDSession_v11)) &&
    	 (2 == iapRecord->RecordId()) )
    	{
    	goOn = ETrue;
    	}
    
    CleanupStack::PopAndDestroy(iapRecord);
    
    if (goOn)
    	{
    	iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	    CleanupStack::PushL(iapRecord); 
	    
	    iapRecord->iBearer=3;
	    
	    if ( (iapRecord->FindL(*iCDSession_v11)) &&
	    	 (3 == iapRecord->RecordId()) )
	    	{
	    	SetTestStepResult(EPass);
	    	}
	    
	    CleanupStack::PopAndDestroy(iapRecord);
    	}
    
    return TestStepResult();
    }

//----------------------------------------------------------------------------------------------------------------
CCommsdat383Step::CCommsdat383Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat383Step);
	}

CCommsdat383Step::~CCommsdat383Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat383Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat383Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat383Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

TVerdict CCommsdat383Step::doTestStepL()
/**
 * @return - TVerdict code
 * This teststep inserts a rank1 connpref record with prompting on. There is IAP
 * record in the database.
 * 
 * Store is made on a record container
 * 
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    TBool goOn = EFalse;
    
    CCDProxiesRecord *proxiesRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    CleanupStack::PushL(proxiesRecord); 
    
    proxiesRecord->iServiceType.SetMaxLengthL((TPtrC(KCDTypeNameDialOutISP)).Length());
    proxiesRecord->iServiceType.SetL( TPtrC( KCDTypeNameDialOutISP ) );
    proxiesRecord->iService=4;
    
    if ( (proxiesRecord->FindL(*iCDSession_v11)) &&
    	 (1 == proxiesRecord->RecordId()) )
    	{
    	goOn = ETrue;
    	}
    
    CleanupStack::PopAndDestroy(proxiesRecord);
    
    if (goOn)
    	{
    	proxiesRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    	CleanupStack::PushL(proxiesRecord); 
	    
    	proxiesRecord->iService=6;
	    
	    if ( (proxiesRecord->FindL(*iCDSession_v11)) &&
	    	 (2 == proxiesRecord->RecordId()) )
	    	{
	    	SetTestStepResult(EPass);
	    	}
	    
	    CleanupStack::PopAndDestroy(proxiesRecord);
    	}
    
    return TestStepResult();
    }

//----------------------------------------------------------------------------------------------------------------
CCommsdat384Step::CCommsdat384Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat384Step);
	}

CCommsdat384Step::~CCommsdat384Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat384Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat384Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat384Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

TVerdict CCommsdat384Step::doTestStepL()
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    TBool goOn = EFalse;
    
    CCDLANServiceRecord *lanServiceRecord = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
    CleanupStack::PushL(lanServiceRecord); 
    
    lanServiceRecord->iServiceExtensionTableName.SetMaxLengthL((TPtrC(KCDTypeNamePANServiceExt)).Length());
    lanServiceRecord->iServiceExtensionTableName.SetL( TPtrC( KCDTypeNamePANServiceExt ) );
    lanServiceRecord->iServiceExtensionTableRecordId=1;
    
    if ( (lanServiceRecord->FindL(*iCDSession_v11)) &&
    	 (6 == lanServiceRecord->RecordId()) )
    	{
    	SetTestStepResult(EPass);
    	}
    	
    CleanupStack::PopAndDestroy(lanServiceRecord);
    
    if (goOn)
    	{
    	lanServiceRecord = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
    	CleanupStack::PushL(lanServiceRecord); 
	    
    	lanServiceRecord->iServiceExtensionTableRecordId=2;
	    
	    if ( (lanServiceRecord->FindL(*iCDSession_v11)) &&
	    	 (7 == lanServiceRecord->RecordId()) )
	    	{
	    	SetTestStepResult(EPass);
	    	}
	    
	    CleanupStack::PopAndDestroy(lanServiceRecord);
    	}
    
    return TestStepResult();
    }

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
CCommsdat385Step::CCommsdat385Step()/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat385Step);
	}

CCommsdat385Step::~CCommsdat385Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

TVerdict CCommsdat385Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat385Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CCommsdat385Step::CreateSessionsL()
	{
    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

TVerdict CCommsdat385Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check that for each Link layer ap created, there is a equivalent ipproto layer ap
 */
	{
    SetTestStepResult(EFail);
    CreateSessionsL();
    
    const TInt KIPProtoTierLink = 0x11800200;
	const TInt KLinkTierLink = 0x11800300;
    
	if (GetAPNumberL(KLinkTierLink, iCDSession_vLatest) == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		SetTestStepResult(EPass);
		}
	
    return TestStepResult();
    }
