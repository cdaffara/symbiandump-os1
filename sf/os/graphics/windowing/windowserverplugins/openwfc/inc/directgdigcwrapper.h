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

#ifndef __DIRECTGDIGCWRAPPER_H__
#define __DIRECTGDIGCWRAPPER_H__

#include <graphics/wsgraphicscontext.h>
#include <graphics/directgdiimagetarget.h>
#include <graphics/directgdicontext.h>
#include <graphics/wsdrawablesourceprovider.h>
#include <graphics/sgimage.h>
#include <graphics/wscursor.h>


/* 
 * Required by MWsFader implementation
 */
struct TFadingParams
    {
    TUint8 blackMap;
    TUint8 whiteMap;
    };

/**
The purpose of this class is to wrap a @c CDirectGdiContext whenever an
@c MWsGraphicsContext is needed.

The methods doesn't do anything other than calling the corresponding method
in @c MWsGraphicsContext.

@see MWsGraphicsContext
@see MWsDrawableSourceProvider
@see CDirectGdiContext;

@internalComponent
*/
class CDirectGdiGcWrapper : public CBase, public MWsGraphicsContext, public MWsFader, public MWsDrawableSourceProvider, public MWsTextCursor
	{
public:
	static CDirectGdiGcWrapper* NewL();
	~CDirectGdiGcWrapper();
	inline CDirectGdiContext& DirectGdiGc();

public: //from MWsGraphicsContext
	void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap);
	void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect);
	void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
	void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos);
	void ResetClippingRegion();
	void Clear();
	void Clear(const TRect& aRect);
	void ResetBrushPattern();
	void ResetFont();
	void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap);
	void DrawBitmap(const TRect& aDestRect,	const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect);
	void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
	void DrawRoundRect(const TRect& aRect, const TSize& aEllipse);
	void DrawPolyLine(const TArray<TPoint>& aPointList);
	void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList);
	void DrawPolygon(const TArray<TPoint>& aPointList, TFillRule aFillRule=EAlternate);
	void DrawEllipse(const TRect& aRect);
	void DrawLine(const TPoint& aStart, const TPoint& aEnd);
	void DrawLineTo(const TPoint& aPoint);
	void DrawLineBy(const TPoint& aVector);
	void DrawRect(const TRect& aRect);
	void DrawText(const TDesC& aText,const TTextParameters* aParam);
	void DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition);
	void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect);
	void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipFillRect,TInt aBaselineOffset,TTextAlign aHrz=ELeft,TInt aMargin=0);
	void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp);
	void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TBool aUp);
	void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TBool aUp);
	void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TInt aBaselineOffset,TInt aTextWidth,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	void MoveTo(const TPoint& aPoint);
	void MoveBy(const TPoint& aVector);
	void Plot(const TPoint& aPoint);
	void Reset();
	void SetBrushColor(const TRgb& aColor);
	void SetBrushOrigin(const TPoint& aOrigin);
	void SetBrushStyle(TBrushStyle aBrushStyle);
	void SetClippingRegion(const TRegion& aRegion);
	void SetDrawMode(TDrawMode aDrawMode);
	void SetOrigin(const TPoint& aPoint = TPoint(0, 0));
	void SetPenColor(const TRgb& aColor);
	void SetPenStyle(TPenStyle aPenStyle);
	void SetPenSize(const TSize& aSize);
	void SetTextShadowColor(const TRgb& aColor);
	void SetCharJustification(TInt aExcessWidth, TInt aNumChars);
	void SetWordJustification(TInt aExcessWidth, TInt aNumGaps);
	void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	void SetBrushPattern(const CFbsBitmap& aBitmap);
	void SetBrushPattern(TInt aFbsBitmapHandle);
	void SetFont(const CFont* aFont);
	void CopyRect(const TPoint& aOffset, const TRect& aRect);
	void UpdateJustification(const TDesC& aText,const TTextParameters* aParam);
	void UpdateJustificationVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp);
	void SetFontNoDuplicate(const CFont* aFont);
	TBool HasBrushPattern() const;
	TBool HasFont() const;
	TRgb BrushColor() const;
	TRgb PenColor() const;
	TRgb TextShadowColor() const;
	
	// Return current error status of MWsGraphicsContext
	TInt GetError();
	// Getters
	TPoint Origin() const;
	const TRegion& ClippingRegion();
	// Save and retrieve gc settings
	TInt Push();
	void Pop();

public: // MWsFader
	void SetFadingParameters(const TDesC8& aData);
	void FadeArea(const TRegion& aRegion);

	//from MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);

	//from MWsDrawableSourceProvider
	TInt CreateDrawableSource(const TSgDrawableId& aDrawableId, TAny*& aSource);
	void CloseDrawableSource(TAny* aSource);
	void DrawResource(const TAny* aSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TAny* aSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TAny* aSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TAny* aSource, const TRect& aRect, const TDesC8& aParam);

public: // from MWsTextCursor
	void DrawTextCursor(const TTextCursorInfo& aTextCursorInfo);	
	
private:
	void SetError(TInt aError);
	
private:
	CDirectGdiContext* iContext;
	TInt iErrorCode;
	CBufBase* iGcBuf; // used for keeping state when drawing CWsGraphicDrawers
	RRegionBuf<20> iClippingRegion; // Clipping region while doing drawing
	TPoint iOrigin; // Window Origin + User defined gc origin
	RPointerArray<RDirectGdiDrawableSource> iDrawableSources;
	//Required by MWsFader implmentation
	TRgb			iFadeColor;
	const TUint16*	iLut;
	};

//
// CDirectGdiGcWrapper
//

inline CDirectGdiContext& CDirectGdiGcWrapper::DirectGdiGc()
	{return(*iContext);}



#endif //__DIRECTGDIGCWRAPPER_H__
