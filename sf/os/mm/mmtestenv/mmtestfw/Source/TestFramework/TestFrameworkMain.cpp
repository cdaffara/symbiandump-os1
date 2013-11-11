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

#include "TestFrameworkMain.h"
#include "../../recog/TestFrameworkRecog.h"
#include "script.h"
#include "parseline.h"
#include "Filename.h"
#include "config.h"

// Hurricane emulator only - start all services which we require to run tests.
// Future enhancement :- add these startups to TestUtils
#if defined(__WINS__)
IMPORT_C TInt FbsStartup();
#endif

/**
 *
 * Literals : program information and usage
 *
 * @xxxx
 *
 */
_LIT(KTxtFrameworkStarting, "%S %S %S starting....");
//_LIT(KTxtUseExample,"Usage:\nTESTFRAMEWORK [-C] [-F] <file.script> [file.ini]"); // EABI warning removal

/**
 *
 * Compiler switches
 *
 * @xxxx
 *
 */
#ifdef _WIN32
_LIT(KTxtTarget,"WINS");
#else 
#ifdef __MARM_THUMB__ 
_LIT(KTxtTarget,"THUMB");
#else
_LIT(KTxtTarget,"ARM4");
#endif
#endif

#ifdef _DEBUG
_LIT(KTxtBuild,"udeb");
#else
_LIT(KTxtBuild,"urel");
#endif


/**
 *
 * max length of command line
 *
 * @xxxx
 *
 */
const TInt KMaxLenCmdLine = 256;

/**
 *
 * Test Framework startup function.
 * Creates an active scheduler for input if required, reads
 * the command line, starts the main test loop.
 *
 * @xxxx
 *
 */
void StartupL()
	{
	CActiveScheduler* pA=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(pA);
	CActiveScheduler::Install(pA);

// Hurricane emulator only - start all services which we require to run tests.
// Future enhancement :- add these startups to TestUtils
#if defined(__WINS__)
	#ifndef EXCLUDE_FOR_UNITTEST
	FbsStartup();
	#endif // EXCLUDE_FOR_UNITTEST
#endif

	// read the command line into cmd
	TPtr16 cmd(REINTERPRET_CAST(TUint16*,User::AllocLC(KMaxLenCmdLine*2)), 0, KMaxLenCmdLine);
	cmd.Fill('\0', KMaxLenCmdLine);

	User::CommandLine(cmd);
	cmd.UpperCase();

	CTestFrameworkMain* tester = CTestFrameworkMain::NewLC();
	tester->StartTestingL(cmd);

	// NOTE. Currently there is no need to start the active scheduler, as the input console is
	// now at the server. This will however change when AOs are implemented to replace
	// the main client loop.

	// CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(3);	//tester, pA, cmd
	}


GLDEF_C TInt E32Main()
	{

	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	// start scheduler 
	TRAPD(error, StartupL());
	__ASSERT_ALWAYS(!error, User::Panic(_L("TestFramework"), error));

	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return KErrNone;
	}

// Instructions for Console Display
// Please add new entries to the right hand column, 
// Precede number and text with "\t "
// Leave \n\ at end of each line except the last line
//_LIT(KTxtMainInstructions, "Welcome to TestFramework. Press Q to quit ");	// EABI warning removal


/**
 *
 * CTestFrameworkMain static constructor.
 *
 * @xxxx
 *
 */
CTestFrameworkMain* CTestFrameworkMain::NewLC()
	{
	CTestFrameworkMain* s = new(ELeave) CTestFrameworkMain;
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

/**
 *
 * CTestFrameworkMain first-phase constructor.
 *
 * @xxxx
 *
 */
CTestFrameworkMain::CTestFrameworkMain()
	{
	}

/**
 *
 * CTestFrameworkMain second-phase constructor.
 * Loads log client and test utils.
 *
 * @xxxx
 *
 */
void CTestFrameworkMain::ConstructL()
	{
	iLogClient	  = CLog::NewL();
	iLogMode	  = ELogToConsole | ELogToFile;
	iTestUtils	  = CTestUtils::NewL(iLogClient);
	iGuardTimer   = KNoGuardTimer;	// default value
	}

/**
 *
 * CTestFrameworkMain destructor.
 *
 * @xxxx
 *
 */
CTestFrameworkMain::~CTestFrameworkMain()
	{
	delete iTestUtils;
	delete iLogClient;
	}

/**
 *
 * CTestFrameworkMain - start testing.
 * Calls main test loop.
 *
 * @param "const TDesC& aCmdLine"
 *			The command line
 * 
 * @xxxx
 *
 */
void CTestFrameworkMain::StartTestingL(const TDesC& aCmdLine)
	{
	RunTestScriptL(aCmdLine);

	RSemaphore sem;
	TInt err = sem.OpenGlobal(KRecogSemaphoreName);
	if (err==KErrNone)
		{
		// Tell the recognizer thread that we're finished
		sem.Signal();
		sem.Close();
		}
	}

/**
 *
 * Accessor : log client
 *
 * @return	"CLog*"
 *			The log client
 * 
 * @xxxx
 *
 */
CLog* CTestFrameworkMain::LogClient() const
	{
	return iLogClient;
	}

/**
 *
 * Main testing loop.
 * Read a script file, parse it and execute each test step in turn.
 *
 * @param "const TDesC& aCmdLine"
 *			The command line
 * 
 * @xxxx
 *
 */
void CTestFrameworkMain::RunTestScriptL(const TDesC& aCmdLine)
	{
	// use TLex to decode the cmd line
	TLex lex(aCmdLine);
	TPtrC token=lex.NextToken();
	
	// if there is no input filename on the cmd line, panic
	if (token.Length() == 0) 
		UsageL();
	else
		{
		// Process any options
		while(token.Length() > 1 && token[0] == '-')
			{
			switch(token[1])
				{
				case 'C':
				case 'c':
					// log to console ONLY
					iLogMode = ELogToConsole;
					break;
				case 'A':
				case 'a':
					iLogMode |= ELogConsoleFull;
					break;
				case 'F':
				case 'f':
					// log to file ONLY
					iLogMode = ELogToFile; 
					break;
				case 'P':
				case 'p':
					// log to port AS WELL AS to console / file
					iLogMode |= ELogToPort;
					break;
				//This stops the emulator from thowing int 3 if a panic occurs in debug builds
				case 'T':
				case 't':
					User::SetJustInTime(EFalse);
				// -S flag removed - was for old Unit Test mode only
				// -A 'automated mode' removed - it's always automated
					break;
				case 'G':
				case 'g':
					{
					// ** guard timer override - get numeric value that follows
					TPtrC val = &token[2];
					TLex lexTimeOut(val);
					if (lexTimeOut.Val(iGuardTimer) != KErrNone)
						UsageL();
					}
					break;
				case 'm':
				case 'M':
					{
					if (token.Length()<=2)
						{
						// only -m found. must be -m<arg> with no space
						UsageL();
						}
					TPtrC restOfLine = &token[2]; // this will be rest of command line
					TLex argument(restOfLine);
					TPtrC matchString = argument.NextToken(); // will be the argument itself
					ASSERT(matchString.Length()>1);
					iTestMatchString = matchString;
					}
					break;
				case 'Q':
				case 'q':
					{
					// This flag has been removed.  This block is just to ensure that if used it wont panic
					}
					break;

				default:
					UsageL();
					return;
				}

			token.Set(lex.NextToken());
			}

		// save the input filename
		CFileName* scriptFileName = CFileName::NewLC();
		*scriptFileName = token;

		// make the log file name from the script file name
		CFileName* logFileName = CFileName::NewLC();
		*logFileName = token;
		RDebug::Print(_L("TestFrameWorkMain.cpp: RunTestScriptL 1"));
		// open the log file
		iLogClient->OpenLogFileL(logFileName->FileName(), iLogMode);
		RDebug::Print(_L("TestFrameWorkMain.cpp: RunTestScriptL 2"));	
		iLogClient->LogExtra(__FILE8__, __LINE__, ESevrInfo,
				KTxtFrameworkStarting, &KTxtVersion(), &KTxtTarget(), &KTxtBuild());
		RDebug::Print(_L("TestFrameWorkMain.cpp: RunTestScriptL 3"));
		// create a ParseScript object
		CScript* parseScript = CScript::NewLC(iTestUtils, 
											  iLogClient, 
											  iGuardTimer,
											  iTestMatchString);
		RDebug::Print(_L("TestFrameWorkMain.cpp: RunTestScriptL 4"));
		// parse all scripts
		do
			{
			// get the next file
			*scriptFileName = token;
				
			// read in the script file
			if ( parseScript->OpenScriptFile(scriptFileName))
				{
				// process it
				parseScript->ExecuteScriptL();
				// display results summary
				parseScript->DisplayResults();
				}
			// get the next
			token.Set(lex.NextToken());
			} while ( token.Length()!=0 );
		RDebug::Print(_L("TestFrameWorkMain.cpp: RunTestScriptL 5"));
		CleanupStack::PopAndDestroy(parseScript);

		// close the logging system
		iLogClient->CloseLogFile();

		CleanupStack::PopAndDestroy(logFileName);
		CleanupStack::PopAndDestroy(scriptFileName);
		}
	}

/**
 *
 * Display command line format.
 * 
 * @xxxx
 *
 */
void CTestFrameworkMain::UsageL()
	{
	// If command line is erroneous, raise a panic. 
	// At this point, there may be no log outputs at all.
	User::Panic(_L("TestFramework"), 2);
	}
