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
//

#ifndef __PARSELINE_H__
#define __PARSELINE_H__

/**
 *
 * Max len of string buffers
 *
 * @xxxx
 *
 */
const TInt KMaxLenStepData = 256;
const TInt KMaxLenScriptLine = 2000;

/**
 *
 * Guard timer default
 *
 * @xxxx
 *
 */
const TInt KNoGuardTimer = -1L;

/**
 * constant that indicates that the panic reason should be ignored
 */
const TInt KNoPanicReason = -1;

/**
 *
 * This class is used for storing information on 
 * test suite DLLs currently loaded
 *
 * @xxxx
 *
 */
class CSuiteDll : public CBase
	{
public:

	static CSuiteDll* NewL(const TDesC& aName, CLog* aLog);
	void ConstructL(const TDesC& aName, CLog* aLog);
	~CSuiteDll();

	// accessors
	CTestSuite* Suite() const;
	TPtrC Name() const;

private:
	CLog*						iLog;
	RLibrary					iLibrary;		// DLL Library 
	CTestSuite*					iTestSuite;		// pointer to Test suite object
	TBuf<KMaxLenTestSuiteName>	iName;		// name of the test suite
	};

/**
 *
 * CStepData class used for passing test step data to the step thread
 * JW 03-01-03 : rewritten from TStepData to use heap - avoiding stack overflow
 *
 * @xxxx
 *
 */
class CStepData
	{
public:
	// constructor
	static CStepData* NewL(const TDesC& aStep, const TDesC& aConfig, CSuiteDll* aSuite);
	static CStepData* NewL(const TDesC& aStep, const TDesC& aConfig, const TDesC& aParamSet, CSuiteDll* aSuite);
	~CStepData();
	CStepData();
	void ConstructL(const TDesC& aStep, const TDesC& aConfig, const TDesC& aParamSet, CSuiteDll* aSuite);

	// accessors
	TPtrC Step() const;
	TPtrC Config() const;
	const TDesC& ParamSet() const;
	CSuiteDll* SuiteDll() const;
private:
	TBuf<KMaxLenStepData> iStep; 
	TBuf<KMaxLenStepData> iConfig;
	TBuf<KMaxLenStepData> iParamSet;
	CSuiteDll*			  iSuite;
	};

/**
 *
 * CParseLine - the main test script interpreter
 *
 * @xxxx
 *
 */
class CParseLine : public CBase
	{
public:
	static CParseLine* NewL(CScript* aScript, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);
	~CParseLine();	
	CParseLine(const TDesC& aMatchString);
	void ConstructL(CScript* aScript, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer);

	void ProcessLineL(const TDesC8& aNarrowline, TInt aLineNo);	// process a line of script

	// accessors
	TPtrC CurrentSuiteName() const;
	TPtrC CurrentStepName() const;

private:

	// thread functions
	static TInt ThreadFunctionL (TAny* aPtr);
	static TInt ThreadTrapFunctionL (TAny* aPtr);

	// script keyword commands
	void ScriptPrint(const TDesC& aText);
	void RunScriptL(const TDesC& aText);
	void RunTestStep(const TDesC& aText);
	void RunPanicTestStep(const TDesC& aText);
	void RunTerminationTestStep(const TDesC& aText);
	void RunUtil(const TDesC& aText);
	void LoadSuiteL(const TDesC& aText);
	void Reboot();
	TVerdict DoTestNewThread(const TDesC& aSuite, const TDesC& aStep, 
				TInt64 aGuardTimerValue, const TDesC& aConfig, const TDesC& aParamSet);
	TVerdict DoPanicTest(const TDesC& aSuite, const TDesC& aStep, TInt64 aGuardTimerValue, const TExitCategoryName aExitCategory, TInt aExitReason, const TDesC& aConfig, const TDesC& aParamSet);
	TVerdict DoTerminationTest(const TDesC& aSuite, const TDesC& aStep, TInt64 aGuardTimerValue, TInt aExitReason, const TDesC& aConfig);
	void GetHeapAndStackSize(const CStepData* aStepData, TInt* aHeapSize, TInt* aStackSize);
	void Unload();
	void HeapMark();
	void HeapCheck();
	void RequestMark();
	void RequestCheck();
	void HandlesMark();
	void HandlesCheck();
	void RunProgram(const TDesC& aText);
	void TestComplete(const TDesC& aText);
	void Delay(const TDesC& aText);
	void SetSeverity(const TDesC& aText);
	void LogSettings(const TDesC& aText);
	// logger - use only with macros
	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt, ...);
	void CreateUniqueTestThreadL(const TDesC& aBaseName, TDes& aThreadName, RThread& aTestThread, TThreadFunction aFunction, TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize, TAny *aPtr);
private:
	// record of the current test suite / step name
	TBuf<KMaxLenTestSuiteName>	iCurrentSuiteName;
	TBuf<KMaxLenTestStepName>	iCurrentStepName;

	// data members
	TPath	iTheTestPath;								// test path
	TInt	iReqsAtStart;
	TInt	iProcessHandleCountBefore;
	TInt	iThreadHandleCountBefore;

	// the current test result
	TVerdict iTestVerdict;
	// owner script
	CScript* iScript;
	// test utils
	CTestUtils* iTestUtils;
	// log
	CLog* iLog;
	// default guard timer, if any
	TInt64 iGuardTimer;
	// severity level
	TInt iSeverity;
	
	const TDesC& iMatchString;
	TBool iSkip;

	// array of CSuiteDll objects which contain 
	// information on the current loaded test suite DLLs
	CArrayPtr<CSuiteDll>*	iArrayLoadedSuiteDll;

	// Flag to indicate that there has been an error
	// and the user has selected to skip the rest of the script until a
	// test result statement is found in the log
	TBool   iBreakOnError;
	};

#endif // __PARSELINE_H__
