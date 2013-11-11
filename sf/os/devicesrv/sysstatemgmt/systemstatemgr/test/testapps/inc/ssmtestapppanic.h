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

#ifndef __SSMTESTAPPPANIC_H__
#define __SSMTESTAPPPANIC_H__


const TInt KSsTestAppPanicNumber = -130258;

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
	// From CCoeAppUi
	virtual TBool FrameworkCallsRendezvous() const;
	};

#endif // __SSMTESTAPPPANIC_H__
