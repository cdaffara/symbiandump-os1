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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <TestFramework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "OneExampleUnitMMFAudClientTest.h"
#include "Flogger.h"

// --------------------------------------------

/**
 *
 * Static constructor for COneExampleUnitMMFAudClientTest.
 *
 *
 * @return	"COneExampleUnitMMFAudClientTest*"
 *			The constructed COneExampleICLTest
 *
 * @xxxx
 * 
 */
COneExampleUnitMMFAudClientTest* COneExampleUnitMMFAudClientTest::NewL()
	{
	COneExampleUnitMMFAudClientTest* self = new(ELeave) COneExampleUnitMMFAudClientTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
COneExampleUnitMMFAudClientTest::COneExampleUnitMMFAudClientTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("OneExampleUnitMMFAudClientTest");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
COneExampleUnitMMFAudClientTest::~COneExampleUnitMMFAudClientTest()
	{
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict COneExampleUnitMMFAudClientTest::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("Example Test 1"));
	INFO_PRINTF1(_L("this test is reading from a config file and should pass"));

	
	// read in a bool value from the config file
	TBool aBoolResult;
	TBool returnValue = GetBoolFromConfig(_L("SectionOne"),_L("Keybool"), aBoolResult);

	// check result
	TESTL(returnValue);
	if ( aBoolResult ) 
		INFO_PRINTF1(_L("Keybool = ETrue"));
	else
		INFO_PRINTF1(_L("Keybool = EFalse"));
	
	// check result
	returnValue = GetBoolFromConfig(_L("SectionOne"),_L("Keybool2"), aBoolResult);
	TESTL(returnValue);

	if ( aBoolResult ) 
		INFO_PRINTF1(_L("Keybool2 = ETrue"));
	else
		INFO_PRINTF1(_L("Keybool2 = EFalse"));

	// read in a TInt value from the config file
	TInt aIntResult;
	returnValue = GetIntFromConfig(_L("SectionOne"),_L("KeyTInt"),aIntResult);
	TESTL(returnValue);
	if (returnValue) INFO_PRINTF2(_L("KeyTInt = %d"), aIntResult );
	
	// read in a bool value from the config file
	TPtrC aPtrResult;
	TPtrC* res = &aPtrResult;
	returnValue = GetStringFromConfig(_L("SectionOne"), _L("KeyStr"), aPtrResult);
	TESTL(returnValue);
	if (returnValue) INFO_PRINTF2(_L("KeyStr  = %S"), res);

	// boolean test expression macros

	// TEST(a) check a boolean expression (a) is true, if not display error, & record fail
	TEST(1);

	// TESTL(a) check a boolean expression (a) is true, if not display error, record fail and LEAVE
	TESTL(1);

	// TESTE(a, b) check a boolean expression (a) is true, if not display error code b and record fail
	TESTE(1, 2);

	// TESTEL(a, b) check a boolean expression (a) is true, if not display error code b, record fail and LEAVE
	TESTEL(1, 2);
	
	// test steps return a result
	return iTestStepResult;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CExampleTest2.
 *
 *
 * @return	"CExampleTest2*"
 *			The constructed CExampleTest2
 *
 * @xxxx
 * 
 */
CExampleTest2* CExampleTest2::NewL()
	{
	CExampleTest2* self = new(ELeave) CExampleTest2;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CExampleTest2::CExampleTest2()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("ExampleTest2");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CExampleTest2::~CExampleTest2()
	{
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CExampleTest2::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("log text from example Test 2"));
	INFO_PRINTF1(_L("This test should fail but not leave"));

	// display error, & record fail
	TEST(0);

	// test steps return a result
	return iTestStepResult;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CExampleTest3.
 *
 *
 * @return	"CExampleTest3*"
 *			The constructed CExampleTest3
 *
 * @xxxx
 * 
 */
CExampleTest3* CExampleTest3::NewL()
	{
	CExampleTest3* self = new(ELeave) CExampleTest3;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CExampleTest3::CExampleTest3()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("ExampleTest3");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CExampleTest3::~CExampleTest3()
	{
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CExampleTest3::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("Self Test 3"));
	INFO_PRINTF1(_L("this test leaves with error code 3 and should fail"));

	// this test should leave with error code 3
	TESTEL(0, 3);

	// test steps return a result
	return iTestStepResult;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CExampleTest4.
 *
 *
 * @return	"CExampleTest4*"
 *			The constructed CExampleTest4
 *
 * @xxxx
 * 
 */
CExampleTest4* CExampleTest4::NewL()
	{
	CExampleTest4* self = new(ELeave) CExampleTest4;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CExampleTest4::CExampleTest4()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("ExampleTest4");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CExampleTest4::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("Self Test 4"));
	INFO_PRINTF1(_L("this test should pass"));

	// this test should pass
	TESTEL(1, 3);

	// test steps return a result
	return iTestStepResult;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CExampleTest5.
 *
 *
 * @return	"CExampleTest5*"
 *			The constructed CExampleTest5
 *
 * @xxxx
 * 
 */
CExampleTest5* CExampleTest5::NewL()
	{
	CExampleTest5* self = new(ELeave) CExampleTest5;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CExampleTest5::CExampleTest5()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("ExampleTest5");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CExampleTest5::DoTestStepL()
	{
	INFO_PRINTF1(_L("Self Test 5"));
	INFO_PRINTF1(_L("this test should fail (not leave)"));

	// test steps return a result
	return iTestStepResult = EFail;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CPanicTest.
 *
 *
 * @return	"CPanicTest*"
 *			The constructed CPanicTest
 *
 * @xxxx
 * 
 */
CPanicTest* CPanicTest::NewL()
	{
	CPanicTest* self = new(ELeave) CPanicTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CPanicTest::CPanicTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("PanicTest");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CPanicTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Panic Test "));
	INFO_PRINTF1(_L("this test should panic (not leave)"));

	// force a panic
	__ASSERT_ALWAYS(KErrNone, User::Panic(_L("TSU_ICL_FRM_01"),1));

	// this test should fail regardless of verdict here !
	return iTestStepResult = EPass;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CPrePanicTest.
 *
 *
 * @return	"CPrePanicTest*"
 *			The constructed CPrePanicTest
 *
 * @xxxx
 * 
 */
CPrePanicTest* CPrePanicTest::NewL()
	{
	CPrePanicTest* self = new(ELeave) CPrePanicTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CPrePanicTest::CPrePanicTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("PrePanicTest");
	}


/**
 *
 * Do the test step preamble.
 *
 * @return	"TVerdict"
 *			The result of the test step preamble
 *
 * @xxxx
 * 
 */
TVerdict CPrePanicTest::DoTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Preamble Panic Test "));
	INFO_PRINTF1(_L("this test should panic in its preamble"));

	// force a panic
	__ASSERT_ALWAYS(KErrNone, User::Panic(_L("TSU_MMF_ACLNT_01"),1));

	// this test should fail regardless of verdict here !
	return iTestStepResult = EPass;
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CPrePanicTest::DoTestStepL()
	{
	return iTestStepResult = EPass;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CPostPanicTest.
 *
 *
 * @return	"CPostPanicTest*"
 *			The constructed CPostPanicTest
 *
 * @xxxx
 * 
 */
CPostPanicTest* CPostPanicTest::NewL()
	{
	CPostPanicTest* self = new(ELeave) CPostPanicTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CPostPanicTest::CPostPanicTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("PostPanicTest");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CPostPanicTest::DoTestStepL()
	{
	return iTestStepResult = EPass;
	}

/**
 *
 * Do the test step postamble.
 *
 * @return	"TVerdict"
 *			The result of the test step postamble
 *
 * @xxxx
 * 
 */
TVerdict CPostPanicTest::DoTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Postamble Panic Test "));
	INFO_PRINTF1(_L("this test should panic in its postamble"));

	// force a panic
	__ASSERT_ALWAYS(KErrNone, User::Panic(_L("TSU_ICL_FRM_01"),1));

	// this test should fail regardless of verdict here !
	return iTestStepResult = EPass;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CTimerTest.
 *
 *
 * @return	"CTimerTest*"
 *			The constructed CTimerTest
 *
 * @xxxx
 * 
 */
CTimerTest* CTimerTest::NewL()
	{
	CTimerTest* self = new(ELeave) CTimerTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTimerTest::CTimerTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("TimerTest");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTimerTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("this test runs forever (so should be caught by the guard timer)"));

	// zero warnings, so comment out
	// do forever
	//TBool forever = ETrue;
	//while (forever) { ; }

	// this test should fail regardless of verdict here !
	// NB this will give an unreachable code warning in compiler - ignore.
	return iTestStepResult = EPass;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CFlogTest.
 *
 *
 * @return	"CFlogTest*"
 *			The constructed CFlogTest
 *
 * @xxxx
 * 
 */
CFlogTest* CFlogTest::NewL()
	{
	CFlogTest* self = new(ELeave) CFlogTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CFlogTest::CFlogTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("FlogTest");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CFlogTest::DoTestStepL()
	{
	// this test is using Flogger (not Test Framework Log)
	_LIT(KTestFloggerLogFile,"FLOGGERTEST.TXT");
	_LIT(KTestFloggerLogFolder,"FLOGGERTEST");

	INFO_PRINTF1(_L("FLog Test "));
	RFileLogger::Write(KTestFloggerLogFolder(), KTestFloggerLogFile(), EFileLoggingModeOverwrite, 
					   _L("TEST FLOGGER"));
	// this test should fail regardless of verdict here !
	return iTestStepResult = EPass;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CErrorCodeTest.
 *
 *
 * @return	"CErrorCodeTest*"
 *			The constructed CErrorCodeTest
 *
 * @xxxx
 * 
 */
CErrorCodeTest* CErrorCodeTest::NewL()
	{
	CErrorCodeTest* self = new(ELeave) CErrorCodeTest;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CErrorCodeTest::CErrorCodeTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("ErrorCodeTest");
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CErrorCodeTest::DoTestStepL()
	{

	INFO_PRINTF1(_L("Error code Test "));

	TESTE(0,103);
	TESTE(0,54);
	TESTE(0,-2);

	return iTestStepResult = EPass;
	}

// --------------------------------------------
