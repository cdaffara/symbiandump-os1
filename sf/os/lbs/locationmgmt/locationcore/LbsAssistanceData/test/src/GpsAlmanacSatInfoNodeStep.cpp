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
 @file GpsAlmanacSatInfoNodeStep.cpp
*/
#include "GpsAlmanacSatInfoNodeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsAlmanacBuilder iServer.iGpsAlmanacBuilder
#define iAlmanacSatInfoArrayBuilder iServer.iAlmanacSatInfoArrayBuilder
#define iAlmanacSatInfoBuilder iServer.iAlmanacSatInfoBuilder

#define iGpsAlmanacReader iServer.iGpsAlmanacReader
#define iAlmanacSatInfoArrayReader iServer.iAlmanacSatInfoArrayReader
#define iAlmanacSatInfoReader iServer.iAlmanacSatInfoReader

#define iArrayIndex iServer.iArrayIndex

CGpsAlmanacSatInfoNodeStep::~CGpsAlmanacSatInfoNodeStep()
/**
 * Destructor
 */
	{
	}

CGpsAlmanacSatInfoNodeStep::CGpsAlmanacSatInfoNodeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
    CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAlmanacSatInfoNodeStep);
	}

TVerdict CGpsAlmanacSatInfoNodeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{

	// A previous step may have built the NavModelSatInfoArrayBuilder. If that is not the case,
	// then build it.
	//
	SetTestStepResult(EPass);
   	iGpsAlmanacReader.DataBuffer() = iGpsAlmanacBuilder.DataBuffer();
   	TBool exists = iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray);
   	if (!exists)
		{
		iGpsAlmanacBuilder.GetArrayBuilder(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, iAlmanacSatInfoArrayBuilder);
   		}

    // Modify index to grab next node. Test only nodes 0, 15 and 31.
    // (initial value is -1 so first used index will be zero)
    if (iArrayIndex == -1)
    	{
    	iArrayIndex = 0;
    	}
    else if (iArrayIndex == 0)
    	{
    	iArrayIndex = 15;
    	}
    else if (iArrayIndex == 15)
    	{
    	iArrayIndex = 31;	
    	}
    else
    	{
    	SetTestStepResult(EFail);	
    	}
    
   	INFO_PRINTF2(_L("Node being proccesed at index %D"),iArrayIndex);

	return TestStepResult();
	}


TVerdict CGpsAlmanacSatInfoNodeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		if((iArrayIndex < 0) || (iArrayIndex > 31))
		{
			ERR_PRINTF1(_L("Index is out of range"));
			SetTestStepResult(EFail);
			return TestStepResult();
		}
		
		iAlmanacSatInfoArrayBuilder.GetFieldBuilder(iArrayIndex, iAlmanacSatInfoBuilder);

    	// Set some data in the node 
    	// 
    	TUint unsignedTestData = 0;
        iAlmanacSatInfoBuilder.SetField(TAlmanacSatInfo::ESatID,unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	iGpsAlmanacReader.DataBuffer()=iGpsAlmanacBuilder.DataBuffer();
    	TBool exists = iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("ENavigationModelSatelliteInfoArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		TUint err = iGpsAlmanacReader.GetArrayReader(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray,iAlmanacSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get iNavModelSatInfoArrayReader"));
    		SetTestStepResult(EFail);
    		}
		
		// Get node reader at index
        if (iAlmanacSatInfoArrayReader.ElementExists(iArrayIndex))
        	{
			err = iAlmanacSatInfoArrayReader.GetFieldReader(iArrayIndex,iAlmanacSatInfoReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in navModelSatInfoArrayReader"),iArrayIndex);
    			SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("ArrayElement doesn't exist at index %D"),iArrayIndex);
    		SetTestStepResult(EFail);				
			}
		
		}
	  return TestStepResult();
	}



TVerdict CGpsAlmanacSatInfoNodeStep::doTestStepPostambleL()
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
	iAlmanacSatInfoArrayBuilder.ClearElement(iArrayIndex);
	return TestStepResult();
	}
