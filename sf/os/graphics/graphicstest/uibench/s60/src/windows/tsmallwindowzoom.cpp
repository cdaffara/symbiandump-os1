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

#include "tsmallwindowzoom.h"


const TRect KMinimumZoomRect(TSize(20, 20));

CTSmallWindowZoom::CTSmallWindowZoom(const TPoint& aStartingPoint, const TSize& aWindowSize):
        CTSmallWindowRaster(aStartingPoint, aWindowSize)
	{
	// empty
	}

CTSmallWindowZoom::~CTSmallWindowZoom()
	{
	// empty
	}

CTWindow* CTSmallWindowZoom::NewL(RWsSession &aWs, 
		const RWindowTreeNode &aParent, 
		const TPoint& aStartingPoint,
		const TSize& aWindowSize)
	{
	CTSmallWindowZoom* self = new (ELeave)CTSmallWindowZoom(aStartingPoint, aWindowSize);
	CleanupStack::PushL(self);
    self->ConstructL(aWs, aParent);
    CleanupStack::Pop(self);
	return self;
	}

void CTSmallWindowZoom::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
	{
	CTSmallWindowRaster::ConstructL(aWs, aParent);
	iDelta = 1;
	iZoom = EZoomIn;
	}

void CTSmallWindowZoom::LoadL(CSurfaceUtility* aUtility, TPtrC aFileName)
	{
	CTSmallWindowRaster::LoadL(aUtility, aFileName);
    iZoomRect = iSourceRect;
	}

void CTSmallWindowZoom::RenderL()
	{
	CTWindow::RenderL();
	if (iLoaded)
	    {
	    DrawBitmap(iZoomRect);
	    if (iZoom == EZoomIn)
            {
            iZoomRect.Shrink(iDelta, iDelta);
            if (iZoomRect.Width() <= KMinimumZoomRect.Width())
                {
                iZoom = EZoomOut;
                }
            }
        else 
            {
            iZoomRect.Grow(iDelta, iDelta);
            if (iZoomRect.Width() >= iSourceRect.Width())
                {
                iZoom = EZoomIn;
                }
            }
	    }
        
    
	}
