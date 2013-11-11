// TTMSGraphicsStep.cpp
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
//

/**
@file
@test
@internalComponent
*/

#include <bautils.h>
#include <test/ttmsgraphicsstep.h>
#include <f32file.h> 
#include <e32std.h>

_LIT8(KLogPass,"PASS");
_LIT8(KLogFail,"FAIL");


void CTTMSGraphicsStep::CleanUpAndWriteResults()
	{
	TBuf<256> testID;
	TBuf<8> testResult;
	
	for (TInt iArrayIndex=0; iArrayIndex < iArrayTMSData.Count(); iArrayIndex++)
		{
		CTMSDataColumn columnData = iArrayTMSData[iArrayIndex];
		
		testID.Copy(columnData.iTheID);
		testResult.Copy(columnData.iTheResult);
		
		INFO_PRINTF2(_L("START_TESTCASE %S"),&testID);
		INFO_PRINTF2(_L("Line = 1 Command = START_TESTCASE %S"),&testID);
		INFO_PRINTF2(_L("END_TESTCASE %S"),&testID);
		INFO_PRINTF3(_L("Line = 1 Command = END_TESTCASE %S ***TestCaseResult = %S"),&testID,&testResult);
		}
	
	iArrayTMSData.Close();		    
	}

EXPORT_C CTTMSGraphicsStep::CTTMSGraphicsStep() : iStepPassFail(ETrue), iShowMultipleResults(EFalse)
	{
	iCurrentTestStepName.Copy(KUnknownSYMTestCaseIDName);
	}

EXPORT_C void CTTMSGraphicsStep::CloseTMSGraphicsStep()
	{
	CTTMSGraphicsStep::CleanUpAndWriteResults();	
	}

EXPORT_C CTTMSGraphicsStep::~CTTMSGraphicsStep()
	{	
	CTTMSGraphicsStep::CleanUpAndWriteResults();
	}

// writes out the results of a test that has just been executed to the log file(s)
// the first time a test result is written it is also added to the iTestStepNames array.
// This array is then checked on subsequent tests to see if the test has already been written to log
// if it has and multiple test results are not wanted further logging of this test is omitted.


void CTTMSGraphicsStep::DebugLogL(const TDesC8& aTestCaseID, const TDesC8& aResult)
	{		
	TBool foundID = EFalse;
	if(!iShowMultipleResults)
		{
		for (TInt iArrayIndex=0; iArrayIndex < iArrayTMSData.Count(); iArrayIndex++)
			{
			CTMSDataColumn columnData = iArrayTMSData[iArrayIndex];
			if (columnData.iTheID.Compare(aTestCaseID)== 0)
				{
				if (columnData.iTheResult.Compare(KLogFail) != 0)
					{
					iArrayTMSData[iArrayIndex].iTheResult.Copy(aResult);
					}
				foundID = ETrue;
				iArrayIndex = iArrayTMSData.Count();
				}
			}
		}
	
	if (!foundID || iShowMultipleResults)
		{
		CTMSDataColumn theData;
		theData.iTheID.Copy(aTestCaseID);
		theData.iTheResult.Copy(aResult);
		iArrayTMSData.Append(theData);
		}
	}

EXPORT_C void CTTMSGraphicsStep::RecordTestResultL()
	{	
	TBuf<64> unknownTMSID(KUnknownSYMTestCaseIDName);
	TBuf8<64> unknownID8;
	unknownID8.Copy(unknownTMSID);
	
	TBuf<64> undefinedTMSID(KUndefinedSYMTestCaseIDName);
	TBuf8<64> undefinedID8;
	undefinedID8.Copy(undefinedTMSID);
	
	TBuf<64> notaTMSID(KNotATestSYMTestCaseIDName);
	TBuf8<64> notaID8;
	notaID8.Copy(notaTMSID);
	
	TBuf<128> currentTSName;
	currentTSName.Copy(iCurrentTestStepName);
	
	if (iCurrentTestStepName.Compare(unknownID8) == 0) 	// unknown tests logged to warning file		
		{
		INFO_PRINTF1(_L("ID NOT SET"));
		}
	else if (iCurrentTestStepName.Compare(undefinedID8) == 0)	// undefined tests logged to warning file
		{
		INFO_PRINTF1(_L("ID UNDEFINED"));
		}
	else if (iCurrentTestStepName.Compare(notaID8) == 0)
		{
		}
	else 			
		{
		if( iStepPassFail == EFalse)
			{
			INFO_PRINTF2(_L("%S FAILED"), &currentTSName ); 
			DebugLogL(iCurrentTestStepName, KLogFail);
			}
		else
			{
			DebugLogL(iCurrentTestStepName, KLogPass);
			}	
		}
	}

// sets the name and pass/fail state of the individual test that is about to be executed 
EXPORT_C void CTTMSGraphicsStep::SetTestStepID(const TDesC& aStepName)
	{
	if(TestStepName() == KNullDesC)
	    {
	    SetTestStepName(aStepName);
	    }

	iCurrentTestStepName.Copy(aStepName);
	iStepPassFail = ETrue;
	}

EXPORT_C void CTTMSGraphicsStep::SetOverallTestStepID(const TDesC& aStepName)
	{
	iCurrentTestStepName.Copy(aStepName);
	iStepPassFail = ETrue;
	if (!TestStepResult() == EPass)
				TEST(EFalse);
	}

// needed for the overide of CTGraphicsBase::testBooleanTrue to allow us to set 
// whether an individual test has passed or failed
EXPORT_C void CTTMSGraphicsStep::MQCTest(TBool aCondition, const TText8* aFile, TInt aLine)
	{
	if(!aCondition)
		{
		iStepPassFail = EFalse;	
		}
	testBooleanTrue( aCondition, aFile, aLine, ETrue);		
	}

// needed for the overide of CTGraphicsBase::testBooleanTrue to allow us to set 
// whether an individual test has passed or failed
EXPORT_C void CTTMSGraphicsStep::MQCTestL(TBool aCondition, const TText8* aFile, TInt aLine)
	{
	if(!aCondition)
		{
		iStepPassFail = EFalse;	
		}
	testBooleanTrueL( aCondition, aFile, aLine, ETrue);		
	}

// needed for the overide of CTGraphicsBase::testBooleanTrueWithErrorCode to allow us to set 
// whether an individual test has passed or failed. Also allows line and file of test failure
// to be displayed.
EXPORT_C void CTTMSGraphicsStep::MQCTestWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	if(!aCondition)
		{
		iStepPassFail = EFalse;
		SetTestStepResult(EFail);
		_LIT(KMessage,"Test Failed with error [%d]");
		Logger().LogExtra(aFile, aLine, ESevrErr, KMessage, aErrorCode);
		}
	}

// sets whether tests with the same @SYMTestCaseID xxxxxx name are printed multiple times in the log files
EXPORT_C void CTTMSGraphicsStep::MultipleResultsForSameID(TBool aShowMultipleResults)
	{
	iShowMultipleResults = aShowMultipleResults;
	}
