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
 @file NavModelSatInfoEphemParameterStep.cpp
*/
#include "NavModelSatInfoEphemParameterStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CNavModelSatInfoEphemParameterStep::~CNavModelSatInfoEphemParameterStep()
/**
 * Destructor
 */
	{
	}

CNavModelSatInfoEphemParameterStep::CNavModelSatInfoEphemParameterStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavModelSatInfoEphemParameterStep);
	}

TVerdict CNavModelSatInfoEphemParameterStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);

    // First time here Ephemeris may not exist in the node. Make sure to
    // create it before starting the step
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
   		// Array exists buy node does not. Get the node:
   		iServer.iNavModelSatInfoArrayBuilder.GetFieldBuilder(iServer.iArrayIndex, iServer.iNavModelSatInfoBuilder);
   		}
   	
   	// Now that the node is guaranteed to exist, check the Ephemeris:
	if (!iServer.iNavModelSatInfoBuilder.FieldExists(TNavigationModelSatInfo::EEphemerisParameter))
		{
		iServer.iNavModelSatInfoBuilder.GetFieldBuilder(TNavigationModelSatInfo::EEphemerisParameter,iServer.iEphemerisBuilder);
		}
		
	return TestStepResult();
	}

TVerdict CNavModelSatInfoEphemParameterStep::doTestStepL()
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
        
        // In order to set different data for each array node, increase the "value" variable
        // using the array index
        value += (10*iServer.iArrayIndex);
        
        // These field ids are CodeOnL2, UraIndex, SatHealth, Iodc, L3Pflag, iTGD,...
    	TEphemerisParameter::TFieldId* fieldIdPtr = reinterpret_cast<TEphemerisParameter::TFieldId*>(&fieldNumber);
    	
    	TUint* intValuePtr;
		intValuePtr = reinterpret_cast<TUint*>(&value);
    	iServer.iEphemerisBuilder.SetField(*fieldIdPtr, *intValuePtr);
    		
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

		// Get ephemeris reader from the node
		iServer.iNavModelSatInfoReader.GetFieldReader(TNavigationModelSatInfo::EEphemerisParameter, iServer.iEphemerisReader);
		
		if(!iServer.iEphemerisReader.FieldExists(*fieldIdPtr))
			{
			ERR_PRINTF2(_L("Ephemeris field %D doesn't exist"),fieldNumber);
    		SetTestStepResult(EFail);	
			}
		else
			{
			TUint fieldValue;
			iServer.iEphemerisReader.GetField(*fieldIdPtr,fieldValue);
			if(fieldValue != value)
				{
				ERR_PRINTF2(_L("Value set different from value got for field number %D of Ephemeris"),fieldNumber);
    		    SetTestStepResult(EFail);
				}
			}
		}
	return TestStepResult();
	}

TVerdict CNavModelSatInfoEphemParameterStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
