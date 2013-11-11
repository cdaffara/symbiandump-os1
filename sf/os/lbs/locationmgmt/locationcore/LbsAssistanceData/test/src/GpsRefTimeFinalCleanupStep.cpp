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
 @file GpsRefTimeFinalCleanupStep.cpp
*/
#include "GpsRefTimeFinalCleanupStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CGpsRefTimeFinalCleanupStep::~CGpsRefTimeFinalCleanupStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeFinalCleanupStep::CGpsRefTimeFinalCleanupStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeFinalCleanupStep);
	}

TVerdict CGpsRefTimeFinalCleanupStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
   	return TestStepResult();
	}


TVerdict CGpsRefTimeFinalCleanupStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	// Close the Navigation Model property to prevent 
	// TEF warnings due to memory leakage.
	iServer.iGpsReferenceTimeBuilder.Close();
	iServer.iGpsReferenceTimeReader.Close();
	return TestStepResult();
	}


TVerdict CGpsRefTimeFinalCleanupStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
