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
#include "commsdatMixedSnapValidationTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
#include <commsdattypeinfov1_1_internal.h>

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

//----------------------------------------------------------------------------------------------------------------
CCommsdat400Step::~CCommsdat400Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat400Step::CCommsdat400Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat400Step);                                                                                           
	}

TVerdict CCommsdat400Step::doTestStepPreambleL()
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

TVerdict CCommsdat400Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 1
 * 	- no prompt
 *  - it points to the same IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 2;
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefDoNotPrompt;
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//no new IPProto AP should be generated (1 default IPProto AP, and the 3 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			ipprotoAPCustSelPol == ipprotoApRec->iCprConfig &&
			1 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP points to the IAP as well then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat401Step::~CCommsdat401Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat401Step::CCommsdat401Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat401Step);                                                                                           
	}

TVerdict CCommsdat401Step::doTestStepPreambleL()
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

TVerdict CCommsdat401Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 1
 * 	- prompt
 *  - it points to the same IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 2;
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt; //this is the change
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (5 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//one new IPProto AP should be generated [a promting AP] (1 default IPProto AP, and the 4 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			cpRecord1->ElementId() == ipprotoApRec->iCprConfig &&
			1 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat402Step::~CCommsdat402Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat402Step::CCommsdat402Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat402Step);                                                                                           
	}

TVerdict CCommsdat402Step::doTestStepPreambleL()
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

TVerdict CCommsdat402Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 1
 * 	- no prompt
 *  - it points to some other IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 1; //this is the change
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefDoNotPrompt;
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//no new IPProto AP should be generated (1 default IPProto AP, and the 3 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			ipprotoAPCustSelPol == ipprotoApRec->iCprConfig &&
			1 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat403Step::~CCommsdat403Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat403Step::CCommsdat403Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat403Step);                                                                                           
	}

TVerdict CCommsdat403Step::doTestStepPreambleL()
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

TVerdict CCommsdat403Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 1
 * 	- prompt
 *  - it points to some other IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 1; //this is the change
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt; //this is the change
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (5 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//one new IPProto AP should be generated [promting AP] (1 default IPProto AP, and the 4 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			cpRecord1->ElementId() == ipprotoApRec->iCprConfig &&
			1 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat404Step::~CCommsdat404Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat404Step::CCommsdat404Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat404Step);                                                                                           
	}

TVerdict CCommsdat404Step::doTestStepPreambleL()
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

TVerdict CCommsdat404Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 2
 * 	- no prompt
 *  - it points to the same IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 2;
	cpRecord->iRanking = 2;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefDoNotPrompt; 
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//no new IPProto AP should be generated (1 default IPProto AP, and the 3 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			ipprotoAPCustSelPol == ipprotoApRec->iCprConfig &&
			2 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat405Step::~CCommsdat405Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat405Step::CCommsdat405Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat405Step);                                                                                           
	}

TVerdict CCommsdat405Step::doTestStepPreambleL()
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

TVerdict CCommsdat405Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 2
 * 	- prompt
 *  - it points to the same IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 2;
	cpRecord->iRanking = 2;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt; 
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (5 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//one new IPProto AP should be generated [prompting AP] (1 default IPProto AP, and the 4 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp2);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			cpRecord1->ElementId() == ipprotoApRec->iCprConfig &&
			2 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat406Step::~CCommsdat406Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat406Step::CCommsdat406Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat406Step);                                                                                           
	}

TVerdict CCommsdat406Step::doTestStepPreambleL()
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

TVerdict CCommsdat406Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 2
 * 	- no prompt
 *  - it points to some other IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 1;
	cpRecord->iRanking = 2;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefDoNotPrompt; 
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//no new IPProto AP should be generated (1 default IPProto AP, and the 3 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp2);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			ipprotoAPCustSelPol == ipprotoApRec->iCprConfig &&
			2 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat407Step::~CCommsdat407Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat407Step::CCommsdat407Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat407Step);                                                                                           
	}

TVerdict CCommsdat407Step::doTestStepPreambleL()
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

TVerdict CCommsdat407Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 2
 * 	- prompt
 *  - it points to some other IAP as the currently default IAPPrioritySelPolRec (IAP1)
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 1;
	cpRecord->iRanking = 2;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt; 
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (5 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//one new IPProto AP should be generated [prompting AP] (1 default IPProto AP, and the 4 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp2);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP == ipprotoAPCustSelPol &&
			cpRecord1->ElementId() == ipprotoApRec->iCprConfig &&
			2 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat408Step::~CCommsdat408Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat408Step::CCommsdat408Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat408Step);                                                                                           
	}

TVerdict CCommsdat408Step::doTestStepPreambleL()
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

TVerdict CCommsdat408Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 1
 * 	- prompt
 *  - no default IAP
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 0;
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefPrompt; 
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (5 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//one new IPProto AP should be generated [prompting AP] (1 default IPProto AP, and the 4 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP != ipprotoAPCustSelPol &&
			0 == cpRecIAP &&
			cpRecord1->ElementId() == ipprotoApRec->iCprConfig &&
			1 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat409Step::~CCommsdat409Step()
/**
 * Destructor
 */
	{
	delete iCDSession_v11;
	delete iCDSession_vLatest;
	}

CCommsdat409Step::CCommsdat409Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat409Step);                                                                                           
	}

TVerdict CCommsdat409Step::doTestStepPreambleL()
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

TVerdict CCommsdat409Step::doTestStepL()
/**
 * @return - TVerdict code
 * This test step inserts a ConnPref record on top of the already existing
 * SNAP path. The inserted ConnPref record has:
 * 	- rank 1
 * 	- no prompt
 *  - no default IAP
 * 
 * StoreL is called on record container.
 */
	{
	SetTestStepResult(EFail);
	
	const TInt KIPProtoTierLink = 0x11800200;
	
	CCDConnectionPrefsRecord *cpRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	CleanupStack::PushL(cpRecord);
	
	cpRecord->SetRecordId(1);
	cpRecord->iDefaultIAP = 0;
	cpRecord->iRanking = 1;
	cpRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	cpRecord->iBearerSet = KCommDbBearerCSD;
	cpRecord->iDialogPref = ECommDbDialogPrefDoNotPrompt; 
	
	cpRecord->StoreL(*iCDSession_v11);
	
	CleanupStack::PopAndDestroy(cpRecord);
	
	if (4 == GetAPNumberL(KIPProtoTierLink, iCDSession_vLatest))
		{
		//ne new IPProto AP should be generated (1 default IPProto AP, and the 3 generated)
		CCDAPPrioritySelectionPolicyRecord* appPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		CleanupStack::PushL(appPrioritySelPolRec);
		appPrioritySelPolRec->SetRecordId(4); //we just know from the config file that it's the 4th rec
		appPrioritySelPolRec->LoadL(*iCDSession_vLatest);
		
		CCDConnectionPrefsRecord *cpRecord1 = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
		CleanupStack::PushL(cpRecord1);
		cpRecord1->SetRecordId(1);
		cpRecord1->LoadL(*iCDSession_vLatest);
		
		CCDAccessPointRecord* ipprotoApRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(ipprotoApRec);
		ipprotoApRec->SetElementId(appPrioritySelPolRec->iAp1);
		ipprotoApRec->LoadL(*iCDSession_vLatest);
		
		TInt cpRecIAP = cpRecord1->iDefaultIAP;
		TInt ipprotoAPCustSelPol = ipprotoApRec->iCustomSelectionPolicy;
		
		if (cpRecIAP != ipprotoAPCustSelPol &&
			0 == cpRecIAP &&
			ipprotoAPCustSelPol == ipprotoApRec->iCprConfig &&
			1 == appPrioritySelPolRec->iApCount)
			{
			//if the connpref rec and the generated IPProtoAP are pointing to the same IAP and
			//the cpr config field of the generated IPProtroAP contains the elementID of the 
			//connpref rec (it's a prompting AP) then it's ok
			SetTestStepResult(EPass);
			}
		
		CleanupStack::PopAndDestroy(ipprotoApRec);
		CleanupStack::PopAndDestroy(cpRecord1);
		CleanupStack::PopAndDestroy(appPrioritySelPolRec);
		}
	
	return TestStepResult();
	}
