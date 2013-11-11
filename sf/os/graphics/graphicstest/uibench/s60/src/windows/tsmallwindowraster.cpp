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


#include "tsmallwindowraster.h"


_LIT(KBitmapPattern, "*.mbm");


CTSmallWindowRaster::CTSmallWindowRaster(const TPoint& aStartingPoint, const TSize& aWindowSize) :
        CTWindow(aStartingPoint, aWindowSize)
	{
	// empty
	}

CTSmallWindowRaster::~CTSmallWindowRaster()
	{
	if (iGc)
	    {
	    iGc->Deactivate();
	    delete iGc;
	    }
	delete iSourceBitmap;
	delete iScreenDev;
	}

CTWindow* CTSmallWindowRaster::NewL(RWsSession &aWs, 
		const RWindowTreeNode &aParent, 
		const TPoint& aStartingPoint,
		const TSize& aWindowSize)
	{
	CTSmallWindowRaster* self = new (ELeave) CTSmallWindowRaster(aStartingPoint, aWindowSize);
	CleanupStack::PushL(self);
	self->ConstructL(aWs, aParent);
	CleanupStack::Pop(self);
	return self;
	}

void CTSmallWindowRaster::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
	{
	CTWindow::ConstructL(aWs, aParent);
	iScreenDev = new (ELeave) CWsScreenDevice(aWs);
	User::LeaveIfError(iScreenDev->Construct());
	iGc = new (ELeave)CWindowGc(iScreenDev);
	User::LeaveIfError(iGc->Construct());
	iGc->Activate(iWindow);
	}

void CTSmallWindowRaster::LoadL(CSurfaceUtility* /*aUtility*/, TPtrC aFileName)
	{
	iLoaded = ETrue;
	if (aFileName.Match(KBitmapPattern()) != KErrNotFound) // it's a Bitmap
        {
        iSourceBitmap = new(ELeave) CFbsBitmap();
        User::LeaveIfError(iSourceBitmap->Load(aFileName, 0));
        }
	else
	    {
	    iSourceBitmap = CreateBitmapFromFileL(aFileName);
	    }
	iSourceRect = iSourceBitmap->SizeInPixels();	
	iDestRect = TRect(Size());
	}

void CTSmallWindowRaster::DrawBitmap(TRect& aSourceRect)
    {
    iWindow.BeginRedraw();
    iGc->DrawBitmap(iDestRect, iSourceBitmap, aSourceRect);
    iWindow.EndRedraw();
    }

void CTSmallWindowRaster::RenderL()
	{
	CTWindow::RenderL();
	if (iLoaded)
	    {
	    DrawBitmap(iSourceRect);
	    }
	}
