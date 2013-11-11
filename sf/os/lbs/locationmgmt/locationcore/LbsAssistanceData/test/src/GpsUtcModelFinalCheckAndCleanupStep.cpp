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
 @file GpsUtcModelFinalCheckAndCleanupStep.cpp
*/
#include "Te_LbsAssistanceDataSuiteServer.h"
#include "GpsUtcModelFinalCheckAndCleanupStep.h"

#define iGpsUtcModelBuilder iServer.iGpsUtcModelBuilder
#define iGpsUtcModelReader iServer.iGpsUtcModelReader

CGpsUtcModelFinalCheckAndCleanupStep::~CGpsUtcModelFinalCheckAndCleanupStep()
/**
 * Destructor
 */
	{
	}

CGpsUtcModelFinalCheckAndCleanupStep::CGpsUtcModelFinalCheckAndCleanupStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsUtcModelFinalCheckAndCleanupStep);
	}

TVerdict CGpsUtcModelFinalCheckAndCleanupStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
   	return TestStepResult();
	}

// Perform some random value checks
//
//
TVerdict CGpsUtcModelFinalCheckAndCleanupStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    // Check that all the fieds exists and their values are correct
    TUEPositioningGpsUtcModel::TFieldId* fieldIdPtr;
    TUint value;
    
    // From EA1 to EDeltaLSF there are 8 field ids
    // 
    for (TUint ii = 0; ii < 8; ii++)
    	{
    	fieldIdPtr = reinterpret_cast<TUEPositioningGpsUtcModel::TFieldId*>(&ii);
    	
    	if(!iGpsUtcModelReader.FieldExists(*fieldIdPtr))
    		{
    		ERR_PRINTF2(_L("Missing field number %D ."),ii);
			SetTestStepResult(EFail);
    		}
    	else
    		{
    		iGpsUtcModelReader.GetField(*fieldIdPtr,value);
    		if(value != (ii+1))
    			{
    			ERR_PRINTF2(_L("Wrong value in field number %D ."),ii);
				SetTestStepResult(EFail);
    			}
    		else
    			{
    			//Clear this field from the builder and get the data
    			// again back in the reader.
    			//
    			iGpsUtcModelBuilder.ClearField(*fieldIdPtr);
				iGpsUtcModelReader.DataBuffer() = iGpsUtcModelBuilder.DataBuffer();
				
				// Check that this field no longer exists
				//
 			   	if(iGpsUtcModelReader.FieldExists(*fieldIdPtr))
    				{
    				ERR_PRINTF2(_L("Field number %D was cleared but still exists."),ii);
					SetTestStepResult(EFail);
    				}				
    			}
    		}
    	
    	}
    	
	return TestStepResult();
	}


TVerdict CGpsUtcModelFinalCheckAndCleanupStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Close the GpsUtcigation Model property to prevent 
	// TEF warnings due to memory leakage.
	iGpsUtcModelBuilder.Close();
	iGpsUtcModelReader.Close();
	return TestStepResult();
	}
