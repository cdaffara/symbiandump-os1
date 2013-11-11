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
 @file GpsRefTimeUtranGpsTimeStep.cpp
*/
#include "GpsRefTimeUtranGpsTimeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

#define	utranGpsRefTimeBuilder iServer.iUtranGpsRefTimeBuilder
#define primaryCpichInfoBuilder iServer.iPrimaryCpichInfoBuilder
	
#define	utranGpsRefTimeReader iServer.iUtranGpsRefTimeReader
#define	primaryCpichInfoReader iServer.iPrimaryCpichInfoReader

CGpsRefTimeUtranGpsTimeStep::~CGpsRefTimeUtranGpsTimeStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeUtranGpsTimeStep::CGpsRefTimeUtranGpsTimeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeUtranGpsTimeStep);
	}

TVerdict CGpsRefTimeUtranGpsTimeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsRefTimeUtranGpsTimeStep::doTestStepL()
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

	    INFO_PRINTF1(_L("CGpsRefTimeUtranGpsTimeStep::doTestStepL()"));

	    // Get from the root property builder a property builder for TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime
		gpsReferenceTimeBuilder.GetFieldBuilder(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsRefTimeBuilder);


    	// Set some data
		//
		TUint timingOfCell_LsPart = 2;
		utranGpsRefTimeBuilder.SetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart,timingOfCell_LsPart);

	    // Now it is possible to check if a reader can access the EUtranGpsReferenceTime
    	//
    	gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
    	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EUtranGpsReferenceTime doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //EUtranGpsReferenceTime reader
		err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime,utranGpsRefTimeReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get utranGpsRefTimeReader"));
    		SetTestStepResult(EFail);
    		}
		}
		
	  return TestStepResult();
	}

TVerdict CGpsRefTimeUtranGpsTimeStep::doTestStepPostambleL()
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
	gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
	
	// Check that field no longer exists
	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime);
    if(exists)
		{
		ERR_PRINTF1(_L("Postamble:: EUtranGpsReferenceTime still exists"));
    	SetTestStepResult(EFail);
    	}
    	

   	return TestStepResult();
	}
