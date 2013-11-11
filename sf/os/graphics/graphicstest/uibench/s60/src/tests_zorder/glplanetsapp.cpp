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


#include "glplanetsapp.h"

#include <e32debug.h>
#include <eikenv.h>
#include <eikstart.h>


_LIT( KParentQueueName, "zorder");


void CGLPlanetsAppUi::ConstructL()
    {
	// Create app framework
	CEikAppUi::BaseConstructL(ENoAppResourceFile);
	iAppView = new(ELeave) CGLPlanetsAppView(*this);
	iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);    
	iAppView->ActivateL();
    }

CGLPlanetsAppUi::~CGLPlanetsAppUi()
	{
	delete iGLPlanets;
	if (iAppView)
		{	
		RemoveFromStack(iAppView);
		delete iAppView;
		}
	}

/** Process command parameters */
TBool CGLPlanetsAppUi::ProcessCommandParametersL(CApaCommandLine &aCommandLine)
	{
	TPtrC backgroundColor = aCommandLine.ExecutableName();
	iGLPlanets = CGLPlanets::NewL(iAppView->Window(), backgroundColor, this, KParentQueueName());
	iGLPlanets->Start();
	return CEikAppUi::ProcessCommandParametersL(aCommandLine);
	}

void CGLPlanetsAppUi::Terminate()
	{
	PrepareToExit();
	CActiveScheduler::Stop();
	}

void CGLPlanetsAppUi::BringToFront()
	{
	RWindow window = iAppView->Window();
	RWsSession wsSession = iAppView->ControlEnv()->WsSession();
	wsSession.SetWindowGroupOrdinalPosition(window.WindowGroupId(), 0);
	wsSession.Flush();
	wsSession.Finish();
	}


CGLPlanetsAppView::CGLPlanetsAppView(CGLPlanetsAppUi& aAppUi): iAppUi(aAppUi)
	{
	// empty
	}

void CGLPlanetsAppView::ConstructL(const TRect& aRect)
    {
	CreateWindowL();
    SetRect(aRect);
	}

CGLPlanetsAppView::~CGLPlanetsAppView()
	{
	// empty
	}


CGLPlanetsAppDocument::CGLPlanetsAppDocument(CEikApplication& aApp)	: CEikDocument(aApp)
	{
	// empty
	}

CEikAppUi* CGLPlanetsAppDocument::CreateAppUiL()
	{
    return new(ELeave) CGLPlanetsAppUi;
	}


TUid CGLPlanetsApp::AppDllUid() const
	{
	return TUid::Uid(0x101FCABE);
	}

CApaDocument* CGLPlanetsApp::CreateDocumentL()
	{
	return new(ELeave) CGLPlanetsAppDocument(*this);
	}

//
// Exported function
//

/** Application factory function */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CGLPlanetsApp;
	}

GLDEF_C TInt E32Main()
	{	
	return EikStart::RunApplication(NewApplication);
	}
