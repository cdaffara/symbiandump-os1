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
// This process waits 1 second then writes the content of the command line to   *
// the log file.                                                                *
// In this way, if the process is launched multiple times in sequence, with     *
// the command line consisting of an incrementing integer, you can discover     *
// how far along the list successful executions have gone. The wait of one      *
// second is there so that a list can be constructed which takes a non-trivial  *
// amount of time to run from beginning to end without having to populate it    *
// with an extrememly large number of processes to launch.                      *
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
#include <startupproperties.h>
#include "testapps.h"

_LIT(KLogFileName, "C:\\testprocslow.log");

static void RunL()
	{
	User::After(1000000); //Microseconds = 1s

	const TInt KMaxCommandLength = 256;
	TBuf<KMaxCommandLength> commandLine;
	if(User::CommandLineLength() > commandLine.MaxLength())
		{
		User::Leave(KErrTooBig);
		}
	User::CommandLine(commandLine);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFileWriteStream ws;
	User::LeaveIfError(ws.Replace(fs, KLogFileName, EFileShareExclusive|EFileWrite));
	ws.PushL();
	ws.WriteL(commandLine);
	ws.CommitL();
	CleanupStack::PopAndDestroy(2);// fs, ws

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
