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
 @file GpsAcqAssistUtranGpsTimeStep.cpp
*/
#include "GpsAcqAssistUtranGpsTimeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder

#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader


#define	utranGpsRefTimeBuilder iServer.iUtranGpsRefTimeBuilder
#define primaryCpichInfoBuilder iServer.iPrimaryCpichInfoBuilder
	
#define	utranGpsRefTimeReader iServer.iUtranGpsRefTimeReader
#define	primaryCpichInfoReader iServer.iPrimaryCpichInfoReader

#define arrayIndex iServer.iArrayIndex

CGpsAcqAssistUtranGpsTimeStep::~CGpsAcqAssistUtranGpsTimeStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistUtranGpsTimeStep::CGpsAcqAssistUtranGpsTimeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistUtranGpsTimeStep);
	}

TVerdict CGpsAcqAssistUtranGpsTimeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistUtranGpsTimeStep::doTestStepL()
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

	    INFO_PRINTF1(_L("CGpsAcqAssistUtranGpsTimeStep::doTestStepL() in the file GpsAcqAssistUtranGpsTimeStep.cpp"));

	    // Get from the root property builder a property builder for TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime
		acquistionAssistanceRootBuilder.GetFieldBuilder(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime, utranGpsRefTimeBuilder);


    	// Set some data
		//
		TUint timingOfCell_LsPart = 2;
		utranGpsRefTimeBuilder.SetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart,timingOfCell_LsPart);

	    // Now it is possible to check if a reader can access the EUtranGpsReferenceTime
    	//
    	acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
    	exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EUtranGpsReferenceTime doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //EUtranGpsReferenceTime reader
		err = acquistionAssistanceRootReader.GetFieldReader(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime,utranGpsRefTimeReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get utranGpsRefTimeReader"));
    		SetTestStepResult(EFail);
    		}
		}
		
	  return TestStepResult();
	}



TVerdict CGpsAcqAssistUtranGpsTimeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	TBool exists = EFalse;
	
	// Clear the field set up in doTestStepL. 
	
	utranGpsRefTimeBuilder.ClearField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart);
	
	// Verify that the element has been cleared form the root builder
	// 
	acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
	
	// Check that the array itself is no longer there
	exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime);
    if(exists)
		{
		ERR_PRINTF1(_L("Postamble:: EUtranGpsReferenceTime still exists"));
    	SetTestStepResult(EFail);
    	}
    	
   	// Set the array index to -1 (first node test will increase it before use)
   	iServer.iArrayIndex = -1;
   	
   	return TestStepResult();
	}
