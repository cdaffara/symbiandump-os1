 // Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GLPLANETSAPP_H__
#define __GLPLANETSAPP_H__


#include "cglplanets.h"

#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <coecntrl.h>


class CGLPlanetsAppUi;
class CEGLRendering;

class CGLPlanetsAppView : public CCoeControl
    {
    friend class CGLPlanetsAppUi;
    
public:
	CGLPlanetsAppView(CGLPlanetsAppUi& aAppUi);
	virtual ~CGLPlanetsAppView();
    void ConstructL(const TRect& aRect);
    
private:	
	CGLPlanetsAppUi& iAppUi;	
    };
    

class CGLPlanetsAppUi : public CEikAppUi
    {    
public:
    virtual ~CGLPlanetsAppUi();
    void ConstructL();	
	void Terminate();
	void BringToFront();
	
private:
	// From CEikAppUi
	virtual TBool ProcessCommandParametersL(CApaCommandLine &aCommandLine);

private:
    CGLPlanetsAppView* iAppView;  
    CGLPlanets*  	   iGLPlanets;
    };


class CGLPlanetsAppDocument : public CEikDocument
	{
public:
	CGLPlanetsAppDocument(CEikApplication& aApp);
	
private:
	 // from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


class CGLPlanetsApp : public CEikApplication
	{
private:
	inline virtual TFileName ResourceFileName() const;
	
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};


inline TFileName CGLPlanetsApp::ResourceFileName() const
    {
    return KNullDesC();
    }


#endif

