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
 @file GpsAcqAssistSatInfoNodeStep.cpp
*/
#include "GpsAcqAssistSatInfoNodeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquisitionSatInfoArrayBuilder iServer.iAcquisitionSatInfoArrayBuilder

#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader
#define acquisitionSatInfoArrayReader iServer.iAcquisitionSatInfoArrayReader

#define acquisitionSatInfoBuilder iServer.iAcquisitionSatInfoBuilder
#define acquisitionSatInfoReader iServer.iAcquisitionSatInfoReader


#define arrayIndex iServer.iArrayIndex

CGpsAcqAssistSatInfoNodeStep::~CGpsAcqAssistSatInfoNodeStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistSatInfoNodeStep::CGpsAcqAssistSatInfoNodeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
    CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistSatInfoNodeStep);
	}

TVerdict CGpsAcqAssistSatInfoNodeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{

	// A previous step may have built the NavModelSatInfoArrayBuilder. If that is not the case,
	// then build it.
	//
	SetTestStepResult(EPass);
   	acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
   	TBool exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
   	if (!exists)
		{
		acquistionAssistanceRootBuilder.GetArrayBuilder(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray, acquisitionSatInfoArrayBuilder);
   		}

    // Modify index to grab next node. Test only nodes 0, 15 and 31.
    // (initial value is -1 so first used index will be zero)
    if (arrayIndex == -1)
    	{
    	arrayIndex = 0;
    	}
    else if (arrayIndex == 0)
    	{
    	arrayIndex = 15;
    	}
    else if (arrayIndex == 15)
    	{
    	arrayIndex = 31;	
    	}
    else
    	{
    	SetTestStepResult(EFail);	
    	}

   	INFO_PRINTF2(_L("Node being proccesed at index %D"),arrayIndex);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistSatInfoNodeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		if((arrayIndex < 0) || (arrayIndex > 31))
		{
			ERR_PRINTF1(_L("Index is out of range"));
			SetTestStepResult(EFail);
			return TestStepResult();
		}
		
		acquisitionSatInfoArrayBuilder.GetFieldBuilder(arrayIndex, acquisitionSatInfoBuilder);

    	// Set some data in the node 
    	// 
    	TUint unsignedTestData = 0;
        acquisitionSatInfoBuilder.SetField(TAcquisitionSatInfo::ESatID,unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	acquistionAssistanceRootReader.DataBuffer()=acquistionAssistanceRootBuilder.DataBuffer();
    	TBool exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("ESatelliteInformationArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		TUint err = acquistionAssistanceRootReader.GetArrayReader(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray,acquisitionSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get iNavModelSatInfoArrayReader"));
    		SetTestStepResult(EFail);
    		}
		
		// Get node reader at index
        if (acquisitionSatInfoArrayReader.ElementExists(arrayIndex))
        	{
			err = acquisitionSatInfoArrayReader.GetFieldReader(arrayIndex,acquisitionSatInfoReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in navModelSatInfoArrayReader"),arrayIndex);
    			SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("ArrayElement doesn't exist at index %D"),arrayIndex);
    		SetTestStepResult(EFail);				
			}
		
		}
	  return TestStepResult();
	}



TVerdict CGpsAcqAssistSatInfoNodeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Since some data has been set in the node, it is better to
	// clear the whole node not to mess up later test steps. If 
	// this was the only node in the array, the array
	// also gets cleared, but that is not neccesarily the case.
	//
	acquisitionSatInfoArrayBuilder.ClearElement(arrayIndex);
	return TestStepResult();
	}
