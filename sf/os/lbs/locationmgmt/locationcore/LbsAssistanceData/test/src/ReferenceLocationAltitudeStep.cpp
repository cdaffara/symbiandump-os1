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
 @file ReferenceLocationAltitudeStep.cpp
*/
#include "ReferenceLocationAltitudeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iEllipsoidPointAltBuilder iServer.iEllipsoidPointAltBuilder
#define iEllipsoidPointAltReader iServer.iEllipsoidPointAltReader
#define iReferenceLocationBuilder iServer.iReferenceLocationBuilder
#define iReferenceLocationReader iServer.iReferenceLocationReader

CReferenceLocationAltitudeStep::~CReferenceLocationAltitudeStep()
/**
 * Destructor
 */
	{
	}

CReferenceLocationAltitudeStep::CReferenceLocationAltitudeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KReferenceLocationAltitudeStep);
	}

TVerdict CReferenceLocationAltitudeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CReferenceLocationAltitudeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{

        iReferenceLocationBuilder.GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, iEllipsoidPointAltBuilder);
        
        //Set some field to some value to get the builder to register the iEllipsoidPointAltBuilder
        //
        TUint value = 11 ;
        iEllipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitude, value);
        
        TInt value2 = 22;
        iEllipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELongitude, value2);
        
        // Copy the data to the reader and check if the field exists
        iReferenceLocationReader.DataBuffer() = iReferenceLocationBuilder.DataBuffer();
        
        if(!iReferenceLocationReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
        	{
        	ERR_PRINTF1(_L("Field EEllipsoidPointAltitudeEllipsoide didn't exist"));
     	  	SetTestStepResult(EFail);
     	  	// Leave if there's any error.
			User::Leave(KErrNotFound);	
        	}
        	
        // Clear EEllipsoidPointAltitudeEllipsoide from the builder, copy data to the reader
        // and verify the field no longer exists
        //
        iReferenceLocationBuilder.ClearField(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide);

        iReferenceLocationReader.DataBuffer() = iReferenceLocationBuilder.DataBuffer();
		if(iReferenceLocationReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
        	{
        	ERR_PRINTF1(_L("Field EEllipsoidPointAltitudeEllipsoide exists after clearing it"));
     	  	SetTestStepResult(EFail);
        	}
		}
	  return TestStepResult();
	}



TVerdict CReferenceLocationAltitudeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
