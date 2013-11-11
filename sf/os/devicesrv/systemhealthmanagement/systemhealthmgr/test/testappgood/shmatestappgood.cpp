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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <eikstart.h> 
#include <eikapp.h>
#include <f32file.h>
#include <sysmonclisess.h>

#include "shmatestappgood.h"
#include "testapps.h"
#include "testprocgoodsession.h"

/**
Standard DLL entry point function.
Creates and returns an instance of the CApaApplication-derived class.
@return an instance of the CApaApplication-derived class
*/
TInt E32Main()
	{
	return EikStart::RunApplication( CTestApplication::NewApplication );
	} //lint -e714 Suppress 'not referenced'

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
	const TUid dll = {KTestAppGoodUid};
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
	delete iPropertyObs;
	}

void CTestAppUi::ConstructL()
	{
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);

	iPropertyObs = new(ELeave) CPropertyObserver;
	iPropertyObs->StartL();

    //Signal that application is started
    RSemaphore sem;
    TInt err = sem.OpenGlobal(KStartShmaTestAppGood);
    if(err == KErrNone)
        {
        sem.Signal();
        sem.Close();
        }

	RFs fs;
	err = fs.Connect();
	User::LeaveIfError(err);
	CleanupClosePushL(fs);
	
	RFile file;
	err = file.Open(fs, KPanicFile, EFileRead);
	if (err == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(&fs);
		// We are not using the file, so just return
		return;
		}
	else
		{
		User::LeaveIfError(err);
		CleanupClosePushL(file);
		
		TBuf8<30> buf;
		err = file.Read(buf);
		User::LeaveIfError(err);

		CleanupStack::PopAndDestroy(&file);
		err = fs.Delete(KPanicFile);

		RBuf8 writableArgs;
		writableArgs.CreateL(buf, 30);
		CleanupClosePushL(writableArgs);
		TPtr16 args16 = writableArgs.Expand();
	
		_LIT(KTestGoodProcPanic, "Panic");
	 	if (args16 == KPanicCommandLineOption1)
	 		User::Panic(KTestGoodProcPanic, KProcPanic);
	 	else if (args16 == KPanicCommandLineOption2)
			User::Panic(KTestGoodProcPanic, KErrNone);
	 	
	 	CleanupStack::PopAndDestroy(2, &fs);
		}
	}

CPropertyObserver::CPropertyObserver() :CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

CPropertyObserver::~CPropertyObserver()
    {
    Cancel();
    iProperty.Close();
    }

void CPropertyObserver::StartL()
    {
    const TInt err = iProperty.Attach(KCancelMonitorProperty, KCancelMonitorKey);
    RDebug::Print(_L("RProperty Attach completed with %d"), err);
	User::LeaveIfError(err);
    iProperty.Subscribe(iStatus);
    SetActive();
    }

void CPropertyObserver::RunL()
    {
    RSysMonSession sess;
    //Opens a session with System monitor in order to cancel the monitoring
    sess.OpenL();
	CleanupClosePushL(sess);
    sess.CancelMonitorSelfL();
   	CleanupStack::PopAndDestroy(&sess);

    //Signal that application will be closed
    RSemaphore sem;
    TInt err = sem.OpenGlobal(KStartShmaTestAppGood);
    if(err == KErrNone)
        {
        sem.Signal();
        sem.Close();
        }

    //Kill the current process
    User::Exit(KErrNone);
    }

void CPropertyObserver::DoCancel()
    {
    iProperty.Cancel();
    }

TInt CPropertyObserver::RunError(TInt aError)
    {
	RDebug::Print(_L("RunL leave with %d"), aError);
    return KErrNone;
    }



