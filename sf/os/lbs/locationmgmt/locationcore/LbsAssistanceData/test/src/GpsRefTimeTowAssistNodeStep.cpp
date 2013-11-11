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
 @file GpsRefTimeTowAssistNodeStep.cpp
*/
#include "GpsRefTimeTowAssistNodeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

#define gpsTowAssistArrayBuilder iServer.iGpsTowAssistArrayBuilder
#define gpsTowAssistArrayReader iServer.iGpsTowAssistArrayReader

#define gpsTowAssistBuilder iServer.iGpsTowAssistBuilder
#define gpsTowAssistReader iServer.iGpsTowAssistReader

#define arrayIndex iServer.iArrayIndex

CGpsRefTimeTowAssistNodeStep::~CGpsRefTimeTowAssistNodeStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeTowAssistNodeStep::CGpsRefTimeTowAssistNodeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeTowAssistNodeStep);
	}

TVerdict CGpsRefTimeTowAssistNodeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// A previous step may have built the gpsTowAssistArrayBuilder. If that is not the case,
	// then build it.
	//
	SetTestStepResult(EPass);
   	gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
   	TBool exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray);
   	if (!exists)
		{
		gpsReferenceTimeBuilder.GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, gpsTowAssistArrayBuilder);
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


TVerdict CGpsRefTimeTowAssistNodeStep::doTestStepL()
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
		
		gpsTowAssistArrayBuilder.GetFieldBuilder(arrayIndex, gpsTowAssistBuilder);

    	// Set some data in the node 
    	// 
    	TUint unsignedTestData = 0;
        gpsTowAssistBuilder.SetField(TGpsTowAssist::ETlmMessage,unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
    	TBool exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EGpsTowAssistArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		TUint err = gpsReferenceTimeReader.GetArrayReader(TUEPositioningGpsReferenceTime::EGpsTowAssistArray,gpsTowAssistArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get gpsTowAssistArrayReader"));
    		SetTestStepResult(EFail);
    		}
		
		// Get node reader at index
        if (gpsTowAssistArrayReader.ElementExists(arrayIndex))
        	{
			err = gpsTowAssistArrayReader.GetFieldReader(arrayIndex,gpsTowAssistReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in gpsTowAssistArrayReader"),arrayIndex);
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



TVerdict CGpsRefTimeTowAssistNodeStep::doTestStepPostambleL()
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
	gpsTowAssistArrayBuilder.ClearElement(arrayIndex);
	return TestStepResult();
	}
