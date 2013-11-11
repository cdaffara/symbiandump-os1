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
// This module contains CParseLine and CSuiteDll classes
// CParseLine contains the functions required to execute
// a line of test script file.
// CSuiteDll objects contains information about test suite
// dlls that have been loaded.
// 
//

// system includes
#include <f32file.h>

// test system includes
#include "TestFramework.h"
#include "script.h"
#include "parseline.h"
#include "Filename.h" 

#include "parseline.inl"

const TInt KTimeIncrement = 100*1000000; // max wait interval in micro-seconds (100s)
const TUint KMaxThreadAttempts = 128;	// max number of times to attempt to create a thread

/**
 *
 * File path literals
 *
 * @xxxx
 *
 */
_LIT(KTxtDLLpath, "c:\\;c:\\system\\libs;d:\\;d:\\system\\libs;e:\\;e:\\system\\libs;z:\\;z:\\system\\libs");

/**
 *
 * Script parameter defaults
 *
 * @xxxx
 *
 */
//const TInt KTestGuardTimerDefault = 1000L;	// EABI warning removal
const TInt KPanicGuardTimerDefault = 1000000L;
const TInt KPanicExitReasonDefault = 0;

/**
 *
 * CParseLine first-phase constructor
 *
 * @xxxx
 *
 */
CParseLine::CParseLine(const TDesC& aMatchString)
	:iTestVerdict(EPass), iMatchString(aMatchString)
	{
	}

/**
 *
 * CParseLine second-phase constructor 
 *
 * @param	"CScript* aScript"
 *			The script to be parsed
 *
 * @param	"CTestUtils* aTestUtils"
 *			The TestUtils object to use
 *
 * @param	"CLog* aLog"
 *			The logger to use
 *
 * @xxxx
 *
 */
void CParseLine::ConstructL(CScript* aScript, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer)
	{
	// create a new Array to store the test steps in
	iArrayLoadedSuiteDll = new(ELeave) CArrayPtrFlat<CSuiteDll>(1);

	iScript = aScript;
	iTestUtils = aTestUtils;
	iLog = aLog;
	iGuardTimer = aGuardTimer;
	iSeverity = ESevrAll;
	iBreakOnError = EFalse;
	}

/**
 *
 * CParseLine static constructor 
 *
 * @param	"CScript* aScript"
 *			The script to be parsed
 *
 * @param	"CTestUtils* aTestUtils"
 *			The TestUtils object to use
 *
 * @param	"CLog* aLog"
 *			The logger to use
 *
 * @xxxx
 *
 */
CParseLine* CParseLine::NewL(CScript* aScript, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	CParseLine* self = new(ELeave) CParseLine(aMatchString);
	CleanupStack::PushL(self);
	self->ConstructL(aScript, aTestUtils, aLog, aGuardTimer);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 * CParseLine destructor 
 *
 * @xxxx
 *
 */
CParseLine::~CParseLine()
	{

	// unload DLLs and their records
	if (iArrayLoadedSuiteDll)
		{
		// delete all objects in iArrayLoadedSuiteDll
		// the destructors will unload any loaded DLLS
		iArrayLoadedSuiteDll->ResetAndDestroy();
		delete iArrayLoadedSuiteDll;
		}

	}

/**
 *
 * Process a single line from the script file.
 *
 * @param	"const TDesC8& aNarrowline"
 *			The script line
 *
 * @param	"TInt8 aLineNo"
 *			The script line number
 *
 * @xxxx
 *
 */
void CParseLine::ProcessLineL(const TDesC8& aNarrowline, TInt aLineNo)
	{
	// make a local unicode buffer
	TPtr16 lineBuf(REINTERPRET_CAST(TUint16*,User::AllocLC(KMaxLenScriptLine*2)), 0, KMaxLenScriptLine);
	lineBuf.Fill('\0', KMaxLenScriptLine);

	// convert the narrow script file to Unicode
	// TBC find a better way to do this
	CFileName* testnameU = CFileName::NewLC();
	testnameU->Copy(aNarrowline);

	// find the end of the line
	TInt end = testnameU->Locate('\n');

	// copy the line into lineBuf
	if ((end != KErrNotFound) && (end < KMaxLenScriptLine))
		lineBuf = testnameU->Left(end - 1);
	else
		lineBuf = testnameU->FileName();

	// destroy filename
	CleanupStack::PopAndDestroy(testnameU);

	// the parser relies on spaces between tokens. Commas are
	// allowed but are just replaced with spaces
	TInt findComma = lineBuf.Locate(TChar(','));
	while (findComma != KErrNotFound )
		{
		// found a comma so replace with space
		lineBuf.Replace(findComma, 1, _L(" "));
		findComma = lineBuf.Locate(TChar(','));
		}

	// for debugging display the line with a line no
#ifdef SCRIPT_DEBUG
	INFO_PRINTF3(_L("Line:%d %S "), aLineNo, &lineBuf);
#endif

	// if there has been a failure and the user has selected
	// x then the next commands in the script are skipped until
	// a test complete statement is found
	if (iBreakOnError)
		{
		if (lineBuf.FindF(_L("TEST_COMPLETE")) == 0)
			{
			TestComplete(lineBuf);
			// reset flag now test complete found
			iBreakOnError = EFalse;
			}

		CleanupStack::PopAndDestroy(); // linebuf
		// do not process the rest of the line
		return;
		}

	// check the line for command keywords
	if ((lineBuf.Find(_L("//")) == 0) || (lineBuf.Find(_L("#")) == 0))
		{
		// ignore comments
		}
	else if (lineBuf.FindF(_L("LOAD_SUITE")) == 0)
		{
		LoadSuiteL(lineBuf);
		}
	else if (lineBuf.FindF(_L("RUN_SCRIPT")) == 0)
		{
		RunScriptL(lineBuf);
		}
	else if (lineBuf.FindF(_L("RUN_TEST_STEP")) == 0)
		{
		RunTestStep(lineBuf);
		}
	else if (lineBuf.FindF(_L("RUN_PANIC_STEP")) == 0)
		{
		RunPanicTestStep(lineBuf);
		}
	else if (lineBuf.FindF(_L("RUN_TERMINATION_STEP")) == 0)
		{
		RunTerminationTestStep(lineBuf);
		}
	else if (lineBuf.FindF(_L("RUN_UTILS")) == 0)
		{
		RunUtil(lineBuf);
		}
	else if (lineBuf.FindF(_L("RUN_PROGRAM")) == 0)
		{
		RunProgram(lineBuf);
		}
	else if (lineBuf.FindF(_L("UNLOAD")) == 0)
		{
		Unload();
		}
	else if (lineBuf.FindF(_L("HEAP_MARK")) == 0)
		{
		HeapMark();
		}
	else if (lineBuf.FindF(_L("HEAP_CHECK")) == 0)
		{
		HeapCheck();
		}
	else if (lineBuf.FindF(_L("REQUEST_MARK")) == 0)
		{
		RequestMark();	
		}
	else if (lineBuf.FindF(_L("REQUEST_CHECK")) == 0)
		{
		RequestCheck();
		}
	else if (lineBuf.FindF(_L("HANDLES_MARK")) == 0)
		{
		HandlesMark();
		}
	else if (lineBuf.FindF(_L("HANDLES_CHECK")) == 0)
		{
		HandlesCheck();
		}
	else if (lineBuf.FindF(_L("PRINT")) == 0)
		{
		ScriptPrint(lineBuf);
		}
	else if (lineBuf.FindF(_L("DELAY")) == 0)
		{
		Delay(lineBuf);
		}
	else if (lineBuf.FindF(_L("SEVERITY")) == 0)
		{
		SetSeverity(lineBuf);
		}
	else if (lineBuf.FindF(_L("PAUSE_AT_END")) == 0)
		{
		// if implemented, add iScript->iPauseAtEnd = ETrue;
		WARN_PRINTF1(_L("Warning : PAUSE_AT_END not implemented"));
		}
	else if (lineBuf.FindF(_L("MULTITHREAD")) == 0)
		{
		WARN_PRINTF1(_L("Warning : MULTITHREAD keyword no longer required"));
		}
	else if (lineBuf.FindF(_L("SINGLETHREAD")) == 0)
		{
		ERR_PRINTF1(_L("Error : Single thread operation no longer supported"));
		}
	else if (lineBuf.FindF(_L("PAUSE")) == 0)
		{
		iScript->Pause();
		}
	else if (lineBuf.FindF(_L("BREAK_ON_ERROR")) == 0)
		{
		// if the current test verdict is not PASS
		// give the user the chance to quit
		if ( iTestVerdict != EPass )
			iBreakOnError = iScript->BreakOnError();
		}
	else if (lineBuf.FindF(_L("TEST_COMPLETE")) == 0)
		{
		// use Tlex to decode the cmd line
		TestComplete(lineBuf);
		}
	else if (lineBuf.FindF(_L("LOG_SETTINGS")) == 0)
		{
		// use Tlex to decode the cmd line
		LogSettings(lineBuf);
		}
	else if (lineBuf.Length() == 0)
		{
		// ignore blank lines
		}
	else
		{
		// failed to decode line
		ERR_PRINTF3(_L("Error in script line:%d - \'%S\'"), aLineNo, &lineBuf);
		}

	CleanupStack::PopAndDestroy(); // linebuf
	}

/**
 *
 * Implements the TEST_COMPLETE script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::TestComplete(const TDesC& aText)
	{
	// use Tlex to decode the cmd line
	TLex lex(aText);

	// start at the begining
	TPtrC token = lex.NextToken();

	// get suite name, if any
	token.Set(lex.NextToken());

	if (token.Length() != 0)
		{
		TBuf<KMaxLenTestSuiteName> currentSuiteName;
		currentSuiteName = token;

		// get step name, if any
		token.Set(lex.NextToken());
	
		if (token.Length() != 0)
			{	
			iCurrentStepName = token;
			iCurrentSuiteName = currentSuiteName;
			}
		else
			{
			// failed to decode line - require 0 or 2 parameters exactly
			// use last suite/step name, return fail
			ERR_PRINTF2(_L("Error in script line: \'%S\'"), &aText);
			iTestVerdict = EFail;
			}
		}

	if (!iSkip)
		{
		// add the current result to the script
		iScript->AddResult(iTestVerdict);		
		}
		
	// reset for next test
	iTestVerdict = EPass;
	}

/**
 *
 * Implements the PRINT script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::ScriptPrint(const TDesC& aText)
	{
	// display the text after the PRINT and 1 space = 6
	INFO_PRINTF2(_L("%s "), (aText.Ptr() + 6));
	}

/**
 *
 * Implements the DELAY script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::Delay(const TDesC& aText)
	{
	// if the test has already failed skip the delay
	if (iTestVerdict != EPass)
		{
		WARN_PRINTF1(_L("Skipped delay as test has already failed"));
		return;
		}

	// get the required time for the delay
	// first get the value as a string
	TLex timeOut(aText);
	timeOut.NextToken();
	TPtrC token = timeOut.NextToken();

	// convert the value into a TInt
	TLex lexTime(token);
	TInt64 guardTimerValue;
	if (lexTime.Val(guardTimerValue) != KErrNone  )
		{
		ERR_PRINTF2(_L("Error in guard timer value : could not decode \'%S\' as value"), 
					&token);
		return;
		}

	INFO_PRINTF2(_L("Delay for %ld mS"), guardTimerValue);

	// wait for the required delay
	User::After(I64INT(guardTimerValue) * 1000);
	}
	
/**
 *
 * Implements the SEVERITY script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::SetSeverity(const TDesC& aText)
	{
	// get the required time for the delay
	// first get the value as a string
	TLex severityOut(aText);
	severityOut.NextToken();
	TPtrC token = severityOut.NextToken();

	// convert the value into a TInt
	TLex lexSeverity(token);
	TInt severityValue = ESevrAll;
	if (lexSeverity.Val(severityValue) != KErrNone)
		{
		ERR_PRINTF2(_L("Error in severity level value : could not decode \'%S\' as value"),
					&token);
		return;
		}

	// check severity value to ensure that only bitmasks in use are set...
	if(!LogSeverity::IsValid(severityValue))
		{
		ERR_PRINTF1(_L("Error in severity value : out of range"));
		return;
		}
	else
		{
		iSeverity = severityValue;

		TInt noOfDlls = iArrayLoadedSuiteDll->Count();
		for ( TInt i = 0; i < noOfDlls; i++)
			{
			CSuiteDll* ptrSuite = iArrayLoadedSuiteDll->At(i);
			CTestSuite* testSuite = ptrSuite->Suite();
			testSuite->SetSeverity(iSeverity);
			}
		}

	INFO_PRINTF2(_L("Severity is set to %d"), severityValue);
	}

/**
 *
 * Implements the RUN_SCRIPT script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::RunScriptL(const TDesC& aText)
	{
	// use Tlex to decode the cmd line
	TLex lex(aText);

	// start at the begining
	TPtrC token=lex.NextToken();

	// step over the keyword
	token.Set(lex.NextToken());

	// format for printing
	INFO_PRINTF2(_L("RUN_SCRIPT %S"), &token);

	// create a new Script object (but use the current parser
	// as it has the dll loaded record)
	CScript* newScript=CScript::NewLC(this, iTestUtils, iLog, iGuardTimer, iMatchString);

	// read in the script file
	CFileName* scriptFileName = CFileName::NewLC();
	*scriptFileName = token;

	if (newScript->OpenScriptFile(scriptFileName))
		{
		// process it
		iTestVerdict = newScript->ExecuteScriptL();

		// don't bother logging verdicts for scripts - not really useful
		// add results from the new script to the owner script
		iScript->AddResult(newScript);
		}
	else
		{
		// failed to find script so verdict incloncusive
		iTestVerdict = EInconclusive;
		}

	CleanupStack::PopAndDestroy(scriptFileName);
	CleanupStack::PopAndDestroy(newScript);
	}

/**
 *
 * Implements the RUN_TEST_STEP script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::RunTestStep(const TDesC& aText)
	{
	// use TLex to decode the cmd line
	TLex lex(aText);

	// step over keyword
	lex.NextToken();

	// get guard timer
	TPtrC timeout;
	timeout.Set(lex.NextToken());

	// get the other parameters
	TPtrC suite, step, config, name, paramSet;
	suite.Set(lex.NextToken());
	step.Set(lex.NextToken());
	config.Set(lex.NextToken());
	name.Set(lex.NextToken());
	if (name.Length()==0)
		{
		// name is optional, if not given use step 
		name.Set(step);
		}
	paramSet.Set(lex.NextToken()); 
	if (paramSet.Length()==0)
		{
		// paramSet is optional, if not given use name 
		paramSet.Set(name);
		}

	// save the name of the current test suite / step
	iCurrentSuiteName = suite;
	iCurrentStepName = name;

	TVerdict currentTestVerdict;

	INFO_PRINTF2(_L("<a name=\"%S\"</a>"),&name);
	
	if (iMatchString.Length()>0 && name.Match(iMatchString)<0)
		{
		// we have a match string but no match - so skip
		INFO_PRINTF2(_L("TEST_STEP:%S skipped"), &name);
		iSkip = ETrue;
		return;
		}
		
	iSkip = EFalse;

	// convert the guard timer value to a TInt64
	TLex lexTimeOut(timeout);
	TInt64 guardTimerValue;
	if (lexTimeOut.Val(guardTimerValue) != KErrNone)
		{
		ERR_PRINTF2(_L("Error in guard timer value: %S"),
					&timeout);
		currentTestVerdict = EInconclusive;
		}

	else
		{
		// override guard timer if necessary
		if((guardTimerValue == KNoGuardTimer) && (iGuardTimer != KNoGuardTimer))
			{
			INFO_PRINTF3(_L("Warning : Guard timer value overridden from %ld to %ld"),
							guardTimerValue, iGuardTimer);
			guardTimerValue = iGuardTimer;
			}

		// log the start of a test step
		INFO_PRINTF7(_L("RUN_TEST_STEP:%S (step:%S suite:%S timeout:%ldmS config:%S(%S))"),
					&name, &step, &suite, guardTimerValue, &config, &paramSet);

		// NOTE. Now running multithreaded all the time.
		currentTestVerdict = DoTestNewThread(suite, step, guardTimerValue, config, paramSet);
		}

	TPtrC verdictText = CLog::TestResultText(currentTestVerdict);
	
	INFO_PRINTF3(_L("TEST_STEP:%S returned:%S "), 
				&name, &verdictText);

	// this result is only significant if everything else has passed
	if (iTestVerdict == EPass)
		iTestVerdict = currentTestVerdict;

	}

/**
 *
 * Implements the RUN_PANIC_STEP script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::RunPanicTestStep(const TDesC& aText)
	{
	// NOTE. RUN_PANIC_STEP now incorporates the panic reason and category

	// use Tlex to decode the cmd line
	TLex lex(aText);

	// start at the begining
	TPtrC timeout=lex.NextToken();

	// step over the keyword
	timeout.Set(lex.NextToken());

	// get the other parameters
	TPtrC suite, step;
	TPtrC category, reason;
	TPtrC config, name, paramSet;

	suite.Set(lex.NextToken());
	step.Set(lex.NextToken());
	category.Set(lex.NextToken());
	reason.Set(lex.NextToken());
	config.Set(lex.NextToken());
	name.Set(lex.NextToken());
	if (name.Length()==0)
		{
		// name is optional, if not given use step 
		name.Set(step);
		}
	paramSet.Set(lex.NextToken()); 
	if (paramSet.Length()==0)
		{
		// paramSet is optional, if not given use name 
		paramSet.Set(name);
		}

	if (iMatchString.Length()>0 && name.Match(iMatchString)<0)
		{
		// we have a match string but no match - so skip
		INFO_PRINTF2(_L("TEST_STEP:%S skipped"), &name);
		iSkip = ETrue;
		return;
		}
		
	iSkip = EFalse;

	// save the name of the current test suite / step
	iCurrentSuiteName = suite;
	iCurrentStepName = name;

	// convert the guard timer value to a TInt
	TLex lexTimeOut(timeout);
	TInt64 guardTimerValue;
	if (lexTimeOut.Val(guardTimerValue) != KErrNone)
		{
		ERR_PRINTF3(_L("Error in guard timer value:%S using default %dmS"), 
					&timeout, KPanicGuardTimerDefault);
		guardTimerValue = KPanicGuardTimerDefault;
		}

	// convert the exitReason value to a TInt
	TLex lexReason(reason);
	TInt exitReason;
	if (lexReason.Val(exitReason) != KErrNone)
		{
		ERR_PRINTF3(_L("Error in exitReason value:%S using default %d"), 
					&reason, KPanicExitReasonDefault);
		exitReason = KPanicExitReasonDefault;
		}

	// override guard timer if necessary
	if((guardTimerValue == KNoGuardTimer) && (iGuardTimer != KNoGuardTimer))
		{
		INFO_PRINTF3(_L("Warning : Guard timer value overridden from %ld to %ld"),
						guardTimerValue, iGuardTimer);
		guardTimerValue = iGuardTimer;
		}

	// log the start of a test step
	INFO_PRINTF9(_L("RUN_PANIC_STEP:%S (step:%S suite:%S timeout:%ldmS category:%S reason:%d config:%S(%S))"),
				&name, &step, &suite, guardTimerValue, &category, exitReason, &config, &paramSet);

	// run the test step
	TVerdict currentTestVerdict;

	// now running multithreaded all the time
	currentTestVerdict = DoPanicTest(suite, step, guardTimerValue,
									 category, exitReason, config, paramSet);

	TPtrC verdictText = CLog::TestResultText(currentTestVerdict);
	INFO_PRINTF3(_L("TEST_STEP:%S returned:%S "), 
				&name, &verdictText);

	// this result is only significant if every thing else has passed
	if (iTestVerdict == EPass)
		iTestVerdict = currentTestVerdict;

	}
	
/**
 *
 * Implements the RUN_TERMINATION_STEP script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::RunTerminationTestStep(const TDesC& aText)
	{
	// use Tlex to decode the cmd line
	TLex lex(aText);

	// start at the begining
	TPtrC timeout=lex.NextToken();

	// step over the keyword
	timeout.Set(lex.NextToken());

	// get the other parameters
	TPtrC suite, step;
	TPtrC reason;
	TPtrC config;

	suite.Set(lex.NextToken());
	step.Set(lex.NextToken());
	reason.Set(lex.NextToken());
	config.Set(lex.NextToken());

	// save the name of the current test suite / step
	iCurrentSuiteName = suite;
	iCurrentStepName = step;

	// convert the guard timer value to a TInt
	TLex lexTimeOut(timeout);
	TInt64 guardTimerValue;
	if (lexTimeOut.Val(guardTimerValue) != KErrNone)
		{
		ERR_PRINTF3(_L("Error in guard timer value:%S using default %dmS"), 
					&timeout, KPanicGuardTimerDefault);
		guardTimerValue = KPanicGuardTimerDefault;
		}

	// convert the exitReason value to a TInt
	TLex lexReason(reason);
	TInt exitReason;
	if (lexReason.Val(exitReason) != KErrNone)
		{
		ERR_PRINTF3(_L("Error in exitReason value:%S using default %d"), 
					&reason, KPanicExitReasonDefault);
		exitReason = KPanicExitReasonDefault;
		}

	// override guard timer if necessary
	if((guardTimerValue == KNoGuardTimer) && (iGuardTimer != KNoGuardTimer))
	{
		INFO_PRINTF3(_L("Warning : Guard timer value overridden from %ld to %ld"),
						guardTimerValue, iGuardTimer);
		guardTimerValue = iGuardTimer;
	}

	// log the start of a test step
	INFO_PRINTF6(_L("RUN_TERMINATION_STEP:%S suite:%S timeout:%ldmS reason:%d config:%S"),
				&step, &suite, guardTimerValue, exitReason, &config);

	// run the test step
	TVerdict currentTestVerdict;

	// now running multithreaded all the time
	currentTestVerdict = DoTerminationTest(suite, step, guardTimerValue,
									 exitReason, config);

	TPtrC verdictText = CLog::TestResultText(currentTestVerdict);
	INFO_PRINTF3(_L("TEST_STEP:%S returned:%S "), 
				&step, &verdictText);

	// this result is only significant if every thing else has passed
	if (iTestVerdict == EPass)
		iTestVerdict = currentTestVerdict;

	}

/**
 *
 * Implements the RUN_UTILS script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::RunUtil(const TDesC& aText)
	{
	// Call the utils
	iTestUtils->RunUtils(aText);
	}

/**
 *
 * Implements the REBOOT script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::Reboot()
	{
	WARN_PRINTF1(_L("Warning : REBOOT command not implemented"));
	}

/**
 *
 * Static function to call DoTestStep which is run
 * in a separate thread
 *
 * @param	"TAny* aPtr"
 *			The test step data
 *
 * @return "TInt"
 *			EPOC error code
 *
 * @xxxx
 *
 */
TInt CParseLine::ThreadFunctionL(TAny* aPtr)
	{
	TInt result = KErrNone;

	// get clean-up stack
  	CTrapCleanup* trapCleanup = CTrapCleanup::New();

	TRAPD(err, result = ThreadTrapFunctionL(aPtr));

	delete trapCleanup;
	return((err != KErrNone) ? err : result);
	}

/**
 *
 * Main function to call DoTestStep, called from within
 * a trap
 *
 * @param	"TAny* aPtr"
 *			The test step data
 *
 * @return "TInt"
 *			EPOC error code
 *
 * @xxxx
 *
 */
TInt CParseLine::ThreadTrapFunctionL(TAny* aPtr)
	{
	// get the data for the test
	CStepData* data = REINTERPRET_CAST(CStepData*, aPtr);
	CSuiteDll* suiteDll = data->SuiteDll();
	CTestSuite* testSuite = suiteDll->Suite();

	// setup local logger
	CLog* logClient	= CLog::NewLC();
	logClient->OpenLogFileL();
	testSuite->SetLogSystem(logClient);

	// do the test step 
	TVerdict result =  testSuite->DoTestStep(data->Step(), data->Config(), data->ParamSet());

	// NB it is the CALLING program's responsibility to save/restore the logger.
	// If the thread terminates prematurely, the logger is in an undefined state.

	CleanupStack::PopAndDestroy(logClient);
	testSuite->SetLogSystem(NULL);

	// return the test result
	return result;
	}

/**
 *
 * Do a test step in a new thread.
 *
 * @param	"const TDesC& aSuite"
 *			The test suite
 *
 * @param	"const TDesC& aStep"
 *			The test step
 *
 * @param	"TInt aGuardTimerValue"
 *			The guard timer value
 *
 * @param	"const TDesC& aConfig"
 *			The config data
 *
 * @return  "TVerdict"
 *			The test result
 *
 * @xxxx
 *
 */
TVerdict CParseLine::DoTestNewThread(const TDesC& aSuite, const TDesC& aStep, 
							TInt64 aGuardTimerValue, const TDesC& aConfig, const TDesC& aParamSet)
	{
	//	get the number of suites loaded
	TInt noOfDlls = iArrayLoadedSuiteDll->Count();

	// search the list of loaded test suite DLLs for the required one
	for (TInt i = 0; i < noOfDlls; i++)
		{
		CSuiteDll* ptrSuite = iArrayLoadedSuiteDll->At(i);
		TPtrC name = ptrSuite->Name();

		if (name.FindF(aSuite)!= KErrNotFound)
			{
			// reset step status
			CTestSuite* testSuite = ptrSuite->Suite();
			testSuite->SetStepStatus(EStepStatusNone);

			// store old log status, for restore at thread exit
			// NB we must do this here, as if thread times out, the log
			// is in an undefined state
			CLog* oldLogger = testSuite->LogSystem();

			CStepData* data = NULL;
			TRAPD(err, data = CStepData::NewL(aStep, aConfig, aParamSet, ptrSuite));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CStepData::NewL() left with error %d : unable to create test data!"), err);
				return EFail;
				}
			
			// get step's own stack and heap sizes
			TInt theHeapSize = KMaxTestThreadHeapSize;
			TInt theStackSize = KTestStackSize;
			GetHeapAndStackSize(data, &theHeapSize, &theStackSize);

			TInt res = KErrAlreadyExists;
			RThread newThread;

			TPtrC threadBaseName(_L("DoTestThread"));
			TBuf<32> threadName;
			
			// create a unique named test thread
			// this will leave if creation is not successful
			TRAP (res, CreateUniqueTestThreadL( threadBaseName, 
												threadName, 
												newThread, 
												ThreadFunctionL,
												theStackSize,
												KMinHeapSize,
												theHeapSize,
												data ) );

			
			if (res != KErrNone)
				{
				ERR_PRINTF2(_L("CreateUniqueTestThreadL() left with error %d : unable to create test thread "), res);
				delete data;
				data = NULL;
				return EFail;
				}

			// start clock
			TTime testStart, testStop;
			testStart.HomeTime();
			
			// start the thread and request the status
			TRequestStatus threadStatus;
			newThread.Logon(threadStatus);

			// if there is no guard timer value, don't time at all
			if (aGuardTimerValue == KNoGuardTimer)
				{
				// no guard timer
				newThread.Resume();
				User::WaitForRequest(threadStatus);
				}
			else
				{
				// wait for either test thread or timer to end
				RTimer guardTimer;
				guardTimer.CreateLocal();			// create for this thread
				TRequestStatus timerStatus;
				newThread.Resume();

				// NB now using At() to allow 64-bit timer values
				TInt64 guardTimerUsec = aGuardTimerValue * 1000;
				TInt64 totalTime = 0;

				for (;;)
					{
					if (totalTime>=guardTimerUsec) // timeout has occured
						break;
					TInt timeout;

					if (totalTime+KTimeIncrement >= guardTimerUsec)
						{
						TInt64 temp = guardTimerUsec-totalTime;
						timeout = I64INT(temp);
						}
					else
						timeout = KTimeIncrement;
					totalTime += timeout;
					guardTimer.After(timerStatus, timeout);
					User::WaitForRequest(threadStatus, timerStatus);
					if (threadStatus!=KRequestPending) // normal exit
						break;
					}

				guardTimer.Cancel();
				guardTimer.Close();
				}

			// reset any file server error simulations
			RFs fs;
			TInt fsError = fs.Connect();
			if (fsError == KErrNone)
				{
				fs.SetErrorCondition(KErrNone);
				}
			fs.Close();

			// restore logger
			testSuite->SetLogSystem(oldLogger);

			// get the test result
			TVerdict result = STATIC_CAST(TVerdict, threadStatus.Int());

			// check terminated ok
			switch(newThread.ExitType())
				{
				case EExitTerminate:
				case EExitKill:
					break;
				case EExitPanic:
					{
					TExitCategoryName exitCategory = newThread.ExitCategory();
					TInt exitReason = newThread.ExitReason();
 					ERR_PRINTF3(_L("Thread had a panic %S:%d"), &exitCategory, exitReason);

					result = EFail;
					}
					break;
				case EExitPending:
					// if the thread is still pending then the guard timer must have expired
					ERR_PRINTF1(_L("Thread timed out"));
					// kill the test step thread
					newThread.Kill(1);
					// give the OS time to cleanup devices, etc.
					// NB if the thread dies, the postamble will NOT run
					User::After(2000000);
					result = EFail;
					break;
				default:
					break;
				}

			// done with the test thread
			newThread.Close();

			// stop clock
			testStop.HomeTime();

			TUint testDuration = I64INT(testStop.MicroSecondsFrom(testStart).Int64());
			testDuration /= 1000; // to microseconds
			TUint testDurationMsec = testDuration % 1000;
			TUint testDurationSec = testDuration / 1000;
			INFO_PRINTF3(_L("Test took %d.%03d sec"), testDurationSec, testDurationMsec);

			// return the test verdict
			delete data;
			data = NULL;
			return result;
			}
		}

	// the required suite has not been found
	ERR_PRINTF3(_L("Error in test step:%S - cannot find suite:%S" ),
					&aStep, &aSuite);

	return ETestSuiteError;
	}

/**
 *
 * Do a test step which is expected to panic.
 *
 * @param	"const TDesC& aSuite"
 *			The test suite
 *
 * @param	"const TDesC& aStep"
 *			The test step
 *
 * @param	"TInt aGuardTimerValue"
 *			The guard timer value
 *
 * @param	"const TExitCategoryName aExitCategory"
 *			The expected exit category
 *
 * @param	"TInt aExitReason"
 *			The expected exit reason
 *
 * @param	"const TDesC& aConfig"
 *			The config data
 *
 * @return "TVerdict"
 *			The test result
 *
 * @xxxx
 *
 */
TVerdict CParseLine::DoPanicTest(const TDesC& aSuite, const TDesC& aStep, TInt64 aGuardTimerValue,
									  const TExitCategoryName aExitCategory, TInt aExitReason, 
									  const TDesC& aConfig, const TDesC& aParamSet)
	{

	//	get the number of suites loaded
	TInt noOfDlls = iArrayLoadedSuiteDll->Count();

	// search the list of loaded test suite DLLs for the required one
	for (TInt i = 0; i < noOfDlls; i++)
		{
		CSuiteDll* ptrSuite = iArrayLoadedSuiteDll->At(i);
		TPtrC name = ptrSuite->Name();

		if (name.FindF(aSuite)!= KErrNotFound)
			{
			// reset step status
			CTestSuite* testSuite = ptrSuite->Suite();
			testSuite->SetStepStatus(EStepStatusNone);

			// store old log status, for restore at thread exit
			// NB we must do this here, as if thread times out, the log
			// is in an undefined state
			CLog* oldLogger = testSuite->LogSystem();

			CStepData* data = NULL;
			TRAPD(err, data = CStepData::NewL(aStep, aConfig, aParamSet, ptrSuite));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CStepData::NewL() left with error %d : unable to create test data!"), err);
				return EFail;
				}

			// get step's own stack and heap sizes
			TInt theHeapSize = KMaxTestThreadHeapSize;
			TInt theStackSize = KTestStackSize;
			GetHeapAndStackSize(data, &theHeapSize, &theStackSize);

			TInt res = KErrAlreadyExists;
			RThread newThread;

			// create a unique test name by appending a counter
			TPtrC threadBaseName(_L("DoTestThread"));
			TBuf<32> threadName;
			
			// create a unique named test thread
			// this will leave if creation is not successful
			TRAP (res, CreateUniqueTestThreadL( threadBaseName, 
												threadName, 
												newThread, 
												ThreadFunctionL,
												theStackSize,
												KMinHeapSize,
												theHeapSize,
												data ) );
			
			if (res != KErrNone)
				{
				ERR_PRINTF2(_L("CreateUniqueTestThreadL() left with error %d : unable to create test thread "), res);
				delete data;
				data = NULL;
				return EFail;
				}

			// start clock
			TTime testStart, testStop;
			testStart.HomeTime();
			
			// start the thread and request the status
			TRequestStatus threadStatus;
			newThread.Logon(threadStatus);

			// if there is no guard timer value, don't time at all
			if (aGuardTimerValue == KNoGuardTimer)
				{
				// no guard timer
				newThread.Resume();
				User::WaitForRequest(threadStatus);
				}
			else
				{
				// wait for either test thread or timer to end
				RTimer guardTimer;
				guardTimer.CreateLocal();			// create for this thread
				TRequestStatus timerStatus;
				newThread.Resume();

				// NB now using At() to allow 64-bit timer values
				TInt64 guardTimerUsec = aGuardTimerValue * 1000;
				TInt64 totalTime = 0;

				for (;;)
					{
					if (totalTime>=guardTimerUsec) // timeout has occured
						break;
					TInt timeout;

					if (totalTime+KTimeIncrement >= guardTimerUsec)
						{
						TInt64 temp = guardTimerUsec-totalTime;
						timeout = I64INT(temp);
						}
					else
						timeout = KTimeIncrement;
						totalTime += timeout;
					guardTimer.After(timerStatus, timeout);
					User::WaitForRequest(threadStatus, timerStatus);
					if (threadStatus!=KRequestPending) // normal exit
						break;
					}

				guardTimer.Cancel();
				guardTimer.Close();
				}

			// restore logger
			testSuite->SetLogSystem(oldLogger);

			// get the test result
			TVerdict result = STATIC_CAST(TVerdict, threadStatus.Int());

			// check terminated ok
			switch(newThread.ExitType())
				{
				case EExitPanic:
					{
					TExitCategoryName exitCategory = newThread.ExitCategory();
					TInt exitReason = newThread.ExitReason();
					if((exitCategory != aExitCategory) || (exitReason != aExitReason && aExitReason != KNoPanicReason) )
						{
						ERR_PRINTF3(_L("Test step had an unexpected panic %S:%d and failed"),
									&exitCategory, exitReason);
						result = EFail;
						}
					else
						{
						// check here that the panic occurred within the test itself
						CTestSuite* testSuite = ptrSuite->Suite();
						TTestStepStatus status = testSuite->StepStatus();
						switch(status)
							{
							case EStepStatusPreamble:
								{
								// thread panicked in the test itself - success
								INFO_PRINTF3(_L("Test step had a panic %S:%d and passed"),
											&exitCategory, exitReason);
								result = EPass;
								}
								break;
							case EStepStatusStart:
								{
								// thread panicked in preamble
								ERR_PRINTF3(_L("Test step had a panic %S:%d in preamble"),
											&exitCategory, exitReason);
								result = EFail;
								}
								break;
							case EStepStatusTest:
								{
								// thread panicked in postamble
								ERR_PRINTF3(_L("Test step had a panic %S:%d in postamble"),
											&exitCategory, exitReason);
								result = EFail;
								}
								break;
							default:
								{
								// thread panicked outside the test
								ERR_PRINTF3(_L("Test step had a panic %S:%d outside the test"),
											&exitCategory, exitReason);
								result = EFail;
								}
								break;
							}	// end switch
						}
					}
					break;
				case EExitPending:
					// if the thread is still pending then the guard timer must have expired
					ERR_PRINTF1(_L("Thread timed out"));
					// kill the test step thread
					newThread.Kill(1);
					// give the OS time to cleanup devices, etc.
					// NB if the thread dies, the postamble will NOT run
					User::After(2000000);
					result = EFail;
					break;
				case EExitTerminate:
				case EExitKill:
				default:
					ERR_PRINTF1(_L("Test did not panic, so failed"));
					result = EFail;
					break;
				}

			// done with the test thread
			newThread.Close();

			// stop clock
			testStop.HomeTime();

			TUint testDuration = I64INT(testStop.MicroSecondsFrom(testStart).Int64());
			testDuration /= 1000; // to microseconds
			TUint testDurationMsec = testDuration % 1000;
			TUint testDurationSec = testDuration / 1000;
			INFO_PRINTF3(_L("Test took %d.%03d sec"), testDurationSec, testDurationMsec);

			// return the test verdict
			delete data;
			data = NULL;
			return result;
			}
		}

	// the required suite has not been found
	ERR_PRINTF3(_L("Error in test step:%S - cannot find suite:%S"),
				&aStep, &aSuite );

	return ETestSuiteError;
	}
	
/**
 *
 * Do a test step which is expected to terminate.
 *
 * @param	"const TDesC& aSuite"
 *			The test suite
 *
 * @param	"const TDesC& aStep"
 *			The test step
 *
 * @param	"TInt aGuardTimerValue"
 *			The guard timer value
 *
 * @param	"TInt aExitReason"
 *			The expected exit reason
 *
 * @param	"const TDesC& aConfig"
 *			The config data
 *
 * @return "TVerdict"
 *			The test result
 *
 * @xxxx
 *
 */
TVerdict CParseLine::DoTerminationTest(const TDesC& aSuite, const TDesC& aStep, TInt64 aGuardTimerValue,
									  TInt aExitReason, const TDesC& aConfig)
	{

	//	get the number of suites loaded
	TInt noOfDlls = iArrayLoadedSuiteDll->Count();

	// search the list of loaded test suite DLLs for the required one
	for (TInt i = 0; i < noOfDlls; i++)
		{
		CSuiteDll* ptrSuite = iArrayLoadedSuiteDll->At(i);
		TPtrC name = ptrSuite->Name();

		if (name.FindF(aSuite)!= KErrNotFound)
			{
			// reset step status
			CTestSuite* testSuite = ptrSuite->Suite();
			testSuite->SetStepStatus(EStepStatusNone);

			// store old log status, for restore at thread exit
			// NB we must do this here, as if thread times out, the log
			// is in an undefined state
			CLog* oldLogger = testSuite->LogSystem();

			CStepData* data = NULL;
			TRAPD(err, data = CStepData::NewL(aStep, aConfig, ptrSuite));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CStepData::NewL() left with error %d : unable to create test data!"), err);
				return EFail;
				}

			// get step's own stack and heap sizes
			TInt theHeapSize = KMaxTestThreadHeapSize;
			TInt theStackSize = KTestStackSize;
			GetHeapAndStackSize(data, &theHeapSize, &theStackSize);

			TInt res = KErrAlreadyExists;
			RThread newThread;

			// create a unique test name by appending a counter
			TPtrC threadBaseName(_L("DoTestThread"));
			TBuf<32> threadName;
			
			// create a unique named test thread
			// this will leave if creation is not successful
			TRAP (res, CreateUniqueTestThreadL( threadBaseName, 
												threadName, 
												newThread, 
												ThreadFunctionL,
												theStackSize,
												KMinHeapSize,
												theHeapSize,
												data ) );
			
			if (res != KErrNone)
				{
				ERR_PRINTF2(_L("CreateUniqueTestThreadL() left with error %d : unable to create test thread "), res);
				delete data;
				data = NULL;
				return EFail;
				}

			// start clock
			TTime testStart, testStop;
			testStart.HomeTime();
			
			// start the thread and request the status
			TRequestStatus threadStatus;
			newThread.Logon(threadStatus);

			// if there is no guard timer value, don't time at all
			if (aGuardTimerValue == KNoGuardTimer)
				{
				// no guard timer
				newThread.Resume();
				User::WaitForRequest(threadStatus);
				}
			else
				{
				// wait for either test thread or timer to end
				RTimer guardTimer;
				guardTimer.CreateLocal();			// create for this thread
				TRequestStatus timerStatus;
				newThread.Resume();

				// NB now using At() to allow 64-bit timer values
				TInt64 guardTimerUsec = aGuardTimerValue * 1000;
				TInt64 totalTime = 0;

				for (;;)
					{
					if (totalTime>=guardTimerUsec) // timeout has occured
						break;
					TInt timeout;

					if (totalTime+KTimeIncrement >= guardTimerUsec)
						{
						TInt64 temp = guardTimerUsec-totalTime;
						timeout = I64INT(temp);
						}
					else
						timeout = KTimeIncrement;
						totalTime += timeout;
					guardTimer.After(timerStatus, timeout);
					User::WaitForRequest(threadStatus, timerStatus);
					if (threadStatus!=KRequestPending) // normal exit
						break;
					}

				guardTimer.Cancel();
				guardTimer.Close();
				}

			// restore logger
			testSuite->SetLogSystem(oldLogger);

			// get the test result
			TVerdict result = STATIC_CAST(TVerdict, threadStatus.Int());

			// check terminated ok
			switch(newThread.ExitType())
				{
				case EExitTerminate:
				case EExitKill:
					{
					TInt exitReason = newThread.ExitReason();
					if(exitReason != aExitReason)
						{
						ERR_PRINTF2(_L("Test step had an unexpected exit reason:%d and failed"),
									exitReason);
						result = EFail;
						}
					else
						{
						// check here that the panic occurred within the test itself
						CTestSuite* testSuite = ptrSuite->Suite();
						TTestStepStatus status = testSuite->StepStatus();
						switch(status)
							{
							case EStepStatusPreamble:
								{
								// thread terminated in the test itself - success
								INFO_PRINTF2(_L("Test step had terminated:%d and passed"),
											exitReason);
								result = EPass;
								}
								break;
							case EStepStatusStart:
								{
								// thread panicked in preamble
								ERR_PRINTF2(_L("Test step had terminated:%d in preamble"),
											exitReason);
								result = EFail;
								}
								break;
							case EStepStatusTest:
								{
								// thread panicked in postamble
								ERR_PRINTF2(_L("Test step had terminated:%d in postamble"),
											exitReason);
								result = EFail;
								}
								break;
							default:
								{
								// thread panicked outside the test
								ERR_PRINTF2(_L("Test step had terminated:%d outside the test"),
											exitReason);
								result = EFail;
								}
								break;
							}	// end switch
						}
					}
					break;
				case EExitPending:
					// if the thread is still pending then the guard timer must have expired
					ERR_PRINTF1(_L("Thread timed out"));
					// kill the test step thread
					newThread.Kill(1);
					// give the OS time to cleanup devices, etc.
					// NB if the thread dies, the postamble will NOT run
					User::After(2000000);
					result = EFail;
					break;
				case EExitPanic:
				default:
					ERR_PRINTF1(_L("Test did not terminate, so failed"));
					result = EFail;
					break;
				}

			// done with the test thread
			newThread.Close();

			// stop clock
			testStop.HomeTime();

			TUint testDuration = I64INT(testStop.MicroSecondsFrom(testStart).Int64());
			testDuration /= 1000; // to microseconds
			TUint testDurationMsec = testDuration % 1000;
			TUint testDurationSec = testDuration / 1000;
			INFO_PRINTF3(_L("Test took %d.%03d sec"), testDurationSec, testDurationMsec);

			// return the test verdict
			delete data;
			data = NULL;
			return result;
			}
		}

	// the required suite has not been found
	ERR_PRINTF3(_L("Error in test step:%S - cannot find suite:%S"),
				&aStep, &aSuite );

	return ETestSuiteError;
	}

/**
 *
 * Gets a step's heap and stack size.
 *
 * @param	"const CStepData& aStepData"
 *			The step data
 * @param	"TInt* aHeapSize"
 *			Returns the step's heap size
 * @param	"TInt* aStackSize"
 *			Returns the step's stack size
 *
 * @xxxx
 *
 */
void CParseLine::GetHeapAndStackSize(const CStepData* aStepData, TInt* aHeapSize, TInt* aStackSize)
	{
	CSuiteDll* suiteDll = aStepData->SuiteDll();
	CTestSuite* testSuite = suiteDll->Suite();
	testSuite->GetHeapAndStackSize(aStepData->Step(), aHeapSize, aStackSize);
	}

/**
 *
 * Implements the RUN_PROGRAM script command.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::RunProgram(const TDesC& aText)
	{
	TPtrC param;

	// use Tlex to decode the cmd line
	TLex lex(aText);

	// step over the keyword
	lex.NextToken();

	// get program name	
	TPtrC token;
	token.Set(lex.NextToken());

	// get the parameters
	param.Set(lex.NextToken());

	INFO_PRINTF1(_L("Run Program "));


	// In the ARM build run program as a new process
	// use the rest of the text as parameters
	RProcess program;
	TInt ret = program.Create(token, lex.Remainder());

	if (ret != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(ret);
		ERR_PRINTF2(_L("Failed to start process - error %S"), &errortxt);
		return;
		}
	else
		{
		INFO_PRINTF1(_L("Program started"));

		// start program
		TRequestStatus threadStatus;
		program.Logon(threadStatus);
		program.Resume();

		// wait for guard timer
		User::WaitForRequest(threadStatus);

		// check return type
		if (program.ExitType() == EExitPanic)
			INFO_PRINTF1(_L("Program returned EExitPanic"));
		else if (program.ExitType() == EExitPending)
			INFO_PRINTF1(_L("Program returned EExitPending"));
		else
			INFO_PRINTF1(_L("Program returned EExitTerminate"));
		}
	}

/**
 *
 * Implements the LOG_SETTINGS script command.
 * Command format is LOG_SETTINGS "put src." (1/0),
 * "HTML format" (1/0)
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::LogSettings(const TDesC& aText)
	{
	// use Tlex to decode the cmd line
	TLex lex(aText);

	// start at the begining
	TPtrC token=lex.NextToken();

	// step over the keyword
	//Get information about source
	token.Set(lex.NextToken());

	TLex srcLex(token);
	TInt isSrc = ETrue; //Shall we put src information?
	if (srcLex.Val(isSrc) != KErrNone)
		{
		ERR_PRINTF2(_L("Error in LOG_SETTINGS: could not decode >%S< as value(0/1)"),
					&token);
		}
	else
		{
		iLog->SetPutSrcInfo(isSrc) ;
		}
	//Get information about format
	token.Set(lex.NextToken());
	TLex htmlLex(token);

	if (htmlLex.Val(isSrc) != KErrNone)
		{
		ERR_PRINTF2(_L("Error in LOG_SETTINGS: could not decode >%S< as value(0/1)"),
					&token);
		}
	else
		{
		iLog->SetHtmlLogMode(isSrc);
		}
	}


/**
 *
 * Implements the LOAD_SUITE script command.
 * This function loads a required test suite DLL
 * It also creates a CTestSuite object as a record
 * of the loaded DLL.
 *
 * @param	"const TDesC& aText"
 *			The script line
 *
 * @xxxx
 *
 */
void CParseLine::LoadSuiteL(const TDesC& aText)
	{
	// use Tlex to decode the cmd line
	TLex lex(aText);

	// step over the keyword
	lex.NextToken();

	// get suite name
	TPtrC token;
	token.Set(lex.NextToken());

	// check not already loaded
	// by searching the list of loaded test suite DLLs for the required one
	// start with the number of suites loaded
	TInt noOfDlls = iArrayLoadedSuiteDll->Count();
	for (TInt i = 0; i < noOfDlls; i++)
		{
		CSuiteDll* ptrSuite = iArrayLoadedSuiteDll->At(i);
		TPtrC name = ptrSuite->Name();

		// check the names
		if (name.FindF(token) != KErrNotFound)
			{
			// this suite DLL is already loaded
	 		WARN_PRINTF2(_L("Warning: Test suite %S already loaded - not re-loaded"), &token);
			return;
			}
		}


	// create a new CSuiteDll object to store info on loaded DLL
	CSuiteDll* newRef = NULL;

	newRef = CSuiteDll::NewL(token, iLog);

	CTestSuite* testSuite = newRef->Suite();

	// set default severity and logging system
	testSuite->SetSeverity(iSeverity);
	testSuite->SetLogSystem(iLog);
	
	// add to data
	iArrayLoadedSuiteDll->AppendL(newRef);
	}


/**
 *
 * Unload all the loaded DLLs
 *
 * @xxxx
 *
 */
void CParseLine::Unload()
	{
	if (iArrayLoadedSuiteDll)
		{
		// unload all the loaded DLLS and their records
		iArrayLoadedSuiteDll->ResetAndDestroy();
		}
	}

/**
 *
 * Mark the heap
 *
 * @xxxx
 *
 */
void CParseLine::HeapMark()
	{
	ERR_PRINTF1(_L("Warning: Command HEAP_MARK no longer supported. Heap marking/checking should be done within test code"));

	// __UHEAP_MARK;
	}


/**
 *
 * Check the heap
 *
 * @xxxx
 *
 */
void CParseLine::HeapCheck()
	{
	ERR_PRINTF1(_L("Warning: Command HEAP_CHECK no longer supported. Heap marking/checking should be done within test code"));

	// __UHEAP_MARKEND;
	}

/**
 *
 * Mark request
 *
 * @xxxx
 *
 */
void CParseLine::RequestMark()
	{
	// get number of outstanding requetsts on thread before we run the test
	iReqsAtStart = RThread().RequestCount();
	INFO_PRINTF2(_L("Requests at the start %d "),iReqsAtStart);
	}


/**
 *
 * Check request
 *
 * @xxxx
 *
 */
void CParseLine::RequestCheck()
	{
	// check the number of outstanding requests against recorded value
	INFO_PRINTF3(_L("Requests at the start %d now %d"),
				iReqsAtStart, RThread().RequestCount());

	if (iReqsAtStart != RThread().RequestCount())
		{
		ERR_PRINTF1(_L("Test failed on requests count"));

		// this result is only significant if every thing else has passed
		if (iTestVerdict == EPass)
			iTestVerdict = EFail;

		}
	}


/**
 *
 * Mark number of handles
 *
 * @xxxx
 *
 */
void CParseLine::HandlesMark()
	{
	// get number of Handles *before* we start the program
	RThread().HandleCount(iProcessHandleCountBefore, iThreadHandleCountBefore);

	INFO_PRINTF3(_L("HandlesMark : process handle count %d thread handle count %d"),
				iProcessHandleCountBefore,
				iThreadHandleCountBefore);
	}

/**
 *
 * Check number of handles
 *
 * @xxxx
 *
 */
void CParseLine::HandlesCheck()
	{
	TInt processHandleCountAfter;
	TInt threadHandleCountAfter;
	RThread().HandleCount(processHandleCountAfter, threadHandleCountAfter);

	INFO_PRINTF3(_L("HandlesCheck : process handle count %d thread handle count %d"),
				processHandleCountAfter,
				threadHandleCountAfter);

	// check that we are closing all the threads
	if(iThreadHandleCountBefore != threadHandleCountAfter)
		{
		ERR_PRINTF1(_L("Test failed on thread handle count"));

		// this result is only significant if everything else has passed
		if (iTestVerdict == EPass)
			iTestVerdict = EFail;
		}

	// check that we are closing all the handles
	if(iProcessHandleCountBefore != processHandleCountAfter)
		{
		ERR_PRINTF1(_L("Test failed on process handle count"));

		// this result is only significant if everything else has passed
		if (iTestVerdict == EPass)
			iTestVerdict = EFail;
		}
	}

/**
 *
 * Traceable logging function for parseline.
 * To be called only with macros
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
void CParseLine::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	if(LogSeverity::IsActive(aSeverity, iSeverity))
		{
		if(iLog)
			{
			iLog->LogExtra(aFile, aLine, aSeverity, aFmt, aList);
			}
		}

	VA_END(aList);
	}

/**
 *
 * Get current suite name.
 *
 * @return "TPtrC"
 *			The suite name
 *
 * @xxxx
 *
 */
TPtrC CParseLine::CurrentSuiteName() const
	{
	return iCurrentSuiteName;
	}

/**
 *
 * Get current step name.
 *
 * @return "TPtrC"
 *			The step name
 *
 * @xxxx
 *
 */
TPtrC CParseLine::CurrentStepName() const
	{
	return iCurrentStepName;
	}

/**
 *
 * Create a thread with a unique name from a base thread name
 * e.g. "TestThread" may become "TestThread00000002"
 * This test will leave instantly if an error other than 
 * KErrAlreadyExists occurs. 
 *
 *
 * @param	"TDesC& aBaseName"
 *			The base name to use.  This will be modified to contain
 *			the new unique thread name if creation is successful.
 *
 * @param	"TDes& aThreadName"
 *			The thread name to use.  This will be modified to contain
 *			the new unique thread name if creation is successful.  This must
 *			be non NULL.
 *
 * @param	"RThread&"
 *			An RThread which will be created.  This must not be a valid handle.
 *
 * @param	"TThreadFunction aFunction"
 *			Thread function to use in RThread.
 *
 * @param	"TInt aStackSize"
 *			The size of the new thread's stack.
 *
 * @param	"TInt aHeapMinSize"
 *			The minimum size for the new thread's heap.
 *
 * @param	"TInt aHeapMaxSize"
 *			The maximum size for the new thread's heap.
 *
 * @param	"TAny *aPtr"
 *			Data to pass to the new thread.
 *
 * @leave	"function will leave with an error code if a thread cannot
 *			be created after KMaxThreadAttempts tries."
 *
 * @xxxx
 *
 */
void CParseLine::CreateUniqueTestThreadL(const TDesC& aBaseName, TDes& aThreadName, RThread& aTestThread, TThreadFunction aFunction, TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize, TAny *aPtr)
	{	
	TInt res = KErrAlreadyExists;
	
	// attempt to create a thread with the name aBaseName + counter.
	for (TUint i = 0; i < KMaxThreadAttempts; i++)
		{
		// copy the base thread name
		aThreadName.Copy(aBaseName);
		
		// append the current counter to the threadname
		aThreadName.AppendNumFixedWidth(i, EDecimal, 8);
			
		// run in a new thread, with a new heap		
		res = aTestThread.Create(aThreadName,
				aFunction,
				aStackSize,
				aHeapMinSize,
				aHeapMaxSize,
				aPtr);
					
		// if thread created successfully then we have
		// a unique threadname else if an error code other
		// than KErrAlreadyExists occurs then exit immediately.
		if ((res == KErrNone) || (res != KErrAlreadyExists)) 
			break;
		}
		
	User::LeaveIfError(res);
	}

/**
 *
 * Static constructor for CSuiteDll.
 *
 *
 * @return	"CSuiteDll*"
 *			The constructed CSuiteDll
 *
 * @xxxx
 *
 */
CSuiteDll* CSuiteDll::NewL(const TDesC& aName, CLog* aLog)
	{
	CSuiteDll* self = new(ELeave) CSuiteDll;
	CleanupStack::PushL(self);
	self->ConstructL(aName, aLog);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 * CSuiteDLL second-phase constructor
 * Loads a test suite dll and saves the name and test
 * suite pointers.
 *
 * @param "TDesC& aName"
 *			The test suite name
 *
 * @param "CLog* aLog"
 *			The logger to use
 *
 * @xxxx
 *
 */
void CSuiteDll::ConstructL(const TDesC& aName, CLog* aLog)
	{
	iLog = aLog;

	User::Check();
	// load DLL by name
	TInt ret = iLibrary.Load(aName, KTxtDLLpath);

	User::Check();

	if (ret == KErrNotFound)
		{
		iLog->LogExtra(__FILE8__, __LINE__, ESevrErr, _L("Test suite %S was not found. Check any other DLLs required by %S"), &aName, &aName);
		User::Leave(ret);
		}
	else if (ret != KErrNone)
		{
		iLog->LogExtra(__FILE8__, __LINE__, ESevrErr, _L("Test suite %S found but would not load. Check any other DLLs required by %S"), &aName, &aName);
		User::Leave(ret);
		}

	// save the name
	iName.Copy(aName);

	// get the interface pointer at ordinal 1
	const TInt KLibraryOrdinal = 1;
	TLibraryFunction  entryL = iLibrary.Lookup(KLibraryOrdinal);

    // Call this interface pointer to create new CTestSuite
	// If this call goes to the wrong function then the test
	// suite does not have the correct function at ordinal 1.
	// This is usually caused by an error in the def file.
    iTestSuite = REINTERPRET_CAST(CTestSuite*, entryL());

    // NB :- Second-phase constructor for CTestSuite has already been called in entryL() above.
	// There is no need to call it again!

	// set suite severity level
	iTestSuite->SetSeverity(iLog->Severity());

	// get the version information
	TPtrC versiontxt = iTestSuite->GetVersion();

	// add to log
	iLog->LogExtra(__FILE8__, __LINE__, ESevrInfo, _L("LOAD_SUITE %S version %S loaded ok"),&aName, &versiontxt );
	}

/**
 *
 * CSuiteDLL destructor
 *
 * @xxxx
 *
 */
CSuiteDll::~CSuiteDll()
	{
	// delete the TestSuiteObject in the loaded DLL
	delete iTestSuite;

	// close and unload the library
	iLibrary.Close();
	}

/**
 *
 * CSuiteDLL accessor : suite
 *
 * @return "CTestSuite*"
 *			The test suite.
 *
 * @xxxx
 *
 */
CTestSuite* CSuiteDll::Suite() const
	{
	return iTestSuite;
	}

/**
 *
 * CSuiteDLL accessor : suite name
 *
 * @return "TPtrC"
 *			The suite name.
 *
 * @xxxx
 *
 */
TPtrC CSuiteDll::Name() const
	{
	return iName;
	}

/**
 *
 * CStepData
 *
 * @xxxx
 *
 */
CStepData* CStepData::NewL(const TDesC& aStep, const TDesC& aConfig, CSuiteDll* aSuite)
	{
	return NewL(aStep, aConfig, KNullDesC, aSuite);
	}

CStepData* CStepData::NewL(const TDesC& aStep, const TDesC& aConfig, const TDesC& aParamSet, CSuiteDll* aSuite)
	{
	CStepData* self = new(ELeave) CStepData;
	CleanupStack::PushL(self);
	self->ConstructL(aStep, aConfig, aParamSet, aSuite);
	CleanupStack::Pop();
	return self;
	}

CStepData::CStepData()
	{
	}

CStepData::~CStepData()
	{
	}

void CStepData::ConstructL(const TDesC& aStep, const TDesC& aConfig, const TDesC& aParamSet, CSuiteDll* aSuite)
	{
	iStep = aStep;
	iConfig = aConfig;
	iParamSet = aParamSet;
	iSuite = aSuite;
	}

const TDesC& CStepData::ParamSet() const
	{
	return iParamSet;
	}


