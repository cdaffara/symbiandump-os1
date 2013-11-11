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

#include "tflowwindow.h"
#include "surfaceutility.h"


CTWindow* CTFlowWindow::NewL(RWsSession &aWs, const RWindowTreeNode &aParent, const TPoint& aStartingPoint, const TSize& aWindowSize)
    {
    CTFlowWindow * self = new (ELeave) CTFlowWindow(aStartingPoint, aWindowSize);
    CleanupStack::PushL(self);
    self->ConstructL(aWs, aParent);
    CleanupStack::Pop(self);
    return self;
    }

CTFlowWindow::CTFlowWindow(const TPoint& aStartingPoint, const TSize& aWindowSize) : CTWindow(aStartingPoint, aWindowSize)
    {
    iImageInfo.iSizeInPixels = aWindowSize;
    iImageInfo.iPixelFormat = EUidPixelFormatXRGB_8888; //todo: is this format okay (EUidPixelFormatARGB_8888)?
    iImageInfo.iCpuAccess = ESgCpuAccessNone;
    iImageInfo.iUsage = ESgUsageOpenGlesTarget| ESgUsageDirectGdiSource | ESgUsageDirectGdiTarget;
    iImageInfo.iShareable = ETrue;
    iImageInfo.iScreenId = KSgScreenIdMain;
    }

void CTFlowWindow::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
	{
	CTWindow::ConstructL(aWs, aParent);
	
	User::LeaveIfError(iImageCollection.Create(iImageInfo, 1)); // creates an image collection with 1 image
	User::LeaveIfError(iImageCollection.OpenImage(0, iImage)); // opens a handle to the image
	// sets the image collection as background surface for the window
	User::LeaveIfError(SetBackgroundSurface(iImageCollection.SurfaceId()));
	// Connect to surface update session
	User::LeaveIfError(iUpdateSession.Connect());
	}

CTFlowWindow::~CTFlowWindow()
    {
    iUpdateSession.Close();
    iImage.Close();
    iImageCollection.Close();
    }

/**
 * Scales a bitmap from file to a surface.
 * 
 * @param aUtility Performs the loading of the bitmap and scaling to the surface.
 * @param aFileName Location of the bitmap.
 */
void CTFlowWindow::LoadL(CSurfaceUtility* aUtility, TPtrC aFileName)
	{
	aUtility->ScaleBitmapFromFileToSurfaceL(aFileName, iImageCollection.SurfaceId());
	}

void CTFlowWindow::RenderL()
    {
    CTWindow::RenderL();
    TRequestStatus status;
    iUpdateSession.NotifyWhenAvailable(status);
    User::LeaveIfError(iUpdateSession.SubmitUpdate(KAllScreens, 
            iImageCollection.SurfaceId(), 0, &DirtyRegion()));
    User::WaitForRequest(status);
    }
