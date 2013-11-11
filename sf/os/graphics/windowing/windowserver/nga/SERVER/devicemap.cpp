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

#include "devicemap.h"
#include <graphics/wsscreendevice.h>

CGraphicsDeviceMap* CGraphicsDeviceMap::NewL(const MWsScreenDevice& aScreenDevice)
	{
	return new(ELeave) CGraphicsDeviceMap(aScreenDevice);
	}

CGraphicsDeviceMap::CGraphicsDeviceMap(const MWsScreenDevice& aScreenDevice) : iScreenDevice(&aScreenDevice)
	{
	}

TInt CGraphicsDeviceMap::HorizontalTwipsToPixels(TInt aTwips) const
	{
	TInt htp = iScreenDevice->SizeInTwips().iWidth;	//horizontal twips	
	TInt htptp = (htp * 1000)/ iScreenDevice->SizeInPixels().iWidth; //horizontal twips per thousand pixels
	return (1000 * aTwips + (htptp >> 1)) / htptp;
	}

TInt CGraphicsDeviceMap::VerticalTwipsToPixels(TInt aTwips) const
	{
	TInt vtp = iScreenDevice->SizeInTwips().iHeight;	//vertical twips	
	TInt vtptp = (vtp * 1000)/ iScreenDevice->SizeInPixels().iHeight; //vertical twips per thousand pixels
	return (1000 * aTwips + (vtptp >> 1)) / vtptp;
	}

TInt CGraphicsDeviceMap::HorizontalPixelsToTwips(TInt aPixels) const
	{
	TInt htp = iScreenDevice->SizeInTwips().iWidth;	//horizontal twips	
	TInt htptp = (htp * 1000)/ iScreenDevice->SizeInPixels().iWidth; //horizontal twips per thousand pixels
	return (htptp * aPixels + 500) / 1000;
	}

TInt CGraphicsDeviceMap::VerticalPixelsToTwips(TInt aPixels) const
	{
	TInt vtp = iScreenDevice->SizeInTwips().iHeight;	//vertical twips	
	TInt vtptp = (vtp * 1000)/ iScreenDevice->SizeInPixels().iHeight; //vertical twips per thousand pixels
	return (vtptp * aPixels + 500) / 1000;
	}

TPoint CGraphicsDeviceMap::TwipsToPixels(const TPoint& aTwipPoint) const
	{
	return TPoint(HorizontalTwipsToPixels(aTwipPoint.iX),VerticalTwipsToPixels(aTwipPoint.iY));
	}

TRect CGraphicsDeviceMap::TwipsToPixels(const TRect& aTwipRect) const
	{
	return TRect(TwipsToPixels(aTwipRect.iTl),TwipsToPixels(aTwipRect.iBr));
	}

TPoint CGraphicsDeviceMap::PixelsToTwips(const TPoint& aPixelPoint) const
	{
	return TPoint(HorizontalPixelsToTwips(aPixelPoint.iX),VerticalPixelsToTwips(aPixelPoint.iY));
	}

TRect CGraphicsDeviceMap::PixelsToTwips(const TRect& aPixelRect) const
	{
	return TRect(PixelsToTwips(aPixelRect.iTl),PixelsToTwips(aPixelRect.iBr));
	}
