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
 @file GpsUtcModelParamsStep.cpp
*/
#include "GpsUtcModelParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsUtcModelBuilder iServer.iGpsUtcModelBuilder
#define iGpsUtcModelReader iServer.iGpsUtcModelReader

CGpsUtcModelParamsStep::~CGpsUtcModelParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsUtcModelParamsStep::CGpsUtcModelParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsUtcModelParamsStep);
	}

TVerdict CGpsUtcModelParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsUtcModelParamsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		
		TInt fieldNumber; // Field Id obtained from .ini  as an integer
    	TInt value; // Value to set in the field
    	if(!GetIntFromConfig(ConfigSection(), KFieldIdAsInt, fieldNumber))
    	    {
     	 	ERR_PRINTF1(_L("Could not get field Id from section"));
     	  	SetTestStepResult(EFail);
     	  	// Leave if there's any error.
			User::Leave(KErrNotFound);
      	    }
        
    	if(!GetIntFromConfig(ConfigSection(),KFieldValueAsInt, value))
    	    {
     	  	ERR_PRINTF1(_L("Could not get field value from section"));
     	  	SetTestStepResult(EFail);
     	  	// Leave if there's any error.
			User::Leave(KErrNotFound);
     	    }
     	    
    	TUEPositioningGpsUtcModel::TFieldId* fieldIdPtr = reinterpret_cast<TUEPositioningGpsUtcModel::TFieldId*>(&fieldNumber);
     	TUint* unsignedValuePtr = reinterpret_cast<TUint*>(&value);
		iGpsUtcModelBuilder.SetField(*fieldIdPtr,*unsignedValuePtr);
		
	    // Copy data to the reader and check that the field just set can be read
		iGpsUtcModelReader.DataBuffer() = iGpsUtcModelBuilder.DataBuffer();
		
		if(!iGpsUtcModelReader.FieldExists(*fieldIdPtr))
			{
			ERR_PRINTF2(_L("Field %D doesn't exist in reader"),fieldNumber);
     	  	SetTestStepResult(EFail);
     	  	// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		else
			{
		    TUint valueRead;
			iGpsUtcModelReader.GetField(*fieldIdPtr,valueRead);
		    if(valueRead != *unsignedValuePtr)
		    	{
		    	ERR_PRINTF2(_L("Value read for field number %d is incorrect"),fieldNumber);
     	  	    SetTestStepResult(EFail);
		    	}
			}
		}
	  return TestStepResult();
	}



TVerdict CGpsUtcModelParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
