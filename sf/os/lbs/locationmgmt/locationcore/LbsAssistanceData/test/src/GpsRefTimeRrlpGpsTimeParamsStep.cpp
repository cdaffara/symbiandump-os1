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
 @file GpsRefTimeRrlpGpsTimeParamsStep.cpp
*/
#include "GpsRefTimeRrlpGpsTimeParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

#define	rrlpGpsRefTimeBuilder iServer.iRrlpGpsRefTimeBuilder
#define	rrlpGpsRefTimeReader iServer.iRrlpGpsRefTimeReader

CGpsRefTimeRrlpGpsTimeParamsStep::~CGpsRefTimeRrlpGpsTimeParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeRrlpGpsTimeParamsStep::CGpsRefTimeRrlpGpsTimeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeRrlpGpsTimeParamsStep);
	}

TVerdict CGpsRefTimeRrlpGpsTimeParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// If the field TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime doesn't exist,
	// build one
	if (!gpsReferenceTimeBuilder.FieldExists(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime))
		{
		gpsReferenceTimeBuilder.GetFieldBuilder(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime, rrlpGpsRefTimeBuilder);
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();

	}


TVerdict CGpsRefTimeRrlpGpsTimeParamsStep::doTestStepL()
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

    	TRrlpGpsReferenceTime::TFieldId* fieldIdPtr = reinterpret_cast<TRrlpGpsReferenceTime:: TFieldId*>(&fieldNumber);
    	TUint* intValuePtr = reinterpret_cast<TUint*>(&value);

        // Check that the field is initially empty
        gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
        if (!gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime))
        	{
        	INFO_PRINTF1(_L("ERrlpGpsReferenceTime doesn't exist (correct the first time in this step)"));
        	}
		else
			{
	        //ERrlpGpsReferenceTime reader
			err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime, rrlpGpsRefTimeReader);
			if(err != KErrNone)
    			{
    			ERR_PRINTF1(_L("Failed to get ERrlpGpsReferenceTime"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
    			}
		    else
		    	{
		    	// Check that the field Id doesn't exist yet
				//
        		if (rrlpGpsRefTimeReader.FieldExists(*fieldIdPtr))
        			{
        			ERR_PRINTF2(_L("Field number %d exists before it's been set"),fieldNumber);
    			    SetTestStepResult(EFail);
    			    User::Leave(KErrNotFound);
        			}
		    	}
			}
			
		// Set the data in the field
		//	  	
    	rrlpGpsRefTimeBuilder.SetField(*fieldIdPtr, *intValuePtr);
  
		// Copy data to the reader and check that the field just set can be read
		//
		gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
    	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime);
    	if(!exists)
			{
			ERR_PRINTF1(_L("ERrlpGpsReferenceTime doesn't exist"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		}
			
	    //ERrlpGpsReferenceTime reader
		err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime, rrlpGpsRefTimeReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get ERrlpGpsReferenceTime"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		}
		
		// Check that the field Id set above can be read
		// and the value is correct
		//
        if (rrlpGpsRefTimeReader.FieldExists(*fieldIdPtr))
        	{
        	TUint readValue;
			err = rrlpGpsRefTimeReader.GetField(*fieldIdPtr,readValue);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get element at field %d"),fieldNumber);
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
				}
			else
				{
				// Get the value
				if (*intValuePtr != readValue)
					{
					ERR_PRINTF1(_L("Data set in the builder is different from data read"));
     	  	    	SetTestStepResult(EFail);
     	  	    	User::Leave(KErrNotFound);
					}
				}
			}
		else
			{
			ERR_PRINTF2(_L("Field %d does not exist"),*fieldIdPtr);
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);				
			}
			
			
		// Clear the element and check that it is correctly cleared
		//
		rrlpGpsRefTimeBuilder.ClearField(*fieldIdPtr);
		
		// Verify element has been cleared
		//
		gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
    	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime);
    	if(!exists)
			{
			INFO_PRINTF1(_L("ERrlpGpsReferenceTime doesn't exist (correct the first time in here)"));
    		}
		else
			{
			//ERrlpGpsReferenceTime reader
			err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime, rrlpGpsRefTimeReader);
			if(err != KErrNone)
    			{
    			ERR_PRINTF1(_L("Failed to get ERrlpGpsReferenceTime"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
    			}
		
				// Check that the field Id was cleared
				//
        		if (rrlpGpsRefTimeReader.FieldExists(*fieldIdPtr))
        			{
        			ERR_PRINTF2(_L("Field %d was cleared but still exists"),*fieldIdPtr);
    				SetTestStepResult(EFail);
    				User::Leave(KErrNotFound);	
        			}
			}
			
		// Before leaving the step, set the data again
		//
		gpsReferenceTimeBuilder.GetFieldBuilder(TUEPositioningGpsReferenceTime::ERrlpGpsReferenceTime, rrlpGpsRefTimeBuilder);
		rrlpGpsRefTimeBuilder.SetField(*fieldIdPtr, *intValuePtr);	
		}
	  return TestStepResult();
	}



TVerdict CGpsRefTimeRrlpGpsTimeParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
