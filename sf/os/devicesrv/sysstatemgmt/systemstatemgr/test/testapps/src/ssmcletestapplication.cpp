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

#include <eikstart.h> 
#include <eikapp.h>
#include "ssmcletestapplication.h"
#include "ssmcletestapp.h"
#include "ssmtestapps.h"

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
	CleanupStack::PopAndDestroy(&fs);
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
				RProcess::Rendezvous(KErrGeneral);
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
			RProcess::Rendezvous(KErrGeneral);
			break;
			}
		case ENoRendezvous:
		case EMultipleTimeout:
			{
			User::After(10000000); // 10 seconds
			break;
			}
		}
	
	RSemaphore sem;
	TInt err = sem.OpenGlobal(KStartAppSignalSemaphore);
	RDebug::Print(_L("KStartAppSignalSemaphore Opened with %d"), err);
	
	if(err == KErrNone)
		{
		sem.Signal();
		sem.Close();
		}
	}

/**
Standard DLL entry point function.
Creates and returns an instance of the CApaApplication-derived class.
@return an instance of the CApaApplication-derived class
*/
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

	if (KErrNone != r)
		{
		User::Panic(_L("CLETEST"),r);
		}
	__UHEAP_MARKEND;
	
	//This process will return to the user and thus be terminated without cleaning up properly
	//To avoid this we give a wait for request so that we can explicitly clean it up
	TRequestStatus status = KRequestPending;
	User::WaitForRequest(status);
	return EikStart::RunApplication( CTestApplication::NewApplication);
	}
	
CTestApplication::CTestApplication()
	{
	}

CTestApplication::~CTestApplication()
	{
	}
		
/**
@return The application's UID 
*/
TUid CTestApplication::AppDllUid() const
	{
	const TUid dll = {KSsmCleTestApplicationUid};
	return dll;
	}

/**
@return CTestApplication or NULL if KErrNoMemory
*/
CApaApplication* CTestApplication::NewApplication()
	{
	// As the framework has at this point not started up enough, and therefore the TRAP-harness and 
	// exception handlers aren’t available yet, this factory function is a non-leaving function and 
	// can't use the new(Eleave) operator.
	return new CTestApplication();
	}
	
/**
Called by the UI framework at application start-up to create an instance of the document class.
@leave	KErrNoMemory
@return A CTestDocument
*/
CApaDocument* CTestApplication::CreateDocumentL()
	{
	return CTestDocument::NewL(*this);
	}

CTestDocument::CTestDocument(CEikApplication& aApp) : CEikDocument(aApp)
	{
	}

CTestDocument::~CTestDocument()
	{
	}

/**
Factory function for this class
@return a new CEndTaskTestDocument instance.
*/
CTestDocument* CTestDocument::NewL(CEikApplication& aApp)
	{
	return new(ELeave) CTestDocument(aApp);
	}


/**
Called by the UI framework to construct the application UI class. 
Note that the app UI's ConstructL() is called by the UI framework.
*/
CEikAppUi* CTestDocument::CreateAppUiL()
	{
	return new(ELeave) CTestAppUi();
	}

CTestAppUi::CTestAppUi()
	{
	}

CTestAppUi::~CTestAppUi()
	{
	}

void CTestAppUi::ConstructL()
	{
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);
	}

