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
// Example CTestStep derived implementation
// 
//

/**
 @file man9Step.cpp
*/
#include "man9Step.h"
#include "Te_manSuiteDefs.h"

Cman9Step::~Cman9Step()
/**
 * Destructor
 */
	{
	}

Cman9Step::Cman9Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman9Step);
	}

TVerdict Cman9Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman9Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman9Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{

		//  ************** Delete the Block, the block start ****************
		INFO_PRINTF1(_L("Please modify me. I am in Cman9Step::doTestStepL() in the file man9Step.cpp"));  //Block start
		TPtrC TheString;
		TBool TheBool;
		TInt TheInt;
		if(!GetStringFromConfig(ConfigSection(),KTe_manSuiteString, TheString) ||
			!GetBoolFromConfig(ConfigSection(),KTe_manSuiteBool,TheBool) ||
			!GetIntFromConfig(ConfigSection(),KTe_manSuiteInt,TheInt)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		else
			{
			INFO_PRINTF4(_L("The test step is %S, The Bool is %d, The int-value is %d"), &TheString, TheBool,TheInt); // Block end
			}

		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict Cman9Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Cman9Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
