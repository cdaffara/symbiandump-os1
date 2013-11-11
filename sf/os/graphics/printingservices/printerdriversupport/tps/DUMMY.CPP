// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "E32Std.h"
#include "E32Cons.h"

#include "dummy.h"

CDummyDevice::CDummyDevice()
	{
	}

EXPORT_C CDummyDevice* CDummyDevice::NewL()
	{
	CDummyDevice* device=new(ELeave) CDummyDevice;
	device->iConsole=(CConsoleBase*)NewConsole();
	return device;
	}

EXPORT_C CDummyDevice::~CDummyDevice()
	{
	delete iConsole;
	}

EXPORT_C TInt CDummyDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return aTwips;
	}

EXPORT_C TInt CDummyDevice::VerticalTwipsToPixels(TInt aTwips) const
	{
	return aTwips;
	}

EXPORT_C TInt CDummyDevice::HorizontalPixelsToTwips(TInt aPixels) const
	{
	return aPixels;
	}

EXPORT_C TInt CDummyDevice::VerticalPixelsToTwips(TInt aPixels) const
	{
	return aPixels;
	}

EXPORT_C TInt CDummyDevice::GetNearestFontInTwips(CFont*& /*aFont*/,const TFontSpec& /*aFontSpec*/)
	{
	return KErrNone;
	}

EXPORT_C TInt CDummyDevice::GetNearestFontToDesignHeightInTwips(CFont*& /*aFont*/,const TFontSpec& /*aFontSpec*/)
	{
	return KErrNone;
	}

EXPORT_C void CDummyDevice::ReleaseFont(CFont* /*aFont*/)
	{
	}

EXPORT_C TDisplayMode CDummyDevice::DisplayMode() const
	{
	return EGray2;
	}

EXPORT_C TSize CDummyDevice::SizeInPixels() const
	{
	return TSize(1000,1000);
	}

EXPORT_C TSize CDummyDevice::SizeInTwips() const
	{
	return TSize(1000,1000);
	}

EXPORT_C TInt CDummyDevice::CreateContext(CGraphicsContext*& aGC)
	{
	TInt ret;
	CDummyGc* gc = new CDummyGc(this);
	if (!gc)
		ret=KErrNoMemory;
	else
		{
		aGC=gc;
		ret=KErrNone;
		} 
	return ret;
	}

EXPORT_C TInt CDummyDevice::NumTypefaces() const
	{
	return 0;
	}

EXPORT_C void CDummyDevice::TypefaceSupport(TTypefaceSupport& /*aTypefaceSupport*/,TInt /*aTypefaceIndex*/) const
	{
	}

EXPORT_C TInt CDummyDevice::FontHeightInTwips(TInt /*aTypefaceIndex*/,TInt /*aHeightIndex*/) const
	{
	return 0;
	}

EXPORT_C void CDummyDevice::PaletteAttributes(TBool& /*aModifiable*/,TInt& /*aNumEntries*/) const
	{
	}

EXPORT_C void CDummyDevice::SetPalette(CPalette* /*aPalette*/)
	{
	}

EXPORT_C TInt CDummyDevice::GetPalette(CPalette*& /*aPalette*/) const
	{
	return 0;
	}
	 
EXPORT_C CDummyGc::CDummyGc(CDummyDevice* aDevice):
	iDevice(aDevice)
	{
	}

EXPORT_C CDummyGc::~CDummyGc()
	{
	iDevice->iConsole->Getch();
	}

EXPORT_C CGraphicsDevice* CDummyGc::Device() const
	{
	return iDevice;
	}

EXPORT_C void CDummyGc::SetOrigin(const TPoint& /*aPos*/)
	{
	iDevice->iConsole->Printf(_L("SetOrigin\n"));
	}

EXPORT_C void CDummyGc::SetDrawMode(TDrawMode /*aDrawingMode*/)
	{
	iDevice->iConsole->Printf(_L("SetDrawMode\n"));
	}

EXPORT_C void CDummyGc::SetClippingRect(const TRect& /*aRect*/)
	{
	iDevice->iConsole->Printf(_L("SetClippingRect\n"));
	}

EXPORT_C void CDummyGc::CancelClippingRect()
	{
	iDevice->iConsole->Printf(_L("CancelClippingRect\n"));
	}

EXPORT_C void CDummyGc::Reset()
	{
	iDevice->iConsole->Printf(_L("Reset\n"));
	}


EXPORT_C void CDummyGc::UseFont(const CFont* /*aFont*/)
	{
	iDevice->iConsole->Printf(_L("UseFont\n"));
	}

EXPORT_C void CDummyGc::DiscardFont()
	{
	iDevice->iConsole->Printf(_L("DiscardFont\n"));
	}

EXPORT_C void CDummyGc::SetUnderlineStyle(TFontUnderline /*aUnderlineStyle*/)
	{
	iDevice->iConsole->Printf(_L("SetUnderlineStyle\n"));
	}

EXPORT_C void CDummyGc::SetStrikethroughStyle(TFontStrikethrough /*aStrikethroughStyle*/)
	{
	iDevice->iConsole->Printf(_L("SetStrikethroughStyle\n"));
	}

EXPORT_C void CDummyGc::SetWordJustification(TInt /*aExcessWidth*/,TInt /*aNumGaps*/)
	{
	iDevice->iConsole->Printf(_L("SetWordJustification\n"));
	}

EXPORT_C void CDummyGc::SetCharJustification(TInt /*aExcessWidth*/,TInt /*aNumChars*/)
	{
	iDevice->iConsole->Printf(_L("SetCharJustification\n"));
	}


EXPORT_C void CDummyGc::SetPenColor(const TRgb& /*aColor*/)
	{
	iDevice->iConsole->Printf(_L("SetPenColor\n"));
	}

EXPORT_C void CDummyGc::SetPenStyle(TPenStyle /*aPenStyle*/)
	{
	iDevice->iConsole->Printf(_L("SetPenStyle\n"));
	}

EXPORT_C void CDummyGc::SetPenSize(const TSize& /*aSize*/)
	{
	iDevice->iConsole->Printf(_L("SetPenSize\n"));
	}


EXPORT_C void CDummyGc::SetBrushColor(const TRgb& /*aColor*/)
	{
	iDevice->iConsole->Printf(_L("SetBrushColor\n"));
	}

EXPORT_C void CDummyGc::SetBrushStyle(TBrushStyle /*aBrushStyle*/)
	{
	iDevice->iConsole->Printf(_L("SetBrushStyle\n"));
	}

EXPORT_C void CDummyGc::SetBrushOrigin(const TPoint& /*aOrigin*/)
	{
	iDevice->iConsole->Printf(_L("SetBrushOrigin\n"));
	}

EXPORT_C void CDummyGc::UseBrushPattern(const CFbsBitmap* /*aBitmap*/)
	{
	iDevice->iConsole->Printf(_L("SetBrushPattern\n"));
	}

EXPORT_C void CDummyGc::DiscardBrushPattern()
	{
	iDevice->iConsole->Printf(_L("DiscardBrushPattern\n"));
	}

EXPORT_C void CDummyGc::MoveTo(const TPoint& /*aPoint*/)
	{
	iDevice->iConsole->Printf(_L("MoveTo\n"));
	}

EXPORT_C void CDummyGc::MoveBy(const TPoint& /*aVector*/)
	{
	iDevice->iConsole->Printf(_L("MoveBy\n"));
	}

EXPORT_C void CDummyGc::Plot(const TPoint& /*aPoint*/)
	{
	iDevice->iConsole->Printf(_L("Plot\n"));
	}

EXPORT_C void CDummyGc::DrawArc(const TRect& /*aRect*/,const TPoint& /*aStart*/,const TPoint& /*aEnd*/)
	{
	iDevice->iConsole->Printf(_L("DrawArc\n"));
	}

EXPORT_C void CDummyGc::DrawLine(const TPoint& /*aPoint1*/,const TPoint& /*aPoint2*/)
	{
	iDevice->iConsole->Printf(_L("DrawLine\n"));
	}

EXPORT_C void CDummyGc::DrawLineTo(const TPoint& /*aPoint*/)
	{
	iDevice->iConsole->Printf(_L("DrawLineTo\n"));
	}

EXPORT_C void CDummyGc::DrawLineBy(const TPoint& /*aVector*/)
	{
	iDevice->iConsole->Printf(_L("DrawLineBy\n"));
	}

EXPORT_C void CDummyGc::DrawPolyLine(const CArrayFix<TPoint>* /*aPointList*/)
	{
	iDevice->iConsole->Printf(_L("DrawPolyLine\n"));
	}

EXPORT_C void CDummyGc::DrawPolyLine(const TPoint* /*aPointList*/,TInt /*aNumPoints*/)
	{
	iDevice->iConsole->Printf(_L("DrawPolyLine\n"));
	}

EXPORT_C void CDummyGc::DrawPie(const TRect& /*aRect*/,const TPoint& /*aStart*/,const TPoint& /*aEnd*/)
	{
	iDevice->iConsole->Printf(_L("DrawPie\n"));
	}

EXPORT_C void CDummyGc::DrawEllipse(const TRect& /*aRect*/)
	{
	iDevice->iConsole->Printf(_L("DrawEllipse\n"));
	}

EXPORT_C void CDummyGc::DrawRect(const TRect& /*aRect*/)
	{
	iDevice->iConsole->Printf(_L("DrawRect\n"));
	}

EXPORT_C void CDummyGc::DrawRoundRect(const TRect& /*aRect*/,const TSize& /*aCornerSize*/)
	{
	iDevice->iConsole->Printf(_L("DrawRoundRect\n"));
	}

EXPORT_C TInt CDummyGc::DrawPolygon(const CArrayFix<TPoint>* /*aPointList*/,TFillRule /*aFillRule*/)
	{
	iDevice->iConsole->Printf(_L("DrawPolygon\n"));
	return KErrNone;
	}

EXPORT_C TInt CDummyGc::DrawPolygon(const TPoint* /*aPointList*/,TInt /*aNumPoints*/,TFillRule /*aFillRule*/)
	{
	iDevice->iConsole->Printf(_L("DrawPolygon\n"));
	return KErrNone;
	}

EXPORT_C void CDummyGc::DrawBitmap(const TPoint& /*aTopLeft*/,const CFbsBitmap* /*aSource*/)
	{
	iDevice->iConsole->Printf(_L("DrawBitmap\n"));
	}

EXPORT_C void CDummyGc::DrawBitmap(const TRect& /*aDestRect*/,const CFbsBitmap* /*aSource*/)
	{
	iDevice->iConsole->Printf(_L("DrawBitmap\n"));
	}

EXPORT_C void CDummyGc::DrawBitmap(const TRect& /*aDestRect*/,const CFbsBitmap* /*aSource*/,const TRect& /*aSourceRect*/)
	{
	iDevice->iConsole->Printf(_L("DrawBitmap\n"));
	}

EXPORT_C void CDummyGc::DrawText(const TDesC& /*aString*/,const TPoint& /*aPosition*/)
	{
	iDevice->iConsole->Printf(_L("DrawText\n"));
	}

EXPORT_C void CDummyGc::DrawText(const TDesC& /*aString*/,const TRect& /*aBox*/,TInt /*aBaselineOffset*/,TTextAlign /*aHoriz*/,TInt /*aLeftMrg*/)
	{
	iDevice->iConsole->Printf(_L("DrawText\n"));
	}

EXPORT_C void CDummyGc::DrawBitmapMasked(const TRect& /*aDestRect*/,const CFbsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CFbsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{
	}

EXPORT_C void CDummyGc::DrawBitmapMasked(const TRect& /*aDestRect*/,const CWsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CWsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{
	}
