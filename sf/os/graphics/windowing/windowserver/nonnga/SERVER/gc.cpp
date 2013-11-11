// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GC and Graphics functions
// 
//

#include <e32std.h>
#include <s32mem.h> 
#include "gc.h"
#include "backedupwindow.h"
#include "windowgroup.h"
#include "ScrDev.H"
#include "wstop.h"
#include "panics.h"
#include "Graphics/WSGRAPHICDRAWER.H"
#include "wsfont.h"

CFbsBitmap *CWsGc::iScratchBitmap=NULL;
CFbsBitmap *CWsGc::iScratchMaskBitmap=NULL;

GLREF_C TInt ExternalizeRegionL(RWriteStream& aWriteStream, const RWsRegion& aRegion);

/*CWsGc*/

CWsGc* CWsGc::NewL(CWsClient *aOwner)	
	{
	CWsGc* self = new(ELeave) CWsGc(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CWsGc::InitStaticsL()
	{
	iScratchBitmap=new(ELeave) CFbsBitmap();
	iScratchMaskBitmap=new(ELeave) CFbsBitmap();
	}

void CWsGc::DeleteStatics()
	{
	delete iScratchBitmap;
	delete iScratchMaskBitmap;
	}

CWsGc::CWsGc(CWsClient *owner) : CWsObject(owner,WS_HANDLE_GC)
	{
	__DECLARE_NAME(_S("CWsGc"));
	}

void CWsGc::ConstructL()
	{
	NewObjL();
	iGdi=CFbsBitGc::NewL();
	}

void CWsGc::Activate(CWsClientWindow *win)
	{
	if (iWin!=NULL)
		{
		if (CWsClient::iCurrentCommand.iOpcode==0)
			{
			WS_PANIC_ALWAYS(EWsPanicDrawCommandsInvalidState);
			}
		else
			{
			OwnerPanic(EWservPanicGcActive);
			}
		}
	iWin=win;
	iGdi->ActivateNoJustAutoUpdate(iWin->Redraw()->OutputDevice());
	iWin->GcActivated(this);
	iGdi->SetBrushColor(iWin->BackColor());
	iOrigin.SetXY(0,0);
	ResetClippingRect();
	if(iWsOwner)
		{
		CWsWindowGroup *winGp = iWin->WinGroup();
		if(!winGp->Device())
			OwnerPanic(EWservPanicGroupWinScreenDeviceDeleted);		
		SetReply(winGp->Device()->ClientDevicePointer());			
		}	
	}

void CWsGc::Activate(const TInt &aHandle)
	{
	CWsClientWindow *win;
	iWsOwner->HandleToClientWindow(aHandle,&win);
	if (!win->BaseParent())
		OwnerPanic(EWservPanicParentDeleted);
	Activate(win);
	}

void CWsGc::Reactivate()
	{
	WS_ASSERT_DEBUG(iWin != NULL, EWsPanicDrawCommandsInvalidState);
	iGdi->ActivateNoJustAutoUpdate(iWin->Redraw()->OutputDevice());
	}

CWsGc::~CWsGc()
	{
	if (iWin!=NULL)
		Deactivate();
	delete iGdi;
	delete iPolyPoints;
	}

void CWsGc::Deactivate()
	{
	if (iWin)	// Protect against deactivating an already deactivated GC, this is allowed to aid clean up code.
		{
		CWsFontCache::Instance()->ReleaseFont(iFont);
		iGdi->Reset();
		iWin->GcDeactivated(this);
		CancelClippingRegion();
		iWin->Redraw()->GcDeactivate(this);
		iWin=NULL;
		}
	}

void CWsGc::Reset()
	{
	iGdi->Reset();
	}

void CWsGc::SetClippingRect(const TRect &aRect)
	{
	iClippingRect=aRect;
	iClippingRect.Move(iOrigin);
	iClippingRectSet=ETrue;
	}

void CWsGc::ResetClippingRect()
	{
	iClippingRectSet=EFalse;
	}

void CWsGc::SetClippingRegionL(TInt aRegionCount)
	{
	RWsRegion *newRegion=GetRegionFromClientL(iWsOwner, aRegionCount);
	CancelClippingRegion();
	iUserDefinedClippingRegion=newRegion;

	if (iUserDefinedClippingRegion)
		{
		iUserDefinedClippingRegion->Offset(iOrigin);
		}
	}

void CWsGc::CancelClippingRegion()
	{
	if (iUserDefinedClippingRegion)
		{
		iUserDefinedClippingRegion->Destroy();
		iUserDefinedClippingRegion=NULL;
		}
	}

void CWsGc::CheckPolyData(const TAny* aDataPtr, TInt aHeaderSize, TInt aNumPoints)
	{
	TInt maxDataLen;
	if (CWsClient::iCurrentCommand.iOpcode>0)
		{
		maxDataLen=CWsClient::EndOfCommandBuffer()-static_cast<const TUint8*>(aDataPtr);
		}
	else	// Playing back from redraw store
		{
		maxDataLen=CWsClient::iCurrentCommand.iCmdLength;
		}
	const TInt dataSize=aHeaderSize+aNumPoints*sizeof(TPoint);
	if (dataSize>maxDataLen)
		GcOwnerPanic(EWservPanicBadPolyData);
	}

void CWsGc::DoDrawPolygon(const TWsGcCmdDrawPolygon *aDrawPolygon)
	{
	CheckPolyData(aDrawPolygon,sizeof(TWsGcCmdDrawPolygon),aDrawPolygon->numPoints);
	iGdi->DrawPolygon((TPoint *)(aDrawPolygon+1),aDrawPolygon->numPoints,aDrawPolygon->fillRule);
	}

void CWsGc::StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon* aDrawPolygon)
	{
	if (iPolyPoints || !Rng(0, aDrawPolygon->totalNumPoints, KMaxTInt/2 - 1))	// Restarting without finishing old polygon or invalid size
		GcOwnerPanic(EWservPanicBadPolyData);
	iPolyPoints=(TPoint *)User::AllocL(aDrawPolygon->totalNumPoints*sizeof(TPoint));
	iPolyPointListSize=aDrawPolygon->totalNumPoints;
	}

void CWsGc::SegmentedDrawPolygonData(const TWsGcCmdSegmentedDrawPolygonData* aDrawPolygon)
	{
	if (aDrawPolygon->index<0 || (aDrawPolygon->index+aDrawPolygon->numPoints)>iPolyPointListSize)
		GcOwnerPanic(EWservPanicBadPolyData);
	Mem::Copy(iPolyPoints+aDrawPolygon->index,aDrawPolygon+1,aDrawPolygon->numPoints*sizeof(TPoint));
	}

void CWsGc::EndSegmentedPolygon()
	{
	delete iPolyPoints;
	iPolyPoints=NULL;
	iPolyPointListSize = 0;
	}

void CWsGc::DoDrawPolyLine(const TWsGcCmdDrawPolyLine *aDrawPolyLine, TBool aContinued)
	{
	TInt numPoints=aDrawPolyLine->numPoints;
	CheckPolyData(aDrawPolyLine,sizeof(TWsGcCmdDrawPolyLine),numPoints);
	const TPoint *points=(TPoint *)(aDrawPolyLine+1);
	if (aContinued)
		{
		numPoints++;
		points=&aDrawPolyLine->last;
		}
	if (aDrawPolyLine->more)	// more to come so don't draw the end point
		iGdi->DrawPolyLineNoEndPoint(points,numPoints);
	else
		iGdi->DrawPolyLine(points,numPoints);
	}

void CWsGc::GcOwnerPanic(TClientPanic aPanic)
	{
	iGdi->SetClippingRegion(NULL);
	EndSegmentedPolygon();
	iWin->WsOwner()->PPanic(aPanic);
	}
	
TPtrC CWsGc::BufferTPtr(TText* aStart,TInt aLen)
	{
	TPtrC gcPtr;
	if (!CWsClient::BufferTPtrGc(aStart,aLen,gcPtr))
		GcOwnerPanic(EWservPanicBufferPtr);
	return(gcPtr);
	}

void CWsGc::DoDrawCommand(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData, const TRegion *aRegion)
	{
	if (aRegion)
		{
		WS_ASSERT_DEBUG(iWin,EWsPanicWindowNull);
		if (aRegion->Count()==0)
			return;
		if (aRegion->IsContainedBy(TRect(TPoint(0,0), iGdi->Device()->SizeInPixels())))
			{
			iGdi->SetClippingRegion(aRegion);
			}
		}
	switch(aOpcode)
		{
		case EWsGcOpDrawWsGraphic:
		case EWsGcOpDrawWsGraphicPtr:
			{
			// CWsGc doesn't support CRPs.  CPlaybackGc does.  This means backedup windows
			// don't get to play with them yet.
			break;
			}
		case EWsGcOpMapColorsLocal:
			iGdi->MapColors(pData.MapColorsLocal->rect, pData.MapColorsLocal->colors,pData.MapColorsLocal->numPairs,pData.MapColorsLocal->mapForwards);
			break;
		case EWsGcOpDrawPolyLineLocalBufLen:
			iGdi->DrawPolyLine(pData.DrawPolyLineLocalBufLen->points,pData.DrawPolyLineLocalBufLen->length);
			break;
		case EWsGcOpDrawPolyLineLocal:
			iGdi->DrawPolyLine(pData.PointList);
			break;
		case EWsGcOpDrawPolygonLocalBufLen:
			iGdi->DrawPolygon(pData.DrawPolygonLocalBufLen->points,pData.DrawPolygonLocalBufLen->length,pData.DrawPolygonLocalBufLen->fillRule);
			break;
		case EWsGcOpDrawPolygonLocal:
			iGdi->DrawPolygon(pData.DrawPolygonLocal->pointList,pData.DrawPolygonLocal->fillRule);
			break;
		case EWsGcOpDrawBitmapLocal:
			iGdi->DrawBitmap(pData.BitmapLocal->pos, pData.BitmapLocal->bitmap);
			break;
		case EWsGcOpDrawBitmap2Local:
			iGdi->DrawBitmap(pData.Bitmap2Local->rect, pData.Bitmap2Local->bitmap);
			break;
		case EWsGcOpDrawBitmap3Local:
			iGdi->DrawBitmap(pData.Bitmap3Local->rect, pData.Bitmap3Local->bitmap, pData.Bitmap3Local->srcRect);
			break;
		case EWsGcOpDrawBitmapMaskedLocal:
			iGdi->DrawBitmapMasked(pData.iBitmapMaskedLocal->iRect, pData.iBitmapMaskedLocal->iBitmap, pData.iBitmapMaskedLocal->iSrcRect, pData.iBitmapMaskedLocal->iMaskBitmap,pData.iBitmapMaskedLocal->iInvertMask);
			break;
		case EWsGcOpAlphaBlendBitmapsLocal:
			iGdi->AlphaBlendBitmaps(pData.AlphaBlendBitmapsLocal->point,pData.AlphaBlendBitmapsLocal->iBitmap,
						   			pData.AlphaBlendBitmapsLocal->source, pData.AlphaBlendBitmapsLocal->iAlpha,
									pData.AlphaBlendBitmapsLocal->alphaPoint);

			break;
		case EWsGcOpDrawText:
			iGdi->DrawText(BufferTPtr((TText *)(pData.DrawText+1),pData.DrawText->length),pData.DrawText->pos);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			iGdi->DrawText(BufferTPtr((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length),pData.BoxTextO1->box,
							pData.BoxTextO1->baselineOffset,CGraphicsContext::ELeft,0);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			iGdi->DrawText(BufferTPtr((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length),pData.BoxTextO2->box,
							pData.BoxTextO2->baselineOffset,pData.BoxTextO2->horiz,pData.BoxTextO2->leftMrg);
			break;
		case EWsGcOpDrawTextPtr:
			iGdi->DrawText(*pData.DrawTextPtr->text,pData.DrawTextPtr->pos);
			break;
		case EWsGcOpDrawTextPtr1:
		   	iGdi->DrawText(*pData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawBoxText:
			iGdi->DrawText(BufferTPtr((TText *)(pData.BoxText+1),pData.BoxText->length),pData.BoxText->box,pData.BoxText->baselineOffset,pData.BoxText->width,pData.BoxText->horiz,pData.BoxText->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iGdi->DrawText(*pData.DrawBoxTextPtr->text,pData.DrawBoxTextPtr->box,pData.DrawBoxTextPtr->baselineOffset,pData.DrawBoxTextPtr->width,pData.DrawBoxTextPtr->horiz,pData.DrawBoxTextPtr->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr1:
			iGdi->DrawText(*pData.DrawBoxTextPtr->text,pData.DrawBoxTextPtr->box);
			break;
		case EWsGcOpDrawTextVertical:
			iGdi->DrawTextVertical(BufferTPtr((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length),pData.DrawTextVertical->pos
							,pData.DrawTextVertical->up);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iGdi->DrawTextVertical(*pData.DrawTextVerticalPtr->text,pData.DrawTextVerticalPtr->pos,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextVerticalPtr1:
			iGdi->DrawTextVertical(*pData.DrawTextVerticalPtr->text,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawBoxTextVertical:
			iGdi->DrawTextVertical(BufferTPtr((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length),
							pData.DrawBoxTextVertical->box,	pData.DrawBoxTextVertical->baselineOffset,
							pData.DrawBoxTextVertical->up,(CGraphicsContext::TTextAlign)pData.DrawBoxTextVertical->vert,pData.DrawBoxTextVertical->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iGdi->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->baselineOffset
							,pData.DrawBoxTextVerticalPtr->width,pData.DrawBoxTextVerticalPtr->up,pData.DrawBoxTextVerticalPtr->vert,pData.DrawBoxTextVerticalPtr->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr1:
			iGdi->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextLocal:
			iGdi->DrawText(*pData.DrawTextLocal->desc,pData.DrawTextLocal->pos);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iGdi->DrawText(*pData.BoxTextLocal->desc,pData.BoxTextLocal->box,pData.BoxTextLocal->baselineOffset,
							pData.BoxTextLocal->horiz,pData.BoxTextLocal->leftMrg);
			break;
		case EWsGcOpDrawLine:
			iGdi->DrawLine(pData.DrawLine->pnt1,pData.DrawLine->pnt2);
			break;
		case EWsGcOpDrawTo:
			iGdi->DrawLine(iLinePos,*pData.Point);
			break;
		case EWsGcOpDrawBy:
			iGdi->DrawLine(iLinePos,iLinePos+(*pData.Point));
			break;
		case EWsGcOpPlot:
			iGdi->Plot(*pData.Point);
			break;
		case EWsGcOpMoveTo:
		case EWsGcOpMoveBy:
			break;
		case EWsGcOpGdiBlt2Local:
			iGdi->BitBlt(pData.GdiBlt2Local->pos,pData.GdiBlt2Local->bitmap);
			break;
		case EWsGcOpGdiBlt3Local:
			iGdi->BitBlt(pData.GdiBlt3Local->pos,pData.GdiBlt3Local->bitmap, pData.GdiBlt3Local->rect);
			break;
		case EWsGcOpGdiBltMaskedLocal:
			iGdi->BitBltMasked(pData.GdiBltMaskedLocal->pos,pData.GdiBltMaskedLocal->bitmap,
								pData.GdiBltMaskedLocal->rect,pData.GdiBltMaskedLocal->maskBitmap,
								pData.GdiBltMaskedLocal->invertMask);
			break;
		case EWsGcOpGdiWsBlt2:
		case EWsGcOpGdiWsBlt3:
		case EWsGcOpGdiWsBltMasked:
		case EWsGcOpGdiWsAlphaBlendBitmaps:
		case EWsGcOpWsDrawBitmapMasked:
			{
			CFbsBitmap* scratchBimap=iScratchBitmap;
			CFbsBitmap* scratchMaskBimap=iScratchMaskBitmap;
			TInt maskHandle=0;
			TInt handle=WsBitmapHandle(aOpcode,pData, maskHandle);
			CWsClient* owner=iWin->WsOwner(); // We can't just use iWsOwner - it can be null for stored commands
			if (owner!=NULL)
				{
				DWsBitmap *bitmap=(DWsBitmap *)owner->HandleToObj(handle, WS_HANDLE_BITMAP);
				if (!bitmap)
					GcOwnerPanic(EWservPanicBitmap);
				scratchBimap=bitmap->FbsBitmap();
				if (aOpcode==EWsGcOpGdiWsBltMasked || aOpcode==EWsGcOpGdiWsAlphaBlendBitmaps || aOpcode==EWsGcOpWsDrawBitmapMasked)
					{
					DWsBitmap *bitmap2=(DWsBitmap *)owner->HandleToObj(maskHandle, WS_HANDLE_BITMAP);
					if (!bitmap2)
						GcOwnerPanic(EWservPanicBitmap);
					scratchMaskBimap=bitmap2->FbsBitmap();
					}
				}
			else 
				{
				GcOwnerPanic(EWservPanicBitmap);
				}
			switch(aOpcode)
				{
				case EWsGcOpGdiWsBlt2:
					iGdi->BitBlt(pData.GdiBlt2->pos,scratchBimap);
					break;
				case EWsGcOpGdiWsBlt3:
					iGdi->BitBlt(pData.GdiBlt3->pos,scratchBimap, pData.GdiBlt3->rect);
					break;
				case EWsGcOpGdiWsBltMasked:
					{
					iGdi->BitBltMasked(pData.GdiBltMasked->destination,scratchBimap,
									pData.GdiBltMasked->source, scratchMaskBimap,
									pData.GdiBltMasked->invertMask);
					}
					break;
				case EWsGcOpGdiWsAlphaBlendBitmaps:
					{
					iGdi->AlphaBlendBitmaps(pData.AlphaBlendBitmaps->point,scratchBimap,
									   pData.AlphaBlendBitmaps->source, scratchMaskBimap,
									   pData.AlphaBlendBitmaps->alphaPoint);
					}
					break;
				case EWsGcOpWsDrawBitmapMasked:
					{
					iGdi->DrawBitmapMasked(pData.iBitmapMasked->iRect,scratchBimap, 
										pData.iBitmapMasked->iSrcRect,scratchMaskBimap,
										pData.iBitmapMasked->iInvertMask);
					}
					break;
				}
			break;
			}
		case EWsGcOpGdiBlt2:
		case EWsGcOpGdiBlt3:
		case EWsGcOpGdiBltMasked:
		case EWsGcOpGdiAlphaBlendBitmaps:
		case EWsGcOpDrawBitmap:
		case EWsGcOpDrawBitmap2:
		case EWsGcOpDrawBitmap3:
		case EWsGcOpDrawBitmapMasked:
			{
			TInt maskHandle=0;
			TInt ret = iScratchBitmap->Duplicate(FbsBitmapHandle(aOpcode, pData,maskHandle));
			if(ret == KErrNoMemory)
				break;	
			if (ret !=KErrNone)
				GcOwnerPanic(EWservPanicBitmap);
			
			switch(aOpcode)
				{
				case EWsGcOpGdiBlt2:
					iGdi->BitBlt(pData.GdiBlt2->pos,iScratchBitmap);
					break;
				case EWsGcOpGdiBlt3:
					iGdi->BitBlt(pData.GdiBlt3->pos,iScratchBitmap, pData.GdiBlt3->rect);
					break;
				case EWsGcOpGdiBltMasked:
					{
					TInt ret = iScratchMaskBitmap->Duplicate(pData.GdiBltMasked->maskHandle);
					if(ret == KErrNoMemory)
						break;	
					if (ret !=KErrNone)
						GcOwnerPanic(EWservPanicBitmap);
			
					iGdi->BitBltMasked(pData.GdiBltMasked->destination,iScratchBitmap,
										pData.GdiBltMasked->source, iScratchMaskBitmap,
										 pData.GdiBltMasked->invertMask);
					iScratchMaskBitmap->Reset();
					}
					break;
				case EWsGcOpGdiAlphaBlendBitmaps:
					{
					TInt ret = iScratchMaskBitmap->Duplicate(pData.AlphaBlendBitmaps->alphaHandle);
					if (ret == KErrNoMemory)
						break;	
					if (ret != KErrNone)
						GcOwnerPanic(EWservPanicBitmap);

					iGdi->AlphaBlendBitmaps(pData.AlphaBlendBitmaps->point, iScratchBitmap,
									   pData.AlphaBlendBitmaps->source, iScratchMaskBitmap,
									   pData.AlphaBlendBitmaps->alphaPoint);
					iScratchMaskBitmap->Reset();
					break;
					}
				case EWsGcOpDrawBitmap:
					iGdi->DrawBitmap(pData.Bitmap->pos, iScratchBitmap);
					break;
				case EWsGcOpDrawBitmap2:
					iGdi->DrawBitmap(pData.Bitmap2->rect, iScratchBitmap);
					break;
				case EWsGcOpDrawBitmap3:
					iGdi->DrawBitmap(pData.Bitmap3->rect, iScratchBitmap, pData.Bitmap3->srcRect);
					break;
				case EWsGcOpDrawBitmapMasked:
					{
					TInt ret = iScratchMaskBitmap->Duplicate(pData.iBitmapMasked->iMaskHandle);
					if (ret == KErrNoMemory)
						break;	
					if (ret != KErrNone)
						GcOwnerPanic(EWservPanicBitmap);

					iGdi->DrawBitmapMasked(pData.iBitmapMasked->iRect, iScratchBitmap, 
										pData.iBitmapMasked->iSrcRect, iScratchMaskBitmap,
										pData.iBitmapMasked->iInvertMask);
					iScratchMaskBitmap->Reset();
					}
					break;
				}
			iScratchBitmap->Reset();
			break;
			}
		case EWsGcOpDrawSegmentedPolygon:
			iGdi->DrawPolygon(iPolyPoints,iPolyPointListSize,pData.DrawSegmentedPolygon->fillRule);
			break;
		case EWsGcOpDrawPolygon:
			DoDrawPolygon(pData.Polygon);
			break;
		case EWsGcOpDrawPolyLine:
			DoDrawPolyLine(pData.PolyLine, EFalse);
			break;
		case EWsGcOpDrawPolyLineContinued:
			DoDrawPolyLine(pData.PolyLine, ETrue);
			break;
		case EWsGcOpClear:
			iGdi->Clear(TRect(iWin->Size()));
			break;
		case EWsGcOpClearRect:
			iGdi->Clear(*pData.Rect);
			break;
		case EWsGcOpDrawRect:
			iGdi->DrawRect(*pData.Rect);
			break;
		case EWsGcOpDrawEllipse:
			iGdi->DrawEllipse(*pData.Rect);
			break;
		case EWsGcOpDrawRoundRect:
			iGdi->DrawRoundRect(*pData.Rect,pData.RoundRect->ellipse);
			break;
		case EWsGcOpDrawArc:
			iGdi->DrawArc(*pData.Rect,pData.ArcOrPie->start,pData.ArcOrPie->end);
			break;
		case EWsGcOpDrawPie:
			iGdi->DrawPie(*pData.Rect,pData.ArcOrPie->start,pData.ArcOrPie->end);
			break;
		case EWsGcOpCopyRect:
			iGdi->CopyRect(pData.CopyRect->pos,*pData.Rect);
			break;
		case EWsGcOpMapColors:
			iGdi->MapColors(pData.MapColors->rect,(TRgb *)(pData.MapColors+1),pData.MapColors->numPairs,pData.MapColors->mapForwards);
			break;
			
		default:
			GcOwnerPanic(EWservPanicOpcode);
		}
	iGdi->SetClippingRegion(NULL);
	}

TInt CWsGc::WsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle) 
	{
	TInt handle=0;
	switch(aOpcode)
		{
		case EWsGcOpGdiWsBlt2:
			handle=pData.GdiBlt2->handle;
			break;
		case EWsGcOpGdiWsBlt3:
			handle=pData.GdiBlt3->handle;
			break;
		case EWsGcOpGdiWsBltMasked:
			handle=pData.GdiBltMasked->handle;
			aMaskHandle = pData.GdiBltMasked->maskHandle;
			break;
		case EWsGcOpGdiWsAlphaBlendBitmaps:
			handle=pData.AlphaBlendBitmaps->bitmapHandle;
			aMaskHandle = pData.AlphaBlendBitmaps->alphaHandle;
			break;
		case EWsGcOpWsDrawBitmapMasked:
			handle=pData.iBitmapMasked->iHandle;
			aMaskHandle=pData.iBitmapMasked->iMaskHandle;
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
		}
	return handle;
	}		

TInt CWsGc::FbsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle)
	{
	TInt handle=0;
	aMaskHandle=0;
	switch(aOpcode)
		{
		case EWsGcOpGdiBlt2:
			handle=pData.GdiBlt2->handle;
			break;
		case EWsGcOpGdiBlt3:
			handle=pData.GdiBlt3->handle;
			break;
		case EWsGcOpGdiBltMasked:
			handle=pData.GdiBltMasked->handle;
			aMaskHandle=pData.GdiBltMasked->maskHandle;
			break;
		case EWsGcOpGdiAlphaBlendBitmaps:
			handle=pData.AlphaBlendBitmaps->bitmapHandle;
			aMaskHandle=pData.AlphaBlendBitmaps->alphaHandle;
			break;
		case EWsGcOpDrawBitmap:
			handle=pData.Bitmap->handle;
			break;
		case EWsGcOpDrawBitmap2:
			handle=pData.Bitmap2->handle;
			break;
		case EWsGcOpDrawBitmap3:
			handle=pData.Bitmap3->handle;
			break;
		case EWsGcOpDrawBitmapMasked:
			handle=pData.iBitmapMasked->iHandle;
			aMaskHandle=pData.iBitmapMasked->iMaskHandle;
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
		}
	return handle;
	}


void CWsGc::UpdateJustification(TText* aText,TInt aLen)
	{
	iGdi->UpdateJustification(BufferTPtr(aText,aLen));
	}

void CWsGc::DoDrawing2(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData)
	{
	iGdi->SetUserDisplayMode(iWin->DisplayMode());
	if (iClippingRectSet)
		{
		iGdi->SetOrigin(TPoint(0,0));
		iGdi->SetClippingRect(iClippingRect);
		}
	iGdi->SetOrigin(iOrigin);
	
	DoDrawCommand(aOpcode,pData,iUserDefinedClippingRegion);

	iGdi->SetUserDisplayMode(ENone);
	iGdi->CancelClippingRect();
	switch(aOpcode)
		{
		case EWsGcOpDrawLine:
			iLinePos=pData.DrawLine->pnt2;
			break;
		case EWsGcOpDrawTo:
		case EWsGcOpMoveTo:
		case EWsGcOpPlot:
			iLinePos=(*pData.Point);
			break;
		case EWsGcOpDrawBy:
		case EWsGcOpMoveBy:
			iLinePos+=(*pData.Point);
			break;
		case EWsGcOpDrawSegmentedPolygon:
			EndSegmentedPolygon();
			break;
		case EWsGcOpDrawText:
			UpdateJustification((TText *)(pData.DrawText+1),pData.DrawText->length);
			break;
		case EWsGcOpDrawTextVertical:
			UpdateJustification((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length);
			break;
		case EWsGcOpDrawBoxText:
			UpdateJustification((TText *)(pData.BoxText+1),pData.BoxText->length);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			UpdateJustification((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			UpdateJustification((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length);
			break;
		case EWsGcOpDrawBoxTextVertical:
			UpdateJustification((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length);
			break;
		case EWsGcOpDrawTextLocal:
			iGdi->UpdateJustification(*pData.DrawTextLocal->desc);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iGdi->UpdateJustification(*pData.BoxTextLocal->desc);
			break;
		case EWsGcOpDrawTextPtr:
			iGdi->UpdateJustification(*pData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iGdi->UpdateJustification(*pData.DrawTextVerticalPtr->text);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iGdi->UpdateJustification(*pData.DrawBoxTextPtr->text);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iGdi->UpdateJustification(*pData.DrawBoxTextVerticalPtr->text);
			break;
		}
	}

void CWsGc::DoDrawing1(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData)
	{
	TWsGcLargeStruct newData;
	TWsGcCmdUnion pNewData;
	TWsGcOpcodes opcode;
	TDesC **string;
	TInt toGo;
	pNewData.LargeStruct=&newData;
	switch (aOpcode)
		{
		case EWsGcOpDrawTextPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdDrawTextPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawTextPtr1;
			toGo=pData.DrawText->length;
			pNewData.DrawTextPtr->pos=pData.DrawText->pos;
			string=&(pNewData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdDrawTextVerticalPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawTextVerticalPtr1;
			toGo=pData.DrawTextVertical->length;
			pNewData.DrawTextVerticalPtr->pos=pData.DrawTextVertical->pos;
			pNewData.DrawTextVerticalPtr->up=pData.DrawTextVertical->up;
			string=&(pNewData.DrawTextVerticalPtr->text);
			break;
			
		case EWsGcOpDrawBoxTextPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdBoxTextPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawBoxTextPtr1;
			toGo=pData.BoxText->length;
			pNewData.DrawBoxTextPtr->box=pData.BoxText->box;
			pNewData.DrawBoxTextPtr->baselineOffset=pData.BoxText->baselineOffset;
			pNewData.DrawBoxTextPtr->horiz=pData.BoxText->horiz;
			pNewData.DrawBoxTextPtr->leftMrg=pData.BoxText->leftMrg;
			pNewData.DrawBoxTextPtr->width=pData.BoxText->width;
			string=&(pNewData.DrawBoxTextPtr->text);
			break;
			
		case EWsGcOpDrawBoxTextVerticalPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdBoxTextVerticalPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawBoxTextVerticalPtr1;
			toGo=pData.DrawBoxTextVertical->length;
			pNewData.DrawBoxTextVerticalPtr->box=pData.DrawBoxTextVertical->box;
			pNewData.DrawBoxTextVerticalPtr->baselineOffset=pData.DrawBoxTextVertical->baselineOffset;
			pNewData.DrawBoxTextVerticalPtr->up=pData.DrawBoxTextVertical->up;
			pNewData.DrawBoxTextVerticalPtr->vert=pData.DrawBoxTextVertical->vert;
			pNewData.DrawBoxTextVerticalPtr->margin=pData.DrawBoxTextVertical->margin;
			pNewData.DrawBoxTextVerticalPtr->width=pData.DrawBoxTextVertical->width;
			string=&(pNewData.DrawBoxTextVerticalPtr->text);
			break;

		default:
			DoDrawing2(aOpcode,pData);
			return;
		}

	TBuf<ETextPtrBufLen> buf;
	TInt len=ETextPtrBufLen;
	TInt bufOffset=0;
	*string=&buf;
	while(toGo>0)
		{
		if (len>toGo)
			len=toGo;
		iWsOwner->RemoteRead(buf,bufOffset);
		DoDrawing2(aOpcode,pNewData);
		aOpcode=opcode;
		bufOffset+=len;
		toGo-=len;
		}
	}

void CWsGc::SetGcAttribute(TInt aOpcode, TWsGcCmdUnion pData)
	{
	switch(aOpcode)
		{
		case EWsGcOpSetDrawMode:
			iGdi->SetDrawMode((CGraphicsContext::TDrawMode)*pData.UInt);
			break;
		case EWsGcOpUseFont:
			if (CWsFontCache::Instance()->UseFont(iFont, *pData.UInt))
				{	// Couldn't cache it
				TInt ret = iGdi->UseFont(*pData.UInt);
				if (ret == KErrNoMemory)
					return;
				if (ret != KErrNone)
					OwnerPanic(EWservPanicFont);
				}
			else
				{
				if (iFont==NULL)
					OwnerPanic(EWservPanicFont);
				iGdi->UseFontNoDuplicate(iFont);
				}
			break;
		case EWsGcOpDiscardFont:
			CWsFontCache::Instance()->ReleaseFont(iFont);
			iGdi->DiscardFont();
			break;
		case EWsGcOpSetUnderlineStyle:
			iGdi->SetUnderlineStyle(*pData.SetUnderlineStyle);
			break;
		case EWsGcOpSetStrikethroughStyle:
			iGdi->SetStrikethroughStyle(*pData.SetStrikethroughStyle);
			break;
		case EWsGcOpUseBrushPattern:
			{
			TInt ret = iGdi->UseBrushPattern(*pData.handle);
			if (ret == KErrNoMemory)
				return;	
			if (ret != KErrNone)
				OwnerPanic(EWservPanicBitmap);
			}
			break;
		case EWsGcOpDiscardBrushPattern:
			iGdi->DiscardBrushPattern();
			break;
		case EWsGcOpSetBrushColor:
			iGdi->SetBrushColor(*pData.rgb);
			break;
		case EWsGcOpSetPenColor:
			iGdi->SetPenColor(*pData.rgb);
			break;
		case EWsGcOpSetPenStyle:
			iGdi->SetPenStyle((CGraphicsContext::TPenStyle)*pData.UInt);
			break;
		case EWsGcOpSetPenSize:
			iGdi->SetPenSize(*pData.Size);
			break;
		case EWsGcOpSetBrushStyle:
			if ((CGraphicsContext::TBrushStyle)*pData.UInt==CGraphicsContext::EPatternedBrush &&
				!iGdi->IsBrushPatternUsed())
				OwnerPanic(EWservPanicNoBrush);
			iGdi->SetBrushStyle((CGraphicsContext::TBrushStyle)*pData.UInt);
			break;
		case EWsGcOpReset:
			CWsFontCache::Instance()->ReleaseFont(iFont);
			iGdi->Reset();
			iOrigin.SetXY(0,0);
			ResetClippingRect();
			iGdi->SetBrushColor(iWin->BackColor());
			break;
		case EWsGcOpSetBrushOrigin:
			iGdi->SetBrushOrigin(*pData.Point);
			break;
		case EWsGcOpSetDitherOrigin:
			iGdi->SetDitherOrigin(*pData.Point);
			break;
		case EWsGcOpSetWordJustification:
			iGdi->SetWordJustification(pData.SetJustification->excessWidth, pData.SetJustification->numGaps);
			break;
		case EWsGcOpSetCharJustification:
			iGdi->SetCharJustification(pData.SetJustification->excessWidth, pData.SetJustification->numGaps);
			break;
		case EWsGcOpSetOrigin:
			SetOrigin(*pData.Point);
			break;
		case EWsGcOpSetOpaque:
			//SetOpaque(*pData.Bool);
			break;
		case EWsGcOpSetShadowColor:
			iGdi->SetShadowColor(*pData.rgb);
			break;
		}
	}

void CWsGc::DoDrawing0L(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData)
	{
	if (iWin==NULL)
		{
		OwnerPanic(EWservPanicGcNotActive);
		return;
		}
			
	switch(aOpcode)
		{
	case EWsGcOpStartSegmentedDrawPolygon:
		// Andy - why are these two special cased like this?
		//tell Redraw Store about the drawing data, so that it is stored and CWsGc::ExternalizeL can be called if required
		if (iWin->Redraw()->DrawCommand(this,pData.any))
			StartSegmentedDrawPolygonL(pData.StartSegmentedDrawPolygon);
		return;
	case EWsGcOpSegmentedDrawPolygonData:
		//tell Redraw Store about the drawing data, so that it is stored and CWsGc::ExternalizeL can be called if required
		if (iWin->Redraw()->DrawCommand(this,pData.any))
				SegmentedDrawPolygonData(pData.SegmentedDrawPolygonData);
		return;
	case EWsGcOpSetClippingRegion:
		SetClippingRegionL(*pData.Int);
		break;
	case EWsGcOpSetClippingRect:
		SetClippingRect(*pData.Rect);
		break;
	case EWsGcOpCancelClippingRect:
		ResetClippingRect();
		break;
	case EWsGcOpCancelClippingRegion:
		CancelClippingRegion();
		break;
	case EWsGcOpSetFaded:
		iGdi->SetFaded(*pData.Bool);
		break;
	case EWsGcOpSetFadeParams:
		iGdi->SetFadingParameters(WservEncoding::ExtractFirst8BitValue(*pData.UInt),WservEncoding::ExtractSecond8BitValue(*pData.UInt));
		break;
	case EWsGcOpSetDrawMode:
	case EWsGcOpUseFont:
	case EWsGcOpDiscardFont:
	case EWsGcOpUseBrushPattern:
	case EWsGcOpDiscardBrushPattern:
	case EWsGcOpSetBrushColor:
	case EWsGcOpSetPenColor:
	case EWsGcOpSetPenStyle:
	case EWsGcOpSetPenSize:
	case EWsGcOpSetBrushStyle:
	case EWsGcOpReset:
	case EWsGcOpSetBrushOrigin:
	case EWsGcOpSetDitherOrigin:
	case EWsGcOpSetUnderlineStyle:
	case EWsGcOpSetStrikethroughStyle:
	case EWsGcOpSetWordJustification:
	case EWsGcOpSetCharJustification:
	case EWsGcOpSetOrigin:
	case EWsGcOpSetOpaque:
	case EWsGcOpSetShadowColor:
		{
		SetGcAttribute(aOpcode,pData);
		break;
		}
	case EWsGcOpDrawBoxText:	
	case EWsGcOpDrawBoxTextOptimised1:
	case EWsGcOpDrawBoxTextOptimised2:
	case EWsGcOpDrawBoxTextPtr:
	case EWsGcOpDrawBoxTextPtr1:
	case EWsGcOpDrawTextPtr:
	case EWsGcOpDrawTextPtr1:
	case EWsGcOpDrawText:
	case EWsGcOpDrawTextVertical:
	case EWsGcOpDrawTextVerticalPtr:
	case EWsGcOpDrawTextVerticalPtr1:
	case EWsGcOpDrawBoxTextVertical:
	case EWsGcOpDrawBoxTextVerticalPtr:
	case EWsGcOpDrawBoxTextVerticalPtr1:
	case EWsGcOpDrawTextLocal:
	case EWsGcOpDrawBoxTextLocal:
		{
		//Make sure a font is set before any text related opcodes are used.
		if (!iGdi->IsFontUsed())
			OwnerPanic(EWservPanicNoFont);
		//fall through
		}
	default:	// Assume remaining functions will draw
		{
		if (iWin->WinType()!=EWinTypeRoot)
			{
			if (!iWin->BaseParent())
				OwnerPanic(EWservPanicParentDeleted);
			if (iWin->WinType()!=EWinTypeClient)
				OwnerPanic(EWservPanicReadOnlyDrawable);
			}
		if (iWin->Redraw()->DrawCommand(this,pData.any))
			DoDrawing1(aOpcode,pData);
		return;
		}
		}
	iWin->Redraw()->GcAttributeChange(this,pData.any);
	}

void CWsGc::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	TWsGcOpcodes opcode = static_cast<TWsGcOpcodes>(aOpcode);

	TWsGcCmdUnion pData;
	pData.any=aCmdData;
	switch(opcode)
		{
		case EWsGcOpActivate:
			Activate(*pData.handle);
			break;
		case EWsGcOpDeactivate:
			Deactivate();
			break;
		case EWsGcOpFree:
			delete this;
			break;
		case EWsGcOpTestInvariant:
			break;
		default:
			DoDrawing0L(opcode,pData);
			break;
		}
	}

void CWsGc::SetOrigin(const TPoint &aOrigin)
	{
	iOrigin=aOrigin;
	}
	

void CWsGc::SetOpaque(TBool /*aDrawOpaque*/)
	{
	}

/*------------------------------------------------------------------------------
  Description: Saves graphics context information into a given buffer from a
               given start position rather than just streaming data to the end.
               This variant allows for buffers that are not fully utilised.
 -----------------------------------------------------------------------------*/
TInt CWsGc::ExternalizeL(CBufBase& aBuffer, TInt aStartPos)
	{
	WS_ASSERT_DEBUG(!IsPolyPointData(), EWsPanicDrawCommandsInvalidState);

	// Open the stream used for the output from the given start position
	// in the buffer.
	RBufWriteStream bufWriteStream;
	bufWriteStream.Open(aBuffer, aStartPos);
	CleanupClosePushL(bufWriteStream);

	// Font/Bitmap Server data is serialised below in a call to 
	// CFbsBitGc::ExternalizeL(). As this method does not return the amount of
	// the data externalised we use methods in the underlying stream to 
	// calculate it. We do this because we need to return an accurate count of
	// data serialised from this method so that the caller can update its 
	// buffer write counter.
	MStreamBuf* ptrToRawStream = bufWriteStream.Sink();	// This is the real stream

	// Position of read seek pointer before externalise
	TStreamPos size1 = ptrToRawStream->TellL(MStreamBuf::EWrite); 

	// Save the font/bitmap server data
	iGdi->ExternalizeL(bufWriteStream);

	bufWriteStream.WriteInt32L(iOrigin.iX);
	bufWriteStream.WriteInt32L(iOrigin.iY);

	bufWriteStream.WriteInt8L(iClippingRectSet);

	// If there is a clipping rectangle output that too.
	if (iClippingRectSet)
		{
		bufWriteStream << iClippingRect;
		}
		
	// Save clipping region data.
	ExternalizeClippingRegionL(bufWriteStream);

	// Save the Alpha values for Brush and Pen colors.
	ExternalizeAlphaValueL(bufWriteStream);

	// Position of read seek pointer after externalise
	TStreamPos size2 = ptrToRawStream->TellL(MStreamBuf::EWrite);
	CleanupStack::PopAndDestroy(&bufWriteStream);

	// Return actual size of data serialized
	return (size2 - size1);
	}

/*------------------------------------------------------------------------------
  Description: Saves TRgb::alpha value information into a given buffer.
  ----------------------------------------------------------------------------*/
void CWsGc::ExternalizeAlphaValueL(RWriteStream& aWriteStream)
	{
	aWriteStream.WriteUint8L(iGdi->BrushColor().Alpha());
	aWriteStream.WriteUint8L(iGdi->PenColor().Alpha());
	}

/*------------------------------------------------------------------------------
  Description: Helper method to store clipping region data to a given
               write stream.
 -----------------------------------------------------------------------------*/
TInt CWsGc::ExternalizeClippingRegionL(RWriteStream& aWriteStream)
	{
	TBool clipRegion = (iUserDefinedClippingRegion != NULL);
	// Store flag to indicate if client has defined a clipping region
	aWriteStream.WriteInt8L(clipRegion);
	// Store client clipping region data if it exists
	if (clipRegion)
		{
		return ExternalizeRegionL(aWriteStream, *iUserDefinedClippingRegion) + sizeof(TInt8);
		}
	return sizeof(TInt8);
	}

