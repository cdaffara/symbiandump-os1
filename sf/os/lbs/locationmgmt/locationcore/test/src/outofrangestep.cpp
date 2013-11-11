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
//

/**
 @file outofrangestep.cpp
 @internalTechnology
*/
#include "outofrangestep.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>

COutOfRangeStep::~COutOfRangeStep()
/**
 * Destructor
 */
	{
	if(iLbsAdmin)
		{
		delete iLbsAdmin;
		}
	}

COutOfRangeStep::COutOfRangeStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KOutOfRangeStep);
	}

TVerdict COutOfRangeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(KOutOfRangePreambleL);
	//
	// Create an admin API 
	//
	iLbsAdmin = CLbsAdmin::NewL();
	iLbsAdmin->ResetToDefault();	//	ResetToDefault() is recommended to reinitialise the properties.
	return TestStepResult();
	}


TVerdict COutOfRangeStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Get function for the 
 * KLbsSpecialFeatureMaximumRequestDelay entry with an out of range delay value.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//  ***************** Test Description *******************
		INFO_PRINTF1(KOutOfRangeTestStepL);
		INFO_PRINTF1(KOutOfRangeDescription1);
		INFO_PRINTF1(KOutOfRangeDescription2);
		INFO_PRINTF1(KOutOfRangeDescription3);
		//  ************** End of the Description ****************
		
		TLbsTimeIntervalMiliSeconds time = 1;
		TInt err = iLbsAdmin->Get(KLbsSpecialFeatureMaximumRequestDelay, time);
		if(KErrArgument==err)
			{
			//	For this test, we expect time to be set for 0.
			//	If 0 is not returned, that's mean that we accepted out of range values.
			if(0==time)
				{
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(KOutOfRangeNotRespected);
				SetTestStepResult(EFail);
				}
			}
		else
			{
			INFO_PRINTF2(ErrorUnexpected, err);
			SetTestStepResult(EFail);
			}
		}
	  return TestStepResult();
	}



TVerdict COutOfRangeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(KOutOfRangePostambleL);
	return TestStepResult();
	}
