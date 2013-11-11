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
 @file GpsRefTimeRootStep.cpp
*/
#include "GpsRefTimeRootStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

CGpsRefTimeRootStep::~CGpsRefTimeRootStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeRootStep::CGpsRefTimeRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeRootStep);
	}

TVerdict CGpsRefTimeRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsRefTimeRootStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
	  	INFO_PRINTF1(_L("Open root property"));
	  	gpsReferenceTimeBuilder.OpenL();
      	gpsReferenceTimeReader.OpenL();
		}
	  return TestStepResult();
	}



TVerdict CGpsRefTimeRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
