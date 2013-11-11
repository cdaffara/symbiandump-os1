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
 @file SimPsyStep.cpp
 @internalTechnology
*/
#include "simpsystep.h"
#include "te_lbssimulationpsysuitedefs.h"
#include "lbssystemcontroller.h"

#include <lbs.h>

#include <centralrepository.h>
#include <lbs/simulationpsyinternalcrkeys.h>

#include "CPosFileReader.h"
#include "EPos_TDesTokeniser.h"
#include "CSimPsyMultiRequester.h"
#include "CSimPsyRequester.h"
//#include "CSimPsyMultiNmeaRequester.h"


// A few useful macros
#define MAX(x,y)      ((x)<(y))?(y):(x)
#define MIN(x,y)      ((x)<(y))?(x):(y)

GLREF_C TPositionModuleStatus ModuleStatus();
GLREF_C TPositionModuleStatus PrevModuleStatus();

// CONSTANTS
//_LIT(KSimPsyName,"Simulation PSY");
_LIT(KSimPsyName,"Simulation");
_LIT(KSimFile, "c:\\system\\data\\simulationdata.sps");

_LIT(KDeterministicFile, "c:\\system\\data\\simulationData_deterministic.sps");
_LIT(KRandomFile, "c:\\system\\data\\simulationData_random.sps");
_LIT(KSimMoveAndBundFile, "c:\\system\\data\\simulationData_simmove_bund.sps");
_LIT(KSimSamePlace, "c:\\system\\data\\simulationdata_same_place.sps");

_LIT(KSimPositiveLongitudeWrapAroundFile, "c:\\system\\data\\simulationData_longwrap1.sps");
_LIT(KSimNegativeLongitudeWrapAroundFile, "c:\\system\\data\\simulationData_longwrap2.sps");

_LIT(KSimWrongHorAcc_File, "c:\\system\\data\\simulationData_wrongHorAcc.sps");
_LIT(KSimWrongVerAcc_File, "c:\\system\\data\\simulationData_wrongVerAcc.sps");
_LIT(KSimWrongLongitude_File, "c:\\system\\data\\simulationData_wrongLongitude.sps");
_LIT(KSimWrongLongitudeMax_File, "c:\\system\\data\\simulationData_wrongLongitudeMax.sps");
_LIT(KSimWrongLatitude_File, "c:\\system\\data\\simulationData_wrongLatitude.sps");
_LIT(KSimWrongLatitudeMax_File, "c:\\system\\data\\simulationData_wrongLatitudeMax.sps");
_LIT(KSimWrongSpeed_File, "c:\\system\\data\\simulationData_wrongSpeed.sps");
_LIT(KSimWrongCourse_File, "c:\\system\\data\\simulationData_wrongCourse.sps");
_LIT(KSimWrongTTF_min_File, "c:\\system\\data\\simulationData_wrongTTF_min.sps");
_LIT(KSimWrongTTF_max_File, "c:\\system\\data\\simulationData_wrongTTF_max.sps");
_LIT(KSimWrongPowerupTimeFile, "c:\\system\\data\\simulationData_wrong_poweruptime.sps");
_LIT(KSimWrongDeterministicFile, "c:\\system\\data\\simulationData_wrong_deterministic.sps");
_LIT(KSimWrongNumOfItemsFile, "c:\\system\\data\\simulationData_wrongNumberOfItems.sps");
_LIT(KSimWrongWordFile, "c:\\system\\data\\simulationData_wrongWordDoc.sps");

const TReal KEarthRadius = 6371010;
const TReal KPI = 3.1415926535897932384626433832795;

const TInt32 KSimModuleId = 270498433;

CSimPsyStep::~CSimPsyStep()
/**
 * Destructor
 */
	{
    iFileReader->CloseResources();
    delete iFileReader;
    iFileReader = NULL;
    delete iSimDataArray; 
    iSimDataArray = NULL;

#ifdef __CTC__ 
    #pragma CTC APPEND
#endif
	}

CSimPsyStep::CSimPsyStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSimPsyStep);
	}

TVerdict CSimPsyStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_LbsSimulationPSYSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	
	TTime time;
	time.UniversalTime();            
	iSeed = time.Int64();            
	iSimDataArray = new (ELeave) CDesC8ArrayFlat(10);                                                 
	iFileReader = CPosFileReader::NewL(); 
	SetSimDataFileL(KSimFile);
	ReadSimDataFromFileL(KSimFile);
	return TestStepResult();
	}


TVerdict CSimPsyStep::doTestStepL()
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



TVerdict CSimPsyStep::doTestStepPostambleL()
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


// ---------------------------------------------------------
// CSimPsyStep::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CSimPsyStep::StartL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0:
        	_LIT(KSingleTestName, "**** Single request test ****");
        	INFO_PRINTF1(KSingleTestName);
        	TestSingleRequestL(); 
        	break;                    
        case 1:
        	_LIT(KSingleCancelTest, "**** Single cancel test ****");
        	INFO_PRINTF1(KSingleCancelTest);
        	TestSingleCancelTestL(); 
        	break;
        case 2: 
        	_LIT(KMultCancelTest, "**** Multiple cancel test %d ****"); 
        	INFO_PRINTF1(KMultCancelTest);
        	TestMultipleCancelTestL(EFalse, EFalse, ETrue); 
        	break;
        case 3: TestMultipleCancelTestL(ETrue, EFalse, ETrue); break;
        case 4: TestMultipleCancelTestL(ETrue, ETrue, ETrue); break;        
        case 5: 
        	_LIT(KSimulatedMovements, "**** Simulated movements ****");
        	INFO_PRINTF1(KSimulatedMovements);
        	TestSimulatedMovementsL(5); 
        	break;        
        case 6: 
        	_LIT(KDeterministicFailure, "**** Deterministic failure ****"); 
        	INFO_PRINTF1(KDeterministicFailure);
        	TestDeterministicFailureL(); 
        	break;        
        case 7:
        	_LIT(KRandomFailure, "**** Random failure ****"); 
        	INFO_PRINTF1(KRandomFailure);
        	TestRandomFailureL(); 
        	break;        
        case 8: 
        	_LIT(KMultipleRequestsMemoryTestName, "**** Multiple requests memory leak test ****");
        	INFO_PRINTF1(KMultipleRequestsMemoryTestName);
        	//TestMultipleRequestsL(20); 
        	HeapTestL(aIndex);
        	break;        
        case 9:
        	_LIT(KMultipleRequestsAndCancelMemoryTestName, "**** Multiple requests and cancel memory leak test ****");
        	INFO_PRINTF1(KMultipleRequestsAndCancelMemoryTestName);
        	//TestMultipleRequestsAndCancelL(2/*20*/, EFalse, ETrue, EFalse); 
        	HeapTestL(aIndex);
        	break;        
        case 10: 
        	_LIT(KLongitudeWraparoundTestName, "**** Longitude wraparound test ****");    
        	INFO_PRINTF1(KLongitudeWraparoundTestName);
        	TestLongitudeWrapAroundL(); 
        	break;        
        case 11:
        	_LIT(KBadSimFile, "**** Bad Simulation Data file ****");
        	INFO_PRINTF1(KBadSimFile);
        	TestBadSimulationDataFileL(); 
        	break;        
        case 12: 
        	_LIT(KReqTimeAfterCancel, "**** Request Time after Cancel ****");
        	INFO_PRINTF1(KReqTimeAfterCancel);
        	TestRequestTimeAfterCancelL(); 
        	break;  
        case 13: 
        	_LIT(KReqCancelReq, "**** Request Cancel Request with Time ****");
        	INFO_PRINTF1(KReqCancelReq);
        	TestRequestCancelRequestL(); 
        	break;
        case 14: 
        	_LIT(KStatusEvent, "**** Status Event test ****");
        	INFO_PRINTF1(KStatusEvent);
        	TestStatusEventL(); 
        	break;
        case 15: 
        	_LIT(KModuleInfo, "**** Module Info test ****");
        	INFO_PRINTF1(KModuleInfo);
        	TestModuleInfoL(); 
        	break;
        case 16: 
        	_LIT(KStillStanding, "**** Standing still test ****");
        	INFO_PRINTF1(KStillStanding);
        	TestStandingStillL(); 
        	break;
        default: break;
        }
    }
  

void CSimPsyStep::DoTestL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0: TestSingleRequestL(); break;                    
        case 1: TestSingleCancelTestL(); break;
        case 2: TestMultipleCancelTestL(EFalse, EFalse, ETrue); break;
        case 3: TestMultipleCancelTestL(ETrue, EFalse, ETrue); break;
        case 4: TestMultipleCancelTestL(ETrue, ETrue, ETrue); break;        
        case 5: TestSimulatedMovementsL(5); break;        
        case 6: TestDeterministicFailureL(); break;        
        case 7: TestRandomFailureL(); break;        
        case 8: TestMultipleRequestsL(20); break;        
        case 9: TestMultipleRequestsAndCancelL(2/*20*/, EFalse, ETrue, EFalse); break;        
        case 10: TestLongitudeWrapAroundL(); break;        
        case 11: TestBadSimulationDataFileL(); break;        
        case 12: TestRequestTimeAfterCancelL(); break;  
        case 13: TestRequestCancelRequestL(); break;
        case 14: TestStatusEventL(); break;
        case 15: TestModuleInfoL(); break;
        case 16: TestStandingStillL(); break;
        default: break;
        }
    }

// ---------------------------------------------------------
// CSimPsyStep::TestSingleRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestSingleRequestL()
    {
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();

    TInt status;
    TPositionInfo position;
    TPosition pos, pos2;    
    TTimeIntervalMicroSeconds requestTime;
    
    requester->MakeRequests();
    requester->GetResult(status, position, requestTime);           
    CheckRequestResultL(status, position);            
	CheckRequestTimeL(KSimFile, ETrue, position, requestTime);            
	position.GetPosition(pos);

	requester->MakeRequests();
    requester->GetResult(status, position, requestTime);     
	CheckRequestTimeL(KSimFile, EFalse, position, requestTime);
	position.GetPosition(pos2);

	// Check if first altitude is != 0.0 
	if (pos.Altitude() != 0.0)
        {
        _LIT(KLong, "Expected altitude == 0.0, but got: %d.");
		TBuf<60> buf;
        buf.Format(KLong, pos.Altitude());
        LogErrorAndFail(buf);
        }
	// Check if first altitude is same as the second
	if (pos.Altitude() == pos2.Altitude())
        {
        _LIT(KLong, "Expected altitude != previous altitude. %d.");
		TBuf<50> buf;
        buf.Format(KLong, pos2.Altitude());
        LogErrorAndFail(buf);
        }

    CleanupStack::PopAndDestroy(); // requester  
	_LIT(KSingleTestName, "Ended: **** Single request test ****");
	INFO_PRINTF1(KSingleTestName);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestSingleCancelTestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestSingleCancelTestL()
    {            
    SetSimDataFileL(KSimMoveAndBundFile);

    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();
    requester->SetRequesterWillCancelL();

    TPositionInfo position;
    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    requester->StopRequests();
    requester->MakeRequests();
    requester->GetResult(status, position, requestTime);
    if (status != KErrCancel)
        {
        _LIT(KReqWasNotCancelled, "The request was not cancelled");
        LogErrorAndFail(KReqWasNotCancelled);
        }
	TPosition pos;
	position.GetPosition(pos);
    if (pos.Time().Int64() != 0.0)
        {
        _LIT(KHasPosition, "The cancelled request returned a position");
        LogErrorAndFail(KHasPosition);        
        }

    CleanupStack::PopAndDestroy(); // requester    
	_LIT(KSingleCancelTest, "Ended: **** Single cancel test ****");
	INFO_PRINTF1(KSingleCancelTest);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestSimpleCancelTestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestMultipleCancelTestL(
	TBool aFirstWillCancel,
	TBool aSecondWillCancel,
	TBool aThirdWillCancel)
    {
    SetSimDataFileL(KSimMoveAndBundFile);

    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC(3);
    if (aFirstWillCancel)
        {
        requester->SetRequesterWillCancelL(0);
        }
    if (aSecondWillCancel)
        {
        requester->SetRequesterWillCancelL(1);
        }
    if (aThirdWillCancel)
        {
        requester->SetRequesterWillCancelL(2);
        }

    TPositionInfo position;
    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    
    requester->MakeRequests();

    for (TInt i = 0; i < 3; i++)
        {
        requester->GetResult(status, position, requestTime, i);

		TPosition pos;
		position.GetPosition(pos);
        TBool shouldHaveCancelled = 
            (i == 0) ? aFirstWillCancel
            : (i == 1) ? aSecondWillCancel
            : aThirdWillCancel;

        if (shouldHaveCancelled)
            {
            if (status != KErrCancel)
                {
                _LIT(KReqWasNotCancelled, "The request was not cancelled");
                LogErrorAndFail(KReqWasNotCancelled);
                }
            if (pos.Time().Int64() != 0.0)
                {
                _LIT(KHasPosition, "The cancelled request returned a position");
                LogErrorAndFail(KHasPosition);        
                }
            }
        else
            {
            if (status != KErrNone)
                {
                _LIT(KReqFailed, "The request failed");
                LogErrorAndFail(KReqFailed);
                }
            if (pos.Time().Int64() == 0.0)
                {
                _LIT(KHasNoPosition, "The request did not return a position");
                LogErrorAndFail(KHasNoPosition);        
                }
            }
        }
    CleanupStack::PopAndDestroy(); // requester       
	_LIT(KMultCancelTest, "Ended: **** Multiple cancel test ****"); 
	INFO_PRINTF1(KMultCancelTest);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestDeterministicFailureL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestDeterministicFailureL()
    {
    _LIT8(KDeterministic, "Deterministic*");
    const TUint8 KPosDataSeparator = '=';
    const TUint8 KPosDefaultDataEndMarker = ';';

    SetSimDataFileL(KDeterministicFile);
    ReadSimDataFromFileL(KDeterministicFile);
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();    

    TInt status;
    TPositionInfo position;    
    TTimeIntervalMicroSeconds requestTime;        

    TInt failFrequency = 0;
    for (TInt i = 0; i < iSimDataArray->Count(); i++)
        {
        if (iSimDataArray->MdcaPoint(i).MatchF(KDeterministic) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);            
            lexer.Val(failFrequency);           
            break;
            }                
        }
        
    TBuf<20> buf;
    TInt request;
    TInt numberOfRequests = 10;
    TBool requestShouldFail = EFalse;
    for (request = 1; request <= numberOfRequests; request++)
        {
        if (failFrequency == 0)
            {
            requestShouldFail = EFalse;
            }
        else if (request % failFrequency == 0)
            {
            requestShouldFail = ETrue;
            }
        requester->MakeRequests();
        requester->GetResult(status, position, requestTime);
		if (request > 1)
			{
			CheckRequestTimeL(KDeterministicFile, EFalse, position, requestTime);
			}

        if (requestShouldFail)            
            {           
            requestShouldFail = EFalse;
			TPosition pos;
			position.GetPosition(pos);
			if (status != KPositionQualityLoss)
                {
                TBuf<40> output;
                _LIT(KStatusError, "KPositionQualityLoss was expected, but got %d");
                output.Format(KStatusError, status);
                LogErrorAndFail(output);
                }
            TBuf<30> buf;
            _LIT(KOut, "Failing request no : %d.");
            buf.Format(KOut, numberOfRequests);
            }
        else
            {
            CheckThatPositionExistsL(position);        
            }
        }
    CleanupStack::PopAndDestroy();  //requester

    TInt noOfClients = 4;
    requester = NULL;
    requester = CSimPsyMultiRequester::NewLC(noOfClients);        
    
    for (TInt j = 1; j <= 10; j++)
        {        
        requester->MakeRequests();        
        for (TInt i = 0; i < noOfClients; i++)
            {
            requester->GetResult(status, position, requestTime, i);
            if (failFrequency == 0)
                {
                CheckThatPositionExistsL(position);        
                }
            else if (j % failFrequency == 0)
                {
    			if (status != KPositionQualityLoss)
                    {
                    TBuf<40> output;
                    _LIT(KStatusError, "KPositionQualityLoss was expected, but got %d");
                    output.Format(KStatusError, status);                               
                    LogErrorAndFail(output);
                    }    
                TBuf<40> out;
                _LIT(KOut, "Fail succeeded for request %d");
                out.Format(KOut, j-1);
                }            
            }        
        }
    CleanupStack::PopAndDestroy(); //requester
	_LIT(KDeterministicFailure, "Ended: **** Deterministic failure ****"); 
	INFO_PRINTF1(KDeterministicFailure);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestRandomFailureL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestRandomFailureL()
    {    
    _LIT8(KRandom, "Random*");    
    const TUint8 KPosDataSeparator = '=';
    const TUint8 KPosDefaultDataEndMarker = ';';

    SetSimDataFileL(KRandomFile);
    ReadSimDataFromFileL(KRandomFile);    

    TInt status;
    TPositionInfo position;    
    TTimeIntervalMicroSeconds requestTime;        

    TInt failFrequency = 0;    
    for (TInt i = 0; i < iSimDataArray->Count(); i++)
        {
        if (iSimDataArray->MdcaPoint(i).MatchF(KRandom) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8 lexer(value);            
            lexer.Val(failFrequency);           
            }
        }
    
    TInt failedRequests = 0;
    TInt succeededRequests = 0;

    TInt noOfRequests = 25;    
    TInt noOfClients = 20;
    
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC(noOfClients);        
        
    for (TInt j = 1; j <= noOfRequests; j++)
        {
		requester->MakeRequests();        
        for (TInt i = 0; i < noOfClients; i++)
            {
            requester->GetResult(status, position, requestTime, i);

			TPosition pos;
			position.GetPosition(pos);
            if (status != KErrNone)
                {
                }
            if (status == KErrGeneral)            
                {
                failedRequests++;             
                }        
            else if (position.ModuleId().iUid != KSimModuleId && status != KErrGeneral)
                {      
                _LIT(KRequestError, "Corrupt status or position encountered.");  
                LogErrorAndFail(KRequestError);  
                }
            else 
                {
                succeededRequests++;
                CheckThatPositionExistsL(position);        
                }            
            }        
        }
    TInt timesToFail = noOfRequests * noOfClients / failFrequency;
    if (failedRequests > timesToFail+noOfClients && 
		failedRequests < timesToFail-noOfClients)
        {
        _LIT(KRandomError, "Random failure failed.");  
        LogErrorAndFail(KRandomError);  
        }

    CleanupStack::PopAndDestroy(); //requester
	_LIT(KRandomFailure, "Ended: **** Random failure ****"); 
	INFO_PRINTF1(KRandomFailure);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestSimulatedMovementsL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestSimulatedMovementsL(TInt aNumberOfRequests)
    {
    _LIT(KAltError,  "Incorrect altitude.");  

    SetSimDataFileL(KSimMoveAndBundFile);
    ReadSimDataFromFileL(KSimMoveAndBundFile);
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();    
    TPositionInfo position;
    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    TTime requestStartTime;
    TTime lastRequestTime;

    // Make requests to sim psy and check that the results are equal 
    //to the sim data in the simulation file
    requestStartTime.UniversalTime();
    lastRequestTime = requestStartTime;
    requester->MakeRequests();
    requester->GetResult(status, position, requestTime);
    CheckRequestResultL(status, position);
	CheckRequestTimeL(KSimMoveAndBundFile, ETrue, position, requestTime);

	TPositionInfoBase oldPosition = position;

    //Check that new simulation data is received when performing further requests    
    TPositionInfo newPosition;
    for (TInt i = 0; i < aNumberOfRequests; i++)
        {               
        requestStartTime.UniversalTime();        
        requester->MakeRequests();        
        requester->GetResult(status, newPosition, requestTime);
		CheckRequestTimeL(KSimMoveAndBundFile, EFalse, newPosition, requestTime);
        
        if (status == KErrNone)
            {
			TPosition newPos;
			newPosition.GetPosition(newPos);

            if (newPos.VerticalAccuracy() != 0)
                {
                if (newPos.Altitude() == 0)  
                    {        
                    LogErrorAndFail(KAltError);
                    }      
                }
            lastRequestTime = requestStartTime;
            }
        else
            {
            User::Leave(status);
            }        
        }        
	CleanupStack::PopAndDestroy(); // requester   
	_LIT(KSimulatedMovements, "Ended: **** Simulated movements ****");
	INFO_PRINTF1(KSimulatedMovements);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestMultRequestsL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::TestMultipleRequestsL(TInt aNumberOfRequests)
    {
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();

    TInt status;
    TPositionInfo position;    
    TTimeIntervalMicroSeconds requestTime;
    _LIT(k, "status = %d");
    TBuf<20> buf;
    for (TInt i = 0; i < aNumberOfRequests; i++)
        {
        requester->MakeRequests();
        requester->GetResult(status, position, requestTime);
        buf.Format(k, status);
        }
    CleanupStack::PopAndDestroy(); // requester     
	_LIT(KMultipleRequestsMemoryTestName, "Ended: **** Multiple requests memory leak test ****");
	INFO_PRINTF1(KMultipleRequestsMemoryTestName);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestMultipleRequestsAndCancelL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestMultipleRequestsAndCancelL(
	TInt aNumberOfRequests, TBool aFirstWillCancel,
	TBool aSecondWillCancel, TBool aThirdWillCancel)
    {
    TBuf8<256> output;    

    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC(3);
    if (aFirstWillCancel)
        {
        requester->SetRequesterWillCancelL(0);
        }
    if (aSecondWillCancel)
        {
        requester->SetRequesterWillCancelL(1);
        }
    if (aThirdWillCancel)
        {
        requester->SetRequesterWillCancelL(2);
        }

    TPositionInfo position;
    TInt status;
    TTimeIntervalMicroSeconds requestTime;        
    TBuf<50> buf;
    _LIT(K, "CSimPsyStep::GetResult %d");

    for (TInt i = 0; i < aNumberOfRequests; i++)
        {
        requester->MakeRequests();

        for (TInt j = 0; j < 3; j++)
            {            
            requester->GetResult(status, position, requestTime, j);    
            buf.Format(K, status);
            }
        }
    CleanupStack::PopAndDestroy(); // requester      
	_LIT(KMultipleRequestsAndCancelMemoryTestName, "Ended: **** Multiple requests and cancel memory leak test ****");
	INFO_PRINTF1(KMultipleRequestsAndCancelMemoryTestName);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestLongitudeWrapAroundL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestLongitudeWrapAroundL()
    {
    const TInt KMaxNofRequests = 10;
    const TReal KDeltaLongitude = 1.0;
    const TReal KMaxLongitude = 180.0;

    TInt status;
    TPositionInfo position;
    TTimeIntervalMicroSeconds requestTime;
    TPosition pos;
        
        { // Positive longitude wrap around
        SetSimDataFileL(KSimPositiveLongitudeWrapAroundFile);
        ReadSimDataFromFileL(KSimPositiveLongitudeWrapAroundFile);

        CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();
    
        TInt i;
        for (i = 0; i < KMaxNofRequests; i++)
            {
            requester->MakeRequests();
            requester->GetResult(status, position, requestTime);           
	        position.GetPosition(pos);

            if (pos.Longitude() <= -KMaxLongitude ||
                pos.Longitude() > KMaxLongitude)
                {
                _LIT(KError, "Invalid longitude. Wraparound did not work properly");
                LogErrorAndFail(KError);
                }

            if (pos.Longitude() < -KMaxLongitude + KDeltaLongitude)
                { // Longitude has wrapped around ok.
                break;
                }
            }

        if (i == KMaxNofRequests)
            {
            _LIT(KError, "Longitude never wrapped around.");
            LogErrorAndFail(KError);
            }

        CleanupStack::PopAndDestroy(requester);
        }
        
        { // Negative longitude wrap around
        SetSimDataFileL(KSimNegativeLongitudeWrapAroundFile);
        ReadSimDataFromFileL(KSimNegativeLongitudeWrapAroundFile);

        CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();
    
        TInt i;
        for (i = 0; i < KMaxNofRequests; i++)
            {
            requester->MakeRequests();
            requester->GetResult(status, position, requestTime);           
	        position.GetPosition(pos);

            if (pos.Longitude() <= -KMaxLongitude ||
                pos.Longitude() > KMaxLongitude)
                {
                _LIT(KError, "Invalid longitude. Wraparound did not work properly");
                LogErrorAndFail(KError);
                }

            if (pos.Longitude() > KMaxLongitude - KDeltaLongitude)
                { // Longitude has wrapped around ok.
                break;
                }

            }

        if (i == KMaxNofRequests)
            {
            _LIT(KError, "Longitude never wrapped around.");
            LogErrorAndFail(KError);
            }

        CleanupStack::PopAndDestroy(requester);
        }
    _LIT(KLongitudeWraparoundTestName, "Ended: **** Longitude wraparound test ****");    
    INFO_PRINTF1(KLongitudeWraparoundTestName);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestBadSimulationDataFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestBadSimulationDataFileL()
    {
    TInt status = KErrNone;
	CSimPsyMultiRequester* requester = NULL;
	
	// Hor Acc
	SetSimDataFileL(KSimWrongHorAcc_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Vert Acc
	SetSimDataFileL(KSimWrongVerAcc_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Time To Fix Min
	SetSimDataFileL(KSimWrongTTF_min_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Time To Fix Max
    SetSimDataFileL(KSimWrongTTF_max_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Power up time
    SetSimDataFileL(KSimWrongPowerupTimeFile);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Longitude
    SetSimDataFileL(KSimWrongLongitude_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Longitude, Max
    SetSimDataFileL(KSimWrongLongitudeMax_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Latitude
    SetSimDataFileL(KSimWrongLatitude_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Latitude, Max
    SetSimDataFileL(KSimWrongLatitudeMax_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Speed
    SetSimDataFileL(KSimWrongSpeed_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Course
    SetSimDataFileL(KSimWrongCourse_File);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Deterministic
    SetSimDataFileL(KSimWrongDeterministicFile);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Wrong number of items in file
    SetSimDataFileL(KSimWrongNumOfItemsFile);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;

	// Wrong number of items in file
    SetSimDataFileL(KSimWrongWordFile);
    TRAP(status, requester = CSimPsyMultiRequester::NewLC());
	if (status != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
	status = KErrNone;
	if(requester)
		{
		User::Leave(KErrGeneral);
		}
	_LIT(KBadSimFile, "Ended: **** Bad Simulation Data file ****");
	INFO_PRINTF1(KBadSimFile);
	}

// ---------------------------------------------------------
// CSimPsyStep::TestRequestTimeAfterCancelL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestRequestTimeAfterCancelL()
    {
    // SetSimDataFileL(KSimFile);
    // ReadSimDataFromFileL(KSimFile);

    _LIT8(KTTFmin, "TimeToFix min*");
    _LIT8(KTTFmax, "TimeToFix max*");
    const TUint8 KPosDataSeparator = '=';
    const TUint8 KPosDefaultDataEndMarker = ';';
	const TInt KEmulatorErrorTiming = 100000;

	TInt ttfMin = 0;
	TInt ttfMax = 0;
    for (TInt i = 0; i < iSimDataArray->Count(); i++)
        {
        if (iSimDataArray->MdcaPoint(i).MatchF(KTTFmin) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(ttfMin);
            }                
        if (iSimDataArray->MdcaPoint(i).MatchF(KTTFmax) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(ttfMax);
            }                
        }
	if (ttfMin == 0)
		{
		User::Leave(KErrGeneral);
		}
	if (ttfMax == 0)
		{
		User::Leave(KErrGeneral);
		}

	TInt numberOfTests = 5;
	TInt status;
	TPositionInfo position;
	TPosition pos, pos2;    
	TTimeIntervalMicroSeconds requestTime;
	for (TInt j = 0; j < numberOfTests; j++)
		{
		CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();
		requester->SetRequesterWillCancelL();
		requester->MakeRequests();
		requester->GetResult(status, position, requestTime);           
		if (status != KErrCancel)
			{
			User::Leave(KErrGeneral);
			}

        User::After(1 * 100000); // 100 ms
		requester->SetRequesterWillCancelL(0, EFalse);
		requester->MakeRequests();
		requester->GetResult(status, position, requestTime);           
		CheckThatPositionExistsL(position);         
		User::LeaveIfError(status);

		if (ttfMin * 1000000 - KEmulatorErrorTiming > requestTime.Int64() || 
			ttfMax * 1000000 + KEmulatorErrorTiming < requestTime.Int64())
			{
			User::Leave(KErrGeneral);	
			}

		CleanupStack::PopAndDestroy(); // requester
		}
	_LIT(KReqTimeAfterCancel, "Ended: **** Request Time after Cancel ****");
	INFO_PRINTF1(KReqTimeAfterCancel);
	}

// ---------------------------------------------------------
// CSimPsyStep::TestRequestCancelRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestRequestCancelRequestL()
    {
    _LIT8(KTTFmin, "TimeToFix min*");
    _LIT8(KTTFmax, "TimeToFix max*");
    const TUint8 KPosDataSeparator = '=';
    const TUint8 KPosDefaultDataEndMarker = ';';
	const TInt KEmulatorErrorTiming = 100000;

	TInt ttfMin = 0;
	TInt ttfMax = 0;
    for (TInt i = 0; i < iSimDataArray->Count(); i++)
        {
        if (iSimDataArray->MdcaPoint(i).MatchF(KTTFmin) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(ttfMin);
            }                
        if (iSimDataArray->MdcaPoint(i).MatchF(KTTFmax) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(ttfMax);
            }                
        }
	if (ttfMin == 0)
		{
		User::Leave(KErrGeneral);
		}
	if (ttfMax == 0)
		{
		User::Leave(KErrGeneral);
		}

	TInt numberOfTests = 5;
	TInt status;
	TPositionInfo position;
	TPosition pos, pos2;    
	TTimeIntervalMicroSeconds requestTime;
	for (TInt j = 0; j < numberOfTests; j++)
		{
		CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();
		requester->MakeRequests();
		requester->GetResult(status, position, requestTime);           
		User::LeaveIfError(status);

		requester->SetRequesterWillCancelL();
		requester->MakeRequests();
		requester->GetResult(status, position, requestTime);           
		if (status != KErrCancel)
			{
			User::Leave(KErrGeneral);
			}

        User::After(1 * 100000); // 100 ms
		requester->SetRequesterWillCancelL(0, EFalse);
		requester->MakeRequests();
		requester->GetResult(status, position, requestTime);           
		CheckThatPositionExistsL(position);         
		User::LeaveIfError(status);

		if (ttfMin * 1000000 - KEmulatorErrorTiming > requestTime.Int64() || 
			ttfMax * 1000000 + KEmulatorErrorTiming < requestTime.Int64())
			{
			User::Leave(KErrGeneral);	
			}

		CleanupStack::PopAndDestroy(); // requester
		}
	_LIT(KReqCancelReq, "Ended: **** Request Cancel Request with Time ****");
	INFO_PRINTF1(KReqCancelReq);
	}

// ---------------------------------------------------------
// CSimPsyStep::TestStatusEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestStatusEventL()
    {
    // SetSimDataFileL(KSimFile);
    // ReadSimDataFromFileL(KSimFile);

 	TInt numberOfTests = 2;
	TInt status;
	TPositionInfo position;
	TTimeIntervalMicroSeconds requestTime;
	for (TInt j = 0; j < numberOfTests; j++)
		{
        // Open PSY
		CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();
		
		// Check that module status is correct
	    if (!requester->GetModuleStatus())
		    {
		    User::Leave(KErrGeneral);
		    }
		// Check that number of reported statuses is correct
	    if (requester->ReportedStatuses() != 1)
		    {
		    User::Leave(KErrGeneral);
		    }
        requester->ResetReportedStatuses();
		    
		TPositionModuleStatus modStatus = requester->ModuleStatus();
		if (modStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady)
		    {
		    User::Leave(KErrGeneral);
		    } 
		if (modStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityNormal)
		    {
		    User::Leave(KErrGeneral);
		    } 
		
		// Make requests
		requester->MakeRequests();
		
		// Check that module status is correct
		if (!requester->GetModuleStatus())
		    {
		    User::Leave(KErrGeneral);
		    }
		// Check that number of reported statuses is correct
	    if (requester->ReportedStatuses() != 0)
		    {
		    User::Leave(KErrGeneral);
		    }
        requester->ResetReportedStatuses();
        
        modStatus = requester->ModuleStatus();
		if (modStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady)
		    {
		    User::Leave(KErrGeneral);
		    } 
		if (modStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityNormal)
		    {
		    User::Leave(KErrGeneral);
		    } 
		
		requester->GetResult(status, position, requestTime);           
		User::LeaveIfError(status);

        // Close PSY
		requester->ClosePositioner();
		
		// Check that module status is correct
		if (!requester->GetModuleStatus())
		    {
		    User::Leave(KErrGeneral);
		    }
		// Check that number of reported statuses is correct
	    if (requester->ReportedStatuses() != 1)
		    {
		    User::Leave(KErrGeneral);
		    }
        requester->ResetReportedStatuses();

		modStatus = requester->ModuleStatus();
		if (modStatus.DeviceStatus() != TPositionModuleStatus::EDeviceInactive)
		    {
		    User::Leave(KErrGeneral);
		    } 
		if (modStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityUnknown)
		    {
		    User::Leave(KErrGeneral);
		    } 
		CleanupStack::PopAndDestroy(); // requester	
		}

	SetSimDataFileL(KSimPsyName);
	CSimPsyMultiRequester* requester = NULL;
	TRAPD(err, requester = CSimPsyMultiRequester::NewLC());

	// Check that module status is correct
	if (err != KErrNotSupported)
		{
		User::Leave(KErrGeneral);
		}
	if (requester)
		{
		User::Leave(KErrGeneral);
		}
		
	TPositionModuleStatus modStatus = ModuleStatus();
	if (modStatus.DeviceStatus() != TPositionModuleStatus::EDeviceInactive)
		{
		User::Leave(KErrGeneral);
		} 
	if (modStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityUnknown)
		{
		User::Leave(KErrGeneral);
		}

	modStatus = PrevModuleStatus();
	if (modStatus.DeviceStatus() != TPositionModuleStatus::EDeviceError)
		{
		User::Leave(KErrGeneral);
		} 
	if (modStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityUnknown)
		{
		User::Leave(KErrGeneral);
		}
	_LIT(KStatusEvent, "Ended: **** Status Event test ****");
	INFO_PRINTF1(KStatusEvent);
	}

// ---------------------------------------------------------
// CSimPsyStep::TestModuleInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestModuleInfoL()
    {
#ifndef NO_LFW_SERVER
    RLbsSystemController lbsSysController;
    lbsSysController.OpenL(TUid::Uid(0x12345678));
    CleanupClosePushL(lbsSysController);
    lbsSysController.InitializeL();
    User::LeaveIfError(lbsSysController.RequestSystemStartup());    

    RPositionServer ps;
    CleanupClosePushL(ps);
    User::LeaveIfError(ps.Connect());
    
    TUid uid = { KSimModuleId };
    TPositionModuleInfo modInfo;
    TInt err = ps.GetModuleInfoById(uid, modInfo);
    
    if (err == KErrNotFound)
        {
        INFO_PRINTF1(_L("Simulation PSY not found in Location Server"));
        User::LeaveIfError(ps.GetModuleInfoById(uid, modInfo));
        }
        
    // Check info
    if (!modInfo.IsAvailable())
        {
        INFO_PRINTF1(_L("Simulation PSY not available"));
        }
    if (modInfo.ModuleId() != uid)
        {
        ERR_PRINTF1(_L("Simulation PSY has wrong module id"));
        User::Leave(KErrGeneral);
        }
    TBuf<30> modName;
    modInfo.GetModuleName(modName);
    if (modName != KSimPsyName)
        {
        ERR_PRINTF1(_L("Simulation PSY has wrong name"));
        User::Leave(KErrGeneral);
        }
    
    if (modInfo.DeviceLocation() != TPositionModuleInfo::EDeviceInternal)
        {
        ERR_PRINTF1(_L("Simulation PSY has wrong device location"));
        User::Leave(KErrGeneral);
        }
    if (modInfo.TechnologyType() != TPositionModuleInfo::ETechnologyTerminal)
        {
        ERR_PRINTF1(_L("Simulation PSY has technology type"));
        User::Leave(KErrGeneral);
        }
        
    TUint32 classesSupp = (EPositionInfoClass | EPositionSatelliteInfoClass | 
        EPositionCourseInfoClass | EPositionGenericInfoClass);
    if (modInfo.ClassesSupported(EPositionInfoFamily) != classesSupp)
        {
        ERR_PRINTF1(_L("Simulation PSY supports wrong classes"));
        User::Leave(KErrGeneral);
        }
    				
    TUint32 capsSupp = (TPositionModuleInfo::ECapabilityHorizontal | 
        TPositionModuleInfo::ECapabilityVertical | 
        TPositionModuleInfo::ECapabilitySpeed |
        TPositionModuleInfo::ECapabilitySatellite | 
        TPositionModuleInfo::ECapabilityCompass | 
        TPositionModuleInfo::ECapabilityNmea);
    if (modInfo.Capabilities() != capsSupp)
        {
        ERR_PRINTF1(_L("Simulation PSY has wrong capabilities"));
        User::Leave(KErrGeneral);
        }
    
    CleanupStack::PopAndDestroy(&ps);

    User::LeaveIfError(lbsSysController.RequestCompleteCloseDown());
    CleanupStack::PopAndDestroy(&lbsSysController);
#endif

    _LIT(KModuleInfoErrorTxt, "Incorrect Centrep value at line %d, got %d expected %d");
    _LIT(KModuleFloatInfoErrorTxt, "Incorrect Centrep value at line %d, got %f expected %d");
    TInt testInt;
    TReal testReal;    
    CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
    User::LeaveIfError(repository->Get(KPSYInterfaceVersion, testInt));
    if (testInt != 1) 
    	{ 
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYPriority, testInt));
    if (testInt != 1) 
    	{ 
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYStatus, testInt));
    if (testInt != 1) 
    	{ 
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    // 4
    User::LeaveIfError(repository->Get(KPSYTimeToFirstFix, testInt));
    if (testInt != 5000000) 
    	{ 
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 5000000);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYTimeToNextFix, testInt));
    if (testInt != 5000000) 
    	{ 
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 5000000);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYHorizontalAccuracy, testReal));
    if (testReal != 30) 
    	{
    	ERR_PRINTF4(KModuleFloatInfoErrorTxt, __LINE__, testReal, 30);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYVerticalAccuracy, testReal));
    if (testReal != 45) 
    	{ 
    	ERR_PRINTF4(KModuleFloatInfoErrorTxt, __LINE__, testReal, 45);
    	SetTestStepResult(EFail);
    	}

    // 8
    User::LeaveIfError(repository->Get(KPSYCostIndicator, testInt));
    if (testInt != 1) 
    	{ 
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYPowerConsumption, testInt));
    if (testInt != 1) 
    	{
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYTechnologyType, testInt));
    if (testInt != 1) 
    	{
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYDeviceLocation, testInt));
    if (testInt != 1) 
    	{
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}

    // 12
    User::LeaveIfError(repository->Get(KPSYCapabilities, testInt));
    if (testInt != 119) 
    	{
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 119);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYClassesSupported, testInt));
    if (testInt != 15) 
    	{
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 15);
    	SetTestStepResult(EFail);
    	}

    User::LeaveIfError(repository->Get(KPSYVisibility, testInt));
    if (testInt != 1) 
    	{
    	ERR_PRINTF4(KModuleInfoErrorTxt, __LINE__, testInt, 1);
    	SetTestStepResult(EFail);
    	}
    
    CleanupStack::PopAndDestroy(repository);
	_LIT(KModuleInfo, "Ended: **** Module Info test ****");
	INFO_PRINTF1(KModuleInfo);
    }

// ---------------------------------------------------------
// CSimPsyStep::TestStandingStillL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::TestStandingStillL()
    {
    SetSimDataFileL(KSimSamePlace);
    
    CSimPsyMultiRequester* requester = CSimPsyMultiRequester::NewLC();

    TInt status;
    TPositionInfo position;
    TPosition pos, pos2;    
    TTimeIntervalMicroSeconds requestTime;
    
    requester->MakeRequests();
    requester->GetResult(status, position, requestTime);           
	CheckRequestTimeL(KSimFile, ETrue, position, requestTime);            
	position.GetPosition(pos);

	// Check if first altitude is != 0.0 
	if (pos.Altitude() != 0.0)
        {
        _LIT(KLong, "Expected altitude == 0.0, but got: %d.");
		TBuf<60> buf;
        buf.Format(KLong, pos.Altitude());
        LogErrorAndFail(buf);
        }


    TReal32 distSum = 0;
    TInt nrOfReq = 100;
    TReal32 expectedAcc = 20;
    TInt moreAcc = 0;
    TReal32 delta;

    for(TInt i=0; i<nrOfReq; i++)
        {

    	INFO_PRINTF2(_L("for loop count=%d"), i);

    	requester->MakeRequests();
        requester->GetResult(status, position, requestTime);     
    	CheckRequestTimeL(KSimFile, EFalse, position, requestTime);
    	position.GetPosition(pos2);

        TReal32 distance;
        TReal32 distance2;
        TInt err = pos.Distance(pos2, distance, delta);
        TInt err2 = pos.Distance(pos2, distance2);
        
        distSum += distance;
        if (distance > expectedAcc) 
            {
            moreAcc++;
            }
        }

    TReal32 median = distSum/nrOfReq;

    // 60 percent of requests should not be over expectedAcc
    if (moreAcc > (nrOfReq * 0.6)) 
        {
        _LIT(KLong, "Expected hor. acc. not normalized.");
        LogErrorAndFail(KLong);
        }

    // Median should be less than expectedAcc plus 20 percent of expectedAcc
    if (median > (expectedAcc * 1.4))
        {
        _LIT(KLong, "Expected hor. acc. over expected. %f > %f");
		TBuf<80> buf;
        buf.Format(KLong, median, expectedAcc);
        LogErrorAndFail(buf);
        }

	// Check if first altitude is same as the second
	if (pos.Altitude() != pos2.Altitude())
        {
        _LIT(KLong, "Expected altitude != previous altitude. %d.");
		TBuf<50> buf;
        buf.Format(KLong, pos2.Altitude());
        LogErrorAndFail(buf);
        }

    CleanupStack::PopAndDestroy(); // requester  
	_LIT(KStillStanding, "Ended: **** Standing still test ****");
	INFO_PRINTF1(KStillStanding);
    }

// ---------------------------------------------------------
// CSimPsyStep::ComputeNewLongAndLat
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CSimPsyStep::ComputeNewLongAndLat(
	TPositionInfoBase& aPosition, TReal aAlpha, TReal& aLon, TReal& aLat)
    {    
    TReal cHdng1;
    TReal cLat1;
    TReal sAlpha;
    TReal sLat1;
    TReal cAlpha;
    TReal toRadians = KPI/180;
    TReal toDegrees = 180/KPI;
    TPositionInfo& aPositionInfo = static_cast<TPositionInfo&> (aPosition);
	TPosition position;
    aPositionInfo.GetPosition(position);
	TReal64 latitude = position.Latitude();
    TReal32 trueCourse = 0.0; // = aPosition->AdvancedInfo()->TrueCourse();

    Math::Cos(cHdng1, trueCourse * toRadians);
    Math::Cos(cLat1,  latitude * toRadians);
    Math::Sin(sAlpha, aAlpha);
    Math::Sin(sLat1,  latitude * toRadians);
    Math::Cos(cAlpha, aAlpha);

    TReal sLat2 = cHdng1*cLat1*sAlpha+sLat1*cAlpha; 
    TReal lat2;
    TInt err = Math::ASin(lat2, sLat2);
    if (err != KErrNone)
        {        
        return err;        
        }

    lat2 *= toDegrees;
    aLat = lat2;    

    TReal cLat2;
    Math::Cos(cLat2, lat2*toRadians);        
    TReal tmp =(cAlpha-sLat1*sLat2)/(cLat1*cLat2);    
    if (tmp > 1.0)
        {
        tmp = 1.0;
        }
    err = Math::ACos(aLon, tmp);    
    if (err != KErrNone)
        {
        return err;        
        }
    aLon *= toDegrees;
    TPosition pos; 
	aPositionInfo.GetPosition(pos);
    aLon += pos.Longitude();    
    
    TReal latError = 0;
    TInt timesToRandom = 20;
    for (TInt i = 0; i < timesToRandom; i++)
        {        
        latError += Math::FRand(iSeed); 
        }

    latError -= timesToRandom/2;
    TReal longError = latError;
    latError *= pos.HorizontalAccuracy();
    longError *= pos.HorizontalAccuracy();    

    aLat += (latError*360)/(2*KEarthRadius*KPI);
    aLon += (longError*360)/(2*KEarthRadius*KPI);    

    return KErrNone;
    }

// ---------------------------------------------------------
// CSimPsyStep::CheckRequestResultL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::CheckRequestResultL(
	TInt aStatus, TPositionInfo& aPosition)
    {    
    if (aStatus != KErrNone)
       {
       User::Leave(aStatus);
       }

    TReal longitudeMin  = 56.33;  
    TReal longitudeMax  = 56.34;  
    TReal latitudeMin   = 78.33;      
    TReal latitudeMax   = 78.34;      
    							//should be the same as the values in simulationdata.txt,
    TReal32 hacc     = 20.0;    //else the check below will fail
    TReal32 vacc     = 30.0;    
    
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
    if (position.Latitude() < latitudeMin || 
        position.Latitude() > latitudeMax)
        {
        _LIT(KLat, "Expected latitudeMin %.4f, latidueMax %.4f.");
        buf.Format(KLat, latitudeMin, latitudeMax);
        LogErrorAndFail(buf);
        }
    if (position.Longitude() < longitudeMin ||
        position.Longitude() > longitudeMax)
        {
        _LIT(KLong, "Expected longitudeMin %.4f, longitudeMax %.4f.");
        buf.Format(KLong, longitudeMin, longitudeMax);
        LogErrorAndFail(buf);
        }
    }

// ---------------------------------------------------------
// CSimPsyStep::CheckRequestTimeL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::CheckRequestTimeL(
	const TDesC& aSimFile, TBool aPowerUp, 
    const TPositionInfo& aPos,
	TTimeIntervalMicroSeconds& aReqTime, 
    TTimeIntervalMicroSeconds aTolerance)
    {    
    _LIT8(KPowerUpTime, "Powerup time*");
    _LIT8(KTTFmin, "TimeToFix min*");
    _LIT8(KTTFmax, "TimeToFix max*");
    const TUint8 KPosDataSeparator = '=';
    const TUint8 KPosDefaultDataEndMarker = ';';
	const TInt KToMs = 1000000;

    ReadSimDataFromFileL(aSimFile);

	TInt powerUpTime = 0;
	TInt ttfMin = 0;
	TInt ttfMax = 0;
    for (TInt i = 0; i < iSimDataArray->Count(); i++)
        {
        if (iSimDataArray->MdcaPoint(i).MatchF(KPowerUpTime) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(powerUpTime);
            }                
        if (iSimDataArray->MdcaPoint(i).MatchF(KTTFmin) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(ttfMin);
            }                
        if (iSimDataArray->MdcaPoint(i).MatchF(KTTFmax) 
                != KErrNotFound)
            {
            TDesTokeniser tokeniser(iSimDataArray->MdcaPoint(i));
            tokeniser.NextToken(KPosDataSeparator);
            TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);                        
            TLex8	lexer(value);
            lexer.Val(ttfMax);
            }                
        }
	if (powerUpTime == 0)
		{
		User::Leave(KErrGeneral);
		}
	if (ttfMin == 0)
		{
		User::Leave(KErrGeneral);
		}
	if (ttfMax == 0)
		{
		User::Leave(KErrGeneral);
		}

	TTimeIntervalMicroSeconds ttfMinMs;
	TTimeIntervalMicroSeconds ttfMaxMs;
	if (aPowerUp)
		{
		ttfMinMs = ttfMin * KToMs - aTolerance.Int64() + powerUpTime * KToMs;
		ttfMaxMs = ttfMax * KToMs + aTolerance.Int64() + powerUpTime * KToMs;	
		}
	else 
		{
		ttfMinMs = ttfMin * KToMs - aTolerance.Int64();
		ttfMaxMs = ttfMax * KToMs + aTolerance.Int64();
		}

	if (ttfMinMs.Int64() > aReqTime.Int64() || 
		ttfMaxMs.Int64() < aReqTime.Int64())
		{
		TBuf<50> output;
        _LIT(KTTFTime, "TTFMin %d, TTFmax %d. TimeToFix was: %d");
        output.Format(KTTFTime, ttfMinMs.Int64(), ttfMaxMs.Int64(), aReqTime.Int64());
		LogErrorAndFail(output);
		// User::Leave(KErrGeneral);	
		}

    TPosition pos;
    aPos.GetPosition(pos);
    TTime now;
    now.UniversalTime();
    TInt nowInt = now.Int64();
    TInt reqIntDiff = (aReqTime.Int64() * 80) / 100;
    TInt posInt = pos.Time().Int64();

    if (nowInt != posInt && (nowInt - reqIntDiff) > posInt)
        {
        _LIT(KTime, "Position fix was made before 80% of requesttime.");
		LogErrorAndFail(KTime);
        }
    }

// ---------------------------------------------------------
// CSimPsyStep::CheckThatPositionExistsL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyStep::CheckThatPositionExistsL(TPositionInfoBase& aInfo)
    {
    if (!&aInfo || aInfo.ModuleId().iUid != KSimModuleId)
        {
        _LIT(KNoPosition, "Request did not return a position");
        ERR_PRINTF1(KNoPosition);
        User::Leave(KErrGeneral);
        }

    if (aInfo.ModuleId().iUid != KSimModuleId)
        {
        _LIT(KWrongUid, "Expected simulationpsy uid");
        LogErrorAndFail(KWrongUid);
        }
	if (aInfo.UpdateType() != EPositionUpdateGeneral)
        {
        _LIT(KWrongUpType, "Expected Update Type General");
        LogErrorAndFail(KWrongUpType);
        }

    }

// ---------------------------------------------------------
// CSimPsyStep::ReadSimDataFromFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::ReadSimDataFromFileL(const TDesC& aFileName)
    {           
    iFileReader->CloseResources();
    iSimDataArray->Reset();
    User::LeaveIfError(iFileReader->PrepareForRead(aFileName));        
    iFileReader->ReadL(iSimDataArray, 10, EFalse);    
    iFileReader->CloseResources();
    }

// ---------------------------------------------------------
// CSimPsyStep::SetSimDataFileL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyStep::SetSimDataFileL(const TDesC& aFileName)
    {
    // Third version of settings simulation psy file
	TFileName fileName = aFileName;
    CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
    User::LeaveIfError(repository->Set(KCRKeySimPSYSimulationFile, fileName));
    CleanupStack::PopAndDestroy(repository);
    }


TInt CSimPsyStep::Replace(
    const TDesC& aFilename,
    const TInt aFileMode)
    {
    _LIT(KIniFileBasePathFmt, "\\system\\data\\%S.ini");
    const TInt KPosIniFileNameMaxLength = 40;
    RFs fsSession;
    RFile file;
    TInt    err = KErrNone;
    TBuf<KPosIniFileNameMaxLength>     filename;

    filename.Format(KIniFileBasePathFmt, &aFilename);
    if ((err = fsSession.Connect()) != KErrNone)
        {
        return err;
        }
    if ((err = file.Replace(fsSession, filename, aFileMode)) != KErrNone)
        {
        fsSession.Close();
        return err;
        }
    file.Close();
    fsSession.Close();
    
    return KErrNone;
    }

// End of File
