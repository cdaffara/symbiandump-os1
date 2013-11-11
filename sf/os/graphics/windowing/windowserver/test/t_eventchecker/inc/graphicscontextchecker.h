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

#ifndef __GRAPHICSCONTEXTCHECKER_H__
#define __GRAPHICSCONTEXTCHECKER_H__

#include <graphics/wsgraphicscontext.h>
#include <graphics/wscursor.h>
#include <graphics/wsgraphicdrawer.h>

class CGraphicsContextChecker : public CBase, 
								public MWsGraphicsContext, 
								public MWsTextCursor, 
								public MWsFader, 
								public MWsDrawAnnotationObserver
	{
public:
	static CGraphicsContextChecker* NewL(MWsGraphicDrawerEnvironment& aEnv);
	~CGraphicsContextChecker();

	void SetGraphicsContext(MWsGraphicsContext* aGraphicsContext);
	const MWsGraphicsContext* GraphicsContext() const;
	void SetTextCursor(MWsTextCursor* aTextCursor);
	const MWsTextCursor* TextCursor() const;
	void SetFader(MWsFader* aFader);
	const MWsFader* Fader() const;

public: //MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);
public: //MWsGraphicsContext
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
	void DrawText(const TDesC& aText, const TTextParameters* aParam);
	void DrawText(const TDesC& aText, const TTextParameters* aParam, const TPoint& aPosition);
	void DrawText(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect);
	void DrawText(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset, TTextAlign aHrz=ELeft, TInt aMargin=0);
	void DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, TBool aUp);
	void DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TPoint& aPosition, TBool aUp);
	void DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TBool aUp);
	void DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, TBool aUp, TTextAlign aVert=ELeft, TInt aMargin=0);
	void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, TInt aTextWidth, TBool aUp, TTextAlign aVert=ELeft, TInt aMargin=0);
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
	void UpdateJustification(const TDesC& aText, const TTextParameters* aParam);
	void UpdateJustificationVertical(const TDesC& aText, const TTextParameters* aParam, TBool aUp);
	void SetFontNoDuplicate(const CFont* aFont);
	TBool HasBrushPattern() const;
	TBool HasFont() const;
	TRgb BrushColor() const;
	TRgb PenColor() const;
	TRgb TextShadowColor() const;
	TInt GetError();
	TPoint Origin() const;
	const TRegion& ClippingRegion();
	TInt Push();
	void Pop();	
public: //MWsTextCursor
	void DrawTextCursor(const TTextCursorInfo& aTextCursorInfo);
public: //MWsFader
	void SetFadingParameters(const TDesC8& aData);
	void FadeArea(const TRegion& aRegion);
public: //MWsDrawAnnotationObserver
	void WindowRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion);
	void WindowRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode);
	void WindowAnimRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion);
	void WindowAnimRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode);
	void SpriteRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion);
	void SpriteRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode);
	void SpriteFlash(const MWsWindowTreeNode& aWindowTreeNode, TBool aFlashOn);
	void SegmentRedrawStart(const TRegion& aRegion);
	void SegmentRedrawEnd();

private:
	enum TDrawingTarget
		{
		ETargetNone,
		ETargetWindow,
		ETargetWindowAnim,
		ETargetWindowSprite,
		ETargetFloatingSprite,
		};
private:
	CGraphicsContextChecker();
private:
	MWsGraphicsContext* iContext;
	MWsTextCursor* iTextCursor;
	MWsFader* iFader;
	TDrawingTarget iTarget;
	};

#endif //__GRAPHICSCONTEXTCHECKER_H__
