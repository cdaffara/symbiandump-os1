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

#include "graphicscontextchecker.h"
#include "panics.h"

#define CHECK_TEXTCURSOR() \
	CHK_ASSERT_ALWAYS(iTextCursor, EEventCheckerPanicUsingMWsTextCursorAfterEnd)

#define CHECK_FADER() \
	CHK_ASSERT_ALWAYS(iFader, EEventCheckerPanicUsingMWsFaderAfterEnd)

#define CHECK_DRAWING_TARGET() \
	CHK_ASSERT_ALWAYS(iTarget != ETargetNone, EEventCheckerPanicDrawingWithoutTarget)
		
#define CHECK_GC_AND_DRAWING_TARGET() \
	do { \
		CHK_ASSERT_ALWAYS(iContext, EEventCheckerPanicUsingMWsGraphicsContextAfterEnd); \
		CHECK_DRAWING_TARGET(); \
	} while(EFalse)

#define CHECK_TEXTCURSOR_AND_DRAWING_TARGET() \
	do { \
		CHECK_TEXTCURSOR(); \
		CHECK_DRAWING_TARGET(); \
	} while(EFalse)

#define CHECK_FADER_AND_DRAWING_TARGET() \
	do { \
		CHECK_FADER(); \
		CHECK_DRAWING_TARGET(); \
	} while(EFalse)

CGraphicsContextChecker* CGraphicsContextChecker::NewL(MWsGraphicDrawerEnvironment& /*aEnv*/)
	{
	CGraphicsContextChecker* self = new(ELeave) CGraphicsContextChecker();
	return self;
	}

CGraphicsContextChecker::CGraphicsContextChecker()
	: iTarget(ETargetNone)
	{
	}

CGraphicsContextChecker::~CGraphicsContextChecker()
	{
	iContext = NULL;
	iTextCursor = NULL;
	iFader = NULL;
	}

void CGraphicsContextChecker::SetGraphicsContext(MWsGraphicsContext* aGraphicsContext)
	{
	iContext = aGraphicsContext;
	}

const MWsGraphicsContext* CGraphicsContextChecker::GraphicsContext() const
	{
	return iContext;
	}

void CGraphicsContextChecker::SetTextCursor(MWsTextCursor* aTextCursor)
	{
	iTextCursor = aTextCursor;
	}

const MWsTextCursor* CGraphicsContextChecker::TextCursor() const
	{
	return iTextCursor;
	}

void CGraphicsContextChecker::SetFader(MWsFader* aFader)
	{
	iFader = aFader;
	}

const MWsFader* CGraphicsContextChecker::Fader() const
	{
	return iFader;
	}

TAny* CGraphicsContextChecker::ResolveObjectInterface(TUint aTypeId)
	{
	if(aTypeId == MWsDrawAnnotationObserver::EWsObjectInterfaceId)
		return static_cast<MWsDrawAnnotationObserver*>(this);
		
	//The remaining part of this method isn't exactly beautiful since we are merging three object
	//provider interfaces into one object, so we have no way of knowing 
	//which interface this method was called on.
	//however, aTypeId is a unique id and the chance that multiple
	//mixins implement the same extension is slim
	TAny* interface = NULL;
	if(!interface && iContext)
		interface = iContext->ResolveObjectInterface(aTypeId);
	if(!interface && iTextCursor)
		interface = iTextCursor->ResolveObjectInterface(aTypeId);
	if(!interface && iFader)
		interface = iFader->ResolveObjectInterface(aTypeId);
	return interface;
	}

void CGraphicsContextChecker::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->BitBlt(aDestPos, aSourceBitmap);
	}

void CGraphicsContextChecker::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->BitBlt(aDestPos, aSourceBitmap, aSourceRect);
	}

void CGraphicsContextChecker::BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->BitBltMasked(aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask);
	}

void CGraphicsContextChecker::BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->BitBltMasked(aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, aMaskPos);
	}

void CGraphicsContextChecker::ResetClippingRegion()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->ResetClippingRegion();
	}

void CGraphicsContextChecker::Clear()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->Clear();
	}

void CGraphicsContextChecker::Clear(const TRect& aRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->Clear(aRect);
	}

void CGraphicsContextChecker::ResetBrushPattern()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->ResetBrushPattern();
	}

void CGraphicsContextChecker::ResetFont()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->ResetFont();
	}

void CGraphicsContextChecker::DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawArc(aRect, aStart, aEnd);
	}

void CGraphicsContextChecker::DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawPie(aRect, aStart, aEnd);
	}

void CGraphicsContextChecker::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawBitmap(aDestRect, aSourceBitmap);
	}

void CGraphicsContextChecker::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawBitmap(aDestRect,	aSourceBitmap, aSourceRect);
	}

void CGraphicsContextChecker::DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawBitmapMasked(aDestRect, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask);
	}

void CGraphicsContextChecker::DrawRoundRect(const TRect& aRect, const TSize& aEllipse)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawRoundRect(aRect, aEllipse);
	}

void CGraphicsContextChecker::DrawPolyLine(const TArray<TPoint>& aPointList)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawPolyLine(aPointList);
	}

void CGraphicsContextChecker::DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawPolyLineNoEndPoint(aPointList);
	}

void CGraphicsContextChecker::DrawPolygon(const TArray<TPoint>& aPointList, TFillRule aFillRule)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawPolygon(aPointList, aFillRule);
	}

void CGraphicsContextChecker::DrawEllipse(const TRect& aRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawEllipse(aRect);
	}

void CGraphicsContextChecker::DrawLine(const TPoint& aStart, const TPoint& aEnd)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawLine(aStart, aEnd);
	}

void CGraphicsContextChecker::DrawLineTo(const TPoint& aPoint)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawLineTo(aPoint);
	}

void CGraphicsContextChecker::DrawLineBy(const TPoint& aVector)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawLineBy(aVector);
	}

void CGraphicsContextChecker::DrawRect(const TRect& aRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawRect(aRect);
	}

void CGraphicsContextChecker::DrawText(const TDesC& aText, const TTextParameters* aParam)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawText(aText, aParam);
	}

void CGraphicsContextChecker::DrawText(const TDesC& aText, const TTextParameters* aParam, const TPoint& aPosition)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawText(aText, aParam, aPosition);
	}

void CGraphicsContextChecker::DrawText(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawText(aText, aParam, aClipRect);
	}

void CGraphicsContextChecker::DrawText(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset, TTextAlign aHrz, TInt aMargin)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawText(aText, aParam, aClipFillRect, aBaselineOffset, aHrz, aMargin);
	}

void CGraphicsContextChecker::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, TBool aUp)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawTextVertical(aText, aParam, aUp);
	}

void CGraphicsContextChecker::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TPoint& aPosition, TBool aUp)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawTextVertical(aText, aParam, aPosition, aUp);
	}

void CGraphicsContextChecker::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TBool aUp)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawTextVertical(aText, aParam, aClipRect, aUp);
	}

void CGraphicsContextChecker::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, TBool aUp, TTextAlign aVert, TInt aMargin)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawTextVertical(aText, aParam, aClipRect, aBaselineOffset, aUp, aVert, aMargin);
	}

void CGraphicsContextChecker::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, TInt aTextWidth, TBool aUp, TTextAlign aVert, TInt aMargin)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->DrawTextVertical(aText, aParam, aClipRect, aBaselineOffset, aTextWidth, aUp, aVert, aMargin);
	}

void CGraphicsContextChecker::MoveTo(const TPoint& aPoint)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->MoveTo(aPoint);
	}

void CGraphicsContextChecker::MoveBy(const TPoint& aVector)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->MoveBy(aVector);
	}

void CGraphicsContextChecker::Plot(const TPoint& aPoint)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->Plot(aPoint);
	}

void CGraphicsContextChecker::Reset()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->Reset();
	}

void CGraphicsContextChecker::SetBrushColor(const TRgb& aColor)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetBrushColor(aColor);
	}

void CGraphicsContextChecker::SetBrushOrigin(const TPoint& aOrigin)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetBrushOrigin(aOrigin);
	}

void CGraphicsContextChecker::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetBrushStyle(aBrushStyle);
	}

void CGraphicsContextChecker::SetClippingRegion(const TRegion& aRegion)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetClippingRegion(aRegion);
	}

void CGraphicsContextChecker::SetDrawMode(TDrawMode aDrawMode)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetDrawMode(aDrawMode);
	}

void CGraphicsContextChecker::SetOrigin(const TPoint& aPoint)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetOrigin(aPoint);
	}

void CGraphicsContextChecker::SetPenColor(const TRgb& aColor)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetPenColor(aColor);
	}

void CGraphicsContextChecker::SetPenStyle(TPenStyle aPenStyle)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetPenStyle(aPenStyle);
	}

void CGraphicsContextChecker::SetPenSize(const TSize& aSize)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetPenSize(aSize);
	}

void CGraphicsContextChecker::SetTextShadowColor(const TRgb& aColor)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetTextShadowColor(aColor);
	}

void CGraphicsContextChecker::SetCharJustification(TInt aExcessWidth, TInt aNumChars)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetCharJustification(aExcessWidth, aNumChars);
	}

void CGraphicsContextChecker::SetWordJustification(TInt aExcessWidth, TInt aNumGaps)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetWordJustification(aExcessWidth, aNumGaps);
	}

void CGraphicsContextChecker::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetUnderlineStyle(aUnderlineStyle);
	}

void CGraphicsContextChecker::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetStrikethroughStyle(aStrikethroughStyle);
	}

void CGraphicsContextChecker::SetBrushPattern(const CFbsBitmap& aBitmap)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetBrushPattern(aBitmap);
	}

void CGraphicsContextChecker::SetBrushPattern(TInt aFbsBitmapHandle)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetBrushPattern(aFbsBitmapHandle);
	}

void CGraphicsContextChecker::SetFont(const CFont* aFont)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetFont(aFont);
	}

void CGraphicsContextChecker::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->CopyRect(aOffset, aRect);
	}

void CGraphicsContextChecker::UpdateJustification(const TDesC& aText, const TTextParameters* aParam)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->UpdateJustification(aText, aParam);
	}

void CGraphicsContextChecker::UpdateJustificationVertical(const TDesC& aText, const TTextParameters* aParam, TBool aUp)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->UpdateJustificationVertical(aText, aParam, aUp);
	}

void CGraphicsContextChecker::SetFontNoDuplicate(const CFont* aFont)
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->SetFontNoDuplicate(aFont);
	}

TBool CGraphicsContextChecker::HasBrushPattern() const
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->HasBrushPattern();
	}

TBool CGraphicsContextChecker::HasFont() const
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->HasFont();
	}

TRgb CGraphicsContextChecker::BrushColor() const
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->BrushColor();
	}

TRgb CGraphicsContextChecker::PenColor() const
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->PenColor();
	}

TRgb CGraphicsContextChecker::TextShadowColor() const
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->TextShadowColor();
	}

TInt CGraphicsContextChecker::GetError()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->GetError();
	}

TPoint CGraphicsContextChecker::Origin() const
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->Origin();
	}

const TRegion& CGraphicsContextChecker::ClippingRegion()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->ClippingRegion();
	}

TInt CGraphicsContextChecker::Push()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	return iContext->Push();
	}

void CGraphicsContextChecker::Pop()
	{
	CHECK_GC_AND_DRAWING_TARGET();
	iContext->Pop();
	}

void CGraphicsContextChecker::DrawTextCursor(const TTextCursorInfo& aTextCursorInfo)
	{
	//CHECK_TEXTCURSOR_AND_DRAWING_TARGET();
	CHECK_TEXTCURSOR();
	iTextCursor->DrawTextCursor(aTextCursorInfo);
	}

void CGraphicsContextChecker::SetFadingParameters(const TDesC8& aData)
	{
	CHECK_FADER_AND_DRAWING_TARGET();
	iFader->SetFadingParameters(aData);
	}

void CGraphicsContextChecker::FadeArea(const TRegion& aRegion)
	{
	CHECK_FADER_AND_DRAWING_TARGET();
	iFader->FadeArea(aRegion);
	}

void CGraphicsContextChecker::WindowRedrawStart(const MWsWindowTreeNode& /*aWindowTreeNode*/, const TRegion& /*aRegion*/)
	{
	CHK_ASSERT_ALWAYS(iTarget == ETargetNone, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	iTarget = ETargetWindow;
	}

void CGraphicsContextChecker::WindowRedrawEnd(const MWsWindowTreeNode& /*aWindowTreeNode*/)
	{
	CHK_ASSERT_ALWAYS(iTarget == ETargetWindow, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	iTarget = ETargetNone;
	}

void CGraphicsContextChecker::WindowAnimRedrawStart(const MWsWindowTreeNode& /*aWindowTreeNode*/, const TRegion& /*aRegion*/)
	{
	CHK_ASSERT_ALWAYS(iTarget == ETargetNone, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	iTarget = ETargetWindowAnim;
	}

void CGraphicsContextChecker::WindowAnimRedrawEnd(const MWsWindowTreeNode& /*aWindowTreeNode*/)
	{
	CHK_ASSERT_ALWAYS(iTarget == ETargetWindowAnim, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	iTarget = ETargetNone;
	}

void CGraphicsContextChecker::SpriteRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& /*aRegion*/)
	{
	CHK_ASSERT_ALWAYS(iTarget==ETargetNone, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	iTarget = (aWindowTreeNode.ParentNode()->NodeType() == MWsWindowTreeNode::EWinTreeNodeRoot) ? ETargetFloatingSprite : ETargetWindowSprite;	
	}

void CGraphicsContextChecker::SpriteRedrawEnd(const MWsWindowTreeNode& /*aWindowTreeNode*/)
	{
	CHK_ASSERT_ALWAYS((iTarget==ETargetFloatingSprite || iTarget==ETargetWindowSprite), EEventCheckerPanicUnbalancedDrawingTargetEvents);
	iTarget = ETargetNone;
	}

void CGraphicsContextChecker::SpriteFlash(const MWsWindowTreeNode& /*aWindowTreeNode*/, TBool /*aFlashOn*/)
	{
	CHK_ASSERT_ALWAYS((iTarget==ETargetFloatingSprite || iTarget==ETargetWindowSprite), EEventCheckerPanicUnbalancedDrawingTargetEvents);
	}

void CGraphicsContextChecker::SegmentRedrawStart(const TRegion& /*aRegion*/)
	{
	CHK_ASSERT_ALWAYS(iTarget == ETargetWindow, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	}

void CGraphicsContextChecker::SegmentRedrawEnd()
	{
	CHK_ASSERT_ALWAYS(iTarget == ETargetWindow, EEventCheckerPanicUnbalancedDrawingTargetEvents);
	}

