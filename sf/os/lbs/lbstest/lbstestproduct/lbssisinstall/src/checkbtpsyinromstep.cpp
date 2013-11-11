// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file checkbtpsyinromstep.cpp
 @internalTechnology
*/

#include "checkbtpsyinromstep.h"

CCheckBTPSYinRomStep::~CCheckBTPSYinRomStep()
/**
 * Destructor
 */
	{
	}

CCheckBTPSYinRomStep::CCheckBTPSYinRomStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KCheckBTPSYinRomStep);
	}

TVerdict CCheckBTPSYinRomStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsSisInstallStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CCheckBTPSYinRomStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		SetTestStepResult(EPass);
		CheckBTPSYinRomL();
		}
	  return TestStepResult();
	}

void CCheckBTPSYinRomStep::CheckBTPSYinRomL()
	{
	TBool btpsySupported(EFalse); // the default is that the component is not present
#ifdef SYMBIAN_FEATURE_MANAGER
	btpsySupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KBluetoothGPSPositioningPlugin);
#endif
	
	if(btpsySupported)
		{
		INFO_PRINTF1(_L("The BTPSY feature is present"));
		}
	else
		{
		INFO_PRINTF1(_L("The BTPSY feature is absent"));
		}
	}

TVerdict CCheckBTPSYinRomStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

// End of file

