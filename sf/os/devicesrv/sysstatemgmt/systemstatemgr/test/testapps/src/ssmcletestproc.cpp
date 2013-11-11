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
// usage: ssmtestprocwritetime.exe <output filename> [<timeout (seconds)> [<succeed on run number>]]    *
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
#include "ssmcletestapp.h"
#include "ssmtestapps.h"
#include "ssmdebug.h"


static void RunL()
	{
	TSsmCleTestAppArgs args;
	XSsmCleTestApp::GetCommandLineArgsL(args);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	
	TInt count = XSsmCleTestApp::GetRunCountL(fs, args.iLogPrefix);
	if (0 == count)
		{
		XSsmCleTestApp::WriteStartTimeL(fs, args.iLogPrefix);
						
		}
	TInt newCount = XSsmCleTestApp::IncrementRunCountL(fs, args.iLogPrefix);
		

	if ((args.iSucceedOnRun == 0)  && (args.iWaitTime > 0))
		{
		User::After(args.iWaitTime * 1000); 
						
		}
	
	if ((args.iSucceedOnRun != 0) && (args.iSucceedOnRun > count) && (args.iWaitTime > 0))
		{
		User::After(args.iWaitTime * 1000); 
		}

	XSsmCleTestApp::WriteResultL(fs, args.iLogPrefix,args.iFailHow);

	switch (args.iFailHow) //Other failure modes don't require special action
		{
		case EDontFail:
			{
			if ((args.iSucceedOnRun == 0) || ((args.iSucceedOnRun > 0) && (count >= args.iSucceedOnRun)))
				{
				RProcess::Rendezvous(KErrNone);
				}
			else
				{
				RProcess::Rendezvous(KErrBadRendezvousTest);
				}
			break;
			}
		case EPanic:
			{
			User::Panic(_L("SSMTEST"), KSsmCleTestPanic);
			break;
			}
		case EBadRendezvous:
			{
			TBuf<KTestCommandLineMaxLength> prefix = KTestAppLogFileLoc();
			XSsmCleTestApp::WriteResultL(fs, prefix, args.iFailHow);
			RProcess::Rendezvous(KErrBadRendezvousTest);
			break;
			}
		case ENoRendezvous:
		case EMultipleTimeout:
			{
			User::After(10000000); // 10 seconds
			break;
			}
		}
	//Signalling the start of the application
	RSemaphore sem;
	TInt err = sem.OpenGlobal(KStartProcSignalSemaphore);
	RDebug::Print(_L("KStartProcSignalSemaphore Opened with %d"), err);
		
	if(err == KErrNone)
		{
		sem.Signal();
		sem.Close();
		}
	CleanupStack::PopAndDestroy(&fs);
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

	//This process will return to the user and thus be terminated without cleaning up properly
	//To avoid this we give a wait for request so that we can explicitly clean it up
	TRequestStatus status = KRequestPending;
	User::WaitForRequest(status);
	return r;
	}
