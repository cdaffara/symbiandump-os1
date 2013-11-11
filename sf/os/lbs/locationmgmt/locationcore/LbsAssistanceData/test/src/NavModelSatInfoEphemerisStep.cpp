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
 @file NavModelSatInfoEphemerisStep.cpp
*/
#include "NavModelSatInfoEphemerisStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

CNavModelSatInfoEphemerisStep::~CNavModelSatInfoEphemerisStep()

	{
	}

CNavModelSatInfoEphemerisStep::CNavModelSatInfoEphemerisStep(CTe_LbsAssistanceDataSuite& aOwningServer):
    CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavModelSatInfoEphemerisStep);
	}

TVerdict CNavModelSatInfoEphemerisStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// An array node must exist at this point (created in previous step)
	//
	if(!iServer.iNavModelSatInfoArrayBuilder.ElementExists(iServer.iArrayIndex))
		{
		ERR_PRINTF1(_L("The array node doesn't exist"));
     	SetTestStepResult(EFail);
     	// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CNavModelSatInfoEphemerisStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    iServer.iNavModelSatInfoBuilder.GetFieldBuilder(TNavigationModelSatInfo::EEphemerisParameter, iServer.iEphemerisBuilder);	
	
	// Set some data using the ephemeris builder
	TUint someSatHealthValue = 37;
	iServer.iEphemerisBuilder.SetField(TEphemerisParameter::ESatHealth,someSatHealthValue);
	 
	// Copy data to the reader and check that the data just set can be read
	//
	iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
    TBool exists = iServer.iNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray);
    if(!exists)
		{
		ERR_PRINTF1(_L("ENavigationModelSatelliteInfoArray doesn't exist"));
    	SetTestStepResult(EFail);
    	}
    	
	// Array reader
	TInt err = iServer.iNavigationModelReader.GetArrayReader(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray,iServer.iNavModelSatInfoArrayReader);
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
	if (!iServer.iNavModelSatInfoReader.FieldExists(TNavigationModelSatInfo::EEphemerisParameter))
		{
		ERR_PRINTF1(_L("Ephemeris does not exist in node reader"));
    	SetTestStepResult(EFail);
		}
	else
		{
		iServer.iNavModelSatInfoReader.GetFieldReader(TNavigationModelSatInfo::EEphemerisParameter, iServer.iEphemerisReader);
		// Set some data in the ephemeris parameter
		if(!iServer.iEphemerisReader.FieldExists(TEphemerisParameter::ESatHealth))
			{
			ERR_PRINTF1(_L("Ephemeris Field doesn't exit in node"));
    	    SetTestStepResult(EFail);
			}
		}

	  return TestStepResult();
	}



TVerdict CNavModelSatInfoEphemerisStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
    // Clear the Ephemeris field so the value set above for SatHealth
    // doesn't interfere with future test steps and check that the 
    // ClearField method works.
    //
    iServer.iNavModelSatInfoBuilder.ClearField(TNavigationModelSatInfo::EEphemerisParameter);
    
    // Check that Ephemeris was cleared OK
    //
    iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
    iServer.iNavigationModelReader.GetArrayReader(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray,iServer.iNavModelSatInfoArrayReader);
    iServer.iNavModelSatInfoArrayReader.GetFieldReader(iServer.iArrayIndex,iServer.iNavModelSatInfoReader);
    if (iServer.iNavModelSatInfoReader.FieldExists(TNavigationModelSatInfo::EEphemerisParameter))
		{
		ERR_PRINTF1(_L("Failed to clear ephemeris"));
    	SetTestStepResult(EFail);
		}
	return TestStepResult();
	}
