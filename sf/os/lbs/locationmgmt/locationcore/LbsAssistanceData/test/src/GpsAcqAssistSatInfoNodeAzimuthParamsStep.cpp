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
 @file GpsAcqAssistSatInfoNodeAzimuthParamsStep.cpp
*/
#include "GpsAcqAssistSatInfoNodeAzimuthParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquisitionSatInfoArrayBuilder iServer.iAcquisitionSatInfoArrayBuilder

#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader
#define acquisitionSatInfoArrayReader iServer.iAcquisitionSatInfoArrayReader

#define acquisitionSatInfoBuilder iServer.iAcquisitionSatInfoBuilder
#define acquisitionSatInfoReader iServer.iAcquisitionSatInfoReader

#define azimuthAndElevationBuilder iServer.iAzimuthAndElevationBuilder
#define azimuthAndElevationReader iServer.iAzimuthAndElevationReader

#define arrayIndex iServer.iArrayIndex

CGpsAcqAssistSatInfoNodeAzimuthParamsStep::~CGpsAcqAssistSatInfoNodeAzimuthParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistSatInfoNodeAzimuthParamsStep::CGpsAcqAssistSatInfoNodeAzimuthParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistSatInfoNodeAzimuthParamsStep);
	}

TVerdict CGpsAcqAssistSatInfoNodeAzimuthParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistSatInfoNodeAzimuthParamsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
        // use the array node built in previous test step and use it to build a TAzimuthAndElevation property
        //
        acquisitionSatInfoBuilder.GetFieldBuilder(TAcquisitionSatInfo::EAzimuthAndElevation, azimuthAndElevationBuilder);
        
        // Set some data in the fields of the TAzimuthAndElevation property
        TUint azimuthValue = 22 + arrayIndex; // random value, different for each node 
        TUint elevationValue = 1117 + arrayIndex; // random value, different for each node 
        azimuthAndElevationBuilder.SetField(TAzimuthAndElevation::EAzimuth,azimuthValue);
        azimuthAndElevationBuilder.SetField(TAzimuthAndElevation::EElevation,elevationValue);
        
        // Copy data to the reader and check that it is readible and correct
        //
        acquistionAssistanceRootReader.DataBuffer()=acquistionAssistanceRootBuilder.DataBuffer();
        
        TBool exists = acquistionAssistanceRootReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray);
    	if(!exists)
			{
			ERR_PRINTF1(_L("ESatelliteInformationArray doesn't exist"));
    		SetTestStepResult(EFail);
    		}
			
	    //Array reader
		TInt err = acquistionAssistanceRootReader.GetArrayReader(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray,acquisitionSatInfoArrayReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get acquisitionSatInfoArrayReader"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		}
		
		// Get node reader at index
        if (acquisitionSatInfoArrayReader.ElementExists(arrayIndex))
        	{
			err = acquisitionSatInfoArrayReader.GetFieldReader(arrayIndex,acquisitionSatInfoReader);
			if(err != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get node %D in acquisitionSatInfoArrayReader"),arrayIndex);
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Node does not exist at index %D"),arrayIndex);
    		SetTestStepResult(EFail);				
			}
		
	    // Get Azimuth property
	    if (!acquisitionSatInfoReader.FieldExists(TAcquisitionSatInfo::EAzimuthAndElevation))
	    	{
	    	ERR_PRINTF1(_L("Failed to get node EAzimuthAndElevation property"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		
	    	}
	    else
	    	{		
	    	acquisitionSatInfoReader.GetFieldReader(TAcquisitionSatInfo::EAzimuthAndElevation, azimuthAndElevationReader);
	    	}
        
        // Check the values of the basic-type properties
        //
        if(azimuthAndElevationReader.FieldExists(TAzimuthAndElevation::EAzimuth))
        	{
        	TUint readAzimuth;
        	azimuthAndElevationReader.GetField(TAzimuthAndElevation::EAzimuth,readAzimuth);
        	if (readAzimuth != azimuthValue)
        		{
	    		ERR_PRINTF1(_L("Value set for EAzimuth is different from value read"));
    			SetTestStepResult(EFail);        		
        		}
        	}
        else
        	{
	    	ERR_PRINTF1(_L("EAzimuth doesn't exist"));
    		SetTestStepResult(EFail);        	
        	}
        	
		if(azimuthAndElevationReader.FieldExists(TAzimuthAndElevation::EElevation))
        	{
        	TUint readElevation;
        	azimuthAndElevationReader.GetField(TAzimuthAndElevation::EElevation, readElevation);
        	if (readElevation != elevationValue)
        		{
	    		ERR_PRINTF1(_L("Value set for EElevation is different from value read"));
    			SetTestStepResult(EFail);        		
        		}
        	}
        else
        	{
	    	ERR_PRINTF1(_L("EElevation doesn't exist"));
    		SetTestStepResult(EFail);        	
        	}
		}
	  return TestStepResult();
	}



TVerdict CGpsAcqAssistSatInfoNodeAzimuthParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
