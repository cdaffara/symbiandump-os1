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
 @file GpsIonosphericRootStep.cpp
*/
#include "GpsIonosphericRootStep.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsIonModelReader iServer.iGpsIonModelReader
#define iGpsIonModelBuilder iServer.iGpsIonModelBuilder

CGpsIonosphericRootStep::~CGpsIonosphericRootStep()
/**
 * Destructor
 */
	{
	}

CGpsIonosphericRootStep::CGpsIonosphericRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsIonosphericRootStep);
	}

TVerdict CGpsIonosphericRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsIonosphericRootStep::doTestStepL()
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
	  	iGpsIonModelReader.OpenL();
	  	iGpsIonModelBuilder.OpenL();

        // Check that the properties are empty

		if (iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa0)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa1)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa2)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa3)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EBeta0)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EBeta1)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EBeta2)||
			iGpsIonModelReader.FieldExists(TUEPositioningGpsIonosphericModel::EBeta3))
			{
			ERR_PRINTF1(_L("Root builder not empty"));
			SetTestStepResult(EFail);
			}
	
		if (iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa0)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa1)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa2)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EAlfa3)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EBeta0)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EBeta1)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EBeta2)||
			iGpsIonModelBuilder.FieldExists(TUEPositioningGpsIonosphericModel::EBeta3))
			{
			ERR_PRINTF1(_L("Root reader not empty"));
			SetTestStepResult(EFail);
			}
		}
	  return TestStepResult();
	}



TVerdict CGpsIonosphericRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
