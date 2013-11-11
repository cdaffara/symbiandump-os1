// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __REMOTEGC_H__
#define __REMOTEGC_H__

#include <w32std.h>
#include <s32mem.h>

class CCommandBuffer;
class RWsDrawableSource;

/**
MCommandBufferObserver should be implemented by classes that want a notification when the commandbuffer has been updated.
@publishedPartner
@released*/
class MCommandBufferObserver
    {
public:
	/** 
	CommandBufferUpdated is called when the commandbuffer has been updated.
	
	@param aDrawRect The rect that has been updated.
	@param aBoundingRect The boundingrect of the commands that has been updated.
	*/
	virtual void CommandBufferUpdated(const TRect& aDrawRect, const TRect& aBoundingRect) = 0;
	};

/** 
Intercepts and logs all draw operations to a window-gc 
@publishedPartner
@released*/
NONSHARABLE_CLASS(CRemoteGc): public CWindowGc
	{
public:
	IMPORT_C static CRemoteGc* NewL(CWsScreenDevice* aDevice);
	IMPORT_C ~CRemoteGc();	
	IMPORT_C void SetCommandBufferObserver(MCommandBufferObserver* aCommandBufferObserver);		
	IMPORT_C void ResetCommandBuffer();
	IMPORT_C void ExternalizeL(RWsGraphicMsgBuf& aMsgBuf, TBool aEntireBuffer);
	IMPORT_C void BeginDraw(const TRect& aRect);
	IMPORT_C void EndDraw();

public: // inherited methods		
	
	// CWindowGc
	void Activate(RDrawableWindow &aDevice);
	void Deactivate();	
	void BitBlt(const TPoint &aPoint, const CWsBitmap *aBitmap);
	void BitBlt(const TPoint &aDestination, const CWsBitmap *aBitmap, const TRect &aSource);
	void BitBltMasked(const TPoint& aPoint, const CWsBitmap *aBitmap, const TRect& aSourceRect, const CWsBitmap *aMaskBitmap, TBool aInvertMask);
	
	// CBitmapContext
	void Clear();
	void Clear(const TRect &aRect);
	void CopyRect(const TPoint &anOffset, const TRect &aRect);
	void BitBlt(const TPoint &aPoint, const CFbsBitmap *aBitmap);
	void BitBlt(const TPoint &aDestination, const CFbsBitmap *aBitmap, const TRect &aSource);
	void BitBltMasked(const TPoint& aPoint, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask);
	void SetFaded(TBool aFaded);
	void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);
	TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
	TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp, const TRect& aSrcRect, const CWsBitmap* aAlphaBmp, const TPoint& aAlphaPt);	
	
	// CGraphicsContext
	void SetOrigin(const TPoint &aPoint);
	void SetDrawMode(TDrawMode aDrawingMode);
	void SetClippingRect(const TRect& aRect);
	void CancelClippingRect();
	void Reset();
	void UseFont(const CFont *aFont);
	void DiscardFont();
	void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
	void SetCharJustification(TInt aExcessWidth,TInt aNumChars);
	void SetPenColor(const TRgb &aColor);
	void SetPenStyle(TPenStyle aPenStyle);
	void SetPenSize(const TSize& aSize);
	void SetBrushColor(const TRgb &aColor);
	void SetBrushStyle(TBrushStyle aBrushStyle);
	void SetBrushOrigin(const TPoint &aOrigin);
	void UseBrushPattern(const CFbsBitmap *aDevice);
	void DiscardBrushPattern();
	void MoveTo(const TPoint &aPoint);
	void MoveBy(const TPoint &aPoint);
	void Plot(const TPoint &aPoint);
	void DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	void DrawLine(const TPoint &aPoint1,const TPoint &aPoint2);
	void DrawLineTo(const TPoint &aPoint);
	void DrawLineBy(const TPoint &aPoint);
	void DrawPolyLine(const CArrayFix<TPoint> *aPointList);
	void DrawPolyLine(const TPoint* aPointList, TInt aNumPoints);
	void DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	void DrawEllipse(const TRect &aRect);
	void DrawRect(const TRect &aRect);
	void DrawRoundRect(const TRect &aRect,const TSize &aEllipse);
	TInt DrawPolygon(const CArrayFix<TPoint> *aPointList, TFillRule aFillRule);
	TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints, TFillRule aFillRule);
	void DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aDevice);
	void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice);
	void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice, const TRect &aSourceRect);
	void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask);
	void DrawBitmapMasked(const TRect& aDestRect, const CWsBitmap* aBitmap, const TRect& aSourceRect, const CWsBitmap* aMaskBitmap, TBool aInvertMask);
	
	// Required as not all DrawText and DrawTextVertical functions are implemented in CRemoteGc
	using CWindowGc::DrawText;
	using CWindowGc::DrawTextVertical;
	
	void DrawText(const TDesC &aBuf,const TPoint &aPos);
	void DrawText(const TDesC &aBuf,const TRect &aBox,TInt aBaselineOffset, TTextAlign aHoriz, TInt aLeftMrg);
	void DrawText(const TDesC& aText,const TPoint& aPosition,const TDrawTextParam& aParam);
	void MapColors(const TRect& aRect,const TRgb* aColors, TInt aNumPairs, TBool aMapForwards);
	TInt SetClippingRegion(const TRegion &aRegion);
	void CancelClippingRegion();	
	void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp);
	void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert,TInt aMargin);
	void DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect);
	void DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect,const TDesC8& aData);
	void SetDitherOrigin(const TPoint& aPoint);
	void SetOpaque(TBool aDrawOpaque=ETrue);	
protected:	
	TInt APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput);
private:
	CRemoteGc(CWsScreenDevice* aDevice);
	void ConstructL();
	TInt APIExGetUnderlineMetrics(TAny*& aOutput);
	TInt APIExSetShadowColor(TAny* aShadowColor);
	TInt APIExGetShadowColor(TAny*& aOutput);
	// New DrawText API's that take in context
	TInt APIExDrawText(const TDesC &aBuf,const TTextParameters* aParam,const TPoint &aPos);
	TInt APIExDrawText(const TDesC &aBuf,const TTextParameters* aParam,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);

	TInt APIExDrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPos,TBool aUp);
	TInt APIExDrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	TInt APIExInterface(TAny*& aInterface, TUid aInterfaceId);

	void DrawResource(const TPoint& aPos, const RWsDrawableSource& aSource, CWindowGc::TGraphicsRotation aRotation=CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, CWindowGc::TGraphicsRotation aRotation=CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, const TRect& aSrcRect, CWindowGc::TGraphicsRotation aRotation=CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, const TDesC8& aParam);

private:
	TRect iDrawRect;
	TRect iBoundingRect;
	TBool iHasBitmapCommand;
	CCommandBuffer* iCommandBuffer;	
	MCommandBufferObserver* iCommandBufferObserver;
#ifdef SYMBIAN_GRAPHICS_GCE
	CFbsFont *iFont;
	TRgb iShadowColor;
	class CPimpl;
	friend class CPimpl;
	CPimpl* iRemoteGcPimpl;
#endif	
	};
	
#endif	// __REMOTEGC_H__
