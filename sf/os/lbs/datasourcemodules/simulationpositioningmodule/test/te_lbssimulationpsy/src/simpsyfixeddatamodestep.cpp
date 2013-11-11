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
//

/**
 @file SimPsyFixedDataModeStep.cpp
 @internalTechnology
*/
#include "simpsyfixeddatamodestep.h"
#include "te_lbssimulationpsysuitedefs.h"



// A few useful macros
#define MAX(x,y)      ((x)<(y))?(y):(x)
#define MIN(x,y)      ((x)<(y))?(x):(y)

GLREF_C TPositionModuleStatus ModuleStatus();
GLREF_C TPositionModuleStatus PrevModuleStatus();

// CONSTANTS
_LIT(KSimEmptyFile, "");

const TInt32 KSimModuleId = 270498433;

CSimPsyFixedDataModeStep::~CSimPsyFixedDataModeStep()
/**
 * Destructor
 */
	{
    delete iSimDataArray; 
    iSimDataArray = NULL;

#ifdef __CTC__ 
    #pragma CTC APPEND
#endif
	}

CSimPsyFixedDataModeStep::CSimPsyFixedDataModeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSimPsyFixedDataModeStep);
	}

TVerdict CSimPsyFixedDataModeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	 CTe_LbsSimulationPSYSuiteStepBase::doTestStepPreambleL();
	 if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
    TTime time;
    time.UniversalTime();            
    iSeed = time.Int64();            
    iSimDataArray = new (ELeave) CDesC8ArrayFlat(10);  
    SetSimDataFileL(KSimEmptyFile);
	return TestStepResult();
	}


TVerdict CSimPsyFixedDataModeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsSimulationPSYSuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	return TestStepResult();
	}



TVerdict CSimPsyFixedDataModeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_LbsSimulationPSYSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSimPsyFixedDataModeStep::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CSimPsyFixedDataModeStep::StartL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0: 
        	_LIT(KSingleTestName, "**** Single request test ****");
        	INFO_PRINTF1(KSingleTestName);
        	TestSingleRequestL();
        	break;                    
        case 1: 
        	_LIT(KMultipleRequestsMemoryTestName, "**** Multiple requests memory leak test ****");
        	INFO_PRINTF1(KMultipleRequestsMemoryTestName);
        	//TestMultipleRequestsL(20); 
        	HeapTestL(aIndex);
        	break;        
        default: break;
        }
    }
 

void CSimPsyFixedDataModeStep::DoTestL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0: TestSingleRequestL(); break;                    
        case 1: TestMultipleRequestsL(20); break;        
        default: break;
        }
    }
// ---------------------------------------------------------
// CSimPsyFixedDataModeStep::TestSingleRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyFixedDataModeStep::TestSingleRequestL()
    {
    SetSimDataFileL(KSimEmptyFile);
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();

    TInt status;
    TPositionInfo position;
    TTimeIntervalMicroSeconds requestTime;
    
    requester->MakeRequests();
    requester->GetResult(status, position, requestTime);           
    CheckRequestResultL(status, position);            

    CleanupStack::PopAndDestroy(); // requester        
	_LIT(KSingleTestName, "Ended: **** Single request test ****");
	INFO_PRINTF1(KSingleTestName);
    }

// ---------------------------------------------------------
// CSimPsyFixedDataModeStep::TestMultRequestsL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyFixedDataModeStep::TestMultipleRequestsL(TInt aNumberOfRequests)
    {
    SetSimDataFileL(KSimEmptyFile);
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();

    TInt status;
    TPositionInfo position;    
    TTimeIntervalMicroSeconds requestTime;

    for (TInt i = 0; i < aNumberOfRequests; i++)
        {
        requester->MakeRequests();
        requester->GetResult(status, position, requestTime);
    	CheckRequestResultL(status, position);            
        }

    CleanupStack::PopAndDestroy(); // requester        
	_LIT(KMultipleRequestsMemoryTestName, "Ended: **** Multiple requests memory leak test ****");
	INFO_PRINTF1(KMultipleRequestsMemoryTestName);
    }

// ---------------------------------------------------------
// CSimPsyFixedDataModeStep::CheckRequestResultL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyFixedDataModeStep::CheckRequestResultL(
	TInt aStatus, TPositionInfo& aPosition)
    {    
    if (aStatus != KErrNone)
       {
       User::Leave(aStatus);
       }

    TReal longitude = 0.0;  
    TReal latitude = 0.0;
    TReal altitude = 0.0;      
    TReal32 hacc = 0.0;
    TReal32 vacc = 0.0;    
    
    TPosition position;		
	aPosition.GetPosition(position);

    TBuf<100> buf;
    if (aPosition.ModuleId().iUid != KSimModuleId)
        {
        _LIT(KWrongUid, "Expected simulationpsy uid");
        LogErrorAndFail(KWrongUid);
        }
    if (aPosition.UpdateType() != EPositionUpdateGeneral)
        {
        _LIT(KWrongUpType, "Expected Update Type General");
        LogErrorAndFail(KWrongUpType);
        }

    if (position.VerticalAccuracy() != vacc)  
        {
        _LIT(KVAcc, "Expected vacc %d.");      
        buf.Format(KVAcc, vacc);
        LogErrorAndFail(buf);
        }
    if (position.HorizontalAccuracy() != hacc)  
        {
        _LIT(KHAcc, "Expected hacc %d.");
        buf.Format(KHAcc, hacc);
        LogErrorAndFail(buf);
        }    
    if (position.Latitude() != latitude)
        {
        _LIT(KLat, "Expected latitude %.4f.");
        buf.Format(KLat, latitude);
        LogErrorAndFail(buf);
        }
    if (position.Longitude() != longitude)
        {
        _LIT(KLong, "Expected longitude %.4f.");
        buf.Format(KLong, longitude);
        LogErrorAndFail(buf);
        }
    if(position.Altitude() != altitude)
    	{
        _LIT(KLong, "Expected altitude %.4f.");
        buf.Format(KLong, altitude);
        LogErrorAndFail(buf);
    	}
    }

// ---------------------------------------------------------
// CSimPsyFixedDataModeStep::SetSimDataFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyFixedDataModeStep::SetSimDataFileL(const TDesC& aFileName)
    {
    // Third version of settings simulation psy file

	TFileName fileName = aFileName;
    CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
    User::LeaveIfError(repository->Set(KCRKeySimPSYSimulationFile, fileName));
    CleanupStack::PopAndDestroy(repository);
    }


// End of File
