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
 @file GpsAcqAssistRootStep.cpp
*/
#include "GpsAcqAssistRootStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader


CGpsAcqAssistRootStep::~CGpsAcqAssistRootStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistRootStep::CGpsAcqAssistRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistRootStep);
	}

TVerdict CGpsAcqAssistRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistRootStep::doTestStepL()
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
	  	acquistionAssistanceRootBuilder.OpenL();
	  	acquistionAssistanceRootReader.OpenL();

        // Check that the properties are empty
		//
		if(acquistionAssistanceRootBuilder.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray)||
		   acquistionAssistanceRootBuilder.FieldExists(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime)||
		   acquistionAssistanceRootBuilder.FieldExists(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime)|| 
		   acquistionAssistanceRootBuilder.FieldExists(TUEPositioningGpsAcquisitionAssistance::ERrlpGpsReferenceTime))
			{
			ERR_PRINTF1(_L("Root Builder not empty"));
			SetTestStepResult(EFail);
			}
		if(acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray)||
		   acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime) ||
		   acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime)||
		   acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ERrlpGpsReferenceTime))
			{
			ERR_PRINTF1(_L("Root Reader not empty"));
			SetTestStepResult(EFail);			
			}
		}
	  return TestStepResult();
	}



TVerdict CGpsAcqAssistRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
