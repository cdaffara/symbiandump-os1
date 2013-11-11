// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestSuite which is the base class for all the TestSuite DLLs
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// do not export if Unit Testing
#if defined (__TSU_TESTFRAMEWORK__)
#undef EXPORT_C
#define EXPORT_C
#endif

/**
 *
 * Default test suite version string
 *
 * @xxxx 
 *
 */
_LIT(KTxtVersion,"?.?");

/**
 *
 * Test suite destructor
 * This destroys all the test steps contained in this class and
 * in classes derived from it
 *
 * @xxxx 
 *
 */
EXPORT_C CTestSuite::~CTestSuite()
	{
	// free all test steps
	if (iArrayTestSteps)
		iArrayTestSteps->ResetAndDestroy();

	// free the dynamic array used for test steps
	delete iArrayTestSteps;

	}

/**
 *
 * Test suite constructor (second phase)
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::ConstructL()
	{
	// create a new Array to store the test steps in
	iArrayTestSteps = new(ELeave) CArrayPtrFlat<RTestStep>(1);

	// default severity
	SetSeverity(ESevrAll);
	iLogger = NULL;

	// initialise the derived test suites
	InitialiseL();
	}

/**
 *
 * Get test suite version.
 *
 * NOTE : this function is not pure virtual i.e. a test suite
 * does not have to provide a version string - however this
 * may change. It is strongly recommended that a test suite DLL
 * overrides this method.
 *
 * @return	"TPtrC"
 *			The version string.
 *
 * @xxxx 
 *
 */
EXPORT_C TPtrC CTestSuite::GetVersion() const
	{
	return KTxtVersion();
	}

/**
 *
 * Add a test step into the suite
 *
 * @param	"RTestStep* aTestStep"
 *			The test step to add.
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::AddTestStepL(RTestStep* aTestStep)
	{
	__ASSERT_ALWAYS(aTestStep, User::Panic(_L("CTestSuite::AddTestStepL"), KErrArgument));
	// test steps contain a pointer back to the suite which owns them
	aTestStep->SetSuite(this);
	// add the step; order is not important, so add it at position 1
	iArrayTestSteps->AppendL(aTestStep, 1);
	}


/**
 *
 * Perform a test step.
 *
 * @param	"const TDesC& aStep"
 *			The test step to run.
 *
 * @param	"const TDesC& aConfig"
 *			The configuration file name.
 *
 * @return	"TVerdict"
 *			Result of the test.
 *
 * @xxxx 
 *
 */
EXPORT_C TVerdict CTestSuite::DoTestStep(const TDesC& aStep, const TDesC& aConfig, const TDesC& aParamSet)
	{
	// This function traps leaves in the test steps, so should never leave

	//	get the number of tests available in this suite
	TInt noOfTests = iArrayTestSteps->Count();

	// search the available test steps for the required one
	for (TInt i = 0; i < noOfTests; i++)
	{
		RTestStep* theStep = iArrayTestSteps->At(i);
		if (theStep->StepName().MatchF(aStep) != KErrNotFound)
			{

			// found required test so initialise to PASS
			theStep->SetResult(EPass);
			
			// set the param set to use
			theStep->SetDefaultParamSet(aParamSet);

			// pass the config file info into the test step
			theStep->LoadConfig(aConfig);

			// assume it's going to work
			TVerdict result = EPass;

			// set step status
			iStepStatus = EStepStatusStart;

			// execute pre-preamble (cleanup stack growth)
			TRAPD(rPreOpen, theStep->PreOpenL());

			if (rPreOpen != KErrNone)
				{
				WARN_PRINTF4(_L("Warning: Test step:%S PreOpenL in suite:%S left, error %d"), &aStep, &iSuiteName, rPreOpen);
				result = EInconclusive;
				}
			else
				{
				// execute test step preamble (if any) inside a trap
				TRAPD(rPreAmble, result = theStep->OpenL());

				if (result != EPass)
					{
					WARN_PRINTF3(_L("Warning: Test step:%S preamble in suite:%S failed"), &aStep, &iSuiteName);
					// here : call to virtual cleanup function in test step, if it exists...
					theStep->CleanupAfterOpenFail();
					result = EInconclusive;
					}
				else if (rPreAmble != KErrNone)
					{
					WARN_PRINTF4(_L("Warning: Test step:%S preamble in suite:%S left, error %d"), &aStep, &iSuiteName, rPreAmble);
					// here : call to virtual cleanup function in test step, if it exists...
					theStep->CleanupAfterOpenFail();
					result = EInconclusive;
					}
				}

			// only continue if the preamble passed
			if (result == EPass)
				{

				// set step status
				iStepStatus = EStepStatusPreamble;

				// now execute test step inside a trap
				TRAPD(r, result = theStep->DoTestStepL());

				if (r!= KErrNone)
					{
					WARN_PRINTF4(_L("Warning: Test step:%S in suite:%S left, error %d"), &aStep, &iSuiteName, r);
					result = EFail;
					}

				// set step status
				iStepStatus = EStepStatusTest;

				// execute test step postamble
				// NB - postamble does not leave, but may panic
				theStep->Close();

				// set step status
				iStepStatus = EStepStatusFinished;
				}

			// *** TO DO - check if there are problems when the preamble leaves.
			// In this case the postamble is not called - there may be memory leaks.
			// (Preambles may allocate memory, call __UHEAP_MARK, etc.)
			// If so, move Close() to this point, and check again.

			// clean up Config data object
			theStep->UnloadConfig();

			return result;

			}
		}

	// suite has been searched but test step not found, so error.
 	ERR_PRINTF3(_L("Failed to find test step:%S in suite:%S"), &aStep, &iSuiteName);

	return ETestSuiteError;

	}

/**
 *
 * Gets a step's heap and stack size.
 *
 * @param	"const TDesC& aStep"
 *			The step name
 * @param	"TInt* aHeapSize"
 *			Returns the step's heap size
 * @param	"TInt* aStackSize"
 *			Returns the step's stack size
 *
 * @xxxx
 *
 */
EXPORT_C void CTestSuite::GetHeapAndStackSize(const TDesC& aStep, TInt* aHeapSize, TInt* aStackSize)
	{
	//get the number of tests available in this suite
	TInt noOfTests = iArrayTestSteps->Count();

	// search the available test steps for the required one
	for (TInt i = 0; i < noOfTests; i++)
		{
		RTestStep* theStep = iArrayTestSteps->At(i);
		if (theStep->StepName().MatchF(aStep) != KErrNotFound)
			{

			// found required test, so get the stack and heap size
			*aHeapSize = theStep->HeapSize();
			*aStackSize = theStep->StackSize();
			}
		}
	}

/**
 *
 * General logging function for test suites.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::Log(TRefByValue<const TDesC16> aFmt, ...)
	{

	VA_LIST aList;
	VA_START(aList, aFmt);

	if(iLogger) 
		iLogger->Log(aFmt, aList);

	VA_END(aList);
	}

/**
 *
 * General logging function for test suites, with severity.
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::Log(TInt aSeverity, TRefByValue<const TDesC16> aFmt, ...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	if(LogSeverity::IsActive(aSeverity, Severity()))
		{
		if(iLogger) 
			iLogger->Log(aFmt, aList);
		}

	VA_END(aList);
	}

/**
 *
 * Traceable logging function for test suites.
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	if(LogSeverity::IsActive(aSeverity, Severity()))
		{
		if(iLogger)
			{
			iLogger->LogExtra(aFile, aLine, aSeverity, aFmt, aList);
			}
		}

	VA_END(aList);
	}


/**
 *
 * Traceable Boolean condition tester.
 *
 * @param	"TBool aCondition"
 *			Condition to be checked
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::TestBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine)
	{

	// check condition
	if (aCondition)
		return;

	// convert filename for log
	TBuf<KMaxLogFilenameLength> fileName;
	TPtrC8 fileName8(aFile);
	fileName.Copy(fileName8);  // TText8->TBuf16

	// display a log message
 	ERR_PRINTF3(_L("Test Failed in file:%S line:%d"), &fileName, aLine);

	// leave with error code
	User::Leave(KTestErrorCode);

	}

/**
 *
 * Set log severity.
 *
 * @param	"TInt aSeverity"
 *			The required severity
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::SetSeverity(TInt aSeverity)
	{
	iSeverity = aSeverity;
	}

/**
 *
 * Get log severity.
 *
 * @return	"TInt"
 *			The current severity
 *
 * @xxxx 
 *
 */
EXPORT_C TInt CTestSuite::Severity() const
	{
	return iSeverity;
	}

/**
 *
 * Set logging system.
 *
 * @param	"CLog *aLogger"
 *			The log to use.
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::SetLogSystem(CLog* aLogger)
	{
	iLogger = aLogger;
	}

/**
 *
 * Get logging system.
 *
 * @return	"CLog*"
 *			The log the test suite is currently using.
 *
 * @xxxx 
 *
 */
EXPORT_C CLog* CTestSuite::LogSystem() const
	{
	return iLogger;
	}

/**
 *
 * Set step status.
 *
 * @param	"TTestStepStatus aStatus"
 *			The status to set.
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestSuite::SetStepStatus(TTestStepStatus aStatus)
	{
	iStepStatus = aStatus;
	}

/**
 *
 * Get step status.
 *
 * @return "TTestStepStatus"
 *			The step status.
 *
 * @xxxx 
 *
 */
EXPORT_C TTestStepStatus CTestSuite::StepStatus() const
	{
	return iStepStatus;
	}
