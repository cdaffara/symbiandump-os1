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
 @file SimPsyNmeaPositionerStep.cpp
 @internalTechnology
*/
#include "simpsynmeapositionerstep.h"
#include "te_lbssimulationpsysuitedefs.h"

#include "CSimPsyMultiNmeaPosRequester.h"


#include <lbs.h>
#include <lbssatellite.h>

// A few useful macros
#define MAX(x,y)      ((x)<(y))?(y):(x)
#define MIN(x,y)      ((x)<(y))?(x):(y)

// CONSTANTS
_LIT(KNmeaFile, "c:\\system\\data\\nmea.nme");
_LIT(KNmeaShortFile, "c:\\system\\data\\nmea_short.nme");
_LIT(KWrongFileType, "c:\\system\\data\\simfile.wng");
_LIT(KNmeaSatelliteFile, "c:\\system\\data\\nmea_garmin3plus.nme");
_LIT(KNmeaMagellanFile, "c:\\system\\data\\nmea_Magellantracker.nme");
_LIT(KNmeaPdopMissingFile, "c:\\system\\data\\nmea_pdopmissing.nme");
_LIT(KNmeaPartialUpdateFile, "c:\\system\\data\\nmea_partial_updates.nme");
_LIT(KNmeaError1, "c:\\system\\data\\nmea_error1.nme");
_LIT(KNmeaError2, "c:\\system\\data\\nmea_error2.nme");
_LIT(KNmeaError3, "c:\\system\\data\\nmea_error3.nme");
_LIT(KNmeaSpecialCase, "c:\\system\\data\\nmea_special_case.nme");

const TInt32 KSimModuleId = 270498433;


CSimPsyNmeaPositionerStep::~CSimPsyNmeaPositionerStep()
/**
 * Destructor
 */
	{
    iFileReader->CloseResources();    
    delete iSimDataArray; 
    iSimDataArray = NULL;
    delete iFileReader;
    iFileReader = NULL;

#ifdef __CTC__ 
    #pragma CTC APPEND
#endif
	}

CSimPsyNmeaPositionerStep::CSimPsyNmeaPositionerStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSimPsyNmeaPositionerStep);
	}

TVerdict CSimPsyNmeaPositionerStep::doTestStepPreambleL()
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
	iFileReader = CPosFileReader::NewL();  
	return TestStepResult();
	}


TVerdict CSimPsyNmeaPositionerStep::doTestStepL()
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



TVerdict CSimPsyNmeaPositionerStep::doTestStepPostambleL()
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
// CSimPsyNmeaPositionerStep::FixSettingsForSimPSY
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CSimPsyNmeaPositionerStep::FixSettingsForSimPSY(const TDesC& aFileName) 
	{	
    // Third version of settings simulation psy file
	TFileName fileName = aFileName;
    CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
    User::LeaveIfError(repository->Set(KCRKeySimPSYSimulationFile, fileName));
    CleanupStack::PopAndDestroy(repository);
	}


// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CSimPsyNmeaPositionerStep::StartL(TInt aIndex)
    {
    switch (aIndex)
        {                   
        case 0:
        	_LIT(KSingleNmeaRequestTest, "**** Single nmea request test ****");
        	INFO_PRINTF1(KSingleNmeaRequestTest);
        	FixSettingsForSimPSY(KNmeaFile); 
        	//TestSingleNmeaRequestL(); 
        	HeapTestL(aIndex);
        	break;                        
        case 1:
        	_LIT(KSingleNmeaCancelTest, "**** Single nmea cancel test ****");
        	INFO_PRINTF1(KSingleNmeaCancelTest);
        	FixSettingsForSimPSY(KNmeaFile); 
        	CancelSingleNmeaRequestL(); 
        	break;  
        case 2: 
        	_LIT(KMultCancelTest, "****  Multiple cancel test 1 ****");
        	INFO_PRINTF1(KMultCancelTest);
        	FixSettingsForSimPSY(KNmeaFile); 
        	TestMultipleNmeaCancelTestL(EFalse, EFalse, ETrue); 
        	break;  
        case 3: 
        	_LIT(KMultCancelTest2, "**** Multiple cancel test 2 ****");
        	INFO_PRINTF1(KMultCancelTest2);
        	FixSettingsForSimPSY(KNmeaFile); 
        	TestMultipleNmeaCancelTestL(ETrue, EFalse, ETrue); 
        	break;
        case 4: 
        	_LIT(KMultCancelTest3, "**** Multiple cancel test 3 ****");
            INFO_PRINTF1(KMultCancelTest3);
        	FixSettingsForSimPSY(KNmeaFile); 
        	TestMultipleNmeaCancelTestL(ETrue, ETrue, ETrue); 
        	break;                        
        case 5:
        	_LIT(KMultipleRequestsMemoryTestName, "**** Multiple req memory leak test ****");
        	INFO_PRINTF1(KMultipleRequestsMemoryTestName);
        	FixSettingsForSimPSY(KNmeaFile); 
        	//TestMultipleRequestsL(10); 
        	HeapTestL(aIndex);
        	break;        
        case 6: 
        	_LIT(KMultipleRequestsAndCancelMemoryTestName, "**** Multiple req and cancel memory leak test ****");
            INFO_PRINTF1(KMultipleRequestsAndCancelMemoryTestName);
        	FixSettingsForSimPSY(KNmeaFile); 
        	TestMultipleRequestsAndCancelL(5, EFalse, ETrue, EFalse); 
        	break;        
        case 7:	
        	_LIT(KRequestNmeaData, "**** Request Nmea data ****");
        	INFO_PRINTF1(KRequestNmeaData);
        	FixSettingsForSimPSY(KNmeaFile); 
        	//TestRequestNmeaDataL(); 
        	HeapTestL(aIndex);
        	break;                        
        case 8: 
        	_LIT(KReadUntilEof, "**** Read until eof ****");
        	INFO_PRINTF1(KReadUntilEof);
        	//TestReadUntilEofL(); 
        	HeapTestL(aIndex);
        	break;                        
        case 9:
        	_LIT(KWrongPsyMode, "**** Unknown PSY mode ****"); 
        	INFO_PRINTF1(KWrongPsyMode);
        	TestWrongSimModeL(); 
        	break;  
        case 10: 
        	_LIT(KRequestCourseInfo, "**** Request course info ****");
        	INFO_PRINTF1(KRequestCourseInfo);
        	TestRequestCourseInfoL(); 
        	break;    
        case 11: 
        	_LIT(KRequestSatelliteInfo, "**** Request satellite info ****");
        	INFO_PRINTF1(KRequestSatelliteInfo);
        	TestRequestSatelliteInfoL(); 
        	break;
        case 12: 
        	_LIT(KRequestGenericInfo, "**** Request generic info ****");
        	INFO_PRINTF1(KRequestGenericInfo);
        	//TestRequestGenericInfoL(); 
        	HeapTestL(aIndex);
        	break;
        case 13: 
        	_LIT(KRequestBasicInfo, "**** Request basic info ****");
        	INFO_PRINTF1(KRequestBasicInfo);
        	TestRequestBasicInfoL(); 
        	break;
        case 14: 
        	_LIT(KRequestMagellan, "**** Request Magellan ****");
        	INFO_PRINTF1(KRequestMagellan);
        	TestRequestMagellanL(); 
        	break;
        case 15: 
        	_LIT(KRequestPartialGenericInfo, "**** Request partial generic info ****");
        	INFO_PRINTF1(KRequestPartialGenericInfo);
        	TestRequestPartialGenericInfoL(); 
        	break;
        case 16: 
        	_LIT(KRequestClearAndSetDefaultPositionData, "**** Test Clear Position Data ****");
        	INFO_PRINTF1(KRequestClearAndSetDefaultPositionData);
        	//TestClearAndSetDefaultPositionDataL(); 
        	HeapTestL(aIndex);
        	break;
        case 17: 
        	_LIT(KTestPartialUpdate, "**** Test Partial Update ****");
        	INFO_PRINTF1(KTestPartialUpdate);
        	TestPartialUpdateL(); 
        	break;
        case 18: 
        	_LIT(KTestErrorSituations, "**** Test wrong situations ****");
        	INFO_PRINTF1(KTestErrorSituations);
        	TestErrorSituationsL(); 
        	break;
        case 19: 
        	_LIT(KTestSpecialCase, "**** Test Special Case ****");
        	INFO_PRINTF1(KTestSpecialCase);
        	TestSpecialCasesL(); 
        	break;

        // Not a real test only a helper method for printing checksums
        default: 
        	break;
        }
    }
  
void CSimPsyNmeaPositionerStep::DoTestL(TInt aIndex)
    {
    switch (aIndex)
        {                   
        case 0: TestSingleNmeaRequestL(); break;                        
        case 1: CancelSingleNmeaRequestL(); break;  
        case 2: TestMultipleNmeaCancelTestL(EFalse, EFalse, ETrue); break;  
        case 3: TestMultipleNmeaCancelTestL(ETrue, EFalse, ETrue); break;
        case 4: TestMultipleNmeaCancelTestL(ETrue, ETrue, ETrue); break;                        
        case 5: TestMultipleRequestsL(10); break;        
        case 6: TestMultipleRequestsAndCancelL(5, EFalse, ETrue, EFalse); break;        
        case 7: TestRequestNmeaDataL(); break;                        
        case 8: TestReadUntilEofL(); break;                        
        case 9: TestWrongSimModeL(); break;  
        case 10: TestRequestCourseInfoL(); break;    
        case 11: TestRequestSatelliteInfoL(); break;
        case 12: TestRequestGenericInfoL(); break;
        case 13: TestRequestBasicInfoL(); break;
        case 14: TestRequestMagellanL(); break;
        case 15: TestRequestPartialGenericInfoL(); break;
        case 16: TestClearAndSetDefaultPositionDataL(); break;
        case 17: TestPartialUpdateL(); break;
        case 18: TestErrorSituationsL(); break;
        case 19: TestSpecialCasesL(); break;

        // Not a real test only a helper method for printing checksums
        default: break;
        }
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestSingleNmeaRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestSingleNmeaRequestL()
    {
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;    
    TTimeIntervalMicroSeconds requestTime;

    for (TInt i = 0; i < 5; i++)
        {
		if (i <= 2) 
			{
			TPositionInfo position = TPositionInfo();    
			requester->MakeRequests(&position);
			requester->GetResult(status, requestTime);
			CheckRequestResultL(status, position);

			if (!&position)
				{
				User::Leave(KErrGeneral);
				}
			TPosition pos;			
			position.GetPosition(pos);
			TTime time = pos.Time();
			TDateTime dtime = time.DateTime();
			if (pos.Latitude() < 0 || pos.Longitude() < 0 || dtime.Year() < 2000)
				{
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			HPositionGenericInfo* genposition = HPositionGenericInfo::NewLC();    
            User::LeaveIfError(genposition->SetRequestedField(EPositionFieldNMEASentences));
			requester->MakeRequests(genposition);
			requester->GetResult(status, requestTime); 
			CheckRequestResultL(status, *genposition);
			if (!&genposition && status == KErrNone)
				{
				User::Leave(KErrGeneral);
				}			
			TPosition genpos;			
			genposition->GetPosition(genpos);
			TTime time = genpos.Time();
			TDateTime dtime = time.DateTime();
			if (genpos.Latitude() < 0 || genpos.Longitude() < 0 || dtime.Year() < 2000)
				{
				User::Leave(KErrGeneral);
				}

            // Check that the burst starts with the same sentence all the time.
            // In the nmea test file, the first burst will be GLL.
            _LIT8(KExpectedFirstSentence, "$GPGLL");
            TPtrC8 firstNmeaString(NULL, 0);
            User::LeaveIfError(genposition->GetValue(EPositionFieldNMEASentencesStart, firstNmeaString));
            if (firstNmeaString.Left(6).Compare(KExpectedFirstSentence) != 0)
                {
                _LIT(KError, "Expected GPGLL sentence at the beginning of the burst");
                LogErrorAndFail(KError);
                }

			CleanupStack::PopAndDestroy(genposition);
			}
        }

    CleanupStack::PopAndDestroy(); // requester        
    _LIT(KSingleNmeaRequestTest, "Ended: **** Single nmea request test ****");
    INFO_PRINTF1(KSingleNmeaRequestTest);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::CancelSingleNmeaRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::CancelSingleNmeaRequestL()
    {        
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC();
    requester->SetRequesterWillCancelL();

    TPositionInfo position = TPositionInfo();
    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    
    requester->StopRequests();
    requester->MakeRequests(&position);
    requester->GetResult(status, requestTime);

    if (status != KErrCancel)
        {
        _LIT(KReqWasNotCancelled, "The request was not cancelled");
        LogErrorAndFail(KReqWasNotCancelled);
        }

    CleanupStack::PopAndDestroy(requester); // requester  
	_LIT(KSingleNmeaCancelTest, "Ended: **** Single nmea cancel test ****");
	INFO_PRINTF1(KSingleNmeaCancelTest);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestMultipleNmeaCancelTestL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestMultipleNmeaCancelTestL(
    TBool aFirstWillCancel,
    TBool aSecondWillCancel,
    TBool aThirdWillCancel)
    {    
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC(3);
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

    TPositionInfo position = TPositionInfo();    
    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    
    requester->MakeRequests(&position);

    for (TInt i = 0; i < 3; i++)
        {
        requester->GetResult(status, requestTime, i);

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
			}
        else
            {
            if (status != KErrNone)
                {
                _LIT(KReqFailed, "The request failed");
                LogErrorAndFail(KReqFailed);
                }
            if (!&position)
                {
                _LIT(KHasNoPosition, "The request did not return a position");
                LogErrorAndFail(KHasNoPosition);        
                }            
            }
        }

    CleanupStack::PopAndDestroy(); // requester    
	_LIT(KMultCancelTest, "Ended: ****  Multiple cancel test ****");
	INFO_PRINTF1(KMultCancelTest);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestMultipleRequestsL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestMultipleRequestsL(TInt aNumberOfRequests)
    {
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TPositionInfo position = TPositionInfo();        
    TTimeIntervalMicroSeconds requestTime;    
    for (TInt i = 0; i < aNumberOfRequests; i++)
        {
        // RDebug::Print(_L("TestMultipleRequestsL making request"));
        requester->MakeRequests(&position);
        requester->GetResult(status, requestTime);   
		CheckRequestResultL(status, position);		
        }
    CleanupStack::PopAndDestroy(); // requester        
    requester = NULL;  
	_LIT(KMultipleRequestsMemoryTestName, "Ended: **** Multiple req memory leak test ****");
	INFO_PRINTF1(KMultipleRequestsMemoryTestName);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestMultipleRequestsAndCancelL
// (other items were commented in a header).
// ---------------------------------------------------------
//      
void CSimPsyNmeaPositionerStep::TestMultipleRequestsAndCancelL(
    TInt aNumberOfRequests, TBool aFirstWillCancel,
    TBool aSecondWillCancel, TBool aThirdWillCancel)
    {    
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC(3);
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

    TPositionInfo position = TPositionInfo();
    TInt status;    
    TTimeIntervalMicroSeconds requestTime;        

    for (TInt i = 0; i < aNumberOfRequests; i++)
        {
        requester->MakeRequests(&position);

        for (TInt j = 0; j < 3; j++)
            {
            requester->GetResult(status, requestTime, j);
            if (status == KErrNoMemory)
                {
                User::Leave(status);
                }
            }
        }
    CleanupStack::PopAndDestroy(); // requester  
	_LIT(KMultipleRequestsAndCancelMemoryTestName, "Ended: **** Multiple req and cancel memory leak test ****");
    INFO_PRINTF1(KMultipleRequestsAndCancelMemoryTestName);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestNmeaDataL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestNmeaDataL()
    {
	CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    for (TInt i = 0; i < 3; i++)
        {
		HPositionGenericInfo* position = HPositionGenericInfo::NewLC();
		TReal32 trueCourse = TReal32(30.00);
		TInt err = position->SetValue(EPositionFieldTrueCourse, trueCourse);
		
		position->SetRequestedField(EPositionFieldNMEASentences);
		req->MakeRequests(position);
        req->GetResult(status, requestTime);          
		
		err = position->GetValue(EPositionFieldTrueCourse, trueCourse);
		if (err != KErrNotFound) 
			{
			User::Leave(KErrGeneral);	
			}
		CheckRequestResultL(status, *position);

		if (status != KErrNone)
			{
			User::Leave(KErrGeneral);
			}	
		TUint8 sentences;
		err = position->GetValue(EPositionFieldNMEASentences, sentences); 
		User::LeaveIfError(err);
		if (sentences <= 0)
			{
			User::Leave(KErrGeneral);
			}

		TUint8 upper;
		TUint16 sent = static_cast<TUint16> (EPositionFieldNMEASentencesStart+sentences); 
		err = position->GetValue(sent, upper);
		if (err != KErrNotFound)
			{
			User::Leave(KErrGeneral);
			}

		TBuf8<200> buf;
		for (TInt j = 0; j < sentences; j++)
			{
			TUint16 sentnr = static_cast<TUint16> (EPositionFieldNMEASentencesStart+j); 
			err = position->GetValue(sentnr, buf); 
			User::LeaveIfError(err);
		    if (buf.Compare(KNullDesC8) == 0) 
				{
				User::Leave(KErrGeneral);
				}
			}	
		TBool mustbetrue = position->IsFieldAvailable(EPositionFieldNMEASentences); 
		if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
		CleanupStack::PopAndDestroy(position);
		}
    CleanupStack::PopAndDestroy(req); // requester   
	_LIT(KRequestNmeaData, "Ended: **** Request Nmea data ****");
	INFO_PRINTF1(KRequestNmeaData);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestReadUntilEofL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestReadUntilEofL()
    {
	FixSettingsForSimPSY(KNmeaShortFile);
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;    
    TTimeIntervalMicroSeconds requestTime;

	// TPositionInfo position = TPositionInfo();    
	HPositionGenericInfo* position = HPositionGenericInfo::NewLC();    
    for (TInt i = 0; i < 3; i++)
        {
		requester->MakeRequests(position);
		requester->GetResult(status, requestTime); 
        if (status == KErrNoMemory)
			{
			User::Leave(status);
			}
        else if (status != KErrNotSupported)
			{
			User::Leave(KErrGeneral);
			}
        }
    CleanupStack::PopAndDestroy(2, requester); // position
	_LIT(KReadUntilEof, "Ended: **** Read until eof ****");
	INFO_PRINTF1(KReadUntilEof);
    }


// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestWrongSimModeL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestWrongSimModeL()
    {
	FixSettingsForSimPSY(KWrongFileType);
	CSimPsyMultiNmeaPosRequester* req = NULL;
    TRAPD(err, req = CSimPsyMultiNmeaPosRequester::NewLC());

	if ((err != KErrNotSupported) || (req!= NULL))
		{
		User::Leave(KErrGeneral);
		}
	_LIT(KWrongPsyMode, "Ended: **** Unknown PSY mode ****"); 
	INFO_PRINTF1(KWrongPsyMode);
    }


// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestCourseInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestCourseInfoL()
    {
    // correct NMEA file
    FixSettingsForSimPSY(KNmeaSatelliteFile);
    
    const TInt KNORequests = 3;
	CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    //expected values for course info parsed from nmea.nme
    const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;
    const TReal32 KExpectedCourse = 360.0F;
    const TReal32 KExpectedSpeed = KKnotsToMetrePerSecFactor * 0.0F; 

    for (TInt i = 0; i < KNORequests; i++)
        {
		TPositionCourseInfo position = TPositionCourseInfo();
		
		req->MakeRequests(&position);
        req->GetResult(status, requestTime);  
        
        CheckRequestResultL(status, position);

        // check data from returned course-info object
        TCourse course;
        position.GetCourse(course);

        if (course.Speed() != KExpectedSpeed)
            {
            User::Leave(KErrGeneral);                
            }
        if (course.Heading() != KExpectedCourse)
            {
            User::Leave(KErrGeneral);                
            }
		}
    CleanupStack::PopAndDestroy(req); // requester        
	_LIT(KRequestCourseInfo, "Ended: **** Request course info ****");
	INFO_PRINTF1(KRequestCourseInfo);
    }


// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestSatelliteInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestSatelliteInfoL()
    {
    // correct NMEA file
    FixSettingsForSimPSY(KNmeaSatelliteFile);

    const TInt KNORequests = 3;
	CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    // expected values for satellite info parsed from KNmeaSatelliteFile
    // RMC
    const TInt KExpectedYear = 2003;
    const TMonth KExpectedMonth = EApril;
    const TInt KExpectedDay = 1;
    const TInt KExpectedHour = 14;
    const TInt KExpectedMin[KNORequests] = {18, 19, 19};
    const TInt KExpectedSec[KNORequests] = {59, 1, 3};
    const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;
    const TReal32 KExpectedCourse = 360.0F;
    const TReal32 KExpectedSpeed = KKnotsToMetrePerSecFactor * 0.0F; 
    // GSA    
    const TInt KExpectedSatUsed = 6;
    const TInt KExpectedSatInView = 10;
        // IDs of satellites used in solution
    const TInt KExpectedUsedSatId[KExpectedSatUsed] = {7,9,11,18,23,28};
    const TReal32 KExpectedHDOP= 2.2F;
    const TReal32 KExpectedVDOP= 2.7F;
    // GSV
        // IDs of satellites in view
    const TInt KExpectedInViewSatId[KExpectedSatInView] = {7,9,11,18,23,28,3,14,20,31};
    const TReal32 KExpectedElevation[KExpectedSatInView] = {2,16,57,8,3,19,10,51,9,30};
    const TReal32 KExpectedAzimuth[KNORequests][KExpectedSatInView] = 
        {
        {331,26,267,66,64,301,179,106,240,196},
        {331,26,267,66,64,301,179,105,240,196},
        {331,26,267,66,64,301,179,105,240,196}
        };

    const TInt KExpectedSR[KNORequests][KExpectedSatInView] = 
        {
        {42,47,44,39,38,46,0,33,0,0},
        {42,48,44,39,38,45,0,33,0,0}, 
        {42,48,44,39,38,45,0,34,0,0}
        };


    for (TInt i = 0; i < KNORequests; i++)
        {
		TPositionSatelliteInfo position = TPositionSatelliteInfo();

		req->MakeRequests(&position);
        req->GetResult(status, requestTime);  
        
        CheckRequestResultL(status, position);
        
        // check data from returned satellite-info object...
        // from RMC
        TTime sattime = position.SatelliteTime();
        TDateTime dtime = sattime.DateTime();
        TInt year = dtime.Year();
        TMonth month = dtime.Month();
        TInt day = dtime.Day();
        TInt hour = dtime.Hour();
        TInt min = dtime.Minute();
        TInt sec = dtime.Second();

        if (year != KExpectedYear)
            {
            User::Leave(KErrGeneral);                
            }
        if (month != KExpectedMonth)
            {
            User::Leave(KErrGeneral);                
            }
        if (day != KExpectedDay)
            {
            User::Leave(KErrGeneral);                
            }
        if (hour != KExpectedHour)
            {
            User::Leave(KErrGeneral);                
            }
        if (min != KExpectedMin[i])
            {
            User::Leave(KErrGeneral);                
            }
        if (sec != KExpectedSec[i])
            {
            User::Leave(KErrGeneral);                
            }

        TCourse course;
        position.GetCourse(course);

        if (course.Speed() != KExpectedSpeed)
            {
            User::Leave(KErrGeneral);                
            }
        if (course.Heading() != KExpectedCourse)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSA
        TInt satused = position.NumSatellitesUsed();
        TInt satinview = position.NumSatellitesInView();
        if (satused != KExpectedSatUsed)
            {
            User::Leave(KErrGeneral);                
            }
        if (satinview != KExpectedSatInView)
            {
            User::Leave(KErrGeneral);                
            }

        TSatelliteData satdata;
        TInt k = 0;
        TInt j;
        for (j = 0; j < satinview; j++)
            {
            TInt err = position.GetSatelliteData(j, satdata);
            if (err != KErrNone)
                {
                User::Leave(KErrGeneral);                
                }
            if (satdata.SatelliteId() == KExpectedUsedSatId[k] && satdata.IsUsed())
                {
                k++; 
                }
            }
        if (k != satused)
            {
            User::Leave(KErrGeneral); // correct n.o Ids used for fix NOT present
            }

        if (position.HorizontalDoP() != KExpectedHDOP)
            {
            User::Leave(KErrGeneral);
            }
        if (position.VerticalDoP() != KExpectedVDOP)
            {
            User::Leave(KErrGeneral);
            }

        // from GSV
        for (j = 0; j < satinview; j++)
            {
            TInt err = position.GetSatelliteData(j, satdata);
            if (err != KErrNone)
                {
                User::Leave(KErrGeneral);                
                }
            if (satdata.SatelliteId() != KExpectedInViewSatId[j])
                {
                User::Leave(KErrGeneral); 
                }
            if (satdata.Azimuth() != KExpectedAzimuth[i][j])
                {
                User::Leave(KErrGeneral); 
                }
            if (satdata.Elevation() != KExpectedElevation[j])
                {
                User::Leave(KErrGeneral); 
                }      
            if (satdata.SignalStrength() != KExpectedSR[i][j])
                {
                User::Leave(KErrGeneral); 
                }
            }
		}
    CleanupStack::PopAndDestroy(req); // requester        
	_LIT(KRequestSatelliteInfo, "Ended: **** Request satellite info ****");
	INFO_PRINTF1(KRequestSatelliteInfo);
    }


// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestGenericInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestGenericInfoL()
    {
    // correct NMEA file
    FixSettingsForSimPSY(KNmeaSatelliteFile);

    const TInt KNORequests = 3;
	CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    // expected values for satellite info parsed from nmea.nme
    // GGA
    const TReal32 KExpectedWGS84Alt[KNORequests] = 
        {169.5F, 169.8F, 169.6F}; 
    const TReal32 KExpectedGeodialSep = 20.7F;
    // RMC
    const TInt KExpectedYear = 2003;
    const TMonth KExpectedMonth = EApril;
    const TInt KExpectedDay = 1;
    const TInt KExpectedHour = 14;
    const TInt KExpectedMin[KNORequests] = {18, 19, 19};
    const TInt KExpectedSec[KNORequests] = {59, 1, 3};
    const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;
    const TReal32 KExpectedCourse = 360.0F;
    const TReal32 KExpectedSpeed = KKnotsToMetrePerSecFactor * 0.0F; 
    const TReal32 KExpectedMagCourse = 354.0F;
    // GSA
    const TInt KExpectedSatUsed = 6;
    const TReal32 KExpectedPDOP= 3.5F;
    const TReal32 KExpectedHDOP= 2.2F;
    const TReal32 KExpectedVDOP= 2.7F;
    // GSV
    const TInt8 KExpectedSatInView = 10;


    for (TInt i = 0; i < KNORequests; i++)
        {
		HPositionGenericInfo* position = HPositionGenericInfo::NewLC();
		
        // set requested fields..
        // GGA
		position->SetRequestedField(EPositionFieldSatelliteSeaLevelAltitude);		
        position->SetRequestedField(EPositionFieldSatelliteGeoidalSeparation);
        // RMC
        position->SetRequestedField(EPositionFieldSatelliteTime);
        position->SetRequestedField(EPositionFieldTrueCourse);
        position->SetRequestedField(EPositionFieldHorizontalSpeed);
        // position->SetRequestedField(EPositionFieldMagneticVariation);        
        position->SetRequestedField(EPositionFieldMagneticCourse);        
        // GSA
        position->SetRequestedField(EPositionFieldSatelliteNumUsed);
        position->SetRequestedField(EPositionFieldSatelliteHorizontalDoP);
        position->SetRequestedField(EPositionFieldSatelliteVerticalDoP);
        position->SetRequestedField(EPositionFieldSatellitePositionDoP);
        // GSV
        position->SetRequestedField(EPositionFieldSatelliteNumInView);

        req->MakeRequests(position);
        req->GetResult(status, requestTime);          

        CheckRequestResultL(status, *position);
        
        // check data from returned generic-info object...
        // from GGA
        TBool mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteSeaLevelAltitude); 
		if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 wgs84alt;
        position->GetValue(EPositionFieldSatelliteSeaLevelAltitude, 
                        wgs84alt);
        if (wgs84alt != KExpectedWGS84Alt[i])
                {
                User::Leave(KErrGeneral); 
                }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteGeoidalSeparation);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 geosep;
        position->GetValue(EPositionFieldSatelliteGeoidalSeparation, 
                        geosep);
        if (geosep != KExpectedGeodialSep)
                {
                User::Leave(KErrGeneral); 
                }

        // from RMC
        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteTime);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TTime sattime;
        position->GetValue(EPositionFieldSatelliteTime, 
                        sattime);
        TDateTime dtime = sattime.DateTime();
        TInt year = dtime.Year();
        TMonth month = dtime.Month();
        TInt day = dtime.Day();
        TInt hour = dtime.Hour();
        TInt min = dtime.Minute();
        TInt sec = dtime.Second();
        if (year != KExpectedYear)
            {
            User::Leave(KErrGeneral);                
            }
        if (month != KExpectedMonth)
            {
            User::Leave(KErrGeneral);                
            }
        if (day != KExpectedDay)
            {
            User::Leave(KErrGeneral);                
            }
        if (hour != KExpectedHour)
            {
            User::Leave(KErrGeneral);                
            }
        if (min != KExpectedMin[i])
            {
            User::Leave(KErrGeneral);                
            }
        if (sec != KExpectedSec[i])
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldTrueCourse);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 trueCourse;
        position->GetValue(EPositionFieldTrueCourse, 
                        trueCourse);
        if (trueCourse != KExpectedCourse)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldHorizontalSpeed);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 speed;
        position->GetValue(EPositionFieldHorizontalSpeed, 
                        speed);
        if (speed != KExpectedSpeed)
            {
            User::Leave(KErrGeneral);                
            }
        mustbetrue = position->IsFieldAvailable(EPositionFieldMagneticCourse);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        // TReal32 magvar;
        TReal32 magCourse;
        position->GetValue(EPositionFieldMagneticCourse, 
                        magCourse);
        if (magCourse != KExpectedMagCourse)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSA
        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteNumUsed);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TInt8 numUsedSatellites;
        position->GetValue(EPositionFieldSatelliteNumUsed, 
                        numUsedSatellites);
        if (numUsedSatellites != KExpectedSatUsed)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteHorizontalDoP);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 hdop;
        position->GetValue(EPositionFieldSatelliteHorizontalDoP, 
                        hdop);
        if (hdop != KExpectedHDOP)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteVerticalDoP);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 vdop;
        position->GetValue(EPositionFieldSatelliteVerticalDoP, 
                        vdop);
        if (vdop != KExpectedVDOP)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatellitePositionDoP);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 pdop;
        position->GetValue(EPositionFieldSatellitePositionDoP, 
                        pdop);
        if (pdop != KExpectedPDOP)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSV
        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteNumInView);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TInt8 satinview;
        position->GetValue(EPositionFieldSatelliteNumInView, 
                        satinview);
        if (satinview != KExpectedSatInView)
            {
            User::Leave(KErrGeneral);                
            }


        CleanupStack::PopAndDestroy(position); // position 
		}
    CleanupStack::PopAndDestroy(req); // requester    
	_LIT(KRequestGenericInfo, "Ended: **** Request generic info ****");
	INFO_PRINTF1(KRequestGenericInfo);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestBasicInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestBasicInfoL()
    {
    // correct NMEA file
    FixSettingsForSimPSY(KNmeaSatelliteFile);

    const TInt KNORequests = 3;
	CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    // expected values for basic info parsed from nmea.nme
    // GGA
    const TReal32 KExpectedAlt[KNORequests] = 
    {169.5F + 20.7F, 169.8F + 20.7F, 169.6F + 20.7F}; 
    // GSA
    const TReal32 KPosUERE = 10.0;
    const TReal32 KExpectedHDOP= 2.2F;
    const TReal32 KExpectedHAcc = KPosUERE * KExpectedHDOP;
    const TReal32 KExpectedVAcc = 1.5F * KPosUERE * KExpectedHDOP;

    for (TInt i = 0; i < KNORequests; i++)
        {
        TPositionInfo position = TPositionInfo();    
		
        requester->MakeRequests(&position);
		requester->GetResult(status, requestTime);
		CheckRequestResultL(status, position);

		if (!&position)
			{
			User::Leave(KErrGeneral);
			}

		TPosition pos;			
		position.GetPosition(pos);

        TTime time = pos.Time();
		TDateTime dtime = time.DateTime();

        if (pos.Altitude() != KExpectedAlt[i])
            {
            User::Leave(KErrGeneral);                
            }
		TTime now;
		now.UniversalTime();
		TInt year = now.DateTime().Year();
        if (dtime.Year() != year)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSA        
        if (pos.HorizontalAccuracy() != KExpectedHAcc)
            {
            User::Leave(KErrGeneral);                
            }
        if (pos.VerticalAccuracy() != KExpectedVAcc)
            {
            User::Leave(KErrGeneral);                
            }
        }
    CleanupStack::PopAndDestroy(requester); // requester 
	_LIT(KRequestBasicInfo, "Ended: **** Request basic info ****");
	INFO_PRINTF1(KRequestBasicInfo);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestMagellanL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestMagellanL()
    {
    // correct NMEA file
    FixSettingsForSimPSY(KNmeaMagellanFile);

    const TInt KNORequests = 3;
	CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    TRealX nan;
    nan.SetNaN();

    // expected values for satellite info parsed from KNmeaSatelliteFile
    // RMC
    const TInt KExpectedYear = 2001;
    const TMonth KExpectedMonth = EMay;
    const TInt KExpectedDay = 14;
    const TInt KExpectedHour = 9;
    const TInt KExpectedMin[KNORequests] = {24, 24, 24};
    const TInt KExpectedSec[KNORequests] = {38, 40, 42};
    const TInt KExpectedMSec[KNORequests] = {620000, 770000, 50000};
    const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;
    const TReal32 KExpectedCourse = 0.0F;
    const TReal32 KExpectedSpeed = KKnotsToMetrePerSecFactor * 0.0F; 
    // GSA    
    const TInt KExpectedSatUsed = 3;
    const TInt KExpectedSatInView = 12;
        // IDs of satellites used in solution
    const TInt KExpectedUsedSatId[KExpectedSatUsed] = {18,15,17};
    const TReal32 KExpectedHDOP= 4.5F;
    const TReal32 KExpectedVDOP= nan;
    // GSV
        // IDs of satellites in view
    const TInt KExpectedInViewSatId[KExpectedSatInView] = {18,15,17,3,22,28,26,27,21,6,8,10};
    const TReal32 KExpectedElevation[KExpectedSatInView] = 
        {40,78,62,48,43,32,18,16,13,10,10,6};
    const TReal32 KExpectedAzimuth[KNORequests][KExpectedSatInView] = 
        {
        {150,143,85,279,224,308,70,332,182,119,356,30},
        {150,143,85,279,224,308,70,332,182,119,356,30},
        {150,143,85,279,224,308,70,332,182,119,356,30}
        };
    const TInt KExpectedSR[KNORequests][KExpectedSatInView] = 
        {
        {49,54,53,49,47,46,44,44,0,0,0,0},
        {49,54,53,49,47,46,43,43,0,0,0,0},
        {49,54,53,49,47,46,44,42,0,0,0,0}
        };


    for (TInt i = 0; i < KNORequests; i++)
        {
		TPositionSatelliteInfo position = TPositionSatelliteInfo();

		req->MakeRequests(&position);
        req->GetResult(status, requestTime);  
        
        CheckRequestResultL(status, position);
        
        // check data from returned satellite-info object...
        // from RMC
        TTime sattime = position.SatelliteTime();
        TDateTime dtime = sattime.DateTime();
        TInt year = dtime.Year();
        TMonth month = dtime.Month();
        TInt day = dtime.Day();
        TInt hour = dtime.Hour();
        TInt min = dtime.Minute();
        TInt sec = dtime.Second();
        TInt msec = dtime.MicroSecond();

        if (year != KExpectedYear)
            {
            User::Leave(KErrGeneral);                
            }
        if (month != KExpectedMonth)
            {
            User::Leave(KErrGeneral);                
            }
        if (day != KExpectedDay)
            {
            User::Leave(KErrGeneral);                
            }
        if (hour != KExpectedHour)
            {
            User::Leave(KErrGeneral);                
            }
        if (min != KExpectedMin[i])
            {
            User::Leave(KErrGeneral);                
            }
        if (sec != KExpectedSec[i])
            {
            User::Leave(KErrGeneral);                
            }
        if (msec != KExpectedMSec[i])
            {
            User::Leave(KErrGeneral);                
            }

        TCourse course;
        position.GetCourse(course);

        if (course.Speed() != KExpectedSpeed)
            {
            User::Leave(KErrGeneral);                
            }
        if (course.Heading() != KExpectedCourse)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSA
        TInt satused = position.NumSatellitesUsed();
        TInt satinview = position.NumSatellitesInView();
        if (satused != KExpectedSatUsed)
            {
            User::Leave(KErrGeneral);                
            }
        if (satinview != KExpectedSatInView)
            {
            User::Leave(KErrGeneral);                
            }

        TSatelliteData satdata;
        TInt k = 0;
        TInt j;
        for (j = 0; j < satinview; j++)
            {
            TInt err = position.GetSatelliteData(j, satdata);
            if (err != KErrNone)
                {
                User::Leave(KErrGeneral);                
                }
            if (satdata.SatelliteId() == KExpectedUsedSatId[k] && satdata.IsUsed())
                {
                k++; 
                }
            }
        if (k != satused)
            {
            User::Leave(KErrGeneral); // correct n.o Ids used for fix NOT present
            }

        if (position.HorizontalDoP() != KExpectedHDOP)
            {
            User::Leave(KErrGeneral);
            }        
        if (!Math::IsNaN(position.VerticalDoP()))
            {
            User::Leave(KErrGeneral);
            }

        // from GSV
        for (j = 0; j < satinview; j++)
            {
            TInt err = position.GetSatelliteData(j, satdata);
            if (err != KErrNone)
                {
                User::Leave(KErrGeneral);                
                }
            if (satdata.SatelliteId() != KExpectedInViewSatId[j])
                {
                User::Leave(KErrGeneral); 
                }
            if (satdata.Azimuth() != KExpectedAzimuth[i][j])
                {
                User::Leave(KErrGeneral); 
                }
            if (satdata.Elevation() != KExpectedElevation[j])
                {
                User::Leave(KErrGeneral); 
                }      
            if (satdata.SignalStrength() != KExpectedSR[i][j])
                {
                User::Leave(KErrGeneral); 
                }
            }
		}
	
	CleanupStack::PopAndDestroy(req); // requester

    // correct NMEA file
    FixSettingsForSimPSY(KNmeaPdopMissingFile);

    // No pdop value in .nme file
	req = CSimPsyMultiNmeaPosRequester::NewLC();
    for (TInt j = 0; j < KNORequests; j++)
        {
		HPositionGenericInfo* genPos = HPositionGenericInfo::NewL();
		CleanupStack::PushL(genPos);

        // GSA
        genPos->SetRequestedField(EPositionFieldSatelliteNumUsed);
        genPos->SetRequestedField(EPositionFieldSatelliteHorizontalDoP);
        genPos->SetRequestedField(EPositionFieldSatelliteVerticalDoP);
        genPos->SetRequestedField(EPositionFieldSatellitePositionDoP);

		req->MakeRequests(genPos);
        req->GetResult(status, requestTime);  
        
        CheckRequestResultL(status, *genPos);

        TReal32 vdop;
		const TReal32 KExpectedVDOP= 0.9F;
        genPos->GetValue(EPositionFieldSatelliteVerticalDoP, vdop);
        if (vdop != KExpectedVDOP)
            {
            User::Leave(KErrGeneral);                
            }
        TReal32 hdop;
		const TReal32 KExpectedHDOP= 4.0F;
        genPos->GetValue(EPositionFieldSatelliteHorizontalDoP, hdop);
        if (hdop != KExpectedHDOP)
            {
            User::Leave(KErrGeneral);                
            }

        TReal32 pdop;
		TReal expectedPDOP;
		TInt err = Math::Sqrt(expectedPDOP, hdop * hdop + vdop * vdop);
		User::LeaveIfError(err);
        genPos->GetValue(EPositionFieldSatellitePositionDoP, pdop);
		TReal32 expectedPDOPreal32 = TReal32(expectedPDOP); 
        if (TRealX(pdop) != TRealX(expectedPDOPreal32))
            {
            User::Leave(KErrGeneral);                
            }

		CleanupStack::PopAndDestroy(genPos);
		}

	CleanupStack::PopAndDestroy(req); // requester
	_LIT(KRequestMagellan, "Ended: **** Request Magellan ****");
	INFO_PRINTF1(KRequestMagellan);
	}

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestRequestPartialGenericInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestRequestPartialGenericInfoL()
    {
    // correct NMEA file
    FixSettingsForSimPSY(KNmeaSatelliteFile);

    const TInt KNORequests = 3;
	CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    // expected values for satellite info parsed from nmea.nme
    // GGA
    const TReal32 KExpectedWGS84Alt[KNORequests] = 
        {169.5F, 169.8F, 169.6F}; 
    //const TReal32 KExpectedGeodialSep = 20.7F;
    // RMC
    const TInt KExpectedYear = 2003;
    const TMonth KExpectedMonth = EApril;
    const TInt KExpectedDay = 1;
    const TInt KExpectedHour = 14;
    const TInt KExpectedMin[KNORequests] = {18, 19, 19};
    const TInt KExpectedSec[KNORequests] = {59, 1, 3};
    //const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;
    const TReal32 KExpectedCourse = 360.0F;
    //const TReal32 KExpectedSpeed = KKnotsToMetrePerSecFactor * 0.0F; 
    const TReal32 KExpectedMagCourse = 354.0F;
    // GSA
    const TInt KExpectedSatUsed = 6;
    const TReal32 KExpectedPDOP= 3.5F;
    const TReal32 KExpectedHDOP= 2.2F;
    //const TReal32 KExpectedVDOP= 2.7F;
    // GSV
    const TInt8 KExpectedSatInView = 10;


    for (TInt i = 0; i < KNORequests; i++)
        {
		HPositionGenericInfo* position = HPositionGenericInfo::NewLC();
		
        // set requested fields..
        // GGA
		position->SetRequestedField(EPositionFieldSatelliteSeaLevelAltitude);		

        // RMC
        position->SetRequestedField(EPositionFieldSatelliteTime);
        position->SetRequestedField(EPositionFieldTrueCourse);
      
        position->SetRequestedField(EPositionFieldMagneticCourse);        
        // GSA
        position->SetRequestedField(EPositionFieldSatelliteNumUsed);
        position->SetRequestedField(EPositionFieldSatelliteHorizontalDoP);
        position->SetRequestedField(EPositionFieldSatellitePositionDoP);
        // GSV
        position->SetRequestedField(EPositionFieldSatelliteNumInView);

        req->MakeRequests(position);
        req->GetResult(status, requestTime);          

        CheckRequestResultL(status, *position);
        
        // check data from returned generic-info object...
        // from GGA
        TBool mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteSeaLevelAltitude); 
		if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 wgs84alt;
        position->GetValue(EPositionFieldSatelliteSeaLevelAltitude, 
                        wgs84alt);
        if (wgs84alt != KExpectedWGS84Alt[i])
                {
                User::Leave(KErrGeneral); 
                }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteGeoidalSeparation);
        if (mustbetrue)
			{
			User::Leave(KErrGeneral);
			}

        // from RMC
        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteTime);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TTime sattime;
        position->GetValue(EPositionFieldSatelliteTime, 
                        sattime);
        TDateTime dtime = sattime.DateTime();
        TInt year = dtime.Year();
        TMonth month = dtime.Month();
        TInt day = dtime.Day();
        TInt hour = dtime.Hour();
        TInt min = dtime.Minute();
        TInt sec = dtime.Second();
        if (year != KExpectedYear)
            {
            User::Leave(KErrGeneral);                
            }
        if (month != KExpectedMonth)
            {
            User::Leave(KErrGeneral);                
            }
        if (day != KExpectedDay)
            {
            User::Leave(KErrGeneral);                
            }
        if (hour != KExpectedHour)
            {
            User::Leave(KErrGeneral);                
            }
        if (min != KExpectedMin[i])
            {
            User::Leave(KErrGeneral);                
            }
        if (sec != KExpectedSec[i])
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldTrueCourse);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 trueCourse;
        position->GetValue(EPositionFieldTrueCourse, 
                        trueCourse);
        if (trueCourse != KExpectedCourse)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldHorizontalSpeed);
        if (mustbetrue)
			{
			User::Leave(KErrGeneral);
			}

        mustbetrue = position->IsFieldAvailable(EPositionFieldMagneticCourse);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 magCourse;
        position->GetValue(EPositionFieldMagneticCourse, 
                        magCourse);
        if (magCourse != KExpectedMagCourse)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSA
        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteNumUsed);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TInt8 numUsedSatellites;
        position->GetValue(EPositionFieldSatelliteNumUsed, 
                        numUsedSatellites);
        if (numUsedSatellites != KExpectedSatUsed)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteHorizontalDoP);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 hdop;
        position->GetValue(EPositionFieldSatelliteHorizontalDoP, 
                        hdop);
        if (hdop != KExpectedHDOP)
            {
            User::Leave(KErrGeneral);                
            }

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteVerticalDoP);
        if (mustbetrue)
			{
			User::Leave(KErrGeneral);
			}

        mustbetrue = position->IsFieldAvailable(EPositionFieldSatellitePositionDoP);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TReal32 pdop;
        position->GetValue(EPositionFieldSatellitePositionDoP, 
                        pdop);
        if (pdop != KExpectedPDOP)
            {
            User::Leave(KErrGeneral);                
            }

        // from GSV
        mustbetrue = position->IsFieldAvailable(EPositionFieldSatelliteNumInView);
        if (!mustbetrue)
			{
			User::Leave(KErrGeneral);
			}
        TInt8 satinview;
        position->GetValue(EPositionFieldSatelliteNumInView, 
                        satinview);
        if (satinview != KExpectedSatInView)
            {
            User::Leave(KErrGeneral);                
            }


        CleanupStack::PopAndDestroy(position); // position 
		}
    CleanupStack::PopAndDestroy(req); // requester  
	_LIT(KRequestPartialGenericInfo, "Ended: **** Request partial generic info ****");
	INFO_PRINTF1(KRequestPartialGenericInfo);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestClearAndSetDefaultPositionDataL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestClearAndSetDefaultPositionDataL()
    {
    const TReal32 KExpectedMagCourse = 354.0F;
    const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;
    const TReal32 KExpectedSpeed = KKnotsToMetrePerSecFactor * 0.0F; 
    const TReal32 KExpectedHeading = 360; 

    // correct NMEA file
    FixSettingsForSimPSY(KNmeaSatelliteFile);

    CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status;
    TTimeIntervalMicroSeconds requestTime;
    
    //
    // Make a request with a HPositionGenericInfo object
	HPositionGenericInfo* position = HPositionGenericInfo::NewLC();
	TReal32 trueCourse = TReal32(30.00);
	TInt err = position->SetValue(EPositionFieldTrueCourse, trueCourse);
        // set position
    TPosition pos;
    TReal lat(66.6);
    TReal lon(77.7);
    TReal32 alt(100);
	pos.SetCoordinate(lat, lon, alt);
	pos.SetCurrentTime();
	position->SetPosition(pos);

    // make request

	position->SetRequestedField(EPositionFieldMagneticCourse);
	req->MakeRequests(position);
    req->GetResult(status, requestTime);   
    
    CheckRequestResultL(status, *position);
	if (status != KErrNone)
		{
		User::Leave(KErrGeneral);
		}

        // Check data from returned generic-info object...
	err = position->GetValue(EPositionFieldTrueCourse, trueCourse);
	if (err != KErrNotFound) // value previously set should be cleared
		{
		User::Leave(KErrGeneral);	
		}		

    position->GetPosition(pos);
    TReal returnedLat = pos.Latitude();
    TReal returnedLon = pos.Longitude();
    TReal32 returnedAlt= pos.Altitude();

    if (returnedLat == lat)
        {
        User::Leave(KErrGeneral);
        }
    if (returnedLon == lon)
        {
        User::Leave(KErrGeneral);
        }
    if (returnedAlt == alt)
        {
        User::Leave(KErrGeneral);
        }

    TInt mustbetrue = position->IsFieldAvailable(EPositionFieldMagneticCourse);
    if (!mustbetrue)
		{
		User::Leave(KErrGeneral);
		}
    TReal32 magCourse;
    position->GetValue(EPositionFieldMagneticCourse, 
                    magCourse);
    if (magCourse != KExpectedMagCourse)
        {
        User::Leave(KErrGeneral);                
        }
	CleanupStack::PopAndDestroy(position);
    
    //
    // Make a request with a TPositionCourseInfo object
    TPositionCourseInfo positionCourse = TPositionCourseInfo();
		
    // set course
    TCourse course;
    TReal32 headingAccuracy(38);
    course.SetHeadingAccuracy(headingAccuracy);
    positionCourse.SetCourse(course);
        
        // make request
	req->MakeRequests(&positionCourse);
    req->GetResult(status, requestTime);  
            
    CheckRequestResultL(status, positionCourse);

        // check data from returned course-info object
    positionCourse.GetCourse(course);
    TReal32 returnedHeadingAccuracy = course.HeadingAccuracy();

	// TRealX nan;
	// nan.SetNaN();
    if (!Math::IsNaN(returnedHeadingAccuracy))
        {
        User::Leave(KErrGeneral);                
        }
    if (course.Heading() != KExpectedHeading)
        {
        User::Leave(KErrGeneral);                
        } 
    if (course.Speed() != KExpectedSpeed)
        {
        User::Leave(KErrGeneral);                
        } 

    //
    // Make a request with a TPositionSatelliteInfo object
    TPositionSatelliteInfo positionSatellite = TPositionSatelliteInfo();
        
        // set sat data
    TSatelliteData satData;
    TInt satelliteId(26);
    satData.SetSatelliteId(satelliteId);
    satData.SetIsUsed(ETrue);
    positionSatellite.AppendSatelliteData(satData);
    
    TCourse courseSat;
    headingAccuracy = 85;
    courseSat.SetHeadingAccuracy(headingAccuracy);
    positionSatellite.SetCourse(course);

        // make request
	req->MakeRequests(&positionSatellite);
    req->GetResult(status, requestTime);  
            
    CheckRequestResultL(status, positionSatellite);

        // check data from returned satellite-info object
    positionSatellite.GetCourse(courseSat);
    returnedHeadingAccuracy = courseSat.HeadingAccuracy();
    positionSatellite.GetSatelliteData(0, satData);
    TInt returnedSatId = satData.SatelliteId();

    if (!Math::IsNaN(returnedHeadingAccuracy))
        {
        User::Leave(KErrGeneral);                
        }
    if (course.Speed() != KExpectedSpeed)
        {
        User::Leave(KErrGeneral);                
        }
    if (returnedSatId == satelliteId)
        {
        User::Leave(KErrGeneral);                
        }
    
    //
    // Make a request with a TPositionInfo object

    TPositionInfo positionInfo = TPositionInfo();    
        // set pos info
    TPosition posInfo;
    TReal latInfo(26.6);
    TReal lonInfo(47.7);
    TReal32 altInfo(300);
	posInfo.SetCoordinate(latInfo, lonInfo, altInfo);
    TUid datumId = {0x01010101};
    posInfo.SetDatum(datumId);
	positionInfo.SetPosition(posInfo);

         // make request
	req->MakeRequests(&positionInfo);
    req->GetResult(status, requestTime);  
            
    CheckRequestResultL(status, positionInfo);

    positionInfo.GetPosition(posInfo);
    returnedLat = posInfo.Latitude();
    returnedLon = posInfo.Longitude();
    returnedAlt= posInfo.Altitude();
    TUid returnedDatumId = posInfo.Datum();

    if (returnedLat == latInfo)
        {
        User::Leave(KErrGeneral);
        }
    if (returnedLon == lonInfo)
        {
        User::Leave(KErrGeneral);
        }
    if (returnedAlt == altInfo)
        {
        User::Leave(KErrGeneral);
        }
    if (returnedDatumId == datumId)
        {
        User::Leave(KErrGeneral);
        }


    CleanupStack::PopAndDestroy(req); // requester 
	_LIT(KRequestClearAndSetDefaultPositionData, "Ended: **** Test Clear Position Data ****");
	INFO_PRINTF1(KRequestClearAndSetDefaultPositionData);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestPartialUpdateL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestPartialUpdateL()
    {
	FixSettingsForSimPSY(KNmeaPartialUpdateFile);
    CSimPsyMultiNmeaPosRequester* requester = CSimPsyMultiNmeaPosRequester::NewLC();
    requester->SetPartialUpdateRestriction(ETrue);

    TInt status;
    TTimeIntervalMicroSeconds requestTime;

    TInt KNrOfRequests = 10;

    for (TInt i = 0; i < KNrOfRequests; i++)
        {
		TPositionInfo position = TPositionInfo();    
		requester->MakeRequests(&position);
		requester->GetResult(status, requestTime); 

        _LIT(KReq, "Request %d completed with %d.");
		TBuf<128> buf;
		buf.Format(KReq, i, status);
        INFO_PRINTF1(buf);
        RDebug::Print(buf);

        if (status == KPositionPartialUpdate && !IsPartialPosition(position) ||
            status != KPositionPartialUpdate && IsPartialPosition(position))
            {
            User::Leave(KErrGeneral);
            }
        else if (status == KErrNone && IsPartialPosition(position))
            {
            User::Leave(KErrGeneral);
            }

        // Fix 0, 2 and 5 is partial
        if (status == KPositionPartialUpdate && 
            i != 0 && i != 2 && i != 5)
            {
            User::Leave(KErrGeneral);            
            }
        // Fix 1, 3, 4 and 6 more is full fixes
        else if (status == KErrNone && 
            i != 1 && i != 3 && i != 4 && 
            i != 6 && i != 7 && i != 8 && i != 9)
            {
            User::Leave(KErrGeneral);            
            }

        }
    CleanupStack::PopAndDestroy(requester); // requester   
	_LIT(KTestPartialUpdate, "Ended: **** Test Partial Update ****");
	INFO_PRINTF1(KTestPartialUpdate);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestErrorSituationsL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestErrorSituationsL()
    {
	FixSettingsForSimPSY(KNmeaError1);
    CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status = KErrNone;
    TTimeIntervalMicroSeconds requestTime;

    TInt KNrOfRequests = 5;

    req->SetRequesterWillCancelL(0, 4 * 1000000);  // 4 seconds timeout

    for (TInt i = 0; i < KNrOfRequests; i++)
        {
		TPositionSatelliteInfo position = TPositionSatelliteInfo();    
		req->MakeRequests(&position);
		req->GetResult(status, requestTime); 
        
        if (status != KErrCancel)
            {
            User::Leave(status);
            }
    
        }
    CleanupStack::PopAndDestroy(req); // req

	FixSettingsForSimPSY(KNmeaError2);
    req = CSimPsyMultiNmeaPosRequester::NewLC();
    req->SetRequesterWillCancelL(0, 4 * 1000000);  // 4 seconds timeout
    
    KNrOfRequests = 2;

    for (TInt j = 0; j < KNrOfRequests; j++)
        {
		TPositionSatelliteInfo position = TPositionSatelliteInfo();    
		req->MakeRequests(&position);
		req->GetResult(status, requestTime); 
        
        if (status != KErrNone || 
            position.NumSatellitesInView() != 15 ||
            position.NumSatellitesUsed() != 4 ||
            position.HorizontalDoP() != 2.0 ||
            position.VerticalDoP() != 2.0)
            {
            User::Leave(KErrGeneral);
            }
        
        }
    CleanupStack::PopAndDestroy(req); // req

    // Check that correct fix is made when NMEA sentence has wrong checksum
	FixSettingsForSimPSY(KNmeaError3);
    req = CSimPsyMultiNmeaPosRequester::NewLC();
    
    KNrOfRequests = 2;

    for (TInt k = 0; k < KNrOfRequests; k++)
        {
		TPositionInfo position = TPositionInfo();    
		req->MakeRequests(&position);
		req->GetResult(status, requestTime); 

        TPosition pos;
        position.GetPosition(pos);
        
        if (status != KErrNone)
            {
            User::Leave(KErrGeneral);
            }
        if (k == 1 && 
            pos.Altitude() != 200 && 
            pos.Latitude() != -6 && 
            pos.Longitude() != -61)
            {
            User::Leave(KErrGeneral);
            }
        }
    CleanupStack::PopAndDestroy(req); // req

    // Add more error situations here is applicable
	_LIT(KTestErrorSituations, "Ended: **** Test wrong situations ****");
	INFO_PRINTF1(KTestErrorSituations);
    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::TestSpecialCasesL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CSimPsyNmeaPositionerStep::TestSpecialCasesL()
    {
	FixSettingsForSimPSY(KNmeaSpecialCase);
    CSimPsyMultiNmeaPosRequester* req = CSimPsyMultiNmeaPosRequester::NewLC();

    TInt status = KErrNone;
    TTimeIntervalMicroSeconds requestTime;

    TInt KNrOfRequests = 4;

    for (TInt i = 0; i < KNrOfRequests; i++)
        {
    	HPositionGenericInfo* position = HPositionGenericInfo::NewLC();
        User::LeaveIfError(position->SetRequestedField(EPositionFieldNMEASentences));
        
        req->MakeRequests(position);
		req->GetResult(status, requestTime); 

        CheckRequestResultL(status, *position);

        TPosition pos;
        position->GetPosition(pos);

        TUint8 sentences;
        TInt err = KErrNone;
        TPtrC8 nmeaString(NULL, 0);
        
        if (i == 0)
            {
            if (pos.Altitude() != 200)
                {
                User::Leave(KErrGeneral);
                }

	        err = position->GetValue(EPositionFieldNMEASentences, sentences); 
	        User::LeaveIfError(err);
	        if (sentences != 7)
		        {
		        User::Leave(KErrGeneral);
		        }

            _LIT8(KExpectedFirstSentence, "$GPGLL,6459.86");
            User::LeaveIfError(position->GetValue(EPositionFieldNMEASentencesStart, nmeaString));
            if (nmeaString.Left(14).Compare(KExpectedFirstSentence) != 0)
                {
                _LIT(KError, "Expected ,$GPGLL,6459.86*, sentence at the beginning of the burst");
                LogErrorAndFail(KError);
                }
            }
        if (i == 1)
            {
            if (!Math::IsNaN(pos.Altitude()))
                {
                User::Leave(KErrGeneral);
                }            
	        err = position->GetValue(EPositionFieldNMEASentences, sentences); 
	        User::LeaveIfError(err);
	        if (sentences != 9)
		        {
		        User::Leave(KErrGeneral);
		        }
            }
        if (i == 2)
            {
            if (pos.Altitude() != 200)
                {
                User::Leave(KErrGeneral);
                }            
            if (pos.Latitude() != -6)
                {
                User::Leave(KErrGeneral);
                }            
            if (pos.Longitude() != -61)
                {
                User::Leave(KErrGeneral);
                }            
            err = position->GetValue(EPositionFieldNMEASentences, sentences); 
	        User::LeaveIfError(err);
	        if (sentences != 6)
		        {
		        User::Leave(KErrGeneral);
		        }

            _LIT8(KExpectedFirstSentence, "$GPGLL,6459.8250");
            User::LeaveIfError(position->GetValue(EPositionFieldNMEASentencesStart, nmeaString));
            if (nmeaString.Left(16).Compare(KExpectedFirstSentence) != 0)
                {
                _LIT(KError, "Expected ,$GPGLL,6459.8250*, sentence at the beginning of the burst");
                LogErrorAndFail(KError);
                }
            }

        CleanupStack::PopAndDestroy(position); // position
        }
    CleanupStack::PopAndDestroy(req); // req
	_LIT(KTestSpecialCase, "Ended: **** Test Special Case ****");
	INFO_PRINTF1(KTestSpecialCase);
    }




// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::CheckRequestResultL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSimPsyNmeaPositionerStep::CheckRequestResultL(
	TInt aStatus, TPositionInfo& aPosition)
    {
    _LIT(KReq, "Checking status: %d.");
	TBuf<128> statusSuf;
	statusSuf.Format(KReq, aStatus);
    INFO_PRINTF1(statusSuf);
    RDebug::Print(statusSuf);

    if (aStatus != KErrNone)
       {
       User::Leave(aStatus);
       }
   
    TPosition position;		
	aPosition.GetPosition(position);

    TBuf<30> buf;
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

    }

// ---------------------------------------------------------
// CSimPsyNmeaPositionerStep::IsPartialPosition
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CSimPsyNmeaPositionerStep::IsPartialPosition(
	TPositionInfoBase& aPosInfo)
	{
	TPositionInfo& posInfo = static_cast<TPositionInfo&> (aPosInfo);
	TPosition pos;
	posInfo.GetPosition(pos);

	// Check if time is set
	// Check if longitude is a number
	// Check if latitude is a number
	if (pos.Time().Int64() == 0 || 
		Math::IsNaN(TRealX(pos.Longitude())) ||
		Math::IsNaN(TRealX(pos.Latitude())))
		{
		return ETrue;
		}

	return EFalse;
	}


// End of File
