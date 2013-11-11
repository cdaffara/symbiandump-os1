// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file NavModelSatInfoNodeParameterStep.cpp
*/
#include "NavModelSatInfoNodeParameterStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CNavModelSatInfoNodeParameterStep::~CNavModelSatInfoNodeParameterStep()
/**
 * Destructor
 */
	{
	}

CNavModelSatInfoNodeParameterStep::CNavModelSatInfoNodeParameterStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavModelSatInfoNodeParameterStep);
	}

TVerdict CNavModelSatInfoNodeParameterStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	SetTestStepResult(EPass);
   	if (iServer.iArrayIndex > 31)
    	{
    	ERR_PRINTF1(_L("Preamble: processing a node too many"));
    	SetTestStepResult(EFail);
    	}

	// A previous step may have built the NavModelSatInfoArrayBuilder and the node. If that is not the case,
	// then get them.
	//
   	iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
   	TBool exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
   	if(!exists)
		{
		iServer.iNavigationModelBuilder.GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, iServer.iNavModelSatInfoArrayBuilder);
		//Get a builder for the node at current index
   		//
   		iServer.iNavModelSatInfoArrayBuilder.GetFieldBuilder(iServer.iArrayIndex, iServer.iNavModelSatInfoBuilder);
   		}
   	else if(!iServer.iNavModelSatInfoArrayBuilder.ElementExists(iServer.iArrayIndex))
   		{
   		// Array exists buy node does not. Get the node only
   		iServer.iNavModelSatInfoArrayBuilder.GetFieldBuilder(iServer.iArrayIndex, iServer.iNavModelSatInfoBuilder);
   		}
	
	return TestStepResult();
	}


TVerdict CNavModelSatInfoNodeParameterStep::doTestStepL()
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

    	TNavigationModelSatInfo::TFieldId* fieldIdPtr = reinterpret_cast<TNavigationModelSatInfo:: TFieldId*>(&fieldNumber);
    	
    	// The type of the value dependens on the filed Id
    	//
    	TUint* intValuePtr = NULL;
    	TSatelliteStatus* satStatusValuePtr = NULL;
    	switch(fieldNumber)
    		{
    		case 0: // TNavigationModelSatInfo::ESatId takes integers
    			intValuePtr = reinterpret_cast<TUint*>(&value);
    			iServer.iNavModelSatInfoBuilder.SetField(*fieldIdPtr, *intValuePtr);
    			break;
    		case 1: // TNavigationModelSatInfo::ESatelliteStatus takes enum TSatelliteStatus
    			satStatusValuePtr = reinterpret_cast<TSatelliteStatus*>(&value);
    			iServer.iNavModelSatInfoBuilder.SetField(*fieldIdPtr, *satStatusValuePtr);
    			break;
    		default:
    		    // Only 0 and 1 are simple types that can be given a value, 3 exists but is a struct (ephemeris)
    		    ERR_PRINTF1(_L("Step is trying to set a field Id that doesn't exist in node"));
     	  	    SetTestStepResult(EFail);
     	  	    // Leave if there's any error.
			    User::Leave(KErrNotFound);
			    break;
    		}
    		
		// Copy data to the reader and check that the field just set can be read
		//
		iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
    	exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
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
			ERR_PRINTF2(_L("Node does not exist at index %D"),iServer.iArrayIndex);
    		SetTestStepResult(EFail);				
			}
			
		// Check that the field Id has been set with the correct value
		if (!iServer.iNavModelSatInfoReader.FieldExists(*fieldIdPtr))
			{
			ERR_PRINTF1(_L("Field Id does not exist"));
    		SetTestStepResult(EFail);
			}
		else
			{   
			TBool res = EFalse;
			// Read the value that was set using the correct value type for
			// the field Id (otherwise it'll panic)
			switch(fieldNumber)
			   	{
			   	case 0: // TNavigationModelSatInfo::ESatId takes integers
    		    	TUint intValue;
					iServer.iNavModelSatInfoReader.GetField(*fieldIdPtr,intValue);
					// Is this the value that was set?
					res = (intValue == *intValuePtr);
    				break;
    			case 1: // TNavigationModelSatInfo::ESatelliteStatus takes enum TSatelliteStatus
    				TSatelliteStatus satStatusValue;
    				iServer.iNavModelSatInfoReader.GetField(*fieldIdPtr, satStatusValue);
    				res = (satStatusValue == *satStatusValuePtr);
    				break;
    			default:
		    	    ERR_PRINTF1(_L("Field Id that doesn't exist in node"));
     	  	   		SetTestStepResult(EFail);
     	  	   		// Leave if there's any error.
			   		User::Leave(KErrNotFound);
			   		break;
			   	}
			if (!res)
			    {
				ERR_PRINTF1(_L("Data set in the builder is different from data read"));
     	  	    SetTestStepResult(EFail);			    	
			    }
			}
		}
	return TestStepResult();		
}


TVerdict CNavModelSatInfoNodeParameterStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
