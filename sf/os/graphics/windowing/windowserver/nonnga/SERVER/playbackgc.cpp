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
// GC.CPP
// GC and Graphics functions
// 
//

#include "playbackgc.h"

#include <e32std.h>
#include <s32mem.h> 

#include "backedupwindow.h"
#include "panics.h"
#include "ScrDev.H"
#include "windowgroup.h"
#include "wsfont.h"
#include "wstop.h"
#include "Graphics/WSGRAPHICDRAWER.H"

#if defined(__WINS__) && defined(_DEBUG)
#   include "offscreenbitmap.h"
#	define DEBUGOSB { CWsOffScreenBitmap * ofb = iWin->Screen()->OffScreenBitmap(); if (ofb) ofb->Update(); }
#else
#	define DEBUGOSB
#endif

CPlaybackGc * CPlaybackGc::iSelf=NULL;

GLREF_C RWsRegion* InternalizeRegionL(RReadStream& aReadStream);

/*CPlaybackGc*/

void CPlaybackGc::InitStaticsL()
	{
	iSelf=new(ELeave) CPlaybackGc();
	iSelf->ConstructL();
	}

void CPlaybackGc::DeleteStatics()
	{
	delete iSelf;
	iSelf = 0;
	}

CPlaybackGc::CPlaybackGc()
	{
	}

void CPlaybackGc::ConstructL()
	{
	iSelf->iScratchBitmap=new(ELeave) CFbsBitmap();
	iSelf->iScratchMaskBitmap=new(ELeave) CFbsBitmap();
	iGcBuf = CBufSeg::NewL(512);
	}

CPlaybackGc::~CPlaybackGc()
	{
	delete iPolyPoints;
	delete iGcBuf;
	iCurrentClippingRegion = NULL;
	delete iScratchBitmap;
	delete iScratchMaskBitmap;
	}

void CPlaybackGc::Activate(CWsClientWindow * aWin, CFbsBitGc * aGc, const TRegion * aRegion)
	{
	iWin = aWin;
	iGc = aGc;
	iTargetRegion = aRegion;
	
	iDrawRegion = iTargetRegion;
	iMasterOrigin = iWin->Origin();
	iOrigin.SetXY(0,0);
	iGc->SetBrushColor(iWin->BackColor());
	ResetClippingRect();
	}
	
void CPlaybackGc::Deactivate()
	{
	iWin = 0;
	iGc = 0;
	iTargetRegion = 0;
	iDrawRegion = 0;
	CancelUserClippingRegion();
	}
	
void CPlaybackGc::CancelUserClippingRegion()
	{
	if (iUserDefinedClippingRegion)
		{
		iUserDefinedClippingRegion->Destroy();
		iUserDefinedClippingRegion = 0;
		iDrawRegion = iTargetRegion;
		}
	}
	
void CPlaybackGc::SetClippingRect(const TRect &aRect)
	{
	iClippingRect=aRect;
	iClippingRect.Move(iOrigin);
	iClippingRectSet=ETrue;
	}

void CPlaybackGc::ResetClippingRect()
	{
	iClippingRectSet=EFalse;
	}

void CPlaybackGc::CheckPolyData(const TAny* aDataPtr, TInt aHeaderSize, TInt aNumPoints)
	{
	TInt maxDataLen;
	if (CWsClient::iCurrentCommand.iOpcode>0)
		{
		maxDataLen=CWsClient::EndOfCommandBuffer()-static_cast<const TUint8*>(aDataPtr);
		}
	else
		{
		maxDataLen=CWsClient::iCurrentCommand.iCmdLength;
		}
	const TInt dataSize=aHeaderSize+aNumPoints*sizeof(TPoint);
	if (dataSize>maxDataLen)
		GcOwnerPanic(EWservPanicBadPolyData);
	}

void CPlaybackGc::DoDrawPolygon(const TWsGcCmdDrawPolygon *aDrawPolygon)
	{
	CheckPolyData(aDrawPolygon, sizeof(TWsGcCmdDrawPolygon), aDrawPolygon->numPoints);
	iGc->DrawPolygon((TPoint *)(aDrawPolygon+1),aDrawPolygon->numPoints,aDrawPolygon->fillRule);
	}

void CPlaybackGc::StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon *aDrawPolygon)
	{
	// In case a Playback have been done before all the segment is in the RedrawStore
	// (This allocation is deleted only thanks to the EWsGcOpDrawSegmentedPolygon opcode
	//  which arrive after all the segments)
	delete iPolyPoints;
	iPolyPoints=NULL;	

	iPolyPoints=(TPoint *)User::AllocL(aDrawPolygon->totalNumPoints*sizeof(TPoint));
	iPolyPointListSize=aDrawPolygon->totalNumPoints;
	}

void CPlaybackGc::SegmentedDrawPolygonData(const TWsGcCmdSegmentedDrawPolygonData *aDrawPolygon)
	{
	if (aDrawPolygon->index<0 || (aDrawPolygon->index + aDrawPolygon->numPoints) > iPolyPointListSize)
		GcOwnerPanic(EWservPanicBadPolyData);
	Mem::Copy(iPolyPoints+aDrawPolygon->index,aDrawPolygon+1,aDrawPolygon->numPoints*sizeof(TPoint));
	}

void CPlaybackGc::EndSegmentedPolygon()
	{
	delete iPolyPoints;
	iPolyPoints=NULL;
	}

void CPlaybackGc::DoDrawPolyLine(const TWsGcCmdDrawPolyLine *aDrawPolyLine, TBool aContinued)
	{
	TInt numPoints=aDrawPolyLine->numPoints;
	CheckPolyData(aDrawPolyLine, sizeof(TWsGcCmdDrawPolyLine), numPoints);
	const TPoint *points=(TPoint *)(aDrawPolyLine+1);
	if (aContinued)
		{
		numPoints++;
		points=&aDrawPolyLine->last;
		}
	if (aDrawPolyLine->more)	// more to come so don't draw the end point
		iGc->DrawPolyLineNoEndPoint(points,numPoints);
	else
		iGc->DrawPolyLine(points,numPoints);
	}

void CPlaybackGc::GcOwnerPanic(TClientPanic aPanic)
	{
	iGc->SetClippingRegion(NULL);
	iCurrentClippingRegion = NULL;
	EndSegmentedPolygon();
	iWin->WsOwner()->PPanic(aPanic);
	}

// implementing MWsGc

MWsClient& CPlaybackGc::Client()
	{
	return *(iWin->WsOwner());
	}

MWsScreen& CPlaybackGc::Screen()
	{
	return *(iWin->Screen());
	}

TPoint CPlaybackGc::GcOrigin() const
	{
	return (iMasterOrigin + iOrigin);
	}

const TRegion& CPlaybackGc::ClippingRegion()
	{
	WS_ASSERT_ALWAYS(iCurrentClippingRegion,EWsPanicDrawCommandsInvalidState);
	return* iCurrentClippingRegion;
	}

CFbsBitGc& CPlaybackGc::BitGc()
	{
	return *iGc;
	}
	
TInt CPlaybackGc::PushBitGcSettings()
	{
	// the buf format is len+data where data is written by the GC's ExternalizeL()
	CBufBase& buf = *iGcBuf;
	const TInt start = buf.Size();
	RBufWriteStream out(buf,start);
	TRAPD(err,out.WriteInt32L(0));
	if(!err)
		{
		TRAP(err,iGc->ExternalizeL(out));
		}
	if(err) //rollback addition
		{
		buf.Delete(start,buf.Size()-start);
		}
	else //fixup len
		{
		TRAP_IGNORE(out.CommitL();) // can't see this failing
		TPckgBuf<TInt32> pckg(buf.Size()-sizeof(TInt32)-start);
		buf.Write(start,pckg);
		}
	return err;
	}

void CPlaybackGc::PopBitGcSettings()
	{
	CBufBase& buf = *iGcBuf;
	TInt ofs = 0;
	FOREVER
		{
		TInt chunk = 0;
		RBufReadStream in(buf,ofs);
		TRAPD(err,chunk = in.ReadInt32L());
		if(err)
			{
			WS_ASSERT_DEBUG(err != 0, EWsPanicWsGraphic);
			return;
			}
		if(ofs+sizeof(TInt32)+chunk >= buf.Size()) // the last chunk?
			{
			TRAP_IGNORE(iGc->InternalizeL(in));
			buf.Delete(ofs,buf.Size()-ofs);
			return;
			}
		ofs += chunk + sizeof(TInt32);
		}
	}

const TTime& CPlaybackGc::Now() const
	{
	return iWin->Screen()->Now();
	}
	
void CPlaybackGc::ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow)
	{
	ScheduleAnimation(aRect,aFromNow,0,0);
	}

void CPlaybackGc::ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop)
	{
	// convert window rect to screen rect
	TRect rect(aRect);
	rect.Move(GcOrigin());
	// clip rect to window extent
	rect.Intersection(iWin->Abs());
	if (!rect.IsEmpty())
		{
		// and schedule it
		iWin->Screen()->ScheduleAnimation(rect,aFromNow,aFreq,aStop);
		}
	}

void CPlaybackGc::SetGcOrigin(const TPoint& aOrigin) 
	{ 
	iOrigin = aOrigin - iMasterOrigin; 
	} 


void CPlaybackGc::RemoteReadDataAndDrawL(const CWsGraphicDrawer* aGraphic, CWsClient* aOwner, const TWsGcCmdUnion &aData)
	{
	TPtrC8 data;
	HBufC8* dataBuf = NULL;
	const TInt len = aData.WsGraphic->iDataLen;
	
	if ((len >= KMaxTInt / 4) || (len < 0))
		{
		aOwner->PPanic(EWservPanicBuffer);	
		}	
	dataBuf = HBufC8::NewLC(len);
	TPtr8 des = dataBuf->Des();
	aOwner->RemoteRead(des, 0);

	if(des.Size() != len)
		{
		aOwner->PPanic(EWservPanicBuffer);
		}
	data.Set(des);																
	aGraphic->Draw(*this, aData.WsGraphic->iRect, data);
	CleanupStack::PopAndDestroy(dataBuf);		
	}

TPtrC CPlaybackGc::BufferTPtr(TText* aStart,TInt aLen, const TDesC8& aCmdData)
	{
	if ((reinterpret_cast<TUint8*>(aStart) < aCmdData.Ptr()
									|| reinterpret_cast<TUint8*>(aStart+aLen) > (aCmdData.Ptr() + aCmdData.Size()) ))
		{
		GcOwnerPanic(EWservPanicBufferPtr);
		}
	TPtrC gcPtr;
	gcPtr.Set(aStart,aLen);
	return(gcPtr);
	} 

void CPlaybackGc::DoDrawCommand(TWsGcOpcodes aOpcode, const TDesC8& aCmdData, const TRegion *aRegion)
	{
	TWsGcCmdUnion pData;
	pData.any=aCmdData.Ptr();
	
	if (aRegion)
		{
		WS_ASSERT_DEBUG(iWin,EWsPanicWindowNull);
		if (aRegion->Count()==0)
			return;
		iGc->SetClippingRegion(aRegion);
		WS_ASSERT_DEBUG(!iCurrentClippingRegion, EWsPanicDrawCommandsInvalidState);
		iCurrentClippingRegion = aRegion;
		}
	switch(aOpcode)
		{
		case EWsGcOpDrawWsGraphic:
		case EWsGcOpDrawWsGraphicPtr:
			{
			TRect screenRect(pData.WsGraphic->iRect);
			screenRect.Move(GcOrigin());
			if(iCurrentClippingRegion->Intersects(screenRect))
				{
				const TInt dataLen = pData.WsGraphic->iDataLen;
				TGraphicDrawerId id;
				id.iId = pData.WsGraphic->iId;
				id.iIsUid = (pData.WsGraphic->iFlags & EWsGraphicIdUid);
				CWsClient* owner = iWin->WsOwner();
				const CWsGraphicDrawer* graphic = owner->WindowServer().ResolveGraphic(id);
				if(graphic && graphic->IsSharedWith(owner->SecureId()))
					{
					if(aOpcode == EWsGcOpDrawWsGraphicPtr)
						{
						TRAPD(err, RemoteReadDataAndDrawL(graphic, owner, pData))
						if(err)
							WS_PANIC_DEBUG(EWsPanicWsGraphic);
						}
					else
						graphic->Draw(*this,pData.WsGraphic->iRect,CWsClient::BufferTPtr8((TUint8*)(pData.WsGraphic+1),dataLen));
	
					WS_ASSERT_DEBUG(!iGcBuf->Size(),EWsPanicWsGraphic);
					iGcBuf->Reset();
					}
				}
			break;
			}
		case EWsGcOpMapColorsLocal:
			iGc->MapColors(pData.MapColorsLocal->rect, pData.MapColorsLocal->colors,pData.MapColorsLocal->numPairs,pData.MapColorsLocal->mapForwards);
			break;
		case EWsGcOpDrawPolyLineLocalBufLen:
			iGc->DrawPolyLine(pData.DrawPolyLineLocalBufLen->points,pData.DrawPolyLineLocalBufLen->length);
			break;
		case EWsGcOpDrawPolyLineLocal:
			iGc->DrawPolyLine(pData.PointList);
			break;
		case EWsGcOpDrawPolygonLocalBufLen:
			iGc->DrawPolygon(pData.DrawPolygonLocalBufLen->points,pData.DrawPolygonLocalBufLen->length,pData.DrawPolygonLocalBufLen->fillRule);
			break;
		case EWsGcOpDrawPolygonLocal:
			iGc->DrawPolygon(pData.DrawPolygonLocal->pointList,pData.DrawPolygonLocal->fillRule);
			break;
		case EWsGcOpDrawBitmapLocal:
			iGc->DrawBitmap(pData.BitmapLocal->pos, pData.BitmapLocal->bitmap);
			break;
		case EWsGcOpDrawBitmap2Local:
			iGc->DrawBitmap(pData.Bitmap2Local->rect, pData.Bitmap2Local->bitmap);
			break;
		case EWsGcOpDrawBitmap3Local:
			iGc->DrawBitmap(pData.Bitmap3Local->rect, pData.Bitmap3Local->bitmap, pData.Bitmap3Local->srcRect);
			break;
		case EWsGcOpDrawBitmapMaskedLocal:
			iGc->DrawBitmapMasked(pData.iBitmapMaskedLocal->iRect, pData.iBitmapMaskedLocal->iBitmap, pData.iBitmapMaskedLocal->iSrcRect, pData.iBitmapMaskedLocal->iMaskBitmap,pData.iBitmapMaskedLocal->iInvertMask);
			break;
		case EWsGcOpAlphaBlendBitmapsLocal:
			iGc->AlphaBlendBitmaps(pData.AlphaBlendBitmapsLocal->point,pData.AlphaBlendBitmapsLocal->iBitmap,
						   			pData.AlphaBlendBitmapsLocal->source, pData.AlphaBlendBitmapsLocal->iAlpha,
									pData.AlphaBlendBitmapsLocal->alphaPoint);

			break;
		case EWsGcOpDrawText:
			iGc->DrawText(BufferTPtr((TText *)(pData.DrawText+1),pData.DrawText->length,aCmdData),pData.DrawText->pos);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length,aCmdData),pData.BoxTextO1->box,
							pData.BoxTextO1->baselineOffset,CGraphicsContext::ELeft,0);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length,aCmdData),pData.BoxTextO2->box,
							pData.BoxTextO2->baselineOffset,pData.BoxTextO2->horiz,pData.BoxTextO2->leftMrg);
			break;
		case EWsGcOpDrawTextPtr:
			iGc->DrawText(*pData.DrawTextPtr->text,pData.DrawTextPtr->pos);
			break;
		case EWsGcOpDrawTextPtr1:
		   	iGc->DrawText(*pData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawBoxText:
			iGc->DrawText(BufferTPtr((TText *)(pData.BoxText+1),pData.BoxText->length,aCmdData),pData.BoxText->box,pData.BoxText->baselineOffset,pData.BoxText->width,pData.BoxText->horiz,pData.BoxText->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iGc->DrawText(*pData.DrawBoxTextPtr->text,pData.DrawBoxTextPtr->box,pData.DrawBoxTextPtr->baselineOffset,pData.DrawBoxTextPtr->width,pData.DrawBoxTextPtr->horiz,pData.DrawBoxTextPtr->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr1:
			iGc->DrawText(*pData.DrawBoxTextPtr->text,pData.DrawBoxTextPtr->box);
			break;
		case EWsGcOpDrawTextVertical:
			iGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length,aCmdData),pData.DrawTextVertical->pos
							,pData.DrawTextVertical->up);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iGc->DrawTextVertical(*pData.DrawTextVerticalPtr->text,pData.DrawTextVerticalPtr->pos,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextVerticalPtr1:
			iGc->DrawTextVertical(*pData.DrawTextVerticalPtr->text,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawBoxTextVertical:
			iGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length,aCmdData),
							pData.DrawBoxTextVertical->box,	pData.DrawBoxTextVertical->baselineOffset,
							pData.DrawBoxTextVertical->up,(CGraphicsContext::TTextAlign)pData.DrawBoxTextVertical->vert,pData.DrawBoxTextVertical->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->baselineOffset
							,pData.DrawBoxTextVerticalPtr->width,pData.DrawBoxTextVerticalPtr->up,pData.DrawBoxTextVerticalPtr->vert,pData.DrawBoxTextVerticalPtr->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr1:
			iGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextLocal:
			iGc->DrawText(*pData.DrawTextLocal->desc,pData.DrawTextLocal->pos);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iGc->DrawText(*pData.BoxTextLocal->desc,pData.BoxTextLocal->box,pData.BoxTextLocal->baselineOffset,
							pData.BoxTextLocal->horiz,pData.BoxTextLocal->leftMrg);
			break;
		case EWsGcOpDrawLine:
			iGc->DrawLine(pData.DrawLine->pnt1,pData.DrawLine->pnt2);
			break;
		case EWsGcOpDrawTo:
			iGc->DrawLine(iLinePos,*pData.Point);
			break;
		case EWsGcOpDrawBy:
			iGc->DrawLine(iLinePos,iLinePos+(*pData.Point));
			break;
		case EWsGcOpPlot:
			iGc->Plot(*pData.Point);
			break;
		case EWsGcOpMoveTo:
		case EWsGcOpMoveBy:
			break;
		case EWsGcOpGdiBlt2Local:
			iGc->BitBlt(pData.GdiBlt2Local->pos,pData.GdiBlt2Local->bitmap);
			break;
		case EWsGcOpGdiBlt3Local:
			iGc->BitBlt(pData.GdiBlt3Local->pos,pData.GdiBlt3Local->bitmap, pData.GdiBlt3Local->rect);
			break;
		case EWsGcOpGdiBltMaskedLocal:
			iGc->BitBltMasked(pData.GdiBltMaskedLocal->pos,pData.GdiBltMaskedLocal->bitmap,
								pData.GdiBltMaskedLocal->rect,pData.GdiBltMaskedLocal->maskBitmap,
								pData.GdiBltMaskedLocal->invertMask);
			break;
		case EWsGcOpGdiWsBlt2:
		case EWsGcOpGdiWsBlt3:
		case EWsGcOpGdiWsBltMasked:
		case EWsGcOpGdiWsAlphaBlendBitmaps:
		case EWsGcOpWsDrawBitmapMasked:
			{
			// Andy - we continually duplicate bitmaps in here, and yet we already have them
			// somewhere as pointers so can't we both simplify and optimize this?
			CFbsBitmap* scratchBitmap=iScratchBitmap;
			CFbsBitmap* scratchMaskBimap=iScratchMaskBitmap;
			TInt maskHandle=0;
			TInt handle=WsBitmapHandle(aOpcode,pData, maskHandle);
			CWsClient* owner=iWin->WsOwner();
			if (owner!=NULL)
				{
				TInt wsBmpErr = KErrNone;
				DWsBitmap *bitmap=(DWsBitmap *)owner->HandleToObj(handle, WS_HANDLE_BITMAP);
				if (!bitmap)
					wsBmpErr = KErrNotFound;
				else
					scratchBitmap=bitmap->FbsBitmap();
				if (wsBmpErr == KErrNone)
					if (aOpcode==EWsGcOpGdiWsBltMasked || aOpcode==EWsGcOpGdiWsAlphaBlendBitmaps || aOpcode==EWsGcOpWsDrawBitmapMasked)
						{
						DWsBitmap *bitmap2=(DWsBitmap *)owner->HandleToObj(maskHandle, WS_HANDLE_BITMAP);
						if (!bitmap2)
							wsBmpErr = KErrNotFound;
						else
							scratchMaskBimap=bitmap2->FbsBitmap();
						}
				if (wsBmpErr == KErrNone)
					{
					switch(aOpcode)
						{
						case EWsGcOpGdiWsBlt2:
							iGc->BitBlt(pData.GdiBlt2->pos,scratchBitmap);
							break;
						case EWsGcOpGdiWsBlt3:
							iGc->BitBlt(pData.GdiBlt3->pos,scratchBitmap, pData.GdiBlt3->rect);
							break;
						case EWsGcOpGdiWsBltMasked:
							{
							iGc->BitBltMasked(pData.GdiBltMasked->destination,scratchBitmap,
											pData.GdiBltMasked->source, scratchMaskBimap,
											pData.GdiBltMasked->invertMask);
							}
							break;
						case EWsGcOpGdiWsAlphaBlendBitmaps:
							{
							iGc->AlphaBlendBitmaps(pData.AlphaBlendBitmaps->point,scratchBitmap,
											   pData.AlphaBlendBitmaps->source, scratchMaskBimap,
											   pData.AlphaBlendBitmaps->alphaPoint);
							}
							break;
						case EWsGcOpWsDrawBitmapMasked:
							{
							iGc->DrawBitmapMasked(pData.iBitmapMasked->iRect,scratchBitmap, 
												pData.iBitmapMasked->iSrcRect,scratchMaskBimap,
												pData.iBitmapMasked->iInvertMask);
							}
							break;
						}
					}
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
			if (ret == KErrNone)
				{
				switch(aOpcode)
					{
					case EWsGcOpGdiBlt2:
						iGc->BitBlt(pData.GdiBlt2->pos,iScratchBitmap);
						break;
					case EWsGcOpGdiBlt3:
						iGc->BitBlt(pData.GdiBlt3->pos,iScratchBitmap, pData.GdiBlt3->rect);
						break;
					case EWsGcOpGdiBltMasked:
						{
						ret = iScratchMaskBitmap->Duplicate(pData.GdiBltMasked->maskHandle);
						if (ret == KErrNone)
							{
							iGc->BitBltMasked(pData.GdiBltMasked->destination,iScratchBitmap,
												pData.GdiBltMasked->source, iScratchMaskBitmap,
												pData.GdiBltMasked->invertMask);
							iScratchMaskBitmap->Reset();
							}
						}
						break;
					case EWsGcOpGdiAlphaBlendBitmaps:
						{
						ret = iScratchMaskBitmap->Duplicate(pData.AlphaBlendBitmaps->alphaHandle);
						if (ret == KErrNone)
							{
							iGc->AlphaBlendBitmaps(pData.AlphaBlendBitmaps->point, iScratchBitmap,
												pData.AlphaBlendBitmaps->source, iScratchMaskBitmap,
												pData.AlphaBlendBitmaps->alphaPoint);
							iScratchMaskBitmap->Reset();
							}
						break;
						}
					case EWsGcOpDrawBitmap:
						iGc->DrawBitmap(pData.Bitmap->pos, iScratchBitmap);
						break;
					case EWsGcOpDrawBitmap2:
						iGc->DrawBitmap(pData.Bitmap2->rect, iScratchBitmap);
						break;
					case EWsGcOpDrawBitmap3:
						iGc->DrawBitmap(pData.Bitmap3->rect, iScratchBitmap, pData.Bitmap3->srcRect);
						break;
					case EWsGcOpDrawBitmapMasked:
						{
						ret = iScratchMaskBitmap->Duplicate(pData.iBitmapMasked->iMaskHandle);
						if (ret == KErrNone)
							{
							iGc->DrawBitmapMasked(pData.iBitmapMasked->iRect, iScratchBitmap, 
												pData.iBitmapMasked->iSrcRect, iScratchMaskBitmap,
												pData.iBitmapMasked->iInvertMask);
							iScratchMaskBitmap->Reset();
							}
						}
						break;
					}
				iScratchBitmap->Reset();
				}
			break;
			}
		case EWsGcOpDrawSegmentedPolygon:
			iGc->DrawPolygon(iPolyPoints,iPolyPointListSize,pData.DrawSegmentedPolygon->fillRule);
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
			iGc->Clear(TRect(iWin->Size()));
			break;
		case EWsGcOpClearRect:
			iGc->Clear(*pData.Rect);
			break;
		case EWsGcOpDrawRect:
			iGc->DrawRect(*pData.Rect);
			break;
		case EWsGcOpDrawEllipse:
			iGc->DrawEllipse(*pData.Rect);
			break;
		case EWsGcOpDrawRoundRect:
			iGc->DrawRoundRect(*pData.Rect,pData.RoundRect->ellipse);
			break;
		case EWsGcOpDrawArc:
			iGc->DrawArc(*pData.Rect,pData.ArcOrPie->start,pData.ArcOrPie->end);
			break;
		case EWsGcOpDrawPie:
			iGc->DrawPie(*pData.Rect,pData.ArcOrPie->start,pData.ArcOrPie->end);
			break;
		case EWsGcOpCopyRect:
			iGc->CopyRect(pData.CopyRect->pos,*pData.Rect);
			break;
		case EWsGcOpMapColors:
			iGc->MapColors(pData.MapColors->rect,(TRgb *)(pData.MapColors+1),pData.MapColors->numPairs,pData.MapColors->mapForwards);
			break;
		default:
			TRAP_IGNORE(iWin->OwnerPanic(EWservPanicOpcode));
			break;
		}
	// DEBUGOSB // comment in for per-draw-command debug osb updates
	iGc->SetClippingRegion(NULL);
	iCurrentClippingRegion = NULL;
	}

TInt CPlaybackGc::WsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle) 
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
		}
	return handle;
	}		

TInt CPlaybackGc::FbsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle)
	{
	TInt handle=0;
	aMaskHandle=0;
	switch(aOpcode)
		{
		case EWsGcOpGdiWsBlt2:
		case EWsGcOpGdiWsBlt3:
		case EWsGcOpGdiWsBltMasked:
		case EWsGcOpGdiWsAlphaBlendBitmaps:
			{
			TInt maskHandle = 0;
			DWsBitmap *bitmap=(DWsBitmap *)iWin->WsOwner()->HandleToObj(WsBitmapHandle(aOpcode,pData, maskHandle), WS_HANDLE_BITMAP);
			WS_ASSERT_DEBUG(bitmap, EWsPanicDrawCommandsInvalidState);
			if (bitmap)
				handle=bitmap->FbsBitmap()->Handle();
			if (aOpcode==EWsGcOpGdiWsBltMasked || aOpcode==EWsGcOpGdiWsAlphaBlendBitmaps)
				{
				DWsBitmap *bitmap2=(DWsBitmap *)iWin->WsOwner()->HandleToObj(maskHandle, WS_HANDLE_BITMAP);
				WS_ASSERT_DEBUG(bitmap2, EWsPanicDrawCommandsInvalidState);
				if (bitmap2)
					aMaskHandle=bitmap2->FbsBitmap()->Handle();
				}
			break;
			}
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
		}
	return handle;
	}


void CPlaybackGc::UpdateJustification(TText* aText,TInt aLen, const TDesC8& aCmdData)
	{
	iGc->UpdateJustification(BufferTPtr(aText,aLen,aCmdData));
	}

void CPlaybackGc::DoDrawing(TWsGcOpcodes aOpcode, const TDesC8& aCmdData)
	{
	TWsGcCmdUnion pData;
	pData.any=aCmdData.Ptr();
	
	// Andy. We do this every time?  Shouldn't this be set up for us already by the render stage?
	// infact, aren't we breaking the render stage by doing it here?
	iGc->SetUserDisplayMode(iWin->DisplayMode());
	if (iClippingRectSet)
		{
		iGc->SetOrigin(iMasterOrigin);
		iGc->SetClippingRect(iClippingRect);
		}
	iGc->SetOrigin(iMasterOrigin + iOrigin);
	
	DoDrawCommand(aOpcode,aCmdData,iDrawRegion);

	iGc->CancelClippingRect();
	iGc->SetUserDisplayMode(ENone);
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
			UpdateJustification((TText *)(pData.DrawText+1),pData.DrawText->length,aCmdData);
			break;
		case EWsGcOpDrawTextVertical:
			UpdateJustification((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length,aCmdData);
			break;
		case EWsGcOpDrawBoxText:
			UpdateJustification((TText *)(pData.BoxText+1),pData.BoxText->length,aCmdData);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			UpdateJustification((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length,aCmdData);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			UpdateJustification((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length,aCmdData);
			break;
		case EWsGcOpDrawBoxTextVertical:
			UpdateJustification((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length,aCmdData);
			break;
		case EWsGcOpDrawTextLocal:
			iGc->UpdateJustification(*pData.DrawTextLocal->desc);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iGc->UpdateJustification(*pData.BoxTextLocal->desc);
			break;
		case EWsGcOpDrawTextPtr:
			iGc->UpdateJustification(*pData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iGc->UpdateJustification(*pData.DrawTextVerticalPtr->text);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iGc->UpdateJustification(*pData.DrawBoxTextPtr->text);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iGc->UpdateJustification(*pData.DrawBoxTextVerticalPtr->text);
			break;
		}
	}

void CPlaybackGc::CommandL(TWsGcOpcodes aOpcode, const TDesC8& aCmdData)
	{
	TWsGcCmdUnion pData;
	pData.any=aCmdData.Ptr();
	
	switch(aOpcode)
		{
	case EWsGcOpStartSegmentedDrawPolygon:
		StartSegmentedDrawPolygonL(pData.StartSegmentedDrawPolygon);
		break;
	case EWsGcOpSegmentedDrawPolygonData:
		SegmentedDrawPolygonData(pData.SegmentedDrawPolygonData);
		break;
	case EWsGcOpSetClippingRegion:
		WS_ASSERT_DEBUG(aOpcode != EWsGcOpSetClippingRegion, EWsPanicDrawCommandsInvalidState);
		break;
	case EWsGcOpSetClippingRect:
		SetClippingRect(*pData.Rect);
		break;
	case EWsGcOpCancelClippingRect:
		ResetClippingRect();
		break;
	case EWsGcOpCancelClippingRegion:
		CancelUserClippingRegion();
		break;
	case EWsGcOpSetFaded:
		iGc->SetFaded(*pData.Bool);
		break;
	case EWsGcOpSetFadeParams:
		iGc->SetFadingParameters(WservEncoding::ExtractFirst8BitValue(*pData.UInt),WservEncoding::ExtractSecond8BitValue(*pData.UInt));
		break;
	case EWsGcOpSetDrawMode:
		iGc->SetDrawMode((CGraphicsContext::TDrawMode)*pData.UInt);
		break;
	case EWsGcOpUseFont:
		if (CWsFontCache::Instance()->UseFont(iFont, *pData.UInt))
			iGc->UseFont(*pData.UInt);
		else
			iGc->UseFontNoDuplicate(iFont);
		break;
	case EWsGcOpDiscardFont:
		CWsFontCache::Instance()->ReleaseFont(iFont);
		iGc->DiscardFont();
		break;
	case EWsGcOpSetUnderlineStyle:
		iGc->SetUnderlineStyle(*pData.SetUnderlineStyle);
		break;
	case EWsGcOpSetStrikethroughStyle:
		iGc->SetStrikethroughStyle(*pData.SetStrikethroughStyle);
		break;
	case EWsGcOpUseBrushPattern:
		iGc->UseBrushPattern(*pData.handle);
		break;
	case EWsGcOpDiscardBrushPattern:
		iGc->DiscardBrushPattern();
		break;
	case EWsGcOpSetBrushColor:
		iGc->SetBrushColor(*pData.rgb);
		break;
	case EWsGcOpSetPenColor:
		iGc->SetPenColor(*pData.rgb);
		break;
	case EWsGcOpSetPenStyle:
		iGc->SetPenStyle((CGraphicsContext::TPenStyle)*pData.UInt);
		break;
	case EWsGcOpSetPenSize:
		iGc->SetPenSize(*pData.Size);
		break;
	case EWsGcOpSetBrushStyle:
		iGc->SetBrushStyle((CGraphicsContext::TBrushStyle)*pData.UInt);
		break;
	case EWsGcOpReset:
		CWsFontCache::Instance()->ReleaseFont(iFont);
		iGc->Reset();
		iOrigin.SetXY(0,0);
		ResetClippingRect();
		iGc->SetBrushColor(iWin->BackColor());
		break;
	case EWsGcOpSetBrushOrigin:
		iGc->SetBrushOrigin(*pData.Point);
		break;
	case EWsGcOpSetDitherOrigin:
		iGc->SetDitherOrigin(*pData.Point);
		break;
	case EWsGcOpSetWordJustification:
		iGc->SetWordJustification(pData.SetJustification->excessWidth, pData.SetJustification->numGaps);
		break;
	case EWsGcOpSetCharJustification:
		iGc->SetCharJustification(pData.SetJustification->excessWidth, pData.SetJustification->numGaps);
		break;
	case EWsGcOpSetOrigin:
		SetOrigin(*pData.Point);
		break;
	case EWsGcOpSetOpaque:
		// Andy - opaque drawing has not been implemented yet.
		//SetOpaque(*pData.Bool);
		break;
	case EWsGcOpSetShadowColor:
		iGc->SetShadowColor(*pData.rgb);
		break;
	default:	// Assume remaining functions will draw
		{
		DoDrawing(aOpcode,aCmdData);
		return;
		}
		}
	}

void CPlaybackGc::SetOrigin(const TPoint &aOrigin)
	{
	iOrigin=aOrigin;
	}
	
/*------------------------------------------------------------------------------
  Description: Retrieves graphics context information back from a given buffer
               from a given start position.
 -----------------------------------------------------------------------------*/
void CPlaybackGc::InternalizeL(const CBufBase& aBuffer,TInt& aStartPos)
	{
	// Open the stream used for the input from the given start position
	// in the buffer.
	RBufReadStream bufReadStream;
	bufReadStream.Open(aBuffer,aStartPos);
	CleanupClosePushL(bufReadStream);

	// Read the font/bitmap server data
	iGc->InternalizeL(bufReadStream);

	iOrigin.iX = bufReadStream.ReadInt32L();
	iOrigin.iY = bufReadStream.ReadInt32L();

	iClippingRectSet=bufReadStream.ReadInt8L();
	
	// If there is a clipping rectangle data read it.
	if (iClippingRectSet)
		bufReadStream>>iClippingRect;
	
	// Force FbsBitGc to reset its user clipping rect in case orientation has changed.
	// The user clipping rect of FbsBitGc will be set to iClippingRect later before
	// drawing the command.
	iGc->CancelClippingRect();
	
	// Read the clipping region data
	InternalizeClippingRegionL(bufReadStream);

	// Read the Alpha values for Brush and Pen colors.
	InternalizeAlphaValueL(bufReadStream);
	
	CleanupStack::PopAndDestroy(&bufReadStream);
	}

/*------------------------------------------------------------------------------
  Description: Retrieves TRgb::alpha value information back from a given buffer
               and updates the Brushcolor with the same.
 -----------------------------------------------------------------------------*/
void CPlaybackGc::InternalizeAlphaValueL(RReadStream& aReadStream)
	{
	TRgb brushColor(iGc->BrushColor());
	brushColor.SetAlpha(aReadStream.ReadUint8L());
	iGc->SetBrushColor(brushColor);
	TRgb penColor(iGc->PenColor());
	penColor.SetAlpha(aReadStream.ReadUint8L());
	iGc->SetPenColor(penColor);
	}

/*------------------------------------------------------------------------------
  Description: Helper method to retrieve clipping region data from a given
               read stream.
 -----------------------------------------------------------------------------*/
void CPlaybackGc::InternalizeClippingRegionL(RReadStream& aReadStream)
	{
	WS_ASSERT_DEBUG(iTargetRegion, EWsPanicDrawCommandsInvalidState);
	// Read flag to indicate if client had defined a clipping region
	TBool clipRegion = aReadStream.ReadInt8L();
	CancelUserClippingRegion();
	if (clipRegion)
		{
		// Note that this clipping region is in window relative coordinates when
		// received from the client (and being stored) but is in screen relative
		// coordinates after being retrieved from the redraw store.
		iUserDefinedClippingRegion = InternalizeRegionL(aReadStream);
		iUserDefinedClippingRegion->Offset(iWin->Origin());
		iUserDefinedClippingRegion->Intersect(*iTargetRegion);
		if (iUserDefinedClippingRegion->CheckError()) // fallback to no user clipping region
			{
			CancelUserClippingRegion();
			}
		else
			{
			iDrawRegion = iUserDefinedClippingRegion;
			}
		}
	}

/**
This pretty much replaces the whole of what was TDrawDestination
This can only be sensibly called from outside a sequence of drawing commands,
since it negates any user defined clipping regions.
*/
void CPlaybackGc::SetTargetRegion(const TRegion* aRegion)
	{
	iTargetRegion = aRegion;
	iDrawRegion = iTargetRegion;
	CancelUserClippingRegion();
	}
	
void CPlaybackGc::Reset()
	{
	iGc->Reset();
	}

TAny * CPlaybackGc::ResolveObjectInterface(TUint aId)
	{
	switch (aId)
		{
		case MWsWindow::EWsObjectInterfaceId:
			return dynamic_cast<MWsWindow *>(iWin);
		case MWsFader::EWsObjectInterfaceId:
		  	return iWin->Screen()->Fader();
		}
	return NULL;
	}
