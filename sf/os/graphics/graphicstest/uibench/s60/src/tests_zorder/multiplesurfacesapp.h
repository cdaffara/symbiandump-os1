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

#ifndef __MULTIPLESURFACESAPP_H__
#define __MULTIPLESURFACESAPP_H__


#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <coecntrl.h>


_LIT(KParentQueueName, "zorder");

class CMultipleSurfacesAppUi;
class CMultipleSurfaces;
class CEGLRendering;


class CMultipleSurfacesAppView : public CCoeControl
    {
    friend class CMultipleSurfacesAppUi;
    
public:
	CMultipleSurfacesAppView(CMultipleSurfacesAppUi& aAppUi);
	virtual ~CMultipleSurfacesAppView();
    void ConstructL(const TRect& aRect);
    
private:	
	CMultipleSurfacesAppUi& iAppUi;
	};


class CMultipleSurfacesAppUi : public CEikAppUi
    {
public:
    virtual ~CMultipleSurfacesAppUi();
    void ConstructL();	
	void Terminate();
	void BringToFront();
	
private:
	// From CEikAppUi
	virtual TBool ProcessCommandParametersL(CApaCommandLine &aCommandLine);

private:
    CMultipleSurfacesAppView* iAppView;
    CMultipleSurfaces* iMultipleSurfaces;
    };


class CMultipleSurfacesAppDocument : public CEikDocument
	{
public:
	CMultipleSurfacesAppDocument(CEikApplication& aApp);
	
private:
	 // from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


class CMultipleSurfacesApp : public CEikApplication
	{
private:
	inline virtual TFileName ResourceFileName() const;
	
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

inline TFileName CMultipleSurfacesApp::ResourceFileName() const
    {
    return KNullDesC();
    }

#endif

