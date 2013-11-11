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
// This module contains CScript class
// 
//

// system includes
#include <f32file.h>

// test system includes
#include <testframework.h>
#include "Filename.h"
#include "script.h"
#include "parseline.h"
#include "config.h"

#if !defined (__TSU_TESTFRAMEWORK__)
/**
 *
 * Script files can reference other script files.
 * KMaxDepthRecursion limits the number of references.
 * This is to catch accidental circular references in script files
 * which would otherwise cause the system to continue until all
 * memory had be used making more CScript objects.
 *
 * @xxxx
 *
 */
const TInt KMaxDepthRecursion = 100; 

#endif

/**
 *
 * Global data : count of how deep in script files parser is.
 * This is to check against infinite recursion
 *
 * NB : we must patch this out for Unit Testing, where script.cpp
 * is part of a DLL
 *
 * @xxxx
 *
 */
// do not define static if Unit Testing
#if !defined (__TSU_TESTFRAMEWORK__)
GLDEF_D TInt CScript::iScriptDepth = 0;
#endif			

/**
 *
 * Console prompts
 *
 * @xxxx
 *
 */
//_LIT(KTxtPressAnyKey,"[press any key to continue]\n");	// EABI warning removal
//_LIT(KTxtBreakOnError,"The test has failed, press X to terminate this test\n [press any other key to continue]\n");	// EABI warning removal

/**
 *
 * CScript first-phase constructor
 *
 * @xxxx
 *
 */
CScript::CScript()
	{
	}

/**
 *
 * CScript second-phase constructor for a script processor which
 * does not inherit a parser.
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
void CScript::ConstructL(CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	iLog = aLog;
	iGuardTimer = aGuardTimer;

	iMatchString = aMatchString.AllocL();

	iParse = CParseLine::NewL(this, aTestUtils, aLog, aGuardTimer, *iMatchString);
	iParseOwner = ETrue;

	iPauseAtEnd = EFalse;

#if !defined (__TSU_TESTFRAMEWORK__)
	iScriptDepth++;
#endif
	}

/**
 *
 * CScript static constructor for a script processor which
 * does not inherit a parser.
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
CScript* CScript::NewL(CTestUtils* aTestUtils, CLog * aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	CScript * self = new(ELeave) CScript;
	CleanupStack::PushL(self);
	self->ConstructL(aTestUtils, aLog, aGuardTimer, aMatchString);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 * CScript static constructor for a script processor which
 * does not inherit a parser.
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
CScript* CScript::NewLC(CTestUtils* aTestUtils, CLog * aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	CScript * self = new(ELeave) CScript;
	CleanupStack::PushL(self);
	self->ConstructL(aTestUtils, aLog, aGuardTimer, aMatchString);
	return self;
	}

/**
 *
 * CScript second-phase constructor, for a script processor which
 * inherits a parser.
 *
 * @param	"CParseLine* aParse"
 *			The parser to use
 *
 * @param	"CTestUtils*"
 *			Dummy parameter (would be used for constructing a parser);
 *			retained to maintain overload distinction
 *
 * @param	"CLog * aLog"
 *			The logger to use
 *
 * @xxxx
 *
 */
void CScript::ConstructL(CParseLine* aParse, CTestUtils*, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	iLog = aLog;
	iGuardTimer = aGuardTimer;
		
	iMatchString = aMatchString.AllocL(); // should be the same as that for aParse, for moment don't check

	iParse = aParse;
	iParseOwner = EFalse;

	iPauseAtEnd = EFalse;

#if !defined (__TSU_TESTFRAMEWORK__)
	iScriptDepth++;
#endif

	}

/**
 *
 * CScript static constructor for a script processor which
 * inherits a parser.
 *
 * @param	"CParseLine* aParse"
 *			The parser to use
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
CScript* CScript::NewL(CParseLine* aParse, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	CScript* self = new(ELeave) CScript;
	CleanupStack::PushL(self);
	self->ConstructL(aParse, aTestUtils, aLog, aGuardTimer, aMatchString);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 * CScript static constructor for a script processor which
 * inherits a parser.
 *
 * @param	"CParseLine* aParse"
 *			The parser to use
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
CScript* CScript::NewLC(CParseLine* aParse, CTestUtils* aTestUtils, CLog* aLog, TInt64 aGuardTimer, const TDesC& aMatchString)
	{
	CScript* self = new(ELeave) CScript;
	CleanupStack::PushL(self);
	self->ConstructL(aParse, aTestUtils, aLog, aGuardTimer, aMatchString);
	return self;
	}

/**
 *
 * CScript destructor
 *
 * @xxxx
 *
 */
CScript::~CScript()
	{
	// delete parser if we own it
	if(iParseOwner)
		{
		delete iParse;
		iParse = NULL;
		}

	// delete scriptbuffer
	delete iScriptBuffer;
	
	delete iMatchString;

#if !defined (__TSU_TESTFRAMEWORK__)
	iScriptDepth--;
#endif
	}


/**
 *
 * Open and read a script file.
 *
 * @param	"TFileName aScriptFileName"
 *			The script file name
 *
 * @return	"TBool"
 *			true if script file successfully read
 *
 * @xxxx
 *
 */
#ifdef EXCLUDE_FOR_UNITTEST
TBool CScript::OpenScriptFile(CFileName* /*aScriptFileName*/)
	{
	// empty function to silence OPT:REF warning under WINS UREL build
	return ETrue;
	}
#else
TBool CScript::OpenScriptFile(CFileName* aScriptFileName)
	{
	// get the full pathname default drive name and extension
	_LIT(KRelated,"\\xx.script"); 
	TParse parseScriptFileName;
	TInt returnCode = parseScriptFileName.Set(aScriptFileName->FileName(), &KRelated, NULL);
	if (returnCode != KErrNone)
		{
		ERR_PRINTF2(_L("Could not set script filename: %S"), &parseScriptFileName.FullName());
		Pause();
		return EFalse;
		}

#if !defined (__TSU_TESTFRAMEWORK__)
	if (iScriptDepth > KMaxDepthRecursion)
		{
		// prevent the parser from recursing forever
		ERR_PRINTF2(_L("Script parser aborting: depth:%d"), iScriptDepth);
		return EFalse;	
		}
#if !defined(__WINS__)
	if (iScriptDepth > 3)
		{
		// on target, we are likely to KERN-EXEC 3 if nesting more than 4 levels
		WARN_PRINTF2(_L("Warning : script parser depth = %d"), iScriptDepth);
		}
#endif
#endif

	// connect to the fileserver
	returnCode = iTheFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error trying to connect to the file server") );	
		return EFalse;
		}


	RFile listfile;
	// have we got a drive letter specified - if not, check all drives
	if (parseScriptFileName.DrivePresent())
		{
		returnCode = listfile.Open(iTheFs, parseScriptFileName.FullName(), EFileRead | EFileShareAny);
		}
	else
		{
		// checks C, D, E and Z drives - this is ugly, is there a better way of doing this?
		INFO_PRINTF1(_L("Looking for script file on all drives..."));
		_LIT(KDriveC, "C:");
		parseScriptFileName.Set(aScriptFileName->FileName(), &KRelated, &KDriveC);
		returnCode = listfile.Open(iTheFs, parseScriptFileName.FullName(), EFileRead | EFileShareAny);
		if (returnCode != KErrNone)
			{
			_LIT(KDriveD, "D:");
			parseScriptFileName.Set(aScriptFileName->FileName(), &KRelated, &KDriveD);
			returnCode = listfile.Open(iTheFs, parseScriptFileName.FullName(), EFileRead | EFileShareAny);
			if (returnCode != KErrNone)
				{
				_LIT(KDriveE, "E:");
				parseScriptFileName.Set(aScriptFileName->FileName(), &KRelated, &KDriveE);
				returnCode = listfile.Open(iTheFs, parseScriptFileName.FullName(), EFileRead | EFileShareAny);
				if (returnCode != KErrNone)
					{
					_LIT(KDriveZ, "Z:");
					parseScriptFileName.Set(aScriptFileName->FileName(), &KRelated, &KDriveZ);
					returnCode = listfile.Open(iTheFs, parseScriptFileName.FullName(), EFileRead | EFileShareAny);
					}
				}
			}
		}

	// check if open fails 
	if (returnCode != KErrNone)
		{
		parseScriptFileName.Set(aScriptFileName->FileName(), &KRelated, NULL);
		ERR_PRINTF2(_L("Failed to open script file : %S"), &parseScriptFileName.FullName());
		listfile.Close();
		iTheFs.Close();
		Pause();
		return EFalse;
		}

	// display the file being processed
	INFO_PRINTF2(_L("Reading script %S"), &parseScriptFileName.FullName());

	// get the script file size
	TInt listfilesize;
	returnCode = listfile.Size(listfilesize);
	if (returnCode != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to read script file: %S size "), &parseScriptFileName.FullName());
		listfile.Close();
		iTheFs.Close();
		return EFalse;
		}

	// JW 30-10-02 DEF004555 
	// Buffer was being orphaned if already allocated, where there was more than one
	// script file on the command line
	// Now, we check for this and delete iScriptBuffer if it already exists
	if(iScriptBuffer)
		{
		delete iScriptBuffer;
		iScriptBuffer = NULL;
		}

	// get a buffer to read the file into
	TRAPD(err, iScriptBuffer = HBufC8::NewL(listfilesize));
	if (err != KErrNone || iScriptBuffer == NULL)
		{
		ERR_PRINTF2(_L("Failed to allocate memory for script file %S "), &parseScriptFileName.FullName());
		listfile.Close();
		iTheFs.Close();
		return EFalse;
		}

	// get a pointer to the buffer
	TPtr8 ptr = iScriptBuffer->Des();

	// read the file into the buffer
	returnCode = listfile.Read(ptr);
	if (returnCode != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to read script file %S "), &parseScriptFileName.FullName());
		listfile.Close();
		iTheFs.Close();
		return EFalse;
		}

	listfile.Close();
	iTheFs.Close();
	return ETrue;
	}
#endif // EXCLUDE_FOR_UNITTEST

/**
 *
 * Parse and execute script file.
 * Assumes script file has been read into iScriptBuffer
 *
 * @return	"TVerdict"
 *			The script verdict (for logging)
 *
 * @xxxx
 *
 */
TVerdict CScript::ExecuteScriptL()
	{
	// use TLex to decode the script
	TLex8 llex(*iScriptBuffer);

	// keep a count of the line number
	TInt8 lineNo = 1;

	// loop though processing the rest a line at a time
	while(!llex.Eos())
		{
		// skip any spaces
		while ( llex.Peek() == ' ' )
			llex.Inc();

		// mark the start of the line
		llex.Mark();
		
		// move to the next
		while(!llex.Eos() && llex.Peek() != '\n')
			llex.Inc();

		// step over \n
		if ( llex.Peek() == '\n' )
			llex.Inc();
				
		// get the line 
		TPtrC8 pline = llex.MarkedToken();
		if (pline.Length() != 0)
			{
			// and then process
			ProcessLineL(pline, lineNo);
			}

		// on to the next line
		lineNo++;
		}

	// script processing complete, now return the script verdict
	// Note: the script verdicts are just for the log
	// if no tests failed then return pass for the script
	// this covers scripts which do not test anything
	return (iFail == 0 ? EPass : EFail );
	}

/**
 *
 * Process a single line from the script file.
 *
 * @param	"const TDesC8& aNarrowline"
 *			The script line
 *
 * @param	"TInt8 lineNo"
 *			The script line number
 *
 * @xxxx
 *
 */
void CScript::ProcessLineL(const TDesC8& aNarrowline, TInt8 aLineNo)
	{
	// call parse to process line
	iParse->ProcessLineL(aNarrowline, aLineNo);
	}

/**
 *
 * Display the accumulated script results.
 *
 * @xxxx
 *
 */
void CScript::DisplayResults()
	{

	INFO_PRINTF1(_L("Test Results Summary ") );
	INFO_PRINTF1(_L("-------------------- ") );
	INFO_PRINTF2(_L("Passed            :%d"),  iPass);
	INFO_PRINTF2(_L("Failed            :%d"),  iFail);
	INFO_PRINTF2(_L("Inconclusive      :%d"),  iInconclusive);
	INFO_PRINTF2(_L("Test suite errors :%d"),  iTestSuiteError);
	INFO_PRINTF2(_L("Aborted           :%d"),  iAbort);
	INFO_PRINTF2(_L("KnownFailure      :%d"),  iKnownFailure); //A new TVerdict
	INFO_PRINTF2(_L("Total             :%d"),  iTotal);

	if(iPauseAtEnd)
		{
		// A pause at the end has been requested
		Pause();
		}

	}

/**
 *
 * Pause testing.
 * NOTE : stubbed pending re-implementation of user input
 *
 * @xxxx
 *
 */
void CScript::Pause()
	{
	WARN_PRINTF1(_L("Warning : PAUSE not implemented"));
	}

/**
 *
 * Display error on the console and invite abort.
 * NOTE : stubbed pending re-implementation of user input
 *
 * @xxxx
 *
 */
TBool CScript::BreakOnError()
	{
	WARN_PRINTF1(_L("Warning : BREAK_ON_ERROR not implemented"));
	return EFalse;
	}

/**
 *
 * Add a test result to the accumulated totals.
 *
 * @param	"TVerdict aTestVerdict"
 *			The test verdict
 *
 * @xxxx
 *
 */
void CScript::AddResult(TVerdict aTestVerdict)
	{
	// another test complete, so increment total
	iTotal++;

	// add in the current result
	switch (aTestVerdict) 
		{
	case EPass:
		iPass++;
		break;
	case EFail:
		iFail++;
		break;
	case EInconclusive:
		iInconclusive++;
		break;
	case ETestSuiteError:
		iTestSuiteError++;
		break;
	case EAbort:
		iAbort++;
		break;
	case EKnownFailure: //A new TVerdict for a known failed test
		iKnownFailure++;
		break;
		}

	// display the result
	TPtrC verdictText = CLog::TestResultText(aTestVerdict);
	TPtrC currentSuiteName = iParse->CurrentSuiteName();
	TPtrC currentStepName = iParse->CurrentStepName();

	iLog->LogResult(aTestVerdict, _L("Test Result for %S:%S is %S "), 
		&currentSuiteName, &currentStepName, &verdictText);
	
	}

/**
 *
 * Add a test result from a subscript to the accumulated totals.
 *
 * @param	"CScript* aSubScript"
 *			The subscript
 *
 * @xxxx
 *
 */
void CScript::AddResult(CScript* aSubScript)
	{

	iPass += aSubScript->iPass;
	iFail += aSubScript->iFail;
	iInconclusive += aSubScript->iInconclusive;
	iTestSuiteError += aSubScript->iTestSuiteError;
	iAbort += aSubScript->iAbort;
	iKnownFailure += aSubScript->iKnownFailure;
	iTotal +=aSubScript->iTotal;
	}

/**
 *
 * Traceable logging function for parseline.
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
void CScript::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	if(aSeverity)
		{
		if(iLog)
			{
			iLog->LogExtra(aFile, aLine, aSeverity, aFmt, aList);
			}
		}

	VA_END(aList);
	}
