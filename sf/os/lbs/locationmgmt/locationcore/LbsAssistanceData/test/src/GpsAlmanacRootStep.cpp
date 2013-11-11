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
 @file GpsAlmanacRootStep.cpp
*/
#include "GpsAlmanacRootStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsAlmanacBuilder iServer.iGpsAlmanacBuilder
#define iAlmanacSatInfoArrayBuilder iServer.iAlmanacSatInfoArrayBuilder
#define iAlmanacSatInfoBuilder iServer.iAlmanacSatInfoBuilder
#define iGpsAlmanacReader iServer.iGpsAlmanacReader
#define iAlmanacSatInfoArrayReader iServer.iAlmanacSatInfoArrayReader
#define iAlmanacSatInfoReader iSever.iAlmanacSatInfoReader

CGpsAlmanacRootStep::~CGpsAlmanacRootStep()
/**
 * Destructor
 */
	{
	}

CGpsAlmanacRootStep::CGpsAlmanacRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAlmanacRootStep);
	}

TVerdict CGpsAlmanacRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iServer.iArrayIndex = 0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAlmanacRootStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
			  	//Check opening builder twice is OK.
	  	INFO_PRINTF1(_L("Open root property"));
	  	iGpsAlmanacBuilder.OpenL();
      	iGpsAlmanacReader.OpenL();
			  	//Check that both builder and reader are empty (leave it open for following tests)
	  	//
	  	TBool exists = EFalse;
	  	exists = ( iGpsAlmanacBuilder.FieldExists(TUEPositioningGpsAlmanac::EWnA) ||
	  			   iGpsAlmanacBuilder.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray)||
	  			   iGpsAlmanacBuilder.FieldExists(TUEPositioningGpsAlmanac::ESvGlobalHealth));	   
	  			   
	  	if (exists)
	  		{
	  		ERR_PRINTF1(_L("Field exits in newly created Builder"));
	  		return EFail;
	  		}
  	  	
	  	exists = ( iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EWnA) ||
	  			   iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray)||
	  			   iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::ESvGlobalHealth));
	  	if (exists)
	  		{
	  		ERR_PRINTF1(_L("ENavigationModelSatelliteInfoArray exits in newly created Reader"));
	  		return EFail;
	  		}
		}
	  return TestStepResult();
	}



TVerdict CGpsAlmanacRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
