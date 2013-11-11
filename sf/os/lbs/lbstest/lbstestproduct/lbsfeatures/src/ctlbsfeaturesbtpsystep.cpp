// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsfeaturesprivacystep.cpp
// 
//

#include <e32std.h>
#include "ctlbsfeaturesbtpsystep.h"

/**
Static Constructor
*/
CT_LbsFeaturesBTPSYStep* CT_LbsFeaturesBTPSYStep::New()
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsFeaturesBTPSYStep* testStep = new CT_LbsFeaturesBTPSYStep();
	if (testStep)
		{
		TInt err = KErrNone;
		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}


/**
 * Constructor
 */
CT_LbsFeaturesBTPSYStep::CT_LbsFeaturesBTPSYStep() : CT_LbsFeaturesStep()
	{
	SetTestStepName(KLbsFeaturesBTPSY);
	}


void CT_LbsFeaturesBTPSYStep::ConstructL()
	{
	// Create the base class objects.
	CT_LbsFeaturesStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsFeaturesBTPSYStep::~CT_LbsFeaturesBTPSYStep()
	{
	}

TVerdict CT_LbsFeaturesBTPSYStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsFeaturesBTPSYStep::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);
	
	TESTL(CT_LbsFeaturesStep::EFeaturePresent == IsBTPSYFeaturePresent());

	return TestStepResult();
	}
