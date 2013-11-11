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
// CCommsdat115Step
// CCommsdat116Step
// CCommsdat117Step
// CCommsdat118Step
// 
//

/**
 @file commsdatUtilityFunctionTests.cpp
*/
 
#include "commsdatUtilityFunctionTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
 
const TInt KLocationRecordId = 41;
const TInt KChargecardRecordId = 0;
//----------------------------------------------------------------------------------------------------------------
CCommsdat115Step::CCommsdat115Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat115Step);                                                                                           
	}


TVerdict CCommsdat115Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve mobile phone number for displaying
 * CED -i Z:\testdata\configs\te_commsdat_location_table2.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing resolving phone number in CCommsdat115Step::doTestStepL() "));
	
	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);
	
	_LIT(KNumber, "6294841");
	TPtrC number(KNumber);
	TBuf<50> dialString;
	
	utils->ResolvePhoneNumberL(number, dialString, EForDisplay, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);
	
	// resolve of a mobile number gives you back the original number - see CCommsDatUtilImpl::ResolvePhoneNumberFromDatabaseL()
	if(dialString == KNumber)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat116Step::CCommsdat116Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat116Step);                                                                                           
	}

TVerdict CCommsdat116Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve mobile phone number for dialing
 * CED -i Z:\testdata\configs\te_commsdat_location_table2.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing resolving phone number in CCommsdat116Step::doTestStepL() "));
	
	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);
	
	_LIT(KNumber, "6294841");
	TPtrC number(KNumber);
	TBuf<50> dialString;

	utils->ResolvePhoneNumberL(number, dialString, EForDialing, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);
	
	// resolve of a mobile number gives you back the original number - see CCommsDatUtilImpl::ResolvePhoneNumberFromDatabaseL()
	if(dialString == KNumber)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat117Step::CCommsdat117Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat117Step);                                                                                           
	}

TVerdict CCommsdat117Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve non-mobile phone number for displaying
 * CED -i Z:\testdata\configs\te_commsdat_location_table3.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing resolving phone number in CCommsdat117Step::doTestStepL() "));
	
	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);
	
	TPtrC number(_L("6294841"));
	TBuf<50> dialString;

	utils->ResolvePhoneNumberL(number, dialString, EForDisplay, KLocationRecordId, KChargecardRecordId);
	INFO_PRINTF2(_L("result: %S"), &dialString);
	
	// 8 is disable call waiting, 9 is local call hence no international code or dialling code
	_LIT(KExpectedNumber, "89,6294841");
	
	if(dialString == KExpectedNumber)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(utils);
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat118Step::CCommsdat118Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat118Step);                                                                                           
	}

TVerdict CCommsdat118Step::doTestStepL()
/**
 * @return - TVerdict code
 * Resolve non-mobile phone number for dialing
 * CED -i Z:\testdata\configs\te_commsdat_location_table3.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing resolving phone number in CCommsdat118Step::doTestStepL() "));
	
	CCommsDatUtils *utils = CCommsDatUtils::NewL();
	CleanupStack::PushL(utils);
	
	TPtrC number(_L("6294841"));
	TBuf<50> dialString;
	
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


