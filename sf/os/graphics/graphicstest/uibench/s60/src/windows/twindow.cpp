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


#include "twindow.h"

#include <hal.h>
#include <ImageConversion.h>


/**
Gets the size of the hardware display in pixels
@return TSize object containing the screen size
*/
TSize CTWindow::GetDisplaySizeInPixels()
    {
    static TSize displaySize;
    static TBool first = ETrue;    
    if (first)
        {
        RWsSession ws;
        ws.Connect();
        CWsScreenDevice* screen = new(ELeave) CWsScreenDevice(ws);
        screen->Construct();        
        TPixelsAndRotation pixelsAndRotation;
        screen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
        delete screen;
        ws.Close();
        displaySize = pixelsAndRotation.iPixelSize;
        first = EFalse;
        }
    /*
    // that doesn't work for S60 emulator, it returns wrong values
    TInt x;
    HAL::Get(HALData::EDisplayXPixels, x);  // Get number x pixel of screen
    TInt y;
    HAL::Get(HALData::EDisplayYPixels, y);  // Get number y pixel of screen
    return TSize(x,y);
    */
    return displaySize;
    }

CFbsBitmap* CTWindow::CreateBitmapFromFileL(const TDesC& aFileName)
    {
    RFs fs;    
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    CImageDecoder* decoder = CImageDecoder::FileNewL(fs, aFileName, CImageDecoder::EOptionAlwaysThread);
    CleanupStack::PushL(decoder);

    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    const TFrameInfo& info = decoder->FrameInfo();
    User::LeaveIfError(bitmap->Create(info.iOverallSizeInPixels, info.iFrameDisplayMode));
    TRequestStatus status;
    decoder->Convert(&status, *bitmap);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());

    CleanupStack::Pop(bitmap);
    CleanupStack::PopAndDestroy(decoder);
    CleanupStack::PopAndDestroy(&fs);
    return bitmap;
    }

CTWindow::CTWindow(const TPoint& aStartingPoint, const TSize& aWindowSize) : 
        iLoaded(EFalse), iSize(aWindowSize), iPosition(aStartingPoint) 
    {
    iScreenSize = GetDisplaySizeInPixels();
    }

void CTWindow::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
	{
	// Create a window with the window group as a parent
	iWindow = RWindow(aWs);
	// it's common to use the address of the owning object as an unique window handle 
	User::LeaveIfError(iWindow.Construct(aParent, reinterpret_cast<TUint32>(this)));
	iWindow.SetPosition(iPosition);
	iWindow.SetSize(iSize);
		
	// Activate window and set it invisible
	iWindow.Activate();
	iWindow.SetVisible(EFalse);	
	}

CTWindow::~CTWindow()
    {
    iDirtyRegion.Close();
    iWindow.Close();
    }

void CTWindow::LoadL(CSurfaceUtility* /*aUtility*/, TPtrC /*aFileName*/)
	{
	// empty
	}

void CTWindow::RenderL()
	{	
	iWindow.SetVisible(ETrue);	
	iDirtyRegion.Clear();
    iDirtyRegion.AddRect(TRect(TPoint(0, 0), iScreenSize));
    iDirtyRegion.ClipRect(TRect(iPosition, iSize));
	}

void CTWindow::Move(TInt aX, TInt aY)
    {
    iPosition.iX += aX;
    iPosition.iY += aY;
    iWindow.SetPosition(iPosition);
    }

TPoint CTWindow::CurrentPosition()
    {
    return iPosition;
    }

void CTWindow::SetPosition(TInt aX, TInt aY)
    {
    iPosition.SetXY(aX, aY);
    }

void CTWindow::SetVisible(TBool aIsVisible)
    {
    iWindow.SetVisible(aIsVisible);
    }

const RRegion& CTWindow::DirtyRegion() const
    {
    return iDirtyRegion;
    }

TInt CTWindow::SetBackgroundSurface(const TSurfaceId& aSurface)
    {
    return iWindow.SetBackgroundSurface(aSurface);
    }

TSize CTWindow::Size()
    {
    return iWindow.Size();
    }
