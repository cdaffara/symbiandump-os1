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
 @file unsupportedsetcalstep.cpp
 @internalTechnology
*/
#include "unsupportedsetcalstep.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>

CUnsupportedSetCalStep::~CUnsupportedSetCalStep()
/**
 * Destructor
 */
	{
	if(iLbsAdmin)
		{
		delete iLbsAdmin;
		}
	}

CUnsupportedSetCalStep::CUnsupportedSetCalStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KUnsupportedSetCalStep);
	}

TVerdict CUnsupportedSetCalStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(KUnsupportedSetCalStepPreambleL);
	//
	// Create an admin API 
	//
	iLbsAdmin = CLbsAdmin::NewL();
	return TestStepResult();
	}


TVerdict CUnsupportedSetCalStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Set function for the KLbsSpecialFeatureMaximumRequestDelay entry.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//  ***************** Test Description *******************
		INFO_PRINTF1(KUnsupportedSetCalStepTestStepL);
		INFO_PRINTF1(KUnsupportedSetCalStepDescription);
		//  ************** End of the Description ****************
		
		TLbsTimeIntervalMiliSeconds time = 1;
		TInt err = iLbsAdmin->Set(KLbsSpecialFeatureMaximumRequestDelay, time);
		if(KErrArgument==err)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(ErrorUnexpected, err);
			SetTestStepResult(EFail);
			}
		}
	  return TestStepResult();
	}


TVerdict CUnsupportedSetCalStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(KUnsupportedSetCalStepPostambleL);
	return TestStepResult();
	}
