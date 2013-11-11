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


#include "multiplesurfacesapp.h"
#include "cmultiplesurfaces.h"

#include <e32debug.h>
#include <eikenv.h>
#include <eikstart.h>


void CMultipleSurfacesAppUi::ConstructL()
    {
	// Create app framework
	CEikAppUi::BaseConstructL(ENoAppResourceFile);
	iAppView = new(ELeave) CMultipleSurfacesAppView(*this);
	iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);
	iAppView->ActivateL();
    }

CMultipleSurfacesAppUi::~CMultipleSurfacesAppUi()
	{
	delete iMultipleSurfaces;
	if (iAppView)
		{	
		RemoveFromStack(iAppView);
		delete iAppView;		
		}
	}

/** Process command parameters */
TBool CMultipleSurfacesAppUi::ProcessCommandParametersL(CApaCommandLine &aCommandLine)
	{
	TPtrC childQueueName = aCommandLine.ExecutableName();	
	iMultipleSurfaces = CMultipleSurfaces::NewL(CCoeEnv::Static()->WsSession(),
	        CCoeEnv::Static()->RootWin(), childQueueName, this, KParentQueueName());
	iMultipleSurfaces->Start();
	return CEikAppUi::ProcessCommandParametersL(aCommandLine);
	}

void CMultipleSurfacesAppUi::Terminate()
	{
	PrepareToExit();
	CActiveScheduler::Stop();
	}

void CMultipleSurfacesAppUi::BringToFront()
	{
	RWindow window = iAppView->Window();
	RWsSession wsSession = iAppView->ControlEnv()->WsSession();
	wsSession.SetWindowGroupOrdinalPosition(window.WindowGroupId(), 0);
	wsSession.Flush();
	wsSession.Finish();		
	}


CMultipleSurfacesAppView::CMultipleSurfacesAppView(CMultipleSurfacesAppUi& aAppUi) :
        CCoeControl(), iAppUi(aAppUi)
	{
	// empty
	}

void CMultipleSurfacesAppView::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    SetRect(aRect);    
	}

CMultipleSurfacesAppView::~CMultipleSurfacesAppView()
	{
	// empty
	}


CMultipleSurfacesAppDocument::CMultipleSurfacesAppDocument(CEikApplication& aApp) : CEikDocument(aApp)
	{
	// empty
	}

CEikAppUi* CMultipleSurfacesAppDocument::CreateAppUiL()
	{
    return new(ELeave) CMultipleSurfacesAppUi;
	}


TUid CMultipleSurfacesApp::AppDllUid() const
	{
	return TUid::Uid(0x101FCABE);
	}

CApaDocument* CMultipleSurfacesApp::CreateDocumentL()
	{
	return new(ELeave) CMultipleSurfacesAppDocument(*this);
	}

LOCAL_C CApaApplication* NewApplication()
	{
	return new CMultipleSurfacesApp;
	}

GLDEF_C TInt E32Main()
	{	
	return EikStart::RunApplication(NewApplication);
	}
