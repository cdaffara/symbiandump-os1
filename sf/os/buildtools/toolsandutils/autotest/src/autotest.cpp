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
// Framework for Automated Test code.
// 
//

/**
 @file autotest.cpp
*/

#include "autotest.h"

#include <flogger.h>		// for RFileLogger 


_LIT(KTestKeywordSuccess, "SUCCESS");
_LIT(KTestKeywordFailed,  "FAILED");
_LIT(KTestKeywordNotSupported,  "NOT SUPPORTED");

//
// implementation of CAutoTest
//

EXPORT_C CAutoTest* CAutoTest::NewL(const struct TAutoTestCase* aTestcases, TInt aNumberOfTestcases, const TDesC& aName, RTest& aTest)
/**
 * 2 phase constructor
 *
 * @param aTestcases         Pointer to the table with test cases
 * @param aNumberOfTestcases Number of automated test cases
 * @param aName              Name of the test report file
 * @param aTest              The RTest object owned by the client
 * @return CAutoTest*        A newly created CAutoTest object
 */
	{
	CAutoTest* p = new (ELeave) CAutoTest(aTestcases, aNumberOfTestcases, aName, aTest);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}


CAutoTest::~CAutoTest()
/**
 * D'tor
 */
	{
	// close file handles
	iTestReport.Close();
	iFileServer.Close();
	
	delete iCommandLine;
	delete iTestResults;
	delete iSkipList;
	iTestCases = NULL;
	}


EXPORT_C TInt CAutoTest::ExecuteL()
/**
 * Execute the test suite. 
 *
 * @return test result, KErrNone if all tests passed
 */
	{
	(void)User::LeaveIfError(iTestReport.Write(_L8("--- test report start---\r\n")));

	//
	// machine info
	//
	MachineInfoL();

	//
	// date & time info
	//
	(void)User::LeaveIfError(iTestReport.Write(_L8("Time started: ")));
	LogTimeL();

	//
	// component under test info (optional)
	//
	if(iCompInfo.Length())
		{
		(void)User::LeaveIfError(iTestReport.Write(_L8("Component info: ")));
		(void)User::LeaveIfError(iTestReport.Write(iCompInfo));
		(void)User::LeaveIfError(iTestReport.Write(_L8("\r\n")));
		}

	
	// decide to run the menu or the complete test
	if(iKeepGoing)
		RunAllTestsL();
	else
		StartMenuL();

	// date & time info
	(void)User::LeaveIfError(iTestReport.Write(_L8("Time finished: ")));
	LogTimeL();

	(void)User::LeaveIfError(iTestReport.Write(_L8("--- test report end---\r\n")));

	return iTestSuiteFailed ? 84 : KErrNone;

	}

EXPORT_C void CAutoTest::TestCheckPointCompareL(TInt aVal,TInt aExpectedVal, const TDesC& aText, char* aFile,TInt aLine)
	{
	if(aVal == aExpectedVal)
		return;

	TBuf<200> temp;
	temp.Copy(TPtrC8((TText8*)aFile));

	iRTest.Printf(_L("FAILED test %d : Val = %d Exp Val = %d file %S, line %d\n")
		, iTestCase+1, aVal, aExpectedVal, &temp, aLine);

	//
	// Log failure to log file, even if iKeepGoing is EFalse.
	// The code used to only log the failure to the file if iKeepGoing was ETrue.
	// If a filaure occurs it needs to be written to log regardless of iKeepGoing.
	TBuf8<256> stringBuf;
	stringBuf.AppendFormat(_L8("FAILED test %d : Val = %d Exp Val = %d file %s, line %d : "),iTestCase+1,aVal,aExpectedVal, aFile, aLine);
	stringBuf.Append(aText);
	stringBuf.AppendFormat(_L8("\r\n"));
	(void)User::LeaveIfError(iTestReport.Write(stringBuf));
	iTestSuiteFailed = ETrue;

	User::Leave(aVal);
	}


EXPORT_C void CAutoTest::TestCheckPointCodeL(TInt aCase, char* aFile, TInt aLine)
/**
 * hidden by the macro GLOBAL_CHECKPOINT_CODE. Used to validate return values etc.
 * If the 'keepgoing' command line option is specified, the test program * will not stop, but just print a warning in the test report, including
 * which file and line where the test failed. Then it will leave the test
 * case and do the next one. 
 *
 * @param aCase 0 no error or the error code
 * @param aFile name of the file involved here
 * @param aLine the line of the file involved here
 */
	{
	if(!aCase)
		return;

	// Convert to unicode for log to console
	TBuf<200> temp;
	temp.Copy(TPtrC8((TText8*)aFile));

	iRTest.Printf(_L("FAILED test %d : Code = %d file %S, line %d\n"), iTestCase+1, aCase, &temp, aLine);

	//
	// Log failure to log file, even if iKeepGoing is EFalse.
	// The code used to only log the failure to the file if iKeepGoing was ETrue.
	// If a filaure occurs it needs to be written to log regardless of iKeepGoing.
	TBuf8<256> stringBuf;
	stringBuf.AppendFormat(_L8("FAILED test %d : Code = %d file %s, line %d\r\n"),iTestCase+1, aCase, aFile, aLine);
	(void)User::LeaveIfError(iTestReport.Write(stringBuf));
	iTestSuiteFailed = ETrue;

	User::Leave(aCase);
	}

EXPORT_C void CAutoTest::TestCheckPointL(TBool aCase, char* aFile, TInt aLine)
/**
 * hidden by the macro GLOBAL_CHECKPOINT. Used to validate return values etc.
 * If the 'keepgoing' option is specified, the test program
 * will not stop, but just print a warning in the test report, including
 * which file and line where the test failed. Then it will leave the test
 * case and do the next one. 
 *
 * @param aCase if true success, false means failed
 * @param aFile name of the file involved here
 * @param aLine the line of the file involved here
 */
	{
	if(aCase)
		return;

	// Convert to unicode for log to console
	TBuf<200> temp;
	temp.Copy(TPtrC8((TText8*)aFile));

	iRTest.Printf(_L("FAILED test %d : file %S, line %d\n"),iTestCase+1, &temp, aLine);

	// Log failure to log file, even if iKeepGoing is EFalse.
	// The code used to only log the failure to the file if iKeepGoing was ETrue.
	// If a filaure occurs it needs to be written to log regardless of iKeepGoing.
	TBuf8<256> stringBuf;
	stringBuf.AppendFormat(_L8("FAILED test %d : file %s, line %d\r\n"),iTestCase+1, aFile, aLine);
	(void)User::LeaveIfError(iTestReport.Write(stringBuf));
	iTestSuiteFailed = ETrue;
	User::Leave(84);
	}

EXPORT_C void CAutoTest::SetCompInfo(const TDesC8& aCompInfo)
/**
 * store information about the Component under test in the autotest object
 *
 * @param aCompInfo descriptor containin the component info
 */
	{
	iCompInfo = aCompInfo;
	}

//
// protected
//

CAutoTest::CAutoTest(const struct TAutoTestCase* aTestcases, TInt aNumberOfTestcases, const TDesC& aName, RTest& aTest)
/**
 * C'tor
 *
 * @param aTestcases         Pointer to the table with test cases
 * @param aNumberOfTestcases Number of automated test cases
 * @param aName              Name of the test report file
 * @param aTest              The RTest object owned by the client
 */
	:iTestCases(aTestcases)
	,iNumberOfTestcases(aNumberOfTestcases)
	,iTestReportName(aName)
	,iKeepGoing(EFalse)
	,iTestSuiteFailed(ETrue)
	,iRTest(aTest)
	,iSkipList(NULL)
	,iCommandLine(NULL)
	{
	}


void CAutoTest::ConstructL()
/**
 * 2nd phase of 2 phased constructor.
 *
 * create a new table for test results storing, depending on the 
 * number of test cases. Connect to the file server and open
 * and replace the test report file.
 */
	{
	// No serial port logging
	iRTest.SetLogged(EFalse);
	// Array of test result codes
	iTestResults = new (ELeave) TInt[iNumberOfTestcases];
	// Array of bools to determine whether a test should be skipped
	// Set to ETrue if a test is not supported
	iSkipList = new (ELeave) TBool[iNumberOfTestcases];
	Mem::FillZ(iSkipList,iNumberOfTestcases * sizeof(TBool));	
	(void)User::LeaveIfError(iFileServer.Connect());

	// Read the command line or contents of config file specified on the command line,
	// into private storage
	GetCommandLineL();
	// Get the name of the output log file
	SetLogFileL();
	// Set up the test skip options
	GetSkipOptions();
	// Set the keepgoing flag
	GetKeepGoingOption();
	}


//
// private
//

void CAutoTest::PrintMenu() const
/**
 * print a menu to the console
 */
	{

	iRTest.Printf(_L("menu:\n") );
	iRTest.Printf(_L("  !  - Run all tests\n") );
	iRTest.Printf(_L("  ^C - Quit\n") );

	for(TInt i=0;i<iNumberOfTestcases;i++)
		{
		iRTest.Printf(_L("%d %s \n"), i+1, iTestCases[i].iText );
		}

	}

EXPORT_C void CAutoTest::Printf(TRefByValue<const TDesC> aFmt,...)
/**
 * print to console and logfile
 */
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> aBuf;
	aBuf.AppendFormatList(aFmt, list);

	// console
	iRTest.Printf(aBuf);

	// logfile
	LogExtra((TText8*)__FILE__, __LINE__, aFmt);
	}


void CAutoTest::StartMenuL()
/**
 * run the menu forever, until press Ctrl-C
 *
 * @leave this function may leave
 */
	{
	TInt ret;

	
	FOREVER
		{
		TBuf<256> inputBuf;
		
		PrintMenu();

		TInt key = iRTest.Getch();
		iRTest.Printf(_L("%c"), key);

		switch(key)
			{
		case '!':
            // for the time, that there is a request in the menu to run all the
            // test cases, the iKeepGoing flag shoulg should be set and deleted
            // afterwards; because test cases behave different in automated and 
            // manual mode; and here we are definitely in the automated mode
            iKeepGoing = ETrue;
			RunAllTestsL();
            iKeepGoing = EFalse;
			break;

		case 0x03: // Ctrl-C
			return;
			  
		default:

			inputBuf.Append(key);
			GetString(*iRTest.Console(), inputBuf);
			TLex lexer(inputBuf);
			TInt value;
			ret = lexer.Val(value);
			value-=1;
			if(ret == KErrNone)
				{
				iRTest.Printf(_L("Testcase: %d\n"), value+1);
				if((value >= 0) && (value < (TInt)iNumberOfTestcases))
					{
					iTestCase = value;
					ret = RunATestL();
					}
				else
					break;

				//
				// Display the success or failure to the console 
				if(ret == KErrNone)
					iRTest.Printf(_L("SUCCESS! \n"));
				else if(ret == KErrNotSupported)
					iRTest.Printf(_L("NOT SUPPORTED\n"), ret);
				else
					iRTest.Printf(_L("FAILED! (ret=%d)\n"), ret);

				//
				// Log our success/failure in the log file
				TBuf8<256> stringBuf8;
				stringBuf8.AppendFormat(_L8("test %d: "), value +1 );

				if(ret == KErrNone)
					stringBuf8.Append(KTestKeywordSuccess);
				else if(ret == KErrNotSupported)
					stringBuf8.Append(KTestKeywordNotSupported);
				else
					stringBuf8.Append(KTestKeywordFailed);

				stringBuf8.AppendFormat(_L8(" (return=%d) "), ret );
				TBuf<256> buf;
				buf.AppendFormat(_L("[%s]\r\n"), iTestCases[value].iText);
				stringBuf8.Append(buf);
				(void)User::LeaveIfError(iTestReport.Write(stringBuf8));
				}
			  
			  break;

			} // end, switch(key)

		} // end, FOREVER
	
	}

void CAutoTest::RunAllTestsL()
/**
 * run all the tests for this test program
 */
	{
	TInt ret;
	iRTest.Printf(_L("Now running all the testcases...\n") );
	
	for(TInt i=0;i<iNumberOfTestcases;i++)
		{
		iTestCase = i;
		ret = RunATestL();
		iTestResults[i] = ret;
		PrintTestReportL(i);
		}
	}

void CAutoTest::PrintThreadsToLogFile()
// Print all the current running thread names to the log file
	{
	TBuf<0x1> asterisk=_L("*");
	TName search=asterisk;

	TFindThread findHb;
	findHb.Find(search);
	TFullName name;

	while (findHb.Next(name)==KErrNone)
		{
		TBuf8<300> stringBuf8(_L8("\tThread Name = "));
		stringBuf8.Append(name);
		stringBuf8.Append(_L8("\r\n"));
		iTestReport.Write(stringBuf8);
		}
	}


TInt CAutoTest::RunATestL()// const
/**
 * run a test and return the error code.
 * the testcase is gived in iTestCase
 *
 * @return error code for that test case
 */
	{
	TInt ret;

	if(iSkipList[iTestCase])
		return KErrNotSupported;

	// check the index first
	if(iTestCase < 0 || iTestCase >= (TInt)iNumberOfTestcases)
		User::Leave(KErrNotFound);
	
	iRTest.Printf(_L("%d %s \n"), iTestCase+1, iTestCases[iTestCase].iText );

	// get number of outstanding requetsts on thread before we run the test
	TInt reqsAtStart = RThread().RequestCount();
	if(reqsAtStart!=0)
		{
		TBuf8<256> stringBuf8;
		stringBuf8.AppendFormat(_L8("Warning: thread had %d oustanding requests before starting test %d!\r\n"),reqsAtStart,iTestCase+1);
		iTestReport.Write(stringBuf8);
		}
			
	// get number of Handles *before* we start the program
	TInt processHandleCountBefore;
	TInt threadHandleCountBefore;
	RThread().HandleCount(processHandleCountBefore, threadHandleCountBefore);

	// ------------------------------------------
	void (*fn)(void) = iTestCases[iTestCase].iFunc;
	TRAP(ret, fn() );
	// ------------------------------------------

	//
	// if the test case failed then we dont bother
	// with checking the outstanding requests etc.
	//
	if(ret == KErrNone)
		{
		// get number of Handles *after* the program is finished
		TInt processHandleCountAfter;
		TInt threadHandleCountAfter;
		RThread().HandleCount(processHandleCountAfter, threadHandleCountAfter);

		// check that we are closing all the handles
		if(threadHandleCountBefore<threadHandleCountAfter)
			{

			
			TBuf8<256> stringBuf8;
			stringBuf8.AppendFormat(_L8("%d handles left open after test %d returned!\r\n"),threadHandleCountAfter-threadHandleCountBefore,iTestCase+1);
			iTestReport.Write(stringBuf8);
			
			PrintThreadsToLogFile();

			return KErrGeneral;
			}

		// check that number of oustanding requests on thread has not increased
		const TInt reqsNow=RThread().RequestCount();
		if(reqsNow>reqsAtStart)
			{
			TBuf8<256> stringBuf8;
			stringBuf8.AppendFormat(_L8("%d requests were left outstanding after test %d returned!\r\n"),reqsNow,iTestCase+1);
			iTestReport.Write(stringBuf8);
			return KErrGeneral;
			}
		}
	return ret;
	}

void CAutoTest::SetLogFileL()
/**
 * Parse the command line buffer to see if a logfile has been specified
 * If a logfile is specified create it
 */
	{
	TPtr ptr = iCommandLine->Des();
	TLex lex(ptr);

	// Parse the command line for the "-log"
	while(!lex.Eos())
		{
		TPtrC ptr = lex.NextToken();
		_LIT(KLogCommand,"-log");
		if(ptr == KLogCommand)
			break;
		}
	// Read the log filename 
	if(!lex.Eos())
		{
		TPtrC file = lex.NextToken();
		iTestReportName.Copy(file);
		}
	// Replace if it exists and if not, create the folder then create the file
	if(iTestReport.Replace(iFileServer, iTestReportName, EFileWrite) != KErrNone)
		{
		if(iTestReport.Create(iFileServer, iTestReportName, EFileWrite) == KErrPathNotFound)
			(void)User::LeaveIfError(iFileServer.MkDirAll(iTestReportName));
		(void)User::LeaveIfError(iTestReport.Create(iFileServer, iTestReportName, EFileWrite));
		}
	}

void CAutoTest::GetCommandLineL()
/**
 * Parse the command line to see if a config file has been specified: "-config afile.txt"
 * If it has, then read the contents of the file into the private command line buffer. Otherwise
 * copy the real command line into the private command line buffer
 */
	{
	TBuf<KMaxCmdLength> cmdLine;
	TBuf<KMaxFileName> fileName;
	
	#ifndef EKA2 
		RProcess().CommandLine(cmdLine);
	#else
		User::CommandLine(cmdLine);
	#endif
	
	TLex lex(cmdLine);
	// Parse the real command line for the -config option
	while(!lex.Eos())
		{
		TPtrC ptr = lex.NextToken();
		_LIT(KConfigCommand,"-config");
		if(ptr == KConfigCommand)
			break;
		}
	// Read the config filename if it's there
	if(!lex.Eos())
		{
		TPtrC file = lex.NextToken();
		fileName.Copy(file);
		}
	// If the filename has not been specified, set up the default
	// This may not exist either, by the way.
	if(!fileName.Length())
		{
		_LIT(KDefaultConfigFile,"c:\\autotestconfig.txt");
		fileName = KDefaultConfigFile;
		}
	RFile file;
	CleanupClosePushL(file);
	// Try to open the config file
	if(file.Open(iFileServer,fileName, EFileRead) == KErrNone)
		// Config file exists
		// read it into a local 8 bit buffer
		{
		TInt size;
		// Get the size of the file
		(void)User::LeaveIfError(file.Size(size));
		HBufC8* buf8 = HBufC8::NewL(size);
		CleanupStack::PushL(buf8);
		TPtr8 ptr8 = buf8->Des();
		// Read the file contents into the heap buffer
		(void)User::LeaveIfError(file.Read(ptr8,size));
		// Copy to private class unicode buffer
		iCommandLine = HBufC::NewL(size);
		TPtr ptr16 = iCommandLine->Des();
		ptr16.Copy(ptr8);
		CleanupStack::PopAndDestroy(buf8);
		}
	else
		// Config file does not exist so copy the real command line buffer to the private class copy
		{
		iCommandLine = HBufC::NewL(cmdLine.Length());
		TPtr ptr16 = iCommandLine->Des();
		ptr16.Copy(cmdLine);
		}
	CleanupStack::PopAndDestroy(&file);
	}

void CAutoTest::GetKeepGoingOption()
/**
 * Read the keepgoing flag from the command line bufffer
 */
	{
	_LIT(KKeepGoingString, "keepgoing");
	TInt ret = iCommandLine->Find(KKeepGoingString);
	if(ret >= KErrNone)
		iKeepGoing = ETrue;
	}

void CAutoTest::GetSkipOptions()
/**
 * Read the test skip options, if any, from the command line buffer
 * Usage: "-skip 1 4 5" to skip tests 1, 4 & 5 and log as NOT SUPPORTED
 */
   {
	TPtr ptr = iCommandLine->Des();
	TLex lex(ptr);
	// Parse for -skip
	while(!lex.Eos())
		{
		TPtrC token = lex.NextToken();
		_LIT(KSkipCommand,"-skip");
		if(token == KSkipCommand)
			break;
		}
	// Loop through reading the test numbers to skip
	while(!lex.Eos())
		{
		TInt testNum;
		TPtrC ptr = lex.NextToken();
		TLex lex(ptr);
		// Read the test number as an integer
		if(lex.Val(testNum) != KErrNone)
			// Exit at the first token that's not a valid number
			break;
		if(testNum <= 0 || testNum > iNumberOfTestcases)
			// Out of range
			continue;
		// Skip this test
		iSkipList[testNum-1] = ETrue;
		}
	}


void CAutoTest::PrintTestReportL(TInt aTestIndex)
/**
 * print a test report to file and console
 */
	{	
	// print status
	TBuf8<256> stringBuf8;
	stringBuf8.AppendFormat(_L8("test %d: "), aTestIndex+1 );
	if(iTestResults[aTestIndex] == KErrNone)
		stringBuf8.Append(KTestKeywordSuccess);
	else if(iTestResults[aTestIndex] == KErrNotSupported)
		stringBuf8.Append(KTestKeywordNotSupported);
	else
		stringBuf8.Append(KTestKeywordFailed);
	stringBuf8.AppendFormat(_L8(" (return=%d) "), iTestResults[aTestIndex] );
	TBuf<256> buf;
	buf.AppendFormat(_L("[%s]\r\n"), iTestCases[aTestIndex].iText);
	stringBuf8.Append(buf);
	(void)User::LeaveIfError(iTestReport.Write(stringBuf8));
	TBuf<256> stringBuf;
	stringBuf.Copy(stringBuf8);
	iRTest.Printf(stringBuf);
	}


EXPORT_C void CAutoTest::GetString(CConsoleBase& aConsole, TDes& aString) const
/**
 * This function re-invents the wheel (once again...)
 * It reads a string from the keyboard. Uses Getch() to get keys and does not
 * return until ENTER is pressed. 
 *
 * @param aConsole const reference to the console
 * @param aString reference to the string to be read into
 */
    {
	TKeyCode code;
	TBuf<1> kjar;

	FOREVER
		{
		code = aConsole.Getch();
		kjar.SetLength(0);
		kjar.Append(code);

		aConsole.Printf(_L("%S"),&kjar);
	
		// If <CR> finish editing string
		if (code == '\r')
			break;
		
		// if <BS> remove last character
		if ((code == 0x08) && (aString.Length() != 0))
			aString.SetLength((aString.Length()-1));
		else
			aString.Append(code);
		}
	aConsole.Printf(_L("\n"));
    }


void CAutoTest::LogTimeL()
/*
 * write date&time info to log file
 */
	{
	TBuf8<64> buf8;
	TTime now;
	now.HomeTime();
	TDateTime dateTime;
	dateTime = now.DateTime();
	buf8.Format(_L8("%02d/%02d/%04d %02d:%02d.%02d \r\n"), dateTime.Day()+1, dateTime.Month()+1, dateTime.Year(), 
				dateTime.Hour(), dateTime.Minute(), dateTime.Second() );
	(void)User::LeaveIfError(iTestReport.Write(buf8));
	}

EXPORT_C TBool CAutoTest::KeepGoing(void)
// For external visibility
// Handy if someone wants to use defaults instead of taking keyboard input
	{
	return iKeepGoing;
	}


EXPORT_C void CAutoTest::LogExtra(const TText8* aFile, TInt aLine, TRefByValue<const TDesC> aFmt,...)
/**
 * Write a free formatted string to a second log file and to the console.
 * The second log files name is the same as the main log file plus '.EXTRA'.
 */
	{
	VA_LIST list;
	VA_START(list,aFmt);
	//
	// Assemble file name for second log file
	_LIT(KExtra,".EXTRA");
	TBuf<KMaxCmdLength+6> logFileName;		// +6 to hold the text ".EXTRA"
	logFileName.Copy(iTestReportName);
	logFileName.Append(KExtra);

	// 
	// Assemble string to be written to second log
	TBuf<256> buf;				 
	{		// Braces used to scope lifetime of TBuf objects
	TPtrC8 fileName8(aFile);
	TBuf<128> fileName;
	fileName.Copy(fileName8);
	buf.Format(_L("\t%S\t%d\t"), &fileName, aLine);
	
	buf.AppendFormatList(aFmt,list);
	}

	//
	// (1) Open/create log file.
	// (2) Write text to log file (RFileLogger adds a time stamp for us)
	// (3) Close log
	_LIT(KLogFolder,"autotest");
	RFileLogger log;
	if(log.Connect()==KErrNone)
		{
		log.CreateLog(KLogFolder,logFileName,EFileLoggingModeAppend);
		log.Write(buf);
		log.CloseLog();
		log.Close();
		}

	//
	// Also write the text to the console (with a new line after)
	buf.Zero();
	buf.AppendFormatList(aFmt,list);
	iRTest.Printf(_L("%S\n"),&buf);
	}


void CAutoTest::MachineInfoL()
/*
 * write machine info to log file
 */
	{
	TBuf8<256> buf8;
	buf8.AppendFormat(_L8("Machine info: \r\n"));
	//
	// platform
	//
#if defined(__WINS__)
	_LIT8(KPlatform, "WINS");
#elif defined(__MARM_ARMI__)
	_LIT8(KPlatform, "ARMI");
#elif defined(__MARM_ARM4__)
	_LIT8(KPlatform, "ARM4");
#elif defined(__MARM_THUMB__)
	_LIT8(KPlatform, "THUMB");
#else
	_LIT8(KPlatform, "unknown");
#endif

#if defined (_DEBUG)
	_LIT8(KDebugRelease, "Debug");
#else
	_LIT8(KDebugRelease, "Release");
#endif

	buf8.AppendFormat(_L8("  Platform:    %S %S\r\n"), &KPlatform, &KDebugRelease );

	//
	// machine info from Hal9000
	//
	TMachineInfoV1Buf machine;
	UserHal::MachineInfo(machine);

	TVersion romver = machine().iRomVersion;
	TInt64 uid = machine().iMachineUniqueId;
    TInt khz = machine().iProcessorClockInKHz;

	buf8.AppendFormat(_L8("  Rom version: %d.%d.%d\r\n"), romver.iMajor, romver.iMinor, romver.iBuild );
	buf8.AppendFormat(_L8("  Unique ID:   0x%016x\r\n"), uid );
	buf8.AppendFormat(_L8("  CPU Speed:   %d kHz\r\n"), khz );

	User::LeaveIfError(iTestReport.Write(buf8));
	}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//
// DLL entry point
//
	{
	return(KErrNone);
	}
#endif
// EOF - AUTOTEST.CPP
