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
 @file GpsUtcModelRootStep.cpp
*/
#include "GpsUtcModelRootStep.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsUtcModelBuilder iServer.iGpsUtcModelBuilder
#define iGpsUtcModelReader iServer.iGpsUtcModelReader

CGpsUtcModelRootStep::~CGpsUtcModelRootStep()
/**
 * Destructor
 */
	{
	}

CGpsUtcModelRootStep::CGpsUtcModelRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsUtcModelRootStep);
	}

TVerdict CGpsUtcModelRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iServer.iArrayIndex = 0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsUtcModelRootStep::doTestStepL()
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
	  	iGpsUtcModelBuilder.OpenL();
	  	iGpsUtcModelReader.OpenL();

        // Check that the properties are empty

		if (iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EA1)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EA0)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::ETot)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EWnt)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EDeltatLS)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EWnlsf)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EDn)||
			iGpsUtcModelBuilder.FieldExists(TUEPositioningGpsUtcModel::EDeltatLSF))
			{
			ERR_PRINTF1(_L("Root builder not empty"));
			SetTestStepResult(EFail);
			}
	
		if (iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EA1)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EA0)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::ETot)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EWnt)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EDeltatLS)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EWnlsf)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EDn)||
			iGpsUtcModelReader.FieldExists(TUEPositioningGpsUtcModel::EDeltatLSF))
			{
			ERR_PRINTF1(_L("Root reader not empty"));
			SetTestStepResult(EFail);
			}
		}
	  return TestStepResult();
	}



TVerdict CGpsUtcModelRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
