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
// Example CTestStep derived implementation
// 
//

/**
 @file commsdatSnapTests.cpp
*/
#include "commsdatSnapTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
#include <commsdattypesv1_1_partner.h>
#include <commsdattypeinfov1_1_internal.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
namespace
	{

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

//----------------------------------------------------------------------------------------------------------------
//Below workaround to load the linked record which should be resolved with DEF58321
CCDRecordBase* GetConnectedRecordL(CMDBSession& aSession, TMDBElementId aElementId)
    {
   	CCDRecordBase *record = static_cast<CCDRecordBase*>(CCDRecordBase::RecordFactoryL(aElementId));
    CleanupStack::PushL(record);
    record->LoadL(aSession);
    CleanupStack::Pop(record);
    return record;
    }

CCommsdat190Step::~CCommsdat190Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat190Step::CCommsdat190Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat190Step);                                                                                           
	}

TVerdict CCommsdat190Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
    
	iCDSession = CMDBSession::NewL(KCDVersion1_1);
	
	iCDSession->OpenTransactionL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat190Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
	
	//Store several IAPs and Selection Policies for later testing
	_LIT(KIAPName1, "SelectionTestIAP1");
	_LIT(KIAPName2, "SelectionTestIAP2");
	_LIT(KIAPName3, "SelectionTestIAP3");
	_LIT(KSelName1, "SelectionPolicy1");
	
	_LIT(KModemBearerType, "ModemBearer");
	_LIT(KLANServiceType, "LANService");

	CCDIAPRecord* newIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(newIapRecord);
    CCDIAPPrioritySelectionPolicyRecord *selPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
	CleanupStack::PushL(selPolRecord);

    selPolRecord->SetRecordId(KCDNewRecordRequest);
    selPolRecord->iRecordName.SetMaxLengthL(KSelName1().Length());
    selPolRecord->iRecordName = KSelName1;
    
     	
  	// create a new request
	newIapRecord->SetRecordId(KCDNewRecordRequest);
    newIapRecord->iRecordName.SetMaxLengthL(KIAPName1().Length());
    newIapRecord->iRecordName = KIAPName1;
  	newIapRecord->iNetworkWeighting = 14;    // dummy value
  	newIapRecord->iBearer = 1;
  	newIapRecord->iBearerType.SetMaxLengthL(KModemBearerType().Length());
  	newIapRecord->iBearerType = KModemBearerType;
  	newIapRecord->iService = 1;
  	newIapRecord->iServiceType.SetMaxLengthL(KLANServiceType().Length());
  	newIapRecord->iServiceType = KLANServiceType;    
	newIapRecord->StoreL(*iCDSession);
    INFO_PRINTF2(_L("newIapRecord stored. Element id is %08x"), newIapRecord->ElementId());
	selPolRecord->iIap1 = newIapRecord->ElementId();

	newIapRecord->SetRecordId(KCDNewRecordRequest);
    newIapRecord->iRecordName.SetMaxLengthL(KIAPName2().Length());
    newIapRecord->iRecordName = KIAPName2;
    newIapRecord->iNetworkWeighting = 14;    // dummy value
  	newIapRecord->iBearer = 1;
  	newIapRecord->iBearerType.SetMaxLengthL(KModemBearerType().Length());
  	newIapRecord->iBearerType = KModemBearerType;
  	newIapRecord->iService = 1;
  	newIapRecord->iServiceType.SetMaxLengthL(KLANServiceType().Length());
  	newIapRecord->iServiceType = KLANServiceType; 
	newIapRecord->StoreL(*iCDSession);
    INFO_PRINTF2(_L("newIapRecord stored. Element id is %08x"), newIapRecord->ElementId());
    selPolRecord->iIap2 = newIapRecord->ElementId();
    
	newIapRecord->SetRecordId(KCDNewRecordRequest);
    newIapRecord->iRecordName.SetMaxLengthL(KIAPName3().Length());
    newIapRecord->iRecordName = KIAPName3;
    newIapRecord->iNetworkWeighting = 14;    // dummy value
  	newIapRecord->iBearer = 1;
  	newIapRecord->iBearerType.SetMaxLengthL(KModemBearerType().Length());
  	newIapRecord->iBearerType = KModemBearerType;
  	newIapRecord->iService = 1;
  	newIapRecord->iServiceType.SetMaxLengthL(KLANServiceType().Length());
  	newIapRecord->iServiceType = KLANServiceType; 
	newIapRecord->StoreL(*iCDSession);
    INFO_PRINTF2(_L("newIapRecord stored. Element id is %08x"), newIapRecord->ElementId());
    selPolRecord->iIap3 = newIapRecord->ElementId();

    selPolRecord->StoreL(*iCDSession);
    INFO_PRINTF2(_L("selPolRecord stored. Element id is %08x"), selPolRecord->ElementId());

	CleanupStack::PopAndDestroy(selPolRecord);
	CleanupStack::PopAndDestroy(newIapRecord);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat190Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	iCDSession->CommitTransactionL();
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat191Step::~CCommsdat191Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat191Step::CCommsdat191Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat191Step);                                                                                           
	}

TVerdict CCommsdat191Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(KCDVersion1_1);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat191Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);

	_LIT(KIAPName1, "SelectionTestIAP1"); //Same as used by CCommsdat100Step
	_LIT(KSelName1, "SelectionPolicy1");
	
	CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(selPolRecord);

    //Find a particular IAP Priority Selection Policy record
    selPolRecord->iRecordName.SetMaxLengthL(KSelName1().Length());
    selPolRecord->iRecordName = KSelName1;
	
	TInt found = selPolRecord->FindL(*iCDSession); //This record has been stored by CCommsdat100Step
	if(found)
	    {
    	//Load corresponding IAP record

    	
   	    //This record has been stored by CCommsdat100Step

   	    // Link records work now
        CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(GetConnectedRecordL(*iCDSession, selPolRecord->iIap1));
	    CleanupStack::PushL(iapRecord);
        

        /*selPolRecord->iIap1.LoadL(*iCDSession);
        CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(selPolRecord->iIap1);   
*/
	    TPtrC iapRecordName(iapRecord->iRecordName);
	    if (iapRecordName == KIAPName1)
	        {
	        INFO_PRINTF2(_L("Iap record (%S) successfuly loaded"), &iapRecordName);
	        SetTestStepResult(EPass);
	        }
	    else
	        {
	        INFO_PRINTF3(_L("Iap record (%S) does not match the expected one (%S)"), &iapRecordName, &KIAPName1());
	        }

        CleanupStack::PopAndDestroy(iapRecord); // not necessary - link records work now.
	    }

    CleanupStack::PopAndDestroy(selPolRecord);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat192Step::~CCommsdat192Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat192Step::CCommsdat192Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat192Step);                                                                                           
	}

TVerdict CCommsdat192Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(KCDVersion1_1);

	iCDSession->OpenTransactionL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat192Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
	
	//Store an IAP priority based AP record for later testing
	_LIT(KApName1, "CorporateLAN"); //Same as used by CCommsdat101Step
	_LIT(KSelName1, "SelectionPolicy1");
	const TInt GIDValue = 1;

    CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(selPolRecord);

    //Find a particular IAP Priority Selection Policy record
    selPolRecord->iRecordName.SetMaxLengthL(KSelName1().Length());
    selPolRecord->iRecordName = KSelName1;
		
	TInt found = selPolRecord->FindL(*iCDSession); //This record has been stored by CCommsdat100Step
	if (found)
	    {
	    CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	    CleanupStack::PushL(apRecord);

	    apRecord->SetRecordId(KCDNewRecordRequest);
        apRecord->iRecordName.SetMaxLengthL(KApName1().Length());
        apRecord->iRecordName = KApName1;
        apRecord->iSelectionPolicy = selPolRecord->ElementId();
        apRecord->iAccessPointGID = GIDValue;
   	    apRecord->StoreL(*iCDSession);
        INFO_PRINTF2(_L("apRecord stored. Element id is %08x"), apRecord->ElementId());

	    CleanupStack::PopAndDestroy(apRecord);
	    SetTestStepResult(EPass);
	    }
	
    CleanupStack::PopAndDestroy(selPolRecord);
	return TestStepResult();
	}
	
TVerdict CCommsdat192Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	iCDSession->CommitTransactionL();
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat193Step::~CCommsdat193Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat193Step::CCommsdat193Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat193Step);                                                                                           
	}

TVerdict CCommsdat193Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(KCDVersion1_1);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat193Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);

   	//Load a previously stored IAP priority based AP record
	_LIT(KApName1, "CorporateLAN"); //Same as used by CCommsdat101Step
	_LIT(KSelName1, "SelectionPolicy1");
	_LIT(KIAPName3, "SelectionTestIAP3"); //Same as used by CCommsdat100Step

    CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);

    apRecord->iRecordName.SetL(KApName1);
	TInt found = apRecord->FindL(*iCDSession); //This record has been stored by CCommsdat102Step
	if (found)
	    {
        INFO_PRINTF2(_L("apRecord loaded. Element id is %08x"), apRecord->ElementId());
        
        //Load the Selection Policy record
        //This record has been stored by CCommsdat100Step
        CCDSelectionPolicyRecordBase* selPolRecord = static_cast<CCDSelectionPolicyRecordBase*>(GetConnectedRecordL(*iCDSession, apRecord->iSelectionPolicy));
    	CleanupStack::PushL(selPolRecord);
        

        /*apRecord->iSelectionPolicy.LoadL(*iCDSession);
	    CCDSelectionPolicyRecordBase* selPolRecord = static_cast<CCDSelectionPolicyRecordBase*>(apRecord->iSelectionPolicy);
    	*/
    	
	    TESTL(TPtrC(selPolRecord->iRecordName) == KSelName1);

       	//Cast to the already known type
       	CCDIAPPrioritySelectionPolicyRecord* iapSelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(selPolRecord);

   	    //This record has been stored by CCommsdat100Step
        CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(GetConnectedRecordL(*iCDSession, iapSelPolRec->iIap3));
        CleanupStack::PushL(iapRecord);
        

   	    /*iapSelPolRec->iIap3.LoadL(*iCDSession);
	    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(iapSelPolRec->iIap3);*/
       
   	    TPtrC iapRecordName(iapRecord->iRecordName);
   	    if(iapRecordName == KIAPName3)
   	        {
   	        INFO_PRINTF2(_L("Iap record (%S) successfuly loaded"), &iapRecordName);
   	        SetTestStepResult(EPass);
   	        }
   	    else
   	        {
   	        INFO_PRINTF3(_L("Iap record (%S) does not match the expected one (%S)"), &iapRecordName, &KIAPName3());
   	        }
        
        CleanupStack::PopAndDestroy(iapRecord);
	    CleanupStack::PopAndDestroy(selPolRecord);
	    
	    }
	CleanupStack::PopAndDestroy(apRecord);

	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------

CCommsdat194Step::~CCommsdat194Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat194Step::CCommsdat194Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat194Step);                                                                                           
	}

TVerdict CCommsdat194Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(KCDVersion1_1);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat194Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);

   	//Load a previously stored IAP priority based AP record
	_LIT(KApName1, "CorporateLAN"); //Same as used by CCommsdat101Step
	_LIT(KSelName1, "SelectionPolicy1");

    CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);

    apRecord->iRecordName.SetL(KApName1);
	TInt found = apRecord->FindL(*iCDSession); //This record has been stored by CCommsdat102Step
	if (found)
	    {
        INFO_PRINTF2(_L("apRecord loaded. Element id is %08x"), apRecord->ElementId());
        
        //Load the Selection Policy record
        //This record has been stored by CCommsdat100Step
        CCDSelectionPolicyRecordBase* selPolRecord = static_cast<CCDSelectionPolicyRecordBase*>(GetConnectedRecordL(*iCDSession, apRecord->iSelectionPolicy));
    	CleanupStack::PushL(selPolRecord);
    	
        /*apRecord->iSelectionPolicy.LoadL(*iCDSession);
        CCDSelectionPolicyRecordBase* selPolRecord = static_cast<CCDSelectionPolicyRecordBase*>(apRecord->iSelectionPolicy);
    	*/
    	TESTL(TPtrC(selPolRecord->iRecordName) == KSelName1);

        //Build the list of IDs
        RArray<TInt> iapIDs;
        CleanupClosePushL(iapIDs);
       	
       	const SRecordTypeInfo* recordInfo = selPolRecord->GetRecordInfo();
       	CMDBRecordLink<CCDIAPRecord>* iapRecordLink = NULL;
       	// Get only 3 fields from the record because the previous test only set 3 fields 
        TInt index(0);

        while ( (recordInfo + index) != NULL  &&  (recordInfo + index)->iTypeId != KCDTIdIap1 )
            {
            // The first field you want to read might not be at the beginning of the table
            ++index;
            }
        /*  
        changed the code because this was too fragile, depending on non-public info (i.e. the order of the fields in the recordinfo table)
        for( TInt index = 2;
			 index < 5 && (iapRecordLink = static_cast<CMDBRecordLink<CCDIAPRecord>*>(selPolRecord->GetFieldByIdL((recordInfo + index)->iTypeId))) != NULL;
       	     index++ )
       	    {
        */
        TInt typeId = (recordInfo + index)->iTypeId ;
        while ( typeId != NULL  &&  typeId <= KCDTIdIap3 )
            {
            iapRecordLink = static_cast<CMDBRecordLink<CCDIAPRecord>*>(selPolRecord->GetFieldByIdL(typeId));

            CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(GetConnectedRecordL(*iCDSession, *iapRecordLink));
            //iapRecordLink->LoadL(*iCDSession);
       	    //CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(*iapRecordLink);

            TPtrC iapRecName(iapRecord->iRecordName);
       	    TUint32 iapId = iapRecord->RecordId();
       	    if (iapRecName.Length() > 0 && iapId > 0)
       	        {
       	        iapIDs.Append(iapId);
       	        }
       	    //delete iapRecord;
            typeId = (++recordInfo + index)->iTypeId;
       	    }
       	
       	if(iapIDs.Count() == 3)
   	        {
   	        INFO_PRINTF1(_L("Iap records successfuly loaded"));
   	        SetTestStepResult(EPass);
   	        }
   	    else
   	        {
   	        INFO_PRINTF1(_L("Iap records not found!"));
   	        }
   	    CleanupStack::PopAndDestroy(); //Call Close() on iapIDs;
	    CleanupStack::PopAndDestroy(selPolRecord);
	    }
	CleanupStack::PopAndDestroy(apRecord);

	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------

CCommsdat195Step::~CCommsdat195Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat195Step::CCommsdat195Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat195Step);
	}

TVerdict CCommsdat195Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(KCDVersion1_1);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat195Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
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
        
        //CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(GetConnectedRecordL(*iCDSession, apRecord->iSelectionPolicy));
    	//CleanupStack::PushL(selPolRecord);
        
        apRecord->iSelectionPolicy.LoadL(*iCDSession);
    	//CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(apRecord->iSelectionPolicy);
    	
    	CCDSelectionPolicyRecordBase* selPolRecord1 = static_cast<CCDSelectionPolicyRecordBase*>(apRecord->iSelectionPolicy);
    	
    	CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(selPolRecord1);
    	 
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
	    }
	CleanupStack::PopAndDestroy(apRecord);
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------

CCommsdat196Step::~CCommsdat196Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat196Step::CCommsdat196Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat196Step);                                                                                           
	}

TVerdict CCommsdat196Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(KCDVersion1_1);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat196Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	_LIT(KSelName2, "SelectionPolicy2");
	const TUint KRecordId = 2;
	
	CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);
	
	apRecord->SetRecordId(KRecordId); //load record 2 which links to a policy with 3 IAPs.
	apRecord->LoadL(*iCDSession);
	
	INFO_PRINTF2(_L("Access Point Record loaded. Element id is %08x"), apRecord->ElementId());
        
    //Load the Selection Policy record
    //CCDAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDConnectionPrefsRecord::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
    CCDAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
    CleanupStack::PushL(selPolRecord);
    selPolRecord->SetRecordId(KRecordId);
    selPolRecord->LoadL(*iCDSession);

    TESTL(TPtrC(selPolRecord->iRecordName) == KSelName2);

    //Build the list of IDs
    RArray<TInt> iapIDs;
    CleanupClosePushL(iapIDs);
       	
//   	CMDBField<TUint32>* theAp = &selPolRecord->iAp1;   	
   	CMDBRecordLink<CCDAccessPointRecord>* theAp = &selPolRecord->iAp1;
	TUint32 theCount = selPolRecord->iApCount;	
	if (theCount != 3)
		{
		INFO_PRINTF2(_L("IAP count in the policy (%) is invalid"), theCount);
		TESTL(EFalse);
		}

   	for (TInt i = 0; i < theCount; i++, theAp++)
		{
		TInt theIapID = static_cast<TInt>(*theAp);
		TESTL(theIapID>0);
		iapIDs.Append(theIapID);
		}

	if (iapIDs.Count() == 3)
		{
		INFO_PRINTF1(_L("Iap records successfuly loaded"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF1(_L("Iap records not found!"));
		}	
	
	CleanupStack::PopAndDestroy(&iapIDs);
	CleanupStack::PopAndDestroy(selPolRecord);
	CleanupStack::PopAndDestroy(apRecord);
		
#else
	_LIT(KSelName2, "SelectionPolicy2");

    CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);

    apRecord->SetRecordId(2); //load record 2 which links to a policy with 3 IAPs.
	apRecord->LoadL(*iCDSession);

    INFO_PRINTF2(_L("Access Point Record loaded. Element id is %08x"), apRecord->ElementId());
        
    //Load the Selection Policy record
    
    //CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(GetConnectedRecordL(*iCDSession, apRecord->iSelectionPolicy));
  	//CleanupStack::PushL(selPolRecord);
    

    apRecord->iSelectionPolicy.LoadL(*iCDSession);
    //CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(apRecord->iSelectionPolicy);
  	CCDSelectionPolicyRecordBase* selPolRecord1 = static_cast<CCDSelectionPolicyRecordBase*>(apRecord->iSelectionPolicy);
    	
    CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(selPolRecord1);
    	
    TESTL(TPtrC(selPolRecord->iRecordName) == KSelName2);

    //Build the list of IDs
    RArray<TInt> iapIDs;
    CleanupClosePushL(iapIDs);
       	
   	CMDBRecordLink<CCDIAPRecord>* theIap = &selPolRecord->iIap1;
	CMDBField<TUint32>* theCount = &selPolRecord->iIapCount;
	TUint count = *theCount;
	if (count != 3)
		{
		INFO_PRINTF2(_L("IAP count in the policy (%) is invalid"), count);
		TESTL(EFalse);
		}

   	for (TInt i = 0; i < count; i++,theIap++)
		{
		TInt theIapID = static_cast<TInt>(*theIap);
		TESTL(theIapID>0);
		iapIDs.Append(theIapID);
		}

	if (iapIDs.Count() == 3)
		{
		INFO_PRINTF1(_L("Iap records successfuly loaded"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF1(_L("Iap records not found!"));
		}
	
	CleanupStack::PopAndDestroy(&iapIDs);
	CleanupStack::PopAndDestroy(apRecord);
#endif

	return TestStepResult();
	}
//---------------------------------------
//				FIELD TESTS
//---------------------------------------
//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat307Step::~CCommsdat307Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat307Step::CCommsdat307Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat307Step);                                                                                           
	}

TVerdict CCommsdat307Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat307Step::doTestStepL()
/**
 * @return - TVerdict code
 * Change the PromptForSnap field in the GlobalSettings record to TRUE
 */
	{
	SetTestStepResult(EFail);
	
	//create the container for the prompt field in the GS
	CMDBField<TBool>* promptForSnapField = new(ELeave) CMDBField<TBool>(KCDTIdPromptForSnap);
	CleanupStack::PushL(promptForSnapField);
	promptForSnapField->SetRecordId(1);
	
	//create the container for the prompt field in the tier rec.
	
	CMDBField<TBool>* promptTierField = new(ELeave) CMDBField<TBool>(KCDTIdPromptUser);
	CleanupStack::PushL(promptTierField);
	promptTierField->SetRecordId(1);
	
	//This loads the 'PromptUser' field from the default Network tier record.
	promptForSnapField->LoadL(*iCDSession_v11);
	promptTierField->LoadL(*iCDSession_vLatest);
	
	if (*promptTierField == *promptForSnapField)
		{
		*promptForSnapField = ETrue;
		promptForSnapField->ModifyL(*iCDSession_v11);
		
		CMDBField<TBool>* promptForSnapField2 = new(ELeave) CMDBField<TBool>(KCDTIdPromptForSnap);
		CleanupStack::PushL(promptForSnapField2);
		promptForSnapField2->SetRecordId(1);
		
		promptForSnapField2->LoadL(*iCDSession_v11);
		promptTierField->LoadL(*iCDSession_vLatest);
		
		if (1 == *promptTierField &&
			1 == *promptForSnapField2)
			{
			SetTestStepResult(EPass);
			}
		CleanupStack::PopAndDestroy(promptForSnapField2);
		}
	
	CleanupStack::PopAndDestroy(promptTierField);
	CleanupStack::PopAndDestroy(promptForSnapField);
	
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat308Step::~CCommsdat308Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat308Step::CCommsdat308Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat308Step);                                                                                           
	}

TVerdict CCommsdat308Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat308Step::doTestStepL()
/**
 * @return - TVerdict code
 * Load the GlobalSettings record from the DB.
 * This test step is dependent form the CCommsdat307Step!!
 */
	{
	SetTestStepResult(EFail);
	
	CCDGlobalSettingsRecord* gsRecord = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
	CleanupStack::PushL(gsRecord);
	
	gsRecord->SetRecordId(1);
	gsRecord->LoadL(*iCDSession_v11);
	
	CMDBField<TBool>* promptForSnap = &(gsRecord->iPromptForSnap);
	
	if ( 1 == *promptForSnap )
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(gsRecord);
		
	return TestStepResult();
	}

#endif
//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat309Step::~CCommsdat309Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat309Step::CCommsdat309Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat309Step);                                                                                           
	}

TVerdict CCommsdat309Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat309Step::doTestStepL()
/**
 * @return - TVerdict code
 * Load the defaultSnap field formthe GS record as a link field.
 * This test step is dependent form the CCommsdat307Step!!
 */
	{
	SetTestStepResult(EFail);
	
	CMDBRecordLink<CCDAccessPointRecord>* gsDefaultSnap = new (ELeave) CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultSnap);
	CMDBRecordLink<CCDAccessPointRecord>* tierDefaultAP = new (ELeave) CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultAccessPoint);
	CleanupStack::PushL(gsDefaultSnap);
	CleanupStack::PushL(tierDefaultAP);
	
	gsDefaultSnap->SetRecordId(1);
	gsDefaultSnap->LoadL(*iCDSession_v11);
	
	tierDefaultAP->SetRecordId(1);
	tierDefaultAP->LoadL(*iCDSession_vLatest);
	
	if ( 1 == *gsDefaultSnap &&
		 1 == ((*tierDefaultAP & KCDMaskShowRecordId) >> 8) ) 
		{
		//ok, the 2 fields are the same
		*gsDefaultSnap = 2;
		gsDefaultSnap->ModifyL(*iCDSession_v11);
		
		CMDBRecordLink<CCDAccessPointRecord>* gsDefaultSnap1 = new (ELeave) CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultSnap);
		CMDBRecordLink<CCDAccessPointRecord>* tierDefaultAP1 = new (ELeave) CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultAccessPoint);
		CleanupStack::PushL(gsDefaultSnap1);
		CleanupStack::PushL(tierDefaultAP1);
		
		//let's check them once again...
		gsDefaultSnap1->SetRecordId(1);
		gsDefaultSnap1->LoadL(*iCDSession_v11);
		
		tierDefaultAP1->SetRecordId(1);
		tierDefaultAP1->LoadL(*iCDSession_vLatest);
		
		if ( 2 == *gsDefaultSnap1 &&
			 2 == ((*tierDefaultAP1 & KCDMaskShowRecordId) >> 8) )
			{
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(tierDefaultAP1);
		CleanupStack::PopAndDestroy(gsDefaultSnap1);
		}
	
	CleanupStack::PopAndDestroy(tierDefaultAP);
	CleanupStack::PopAndDestroy(gsDefaultSnap);
		
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat310Step::~CCommsdat310Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat310Step::CCommsdat310Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat310Step);                                                                                           
	}

TVerdict CCommsdat310Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat310Step::doTestStepL()
/**
 * @return - TVerdict code
 * Load the GlobalSettings record.
 * This test step is dependent form the CCommsdat309Step!!
 */
	{
	SetTestStepResult(EFail);
	
	CCDGlobalSettingsRecord* gsRecord = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
	CCDTierRecord* tierRecord = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
	
	CleanupStack::PushL(gsRecord);
	CleanupStack::PushL(tierRecord);
	
	gsRecord->SetRecordId(1);
	gsRecord->LoadL(*iCDSession_v11);
	
	tierRecord->SetRecordId(1);
	tierRecord->LoadL(*iCDSession_vLatest);
	
	CMDBRecordLink<CCDAccessPointRecord>* gsDefaultSnap = &(gsRecord->iDefaultSnap);
	CMDBRecordLink<CCDAccessPointRecord>* tierDefaultAP = &(tierRecord->iDefaultAccessPoint);
	
	if ( 2 == *gsDefaultSnap &&
		 2 == ((*tierDefaultAP & KCDMaskShowRecordId) >> 8) )
		{
		*gsDefaultSnap = 1;
		
		gsRecord->ModifyL(*iCDSession_v11);
		
		gsRecord->SetRecordId(1);
		gsRecord->LoadL(*iCDSession_v11);
		
		tierRecord->SetRecordId(1);
		tierRecord->LoadL(*iCDSession_vLatest);
		
		gsDefaultSnap = &(gsRecord->iDefaultSnap);
		tierDefaultAP = &(tierRecord->iDefaultAccessPoint);
		
		if ( 1 == *gsDefaultSnap &&
			 1 == ((*tierDefaultAP & KCDMaskShowRecordId) >> 8) )
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(tierRecord);
	CleanupStack::PopAndDestroy(gsRecord);
		
	return TestStepResult();
	}

//IAPPrioritySelectionPolicy mapping

//----------------------------------------------------------------------------------------------------------------

CCommsdat311Step::~CCommsdat311Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat311Step::CCommsdat311Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat311Step);                                                                                           
	}

TVerdict CCommsdat311Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat311Step::doTestStepL()
/**
 * @return - TVerdict code
 * Insert 1 more IAP link in to the default IAPPrioritySelectionPolicy record.
 */
	{
	SetTestStepResult(EFail);
	
	//first insert the new IAP record. This will trigger the generation of the
	//linked link level AP.
	_LIT(KIAPName1, "The new IAP");
	_LIT(KModemBearerType, "ModemBearer");
	_LIT(KLANServiceType, "LANService");

	CCDIAPRecord* newIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(newIapRecord);

	newIapRecord->SetRecordId(KCDNewRecordRequest);
    newIapRecord->iRecordName.SetMaxLengthL(KIAPName1().Length());
    newIapRecord->iRecordName = KIAPName1;
  	newIapRecord->iBearer = 2;
  	newIapRecord->iBearerType.SetMaxLengthL(KModemBearerType().Length());
  	newIapRecord->iBearerType = KModemBearerType;
  	newIapRecord->iService = 1;
  	newIapRecord->iServiceType.SetMaxLengthL(KLANServiceType().Length());
  	newIapRecord->iServiceType = KLANServiceType;
  	newIapRecord->iLocation = 2;
  	newIapRecord->iNetworkWeighting = 14;    // dummy value
  	newIapRecord->iNetwork = 1;
  	
	newIapRecord->StoreL(*iCDSession_v11);
    INFO_PRINTF2(_L("newIapRecord stored. Element id is %08x"), newIapRecord->ElementId());
	
    //Now read the default IAPPrioritySelectionPolicy record.
    CCDIAPPrioritySelectionPolicyRecord *selPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
	CleanupStack::PushL(selPolRecord);
	//We just know that this is the default IAPPrioritySelPol record.
	selPolRecord->SetRecordId(1);

	selPolRecord->LoadL(*iCDSession_v11);
	
	//insert the new IAP link.
	selPolRecord->iIap2 = newIapRecord->ElementId();
	selPolRecord->ModifyL(*iCDSession_v11);
	
	/*
	 * Now with the 1_1  session let's read the IAPPrioritySelPol record and with the latest session the mapped
	 * APPrioritySelPol record. The rule is that the recordIDs of the linked IAP records from the IAPPrioritySelPol
	 * record should be the same as the CustomSelectionPolicy fields of the linked APs from the mapped APPrioritySelPol
	 * record.
	 */
	
	//Finding out the elementIds because here we create new containers for the loading of those...
	TMDBElementId iapSelPolElemId = selPolRecord->ElementId();
	TMDBElementId apSelPolElemId = 0; 
	
	CleanupStack::PopAndDestroy(selPolRecord);
	CleanupStack::PopAndDestroy(newIapRecord);
	
	//search for the mapped APPrioritySelPol record based on the name
	CMDBField<TDesC>* appriorityNameField = new (ELeave) CMDBField<TDesC>(KCDTIdApPrioritySelectionPolicyRecord | KCDTIdRecordName);
	CleanupStack::PushL(appriorityNameField);
	
	_LIT(KSelName1, "SelectionPolicy1");
	
	appriorityNameField->SetMaxLengthL(KSelName1().Length());
	appriorityNameField->SetL(KSelName1);
	
	if ( appriorityNameField->FindL(*iCDSession_vLatest) )
		{
		apSelPolElemId = appriorityNameField->ElementId();
		
		CCDIAPPrioritySelectionPolicyRecord *iapSelPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
		CleanupStack::PushL(iapSelPolRecord);
		
		CCDAPPrioritySelectionPolicyRecord *apSelPolRecord = (CCDAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord);
		CleanupStack::PushL(apSelPolRecord);
		
		iapSelPolRecord->SetElementId(iapSelPolElemId);
		apSelPolRecord->SetElementId(apSelPolElemId & ~KCDMaskShowFieldType);
		
		iapSelPolRecord->LoadL(*iCDSession_v11);
		apSelPolRecord->LoadL(*iCDSession_vLatest);
		
		CMDBField<TDesC>* iapSelPolFieldName = &(iapSelPolRecord->iRecordName);
		CMDBField<TDesC>* apSelPolFieldName = &(apSelPolRecord->iRecordName);
		
		TPtrC iapSelPolname(*iapSelPolFieldName);
		TPtrC apSelPolname(*apSelPolFieldName);
		
		if (iapSelPolname == apSelPolname &&
			iapSelPolRecord->iIapCount == apSelPolRecord->iApCount)
			{
			//let's check the customSelectionPolicy values of the pointed AP records
			//from the APPrioritySelPol record
			CMDBField<TInt>* custSelPol = new (ELeave) CMDBField<TInt>(KCDTIdCustomSelectionPolicy);
			CleanupStack::PushL(custSelPol);
			
			CMDBRecordLink<CCDAccessPointRecord>* apXField = &(apSelPolRecord->iAp1);
			CMDBRecordLink<CCDIAPRecord>* iapXField = &(iapSelPolRecord->iIap1);
			TBool recordIDsAreTheSame = ETrue;
			for (TInt i = 1; i <= apSelPolRecord->iApCount && recordIDsAreTheSame; ++i)
				{
				custSelPol->SetRecordId((*apXField & KCDMaskShowRecordId) >> 8);
				custSelPol->LoadL(*iCDSession_vLatest);
				if ( *custSelPol != ((*iapXField & KCDMaskShowRecordId) >> 8) )
					{
					recordIDsAreTheSame = EFalse;
					}
				++apXField;
				++iapXField;
				}
			
			CleanupStack::PopAndDestroy(custSelPol);
			
			if (recordIDsAreTheSame)
				{
				SetTestStepResult(EPass);
				}
			}
		
		CleanupStack::PopAndDestroy(apSelPolRecord);
		CleanupStack::PopAndDestroy(iapSelPolRecord);
		}
	
    CleanupStack::PopAndDestroy(appriorityNameField);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat312Step::~CCommsdat312Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat312Step::CCommsdat312Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat312Step);                                                                                           
	}

TVerdict CCommsdat312Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat312Step::doTestStepL()
/**
 * @return - TVerdict code
 * Insert 1 IAP link is deleted form the  IAPPrioritySelectionPolicy record, inserted
 * in the previous test step.
 * This test ste is dependent from CCommsdat312Step 
 */
	{
	SetTestStepResult(EFail);
	
    CCDIAPPrioritySelectionPolicyRecord *selPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
	CleanupStack::PushL(selPolRecord);
	//We just know that this is the 2nd record.
	selPolRecord->SetRecordId(2);
	
	selPolRecord->LoadL(*iCDSession_v11);
	
	//and now let's remove the 2nd IAP link so this will cause a 'hole' in the IAP links in the record.
	selPolRecord->iIap2 = 0;
	
	selPolRecord->ModifyL(*iCDSession_v11);
	
	//Finding out the elementIds because here we create new containers for the loading of those...
	TMDBElementId iapSelPolElemId = selPolRecord->ElementId();
	TMDBElementId apSelPolElemId = 0; 
	
	CleanupStack::PopAndDestroy(selPolRecord);
	
	//search for the mapped APPrioritySelPol record based on the name
	CMDBField<TDesC>* appriorityNameField = new (ELeave) CMDBField<TDesC>(KCDTIdApPrioritySelectionPolicyRecord | KCDTIdRecordName);
	CleanupStack::PushL(appriorityNameField);
	
	_LIT(KSelName1, "SelectionPolicy2");
		
	appriorityNameField->SetMaxLengthL(KSelName1().Length());
	appriorityNameField->SetL(KSelName1);
	
	if ( appriorityNameField->FindL(*iCDSession_vLatest) )
		{
		apSelPolElemId = appriorityNameField->ElementId();
		
		CCDIAPPrioritySelectionPolicyRecord *iapSelPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
		CleanupStack::PushL(iapSelPolRecord);
		
		CCDAPPrioritySelectionPolicyRecord *apSelPolRecord = (CCDAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord);
		CleanupStack::PushL(apSelPolRecord);
		
		iapSelPolRecord->SetElementId(iapSelPolElemId);
		apSelPolRecord->SetElementId(apSelPolElemId & ~KCDMaskShowFieldType);
		
		iapSelPolRecord->LoadL(*iCDSession_v11);
		apSelPolRecord->LoadL(*iCDSession_vLatest);
		
		CMDBField<TDesC>* iapSelPolFieldName = &(iapSelPolRecord->iRecordName);
		CMDBField<TDesC>* apSelPolFieldName = &(apSelPolRecord->iRecordName);
		
		TPtrC iapSelPolname(*iapSelPolFieldName);
		TPtrC apSelPolname(*apSelPolFieldName);
		
		if (iapSelPolname == apSelPolname &&
			iapSelPolRecord->iIapCount == apSelPolRecord->iApCount)
			{
			//let's check the customSelectionPolicy values of the pointed AP records
			//from the APPrioritySelPol record
			CMDBField<TInt>* custSelPol = new (ELeave) CMDBField<TInt>(KCDTIdCustomSelectionPolicy);
			CleanupStack::PushL(custSelPol);
			
			CMDBRecordLink<CCDAccessPointRecord>* apXField = &(apSelPolRecord->iAp1);
			CMDBRecordLink<CCDIAPRecord>* iapXField = &(iapSelPolRecord->iIap1);
			TBool recordIDsAreTheSame = ETrue;
			TInt err = 0;
			for (TInt i = 1; i <= apSelPolRecord->iApCount && recordIDsAreTheSame; ++i)
				{
				custSelPol->SetRecordId((*apXField & KCDMaskShowRecordId) >> 8);
				TRAP(err, custSelPol->LoadL(*iCDSession_vLatest));
				if (KErrNotFound == err && *iapXField == 0 && *apXField == 0)
					{
					//it's ok
					recordIDsAreTheSame = ETrue;
					}
				else if ( *custSelPol != ((*iapXField & KCDMaskShowRecordId) >> 8) )
					{
					recordIDsAreTheSame = EFalse;
					}
				++apXField;
				++iapXField;
				}
			
			CleanupStack::PopAndDestroy(custSelPol);
			
			if (recordIDsAreTheSame)
				{
				SetTestStepResult(EPass);
				}
			}
		
		CleanupStack::PopAndDestroy(apSelPolRecord);
		CleanupStack::PopAndDestroy(iapSelPolRecord);
		}
	
    CleanupStack::PopAndDestroy(appriorityNameField);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat313Step::~CCommsdat313Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat313Step::CCommsdat313Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat313Step);                                                                                           
	}

TVerdict CCommsdat313Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat313Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete 1 IAPPriorirytSelPol record form the DB - not the default one...
 */
	{
	SetTestStepResult(EFail);
	
    CCDIAPPrioritySelectionPolicyRecord *iapSelPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
	CleanupStack::PushL(iapSelPolRecord);
	//We just know that this is the 2nd record.
	iapSelPolRecord->SetRecordId(2);
	
	iapSelPolRecord->LoadL(*iCDSession_v11);
	
	CCDAPPrioritySelectionPolicyRecord *apSelPolRecord = (CCDAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord);
	CleanupStack::PushL(apSelPolRecord);
	
	apSelPolRecord->SetRecordId(iapSelPolRecord->RecordId());
	apSelPolRecord->LoadL(*iCDSession_vLatest);
	
	RArray<TInt> linkedIAPIds;
	CleanupClosePushL(linkedIAPIds);
	
	CMDBRecordLink<CCDAccessPointRecord>* apXLink = &(apSelPolRecord->iAp1);
	
	for (TInt i = 1; i <= apSelPolRecord->iApCount; ++i)
		{
		linkedIAPIds.Append(*apXLink);
		++apXLink;
		}
	
	iapSelPolRecord->DeleteL(*iCDSession_v11);
	
	//from this point none the original IAPPrioritySelPol record, nor the mapped APPrioritySelPol record 
	//should be available. However all of the linked AP records from the APPrioritySelPol 
	//should be available because as originally there were more thna 1 links to each.
	TInt err = 0;
	
	TRAP(err, iapSelPolRecord->LoadL(*iCDSession_v11));
	if (KErrNotFound == err)
		{
		TRAP(err, apSelPolRecord->LoadL(*iCDSession_vLatest));
		if (KErrNotFound == err)
			{
			CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(apRec);
			
			TInt i = 0;
			
			err = KErrNone;
			
			for (i = 0; (i < linkedIAPIds.Count()) && (KErrNone == err); ++i)
				{
				apRec->SetElementId(linkedIAPIds[i]);
				TRAP(err, apRec->LoadL(*iCDSession_vLatest));
				}
			
			CleanupStack::PopAndDestroy(apRec);
			
			if (KErrNone == err &&
				i == linkedIAPIds.Count()	)
				{
				SetTestStepResult(EPass);
				}
			}
		}
		
	CleanupStack::PopAndDestroy(&linkedIAPIds);
	CleanupStack::PopAndDestroy(apSelPolRecord);
	CleanupStack::PopAndDestroy(iapSelPolRecord);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat314Step::~CCommsdat314Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat314Step::CCommsdat314Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat314Step);                                                                                           
	}

TVerdict CCommsdat314Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat314Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete 1 IAPPriorirytSelPol record form the DB - now the default one...
 */
	{
	SetTestStepResult(EFail);
	
    CCDIAPPrioritySelectionPolicyRecord *iapSelPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
	CleanupStack::PushL(iapSelPolRecord);
	//We just know that this is the 1st record.
	iapSelPolRecord->SetRecordId(1);
	
	iapSelPolRecord->LoadL(*iCDSession_v11);
	
	CCDAPPrioritySelectionPolicyRecord *apSelPolRecord = (CCDAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord);
	CleanupStack::PushL(apSelPolRecord);
	
	apSelPolRecord->SetRecordId(iapSelPolRecord->RecordId());
	apSelPolRecord->LoadL(*iCDSession_vLatest);
	
	RArray<TInt> linkedIAPIds;
	CleanupClosePushL(linkedIAPIds);
	
	CMDBRecordLink<CCDAccessPointRecord>* apXLink = &(apSelPolRecord->iAp1);
	
	for (TInt i = 1; i <= apSelPolRecord->iApCount; ++i)
		{
		linkedIAPIds.Append(*apXLink);
		++apXLink;
		}
	
	iapSelPolRecord->DeleteL(*iCDSession_v11);
	
	//from this point none the original IAPPrioritySelPol record, nor the mapped APPrioritySelPol record 
	//can be read. The IPProto TierRecord should point to the default IPProto AP (form the meshpreface file)
	TInt err = 0;
	
	TRAP(err, iapSelPolRecord->LoadL(*iCDSession_v11));
	if (KErrNotFound == err)
		{
		TRAP(err, apSelPolRecord->LoadL(*iCDSession_vLatest));
		if (KErrNotFound == err)
			{
			//default IPProto Tier has to point to the default IPProto AP
			CCDAccessPointRecord* defIPProtoAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(defIPProtoAPRec);
			
			defIPProtoAPRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
			if (defIPProtoAPRec->FindL(*iCDSession_vLatest))
				{
				TPtrC recName(defIPProtoAPRec->iRecordName);
				_LIT(KIPProtoDefRecName, "IPProtoDefault");
				
				if (0 == recName.Compare(KIPProtoDefRecName))
					{
					CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
					CleanupStack::PushL(ipprotoTierRec);
					
					//we just know that the given record is the 2nd one (it's possible to do a FindL based
					//on the IPProto tagId)
					ipprotoTierRec->SetRecordId(2);
					ipprotoTierRec->LoadL(*iCDSession_vLatest);
					
					if (ipprotoTierRec->iDefaultAccessPoint == (defIPProtoAPRec->ElementId() & ~KCDMaskShowFieldType))
						{
						//based on the base configuration no IPProto AP should be deleted...
						
						CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
						CleanupStack::PushL(apRec);
						
						TInt i = 0;
						
						TInt err = KErrNone;
						
						for (i = 0; (i < linkedIAPIds.Count()) && (KErrNone == err); ++i)
							{
							apRec->SetElementId(linkedIAPIds[i]);
							TRAP(err, apRec->LoadL(*iCDSession_vLatest));
							}
						
						CleanupStack::PopAndDestroy(apRec);
						
						if (KErrNone == err &&
							i == linkedIAPIds.Count()	)
							{
							SetTestStepResult(EPass);
							}
						}
					CleanupStack::PopAndDestroy(ipprotoTierRec);
					}
				}
			
			CleanupStack::PopAndDestroy(defIPProtoAPRec);
			}
		}
		
	CleanupStack::PopAndDestroy(&linkedIAPIds);
	CleanupStack::PopAndDestroy(apSelPolRecord);
	CleanupStack::PopAndDestroy(iapSelPolRecord);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat315Step::~CCommsdat315Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat315Step::CCommsdat315Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat315Step);                                                                                           
	}

TVerdict CCommsdat315Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat315Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify 1 IAP link through a field container in the 2nd IAPPriorirytSelPol record.
 */
	{
	SetTestStepResult(EFail);
	
	CCDIAPRecord* linkedIAP = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(linkedIAP);
	
	linkedIAP->SetRecordId(3);
	linkedIAP->LoadL(*iCDSession_v11);
	
	CMDBRecordLink<CCDIAPRecord>* iap5Link = new(ELeave) CMDBRecordLink<CCDIAPRecord>(KCDTIdIap5);
	CleanupStack::PushL(iap5Link);
    
	iap5Link->SetRecordId(2);
	*iap5Link = linkedIAP->ElementId();
	
	//now modify the 2nd IAPPrioritySelPol record through a link container
	iap5Link->ModifyL(*iCDSession_v11);

	CleanupStack::PopAndDestroy(iap5Link);
	CleanupStack::PopAndDestroy(linkedIAP);
	
	//check the results...
	CCDIAPPrioritySelectionPolicyRecord* iapSelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(iapSelPolRec);
	
	CCDAPPrioritySelectionPolicyRecord* apSelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
	CleanupStack::PushL(apSelPolRec);
	
	iapSelPolRec->SetRecordId(2);
	apSelPolRec->SetRecordId(2);
	
	iapSelPolRec->LoadL(*iCDSession_v11);
	apSelPolRec->LoadL(*iCDSession_vLatest);
	
	CMDBField<TDesC>* iapSelPolFieldName = &(iapSelPolRec->iRecordName);
	CMDBField<TDesC>* apSelPolFieldName = &(apSelPolRec->iRecordName);
	
	TPtrC iapSelPolname(*iapSelPolFieldName);
	TPtrC apSelPolname(*apSelPolFieldName);
	
	if (iapSelPolname == apSelPolname &&
		iapSelPolRec->iIapCount == apSelPolRec->iApCount)
		{
		//let's check the customSelectionPolicy values of the pointed AP records
		//from the APPrioritySelPol record
		CMDBField<TInt>* custSelPol = new (ELeave) CMDBField<TInt>(KCDTIdCustomSelectionPolicy);
		CleanupStack::PushL(custSelPol);
		
		CMDBRecordLink<CCDAccessPointRecord>* apXField = &(apSelPolRec->iAp1);
		CMDBRecordLink<CCDIAPRecord>* iapXField = &(iapSelPolRec->iIap1);
		TBool recordIDsAreTheSame = ETrue;
		
		for (TInt i = 1; i <= apSelPolRec->iApCount && recordIDsAreTheSame; ++i)
			{
			if (4 == i)
				{
				//if we read the 4th link to LoadL should be called
				if ((0 != *iapXField) || (0 != *apXField))
					{
					//the AP4/IAP4 links have to 0!
					recordIDsAreTheSame = EFalse;
					break;
					}
				}
			else
				{
				custSelPol->SetRecordId((*apXField & KCDMaskShowRecordId) >> 8);
				custSelPol->LoadL(*iCDSession_vLatest);
				
				if ( *custSelPol != ((*iapXField & KCDMaskShowRecordId) >> 8) )
					{
					recordIDsAreTheSame = EFalse;
					}
				}
				++apXField;
				++iapXField;
			}
		
		CleanupStack::PopAndDestroy(custSelPol);
		
		if (recordIDsAreTheSame)
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(apSelPolRec);
	CleanupStack::PopAndDestroy(iapSelPolRec);
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat316Step::~CCommsdat316Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat316Step::CCommsdat316Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat316Step);                                                                                           
	}

TVerdict CCommsdat316Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat316Step::doTestStepL()
/**
 * @return - TVerdict code
 * Find an IAPPrioritySelPol record based on an IAP link value...
 * find that link in the IAP/APSelPol table.
 */
	{
	SetTestStepResult(EFail);
	
	
	CMDBRecordLink<CCDIAPRecord>* iap5Link = new(ELeave) CMDBRecordLink<CCDIAPRecord>(KCDTIdIap5);
	CleanupStack::PushL(iap5Link);
    
	iap5Link->SetRecordId(2);
	*iap5Link = 1;
	
	iap5Link->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iap5Link);
	
	CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(selPolRecord);
	
	selPolRecord->iIap5 = 1;
	
	if (selPolRecord->FindL(*iCDSession_v11))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(selPolRecord);
	
	
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat317Step::~CCommsdat317Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat317Step::CCommsdat317Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat317Step);                                                                                           
	}

TVerdict CCommsdat317Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat317Step::doTestStepL()
/**
 * @return - TVerdict code
 * Find an IAPPrioritySelPol field based on an IAP link value...
 */
	{
	SetTestStepResult(EFail);
	
	CMDBRecordLink<CCDIAPRecord>* iapLinkField = new(ELeave) CMDBRecordLink<CCDIAPRecord>(KCDTIdIap3);
	CleanupStack::PushL(iapLinkField);
	
	//this means - give me back the first IAPPrioritySelPol field which has the value of 3
	*iapLinkField = 3;
	
	if (iapLinkField->FindL(*iCDSession_v11))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(iapLinkField);
	
	
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat318Step::~CCommsdat318Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat318Step::CCommsdat318Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat318Step);                                                                                           
	}

TVerdict CCommsdat318Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat318Step::doTestStepL()
/**
 * @return - TVerdict code
 * Load an IAPPrioritySelPol field...
 */
	{
	SetTestStepResult(EFail);
	
	CMDBRecordLink<CCDIAPRecord>* iapLinkField = new(ELeave) CMDBRecordLink<CCDIAPRecord>(KCDTIdIap3);
	CleanupStack::PushL(iapLinkField);
	
	iapLinkField->SetRecordId(2);
	
	iapLinkField->LoadL(*iCDSession_v11);
	
	//there is no leave so it's ok
	
	SetTestStepResult(EPass);
	
	CleanupStack::PopAndDestroy(iapLinkField);
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat319Step::~CCommsdat319Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat319Step::CCommsdat319Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat319Step);                                                                                           
	}

TVerdict CCommsdat319Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat319Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete an IAPPrioritySelPol field...
 */
	{
	SetTestStepResult(EFail);
	
	CMDBRecordLink<CCDIAPRecord>* iapLinkField = new(ELeave) CMDBRecordLink<CCDIAPRecord>(KCDTIdIap3);
	CleanupStack::PushL(iapLinkField);
	
	iapLinkField->SetRecordId(2);
	
	iapLinkField->DeleteL(*iCDSession_v11);
	
	//there is no leave so it's ok
	
	SetTestStepResult(EPass);
	
	CleanupStack::PopAndDestroy(iapLinkField);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat320Step::~CCommsdat320Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat320Step::CCommsdat320Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat320Step);                                                                                           
	}

TVerdict CCommsdat320Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat320Step::doTestStepL()
/**
 * @return - TVerdict code
 * Changing the IAPCount in the IAPPrioritySelPol table and then read it back
 */
	{
	SetTestStepResult(EFail);
	
	CMDBField<TUint32>* iapCount = new(ELeave) CMDBField<TUint32>(KCDTIdIapCount);
	CleanupStack::PushL(iapCount);
	
	iapCount->SetRecordId(2);
	*iapCount = 2;
	
	iapCount->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iapCount);
	
	CMDBField<TUint32>* iapCount1 = new(ELeave) CMDBField<TUint32>(KCDTIdIapCount);
	CleanupStack::PushL(iapCount1);
	
	iapCount1->SetRecordId(2);
	iapCount1->LoadL(*iCDSession_v11);
	if (2 == *iapCount1)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(iapCount1);
	
	return TestStepResult();
	}

// AccessPoint mapping 

//----------------------------------------------------------------------------------------------------------------

CCommsdat321Step::~CCommsdat321Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat321Step::CCommsdat321Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat321Step);                                                                                           
	}

TVerdict CCommsdat321Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat321Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the accesspointSelectionPolicy field in the 2nd AccessPoint record 
 * to point to the 3rd IAPPrioritySelPol record. Modification is made through 
 * a record container.
 */
	{
	SetTestStepResult(EFail);
	
	CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(selPolRecord);
	
	selPolRecord->SetRecordId(3);
	selPolRecord->LoadL(*iCDSession_v11);
	
	CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec);
	
	apRec->SetRecordId(2);
	apRec->LoadL(*iCDSession_v11);
	
	apRec->iSelectionPolicy = selPolRecord->ElementId();
	
	apRec->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(apRec);
	CleanupStack::PopAndDestroy(selPolRecord);
	
	CCDAccessPointRecord* apRec1 = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec1);
		
	apRec1->SetRecordId(2);
	apRec1->LoadL(*iCDSession_v11);
	
	CMDBRecordLink<CCDSelectionPolicyRecordBase>* selPolField = &(apRec1->iSelectionPolicy);
	
	if ( 3 == ((*selPolField & KCDMaskShowRecordId) >> 8) )
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(apRec1);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat322Step::~CCommsdat322Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat322Step::CCommsdat322Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat322Step);                                                                                           
	}

TVerdict CCommsdat322Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat322Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the accesspointSelectionPolicy field in the 3rd AccessPoint record 
 * to point to the 4th IAPPrioritySelPol record. Modification is made through 
 * a record container but this time without filling the GID field.
 */
	{
	SetTestStepResult(EFail);
	
	CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(selPolRecord);
	
	selPolRecord->SetRecordId(4);
	selPolRecord->LoadL(*iCDSession_v11);
	
	CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec);
	
	apRec->SetRecordId(3);
	apRec->iSelectionPolicy = selPolRecord->ElementId();
	
	apRec->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(apRec);
	CleanupStack::PopAndDestroy(selPolRecord);
	
	
	CCDAccessPointRecord* apRec1 = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec1);
	
	apRec1->SetRecordId(3);
	apRec1->LoadL(*iCDSession_v11);
	
	CMDBRecordLink<CCDSelectionPolicyRecordBase>* selPolField = &(apRec1->iSelectionPolicy);
	
	if (4 == ((KCDMaskShowRecordId & (*selPolField)) >> 8) &&
		KCDTIdIapPrioritySelectionPolicyRecord == (KCDMaskShowRecordType & (*selPolField)) )
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(apRec1);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat323Step::~CCommsdat323Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat323Step::CCommsdat323Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat323Step);                                                                                           
	}

TVerdict CCommsdat323Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat323Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the accesspointSelectionPolicy field in the 3rd AccessPoint record 
 * to point to the 4th IAPPrioritySelPol record. Modification is made through 
 * a field container.
 */
	{
	SetTestStepResult(EFail);
	
	CCDIAPPrioritySelectionPolicyRecord* selPolRecord = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(selPolRecord);
	
	selPolRecord->SetRecordId(4);
	selPolRecord->LoadL(*iCDSession_v11);
	
	CMDBRecordLink<CCDSelectionPolicyRecordBase>* apSelPolField = new(ELeave)CMDBRecordLink<CCDSelectionPolicyRecordBase>(KCDTIdSelectionPolicy); 
	CleanupStack::PushL(apSelPolField);
	
	apSelPolField->SetRecordId(3);
	*apSelPolField = selPolRecord->ElementId();
	
	apSelPolField->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(apSelPolField);
	CleanupStack::PopAndDestroy(selPolRecord);
	
	
	CMDBRecordLink<CCDSelectionPolicyRecordBase>* apSelPolField1 = new(ELeave)CMDBRecordLink<CCDSelectionPolicyRecordBase>(KCDTIdSelectionPolicy); 
	CleanupStack::PushL(apSelPolField1);
	
	apSelPolField1->SetRecordId(3);
	apSelPolField1->LoadL(*iCDSession_v11);
	
	if (4 == ((KCDMaskShowRecordId & (*apSelPolField1)) >> 8) &&
		KCDTIdIapPrioritySelectionPolicyRecord == (KCDMaskShowRecordType & (*apSelPolField1)) )
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(apSelPolField1);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat324Step::~CCommsdat324Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat324Step::CCommsdat324Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat324Step);                                                                                           
	}

TVerdict CCommsdat324Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat324Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the GID field to the value of 2. Modification is made through 
 * a field container.
 */
	{
	SetTestStepResult(EFail);
	CMDBField<TInt>* gidField = new(ELeave)CMDBField<TInt>(KCDTIdAccessPointGID);
	CleanupStack::PushL(gidField);
	
	gidField->SetRecordId(1);
	*gidField = 2;
	
	TRAPD(err, gidField->ModifyL(*iCDSession_v11));
	if (KErrArgument == err)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(gidField);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat325Step::~CCommsdat325Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat325Step::CCommsdat325Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat325Step);                                                                                           
	}

TVerdict CCommsdat325Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat325Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the name field of the 1st accesspointRec. Modification is made through 
 * a field container. (this is field is not mapped)
 */
	{
	SetTestStepResult(EFail);
	_LIT(KName, "Changed AP name field");
	
	CMDBField<TDesC>* name = new(ELeave)CMDBField<TDesC>(KCDTIdAccessPointRecord | KCDTIdRecordName);
	CleanupStack::PushL(name);
	
	name->SetRecordId(1);
	name->SetMaxLengthL(KName().Length());
	name->SetL(KName);
		
	name->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(name);
	
	CMDBField<TDesC>* name1 = new(ELeave)CMDBField<TDesC>(KCDTIdAccessPointRecord | KCDTIdRecordName);
	CleanupStack::PushL(name1);
	
	name1->SetMaxLengthL(KName().Length());
	name1->SetL(KName);
		
	if ( name1->FindL(*iCDSession_v11) )
		{
		TPtrC name(*name1);
		if (0 == name.Compare(KName))
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(name1);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat326Step::~CCommsdat326Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat326Step::CCommsdat326Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat326Step);                                                                                           
	}

TVerdict CCommsdat326Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat326Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the GID field to the value of 2. Modification is made through 
 * a record container.
 */
	{
	SetTestStepResult(EFail);
	
	CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);
	
	apRecord->SetRecordId(1);
	apRecord->iAccessPointGID = 2;
	
	TRAPD(err, apRecord->ModifyL(*iCDSession_v11));
	
	if (KErrArgument == err)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(apRecord);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat327Step::~CCommsdat327Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat327Step::CCommsdat327Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat327Step);                                                                                           
	}

TVerdict CCommsdat327Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat327Step::doTestStepL()
/**
 * @return - TVerdict code
 * Modify the name field of the 1st accesspointRec. Modification is made through 
 * a record container. (this is field is not mapped)
 */
	{
	SetTestStepResult(EFail);
	
	CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);
	
	apRecord->SetRecordId(1);

	_LIT(KName, "Changed AP name field");
	apRecord->iRecordName.SetMaxLengthL(KName().Length());
	apRecord->iRecordName.SetL(KName);
		
	apRecord->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(apRecord);
	
	
	CCDAccessPointRecord* apRecord1 = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord1);
	
	apRecord1->iRecordName.SetMaxLengthL(KName().Length());
	apRecord1->iRecordName.SetL(KName);
		
	if (apRecord1->FindL(*iCDSession_v11))
		{
		TPtrC name(apRecord1->iRecordName);
		if (0 == name.Compare(KName))
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(apRecord);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat328Step::~CCommsdat328Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat328Step::CCommsdat328Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat328Step);                                                                                           
	}

TVerdict CCommsdat328Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat328Step::doTestStepL()
/**
 * @return - TVerdict code
 * Find the 1st AP record based on the linked IAPPrioritySelPol elementId. The find is made
 * through a record container.
 */
	{
	SetTestStepResult(EFail);
	
	CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRecord);
	
	apRecord->iSelectionPolicy = (KCDTIdIapPrioritySelectionPolicyRecord | (1 << 8));
		
	if (apRecord->FindL(*iCDSession_v11))
		{
		TPtrC recName(apRecord->iRecordName);
		_LIT(KRecName, "SNAP1");
		
		if (1 == apRecord->RecordId() &&
			0 == recName.Compare(KRecName))
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(apRecord);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat329Step::~CCommsdat329Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat329Step::CCommsdat329Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat329Step);                                                                                           
	}

TVerdict CCommsdat329Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat329Step::doTestStepL()
/**
 * @return - TVerdict code
 * Find the 1st AP record based on the linked IAPPrioritySelPol elementId. The find is made
 * through a field container.
 */
	{
	SetTestStepResult(EFail);
	CMDBRecordLink<CCDSelectionPolicyRecordBase>* selPolField = new(ELeave)CMDBRecordLink<CCDSelectionPolicyRecordBase>(KCDTIdSelectionPolicy);
	CleanupStack::PushL(selPolField);
	
	*selPolField = (KCDTIdIapPrioritySelectionPolicyRecord | (1 << 8));
	if (selPolField->FindL(*iCDSession_v11))
		{
		if (1 == selPolField->RecordId())
			{
			SetTestStepResult(EPass);
			}
		}
	
	CleanupStack::PopAndDestroy(selPolField);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat330Step::~CCommsdat330Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat330Step::CCommsdat330Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat330Step);                                                                                           
	}

TVerdict CCommsdat330Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat330Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete the selPol link in the 1st AP record. The delete is made through a field container.
 */
	{
	SetTestStepResult(EFail);
	CMDBRecordLink<CCDSelectionPolicyRecordBase>* selPolField = new(ELeave)CMDBRecordLink<CCDSelectionPolicyRecordBase>(KCDTIdSelectionPolicy);
	CleanupStack::PushL(selPolField);
	
	selPolField->SetRecordId(1);
	
	selPolField->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(selPolField);

	
	CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec);
	
	apRec->SetRecordId(1);
	apRec->LoadL(*iCDSession_v11);
	
	if (NULL == apRec->iSelectionPolicy)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(apRec);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat331Step::~CCommsdat331Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat331Step::CCommsdat331Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat331Step);                                                                                           
	}

TVerdict CCommsdat331Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
    iCDSession_vLatest = NULL;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat331Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete the 2nd AP record. The delete is made through a record container.
 */
	{
	SetTestStepResult(EFail);	
	CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec);
	
	apRec->SetRecordId(2);
	apRec->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(apRec);
	
	CCDAccessPointRecord* apRec1 = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apRec1);
	
	apRec1->SetRecordId(2);
	apRec1->LoadL(*iCDSession_v11);
	
	if ( NULL == apRec1->iAccessPointGID &&
	     NULL == apRec1->iSelectionPolicy )
		{
		//although LoadL didn't leave no data is read from the database.
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(apRec1);
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3311Step::~CCommsdat3311Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3311Step::CCommsdat3311Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3311Step);                                                                                           
	}

TVerdict CCommsdat3311Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3311Step::doTestStepL()
/**
 * @return - TVerdict code
 * Insert a new IAP link into the record one - the same as IAP1.
 * Insert is called on a field container.
 * 
 * No new IPProto AP should be generated and the new link should be inserted into
 * the record.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CMDBField<TInt>* iap2Link = new(ELeave)CMDBField<TInt>(KCDTIdIap2);
	CleanupStack::PushL(iap2Link);
	
	iap2Link->SetRecordId(1);
	*iap2Link = 1;
	iap2Link->ModifyL(*iCDSession_v11);
	
	//The IPProto AP number is 4 because we have 3 generated APs and
	//we have the default one as well in the DB...
	if ( 4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest) )
		{
		CCDIAPPrioritySelectionPolicyRecord* iapSelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
		CleanupStack::PushL(iapSelPolRec);
		
		iapSelPolRec->SetRecordId(1);
		iapSelPolRec->LoadL(*iCDSession_v11);
		
		TInt iap1 = iapSelPolRec->iIap1;
		TInt iap2 = iapSelPolRec->iIap2;
		
		if (iap1 == iap2 &&
			2 == iapSelPolRec->iIapCount)
			{
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(iapSelPolRec);
		}
	
	CleanupStack::PopAndDestroy(iap2Link);
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3312Step::~CCommsdat3312Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3312Step::CCommsdat3312Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3312Step);                                                                                           
	}

TVerdict CCommsdat3312Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3312Step::doTestStepL()
/**
 * @return - TVerdict code
 * Now deleted the previously inserted IAP link form the record one.
 * 
 * No IPProto AP should be deleted only the link
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CMDBField<TInt>* iap2Link = new(ELeave)CMDBField<TInt>(KCDTIdIap2);
	CleanupStack::PushL(iap2Link);
	
	iap2Link->SetRecordId(1);
	//*iap2Link = 1;
	iap2Link->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iap2Link);
	
	//The IPProto AP number is 4 because we have 3 generated APs and
	//we have the default one as well in the DB...
	if ( 4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest) )
		{
		CCDIAPPrioritySelectionPolicyRecord* iapSelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
		CleanupStack::PushL(iapSelPolRec);
		
		iapSelPolRec->SetRecordId(1);
		iapSelPolRec->LoadL(*iCDSession_v11);
		
		TInt iap1 = iapSelPolRec->iIap1;
		TInt iap2 = iapSelPolRec->iIap2;
		
		if (iap1 &&
		    NULL == iap2 &&
			1 == iapSelPolRec->iIapCount)
			{
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(iapSelPolRec);
		}
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3313Step::~CCommsdat3313Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3313Step::CCommsdat3313Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3313Step);                                                                                           
	}

TVerdict CCommsdat3313Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3313Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete the default IAPPrioritySelPol record. As a result the default IPProto
 * AP should be linked from the IPProto Tier record and from the defualt APPriotySelPol record.
 * The default APPrioritySelPol record should be linked form the Network default AP.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDIAPPrioritySelectionPolicyRecord* defIAPPrioritySelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(defIAPPrioritySelPolRec);
	//we just know form the config file that the default IAPPrioritySelPol rec is the 1st one
	
	defIAPPrioritySelPolRec->SetRecordId(1);
	defIAPPrioritySelPolRec->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(defIAPPrioritySelPolRec);
	
	CCDAccessPointRecord* ipprotoDefAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(ipprotoDefAPRec);
	
	ipprotoDefAPRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	ipprotoDefAPRec->FindL(*iCDSession_vLatest);
	
	CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
	CleanupStack::PushL(ipprotoTierRec);
	
	ipprotoTierRec->SetElementId(KIPProtoTierLink);
	ipprotoTierRec->LoadL(*iCDSession_vLatest);
	
	if (ipprotoTierRec->iDefaultAccessPoint == (ipprotoDefAPRec->ElementId() & ~KCDMaskShowFieldType))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(ipprotoTierRec);
	CleanupStack::PopAndDestroy(ipprotoDefAPRec);
	
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3314Step::~CCommsdat3314Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3314Step::CCommsdat3314Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3314Step);                                                                                           
	}

TVerdict CCommsdat3314Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3314Step::doTestStepL()
/**
 * @return - TVerdict code
 * Delete the 1st link from the default IAPPrioritySelPol record. As a result the default IPProto
 * AP should be linked from the IPProto Tier record and from the defualt APPriotySelPol record.
 * The default APPrioritySelPol record should be linked form the Network default AP.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CMDBField<TInt>* defIAPPrioritySelPolLink = new(ELeave)CMDBField<TInt>(KCDTIdIap1);
	CleanupStack::PushL(defIAPPrioritySelPolLink);
	//we just know form the config file that the default IAPPrioritySelPol rec is the 1st one
	
	defIAPPrioritySelPolLink->SetRecordId(1);
	defIAPPrioritySelPolLink->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(defIAPPrioritySelPolLink);
	
	CCDAccessPointRecord* ipprotoDefAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(ipprotoDefAPRec);
	
	ipprotoDefAPRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	ipprotoDefAPRec->FindL(*iCDSession_vLatest);
	
	CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
	CleanupStack::PushL(ipprotoTierRec);
	
	ipprotoTierRec->SetElementId(KIPProtoTierLink);
	ipprotoTierRec->LoadL(*iCDSession_vLatest);
	
	if (ipprotoTierRec->iDefaultAccessPoint == (ipprotoDefAPRec->ElementId() & ~KCDMaskShowFieldType))
		{
		CCDAPPrioritySelectionPolicyRecord* defAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(defAPPrioritySelPolRec);
		//we just know form the config file that the default IAPPrioritySelPol rec is the 1st one
		
		defAPPrioritySelPolRec->SetRecordId(1);
		defAPPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if (0 == defAPPrioritySelPolRec->iApCount)
			{
			SetTestStepResult(EPass);
			}
		CleanupStack::PopAndDestroy(defAPPrioritySelPolRec);
		}
	
	CleanupStack::PopAndDestroy(ipprotoTierRec);
	CleanupStack::PopAndDestroy(ipprotoDefAPRec);
	
	return TestStepResult();
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//----------------------------------------------------------------------------------------------------------------

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3315Step::~CCommsdat3315Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3315Step::CCommsdat3315Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3315Step);                                                                                           
	}

TVerdict CCommsdat3315Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3315Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step modifies the 1st IAPLink in the default IAPPrioritySelPol record
 * to 0.
 * 
 * Modify is called on field container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CMDBField<TInt>* defIAPPrioritySelPolLink = new(ELeave)CMDBField<TInt>(KCDTIdIap1);
	CleanupStack::PushL(defIAPPrioritySelPolLink);
	//we just know form the config file that the default IAPPrioritySelPol rec is the 1st one
	
	defIAPPrioritySelPolLink->SetRecordId(1);
	*defIAPPrioritySelPolLink = 0;
	defIAPPrioritySelPolLink->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(defIAPPrioritySelPolLink);
	
	CCDAccessPointRecord* ipprotoDefAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(ipprotoDefAPRec);
	
	ipprotoDefAPRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	ipprotoDefAPRec->FindL(*iCDSession_vLatest);
	
	CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
	CleanupStack::PushL(ipprotoTierRec);
	
	ipprotoTierRec->SetElementId(KIPProtoTierLink);
	ipprotoTierRec->LoadL(*iCDSession_vLatest);
	
	if (ipprotoTierRec->iDefaultAccessPoint == (ipprotoDefAPRec->ElementId() & ~KCDMaskShowFieldType))
		{
		CCDAPPrioritySelectionPolicyRecord* defAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(defAPPrioritySelPolRec);
		//we just know form the config file that the default IAPPrioritySelPol rec is the 1st one
		
		defAPPrioritySelPolRec->SetRecordId(1);
		defAPPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if (0 == defAPPrioritySelPolRec->iApCount)
			{
			SetTestStepResult(EPass);
			}
		CleanupStack::PopAndDestroy(defAPPrioritySelPolRec);
		}
	
	CleanupStack::PopAndDestroy(ipprotoTierRec);
	CleanupStack::PopAndDestroy(ipprotoDefAPRec);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------

CCommsdat3316Step::~CCommsdat3316Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3316Step::CCommsdat3316Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3316Step);                                                                                           
	}

TVerdict CCommsdat3316Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3316Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step modifies the 2nd IAPLink in the 2nd IAPPrioritySelPol record
 * (not the default one) to 0.
 * 
 * Modify is called on field container.
 * This teststep is depending on CCommsdat3315Step
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CMDBField<TInt>* defIAPPrioritySelPolLink = new(ELeave)CMDBField<TInt>(KCDTIdIap2);
	CleanupStack::PushL(defIAPPrioritySelPolLink);
	
	defIAPPrioritySelPolLink->SetRecordId(2);
	*defIAPPrioritySelPolLink = 0;
	defIAPPrioritySelPolLink->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(defIAPPrioritySelPolLink);
	
	if (4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		CCDAPPrioritySelectionPolicyRecord* aPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(aPPrioritySelPolRec);
		
		aPPrioritySelPolRec->SetRecordId(2);
		aPPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL != aPPrioritySelPolRec->iAp1 &&
		     NULL == aPPrioritySelPolRec->iAp2 &&
		     NULL != aPPrioritySelPolRec->iAp3 &&
		     3 == aPPrioritySelPolRec->iApCount )
			{
			SetTestStepResult(EPass);
			}
		CleanupStack::PopAndDestroy(aPPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3317Step::~CCommsdat3317Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3317Step::CCommsdat3317Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3317Step);                                                                                           
	}

TVerdict CCommsdat3317Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3317Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step modifies the 3rd IAPLink in the 3rd IAPPrioritySelPol record
 * (not the default one) to 0. Note that this is the last reference to the given
 * IPProto AP -> so as a result of the modification the given IPProto AP will be
 * deleted.
 * 
 * Modify is called on record container.
 * This teststep is depending on CCommsdat3316Step
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDIAPPrioritySelectionPolicyRecord* iAPPrioritySelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(iAPPrioritySelPolRec);
	
	iAPPrioritySelPolRec->SetRecordId(3);
	iAPPrioritySelPolRec->LoadL(*iCDSession_v11);
	
	iAPPrioritySelPolRec->iIap3 = 0;
	
	iAPPrioritySelPolRec->ModifyL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iAPPrioritySelPolRec);
	
	if (3 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		CCDAPPrioritySelectionPolicyRecord* aPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(aPPrioritySelPolRec);
		
		aPPrioritySelPolRec->SetRecordId(3);
		aPPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		if ( NULL != aPPrioritySelPolRec->iAp1 &&
		     NULL != aPPrioritySelPolRec->iAp2 &&
		     NULL == aPPrioritySelPolRec->iAp3 &&
		     2 == aPPrioritySelPolRec->iApCount )
			{
			SetTestStepResult(EPass);
			}
		CleanupStack::PopAndDestroy(aPPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3318Step::~CCommsdat3318Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3318Step::CCommsdat3318Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3318Step);                                                                                           
	}

TVerdict CCommsdat3318Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3318Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step delets the 1st IAP link form the 1st IAPPriotySelPol record
 * (the default one). At this point the AP link in the IPProto tier record points
 * to the default IPProto AP.
 * After this the 1st IAP link in the 1st IAPPrioritySelPol record is modified
 * again to point to a valid IAP.
 * 
 * The AP link in the IPProto tier has to be modified as well to point to the 
 * right IPProto AP. 
 * 
 * Modify is called on field container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIAPNumber = 3;
	TBool keepGoing = EFalse;
	
	CMDBField<TInt>* defIAPPrioritySelPolLink = new(ELeave)CMDBField<TInt>(KCDTIdIap1);
	CleanupStack::PushL(defIAPPrioritySelPolLink);
	
	defIAPPrioritySelPolLink->SetRecordId(1);
	defIAPPrioritySelPolLink->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(defIAPPrioritySelPolLink);
	
	CCDAccessPointRecord* defApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(defApRec);
	
	defApRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	
	if (defApRec->FindL(*iCDSession_vLatest))
		{
		CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
		CleanupStack::PushL(ipprotoTierRec);
		
		ipprotoTierRec->SetRecordId(2);
		ipprotoTierRec->LoadL(*iCDSession_vLatest);
		
		if (ipprotoTierRec->iDefaultAccessPoint == (defApRec->ElementId() & ~KCDMaskShowFieldType))
			{
			keepGoing = ETrue;
			}
		
		CleanupStack::PopAndDestroy(ipprotoTierRec);
		}
	CleanupStack::PopAndDestroy(defApRec);
	
	
	if (keepGoing)
		{
		//do the modification
		CMDBField<TInt>* defIAPPrioritySelPolLink1 = new(ELeave)CMDBField<TInt>(KCDTIdIap1);
		CleanupStack::PushL(defIAPPrioritySelPolLink1);
		
		defIAPPrioritySelPolLink1->SetRecordId(1);
		*defIAPPrioritySelPolLink1 = KIAPNumber;
		
		defIAPPrioritySelPolLink1->ModifyL(*iCDSession_v11);
		
		CleanupStack::PopAndDestroy(defIAPPrioritySelPolLink1);
		
		CCDAPPrioritySelectionPolicyRecord* defAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(defAPPrioritySelPolRec);
		
		//we just know that it's the 1st record...
		defAPPrioritySelPolRec->SetRecordId(1);
		defAPPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
		CleanupStack::PushL(ipprotoTierRec);
		
		ipprotoTierRec->SetRecordId(2);
		ipprotoTierRec->LoadL(*iCDSession_vLatest);
		
		TInt aPFromTheIPProtoTier = ipprotoTierRec->iDefaultAccessPoint;
		TInt defAPPriorioritySelPolLink = defAPPrioritySelPolRec->iAp1;
		
		if (aPFromTheIPProtoTier == defAPPriorioritySelPolLink)
			{
			//ok the 2 links are the same. Now let's load the IPProto AP and check
			//its' CustomSelectionPolicy. It has to be the same as the IAP number we
			//modified the IAPPriorioritySelPol field with
			CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(apRec);
			
			apRec->SetElementId(aPFromTheIPProtoTier);
			apRec->LoadL(*iCDSession_vLatest);
			
			if (KIAPNumber == apRec->iCustomSelectionPolicy)
				{
				SetTestStepResult(EPass);
				}
			
			CleanupStack::PopAndDestroy(apRec);
			}
		
		CleanupStack::PopAndDestroy(ipprotoTierRec);
		CleanupStack::PopAndDestroy(defAPPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//----------------------------------------------------------------------------------------------------------------

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CCommsdat3319Step::~CCommsdat3319Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat3319Step::CCommsdat3319Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat3319Step);                                                                                           
	}

TVerdict CCommsdat3319Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);

    iCDSession_v11 = CMDBSession::NewL(KCDVersion1_1);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_2);
#else
    iCDSession_vLatest = CMDBSession::NewL(KCDVersion1_1);
#endif

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCommsdat3319Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step delets the 1st IAP link form the 1st IAPPriotySelPol record
 * (the default one). At this point the AP link in the IPProto tier record points
 * to the default IPProto AP.
 * After this the 1st IAP link in the 1st IAPPrioritySelPol record is modified
 * again to point to a valid IAP.
 * 
 * The AP link in the IPProto tier has to be modified as well to point to the 
 * right IPProto AP. 
 * 
 * Modify is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIAPNumber = 3;
	TBool keepGoing = EFalse;
	
	CCDIAPPrioritySelectionPolicyRecord* iAPPrioritySelPolRec = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
	CleanupStack::PushL(iAPPrioritySelPolRec);
	
	iAPPrioritySelPolRec->SetRecordId(1);
	iAPPrioritySelPolRec->DeleteL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(iAPPrioritySelPolRec);
	
	CCDAccessPointRecord* defApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(defApRec);
	
	defApRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	
	if (defApRec->FindL(*iCDSession_vLatest))
		{
		CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
		CleanupStack::PushL(ipprotoTierRec);
		
		ipprotoTierRec->SetRecordId(2);
		ipprotoTierRec->LoadL(*iCDSession_vLatest);
		
		if (ipprotoTierRec->iDefaultAccessPoint == (defApRec->ElementId() & ~KCDMaskShowFieldType))
			{
			keepGoing = ETrue;
			}
		
		CleanupStack::PopAndDestroy(ipprotoTierRec);
		}
	CleanupStack::PopAndDestroy(defApRec);
	
	
	if (keepGoing)
		{
		//do the modification
		CCDIAPPrioritySelectionPolicyRecord* iAPPrioritySelPolRec1 = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
		CleanupStack::PushL(iAPPrioritySelPolRec1);
		
		iAPPrioritySelPolRec1->SetRecordId(1);
		iAPPrioritySelPolRec1->iIap1 = KIAPNumber;
		
		iAPPrioritySelPolRec1->ModifyL(*iCDSession_v11);
		
		CleanupStack::PopAndDestroy(iAPPrioritySelPolRec1);
		
		CCDAPPrioritySelectionPolicyRecord* defAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(defAPPrioritySelPolRec);
		
		//we just know that it's the 1st record...
		defAPPrioritySelPolRec->SetRecordId(1);
		defAPPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
		CleanupStack::PushL(ipprotoTierRec);
		
		ipprotoTierRec->SetRecordId(2);
		ipprotoTierRec->LoadL(*iCDSession_vLatest);
		
		TInt aPFromTheIPProtoTier = ipprotoTierRec->iDefaultAccessPoint;
		TInt defAPPriorioritySelPolLink = defAPPrioritySelPolRec->iAp1;
		
		if (aPFromTheIPProtoTier == defAPPriorioritySelPolLink)
			{
			//ok the 2 links are the same. Now let's load the IPProto AP and check
			//its' CustomSelectionPolicy. It has to be the same as the IAP number we
			//modified the IAPPriorioritySelPol field with
			CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
			CleanupStack::PushL(apRec);
			
			apRec->SetElementId(aPFromTheIPProtoTier);
			apRec->LoadL(*iCDSession_vLatest);
			
			if (KIAPNumber == apRec->iCustomSelectionPolicy)
				{
				SetTestStepResult(EPass);
				}
			
			CleanupStack::PopAndDestroy(apRec);
			}
		
		CleanupStack::PopAndDestroy(ipprotoTierRec);
		CleanupStack::PopAndDestroy(defAPPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
