// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file commsdatExtensionTests.cpp
*/
#include "commsdatExtensionTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

const TInt TELEPHONE_NO_SIZE = 50;
//----------------------------------------------------------------------------------------------------------------
CCommsdat201Step::CCommsdat201Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat201Step);
	}


TVerdict CCommsdat201Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test Load on record set; should return KErrNotFound
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Load within transaction - CCommsdat201Step::doTestStepL() "));

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

CCommsdat202Step::CCommsdat202Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat202Step);
	}


TVerdict CCommsdat202Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the element id and delete it
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("CCommsdat202Step::doTestStepL()"));

	TMDBElementId elementId = 0x01010500;

	TMDBElementId result;
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(elementId);
	result = dbField->ElementId();
	INFO_PRINTF3(_L("result = %08x.  Should be %08x"), result, elementId);

    if (result == elementId)
    	{
    	SetTestStepResult(EPass);
    	}

	delete dbField;

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat203Step::CCommsdat203Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat203Step);
	}


TVerdict CCommsdat203Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordSet object and delete it
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("CCommsdat203Step::doTestStepL() "));

	TMDBElementId elementLinking = 5;

	CMDBRecordSet<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordSet<CCDNetworkRecord>(elementLinking);
	delete dbField;

    SetTestStepResult(EPass);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat204Step::CCommsdat204Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat204Step);
	}


TVerdict CCommsdat204Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the attributes and delete it
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("CCommsdat204Step::doTestStepL() "));

	TMDBElementId elementId = 0xfffffff;

 	TMDBAttributeFlags result;
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(elementId);
    result = dbField->Attributes();

    INFO_PRINTF2(_L("attributes : result = %08x"), result);

    if (result  == (elementId & KCDMaskShowAttributes))
    	{
    	SetTestStepResult(EPass);
    	}

	delete dbField;

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat205Step::CCommsdat205Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat205Step);
	}


TVerdict CCommsdat205Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object with an element id and an element and delete it
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("CCommsdat205Step::doTestStepL()"));

	TMDBElementId elementId = 6;
	CCDNetworkRecord *element = (CCDNetworkRecord*)CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord);
	CMDBRecordLink<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordLink<CCDNetworkRecord>(elementId,element);
	delete dbField;

    SetTestStepResult(EPass);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

_LIT(KLongestPossibleTableName, "A table name which has 64 characters; that is the limit defined.");

CCommsdat206Step::CCommsdat206Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat206Step);
	}

TVerdict CCommsdat206Step::doTestStepL()
/**
 * @return - TVerdict code
 * Testing medium texts as table names
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing long table names in CCommsdat206Step::doTestStepL() "));

	INFO_PRINTF1(_L("CCommsdat206Step::doTestStepL()"));

	// Create a new table
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord);
    testRecord->InitializeL(KLongestPossibleTableName(), TestGenRecordInfo);

	TRAPD(storeErr, testRecord->StoreL(*cmdbSession));

	CleanupStack::PopAndDestroy(testRecord);

	testRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(testRecord);

	TRAPD(loadErr,testRecord->LoadL(*cmdbSession));

	if(storeErr == KErrNone || loadErr == KErrNotFound)
		{
		SetTestStepResult(EPass);
		}

	cmdbSession->Close();
    CleanupStack::PopAndDestroy(2,cmdbSession);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat207Step::CCommsdat207Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat207Step);
	}

TVerdict CCommsdat207Step::doTestStepL()
/**
 * @return - TVerdict code
 * Conn pref records with a rank of 2 are not permitted to be incoming
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing validation for connection preferences "));

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

	cmdbSession->Close();
	CleanupStack::PopAndDestroy(2,cmdbSession);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat208Step::CCommsdat208Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat208Step);
	}

TVerdict CCommsdat208Step::doTestStepL()
/**
 * @return - TVerdict code
 * The ranking of conn pref record must be eiher 1 or 2,
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing validation for connection preferences in CCommsdat208Step::doTestStepL() "));

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
	cpRecord->iRanking = 5;    // Invalid value, should cause KErrOverflow
	TRAPD(storeErr, cpRecord->StoreL(*cmdbSession));
	INFO_PRINTF2(_L("store returns %d"), storeErr);

	if(storeErr == KErrOverflow)
		{
		SetTestStepResult(EPass);
		}
    cmdbSession->Close();
	CleanupStack::PopAndDestroy(2,cmdbSession);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat209Step::CCommsdat209Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat209Step);
	}

TVerdict CCommsdat209Step::doTestStepL()
/**
 * @return - TVerdict code
 * Finding a record;
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("CCommsdat209Step::doTestStepL() "));

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
    cmdbSession->Close();
    CleanupStack::PopAndDestroy(2,cmdbSession);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat210Step::CCommsdat210Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat210Step);
	}

TVerdict CCommsdat210Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve non-mobile phone number - EForDisplay
 * CED -i Z:\testdata\configs\te_commsdat_location_table3.cfg
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing resolving phone number display in CCommsdat210Step::doTestStepL() "));

	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);

	TPtrC number(_L("6294841"));
	TBuf<TELEPHONE_NO_SIZE> dialString;

	utils->ResolvePhoneNumberL(number, dialString, EForDisplay, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);

	_LIT(KExpectedNumber, "89,6294841");

	if(dialString == KExpectedNumber)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat211Step::CCommsdat211Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat211Step);
	}

TVerdict CCommsdat211Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve non-mobile phone number - EForDialing
 * CED -i Z:\testdata\configs\te_commsdat_location_table3.cfg
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing resolving phone number for dial in CCommsdat211Step::doTestStepL() "));

	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);

	TPtrC number(_L("6294841"));
	TBuf<TELEPHONE_NO_SIZE> dialString;

	utils->ResolvePhoneNumberL(number, dialString, EForDialing, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);

	// 8 is disable call waiting, 9 is local call hence no international code or dialling code
	_LIT(KExpectedNumber, "8WT9,P6294841");

	if(dialString == KExpectedNumber)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat212Step::~CCommsdat212Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat212Step::CCommsdat212Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat212Step);
	}

TVerdict CCommsdat212Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Test looks up a previously stored access point record and
 * verifies its consistence
 */
	{
    SetTestStepResult(EFail);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	SetTestStepResult(EPass);
	return TestStepResult();
	}

CCDRecordBase* LoadLinkedRecordL(CMDBSession& aSession, TMDBElementId aElementId)
    {
   	CCDRecordBase *rec = static_cast<CCDRecordBase*>(CCDRecordBase::RecordFactoryL(aElementId));
    CleanupStack::PushL(rec);
    rec->LoadL(aSession);
    CleanupStack::Pop(rec);
    return rec;
    }


TVerdict CCommsdat212Step::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
	SetTestStepResult(EFail);
	// TH - This test looks exactly like test 195 I wonder why is that. Play dumb now, will raise later...
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	_LIT(KApName1, "dummynifMappedFromIAP1");
	//_LIT(KSelName1, "SelectionPolicy1");
	//const TUint KAp1 = 22;
	const TUint KRecordId = 1;
	
	CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);
	
	apRecord->iRecordName.SetL(KApName1);
	
	TInt found = apRecord->FindL(*iCDSession);
	if (found)
	    {
        INFO_PRINTF2(_L("apRecord loaded. Element id is %08x"), apRecord->ElementId());
        
        //Load the related IAP record
        CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    	CleanupStack::PushL(iapRecord);
    	iapRecord->SetRecordId(KRecordId);
    	iapRecord->LoadL(*iCDSession);
        
        if(static_cast<TInt>(apRecord->iRecordTag) == iapRecord->RecordId())
   	        {
   	        INFO_PRINTF1(_L("AP and the 'linked' IAP record found"));
   	        SetTestStepResult(EPass);
   	        }
   	    else
   	        {
   	        INFO_PRINTF1(_L("AP and the 'linked' IAP record found"));
   	        }
   	        
        CleanupStack::PopAndDestroy(iapRecord);
        
	    }
	else
	    {
	    INFO_PRINTF1(_L("AccessPoint record not found"));
	    }
        
    CleanupStack::PopAndDestroy(apRecord);
        
        
        
        
        
        
        
        /*
        
        //Load the Selection Policy record
       	CCDAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDConnectionPrefsRecord::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
    	CleanupStack::PushL(selPolRecord);
    	selPolRecord->SetRecordId(KRecordId);
    	selPolRecord->LoadL(*iCDSession);
        
    	TESTL(TPtrC(selPolRecord->iRecordName) == KSelName1);
        
        //-----------------------
        //OK, got the SelectionPolicyRecord - next step read the referenced AccessPointRecord
        CCDAccessPointRecord* apLink = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
        apLink->SetRecordId(KRecordId);
        apLink->LoadL(*iCDSession);
        
        TUint recId = apLink->RecordId();
        //-----------------------
        
   	    //if(selPolRecord->iAp1 == KAp1)
   	    if(recId == KAp1)
   	        {
   	        INFO_PRINTF1(_L("Selection Policy record found"));
   	        SetTestStepResult(EPass);
   	        }
   	    else
   	        {
   	        INFO_PRINTF1(_L("Selection Policy record not found"));
   	        TUint s = selPolRecord->iAp1;
   	        INFO_PRINTF2(_L("selPolRecord->iAp1 is %d"), s);
   	        }
   	    CleanupStack::PopAndDestroy(apLink);
	    CleanupStack::PopAndDestroy(selPolRecord);
	    }
	CleanupStack::PopAndDestroy(apRecord);*/
#else
	_LIT(KApName1, "SNAP1");
	_LIT(KSelName1, "SelectionPolicy1");

    CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);

    apRecord->iRecordName.SetL(KApName1);
	TInt found = apRecord->FindL(*iCDSession);
	if (found)
	    {
        INFO_PRINTF2(_L("apRecord loaded. Element id is %08x"), apRecord->ElementId());

        //Load the Selection Policy record
       	CCDIAPPrioritySelectionPolicyRecord * selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord *>(LoadLinkedRecordL(*iCDSession, apRecord->iSelectionPolicy));
    	CleanupStack::PushL(selPolRecord);

    	TESTL(TPtrC(selPolRecord->iRecordName) == KSelName1);

   	    if(selPolRecord->iIap1.RecordId() == 1)
   	        {
   	        INFO_PRINTF1(_L("Selection Policy record found"));
   	        SetTestStepResult(EPass);
   	        }
   	    else
   	        {
   	        INFO_PRINTF1(_L("Selection Policy record not found"));
   	        }
	    CleanupStack::PopAndDestroy(selPolRecord);
	    }
	CleanupStack::PopAndDestroy(apRecord);
#endif
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat213Step::CCommsdat213Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat213Step);
	}

TVerdict CCommsdat213Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check the read and write access to database fields
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing read write Check access to database fields Commsdat213Step::doTestStepL() "));

	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);

	TDesC16 number(_L("6294841"));
	RMessagePtr2 aMessage;


	TInt res1 = utils->CheckReadCapability(number,&aMessage);

	TInt res2 = utils->CheckWriteCapability(number,&aMessage);

	if(res1 == KErrNone && res2 == KErrNone)
		{
		INFO_PRINTF1(_L("Read and write access to database is checked"));
		SetTestStepResult(EPass);
		}
	CleanupStack::PopAndDestroy(utils);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat214Step::CCommsdat214Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat214Step);
	}

TVerdict CCommsdat214Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve mobile phone number for dialing
 * CED -i Z:\testdata\configs\te_commsdat_location_table2.cfg
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing resolving phone number for dialing in CCommsdat214Step::doTestStepL() "));

	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);

	_LIT(KNumber, "6294841");
	TPtrC number(KNumber);
	TBuf<TELEPHONE_NO_SIZE> dialString;

	utils->ResolvePhoneNumberFromDatabaseL(number, dialString, EForDialing, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);

	if(dialString == KNumber)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat215Step::CCommsdat215Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat215Step);
	}

TVerdict CCommsdat215Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve mobile phone number for display
 * CED -i Z:\testdata\configs\te_commsdat_location_table2.cfg
 */
	{

    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Testing resolving phone number for display in CCommsdat215Step::doTestStepL() "));

	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);

	_LIT(KNumber, "6294841");
	TPtrC number(KNumber);
	TBuf<TELEPHONE_NO_SIZE> dialString;

	utils->ResolvePhoneNumberFromDatabaseL(number, dialString, EForDisplay, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);

	if(dialString == KNumber)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}





