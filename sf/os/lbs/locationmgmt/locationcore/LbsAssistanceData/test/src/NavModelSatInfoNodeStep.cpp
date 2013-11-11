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
 @file NavModelSatInfoNodeStep.cpp
*/
#include "NavModelSatInfoNodeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CNavModelSatInfoNodeStep::~CNavModelSatInfoNodeStep()
/**
 * Destructor
 */
	{
	}

CNavModelSatInfoNodeStep::CNavModelSatInfoNodeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
    CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavModelSatInfoNodeStep);
	}

TVerdict CNavModelSatInfoNodeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	// A previous step may have built the NavModelSatInfoArrayBuilder. If that is not the case,
	// then build it.
	//
	SetTestStepResult(EPass);
   	iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
   	TBool exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
   	if(!exists)
		{
		iServer.iNavigationModelBuilder.GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, iServer.iNavModelSatInfoArrayBuilder);
   		}	

    // Modify index to grab next node. Test only nodes 0, 15 and 31.
    // (initial value is -1 so first used index will be zero)
    if (iServer.iArrayIndex == -1)
    	{
    	iServer.iArrayIndex = 0;
    	}
    else if (iServer.iArrayIndex == 0)
    	{
    	iServer.iArrayIndex = 15;
    	}
    else if (iServer.iArrayIndex == 15)
    	{
    	iServer.iArrayIndex = 31;	
    	}
    else
    	{
    	SetTestStepResult(EFail);	
    	}
    
   	INFO_PRINTF2(_L("Node being proccesed at index %D"),iServer.iArrayIndex);

	return TestStepResult();
	}


TVerdict CNavModelSatInfoNodeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt err;
	// This step builds the node at index iServer.iArrayIndex, copies the data to the reader
	// and checks that the reader can read it.
	//
	if (TestStepResult()==EPass)
		{
		if((iServer.iArrayIndex < 0) || (iServer.iArrayIndex > 31))
		{
			ERR_PRINTF1(_L("Index is out of range"));
			SetTestStepResult(EFail);
			return TestStepResult();
		}
		
		iServer.iNavModelSatInfoArrayBuilder.GetFieldBuilder(iServer.iArrayIndex, iServer.iNavModelSatInfoBuilder);

    	// Set some data in the node 
    	TUint unsignedTestData = 0;
        iServer.iNavModelSatInfoBuilder.SetField(TNavigationModelSatInfo::ESatId,unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
    	TBool exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
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
		
		// Get node reader at index
        if (iServer.iNavModelSatInfoArrayReader.ElementExists(iServer.iArrayIndex))
        	{
			err = iServer.iNavModelSatInfoArrayReader.GetFieldReader(iServer.iArrayIndex,iServer.iNavModelSatInfoReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in navModelSatInfoArrayReader"),iServer.iArrayIndex);
    			SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("ArrayElement doesn't exist at index %D"),iServer.iArrayIndex);
    		SetTestStepResult(EFail);				
			}
		}
	  return TestStepResult();
	}


TVerdict CNavModelSatInfoNodeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	// Since some data has been set in the node, it is better to
	// clear the whole node not to confuse later test steps. If 
	// this was the only node in the array, the array
	// also gets cleared, but that is not neccesarily the case.
	//
	iServer.iNavModelSatInfoArrayBuilder.ClearElement(iServer.iArrayIndex);
	return TestStepResult();
	}
