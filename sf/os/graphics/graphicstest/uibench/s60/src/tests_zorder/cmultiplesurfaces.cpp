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


#include "cmultiplesurfaces.h"
#include "tsmallwindowraster.h"


const TInt KTimeBetweenFrames = 100000; // checks every 100 ms for a new message

// messages which are sent by the test step
const TInt KBringToFront = 1;
const TInt KTerminate = 2;

// defines number of windows on the screen
const TInt KWindowsAcross = 1;
const TInt KWindowsDown = 1;

_LIT(KSmallBitmap,"z:\\resource\\apps\\cover2.jpg"); // bitmap that is displayed by the windows


CMultipleSurfaces* CMultipleSurfaces::NewL(RWsSession& aWs, RWindowGroup& aWindowGroup, TPtrC childQueueName, CMultipleSurfacesAppUi* aMultipleSurfacesAppUi, TPtrC aParentQueueName)
    {
    CMultipleSurfaces* self = new (ELeave)CMultipleSurfaces(aWs, aWindowGroup, aMultipleSurfacesAppUi);
    self->ConstructL(childQueueName, aParentQueueName);
    return self;
    }

CMultipleSurfaces::CMultipleSurfaces(RWsSession& aWs, RWindowGroup& aWindowGroup, CMultipleSurfacesAppUi* aMultipleSurfacesAppUi) :
        iWs(aWs), iWindowGroup(aWindowGroup), iMultipleSurfacesAppUi(aMultipleSurfacesAppUi)
	{
	// empty
	}

CMultipleSurfaces::~CMultipleSurfaces()
    {
    delete iTimer;
    iParentMsgQueue.Send(KTerminate);
    iParentMsgQueue.Close();
    iMsgQueue.Close();
    for (TInt w = 0; w < iSmallWindows.Count(); w++)
        {
        delete iSmallWindows[w];
        }
    iSmallWindows.Close();
    }

void CMultipleSurfaces::ConstructL(TPtrC childQueueName, TPtrC aParentQueueName)
    {
    iTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    // available screensize is reduced by the menu bar
    TSize screenSize = iMultipleSurfacesAppUi->ClientRect().Size();
    
    TSize windowSize;
    windowSize.iHeight = screenSize.iHeight/KWindowsDown;
    windowSize.iWidth = screenSize.iWidth/KWindowsAcross;
    // adjust window size to make square windows
    if (windowSize.iWidth > windowSize.iHeight)
        {
        windowSize.iWidth = windowSize.iHeight;
        }
    else
        {
        windowSize.iHeight = windowSize.iWidth;
        }
    
    // available screensize is reduced by the menu bar
    TPoint initialPosition(iMultipleSurfacesAppUi->ClientRect().iTl);
    CSurfaceUtility* utility = CSurfaceUtility::NewL();
    CleanupStack::PushL(utility);
    CTWindow* smallWindow;
    for (TInt i = 0; i < KWindowsAcross; i++)
        {
        initialPosition.iX = 0;
        for (TInt j = 0; j < KWindowsDown; j++)
            {
            smallWindow = CTSmallWindowRaster::NewL(iWs, iWindowGroup, initialPosition, windowSize);
            CleanupStack::PushL(smallWindow);
            smallWindow->LoadL(utility, KSmallBitmap());
            iSmallWindows.AppendL(smallWindow);
            CleanupStack::Pop(smallWindow);
            initialPosition.iX += windowSize.iWidth;
            }
        initialPosition.iY += windowSize.iHeight;
        }
    CleanupStack::PopAndDestroy(utility);
    User::LeaveIfError(iMsgQueue.OpenGlobal(childQueueName));
    User::LeaveIfError(iParentMsgQueue.OpenGlobal(aParentQueueName));
    // inform test app that the message queues are ready
    RProcess::Rendezvous(KErrNone);
    }



/**
 * Starts a timer which redraws the screen periodically.
 */
void CMultipleSurfaces::Start()
	{
	iTimer->Start(0, KTimeBetweenFrames, TCallBack(TimerCallBack, this));
	}

TInt CMultipleSurfaces::TimerCallBack(TAny* MultipleSurfaces)
	{
	TRAPD(err, static_cast<CMultipleSurfaces*>(MultipleSurfaces)->RenderL());
	return err;
	}

/**
 * Meassures the time it takes to bring this application to the foreground.
 * It receives a message from the test step which coordinates the switching.
 */
void CMultipleSurfaces::RenderL()
	{
	TInt msg = 0;
    iMsgQueue.ReceiveBlocking(msg);
    
    for (TInt w = 0; w < iSmallWindows.Count(); w++)
        {
        iSmallWindows[w]->Move(0, 1);
        iSmallWindows[w]->RenderL();
        }
    iWs.Flush();
    iWs.Finish();	    
    
    switch (msg)
        {
        case KBringToFront:
            {
            TUint32 timeStamp = User::FastCounter();
            iMultipleSurfacesAppUi->BringToFront();
            TUint32 time = User::FastCounter() - timeStamp;
            iParentMsgQueue.Send(time);
            break;
            }
        case KTerminate:
            {
            Stop();
            iMultipleSurfacesAppUi->Terminate();
            break;
            }
        }    
	}

/**
 * Stops the timer.
 */
void CMultipleSurfaces::Stop()
	{
	if(iTimer)
		{
		iTimer->Cancel();
		}	
	}
