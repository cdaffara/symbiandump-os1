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

#include "RemoteGc.h"
#include "RemoteGcUtils.h"
#include "CommandBuffer.h"
#include <graphics/gdi/gdiconsts.h>

/**
Creates a new remotegc.

@param aDevice The windowserver screendevice to use.
@param aCommandBufferObserver Pointer to a commandbufferobserver.
@return A pointer to a new instance of CRemoteGc.
*/
EXPORT_C CRemoteGc* CRemoteGc::NewL(CWsScreenDevice* aDevice)
	{
	CRemoteGc* remoteGc = new (ELeave) CRemoteGc(aDevice);
	CleanupStack::PushL(remoteGc);
	remoteGc->ConstructL();
	CleanupStack::Pop(remoteGc);
	return remoteGc;
	}

CRemoteGc::CRemoteGc(CWsScreenDevice* aDevice) : CWindowGc(aDevice), iCommandBufferObserver(NULL)
	{	
	}
	
EXPORT_C CRemoteGc::~CRemoteGc()
	{
	delete iCommandBuffer;	
	}
	
void CRemoteGc::ConstructL()
	{
	User::LeaveIfError(CWindowGc::Construct());
	iCommandBuffer = CCommandBuffer::NewL();
	}
	
EXPORT_C void CRemoteGc::SetCommandBufferObserver(MCommandBufferObserver* aCommandBufferObserver)
	{
	iCommandBufferObserver = aCommandBufferObserver;
	}

/**
Resets the commandbuffer.
*/
EXPORT_C void CRemoteGc::ResetCommandBuffer()
	{
	iCommandBuffer->Reset();
	}

/**
Externalizes commandbuffer sections into a format which makes it possible to send over IPC.
If ETrue is sent as a parameter to this method, the entire commandbuffer will be externalized,
otherwise only sections which has not been externalized before will be externalized. Note that if only
not externalized sections is asked for, the flag will be reset on that section so next call
to ExternalizeLC will not externalize that section.

@param aMsgBuf A buffer used to externalize the commandbuffer to.
@param aEntireBuffer If ETrue, the entire commandbuffer will be externalized, otherwise only sections which has not been externalized before.
*/	
EXPORT_C void CRemoteGc::ExternalizeL(RWsGraphicMsgBuf& aMsgBuf, TBool aEntireBuffer)
	{
	return iCommandBuffer->ExternalizeL(aMsgBuf, aEntireBuffer);
	}

/**
Prepares the remotegc to be drawn to.

@param aRect The rect to be drawn.
*/
EXPORT_C void CRemoteGc::BeginDraw(const TRect& aRect)
	{
	iDrawRect = aRect;
	iBoundingRect = TRect();
	iHasBitmapCommand = EFalse;
	iCommandBuffer->Prepare(aRect);
	}
 
/**
Finishes the current redraw.
This method should be called when drawing to the remotegc is complete.
*/
EXPORT_C void CRemoteGc::EndDraw()
	{
	iBoundingRect.Intersection(iDrawRect);
	const TInt err = iCommandBuffer->Finish(iDrawRect, iBoundingRect, iHasBitmapCommand);

	if(iCommandBufferObserver && !err)
		iCommandBufferObserver->CommandBufferUpdated(iDrawRect, iBoundingRect); 
 	}

void CRemoteGc::Activate(RDrawableWindow &aDevice)
	{
	BeginDraw(aDevice.GetDrawRect());
	CWindowGc::Activate(aDevice);
	}
	
void CRemoteGc::Deactivate()
	{	
	CWindowGc::Deactivate();
	EndDraw();
	}
	
void CRemoteGc::Clear()
	{
	iCommandBuffer->Write<TDrawCode>(ECommandClear);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::Clear(const TRect& aRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandClearRect);
	iCommandBuffer->Write<TRect>(aRect);
	iBoundingRect.BoundingRect(aRect);
	}
	
void CRemoteGc::CopyRect(const TPoint &anOffset, const TRect &aRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandCopyRect);
	iCommandBuffer->Write<TPoint>(anOffset);
	iCommandBuffer->Write<TRect>(aRect);
	iBoundingRect.BoundingRect(iDrawRect);
	}

void CRemoteGc::BitBlt(const TPoint &aPoint, const CFbsBitmap *aBitmap)
	{
	__ASSERT_DEBUG(aBitmap, User::Invariant());
	if(aBitmap)
		{
		iCommandBuffer->Write<TDrawCode>(ECommandBitBlt1);
		iCommandBuffer->Write<TPoint>(aPoint);
		iCommandBuffer->Write<TInt>(aBitmap->Handle());
		iBoundingRect.BoundingRect(TRect(aPoint, aBitmap->SizeInPixels()));
		iHasBitmapCommand = ETrue;
		}
	}
	
void CRemoteGc::BitBlt(const TPoint &aDestination, const CFbsBitmap *aBitmap, const TRect &aSource)
	{
	__ASSERT_DEBUG(aBitmap, User::Invariant());
	if(aBitmap)
		{	
		iCommandBuffer->Write<TDrawCode>(ECommandBitBlt2);
		iCommandBuffer->Write<TPoint>(aDestination);
		iCommandBuffer->Write<TInt>(aBitmap->Handle());
		iCommandBuffer->Write<TRect>(aSource);
		iBoundingRect.BoundingRect(TRect(aDestination, aSource.Size()));
		iHasBitmapCommand = ETrue;
		}
	}
	
void CRemoteGc::BitBltMasked(const TPoint& aPoint, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	__ASSERT_DEBUG(aBitmap && aMaskBitmap, User::Invariant());
	if(aBitmap && aMaskBitmap)
		{
		iCommandBuffer->Write<TDrawCode>(ECommandBitBltMasked);
		iCommandBuffer->Write<TPoint>(aPoint);
		iCommandBuffer->Write<TInt>(aBitmap->Handle());
		iCommandBuffer->Write<TRect>(aSourceRect);
		iCommandBuffer->Write<TInt>(aMaskBitmap->Handle());
		iCommandBuffer->Write<TBool>(aInvertMask);
		iBoundingRect.BoundingRect(TRect(aPoint, aSourceRect.Size()));
		iHasBitmapCommand = ETrue;
		}
	}

void CRemoteGc::BitBlt(const TPoint &aPoint, const CWsBitmap *aBitmap)
	{
	BitBlt(aPoint, reinterpret_cast<const CFbsBitmap*>(aBitmap));		
	}
	
void CRemoteGc::BitBlt(const TPoint &aDestination, const CWsBitmap *aBitmap, const TRect &aSource)
	{
	BitBlt(aDestination, reinterpret_cast<const CFbsBitmap*>(aBitmap), aSource);
	}
	
void CRemoteGc::BitBltMasked(const TPoint& aPoint, const CWsBitmap *aBitmap, const TRect& aSourceRect, const CWsBitmap *aMaskBitmap, TBool aInvertMask)
	{		
	BitBltMasked(aPoint, reinterpret_cast<const CFbsBitmap*>(aBitmap), aSourceRect, reinterpret_cast<const CFbsBitmap*>(aMaskBitmap), aInvertMask);
	}

void CRemoteGc::SetFaded(TBool aFaded)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetFaded);
	iCommandBuffer->Write<TBool>(aFaded);
	}
	
void CRemoteGc::SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetFadingParameters);
	iCommandBuffer->Write<TUint8>(aBlackMap);
	iCommandBuffer->Write<TUint8>(aWhiteMap);
	}
	
TInt CRemoteGc::AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandAlphaBlendBitmaps);
	iCommandBuffer->Write<TPoint>(aDestPt);
	iCommandBuffer->Write<TInt>(aSrcBmp->Handle());
	iCommandBuffer->Write<TRect>(aSrcRect);
	iCommandBuffer->Write<TInt>(aAlphaBmp->Handle());
	iCommandBuffer->Write<TPoint>(aAlphaPt);
	iBoundingRect.BoundingRect(iDrawRect);
	iHasBitmapCommand = ETrue;
	return KErrNone;
	}
	
TInt CRemoteGc::AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp, const TRect& aSrcRect, const CWsBitmap* aAlphaBmp, const TPoint& aAlphaPt)
	{
	return AlphaBlendBitmaps(aDestPt, reinterpret_cast<const CFbsBitmap*>(aSrcBmp), aSrcRect, reinterpret_cast<const CFbsBitmap*>(aAlphaBmp), aAlphaPt);
	}

void CRemoteGc::SetOrigin(const TPoint &aPoint)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetOrigin);
	iCommandBuffer->Write<TPoint>(aPoint);
	}
	
void CRemoteGc::SetDrawMode(TDrawMode aDrawingMode)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetDrawMode);
	iCommandBuffer->Write<TDrawMode>(aDrawingMode);		
	}
	
void CRemoteGc::SetClippingRect(const TRect& aRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetClippingRect);
	iCommandBuffer->Write<TRect>(aRect);		
	}
	
void CRemoteGc::CancelClippingRect()
	{
	iCommandBuffer->Write<TDrawCode>(ECommandCancelClippingRect);		
	}
	
void CRemoteGc::Reset()
	{
	iCommandBuffer->Write<TDrawCode>(ECommandReset);			
	}
	
void CRemoteGc::UseFont(const CFont *aFont)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandUseFont);
	iCommandBuffer->Write<TInt>(((CFbsFont*)aFont)->Handle());
	}
	
void CRemoteGc::DiscardFont()
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDiscardFont);
	}
	
void CRemoteGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetUnderlineStyle);
	iCommandBuffer->Write<TFontUnderline>(aUnderlineStyle);
	}
	
void CRemoteGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetStrikethroughStyle);
	iCommandBuffer->Write<TFontStrikethrough>(aStrikethroughStyle);
	}
	
void CRemoteGc::SetWordJustification(TInt aExcessWidth, TInt aNumGaps)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetWordJustification);
	iCommandBuffer->Write<TInt>(aExcessWidth);
	iCommandBuffer->Write<TInt>(aNumGaps);
	}
	
void CRemoteGc::SetCharJustification(TInt aExcessWidth, TInt aNumChars)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetCharJustification);
	iCommandBuffer->Write<TInt>(aExcessWidth);
	iCommandBuffer->Write<TInt>(aNumChars);	
	}

void CRemoteGc::SetPenColor(const TRgb &aColor)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetPenColor);
	iCommandBuffer->Write<TRgb>(aColor);		
	}
	
void CRemoteGc::SetPenStyle(TPenStyle aPenStyle)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetPenStyle);
	iCommandBuffer->Write<TPenStyle>(aPenStyle);			
	}
	
void CRemoteGc::SetPenSize(const TSize& aSize)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetPenSize);
	iCommandBuffer->Write<TSize>(aSize);			
	}

void CRemoteGc::SetBrushColor(const TRgb &aColor)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetBrushColor);
	iCommandBuffer->Write<TRgb>(aColor);
	}

void CRemoteGc::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetBrushStyle);
	iCommandBuffer->Write<TBrushStyle>(aBrushStyle);		
	}
	
void CRemoteGc::SetBrushOrigin(const TPoint &aOrigin)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetBrushOrigin);
	iCommandBuffer->Write<TPoint>(aOrigin);			
	}
	
void CRemoteGc::UseBrushPattern(const CFbsBitmap *aDevice)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandUseBrushPattern);
	iCommandBuffer->Write<TInt>(aDevice->Handle());		
	}
	
void CRemoteGc::DiscardBrushPattern()
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDiscardBrushPattern);	
	}

void CRemoteGc::MoveTo(const TPoint &aPoint)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandMoveTo);
	iCommandBuffer->Write<TPoint>(aPoint);
	}
	
void CRemoteGc::MoveBy(const TPoint &aPoint)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandMoveBy);
	iCommandBuffer->Write<TPoint>(aPoint);
	}
	
void CRemoteGc::Plot(const TPoint &aPoint)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandPlot);
	iCommandBuffer->Write<TPoint>(aPoint);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawArc);
	iCommandBuffer->Write<TRect>(aRect);
	iCommandBuffer->Write<TPoint>(aStart);
	iCommandBuffer->Write<TPoint>(aEnd);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawLine(const TPoint &aPoint1,const TPoint &aPoint2)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawLine);
	iCommandBuffer->Write<TPoint>(aPoint1);
	iCommandBuffer->Write<TPoint>(aPoint2);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawLineTo(const TPoint &aPoint)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawLineTo);
	iCommandBuffer->Write<TPoint>(aPoint);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawLineBy(const TPoint &aPoint)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawLineBy);
	iCommandBuffer->Write<TPoint>(aPoint);
	iBoundingRect.BoundingRect(iDrawRect);
	}

void CRemoteGc::DrawPolyLine(const CArrayFix<TPoint> *aPointList)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawPolyLine);
	iCommandBuffer->Write<TInt>(aPointList->Count()); // Write number of points
	
	const TInt count = aPointList->Count();
	for(TInt i = 0; i < count; i++)	
		{
		iCommandBuffer->Write<TPoint>(aPointList->At(i));
		}
	iBoundingRect.BoundingRect(iDrawRect);
	}

void CRemoteGc::DrawPolyLine(const TPoint* aPointList, TInt aNumPoints)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawPolyLine);
	iCommandBuffer->Write<TInt>(aNumPoints); // Write number of points
	
	for(TInt i = 0; i < aNumPoints; i++)	
		{
		iCommandBuffer->Write<TPoint>(aPointList[i]);
		}
	iBoundingRect.BoundingRect(iDrawRect);
	}

void CRemoteGc::DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawPie);
	iCommandBuffer->Write<TRect>(aRect);
	iCommandBuffer->Write<TPoint>(aStart);
	iCommandBuffer->Write<TPoint>(aEnd);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawEllipse(const TRect &aRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawEllipse);
	iCommandBuffer->Write<TRect>(aRect);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawRect(const TRect &aRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawRect);
	iCommandBuffer->Write<TRect>(aRect);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawRoundRect(const TRect &aRect,const TSize &aEllipse)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawRoundRect);
	iCommandBuffer->Write<TRect>(aRect);
	iCommandBuffer->Write<TSize>(aEllipse);
	iBoundingRect.BoundingRect(iDrawRect);
	}

TInt CRemoteGc::DrawPolygon(const CArrayFix<TPoint> *aPointList, TFillRule aFillRule)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawPolygon);
	iCommandBuffer->Write<TInt>(aPointList->Count()); // Write number of points
	
	for(TInt i = 0; i < aPointList->Count(); i++)	
		{
		iCommandBuffer->Write<TPoint>(aPointList->At(i));
		}
		
	iCommandBuffer->Write<TFillRule>(aFillRule);
	iBoundingRect.BoundingRect(iDrawRect);
	return KErrNone;
	}
	
TInt CRemoteGc::DrawPolygon(const TPoint* aPointList, TInt aNumPoints, TFillRule aFillRule)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawPolygon);
	iCommandBuffer->Write<TInt>(aNumPoints); // Write number of points
	
	for(TInt i = 0; i < aNumPoints; i++)	
		{
		iCommandBuffer->Write<TPoint>(aPointList[i]);
		}
		
	iCommandBuffer->Write<TFillRule>(aFillRule);
	iBoundingRect.BoundingRect(iDrawRect);
	return KErrNone;
	}

void CRemoteGc::DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aDevice)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawBitmap1);
	iCommandBuffer->Write<TPoint>(aTopLeft);
	iCommandBuffer->Write<TInt>(aDevice->Handle());
	iBoundingRect.BoundingRect(TRect(aTopLeft, aDevice->SizeInPixels()));
	iHasBitmapCommand = ETrue;
	}
	
void CRemoteGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawBitmap2);
	iCommandBuffer->Write<TRect>(aDestRect);
	iCommandBuffer->Write<TInt>(aDevice->Handle());
	iBoundingRect.BoundingRect(aDestRect);
	iHasBitmapCommand = ETrue;
	}
	
void CRemoteGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice, const TRect &aSourceRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawBitmap3);
	iCommandBuffer->Write<TRect>(aDestRect);
	iCommandBuffer->Write<TInt>(aDevice->Handle());
	iCommandBuffer->Write<TRect>(aSourceRect);
	iBoundingRect.BoundingRect(aDestRect);
	iHasBitmapCommand = ETrue;
	}
	
void CRemoteGc::DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawBitmapMasked);
	iCommandBuffer->Write<TRect>(aDestRect);
	iCommandBuffer->Write<TInt>(aBitmap->Handle());
	iCommandBuffer->Write<TRect>(aSourceRect);
	iCommandBuffer->Write<TInt>(aMaskBitmap->Handle());
	iCommandBuffer->Write<TBool>(aInvertMask);
	iBoundingRect.BoundingRect(aDestRect);
	iHasBitmapCommand = ETrue;
	}
	
void CRemoteGc::DrawBitmapMasked(const TRect& aDestRect, const CWsBitmap* aBitmap, const TRect& aSourceRect, const CWsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	DrawBitmapMasked(aDestRect, reinterpret_cast<const CFbsBitmap*>(aBitmap), aSourceRect, reinterpret_cast<const CFbsBitmap*>(aMaskBitmap), aInvertMask);
	}

void CRemoteGc::DrawText(const TDesC &aBuf,const TPoint &aPos)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawText1);
	iCommandBuffer->WriteText(aBuf);
	iCommandBuffer->Write<TPoint>(aPos);
	iBoundingRect.BoundingRect(iDrawRect);
	}

void CRemoteGc::DrawText(const TDesC &aBuf, const TRect &aBox, TInt aBaselineOffset, TTextAlign aHoriz, TInt aLeftMrg)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawText2);
	iCommandBuffer->WriteText(aBuf);
	iCommandBuffer->Write<TRect>(aBox);
	iCommandBuffer->Write<TInt>(aBaselineOffset);
	iCommandBuffer->Write<TTextAlign>(aHoriz);
	iCommandBuffer->Write<TInt>(aLeftMrg);
	iBoundingRect.BoundingRect(aBox);
	}
	
void CRemoteGc::DrawText(const TDesC& aText, const TPoint& aPosition, const TDrawTextParam& aParam)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawText3);
	iCommandBuffer->WriteText(aText);
	iCommandBuffer->Write<TPoint>(aPosition);
	iCommandBuffer->Write<TDrawTextParam>(aParam);
	iBoundingRect.BoundingRect(iDrawRect);
	}

void CRemoteGc::MapColors(const TRect& aRect, const TRgb* aColors, TInt aNumPairs, TBool aMapForwards)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandMapColors);
	iCommandBuffer->Write<TRect>(aRect);	
	iCommandBuffer->Write<TInt>(aNumPairs);
	
	for(TInt i = 0; i < aNumPairs; i++)	
		{
		iCommandBuffer->Write<TRgb>(aColors[i]);
		iCommandBuffer->Write<TRgb>(aColors[i+1]);
		}
		
	iCommandBuffer->Write<TBool>(aMapForwards);
	}

TInt CRemoteGc::SetClippingRegion(const TRegion &aRegion)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandSetClippingRegion);
	
	const TInt count = aRegion.Count();
	iCommandBuffer->Write<TInt>(count);
	
	for(TInt i = 0; i < count; i++)	
		{
		iCommandBuffer->Write<TRect>(aRegion.RectangleList()[i]);
		}
		
	return KErrNone;
	}
	
void CRemoteGc::CancelClippingRegion()
	{
	iCommandBuffer->Write<TDrawCode>(ECommandCancelClippingRegion);
	}
	
void CRemoteGc::DrawTextVertical(const TDesC& aText, const TPoint& aPos, TBool aUp)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawTextVertical1);
	iCommandBuffer->WriteText(aText);
	iCommandBuffer->Write<TPoint>(aPos);	
	iCommandBuffer->Write<TBool>(aUp);
	iBoundingRect.BoundingRect(iDrawRect);
	}
	
void CRemoteGc::DrawTextVertical(const TDesC& aText, const TRect& aBox, TInt aBaselineOffset, TBool aUp, TTextAlign aVert, TInt aMargin)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawTextVertical2);
	iCommandBuffer->WriteText(aText);
	iCommandBuffer->Write<TRect>(aBox);
	iCommandBuffer->Write<TInt>(aBaselineOffset);
	iCommandBuffer->Write<TBool>(aUp);
	iCommandBuffer->Write<TTextAlign>(aVert);
	iCommandBuffer->Write<TInt>(aMargin);
	iBoundingRect.BoundingRect(aBox);
	}
	
void CRemoteGc::DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawWsGraphic1);
	iCommandBuffer->Write<TInt>(aId.IsUid()? aId.Uid().iUid: aId.Id());
	iCommandBuffer->Write<TBool>(aId.IsUid());
	iCommandBuffer->Write<TRect>(aDestRect);
	iBoundingRect.BoundingRect(aDestRect);
	}
	
void CRemoteGc::DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect,const TDesC8& aData)
	{
	iCommandBuffer->Write<TDrawCode>(ECommandDrawWsGraphic2);
	iCommandBuffer->Write<TInt>(aId.IsUid()? aId.Uid().iUid: aId.Id());
	iCommandBuffer->Write<TBool>(aId.IsUid());
	iCommandBuffer->Write<TRect>(aDestRect);
	iCommandBuffer->WriteText(aData);
	iBoundingRect.BoundingRect(aDestRect);
	}

void CRemoteGc::SetDitherOrigin(const TPoint& /*aPoint*/)
	{
	// do nothing, does not apply to CBitmapContext which CCommandBuffer is using
	}

void CRemoteGc::SetOpaque(TBool /*aDrawOpaque*/)
	{
	// overrides to prevent calling CWindowGc::SetOpaque, it's specific to how wserv blends windows content
	}

TInt CRemoteGc::APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput)
	{
	if (aUid == KSetShadowColor)
		{
		return APIExSetShadowColor(aInput);
		}
	/* Future cases may be placed here later.*/
	else
		{
		return CBitmapContext::APIExtension(aUid, aOutput, aInput);
		}
	}

TInt CRemoteGc::APIExSetShadowColor(TAny* aShadowColor)
	{
	const TRgb shadowColor = *(reinterpret_cast<TRgb*> (aShadowColor));
	iCommandBuffer->Write<TDrawCode>(ECommandSetShadowColor);
	iCommandBuffer->Write<TRgb>(shadowColor);
	return KErrNone;
	}
