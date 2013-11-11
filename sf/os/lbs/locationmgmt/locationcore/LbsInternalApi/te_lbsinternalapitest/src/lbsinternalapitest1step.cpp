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
 @file lbsinternalapiTest1Step.cpp
*/
#include "lbsinternalapitest1step.h"
#include "te_lbsinternalapiTestsuitedefs.h"

#include "LbsInternalInterface.h"
#include "LbsExtendModuleInfo.h"

ClbsinternalapiTest1Step::~ClbsinternalapiTest1Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest1Step::ClbsinternalapiTest1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest1Step);
	}

TVerdict ClbsinternalapiTest1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest1Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//Let's do some tests...
		
		TPositionModuleInfoExtended moduleInfo1;
		TPositionModuleInfoExtended moduleInfo2;
		
		const TUid KModuleUid1 = {0x0ABC0987};
		const TUid KModuleUid2 = {0x01230987};
		
		TPositionModuleInfo* posModInfo;
		posModInfo = 	(TPositionModuleInfo*)(&moduleInfo1.iPositionModuleInfoBytes[0]);
		posModInfo->SetModuleId(TPositionModuleId(KModuleUid1));
		moduleInfo1.iModuleId = KModuleUid1;
		
		posModInfo = 	(TPositionModuleInfo*)(&moduleInfo2.iPositionModuleInfoBytes[0]);
		posModInfo->SetModuleId(TPositionModuleId(KModuleUid2));
		moduleInfo2.iModuleId = KModuleUid2;
		
		const TPositionModuleInfoExtended* KModuleInfoArray[] =
			{
			&moduleInfo1,
			&moduleInfo2
			};
		
		LbsModuleInfo::InitializeL(KModuleInfoArray, 2);
		
		TUint numModules = 0;
		
		User::LeaveIfError(LbsModuleInfo::GetNumModules(numModules));
		
		if(numModules!=2)
			{
			User::Leave(KErrGeneral);
			}
		
		TPositionModuleInfo retrievingModuleInfo;
		
		//Check we can retrieve the two objects by index
		User::LeaveIfError(LbsModuleInfo::GetModuleInfoByIndex(0, &retrievingModuleInfo, sizeof(TPositionModuleInfo)));
		
		if(retrievingModuleInfo.ModuleId()!=KModuleUid1)
			User::Leave(KErrGeneral);
		
		User::LeaveIfError(LbsModuleInfo::GetModuleInfoByIndex(1, &retrievingModuleInfo, sizeof(TPositionModuleInfo)));
		
		if(retrievingModuleInfo.ModuleId()!=KModuleUid2)
			User::Leave(KErrGeneral);
		
		//Check we can retrieve the two objects by id
		User::LeaveIfError(LbsModuleInfo::GetModuleInfoById(KModuleUid1, &retrievingModuleInfo, sizeof(TPositionModuleInfo)));
		
		if(retrievingModuleInfo.ModuleId()!=KModuleUid1)
			User::Leave(KErrGeneral);
		
		User::LeaveIfError(LbsModuleInfo::GetModuleInfoById(KModuleUid2, &retrievingModuleInfo, sizeof(TPositionModuleInfo)));
		
		if(retrievingModuleInfo.ModuleId()!=KModuleUid2)
			User::Leave(KErrGeneral);
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict ClbsinternalapiTest1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest1Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
