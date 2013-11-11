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
//

/**
 @file NavigationModelRootStep.cpp
*/
#include "NavigationModelRootStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CNavigationModelRootStep::~CNavigationModelRootStep()
/**
 * Destructor
 */
	{
	}

CNavigationModelRootStep::CNavigationModelRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavigationModelRootStep);
	}

TVerdict CNavigationModelRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual. Nothing done here.
 */
	{
	iServer.iArrayIndex = 0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CNavigationModelRootStep::doTestStepL()
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
	  	iServer.iNavigationModelBuilder.OpenL();
      	iServer.iNavigationModelReader.OpenL();
	  
	  	//Check that both builder and reader are empty (leave it open for following tests)
	  	//
	  	TBool exists = EFalse;
	  	exists = iServer.iNavigationModelBuilder.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
	  	
	  	if (exists)
	  		{
	  		ERR_PRINTF1(_L("ENavigationModelSatelliteInfoArray exits in newly created Builder"));
	  		return EFail;
	  		}
  	  	
  	  	exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);  	  

	  	if (exists)
	  		{
	  		ERR_PRINTF1(_L("ENavigationModelSatelliteInfoArray exits in newly created Reader"));
	  		return EFail;
	  		}
		}
		
	  return TestStepResult();
	}


TVerdict CNavigationModelRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
