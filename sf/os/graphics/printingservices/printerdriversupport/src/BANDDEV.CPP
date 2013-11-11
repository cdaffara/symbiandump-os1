// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitdev.h>
#include <bitstd.h>
#include <banddev.h>

const TInt KBandSizeInBytes=0x8000; // 32k

EXPORT_C CBandedDevice::~CBandedDevice()
	{
	delete iClearGc;
	iClearGc=NULL;
	delete iBandBitmapDevice;
	iBandBitmapDevice=NULL;
	delete iBandBitmap;
	iBandBitmap=NULL;
	}

EXPORT_C CBandedDevice* CBandedDevice::NewL(TRect aRectInPixels,TSize aKPixelSizeInTwips,TDisplayMode aDisplayMode,TBandingDirection aBandingDirection,TInt aScanLinesPerBand)
	{
	CBandedDevice* device=new(ELeave) CBandedDevice(aRectInPixels,aBandingDirection,aScanLinesPerBand);
	CleanupStack::PushL(device);
	device->ConstructL(aDisplayMode,aKPixelSizeInTwips);
	CleanupStack::Pop();
	return device;
	}

EXPORT_C TInt CBandedDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return iBandBitmapDevice->HorizontalTwipsToPixels(aTwips);
	}

EXPORT_C TInt CBandedDevice::VerticalTwipsToPixels(TInt aTwips) const
	{
	return iBandBitmapDevice->VerticalTwipsToPixels(aTwips);
	}

EXPORT_C TInt CBandedDevice::HorizontalPixelsToTwips(TInt aPixels) const
	{
	return iBandBitmapDevice->HorizontalPixelsToTwips(aPixels);
	}

EXPORT_C TInt CBandedDevice::VerticalPixelsToTwips(TInt aPixels) const
	{
	return iBandBitmapDevice->VerticalPixelsToTwips(aPixels);
	}

/** Creates a font from those available in the printer device's 
typeface store that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function is replaced by GetNearestFontToDesignHeightInTwips()

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes.
@deprecated */
EXPORT_C TInt CBandedDevice::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Creates a font from those available in the printer device's 
typeface store that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function replaces GetNearestFontInTwips()

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes. */
EXPORT_C TInt CBandedDevice::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return iBandBitmapDevice->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** This call is defined because it had to be - it appeared as an abstract virtual in
the base class. But it should never actually get called for this class. */
EXPORT_C TInt CBandedDevice::GetNearestFontToMaxHeightInTwips(CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/, TInt /*aMaxHeight*/)
	{
	return KErrNotSupported;
	}

EXPORT_C void CBandedDevice::ReleaseFont(CFont* aFont)
	{
	iBandBitmapDevice->ReleaseFont(aFont);
	}

EXPORT_C TDisplayMode CBandedDevice::DisplayMode() const
	{
	return iBandBitmapDevice->DisplayMode();
	}

EXPORT_C TSize CBandedDevice::SizeInPixels() const
	{
	return iFullRectInPixels.Size();
	}

EXPORT_C TSize CBandedDevice::SizeInTwips() const
	{
	TSize size=SizeInPixels();
	size.iWidth=HorizontalPixelsToTwips(size.iWidth);
	size.iHeight=VerticalPixelsToTwips(size.iHeight);
	return size;
	}

EXPORT_C TInt CBandedDevice::CreateContext(CGraphicsContext*& aGC)
	{
	TInt ret=iBandBitmapDevice->CreateContext(aGC);
	if (ret==KErrNone)
		{
		TPoint origin=FullOriginToBandOrigin(TPoint(0,0));
		aGC->SetOrigin(origin);
		}
	return ret;
	}

EXPORT_C TInt CBandedDevice::NumTypefaces() const
	{
	return iBandBitmapDevice->NumTypefaces();
	}

EXPORT_C void CBandedDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const
	{
	iBandBitmapDevice->TypefaceSupport(aTypefaceSupport,aTypefaceIndex);
	}

EXPORT_C TInt CBandedDevice::FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
	{
	return iBandBitmapDevice->FontHeightInTwips(aTypefaceIndex,aHeightIndex);
	}

EXPORT_C void CBandedDevice::PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
	{
	iBandBitmapDevice->PaletteAttributes(aModifiable,aNumEntries);
	}

EXPORT_C void CBandedDevice::SetPalette(CPalette* aPalette)
	{
	iBandBitmapDevice->SetPalette(aPalette);
	}

EXPORT_C TInt CBandedDevice::GetPalette(CPalette*& aPalette) const
	{
	return iBandBitmapDevice->GetPalette(aPalette);
	}

EXPORT_C TInt CBandedDevice::NumBands() const
	{
	TInt numbands;
	if ((iBandingDirection==EBandingTopToBottom) || (iBandingDirection==EBandingBottomToTop))
		numbands = (iFullRectInPixels.Size().iHeight/iScanLinesPerBand)+1;
	else
		numbands = (iFullRectInPixels.Size().iWidth/iScanLinesPerBand)+1;
	return numbands;
	}

EXPORT_C TInt CBandedDevice::NextBand()
	{
	iBandIndex++;
	if (iBandIndex==NumBands())
		iBandIndex=0;
	iClearGc->Clear();
	return iBandIndex;
	}

EXPORT_C TRect CBandedDevice::BandRect() const
	{
	TRect bandrect;
	if (iBandingDirection==EBandingTopToBottom)
		{
		bandrect.iTl.iX=iFullRectInPixels.iTl.iX;
		bandrect.iTl.iY=iFullRectInPixels.iTl.iY+(iBandIndex*iScanLinesPerBand);
		bandrect.iBr.iX=iFullRectInPixels.iBr.iX;
		bandrect.iBr.iY=bandrect.iTl.iY+iScanLinesPerBand;
		if (bandrect.iBr.iY>iFullRectInPixels.iBr.iY)
			bandrect.iBr.iY=iFullRectInPixels.iBr.iY;
		}
	else if (iBandingDirection==EBandingBottomToTop)
		{
		bandrect.iBr.iX=iFullRectInPixels.iBr.iX;
		bandrect.iBr.iY=iFullRectInPixels.iBr.iY-(iBandIndex*iScanLinesPerBand);
		bandrect.iTl.iX=iFullRectInPixels.iTl.iX;
		bandrect.iTl.iY=bandrect.iBr.iY-iScanLinesPerBand;
		if (bandrect.iTl.iY<iFullRectInPixels.iTl.iY)
			bandrect.iTl.iY=iFullRectInPixels.iTl.iY;
		}
	else if (iBandingDirection==EBandingLeftToRight)
		{
		bandrect.iTl.iX=iFullRectInPixels.iTl.iX+(iBandIndex*iScanLinesPerBand);
		bandrect.iTl.iY=iFullRectInPixels.iTl.iY;
		bandrect.iBr.iX=bandrect.iTl.iX+iScanLinesPerBand;
		bandrect.iBr.iY=iFullRectInPixels.iBr.iY;
		if (bandrect.iBr.iX>iFullRectInPixels.iBr.iX)
			bandrect.iBr.iX=iFullRectInPixels.iBr.iX;
		}
	else if (iBandingDirection==EBandingRightToLeft)
		{
		bandrect.iBr.iX=iFullRectInPixels.iBr.iX-(iBandIndex*iScanLinesPerBand);
		bandrect.iBr.iY=iFullRectInPixels.iBr.iY;
		bandrect.iTl.iX=bandrect.iBr.iX-iScanLinesPerBand;
		bandrect.iTl.iY=iFullRectInPixels.iTl.iY;
		if (bandrect.iTl.iX<iFullRectInPixels.iTl.iX)
			bandrect.iTl.iX=iFullRectInPixels.iTl.iX;
		}
	return bandrect;
	}

EXPORT_C TPoint CBandedDevice::FullOriginToBandOrigin(const TPoint& aPoint) const
	{
	TRect rect=BandRect();
	return aPoint-rect.iTl;
	}

EXPORT_C void CBandedDevice::Reset()
	{
	iBandIndex=-1;
	}

CBandedDevice::CBandedDevice(TRect aRectInPixels,TBandingDirection aBandingDirection,TInt aScanLinesPerBand):
	iFullRectInPixels(aRectInPixels),
	iScanLinesPerBand(aScanLinesPerBand),
	iBandIndex(-1),
	iBandBitmap(NULL),
	iBandBitmapDevice(NULL),
	iBandingDirection(aBandingDirection)
	{
	}

void CBandedDevice::ConstructL(TDisplayMode aDisplayMode,TSize aKPixelSizeInTwips)
	{
	iBandBitmap=new(ELeave)	CFbsBitmap;
	if ((iBandingDirection==EBandingTopToBottom) || (iBandingDirection==EBandingBottomToTop))
		{
		TInt widthinpixels=iFullRectInPixels.Size().iWidth;
		if (!iScanLinesPerBand)
			iScanLinesPerBand=KBandSizeInBytes/(CFbsBitmap::ScanLineLength(widthinpixels,aDisplayMode));
		iScanLinesPerBand&=~1;
		User::LeaveIfError(iBandBitmap->Create(TSize(widthinpixels,iScanLinesPerBand),aDisplayMode));
		TSize sizeintwips;
		sizeintwips.iWidth=(aKPixelSizeInTwips.iWidth*widthinpixels)/1000;
		sizeintwips.iHeight=(aKPixelSizeInTwips.iHeight*iScanLinesPerBand)/1000;
		iBandBitmap->SetSizeInTwips(sizeintwips);
		}
	else
		{
		TInt heightinpixels=iFullRectInPixels.Size().iHeight;
		if (!iScanLinesPerBand)
			iScanLinesPerBand=KBandSizeInBytes/(CFbsBitmap::ScanLineLength(heightinpixels,aDisplayMode));
		iScanLinesPerBand&=~1;
		User::LeaveIfError(iBandBitmap->Create(TSize(iScanLinesPerBand,heightinpixels),aDisplayMode));
		TSize sizeintwips;
		sizeintwips.iWidth=(aKPixelSizeInTwips.iWidth*iScanLinesPerBand)/1000;
		sizeintwips.iHeight=(aKPixelSizeInTwips.iHeight*heightinpixels)/1000;
		iBandBitmap->SetSizeInTwips(sizeintwips);
		}
	iBandBitmapDevice=CFbsBitmapDevice::NewL(iBandBitmap);
	User::LeaveIfError(iBandBitmapDevice->CreateContext((CGraphicsContext*&) iClearGc));
	}

