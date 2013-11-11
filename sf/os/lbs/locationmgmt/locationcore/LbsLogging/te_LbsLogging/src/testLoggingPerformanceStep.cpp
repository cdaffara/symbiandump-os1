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
 @file TestLoggingPerformanceStep.cpp
*/
#include "testLoggingPerformanceStep.h"
#include "Te_LbsLoggingSuiteDefs.h"

// derived lbs logging event classes
#include <lbs/lbsselflocatelogevent.h>
#include <lbs/lbstransmitlocationlogevent.h>
#include <lbs/lbsnetworklocatelogevent.h>
#include <lbs/lbsexternallocatelogevent.h>
#include <lbs/lbsassistancedatalogevent.h>

CTestLoggingPerformanceStep::~CTestLoggingPerformanceStep()
/**
 * Destructor
 */
	{
	}

CTestLoggingPerformanceStep::CTestLoggingPerformanceStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestLoggingPerformanceStep);
	}

TVerdict CTestLoggingPerformanceStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLoggingSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTestLoggingPerformanceStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("CTestLoggingPerformanceStep::doTestStepL() in the file TestLoggingPerformanceStep.cpp"));  //Block start


		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestLoggingPerformanceStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	CTe_LbsLoggingSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
