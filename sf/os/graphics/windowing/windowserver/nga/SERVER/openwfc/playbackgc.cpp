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

#include "playbackgc.h"

#include <e32std.h>
#include <s32mem.h> 

#include "panics.h"
#include "ScrDev.H"
#include "windowgroup.h"
#include "wsfont.h"
#include "wstop.h"
#include "graphics/WSGRAPHICDRAWER.H"
#include "graphics/surfaceconfiguration.h"
#include "windowelementset.h"

#include <graphics/wsgraphicscontext.h>
#include <graphics/wsuibuffer.h>
#include "bitgditomwsgraphicscontextmappings.h"

#include "graphicscontextstate.h"
#include "drawresource.h"
#include "devicemap.h"

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
	iGcBuf = CBufSeg::NewL(512);
	}

CPlaybackGc::~CPlaybackGc()
	{
	delete iPolyPoints;
	delete iGcBuf;
	iCurrentClippingRegion = NULL;
	iIntersectedRegion.Close();
	}

void CPlaybackGc::Activate(CWsClientWindow * aWin, MWsGraphicsContext * aGc, const TRegion * aRegion)
	{
	iWin = aWin;
	iGc = aGc;
	iTargetRegion = aRegion;
	
	iDrawRegion = iTargetRegion;
	iMasterOrigin = iWin->Origin();
	iOrigin.SetXY(0,0);
	iSendOrigin = ETrue;
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
	TArrayWrapper<TPoint> points((TPoint*)(aDrawPolygon + 1), aDrawPolygon->numPoints);
	iGc->DrawPolygon(points, BitGdiToMWsGraphicsContextMappings::Convert(aDrawPolygon->fillRule));
	}

void CPlaybackGc::StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon *aDrawPolygon)
	{
// In case a Playback have been done before all the segment is in the RedrawStore
// (This allocation is deleted only thanks to the EWsGcOpDrawSegmentedPolygon opcode
//  which arrive after all the segments)
	if (iPolyPoints)
		{
		delete iPolyPoints;
		iPolyPoints=NULL;	
		}
	if(!Rng(0, aDrawPolygon->totalNumPoints, KMaxTInt/2 - 1))
		GcOwnerPanic(EWservPanicBadPolyData);	
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
	TArrayWrapper<TPoint> pointsArr(points, numPoints);
	if (aDrawPolyLine->more)	// more to come so don't draw the end point
		iGc->DrawPolyLineNoEndPoint(pointsArr);
	else
		iGc->DrawPolyLine(pointsArr);
	}

void CPlaybackGc::GcOwnerPanic(TClientPanic aPanic)
	{
	iGc->ResetClippingRegion();
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
	rect.Move(iGc->Origin());
	// clip rect to window extent
	rect.Intersection(iWin->Abs());
	if (!rect.IsEmpty())
		{
		// and schedule it
		iWin->Screen()->ScheduleAnimation(ECrpAnim, rect, aFromNow, aFreq, aStop, iWin);
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
    if (aRegion->Count()==0)
        return;

    TWsGcCmdUnion pData;
	// coverity[returned_pointer]
	pData.any=aCmdData.Ptr();

	SendClippingRegionIfRequired(aRegion);

	WS_ASSERT_DEBUG(!iCurrentClippingRegion, EWsPanicDrawCommandsInvalidState);
	iCurrentClippingRegion = aRegion;

	CGraphicsContext::TTextParameters contextParam;
	
	TBool bGcDrawingOccurred = ETrue;	//most commands in here draw using the gc
	
	switch(aOpcode)
		{
		case EWsGcOpDrawWsGraphic:
		case EWsGcOpDrawWsGraphicPtr:
			{
			bGcDrawingOccurred = EFalse;	//best guess if the drawer did no happen 
			TRect screenRect(pData.WsGraphic->iRect);
			screenRect.Move(iGc->Origin());
			if(iCurrentClippingRegion->Intersects(screenRect))
				{
				const TInt dataLen = pData.WsGraphic->iDataLen;
				TGraphicDrawerId id;
				id.iId = pData.WsGraphic->iId;
				id.iIsUid = (pData.WsGraphic->iFlags & EWsGraphicIdUid);
				CWsClient* owner = iWin->WsOwner();
				const CWsGraphicDrawer* graphic = CWsTop::WindowServer()->ResolveGraphic(id);
				TInt lastDrawCount=GcDrawingCount();
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
				if (lastDrawCount!=GcDrawingCount())
					{
					// Changes to the GcDrawingCount are used to tag placed and background surfaces as dirty.
					// If drawing occurs inside a CRP and perhaps PlaceSurface occurs inside the CRP 
					// then we tag to count again to cause the placed surface to get marked as dirty later.
					bGcDrawingOccurred = ETrue;	//some GC drawing did occurr at some point
					}
				}
			break;
			}
		case EWsGcOpMapColorsLocal:
			GcOwnerPanic(EWservPanicOpcode); //deprecated, the client should never generate this op
			break;
		case EWsGcOpDrawPolyLineLocalBufLen:
			{
			TArrayWrapper<TPoint> points(pData.DrawPolyLineLocalBufLen->points, pData.DrawPolyLineLocalBufLen->length);
			iGc->DrawPolyLine(points);
			break;
			}
		case EWsGcOpDrawPolyLineLocal:
			iGc->DrawPolyLine(pData.PointList->Array());
			break;
		case EWsGcOpDrawPolygonLocalBufLen:
			{
			TArrayWrapper<TPoint> points(pData.DrawPolygonLocalBufLen->points, pData.DrawPolygonLocalBufLen->length);
			iGc->DrawPolygon(points, BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawPolygonLocalBufLen->fillRule));
			break;
			}
		case EWsGcOpDrawPolygonLocal:
			iGc->DrawPolygon(pData.DrawPolygonLocal->pointList->Array(),BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawPolygonLocal->fillRule));
			break;
		case EWsGcOpDrawBitmapLocal:
			{
			// DrawBitmap(TPoint&, ) uses the size of the bitmap in twips, but 
			// MWsGraphicsContext::DrawBitmap() takes a TRect in pixels, so we need to convert
			TRect destRect(iWin->Screen()->DeviceMap().TwipsToPixels(pData.BitmapLocal->bitmap->SizeInTwips()));
			destRect.Move(pData.BitmapLocal->pos); //pos is defined in pixels, that's why we're not converting it
			iGc->DrawBitmap(destRect, *pData.BitmapLocal->bitmap);
			break;
			}
		case EWsGcOpDrawBitmap2Local:
			iGc->DrawBitmap(pData.Bitmap2Local->rect, *pData.Bitmap2Local->bitmap);
			break;
		case EWsGcOpDrawBitmap3Local:
			iGc->DrawBitmap(pData.Bitmap3Local->rect, *pData.Bitmap3Local->bitmap, pData.Bitmap3Local->srcRect);
			break;
		case EWsGcOpDrawBitmapMaskedLocal:
			iGc->DrawBitmapMasked(pData.iBitmapMaskedLocal->iRect, *pData.iBitmapMaskedLocal->iBitmap, pData.iBitmapMaskedLocal->iSrcRect, *pData.iBitmapMaskedLocal->iMaskBitmap,pData.iBitmapMaskedLocal->iInvertMask);
			break;
		case EWsGcOpAlphaBlendBitmapsLocal:
			iGc->BitBltMasked(pData.AlphaBlendBitmapsLocal->point, *pData.AlphaBlendBitmapsLocal->iBitmap,
						                  pData.AlphaBlendBitmapsLocal->source, *pData.AlphaBlendBitmapsLocal->iAlpha,
						                  pData.AlphaBlendBitmapsLocal->alphaPoint);
			break;
		case EWsGcOpDrawText:
			if (iGc->HasFont())
				iGc->DrawText(BufferTPtr((TText *)(pData.DrawText+1),pData.DrawText->length,aCmdData), NULL, pData.DrawText->pos);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			if (iGc->HasFont())
				iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length,aCmdData), NULL, pData.BoxTextO1->box,
							  pData.BoxTextO1->baselineOffset,MWsGraphicsContext::ELeft,0);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			if (iGc->HasFont())
				iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length,aCmdData), NULL, pData.BoxTextO2->box,
							  pData.BoxTextO2->baselineOffset,BitGdiToMWsGraphicsContextMappings::Convert(pData.BoxTextO2->horiz),pData.BoxTextO2->leftMrg);
			break;
		case EWsGcOpDrawTextPtr:
			if (iGc->HasFont())
				iGc->DrawText(*pData.DrawTextPtr->text, NULL, pData.DrawTextPtr->pos);
			break;
		case EWsGcOpDrawTextPtr1:
			if (iGc->HasFont())
				iGc->DrawText(*pData.DrawTextPtr->text, NULL);
			break;
		case EWsGcOpDrawBoxText:
			if (iGc->HasFont())
				iGc->DrawText(BufferTPtr((TText *)(pData.BoxText+1),pData.BoxText->length,aCmdData), NULL, pData.BoxText->box,
							  pData.BoxText->baselineOffset,BitGdiToMWsGraphicsContextMappings::Convert(pData.BoxText->horiz),pData.BoxText->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr:
			if (iGc->HasFont())
				iGc->DrawText(*pData.DrawBoxTextPtr->text, NULL, pData.DrawBoxTextPtr->box,pData.DrawBoxTextPtr->baselineOffset,BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawBoxTextPtr->horiz),pData.DrawBoxTextPtr->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr1:
			if (iGc->HasFont())
				iGc->DrawText(*pData.DrawBoxTextPtr->text, NULL, pData.DrawBoxTextPtr->box);
			break;
		case EWsGcOpDrawTextVertical:
			if (iGc->HasFont())
				iGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length,aCmdData),NULL,pData.DrawTextVertical->pos
							          ,pData.DrawTextVertical->up);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			if (iGc->HasFont())
				iGc->DrawTextVertical(*pData.DrawTextVerticalPtr->text,NULL,pData.DrawTextVerticalPtr->pos,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextVerticalPtr1:
			if (iGc->HasFont())
				iGc->DrawTextVertical(*pData.DrawTextVerticalPtr->text,NULL,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawBoxTextVertical:
			if (iGc->HasFont())
				iGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length,aCmdData), NULL,
									  pData.DrawBoxTextVertical->box,	pData.DrawBoxTextVertical->baselineOffset,
									  pData.DrawBoxTextVertical->up,BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawBoxTextVertical->vert),pData.DrawBoxTextVertical->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			if (iGc->HasFont())
				iGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,NULL,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->baselineOffset
									  ,pData.DrawBoxTextVerticalPtr->width,pData.DrawBoxTextVerticalPtr->up,BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawBoxTextVerticalPtr->vert),pData.DrawBoxTextVerticalPtr->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr1:
			if (iGc->HasFont())
				iGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,NULL,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextLocal:
			if (iGc->HasFont())
				iGc->DrawText(*pData.DrawTextLocal->desc, NULL, pData.DrawTextLocal->pos);
			break;
		case EWsGcOpDrawBoxTextLocal:
			if (iGc->HasFont())
				iGc->DrawText(*pData.BoxTextLocal->desc, NULL, pData.BoxTextLocal->box,pData.BoxTextLocal->baselineOffset,
							  BitGdiToMWsGraphicsContextMappings::Convert(pData.BoxTextLocal->horiz),pData.BoxTextLocal->leftMrg);
			break;
		/************* DrawText in Context function calls*********************************************/
		case EWsGcOpDrawTextInContext:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(BufferTPtr((TText *)(pData.DrawTextInContext+1),pData.DrawTextInContext->length,aCmdData),BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawTextInContext->pos);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised1:
			contextParam.iStart = pData.BoxTextInContextO1->start;
			contextParam.iEnd = pData.BoxTextInContextO1->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextInContextO1+1),pData.BoxTextInContextO1->length,aCmdData),BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.BoxTextInContextO1->box,
							  pData.BoxTextInContextO1->baselineOffset,MWsGraphicsContext::ELeft,0);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised2:
			contextParam.iStart = pData.BoxTextInContextO2->start;
			contextParam.iEnd = pData.BoxTextInContextO2->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextInContextO2+1),pData.BoxTextInContextO2->length,aCmdData),BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.BoxTextInContextO2->box,
							  pData.BoxTextInContextO2->baselineOffset,BitGdiToMWsGraphicsContextMappings::Convert(pData.BoxTextInContextO2->horiz),pData.BoxTextInContextO2->leftMrg);
				}
			break;
		case EWsGcOpDrawTextInContextPtr:
			contextParam.iStart = pData.DrawTextInContextPtr->start;
			contextParam.iEnd = pData.DrawTextInContextPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(*pData.DrawTextInContextPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawTextInContextPtr->pos);
				}
			break;
		case EWsGcOpDrawTextInContextPtr1:
			contextParam.iStart = pData.DrawTextInContextPtr->start;
			contextParam.iEnd = pData.DrawTextInContextPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(*pData.DrawTextInContextPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		case EWsGcOpDrawBoxTextInContext:
			contextParam.iStart = pData.BoxTextInContext->start;
			contextParam.iEnd = pData.BoxTextInContext->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(BufferTPtr((TText *)(pData.BoxTextInContext+1),pData.BoxTextInContext->length,aCmdData),
							  BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.BoxTextInContext->box,pData.BoxTextInContext->baselineOffset,
							  BitGdiToMWsGraphicsContextMappings::Convert(pData.BoxTextInContext->horiz),pData.BoxTextInContext->leftMrg);
				}
			break;
		case EWsGcOpDrawBoxTextInContextPtr:
			contextParam.iStart = pData.DrawBoxTextInContextPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(*pData.DrawBoxTextInContextPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawBoxTextInContextPtr->box,pData.DrawBoxTextInContextPtr->baselineOffset,
							  BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawBoxTextInContextPtr->horiz),pData.DrawBoxTextInContextPtr->leftMrg);
				}
			break;
		case EWsGcOpDrawBoxTextInContextPtr1:
			contextParam.iStart = pData.DrawBoxTextInContextPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(*pData.DrawBoxTextInContextPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawBoxTextInContextPtr->box);
				}
			break;
		case EWsGcOpDrawTextInContextVertical:
			contextParam.iStart = pData.DrawTextInContextVertical->start;
			contextParam.iEnd = pData.DrawTextInContextVertical->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawTextInContextVertical+1),pData.DrawTextInContextVertical->length,aCmdData),
									  BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawTextInContextVertical->pos,pData.DrawTextInContextVertical->up);
				}
			break;
		case EWsGcOpDrawTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawTextInContextVerticalPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawTextVertical(*pData.DrawTextInContextVerticalPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),
									  pData.DrawTextInContextVerticalPtr->pos,pData.DrawTextInContextVerticalPtr->up);
				}
			break;
		case EWsGcOpDrawTextInContextVerticalPtr1:
			contextParam.iStart = pData.DrawTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawTextInContextVerticalPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawTextVertical(*pData.DrawTextInContextVerticalPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawTextInContextVerticalPtr->up);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVertical:
			contextParam.iStart = pData.DrawBoxTextInContextVertical->start;
			contextParam.iEnd = pData.DrawBoxTextInContextVertical->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawBoxTextInContextVertical+1),pData.DrawBoxTextInContextVertical->length,aCmdData),
									  BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawBoxTextInContextVertical->box,pData.DrawBoxTextInContextVertical->baselineOffset,
									  pData.DrawBoxTextInContextVertical->up,BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawBoxTextInContextVertical->vert),pData.DrawBoxTextInContextVertical->margin);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawBoxTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextVerticalPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->baselineOffset
									  ,pData.DrawBoxTextVerticalPtr->width,pData.DrawBoxTextVerticalPtr->up,BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawBoxTextVerticalPtr->vert),pData.DrawBoxTextVerticalPtr->margin);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr1:
			contextParam.iStart = pData.DrawBoxTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextVerticalPtr->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->up);
				}
			break;
		case EWsGcOpDrawTextInContextLocal:
			contextParam.iStart = pData.DrawTextInContextLocal->start;
			contextParam.iEnd = pData.DrawTextInContextLocal->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(*pData.DrawTextInContextLocal->desc,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.DrawTextInContextLocal->pos);
				}
			break;
		case EWsGcOpDrawBoxTextInContextLocal:
			contextParam.iStart = pData.BoxTextInContextLocal->start;
			contextParam.iEnd = pData.BoxTextInContextLocal->end;
			if((contextParam.iStart < contextParam.iEnd) && (iGc->HasFont()))
				{
				iGc->DrawText(*pData.BoxTextInContextLocal->desc,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam),pData.BoxTextInContextLocal->box,pData.BoxTextInContextLocal->baselineOffset,
							  BitGdiToMWsGraphicsContextMappings::Convert(pData.BoxTextInContextLocal->horiz),pData.BoxTextInContextLocal->leftMrg);
				}
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
			iGc->BitBlt(pData.GdiBlt2Local->pos,*pData.GdiBlt2Local->bitmap);
			break;
		case EWsGcOpGdiBlt3Local:
			iGc->BitBlt(pData.GdiBlt3Local->pos,*pData.GdiBlt3Local->bitmap, pData.GdiBlt3Local->rect);
			break;
		case EWsGcOpGdiBltMaskedLocal:
			iGc->BitBltMasked(pData.GdiBltMaskedLocal->pos,*pData.GdiBltMaskedLocal->bitmap,
								pData.GdiBltMaskedLocal->rect,*pData.GdiBltMaskedLocal->maskBitmap,
								pData.GdiBltMaskedLocal->invertMask);
			break;
		case EWsGcOpGdiWsBlt2:
		case EWsGcOpGdiWsBlt3:
		case EWsGcOpGdiWsBltMasked:
		case EWsGcOpGdiWsAlphaBlendBitmaps:
		case EWsGcOpWsDrawBitmapMasked:
			{
			CFbsBitmap* scratchBitmap = NULL;
			CFbsBitmap* scratchMaskBimap = NULL;
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
							iGc->BitBlt(pData.GdiBlt2->pos,*scratchBitmap);
							break;
						case EWsGcOpGdiWsBlt3:
							iGc->BitBlt(pData.GdiBlt3->pos,*scratchBitmap, pData.GdiBlt3->rect);
							break;
						case EWsGcOpGdiWsBltMasked:
							{
							iGc->BitBltMasked(pData.GdiBltMasked->destination,*scratchBitmap,
											pData.GdiBltMasked->source, *scratchMaskBimap,
											pData.GdiBltMasked->invertMask);
							}
							break;
						case EWsGcOpGdiWsAlphaBlendBitmaps:
							{
							iGc->BitBltMasked(pData.AlphaBlendBitmaps->point,*scratchBitmap,
											   pData.AlphaBlendBitmaps->source, *scratchMaskBimap,
											   pData.AlphaBlendBitmaps->alphaPoint);
							}
							break;
						case EWsGcOpWsDrawBitmapMasked:
							{
							iGc->DrawBitmapMasked(pData.iBitmapMasked->iRect,*scratchBitmap, 
												pData.iBitmapMasked->iSrcRect,*scratchMaskBimap,
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
        case EWsGcOpDrawBitmap:
        case EWsGcOpDrawBitmap2:
        case EWsGcOpDrawBitmap3:
            {
			TInt bitmapMaskHandle=0;
            TInt bitmapHandle = FbsBitmapHandle(aOpcode, pData, bitmapMaskHandle);
            const CFbsBitmap* bitmap = iWin->Redraw()->BitmapFromHandle(bitmapHandle);
			if(!bitmap)
			    {
			    WS_PANIC_DEBUG(EWsPanicBitmapNotFound);
			    break;
			    }
			
            switch(aOpcode)
                {
                case EWsGcOpGdiBlt2:
                    iGc->BitBlt(pData.GdiBlt2->pos,*bitmap);
                    break;
                case EWsGcOpGdiBlt3:
                    iGc->BitBlt(pData.GdiBlt3->pos,*bitmap, pData.GdiBlt3->rect);
                    break;
                case EWsGcOpDrawBitmap:
                    {
                    // DrawBitmap(TPoint&, ) uses the size of the bitmap in twips, but 
                    // MWsGraphicsContext::DrawBitmap() takes a TRect in pixels, so we need to convert
                    TRect destRect(iWin->Screen()->DeviceMap().TwipsToPixels(bitmap->SizeInTwips()));
                    destRect.Move(pData.Bitmap->pos); //pos is defined in pixels, that's why we're not converting it
                    iGc->DrawBitmap(destRect, *bitmap);
                    break;
                    }
                case EWsGcOpDrawBitmap2:
                    iGc->DrawBitmap(pData.Bitmap2->rect, *bitmap);
                    break;
                case EWsGcOpDrawBitmap3:
                    iGc->DrawBitmap(pData.Bitmap3->rect, *bitmap, pData.Bitmap3->srcRect);
                    break;
                }
			break;
            }
        case EWsGcOpGdiBltMasked:
        case EWsGcOpGdiAlphaBlendBitmaps:
        case EWsGcOpDrawBitmapMasked:
            {
            TInt bitmapMaskHandle=0;
            TInt bitmapHandle = FbsBitmapHandle(aOpcode, pData, bitmapMaskHandle);
            const CFbsBitmap* bitmap = iWin->Redraw()->BitmapFromHandle(bitmapHandle);
            if(!bitmap)
                {
                WS_PANIC_DEBUG(EWsPanicBitmapNotFound);
                break;
                }
            
            CFbsBitmap* bitmapMask = iWin->Redraw()->BitmapFromHandle(bitmapMaskHandle);
            if(!bitmapMask)
                {
                WS_PANIC_DEBUG(EWsPanicBitmapNotFound);
                break;
                }
                        
            switch(aOpcode)
                {
                case EWsGcOpGdiBltMasked:
                    {
                    iGc->BitBltMasked(pData.GdiBltMasked->destination,*bitmap,
                                        pData.GdiBltMasked->source, *bitmapMask,
                                        pData.GdiBltMasked->invertMask);
                    break;
                    }
                case EWsGcOpGdiAlphaBlendBitmaps:
                    {
                    iGc->BitBltMasked(pData.AlphaBlendBitmaps->point, *bitmap,
                                        pData.AlphaBlendBitmaps->source, *bitmapMask,
                                        pData.AlphaBlendBitmaps->alphaPoint);
                    break;
                    }
                case EWsGcOpDrawBitmapMasked:
                    {
                    iGc->DrawBitmapMasked(pData.iBitmapMasked->iRect, *bitmap, 
                                        pData.iBitmapMasked->iSrcRect, *bitmapMask,
                                        pData.iBitmapMasked->iInvertMask);
                    break;
                    }
                }
			break;
			}
		case EWsGcOpDrawSegmentedPolygon:
			{
			TArrayWrapper<TPoint> points(iPolyPoints, iPolyPointListSize);
			iGc->DrawPolygon(points, BitGdiToMWsGraphicsContextMappings::Convert(pData.DrawSegmentedPolygon->fillRule));
			break;
			}
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
			GcOwnerPanic(EWservPanicOpcode); //deprecated, the client should never generate this op
			break;
		case EWsGcOpSetShadowColor:
			iGc->SetTextShadowColor(*pData.rgb);
			break;
		case EWsGcOpDrawResourceToPos:
		case EWsGcOpDrawResourceToRect:
		case EWsGcOpDrawResourceFromRectToRect:
		case EWsGcOpDrawResourceWithData:
			DoDrawResource(aOpcode, pData);
			break;
		default:
			TRAP_IGNORE(iWin->OwnerPanic(EWservPanicOpcode));
			break;
		}
	iGc->ResetClippingRegion();
	iCurrentClippingRegion = NULL;
	if (bGcDrawingOccurred)
		{
		GcDrawingDone();	//up the count (again for CRPs)
		}
	}
/**
 Helper function for drawing resources. 
 It extracts DWsDrawableSource objects which corresponds RWsDrawableResource object on the server side and then redirect call to concrete implementation. 
 @param aOpcode GC opcodes
 @param aData An extra data which will be used for resource drawing 
 */
void CPlaybackGc::DoDrawResource(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &aData)
	{
	CWsClient* owner=iWin->WsOwner(); // We can't just use iWsOwner - it can be null for stored commands
	if (owner!=NULL)
		{
		CWsDrawableSource *drawable = static_cast<CWsDrawableSource*>(owner->HandleToObj(*aData.Int, WS_HANDLE_DRAWABLE_SOURCE));
		if (!drawable)
			return;
		
		switch(aOpcode)
			{
		case EWsGcOpDrawResourceToPos:
			drawable->DrawResource(iGc, aData.DrawWsResourceToPos->pos, aData.DrawWsResourceToPos->rotation);
			break;
		case EWsGcOpDrawResourceToRect:
			drawable->DrawResource(iGc, aData.DrawWsResourceToRect->rect, aData.DrawWsResourceToRect->rotation);
			break;
		case EWsGcOpDrawResourceFromRectToRect:
			drawable->DrawResource(iGc, aData.DrawWsResourceFromRectToRect->rectDest, aData.DrawWsResourceFromRectToRect->rectSrc, aData.DrawWsResourceFromRectToRect->rotation);
			break;
		case EWsGcOpDrawResourceWithData:
			drawable->DrawResource(iGc, aData.DrawWsResourceWithData->rect, *aData.DrawWsResourceWithData->desc);
			break;
		default:
			break;
			}
		}
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
			WS_ASSERT_DEBUG(EFalse, EWsPanicInvalidOperation);
			break;
		}
	return handle;
	}

void CPlaybackGc::UpdateJustification(TText* aText,TInt aLen,const TDesC8& aCmdData,CGraphicsContext::TTextParameters* aParam)
	{
	iGc->UpdateJustification(BufferTPtr(aText,aLen,aCmdData), BitGdiToMWsGraphicsContextMappings::Convert(aParam));
	}

void CPlaybackGc::SendOriginIfRequired()
    {
    const TPoint currentOrigin(iMasterOrigin + iOrigin); 
    if (iSendOrigin || currentOrigin != iLastSentOrigin)
        {
        iGc->SetOrigin(currentOrigin);
        iLastSentOrigin = currentOrigin;
        iSendOrigin = EFalse;
        }
    }

void CPlaybackGc::SendClippingRegionIfRequired(const TRegion* aRegion)
    {
    if (iUserDefinedClippingRegion || iClippingRectSet || !iWin->Screen()->ChangeTracking())
        {
        iGc->SetClippingRegion(*aRegion);
        }
    }

void CPlaybackGc::DoDrawing(TWsGcOpcodes aOpcode, const TDesC8& aCmdData)
	{
	TWsGcCmdUnion pData;
	// coverity[returned_pointer]
	pData.any=aCmdData.Ptr();

	iIntersectedRegion.Clear();
	iIntersectedRegion.Copy(*iDrawRegion);
	
	if (iClippingRectSet)
		{
	    // MWsGraphicsContext doesn't provide a SetClippingRect API. If a client calls SetClippingRect
	    // the rect is passed to the render stage using MWsGraphicsContext::SetClippingRegion
        TRect clippingRectRelativeToScreen(iClippingRect);
        clippingRectRelativeToScreen.Move(iMasterOrigin);
		iIntersectedRegion.ClipRect(clippingRectRelativeToScreen);
        iIntersectedRegion.ClipRect(iWin->AbsRect());
		}

	SendOriginIfRequired();
	
	DoDrawCommand(aOpcode,aCmdData,&iIntersectedRegion);

	CGraphicsContext::TTextParameters contextParam;
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
			UpdateJustification((TText *)(pData.DrawText+1),pData.DrawText->length,aCmdData,NULL);
			break;
		case EWsGcOpDrawTextVertical:
			UpdateJustification((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length,aCmdData,NULL);
			break;
		case EWsGcOpDrawBoxText:
			UpdateJustification((TText *)(pData.BoxText+1),pData.BoxText->length,aCmdData,NULL);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			UpdateJustification((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length,aCmdData,NULL);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			UpdateJustification((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length,aCmdData,NULL);
			break;
		case EWsGcOpDrawBoxTextVertical:
			UpdateJustification((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length,aCmdData,NULL);
			break;
		case EWsGcOpDrawTextLocal:
			iGc->UpdateJustification(*pData.DrawTextLocal->desc,NULL);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iGc->UpdateJustification(*pData.BoxTextLocal->desc,NULL);
			break;
		case EWsGcOpDrawTextPtr:
			iGc->UpdateJustification(*pData.DrawTextPtr->text,NULL);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iGc->UpdateJustification(*pData.DrawTextVerticalPtr->text,NULL);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iGc->UpdateJustification(*pData.DrawBoxTextPtr->text,NULL);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iGc->UpdateJustification(*pData.DrawBoxTextVerticalPtr->text,NULL);
			break;
		/***************DrawTextInContext*****************************************************************/
		case EWsGcOpDrawTextInContext:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.DrawTextInContext+1),pData.DrawTextInContext->length,aCmdData,&contextParam);
				}
			break;
		case EWsGcOpDrawTextInContextVertical:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.DrawTextInContextVertical+1),pData.DrawTextInContextVertical->length,aCmdData,&contextParam);
				}
			break;
		case EWsGcOpDrawBoxTextInContext:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.BoxTextInContext+1),pData.BoxTextInContext->length,aCmdData,&contextParam);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised1:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.BoxTextInContextO1+1),pData.BoxTextInContextO1->length,aCmdData,&contextParam);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised2:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.BoxTextInContextO2+1),pData.BoxTextInContextO2->length,aCmdData,&contextParam);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVertical:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.DrawBoxTextInContextVertical+1),pData.DrawBoxTextInContextVertical->length,aCmdData,&contextParam);
				}
			break;
		case EWsGcOpDrawTextInContextLocal:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iGc->UpdateJustification(*pData.DrawTextInContextLocal->desc,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		case EWsGcOpDrawBoxTextInContextLocal:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iGc->UpdateJustification(*pData.BoxTextInContextLocal->desc,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		case EWsGcOpDrawTextInContextPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iGc->UpdateJustification(*pData.DrawTextInContextPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		case EWsGcOpDrawTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iGc->UpdateJustification(*pData.DrawTextInContextVerticalPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		case EWsGcOpDrawBoxTextInContextPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iGc->UpdateJustification(*pData.DrawBoxTextInContextPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iGc->UpdateJustification(*pData.DrawBoxTextInContextVerticalPtr->text,BitGdiToMWsGraphicsContextMappings::Convert(&contextParam));
				}
			break;
		}
	}

void CPlaybackGc::CommandL(TWsGcOpcodes aOpcode, const TDesC8& aCmdData)
	{
    WS_ASSERT_DEBUG(iWin,EWsPanicWindowNull);
	TWsGcCmdUnion pData;
	// coverity[returned_pointer]
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
	case EWsGcOpSetFaded: // deprecated
		// do nothing
		break;
	case EWsGcOpSetFadeParams: // deprecated
		// do nothing
		break;
	case EWsGcOpSetDrawMode:
		iGc->SetDrawMode(BitGdiToMWsGraphicsContextMappings::LossyConvert((CGraphicsContext::TDrawMode)*pData.UInt));
		break;
	case EWsGcOpUseFont:
		if (CWsFontCache::Instance()->UseFont(iFont, *pData.UInt))
			{
			CFbsBitGcFont font;
			if(font.Duplicate(*pData.UInt) == KErrNone)
				iGc->SetFont(&font);
			font.Reset();
			}
		else
			iGc->SetFontNoDuplicate(iFont);
		break;
	case EWsGcOpDiscardFont:
		CWsFontCache::Instance()->ReleaseFont(iFont);
		iGc->ResetFont();
		break;
	case EWsGcOpSetUnderlineStyle:
		iGc->SetUnderlineStyle(BitGdiToMWsGraphicsContextMappings::Convert(*pData.SetUnderlineStyle));
		break;
	case EWsGcOpSetStrikethroughStyle:
		iGc->SetStrikethroughStyle(BitGdiToMWsGraphicsContextMappings::Convert(*pData.SetStrikethroughStyle));
		break;
	case EWsGcOpUseBrushPattern:
		iGc->SetBrushPattern(*pData.handle);
		break;
	case EWsGcOpDiscardBrushPattern:
		iGc->ResetBrushPattern();
		break;
	case EWsGcOpSetBrushColor:
		iGc->SetBrushColor(*pData.rgb);
		break;
	case EWsGcOpSetPenColor:
		iGc->SetPenColor(*pData.rgb);
		break;
	case EWsGcOpSetPenStyle:
		iGc->SetPenStyle(BitGdiToMWsGraphicsContextMappings::Convert((CGraphicsContext::TPenStyle)*pData.UInt));
		break;
	case EWsGcOpSetPenSize:
		iGc->SetPenSize(*pData.Size);
		break;
	case EWsGcOpSetBrushStyle:
		{
		MWsGraphicsContext::TBrushStyle style = BitGdiToMWsGraphicsContextMappings::Convert((CGraphicsContext::TBrushStyle)*pData.UInt); 
		if (iGc->HasBrushPattern() || style != MWsGraphicsContext::EPatternedBrush)
			{
			iGc->SetBrushStyle(style);
			}
		break;
		}
	case EWsGcOpReset:
		CWsFontCache::Instance()->ReleaseFont(iFont);
		iGc->Reset();
		iOrigin.SetXY(0,0);
		iSendOrigin = ETrue; // we must call SetOrigin at next opportunity because it's likely the render stage implementation of Reset (when resetting origin) doesn't take into account the window origin
		ResetClippingRect();
		iGc->SetBrushColor(iWin->BackColor());
		break;
	case EWsGcOpSetBrushOrigin:
		iGc->SetBrushOrigin(*pData.Point);
		break;
	case EWsGcOpSetDitherOrigin:
		GcOwnerPanic(EWservPanicOpcode); //deprecated, the client should never generate this op
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
	case EWsGcOpSetOpaque: // deprecated
		// do nothing
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
	TInternalGcStatus::InternalizeGcAttributesL(iGc, bufReadStream);

	iOrigin.iX = bufReadStream.ReadInt32L();
	iOrigin.iY = bufReadStream.ReadInt32L();
	iSendOrigin = ETrue;

	iClippingRectSet=bufReadStream.ReadInt8L();
	
	// If there is a clipping rectangle data read it.
	if (iClippingRectSet)
		bufReadStream>>iClippingRect;
		
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
* @deprecated
*/
TInt CPlaybackGc::PlaceSurface(const TSurfaceConfiguration& /*aConfig*/)
    {
    return KErrNotSupported;
    }

/**	Get the drawing occurred indication counter.	
  	Callers can detect if drawing has occurred between two points 
 	by detecting that this count has changed.
 	Note that the changed value does not necessarily represent the exact number of operations which occurred.
  	@return value which changes each time GC drawing occurrs.
 **/
TInt CPlaybackGc::GcDrawingCount()
	{
	return iGcDrawingCounter;
	}

/**	Update the drawing occurred indication counter.	
 	Called internally each time a drawing operation updates the UI content 
 **/
void CPlaybackGc::GcDrawingDone()
	{
	iGcDrawingCounter++;
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
	    case MWsSurfacePlacement::EWsObjectInterfaceId:
	        return static_cast<MWsSurfacePlacement *>(this); //deprecated
		case MWsWindow::EWsObjectInterfaceId:
			return dynamic_cast<MWsWindow *>(iWin);
		case MWsGraphicsContext::EWsObjectInterfaceId:
			return static_cast<MWsGraphicsContext*>(iGc);
		case MWsUiBuffer::EWsObjectInterfaceId:
		case MWsFader::EWsObjectInterfaceId:	
			return iWin->Screen()->ResolveObjectInterface(aId);
		}
	return NULL;
	}
