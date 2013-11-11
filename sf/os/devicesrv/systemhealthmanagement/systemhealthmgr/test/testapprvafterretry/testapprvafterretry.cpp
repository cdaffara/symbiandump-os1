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
#include "testapprvafterretry.h"
#include "testapps.h"


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
	const TUid dll = {KTestAppRvAfterRetryUid};
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
 In this case however, we do some jiggery-pokery in order to invoke the StartSafe retry mechanism.
 ie we return EFalse on each of several invocations but don't do the Rendezvous, then 
 eventually ETrue, so the Framework Rendezvous for us. our final invocation therefore being successful.
 
 The count file is written from the test-step, and 
 if successful, deleted here.
 
 The code is nearly all parenthesised within a TRAP harness in order to preserve the integrity 
 of this function, which is called by the framework.

*/
TBool CTestAppUi::FrameworkCallsRendezvous() const
	{
	
	TBool frameworkRendezvous = EFalse;

	TRAPD( err,
		{			
		RFs fs;
		RFileReadStream readStream;
		RFileWriteStream writeStream;
						
		CleanupClosePushL( fs );
		CleanupClosePushL( readStream );
		CleanupClosePushL( writeStream );
		
			
		if( KErrNone != fs.Connect()
		 || KErrNone != readStream.Open(fs, KDontRvCountFile, EFileRead) )
			{
			User::Leave( KErrCouldNotConnect );	
			}
		

		TInt failCount = readStream.ReadInt8L();
		readStream.Close();	
		

		if( 0 == failCount )
			{
			fs.Delete( KDontRvCountFile );
			
			// We've decided to be good this time. Let the framework do it for us.
			frameworkRendezvous = ETrue;
			}
		else
			{
			// StartSafe will timeout while waiting for a Rendezvous()
			User::LeaveIfError( writeStream.Open(fs, KDontRvCountFile, EFileWrite) );
	
			writeStream.WriteInt8L( --failCount );
			writeStream.CommitL();
			writeStream.Close();
			}
			
		CleanupStack::PopAndDestroy( 3, &fs );
		} );
		
		
	// In the case of error, returning Efalse will mean that StartSafe 
	// times out. The test will therefore fail.
	return ( KErrNone == err ) ? frameworkRendezvous : EFalse;
	}
