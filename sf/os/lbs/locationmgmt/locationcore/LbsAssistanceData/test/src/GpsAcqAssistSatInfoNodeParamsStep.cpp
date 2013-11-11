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
 @file GpsAcqAssistSatInfoNodeParamsStep.cpp
*/
#include "GpsAcqAssistSatInfoNodeParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquisitionSatInfoArrayBuilder iServer.iAcquisitionSatInfoArrayBuilder

#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader
#define acquisitionSatInfoArrayReader iServer.iAcquisitionSatInfoArrayReader

#define acquisitionSatInfoBuilder iServer.iAcquisitionSatInfoBuilder
#define acquisitionSatInfoReader iServer.iAcquisitionSatInfoReader

#define arrayIndex iServer.iArrayIndex


CGpsAcqAssistSatInfoNodeParamsStep::~CGpsAcqAssistSatInfoNodeParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistSatInfoNodeParamsStep::CGpsAcqAssistSatInfoNodeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistSatInfoNodeParamsStep);
	}

TVerdict CGpsAcqAssistSatInfoNodeParamsStep::doTestStepPreambleL()
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
   	acquistionAssistanceRootReader.DataBuffer()=acquistionAssistanceRootBuilder.DataBuffer();
   	TBool exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
   	if(!exists)
		{
		acquistionAssistanceRootBuilder.GetArrayBuilder(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray, acquisitionSatInfoArrayBuilder);
		//Get a builder for the node at current index
   		//
   		acquisitionSatInfoArrayBuilder.GetFieldBuilder(arrayIndex, acquisitionSatInfoBuilder);
   		}
   	else if(!acquisitionSatInfoArrayBuilder.ElementExists(arrayIndex))
   		{
   		// Array exists buy node does not. Get the node only
   		acquisitionSatInfoArrayBuilder.GetFieldBuilder(arrayIndex, acquisitionSatInfoBuilder);
   		}
	
	return TestStepResult();
	}


TVerdict CGpsAcqAssistSatInfoNodeParamsStep::doTestStepL()
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

    	TAcquisitionSatInfo::TFieldId* fieldIdPtr = reinterpret_cast<TAcquisitionSatInfo:: TFieldId*>(&fieldNumber);
    	
    	// The type of the value dependens on the filed Id
    	//
    	TUint* intValuePtr = NULL;
    	TCodePhaseSearchWindow* codePhaseSearchValuePtr = NULL;
    	switch(fieldNumber)
    		{
    		case 0: // TAcquisitionSatInfo::ESatId takes  unsigned integers
    		case 3: // TAcquisitionSatInfo::ECodePhase takes  unsigned integers
    		case 4: // TAcquisitionSatInfo::EIntegerCodePhase
    		case 5: // TAcquisitionSatInfo::EGpsBitNumber
    			intValuePtr = reinterpret_cast<TUint*>(&value);
    			acquisitionSatInfoBuilder.SetField(*fieldIdPtr, *intValuePtr);
    			break;
    		case 1: // TAcquisitionSatInfo::EDoppler0thOrder takes integers
    			acquisitionSatInfoBuilder.SetField(*fieldIdPtr, value);
    			break;
    	    case 6: // TAcquisitionSatInfo::ECodePhaseSearchWindow takes TCodePhaseSearchWindow
                codePhaseSearchValuePtr = reinterpret_cast<TCodePhaseSearchWindow*>(&value);
                acquisitionSatInfoBuilder.SetField(*fieldIdPtr, *codePhaseSearchValuePtr);
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
		acquistionAssistanceRootReader.DataBuffer()=acquistionAssistanceRootBuilder.DataBuffer();
    	exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("ESatelliteInformationArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		err = acquistionAssistanceRootReader.GetArrayReader(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray,acquisitionSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get acquisitionSatInfoArrayReader"));
    		SetTestStepResult(EFail);
    		}
		
		// Get node reader at index
        if (acquisitionSatInfoArrayReader.ElementExists(arrayIndex))
        	{
			err = acquisitionSatInfoArrayReader.GetFieldReader(arrayIndex,acquisitionSatInfoReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in acquisitionSatInfoArrayReader"),arrayIndex);
    			SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Node does not exist at index %D"),arrayIndex);
    		SetTestStepResult(EFail);				
			}
			
		// Check that the field Id has been set with the correct value
		if (!acquisitionSatInfoReader.FieldExists(*fieldIdPtr))
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
    			case 0: // TAcquisitionSatInfo::ESatId takes  unsigned integers
    			case 3: // TAcquisitionSatInfo::ECodePhase takes  unsigned integers
    			case 4: // TAcquisitionSatInfo::EIntegerCodePhase
    			case 5: // TAcquisitionSatInfo::EGpsBitNumber
    		    	TUint intValue;
					acquisitionSatInfoReader.GetField(*fieldIdPtr,intValue);
					// Is this the value that was set?
					res = (intValue == *intValuePtr);
    				break;
    			case 1: // TAcquisitionSatInfo::EDoppler0thOrder takes integers
    				TInt readValue;
    				acquisitionSatInfoReader.GetField(*fieldIdPtr, readValue);
    				res = (readValue == value);
    			break;

    			case 6: // TAcquisitionSatInfo::ECodePhaseSearchWindow takes TCodePhaseSearchWindow
    				TCodePhaseSearchWindow codePhaseSearchValue;
    				acquisitionSatInfoReader.GetField(*fieldIdPtr, codePhaseSearchValue);
    				res = (codePhaseSearchValue == *codePhaseSearchValuePtr);
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


TVerdict CGpsAcqAssistSatInfoNodeParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
