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
// defineemergencycallps.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <eikstart.h> 
#include <eikapp.h>
#include <e32property.h>
#include "definesecuritystateps.h"

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
	const TUid dll = {0x2000E664};
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
	//This appplication is used to define the property used by CSsmUiSpecific::SecurityStatus() and CSsmUiSpecific::SetSecurityStatus. Uid3 of the 
	//application should be same as the security status property category uid. This is to allow the application to define the property as
	//security state property uid falls under system range.

	const TUid KSecurityStatusPropertyCategory =  {0x2000E664};
	const TUint KSecurityStatusPropertyKey = 0x0101; 
	const TUint32 KStarterSecurityPhase = 0x00000001;
	//Define the property for emergency call
	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	TInt result = RProperty::Define(KSecurityStatusPropertyCategory, KSecurityStatusPropertyKey, RProperty::EInt,
									KAllowAllPolicy, KAllowAllPolicy);

	_LIT(KPRINT,"Return code for RProperty::Define for security state property key - %d -");
	RDebug::Print(KPRINT, result);
	
	if( KErrNone != result && KErrAlreadyExists != result)
		{
		User::Leave(result);
		}
	
	//Define the property for security call
	result = RProperty::Define(KSecurityStatusPropertyCategory, KStarterSecurityPhase, RProperty::EInt,
									KAllowAllPolicy, KAllowAllPolicy);

	RDebug::Print(KPRINT, result);
	
	if( KErrNone != result && KErrAlreadyExists != result)
		{
		User::Leave(result);
		}
	RProcess::Rendezvous(KErrNone);
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);
	}
