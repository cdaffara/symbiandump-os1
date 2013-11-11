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
 @file ReferenceLocationAltitudeParamsStep.cpp
*/
#include "ReferenceLocationAltitudeParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iEllipsoidPointAltBuilder iServer.iEllipsoidPointAltBuilder
#define iEllipsoidPointAltReader iServer.iEllipsoidPointAltReader
#define iReferenceLocationBuilder iServer.iReferenceLocationBuilder
#define iReferenceLocationReader iServer.iReferenceLocationReader

CReferenceLocationAltitudeParamsStep::~CReferenceLocationAltitudeParamsStep()
/**
 * Destructor
 */
	{
	}

CReferenceLocationAltitudeParamsStep::CReferenceLocationAltitudeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KReferenceLocationAltitudeParamsStep);
	}

TVerdict CReferenceLocationAltitudeParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	
	// First time here there won't a TReferenceLocation::EEllipsoidPointAltitudeEllipsoide builder
	// so build it.
	iReferenceLocationReader.DataBuffer() = iReferenceLocationBuilder.DataBuffer();
	if (!iReferenceLocationReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
		{
		iReferenceLocationBuilder.GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, iEllipsoidPointAltBuilder);
		}
	return TestStepResult();
	}


TVerdict CReferenceLocationAltitudeParamsStep::doTestStepL()
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
		SetTestStepResult(EPass);
		
		TEllipsoidPointAltitudeEllipsoide::TFieldId* fieldIdPtr = reinterpret_cast<TEllipsoidPointAltitudeEllipsoide:: TFieldId*>(&fieldNumber);
		
		// The type of the value dependens on the filed Id
    	//
    	TUint* uIntValuePtr;
    	TEllipsoidPointAltitudeEllipsoide::TLatitudeSign* latitudeSignValuePtr;
    	TEllipsoidPointAltitudeEllipsoide::TAltitudeDirection* altitudDirectionValuePtr;
    	switch(fieldNumber)
    		{
    		case 0: // TEllipsoidPointAltitudeEllipsoide::TLatitudeSign takes enum TLatitudeSign
    			latitudeSignValuePtr = reinterpret_cast<TEllipsoidPointAltitudeEllipsoide::TLatitudeSign*>(&value);
    			iEllipsoidPointAltBuilder.SetField(*fieldIdPtr, *latitudeSignValuePtr);
    			break;
    		case 1: // Latitude (unsigned integer)
    		case 4: // Altitude
    		case 5: // UncertaintySemiMajor
    		case 6: // UncertaintySemiMinor
    		case 7: // OrientationMajorAxis
    		case 8: // UncertaintyAltitude
    		case 9: // EConfidence
				uIntValuePtr = reinterpret_cast<TUint*>(&value);
				iEllipsoidPointAltBuilder.SetField(*fieldIdPtr, *uIntValuePtr);
				break;
    		case 2: // Longitud is an integer
    		    iEllipsoidPointAltBuilder.SetField(*fieldIdPtr, value);
    		    break;
    		case 3: // takes enum TAltitudeDirection
    			altitudDirectionValuePtr = reinterpret_cast<TEllipsoidPointAltitudeEllipsoide::TAltitudeDirection*>(&value);
    			iEllipsoidPointAltBuilder.SetField(*fieldIdPtr, *altitudDirectionValuePtr);
    			break;
    		default:
    		    ERR_PRINTF1(_L("Step is trying to set a field Id that doesn't exist in node"));
     	  	    SetTestStepResult(EFail);
     	  	    // Leave if there's any error.
			    User::Leave(KErrNotFound);
			    break;
    		}
		}
	  return TestStepResult();
	}



TVerdict CReferenceLocationAltitudeParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
