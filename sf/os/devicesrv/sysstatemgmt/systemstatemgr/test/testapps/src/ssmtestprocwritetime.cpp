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
// usage: ssmtestprocwritetime.exe <output filename> [<timeout (seconds)>]    *
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <sysmonclisess.h>
#include <startupproperties.h>
#include "ssmtestapps.h"
#include "ssmdebug.h"

static void RunL()
	{
	TBuf<KTestCommandLineMaxLength> commandLine;
	if(User::CommandLineLength() > commandLine.MaxLength())
		{
		User::Leave(KErrTooBig);
		}
	User::CommandLine(commandLine);
	
	commandLine.TrimAll();
	
	DEBUGPRINT2A("Argument string: %S", &commandLine);
	DEBUGPRINT2A("Logfile location: %S", &KTestAppLogFileLoc);
	
	TLex cmdLnLex(commandLine);

	TBuf<KTestCommandLineMaxLength> logFileName;	
	if (cmdLnLex.Eos()) // If there aren't any arguments, that is an error
		{
		User::Leave(KErrArgument);
		}
	else
		{
		TPtrC token(cmdLnLex.NextToken());
		logFileName.Format(_L("%S%S"), &KTestAppLogFileLoc, &token);
		DEBUGPRINT3A("Log file name (length: %d): %S", logFileName.Length(), &logFileName);
		}
	
	TInt timeout = 0;
	if(!cmdLnLex.Eos()) // If there was only one argument, we are finished and there was no timeout requested.
		{
		TPtrC token(cmdLnLex.NextToken());
		TLex timeoutLex(token);
		User::LeaveIfError(timeoutLex.Val(timeout)); // If Val() returns an error the second argument wasn't an integer
		}
	
	TTime now;
	now.UniversalTime();
	TBuf<20> nowStr; // 64-bit integers have a maximum of 19 digits, plus the possibility of a - (though a current time shouldn't be negative)
    nowStr.AppendNum( now.Int64() );
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFileWriteStream ws;
	User::LeaveIfError(ws.Replace(fs, logFileName, EFileShareExclusive|EFileWrite));
	ws.PushL();
	ws.WriteL(nowStr);
	ws.CommitL();
	CleanupStack::PopAndDestroy(2); // fs, ws

	User::After(timeout * 1000000); // Microseconds = 1s
	
	RProcess::Rendezvous(KErrNone);
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunL());
		delete cleanup;
		}

	__UHEAP_MARKEND;
	return r;
	}
