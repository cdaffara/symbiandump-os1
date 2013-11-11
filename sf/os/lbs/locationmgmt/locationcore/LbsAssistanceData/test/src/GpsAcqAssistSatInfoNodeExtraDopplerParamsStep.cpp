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
 @file GpsAcqAssistSatInfoNodeExtraDopplerParamsStep.cpp
*/
#include "GpsAcqAssistSatInfoNodeExtraDopplerParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define acquistionAssistanceRootBuilder iServer.iAcquistionAssistanceRootBuilder
#define acquisitionSatInfoArrayBuilder iServer.iAcquisitionSatInfoArrayBuilder

#define acquistionAssistanceRootReader iServer.iAcquistionAssistanceRootReader
#define acquisitionSatInfoArrayReader iServer.iAcquisitionSatInfoArrayReader

#define acquisitionSatInfoBuilder iServer.iAcquisitionSatInfoBuilder
#define acquisitionSatInfoReader iServer.iAcquisitionSatInfoReader

#define extraDopplerInfoBuilder iServer.iExtraDopplerInfoBuilder
#define extraDopplerInfoReader iServer.iExtraDopplerInfoReader

#define arrayIndex iServer.iArrayIndex



CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep::~CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep::CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAcqAssistSatInfoNodeExtraDopplerParamsStep);
	}

TVerdict CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_finalCompleteSuiteSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
        // use the array node built in previous test step and use it to build a EExtraDopplerInfo property
        //
        acquisitionSatInfoBuilder.GetFieldBuilder(TAcquisitionSatInfo::EExtraDopplerInfo, extraDopplerInfoBuilder);
        
        // Set some data in the fields of the EExtraDopplerInfo property
        //
        TInt dopplerFirstOrderValue = 1234 + arrayIndex; // random value, different for each node 
        TDopplerUncertainty dopplerUncertaintyValue = EHz200;
        extraDopplerInfoBuilder.SetField(TExtraDopplerInfo::EDoppler1stOrder, dopplerFirstOrderValue);
        extraDopplerInfoBuilder.SetField(TExtraDopplerInfo::EDopplerUncertainty, dopplerUncertaintyValue);
        
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
		
	    // Get EExtraDopplerInfo property
	    if (!acquisitionSatInfoReader.FieldExists(TAcquisitionSatInfo::EExtraDopplerInfo))
	    	{
	    	ERR_PRINTF1(_L("Failed to get node EExtraDopplerInfo property"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		
	    	}
	    else
	    	{		
	    	acquisitionSatInfoReader.GetFieldReader(TAcquisitionSatInfo::EExtraDopplerInfo, extraDopplerInfoReader);
	    	}
        
        // Check the values of the basic-type properties
        //
        if(extraDopplerInfoReader.FieldExists(TExtraDopplerInfo::EDoppler1stOrder))
        	{
        	TInt readDopplerFirstOrderValue;
        	extraDopplerInfoReader.GetField(TExtraDopplerInfo::EDoppler1stOrder,readDopplerFirstOrderValue);
        	if (readDopplerFirstOrderValue != dopplerFirstOrderValue)
        		{
	    		ERR_PRINTF1(_L("Value set for EDoppler1stOrder is different from value read"));
    			SetTestStepResult(EFail);        		
        		}
        	}
        else
        	{
	    	ERR_PRINTF1(_L("EDoppler1stOrder doesn't exist"));
    		SetTestStepResult(EFail);        	
        	}
        	
		if(extraDopplerInfoReader.FieldExists(TExtraDopplerInfo::EDopplerUncertainty))
        	{
        	TDopplerUncertainty readDopplerUncertaintyValue;
        	extraDopplerInfoReader.GetField(TExtraDopplerInfo::EDopplerUncertainty, readDopplerUncertaintyValue);
        	if (readDopplerUncertaintyValue != dopplerUncertaintyValue)
        		{
	    		ERR_PRINTF1(_L("Value set for EDopplerUncertainty is different from value read"));
    			SetTestStepResult(EFail);        		
        		}
        	}
        else
        	{
	    	ERR_PRINTF1(_L("EDopplerUncertainty doesn't exist"));
    		SetTestStepResult(EFail);        	
        	}
		}
	  return TestStepResult();

	}



TVerdict CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
