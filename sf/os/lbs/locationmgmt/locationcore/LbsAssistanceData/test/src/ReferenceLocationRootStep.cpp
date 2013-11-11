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
 @file ReferenceLocationRootStep.cpp
*/
#include "ReferenceLocationRootStep.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iReferenceLocationBuilder iServer.iReferenceLocationBuilder
#define iReferenceLocationReader iServer.iReferenceLocationReader

CReferenceLocationRootStep::~CReferenceLocationRootStep()
/**
 * Destructor
 */
	{
	}

CReferenceLocationRootStep::CReferenceLocationRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KReferenceLocationRootStep);
	}

TVerdict CReferenceLocationRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CReferenceLocationRootStep::doTestStepL()
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
	  	iReferenceLocationBuilder.OpenL();
	  	iReferenceLocationReader.OpenL();

        // Check that the properties are empty
		//
		if(iReferenceLocationBuilder.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
			{
			ERR_PRINTF1(_L("Root Builder not empty"));
			SetTestStepResult(EFail);
			}
		if(iReferenceLocationReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
			{
			ERR_PRINTF1(_L("Root Reader not empty"));
			SetTestStepResult(EFail);			
			}
		}
	  return TestStepResult();
	}



TVerdict CReferenceLocationRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
