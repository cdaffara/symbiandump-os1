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
 @file ReferenceLocationFinalCheckAndCleanupStep.cpp
*/
#include "Te_LbsAssistanceDataSuiteServer.h"
#include "ReferenceLocationFinalCheckAndCleanupStep.h"

#define iEllipsoidPointAltBuilder iServer.iEllipsoidPointAltBuilder
#define iEllipsoidPointAltReader iServer.iEllipsoidPointAltReader
#define iReferenceLocationBuilder iServer.iReferenceLocationBuilder
#define iReferenceLocationReader iServer.iReferenceLocationReader

CReferenceLocationFinalCheckAndCleanupStep::~CReferenceLocationFinalCheckAndCleanupStep()
/**
 * Destructor
 */
	{
	}

CReferenceLocationFinalCheckAndCleanupStep::CReferenceLocationFinalCheckAndCleanupStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KReferenceLocationFinalCheckAndCleanupStep);
	}

TVerdict CReferenceLocationFinalCheckAndCleanupStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	
	// Previous teststep ReferenceLocationAltitudeParamsStep
	// has left valid builders place and have built the data.
	// Get the readers
    iReferenceLocationReader.DataBuffer() = iReferenceLocationBuilder.DataBuffer();
	iReferenceLocationReader.GetFieldReader(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide,iEllipsoidPointAltReader);
	
   	return TestStepResult();
	}

// Perform some random value checks
//
//
TVerdict CReferenceLocationFinalCheckAndCleanupStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{

    // Check that all the fieds exists and their values are correct
    TEllipsoidPointAltitudeEllipsoide::TFieldId* fieldIdPtr;
    TUint value;

    // From EA1 to EDeltaLSF there are 10 field ids
    // TEllipsoidPointAltitudeEllipsoide::ELatitudeSign
    for (TUint ii = 9; ii+1 > 0 ; ii--)
    	{
    	
    	fieldIdPtr = reinterpret_cast<TEllipsoidPointAltitudeEllipsoide::TFieldId*>(&ii);
    	
    	if(!iEllipsoidPointAltReader.FieldExists(*fieldIdPtr))
    		{
    		ERR_PRINTF2(_L("Missing field number %D."),ii);
			SetTestStepResult(EFail);
    		}
    	else
    		{
    		// Checking the value. Skip value checking for fields 0,2 and 3 that
    		// don't take unsigned integers and would make things even messier.
    		//
    		if ((ii!=0) && (ii!=2) && (ii!=3))
 	   			{
    			iEllipsoidPointAltReader.GetField(*fieldIdPtr,value);
    		
    			if(value != (ii+1)) //in the ini file the value is set like this (fieldId +1)
    				{
    				ERR_PRINTF2(_L("Wrong value in field number %D ."),ii);
					SetTestStepResult(EFail);
    				}
 	   			}
  
    		// Clear this field from the builder and get the data
    		// again back in the reader.
    		//
    		iEllipsoidPointAltBuilder.ClearField(*fieldIdPtr);
    		iReferenceLocationReader.DataBuffer() = iReferenceLocationBuilder.DataBuffer();
			iReferenceLocationReader.GetFieldReader(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, iEllipsoidPointAltReader);

			// Check that this field no longer exists
			//
 			if(iEllipsoidPointAltReader.FieldExists(*fieldIdPtr))
    			{
    			ERR_PRINTF2(_L("Field number %D was cleared but still exists."),ii);
				SetTestStepResult(EFail);
    			}				
 	   		}
    	}
    	
	return TestStepResult();
	}


TVerdict CReferenceLocationFinalCheckAndCleanupStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Close the GpsUtcigation Model property to prevent 
	// TEF warnings due to memory leakage.
	iReferenceLocationBuilder.Close();
	iReferenceLocationReader.Close();
	return TestStepResult();
	}
