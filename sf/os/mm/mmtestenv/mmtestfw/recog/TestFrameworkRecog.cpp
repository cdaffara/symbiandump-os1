// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <fbs.h>
#include "TestFrameworkRecog.h"
#include "TestFrameworkMain.h"

#ifdef __WINS__
#include <windows.h>	// for ExitProcess
#endif // __WINS__

GLREF_C void StartupL();
const TInt KThreadStackSize=0x2000;			//  8KB
const TInt KThreadInitHeapSize=0x1000;		//  4KB
const TInt KThreadMaxHeapSize=0x1000000;	// 16MB
const TInt KThreadStartupDelay=30000000;	// 30 seconds
const TInt KMaxLineLength=256;				// max length of config file line
_LIT(KLitConfigFileName, "C:\\MM\\AutorunTests.cfg");

//

CTestFrameworkRecognizer::CTestFrameworkRecognizer()
	: CApaDataRecognizerType(KUidTestFrameworkRecognizer, CApaDataRecognizerType::ENormal)
	{
	}

CTestFrameworkRecognizer::~CTestFrameworkRecognizer()
	{
	delete iTestActive;
	}

//
// CApaDataRecognizerType stuff...
TUint CTestFrameworkRecognizer::PreferredBufSize()
	{
	return 0;
	}

TDataType CTestFrameworkRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType();
	}

void CTestFrameworkRecognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& /*aBuffer*/)
	{
	}

//
// Entry point of recognizer
EXPORT_C CApaDataRecognizerType* CreateRecognizer()
	{
	CTestFrameworkRecognizer* self = new CTestFrameworkRecognizer();
	TRAPD(err, self->DoCreateL());
	return self;
	}

void CTestFrameworkRecognizer::DoCreateL()
	{
	// Open the config file
	LoadConfigFileL(KLitConfigFileName);

	// If the RUN_SCRIPT command is present in the config file, run each test in a separate thread
	if (iRunScript)
		{
		// Create active object waiting on thread death
		iTestActive = new(ELeave) CTestFrameworkRecogActive(iTestScriptArray);	// Takes ownership of iTestScriptArray

		// Create the first test thread
		iTestActive->CreateNextTestThread();
		}
	}

void CTestFrameworkRecognizer::LoadConfigFileL(const TDesC& aFileName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TEntry entry;
	User::LeaveIfError(fs.Entry(aFileName, entry));

	RFile file;
	User::LeaveIfError(file.Open(fs, aFileName, EFileRead));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));
	TUint8* fileData = (TUint8*)User::AllocLC(size);
	TPtr8 ptr(fileData, 0, size);
	User::LeaveIfError(file.Read(ptr));

	iTestScriptArray = new(ELeave) CTestScriptArray(4);

	// Process the config file
	TLex8 lex(ptr);
	while (!lex.Eos())
		{
		// skip any spaces
		while (lex.Peek() == ' ')
			lex.Inc();
		// mark the start of the line
		lex.Mark();
		// move to the next
		while (!lex.Eos() && lex.Peek() != '\n')
			lex.Inc();
		// step over \n
		if (lex.Peek() == '\n' )
			lex.Inc();

		// Process line
		TPtrC8 linePtr = lex.MarkedToken();
		ProcessLineL(linePtr);
		}
	CleanupStack::PopAndDestroy(fileData);
	CleanupStack::PopAndDestroy(2);	// file, fs
	}

void CTestFrameworkRecognizer::ProcessLineL(const TDesC8& aLine)
	{
	ASSERT(aLine.Length() <= KMaxLineLength);
	TBuf<KMaxLineLength> buf;
	buf.Copy(aLine);
	if (buf.Find(_L("//"))==0)
		{
		// ignore comments
		}
	else
		{
		// Get the script path and startup delay
		TLex lex(buf);
		if (!iRunScript)
			{
			if (lex.NextToken().Compare(_L("RUN_SCRIPT")) == 0)
				iRunScript = ETrue;
			}
		else
			{
			// Parse the parameters
			TTestScriptInfo info;
			info.iScriptPath = lex.NextToken();
			info.iThreadStartupDelay = 0;

			TPtrC token(lex.NextToken());
			while (token.Length())
				{
				if (token[0] == '-')
					{
					info.iParams.Append(token);
					info.iParams.Append(' ');
					}
				else
					{
					// Assume this to be the startup delay
					TLex tokenLex(token);
					User::LeaveIfError(tokenLex.Val(info.iThreadStartupDelay));
					if (info.iThreadStartupDelay < 0)
						info.iThreadStartupDelay = 0;
					}
				token.Set(lex.NextToken());
				}

			// Add the script info
			if (info.iScriptPath.Length())
				iTestScriptArray->AppendL(info);
			}
		}
	}

//
// CTestFrameworkRecogActive
CTestFrameworkRecogActive::CTestFrameworkRecogActive(CTestScriptArray* aTestScriptArray)
	: CActive(EPriorityStandard), iTestScriptArray(aTestScriptArray)
	{
	CActiveScheduler::Add(this);
	iCurrentScript = -1;
	}

CTestFrameworkRecogActive::~CTestFrameworkRecogActive()
	{
	delete iTestScriptArray;
	}

TInt CTestFrameworkRecogActive::CreateNextTestThread()
	{
	// Create the next test in a separate thread
	iCurrentScript++;
	RThread thread;
	TBuf<16> threadName;
	threadName.Format(_L("TFR_THREAD_%d"), iCurrentScript);

	TInt err = thread.Create(threadName, &StartTestThreadFn, KThreadStackSize,
							 &User::Heap(), this);

	if (err == KErrNone)
		{
		thread.Logon(iStatus);
		thread.Resume();
		SetActive();
		}
	return err;
	}

void CTestFrameworkRecogActive::DoCancel()
	{
	}

void CTestFrameworkRecogActive::RunL()
	{
	// This will run when the thread created in CreateNextTestThreadL dies
	//
	TInt err = KErrNone;
	if (iCurrentScript < (iTestScriptArray->Count() - 1))
		{
		err = CreateNextTestThread();
		}
	else
		{
		// Tests finished
		delete iTestScriptArray;
		iTestScriptArray = NULL;
#ifdef __WINS__
		// Cause the emulator to exit
		ExitProcess(0);
#endif // __WINS__
		}
	}

TInt CTestFrameworkRecogActive::StartTestThreadFn(TAny* aPtr)
	{
	CTestFrameworkRecogActive* self = static_cast<CTestFrameworkRecogActive*>(aPtr);
	TRAPD(err, self->DoStartTestThreadL());
	return err;
	}

void CTestFrameworkRecogActive::DoStartTestThreadL()
	{
	// Create the thread and wait until it's finished
	RThread thread;
	TBuf<16> threadName;
	threadName.Format(_L("TESTFRMRECOG_%d"), iCurrentScript);

	TInt err = thread.Create(threadName, &ThreadFunc, KThreadStackSize,
							 &User::Heap(), this);

	if (err == KErrNone)
		{
		RSemaphore sem;
		err = sem.CreateGlobal(KRecogSemaphoreName, 0);
		if (err == KErrAlreadyExists)
			err = sem.OpenGlobal(KRecogSemaphoreName);
		if (err == KErrNone)
			{
			// Start the thread and wait for it to signal us that it's finished
			thread.Resume();
			sem.Wait();
			}
		}
	User::LeaveIfError(err);
	}


TInt CTestFrameworkRecogActive::ThreadFunc(TAny* aPtr)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	CTestFrameworkRecogActive* self = static_cast<CTestFrameworkRecogActive*>(aPtr);
	TRAPD(err, self->DoThreadFuncL());
	delete cleanup; // destroy clean-up stack
	return err;
	}

void CTestFrameworkRecogActive::DoThreadFuncL()
	{
	// Run the test script, using filename held in iScriptPath
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

// Hurricane emulator only - start all services which we require to run tests.
// Future enhancement :- add these startups to TestUtils
#if defined(__WINS__)
	#ifndef EXCLUDE_FOR_UNITTEST
	FbsStartup();
	#endif // EXCLUDE_FOR_UNITTEST
#endif

	// Get the current script
	const TTestScriptInfo& script = iTestScriptArray->At(iCurrentScript);

	// Delay for several seconds to allow vital bits of the emulator to start up (window server bits)
	User::After(script.iThreadStartupDelay);

	// Format the parameter to be passed to the test framework
	TFileName args;
	if (script.iParams.Length())
		{
		// Add the params
		args.Append(script.iParams);
		args.Append(' ');
		}
	args.Append(script.iScriptPath);

	// Run the script
	CTestFrameworkMain* tester = CTestFrameworkMain::NewLC();
	tester->StartTestingL(args);

	CleanupStack::PopAndDestroy(2, scheduler);	// tester, scheduler
	}


