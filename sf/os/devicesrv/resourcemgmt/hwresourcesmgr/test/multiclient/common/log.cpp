// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// includes
//



#include "tc_log.h"
#include <e32base.h>
#include <e32cons.h>
#include <stdio.h>

#if (defined USE_TF_LOGGING)

//#include <tflogger.h>
#include <flogger.h>
_LIT(KTfLogFolder,"TF");
_LIT(KTfLogFile,"TFLOG.TXT");
#define TFLOGTEXT(x) RFileLogger::Write(KTfLogFolder(),KTfLogFile(),EFileLoggingModeAppend, (x))

#endif

_LIT(KFailed,   "Failed  ");
_LIT(KPassed,   "Passed  ");
_LIT(KWarnings, "Warnings");
_LIT(KComments, "Comments");

/*
================================================================================= 
Function:  Constructor

Description: Constructor initialises line counters for middle summary report and
			 for total summary report. Input arguments have been initialized also.
			 Constructor opens log file, sets file pointer to beginning of the log file
			 and prints starting text to log file.

Inputs: CConsoleBase *aConsole  (pointer of CConsoleBase -class),
		and next command lien switches:
		TDesC &aFileName		(file name of log file (-f<file name>)), 
		bool echoOn				(state of echoing (-n)),
		Priority priorityFilter (verbosity level (-v<value>)). 

Outputs: No return values
			 
=================================================================================
*/


Log::Log()
:iPassed(KPassed),
 iFailed(KFailed),
 iWarnings(KWarnings),
 iComments(KComments)
{
	iConsole = 0;
	iBatch = EFalse;
	iFile = 0;
}

void Log::OpenL(const TDesC &aFileName, TBool aBatch, TBool aEchoOn, TBool aTimestamp, Priority aPriorityFilter)
{
	// initialisation of arguments
    iPriorityFilter  = aPriorityFilter;
	iWriteTimestamp  = aTimestamp;
	iBatch           = aBatch;

	// open console, but only if echo is on...
	if (aEchoOn)
	{
		iConsole = Console::NewL(_L("TClient"), TSize(KConsFullScreen, KConsFullScreen));
	}
	
	// open file
    TBuf8<MAX_LEN> fname;   // descriptor for the file name
    fname.Copy(aFileName);	// conversion from TDesC type to TBuf8<>  
    iFile = (FILE*)fopen((const char*) fname.PtrZ(), "a");
	if (!iFile)
	{
		if (iConsole)
		{
			iConsole->Printf(_L("Can't open file %S\n"), &aFileName);
			//iConsole->Getch();
		}
		User::Leave(KErrNotFound);
	}

	// seek to start of file
	fseek((FILE*)iFile, 0, SEEK_END);		// sets file pointer to end of log file

	// mark session as started..
	Printf(Log::SESSION_MARK, _L("*******************************************\nBEGIN LOG\n"));	// starting the session text
}

/*
================================================================================= 
Function:  Destructor

Description: Destructor prints total summary report to log file and console if 
			 allowed. Finally destructor close the log file and console.
			
Inputs:  No inputs
Outputs: No return values

=================================================================================
*/

Log::~Log()
{
    if (iFile) 
	{
		// print total summary report 
		Printf(SUMMARY, _L("SESSION TOTAL"));
		iFailed.Total(*this);
		iPassed.Total(*this);
		iWarnings.Total(*this);
		iComments.Total(*this);
		Printf(SESSION_MARK, _L("END LOG"));
		fclose((FILE*)iFile);
	}

	if (iConsole)
	{
		// if printing to console is allowed. then next line will be printed and console window will
		// closed only after pressing return.
		if (!iBatch)
		{
			iConsole->Printf(_L("[press return to continue]\n"));
			iConsole->Getch();
		}
		delete iConsole;
	}
}


/*
================================================================================= 
Function:  SummaryReport

Description: Prints middle summary report. Summary lines includes number of 
			 lines between script file commands begin and end   

Inputs: TBuf<MAX_LEN> aTestCaseName Name of test case; 
Output: No return values	


=================================================================================
*/

void Log::SummaryReport(const TDesC& aTestCaseName)
{
	if (iFile)
    {
		Write(SUMMARY, aTestCaseName);
		iFailed.Summary(*this);
		iPassed.Summary(*this);
		iWarnings.Summary(*this);
		iComments.Summary(*this);
    }

}



void Log::RequestKeyPress()
{
	_LIT(KPressReturn, "[press return to contine]\n");

	if (iConsole && !iBatch)
	{
		iConsole->Write(KPressReturn);
		iConsole->Read(iConsoleKeyRequest); // request key press
	}
}


TBool Log::ReceiveKeyPress()
{
	TBool received = EFalse;
	if (iConsole && !iBatch)
	{
		if (iConsoleKeyRequest == KRequestPending)
		{
			received = EFalse; // still waiting
		}
		else
		{
			User::WaitForRequest(iConsoleKeyRequest);
			if (iConsole->KeyCode() == '\r')
			{
				received = ETrue; // got the right key
			}
			else
			{
				iConsole->Read(iConsoleKeyRequest);
				received = EFalse; // wait some more (wrong key)
			}
		}
	} else
	{
		received = ETrue; // simulate received
	}
	return received;
}

void Log::WaitKeyPress()
{
	if (iConsole && !iBatch)
	{
		TBool waiting = ETrue;
		while (waiting)
		{
			User::WaitForRequest(iConsoleKeyRequest);
			if (iConsole->KeyCode() == '\r')
			{
				waiting = EFalse;
			}
			else
			{
				iConsole->Read(iConsoleKeyRequest);
			}
		}
	}
}

TInt Log::MultipleChoice(TInt aMin, TInt aMax)
{
	TInt choice = aMin;
	if (iConsole && !iBatch)
	{
		do
		{
			iConsole->Printf(_L("[Press key %d...%d]\n"), aMin, aMax);
			iConsole->Read(iConsoleKeyRequest);
			User::WaitForRequest(iConsoleKeyRequest);
			choice = TInt(iConsole->KeyCode()) - TInt('0');
		} while (choice < aMin || choice > aMax);
	}
	return choice;
}

void Log::SetTitle(const TDesC& aTitle)
{
	if (iConsole)
	{
		iConsole->SetTitle(aTitle);
	}
}


/*
================================================================================= 
Function:  Printf

Description: This method adds line counters prints,  C-string format information to logfile.

Inputs: Priority priority, const char *fmt This method can have variant number 
		or arguments prints only allowed lines to buffer. Content of buffer will
		be printed to log file and to console if allowed.

Ouputs	No return values:


=================================================================================
*/

void Log::Printf(Priority priority, const char *fmt, ...)
{
	// route to another printf..
	TInt   len = 0;

    __e32_va_list list;
    va_start(list, fmt);
    len = vsprintf((char*)iUint8, fmt, list);
	va_end(list);

	iStr.Copy(TPtrC8(iUint8,len) );
	Write(priority, iStr);

}





/*
================================================================================= 
Function:  Printf

Description: This method prints TDesC -format information to logfile.

Inputs: TRefByValue<const TDesC> aFmt. This method can 
		have variant number of arguments.


=================================================================================
*/
void Log::Printf(TRefByValue<const TDesC> aFmt, ...)
{
	// rest of printf...
	VA_LIST list;
    VA_START(list,aFmt);
	iStr.FormatList(aFmt, list);
	VA_END(list);
	Write(Log::COMMENT, iStr);
}

/*
================================================================================= 
Function:  Printf

Description: This method prints TDesC -format information to logfile.

Inputs: Priority priority, TRefByValue<const TDesC> aFmt. This method can 
		have variant number of arguments.


=================================================================================
*/
void Log::Printf(Priority priority, TRefByValue<const TDesC> aFmt, ...)
{
	// rest of printf...
	VA_LIST list;
    VA_START(list,aFmt);
	iStr.FormatList(aFmt, list);
	VA_END(list);
	Write(priority, iStr);
}

/*
================================================================================= 
Function:  Write

Description: This method writes TDesC -format information to logfile.

Inputs: Priority priority, const TDesC& aFmt.


=================================================================================
*/

void Log::Write(Priority priority, const TDesC& aStr)
{
	// names of priorities
  //don't need these (paivheik)
	/* static TText *priorityNames[] = {
        (TText*)L"ZERO    ",
        (TText*)L"PASS    ",
        (TText*)L"NOTIFY  ",
		(TText*)L"SCHEDUL ",
		(TText*)L"MONITOR ",
        (TText*)L"WARNING ",
        (TText*)L"FAIL    ",
        (TText*)L"COMMENT ",
        (TText*)L"SUMMARY ",
        (TText*)L"SESSION ",
		(TText*)L"BEGIN   ",
		(TText*)L"END     "
    };
   
	*/

   // add line counters
    switch (priority)
    {
    case PASS:    iPassed++; break;
	case FAIL:    iFailed++; break;
	case WARNING: iWarnings++; break;
    case COMMENT: iComments++; break;
	default: break;
    }

    // filter out unnecessary stuff
   if ((int)iPriorityFilter > (int)priority)
        return;


   // get current time time and format it to string
	TBuf<20> time_str;
    iTime.UniversalTime();
    TRAPD(error, iTime.FormatL(time_str, _L("%H:%T:%S:%*C3")) );
    if (error != KErrNone)
	{
		time_str.Copy(_L("TIMER-ERROR"));
	}


	// cleaning iLog_bufout
	iLog_bufout.Delete(0,MAX_LEN*2);
	iLog_bufout.SetLength(0);

	// priority name and line number

	//don't need in these tests,
/*	TPtrC priName(priorityNames[priority]);
	iLog_bufout.AppendFormat(_L(" %S (%3d)\t"), &priName, Lex::GetLineNo());*/

	// rest of printf...
	iLog_bufout.Append(aStr);


	// Log can ouput to three diffrent destinations
	// (at the same time). Console window,
	// text file, or flogger.
	
	// output to console
	if (iConsole)
	{
		iConsole->Write(time_str);
		iConsole->Write(iLog_bufout);
		iConsole->Write(_L("\n"));
	}

	// output to file
	if (iFile)
	{
		// cleaning iOut8
		iOut8.Delete(0,MAX_LEN*2);
		iOut8.SetLength(0);
		if (iWriteTimestamp)
		{
			iOut8.Copy(time_str);
			fputs((char*)iOut8.PtrZ(), (FILE*)iFile);
		}
		iOut8.Copy(iLog_bufout);
		fputs((char*)iOut8.PtrZ(), (FILE*)iFile);
		fputs("\r\n", (FILE*)iFile);
		fflush((FILE*)iFile);
	}

#if (defined USE_TF_LOGGING)
	// output to flogger
	{
		_LIT(KTClient, "TC:");
		iLog_bufout.Insert(0, KTClient);
		TFLOGTEXT(iLog_bufout);
	}
#endif

}


