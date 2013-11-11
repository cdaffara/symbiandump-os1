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
 @file GpsAlmanacParamsStep.cpp
*/
#include "GpsAlmanacParamsStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define iGpsAlmanacBuilder iServer.iGpsAlmanacBuilder
#define iAlmanacSatInfoArrayBuilder iServer.iAlmanacSatInfoArrayBuilder
#define iAlmanacSatInfoBuilder iServer.iAlmanacSatInfoBuilder

#define iGpsAlmanacReader iServer.iGpsAlmanacReader
#define iAlmanacSatInfoArrayReader iServer.iAlmanacSatInfoArrayReader
#define iAlmanacSatInfoReader iSever.iAlmanacSatInfoReader

CGpsAlmanacParamsStep::~CGpsAlmanacParamsStep()
/**
 * Destructor
 */
	{
	}

CGpsAlmanacParamsStep::CGpsAlmanacParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGpsAlmanacParamsStep);
	}

TVerdict CGpsAlmanacParamsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGpsAlmanacParamsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
        TUint8 globalHealthTestData[46] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                           0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
                                           0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                           0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
                                           0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                           0x07, 0x08, 0x09, 0x0A};
        TUint wnaTestData = 0x1234ABCD;
        TBuf8<46> buffer;
        buffer.Append(&globalHealthTestData[0],sizeof(globalHealthTestData));

        
        iGpsAlmanacBuilder.SetField(TUEPositioningGpsAlmanac::EWnA, wnaTestData);
        iGpsAlmanacBuilder.SetField(TUEPositioningGpsAlmanac::ESvGlobalHealth, buffer);
        
        // Copy data to the reader and check that it can be read correctly and cleared
        //
        iGpsAlmanacReader.DataBuffer() = iGpsAlmanacBuilder.DataBuffer();
        if(iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EWnA))
        	{
        	    TUint readWna;
        		iGpsAlmanacReader.GetField(TUEPositioningGpsAlmanac::EWnA, readWna);
        		if(readWna != wnaTestData)
        			{
        			ERR_PRINTF1(_L("Incorrect data read from field EWnA"));
					SetTestStepResult(EFail);
        			}
        		iGpsAlmanacBuilder.ClearField(TUEPositioningGpsAlmanac::EWnA);
        	}
        else
        	{
        	ERR_PRINTF1(_L("Missing Field EWnA"));
     	  	SetTestStepResult(EFail);
        	}
        	
        if(iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::ESvGlobalHealth))
        	{
        	    TBuf8<46> readBuffer;
        		iGpsAlmanacReader.GetField(TUEPositioningGpsAlmanac::ESvGlobalHealth,readBuffer);
        		if(readBuffer != buffer)
        			{
        			ERR_PRINTF1(_L("Incorrect data read from field ESvGlobalHealth"));
					SetTestStepResult(EFail);
        			}
        		iGpsAlmanacBuilder.ClearField(TUEPositioningGpsAlmanac::ESvGlobalHealth);
        	}
        else
        	{
        	ERR_PRINTF1(_L("Missing Field EWnA"));
     	  	SetTestStepResult(EFail);
        	}
         
         
        // Copy data again to the reader and check it's been cleared
        //
        iGpsAlmanacReader.DataBuffer() = iGpsAlmanacBuilder.DataBuffer();
        if(iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EWnA))
        	{
        	ERR_PRINTF1(_L("Field EWnA exits after clearing it"));
			SetTestStepResult(EFail);
        	}
        if(iGpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::ESvGlobalHealth))
        	{
        	ERR_PRINTF1(_L("Field ESvGlobalHealth exits after clearing it"));
			SetTestStepResult(EFail);
        	}
		}
	  return TestStepResult();
	}



TVerdict CGpsAlmanacParamsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
