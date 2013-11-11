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

#ifndef VGENGINE_H_
#define VGENGINE_H_

/**
@file
@internalComponent Reference implementation of Open VG hardware accelerated DirectGDI adaptation.
*/

#include "directgdidriverimpl.h"
#include "directgdiadapter.h"
#include "clippingregionmanager.h"
#include <bitdraworigin.h>
#include <graphics/directgdiengine.h>
#include <graphics/sgimage.h>
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <gdi.h>

class CDirectGdiDriverImpl;

#ifdef DRAWGLYPH_BUFFERED
const TInt KMaxGlyphs = 64;
typedef struct TDrawGlyphCommand_Tag
{
	TPoint pos;
	TChar aChar;
	TUint8* aGlyphImage;
	TGlyphBitmapType aGlyphBitmapType;
	TSize aGlyphImageSize;
	TRect aClipRect;
	DirectGdi::TGraphicsRotation aRotation;	
	
	TBool SameGlyph(const TDrawGlyphCommand_Tag aOther)
		{ 
		return aGlyphImage == aOther.aGlyphImage && 
				aChar == aOther.aChar && 
				aGlyphImage == aOther.aGlyphImage && 
				aGlyphBitmapType == aOther.aGlyphBitmapType && 
				aGlyphImageSize == aOther.aGlyphImageSize;
		}
} TDrawGlyphCommand;
#endif

/**
Concrete (reference) implementation of a MDirectGdiEngine.

This class implements the rendering mechanism for DirectGDI. In this case,
OpenVG is used to get things done, so hardware acceleration is used.
*/
NONSHARABLE_CLASS(CVgEngine): public CBase, public MDirectGdiEngine, public MDrawDeviceOrigin
	{
public:
	virtual ~CVgEngine();
	CVgEngine(CDirectGdiDriverImpl& aDriver);

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
	void DrawPolygon(const TArray<TPoint>& aPoints,	DirectGdi::TFillRule aRule);
	void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	void DrawEllipse(const TRect& aRect);
	void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect);
	void BitBltMasked(
			const TPoint& aDestPos,				
			const CFbsBitmap& aSourceBitmap,
			const TRect& aSourceRect,				
			const CFbsBitmap& aMaskBitmap, 
			TBool aInvertMask);
	void BitBltMasked(const TPoint& aDestPt,
			const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect,				
			const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos);
	void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap,
			const TRect& aSrcRect);
	void DrawBitmapMasked(const TRect& aDestRect,				
			const CFbsBitmap& aSourceBitmap,const TRect& aSourceRect,				
			const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
	void DrawResource(const TPoint& aPos, 
			const RDirectGdiDrawableSource& aSource, 
			DirectGdi::TGraphicsRotation aRotation);	
	void DrawResource(const TRect& aDestRect,
			const RDirectGdiDrawableSource& aSource,
			DirectGdi::TGraphicsRotation aRotation);
	void DrawResource(const TRect& aDestRect,				
			const RDirectGdiDrawableSource& aSource, 
			const TRect& aSrcRect,
			DirectGdi::TGraphicsRotation aRotation);
	void DrawResource(
			const TRect& aDestRect,				
			const RDirectGdiDrawableSource& aSource,
			const TDesC8& aParam);
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
	TRect SgMetricsToVgTRect (const TPoint& aCoord, const TInt width, const TInt height) const;

	const TPoint ConvertToVgCoords(const TPoint& aPoint);
	const TPoint ConvertToVgCoords(const TRect& aRect);
	void ResetVgMatrix();
	void ResetVgMatrix(const VGMatrixMode aMatrixMode);

	// Path (line) creation commands
	TBool PreparePath(VGPath& aPath, TInt aExpectedCommandCount);
	void AppendPathCommand(VGubyte aCommand);
	void AppendPathCommand(VGubyte aCommand, VGfloat aCoord);
	void AppendPathCommand(VGubyte aCommand, VGfloat aCoord1, VGfloat aCoord2);
	TInt PrepareForPathCommand(TInt aCommandCount, TInt aCoordCount);
	void FinishPath(VGPath aPath);	
	TInt AllocPathCommands(TInt aCommandCount);
	TInt AllocPathCoords(TInt aCoordCount);
	TBool ConvertBitmapToVgImage(const CFbsBitmap& aBitmap, VGImage& aImage, TBool aIsMask = EFalse, const TPoint& aOrigin = TPoint(0,0));
	VGImage CreateSourceVGImage(const CFbsBitmap& aSource, TBool aFlipY = EFalse, const TPoint& aOrigin = TPoint(0,0));	
	TInt CreateStandardBrush(TSize& aPatternSize, VGbyte* aBrushPattern);
	TBool NonZeroBrushPatternOrigin();
	TInt CopyCurrentBrushPatternForNonZeroOrigin();	
	inline VGfloat* Identity (void) {return iIdentityMatrix;}
	void SetVgState();
	void SetVguError(VGUErrorCode aErr);
	TBool GetCurrentBrushPattern(VGImage& aBrush, TSize& aSize, VGImageFormat& aFormat) const;

	TReal GetAngleFromXAxisAnticlockwise(const TReal aOriginX, const TReal aOriginY, const TReal aPointX, const TReal aPointY, const TReal aWidth, const TReal aHeight);

	void DoDrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd, VGUArcType aArcType);
	void DoDrawPoly(const TArray<TPoint>& aPointList, TBool aClosed);

	TBool MakeEngineCurrent();

	void DoBitBltMasked (
		const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap,
		TBool aInvertMask,
		const TPoint& aMaskPos);

	void DoVgImageDraw (
		const TRect& aDestRect, 
		const CFbsBitmap& aSourceBitmap, 
		const TRect& aSourceRect);

	void DoVgMaskedImageDraw(
		const TPoint& aDestPos,
		VGImage aSourceImage,
		const TRect& aSourceImageSize,
		const TRect& aSourceRect,
		const VGImage aMaskImage,
		const TSize& srcMaskSize,
		const TSize& aDestSize,
		TBool aInvertMask);

	inline TBool IntersectsClippingRegion (const TRect& aRect);

	void DoDrawResource(const TRect& aDestRect, CDirectGdiImageSourceImpl* aSource, DirectGdi::TGraphicsRotation aRotation);	
	void DoDrawResource(const TRect& aDestRect, CDirectGdiImageSourceImpl* aSource, const TRect& aSrcRect, DirectGdi::TGraphicsRotation aRotation);
	
	VGImage DoVgCreateImage(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality);
	void SetVgPaintColor(VGPaint& aPaint, const TRgb& aColor);
	
private:	
	CDirectGdiDriverImpl& iDriver;
	RClippingRegionManager iRegionManager;
	CDirectGdiImageTargetImpl* iRenderingTarget;
	TRegionFix<1> iTargetRegion;	/**< The region which the rendering target occupies.*/
	TSize iSize;	/**< The size of the rendering target.*/

	TPoint iOrigin;		/**< The origin of the drawing engine coordinate system.*/
	TPoint iDrawOrigin; /**< The origin of all drawing, affected by app mode offset */
	VGPaint iPen;		/**< VGPaint object for all pen operations (Stroke operations in OpenVG).*/
	TSize iPenSize;		/**< The current pen size.*/
	TRgb iPenColor;		/**< The current pen colour.*/
	TRgb iTextShadowColor;	/**< The current colour for blending text shadow.*/
	DirectGdi::TPenStyle iPenStyle;	/**< The current pen style.*/
	VGPaint iBrush;		/**< VGPaint object for all brush operations (Fill operations in OpenVG).*/
	VGPaint iClearBrush;	/**< VGPaint object for Clear() operations only.*/
	TRgb iBrushColor;	/**< The current brush colour.*/
	VGPath iVgPath;		/**< OpenVG path used for all path drawing.*/
	TUint32 iFontId;	/**< The id of the current font.*/
	VGPaint iTextBrush; /**< VGPaint object used to draw text. */

	/**
	VGImage for holding the current user brush pattern (i.e when 
	DirectGdi::TBrushStyle::EPatternedBrush is selected).
	*/
	VGImage iBrushPatternUser;

	/**
	VGImage for holding the current standard brush pattern (i.e. when one of 
	DirectGdi::TBrushStyle::EVerticalHatchBrush to DirectGdi::TBrushStyle::EDiamondCrossHatchBrush
	brush styles are selected).
	*/
	VGImage iBrushPatternStandard;

	VGImage iBrushPatternStandardRegion; /**< VGImage for holding a sub-region of iBrushPatternStandard.*/
	TInt iBrushPatternStandardId;	/**< The id of the currently loaded standard brush pattern. This is initialised to -1 inside Activate().*/
	DirectGdi::TBrushStyle iBrushStyle;	/**< The current brush style.*/
	TPoint iBrushOrigin;	/**< The current brush origin.*/

	/**
	This VGImage should always hold the current brush pattern if iBrushOrigin is not (0,0).
	This could be a shifted version of either iBrushPatternUser or iBrushPatternStandard.
	*/
	VGImage iBrushPatternNonZeroOrigin;

	VGbitfield iPaintMode;	/**< The current paint mode.*/

	// The following members variables are used when drawing paths
	VGubyte* iPathCommands;	/**< Pointer to list of stored commands to be used in a VGPath.*/
	VGfloat* iPathCoords;	/**< Pointer to list of stored coordinates to be used in a VGPath.*/
	TInt iPathCommandCount; /**< The number of commands to be used in a VGPath.*/
	TInt iPathCoordCount;	/**< The number of coordinates to be used in a VGPath.*/
	TInt iCurrentMaxCommands;	/**< The current maximum number of commands which can be entered in a VGPath.*/
	TInt iCurrentMaxCoords;	/**< The current maximum number of coordinates which can be entered in a VGPath.*/

	TPoint iLinePos;	/**< The current line position. Can be changed using MoveTo(), MoveBy(), DrawLineTo(), DrawLineBy() and DrawPolyline().*/
	DirectGdi::TDrawMode iDrawMode;	/**< The current drawing mode.*/
	VGfloat iIdentityMatrix[9];	/**< The matrix used to convert coordinate systems.*/

	// Brush sizes. We'll remember them instead of doing potentially expensive vgGets
	TSize iBrushPatternStandardRegionSize;	/**< The size of the current sub-region of the standard brush pattern.*/
	TSize iBrushPatternStandardSize;	/**< The size of the current standard brush pattern.*/
	TSize iBrushPatternUserSize;	/**< The size of the curent  user brush pattern.*/

	TInt iBrushPatternUserBitmapHandle;	/**< Stores the bitmap handle to recreate the iBrushPatternUser VGImage.*/

	MFontGlyphImageStorage* iFontGlyphImageStorage;	/**< Interface which deals with glyph cache images.*/

#ifdef	DRAWGLYPH_BUFFERED
	TInt iDrawGlyphCount;
	TDrawGlyphCommand iDrawGlyphCommand[KMaxGlyphs];
	void FlushDrawGlyphs();
#endif
	};

inline TBool CVgEngine::NonZeroBrushPatternOrigin()
	{
	return ((iBrushOrigin.iX != 0) || (iBrushOrigin.iY != 0));
	}

/**
Helper method to determine if a specified rectangle intersects with the current clipping region.
If the current clipping region is a null pointer then intersection is assumed.

@param 	aRect Rectangle to check for intersection with clipping region.

@return ETrue if aRect intersects with clipping region, else EFalse.
 */
inline TBool CVgEngine::IntersectsClippingRegion (const TRect& aRect)
	{
	if(iRegionManager.ClippingRegion().IsEmpty())
		{
		return (aRect.Intersects(iTargetRegion.BoundingRect()));
		}
	else 
		{
		return (iRegionManager.ClippingRegion().Intersects(aRect));
		}
	}

#endif /*VGENGINE_H_*/
