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
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

 #include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include "ssmcletestapp.h"
#include "ssmdebug.h"

void XSsmCleTestApp::WriteStartTimeL(RFs aFs, const TDesC& aLogPrefix)
	{
	TBuf<KTestCommandLineMaxLength> logFileName;
	logFileName.Format(_L("%Stime"), &aLogPrefix);
	TTime now;
	now.UniversalTime();
	TUint32 high = 0;
	TUint32 low = 0;
	high = I64HIGH(now.Int64());
	low = I64LOW(now.Int64());
	RFileWriteStream ws;
	User::LeaveIfError(ws.Replace(aFs, logFileName, EFileWrite));
	ws.PushL();
	ws.WriteUint32L(high);
	ws.WriteUint32L(low);
	ws.CommitL();
	CleanupStack::PopAndDestroy(); // ws
	}

void XSsmCleTestApp::WriteResultL(RFs aFs, const TDesC& aLogPrefix, const TCleTestFailHow& aFailHow)
	{
	TBuf<KTestCommandLineMaxLength> logFileName, logFileNameExitReason;
	TInt result, err;
	RFileWriteStream fileWriteStream;
			
	//For Getting the Exit Reason
	if(aFailHow == EBadRendezvous)
		{
		//This is done because for BadRendezvous Case, we need to do 2 things:
		//1. Write the constant KErrBadRendezvousTest as the Exit Reason for the process in a separate file.
		//2. Write the EBadRendezvous value to a separate file, hence separate close and pop operations required.
		logFileNameExitReason.Format(_L("%Sreason"), &aLogPrefix);
		result = KErrBadRendezvousTest;
		err =fileWriteStream.Replace(aFs, logFileNameExitReason, EFileWrite);
		CleanupClosePushL(fileWriteStream);
		User::LeaveIfError(err);
		fileWriteStream.WriteUint32L(result);
		fileWriteStream.CommitL();
		CleanupStack::PopAndDestroy(); // fileWriteStream
		}

	logFileName.Format(_L("%Sresult"), &aLogPrefix);
	result = aFailHow;
	err =fileWriteStream.Replace(aFs, logFileName, EFileWrite);
	CleanupClosePushL(fileWriteStream);
	User::LeaveIfError(err);
	fileWriteStream.WriteUint32L(result);
	fileWriteStream.CommitL();
	CleanupStack::PopAndDestroy(); // fileWriteStream
	}

TInt XSsmCleTestApp::GetRunCountL(RFs aFs, const TDesC& aLogPrefix)
	{
	TBuf<KTestCommandLineMaxLength> logFileName;
	logFileName.Format(_L("%Scount"), &aLogPrefix);

	RFileReadStream file;
	TInt err = 0;
	err = file.Open(aFs, logFileName, EFileRead);
	TInt result = 0;
	if (err != KErrNotFound && err != KErrPathNotFound)
		{
		file.PushL();
		User::LeaveIfError(err);
		result = file.ReadInt32L();
		file.Close();
		CleanupStack::PopAndDestroy(); //file
		}
	return result;
	}

TInt XSsmCleTestApp::IncrementRunCountL(RFs aFs, const TDesC& aLogPrefix)
	{
	TInt runcount = GetRunCountL(aFs, aLogPrefix);
	TBuf<KTestCommandLineMaxLength> logFileName;
	logFileName.Format(_L("%Scount"), &aLogPrefix);

	runcount++;

	RFileWriteStream file;
	User::LeaveIfError(file.Replace(aFs, logFileName, EFileWrite));
	file.PushL();
	file.WriteInt32L(runcount);
	file.Close();
	
	CleanupStack::PopAndDestroy();//file
	return runcount;
	}
	
void XSsmCleTestApp::GetCommandLineArgsL(TSsmCleTestAppArgs& aArgStruct)
	{
	DEBUGPRINT1A("Parsing commandline argument");
	TBuf<KTestCommandLineMaxLength> commandLine;
	commandLine.SetLength(0);
	if(User::CommandLineLength() > commandLine.MaxLength())
		{
		User::Leave(KErrTooBig);
		}
	User::CommandLine(commandLine);
	
	DEBUGPRINT3(_L("ssmcletestapp.cpp %d: Argument string: %S"), __LINE__, &commandLine);
	aArgStruct.iWaitTime = 0;
	aArgStruct.iSucceedOnRun = 0;
	aArgStruct.iFailHow = EDontFail;
	
	TLex cmdLnLex(commandLine);

	if (cmdLnLex.Eos()) // If there aren't any arguments, that is an error
		{
		DEBUGPRINT1A("Argument error");
		User::Leave(KErrArgument);
		}
	else
		{
		TPtrC token(cmdLnLex.NextToken());
		aArgStruct.iLogPrefix.SetLength(0);
		aArgStruct.iLogPrefix.Format(_L("%S%S"), &KTestAppLogFileLoc, &token);
		DEBUGPRINT4(_L("ssmcletestapp.cpp %d: Log file prefix (length: %d): %S"), __LINE__, aArgStruct.iLogPrefix.Length(), &(aArgStruct.iLogPrefix));
		}
	
	if(!cmdLnLex.Eos()) // If there was only one argument, we are finished and there was no timeout requested.
		{
		TPtrC token(cmdLnLex.NextToken());
		TLex timeoutLex(token);
		User::LeaveIfError(timeoutLex.Val(aArgStruct.iWaitTime)); // If Val() returns an error the second argument wasn't an integer, which it should have been
		}
	DEBUGPRINT2A("Wait time: %d", aArgStruct.iWaitTime);
	
	if(!cmdLnLex.Eos()) // If we've run out of arguments, we are finished and there was no failed runs requested.
		{
		TPtrC token(cmdLnLex.NextToken());
		TLex timeoutLex(token);
		User::LeaveIfError(timeoutLex.Val(aArgStruct.iSucceedOnRun)); // If Val() returns an error the third argument wasn't an integer, which it should have been
		}
	DEBUGPRINT2A("Times to fail: %d", aArgStruct.iSucceedOnRun);

	if(!cmdLnLex.Eos()) // If we've run out of arguments, we are finished and there was no failure mode requested.
		{
		TInt temp;
		TPtrC token(cmdLnLex.NextToken());
		TLex timeoutLex(token);
		User::LeaveIfError(timeoutLex.Val(temp)); // If Val() returns an error the third argument wasn't an integer, which it should have been
		aArgStruct.iFailHow = static_cast<TCleTestFailHow>(temp);
		}
	DEBUGPRINT2A("Failure mode: %d", aArgStruct.iFailHow);
	}
