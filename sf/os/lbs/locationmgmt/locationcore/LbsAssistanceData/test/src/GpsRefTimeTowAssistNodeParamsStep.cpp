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
 @file GpsRefTimeTowAssistNodeParamsStep.cpp
*/
#include "GpsRefTimeTowAssistNodeParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

#define gpsTowAssistArrayBuilder iServer.iGpsTowAssistArrayBuilder
#define gpsTowAssistArrayReader iServer.iGpsTowAssistArrayReader

#define gpsTowAssistBuilder iServer.iGpsTowAssistBuilder
#define gpsTowAssistReader iServer.iGpsTowAssistReader

#define arrayIndex iServer.iArrayIndex
CGpsRefTimeTowAssistNodeParamsStep::~CGpsRefTimeTowAssistNodeParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeTowAssistNodeParamsStep::CGpsRefTimeTowAssistNodeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeTowAssistNodeParamsStep);
	}

TVerdict CGpsRefTimeTowAssistNodeParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
   	if (arrayIndex > 31)
    	{
    	ERR_PRINTF1(_L("Preamble: processing a node too many"));
    	SetTestStepResult(EFail);
    	}

	// A previous step may have built the ArrayBuilder and the node. If that is not the case,
	// then get them.
	//
   	gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
   	TBool exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray);
   	if(!exists)
		{
		gpsReferenceTimeBuilder.GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, gpsTowAssistArrayBuilder);
		//Get a builder for the node at current index
   		//
   		gpsTowAssistArrayBuilder.GetFieldBuilder(arrayIndex, gpsTowAssistBuilder);
   		}
   	else if(!gpsTowAssistArrayBuilder.ElementExists(arrayIndex))
   		{
   		// Array exists buy node does not. Get the node only
   		gpsTowAssistArrayBuilder.GetFieldBuilder(arrayIndex, gpsTowAssistBuilder);
   		}
	
	return TestStepResult();
	}


TVerdict CGpsRefTimeTowAssistNodeParamsStep::doTestStepL()
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

    	TGpsTowAssist::TFieldId* fieldIdPtr = reinterpret_cast<TGpsTowAssist:: TFieldId*>(&fieldNumber);

    	// The type of the value dependens on the filed Id
    	//
    	TUint* intValuePtr = NULL;
    	TBool BooleanValue = EFalse;
    	switch(fieldNumber)
    		{
    		case 0: // TGpsTowAssist::ESatID takes unsigned integers
    		case 1: // TGpsTowAssist::ETlmMessage takes  unsigned integers
    		case 4: // TGpsTowAssist::ETlmReserved takes  unsigned integers
    			intValuePtr = reinterpret_cast<TUint*>(&value);
    			gpsTowAssistBuilder.SetField(*fieldIdPtr, *intValuePtr);
    			break;
    		case 2: // TGpsTowAssist::EDoppler0thOrder takes boolean
    		case 3: // TGpsTowAssist::EAntiSpoof takes boolean
    			BooleanValue = ETrue;
  				gpsTowAssistBuilder.SetField(*fieldIdPtr, BooleanValue);
  				break;
    		default:
    		    ERR_PRINTF1(_L("Config file provides a field Id that is not tested in this step"));
     	  	    SetTestStepResult(EFail);
     	  	    // Leave if there's any error.
			    User::Leave(KErrNotFound);
			    break;
    		}
    		
		// Copy data to the reader and check that the field just set can be read
		//
		gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
    	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EGpsTowAssistArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		err = gpsReferenceTimeReader.GetArrayReader(TUEPositioningGpsReferenceTime::EGpsTowAssistArray,gpsTowAssistArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get gpsTowAssistArrayReader"));
    		SetTestStepResult(EFail);
    		}
		
		// Get node reader at index
        if (gpsTowAssistArrayReader.ElementExists(arrayIndex))
        	{
			err = gpsTowAssistArrayReader.GetFieldReader(arrayIndex,gpsTowAssistReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in gpsTowAssistArrayReader"),arrayIndex);
    			SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Node does not exist at index %D"),arrayIndex);
    		SetTestStepResult(EFail);				
			}
			
		// Check that the field Id has been set with the correct value
		if (!gpsTowAssistReader.FieldExists(*fieldIdPtr))
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
     			case 0: // TGpsTowAssist::ESatID takes unsigned integers
    			case 1: // TGpsTowAssist::ETlmMessage takes  unsigned integers
        		case 4: // TGpsTowAssist::ETlmReserved takes  unsigned integers
    		    	TUint intValue;
					gpsTowAssistReader.GetField(*fieldIdPtr,intValue);
					// Is this the value that was set?
					res = (intValue == *intValuePtr);
    				break;
    			case 2: // TGpsTowAssist::EDoppler0thOrder takes boolean
    			case 3: // TGpsTowAssist::EAntiSpoof takes boolean
    				TBool readValue;
    				gpsTowAssistReader.GetField(*fieldIdPtr,readValue);
    				// readValue should be true (ETrue set above)
    				res = (readValue == BooleanValue);
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



TVerdict CGpsRefTimeTowAssistNodeParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
