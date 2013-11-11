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

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

/**
 *
 * File name / line buffer lengths
 *
 * @xxxx
 *
 */

class CParseLine;
class CFileName;

/**
 *
 * Script processor.
 * Reads, parses and executes the test script.
 *
 * @xxxx
 *
 */
class CScript : public CBase
	{
public:
	static CScript* NewL(CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);
	static CScript* NewL(CParseLine* aParse, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);
	static CScript* NewLC(CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);
	static CScript* NewLC(CParseLine* aParse, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);
	~CScript();	

	TBool OpenScriptFile(CFileName* aScriptFileName);		// read in a script file
	TVerdict ExecuteScriptL();						// parse and execute script 
	void DisplayResults();
	void AddResult(TVerdict aTestVerdict);
	void AddResult(CScript* aSubScript);
	void Pause();
	TBool BreakOnError();

	TBool iPauseAtEnd;
	// iMultThread removed - multithreading now standard

protected:
	CScript();
	void ConstructL(CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);
	void ConstructL(CParseLine* aParse, CTestUtils*, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString);

private:
	void ProcessLineL(const TDesC8& aNarrowline, TInt8 aLineNo);	// process a line of script
	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt, ...);

private:
	// data members
	RFs		iTheFs;										// the file system
	HBufC8* iScriptBuffer;								// ptr to script file in memory
	TPath	iTheTestPath;								// test path

	// line parse object
	CParseLine* iParse;
	// flag indicates whether iParse is owned or not
	TBool	iParseOwner;

	// recursion count - static global 
// do not define static if Unit Testing
#if !defined (__TSU_TESTFRAMEWORK__)
	static TInt iScriptDepth;
#endif

	// log
	CLog* iLog;

	// default guard timer, if any
	TInt64 iGuardTimer;
	
	// matchString if any
	HBufC* iMatchString;

	// current results
	TInt	iPass;
	TInt	iFail;
	TInt	iInconclusive;
	TInt	iTestSuiteError;
	TInt	iAbort;
	TInt	iKnownFailure;
	TInt	iTotal;

	};

#endif // __SCRIPT_H__
