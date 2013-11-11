// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: This app is used to define the P & S keys required for activating the reference plugins.
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <eikstart.h> 
#include <eikapp.h>
#include <e32property.h>
#include "definetestps.h"
#include "ti_adaptationplugin_stepbase.h"


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
	const TUid dll = {0x200315B2};
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
	//This appplication is used to define the property for using dummy plugins. Uid3 of the application should be 
	//same as the SSM's Secure Id. 
	
	
	//Define the property for using Dummy plugins.
	TInt result = KErrNone;
	result = RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt);
	if ((KErrNone != result) && (KErrAlreadyExists !=result))
        {
        User::Leave(result);            
        }
	
	
	result = RProperty::Define(KPropertyCategory, KStateAdaptationPluginPropertyKey, RProperty::EInt);
	if ((KErrNone != result) && (KErrAlreadyExists !=result))
        {
        User::Leave(result);            
        }	
	
    result = RProperty::Define(KPropertyCategory, KRtcAdaptationPluginPropertyKey, RProperty::EInt);
	if ((KErrNone != result) && (KErrAlreadyExists !=result))
        {
        User::Leave(result);            
        }    

    result = RProperty::Define(KPropertyCategory, KSimPluginPropertyKey, RProperty::EInt);
	if ((KErrNone != result) && (KErrAlreadyExists !=result))
        {
        User::Leave(result);            
        }   

    result = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
	if ((KErrNone != result) && (KErrAlreadyExists !=result))
        {
        User::Leave(result);            
        }
	
	RProcess::Rendezvous(KErrNone);
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);
	}
