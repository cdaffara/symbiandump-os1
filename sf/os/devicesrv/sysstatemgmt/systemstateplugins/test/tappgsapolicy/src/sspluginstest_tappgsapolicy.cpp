// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bautils.h>
#include <centralrepository.h>
#include "sspluginstest_tappgsapolicy.h"
#include "ssmpolicypluginsprivatecrkeys.h"

//Fail to execute this exe for KDontRvBootCount times
const TInt KDontRvBootCount = 3;

/**
Standard DLL entry point function.
Creates and returns an instance of the CApaApplication-derived class.
@return an instance of the CApaApplication-derived class
*/
TInt E32Main()
	{
	return EikStart::RunApplication( CTestApplication::NewApplication );
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
	const TUid dll = {KTestAppReboots};
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

/**
 Overload of the CCoeAppUi virtual method.
 The method in the base class merely returns ETrue. Overloads would normally just return EFalse.
 In this case however, we do some jiggery-pokery in order to invoke the ssma retry mechanism.
 ie we panic for KDontRvBootCount invocations but don't do the Rendezvous, then  eventually ETrue, so the Framework 
 Rendezvous for us. our final invocation therefore being successful.
 
 This particular test case can be used to test following
 1.Transitions to fail 
 	To test above need to set KDontRvBootCount to KSsmStartupErrorResetLimit+1.
 2.Persisted number of consecutive failed device start-ups
 	To test above feature need set KDontRvBootCount to KSsmStartupErrorResetLimit
 */
void DoFrameworkCallsRendezvousL();

TBool CTestAppUi::FrameworkCallsRendezvous() const
	{	
	TRAPD(err,DoFrameworkCallsRendezvousL());
	return ( KErrNone == err ) ? ETrue : EFalse;
	}

void DoFrameworkCallsRendezvousL()
	{
	CRepository* repository = CRepository::NewLC( KCRUidSsmStartupPolicy );
	TInt Resetlimit;
	User::LeaveIfError( repository->Get( KSsmStartupErrorResetLimit, Resetlimit ) );
	CleanupStack::Pop(repository);
	if( Resetlimit < KDontRvBootCount )
		{
		_LIT(KMainErrorStatement, "*** Program error in Starting sspluginstest_tappgsapolicy.exe: %d");
		RDebug::Print(KMainErrorStatement, KErrNone);
	   	User::Panic(KMainErrorStatement,KErrNone);
		}

	}
