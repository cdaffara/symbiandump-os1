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
#include <e32property.h>
#include <ssm/startupdomainpskeys.h>
#include "definestartupps.h"
#include "ssmuiproviderdll.h"

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
	const TUid dll = {0x2000E65E};
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
	//This appplication is used to define the property for startup. Uid3 of the application should be 
	//same as the startup property uid. This is to allow the application to define the property as
	//startup property uid falls under system range.

	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);

	TBuf<20> commandLine;
	User::CommandLine(commandLine);
	
	TInt result = KErrNone;

	_LIT(KStartUpPSKeys, "startupkeys");
	if (!(commandLine.Compare(KStartUpPSKeys)))
	    {
        //Define the property for global statup mode.
        result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KPSGlobalStartupMode, RProperty::EInt,
                                        KAllowAllPolicy, KAllowAllPolicy);

	    _LIT(KPRINT6,"Success code for RProperty::Define for global statup mode property key - %d -");
	    RDebug::Print(KPRINT6, result);
	    if ((KErrNone != result) && (KErrAlreadyExists !=result))
	        {
	        User::Leave(result);	        
	        }
   
	    //Define the property for global statup mode.
	    result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KPSStartupReason, RProperty::EInt,
	                                    KAllowAllPolicy, KAllowAllPolicy);

	    _LIT(KPRINT7,"Success code for RProperty::Define for statup reason property key - %d -");
	    RDebug::Print(KPRINT7, result);
        if ((KErrNone != result) && (KErrAlreadyExists !=result))
            {
            User::Leave(result);            
            }
	    }
	else
	    {
	    //Define the property for sim status.
	    result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), RProperty::EInt,
	                                    KAllowAllPolicy, KAllowAllPolicy);
	    
	    _LIT(KPRINT1,"Success code for RProperty::Define for sim status property key - %d -");
	    RDebug::Print(KPRINT1, result);
        if ((KErrNone != result) && (KErrAlreadyExists !=result))
            {
            User::Leave(result);            
            }
        
	    //Define the property for sim security status.
	    result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KStartupSimSecurityStatus, RProperty::EInt,
	                                    KAllowAllPolicy, KAllowAllPolicy);

	    _LIT(KPRINT2,"Success code for RProperty::Define for sim security status property key - %d -");
	    RDebug::Print(KPRINT2, result);
        if ((KErrNone != result) && (KErrAlreadyExists !=result))
            {
            User::Leave(result);            
            }

	    //Define the property for splash status.
	    result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KPSSplashShutdown, RProperty::EInt,
	                                    KAllowAllPolicy, KAllowAllPolicy);

	    _LIT(KPRINT3,"Success code for RProperty::Define for splash status property key - %d -");
	    RDebug::Print(KPRINT3, result);
        if ((KErrNone != result) && (KErrAlreadyExists !=result))
            {
            User::Leave(result);            
            }
	    //Define the property for security code status.
	    result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KStartupSecurityCodeQueryStatus, RProperty::EInt,
	                                    KAllowAllPolicy, KAllowAllPolicy);

	    _LIT(KPRINT4,"Success code for RProperty::Define for security code query status property key - %d -");
	    RDebug::Print(KPRINT4, result);
        if ((KErrNone != result) && (KErrAlreadyExists !=result))
            {
            User::Leave(result);            
            }

	    //Define the property for security code status.
	    result = RProperty::Define(CSsmUiSpecific::StartupPSUid(), KStartupSecurityCodeStatus, RProperty::EInt,
	                                    KAllowAllPolicy, KAllowAllPolicy);

	    _LIT(KPRINT5,"Success code for RProperty::Define for security code query status property key - %d -");
	    RDebug::Print(KPRINT5, result);
        if ((KErrNone != result) && (KErrAlreadyExists !=result))
            {
            User::Leave(result);            
            }
	    }
	RProcess::Rendezvous(KErrNone);
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);
	}
