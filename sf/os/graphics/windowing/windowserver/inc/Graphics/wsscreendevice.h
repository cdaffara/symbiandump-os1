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
// Interface for Render Stage Screen Abstraction
// 
//

/**
 @publishedPartner
 @prototype
*/

#ifndef WSSCREENDEVICE_H
#define WSSCREENDEVICE_H

#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>

/** Render Stage Palette support.

This interface is intended to be optionally provided by a Render Stage
when the pixel target can provide a Colour Palette.  Not all pixel targets have
this feature, for example OpenVG only targets.
@publishedPartner
@prototype
*/
class MWsPalette : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsPalette)
	virtual void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const = 0;
	virtual TInt GetPalette(CPalette*& aPalette) const = 0;
	virtual TInt SetCustomPalette(const CPalette* aPalette) = 0;
	};


/** Render Stage Screen Device abstraction.

Render Stages provide this interface as an abstraction of the target screen.
The screen may not be pixel based, in which case read back of pixel data may
not be a reflection of what appears on the screen.  The screen might also only
be partially readable, for example external video surfaces may be blanked out
when a read back is done.  The intention of the CopyScreenToBitmap methods is
to provide a means to obtain a snapshot of the entire screen for architectures
where a composition of the screen can be done once followed by a read back
of the entire screen, as opposed to servicing a sequence of GetScanLine calls
which may be prohibitively expensive.

@publishedPartner
@prototype
*/
class MWsScreenDevice : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsScreenDevice)

	virtual TInt ScreenNumber() const = 0;
	virtual TDisplayMode DisplayMode() const = 0;
	virtual TSize SizeInPixels() const = 0;
	virtual TSize SizeInTwips() const = 0;
	virtual TRect PointerRect() const = 0;
	virtual void GetPixel(TRgb& aColor,const TPoint& aPixel) const = 0;
	virtual void GetScanLine(TDes8& aScanLine,const TPoint& aStartPixel,TInt aPixelLength, TDisplayMode aDispMode) const = 0;
	virtual TBool RectCompare(const TRect& aRect1, const TRect& aRect2) const = 0;
	virtual void CopyScreenToBitmapL(CFbsBitmap* aBitmap, const TRect& aRect) const = 0;
	virtual void ClearDisplay(TRgb aBackColor)=0;
	};

#endif // WSSCREENDEVICE_H
