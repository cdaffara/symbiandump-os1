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
 @file GpsAcqAssistSatInfoArrayStep.cpp
*/
#include "GpsAcqAssistSatInfoArrayStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquisitionSatInfoArrayBuilder iServer.iAcquisitionSatInfoArrayBuilder

#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader
#define acquisitionSatInfoArrayReader iServer.iAcquisitionSatInfoArrayReader

#define arrayIndex iServer.iArrayIndex

CGpsAcqAssistSatInfoArrayStep::~CGpsAcqAssistSatInfoArrayStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistSatInfoArrayStep::CGpsAcqAssistSatInfoArrayStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistSatInfoArrayStep);
	}

TVerdict CGpsAcqAssistSatInfoArrayStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistSatInfoArrayStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		
		acquistionAssistanceRootBuilder.GetArrayBuilder(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray, acquisitionSatInfoArrayBuilder);
		
		// Set some data in the array and check that it exists
		//
		TUint unsignedTestData = 8;
		RAcquisitionSatInfoBuilder satInfoBuilder;
		acquisitionSatInfoArrayBuilder.GetFieldBuilder(2, satInfoBuilder);
        satInfoBuilder.SetField(TAcquisitionSatInfo::ESatID, unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
    	TBool exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EAlmanacInfoSatArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
    		
		TUint err = acquistionAssistanceRootReader.GetArrayReader(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray,acquisitionSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get EAlmanacInfoSatArray"));
    		SetTestStepResult(EFail);
    		}
		}
	  return TestStepResult();

	}



TVerdict CGpsAcqAssistSatInfoArrayStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	TBool exists = EFalse;
	
	// Clear the array node set up in doTestStepL. Since it is the only node, this should clear
	// the whole array from the root builder.
	//
	acquisitionSatInfoArrayBuilder.ClearElement(2);
	
	// Verify that the element has been cleared form the root builder
	//
    acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
	
	// Check that the array itself is no longer there
	exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
    if(exists)
		{
		ERR_PRINTF1(_L("Postamble:: ESatelliteInformationArray still exists"));
    	SetTestStepResult(EFail);
    	}

   	// Set the array index to -1 (first node test step will increase it before use)
   	arrayIndex = -1;
   	
	return TestStepResult();
	}
