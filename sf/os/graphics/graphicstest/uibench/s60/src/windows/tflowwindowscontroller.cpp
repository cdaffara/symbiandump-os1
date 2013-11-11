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

#include "tflowwindowscontroller.h"
#include "surfaceutility.h"


const TInt KDefaultFlowRate = -1; // by default window is moved by one pixel to the left
//const TInt KScreenMode = 1; // Default Screen Mode (used by rotate screen)


CTFlowWindowsController* CTFlowWindowsController::NewL (TBool aPreload, RArray<TPtrC>& aFileNames,
        TSize& aWindowSize, RArray<pTWindowCreatorFunction>& aNewLs,
        RArray<TPoint>& aInitialPositions, TBool aBouncing, TBool aMoveHorizontal /*= ETrue*/)
    {
    CTFlowWindowsController* self = new (ELeave) CTFlowWindowsController(aFileNames.Count(), 
            aPreload, aWindowSize, aBouncing, aMoveHorizontal);
    CleanupStack::PushL(self);        
    self->ConstructL(aFileNames, aNewLs, aInitialPositions);
    CleanupStack::Pop(self);
    return self;
    }

CTFlowWindowsController::CTFlowWindowsController(TInt aNumWindows, TBool aPreload, TSize& aWindowSize, 
        TBool aBouncing, TBool aMoveHorizontal) :
        iWindowSize (aWindowSize), iPreload (aPreload), iNumWindows (aNumWindows), iBouncing(aBouncing)
    {
    iScreenRect = TRect(CTWindow::GetDisplaySizeInPixels());
    // if screen is heigher than wide movement is vertical otherwise horizontal
    if (aMoveHorizontal)
        {
        iDeltaX = KDefaultFlowRate;
        iDeltaY = 0;
        }
    else
        {
        iDeltaX = 0;
        iDeltaY = KDefaultFlowRate;
        }
    }

void CTFlowWindowsController::ConstructL(RArray<TPtrC>& aFileNames,
        RArray<pTWindowCreatorFunction>& aNewLs, RArray<TPoint>& aInitialPositions)
    {
    iFileNames = aFileNames;    
    // Connect to windows server session
    User::LeaveIfError(iWs.Connect());    
    iScreenDev = new (ELeave) CWsScreenDevice(iWs);
    User::LeaveIfError(iScreenDev->Construct());
    iUtility = CSurfaceUtility::NewL();    

    iWinGroup = RWindowGroup(iWs);
    // it's common to use the address of the owning object as an unique window group handle 
    User::LeaveIfError(iWinGroup.Construct(reinterpret_cast<TUint32>(this)));
    
    CTWindow* flowWindow;
    for (TInt w = 0; w < iNumWindows; w++)
        {
        flowWindow = aNewLs[w](iWs, iWinGroup, aInitialPositions[w], iWindowSize);
        iWindowsRegion.AddRect(TRect(aInitialPositions[w], iWindowSize));
        CleanupStack::PushL(flowWindow);        
        // always preload the first window regardless of iPreload value
        if (w == 0 || iPreload)
            {            
            flowWindow->LoadL(iUtility, iFileNames[w]);
            }
        iFlowWindows.AppendL(flowWindow);
        CleanupStack::Pop(flowWindow);        
        }
    }

CTFlowWindowsController::~CTFlowWindowsController()
    {
    // windows needs to be deleted seperately
    for (TInt i = 0; i < iNumWindows; i++)
        {
        delete iFlowWindows[i];
        }
    iFlowWindows.Close();
    iWindowsRegion.Close();
	iWinGroup.Close();
	delete iUtility;
	delete iScreenDev;
	iWs.Close();
    }

/**
Move all Windows and draw them to the screen with the window server.

*/
void CTFlowWindowsController::MoveL()
	{
	// Prepare to move block of small windows one step in the right direction.
    // First check to see if moving them in the current direction would put them off
    // the screen.  If so, change the sign of the offset to make the block move in 
    // the other direction.
    if (iBouncing)
        {
        iWindowsRegion.Offset(iDeltaX, iDeltaY);
        if (!iWindowsRegion.IsContainedBy(iScreenRect))
            {
            iDeltaX = -iDeltaX;
            iDeltaY = -iDeltaY;
            iWindowsRegion.Offset(2*iDeltaX, 2*iDeltaY);
            }
        }
	    
	for (TInt w = iNumWindows - 1; w >= 0; --w)
		{
		// make sure that a window has loaded it's content before it moves into the screen rectangle
		if (!iPreload)
            {
            if ((iFlowWindows[w]->CurrentPosition().iX == iWindowSize.iWidth - 1)
                    || (iFlowWindows[w]->CurrentPosition().iY == iWindowSize.iHeight - 1))
                {
                iFlowWindows[w]->LoadL(iUtility, iFileNames[w]);
                }
            }
		iFlowWindows[w]->Move(iDeltaX, iDeltaY);
		iFlowWindows[w]->RenderL();
        if (!iBouncing)
            {
            if (iFlowWindows[w]->CurrentPosition().iX < -iWindowSize.iWidth * (iNumWindows - 1))
                {
                iFlowWindows[w]->SetPosition(iScreenRect.Width() - 1, 0);
                }
            if (iFlowWindows[w]->CurrentPosition().iY < -iWindowSize.iHeight * (iNumWindows - 1))
                {
                iFlowWindows[w]->SetPosition(0, iScreenRect.Height() - 1);
                }
            }
		}
	iWs.Flush();
	iWs.Finish();
	}

void CTFlowWindowsController::SetWindowsVisibility(TBool aVisible)
    {
	for (TInt w = iNumWindows -1; w >= 0; --w)
		{
		iFlowWindows[w]->SetVisible(aVisible);
        }
	iWs.Flush();
	iWs.Finish();
    }
    
/**
Rotate the screen to a given orientation. Note that if the screen device 
and window server session used to display the image aren't used for the 
rotation then the image will disappear.
*/    
void CTFlowWindowsController::Rotate(CFbsBitGc::TGraphicsOrientation aOrientation)
    {
    // Set new screen mode. The mode is defined in wsini_rotate_test.ini.
    //iScreenDev->SetAppScreenMode(KScreenMode);
	//iScreenDev->SetScreenMode(KScreenMode);
    
    // Set the screen orientation.
	// todo: check which mode to use
	iScreenDev->SetCurrentRotations(iScreenDev->CurrentScreenMode(), aOrientation);
	iWs.Flush();
	iWs.Finish();
    }
