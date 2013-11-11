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
// CCommsdat1Step
// CCommsdat2Step
// CCommsdat3Step
// CCommsdat4Step
// CCommsdat5Step
// CCommsdat6Step
// CCommsdat7Step
// CCommsdat8Step
// CCommsdat9Step
// CCommsdat10Step
// CCommsdat11Step
// CCommsdat12Step
// CCommsdat13Step
// CCommsdat14Step
// CCommsdat15Step
// CCommsdat16Step
// CCommsdat17Step
// CCommsdat18Step
// CCommsdat19Step
// CCommsdat20Step
// CCommsdat21Step
// CCommsdat22Step
// CCommsdat23Step
// CCommsdat24Step
// CCommsdat25Step
// CCommsdat26Step
// CCommsdat27Step
// CCommsdat28Step
// 
//

/**
 @file commsdatBasicFunctionalityTests.cpp
*/
#include "commsdatBasicFunctionalityTests.h"
#include "Te_commsdatSuiteDefs.h"
#include "CommsDatInternalDefs.h"
#include <e32test.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

CCommsdat1Step::~CCommsdat1Step()
/**
 * Destructor
 */
	{
	}

CCommsdat1Step::CCommsdat1Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat1Step);                                                                                           
	}


TVerdict CCommsdat1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a session object and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Setting Repository.  In CCommsdat1Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
    delete cmdbSession;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat2Step::CCommsdat2Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat2Step);
	}


TVerdict CCommsdat2Step::doTestStepL()
/**
 * @return - TVerdict code
 * Set and get attributes
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L(" Setting Repository & creating IAP RecordSet & setting/getting attributes - CCommsdat2Step::doTestStepL()"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession);  
	
    CMDBRecordSet<CCDIAPRecord>* IAPRecords = new(ELeave) CMDBRecordSet<CCDIAPRecord>;

    IAPRecords->SetAttributes(ECDProtectedWrite);

    TMDBAttributeFlags atts = IAPRecords->Attributes();

    if (atts == ECDProtectedWrite)
        SetTestStepResult(EPass);	
	
    delete IAPRecords;
    CleanupStack::PopAndDestroy(cmdbSession);

    return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat3Step::CCommsdat3Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3Step);
	}

TVerdict CCommsdat3Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object and delete it 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat3Step::doTestStepL() in the file commsdat3Step.cpp"));
	
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(); 
	delete dbField;
	
    SetTestStepResult(EPass);	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat4Step::CCommsdat4Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat4Step);
	}


TVerdict CCommsdat4Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object, set element id and delete it 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Creating a field and setting element id - CCommsdat4Step::doTestStepL() in the file commsdat4Step.cpp"));
	
	TMDBElementId elementId = 0x04021600;
	CMDBField<TUint32> *dbField = new(ELeave) CMDBField<TUint32>(elementId); 

    INFO_PRINTF3(_L("ElementId is %08x, should be %08x"), dbField->ElementId(), elementId); 
	
    if (elementId == dbField->ElementId())
        SetTestStepResult(EPass);	

    delete dbField;

	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat5Step::CCommsdat5Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat5Step);
	}


TVerdict CCommsdat5Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id and delete it 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat5Step::doTestStepL() in the file commsdat5Step.cpp"));
	
	TMDBElementId elementId = 5;

	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(elementId); 
	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat6Step::CCommsdat6Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat6Step);
	}


TVerdict CCommsdat6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object, assign a value and delete it 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat6Step::doTestStepL() in the file commsdat6Step.cpp.  Setting TUint32 value"));
	
	CMDBField<TUint32> *dbField = new(ELeave) CMDBField<TUint32>();
	TMDBElementId data = 13;
    *dbField = data; 

    INFO_PRINTF3(_L("value is %08x, should be %08x"), (TUint32)*dbField, data); 
        
    if( data == *dbField)
        SetTestStepResult(EPass);	

	delete dbField;

 	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat7Step::CCommsdat7Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat7Step);
	}


TVerdict CCommsdat7Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object and variable with the same element id, check they both return
 * same element id and delete object 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat7Step::doTestStepL() in the file commsdat7Step.cpp"));
	
	TMDBElementId elementId = 0x01010500;

	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(elementId);
	CMDBField<TInt> dbField2(elementId);
	TUint32 result = dbField->ElementId();
    INFO_PRINTF3(_L("field id = %08x.  Should be %08x"), elementId, dbField->ElementId() & KCDMaskHideRes);

	delete dbField;
	dbField = 0;
   
    if (dbField2.ElementId() == result)
        SetTestStepResult(EPass);	
	
    delete dbField;

    return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat8Step::CCommsdat8Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat8Step);
	}


TVerdict CCommsdat8Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the element id and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat8Step::doTestStepL() in the file commsdat8Step.cpp"));
	
	TMDBElementId elementId = 0x01010600;

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
CCommsdat9Step::CCommsdat9Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat9Step);
	}


TVerdict CCommsdat9Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the record id and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat9Step::doTestStepL() in the file commsdat9Step.cpp"));
	
	TMDBElementId elementId = 0x020304f0;

	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(elementId);
	TMDBElementId result = dbField->RecordId();

    INFO_PRINTF3(_L("result = %08x, should be %08x"), result, (elementId & KCDMaskShowRecordId) >> 8);

    TInt id = (elementId & KCDMaskShowRecordId) >> 8;
    if (result == id)
    	{
    	SetTestStepResult(EPass);
    	}
        
	delete dbField;
	
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------	
CCommsdat10Step::CCommsdat10Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat10Step);
	}


TVerdict CCommsdat10Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the record id and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat10Step::doTestStepL() in the file commsdat10Step.cpp"));
	
	TMDBElementId elementId = 0x01810080;

	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>(elementId);
	TMDBElementId recordId = (dbField->ElementId() & KCDMaskShowRecordId);
	
    INFO_PRINTF3(_L("recordId = %08x, element id = %08x"), recordId, elementId);

    if (recordId == 0)
    	{
        SetTestStepResult(EPass);
    	}

    delete dbField;
	
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------	
CCommsdat11Step::CCommsdat11Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat11Step);
	}


TVerdict CCommsdat11Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the attributes and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat11Step::doTestStepL() in the file commsdat11Step.cpp"));
	
	TMDBElementId elementId = 0xfffffe20;

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
CCommsdat12Step::CCommsdat12Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat12Step);
	}


TVerdict CCommsdat12Step::doTestStepL()
/**
 * Create a CMDBField object with an element id, check the element id and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat12Step::doTestStepL() in the file commsdat12Step.cpp"));
	
	TMDBElementId elementId = 5;
	TMDBElementId result = 0;  
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>();
	dbField->SetElementId(elementId);
	result = dbField->ElementId();

    INFO_PRINTF2(_L("element Id - result = %d"), result);

	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat13Step::CCommsdat13Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat13Step);
	}


TVerdict CCommsdat13Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, check the attributes and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat13Step::doTestStepL() in the file commsdat13Step.cpp"));
	
	TMDBAttributeFlags attributes = 5;
	TMDBAttributeFlags result = 0; 
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>();
	dbField->SetAttributes(attributes);
	result = dbField->Attributes();

    INFO_PRINTF2(_L("attributes - result = %d"), result);

	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat14Step::CCommsdat14Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat14Step);
	}


TVerdict CCommsdat14Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, set attributes, check whether the attributes 
 * are set and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat14Step::doTestStepL() in the file commsdat14Step.cpp"));
	
	TMDBAttributeFlags attributes = 5;
	TBool result; 
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>();
	dbField->SetAttributes(attributes);
	result = dbField->IsSetAttribute(attributes);

    INFO_PRINTF2(_L("Bool result = %b"), result);

	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------	
CCommsdat15Step::CCommsdat15Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat15Step);
	}


TVerdict CCommsdat15Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBField object with an element id, set attributes, clear attributes, check whether
 * the attributes are set and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat15Step::doTestStepL() in the file commsdat15Step.cpp"));
	
	TMDBAttributeFlags attributes = ECDHidden;
	TBool result(EFalse); 
	CMDBField<TInt> *dbField = new(ELeave) CMDBField<TInt>();
	dbField->SetAttributes(attributes);
    result = dbField->IsSetAttribute(attributes);
    INFO_PRINTF3(_L("Attributes %08x should be set.  Result is %b.  Should be 1"), attributes, result);

    if(result)
		{
	    dbField->ClearAttributes(attributes);
	    result = dbField->IsSetAttribute(attributes);

        INFO_PRINTF3(_L("Attributes %08x should be cleared.  Result is %b.  Should be 0"), attributes, result);

	    if(!result)
		    {
    	    SetTestStepResult(EPass);	
		    }
        }
		
	delete dbField;
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat16Step::CCommsdat16Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat16Step);                                                                                           
	}


TVerdict CCommsdat16Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat16Step::doTestStepL() in the file commsdat16Step.cpp"));

	CMDBRecordLink<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordLink<CCDNetworkRecord>(); 
	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------

CCommsdat17Step::CCommsdat17Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat17Step);                                                                                           
	}


TVerdict CCommsdat17Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object with an element id and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat17Step::doTestStepL() in the file commsdat17Step.cpp"));

	TMDBElementId elementId = 5;
	CMDBRecordLink<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordLink<CCDNetworkRecord>(elementId); 
	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------

CCommsdat18Step::CCommsdat18Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat18Step);                                                                                           
	}


TVerdict CCommsdat18Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object with an element id and a null element and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat18Step::doTestStepL() in the file commsdat18Step.cpp"));

	TMDBElementId elementId = 5;
	CCDNetworkRecord *element = NULL;
	CMDBRecordLink<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordLink<CCDNetworkRecord>(elementId,element);  
	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat19Step::~CCommsdat19Step()
/**
 * Destructor
 */
	{
	}

CCommsdat19Step::CCommsdat19Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat19Step);                                                                                           
	}


TVerdict CCommsdat19Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object with an element id and an element and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat19Step::doTestStepL() in the file commsdat19Step.cpp"));

	TMDBElementId elementId = 5;
	CCDNetworkRecord *element = (CCDNetworkRecord*)CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord);
	CMDBRecordLink<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordLink<CCDNetworkRecord>(elementId,element);  
	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat20Step::~CCommsdat20Step()
/**
 * Destructor
 */
	{
	}

CCommsdat20Step::CCommsdat20Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat20Step);                                                                                           
	}


TVerdict CCommsdat20Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object, check record id and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat20Step::doTestStepL() in the file commsdat20Step.cpp"));

	TMDBElementId elementId = 5;

    INFO_PRINTF2(_L("elementId = %d"), elementId);
   
	CMDBRecordLink<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordLink<CCDNetworkRecord>();  
	
	TInt id = (elementId & KCDMaskShowRecordId) >> 8;
	if (dbField->RecordId() == id)
    	{
    	SetTestStepResult(EPass);
    	}
    INFO_PRINTF2(_L("reset elementId = %d"), elementId);

	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat21Step::~CCommsdat21Step()
/**
 * Destructor
 */
	{
	}

CCommsdat21Step::CCommsdat21Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat21Step);                                                                                           
	}


TVerdict CCommsdat21Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object, check linked record and delete object
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat21Step::doTestStepL() in the file commsdat21Step.cpp"));

	TUint temp(0);
	
	CMDBRecordLink<CCDLocationRecord> *dbField = new(ELeave) CMDBRecordLink<CCDLocationRecord>();  
	if(dbField->iLinkedRecord == NULL)
		{
		INFO_PRINTF1(_L("iLinkedRecord is null"));
		SetTestStepResult(EPass);
		}
		
    INFO_PRINTF2(_L("linked record id = %d"), temp);


	delete dbField;
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat22Step::~CCommsdat22Step()
/**
 * Destructor
 */
	{
	}

CCommsdat22Step::CCommsdat22Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat22Step);                                                                                           
	}


TVerdict CCommsdat22Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordSet object and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat22Step::doTestStepL() in the file commsdat22Step.cpp"));

	TMDBElementId elementLinking = 5;

	CMDBRecordSet<CCDNetworkRecord> *dbField = new(ELeave) CMDBRecordSet<CCDNetworkRecord>(elementLinking); 
	delete dbField;

    SetTestStepResult(EPass);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat23Step::~CCommsdat23Step()
/**
 * Destructor
 */
	{
	}

CCommsdat23Step::CCommsdat23Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat23Step);                                                                                           
	}


TVerdict CCommsdat23Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBSession object, check the version info and delete it 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat23Step::doTestStepL() in the file commsdat23Step.cpp"));

	CMDBSession *cmdbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(cmdbSession);  
	TVersion versionInUse = cmdbSession->VersionInUse();
	if(versionInUse.iBuild != CMDBSession::LatestVersion().iBuild )
		{
		INFO_PRINTF1(_L("Something wrong with handling of build version information"));
		}
	if(versionInUse.iMajor != CMDBSession::LatestVersion().iMajor )
		{
		INFO_PRINTF1(_L("Something wrong with handling of major version information"));
		}
	if(versionInUse.iMinor != CMDBSession::LatestVersion().iMinor )
		{
		INFO_PRINTF1(_L("Something wrong with handling of minor version information"));
		}
		
	if(versionInUse.iBuild == CMDBSession::LatestVersion().iBuild  &&
	   versionInUse.iMajor == CMDBSession::LatestVersion().iMajor &&
	   versionInUse.iMinor == CMDBSession::LatestVersion().iMinor)
		{
		SetTestStepResult(EPass);
		}
    	
    CleanupStack::PopAndDestroy(cmdbSession);	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat24Step::~CCommsdat24Step()
/**
 * Destructor
 */
	{
	}

CCommsdat24Step::CCommsdat24Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat24Step);                                                                                           
	}


TVerdict CCommsdat24Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBSession object, check transaction status and delete it
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Opening and closing transactions - CCommsdat24Step::doTestStepL() in the file commsdat24Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	cmdbSession->OpenTransactionL();
	TBool inTransaction = cmdbSession->IsInTransaction();
	if(!inTransaction)
		{
		INFO_PRINTF1(_L("Something wrong with opening transaction"));
		}
		
	cmdbSession->RollbackTransactionL();
	
	TBool notInTransactionRollback = cmdbSession->IsInTransaction();
	if(notInTransactionRollback)
		{
		INFO_PRINTF1(_L("Something wrong with rolling back transaction"));
		}
		
	cmdbSession->OpenTransactionL();
	cmdbSession->CommitTransactionL();

	TBool notInTransactionCommit = cmdbSession->IsInTransaction();
	if(notInTransactionCommit)
		{
		INFO_PRINTF1(_L("Something wrong with comitting transaction"));
		}
		
	CleanupStack::PopAndDestroy(cmdbSession);

	if(inTransaction && !notInTransactionRollback && !notInTransactionCommit)
		{
		SetTestStepResult(EPass);	
		}
    	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat25Step::~CCommsdat25Step()
/**
 * Destructor
 */
	{
	}

CCommsdat25Step::CCommsdat25Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat25Step);                                                                                           
	}


TVerdict CCommsdat25Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBSession, set attribute mask and delete it 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat25Step::doTestStepL() in the file commsdat25Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession);  
	
	cmdbSession->SetAttributeMask(ECDHidden | ECDPrivate );
	
	SetTestStepResult(EPass);		
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
#ifndef __TOOLS2__
// This test is not used on the TOOLS2 platform.
CCommsdat26Step::~CCommsdat26Step()
/**
 * Destructor
 */
	{
	}

CCommsdat26Step::CCommsdat26Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat26Step);                                                                                           
	}


TVerdict CCommsdat26Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBSession object, publish properties and  delete it.
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat26Step::doTestStepL() and trying to test Publish Properties"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession);  
	TRAPD(err, cmdbSession->PublishProperties())
	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	if(err == KErrNone)	
		{
		SetTestStepResult(EPass);
		}
		
	return TestStepResult(); 
	}
#endif 
//----------------------------------------------------------------------------------------------------------------
CCommsdat27Step::~CCommsdat27Step()
/**
 * Destructor
 */
	{
	}

CCommsdat27Step::CCommsdat27Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat27Step);                                                                                           
	}


TVerdict CCommsdat27Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBSession object, publish properties and  delete it.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Reading templated value underneath empty record"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDModemBearerRecord *mbRecord = (CCDModemBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord);
	CleanupStack::PushL(mbRecord);

	// create record No.1 with 
	mbRecord->SetRecordId(1);
    mbRecord->iSpeakerVolPref = 1;
    
	mbRecord->StoreL(*cmdbSession);
	
	// create TemplateRecord
	mbRecord->SetRecordId(KCDDefaultRecord);
    mbRecord->iSpeakerPref = 1;
    
	mbRecord->StoreL(*cmdbSession);

	// create field looking at speakerPref
	CMDBField<TUint32>* speakerPrefField = new(ELeave) CMDBField<TUint32>(KCDTIdSpeakerPref);
	CleanupStack::PushL(speakerPrefField);
	// load the speakerPref from record No.1 which is Null value
    // The data from the template should be loaded giving the value of 1, not null
	speakerPrefField->SetRecordId(1);
	speakerPrefField->LoadL(*cmdbSession);
	
	if (speakerPrefField->IsNull())
		{
		// return with failure because template value was not found
		return TestStepResult();
		}
	
	if(*speakerPrefField == 1)

		{
		SetTestStepResult(EPass);		
		}
	
	CleanupStack::PopAndDestroy(speakerPrefField);
	
	CleanupStack::PopAndDestroy(mbRecord);

	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat28Step::~CCommsdat28Step()
/**
 * Destructor
 */
	{
	}

CCommsdat28Step::CCommsdat28Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat28Step);
	}

TVerdict CCommsdat28Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a CMDBRecordLink object and delete it
 */
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat28Step::doTestStepL() in the file commsdat28Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

//-- Load Linked with tag record test

	CCDIAPRecord *anIAPRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	CleanupStack::PushL(anIAPRecord);  
	
	anIAPRecord->SetRecordId(1);
	anIAPRecord->LoadL(*cmdbSession);

    CCDLocationRecord* locationRecord = anIAPRecord->iLocation;

    if (locationRecord)
        {
        // Oops Loading the IAP record mustn't automatically create records linked fields too.
        return TestStepResult();
        }
    
    anIAPRecord->iLocation.LoadL(*cmdbSession);
	
    locationRecord = anIAPRecord->iLocation;
	
	if(!locationRecord || locationRecord->iPauseAfterDialOut != 1)
		{
	    return TestStepResult();
		}
	

    CCDServiceRecordBase* serviceRecord = anIAPRecord->iService;

    if (serviceRecord)
        {
        // Oops LoadLing the IAP record mustn't automatically create records linked fields too.
        return TestStepResult();
        }

    /* Uncomment when validating general links correctly.*/
    anIAPRecord->iService.LoadL(*cmdbSession);

    serviceRecord = anIAPRecord->iService;

    if (!serviceRecord) // TODO - add the llmnr field to this test ced and check that here
        {
        return TestStepResult();
        }

    // ENHANCE - could also check the type of the service record matches the type named in ServiceType field
   

	CleanupStack::PopAndDestroy(anIAPRecord);
    

//-- Find Linked record test

	CCDIAPRecord* IAPRecord1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(IAPRecord1);  
	
	IAPRecord1->SetRecordId(2);
	IAPRecord1->LoadL(*cmdbSession);

    IAPRecord1->iLocation.SetRecordId(2);
    
	CCDLocationRecord* LocationRecord1 = IAPRecord1->iLocation; // will be NULL at first
    
    LocationRecord1 = static_cast<CCDLocationRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));    
    
    if(! LocationRecord1 )
        {
        return TestStepResult();
        }

    LocationRecord1->iPauseAfterDialOut = 2;
/*
	if (! IAPRecord1->iLocation.FindL(*cmdbSession))
        {
        return TestStepResult();
        }
*/	
	if(LocationRecord1->iPauseAfterDialOut == 2) // TODO - find some other field
		{
	    SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(IAPRecord1); // will cleanup all linked fields too.

	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat29Step::~CCommsdat29Step()
/**
 * Destructor
 */
	{
	}

CCommsdat29Step::CCommsdat29Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat29Step);
	}

TVerdict CCommsdat29Step::doTestStepL()
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat29Step::doTestStepL() in the file commsdat29Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

//-- Load a CMDBField from the database

		CMDBField<TUint32>* intervalField = new(ELeave) CMDBField<TUint32>(KCDTIdPauseAfterDialOut);
		CleanupStack::PushL(intervalField);
		intervalField->SetRecordId(1);
		intervalField->LoadL(*cmdbSession);
		
		CleanupStack::PopAndDestroy(intervalField);
		CleanupStack::PopAndDestroy(cmdbSession);
		
		SetTestStepResult(EPass);
		
		return TestStepResult();
	}


//----------------------------------------------------------------------------------------------------------------

CCommsdat30Step::~CCommsdat30Step()
/**
 * Destructor
 */
	{
	}

CCommsdat30Step::CCommsdat30Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat30Step);
	}

//This code here is from the TSYCONFIG.cpp file...
TVerdict CCommsdat30Step::doTestStepL()
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat30Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	//-------------------------------- part 1 ---------------------------------
	INFO_PRINTF1(_L("Loading the ModemRecord..."));
    CCDModemBearerRecord *modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
    CleanupStack::PushL(modemRecord);
    
    _LIT(KTsyName,"MM");
    modemRecord->iTsyName.SetMaxLengthL(KMaxTextLength);
    modemRecord->iTsyName = KTsyName;
    
    TBool searchResult = modemRecord->FindL(*cmdbSession);
    
    TUint32 bearerId(0);
    
    if (searchResult)
	    {
	    INFO_PRINTF1(_L("OK, found the ModemRecord..."));
	    
		bearerId = modemRecord->RecordId();	
		
		INFO_PRINTF2(_L("so the bearerId : %d"),bearerId);
	    }
	else
		{
		bearerId = static_cast<TUint32>(KErrNotFound);
		}
    
    CleanupStack::PopAndDestroy(modemRecord);
    
    //-------------------------------- part 2 ---------------------------------
    INFO_PRINTF1(_L("Finding the location ID in the IAP record..."));
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
  	CleanupStack::PushL(iapRecord);
  	
  	TUint32 locationId(0);
  	
  	iapRecord->iBearer = bearerId;
  	TBool err = iapRecord->FindL(*cmdbSession);
  	if (err)
  		{
	    INFO_PRINTF1(_L("OK, found the ModemRecord..."));
	    
	    locationId = iapRecord->iLocation;
		
		INFO_PRINTF2(_L("so the locationId : %d"),locationId);
  		
  		}
  	else
  		{
  		locationId = static_cast<TUint32>(KErrNotFound);
  		}
  	
    CleanupStack::PopAndDestroy(iapRecord);
    
    TBool goOn = EFalse;
    
    if (bearerId == 2 && locationId == 4)
    	{
     	goOn = ETrue;
     	}
    
    if (goOn)
    	{    
    	goOn = EFalse;
    	//Finding the IAP record based on the service field
	    CCDIAPRecord *iapRecord1 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	    CleanupStack::PushL(iapRecord1);
	    
	    iapRecord1->iService = 2;
	    err = iapRecord1->FindL(*cmdbSession);
	    
	    if (err)
	    	{
		    locationId = iapRecord1->iLocation;
		    bearerId = iapRecord1->iBearer;
		    
		    if (bearerId == 2 && locationId == 4)
		    	{
		    	//SetTestStepResult(EPass);
		    	goOn = ETrue;
		    	}
	    	}
	    CleanupStack::PopAndDestroy(iapRecord1);
    	}

//------------------------------------- part 2 --------------------------------
    //Fingind an IAP record based on the location id
    if (goOn)
    	{
	    CCDIAPRecord *iapRecord2 = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	    CleanupStack::PushL(iapRecord2);
	    
	    iapRecord2->iLocation = 4;
	    err = iapRecord2->FindL(*cmdbSession);
	    
	    if (err)
	    	{
		    TInt service = iapRecord2->iService;
		    bearerId = iapRecord2->iBearer;
		    
		    if (bearerId == 2 && service == 2)
		    	{
		    	SetTestStepResult(EPass);
		    	}
	    	}
	    CleanupStack::PopAndDestroy(iapRecord2);
       	}

    
	CleanupStack::PopAndDestroy(cmdbSession);
		
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat31Step::~CCommsdat31Step()
/**
 * Destructor
 */
	{
	}

CCommsdat31Step::CCommsdat31Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat31Step);
	}

//This code here is from the TSYCONFIG.cpp file...
TVerdict CCommsdat31Step::doTestStepL()
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat31Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	INFO_PRINTF1(_L("Loading the ProxiesRecord..."));
	CCDProxiesRecord *proxiesRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    CleanupStack::PushL(proxiesRecord);
    
  	TUint32 portNumber(0);
  	
  	proxiesRecord->iService = 4; //the value 4 is from the database...
  	TBool err = proxiesRecord->FindL(*cmdbSession);
  	if (err)
  		{
	    INFO_PRINTF1(_L("OK, found the ProxiesRecord..."));
	    
	    portNumber = proxiesRecord->iPortNumber;
		
		INFO_PRINTF2(_L("so the portnumber : %d"),portNumber);
  		
  		}
  	else
  		{
  		portNumber = static_cast<TUint32>(KErrNotFound);
  		}
  	
    CleanupStack::PopAndDestroy(proxiesRecord);
    
    if (portNumber == 80)
    	{
    	SetTestStepResult(EPass);
    	}
    
	CleanupStack::PopAndDestroy(cmdbSession);
		
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat32Step::~CCommsdat32Step()
/**
 * Destructor
 */
	{
	}

CCommsdat32Step::CCommsdat32Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat32Step);
	}

//This code here is from the TSYCONFIG.cpp file...
TVerdict CCommsdat32Step::doTestStepL()
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat32Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	INFO_PRINTF1(_L("Loading the LanServiceRecord..."));
	CCDLANServiceRecord *lanserviceRecord = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
    CleanupStack::PushL(lanserviceRecord);
    
  	
    lanserviceRecord->iServiceExtensionTableRecordId = 1; //the value 1 is from the database...
  	TBool err = lanserviceRecord->FindL(*cmdbSession);
  	if (err)
  		{
	    INFO_PRINTF1(_L("OK, found the LanServiceRecord..."));
	    
	    TPtrC ipAddr(lanserviceRecord->iIpAddr);
		
		INFO_PRINTF2(_L("so the ipAddr : %S"),&ipAddr);
  		
		_LIT(KIPAddr,"172.16.0.1"); //the value is from the DB...
		
	    if (ipAddr == KIPAddr)
	    	{
	    	SetTestStepResult(EPass);
	    	}
  		}
  	else
  		{
  		INFO_PRINTF1(_L("NOT found the LanServiceRecord..."));
  		}
  	
    CleanupStack::PopAndDestroy(lanserviceRecord);
    
	CleanupStack::PopAndDestroy(cmdbSession);
		
	return TestStepResult();
	}


//----------------------------------------------------------------------------------------------------------------

CCommsdat33Step::~CCommsdat33Step()
/**
 * Destructor
 */
	{
	}

CCommsdat33Step::CCommsdat33Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat33Step);
	}

TVerdict CCommsdat33Step::doTestStepL()
	{
	/* This test test the reading of the link between a WAPAccessPoint and a WAPIPBearer
	 * record. The reading happens backwards, meaning that based on the recordID of the linked WAPAP
	 * recrod we try to find the linking WAPIPBearer record.
	 */
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat33Step::doTestStepL() - Find a WapIpBearer record based on AccessPointId"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	INFO_PRINTF1(_L("Finding the WapIpBearer record..."));
	
    CMDBRecordSet<CCDWAPIPBearerRecord>* recordSet = new(ELeave) CMDBRecordSet<CCDWAPIPBearerRecord>(KCDTIdWAPIPBearerRecord);
    CleanupStack::PushL(recordSet);
	
	
	CCDWAPIPBearerRecord *wapIPBearer = static_cast<CCDWAPIPBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPIPBearerRecord));
	CleanupStack::PushL(wapIPBearer);
    wapIPBearer->iWAPAccessPointId = 4; //the value 4 is from the database...
    
    //ownership of 'wapIPBearer' goes to the recordSet...
    recordSet->iRecords.AppendL(wapIPBearer);
  	CleanupStack::Pop(wapIPBearer);
  	
  	TBool err = recordSet->FindL(*cmdbSession);
  	if (err)
  		{
	    INFO_PRINTF1(_L("OK, found the WapIpBearer Record..."));
	    
	    TInt iap = wapIPBearer->iWAPIAP;
		
		INFO_PRINTF2(_L("so the iap : %d"),iap);
  		
		
	    if (iap == 4) //4 from the database
	    	{
	    	SetTestStepResult(EPass);
	    	}
  		}
  	else
  		{
  		INFO_PRINTF1(_L("NOT found the WapIpBearer Record..."));
  		}
  	
  	CleanupStack::PopAndDestroy(recordSet);
	CleanupStack::PopAndDestroy(cmdbSession);
		
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat34Step::~CCommsdat34Step()
/**
 * Destructor
 */
	{
	}

CCommsdat34Step::CCommsdat34Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat34Step);
	}

TVerdict CCommsdat34Step::doTestStepL()
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat34Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	INFO_PRINTF1(_L("Loading the ProxiesRecord..."));
	CCDProxiesRecord *proxiRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
	CleanupStack::PushL(proxiRecord);
	
	proxiRecord->iService = 6;
	proxiRecord->FindL(*cmdbSession);
	
	//---- just to check whether the correct record is read...
	{
	CMDBField<TDesC>& proxyServerNameField = proxiRecord->iServerName;
	CMDBField<TDesC>& recordNameField = proxiRecord->iRecordName;
	TPtrC serverName(proxyServerNameField);
	TPtrC recordName(recordNameField);
	_LIT(KExpectedServerName, "Dummy Server Name");
	_LIT(KExpectedRecordName, "DefaultRecordName-2");
	
	TESTL((serverName.Compare(KExpectedServerName) == 0) && (recordName.Compare(KExpectedRecordName) == 0));
	}
	//--------------------------------------------------------
	
	_LIT(KModifiedName, "janos_modified");
	CMDBField<TDesC>& nameField = proxiRecord->iRecordName;
	TPtrC name(nameField);
	nameField.SetMaxLengthL(KModifiedName().Length());
	nameField.SetL(KModifiedName);
	
	INFO_PRINTF1(_L("Modifying the ProxiesRecord..."));
	proxiRecord->ModifyL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(proxiRecord);
		
	CMDBField<TDesC>* proxyServerNameField = new (ELeave) CMDBField<TDesC>(KCDTIdProxiesRecord | KCDTIdServerName);
	CleanupStack::PushL(proxyServerNameField);
	proxyServerNameField->SetRecordId(2);
	
	/* This LoadL shouldn't leave. The field is loaded from the template record but the record id should be the 
	 * correct one - in this case 2. This was overwritten by commsdat to 0 as the field was loaded from the template 
	 * rec.
	 */
	
	proxyServerNameField->LoadL(*cmdbSession);
	
	if (2 == proxyServerNameField->RecordId() &&
		KCDNotNullFlag == (proxyServerNameField->ElementId() & KCDNotNullFlag))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(proxyServerNameField);
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat35Step::~CCommsdat35Step()
/**
 * Destructor
 */
	{
	}

CCommsdat35Step::CCommsdat35Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat35Step);
	}

/* This test step is for testing the template records. A proxies record is 
 * loaded from the database and some of the fields are filled up from the template
 * record. The point is that the fields from the template record mustn't have the 
 * hidden attribute and has to have the correct record number when loading into the 
 * record
 */ 
TVerdict CCommsdat35Step::doTestStepL()
	{
    SetTestStepResult(EFail);	
    
	INFO_PRINTF1(_L("CCommsdat35Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	INFO_PRINTF1(_L("Loading the ProxiesRecord..."));
	CCDProxiesRecord *proxiRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
	CleanupStack::PushL(proxiRecord);
	
	proxiRecord->SetRecordId(2);
	proxiRecord->LoadL(*cmdbSession);
	
	//---- just to check whether the correct record is read...
	{
	CMDBField<TDesC>& proxyServerNameField = proxiRecord->iServerName;
	CMDBField<TDesC>& recordNameField = proxiRecord->iRecordName;
	TPtrC serverName(proxyServerNameField);
	TPtrC recordName(recordNameField);
	_LIT(KExpectedServerName, "Dummy Server Name");
	_LIT(KExpectedRecordName, "DefaultRecordName-2");
	
	TESTL((serverName == KExpectedServerName) && (recordName == KExpectedRecordName));
	}
	//--------------------------------------------------------
	
	_LIT(KModifiedName, "janos_modified");
	CMDBField<TDesC>& nameField = proxiRecord->iRecordName;
	TPtrC name(nameField);
	nameField.SetMaxLengthL(KModifiedName().Length());
	nameField.SetL(KModifiedName);
	
	/* At this point the 'proxiRecord' still has the 'ProxyServerName' field
	 * which comes form the template record. As the field is not modified it
	 * shouldn't be stored into the record. This will be checked after the
	 * ModifiyL call.
	 */
	
	INFO_PRINTF1(_L("Modifying the ProxiesRecord..."));
	proxiRecord->ModifyL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(proxiRecord);
	
	proxiRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
	CleanupStack::PushL(proxiRecord);
	
	proxiRecord->SetRecordId(2);
	proxiRecord->LoadL(*cmdbSession);
	
	if (2 == proxiRecord->iServerName.RecordId() &&
		KCDNotNullFlag == (proxiRecord->iServerName.ElementId() & KCDNotNullFlag))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(proxiRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
