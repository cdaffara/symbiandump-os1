//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file contains the implementation for the base class from 
// *               which all unit test classes are derived.
//

// UnitTestBase.cpp

/** @file UnitTestBase.cpp
 *
 */

#include "unitTestBase.h"

// This indicates that the CSY is loaded by the TSY
#define LOADED_BY_TSY	1

//
// CUnitTestBase
//

// These methods can be overwritten

TBool CUnitTestBase::HasTestRangesDefined()
	{
	return EFalse;
	}


void CUnitTestBase::ConstructL()
/**
 *
 *
 */
	{
	//LOGTEXT(">>ConstructL");

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.SetSessionPath(KTestSessionPath));

	TInt err=iLogFile.Open(iFs,iLogFileName, EFileWrite);

	if (err==KErrNotFound) // file does not exist - create it
		{
		err = iLogFile.Create(iFs, iLogFileName, EFileWrite);
		}
	// In simultaneous tests, someone else may have created it between our
	// calls to Open and Create, so no 'else' here.
	if ( err == KErrInUse || err == KErrAlreadyExists )
		{
		//LOGTEXT("Distinct logging required");

		// Same filename already open by someone else. In order to run more
		// than one test in, say, BasicCall at once AND have logging that
		// works, create distinct log files when required.
		TInt n = 0;
		while ( err == KErrInUse || err == KErrAlreadyExists )
			{
			TName newLogFileName(iLogFileName);
			n++;
			newLogFileName.AppendFormat(_L("%d"),n);
			err = iLogFile.Open(iFs, newLogFileName, EFileWrite);
			if (! err)
				{
				TInt pos = 0;
				iLogFile.Seek(ESeekEnd, pos);
				}
			if ( err == KErrNotFound )
				{
				err = iLogFile.Create(iFs, newLogFileName, EFileWrite);
				break;
				}
			}
		}

	if (err)
		{
		iTest.Printf(_L("Couldn't open log file (%d). Does the %S path exist?\n"), err, &KTestSessionPath);
		User::After(4000000);
		User::Leave(err);
		}
	TInt pos = 0;
	iLogFile.Seek(ESeekEnd, pos);

	err = iLogFile.Write(_L8("#------------------------Test Run Start-------------------------\r\n"));
	iLogFile.Flush();

	}

CUnitTestBase::CUnitTestBase(const TDesC& aUnitTestName,
							 const TDesC& aLogFileName,
							 const TDesC& aTestSpecName,
							 const TDesC& aTestScriptName)
/**
 *
 *
 */
	:iTest(aUnitTestName),
	 iUnitTestName(aUnitTestName),
	 iLogFileName(aLogFileName),
	 iTestScriptName(aTestScriptName),
	 iTestSpecName(aTestSpecName)
	{
#ifndef __WINS__
	iTest.SetLogged(EFalse);
#endif // __WINS__
	}

CUnitTestBase::~CUnitTestBase()
/**
 *
 *
 */
	{
	iFailedTests.Close();

	if (iLogFile.SubSessionHandle())
		{
		iLogFile.Write(_L8("#------------------------Test Run End-------------------------\r\n"));
		iLogFile.Flush();
		}
	iLogFile.Close();
	iFs.Close();
	iTest.Close();
	}

void CUnitTestBase::ResizeConsoleL(const TDesC& aName, TInt aXOrg, TInt aYOrg, TInt aXSize, TInt aYSize)
	{
#if defined (__WINS__)
	CConsoleBase* cons = iTest.Console();
	CConsoleBase* newCon = NULL;

	TRAPD(err,newCon = Console::NewL(aName,TSize(aXSize,aYSize)));
	if (!err)
		{
		(void)aXOrg; (void)aYOrg; //unable to use these without a dirty hack!
		delete cons;
		iTest.SetConsole(newCon);
		}
#else //__WINS__
	(void)aName; (void)aXOrg; (void)aYOrg; (void)aXSize; (void)aYSize;
#endif //__WINS__
	}

TBool CUnitTestBase::DoSingleTestL(TInt aMethodNum, TInt aTestNum, TTestMode aTestMode)
	{
	//LOGTEXT3(">>DoSingleTestL [aMethodNum=%d, aTestNum=%d]",aMethodNum,aTestNum);

	TInt numMethods;
	TInt numTests;
	TInt errorCode;
	TInt leaveCode;

	// Check if the specified test is in a valid range - if we can
	if (HasTestRangesDefined())
		{
		RunTestsL(EFalse, EBatch, numMethods, leaveCode, 0, 0);

		// Check that the method number is correct
		if (aMethodNum == 0 || aMethodNum > numMethods)
			{
			iTest.Printf(_L("Bad method number: %d - Needs to be in range 1 to %d\n"),
						aMethodNum, numMethods);
			iTest.Printf(_L("Press a key when ready\n"));
			iTest.Console()->Getch();
			//LOGTEXT("<<DoSingleTestL bad method number");
			return ETrue;
			}


		TInt testRange = 0;
		if (aTestNum > 100) testRange = 100;

		// Get the number of tests for this method and test if aTestNum is in range
		RunTestsL(EFalse, EBatch, numTests, leaveCode, aMethodNum, testRange);
		numTests += testRange;

		if (aTestNum <= testRange || aTestNum > numTests)
			{
			iTest.Printf(_L("Bad test number (%d) for method %d - Needs to be in range %d to %d\n"),
						aTestNum, aMethodNum, testRange+1, numTests);
			iTest.Printf(_L("Press a key when ready\n"));
			iTest.Console()->Getch();
			//LOGTEXT("<<DoSingleTestL bad test number");
			return ETrue;
			}
		}

	errorCode = KErrNone;
	leaveCode = KErrNone;

	RunTestsL(EFalse, aTestMode, errorCode, leaveCode, aMethodNum,aTestNum);

	LogTestResult(errorCode, leaveCode, EFalse, aMethodNum, aTestNum, aTestMode);

	//LOGTEXT("<<DoSingleTestL ETrue");
	return ETrue;
	}

TBool CUnitTestBase::DoAutoMethodTestsL(TInt aMethodNum)
	{
	//LOGTEXT2(">>DoAutoMethodTestsL [aMethodNum=%d]",aMethodNum);

	TInt numMethods;
	TInt numTests;
	TInt errorCode;
	TInt leaveCode;

	if (! HasTestRangesDefined())
		{
		iTest.Printf(_L("Cannot auto run tests as test ranges not implemented\n"));
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}

	// Check that the method number is correct
	RunTestsL(EFalse, EBatch, numMethods, leaveCode, 0, 0);

	if (aMethodNum == 0 || aMethodNum > numMethods)
		{
		iTest.Printf(_L("Bad method number: %d - Needs to be in range 1 to %d\n"),
					aMethodNum, numMethods);
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}


	// Get the number of automated tests for this method
	RunTestsL(EFalse, EBatch, numTests, leaveCode, aMethodNum, 0);

	for (TInt testIndex = 1; testIndex <= numTests; testIndex++)
		{
		errorCode = KErrNone;
		leaveCode = KErrNone;

		RunTestsL(EFalse, EBatch, errorCode, leaveCode, aMethodNum, testIndex);
		LogTestResult(errorCode, leaveCode, EFalse, aMethodNum, testIndex, EBatch);

		if (errorCode || leaveCode)
			{
			TTestFailure theFailedTest(aMethodNum, testIndex);
			(void) iFailedTests.Append(theFailedTest);
			}
		}

	//LOGTEXT("<<DoAutoMethodTestsL");
	return ETrue;
	}

TBool CUnitTestBase::DoTestsInSpecifiedRangeL(TInt aStartMethodNum,
											  TInt aStartTestNum,
											  TInt aEndMethodNum,
											  TInt aEndTestNum)
	{
	//LOGTEXT3(">>DoTestsInSpecifiedRangeL [aStartMethodNum=%d, aEndMethodNum=%d]",
	//	aStartMethodNum,aEndMethodNum);

	TInt numMethods;
	TInt numTests;
	TInt errorCode;
	TInt leaveCode;

	if (! HasTestRangesDefined())
		{
		iTest.Printf(_L("Cannot auto run tests as test ranges not implemented\n"));
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}

	// Check that the method number is correct
	RunTestsL(EFalse, EBatch, numMethods, leaveCode, 0, 0);

	if (aStartMethodNum == 0 || aStartMethodNum > numMethods)
		{
		iTest.Printf(_L("Bad method number: %d - Needs to be in range 1 to %d\n"),
					aStartMethodNum, numMethods);
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}
	if (aEndMethodNum == 0 || aEndMethodNum > numMethods)
		{
		iTest.Printf(_L("Bad method number: %d - Needs to be in range 1 to %d\n"),
					aEndMethodNum, numMethods);
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}
	if (aEndMethodNum == -1)
		aEndMethodNum = numMethods;

	for (TInt methodIndex = aStartMethodNum; methodIndex <= aEndMethodNum; methodIndex++)
		{
		TInt testIndex;

		if ((methodIndex < aEndMethodNum) || (aEndTestNum == -1))
			{
			// Get the number of automated tests for the current method
			RunTestsL(EFalse, EBatch, numTests, leaveCode, methodIndex, 0);
			}
		else
			{
			// Finish on a specified test.
			numTests = aEndTestNum;
			}

		if (methodIndex == aStartMethodNum)
			testIndex = aStartTestNum;
		else
			testIndex = 1;

		iTest.Printf(_L("Running tests %d to %d in method %d\n"),
			testIndex, numTests, methodIndex);
		for (; testIndex <= numTests; testIndex++)
			{
			errorCode = KErrNone;
			leaveCode = KErrNone;

			RunTestsL(EFalse, EBatch, errorCode, leaveCode, methodIndex, testIndex);

			LogTestResult(errorCode, leaveCode, EFalse, methodIndex, testIndex, EBatch);
			}
		}

	//LOGTEXT("<<DoTestsInSpecifiedRangeL");
	return ETrue;
	}

TBool CUnitTestBase::DoAutoMethodsL()
	{
	//LOGTEXT(">>DoAutoMethodsL");

	TInt numMethods;
	TInt leaveCode;

	if (! HasTestRangesDefined())
		{
		iTest.Printf(_L("Cannot auto run tests as test ranges not implemented\n"));
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}

	// Get the number of automated tests for this method
	RunTestsL(EFalse, EBatch, numMethods, leaveCode, 0, 0);

	iFailedTests.Reset();
	for (TInt methodIndex = 1; methodIndex <= numMethods; methodIndex++)
		{
		DoAutoMethodTestsL(methodIndex);
		}

	TInt failedCount = iFailedTests.Count();
	if (   iRepeatFailedTests
		&& failedCount > 0)
		{
		iTest.Printf(_L("%d tests failed. Will retry them now.\n"), failedCount);
		iLogFile.Write(_L8("#--- Retrying the tests that failed ---\r\n"));
		for (TInt i = 0; i < failedCount; i++)
			{
			TTestFailure theFailedTest = iFailedTests[i];
			DoSingleTestL(theFailedTest.iMethodNum, theFailedTest.iTestIndex, EBatch);
			}
		}
	iFailedTests.Reset();

	//LOGTEXT("<<DoAutoMethodsL");
	return ETrue;
	}

TBool CUnitTestBase::DoTestsL()
/**
 * This function selects a unit test, runs it and then logs the result
 *
 * @return return a TBool
 * @retval ETrue if another test to run in interactive mode
 * @retval EFalse if the test is in batch mode or there is not another
 *         test to run in interactive mode
 */
	{
	TInt methodNum;
	TInt testNum;
	TInt endMethodNum;
	TInt endTestNum;
	TTestMode testMode = EBatch;
	TBool exitTests;

	TBool numOK = SelectTestL(methodNum, testNum,
		endMethodNum, endTestNum, testMode, exitTests);

	if (! numOK)
		{
		iTest.Printf(_L("Bad number entered\n"));
		iTest.Printf(_L("Press a key when ready\n"));
		iTest.Console()->Getch();
		return ETrue;
		}

	if (exitTests)
		{
		return EFalse;
		}

	// Is this a manual test?
	if ((testNum != -1) && ((endTestNum == -1) || (testMode == EInteractive)))
		{
		iTest.Printf(_L("Running single test %d-%d\n"), methodNum, testNum);
		DoSingleTestL(methodNum, testNum, testMode);
		return ETrue;
		}

	// Do we want to auto run all the tests for a specified method
	if ((methodNum != -1) && (testNum == -1))
		{
		iTest.Printf(_L("Running all tests in method %d\n"), methodNum);
		DoAutoMethodTestsL(methodNum);
		return ETrue;
		}

	// Do we want to run tests from one index to another?
	if (methodNum != -1)
		{
		iTest.Printf(_L("Running tests %d-%d to %d-%d\n"), methodNum, testNum, endMethodNum, endTestNum);
		DoTestsInSpecifiedRangeL(methodNum, testNum, endMethodNum, endTestNum);
		return ETrue;
		}

	// methodNum == -1 && testNum == -1 =>
	// We want to all the tests for every method automatically
	iTest.Printf(_L("Running all tests\n"));
	return DoAutoMethodsL();
  	}

TInt CUnitTestBase::PostTestDelayTime()
/**
 * This virtual function can be overridden by derived tests to
 * configure the length of delay between running tests in
 * automated mode. This used to be hard coded to KTsyTestCompleteTime,
 * which was 3 seconds.
 *
 * With 100 tests, that adds up to 5 minutes of wasted time per run!
 */
	{
	return KTsyTestCompleteTime;
	}

void  CUnitTestBase::ActionNoMatch(TBool aEndInd,TTestMode aTestMode,
								   TInt aApiNumber,TInt aTestNumber)
/**
 * This function provides the default action for the case
 * when the test identified by the API and test number is
 * not implemented. It is to be called within RunTestsL()
 * in the default case of its switch statement
 */
	{
	if(!aEndInd)
		{
		iTest.Printf(_L("** Not implemented CSY%S-%d-%d yet **\n"),
			         &iUnitTestName,aApiNumber,aTestNumber);
		if(aTestMode==EInteractive)
			{
			iTest.Printf(_L("press any key to continue\n"));
			iTest.Console()->Getch();
			}
		}
	}

TInt CUnitTestBase::CompareTestResult
(TInt aExpectedResult,TRequestStatus aStatus)
/**
 * This function compare the test result with the expected one
 *
 * @param aStatus stores the result of a client API request
 *
 */
	{
	TInt ret=KErrNone;
	if (aStatus.Int() != aExpectedResult)
		{
		if(aStatus==KErrNone)
			ret=__LINE__;
		else
			ret=aStatus.Int();
		}
	return ret;
	}

void  CUnitTestBase::LogTestResult(TInt aErrCode, TInt aLeaveCode,
								   TBool aEndInd,TInt aApiNumber,
								   TInt aTestNumber,TTestMode aTestMode)
/**
 * This function logs the test result
 */
	{
	TInt err=KErrNone;
	if ((aErrCode)||(aLeaveCode))
		{
		if (aLeaveCode)
			{
			iTest.Printf(_L("Test failed with a LEAVE: CSY%S-%d-%d\n"),
				         &iUnitTestName,aApiNumber,aTestNumber);
			err=aLeaveCode;
			}
		else
			{
			iTest.Printf(_L("FAILURE: CSY%S-%d-%d failed with %d\n"),
				         &iUnitTestName,aApiNumber,aTestNumber,aErrCode);
			err=aErrCode;
			}

		//Print out test results in Symbian's requested format
		TUint count=iFunIndices.Count();
		for(TUint i = 0; i < count; i++)
			WriteLog(EFalse,iFunIndices[i],aApiNumber,aTestNumber,err);
		iFunIndices.Reset();

		if (aTestMode==EInteractive)
			{
			iTest.Printf(_L("press any key to continue\n"));
			iTest.Console()->Getch();
			}
		}
	else
		{
		//these code will not be run if a user type E or e
		//to end running the tests in interactive mode
		if (!aEndInd)
			{
			iTest.Printf(_L("CSY%S-%d-%d finished successfully\n"),
				         &iUnitTestName,aApiNumber,aTestNumber);

#ifndef LOG_FAILED_TESTS_ONLY
			//Print out test results in Symbian's requested format
			TUint count=iFunIndices.Count();
			for(TUint i = 0; i < count; i++)
				WriteLog(ETrue,iFunIndices[i],aApiNumber,aTestNumber,err);
			iFunIndices.Reset();
#endif
			iTest.Printf(_L("Waiting briefly for system to close\n"));

			// give some time for test to complete
//			User::After(this->PostTestDelayTime());
			}
		}
	}


TBool CUnitTestBase::UserSelectionL(TChar aMainNumber,
									TInt aNumMethods,
									TBool aRangeSpecified,
									TInt& aApiNum, TInt& aTestNum,
								    TTestMode& aTestMode)
	{
	TInt leaveCode  = 0;
	aApiNum = UserChoice(aMainNumber);

	if (aApiNum == -1) return EFalse;

	if (aNumMethods != -1 && (aApiNum < 1 || aApiNum > aNumMethods))
		{
		aApiNum = -1;
		aTestNum  = -1;
		return EFalse;
		}

	PrintTestDetails(aApiNum);

	TInt numAutoTests   = -1;
	TInt numManualTests = -1;

	if (! HasTestRangesDefined())
		iTest.Printf(_L("Test number:"));
	else
		{
		RunTestsL(EFalse, EBatch, numAutoTests, leaveCode, aApiNum, 0);
		RunTestsL(EFalse, EBatch, numManualTests, leaveCode, aApiNum, 100);

		iTest.Printf(_L("There are %d auto and %d manual tests\n"),
											numAutoTests, numManualTests);
		if (aRangeSpecified)
			iTest.Printf(_L("Range test number:"));
		else
			iTest.Printf(_L("Test number or 'A' for all tests:"));
		}

	TChar numberKey = iTest.Console()->Getch();
	iTest.Printf(_L("\n"));

	if ((!aRangeSpecified) && HasTestRangesDefined() && ((numberKey == 'A')||(numberKey == 'a')))
		{
		aTestMode = EBatch;
		aTestNum  = -1;
		return ETrue;
		}

	aTestNum = UserChoice(numberKey);

	if (aTestNum == -1) return EFalse;

	TBool ret = EFalse;

	if (! HasTestRangesDefined())
		ret = ETrue;

	if (aTestNum > 0 && aTestNum <= numAutoTests)
		ret = ETrue;

	if (aTestNum > 100 && aTestNum <= (100+numManualTests))
		ret = ETrue;

	return ret;
	}

TBool CUnitTestBase::SelectTestL(TInt& aApiNum, TInt& aTestNum,
								 TInt& aEndApiNum, TInt& aEndTestNum,
								 TTestMode& aTestMode, TBool& aExit)
/**
 * This function selects a unit test which is identified by its API &
 * test numbers to be run either in batch or interactive mode
 *
 * @return return a TBool to indicate whether or not to end the test in
 *         interactive mode
 */
	{
	aTestMode = EBatch;
	aExit     = EFalse;

	iTest.Console()->ClearScreen();

	if (CommandLineL(aApiNum, aTestNum, aEndApiNum, aEndTestNum))
		{
		aTestMode = EBatch;
		iExitAfterTestsComplete = ETrue;

		// we have got the numbers from the command line
		iTest.Printf(_L("Running %S automated API tests\n"),&iUnitTestName);

		// give tester time to get up and running
		User::After(KTsyTesterSetupTime);
		return ETrue;
		}

	// need to get numbers from the user
	iTest.Printf(_L("Running %S API Tests - interactively\n"), &iUnitTestName);

	aTestMode = EInteractive;

	// helper to aid user - which identifier relates to which number
	PrintTestSpecIdentifiers();

	TInt numMethods = -1;
	TInt leaveCode  = 0;
	TBool rangeSpecified = EFalse;

	// Get the numbers
	if (! HasTestRangesDefined())
		iTest.Printf(_L("\nNumber or 'S' for specific test or 'E' to end:"));
	else
		{
		RunTestsL(EFalse, EBatch, numMethods, leaveCode, 0, 0);
		iTest.Printf(_L("\nNumber or 'S' for specific test, 'R' for range, 'A' for all or 'E' to end:"), numMethods);
		}

	TChar numberKey = iTest.Console()->Getch();
	iTest.Printf(_L("\n"));

	if (!numberKey.IsDigit())
		{
		switch (numberKey)
			{
		case 'E':
		case 'e':
			aExit = ETrue;
			return ETrue;
		case 'A':
		case 'a':
			{
			if (HasTestRangesDefined())
				{
				aTestMode = EBatch;
				aApiNum   = -1;
				aTestNum  = -1;
				return ETrue;
				}
			}
			return EFalse;
		case 'R':
		case 'r':
			{
			if (HasTestRangesDefined())
				{
				aTestMode = EBatch;
				iTest.Printf(_L("Enter Start Method of range (1 -> %d):"), numMethods);
				numberKey = iTest.Console()->Getch();
				rangeSpecified = ETrue;
				}
			else
				return EFalse;
			}
			break;
		case 'S':
		case 's':
			{
			iTest.Printf(_L("Method to test (1 -> %d):"), numMethods);
			numberKey = iTest.Console()->Getch();
			}
			break;
		default:
			iTest.Printf(_L("Incorrect value\n"));
			return EFalse;
			}
		}

	// User to select test case or start of range
	TBool ret = UserSelectionL(numberKey, numMethods, rangeSpecified,
		aApiNum, aTestNum, aTestMode);

	if (ret && rangeSpecified)
		{
		PrintTestSpecIdentifiers();

		iTest.Printf(_L("Enter end method of range (1 -> %d) or 'L' last"), numMethods);
		numberKey = iTest.Console()->Getch();
		iTest.Printf(_L("\n"));

		if ((numberKey == 'L')||(numberKey == 'l'))
			{
			aTestMode = EBatch;
			aEndApiNum   = -1;
			aEndTestNum  = -1;
			return ETrue;
			}

		// User to select end of range
		ret = UserSelectionL(numberKey, numMethods, ETrue,
			aEndApiNum, aEndTestNum, aTestMode);
		}

	return ret;
	}

TInt  CUnitTestBase::UserChoice(TChar aNumberKey)
/**
 * This function gets the number equivalent to the user's keyboard input
 * and will return once the enter key is pressed
 *
 * @param  aNumberKey is a key pressed by the user
 * @return return the number equivalent to the user's keyboard input
 */
	{
	TInt choice=0;
	TInt mult=1;

	while ( STATIC_CAST(TInt,aNumberKey) != EKeyEnter )
		{
		// If the user pressed Backspace, delete the previous number.
		if (aNumberKey == 0x8)
			{
			if (choice > 0)
				{
				iTest.Printf(_L("%c %c"), aNumberKey, aNumberKey);
				choice /= 10;
				if (choice == 0)
					mult = 1;
				}
			}

		if (!aNumberKey.IsDigit())
			{
			aNumberKey = iTest.Console()->Getch();
			continue;
			}

		TInt val = aNumberKey.GetNumericValue();
		iTest.Printf(_L("%d"),val) ;
		choice = (choice * mult) + val;
		if(mult==1)
			mult *=  10;
		aNumberKey = iTest.Console()->Getch() ;
		}

	iTest.Printf(_L("\n")) ;
	return choice;
	}

TBool CUnitTestBase::CommandLineL(TInt& aApiNum, TInt& aTestNum,
								  TInt& aEndApiNum, TInt& aEndTestNum)
/**
 * This function takes inputs from command lines in the batch file run_me.bat
 */
	{
	aApiNum = 0;
	aTestNum = 0;

	TInt commandLineLength;
#ifndef BUILD_FOR_JETSTREAM
	commandLineLength = User::CommandLineLength();
#else
	RProcess eteltest;
	commandLineLength = eteltest.CommandLineLength();
#endif // BUILD_FOR_JETSTREAM

	if (commandLineLength > 0)
		{
		HBufC* argv = HBufC::NewLC(commandLineLength);
		TPtr cmd(argv->Des());

#ifndef BUILD_FOR_JETSTREAM
		User::CommandLine(cmd);
#else
		eteltest.CommandLine(cmd);
#endif // BUILD_FOR_JETSTREAM

		TLex argParser(*argv);

		TInt argc = 0;
		while (argParser.NextToken().Length()) argc++;
		if ((argc < 2) || (argc > 4))
			{
			iTest.Console()->Printf(_L("Usage: <app> <api num> <test num> [<end api> <end test>]\n"));
			return EFalse;
			}

		argParser.UnGetToMark(); //Returns to the beginning of argv.
		argParser.SkipSpace();

		argParser.Val(aApiNum);
		argParser.SkipSpace();
		argParser.Val(aTestNum);

		if (argc == 4)
			{
			argParser.SkipSpace();
			argParser.Val(aEndApiNum);
			argParser.SkipSpace();
			argParser.Val(aEndTestNum);
			}
		else
			{
			aEndApiNum = aApiNum;
			aEndTestNum = aTestNum;
			}

		delete argv;
		CleanupStack::Pop();//argv

		// we did find some command line params
		return ETrue;
		}
	return EFalse;
	}

TInt CUnitTestBase::WriteLog(TBool aIsPassed,TUint aFunIndex,TUint aApiNum,
		                     TUint aTstNum, TInt aErrorCode)
/**
 * This function write test result to log in the folowing formats:
 * AUTO;<DATE>;<TIME>;UTS;<TS-Identifier>;<Functionality-Index>;
 * PASSED;<Test-Script>;<Method-In-Test-Script>;
 * or
 * AUTO;<DATE>;<TIME>;UTS;<TS-Identifier>;<Functionality-Index>;
 * FAILED;<Test-Script>;<Method-In-Test-Script>;<ErrorCode>
 */
	{
	//Get date & time
	TTime time;
	TDateTime dateTime;
	time.HomeTime();
	dateTime=time.DateTime();
	TUint year = dateTime.Year();
	TUint day = dateTime.Day() + 1;//Day() & Month() starts from 0
	TUint month = dateTime.Month() + 1;
	TUint hour = dateTime.Hour();
	TUint minute = dateTime.Minute();
	TUint second = dateTime.Second();

	//Prepared the PASSED/FAILED field
	TBuf8<KBufSize10> passedOrFailed;
	if(aIsPassed)
		passedOrFailed.Copy(KPassed);
	else
		passedOrFailed.Copy(KFailed);

	TBuf8<KMaxName> testResult;

	testResult.Copy(KAutoTestMarker);//AUTO;

	//<DATE>;<TIME>;
	TBuf8<KBufSize25> dateTimeField;
	FormDateTimeBit(dateTimeField,day,ETrue,ETrue);
	FormDateTimeBit(dateTimeField,month,ETrue,ETrue);
	FormDateTimeBit(dateTimeField,year,ETrue,EFalse);
	FormDateTimeBit(dateTimeField,hour,EFalse,ETrue);
	FormDateTimeBit(dateTimeField,minute,EFalse,ETrue);
	FormDateTimeBit(dateTimeField,second,EFalse,EFalse);
	testResult.Append(dateTimeField);

	testResult.Append(iTestSpecName);//UTS;

	//<TS-Identifier>;
	testResult.Append(iTsIdentifier);
	testResult.Append(KSemiColon);

	//<Functionality-Index>;
	TBuf8<KBufSize25> buf;
	buf.Num(aFunIndex);
	buf.Append(KSemiColon);
	testResult.Append(buf);

	//PASSED/FAILED;
	testResult.Append(passedOrFailed);

	//<Test-Script>;
	testResult.Append(iTestScriptName);

	//<Method-In-Test-Script>
	TBuf8<KBufSize100> bufMethodWithId;
	bufMethodWithId.Copy(iMethodName);
	bufMethodWithId.Append(KHyphen);
	buf.Num(aApiNum);
	buf.Append(KHyphen);
	bufMethodWithId.Append(buf);
	buf.Num(aTstNum);
	buf.Append(KSemiColon);
	bufMethodWithId.Append(buf);
	testResult.Append(bufMethodWithId);

	if(!aIsPassed)
		{
		buf.Num(aErrorCode);
		testResult.Append(buf);
		}
	testResult.Append(KNewLine);

	TInt err = iLogFile.Write(testResult);
	iLogFile.Flush();

	return err;
	}

void CUnitTestBase::FormDateTimeBit(TDes8& aDateTimeField,TUint aDateTimeBit,
		                            TBool aIsDateSepa,TBool aAddSepa)
/**
 * Append a date/time bit(month & hour etc.) and separator or delimiter
 * into the dateTimeField descriptor
 */
	{
	TBuf8<KBufSize1> separator;//- or :
	TBuf8<KBufSize4> buf;//year takes 4 digits, the rest takes two digits
	if(aIsDateSepa)
		separator.Copy(KHyphen);
	else
		separator.Copy(KColon);
	if(aDateTimeBit < KZeroPadValue)
		aDateTimeField.Append(KZeroPadding);
	buf.Num(aDateTimeBit);
	aDateTimeField.Append(buf);
	if(aAddSepa)
		aDateTimeField.Append(separator);
	else
		aDateTimeField.Append(KSemiColon);
	}


TInt CUnitTestBase::LoadComms()
/**
 *
 */
	{
	// connect to C32
	TInt ret = iCommServer.Connect();
	if (ret)
		{
		return ret;
		}
	return ret;
	}

TInt CUnitTestBase::UnloadComms()
/**
 *
 */
	{
	iCommServer.Close();
	
	return (KErrNone);
	}

void CUnitTestBase::ReadStr(TDes& aDes)
	{
	TChar inputKey;
	TInt count = 0;
	const TInt KMaxLength = aDes.MaxLength();

	aDes.Zero();
	for (;;)
		{
		inputKey = (TInt) iTest.Getch();

		if ((TInt)inputKey == EKeyEnter)
			{
			iTest.Printf(_L("\n"));
			break;
			}

		if (inputKey == EKeyBackspace)
			{
			if (count > 0)
				{
				iTest.Printf(_L("%c"), inputKey);
				aDes.Delete(--count,1);
				}
			}
		else if (inputKey.IsPrint() && count < KMaxLength)
			{
			iTest.Printf(_L("%c"), inputKey);
			aDes.Append(inputKey);
			count++;
			}
		}
	}

void CUnitTestBase::GetCompilationDate(TInt& aDay, TInt& aMonth, TInt& /*aYear*/)
	{
	// The __DATE__ string is guaranteed to be of the form "Mmm dd yyyy"
	// eg. Sep  9 2002

	const TUint8* KMonthNames[] = {
		_S8("Jan"), _S8("Feb"), _S8("Mar"), _S8("Apr"), _S8("May"), _S8("Jun"),
		_S8("Jul"), _S8("Aug"), _S8("Sep"), _S8("Oct"), _S8("Nov"), _S8("Dec") };
	const TUint8 KDateString[] = __DATE__;
	const TPtrC8 KMonth(KDateString, 3);
	const TPtrC8 KDay(KDateString + 4, 2);
	const TPtrC8 KYear(KDateString + 7, 4);

	TLex8 lex(KDay);
	lex.SkipSpaceAndMark();
	//kmif (lex.Val(aDay)) PANIC(10000);
	aDay--;               // get a zero based day for compatibility with TDateTime
	lex.Assign(KYear);
	//kmif (lex.Val(aYear)) PANIC(10000);

	aMonth = KErrNotFound;
	for (TInt i = 0; i < 12; i++)
		{
		TPtrC8 month(KMonthNames[i], 3);
		if (month == KMonth) aMonth = i;
		}

	//kmif (aMonth < 0) PANIC(10000);
	//kmif (aMonth > 11) PANIC(10000);
	}

void CUnitTestBase::EnsureTargetDeviceClockIsSet()
	{
	TTime now;
	now.HomeTime();
	TDateTime date = now.DateTime();

	TInt month, dayInMonth, year;
	GetCompilationDate(dayInMonth, month, year);

	//LOGTEXT4("day %d  month %d  year %d", dayInMonth, month, year);

	// If boards current date is later than our compilation year, just return
	if (   date.Year() >= year
		&& date.Month() >= month
		&& date.Day() >= dayInMonth)
		{
		return;
		}

	const TInt KCompiledYear = year;
	for (;;)
		{
		iTest.Printf(_L("Enter date as DD MM YYYY (year is optional, defaults to %d) "), year);

		TBuf<12> input;
		TBuf8<12> input8;
		ReadStr(input);
		input.TrimAll();
		input8.Copy(input);

		TLex8 lex(input8);

		lex.Val(dayInMonth);
		lex.SkipSpaceAndMark();
		lex.Val(month);
		lex.SkipSpaceAndMark();
		lex.Val(year); // if no year entered, this just returns error - year is unchanged

		TInt err;
		err = date.SetYear(year);
		if (!err) err = date.SetMonth(static_cast<TMonth>(month-1));
		if (!err) err = date.SetDay(dayInMonth-1);
		if (!err && year < KCompiledYear) err = KErrNotReady;

		if (err == KErrNone)
			{
			break; // loop termination condition
			}
		else
			{
			iTest.Printf(_L("Does not appear to be a correctly formatted date, try again.\n"));
			month = 0;
			dayInMonth = 0;
			year = KCompiledYear;
			}
		}

	// Refresh the other time values
	now.HomeTime();
	date = now.DateTime();
	// Now restore the user-entered values
	date.SetYear(year);
	date.SetMonth(static_cast<TMonth>(month-1));
	date.SetDay(dayInMonth-1);
	User::SetHomeTime(date);
	}

void CUnitTestBase::RunUnitTestsL()
/**
 * This function is called within CreateTestL(), which is a
 * static member function of CUnitTestBase derived classes,  to run
 * the unit tests for the appropriate module
 */
	{
	// load the CSY
	iTest(LoadComms() == KErrNone);

#ifdef __EPOC32__
//	EnsureTargetDeviceClockIsSet();
#endif

	// do test sequence
	TBool noEnd = ETrue;
	while(noEnd)
		{
		noEnd = DoTestsL();
		if (iExitAfterTestsComplete)
			noEnd = EFalse;
		}

	// unload the CSY
	iTest(UnloadComms() == KErrNone);
	}

TInt CUnitTestBase::SetUpTest()
/**
 * This function is called within CreateTestL(), which is a
 * static member function of CUnitTestBase derived classes,
 * to load drivers & start C32
 *
 * If you want to give an 'opening page' of instructions for your unit test,
 * override this and do so. Remember to base call! See DataUnitTest for an
 * example.
 *
 * @return return an error code if set up fails
 */
	{
	iTest.Title();

	iTest.Start(_L("Loading Serial drivers\n"));

	TInt r;

#if defined (__WINS__)
	r = User::LoadPhysicalDevice(PDD_NAME);
    r = User::LoadLogicalDevice(LDD_NAME);

//taproot: tushar commented for OMAP
//#else  // __GCC32__ 
//	r = User::LoadPhysicalDevice(PDD_SIR_NAME);
//	r = User::LoadPhysicalDevice(PDD_UART_NAME);
//  r = User::LoadLogicalDevice(LDD_NAME);
#endif

	r = StartC32();
	if ((r != KErrNone) && (r != KErrAlreadyExists))
		iTest.Printf(_L("Failed to start C32 %d\n"), r);

	return KErrNone;
	}

void CUnitTestBase::PrintTestSpecIdentifiers()
	{
	// This version prints no helpful info on the screen. Overload this in
	// your own test class to put out test identifier and number of API test
	}

void CUnitTestBase::PrintTestDetails(TInt /*aTstSpecNumber*/)
	{
	// This version prints no helpful info on the screen. Overload this in
	// your own test class to put out test details for the test spec idenifier
	// selected
	}

void CUnitTestBase::SetFunIndex2(TUint aFunIndex1,TUint aFunIndex2)
	{
	iFunIndices.Insert(aFunIndex1,0);
	iFunIndices.Insert(aFunIndex2,1);
	}

void CUnitTestBase::SetFunIndex3(TUint aFunIndex1,TUint aFunIndex2,
		                         TUint aFunIndex3)
	{
	iFunIndices.Insert(aFunIndex1,0);
	iFunIndices.Insert(aFunIndex2,1);
	iFunIndices.Insert(aFunIndex3,2);
	}

void CUnitTestBase::SetFunIndex4(TUint aFunIndex1,TUint aFunIndex2,
		                         TUint aFunIndex3,TUint aFunIndex4)
	{
	iFunIndices.Insert(aFunIndex1,0);
	iFunIndices.Insert(aFunIndex2,1);
	iFunIndices.Insert(aFunIndex3,2);
	iFunIndices.Insert(aFunIndex4,3);
	}

TInt CUnitTestBase::GetRequestOrTimeOut(TRequestStatus& aRequestStatus,
										TTimeIntervalMicroSeconds32 aTimeOut)
/**
 * Acts exactly like User::WaitForRequest, except aTimeOut is a timeout period
 * to give up waiting after.
 * If the request times out, returns KErrTimedOut, otherwise returns KErrNone
 * and aRequestStatus is completed as usual.
 */
	{
	//LOGTEXT(">>GetRequestOrTimeOut");

	RTimer timer;
	TRequestStatus timerRequest;
	timer.CreateLocal();
	timer.After(timerRequest, aTimeOut);

	// Wait for the requestto complete or until we time out
	User::WaitForRequest(timerRequest, aRequestStatus);

	// If timer is no longer pending => we have timed out
	if (timerRequest != KRequestPending)
		{
		//LOGTEXT("<<GetRequestOrTimeOut **** Timed out ****");
		return KErrTimedOut;
		}

	timer.Cancel();

	User::WaitForRequest(timerRequest);

	//LOGTEXT2("<<GetRequestOrTimeOut [aRequestStatus=%d]",aRequestStatus);
	return KErrNone;
	}
