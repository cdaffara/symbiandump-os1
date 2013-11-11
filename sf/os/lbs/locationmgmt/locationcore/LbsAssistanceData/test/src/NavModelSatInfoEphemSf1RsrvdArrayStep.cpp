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
 @file NavModelSatInfoEphemSf1RsrvdArrayStep.cpp
*/
#include "NavModelSatInfoEphemSf1RsrvdArrayStep.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  sf1RsrvdArrayBuilder iServer.iSf1RsrvdArrayBuilder

#define  sf1RsrvdArrayReader iServer.iSf1RsrvdArrayReader


CNavModelSatInfoEphemSf1RsrvdArrayStep::~CNavModelSatInfoEphemSf1RsrvdArrayStep()
/**
 * Destructor
 */
	{
	}

CNavModelSatInfoEphemSf1RsrvdArrayStep::CNavModelSatInfoEphemSf1RsrvdArrayStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNavModelSatInfoEphemSf1RsrvdArrayStep);
	}

TVerdict CNavModelSatInfoEphemSf1RsrvdArrayStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CNavModelSatInfoEphemSf1RsrvdArrayStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
	    iServer.iNavigationModelBuilder.GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray,iServer.iNavModelSatInfoArrayBuilder);
	    iServer.iNavModelSatInfoArrayBuilder.GetFieldBuilder(iServer.iArrayIndex,iServer.iNavModelSatInfoBuilder);
	    iServer.iNavModelSatInfoBuilder.GetFieldBuilder(TNavigationModelSatInfo::EEphemerisParameter, iServer.iEphemerisBuilder);
	    iServer.iEphemerisBuilder.GetArrayBuilder(TEphemerisParameter::ESf1Revd, sf1RsrvdArrayBuilder);
		
		// Set some data in the array
		//
		for (TInt ii = 0; ii<4; ii++)
			{
			sf1RsrvdArrayBuilder.SetElement(ii, ii);
			}

	    // Check if a reader can access the array
    	//
	    iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
	    iServer.iNavigationModelReader.GetArrayReader(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray,iServer.iNavModelSatInfoArrayReader);
	    iServer.iNavModelSatInfoArrayReader.GetFieldReader(iServer.iArrayIndex,iServer.iNavModelSatInfoReader);
	    iServer.iNavModelSatInfoReader.GetFieldReader(TNavigationModelSatInfo::EEphemerisParameter, iServer.iEphemerisReader);
	    if(!iServer.iEphemerisReader.FieldExists(TEphemerisParameter::ESf1Revd))
    		{
			ERR_PRINTF1(_L("TEphemerisParameter::ESf1Revd field didn't exist"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound); 		
    		}
    		
	    iServer.iEphemerisReader.GetArrayReader(TEphemerisParameter::ESf1Revd, sf1RsrvdArrayReader);

        // Get each element, check its value and clear it from the array.
        //
		for (TInt ii = 0; ii<4; ii++)
			{
			if(!sf1RsrvdArrayReader.ElementExists(ii))
				{
				ERR_PRINTF2(_L("Missing element %d"), ii);
    			SetTestStepResult(EFail);	
				}
			else
				{
				TUint readValue;
				sf1RsrvdArrayReader.GetElement(ii, readValue);
				if (readValue != ii)
					{
					ERR_PRINTF2(_L("Incorrect value for element number %d"), ii);
    				SetTestStepResult(EFail);						
					}
					
				// Clear this element from the array
				//
				sf1RsrvdArrayBuilder.ClearElement(ii);
				
				// Get all data from builder to reader again
				// after deleting each element.
				//
		        iServer.iNavigationModelReader.DataBuffer()=iServer.iNavigationModelBuilder.DataBuffer();
		        iServer.iNavigationModelReader.GetArrayReader(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray,iServer.iNavModelSatInfoArrayReader);
		        iServer.iNavModelSatInfoArrayReader.GetFieldReader(iServer.iArrayIndex,iServer.iNavModelSatInfoReader);
		        iServer.iNavModelSatInfoReader.GetFieldReader(TNavigationModelSatInfo::EEphemerisParameter, iServer.iEphemerisReader);
		        if(!iServer.iEphemerisReader.FieldExists(TEphemerisParameter::ESf1Revd))
    				{
    				if (ii != 3)
    					{
						ERR_PRINTF1(_L("TEphemerisParameter::ESf1Revd field didn't exist"));
    					SetTestStepResult(EFail);
    					User::Leave(KErrNotFound); 		
    					}
    				else
    				    {
    					INFO_PRINTF1(_L("Array has been correctly cleared after having deleted all the elements"));
    					break;
    				    }
    				}
    			else
    				{
    				iServer.iEphemerisReader.GetArrayReader(TEphemerisParameter::ESf1Revd, sf1RsrvdArrayReader);
    				}
				}
			}
		}

	  return TestStepResult();
	}



TVerdict CNavModelSatInfoEphemSf1RsrvdArrayStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
