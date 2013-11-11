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
 @file commsdatCorruptionTests.cpp
*/
 
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "Te_commsdatSuiteStepBase.h"
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>
#include "CommsDatCorruptionTests.h"

using namespace CommsDat;
using namespace Meta;

typedef TBuf<1000> TLargeText;


_LIT(KBigText, "bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla ..........");
 
CCommsdat300Step::~CCommsdat300Step()
/**
 * Destructor
 */
	{
	}

CCommsdat300Step::CCommsdat300Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat300Step);                                                                                           
	}
 

TVerdict CCommsdat300Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TUint32 value to TDesC field.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TUint32 value to TDesC field."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDDialOutISPRecord *DialOutRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(DialOutRecord);

	DialOutRecord->SetRecordId(KCDNewRecordRequest);
    DialOutRecord->StoreL(*cmdbSession);
	TInt recordId = DialOutRecord->RecordId();

	// create field looking at IdDescription, IdDescription should be of type CMDBField<TDesC> **TMDBText**
	CMDBField<TUint32>* IdDescriptionField = new(ELeave) CMDBField<TUint32>(KCDTIdDescription);
	CleanupStack::PushL(IdDescriptionField);

	// deliberately set wrong type of field
	TUint32 bigInt = 3300;
	IdDescriptionField->SetL(bigInt);
	IdDescriptionField->SetRecordId(recordId);
	TRAPD(	
			err,
			IdDescriptionField->StoreL(*cmdbSession)
		 );

	// Sanity Check expect this to crash if DB has Stored the mal formed field
	CCDDialOutISPRecord *sanityRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(sanityRecord);

	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession); 

	CleanupStack::PopAndDestroy(sanityRecord);		
	CleanupStack::PopAndDestroy(IdDescriptionField);
	CleanupStack::PopAndDestroy(DialOutRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err != KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}
	
	
CCommsdat301Step::~CCommsdat301Step()
/**
 * Destructor
 */
	{
	}

CCommsdat301Step::CCommsdat301Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat301Step);                                                                                           
	}
	
TVerdict CCommsdat301Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TDesC value to TBool field.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TDesC value to TBool field."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDDialOutISPRecord *DialOutRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(DialOutRecord);

	DialOutRecord->SetRecordId(KCDNewRecordRequest);
    DialOutRecord->StoreL(*cmdbSession);
	TInt recordId = DialOutRecord->RecordId();

	// create field looking at DialResolutionField, DialResolutionField should be of type CMDBField<TBool>, TMDBNum 
	CMDBField<TDesC>* DialResolutionField = new(ELeave) CMDBField<TDesC>(KCDTIdDialResolution);
	CleanupStack::PushL(DialResolutionField);
	// deliberately set wrong type of field
	DialResolutionField->SetL(KBigText);
	DialResolutionField->SetRecordId(recordId);
	TRAPD(	
			err,
			DialResolutionField->StoreL(*cmdbSession)
		 );

	// Sanity Check expect this to Leave if DB has Stored the mal formed field
	CCDDialOutISPRecord *sanityRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(sanityRecord);

	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession);	
	
	CleanupStack::PopAndDestroy(sanityRecord);		
	
	CleanupStack::PopAndDestroy(DialResolutionField);
	CleanupStack::PopAndDestroy(DialOutRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err != KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}



CCommsdat302Step::~CCommsdat302Step()
/**
 * Destructor
 */
	{
	}

CCommsdat302Step::CCommsdat302Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat302Step);                                                                                           
	}


TVerdict CCommsdat302Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TDesC value to TUint32 field.
 */
	{
     SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TDesC value to TUint32 field."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDDialOutISPRecord *DialOutRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(DialOutRecord);

	DialOutRecord->SetRecordId(KCDNewRecordRequest);
    DialOutRecord->StoreL(*cmdbSession);
	TInt recordId = DialOutRecord->RecordId();

	// create field looking at IdAckTimer, IdAckTimer should be of type CMDBField<TUint32>, TMDBNum
	CMDBField<TDesC>* IdAckTimer = new(ELeave) CMDBField<TDesC>(KCDTIdAckTimer);
	CleanupStack::PushL(IdAckTimer);
	IdAckTimer->SetL(KBigText);
	IdAckTimer->SetRecordId(recordId);
	TRAPD(	
			err,
			IdAckTimer->StoreL(*cmdbSession)
		 );

	// Sanity Check, expect this to crash if DB has Stored the mal formed field
	CCDDialOutISPRecord *sanityRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(sanityRecord);

	// create record No.1 with 
	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession);	
	
	CleanupStack::PopAndDestroy(sanityRecord);		
	CleanupStack::PopAndDestroy(IdAckTimer);
	CleanupStack::PopAndDestroy(DialOutRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err != KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}
	

CCommsdat303Step::~CCommsdat303Step()
/**
 * Destructor
 */
	{
	}

CCommsdat303Step::CCommsdat303Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat303Step);                                                                                           
	}


 TVerdict CCommsdat303Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TUint32 value to TDesC field.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TUint32 value to TDesC field."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDDialOutISPRecord *DialOutRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(DialOutRecord);

	DialOutRecord->SetRecordId(KCDNewRecordRequest);
    DialOutRecord->StoreL(*cmdbSession);
	TInt recordId = DialOutRecord->RecordId();

	// create field looking at IdLoginScript, IdLoginScript should be of type CMDBField<TDesC>, TMDBLongText
	CMDBField<TUint32>* IdLoginField = new(ELeave) CMDBField<TUint32>(KCDTIdLoginScript);
	CleanupStack::PushL(IdLoginField);

	TUint32 bigInt = 3300;
	IdLoginField->SetL(bigInt);
	IdLoginField->SetRecordId(recordId);
	TRAPD(	
			err,
			IdLoginField->StoreL(*cmdbSession)
		 );

	// Sanity Check expect this to crash if DB has Stored the mal formed field
	CCDDialOutISPRecord *sanityRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(sanityRecord);

	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession);	
	
	CleanupStack::PopAndDestroy(sanityRecord);		
	CleanupStack::PopAndDestroy(IdLoginField);
	CleanupStack::PopAndDestroy(DialOutRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err != KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}
	

CCommsdat304Step::~CCommsdat304Step()
/**
 * Destructor
 */
	{
	}

CCommsdat304Step::CCommsdat304Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat304Step);                                                                                           
	}
	

 TVerdict CCommsdat304Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TInt value to TDesC8 field.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TInt value to TDesC8 field."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDDialOutISPRecord *DialOutRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(DialOutRecord);

	DialOutRecord->SetRecordId(KCDNewRecordRequest);
    DialOutRecord->StoreL(*cmdbSession);
	TInt recordId = DialOutRecord->RecordId();

	// create field looking at IdInitString, IdInitString should be of type CMDBField<TDesC8>, *** TMDBBin **
	CMDBField<TInt>* IdInitStringField = new(ELeave) CMDBField<TInt>(KCDTIdInitString);
	CleanupStack::PushL(IdInitStringField);

	TInt bigInt = 3200;
	IdInitStringField->SetL(bigInt);
	IdInitStringField->SetRecordId(recordId);
	TRAPD(	
			err,
			IdInitStringField->StoreL(*cmdbSession)
		 );

	// Sanity Check expect this to crash if DB has Stored the mal formed field
	CCDDialOutISPRecord *sanityRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(sanityRecord);

	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession);	
	
	CleanupStack::PopAndDestroy(sanityRecord);		
	CleanupStack::PopAndDestroy(IdInitStringField);
	CleanupStack::PopAndDestroy(DialOutRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err != KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}
	
 
 
 
CCommsdat305Step::~CCommsdat305Step()
/**
 * Destructor
 */
	{
	}

CCommsdat305Step::CCommsdat305Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat305Step);                                                                                           
	}


 TVerdict CCommsdat305Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TInt value to TDesC field.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TInt value to TDesC field."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDOutgoingGprsRecord *WCDMAPacketServiceRecord = static_cast<CCDOutgoingGprsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWCDMAPacketServiceRecord));
	CleanupStack::PushL(WCDMAPacketServiceRecord);

	WCDMAPacketServiceRecord->SetRecordId(KCDNewRecordRequest);
    WCDMAPacketServiceRecord->StoreL(*cmdbSession);
	TInt recordId = WCDMAPacketServiceRecord->RecordId();

	TMDBElementId APNElementID  = KCDTIdWCDMAPacketServiceRecord | KCDTIdAPN;

	// create field looking at IdDescription, IdDescription should be of type CMDBField<TDesC>, TMDBMedText
	CMDBField<TInt>* IdAPNField = new(ELeave) CMDBField<TInt>(APNElementID);
	CleanupStack::PushL(IdAPNField);

	TInt bigInt = 3200;
	IdAPNField->SetL(bigInt);
	IdAPNField->SetRecordId(recordId);
	TRAPD(	
			err,
			IdAPNField->StoreL(*cmdbSession)
		 );

	// Sanity Check expect this to crash if DB has Stored the mal formed field
	
	CCDOutgoingGprsRecord *sanityRecord = static_cast<CCDOutgoingGprsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWCDMAPacketServiceRecord));
	CleanupStack::PushL(sanityRecord);

	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession);	
	
	CleanupStack::PopAndDestroy(sanityRecord);		
	CleanupStack::PopAndDestroy(IdAPNField);
	CleanupStack::PopAndDestroy(WCDMAPacketServiceRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err != KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}
	
	
CCommsdat306Step::~CCommsdat306Step()
/**
 * Destructor
 */
	{
	}

CCommsdat306Step::CCommsdat306Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat306Step);                                                                                           
	}
 

 TVerdict CCommsdat306Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check for assignement of TDesC value to TDesC field. Passing test.
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Check for assignement of TDesC value to TDesC field. Passing test."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);  

	CCDDialOutISPRecord *DialOutRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(DialOutRecord);

	DialOutRecord->SetRecordId(KCDNewRecordRequest);
    DialOutRecord->StoreL(*cmdbSession);
	TInt recordId = DialOutRecord->RecordId();

	// create field looking at IdDescription, IdDescription should be of type CMDBField<TDesC> **TMDBText**
	CMDBField<TDesC>* IdDescriptionField = new(ELeave) CMDBField<TDesC>(KCDTIdDescription);
	CleanupStack::PushL(IdDescriptionField);
	_LIT( KDescription, "correct data type");
	IdDescriptionField->SetL(KDescription);
	IdDescriptionField->SetRecordId(recordId);
	TRAPD(	
			err,
			IdDescriptionField->StoreL(*cmdbSession)
		 );

	// Sanity Check expect should create and load record without error
	CCDDialOutISPRecord *sanityRecord = static_cast<CCDDialOutISPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord));
	CleanupStack::PushL(sanityRecord);

	sanityRecord->SetRecordId(recordId);
	sanityRecord->LoadL(*cmdbSession); 

	CleanupStack::PopAndDestroy(sanityRecord);		
	CleanupStack::PopAndDestroy(IdDescriptionField);
	CleanupStack::PopAndDestroy(DialOutRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	if(err == KErrNone)
		{
		SetTestStepResult(EPass);			
		}

	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
