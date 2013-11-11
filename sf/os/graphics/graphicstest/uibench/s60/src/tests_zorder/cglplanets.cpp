 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cglplanets.h"
#include "glplanetsapp.h"

const TInt KTimeBetweenFrames = 100000; // 100 ms

const TInt KBringToFront = 1;
const TInt KTerminate = 2;


CGLPlanets* CGLPlanets::NewL(RWindow& aWindow, TPtrC aBackgroundColor, CGLPlanetsAppUi* aApplication, TPtrC aParentQueueName)
    {
    CGLPlanets* self = new (ELeave)CGLPlanets(aWindow, aApplication);
    self->ConstructL(aBackgroundColor, aParentQueueName);
    return self;
    }

CGLPlanets::CGLPlanets(RWindow& aWindow, CGLPlanetsAppUi* aApplication) : iWindow(aWindow),
        iApplication(aApplication)
	{
	// empty
	}

CGLPlanets::~CGLPlanets()
	{
	delete iTimer;
	delete iEglEnvironment;
	delete iModel;
	iParentMsgQueue.Send(KTerminate);
	iParentMsgQueue.Close();
	iMsgQueue.Close();
	}



void CGLPlanets::ConstructL(TPtrC aBackgroundColor, TPtrC aParentQueueName)
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityIdle);
	iEglEnvironment = CEglEnvironment::NewL(iWindow);
	iModel = CModel::NewL(iEglEnvironment->Display(), iEglEnvironment->Surface(), aBackgroundColor);
	User::LeaveIfError(iMsgQueue.OpenGlobal(aBackgroundColor));
	User::LeaveIfError(iParentMsgQueue.OpenGlobal(aParentQueueName));
	// inform test app that the message queues are ready
	RProcess::Rendezvous(KErrNone);
	}

void CGLPlanets::Start()
	{
	// redraw the screen periodically
	iTimer->Start(0, KTimeBetweenFrames, TCallBack(TimerCallBack, this));
	}

TInt CGLPlanets::TimerCallBack(TAny* aGLPlanet)
	{
	static_cast<CGLPlanets*>(aGLPlanet)->Render();
	return KErrNone;
	}

void CGLPlanets::Render()
	{
	TInt msg = 0;
	iMsgQueue.ReceiveBlocking(msg); // Receive info from the test
	iModel->DrawToBuffer(iTime++);
	iEglEnvironment->DrawToWindow();
	switch (msg)
		{
		case KBringToFront:
			{
			TUint32 then = User::FastCounter();
			iApplication->BringToFront();
			TUint32 now = User::FastCounter();
			TUint32 time = (now - then);
			iParentMsgQueue.Send(time);
			break;
			}
		case KTerminate:
			Stop();
			iApplication->Terminate();
			break;
		}
	}

void CGLPlanets::Stop()
	{
	if(iTimer)
		{
		iTimer->Cancel();
		}	
	}
