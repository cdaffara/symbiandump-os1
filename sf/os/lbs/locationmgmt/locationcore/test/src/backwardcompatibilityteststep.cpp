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
//

/**
 @file backwardcompatibilityteststep.cpp
 @internalTechnology
*/
#include "backwardcompatibilityteststep.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>

//	Used to initially create all the properties
//	This source code architecture is a copy of lbsadminimpl.cpp.
const TLbsAdminSetting KKeysArray[]=
	{
	KLbsSettingHomeSelfLocate,
	KLbsSettingLogger,
	KLbsSpecialFeatureMaximumRequestDelay,
	KLbsSpecialFeatureAllowTBFinalNetPos,
	};
const TInt KKeysArrayCount = sizeof(KKeysArray)/sizeof(TLbsAdminSetting);

CBackwardCompatibilityTestStep::~CBackwardCompatibilityTestStep()
/**
 * Destructor
 */
	{
	if(iLbsAdmin)
		{
		delete iLbsAdmin;
		}
	}

CBackwardCompatibilityTestStep::CBackwardCompatibilityTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KBackwardCompatibilityTestStep);
	}

TVerdict CBackwardCompatibilityTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(KBackwardCompatibilityPreambleL);
	return TestStepResult();
	}


TVerdict CBackwardCompatibilityTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test read the boolean resetmandatory from the te_LbsAdminsuite.ini file.
 * If resetall is set to False, we will call the CLbsAdmin::NewL(), then CLbsAdmin::Get() methods.
 * If resetall is set to True, we will call the CLbsAdmin::ResetToDefault() between the NewL and the Get() methods.
 */
	{
	  if (EPass==TestStepResult())
			{
			//  ******************* Test Description *********************
			INFO_PRINTF1(KBackwardCompatibilitydoTestStepL);
			INFO_PRINTF1(KBackwardCompatibilitydoTestStepLDescription);
			//  **************** End of the Description ******************
			
			//	This boolean read from te_LbsAdminSuite.ini indicate if we ne to call ResetToDefault()
			//	between NewL() and Get()
			TBool needResetFromDefault = EFalse;
			GetBoolFromConfig(ConfigSection(),KTe_BoolNeedResetFromDefault,needResetFromDefault);
			
			//	Teststep done in TestWithReset
			TBool stepResult = DoTestL(needResetFromDefault);
			
			if(stepResult)
				{
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(KErrorBackwardCompatibilityNotRespected);
				SetTestStepResult(EFail);
				}
			}
		  return TestStepResult();
	}

TBool CBackwardCompatibilityTestStep::DoTestL(TBool aNeedResetToDefault)
	{
	//	Initialised variables used to check that the return value Get() is correct
	CLbsAdmin::TLogger logger = CLbsAdmin::ELoggerUnknown;							//	for KLbsSettingLogger
	TLbsTimeIntervalMiliSeconds time = 1;											//	for KLbsSpecialFeatureMaximumRequestDelay
	CLbsAdmin::TSpecialFeature specialFeature = CLbsAdmin::ESpecialFeatureUnknown;	//	for KLbsSpecialFeatureAllowTBFinalNetPos
	CLbsAdmin::TSelfLocateService defaultTest = CLbsAdmin::ESelfLocateUnknown;		//	for the default mechanism
	
	//	this boolean store our test result
	TBool stepResult = ETrue;
	
	//	******* Testing NewL, then ResetToDefault, then Set *******
	iLbsAdmin = CLbsAdmin::NewL();
	if(aNeedResetToDefault)
		{
		User::LeaveIfError(iLbsAdmin->ResetToDefault());
		}
	
	TInt err = KErrNone;
	for (TInt i = 0; i < KKeysArrayCount; ++i)
		{
		
		switch (KKeysArray[i])
			{
			case KLbsSettingLogger:
				{
				INFO_PRINTF1(KBackwardCompatibilityKLbsSettingLogger);
				err = iLbsAdmin->Get(KLbsSettingLogger, logger);
				if(CLbsAdmin::ELoggerOn!=logger)
					{
					stepResult = EFalse;
					INFO_PRINTF2(KBackwardCompatibilityWrongLoggerSettings, logger);
					}
				break;
				}
			case KLbsSpecialFeatureMaximumRequestDelay:
				{
				INFO_PRINTF1(KBackwardCompatibilityKLbsSpecialFeatureMaxRequDelay);
				err = iLbsAdmin->Get(KLbsSpecialFeatureMaximumRequestDelay, time);
				if(0!=time)
					{
					stepResult = EFalse;
					INFO_PRINTF2(KBackwardCompatibilityWrongSettings, time);
					}
				break;
				}
			case KLbsSpecialFeatureAllowTBFinalNetPos:
				{
				INFO_PRINTF1(KBackwardCompatibilityKLbsSpecialFeatureAllowTBFinalNetPos);
				err = iLbsAdmin->Get(KLbsSpecialFeatureAllowTBFinalNetPos, specialFeature);
				if(CLbsAdmin::ESpecialFeatureOff!=specialFeature)
					{
					stepResult = EFalse;
					INFO_PRINTF2(KBackwardCompatibilitySpecialFeatureOff, specialFeature);
					}
				break;
				}
			case KLbsSettingHomeSelfLocate:
				{
				// The following code allows to test an existing entry in the central repository
				INFO_PRINTF1(KBackwardCompatibilityDefaultMechanism);
				err = iLbsAdmin->Get(KLbsSettingHomeSelfLocate, defaultTest);
				break;
				}
			default:
				{
				break;
				}
			}
		if(KErrNone!=err)
			{
			stepResult = EFalse;
			INFO_PRINTF2(ErrorUnexpected, err);
			}
		}
	
	return stepResult;
	}

TVerdict CBackwardCompatibilityTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(KBackwardCompatibilityPostambleL);
	return TestStepResult();
	}
