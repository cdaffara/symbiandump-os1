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
 @internalComponent
*/

#include "stresslet.h"
#include "panic.h"

//
// CStresslet::CRedrawHandler
//

CStresslet::CRedrawHandler* CStresslet::CRedrawHandler::NewL(CStresslet& aStresslet)
	{
	CRedrawHandler * self = new (ELeave) CRedrawHandler(aStresslet);
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop ();	
	return self;
	}

CStresslet::CRedrawHandler::~CRedrawHandler ()
	{
	Cancel ();
	}

CStresslet::CRedrawHandler::CRedrawHandler(CStresslet& aStresslet) :
	CActive(EPriorityNormal), iStresslet(aStresslet)
	{
	}

void CStresslet::CRedrawHandler::ConstructL ()
	{
	CActiveScheduler::Add (this);
	iStresslet.Session().RedrawReady (&iStatus);
	SetActive ();
	}

void CStresslet::CRedrawHandler::RunL ()
	{
	iStresslet.Session().GetRedraw (iRedrawEvent);

	if(iRedrawEvent.Handle () != 0)
		{
		iStresslet.HandleRedraw (iRedrawEvent);
		}

	iStresslet.Session().RedrawReady (&iStatus); 
	SetActive ();								 
	}

void CStresslet::CRedrawHandler::DoCancel ()
	{
	iStresslet.Session().RedrawReadyCancel ();
	}

//
// CStresslet::CEventHandler
//

CStresslet::CEventHandler* CStresslet::CEventHandler::NewL (CStresslet& aStresslet)
	{
	CEventHandler * self = new (ELeave) CEventHandler(aStresslet);
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (); 
	return self;
	}

CStresslet::CEventHandler::~CEventHandler ()
	{
	Cancel ();
	}

CStresslet::CEventHandler::CEventHandler (CStresslet& aStresslet) :
	CActive(EPriorityNormal), iStresslet(aStresslet)
	{
	}

void CStresslet::CEventHandler::ConstructL ()
	{
	CActiveScheduler::Add (this);
	iStresslet.Session().EventReady (&iStatus);
	SetActive ();
	}

void CStresslet::CEventHandler::RunL ()
	{
	iStresslet.Session().GetEvent (iEvent);

	iStresslet.HandleEvent (iEvent);
	
	iStresslet.Session().EventReady (&iStatus);		
	SetActive ();									
	
	}

void CStresslet::CEventHandler::DoCancel ()
	{
	iStresslet.Session().EventReadyCancel ();
	}

//
// CStresslet::CStresslet
//

CStresslet::CStresslet (MTestStepReporter& aReporter) :
	iReporter(aReporter)
	{
	}

MTestStepReporter& CStresslet::Reporter ()
	{
	return iReporter;
	}

void CStresslet::ConcludeNow (void)
	{
	CActiveScheduler::Stop ();
	}

void CStresslet::LaunchL (CStresslet* aStresslet)
	{
	CStresslet* obj = aStresslet;

	CActiveScheduler* runner = new (ELeave) CActiveScheduler;
	CleanupStack::PushL (runner);
	CActiveScheduler::Install (runner);

	obj->ConstructL();
	
	CActiveScheduler::Start (); //runner

	delete obj;

	CleanupStack::PopAndDestroy (runner);
	}

void CStresslet::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iRedrawHandler = CRedrawHandler::NewL (*this);
	iEventHandler = CEventHandler::NewL (*this);
	iScreenDevice = new (ELeave)CWsScreenDevice(iWs);
	User::LeaveIfError(iScreenDevice->Construct ());
	User::LeaveIfError(iScreenDevice->CreateContext (iGc));
	StartL ();
	}

CStresslet::~CStresslet ()
	{
	delete iRedrawHandler;
	delete iEventHandler;
	iWs.Close ();
	}

RWsSession& CStresslet::Session ()
	{
	return iWs;
	}

CWindowGc& CStresslet::WindowGc()
	{
	__ASSERT_ALWAYS(iGc, Panic(EPanic1));
	return *iGc;
	}
