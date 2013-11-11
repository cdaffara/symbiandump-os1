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
 @file CTestLtsySmsControlReceiveSmsCase1Step.cpp
 @internalTechnology
*/
#include "testltsysmscontrolreceivesmscase1step.h"
#include "te_integration_stltsysuitedefs.h"

CCTestLtsySmsControlReceiveSmsCase1Step::~CCTestLtsySmsControlReceiveSmsCase1Step()
/**
 * Destructor
 */
	{
	}

CCTestLtsySmsControlReceiveSmsCase1Step::CCTestLtsySmsControlReceiveSmsCase1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCTestLtsySmsControlReceiveSmsCase1Step);
	}

TVerdict CCTestLtsySmsControlReceiveSmsCase1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CCTestLtsySmsControlReceiveSmsCase1Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_integration_stltsySuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CCTestLtsySmsControlReceiveSmsCase1Step::doTestStepL()
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
		INFO_PRINTF1(_L("Please modify me. I am in CCTestLtsySmsControlReceiveSmsCase1Step::doTestStepL() in the file CTestLtsySmsControlReceiveSmsCase1Step.cpp"));  //Block start
		TPtrC TheString;
		TBool TheBool;
		TInt TheInt;
		if(!GetStringFromConfig(ConfigSection(),KTe_integration_stltsySuiteString, TheString) ||
			!GetBoolFromConfig(ConfigSection(),KTe_integration_stltsySuiteBool,TheBool) ||
			!GetIntFromConfig(ConfigSection(),KTe_integration_stltsySuiteInt,TheInt)
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



TVerdict CCTestLtsySmsControlReceiveSmsCase1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CCTestLtsySmsControlReceiveSmsCase1Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_integration_stltsySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
