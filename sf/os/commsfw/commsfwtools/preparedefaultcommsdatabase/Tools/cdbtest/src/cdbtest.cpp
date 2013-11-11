// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cdbtest.cpp
 @internalComponent
*/

#include <bacline.h>
#include "testlog.h"
#include "testscriptparser.h"
#include "testexecutestepbase.h"
#include "Te_commsdatSuiteServer.h"
#include "Te_cedSuiteServer.h"
#include "posix.h"



// max length of path/filename
#define MAX_BUFFER_LEN			256								

// max length of arg list to program
#define MAX_ARG_LEN				(MAX_BUFFER_LEN * 3)			

// Application name
#define APPLICATIONNAME			_L("cdbtest")

_LIT(SCRIPT_TARGET, "");
_LIT(LOG_TARGET, "cdbtest.log");


// PROTOTYPES
TInt ParseCommandLine(TInt &aDebugOn, TDes &aIn, TDes &aOut, CConsoleBase* aConsole);
void ProcessScriptFile(CTestScriptParser* afileParser, CTestLog* aMsg);
void HelpDump(CConsoleBase* aConsole);


void MainL(CConsoleBase* aConsole)
/** Central processing unit of cdbtest

 @return void
 @leave refer only called functions and dependant components
 */
	{
	TInt bDebugOn = EFalse;

	TBuf<MAX_BUFFER_LEN> fIn;
	TBuf<MAX_BUFFER_LEN> fOut;
	RFs fsSession; // file system session

	// connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	// Get arguments from the command line
	TInt valid = ParseCommandLine(bDebugOn, fIn, fOut, aConsole);

	if (valid)
		{
		// initialise output log file
		CTestLog* outMsg = CTestLog::NewLC(fsSession, fOut, bDebugOn, aConsole);

		outMsg->Msg(_L(" "));
		outMsg->Msg(_L("===================================================="));
		outMsg->Msg(APPLICATIONNAME);
		outMsg->Msg(_L("===================================================="));
		outMsg->Msg(_L(" "));
		outMsg->Msg(_L("Processing test script from [%S]"), &fIn);
		outMsg->Msg(_L("Writing logfile to [%S]"), &fOut);
		outMsg->Msg(_L(" "));

		CTestScriptParser* fileParser = CTestScriptParser::FactoryLC( fIn, outMsg );

		TRAPD(err, fileParser->OpenScriptFileL());
		if (err == KErrNone)
			{
			ProcessScriptFile(fileParser, outMsg);
			}
		else
			{
			outMsg->Msg(_L("Failed to open test script from [%S]"), &fIn);
			}
		CleanupStack::PopAndDestroy(fileParser);
		CleanupStack::PopAndDestroy(outMsg);
		}
	CleanupStack::PopAndDestroy(&fsSession);
	}

LOCAL_C void doMainL()
	{
	// allocate a Console
	CConsoleBase* aConsole;
	aConsole = Console::NewL(APPLICATIONNAME, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(aConsole);

	// call main routine
	MainL(aConsole);

	CleanupStack::PopAndDestroy(aConsole);
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	// set up trap cleanup framework
	CTrapCleanup* theCleanup = CTrapCleanup::New();

	// call main routine and trap any exceptions
	TRAPD(ret, doMainL());
	
	TBool bSuccessful = ETrue;
	if (ret != KErrNone)
		{
		bSuccessful = EFalse;
		}

	// clean up when finished
	delete theCleanup;

	__UHEAP_MARKEND;

	// Convert the boolean idea of "success" into the customary process idea of errorlevel, where zero
	// is success and anything else designates particular kinds of failure. Specific failure reasons aren't
	// provided, so the caller should only look for process exit != 0 to mean failure, and then examine the
	// log to see what failed.
	return !bSuccessful;
	}

TInt ParseCommandLine(TBool &aDebugOn, TDes &aIn, TDes &aOut, CConsoleBase* aConsole)
/** Parse the command line for any overriding settings from exe command line 

 @param bDebugOn Wether to output debug messages to the log file using CTestLog::Dbg(TPtrC text, ...)
 @param fIn Path and filename of the configuration database file
 @param fOut Filename of the file to send logging too
 @return ETrue if Successful else EFalse 
 */
	{
	TInt valid = EFalse;
	CCommandLineArguments *pCmd;
	pCmd = CCommandLineArguments::NewL();

	// set defaults
	aIn  = SCRIPT_TARGET;
	aOut = LOG_TARGET;

	// Flags for encountering the specification
	// of an input and an output file
	TBool bInFound = EFalse;
	TBool bOutFound = EFalse;

	if (pCmd)
		{
		TBuf<MAX_ARG_LEN> arg;

		// check all arguments for switches
		TInt i = 0;
		while ( ++i < pCmd->Count() )
			{
			arg = pCmd->Arg(i);
			arg.UpperCase();

			//Display help
			if (arg.FindF(_L("-H")) != KErrNotFound)
				{
				HelpDump(aConsole);
				delete pCmd;
				return valid;
				}

			// Debug switch
			if (arg.FindF(_L("-D")) != KErrNotFound)
				{
				aDebugOn = ETrue;
				continue;
				}

			// Specification of an input script file
			if (arg.FindF(_L("-I")) != KErrNotFound)
				{
				if (i != pCmd->Count()-1)
					{
					aIn = pCmd->Arg(++i);
					bInFound = ETrue;
					continue;
					}
				else
					{
					delete pCmd;
					aConsole->Printf(_L("Argument missing after '-i' switch\n"));
#ifndef __TOOLS2__
					aConsole->Printf(_L("\nPress any key to finish"));
					aConsole->Getch();
#endif
					return valid;
					}
				}
			// Specification of an output log file
			if (arg.FindF(_L("-O")) != KErrNotFound)
				{
				if (i != pCmd->Count()-1)
					{
					aOut = pCmd->Arg(++i);
					bOutFound = ETrue;
					continue;
					}
				else
					{
					delete pCmd;
					aConsole->Printf(_L("Argument missing after '-o' switch\n"));
#ifndef __TOOLS2__
					aConsole->Printf(_L("\nPress any key to finish"));
					aConsole->Getch();
#endif
					return valid;
					}
				}

			// No flag is supplied: first file encountered is the input
			// file and the following one is the output file

			if ( !bInFound)
				{
				aIn = pCmd->Arg(i);
				bInFound = ETrue;
				continue;
				}
			if ( !bOutFound)
				{
				aOut = pCmd->Arg(i);
				bOutFound = ETrue;
				continue;
				}
			}

		if (aIn.Length() > 0)
			valid = ETrue;

		if ( !bInFound)
			{
			    aConsole->Printf(_L("No test script file found please specify using -i\n"));
#ifndef __TOOLS2__
			    aConsole->Printf(_L("\nPress any key to finish"));
			    aConsole->Getch();
#endif

			    valid=EFalse;
			}

		delete pCmd;
		}

	return valid;
	}

void ProcessScriptFile(CTestScriptParser* afileParser, CTestLog* aMsg)
    {
	
	// Initialise the test counters
	TUint totalTests       = 0;
	TUint testPass         = 0;
	TUint testInconclusive = 0;
	TUint testFail         = 0;
	TUint testNotExecuted  = 0;
	TUint testAbort        = 0;

	TBool end(EFalse);
	CTestScriptParser::TScriptAction runAction;

    CTestServer* simServer = NULL;
	
    do
	    {
	    TBuf<MAX_BUFFER_LEN> stepParameters;
	
	    end = afileParser->CTestScriptParser::GetNextTestStepL(runAction, stepParameters);
	    if (runAction == CTestScriptParser::ELoadTestSuite)
	    	{
	    	if (simServer)
	    		{
				CleanupStack::PopAndDestroy(simServer);
				simServer = NULL;
	    		}
	    	
	    	if (stepParameters.Compare(_L("Te_commsdat")) == 0)
	    		{
			    // Create the simulator for the TestExecute Suite
			    simServer = CTe_commsdatSuite::NewL();
			    CleanupStack::PushL(simServer);
	    		}
	    	if (stepParameters.Compare(_L("te_cedSuite")) == 0)
	    		{
			    // Create the simulator for the TestExecute Suite
			    simServer = CTe_cedSuite::NewL();
			    CleanupStack::PushL(simServer);
	    		}

	    	}
	    else if(runAction == CTestScriptParser::ERunTestStep)
	        {
		    
		    // Create the next test
		    CTestStep* testStep = NULL;
		    if (simServer)
		    	{
		    	testStep = simServer->CreateTestStep(stepParameters);
		    	}
		    else
		    	{
		    	aMsg->Msg(_L("Error: Test step found but no test suite loaded."));
		    	}
		    
		    if (testStep)
		    	{
                CleanupStack::PushL(testStep);			    

		    	// Now run the test
		    	totalTests++;
		    	
		    	testStep->SetLogger(aMsg);
		    	
			    // Do the Test Preamble
		    	TRAPD(err, testStep->doTestStepPreambleL())
		    	if (err == KErrNone)
		        	{
		    		// Preamble was OK so do the actual Test step
		    		TRAP(err, testStep->doTestStepL());
		    
			        // Always run the Test Postamble step even if the test failed
		    		TRAP(err, testStep->doTestStepPostambleL());
		        	}
		    	else
		    	    {
				    // Do not run the test
		    		// Tell the user about the error
		    		aMsg->Dbg(_L("%S->doTestStepPreambleL failed - test not run"), &stepParameters);
		    	    }
		    					    
			    // Now print out the result
				switch(testStep->TestStepResult())
					{
					case EPass:
					    {
					    aMsg->Msg(_L("%S passed"), &stepParameters);
					    testPass++;
					    break;
					    }
					case EInconclusive:
					    {
					    aMsg->Msg(_L("%S was inconclusive"), &stepParameters);
					    testInconclusive++;
					    break;
					    }
					case EAbort:
					    {
					    aMsg->Msg(_L("%S was aborted"), &stepParameters);
						testAbort++;
					    break;
					    }
					case EIgnore:
					    {
					    aMsg->Msg(_L("%S was ignored"), &stepParameters);
					    testNotExecuted++;
					    break;
					    }
					case ETestSuiteError:
					    {
					    aMsg->Msg(_L("%S had a testsuite error"), &stepParameters);
					    testFail++;
					    break;
					    }
					default:
					case EFail:
					    {
					    aMsg->Msg(_L("%S failed"), &stepParameters);
					    testFail++;
					    break;
					    }
					}
                    CleanupStack::PopAndDestroy(testStep);
			    testStep = NULL;
		        }
		        else
		        {
		        	aMsg->Msg(_L("%S not executed"), &stepParameters);
				    testNotExecuted++;
		        }
	        }
	    else if(runAction == CTestScriptParser::ERunProgram)
	        {
	    	aMsg->Msg(_L("Running %S"), &stepParameters);

		    // stepParameters contains the entire command line.
		    // Make a POSIX call to launch the command
		    char commandLine[MAX_BUFFER_LEN];
		    
		    TInt i;
		    for(i = 0; i < stepParameters.Length(); i++)
		        {
		    	commandLine[i] = (char)stepParameters[i];
		        }
		    commandLine[i] = '\0';
		    TInt exitValue = RunProgram(commandLine);
		    if (exitValue)
		    	{
		    	 aMsg->Msg(_L("%S failed with exit value %d"),&stepParameters, exitValue);
		    	}
	        }
	    else if(runAction == CTestScriptParser::EPrint)
	        {
	    	aMsg->Msg(_L("%S"),&stepParameters);
	        }

	    }while(!end);
    
	    if (simServer)
		    {
		    CleanupStack::PopAndDestroy(simServer);
		    }

	aMsg->Msg(_L(" "));
	aMsg->Msg(_L("Test Summary"));
	aMsg->Msg(_L("-----------------------"));
	aMsg->Msg(_L("total tests run    = %d"),  totalTests );
	aMsg->Msg(_L("tests passed       = %d "), testPass );
	aMsg->Msg(_L("tests failed       = %d"),  testFail );
	aMsg->Msg(_L("tests aborted      = %d"),  testAbort );
	aMsg->Msg(_L("tests inconclusive = %d"),  testInconclusive );
	aMsg->Msg(_L("tests not executed = %d"),  testNotExecuted );
    }


void HelpDump(CConsoleBase* aConsole)
// Prints basic help information to the console window including command switches
	{
	aConsole->Printf(_L("Tests commsdat by simulatimg the TestExecute framework."));
	aConsole->Printf(_L("\n\ncdbtest [-d] [-i [path]filename] [-o [path]filename]"));
	aConsole->Printf(_L("\n-i\tSpecifies an input test script file. Failure to provide this parameter \r\n"));
	aConsole->Printf(_L("\twill lead to the application exiting with no tests being run. \r\n"));
	aConsole->Printf(_L("\n-o\tSpecifies an output file for logging the test output messages. \r\n"));
	aConsole->Printf(_L("\tIf this parameter is not specified, the application defaults to the \r\n"));
	aConsole->Printf(_L("\tfile cdbtest.log in the local directory. \r\n"));
	aConsole->Printf(_L("\n-d\tEnables debug mode which outputs more data to the log file.\n"));
#ifndef __TOOLS2__
	aConsole->Printf(_L("\nPress any key to finish"));
	aConsole->Getch();
#endif
	}



