// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef E32AUTOTEST_H__
#define E32AUTOTEST_H__

/** @file autotest.h
 *
 * Framework for Automated Test code.
 */

#include <e32test.h>
#include <f32file.h>

#define PRINTF gAutoTest->Printf

/**
 * Latest version can always be found here:
 * //EPOC/development/personal/alfredh/autotest/...
 *
 * Revision history
 *
 * 0.04 Merged with RefTsy and SmsStack
 * 0.03 imported to C32
 * 0.02 imported to SmsStack
 * 0.01 first version used in RefTSY
 */

//
// prototypes
//


/**
 * contains information about one test case, including
 * name of test case and pointer to the test function
 *
 * @internalComponent
 * @deprecated
 */
struct TAutoTestCase
	{
	void (*iFunc)(void);   //< function pointer to the actual test case
	const TText* iText;    //< textual info about the test case
	};

static const TInt KMaxCmdLength = 256;

/**
 * This class implements the automated test framework.
 * It is very simple but lets you run all the test cases
 * separately or in one go.
 *
 * @internalComponent
 * @deprecated
 */
class CAutoTest : public CBase
	{
  public:
	IMPORT_C static CAutoTest* NewL(const struct TAutoTestCase* aTestcases, TInt aNumberOfTestcases, const TDesC& aName, RTest& aTest);
	~CAutoTest();
	IMPORT_C TInt ExecuteL();
	IMPORT_C void TestCheckPointL(TBool aCase, char* aFile, TInt aLine);
	IMPORT_C void TestCheckPointCodeL(TInt aCase, char* aFile, TInt aLine);
	IMPORT_C void TestCheckPointCompareL(TInt aVal,TInt aExpectedVal, const TDesC& aText, char* aFile,TInt aLine);
	IMPORT_C void SetCompInfo(const TDesC8& aCompInfo);
	IMPORT_C void GetString(CConsoleBase& aConsole, TDes& aString) const;
	IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void Printf(TRefByValue<const TDesC> aFmt,...);
	IMPORT_C TBool KeepGoing(void);

  protected:
	CAutoTest(const struct TAutoTestCase* aTestcases, TInt aNumberOfTestcases, const TDesC& aName, RTest& aTest);
	void ConstructL();
	
  protected:
	void PrintMenu() const;
	void StartMenuL();
	void RunAllTestsL();
	TInt RunATestL();
	void PrintTestReportL(TInt aTestIndex);
	void LogTimeL();
	void MachineInfoL();
	void PrintThreadsToLogFile();
	void SetLogFileL();
	const TBool TestSkipped(TUint8 aTestNum);
	void GetCommandLineL();
	void GetSkipOptions();
	void GetKeepGoingOption();

  private:

  private:
	const struct TAutoTestCase* iTestCases; //< pointer to table with all the test cases
	const TInt iNumberOfTestcases;			//< number of test cases in the table
	TBuf<KMaxCmdLength> iTestReportName;	//< holds the name of the test report file
	TBool iKeepGoing;						//< if true, does not stop if a test fails, but continues
	TBool iTestSuiteFailed;					//< indicates if the whole test suite failed or not
	TInt* iTestResults;						//< contains all the test results after running test suite
	RFs iFileServer;						//< handle to the File Server
	RFile iTestReport;						//< handle to the file containing the test report
	TInt iTestCase;							//< the test case currently being run
	TBuf8<256> iCompInfo;					//< contains information about the Component under test
	RTest& iRTest;							//< reference to the test utility class
	TBool* iSkipList;						//< List of tests to be skipped in decimal
	HBufC* iCommandLine;					//< Internal copy of command line or config file
	};

//
// macros
//

/**
 * call this macro to start the automated test framework.
 *
 * @param tc Table containing the test cases
 * @param name Name of the test report file
 */
// Global pointer instance version
#define GLOBAL_AUTOTEST_EXECUTE(tc, name, compinfo, test) \
gAutoTest = CAutoTest::NewL((tc), sizeof(tc)/sizeof((tc)[0]), name, test); \
CleanupStack::PushL(gAutoTest);\
gAutoTest->SetCompInfo(compinfo); \
gAutoTest->ExecuteL(); \
CleanupStack::PopAndDestroy(); 

// Stack version
#define AUTOTEST_EXECUTE(tc, name, compinfo, test) \
CAutoTest* autoTest = CAutoTest::NewL((tc), sizeof(tc)/sizeof((tc)[0]), name, test); \
CleanupStack::PushL(autoTest);\
autoTest->SetCompInfo(compinfo); \
autoTest->ExecuteL(); \
CleanupStack::PopAndDestroy(); 

// Global checkpoint versions
// Writes failure with the error code to file. 'a' is the error code
#define GLOBAL_CHECKPOINT_CODE(a) gAutoTest->TestCheckPointCodeL((a), (__FILE__), (__LINE__))
// Writes failure only to file. 'a' Parameter is boolean
#define GLOBAL_CHECKPOINT(a) gAutoTest->TestCheckPointL((a), (__FILE__), (__LINE__))
// Compare expected value a with b. If they're different, string C is logged along with the values
#define GLOBAL_CHECKPOINT_COMPARE(a,b,c) gAutoTest->TestCheckPointCompareL((a),(b),(c),(__FILE__),(__LINE__))
#endif // E32AUTOTEST_H__
