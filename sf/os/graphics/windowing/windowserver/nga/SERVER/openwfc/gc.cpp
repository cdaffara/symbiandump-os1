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
#include "graphics/WSGRAPHICDRAWER.H"
#include "wsfont.h"

class TPlacedAttributes;

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
	iBackedUpWinGc=CFbsBitGc::NewL();
	iInternalStatus.ResetInternalStatus(iWin);
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

	iWin = win;
	if (iWin->Redraw()->OutputDevice()) // Activated on a backed up window
		iBackedUpWinGc->ActivateNoJustAutoUpdate(iWin->Redraw()->OutputDevice());
	iWin->GcActivated(this);
	iInternalStatus.iBrushColor = iWin->BackColor();
	if (iWin->Redraw()->OutputDevice())
		iBackedUpWinGc->SetBrushColor(iInternalStatus.iBrushColor);
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
	if (iWin->Redraw()->OutputDevice()) // Activated on a backed up window
		iBackedUpWinGc->ActivateNoJustAutoUpdate(iWin->Redraw()->OutputDevice());
	}

CWsGc::~CWsGc()
	{
	if (iWin!=NULL)
		Deactivate();
	delete iBackedUpWinGc;
	delete iPolyPoints;
	}

void CWsGc::Deactivate()
	{
	if (iWin)	// Protect against deactivating an already deactivated GC, this is allowed to aid clean up code.
		{
		CWsFontCache::Instance()->ReleaseFont(iFont);
		iBackedUpWinGc->Reset();
		iInternalStatus.ResetInternalStatus(iWin);
		iWin->GcDeactivated(this);
		CancelClippingRegion();
		iWin->Redraw()->GcDeactivate(this);
		iWin=NULL;
		}
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
	else	// Playing back from redraw store??
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
	iBackedUpWinGc->DrawPolygon((TPoint *)(aDrawPolygon+1),aDrawPolygon->numPoints,aDrawPolygon->fillRule);
	}

void CWsGc::StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon* aDrawPolygon)
	{
	WS_ASSERT_DEBUG(iWin->Redraw()->OutputDevice(), EWsPanicWindowType);
	if (iPolyPoints || !Rng(0, aDrawPolygon->totalNumPoints, KMaxTInt/2 - 1))	// Restarting without finishing old polygon or invalid size
		GcOwnerPanic(EWservPanicBadPolyData);
	iPolyPoints=(TPoint *)User::AllocL(aDrawPolygon->totalNumPoints*sizeof(TPoint));
	iPolyPointListSize=aDrawPolygon->totalNumPoints;
	}

void CWsGc::SegmentedDrawPolygonData(const TWsGcCmdSegmentedDrawPolygonData* aDrawPolygon)
	{
	WS_ASSERT_DEBUG(iWin->Redraw()->OutputDevice(), EWsPanicWindowType);
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
		iBackedUpWinGc->DrawPolyLineNoEndPoint(points,numPoints);
	else
		iBackedUpWinGc->DrawPolyLine(points,numPoints);
	}

void CWsGc::GcOwnerPanic(TClientPanic aPanic)
	{
	iBackedUpWinGc->SetClippingRegion(NULL);
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

void CWsGc::DoDrawCommand(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData)
	{
	WS_ASSERT_DEBUG(iWin,EWsPanicWindowNull);
	WS_ASSERT_DEBUG(iWin->Redraw()->OutputDevice(), EWsPanicWindowType); // Must be activated on a backed up window

	if (iUserDefinedClippingRegion)
		{
		if (iUserDefinedClippingRegion->Count()==0)
			return;
		if (iUserDefinedClippingRegion->IsContainedBy(TRect(TPoint(0,0), iBackedUpWinGc->Device()->SizeInPixels())))
			{
			iBackedUpWinGc->SetClippingRegion(iUserDefinedClippingRegion);
			}
		}
	CGraphicsContext::TTextParameters contextParam;
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
			GcOwnerPanic(EWservPanicOpcode); //deprecated, the client should never generate this op
			break;
		case EWsGcOpDrawPolyLineLocalBufLen:
			iBackedUpWinGc->DrawPolyLine(pData.DrawPolyLineLocalBufLen->points,pData.DrawPolyLineLocalBufLen->length);
			break;
		case EWsGcOpDrawPolyLineLocal:
			iBackedUpWinGc->DrawPolyLine(pData.PointList);
			break;
		case EWsGcOpDrawPolygonLocalBufLen:
			iBackedUpWinGc->DrawPolygon(pData.DrawPolygonLocalBufLen->points,pData.DrawPolygonLocalBufLen->length,pData.DrawPolygonLocalBufLen->fillRule);
			break;
		case EWsGcOpDrawPolygonLocal:
			iBackedUpWinGc->DrawPolygon(pData.DrawPolygonLocal->pointList,pData.DrawPolygonLocal->fillRule);
			break;
		case EWsGcOpDrawBitmapLocal:
			iBackedUpWinGc->DrawBitmap(pData.BitmapLocal->pos, pData.BitmapLocal->bitmap);
			break;
		case EWsGcOpDrawBitmap2Local:
			iBackedUpWinGc->DrawBitmap(pData.Bitmap2Local->rect, pData.Bitmap2Local->bitmap);
			break;
		case EWsGcOpDrawBitmap3Local:
			iBackedUpWinGc->DrawBitmap(pData.Bitmap3Local->rect, pData.Bitmap3Local->bitmap, pData.Bitmap3Local->srcRect);
			break;
		case EWsGcOpDrawBitmapMaskedLocal:
			iBackedUpWinGc->DrawBitmapMasked(pData.iBitmapMaskedLocal->iRect, pData.iBitmapMaskedLocal->iBitmap, pData.iBitmapMaskedLocal->iSrcRect, pData.iBitmapMaskedLocal->iMaskBitmap,pData.iBitmapMaskedLocal->iInvertMask);
			break;
		case EWsGcOpAlphaBlendBitmapsLocal:
			iBackedUpWinGc->AlphaBlendBitmaps(pData.AlphaBlendBitmapsLocal->point,pData.AlphaBlendBitmapsLocal->iBitmap,
						   			pData.AlphaBlendBitmapsLocal->source, pData.AlphaBlendBitmapsLocal->iAlpha,
									pData.AlphaBlendBitmapsLocal->alphaPoint);

			break;
		case EWsGcOpDrawText:
			iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.DrawText+1),pData.DrawText->length),pData.DrawText->pos);
			break;
		case EWsGcOpDrawBoxTextOptimised1:
			iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.BoxTextO1+1),pData.BoxTextO1->length),pData.BoxTextO1->box,
							pData.BoxTextO1->baselineOffset,CGraphicsContext::ELeft,0);
			break;
		case EWsGcOpDrawBoxTextOptimised2:
			iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.BoxTextO2+1),pData.BoxTextO2->length),pData.BoxTextO2->box,
							pData.BoxTextO2->baselineOffset,pData.BoxTextO2->horiz,pData.BoxTextO2->leftMrg);
			break;
		case EWsGcOpDrawTextPtr:
			iBackedUpWinGc->DrawText(*pData.DrawTextPtr->text,pData.DrawTextPtr->pos);
			break;
		case EWsGcOpDrawTextPtr1:
		   	iBackedUpWinGc->DrawText(*pData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawBoxText:
			iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.BoxText+1),pData.BoxText->length),pData.BoxText->box,pData.BoxText->baselineOffset,pData.BoxText->width,pData.BoxText->horiz,pData.BoxText->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iBackedUpWinGc->DrawText(*pData.DrawBoxTextPtr->text,pData.DrawBoxTextPtr->box,pData.DrawBoxTextPtr->baselineOffset,pData.DrawBoxTextPtr->width,pData.DrawBoxTextPtr->horiz,pData.DrawBoxTextPtr->leftMrg);
			break;
		case EWsGcOpDrawBoxTextPtr1:
			iBackedUpWinGc->DrawText(*pData.DrawBoxTextPtr->text,pData.DrawBoxTextPtr->box);
			break;
		case EWsGcOpDrawTextVertical:
			iBackedUpWinGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawTextVertical+1),pData.DrawTextVertical->length),pData.DrawTextVertical->pos
							,pData.DrawTextVertical->up);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iBackedUpWinGc->DrawTextVertical(*pData.DrawTextVerticalPtr->text,pData.DrawTextVerticalPtr->pos,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextVerticalPtr1:
			iBackedUpWinGc->DrawTextVertical(*pData.DrawTextVerticalPtr->text,pData.DrawTextVerticalPtr->up);
			break;
		case EWsGcOpDrawBoxTextVertical:
			iBackedUpWinGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawBoxTextVertical+1),pData.DrawBoxTextVertical->length),
							pData.DrawBoxTextVertical->box,	pData.DrawBoxTextVertical->baselineOffset,
							pData.DrawBoxTextVertical->up,(CGraphicsContext::TTextAlign)pData.DrawBoxTextVertical->vert,pData.DrawBoxTextVertical->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iBackedUpWinGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->baselineOffset
							,pData.DrawBoxTextVerticalPtr->width,pData.DrawBoxTextVerticalPtr->up,pData.DrawBoxTextVerticalPtr->vert,pData.DrawBoxTextVerticalPtr->margin);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr1:
			iBackedUpWinGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->up);
			break;
		case EWsGcOpDrawTextLocal:
			iBackedUpWinGc->DrawText(*pData.DrawTextLocal->desc,pData.DrawTextLocal->pos);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iBackedUpWinGc->DrawText(*pData.BoxTextLocal->desc,pData.BoxTextLocal->box,pData.BoxTextLocal->baselineOffset,
							pData.BoxTextLocal->horiz,pData.BoxTextLocal->leftMrg);
			break;
		/************* DrawText in Context function calls*********************************************/
		case EWsGcOpDrawTextInContext:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.DrawTextInContext+1),pData.DrawTextInContext->length),&contextParam,pData.DrawTextInContext->pos);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised1:
			contextParam.iStart = pData.BoxTextInContextO1->start;
			contextParam.iEnd = pData.BoxTextInContextO1->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.BoxTextInContextO1+1),pData.BoxTextInContextO1->length),&contextParam,pData.BoxTextInContextO1->box,
						pData.BoxTextInContextO1->baselineOffset,CGraphicsContext::ELeft,0);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised2:
			contextParam.iStart = pData.BoxTextInContextO2->start;
			contextParam.iEnd = pData.BoxTextInContextO2->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.BoxTextInContextO2+1),pData.BoxTextInContextO2->length),&contextParam,pData.BoxTextInContextO2->box,
						pData.BoxTextInContextO2->baselineOffset,pData.BoxTextInContextO2->horiz,pData.BoxTextInContextO2->leftMrg);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextPtr:
			contextParam.iStart = pData.DrawTextInContextPtr->start;
			contextParam.iEnd = pData.DrawTextInContextPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(*pData.DrawTextInContextPtr->text,&contextParam,pData.DrawTextInContextPtr->pos);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextPtr1:
			contextParam.iStart = pData.DrawTextInContextPtr->start;
			contextParam.iEnd = pData.DrawTextInContextPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(*pData.DrawTextInContextPtr->text,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContext:
			contextParam.iStart = pData.BoxTextInContext->start;
			contextParam.iEnd = pData.BoxTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(BufferTPtr((TText *)(pData.BoxTextInContext+1),pData.BoxTextInContext->length),&contextParam,
						pData.BoxTextInContext->box,pData.BoxTextInContext->baselineOffset,pData.BoxTextInContext->width,pData.BoxTextInContext->horiz,pData.BoxTextInContext->leftMrg);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextPtr:
			contextParam.iStart = pData.DrawBoxTextInContextPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(*pData.DrawBoxTextInContextPtr->text,&contextParam,pData.DrawBoxTextInContextPtr->box,pData.DrawBoxTextInContextPtr->baselineOffset,pData.DrawBoxTextInContextPtr->width,pData.DrawBoxTextInContextPtr->horiz,pData.DrawBoxTextInContextPtr->leftMrg);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextPtr1:
			contextParam.iStart = pData.DrawBoxTextInContextPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(*pData.DrawBoxTextInContextPtr->text,&contextParam,pData.DrawBoxTextInContextPtr->box);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextVertical:
			contextParam.iStart = pData.DrawTextInContextVertical->start;
			contextParam.iEnd = pData.DrawTextInContextVertical->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawTextInContextVertical+1),pData.DrawTextInContextVertical->length),&contextParam,pData.DrawTextInContextVertical->pos
						,pData.DrawTextInContextVertical->up);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawTextInContextVerticalPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawTextVertical(*pData.DrawTextInContextVerticalPtr->text,&contextParam,pData.DrawTextInContextVerticalPtr->pos,pData.DrawTextInContextVerticalPtr->up);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextVerticalPtr1:
			contextParam.iStart = pData.DrawTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawTextInContextVerticalPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawTextVertical(*pData.DrawTextInContextVerticalPtr->text,&contextParam,pData.DrawTextInContextVerticalPtr->up);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVertical:
			contextParam.iStart = pData.DrawBoxTextInContextVertical->start;
			contextParam.iEnd = pData.DrawBoxTextInContextVertical->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawTextVertical(BufferTPtr((TText *)(pData.DrawBoxTextInContextVertical+1),pData.DrawBoxTextInContextVertical->length),&contextParam,
						pData.DrawBoxTextInContextVertical->box,pData.DrawBoxTextInContextVertical->baselineOffset,
						pData.DrawBoxTextInContextVertical->up,(CGraphicsContext::TTextAlign)pData.DrawBoxTextInContextVertical->vert,pData.DrawBoxTextInContextVertical->margin);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawBoxTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextVerticalPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,&contextParam,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->baselineOffset
						,pData.DrawBoxTextVerticalPtr->width,pData.DrawBoxTextVerticalPtr->up,pData.DrawBoxTextVerticalPtr->vert,pData.DrawBoxTextVerticalPtr->margin);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr1:
			contextParam.iStart = pData.DrawBoxTextInContextVerticalPtr->start;
			contextParam.iEnd = pData.DrawBoxTextInContextVerticalPtr->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawTextVertical(*pData.DrawBoxTextVerticalPtr->text,&contextParam,pData.DrawBoxTextVerticalPtr->box,pData.DrawBoxTextVerticalPtr->up);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextLocal:
			contextParam.iStart = pData.DrawTextInContextLocal->start;
			contextParam.iEnd = pData.DrawTextInContextLocal->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(*pData.DrawTextInContextLocal->desc,&contextParam,pData.DrawTextInContextLocal->pos);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextLocal:
			contextParam.iStart = pData.BoxTextInContextLocal->start;
			contextParam.iEnd = pData.BoxTextInContextLocal->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->DrawText(*pData.BoxTextInContextLocal->desc,pData.BoxTextInContextLocal->box,pData.BoxTextInContextLocal->baselineOffset,
						pData.BoxTextInContextLocal->horiz,pData.BoxTextInContextLocal->leftMrg);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawLine:
			iBackedUpWinGc->DrawLine(pData.DrawLine->pnt1,pData.DrawLine->pnt2);
			break;
		case EWsGcOpDrawTo:
			iBackedUpWinGc->DrawLine(iLinePos,*pData.Point);
			break;
		case EWsGcOpDrawBy:
			iBackedUpWinGc->DrawLine(iLinePos,iLinePos+(*pData.Point));
			break;
		case EWsGcOpPlot:
			iBackedUpWinGc->Plot(*pData.Point);
			break;
		case EWsGcOpMoveTo:
		case EWsGcOpMoveBy:
			break;
		case EWsGcOpGdiBlt2Local:
			iBackedUpWinGc->BitBlt(pData.GdiBlt2Local->pos,pData.GdiBlt2Local->bitmap);
			break;
		case EWsGcOpGdiBlt3Local:
			iBackedUpWinGc->BitBlt(pData.GdiBlt3Local->pos,pData.GdiBlt3Local->bitmap, pData.GdiBlt3Local->rect);
			break;
		case EWsGcOpGdiBltMaskedLocal:
			iBackedUpWinGc->BitBltMasked(pData.GdiBltMaskedLocal->pos,pData.GdiBltMaskedLocal->bitmap,
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
					iBackedUpWinGc->BitBlt(pData.GdiBlt2->pos,scratchBimap);
					break;
				case EWsGcOpGdiWsBlt3:
					iBackedUpWinGc->BitBlt(pData.GdiBlt3->pos,scratchBimap, pData.GdiBlt3->rect);
					break;
				case EWsGcOpGdiWsBltMasked:
					{
					iBackedUpWinGc->BitBltMasked(pData.GdiBltMasked->destination,scratchBimap,
									pData.GdiBltMasked->source, scratchMaskBimap,
									pData.GdiBltMasked->invertMask);
					}
					break;
				case EWsGcOpGdiWsAlphaBlendBitmaps:
					{
					iBackedUpWinGc->AlphaBlendBitmaps(pData.AlphaBlendBitmaps->point,scratchBimap,
									   pData.AlphaBlendBitmaps->source, scratchMaskBimap,
									   pData.AlphaBlendBitmaps->alphaPoint);
					}
					break;
				case EWsGcOpWsDrawBitmapMasked:
					{
					iBackedUpWinGc->DrawBitmapMasked(pData.iBitmapMasked->iRect,scratchBimap, 
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
					iBackedUpWinGc->BitBlt(pData.GdiBlt2->pos,iScratchBitmap);
					break;
				case EWsGcOpGdiBlt3:
					iBackedUpWinGc->BitBlt(pData.GdiBlt3->pos,iScratchBitmap, pData.GdiBlt3->rect);
					break;
				case EWsGcOpGdiBltMasked:
					{
					TInt ret = iScratchMaskBitmap->Duplicate(pData.GdiBltMasked->maskHandle);
					if(ret == KErrNoMemory)
						break;	
					if (ret !=KErrNone)
						GcOwnerPanic(EWservPanicBitmap);
			
					iBackedUpWinGc->BitBltMasked(pData.GdiBltMasked->destination,iScratchBitmap,
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

					iBackedUpWinGc->AlphaBlendBitmaps(pData.AlphaBlendBitmaps->point, iScratchBitmap,
									   pData.AlphaBlendBitmaps->source, iScratchMaskBitmap,
									   pData.AlphaBlendBitmaps->alphaPoint);
					iScratchMaskBitmap->Reset();
					break;
					}
				case EWsGcOpDrawBitmap:
					iBackedUpWinGc->DrawBitmap(pData.Bitmap->pos, iScratchBitmap);
					break;
				case EWsGcOpDrawBitmap2:
					iBackedUpWinGc->DrawBitmap(pData.Bitmap2->rect, iScratchBitmap);
					break;
				case EWsGcOpDrawBitmap3:
					iBackedUpWinGc->DrawBitmap(pData.Bitmap3->rect, iScratchBitmap, pData.Bitmap3->srcRect);
					break;
				case EWsGcOpDrawBitmapMasked:
					{
					TInt ret = iScratchMaskBitmap->Duplicate(pData.iBitmapMasked->iMaskHandle);
					if (ret == KErrNoMemory)
						break;	
					if (ret != KErrNone)
						GcOwnerPanic(EWservPanicBitmap);

					iBackedUpWinGc->DrawBitmapMasked(pData.iBitmapMasked->iRect, iScratchBitmap, 
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
			iBackedUpWinGc->DrawPolygon(iPolyPoints,iPolyPointListSize,pData.DrawSegmentedPolygon->fillRule);
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
			iBackedUpWinGc->Clear(TRect(iWin->Size()));
			break;
		case EWsGcOpClearRect:
			iBackedUpWinGc->Clear(*pData.Rect);
			break;
		case EWsGcOpDrawRect:
			iBackedUpWinGc->DrawRect(*pData.Rect);
			break;
		case EWsGcOpDrawEllipse:
			iBackedUpWinGc->DrawEllipse(*pData.Rect);
			break;
		case EWsGcOpDrawRoundRect:
			iBackedUpWinGc->DrawRoundRect(*pData.Rect,pData.RoundRect->ellipse);
			break;
		case EWsGcOpDrawArc:
			iBackedUpWinGc->DrawArc(*pData.Rect,pData.ArcOrPie->start,pData.ArcOrPie->end);
			break;
		case EWsGcOpDrawPie:
			iBackedUpWinGc->DrawPie(*pData.Rect,pData.ArcOrPie->start,pData.ArcOrPie->end);
			break;
		case EWsGcOpCopyRect:
			iBackedUpWinGc->CopyRect(pData.CopyRect->pos,*pData.Rect);
			break;
		case EWsGcOpMapColors:
			GcOwnerPanic(EWservPanicOpcode); //deprecated, the client should never generate this op
			break;
			
		default:
			GcOwnerPanic(EWservPanicOpcode);
		}
	iBackedUpWinGc->SetClippingRegion(NULL);
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

TInt CWsGc::WsDrawableSourceHandle(TInt aOpcode, const TWsGcCmdUnion &aData) 
	{
	TInt handle=0;
	switch(aOpcode)
		{
		case EWsGcOpDrawResourceToPos:
			handle=aData.DrawWsResourceToPos->wsHandle;
			break;
		case EWsGcOpDrawResourceToRect:
			handle=aData.DrawWsResourceToRect->wsHandle;
			break;
		case EWsGcOpDrawResourceFromRectToRect:
			handle=aData.DrawWsResourceFromRectToRect->wsHandle;
			break;
		case EWsGcOpDrawResourceWithData:
			handle=aData.DrawWsResourceWithData->wsHandle;
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
		}
	return handle;
	}		

void CWsGc::UpdateJustification(TText* aText,TInt aLen)
	{
	iBackedUpWinGc->UpdateJustification(BufferTPtr(aText,aLen));
	}

void CWsGc::UpdateJustification(TText* aText,TInt aLen,CGraphicsContext::TTextParameters* aParam)
	{
	iBackedUpWinGc->UpdateJustification(BufferTPtr(aText,aLen),aParam);
	}

void CWsGc::DoDrawing2(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData)
	{
	WS_ASSERT_DEBUG(iWin->Redraw()->OutputDevice(), EWsPanicWindowType);

	iBackedUpWinGc->SetUserDisplayMode(iWin->DisplayMode());
	if (iClippingRectSet)
		{
		iBackedUpWinGc->SetOrigin(TPoint(0,0));
		iBackedUpWinGc->SetClippingRect(iClippingRect);
		}
	iBackedUpWinGc->SetOrigin(iOrigin);
	iInternalStatus.iOrigin = iOrigin;
	
	DoDrawCommand(aOpcode,pData);

	iBackedUpWinGc->SetUserDisplayMode(ENone);
	iBackedUpWinGc->CancelClippingRect();
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
			iBackedUpWinGc->UpdateJustification(*pData.DrawTextLocal->desc);
			break;
		case EWsGcOpDrawBoxTextLocal:
			iBackedUpWinGc->UpdateJustification(*pData.BoxTextLocal->desc);
			break;
		case EWsGcOpDrawTextPtr:
			iBackedUpWinGc->UpdateJustification(*pData.DrawTextPtr->text);
			break;
		case EWsGcOpDrawTextVerticalPtr:
			iBackedUpWinGc->UpdateJustification(*pData.DrawTextVerticalPtr->text);
			break;
		case EWsGcOpDrawBoxTextPtr:
			iBackedUpWinGc->UpdateJustification(*pData.DrawBoxTextPtr->text);
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			iBackedUpWinGc->UpdateJustification(*pData.DrawBoxTextVerticalPtr->text);
			break;
		/***************DrawTextInContext*****************************************************************/
		case EWsGcOpDrawTextInContext:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.DrawTextInContext+1),pData.DrawTextInContext->length,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextVertical:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.DrawTextInContextVertical+1),pData.DrawTextInContextVertical->length,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContext:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.BoxTextInContext+1),pData.BoxTextInContext->length,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised1:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.BoxTextInContextO1+1),pData.BoxTextInContextO1->length,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextOptimised2:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.BoxTextInContextO2+1),pData.BoxTextInContextO2->length,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVertical:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				UpdateJustification((TText *)(pData.DrawBoxTextInContextVertical+1),pData.DrawBoxTextInContextVertical->length,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextLocal:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->UpdateJustification(*pData.DrawTextInContextLocal->desc,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextLocal:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->UpdateJustification(*pData.BoxTextInContextLocal->desc,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->UpdateJustification(*pData.DrawTextInContextPtr->text,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->UpdateJustification(*pData.DrawTextInContextVerticalPtr->text,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->UpdateJustification(*pData.DrawBoxTextInContextPtr->text,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr:
			contextParam.iStart = pData.DrawTextInContext->start;
			contextParam.iEnd = pData.DrawTextInContext->end;
			if(contextParam.iStart < contextParam.iEnd)
				{
				iBackedUpWinGc->UpdateJustification(*pData.DrawBoxTextInContextVerticalPtr->text,&contextParam);
				}
			else
				{
				iWin->WsOwner()->PPanic(EWservPanicInvalidParameter);
				}
			break;
		}
	}

void CWsGc::DoDrawing1(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData)
	{
	WS_ASSERT_DEBUG(iWin->Redraw()->OutputDevice(), EWsPanicWindowType);

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

		case EWsGcOpDrawTextInContextPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdDrawTextInContextPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawTextInContextPtr1;
			toGo=pData.DrawTextInContext->length;
			pNewData.DrawTextInContextPtr->pos=pData.DrawTextInContext->pos;
			pNewData.DrawTextInContextPtr->start=pData.DrawTextInContext->start;
			pNewData.DrawTextInContextPtr->end=pData.DrawTextInContext->end;
			string=&(pNewData.DrawTextInContextPtr->text);
			break;
			
		case EWsGcOpDrawTextInContextVerticalPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdDrawTextInContextVerticalPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawTextInContextVerticalPtr1;
			toGo=pData.DrawTextInContextVertical->length;
  			pNewData.DrawTextInContextVerticalPtr->pos=pData.DrawTextInContextVerticalPtr->pos;
  			pNewData.DrawTextInContextVerticalPtr->up=pData.DrawTextInContextVerticalPtr->up;
  			pNewData.DrawTextInContextVerticalPtr->start=pData.DrawTextInContextVerticalPtr->start;
  			pNewData.DrawTextInContextVerticalPtr->end=pData.DrawTextInContextVerticalPtr->end;
			string=&(pNewData.DrawTextVerticalPtr->text);
			break;
			
		case EWsGcOpDrawBoxTextInContextPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdBoxTextInContextPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawBoxTextInContextPtr1;
			toGo=pData.BoxTextInContext->length;
   			pNewData.DrawBoxTextInContextPtr->box=pData.BoxTextInContext->box;
   			pNewData.DrawBoxTextInContextPtr->baselineOffset=pData.BoxTextInContext->baselineOffset;
   			pNewData.DrawBoxTextInContextPtr->horiz=pData.BoxTextInContext->horiz;
   			pNewData.DrawBoxTextInContextPtr->leftMrg=pData.BoxTextInContext->leftMrg;
   			pNewData.DrawBoxTextInContextPtr->width=pData.BoxTextInContext->width;
   			pNewData.DrawBoxTextInContextPtr->start=pData.BoxTextInContext->start;
  			pNewData.DrawBoxTextInContextPtr->end=pData.BoxTextInContext->end;
			string=&(pNewData.DrawBoxTextPtr->text);
			break;
			
		case EWsGcOpDrawBoxTextInContextVerticalPtr:
			WS_ASSERT_DEBUG(sizeof(TWsGcLargeStruct)>=sizeof(TWsGcCmdBoxTextInContextVerticalPtr), EWsPanicGcStructSizeError);
			opcode=EWsGcOpDrawBoxTextInContextVerticalPtr1;
			toGo=pData.DrawBoxTextInContextVertical->length;
   			pNewData.DrawBoxTextInContextVerticalPtr->box=pData.DrawBoxTextInContextVertical->box;
   			pNewData.DrawBoxTextInContextVerticalPtr->baselineOffset=pData.DrawBoxTextInContextVertical->baselineOffset;
   			pNewData.DrawBoxTextInContextVerticalPtr->up=pData.DrawBoxTextInContextVertical->up;
   			pNewData.DrawBoxTextInContextVerticalPtr->vert=pData.DrawBoxTextInContextVertical->vert;
   			pNewData.DrawBoxTextInContextVerticalPtr->margin=pData.DrawBoxTextInContextVertical->margin;
   			pNewData.DrawBoxTextInContextVerticalPtr->width=pData.DrawBoxTextInContextVertical->width;
   			pNewData.DrawBoxTextInContextVerticalPtr->start=pData.DrawBoxTextInContextVertical->start;
   			pNewData.DrawBoxTextInContextVerticalPtr->end=pData.DrawBoxTextInContextVertical->end;
 			string=&(pNewData.DrawBoxTextInContextVerticalPtr->text);			
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
			iInternalStatus.iDrawMode = (CGraphicsContext::TDrawMode) (*pData.UInt);
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetDrawMode(iInternalStatus.iDrawMode);
			break;
		case EWsGcOpUseFont:
			if (CWsFontCache::Instance()->UseFont(iFont, *pData.UInt))
				{	// Couldn't cache it
				if (iWin->Redraw()->OutputDevice())
					{
					TInt ret = iBackedUpWinGc->UseFont(*pData.UInt);
					if (ret == KErrNoMemory)
						return;
					if (ret != KErrNone)
						GcOwnerPanic(EWservPanicFont);
					}
				iInternalStatus.iFontHandle = *pData.UInt;
				}
			else
				{
				if (iFont==NULL)
					{
					iInternalStatus.iFontHandle = NULL;
					GcOwnerPanic(EWservPanicFont);
					}
				if (iWin->Redraw()->OutputDevice())
					iBackedUpWinGc->UseFontNoDuplicate(iFont);
				iInternalStatus.iFontHandle = iFont->Handle();
				}
			break;
		case EWsGcOpDiscardFont:
			CWsFontCache::Instance()->ReleaseFont(iFont);
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->DiscardFont();
			iInternalStatus.iFontHandle = NULL;
			break;
		case EWsGcOpSetUnderlineStyle:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetUnderlineStyle(*pData.SetUnderlineStyle);
			iInternalStatus.iUnderline = *pData.SetUnderlineStyle;
			break;
		case EWsGcOpSetStrikethroughStyle:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetStrikethroughStyle(*pData.SetStrikethroughStyle);
			iInternalStatus.iStrikethrough = *pData.SetStrikethroughStyle;
			break;
		case EWsGcOpUseBrushPattern:
			if (iWin->Redraw()->OutputDevice())
				{
				TInt ret = iBackedUpWinGc->UseBrushPattern(*pData.handle);
				if (ret == KErrNoMemory)
					return;
				if (ret != KErrNone)
					GcOwnerPanic(EWservPanicBitmap);
				}
			else
				{
				// Make sure the bitmap handle is valid
				TInt ret = iScratchBitmap->Duplicate(*pData.handle);
				if (ret == KErrNoMemory)
					return;
				if (ret != KErrNone)
					GcOwnerPanic(EWservPanicBitmap);
				iScratchBitmap->Reset();
				}
			iInternalStatus.iBrushPatternHandle = *pData.handle;
			break;
		case EWsGcOpDiscardBrushPattern:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->DiscardBrushPattern();
			iInternalStatus.iBrushPatternHandle = NULL;
			if (iInternalStatus.iBrushStyle == CGraphicsContext::EPatternedBrush)
				iInternalStatus.iBrushStyle = CGraphicsContext::ENullBrush;
			break;
		case EWsGcOpSetBrushColor:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetBrushColor(*pData.rgb);
			iInternalStatus.iBrushColor = *pData.rgb; 
			break;
		case EWsGcOpSetPenColor:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetPenColor(*pData.rgb);
			iInternalStatus.iPenColor = *pData.rgb;
			break;
		case EWsGcOpSetPenStyle:
			iInternalStatus.iPenStyle = (CGraphicsContext::TPenStyle) (*pData.UInt);
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetPenStyle(iInternalStatus.iPenStyle);
			break;
		case EWsGcOpSetPenSize:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetPenSize(*pData.Size);
			iInternalStatus.iPenSize = *pData.Size; 
			break;
		case EWsGcOpSetBrushStyle:
			if ((CGraphicsContext::TBrushStyle)*pData.UInt==CGraphicsContext::EPatternedBrush && !iInternalStatus.iBrushPatternHandle)
				GcOwnerPanic(EWservPanicNoBrush);
			iInternalStatus.iBrushStyle = (CGraphicsContext::TBrushStyle) (*pData.UInt);
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetBrushStyle(iInternalStatus.iBrushStyle); 
			break;
		case EWsGcOpReset:
			CWsFontCache::Instance()->ReleaseFont(iFont);
			iBackedUpWinGc->Reset();
			iOrigin.SetXY(0,0);
			ResetClippingRect();
			iInternalStatus.ResetInternalStatus(iWin);
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetBrushColor(iWin->BackColor());
			iInternalStatus.iBrushColor = iWin->BackColor();
			break;
		case EWsGcOpSetBrushOrigin:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetBrushOrigin(*pData.Point);
			iInternalStatus.iBrushOrigin = *pData.Point; 
			break;
		case EWsGcOpSetDitherOrigin:
			GcOwnerPanic(EWservPanicOpcode); //deprecated, the client should never generate this op
			break;
		case EWsGcOpSetWordJustification:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetWordJustification(pData.SetJustification->excessWidth, pData.SetJustification->numGaps);
			iInternalStatus.iWordExcessWidth = pData.SetJustification->excessWidth;
			iInternalStatus.iWordNumChars = pData.SetJustification->numGaps;
			break;
		case EWsGcOpSetCharJustification:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetCharJustification(pData.SetJustification->excessWidth, pData.SetJustification->numGaps);
			iInternalStatus.iCharExcessWidth = pData.SetJustification->excessWidth;
			iInternalStatus.iCharNumChars = pData.SetJustification->numGaps;
			break;
		case EWsGcOpSetOrigin:
			SetOrigin(*pData.Point);
			iInternalStatus.iOrigin = *pData.Point; 
			break;
		case EWsGcOpSetOpaque: // deprecated
			// do nothing
			break;
		case EWsGcOpSetShadowColor:
			if (iWin->Redraw()->OutputDevice())
				iBackedUpWinGc->SetShadowColor(*pData.rgb);
			iInternalStatus.iShadowColor = *pData.rgb; 
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
	
	iWin->SetValidRedraw();

	switch(aOpcode)
		{
	case EWsGcOpStartSegmentedDrawPolygon:

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
		if (iWin->Redraw()->OutputDevice())
			iBackedUpWinGc->SetFaded(*pData.Bool);
		break;
	case EWsGcOpSetFadeParams:
		if (iWin->Redraw()->OutputDevice())
			iBackedUpWinGc->SetFadingParameters(WservEncoding::ExtractFirst8BitValue(*pData.UInt),WservEncoding::ExtractSecond8BitValue(*pData.UInt));
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
		if (!iInternalStatus.iFontHandle)
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
	// ExternalizeGcAttributesL(). As this method does not return the amount of
	// the data externalised we use methods in the underlying stream to 
	// calculate it. We do this because we need to return an accurate count of
	// data serialised from this method so that the caller can update its 
	// buffer write counter.
	MStreamBuf* ptrToRawStream = bufWriteStream.Sink();	// This is the real stream

	// Position of read seek pointer before externalise
	TStreamPos size1 = ptrToRawStream->TellL(MStreamBuf::EWrite); 

	// Save the font/bitmap server data
	iInternalStatus.ExternalizeGcAttributesL(bufWriteStream);

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
	aWriteStream.WriteUint8L(iInternalStatus.iBrushColor.Alpha());
	aWriteStream.WriteUint8L(iInternalStatus.iPenColor.Alpha());
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

