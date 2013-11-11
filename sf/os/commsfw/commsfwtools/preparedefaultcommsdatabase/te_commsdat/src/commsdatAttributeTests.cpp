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
// CCommsdatInitAttributes
// CCommsdat130Step
// CCommsdat131Step
// CCommsdat132Step
// CCommsdat133Step
// CCommsdat134Step
// CCommsdat135Step
// CCommsdat136Step
// CCommsdat137Step
// CCommsdat138Step
// CCommsdat139Step
// CCommsdat140Step
// CCommsdat141Step
// CCommsdat142Step
// 
//

/**
 @file commsdatAttributeTests.cpp
*/
#include "commsdatAttributeTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
 
const TInt KNetworkWeighting = 125;
//----------------------------------------------------------------------------------------------------------------
CCommsdatInitAttributes::CCommsdatInitAttributes()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdatInitAttributes);                                                                                           
	}

TVerdict CCommsdatInitAttributes::doTestStepL()
/**
 * @return - TVerdict code
 * This test step actually does not test anything at all.As there is no way to set attributes
 * with CED for the time being; this step creates the necessary records for the test steps in 
 * this section. THIS TEST STEP SHOULD BE RUN BEFORE RUNNING ANY TEST STEPS IN THIS SECTION
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("THIS TEST STEP SHOULD BE RUN BEFORE RUNNING ANY TEST STEPS IN THIS SECTION"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	 
	CleanupStack::PushL(cmdbSession); 
	
	// Enable ECDNoWriteButDelete on session
	cmdbSession->SetAttributeMask(ECDNoWriteButDelete);

	//
	// Necessary for test step 130
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);

	record->SetRecordId(1);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDNoWriteButDelete);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
    //
	// Necessary for test step 131
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(2);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDNoWriteButDelete);
	record->StoreL(*cmdbSession);
    
    CleanupStack::PopAndDestroy(record);
    
    //
	// Necessary for test step 132
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(3);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDNoWriteButDelete);
	record->StoreL(*cmdbSession);
    
    CleanupStack::PopAndDestroy(record);
    
    //
	// Necessary for test step 133
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(4);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDNoWriteButDelete);
	record->StoreL(*cmdbSession);
    
    CleanupStack::PopAndDestroy(record);
    
    // Disable ECDNoWriteButDelete on session
	cmdbSession->ClearAttributeMask(ECDNoWriteButDelete);
	
	// Enable ECDHidden on session
	cmdbSession->SetAttributeMask(ECDHidden);

    //
	// Necessary for test step 134
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(5);
	record->iNetworkWeighting = KNetworkWeighting+1;
	record->SetAttributes(ECDHidden);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	//
	// Necessary for test step 135
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(6);
	record->iNetworkWeighting = KNetworkWeighting+2;
	record->SetAttributes(ECDHidden);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	//
	// Necessary for test step 136
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(7);
	record->iNetworkWeighting = KNetworkWeighting+3;
	record->SetAttributes(ECDHidden);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	//
	// Necessary for test step 137
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(8);
	record->iNetworkWeighting = KNetworkWeighting+4;
	record->SetAttributes(ECDHidden);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	// Disable ECDHidden on session	
	cmdbSession->ClearAttributeMask(ECDHidden);
	
	// Enable ECDPrivate on session
	cmdbSession->SetAttributeMask(ECDPrivate);
	
	//
	// Necessary for test step 138
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(9);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDPrivate);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	//
	// Necessary for test step 139
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(10);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDPrivate);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	// Disable ECDPrivate on session
	cmdbSession->ClearAttributeMask(ECDPrivate);

	// Enable ECDProtectedWrite on session
	cmdbSession->SetAttributeMask(ECDProtectedWrite);
	//
	// Necessary for test step 140
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(11);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDProtectedWrite);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);
	
	//
	// Necessary for test step 141
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(12);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDProtectedWrite);
	record->StoreL(*cmdbSession);
	
	CleanupStack::PopAndDestroy(record);

	//
	// Necessary for test step 142
    record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(13);
	record->iNetworkWeighting = KNetworkWeighting;
	record->SetAttributes(ECDProtectedWrite);
	record->StoreL(*cmdbSession);

	// Disable ECDProtectedWrite on session
	cmdbSession->ClearAttributeMask(ECDProtectedWrite);
	
	CleanupStack::PopAndDestroy(record);
	
    CleanupStack::PopAndDestroy(cmdbSession);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat130Step::CCommsdat130Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat130Step);                                                                                           
	}


TVerdict CCommsdat130Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDNoWriteButDelete with ModifyL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    
	INFO_PRINTF1(_L("Testing ECDNoWriteButDelete functionality in CCommsdat130Step::doTestStepL() "));
	 
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(1); // See CCommsdatInitAttributes::doTestStepL()
	record->LoadL(*cmdbSession);
	TInt oldNw = record->iNetworkWeighting;
	record->iNetworkWeighting = 25;
	TRAPD(modifyErr, record->ModifyL(*cmdbSession)); //Must return KErrAccessDenied
	record->LoadL(*cmdbSession);
	
	if(modifyErr==KErrAccessDenied && oldNw == record->iNetworkWeighting)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Modify returned : %d should be -21"), modifyErr);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat131Step::CCommsdat131Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat131Step);                                                                                           
	}

TVerdict CCommsdat131Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDNoWriteButDelete with ModifyL on single field
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDHidden functionality in CCommsdat131Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(2); // See CCommsdatInitAttributes::doTestStepL()
	record->LoadL(*cmdbSession);
	TInt oldNw = record->iNetworkWeighting;
	record->iNetworkWeighting = 25;
	
	TRAPD(modifyErr, record->iNetworkWeighting.ModifyL(*cmdbSession)); //Must return KErrAccessDenied
	record->LoadL(*cmdbSession);
	
	if(modifyErr==KErrAccessDenied && oldNw == record->iNetworkWeighting)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Modify returned : %d should be -21"), modifyErr);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat132Step::CCommsdat132Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat132Step);                                                                                           
	}

TVerdict CCommsdat132Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDNoWriteButDelete with DeleteL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDNoWriteButDelete functionality in CCommsdat132Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(3); // See CCommsdatInitAttributes::doTestStepL()
	record->LoadL(*cmdbSession);
	TRAPD(deleteErr, record->DeleteL(*cmdbSession));
	
	if(deleteErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat133Step::CCommsdat133Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat133Step);                                                                                           
	}
TVerdict CCommsdat133Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDNoWriteButDelete with DeleteL on a single field
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDNoWriteButDelete functionality in CCommsdat133Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(4); // See CCommsdatInitAttributes::doTestStepL()
	record->LoadL(*cmdbSession);
	TRAPD(deleteErr, record->iNetworkWeighting.DeleteL(*cmdbSession));
	
	if(deleteErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat134Step::CCommsdat134Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat134Step);                                                                                           
	}

TVerdict CCommsdat134Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDHidden with FindL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDHidden functionality in CCommsdat134Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->iNetworkWeighting = KNetworkWeighting+1;
		
	if(!record->FindL(*cmdbSession))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat135Step::CCommsdat135Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat135Step);                                                                                           
	}

TVerdict CCommsdat135Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDHidden with FindL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDHidden functionality in CCommsdat135Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);

	record->iNetworkWeighting = KNetworkWeighting+2;
	cmdbSession->SetAttributeMask(ECDHidden);
	
	if(record->FindL(*cmdbSession))
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat136Step::CCommsdat136Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat136Step);                                                                                           
	}

TVerdict CCommsdat136Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDHidden with LoadL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDHidden functionality in CCommsdat136Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(7); // See CCommsdatInitAttributes::doTestStepL()
	cmdbSession->SetAttributeMask(ECDHidden);
	TRAPD(loadErr, record->LoadL(*cmdbSession));
	
	if(loadErr == KErrNone )
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat137Step::CCommsdat137Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat137Step);                                                                                           
	}

TVerdict CCommsdat137Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDHidden with ModifyL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDHidden functionality in CCommsdat137Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(8); // See CCommsdatInitAttributes::doTestStepL()
	cmdbSession->SetAttributeMask(ECDHidden);
	TRAPD(loadErr, record->LoadL(*cmdbSession));
	record->iNetworkWeighting = KNetworkWeighting + 100; // just a new iNetworkWeighting value
	TRAPD(modifyErr, record->ModifyL(*cmdbSession));
	
	if(loadErr == KErrNone && modifyErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
    
   	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat138Step::CCommsdat138Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat138Step);                                                                                           
	}

TVerdict CCommsdat138Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDPrivate with LoadL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDPrivate functionality in CCommsdat138Step::doTestStepL() "));
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(9); // See CCommsdatInitAttributes::doTestStepL()
	cmdbSession->SetAttributeMask(ECDPrivate);
	TRAPD(loadErr, record->LoadL(*cmdbSession));
	
	if(loadErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);

	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat139Step::CCommsdat139Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat139Step);                                                                                           
	}

TVerdict CCommsdat139Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDPrivate with LoadL
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing ECDPrivate functionality in CCommsdat139Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(10); // See CCommsdatInitAttributes::doTestStepL()
	cmdbSession->SetAttributeMask(ECDPrivate);
	TRAPD(loadErr, record->LoadL(*cmdbSession));
	
	if(loadErr == KErrPermissionDenied)
		{
		// checks if DeleteL will return correct error value
		TRAP(loadErr, record->DeleteL(*cmdbSession));

		if(loadErr == KErrPermissionDenied)
			{
			SetTestStepResult(EPass);
			}
		}

	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat140Step::CCommsdat140Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat140Step);                                                                                           
	}

TVerdict CCommsdat140Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDProtectedWrite with LoadL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
    	
	INFO_PRINTF1(_L("Testing ECDProtectedWrite functionality in CCommsdat140Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(11); // See CCommsdatInitAttributes::doTestStepL()
	cmdbSession->SetAttributeMask(ECDProtectedWrite);
	TRAPD(loadErr, record->LoadL(*cmdbSession));
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);	
    
	if(loadErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat141Step::CCommsdat141Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat141Step);                                                                                           
	}

TVerdict CCommsdat141Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDProtectedWrite with LoadL
 */
	{
    SetTestStepResult(EFail);
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));	
	INFO_PRINTF1(_L("Testing ECDProtectedWrite functionality in CCommsdat141Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(12); // See CCommsdatInitAttributes::doTestStepL()
	record->iNetworkWeighting = KNetworkWeighting;
	cmdbSession->SetAttributeMask(ECDProtectedWrite);
	record->LoadL(*cmdbSession);
	record->iNetworkWeighting = KNetworkWeighting + 100; // just a new iNetworkWeighting value
	TRAPD(modErr, record->ModifyL(*cmdbSession));
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	if(modErr == KErrPermissionDenied)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat142Step::CCommsdat142Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat142Step);                                                                                           
	}

TVerdict CCommsdat142Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test effects of ECDProtectedWrite with ModifyL
 */
	{
    SetTestStepResult(EFail);	
    INFO_PRINTF1(_L("========================================================================================"));
    INFO_PRINTF1(_L("WARNING:CCommsdatInitAttributes::doTestStepL() MUST be run successfully before this test"));
    INFO_PRINTF1(_L("========================================================================================"));
	INFO_PRINTF1(_L("Testing ECDProtectedWrite functionality in CCommsdat142Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
 	
	CleanupStack::PushL(cmdbSession); 
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(13); // See CCommsdatInitAttributes::doTestStepL()
	record->iNetworkWeighting = KNetworkWeighting;
	cmdbSession->SetAttributeMask(ECDProtectedWrite);
	record->LoadL(*cmdbSession);
	record->iNetworkWeighting = KNetworkWeighting + 100; // just a new iNetworkWeighting value
	TRAPD(modErr, record->ModifyL(*cmdbSession));
	
	CleanupStack::PopAndDestroy(record);
    CleanupStack::PopAndDestroy(cmdbSession);
	
	if(modErr == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
