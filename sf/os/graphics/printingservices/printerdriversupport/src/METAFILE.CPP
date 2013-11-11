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

#include <metafile.h>
#include <bitdev.h>
#include <fbs.h>

NONSHARABLE_CLASS(CFontStack) : public CBase
	{
protected:
	CFontStack(CGraphicsDevice* aDevice);
public:
	static CFontStack* NewL(CGraphicsDevice* aDevice);
	~CFontStack();
	void AddFontL(CFont* aFont);
private:
	CGraphicsDevice* iDevice;
	CArrayPtrFlat<CFont>* iFontList;
	};

CFontStack::CFontStack(CGraphicsDevice* aDevice):
	iDevice(aDevice),
	iFontList(NULL)
	{
	__DECLARE_NAME(_S("CFontStack"));
	}

CFontStack* CFontStack::NewL(CGraphicsDevice* aDevice)
	{
	CFontStack* fontstack=new(ELeave) CFontStack(aDevice);
	CleanupStack::PushL(fontstack);
	fontstack->iFontList=new(ELeave) CArrayPtrFlat<CFont>(8);
	CleanupStack::Pop();
	return fontstack;
	}

CFontStack::~CFontStack()
	{
	if (iFontList)
		{
		TInt count=iFontList->Count();
		for (TInt i=0; i<count; i++)
			iDevice->ReleaseFont((*iFontList)[i]);
		delete iFontList;
		}
	}

void CFontStack::AddFontL(CFont* aFont)
	{
	TRAPD(ret,iFontList->AppendL(aFont));
	if (ret!=KErrNone)
		{
		iDevice->ReleaseFont(aFont);
		User::Leave(ret);
		}
	}

enum TGraphicsContextCommandCode
	{
	EUseGc,     //  Not gc command
	EEndOfStream,

	ESetOrigin,
	ESetDrawMode,
	ESetClippingRect,
	ECancelClippingRect,
	EReset,
	EUseFont,
	EDiscardFont,
	ESetUnderlineStyle,
	ESetStrikethroughStyle,
	ESetWordJustification,
	ESetCharJustification,
	ESetPenColor,
	ESetPenStyle,
	ESetPenSize,
	ESetBrushColor,
	ESetBrushStyle,
	ESetBrushOrigin,
	EUseBrushPattern,
	EDiscardBrushPattern,
	EMoveTo,
	EMoveBy,
	EPlot,
	EDrawArc,
	EDrawLine,
	EDrawLineTo,
	EDrawLineBy,
	EDrawPolyLine1, 
	EDrawPolyLine2, 
	EDrawPie,
	EDrawEllipse,
	EDrawRect,
	EDrawRoundRect,
	EDrawPolygon1,  
	EDrawPolygon2,  
	EDrawBitmap1,   
	EDrawBitmap2,   
	EDrawBitmap3,   
	EDrawText1,	   
	EDrawText2	   
	};

inline RWriteStream& operator<<(RWriteStream& aStream,TGraphicsContextCommandCode aCommandCode)
	{
	aStream.WriteUint8L(TUint8(aCommandCode));
	return aStream;
	}

EXPORT_C CMetafileDevice::CMetafileDevice(CGraphicsDevice* aDevice):
	CGraphicsDevice(),
	iGcCount(0),
	iGcIndex(-1),
	iRealDevice(aDevice),
	iWriteStream(NULL)
	{
	__DECLARE_NAME(_S("CMetafileDevice"));
	}

EXPORT_C CMetafileDevice* CMetafileDevice::NewL(CGraphicsDevice* aDevice)
	{
	CMetafileDevice* device=new(ELeave) CMetafileDevice(aDevice);	
	return device;
	}

EXPORT_C CMetafileDevice::~CMetafileDevice()
	{
	}

EXPORT_C TInt CMetafileDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return iRealDevice->HorizontalTwipsToPixels(aTwips);
	}

EXPORT_C TInt CMetafileDevice::VerticalTwipsToPixels(TInt aTwips) const
	{
	return iRealDevice->VerticalTwipsToPixels(aTwips);
	}

EXPORT_C TInt CMetafileDevice::HorizontalPixelsToTwips(TInt aPixels) const
	{
	return iRealDevice->HorizontalPixelsToTwips(aPixels);
	}

EXPORT_C TInt CMetafileDevice::VerticalPixelsToTwips(TInt aPixels) const
	{
	return iRealDevice->VerticalPixelsToTwips(aPixels);
	}

EXPORT_C TInt CMetafileDevice::GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

EXPORT_C TInt CMetafileDevice::GetNearestFontToDesignHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec)
	{
	return iRealDevice->GetNearestFontToDesignHeightInTwips(aFont,aFontSpec);
	}

EXPORT_C TInt CMetafileDevice::GetNearestFontToMaxHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return iRealDevice->GetNearestFontToMaxHeightInTwips(aFont, aFontSpec, aMaxHeight);
	}

EXPORT_C void CMetafileDevice::ReleaseFont(CFont* aFont)
	{
	iRealDevice->ReleaseFont(aFont);
	}					

EXPORT_C TDisplayMode CMetafileDevice::DisplayMode() const
	{
	return iRealDevice->DisplayMode();
	}

EXPORT_C TSize CMetafileDevice::SizeInPixels() const
	{
	return iRealDevice->SizeInPixels();
	}

EXPORT_C TSize CMetafileDevice::SizeInTwips() const
	{
	return iRealDevice->SizeInTwips();
	}

EXPORT_C TInt CMetafileDevice::CreateContext(CGraphicsContext*& aGC)
	{
	CMetafileGc* gc=new CMetafileGc(this,iGcCount);
	if (!gc)
		return KErrNoMemory;
	iGcCount++;
	aGC=gc;
	return KErrNone;
	}

EXPORT_C TInt CMetafileDevice::NumTypefaces() const
	{
	return iRealDevice->NumTypefaces();
	}

EXPORT_C void CMetafileDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const
	{
	iRealDevice->TypefaceSupport(aTypefaceSupport,aTypefaceIndex);
	}

EXPORT_C TInt CMetafileDevice::FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
	{
	return iRealDevice->FontHeightInTwips(aTypefaceIndex,aHeightIndex);
	}

EXPORT_C void CMetafileDevice::PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
	{
	iRealDevice->PaletteAttributes(aModifiable,aNumEntries);
	}

EXPORT_C void CMetafileDevice::SetPalette(CPalette* aPalette)
	{
	iRealDevice->SetPalette(aPalette);
	}

EXPORT_C TInt CMetafileDevice::GetPalette(CPalette*& aPalette) const
	{
	return iRealDevice->GetPalette(aPalette);
	}

EXPORT_C void CMetafileDevice::UseGcL(TInt aGcIndex)
	{
	if (iGcIndex!=aGcIndex)
		{
		iGcIndex=aGcIndex;
		*iWriteStream << EUseGc;
		iWriteStream->WriteInt32L(iGcIndex);
		}
	}

EXPORT_C void CMetafileDevice::StartOutputStreamL(RWriteStream& aStream)  // Returns error code
	{
	iWriteStream=&aStream;
	TSize size(HorizontalPixelsToTwips(1000),VerticalPixelsToTwips(1000));
	*iWriteStream << size;
	}

EXPORT_C void CMetafileDevice::EndOfStreamL()  // Returns error code
	{
	*iWriteStream << EEndOfStream;
	}

EXPORT_C RWriteStream& CMetafileDevice::WriteStream()
	{
	return *iWriteStream;
	}

EXPORT_C CMetafileGc::CMetafileGc(CMetafileDevice* aDevice,TInt anIndex):
	CGraphicsContext(),
	iDevice(aDevice),
	iIndex(anIndex)
	{
	}

EXPORT_C CMetafileGc::~CMetafileGc()
	{
	}

EXPORT_C CGraphicsDevice* CMetafileGc::Device() const
	{
	return iDevice;
	}

EXPORT_C void CMetafileGc::SetOrigin(const TPoint& aPos)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetOrigin;
	iDevice->WriteStream() << aPos;
	}

EXPORT_C void CMetafileGc::SetDrawMode(TDrawMode aDrawingMode)
	{
	TInt	errCode = 0;
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetDrawMode;
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aDrawingMode));
	}

EXPORT_C void CMetafileGc::SetClippingRect(const TRect& aRect)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetClippingRect;
	iDevice->WriteStream() << aRect;
	}

EXPORT_C void CMetafileGc::CancelClippingRect()
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ECancelClippingRect;
	}

EXPORT_C void CMetafileGc::Reset()
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EReset;
	}

EXPORT_C void CMetafileGc::UseFont(const CFont* aFont)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EUseFont;
	iDevice->WriteStream() << aFont->FontSpecInTwips();
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aFont->HeightInPixels()));
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aFont->BaselineOffsetInPixels()));
	}

EXPORT_C void CMetafileGc::DiscardFont()
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDiscardFont;
	}

EXPORT_C void CMetafileGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetUnderlineStyle;
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aUnderlineStyle));
	}

EXPORT_C void CMetafileGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetStrikethroughStyle;
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aStrikethroughStyle));
	}

EXPORT_C void CMetafileGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetWordJustification;
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aExcessWidth));
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aNumGaps));
	}

EXPORT_C void CMetafileGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetCharJustification;
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aExcessWidth));
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aNumChars));
	}

EXPORT_C void CMetafileGc::SetPenColor(const TRgb& aColor)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetPenColor;
	iDevice->WriteStream() << aColor;
	}

EXPORT_C void CMetafileGc::SetPenStyle(TPenStyle aPenStyle)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetPenStyle;
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aPenStyle));
	}

EXPORT_C void CMetafileGc::SetPenSize(const TSize& aSize)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetPenSize;
	iDevice->WriteStream() << aSize;
	}

EXPORT_C void CMetafileGc::SetBrushColor(const TRgb& aColor)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE( iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetBrushColor;
	iDevice->WriteStream() << aColor;
	}

EXPORT_C void CMetafileGc::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetBrushStyle;
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aBrushStyle));
	}

EXPORT_C void CMetafileGc::SetBrushOrigin(const TPoint& aOrigin)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << ESetBrushOrigin;
	iDevice->WriteStream() << aOrigin;
	}

EXPORT_C void CMetafileGc::UseBrushPattern(const CFbsBitmap* aBitmap)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EUseBrushPattern;
	TRAP(errCode, aBitmap->ExternalizeL(iDevice->WriteStream()));
	}

EXPORT_C void CMetafileGc::DiscardBrushPattern()
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDiscardBrushPattern;
	}

EXPORT_C void CMetafileGc::MoveTo(const TPoint& aPoint)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EMoveTo;
	iDevice->WriteStream() << aPoint;
	}

EXPORT_C void CMetafileGc::MoveBy(const TPoint& aVector)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EMoveBy;
	iDevice->WriteStream() << aVector;
	}

EXPORT_C void CMetafileGc::Plot(const TPoint& aPoint)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EPlot;
	iDevice->WriteStream() << aPoint;
	}

EXPORT_C void CMetafileGc::DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawArc;
	iDevice->WriteStream() << aRect;
	iDevice->WriteStream() << aStart;
	iDevice->WriteStream() << aEnd;
	}

EXPORT_C void CMetafileGc::DrawLine(const TPoint& aPoint1,const TPoint& aPoint2)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawLine;
	iDevice->WriteStream() << aPoint1;
	iDevice->WriteStream() << aPoint2;
	}

EXPORT_C void CMetafileGc::DrawLineTo(const TPoint& aPoint)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawLineTo;
	iDevice->WriteStream() << aPoint;
	}

EXPORT_C void CMetafileGc::DrawLineBy(const TPoint& aVector)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawLineBy;
	iDevice->WriteStream() << aVector;
	}

EXPORT_C void CMetafileGc::DrawPolyLine(const CArrayFix<TPoint>* aPointList)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawPolyLine1;
	
	TInt numpoints=aPointList->Count();
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(numpoints));
	for (TInt i=0; i<numpoints; i++)
		iDevice->WriteStream() << (*aPointList)[i];
	}

EXPORT_C void CMetafileGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawPolyLine2;
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aNumPoints));
	TPoint *p=(TPoint*) aPointList,*pEnd=p+aNumPoints;
	for (; p<pEnd; p++)
		iDevice->WriteStream() << *p;
	}

EXPORT_C void CMetafileGc::DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawPie;
	iDevice->WriteStream() << aRect;
	iDevice->WriteStream() << aStart;
	iDevice->WriteStream() << aEnd;
	}

EXPORT_C void CMetafileGc::DrawEllipse(const TRect& aRect)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawEllipse;
	iDevice->WriteStream() << aRect;
	}

EXPORT_C void CMetafileGc::DrawRect(const TRect& aRect)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawRect;
	iDevice->WriteStream() << aRect;
	}

EXPORT_C void CMetafileGc::DrawRoundRect(const TRect& aRect,const TSize& aCornerSize)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawRoundRect;
	iDevice->WriteStream() << aRect;
	iDevice->WriteStream() << aCornerSize;
	}

EXPORT_C TInt CMetafileGc::DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawPolygon1;
	TInt numpoints=aPointList->Count();
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(numpoints));
	for (TInt i=0; i<numpoints; i++)
		iDevice->WriteStream() << (*aPointList)[i];
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aFillRule));
	return KErrNone;
	}

EXPORT_C TInt CMetafileGc::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawPolygon2;
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aNumPoints));
	TPoint *p=(TPoint*) aPointList,*pEnd=p+aNumPoints;
	for (; p<pEnd; p++)
		iDevice->WriteStream() << *p;
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aFillRule));
	return KErrNone;
	}

EXPORT_C void CMetafileGc::DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawBitmap1;
	iDevice->WriteStream() << aTopLeft;
	TRAP(errCode, ExternalizeBitmapL(aSource));
	}

EXPORT_C void CMetafileGc::DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawBitmap2;
	iDevice->WriteStream() << aDestRect;
	TRAP(errCode, ExternalizeBitmapL(aSource));
	}

EXPORT_C void CMetafileGc::DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,const TRect& aSourceRect)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawBitmap3;
	iDevice->WriteStream() << aDestRect;
	TRAP(errCode, ExternalizeBitmapL(aSource));
	iDevice->WriteStream() << aSourceRect;
	}

EXPORT_C void CMetafileGc::DrawBitmapMasked(const TRect& /*aDestRect*/,const CFbsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CFbsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{}

EXPORT_C void CMetafileGc::DrawBitmapMasked(const TRect& /*aDestRect*/,const CWsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CWsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{}
		
EXPORT_C void CMetafileGc::MapColors(const TRect& /*aRect*/,const TRgb* /*aColors*/,TInt /*aNumPairs*/,TBool /*aMapForwards*/)
	{}

EXPORT_C TInt CMetafileGc::SetClippingRegion(const TRegion &/*aRegion*/)
	{	
		return KErrNone;
	}

EXPORT_C void CMetafileGc::CancelClippingRegion()
	{}

EXPORT_C void CMetafileGc::DrawTextVertical(const TDesC& /*aText*/,const TPoint& /*aPos*/,TBool /*aUp*/)
	{}
	
EXPORT_C void CMetafileGc::DrawTextVertical(const TDesC& /*aText*/,const TRect& /*aBox*/,TInt /*aBaselineOffset*/,TBool /*aUp*/,TTextAlign /*aVert*/,TInt /*aMargin*/)
	{}

EXPORT_C TInt CMetafileGc::AlphaBlendBitmaps(const TPoint& /*aDestPt*/, const CFbsBitmap* /*aSrcBmp*/, const TRect& /*aSrcRect*/, const CFbsBitmap* /*aAlphaBmp*/, const TPoint& /*aAlphaPt*/) 
	{
		return KErrNone;
	}
	
EXPORT_C TInt CMetafileGc::AlphaBlendBitmaps(const TPoint& /*aDestPt*/, const CWsBitmap* /*aSrcBmp*/,  const TRect& /*aSrcRect*/, const CWsBitmap*  /*aAlphaBmp*/, const TPoint& /*aAlphaPt*/)
	{
		return KErrNone;
	}


EXPORT_C void CMetafileGc::DrawText(const TDesC& aString,const TPoint& aPosition)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TRAP_IGNORE(iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawText1;
	iDevice->WriteStream() << aString;
	iDevice->WriteStream() << aPosition;
	}

EXPORT_C void CMetafileGc::DrawText(const TDesC& aString,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHoriz,TInt aLeftMrg)
	{
	// TRAP and ignore the ERROR code due to this beeing a non-leaving method
	TInt	errCode = 0;
	TRAP(errCode, iDevice->UseGcL(iIndex));
	iDevice->WriteStream() << EDrawText2;
	iDevice->WriteStream() << aString;
	iDevice->WriteStream() << aBox;
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aBaselineOffset));
	TRAP(errCode, iDevice->WriteStream().WriteUint8L((TUint8) aHoriz));
	TRAP(errCode, iDevice->WriteStream().WriteInt32L(aLeftMrg));
	}

void CMetafileGc::ExternalizeBitmapL(const CFbsBitmap* aSource)
	{
	CFbsBitmap* bitmap=new(ELeave)	CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(aSource->SizeInPixels(),iDevice->DisplayMode()));
	CFbsBitmapDevice* bitmapdevice=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapdevice);
	CFbsBitGc* gc;
	User::LeaveIfError(bitmapdevice->CreateContext((CGraphicsContext*&) gc));
	CleanupStack::PushL(gc);
	gc->BitBlt(TPoint(0,0),aSource);
	bitmap->ExternalizeL(iDevice->WriteStream());
	CleanupStack::PopAndDestroy(3);
	}

EXPORT_C CMetafilePlayback::CMetafilePlayback(CGraphicsDevice* aDevice):
	iDevice(aDevice)	
	{
	__DECLARE_NAME(_S("CMetafilePlayback"));
	}

EXPORT_C CMetafilePlayback* CMetafilePlayback::NewL(CGraphicsDevice* aDevice)
	{
	CMetafilePlayback* playback=new(ELeave) CMetafilePlayback(aDevice);
	return playback;
	}

EXPORT_C CMetafilePlayback::~CMetafilePlayback()
	{
	}


EXPORT_C void CMetafilePlayback::DrawL(RReadStream& aReadStream)
	{
	CArrayPtrFlat<CGraphicsContext>* gclist=new(ELeave) CArrayPtrFlat<CGraphicsContext>(8);
	CleanupStack::PushL(gclist);
	CFontStack* fontstack = CFontStack::NewL(iDevice);
	CleanupStack::PushL(fontstack);
	CArrayFixFlat<TPoint>* pointlist=new(ELeave) CArrayFixFlat<TPoint>(8);
	CleanupStack::PushL(pointlist);
	CFbsBitmap* bitmap=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	TSize kpixelsizeintwips;
	aReadStream	>> kpixelsizeintwips;
	TGraphicsContextCommandCode code;
	TInt gcindex=0;
	do
		{
		code = (TGraphicsContextCommandCode) aReadStream.ReadUint8L();
		switch (code)
			{
			case EUseGc:
				{
				TInt gcindex=aReadStream.ReadInt32L();
				if (gcindex>=gclist->Count())
					{
					for (TInt i=gclist->Count(); i<=gcindex; i++)
						{
						CGraphicsContext* gc;
						User::LeaveIfError(iDevice->CreateContext(gc));
						CleanupStack::PushL(gc);
						gclist->AppendL(gc);
						}
					}

				break;
				}
			case EEndOfStream:
				{
				break;
				}
			case ESetOrigin:
				{
				TPoint pos;
				aReadStream >> pos;
				(*gclist)[gcindex]->SetOrigin(pos);
				break;
				}
			case ESetDrawMode:
				{
				CGraphicsContext::TDrawMode drawingmode;
				drawingmode = (CGraphicsContext::TDrawMode) aReadStream.ReadUint8L();
				(*gclist)[gcindex]->SetDrawMode(drawingmode);
				break;
				}
			case ESetClippingRect:
				{
				TRect rect;
				aReadStream >> rect;
				(*gclist)[gcindex]->SetClippingRect(rect);
				break;
				}
			case ECancelClippingRect:
				{
				(*gclist)[gcindex]->CancelClippingRect();
				break;
				}
			case EReset:
				{
				(*gclist)[gcindex]->CancelClippingRect();
				break;
				}
			case EUseFont:
				{
				TFontSpec spec;
				aReadStream >> spec;
				aReadStream.ReadInt32L();  // height in pixels
				aReadStream.ReadInt32L();  // baseline offset in pixels
				spec.iHeight=((spec.iHeight*iDevice->VerticalPixelsToTwips(1000))+(kpixelsizeintwips.iHeight/2))/kpixelsizeintwips.iHeight;
				CFont* font;
				User::LeaveIfError(iDevice->GetNearestFontToDesignHeightInTwips(font,spec));
				fontstack->AddFontL(font);
				(*gclist)[gcindex]->UseFont(font);
				break;
				}
			case EDiscardFont:
				{
				(*gclist)[gcindex]->DiscardFont();
				break;
				}
			case ESetUnderlineStyle:
				{
				TFontUnderline underlinestyle;
				underlinestyle = (TFontUnderline) aReadStream.ReadUint8L();
				(*gclist)[gcindex]->SetUnderlineStyle(underlinestyle);
				break;
				}
			case ESetStrikethroughStyle:
				{
				TFontStrikethrough strikethroughstyle;
				strikethroughstyle = (TFontStrikethrough) aReadStream.ReadUint8L();
				(*gclist)[gcindex]->SetStrikethroughStyle(strikethroughstyle);
				break;
				}
			case ESetWordJustification:
				{
				TInt excesswidth,numgaps;
				excesswidth=aReadStream.ReadInt32L();
				numgaps=aReadStream.ReadInt32L();
				(*gclist)[gcindex]->SetWordJustification(excesswidth,numgaps);
				break;
				}
			case ESetCharJustification:
				{
				TInt excesswidth,numgaps;
				excesswidth=aReadStream.ReadInt32L();
				numgaps=aReadStream.ReadInt32L();
				(*gclist)[gcindex]->SetCharJustification(excesswidth,numgaps);
				break;
				}
			case ESetPenColor:
				{
				TRgb color;
				aReadStream >> color;
				(*gclist)[gcindex]->SetPenColor(color);
				break;
				}
			case ESetPenStyle:
				{
				CGraphicsContext::TPenStyle penstyle;
				penstyle=(CGraphicsContext::TPenStyle) aReadStream.ReadUint8L();
				(*gclist)[gcindex]->SetPenStyle(penstyle);
				break;
				}
			case ESetPenSize:
				{
				TSize size;
				aReadStream >> size;
				(*gclist)[gcindex]->SetPenSize(size);
				break;
				}
			case ESetBrushColor:
				{
				TRgb color;
				aReadStream >> color;
				(*gclist)[gcindex]->SetBrushColor(color);
				break;
				}
			case ESetBrushStyle:
				{
				CGraphicsContext::TBrushStyle brushstyle;
				brushstyle = (CGraphicsContext::TBrushStyle) aReadStream.ReadUint8L();
				(*gclist)[gcindex]->SetBrushStyle(brushstyle);
				break;
				}
			case ESetBrushOrigin:
				{
				TPoint origin;
				aReadStream >> origin;
				(*gclist)[gcindex]->SetBrushOrigin(origin);
				break;
				}
			case EUseBrushPattern:
				{
				bitmap->InternalizeL(aReadStream);
				(*gclist)[gcindex]->UseBrushPattern(bitmap);
				bitmap->Reset();
				break;
				}
			case EDiscardBrushPattern:
				{
				(*gclist)[gcindex]->DiscardBrushPattern();
				break;
				}
			case EMoveTo:
				{
				TPoint point;
				aReadStream >> point;
				(*gclist)[gcindex]->MoveTo(point);
				break;
				}
			case EMoveBy:
				{
				TPoint vector;
				aReadStream >> vector;
				(*gclist)[gcindex]->MoveBy(vector);
				break;
				}
			case EPlot:
				{
				TPoint point;
				aReadStream >> point;
				(*gclist)[gcindex]->Plot(point);
				break;
				}
			case EDrawArc:
				{
				TRect rect;
				aReadStream >> rect;
				TPoint start,end;
				aReadStream >> start;
				aReadStream >> end;
				(*gclist)[gcindex]->DrawArc(rect,start,end);
				break;
				}
			case EDrawLine:
				{
				TPoint point1,point2;
				aReadStream >> point1;
				aReadStream >> point2;
				(*gclist)[gcindex]->DrawLine(point1,point2);
				break;
				}
			case EDrawLineTo:
				{
				TPoint point;
				aReadStream >> point;
				(*gclist)[gcindex]->DrawLineTo(point);
				break;
				}
			case EDrawLineBy:
				{
				TPoint vector;
				aReadStream >> vector;
				(*gclist)[gcindex]->DrawLineBy(vector);
				break;
				}
			case EDrawPolyLine1: 
				{
				}
			case EDrawPolyLine2: 
				{
				TInt numpoints;
				numpoints=aReadStream.ReadInt32L();
				for (TInt i=0; i<numpoints; i++)
					{
					TPoint point;
					aReadStream >> point;
					pointlist->AppendL(point);
					}
				(*gclist)[gcindex]->DrawPolyLine(pointlist);
				pointlist->Reset();
				break;
				}
			case EDrawPie:
				{
				TRect rect;
				aReadStream >> rect;
				TPoint start,end;
				aReadStream >> start;
				aReadStream >> end;
				(*gclist)[gcindex]->DrawPie(rect,start,end);
				break;
				}
			case EDrawEllipse:
				{
				TRect rect;
				aReadStream >> rect;
				(*gclist)[gcindex]->DrawEllipse(rect);
				break;
				}
			case EDrawRect:
				{
				TRect rect;
				aReadStream >> rect;
				(*gclist)[gcindex]->DrawRect(rect);
				break;
				}
			case EDrawRoundRect:
				{
				TRect rect;
				aReadStream >> rect;
				TSize cornersize;
				aReadStream >> cornersize;
				(*gclist)[gcindex]->DrawRoundRect(rect,cornersize);
				break;
				}
			case EDrawPolygon1:  
				{
				}
			case EDrawPolygon2:  
				{
				TInt numpoints;
				numpoints=aReadStream.ReadInt32L();
				for (TInt i=0; i<numpoints; i++)
					{
					TPoint point;
					aReadStream >> point;
					pointlist->AppendL(point);
					}
				CGraphicsContext::TFillRule fillrule=(CGraphicsContext::TFillRule) aReadStream.ReadUint8L();
				(*gclist)[gcindex]->DrawPolygon(pointlist,fillrule);
				pointlist->Reset();
				break;
				}
			case EDrawBitmap1:   
				{
				TPoint topleft;
				aReadStream >> topleft;
				bitmap->InternalizeL(aReadStream);
				(*gclist)[gcindex]->DrawBitmap(topleft,bitmap);
				bitmap->Reset();
				break;
				}
			case EDrawBitmap2:   
				{
				TRect destrect;
				aReadStream >> destrect;
				bitmap->InternalizeL(aReadStream);
				(*gclist)[gcindex]->DrawBitmap(destrect,bitmap);
				bitmap->Reset();
				break;
				}
			case EDrawBitmap3:   
				{
				TRect destrect;
				aReadStream >> destrect;
				bitmap->InternalizeL(aReadStream);
				TRect sourcerect;
				aReadStream >> sourcerect;
				(*gclist)[gcindex]->DrawBitmap(destrect,bitmap,sourcerect);
				bitmap->Reset();
				break;
				}
			case EDrawText1:	    
				{
				HBufC* string=HBufC::NewLC(aReadStream,KMaxTInt);
				TPoint position;
				aReadStream >> position;
				(*gclist)[gcindex]->DrawText(*string,position);
				CleanupStack::PopAndDestroy();
				break;
				}
			case EDrawText2:	    
				{
				HBufC* string=HBufC::NewLC(aReadStream,KMaxTInt);
				TRect box;
				aReadStream >> box;
				TInt baselineoffset=aReadStream.ReadInt32L();
				CGraphicsContext::TTextAlign horiz=(CGraphicsContext::TTextAlign) aReadStream.ReadUint8L();
				TInt leftmrg=aReadStream.ReadInt32L();
				(*gclist)[gcindex]->DrawText(*string,box,baselineoffset,horiz,leftmrg);
				CleanupStack::PopAndDestroy();
				break;
				}
			}
		}
	while (code!=EEndOfStream);
	CleanupStack::PopAndDestroy(gclist->Count()+4);
	}
