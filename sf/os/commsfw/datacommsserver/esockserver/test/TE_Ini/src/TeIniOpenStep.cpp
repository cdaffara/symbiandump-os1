// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
*/

#include "TeIniOpenStep.h"

//EPOC includes
#include <test/testexecutelog.h>
#include <es_sock.h>
#include <es_ini.h>

_LIT(KFileName,				"c:\\te_ini.ini");


CTestIniOpenStep::CTestIniOpenStep() : iESockIniData(NULL)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KOpenIniFile);
	}

CTestIniOpenStep::~CTestIniOpenStep()
/**
 * Destructor
 */
	{
	delete iESockIniData;
	iESockIniData = NULL;
	}


TVerdict CTestIniOpenStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini iESockIniData section
 */
	{

	INFO_PRINTF1(_L("In Test Step OpenIniFile"));

	iESockIniData = CESockIniData::NewL(KFileName);

	TPtrC result;
	TPtrC* res = &result;

	TInt ret1 = 0;

	// test [SectionOne] VarOne
	TRAP(ret1, iESockIniData->FindVar(_L("VarOne"),result));
	TInt ret2 = result.Compare(_L("FromSectionOne"));
	if (ret2 != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to compare VarOne. Error: %d"), ret2);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("VarOne [RAW] = %S\n\r"), res);
		}

	// test [SectionOne] VarOne
	TRAP(ret1, iESockIniData->FindVar(_L("SectionOne"),_L("VarOne"),result));
	ret2 = result.Compare(_L("FromSectionOne"));
	if (ret2 != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionOne] VarOne. Error: %d"), ret2);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionOne] VarOne = %S\n\r"), res);
		}

	// test [SectionOne] VarTwo, which is not existing
	TBool ret3 = iESockIniData->FindVar(_L("SectionOne"),_L("VarTwo"),result);
	if (ret3 != EFalse)
		{
		ERR_PRINTF1(_L("Failed to compare [SectionOne] VarTwo."));
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("[SectionOne] VarOne NotFound\n\r"));
		}

	// test [SectionTwo] VarOne
	TRAP(ret1, iESockIniData->FindVar(_L("SectionTwo"),_L("VarOne"),result));
	ret2 = result.Compare(_L("FromSectionTwo"));
	if (ret2 != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionTwo] VarOne. Error: %d"), ret2);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionTwo] VarOne = %S\n\r"), res);
		}

	// test [SectionThree] VarOne
	TRAP(ret1, iESockIniData->FindVar(_L("SectionThree"),_L("VarOne"),result));
	ret2 = result.Compare(_L("FromSectionThree"));
	if (ret2 != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionThree] VarOne. Error: %d"), ret2);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionThree] VarOne = %S\n\r"), res);
		}

	// test [SectionTwo][SectionThree]
	TRAP(ret1, iESockIniData->FindVar(_L("SectionTwo"),_L("SectionThree"),result));
	ret2 = result.Compare(_L("VarNameSectionThree"));
	if (ret2 != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionTwo] SectionThree. Error: %d"), ret2);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionTwo] SectionThree = %S\n\r"),res);
		}


	// test [SectionOne] sectionNum
	TInt numResult = 0;
	TRAP(ret1, iESockIniData->FindVar(_L("SectionOne"),_L("sectionNum"),numResult));
	if (numResult != 1)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionOne] sectionNum. numResult is: %d"), numResult);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionOne] sectionNum = %d\n\r"),numResult);
		}

	// test [SectionTwo] sectionNum
	TRAP(ret1, iESockIniData->FindVar(_L("SectionTwo"),_L("sectionNum"),numResult));
	if (numResult != 2)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionTwo] sectionNum. numResult is: %d"), numResult);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionTwo] sectionNum = %d\n\r"),numResult);
		}

	// test [SectionThree] sectionNum
	TRAP(ret1, iESockIniData->FindVar(_L("SectionThree"),_L("sectionNum"),numResult));
	if (numResult != 3)
		{
		ERR_PRINTF2(_L("Failed to compare [SectionThree] sectionNum. numResult is: %d"), numResult);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("[SectionThree] sectionNum = %d\n\r"),numResult);
		}

	// test [SectionOne] test1, which is not existing
	ret3 = iESockIniData->FindVar(_L("SectionOne"),_L("test1"),result);
	if (ret3 != EFalse)
		{
		ERR_PRINTF1(_L("Failed to compare [SectionOne] test1."));
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("[SectionOne] test1 NotFound\n\r"));
		}

	// test [SectionOne] test2, which is not existing
	ret3 = iESockIniData->FindVar(_L("SectionOne"),_L("test2"),result);
	if (ret3 != EFalse)
		{
		ERR_PRINTF1(_L("Failed to compare [SectionOne] test2."));
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("[SectionOne] test2 NotFound\n\r"));
		}
	
	return TestStepResult();
	}



