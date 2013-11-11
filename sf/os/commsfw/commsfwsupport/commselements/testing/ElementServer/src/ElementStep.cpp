// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ElementStep.h"


CElementStep1::~CElementStep1()
/**
 * Destructor
 */
	{
	}

CElementStep1::CElementStep1()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KElementStep1);
	}

TVerdict CElementStep1::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Preamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CElementStep1::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{

	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("doTestStepL"));
	return TestStepResult();
	}

TVerdict CElementStep1::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


CElementStep2::~CElementStep2()
/**
 * Destructor
 */
	{
	}

CElementStep2::CElementStep2(CElementServer& aParent) : iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KElementStep2);
	}

TVerdict CElementStep2::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Preamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CElementStep2::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	INFO_PRINTF1(_L("doTestStepL"));
	SetTestStepResult(EPass);

	return TestStepResult();
	}

TVerdict CElementStep2::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}
//

CElementStep3::~CElementStep3()
/**
 * Destructor
 */
	{
	}

CElementStep3::CElementStep3(CElementServer& aParent) : iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KElementStep3);
	}

TVerdict CElementStep3::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Preamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CElementStep3::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	INFO_PRINTF1(_L("doTestStepL"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CElementStep3::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}
