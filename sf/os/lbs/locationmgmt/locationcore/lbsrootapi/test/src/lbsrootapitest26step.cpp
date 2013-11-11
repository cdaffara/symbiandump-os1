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
 @file lbsrootapiTest26Step.cpp
*/
#include "lbscommon.h" 
#include "LbsInternalInterface.h" 
#include "lbsrootapitest26step.h"
#include "te_lbsrootapiTestSuiteDefs.h"

#include "lbsrootapi.h"



CLbsRootApiTest26Step::~CLbsRootApiTest26Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest26Step::CLbsRootApiTest26Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest26Step);
	}

TVerdict CLbsRootApiTest26Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest26Step"));

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest26Step::DoMainL()
{
}

const TReal32 KExpectedHorizontalAccuracy = 23.2;
const TReal32 KExpectedVerticalAccuracy = 87.9;
const TTimeIntervalMicroSeconds KExpectedTimeToNextFix = 5000000;
const TInt8 KExpectedMajorVersion = 9;
const TInt8 KExpectedMinorVersion = 8;
const TInt8 KExpectedBuildVersion = 7;

TVerdict CLbsRootApiTest26Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt retVal;
	if (TestStepResult()==EPass)
		{

		INFO_PRINTF1(_L("CLbsRootApiTest26Step : open RLbsSystemController"));

		RLbsSystemController lbsRootProcessApi;
		lbsRootProcessApi.OpenL(KLbsTestProcessUid);
		retVal = lbsRootProcessApi.RequestCompleteCloseDown();
		INFO_PRINTF2(_L("CLbsRootApiTest26Step : Request CompleteCloseDown complete (%d) "),retVal);
		
		retVal = lbsRootProcessApi.RequestSystemStartup();
		INFO_PRINTF2(_L("CLbsRootApiTest26Step : Request SystemStartup complete (%d) "),retVal);
		if(retVal != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}

		// Check the module info was read in correctly.

		TPositionModuleInfo modInfo;
		TPositionQuality quality;
		TVersion version;
		TUid moduleId ={98765432};

		TInt size = modInfo.PositionClassSize();
		TInt err = LbsModuleInfo::GetModuleInfoById(moduleId, &modInfo,size);
		if(err)
			{
			INFO_PRINTF2(_L("CLbsRootApiTest26Step : Failed to get module info (%d) "),err);
			SetTestStepResult(EFail);
			}
		modInfo.GetPositionQuality(quality);
		version = modInfo.Version();

		if(quality.HorizontalAccuracy() != KExpectedHorizontalAccuracy)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest26Step : Unexpected horizontal accuracy returned "));
			SetTestStepResult(EFail);
			}
		if(quality.VerticalAccuracy() != KExpectedVerticalAccuracy)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest26Step : Unexpected vertical accuracy returned "));
			SetTestStepResult(EFail);
			}
			TTimeIntervalMicroSeconds tim = quality.TimeToNextFix();
		if(quality.TimeToNextFix() != KExpectedTimeToNextFix)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest26Step : Unexpected time to next fix returned "));
			SetTestStepResult(EFail);
			}
		if(version.iMajor != KExpectedMajorVersion || 
			version.iMinor != KExpectedMinorVersion ||
			version.iBuild != KExpectedBuildVersion)
			{
			INFO_PRINTF1(_L("CLbsRootApiTest26Step : Unexpected version returned "));
			SetTestStepResult(EFail);
			}
			
		INFO_PRINTF1(_L("CLbsRootApiTest26Step : close system down"));
		lbsRootProcessApi.RequestSystemCloseDown();

		}		
		
	return TestStepResult();
	}

TVerdict CLbsRootApiTest26Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest26Step"));
	// process something post setting to the test step
	// uncomment the following line if you have post process or remove the following line if no post process
	return TestStepResult();
	}
