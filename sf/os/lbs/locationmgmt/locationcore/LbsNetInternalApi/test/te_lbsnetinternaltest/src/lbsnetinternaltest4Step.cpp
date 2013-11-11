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
 @file lbsnetinternaltest4Step.cpp
*/
#include "lbsnetinternaltest4Step.h"
#include "Te_lbsnetinternaltestSuiteDefs.h"

Clbsnetinternaltest4Step::~Clbsnetinternaltest4Step()
/**
 * Destructor
 */
	{
	}

Clbsnetinternaltest4Step::Clbsnetinternaltest4Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetinternaltest4Step);
	}

TVerdict Clbsnetinternaltest4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetinternaltest4Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsnetinternaltestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetinternaltest4Step::doTestStepL()
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
		INFO_PRINTF1(_L("Please modify me. I am in Clbsnetinternaltest4Step::doTestStepL() in the file lbsnetinternaltest4Step.cpp"));  //Block start
		TPtrC TheString;
		TBool TheBool;
		TInt TheInt;
		if(!GetStringFromConfig(ConfigSection(),KTe_lbsnetinternaltestSuiteString, TheString) ||
			!GetBoolFromConfig(ConfigSection(),KTe_lbsnetinternaltestSuiteBool,TheBool) ||
			!GetIntFromConfig(ConfigSection(),KTe_lbsnetinternaltestSuiteInt,TheInt)
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



TVerdict Clbsnetinternaltest4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetinternaltest4Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsnetinternaltestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
