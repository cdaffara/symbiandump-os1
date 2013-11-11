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
 @file GpsRefTimeUtranGpsTimeCpichStep.cpp
*/
#include "GpsRefTimeUtranGpsTimeCpichStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  gpsReferenceTimeBuilder iServer.iGpsReferenceTimeBuilder
#define  gpsReferenceTimeReader iServer.iGpsReferenceTimeReader

#define	utranGpsRefTimeBuilder iServer.iUtranGpsRefTimeBuilder
#define primaryCpichInfoBuilder iServer.iPrimaryCpichInfoBuilder
	
#define	utranGpsRefTimeReader iServer.iUtranGpsRefTimeReader
#define	primaryCpichInfoReader iServer.iPrimaryCpichInfoReader

CGpsRefTimeUtranGpsTimeCpichStep::~CGpsRefTimeUtranGpsTimeCpichStep()
/**
 * Destructor
 */
	{
	}

CGpsRefTimeUtranGpsTimeCpichStep::CGpsRefTimeUtranGpsTimeCpichStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsRefTimeUtranGpsTimeCpichStep);
	}

TVerdict CGpsRefTimeUtranGpsTimeCpichStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsRefTimeUtranGpsTimeCpichStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt err;
      	TUint value = 123; 
      	
		// Check that the field is initially empty
        gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
        if (!gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime))
        	{
        	// It should exist if previous test steps have been run
        	ERR_PRINTF1(_L("EUtranGpsReferenceTime doesn't exist"));
        	SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
        	}
		else
			{
	        //EUtranGpsReferenceTime reader
			err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsRefTimeReader);
			if(err != KErrNone)
    			{
    			ERR_PRINTF1(_L("Failed to get EUtranGpsReferenceTime"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
    			}
		    else
		    	{
		    	// Check that the field Id EModeSpecificInfo_Fdd_ReferenceIdentity doesn't exist yet
				//
        		if (utranGpsRefTimeReader.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity))
        			{
        			ERR_PRINTF1(_L("Field EModeSpecificInfo_Fdd_ReferenceIdentity exists before it's been set"));
    			    SetTestStepResult(EFail);
    			    User::Leave(KErrNotFound);
        			}
		    	}
			}
			
		// Get the builder for the CpichInfoReader and set some data in it
        utranGpsRefTimeBuilder.GetFieldBuilder(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity,primaryCpichInfoBuilder);
        
        primaryCpichInfoBuilder.SetField(TPrimaryCpichInfo::EPrimaryScramblingCode,value);
        
        // Copy data to the reader and check that the field just set can be read
		//
		gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
    	TBool exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime);
    	if(!exists)
			{
			ERR_PRINTF1(_L("EUtranGpsReferenceTime doesn't exist"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		}
			
	    //EUtranGpsReferenceTime reader
		err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsRefTimeReader);
		if(err != KErrNone)
    		{
    		ERR_PRINTF1(_L("Failed to get EUtranGpsReferenceTime"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);
    		}
		
		// Check that the field Id set above can be read
		// and the value is correct
		//
        if (utranGpsRefTimeReader.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity))
        	{
        	TUint readValue;
        	err = utranGpsRefTimeReader.GetFieldReader(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity,primaryCpichInfoReader);
        	if (err != KErrNone)
        		{
    			ERR_PRINTF1(_L("Failed to get EModeSpecificInfo_Fdd_ReferenceIdentity"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);        		
        		}
        	
			err = primaryCpichInfoReader.GetField(TPrimaryCpichInfo::EPrimaryScramblingCode,readValue);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Failed to get element EPrimaryScramblingCode"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
				}
			else
				{
				// Get the value
				if (value != readValue)
					{
					ERR_PRINTF1(_L("Data set in the builder is different from data read"));
     	  	    	SetTestStepResult(EFail);
     	  	    	User::Leave(KErrNotFound);
					}
				}
			}
		else
			{
			ERR_PRINTF1(_L("Field EModeSpecificInfo_Fdd_ReferenceIdentity does not exist"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound);				
			}
			
			
		// Clear the element and check that it is correctly cleared
		//
		primaryCpichInfoBuilder.ClearField(TPrimaryCpichInfo::EPrimaryScramblingCode);
		
		// Verify element has been cleared
		//
		gpsReferenceTimeReader.DataBuffer()=gpsReferenceTimeBuilder.DataBuffer();
    	exists = gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime);
    	if(!exists)
			{
			INFO_PRINTF1(_L("EUtranGpsReferenceTime doesn't exist (correct the first time in here)"));
    		}
		else
			{
			//EUtranGpsReferenceTime reader
			err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsRefTimeReader);
			if(err != KErrNone)
    			{
    			ERR_PRINTF1(_L("Failed to get EUtranGpsReferenceTime"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);
    			}
    		if (utranGpsRefTimeBuilder.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity))
    			{
    			ERR_PRINTF1(_L("Field was cleared but still exits"));
    			SetTestStepResult(EFail);
    			User::Leave(KErrNotFound);        		
        		}
			}
			
		// Before leaving the step, set the data again
		//
		utranGpsRefTimeBuilder.GetFieldBuilder(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity,primaryCpichInfoBuilder);
		primaryCpichInfoBuilder.SetField(TPrimaryCpichInfo::EPrimaryScramblingCode,value);

		}
	  return TestStepResult();
	}



TVerdict CGpsRefTimeUtranGpsTimeCpichStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
