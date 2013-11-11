// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CSWDIRECTGDIENGINE_H
#define CSWDIRECTGDIENGINE_H

/**
@file
@internalComponent
*/

#include "directgdiadapter.h"
#include "swdirectgdibitmap.h"
#include <graphics/directgdiengine.h>
#include <e32base.h>
#include <bitdraw.h>
#include <bitdraworigin.h>
#include <bitmap.h>

// For use when testing the code, allows the fast blending to be disabled so back to back comparisons
// can done be to check either timing or exact output.
//#define __ALLOW_FAST_BLEND_DISABLE__

class RDirectGdiDrawableSource;
class CSwDirectGdiDriverImpl;
class CSwDirectGdiImageSourceImpl;
class CSwDirectGdiImageTargetImpl;
class CFbsDrawDevice;
class CBitwiseBitmap;
class MFastBlend;

/**
Concrete (reference) implementation of a MDirectGdi Engine. It is a
collection of drawing routines which implement the software adaptation of DirectGDI.
*/
NONSHARABLE_CLASS(CSwDirectGdiEngine) : public CBase, public MDirectGdiEngine, public MDrawDeviceOrigin
	{
public:
	CSwDirectGdiEngine(CSwDirectGdiDriverImpl*);
	virtual ~CSwDirectGdiEngine();
	
	// from MDirectGdiEngine
	TInt Activate(RDirectGdiImageTarget& aTarget);
	void Deactivate();
	void SetOrigin(const TPoint& aOrigin);
	void SetClippingRegion(const TRegion& aRegion);
	void ResetClippingRegion();
	void SetDrawMode(DirectGdi::TDrawMode aMode);
	void SetPenColor(const TRgb& aColor);
	void SetPenStyle(DirectGdi::TPenStyle aStyle);
	void SetPenSize(const TSize& aSize);
	void SetTextShadowColor(const TRgb& aColor);
	void SetBrushColor(const TRgb& aColor);
	void SetBrushStyle(DirectGdi::TBrushStyle aStyle);
	void SetBrushOrigin(const TPoint& aOrigin);
	TInt SetBrushPattern(const CFbsBitmap& aPattern);
	void ResetBrushPattern();
	void SetFont(TUint32 aFontId);
	void ResetFont();
	void Reset();
	void Clear(const TRect& aRect);
	void Clear();
	void MoveTo(const TPoint& aPoint);
	void MoveBy(const TPoint& aVector);
	void Plot(const TPoint& aPoint);
	void DrawLine(const TPoint& aStart, const TPoint& aEnd);
	void DrawLineTo(const TPoint& aPoint);
	void DrawLineBy(const TPoint& aVector);
	void DrawRect(const TRect& aRect);
	void DrawRoundRect(const TRect& aRect, const TSize& aCornerSize);
	void DrawPolyLine(const TArray<TPoint>& aPointList);
	void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList);
	void DrawPolygon(const TArray<TPoint>& aPoints, DirectGdi::TFillRule aRule);
	void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	void DrawEllipse(const TRect& aRect);
	void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aBitmap, const TRect& aSrcRect);
	void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aBitmap, const TRect& aSrcRect,				
			const CFbsBitmap& aMask, TBool aInvertMask);	
	void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aBitmap, const TRect& aSrcRect,				
			const CFbsBitmap& aAlpha, const TPoint& aAlphaPos);
	void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aBitmap, const TRect& aSrcRect);	
	void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap& aBitmap, const TRect& aSrcRect,				
			const CFbsBitmap& aMask, TBool aInvertMask);	
	void DrawResource(const TRect& aDestRect, const RDirectGdiDrawableSource& aSource, const TRect& aSrcRect,
			DirectGdi::TGraphicsRotation aRotation);
	void DrawResource(const TPoint& aPos, const RDirectGdiDrawableSource& aSource,
			DirectGdi::TGraphicsRotation aRotation);
	void DrawResource(const TRect& aDestRect, const RDirectGdiDrawableSource& aSource,
			DirectGdi::TGraphicsRotation aRotation);
	void DrawResource(const TRect& aDestRect, const RDirectGdiDrawableSource& aSource, const TDesC8& aParam);
	void DrawGlyph(const TPoint& aPos, const TChar aChar, const TUint8* aGlyphImage,
			const TGlyphBitmapType aGlyphBitmapType, const TSize& aGlyphImageSize, const TRect& aClipRect,
			const DirectGdi::TGraphicsRotation aRotation = DirectGdi::EGraphicsRotationNone);	
	void CopyRect(const TPoint& aOffset, const TRect& aRect);
	void ExternalizeL(RWriteStream& aWriteStream);
	void InternalizeL(RReadStream& aReadStream);
	TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);	
	void BeginDrawGlyph();
	void EndDrawGlyph();
	
	//from MDrawDeviceOrigin 
	TInt Set(const TPoint& aDrawOrigin);
	void Get(TPoint& aDrawOrigin);

private:
	CSwDirectGdiEngine();
	TBool AnalyseEllipse(const TRect& rc,TPoint& srad,TPoint& erad, TInt& startq,TInt& endq,TBool* quads);
	TUint32* ClipBinaryArray(TUint32* aArray, TUint32* aArrayLimit, TInt& aDataWd, TInt& aDataHt, TPoint& aPos, const TRect& aClipRect);
	void ClipFillLine(TPoint aLeft,TPoint aRight, TRect aClipRect);
	void CopyCharLine(TUint32* aBinaryDataPtr, TInt aBufferWords, const TUint8* aData, TInt aBitShift, TInt aCharWidth);
	void DrawPolyLineNoEndPoint(const TPoint* aPointList, TInt aNumPoints);
	void DoBitBlt(const TPoint& aDest, CBitwiseBitmap* aSrce, TUint32* aBase, TInt aStride,
			const TRect& aSrceRect);
	void DoBitBltAlpha(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap, TUint32* aSourceBase,
			const TRect& aSourceRect, CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,
			const TPoint& aAlphaPoint, TBool aInvertMask);
	void DoBitBltMasked(const TPoint& aDest, CBitwiseBitmap* aSourceBitmap, 
			TUint32* aSourceBase, const TRect& aSourceRect, CBitwiseBitmap* aMaskBitmap,
			TUint32* aMaskBase, TBool aInvertMask);	
	void DoBitBltMaskedFlicker(const TPoint& aDest, CBitwiseBitmap* aSourceBitmap, TUint32* aSourceBase,
			  const TRect& aSourceRect, CBitwiseBitmap* aMaskBitmap, TUint32* aMaskBase, TBool aInvertMask);
	void DoCopyRect(const TPoint& aOffset, const TRect& rect);
	void DoDrawLine(TPoint aPt1,TPoint aPt2,TBool aDrawStartPoint);
	void DoPlot(const TPoint& aPoint);
	void DoDrawBitmap(const TRect& aDestRect, CBitwiseBitmap* aBitmap, TUint32* aBase, TInt aStride,
			 const TRect& aSrceRect, const TRect& aClipRect);
	void DoDrawBitmapMasked(const TRect& aDestRect, CBitwiseBitmap* aSourceBitmap, TUint32* aSourceBase,
			   const TRect& aSourceRect, CBitwiseBitmap* aMaskBitmap, TUint32* aMaskBase, TBool aInvertMask,
			   const TRect& aClipRect);
	TInt CheckImageSource(TInt aSourceHandle, CSwDirectGdiImageSourceImpl*& aImpl, TSize* aImgSize);
	void DrawResourceCommon(const TRect& aDestRect,	const CSwDirectGdiImageSourceImpl* aImpl,
			const TRect& aSrcRect, DirectGdi::TGraphicsRotation aRotation);		
	void DoDrawResource(const TRect& aDestRect, const CSwDirectGdiImageSourceImpl* aImpl,
			const TRect& aSrceRect, DirectGdi::TGraphicsRotation aRotation, const TRect& aClipRect);
	void DoBlitResource(const TPoint& aDest, const CSwDirectGdiImageSourceImpl* aImpl,
			const TRect& aSrceRect, DirectGdi::TGraphicsRotation aRotation);
	void DoScaledBlitResource(const TRect& aDestRect, const CSwDirectGdiImageSourceImpl* aImpl,
			const TRect& aSrceRect, DirectGdi::TGraphicsRotation aRotation, const TRect& aClipRect);
	void DoDrawDottedWideLine(const TPoint& aPt1, const TPoint& aPt2, TBool aDrawStartPoint,
			 const TRect& aScreenRect, TRect aClipRect);
	void DoDrawSolidWideLine(const TPoint& aPt1, const TPoint& aPt2, TBool aDrawStartPoint,
			 const TRect& aScreenRect, TRect aClipRect);
	void DrawBitmapGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
						 const TSize& aGlyphImageSize, const TRect& aClipRect);
	void DrawLargeBitmapGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
							  const TSize& aGlyphImageSize, const TRect& aClipRect);
	void DrawRotatedBitmapGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
								const TSize& aGlyphImageSize, const TRect& aClipRect, const DirectGdi::TGraphicsRotation aRotation);
	void DrawAntiAliasedGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
							  const TSize& aGlyphImageSize, const TRect& aClipRect);
	void DrawRotatedAntiAliasedGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
									 const TSize& aGlyphImageSize, const TRect& aClipRect, const DirectGdi::TGraphicsRotation aRotation);
	void DrawFourColourGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
							 const TSize& aGlyphImageSize, const TRect& aClipRect);
	void DrawRotatedFourColourGlyph(const TPoint& aPos, const TUint8* aGlyphImage,
									const TSize& aGlyphImageSize, const TRect& aClipRect, const DirectGdi::TGraphicsRotation aRotation);
	void EllipseFill(const TRect& aRect);
	void EllipseOutline(const TRect& aRect);
	void EllipseOutlineWide(const TRect& aRect);
	TUint32* GetScanLineOffsetPtr(CBitwiseBitmap* aSrce, TUint32*& aSlptr, TInt aLength, TPoint aPixel,
								  TUint32* aBase, TLineScanningPosition& aLineScanningPosition, TUint aXOffset);
	inline static TInt16 Load16(const TUint8* aPtr) { return TInt16(aPtr[0]+(aPtr[1]<<8)); }
	TUint MemoryOffsetForPixelPitch(TUint aX, TDisplayMode aDisplayMode) const;
	void OutputCharLineMultiplied(TPoint aPrintPos, TUint32* aBuffer, TInt aDataLength, TInt aNum, const TRect& aClipRect);
	void OutputCharLineVertical(TPoint aPrintPos, TUint32* aBuffer, TInt aDataLength, TInt aNum, TInt aDirection, const TRect& aClipRect);
	TInt PenAllocate();
	void PenDrawClipped(TPoint aPoint, const TRect& aClipRect);
	void PenDrawDeferred(TPoint aPoint,TInt* aArray,TInt aFirstElement);
	void PieArcOutline(const TRect& ellrect,const TPoint& startradius, const TPoint& endradius, TBool pie);
	void PieFill(const TRect& ellrect, const TPoint& aStartIntersect, const TPoint& aEndIntersect,
			TInt aStartQuadrant, TInt aEndQuadrant, const TBool* aQuadrants);
	void PieShell(const TRect& ellrect,const TPoint& startradius, const TPoint& endradius,
				  const TBool* quads, TInt startquad, TInt endquad, TRect aClipRect);
	void PieSliver(const TRect& ellrect,const TPoint& startradius, const TPoint& endradius,
				   TInt quad, TRect aClipRect);
	void PieTriangles(TBool aInside,const TPoint& aStart,const TPoint& aEnd, TRect aClipRect);
	void PolyFill(const TPoint* aPointList, TInt aNumPoints, DirectGdi::TFillRule aFillRule);
	void PolyFill(const TArray<TPoint>* aPointList, DirectGdi::TFillRule aFillRule);
	void PolyFillLarge(const TArray<TPoint>* aPointList, DirectGdi::TFillRule aFillRule);
	void PolyFillLarge(const TPoint* aPointList, TInt aNumPoints, DirectGdi::TFillRule aFillRule);
	void PolyOutline(const TArray<TPoint>* aPointList);
	void PolyOutline(const TPoint* aPointList,TInt aNumPoints);
	void RectFill(const TRect& aRect);
	void ResetPenArray();
	void RoundRectFill(const TRect& aRect, const TSize& aCornerSize);
	void RoundRectOutline(const TRect& aRect, const TSize& aCornerSize);
	void SetPenArray(TInt* aPenArray);
	void TruncateRect(TRect& aRect);
	void Rotate90(TRect& aRect, const TPoint& aOrig);
	void Rotate180(TRect& aRect, const TPoint& aOrig);
	void Rotate270(TRect& aRect, const TPoint& aOrig);
	TBool RotatedSizeMatch(const TRect& aDst, const TRect& aSrc, DirectGdi::TGraphicsRotation aRot);
	void TileScanLine(TPtr8& aScanLine, TInt aLengthInPixels, const TPoint& aSrcPt,
			const CBitwiseBitmap* aMaskBitmap, TLineScanningPosition& aScanLinePos, 
			TUint32* aMaskBase, TDisplayMode aDisplayMode);
	TInt FastBlendInterface(const CBitwiseBitmap* aSource, const CBitwiseBitmap* aMask, MFastBlend*& aFastBlend) const;
	inline CGraphicsContext::TDrawMode GcDrawMode(DirectGdi::TDrawMode aMode) const ;
	TDisplayMode ScanLineBufferDisplayMode(CFbsDrawDevice* aDrawDevice);

private:
	CSwDirectGdiBitmap iBrushBitmap;
	TRgb iBrushColor;
	TPoint iBrushOrigin;
	DirectGdi::TBrushStyle iBrushStyle;	
	TRegionFix<1> iDefaultRegion;
	const TRegion* iDefaultRegionPtr;
	TInt iDotLength;
	TInt iDotMask;
	TInt iDotParam;
	TInt iDotDirection;
	CFbsDrawDevice* iDrawDevice;
	DirectGdi::TDrawMode iDrawMode;
	CSwDirectGdiDriverImpl* iDriver;
	TPoint iLinePosition;
	TPoint iOrigin;
	TPoint iDrawOrigin;
	TInt* iPenArray;
	TRgb iPenColor;			/**< The current pen colour. The default pen colour is black. */
	TRgb iTextShadowColor;	/**< Colour for text shadows. */
	TSize iPenSize;
	DirectGdi::TPenStyle iPenStyle;	
	RRegion iClippingRegion;
    CSwDirectGdiImageTargetImpl* iRenderingTarget;
#if defined(__ALLOW_FAST_BLEND_DISABLE__)
    TBool iFastBlendDisabled;
#endif

	};

/**
Converts a draw-mode enum from the DirectGdi namespace to the CGraphicsContext namespace.
*/
inline CGraphicsContext::TDrawMode CSwDirectGdiEngine::GcDrawMode(DirectGdi::TDrawMode aMode) const
	{
	switch(aMode)
		{
		case DirectGdi::EDrawModePEN:
			return CGraphicsContext::EDrawModePEN;
		case DirectGdi::EDrawModeWriteAlpha:
			return CGraphicsContext::EDrawModeWriteAlpha;
		default:
			return CGraphicsContext::EDrawModePEN;
		}
	}

#endif /*CSWDIRECTGDIENGINE_H*/
