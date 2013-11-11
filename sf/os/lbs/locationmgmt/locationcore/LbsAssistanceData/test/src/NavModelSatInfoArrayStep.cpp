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
 @file NavModelSatInfoArrayStep.cpp
*/
#include "NavModelSatInfoArrayStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CNavModelSatInfoArrayStep::~CNavModelSatInfoArrayStep()
/**
 * Destructor
 */
	{
	}

CNavModelSatInfoArrayStep::CNavModelSatInfoArrayStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavModelSatInfoArrayStep);
	}

TVerdict CNavModelSatInfoArrayStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual. Nothing done here.
 */
	{
	iServer.iArrayIndex = 0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CNavModelSatInfoArrayStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TBool exists;
	TInt err;
	if (TestStepResult()==EPass)
		{

	    INFO_PRINTF1(_L("CNavModelSatInfoArrayStep::doTestStepL() in the file NavModelSatInfoArrayStep.cpp"));

	    // Get the navigatin model array builder from the root property builder
    	iServer.iNavigationModelBuilder.GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, iServer.iNavModelSatInfoArrayBuilder);

    	// Get a builder for the element at index 2 in the array and set the ESatId to 8
    	// (the actual numbers don't matter, what's important is that the call to
    	// SetField will register the array in the root builder).
		//
		TUint anyIndex = 2;
		TUint unsignedTestData = 8;
		RNavigationModelSatInfoBuilder navModelSatInfoBuilder;
		iServer.iNavModelSatInfoArrayBuilder.GetFieldBuilder(anyIndex, navModelSatInfoBuilder);
        navModelSatInfoBuilder.SetField(TNavigationModelSatInfo::ESatId,unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
    	exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("ENavigationModelSatelliteInfoArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		err = iServer.iNavigationModelReader.GetArrayReader(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray,iServer.iNavModelSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get iNavModelSatInfoArrayReader"));
    		SetTestStepResult(EFail);
    		}
		}
		
	  return TestStepResult();
	}


// Clear array node 2 that was set in doTestStepL to make sure it doesn't interfere with
// future test steps
//
TVerdict CNavModelSatInfoArrayStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	TBool exists = EFalse;
	
	// Clear the array node set up in doTestStepL. Since it is the only node, this should clear
	// the whole array from the root builder.
	
	iServer.iNavModelSatInfoArrayBuilder.ClearElement(2);
	
	// Verify that the element has been cleared form the root builder
	// 
	iServer.iNavigationModelReader.DataBuffer() = iServer.iNavigationModelBuilder.DataBuffer();
	
	// Check that the array itself is no longer there
	exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
    if(exists)
		{
		ERR_PRINTF1(_L("Postamble:: ENavigationModelSatelliteInfoArray still exists"));
    	SetTestStepResult(EFail);
    	}
    	
   	// Set the array index to -1 (first node test will increase it before use)
   	iServer.iArrayIndex = -1;
   	
   	return TestStepResult();
	}
