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
// Description: This is the header file for "DefineTestPs" app. This app is used to define the P & S keys required for activating the reference plugins.
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __DEFINETESTPS_H
#define __DEFINETESTPS_H

/**
Application class
*/
#include <eikapp.h>
class CTestApplication : public CEikApplication
	{
public:
	static CApaApplication* NewApplication();
	~CTestApplication();
	
private:
	CTestApplication();
	
	// from CApaApplication
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	};

/**
Document class
*/
#include <eikdoc.h>
class CEikAppUi;
class CEikApplication;
class CTestDocument : public CEikDocument
	{
public:
	static CTestDocument* NewL(CEikApplication& aApp);
	~CTestDocument();
	
private:
	CTestDocument(CEikApplication& aApp);
	
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

/**
Application UI class, root of all graphical user interface in this application
*/
#include <eikappui.h>
class CTestAppUi : public CEikAppUi
    {
public:
	CTestAppUi();
	~CTestAppUi();
	
	// from CEikAppUi
	void ConstructL();
	};

#endif // __DEFINETESTPS_H
