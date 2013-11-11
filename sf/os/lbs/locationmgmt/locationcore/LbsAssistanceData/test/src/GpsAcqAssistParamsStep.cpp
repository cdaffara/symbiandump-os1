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
 @file GpsAcqAssistParamsStep.cpp
*/
#include "GpsAcqAssistParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader


CGpsAcqAssistParamsStep::~CGpsAcqAssistParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistParamsStep::CGpsAcqAssistParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistParamsStep);
	}

TVerdict CGpsAcqAssistParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistParamsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{

        TUint referenceTimeTestData = 0x1234ABCD;
        
        acquistionAssistanceRootBuilder.SetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, referenceTimeTestData);
        
        // Copy data to the reader and check that it can be read correctly and cleared
        //
        acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
        if(acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime))
        	{
        	    TUint readTime;
        		acquistionAssistanceRootReader.GetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, readTime);
        		if(readTime != referenceTimeTestData)
        			{
        			ERR_PRINTF1(_L("Incorrect data read from field EGpsReferenceTime"));
					SetTestStepResult(EFail);
        			}
        		acquistionAssistanceRootBuilder.ClearField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime);
        	}
        else
        	{
        	ERR_PRINTF1(_L("Missing Field EGpsReferenceTime"));
     	  	SetTestStepResult(EFail);
        	}
        	
        // Copy data again to the reader and check it's been cleared
        //
        acquistionAssistanceRootReader.DataBuffer() = acquistionAssistanceRootBuilder.DataBuffer();
        if(acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime))
        	{
        	ERR_PRINTF1(_L("Field EGpsReferenceTime exits after clearing it"));
			SetTestStepResult(EFail);
        	}
		}
	  return TestStepResult();
	}



TVerdict CGpsAcqAssistParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
