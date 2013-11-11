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
// Class for mapping between twips and device-specific units (pixels).
// 
//


#ifndef __DEVICEMAP_H__
#define __DEVICEMAP_H__

#include <e32std.h>
#include <e32base.h>

class MWsScreenDevice;

class CGraphicsDeviceMap : public CBase
	{
public:
	static CGraphicsDeviceMap* NewL(const MWsScreenDevice& aScreenDevice);
	
	TPoint TwipsToPixels(const TPoint& aTwipPoint) const;
	TRect TwipsToPixels(const TRect& aTwipRect) const;
	TPoint PixelsToTwips(const TPoint& aPixelPoint) const;
	TRect PixelsToTwips(const TRect& aPixelRect) const;
	/** Converts a horizontal dimension from twips to pixels.
		
	@param aTwips A horizontal dimension of a device in twips. 
	@return A horizontal dimension of a device in pixels. */
	TInt HorizontalTwipsToPixels(TInt aTwips) const;

	/** Converts a vertical dimension from twips to pixels.
	
	@param aTwips A vertical dimension of a device in twips. 
	@return A vertical dimension of a device in pixels. */
	TInt VerticalTwipsToPixels(TInt aTwips) const;

	/** Converts a horizontal dimension from pixels to twips.
	
	@param aPixels A horizontal dimension of a device in pixels. 
	@return A horizontal dimension of a device in twips. */
	TInt HorizontalPixelsToTwips(TInt aPixels) const;

	/** Converts a vertical dimension from pixels to twips.
		
	@param aPixels A vertical dimension of a device in pixels. 
	@return A vertical dimension of a device in twips. */
	TInt VerticalPixelsToTwips(TInt aPixels) const;
private:
	CGraphicsDeviceMap(const MWsScreenDevice& aScreenDevice);
private:
	const MWsScreenDevice* iScreenDevice;
	};
#endif
