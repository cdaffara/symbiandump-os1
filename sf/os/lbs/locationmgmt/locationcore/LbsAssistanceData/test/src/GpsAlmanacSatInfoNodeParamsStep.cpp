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
 @file GpsAlmanacSatInfoNodeParamsStep.cpp
*/
#include "GpsAlmanacSatInfoNodeParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsAlmanacBuilder iServer.iGpsAlmanacBuilder
#define iAlmanacSatInfoArrayBuilder iServer.iAlmanacSatInfoArrayBuilder
#define iAlmanacSatInfoBuilder iServer.iAlmanacSatInfoBuilder

#define iGpsAlmanacReader iServer.iGpsAlmanacReader
#define iAlmanacSatInfoArrayReader iServer.iAlmanacSatInfoArrayReader
#define iAlmanacSatInfoReader iServer.iAlmanacSatInfoReader

#define iArrayIndex iServer.iArrayIndex


CGpsAlmanacSatInfoNodeParamsStep::~CGpsAlmanacSatInfoNodeParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsAlmanacSatInfoNodeParamsStep::CGpsAlmanacSatInfoNodeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
    CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAlmanacSatInfoNodeParamsStep);
	}

TVerdict CGpsAlmanacSatInfoNodeParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
   	if (iArrayIndex > 31)
    	{
    	ERR_PRINTF1(_L("Preamble: processing a node too many"));
    	SetTestStepResult(EFail);
    	}

	// A previous step may have built the ArrayBuilder and the node. If that is not the case,
	// then get them.
	//
   	iGpsAlmanacReader.DataBuffer()=iGpsAlmanacBuilder.DataBuffer();
   	TBool exists = iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray);
   	if(!exists)
		{
		iGpsAlmanacBuilder.GetArrayBuilder(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, iAlmanacSatInfoArrayBuilder);
		//Get a builder for the node at current index
   		//
   		iAlmanacSatInfoArrayBuilder.GetFieldBuilder(iArrayIndex, iAlmanacSatInfoBuilder);
   		}
   	else if(!iAlmanacSatInfoArrayBuilder.ElementExists(iArrayIndex))
   		{
   		// Array exists buy node does not. Get the node only
   		iAlmanacSatInfoArrayBuilder.GetFieldBuilder(iArrayIndex, iAlmanacSatInfoBuilder);
   		}
	
	return TestStepResult();
	}


TVerdict CGpsAlmanacSatInfoNodeParamsStep::doTestStepL()
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

    	TAlmanacSatInfo::TFieldId* fieldIdPtr = reinterpret_cast<TAlmanacSatInfo:: TFieldId*>(&fieldNumber);
    	
    	// The type of the value dependens on the filed Id
    	//
    	TUint* intValuePtr;
		intValuePtr = reinterpret_cast<TUint*>(&value);
    	iAlmanacSatInfoBuilder.SetField(*fieldIdPtr, *intValuePtr);
  
		// Copy data to the reader and check that the field just set can be read
		//
		iGpsAlmanacReader.DataBuffer()=iGpsAlmanacBuilder.DataBuffer();
    	exists = iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EAlmanacInfoSatArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		err = iGpsAlmanacReader.GetArrayReader(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray,iAlmanacSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get iAlmanacSatInfoArrayReader"));
    		SetTestStepResult(EFail);
    		}
		
		// Get node reader at index
        if (iAlmanacSatInfoArrayReader.ElementExists(iArrayIndex))
        	{
			err = iAlmanacSatInfoArrayReader.GetFieldReader(iArrayIndex,iAlmanacSatInfoReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in AlmanacSatInfoArrayReader"),iArrayIndex);
    			SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Node does not exist at index %D"),iArrayIndex);
    		SetTestStepResult(EFail);				
			}
			
		// Check that the field Id has been set with the correct value
		if (!iAlmanacSatInfoReader.FieldExists(*fieldIdPtr))
			{
			ERR_PRINTF1(_L("Field Id does not exist"));
    		SetTestStepResult(EFail);
			}
		else
			{   
	    	TUint intValue;
			iAlmanacSatInfoReader.GetField(*fieldIdPtr,intValue);
			// Is this the value that was set?
			if (intValue != *intValuePtr)
			    {
				ERR_PRINTF1(_L("Data set in the builder is different from data read"));
     	  	    SetTestStepResult(EFail);			    	
			    }
			}
		}
	return TestStepResult();
	}



TVerdict CGpsAlmanacSatInfoNodeParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
