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
 @file GpsRefTimeTowAssistArrayStep.cpp
*/
#include "GpsRefTimeTowAssistArrayStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

#define gpsTowAssistArrayBuilder iServer.iGpsTowAssistArrayBuilder
#define gpsTowAssistArrayReader iServer.iGpsTowAssistArrayReader


CGpsRefTimeTowAssistArrayStep::~CGpsRefTimeTowAssistArrayStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeTowAssistArrayStep::CGpsRefTimeTowAssistArrayStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeTowAssistArrayStep);
	}

TVerdict CGpsRefTimeTowAssistArrayStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsRefTimeTowAssistArrayStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		
		gpsReferenceTimeBuilder.GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, gpsTowAssistArrayBuilder);
		
		// Set some data in the array and check that it exists
		//
		TUint unsignedTestData = 8;
		RGpsTowAssistBuilder towAssistBuilder;
		gpsTowAssistArrayBuilder.GetFieldBuilder(2, towAssistBuilder);
        towAssistBuilder.SetField(TGpsTowAssist::ETlmMessage, unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
    	TBool exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EAlmanacInfoSatArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
    		
		TUint err = gpsReferenceTimeReader.GetArrayReader(TUEPositioningGpsReferenceTime::EGpsTowAssistArray,gpsTowAssistArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get EAlmanacInfoSatArray"));
    		SetTestStepResult(EFail);
    		}
		}
	  return TestStepResult();

	}



TVerdict CGpsRefTimeTowAssistArrayStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	TBool exists = EFalse;
	
	// Clear the array node set up in doTestStepL. Since it is the only node, this should clear
	// the whole array from the root builder.
	//
	gpsTowAssistArrayBuilder.ClearElement(2);
	
	// Verify that the element has been cleared form the root builder
	//
    gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
	
	// Check that the array itself is no longer there
	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray);
    if(exists)
		{
		ERR_PRINTF1(_L("Postamble:: EGpsTowAssistArray still exists"));
    	SetTestStepResult(EFail);
    	}

   	// Set the array index to -1 (first node test step will increase it before use)
   	iServer.iArrayIndex = -1;
   	
	return TestStepResult();

	}
