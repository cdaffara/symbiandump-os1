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
 @file GpsRefTimeParamsStep.cpp
*/
#include "GpsRefTimeParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

CGpsRefTimeParamsStep::~CGpsRefTimeParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeParamsStep::CGpsRefTimeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeParamsStep);
	}

TVerdict CGpsRefTimeParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsRefTimeParamsStep::doTestStepL()
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

    	TUEPositioningGpsReferenceTime::TFieldId* fieldIdPtr = reinterpret_cast<TUEPositioningGpsReferenceTime:: TFieldId*>(&fieldNumber);
    	
    	// The type of the value dependens on the filed Id
    	//
    	TUint* intValuePtr = NULL;
    	TSfnTowUncertainty* utranGpsRefTimeValuePtr = NULL;
    	TUtranGpsDriftRate* utranGpsDriftRateValuePtr = NULL;
    	
    	switch(fieldNumber)
    		{
    		case 0: // TUEPositioningGpsReferenceTime::EGpsWeek takes  unsigned integers
    		case 1: // TUEPositioningGpsReferenceTime::EGpsTow1Msec takes  unsigned integers
    		case 6: // TUEPositioningGpsReferenceTime::EBcchCarrier takes  unsigned integers
            case 7: // TUEPositioningGpsReferenceTime::EBsic takes  unsigned integers
            case 8: // TUEPositioningGpsReferenceTime::EFrameNumber takes  unsigned integers
            case 9: // TUEPositioningGpsReferenceTime::ETimeslotsNumber takes  unsigned integers
            case 10: // TUEPositioningGpsReferenceTime::EBitsNumber takes  unsigned integers
    			intValuePtr = reinterpret_cast<TUint*>(&value);
    			gpsReferenceTimeBuilder.SetField(*fieldIdPtr, *intValuePtr);
    			break;
    		case 3: // TUEPositioningGpsReferenceTime::ESfnTowUncertainty takes TSfnTowUncertainty
    		    utranGpsRefTimeValuePtr = reinterpret_cast<TSfnTowUncertainty*>(&value);
    			gpsReferenceTimeBuilder.SetField(*fieldIdPtr, *utranGpsRefTimeValuePtr);
    			break;
    	    case 4: // TUEPositioningGpsReferenceTime::EUtranGpsDriftRate takes TUtranGpsDriftRate
                utranGpsDriftRateValuePtr = reinterpret_cast<TUtranGpsDriftRate*>(&value);
                gpsReferenceTimeBuilder.SetField(*fieldIdPtr, *utranGpsDriftRateValuePtr);
			    break;
    		default:
    		    ERR_PRINTF1(_L("Config file provides a field Id that is not tested in this step"));
     	  	    SetTestStepResult(EFail);
     	  	    // Leave if there's any error.
			    User::Leave(KErrNotFound);
			    break;
    		}
    		
		// Copy data to the reader and check that the fields just set can be read
		//
		gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
		
		if (!gpsReferenceTimeReader.FieldExists(*fieldIdPtr))
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
    			case 0: // TUEPositioningGpsReferenceTime::EGpsWeek takes  unsigned integers
    			case 1: // TUEPositioningGpsReferenceTime::EGpsTow1Msec takes  unsigned integers
                case 6: // TUEPositioningGpsReferenceTime::EBcchCarrier takes  unsigned integers
                case 7: // TUEPositioningGpsReferenceTime::EBsic takes  unsigned integers
                case 8: // TUEPositioningGpsReferenceTime::EFrameNumber takes  unsigned integers
                case 9: // TUEPositioningGpsReferenceTime::ETimeslotsNumber takes  unsigned integers
                case 10: // TUEPositioningGpsReferenceTime::EBitsNumber takes  unsigned integers
    		    	TUint readValue;
					gpsReferenceTimeReader.GetField(*fieldIdPtr,readValue);
					// Is this the value that was set?
					res = (readValue == *intValuePtr);
    				break;
    		 	case 3: // TUEPositioningGpsReferenceTime::ESfnTowUncertainty takes TSfnTowUncertainty
    				TSfnTowUncertainty readGpsRefTimeValue;
    				gpsReferenceTimeReader.GetField(*fieldIdPtr, readGpsRefTimeValue);
    				res = (readGpsRefTimeValue == *utranGpsRefTimeValuePtr);
    				break;
    			case 4: // TUEPositioningGpsReferenceTime::EUtranGpsDriftRate takes TUtranGpsDriftRate
    				TUtranGpsDriftRate readUtranGspDriftValue;
    				gpsReferenceTimeReader.GetField(*fieldIdPtr, readUtranGspDriftValue);
    				res = (readUtranGspDriftValue == *utranGpsDriftRateValuePtr);
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



TVerdict CGpsRefTimeParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
