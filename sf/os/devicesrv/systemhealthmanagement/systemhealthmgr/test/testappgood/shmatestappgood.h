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

#ifndef __SHMATESTAPPGOOD_H
#define __SHMATESTAPPGOOD_H

#include <e32property.h>
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

// This class is used for observing the RProperty which is set by the test case.  After getting the
// notification for changed property the application would be killed.
class CPropertyObserver : public CActive
    {
public:
    CPropertyObserver();
    ~CPropertyObserver();
    
    void StartL();

protected:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
private:
    RProperty iProperty;   
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

private:
    CPropertyObserver* iPropertyObs;
	};

#endif // __SHMATESTAPPGOOD_H
