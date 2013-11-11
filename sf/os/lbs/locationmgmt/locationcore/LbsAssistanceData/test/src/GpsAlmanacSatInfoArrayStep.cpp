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
 @file GpsAlmanacSatInfoArrayStep.cpp
*/
#include "GpsAlmanacSatInfoArrayStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsAlmanacBuilder iServer.iGpsAlmanacBuilder
#define iAlmanacSatInfoArrayBuilder iServer.iAlmanacSatInfoArrayBuilder
#define iAlmanacSatInfoBuilder iServer.iAlmanacSatInfoBuilder

#define iGpsAlmanacReader iServer.iGpsAlmanacReader
#define iAlmanacSatInfoArrayReader iServer.iAlmanacSatInfoArrayReader
#define iAlmanacSatInfoReader iSever.iAlmanacSatInfoReader

#define iArrayIndex iServer.iArrayIndex

CGpsAlmanacSatInfoArrayStep::~CGpsAlmanacSatInfoArrayStep()
/**
 * Destructor
 */
	{
	}

CGpsAlmanacSatInfoArrayStep::CGpsAlmanacSatInfoArrayStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAlmanacSatInfoArrayStep);
	}

TVerdict CGpsAlmanacSatInfoArrayStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CGpsAlmanacSatInfoArrayStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		iGpsAlmanacBuilder.GetArrayBuilder(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, iAlmanacSatInfoArrayBuilder);
		
		// Set some data in the array and check that it exists
		//
		TUint unsignedTestData = 8;
		RAlmanacSatInfoBuilder satInfoBuilder;
		iAlmanacSatInfoArrayBuilder.GetFieldBuilder(2, satInfoBuilder);
        satInfoBuilder.SetField(TAlmanacSatInfo::ESatID, unsignedTestData);

	    // Now it is possible to check if a reader can access the array
    	//
    	iGpsAlmanacReader.DataBuffer() = iGpsAlmanacBuilder.DataBuffer();
    	TBool exists = iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EAlmanacInfoSatArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
    		
		TUint err = iGpsAlmanacReader.GetArrayReader(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray,iAlmanacSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get EAlmanacInfoSatArray"));
    		SetTestStepResult(EFail);
    		}
		}
	  return TestStepResult();
	}



TVerdict CGpsAlmanacSatInfoArrayStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	TBool exists = EFalse;
	
	// Clear the array node set up in doTestStepL. Since it is the only node, this should clear
	// the whole array from the root builder.
	//
	iAlmanacSatInfoArrayBuilder.ClearElement(2);
	
	// Verify that the element has been cleared form the root builder
	//
	iGpsAlmanacReader.DataBuffer() = iGpsAlmanacBuilder.DataBuffer();
	
	// Check that the array itself is no longer there
	exists = iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray);
    if(exists)
		{
		ERR_PRINTF1(_L("Postamble:: EAlmanacInfoSatArray still exists"));
    	SetTestStepResult(EFail);
    	}

   	// Set the array index to -1 (first node test step will increase it before use)
   	iArrayIndex = -1;
   	
	return TestStepResult();
	}
