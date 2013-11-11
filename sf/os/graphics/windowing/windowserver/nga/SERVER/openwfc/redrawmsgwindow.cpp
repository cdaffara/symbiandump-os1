// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Window redraw code, three sorts of redrawing are supported
// CRedrawMsgWindow handles it via sending a redraw message to the client
// 
//

#include "redrawmsgwindow.h"
#include "gc.h"
#include "playbackgc.h"
#include "inifile.h"
#include "rootwin.h"
#include "wstop.h"
#include "ANIM.H"
#include "EVQUEUE.H"
#include <s32mem.h>
#include <gdi.h>
#include "panics.h"
#include "rootwin.h"
#include "EVENT.H"
#include "wsfont.h"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include <graphics/surface.h>
#include "windowelementset.h"

#include "drawresource.h"
#include "../debuglog/DEBUGLOG.H"

const TUint KDrawBufferGranularity = 240;
const TInt KRedrawRegionGranularity = 8;
const TInt KReadBufferMaxLen=0x100;
const TInt KRegionCompressThreshold = 6; // number of rectangles in region at which we try to compress it

TBool CWsRedrawMsgWindow::iAtomic = EFalse;

extern CDebugLogBase *wsDebugLog;

#ifndef _DEBUG

#define LOG_WINDOW_DRAW_COMMANDS_START(wswin,region)
#define LOG_WINDOW_DRAW_COMMANDS_END(wswin)
#define LOG_REDRAW_SEGMENT(segmentIndex,segmentType)
#define LOG_REDRAW_SEGMENT_REGION(region)
#define LOG_PLAYBACK_GC_COMMAND(opcode,data)

#else

#define LOG_WINDOW_DRAW_COMMANDS_START(wswin,region) LogDrawCommandsStart(wswin,region)
#define LOG_WINDOW_DRAW_COMMANDS_END(wswin) LogDrawCommandsEnd(wswin)
#define LOG_REDRAW_SEGMENT(segmentIndex,segmentType) LogRedrawSegment(segmentIndex, segmentType)
#define LOG_REDRAW_SEGMENT_REGION(region) {if(wsDebugLog){ LogRegion(region);}}
#define LOG_PLAYBACK_GC_COMMAND(opcode,data)    {if (wsDebugLog) {wsDebugLog->Command(WS_HANDLE_GC, opcode, data, NULL);}}

LOCAL_C void LogRedrawSegment(TUint aSegmentIndex, CWsRedrawMsgWindow::TRedrawSegmentType aSegmentType)
	{
	if (wsDebugLog)
		{
		TBuf<LogTBufSize> log;
		TTruncateOverflow overflow;
		_LIT(KLogRedrawSegment, ">> CRedrawSegment[%d] ");
		log.AppendFormat(KLogRedrawSegment, &overflow, aSegmentIndex);
		_LIT(KLogRedrawSegmentPending, "Pending");
		_LIT(KLogRedrawSegmentRedraw, "Redraw");
		switch(aSegmentType)
			{
			case CWsRedrawMsgWindow::ESegmentTypePendingRedraw :
				log.AppendFormat(KLogRedrawSegmentPending, &overflow);
				break;
			case CWsRedrawMsgWindow::ESegmentTypeRedraw :
				log.AppendFormat(KLogRedrawSegmentRedraw, &overflow);
				break;
			default :
				{
				}
			}
		wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
		}
	}

LOCAL_C void LogDrawCommandsStart(const CWsWindow* aWsWin, const TRegion* aRegion)
	{
	if (wsDebugLog)
		{
		_LIT(KLogDrawCommandsStart, ">> CWsRedrawMsgWindow::DrawCommandsL() [%S][app %d] RWindow[%d]");
		const TDesC& clientName = aWsWin->WsOwner()->Client().FullName();
		TBuf<LogTBufSize> log;
		TTruncateOverflow overflow;
		log.AppendFormat(KLogDrawCommandsStart, &overflow, &clientName, aWsWin->WsOwner()->ConnectionHandle(), aWsWin->LogHandle());
		wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
		LogRegion(aRegion);
		}
	}

LOCAL_C void LogDrawCommandsEnd(const CWsWindow* aWsWin)
	{
	if (wsDebugLog)
		{
		_LIT(KLogDrawCommandsEnd, "<< CWsRedrawMsgWindow::DrawCommandsL() [%S][app %d] RWindow[%d]");
		const TDesC& clientName = aWsWin->WsOwner()->Client().FullName();
		TBuf<LogTBufSize> log;
		TTruncateOverflow overflow;
		log.AppendFormat(KLogDrawCommandsEnd, &overflow, &clientName, aWsWin->WsOwner()->ConnectionHandle(), aWsWin->LogHandle());		
		wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
		}
	}

#endif

//
// Redraw windows //
//

CWsRedrawMsgWindow::CRedrawSegment::CRedrawSegment(CWsRedrawMsgWindow& aRedraw) : iRedraw(aRedraw) 
	{
	}

CWsRedrawMsgWindow::CRedrawSegment* CWsRedrawMsgWindow::CRedrawSegment::NewLC(const TRect& aRect, TRedrawSegmentType aNewRegionType, CWsRedrawMsgWindow& aRedraw) 
	{
	CRedrawSegment* self = new (ELeave) CRedrawSegment(aRedraw);
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aNewRegionType);
	return self;
	}

void CWsRedrawMsgWindow::StaticInitL()
	{
	// Note that the wsini.ini setting NONREDRAWAGELIMIT is obselete in
	// Wserv2 NGA.  The effective behaviour of the system is as if the
	// value was set to zero.
	
	_LIT(KAtomicRedraws, "ATOMICREDRAWS");
	if (WsIniFile->FindVar(KAtomicRedraws) || 
			WsIniFile->FindVar(KWSERVIniFileVarChangeTracking)) //ChangeTracking requires atomic redraws to be enabled to
		iAtomic = ETrue;										//prevent playing redraw-segemts before they are completed.
	else
		iAtomic = EFalse;
	}

static TInt FindBitmapRefByHandle(const TInt* aKey, const CFbsBitmapRef& aBitmapRef)
    { // compare handles
    return *aKey - aBitmapRef.Handle();
    }

static TInt InsertBitmapRefByHandle(const CFbsBitmapRef& aFirst, const CFbsBitmapRef& aSecond)
    {
    return aFirst.Handle() - aSecond.Handle();
    }

void CWsRedrawMsgWindow::CRedrawSegment::ReleaseFontsBitmapsDrawableSources()
	{
	// release Bitmap, Font and Drawable Source handles
	TInt count = iWsBitmapArray.Count();
	TInt ii;
	for (ii = count - 1; ii >= 0; ii--)
		{
		iWsBitmapArray[ii]->DecRefCount();
		iWsBitmapArray.Remove(ii);
		}

	count = iWsFontArray.Count();
	for (ii = count - 1; ii >= 0; --ii)
		{
		CWsFontCache::Instance()->ReleaseFont(iWsFontArray[ii]);
		iWsFontArray.Remove(ii);
		}

	iRedraw.CleanupBitmapRefArray(iBitmapHandleArray);
	iBitmapHandleArray.Close();

	count = iWsDrawableSourceArray.Count();
	for(ii = count - 1 ; ii >= 0; ii--)
		{
		iWsDrawableSourceArray[ii]->DecRefCount();
		iWsDrawableSourceArray.Remove(ii);
		}
	}

/* Set new rectangle and region type for initial or reset redraw region
 @leave KErrNoMemory no memory to update region details
 */
void CWsRedrawMsgWindow::CRedrawSegment::ConstructL(const TRect& aRect, TRedrawSegmentType aNewRegionType)
	{
	iDrawCommands = CBufSeg::NewL(KDrawBufferGranularity);
	iCreationTime.UniversalTime();

	iRegion.AddRect(aRect);
	if (iRegion.CheckError())
		{
		User::Leave(KErrNoMemory);
		}
	iRedrawSegmentType = aNewRegionType;
	}

CWsRedrawMsgWindow::CRedrawSegment::~CRedrawSegment()
	{
	
	if (iDrawCommands)
		{
		delete iDrawCommands;
		}
	iRegion.Close();
	// Release Font,  Bitmap and Drawable Source handles, close arrays
	ReleaseFontsBitmapsDrawableSources();
	iWsBitmapArray.Close();
	iWsFontArray.Close();
	iDrawerArray.Close();	
	iWsDrawableSourceArray.Close();
	}


TInt CWsRedrawMsgWindow::CRedrawSegment::SizeInBytes() const
	{
	TInt size = sizeof(CWsRedrawMsgWindow::CRedrawSegment);
	size += iDrawCommands->Size();
	size += iBitmapHandleArray.Count() * sizeof(TInt);
	size += iWsBitmapArray.Count() * sizeof(DWsBitmap);
	size += iWsFontArray.Count() * sizeof(CWsFbsFont);
	size += iDrawerArray.Count() * sizeof(TGraphicDrawerId);
	size += iWsDrawableSourceArray.Count() * sizeof(CWsDrawableSource);
	return size;
	}

TBool CWsRedrawMsgWindow::CRedrawSegment::AppendNonDuplicateBitmapHandleL(TInt aHandle)
    {
    TBool handleAppend = EFalse;
    TInt indexBitmapHandle = iBitmapHandleArray.Find(aHandle);
    if (indexBitmapHandle<0)
        {
        iBitmapHandleArray.AppendL(aHandle);
        handleAppend = ETrue;
        }
    return handleAppend;
    }

CWsRedrawMsgWindow::CWsRedrawMsgWindow(CWsWindow *aWin)
	:CWsWindowRedraw(aWin), iBackColor(aWin->RootWindow()->DefaultBackgroundColor()), iFlags(EBackgroundClear|EStoringEntireWindow),
	iRedrawSegments(KRedrawRegionGranularity),
	iCurrentSegment(0),
	iOSBStatus(ETrue)
	{
	}

void CWsRedrawMsgWindow::ConstructL()
	{
	CWsWindowRedraw::ConstructL();
	Invalidate(&WsWin()->Rel());
	iWsWin->WsOwner()->RedrawQueue()->ReCalcOrder();
	}

CWsRedrawMsgWindow::~CWsRedrawMsgWindow()
	{
	WS_ASSERT_DEBUG(iMemoryLock == 0, EWsPanicMemoryLock);
	RemoveFromRedrawQueueIfEmpty();	
	iInvalid.Close();
	iLocalRedrawRegion.Close();
	iRedrawSegments.ResetAndDestroy();
	TInt count = iFbsBitmapRefArray.Count();
	WS_ASSERT_DEBUG(count==0,EWsPanicBitmapArrayNotEmpty);
	iFbsBitmapRefArray.ResetAndDestroy();
	}

/**
These three functions actually check for a value they have already asserted on.  This is intentional.
*/
void CWsRedrawMsgWindow::ExpandCommandBufferL(TInt aLength)
	{
	WS_ASSERT_DEBUG(iCurrentSegment != NULL, EWsPanicRedrawSegmentsInvalidState);
	
	if (iCurrentSegment)
		{
		// need more space?
		if (iCurrentSegment->iCurrentCommandBufferWritePos + aLength > iCurrentSegment->iDrawCommands->Size())
			{
			iCurrentSegment->iDrawCommands->ResizeL(iCurrentSegment->iCurrentCommandBufferWritePos + aLength);
			}
		}
	}

void CWsRedrawMsgWindow::CommandBufferWrite(const TDesC8& aDes, TInt aLength)
	{
	WS_ASSERT_DEBUG(iCurrentSegment != NULL, EWsPanicRedrawSegmentsInvalidState);
	WS_ASSERT_DEBUG(iCurrentSegment->iCurrentCommandBufferWritePos + aLength <= iCurrentSegment->iDrawCommands->Size(), EWsPanicDrawCommandsInvalidState);
	if (iCurrentSegment)
		{
		iCurrentSegment->iDrawCommands->Write(iCurrentSegment->iCurrentCommandBufferWritePos, aDes, aLength);
		iCurrentSegment->iCurrentCommandBufferWritePos += aLength;
		}
	}

void CWsRedrawMsgWindow::CommandBufferWrite(const TAny* aPtr,TInt aLength)
	{
	WS_ASSERT_DEBUG(iCurrentSegment != NULL, EWsPanicRedrawSegmentsInvalidState);
	WS_ASSERT_DEBUG(iCurrentSegment->iCurrentCommandBufferWritePos + aLength <= iCurrentSegment->iDrawCommands->Size(), EWsPanicDrawCommandsInvalidState);
	if (iCurrentSegment)
		{
		iCurrentSegment->iDrawCommands->Write(iCurrentSegment->iCurrentCommandBufferWritePos, aPtr, aLength);
		iCurrentSegment->iCurrentCommandBufferWritePos += aLength;
		}
	}

/*------------------------------------------------------------------------------
  Description: Processes draw commands. These are received as opcodes.
 -----------------------------------------------------------------------------*/
TBool CWsRedrawMsgWindow::CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)
	{
	switch(aOpcode)
		{
		case EWsWinOpEnableOSB:
			iOSBStatus = ETrue;
			break;
		case EWsWinOpDisableOSB:
			iOSBStatus = EFalse;
			break;
		case EWsWinOpSetBackgroundColor:
			iBackColor = *aCmd.rgb;
			iFlags |= EBackgroundClear;
			break;
		case EWsWinOpSetNoBackgroundColor:
			iFlags &= ~EBackgroundClear;
			break;
		case EWsWinOpInvalidate:
			Invalidate(aCmd.rect);
			break;
		case EWsWinOpInvalidateFull:
			Invalidate();
			break;
		case EWsWinOpBeginRedraw:
			BeginRedraw(aCmd.rect);
			ValidateRect(aCmd.rect);
			break;
		case EWsWinOpBeginRedrawFull:
			BeginRedraw(NULL);
			ValidateRect(NULL);
			break;
		case EWsWinOpEndRedraw:
			EndRedraw();
			break;
		case EWsWinOpGetInvalidRegionCount:
			{
			SetReply(iInvalid.Count());
			}
			break;
		case EWsWinOpGetInvalidRegion:
			{
			if ((*aCmd.Int) <= 0)
				OwnerPanic(EWservPanicInvalidRegionCount);
			if ((!iInvalid.CheckError()) && iInvalid.Count() == (*aCmd.Int))
				{
				CWsClient::ReplyBuf(iInvalid.RectangleList(),(*aCmd.Int) * sizeof(TRect));
				SetReply(EFalse);
				}
			else
				SetReply(ETrue);
			}
			break;
		case EWsWinOpStoreDrawCommands:
			/* If the client asks us not to store commands, we still store the commands
			for the region of the window which can be seen through the parent, but
			won't attempt to obtain the entire window.
			*/
			if (*aCmd.Bool)
				{
				SetScope(EStoreEntireWindow);
				}
			else
				{
				// Clients that turn their redraw store off will still get one,
				// but it will only attempt to store the current viewport.
				SetScope(EStoreViewport);
				}
			break;
		case EWsWinOpHandleTransparencyUpdate:	// deprecated
		case EWsWinOpSetTransparencyBitmap:		// deprecated
		case EWsWinOpSetTransparencyFactor:		// deprecated
		case EWsWinOpSetTransparencyBitmapCWs:	// deprecated
			break;	// do nothing.
		case EWsWinOpIsRedrawStoreEnabled:
			SetReply(ETrue);
			break;
		case EWsWinOpClearRedrawStore:
			DiscardStoredCommands();
			break;
		case EWsWinOpSetBackgroundSurface:
			SetBackgroundSurfaceL(*aCmd.Surface);
			break;
		case EWsWinOpSetBackgroundSurfaceConfig:
			SetBackgroundSurfaceL(aCmd.SurfaceConfigurationAndTrigger->surfaceConfig, aCmd.SurfaceConfigurationAndTrigger->triggerRedraw, EFalse);
			break;
		case EWsWinOpRemoveBackgroundSurface:
			RemoveBackgroundSurface(*aCmd.Bool);
			break;
		case EWsWinOpGetBackgroundSurfaceConfig:
			{
			TSurfaceConfiguration tempConfiguration = *aCmd.SurfaceConfiguration;
			GetBackgroundSurfaceL(tempConfiguration);
			TInt tempSize = aCmd.SurfaceConfiguration->Size();
			if (sizeof(TSurfaceConfiguration)<tempSize)
				tempSize = sizeof(TSurfaceConfiguration);
			CWsClient::ReplyBuf(&tempConfiguration,tempSize);
			}
			break;
		default:
			return(EFalse);
		}
	return(ETrue);
	}

/**
*/
void CWsRedrawMsgWindow::BeginRedraw(const TRect* aRect)
	{
	if(InRedraw())
		OwnerPanic(EWservPanicDrawCommandsInvalidState);
	iFlags|=EBeginEndRedraw;
	TRAPD(err,DoBeginRedrawL(aRect));
	DiscardStoredCommandsIfError(err);
	}

void CWsRedrawMsgWindow::DoBeginRedrawL(const TRect* aRect)
	{
	const TRect redrawRect = (aRect ? *aRect : TRect(WsWin()->Size()));
	if (redrawRect.IsEmpty())
		{
		//Skip empty rects since they are not added to the region
		iCurrentSegment = NULL;
		}
	else
		{
		CreateNewSegmentL(redrawRect, CWsRedrawMsgWindow::ESegmentTypePendingRedraw);
		if (!iAtomic)
			PromotePendingSegment();
		}
	}

void CWsRedrawMsgWindow::Invalidate(const TRect * aRect)
	{
	//The memory allocation in this function can trigger a call to ReleaseMemory(), which would
	//recursively call this function again. This would cause a memory leak. To avoid this
	//we call Lock() to block ReleaseMemory() from this object while executing this function.
    Lock();
	if (!aRect)
		{
		iInvalid.Clear();
		iInvalid.Copy(iWsWin->WindowArea());
		iInvalid.Offset(-iWsWin->Origin());
		}
	else if((!aRect->IsEmpty()) && aRect->IsNormalized())
		{
		iInvalid.AddRect(*aRect);
		iInvalid.Tidy();
		}
	if (iWsWin->IsVisible())
		{
		QueueRedraw();
		iWsWin->WsOwner()->TriggerRedraw(); //wtf isn't the redrawq already scheduling itself?
		}
    Unlock();
	}

/**
Obtains a region from the redraw store, intersects it with the global redraw region which
we have been asked to draw to, and returns the intersection.
*/
const TRegion * CWsRedrawMsgWindow::ReadRegion(const TInt aRegionNum)
	{
	// We are drawing to the global region, and we have a region in the redraw store to clip to.
	// We want the intersection of these to actually draw to.
	iLocalRedrawRegion.Copy(iRedrawSegments[aRegionNum]->iRegion);
	iLocalRedrawRegion.Offset(WsWin()->Origin());
	iLocalRedrawRegion.Intersect(*iRedrawRegion);
	iLocalRedrawRegion.Tidy();

	// If the resulting region is empty there is no point drawing its corresponding commands
	if (iLocalRedrawRegion.IsEmpty())
		return NULL;
	else
		return &iLocalRedrawRegion;
	}

void CWsRedrawMsgWindow::SubtractRectFromSegmentArray(const TRect& aRect)
	{
	for (TInt regionNum = iRedrawSegments.Count() - 1; regionNum >= 0; --regionNum)
		{
		if (iRedrawSegments[regionNum]->iRedrawSegmentType != ESegmentTypePendingRedraw)
			{
			RWsRegion& region = iRedrawSegments[regionNum]->iRegion;
			region.SubRect(aRect);
			if (region.CheckError())
				{
				// Ouch. Drop the now broken segment and ask for a full redraw
				delete iRedrawSegments[regionNum];
				iRedrawSegments.Remove(regionNum);
				Invalidate();
				}
			else
				{
				// check if region has zero uncovered rectangles left
				if (region.IsEmpty())
					{ // delete draw commands, release bitmaps and fonts
					delete iRedrawSegments[regionNum];
					iRedrawSegments.Remove(regionNum);
					}
				else
					{
					if (region.Count() > KRegionCompressThreshold)
						{ // tidy up the rectangles
						region.Tidy();
						}
					}
				}
			}
		}
	// coverity[extend_simple_error]	
	}

/*------------------------------------------------------------------------------
  Description: Clears out the command buffer if aError indicates an
               error has occurred whilst storing commands.
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::DiscardStoredCommandsIfError(TInt aError)
	{
	if (aError != KErrNone)
		{
		// Discard stored commands by clearing out the command buffer
		DiscardStoredCommands();

		if(!(iFlags&ECurrentRedrawFailedStorage)) //first time we fail during the current redraw
			{
			iFlags |= ECurrentRedrawFailedStorage;

			if(!(iFlags&EPreviousRedrawFailedStorage)) //unless the previous redraw failed as well (to avoid infinite loop)
				{
				Invalidate(); //request new redraw from the client
				}
			}
		}
	}

/*------------------------------------------------------------------------------
  Description: If the graphics context has changed and we are currently storing
               commands, store the data given by aCmdData.
               
 -----------------------------------------------------------------------------*/
TBool CWsRedrawMsgWindow::DrawCommand(CWsGc* aGc,const TAny *aCmdData)
	{
	// Store commands, clearing out buffer if error occurs.
	TRAPD(err,StoreDrawCommandL(aGc,aCmdData));
	DiscardStoredCommandsIfError(err);
	return EFalse;
	}

void CWsRedrawMsgWindow::GcAttributeChange(CWsGc* aGc,const TAny *aCmdData)
	{
	if (iLastDrawGc == aGc && iCurrentSegment)
		{
		TInt err = KErrNone;
		if (IsWsFontOperation(CWsClient::iCurrentCommand.iOpcode))
			{
			TWsGcCmdUnion pData;
			pData.any=aCmdData;
			TRAP(err,AddWsFontL(*pData.UInt));
			}
		if (KErrNone == err)
			{
			TRAP(err,AppendCommandL(aCmdData));
			}
		DiscardStoredCommandsIfError(err);
		}
	
	// INC135845: 
	// Retain the bitmap handle for the lifetime of the redraw store
	// If the client destroys it, we will still have a reference to it
	if (iCurrentSegment && CWsClient::iCurrentCommand.iOpcode == EWsGcOpUseBrushPattern)
		{
		TInt err = KErrNone;
		TWsGcCmdUnion pData;
		pData.any=aCmdData;
		TRAP(err, AddFbsBitmapsL(*pData.handle, 0));
		DiscardStoredCommandsIfError(err);
		}
	}

void CWsRedrawMsgWindow::GcDeactivate(CWsGc* aGc)
	{
	if (iLastDrawGc==aGc)
		iLastDrawGc=NULL;
	}

inline TBool CWsRedrawMsgWindow::IsFbsBitmapOperation(TInt aOpCode) const
	{
	WS_ASSERT_DEBUG((EWsGcOpGdiBlt3==EWsGcOpGdiBlt2+1)&&(EWsGcOpGdiBltMasked==EWsGcOpGdiBlt3+1)
				&&(EWsGcOpDrawBitmap2==EWsGcOpDrawBitmap+1)&&(EWsGcOpDrawBitmap3==EWsGcOpDrawBitmap2+1)&&(EWsGcOpDrawBitmapMasked==EWsGcOpDrawBitmap3+1),EWsPanicBitmapOpcodeInvalid);
	return (aOpCode>=EWsGcOpGdiBlt2&&aOpCode<=EWsGcOpGdiBltMasked)||(aOpCode>=EWsGcOpDrawBitmap&&aOpCode<=EWsGcOpDrawBitmapMasked)||(aOpCode==EWsGcOpGdiAlphaBlendBitmaps);
	}

inline TBool CWsRedrawMsgWindow::IsWsBitmapOperation(TInt aOpCode) const
	{
	WS_ASSERT_DEBUG((EWsGcOpGdiBlt3==EWsGcOpGdiBlt2+1)&&(EWsGcOpGdiBltMasked==EWsGcOpGdiBlt3+1)
				&&(EWsGcOpDrawBitmap2==EWsGcOpDrawBitmap+1)&&(EWsGcOpDrawBitmap3==EWsGcOpDrawBitmap2+1)&&(EWsGcOpDrawBitmapMasked==EWsGcOpDrawBitmap3+1),EWsPanicBitmapOpcodeInvalid);
	return (aOpCode>=EWsGcOpGdiWsBlt2&&aOpCode<=EWsGcOpGdiWsBltMasked)||(aOpCode==EWsGcOpGdiWsAlphaBlendBitmaps)||(aOpCode==EWsGcOpWsDrawBitmapMasked);
	}

inline TBool CWsRedrawMsgWindow::IsWsFontOperation(TInt aOpCode) const
	{
	return aOpCode==EWsGcOpUseFont;
	}

inline TBool CWsRedrawMsgWindow::IsDrawWsGraphicOperation(TInt aOpCode) const
	{
	return (aOpCode == EWsGcOpDrawWsGraphic) || (aOpCode == EWsGcOpDrawWsGraphicPtr);
	}

inline TBool CWsRedrawMsgWindow::IsWsDrawableSourceOperation(TInt aOpCode) const
	{
	return (aOpCode == EWsGcOpDrawResourceToPos) || (aOpCode == EWsGcOpDrawResourceToRect) || (aOpCode == EWsGcOpDrawResourceFromRectToRect) || (aOpCode == EWsGcOpDrawResourceWithData);
	}

void CWsRedrawMsgWindow::ReplaceAndAppendCommandL(TInt aOpcode,const TAny* aCmdData)
	{
	const TInt KCharWidthInBytes = 2; // # of bytes for a Unicode character
	CWsClient* owner=iWsWin->WsOwner();
	WS_ASSERT_DEBUG(owner,EWsPanicDrawCommandsNullSession);

	// aCmdData doesn't contain data, it should be retrieved from client space using remote read
	TWsGcCmdUnion cmd;
	cmd.any=aCmdData;
	TUint16 newOpcode=EWsGcOpDrawText;
	TInt strLen=0;
	switch (aOpcode)
		{
		case EWsGcOpDrawTextPtr:
			newOpcode=EWsGcOpDrawText;
			strLen=cmd.DrawText->length;
			break;
		case EWsGcOpDrawTextVerticalPtr:
			newOpcode=EWsGcOpDrawTextVertical;
			strLen=cmd.DrawTextVertical->length;
			break;
		case EWsGcOpDrawBoxTextPtr:
			newOpcode=EWsGcOpDrawBoxText;
			strLen=cmd.BoxText->length;
			break;
		case EWsGcOpDrawBoxTextVerticalPtr:
			newOpcode=EWsGcOpDrawBoxTextVertical;
			strLen=cmd.DrawBoxTextVertical->length;
 			break;
		case EWsGcOpDrawTextInContextPtr:
			newOpcode=EWsGcOpDrawTextInContextPtr1;
			strLen=cmd.DrawTextInContext->length;
			break;
		case EWsGcOpDrawTextInContextVerticalPtr:
			newOpcode=EWsGcOpDrawTextInContextVerticalPtr1;
			strLen=cmd.DrawTextInContextVertical->length;
			break;
		case EWsGcOpDrawBoxTextInContextPtr:
			newOpcode=EWsGcOpDrawBoxTextInContextPtr1;
			strLen=cmd.BoxTextInContext->length;
			break;
		case EWsGcOpDrawBoxTextInContextVerticalPtr:
			newOpcode=EWsGcOpDrawBoxTextInContextVerticalPtr1;
			strLen=cmd.DrawBoxTextInContextVertical->length;
			break;
		}
	TInt strSize = strLen * KCharWidthInBytes;
	TInt oldCmdLen=CWsClient::iCurrentCommand.iCmdLength;
	TInt newCmdLen=sizeof(TWsCmdHeaderBase)+oldCmdLen+strSize;
	// resize buffer
	ExpandCommandBufferL(newCmdLen);
	// update current command to reflect the new command and data
	CWsClient::iCurrentCommand.iOpcode=newOpcode;
	CWsClient::iCurrentCommand.iOpcode|=EWsGcOpFlagDrawOp;
	CWsClient::iCurrentCommand.iCmdLength=(TInt16)(oldCmdLen+strSize);
	// write command header
	CommandBufferWrite(&CWsClient::iCurrentCommand, sizeof(TWsCmdHeaderBase));
	// write command
	CommandBufferWrite(aCmdData, oldCmdLen);

	// remote read
	TBuf<KReadBufferMaxLen> buf;
	TInt len=KReadBufferMaxLen;
	TInt bufOffset=0;
	TInt toGo=strLen;
	while(toGo>0)
		{
		if (len>toGo)
			len=toGo;
		owner->RemoteRead(buf,bufOffset);
		CommandBufferWrite(buf.Ptr(), len * KCharWidthInBytes);
		bufOffset+=len;
		toGo-=len;
		}
	}

/*------------------------------------------------------------------------------
  Description: Stores drawing related commands into the command buffer
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::StoreDrawCommandL(CWsGc* aGc,const TAny *aCmdData)
	{
	TWsGcOpcodes currentOpcode = static_cast<TWsGcOpcodes>(CWsClient::iCurrentCommand.iOpcode);
	
	if (!InRedraw())
		{
		// We've received a drawing operation outside of a BeginRedraw()/EndRedraw()
		// bracketed block.  This is known as a "non redraw drawing operation".
		
#ifdef __WINS__	
		// If configured, panic on non redraw drawing operations.	
		if( CWsClient::DebugEnforceRedrawCallingConvention())
			CWsClient::PanicCurrentClient(EWservPanicWindowBeginRedrawNotCalled);		
#endif
		
		// We get here if we are not configured to panic when a non-redraw drawing operation
		// is issued. 
		// We ignore this DrawOp as WServ2 is not supporting non-redraw drawing because 
		// storing such non-redraw commands in the redraw store consumes significant 
		// amounts of memory, and reduces rendering performance.
		// 
		// Issuing non-redraw drawing from a client Redrawer is a special case.  Here
		// an infinite loop would occur if we mark the area as invalid.  Therefore
		// we must always panic in this case.
		if (iWsWin->WsOwner()->ClientProcessingRedrawEvent())
			{
			// Non-redraw drawing in the Redrawer!!
			CWsClient::PanicCurrentClient(EWservPanicWindowBeginRedrawNotCalled);
			}
		else
			{
			// The Redrawer will eventually fix up the screen
			Invalidate();
			}
		return;
		}
	
	// If there is no current segment then we have discarded it at some point
	// since beginning this redraw.  
	if (iCurrentSegment)
		{
		TWsGcCmdUnion pData;
		pData.any = aCmdData;
		if (IsFbsBitmapOperation(currentOpcode))
			{
			TInt maskHandle = 0;
			TInt handle = aGc->FbsBitmapHandle(currentOpcode, pData, maskHandle);
			AddFbsBitmapsL(handle, maskHandle);
			}
		else if (IsWsBitmapOperation(currentOpcode))
			{
			TInt maskHandle = 0;
			TInt handle = aGc->WsBitmapHandle(currentOpcode, pData, maskHandle);
			AddWsBitmapsL(handle, maskHandle);
			}
		else if (IsDrawWsGraphicOperation(currentOpcode))
			{
			TGraphicDrawerId drawerId;
			drawerId.iId = pData.WsGraphic->iId;
			drawerId.iIsUid = (pData.WsGraphic->iFlags & EWsGraphicIdUid);
			iCurrentSegment->AddDrawerL(drawerId);
			}
		else if (IsWsDrawableSourceOperation(currentOpcode))
			{
			TInt handle = aGc->WsDrawableSourceHandle(currentOpcode, pData);
			AddWsDrawableSourceL(handle);
			}

		// If the graphics context has changed since last time store the new graphics
		// context attributes.
		if (aGc != iLastDrawGc)
			{
			StoreAllGcAttributesL(aGc);
			iLastDrawGc = aGc;
			}

		// For operation which requires remote read from client space, we must retrieve that data and store
		// it in command buffer at server side and change opcode if necessary e.g EWsGcOpDrawTextPtr to EWsGcOpDrawText
		// to avoid remote read during DoDrawing operation
		if (IsRemoteReadRequired(currentOpcode))
			ReplaceAndAppendCommandL(currentOpcode,aCmdData);
		else
			// Append the command data to the command buffer
			AppendCommandL(aCmdData, EWsGcOpFlagDrawOp);
		}
	}

/*------------------------------------------------------------------------------
  Description: Stores given drawing command data into the command buffer.
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::AppendCommandL(const TAny* aCmdData, const TUint16 aOpcodeFlags)
	{
	if (CWsClient::iCurrentCommand.iOpcode == EWsGcOpSetClippingRegion)
		{
		// The client is defining a clipping region

		// make room for the header
		ExpandCommandBufferL(sizeof(TWsCmdHeaderBase));

		// Externalize the clipping region data from position after the header
		RBufWriteStream bufWriteStream;
		bufWriteStream.Open(*CurrentDrawCommandBuffer(), CurrentCommandBufferWritePos() + sizeof(TWsCmdHeaderBase));
		CleanupClosePushL(bufWriteStream);
		TInt dataLen = iLastDrawGc->ExternalizeClippingRegionL(bufWriteStream);

		// Setup the clipping region data header
		CWsClient::iCurrentCommand.iOpcode = EWsStoreClippingRegion;
		CWsClient::iCurrentCommand.iCmdLength = REINTERPRET_CAST(TInt16&,dataLen);

		// Store command header for clipping region data at current write position
		CommandBufferWrite(&CWsClient::iCurrentCommand,sizeof(TWsCmdHeaderBase));

		// Update write position for command data
		iCurrentSegment->iCurrentCommandBufferWritePos += dataLen;
		CleanupStack::PopAndDestroy(&bufWriteStream);
		}
	else
		{		
		TUint16 opcode = CWsClient::iCurrentCommand.iOpcode;
		CWsClient::iCurrentCommand.iOpcode |= aOpcodeFlags;

		// ensure room in command buffer
		ExpandCommandBufferL(sizeof(TWsCmdHeaderBase) + CWsClient::iCurrentCommand.iCmdLength);

		// Store command header to current position
		CommandBufferWrite(&CWsClient::iCurrentCommand, sizeof(TWsCmdHeaderBase));

		// If there's command data (other than header), store it
		if (CWsClient::iCurrentCommand.iCmdLength > 0)
			{
			CommandBufferWrite(aCmdData, CWsClient::iCurrentCommand.iCmdLength);
			}

		CWsClient::iCurrentCommand.iOpcode = opcode;
		}
	}


/*------------------------------------------------------------------------------
  Description: Stores graphics context information into the command buffer
               from the current write position.
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::StoreAllGcAttributesL(CWsGc* aGc)
	{
	// In order for the externalize below to work correctly from
	// a non-zero position we have to create the header placeholder
	ExpandCommandBufferL(sizeof(TWsCmdHeaderBase));

	// Externalise GC attribute data. We do this before writing the
	// header as we do not know the size of the data yet and it is
	// part of the header.
	TInt numOfBytesAdded = aGc->ExternalizeL(*CurrentDrawCommandBuffer(),
				CurrentCommandBufferWritePos() + sizeof(TWsCmdHeaderBase));

	// Setup the header
	TWsCmdHeaderBase cmdHeader;
	cmdHeader.iCmdLength = (TInt16) numOfBytesAdded;		// as calculated above
	cmdHeader.iOpcode = (TInt16) EWsStoreAllGcAttributes;

	// Store the header for the GC data into the space we created
	CommandBufferWrite(&cmdHeader, sizeof(TWsCmdHeaderBase));

	// Update write position for command data
	iCurrentSegment->iCurrentCommandBufferWritePos += numOfBytesAdded;
	}

LOCAL_C void CallSegmentEnd(TAny* aAnnotationObserver)
	{
	static_cast<MWsDrawAnnotationObserver*>(aAnnotationObserver)->SegmentRedrawEnd();
	}

/*------------------------------------------------------------------------------
  Description: Loops through the whole of the current command buffer, processing
               each in turn.
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::DrawCommandsL()
	{
	LOG_WINDOW_DRAW_COMMANDS_START(WsWin(), iRedrawRegion);
	WS_ASSERT_DEBUG(iMemoryLock > 0, EWsPanicMemoryLock);
	static TBuf8<EClientBufferMaxSize> buf;
	TInt regionCount = iRedrawSegments.Count();
	
	for (TInt regionNum = 0; regionNum < regionCount; ++regionNum)
		{
		CRedrawSegment* segment = iRedrawSegments[regionNum];
		LOG_REDRAW_SEGMENT(regionNum, segment->iRedrawSegmentType);
		if (segment->iRedrawSegmentType == ESegmentTypePendingRedraw)
			continue;

		// The amount of commands we process is given by the value of the
		// current write position rather than the size of the command buffer.
		// Note: the write position is incremented as each command is stored and
		// will typically be less than the buffer size.
		const TInt length = segment->iCurrentCommandBufferWritePos;

		// need to draw this region?
		const TRegion * localDrawRegion = 0;
		if (length)
		 	localDrawRegion = ReadRegion(regionNum);
		if (localDrawRegion)
			{
			MWsDrawAnnotationObserver* const annoObs = Screen()->DrawAnnotationObserver();
			if(annoObs)
				{
				annoObs->SegmentRedrawStart(*localDrawRegion);
				CleanupStack::PushL(TCleanupItem(CallSegmentEnd, annoObs));
				}
			CPlaybackGc::Instance()->SetTargetRegion(localDrawRegion);
			LOG_REDRAW_SEGMENT_REGION(localDrawRegion)
			
			TWsCmdHeaderBase header;
			TInt pos = 0; // Set to first command position in buffer
			CBufSeg* drawCmdBuffer = segment->iDrawCommands;

#ifdef _DEBUG
			// Read the first command header. The associated opcode must always be
			// EWsStoreAllGcAttributes as this is always the first stored item.
			drawCmdBuffer->Read(pos,&header,sizeof(TWsCmdHeaderBase));
			WS_ASSERT_DEBUG(header.iOpcode == EWsStoreAllGcAttributes, EWsPanicDrawCommandsBufferCorrupt);
#endif

			// Read through remaining commands
			while (pos < length)
				{
				// Get header of command
				drawCmdBuffer->Read(pos, &header, sizeof(TWsCmdHeaderBase));
				pos += sizeof(TWsCmdHeaderBase);

				switch(header.iOpcode)
					{
					case EWsStoreAllGcAttributes:
						{
						// Header indicates command encapsulates gc data
						CPlaybackGc::Instance()->Reset();

						// Read gc data
						CPlaybackGc::Instance()->InternalizeL(*drawCmdBuffer,pos);

						}
						break;
					case EWsStoreClippingRegion:
						{
						// Clipping region data read in from current position via stream
						RBufReadStream bufReadStream;
						bufReadStream.Open(*drawCmdBuffer,pos);
						CleanupClosePushL(bufReadStream);
						CPlaybackGc::Instance()->InternalizeClippingRegionL(bufReadStream);
						CleanupStack::PopAndDestroy(&bufReadStream);
						}
						break;
					default:
						{
						// Another type of command. Read it.
						CWsClient::iCurrentCommand.iCmdLength = header.iCmdLength;
						drawCmdBuffer->Read(pos,buf,header.iCmdLength);

						TInt opcode = header.iOpcode;

						// Drawing command?
						if (opcode & EWsGcOpFlagDrawOp)
							{
							opcode &= ~EWsGcOpFlagDrawOp;
							}
						if (opcode > -1)
							{
							LOG_PLAYBACK_GC_COMMAND(opcode, buf.Ptr())
							CPlaybackGc::Instance()->CommandL(static_cast<TWsGcOpcodes>(opcode),buf);
							}
						}
						break;
					}
				pos += header.iCmdLength; // Move on, header indicates length
				}
			if (annoObs)
				CleanupStack::PopAndDestroy(annoObs);
			}
		}
	LOG_WINDOW_DRAW_COMMANDS_END(WsWin());
	}

/*------------------------------------------------------------------------------
  Description: Called when the currently stored graphics commands
               are no longer required.
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::DiscardStoredCommands()
	{
	iCurrentSegment = NULL;
	if (iRedrawSegments.Count() > 0)
		{
		// First of all, if we have any redraws pending, update the screen with
		// whatever commands we have before we throw them away:
		if (iFlags & EPendingScheduledDraw)
			Screen()->DoRedrawNow();

		// for all regions or just Partial Redraw regions > index 0: delete bitmaps and draw commands
		iRedrawSegments.ResetAndDestroy();

		iLastDrawGc = NULL;
		}
	}

void CWsRedrawMsgWindow::CreateNewSegmentL(const TRect& aRect, TRedrawSegmentType aNewRedrawRegionType)
	{
	CWsRedrawMsgWindow::CRedrawSegment* newRegion = CWsRedrawMsgWindow::CRedrawSegment::NewLC(aRect, aNewRedrawRegionType, *this);

	iRedrawSegments.AppendL(newRegion);
	iCurrentSegment = newRegion;
	CleanupStack::Pop(newRegion);

	// Set iLastDrawGc to NULL. This will cause all GC attributes to be stored
	// in redraw store when the window receives the next command
	iLastDrawGc = NULL;
	}

void CWsRedrawMsgWindow::AddFbsBitmapsL(TInt aHandle, TInt aMaskHandle)
	{
	AddFbsBitmapRefL(aHandle);
	if (aMaskHandle)
		{
		AddFbsBitmapRefL(aMaskHandle);
		}
	}

void CWsRedrawMsgWindow::AddFbsBitmapRefL(TInt aHandle)
	{
	TInt indexBitmapRef = iFbsBitmapRefArray.FindInOrder(aHandle, &FindBitmapRefByHandle);
	
	// check whether the window already has a reference for the bitmap through a segment other than the current one
	if (indexBitmapRef >=0)
	    {
	    TBool handleAppend = iCurrentSegment->AppendNonDuplicateBitmapHandleL(aHandle);
	    if (handleAppend)
	        {
	        iFbsBitmapRefArray[indexBitmapRef]->IncRefCount();
	        }
        }
    else
        {
        CFbsBitmapRef* bitmapRef = new(ELeave) CFbsBitmapRef;
        CleanupStack::PushL(bitmapRef);
        if (bitmapRef->Duplicate(aHandle)!=KErrNone)
            OwnerPanic(EWservPanicBitmap);
        iFbsBitmapRefArray.InsertInOrderL(bitmapRef, TLinearOrder<CFbsBitmapRef>(InsertBitmapRefByHandle));
        CleanupStack::Pop(bitmapRef);
        bitmapRef->IncRefCount();
#ifdef _DEBUG
        TBool bitmapHandleAppended = EFalse;
        TRAPD(err, bitmapHandleAppended = iCurrentSegment->AppendNonDuplicateBitmapHandleL(aHandle);
                   WS_ASSERT_DEBUG(bitmapHandleAppended,EWsPanicUnexpectedBitmapHandleInArray););
#else
        TRAPD(err, iCurrentSegment->AppendNonDuplicateBitmapHandleL(aHandle););
#endif
        if (err != KErrNone)
            {
            // Cleanup the array, then propagate the error
            indexBitmapRef = iFbsBitmapRefArray.FindInOrder(aHandle, &FindBitmapRefByHandle);
            if (indexBitmapRef >= 0)
                {
                iFbsBitmapRefArray[indexBitmapRef]->DecRefCount();
                delete iFbsBitmapRefArray[indexBitmapRef];
                iFbsBitmapRefArray.Remove(indexBitmapRef);
                }
            User::Leave(err);
            }
        }
	}

// Only called during playback of a redraw store segment
CFbsBitmap* CWsRedrawMsgWindow::BitmapFromHandle(TInt aHandle) const
    {
    const TInt index = iFbsBitmapRefArray.FindInOrder(aHandle, &FindBitmapRefByHandle);
    if (index != KErrNotFound)
        {
        CFbsBitmap* bitmap = iFbsBitmapRefArray[index];
        return bitmap;
        }
    return NULL;
    }


void CWsRedrawMsgWindow::AddWsBitmapsL(TInt aHandle, TInt aMaskHandle)
	{
	if (iWsWin->WsOwner() == NULL)
		Panic(EWsPanicDrawCommandsInvalidState);
	DWsBitmap * bmp = static_cast<DWsBitmap*>(iWsWin->WsOwner()->HandleToObj(aHandle, WS_HANDLE_BITMAP));
	if (!bmp)
		OwnerPanic(EWservPanicBitmap);
	iCurrentSegment->AddWsBitmapL(bmp);
	if (aMaskHandle)
		{
		bmp = static_cast<DWsBitmap*>(iWsWin->WsOwner()->HandleToObj(aMaskHandle, WS_HANDLE_BITMAP));
		if (!bmp)
			OwnerPanic(EWservPanicBitmap);
		iCurrentSegment->AddWsBitmapL(bmp);
		}
	}

void CWsRedrawMsgWindow::CRedrawSegment::AddWsBitmapL(DWsBitmap* bitmap)
	{
	iWsBitmapArray.AppendL(bitmap);
	bitmap->IncRefCount();
	}

void CWsRedrawMsgWindow::AddWsFontL(TInt aHandle)
	{
	if (iWsWin->WsOwner()==NULL)
		Panic(EWsPanicDrawCommandsInvalidState);
	TDblQueIter<CWsFbsFont> iter(CWsFontCache::List());
	CWsFbsFont* font=NULL;
	while((font=iter++)!=NULL)
		{
		if (font->Handle()==aHandle)
			break;
		}
	if (font)
		{
		iCurrentSegment->iWsFontArray.AppendL(font);
		++(font->iCount);
		}
	}

void CWsRedrawMsgWindow::CRedrawSegment::AddDrawerL(TGraphicDrawerId aDrawerId)
	{
	TInt error = iDrawerArray.InsertInOrder(aDrawerId, TLinearOrder<TGraphicDrawerId>(TGraphicDrawerId::Compare));
	if (error != KErrAlreadyExists && error != KErrNone)
		{
		User::Leave(error);
		}
	}

TBool CWsRedrawMsgWindow::CRedrawSegment::ContainsDrawers(const TArray<TGraphicDrawerId>& aDrawers,const TRegion& aRegion) const
	{
	TBool result = EFalse;
	if (iDrawerArray.Count() > 0)
		{
		STACK_REGION tempRegion;
		tempRegion.Intersection(iRegion, aRegion);
		if (tempRegion.CheckError() || (tempRegion.Count() > 0) )
			{ // regions do intersect, (presumed if region had an error); so check for a matching Id
			const TInt drawersCount = aDrawers.Count();
			for (TInt idx = 0; idx < drawersCount; ++idx)
				{ // (iDrawerArray is kept sorted)
				if (KErrNotFound != iDrawerArray.FindInOrder(aDrawers[idx], TLinearOrder<TGraphicDrawerId>(TGraphicDrawerId::Compare)))
					{
					result = ETrue;
					break;
					}
 					
				const TInt count = iDrawerArray.Count();
				for(TInt i = 0; i < count; i++)
					{
					const CWsGraphicDrawer* drawer = CWsTop::WindowServer()->ResolveGraphic(iDrawerArray[i]);
					if(drawer && drawer->Contains(aDrawers))
						{
						result = ETrue;
						break;
						}
					}
				}
			}
		tempRegion.Close();
		}
	return result;
	}

void CWsRedrawMsgWindow::AddWsDrawableSourceL(TInt aHandle)
	{
	if (iWsWin->WsOwner() == NULL)
		Panic(EWsPanicDrawCommandsInvalidState);
	CWsDrawableSource* drawableSource = static_cast<CWsDrawableSource*>(iWsWin->WsOwner()->HandleToObj(aHandle, WS_HANDLE_DRAWABLE_SOURCE));
	if (!drawableSource)
		OwnerPanic(EWservPanicDrawableSource);
	iCurrentSegment->AddWsDrawableSourceL(drawableSource);
	}

void CWsRedrawMsgWindow::CRedrawSegment::AddWsDrawableSourceL(CWsDrawableSource* aDrawableSource)
	{
	iWsDrawableSourceArray.AppendL(aDrawableSource);
	aDrawableSource->IncRefCount();
	}

inline TBool CWsRedrawMsgWindow::NoBuffer() const
	{
	return (iRedrawSegments.Count() == 0);
	}

void CWsRedrawMsgWindow::ClientExposing()
	{
	Invalidate();
	}

/*------------------------------------------------------------------------------
  Description: If a complete set of drawing commands have been stored
               this method attempts to draw ALL the commands via DrawCommandsL().
               It also draws the window in the background colour if the window is
               opaque.
 -----------------------------------------------------------------------------*/
void CWsRedrawMsgWindow::DrawWindow()
	{
	iFlags &= ~EPendingScheduledDraw;
	// This is a happy window - it can draw itself whenever we ask.
	CScreen* screen = Screen();
	if ((screen->AutoClear() && (iFlags & EBackgroundClear)) || HasElement())
		{
		DrawBackgroundColor(*iRedrawRegion,(iFlags&EBackgroundClear)!=0);
		}
	if (HasElement())
		{
		screen->WindowElements().UnassignPlacedElements(*iRedrawRegion,*CliWin(),CPlaybackGc::Instance()->GcDrawingCount());
		}
	// If valid commands have been stored, draw them.
	if (iRedrawSegments.Count() > 0)
		{
		Lock();
		TRAP_IGNORE(DrawCommandsL());
		Unlock();
		}
	if (HasElement())	//HasElement won't be cleared following first call above. May get set...
		{
		TInt state=screen->WindowElements().CleanUpPlacedElements(*CliWin(),CPlaybackGc::Instance()->GcDrawingCount());
		if (state&CWindowElement::EFastPath)
			{
            if (HasElement())
                {
                screen->ElementAdded();
                }
            else
                {
                screen->ElementRemoved();
                }
			}
		}
	}

void CWsRedrawMsgWindow::RemoveFromRedrawQueueIfEmpty()
	{
	if (iInvalid.Count()==0)
		{
		iInvalid.Clear();	// Ensures heap cell is freed, otherwise may be left as an empty cell
		iWsWin->WsOwner()->RedrawQueue()->RemoveInvalid(this);
		}
	}

const TRegion& CWsRedrawMsgWindow::InvalidArea() const
	{
	return(iInvalid);
	}

TBool CWsRedrawMsgWindow::NeedsRedraw() const
// If iInvalid has an persistant error it will not be reported as needing a redraw,
// this is needed as otherwise cases where validation of a window results
// in iInvalid having an error will get into an endless cycle of redraws.
// The down side of this is that sometimes a window will not be sent a redraw
// message when it needs it, some things can't be perfect!
//
	{
	if ((!iWsWin->IsVisible()) || iInvalid.IsEmpty())
		return EFalse;
	
	TRect nextRedrawRect;
	return GetRedrawRect(nextRedrawRect);
	}

TBool CWsRedrawMsgWindow::GetRedrawRect(TRect &aRect) const
	{
	if (iWsWin->ClientSetInvisible())
		return EFalse;
	
	if(InRedraw())
		{
		aRect = iRedrawRect;
		return (!aRect.IsEmpty());
		}
	else if(iInvalid.CheckError())
		{
		if (Screen()->ChangeTracking())
			{
			aRect = iWsWin->AbsRect();
			}
		else
			{
			if (iFlags & EStoringEntireWindow || iWsWin->VisibleRegion().CheckError())
				{
				aRect = iWsWin->AbsRect();
				}
			else
				{
				aRect = iWsWin->VisibleRegion().BoundingRect();
				}
			}
		if (!(iFlags & EStoringEntireWindow))
			iWsWin->ClipRectToViewport(aRect);
		aRect.Move(-iWsWin->Origin());
		return (!aRect.IsEmpty());
		}
	else if(iInvalid.Count())
		{
		if (iFlags & EStoringEntireWindow)
			{
			aRect = iInvalid.BoundingRect();
			}
		else
			{
			RWsRegion region;
			region.Copy(iInvalid);
			region.Offset(iWsWin->Origin());
			if (!Screen()->ChangeTracking())
				{
				region.Intersect(iWsWin->VisibleRegion());
				}
			if (region.CheckError())
				{
				aRect = iInvalid.BoundingRect();
				aRect.Move(iWsWin->Origin());
				}
			else
				{
				aRect = region.BoundingRect();
				}
			region.Close();
			iWsWin->ClipRectToViewport(aRect);
			aRect.Move(-iWsWin->Origin());
			}
		return (!aRect.IsEmpty());
		}
	else
		{
		return EFalse;
		}
	}

const TRegion &CWsRedrawMsgWindow::BaseDrawRegion() const
	{
	return(iWsWin->VisibleRegion());
	}

void CWsRedrawMsgWindow::ClipInvalidRegion(const TRect &aRect)
	{
	if (iInvalid.Count()>0)
		{
		iInvalid.ClipRect(aRect);
		RemoveFromRedrawQueueIfEmpty();
		}
	}

void CWsRedrawMsgWindow::EndRedraw()
	{
	if(!InRedraw())
		OwnerPanic(EWservPanicDrawCommandsInvalidState);
	if (iCurrentSegment)
		{
		iCurrentSegment->iDrawCommands->Compress();
		if (iAtomic)
			PromotePendingSegment();

		// Schedule an update of the area of the screen we just drew to:
		iFlags |= EPendingScheduledDraw;
		if (Screen()->ChangeTracking())
			{
			iWsWin->AddDirtyWindowRegion(iCurrentSegment->iRegion); // stored in window coordinates
			if (iWsWin->IsVisible())
				{
				// Window is visible, (we're ignoring whether it's obscured or not)
				// we need to send the new draw commands to the render stage.
				Screen()->ScheduleWindow(iWsWin);
				}
			}
		if (!iWsWin->HasBeenDrawnToScreen())
			{
			CliWin()->ScheduleRegionUpdate(NULL);
			}
		else if(!Screen()->ChangeTracking() && (iWsWin->VisibleRegion().Count() || iWsWin->VisibleRegion().CheckError())) // on screen? good.
			{
			STACK_REGION draw; //### in low memory where VisibleRegion() is intact we can degrade much better than this!
			draw.Copy(iCurrentSegment->iRegion);
			draw.Offset(iWsWin->Origin());
			draw.Intersect(iWsWin->VisibleRegion());
			if(!draw.CheckError())
				Screen()->AddRedrawRegion(draw);
			else
				Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
			draw.Close();
			}
		}

	iCurrentSegment = NULL;
	
	//store the result of the current redraw 
	if(iFlags&ECurrentRedrawFailedStorage)
		iFlags |= EPreviousRedrawFailedStorage; //set
	else
		iFlags &= ~EPreviousRedrawFailedStorage; //unset
	iFlags &= ~ECurrentRedrawFailedStorage; //unset the flag for the next redraw
	//

	iFlags&=~EBeginEndRedraw;
	}

void CWsRedrawMsgWindow::ValidateRect(const TRect *aRect)
	{
	if (!WsWin()->BaseParent())
		OwnerPanic(EWservPanicParentDeleted);
	if (aRect)
		iRedrawRect = *aRect;
	if (!iInvalid.IsEmpty())
		{
		STACK_REGION validated;
		validated.Copy(iInvalid);
		if (aRect)
			validated.ClipRect(iRedrawRect);
		
		if (iInvalid.CheckError())
			{
			iInvalid.Copy(iWsWin->VisibleRegion());
			iInvalid.Offset(-iWsWin->Origin());
			}
		iInvalid.SubRegion(validated);
		validated.Close();		
		}
	RemoveFromRedrawQueueIfEmpty();
	}

TRgb CWsRedrawMsgWindow::BackColor() const
	{
	return(iBackColor);
	}

/**
This function used to be quite clever about what it invalidated and what it redrew by copying
rectangles of the screen around.  This is a lot less subtle, and makes calling Scroll pretty much
pointless, but it IS functionally correct.
*/
void CWsRedrawMsgWindow::Scroll(const TRect &aClipRect, const TPoint &aOffset,const TRect &aRect)
	{
	TRect rect = aRect;
	rect.Intersection(aClipRect);	
	Invalidate(&rect);
	rect = aRect;
	rect.Move(aOffset);
	rect.Intersection(aClipRect);
	Invalidate(&rect);
	}

void CWsRedrawMsgWindow::ClearRedrawStore(TBool aClearPendingRedraw)
	{
	if(aClearPendingRedraw && (iFlags & EPendingScheduledDraw))
		iFlags &= ~EPendingScheduledDraw;

	DiscardStoredCommands();
	Invalidate();
	}


void CWsRedrawMsgWindow::PrepareForResizeL(const TSize& aSize, TSize& /*aOldSize*/)
	{
	TBool anyIncreases(EFalse);
	if (aSize.iWidth>iWsWin->Size().iWidth||aSize.iHeight>iWsWin->Size().iHeight)
		{
		anyIncreases = ETrue;
		}

	TRect newWinRect(TPoint(0,0),aSize);
	iInvalid.ClipRect(newWinRect);
	if (anyIncreases)
		{
		// add new invalid region to iInvalid
		iInvalid.AddRect(newWinRect);
		QueueRedraw();
		iWsWin->WsOwner()->TriggerRedraw();
		}
	}

void CWsRedrawMsgWindow::Moved()
	{
	if (!(iFlags & EStoringEntireWindow))
		{
		DiscardSegmentsOutsideViewport();
		}
	if (iInvalid.Count())
		{
		QueueRedraw();
		iWsWin->WsOwner()->TriggerRedraw();
		}
	}

TBool CWsRedrawMsgWindow::Contains(const TArray<TGraphicDrawerId>& aDrawers,const TRegion& aRegion) const
	{
	if (iRedrawSegments.Count() > 0)
		{
		// scan redraw store: calls Contains() on every region drawing commands are stored for,
		// looking for a DrawWsGraphic command that intersects the aRegion
		TBool contains = EFalse;
		const TInt regionCount = iRedrawSegments.Count();
		
		// Apply an origin correction. The input aRegion is screen-absolute, while the redraw regions are window-relative.
		STACK_REGION relRegion;
		relRegion.Copy(aRegion);
		relRegion.Offset(-(CliWin()->Origin().iX), -(CliWin()->Origin().iY));

		// loop through regions, stops when a match is found
		for (TInt regionNum = 0; (regionNum < regionCount) && !contains; ++regionNum)
			{
			contains = iRedrawSegments[regionNum]->ContainsDrawers(aDrawers, relRegion);
			}
		relRegion.Close();
		return contains;
		}
	else
		{
		return CWsWindowRedraw::Contains(aDrawers,aRegion);
		}
	}

TBool CWsRedrawMsgWindow::RedrawingInProgress() const
	{
	return (iFlags & EBeginEndRedraw);	
	}

void CWsRedrawMsgWindow::WindowClosing()
	{
	iWsWin->WsOwner()->RedrawQueue()->RemoveInvalid(this);
	CWsWindowRedraw::WindowClosing();
	}

TBool CWsRedrawMsgWindow::IsRedrawStoreEmpty() const
	{
	return (iRedrawSegments.Count() <= 0); // Begin and End redraw are in the store too.
	}

TBool CWsRedrawMsgWindow::IsBackgroundClearEnabled() const
	{
	return ((iFlags & EBackgroundClear) != 0);
	}

void CWsRedrawMsgWindow::CleanupBitmapRefArray(const RArray<TInt>& aHandleArray)
    {
    TInt count = aHandleArray.Count();
    for(TInt ii = count - 1 ; ii >= 0; ii--)
        {
        const TInt indexBitmapRef = iFbsBitmapRefArray.FindInOrder(aHandleArray[ii], &FindBitmapRefByHandle);
        WS_ASSERT_DEBUG(indexBitmapRef >= 0,EWsPanicUnexpectedBitmapHandleInArray);
        if (indexBitmapRef>=0)
            {
            iFbsBitmapRefArray[indexBitmapRef]->DecRefCount();
            if (iFbsBitmapRefArray[indexBitmapRef]->RefCount()==0)
                {
                delete iFbsBitmapRefArray[indexBitmapRef];
                iFbsBitmapRefArray.Remove(indexBitmapRef);
                }
            }
        }
    }

void CWsRedrawMsgWindow::SetScope(TScope aScope)
	{
	if (aScope == EStoreEntireWindow)
		{
		if (!(iFlags & EStoringEntireWindow))
			{
			iFlags |= EStoringEntireWindow;
			Invalidate();
			}
		}
	else
		{
		if (iFlags & EStoringEntireWindow)
			{
			iFlags &= ~ EStoringEntireWindow;
			DiscardSegmentsOutsideViewport();
			}
		}
	}

/**
Removes all segments from the redraw store which are outside the viewport onto the window.
Note that this doesn't clip the regions of those segments which are partly outside, since
this wouldn't actually achieve anything useful.

This function allocates memory so it is not suitable to run as part of ReleaseMemory.
*/
TBool CWsRedrawMsgWindow::DiscardSegmentsOutsideViewport()
	{
	TBool discarded = EFalse;
	TInt count = iRedrawSegments.Count();
	STACK_REGION viewport;
	CliWin()->GetClippedBaseArea(viewport);
	viewport.Offset(-iWsWin->Origin());
	STACK_REGION intersect;
	for (TInt idx = count - 1; idx >= 0; --idx)
		{
		CRedrawSegment * segment = iRedrawSegments[idx];
		intersect.Intersection(segment->iRegion, viewport);
		if (!intersect.CheckError() && intersect.IsEmpty())
			{
			iInvalid.Union(segment->iRegion);
			delete segment;
			iRedrawSegments.Remove(idx);
			if (iCurrentSegment == segment)
				iCurrentSegment = NULL;
			discarded = ETrue;
			}
		}
	intersect.Close();
	viewport.Close();
	return discarded;
	}

/**
Statements encapsulated in between Lock() and Unlock() is guaranteed to execute in an 
atomic way without being interupted by a call to ReleaseMemory from CWsMemoryManager. 
Locking will prevent memory belonging to this object to be freed during a 
memory alloc/realloc originating from self.
*/
void CWsRedrawMsgWindow::Lock()
	{
	++iMemoryLock;
	}
	
void CWsRedrawMsgWindow::Unlock()
	{
	--iMemoryLock;
	WS_ASSERT_DEBUG(iMemoryLock >= 0, EWsPanicMemoryLock);
	}

TBool CWsRedrawMsgWindow::ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel)
	{
	//When this function is called, wserv is in the middle of executing something.
	//Therefore we can not safely do anything that alters the state of any shared 
	//resouces (like e.g. CScreenRedraw::iInvalid).
	//In addition, we should refrain from anything that might try to allocate memory.
	TBool released = EFalse;
	//Don't release iRedrawSegments from this win if its currently being rendered, 
	//is releasing memory or is receiving drawcommands.	
	if (iMemoryLock == 0 && !iCurrentSegment)
		{
		Lock();
		switch (aLevel)
			{
			case MWsMemoryRelease::ELow:
				break;
			case MWsMemoryRelease::EMedium:
				break;
			case MWsMemoryRelease::EHigh:
				//Only release memory from background windows.
				if (iRedrawSegments.Count() > 0 && iWsWin->VisibleRegion().IsEmpty())
					{
					ReleaseRedrawSegments();
					released = ETrue;
					}
				break;
			}
		Unlock();
		}
	return released;
	}

void CWsRedrawMsgWindow::ReleaseRedrawSegments()
	{
 	iLastDrawGc = NULL;
 	iCurrentSegment = NULL;
 	iRedrawSegments.ResetAndDestroy();
 	
 	//The call to ResetAndDestroy just freed some memory so it should be 
 	//possible to call Invalidate() now.
 	Invalidate(); 
 	
 	//Releasing the same window over and over again could quickly end up in 
 	//a never ending loop with a high-prio client before we find the window
 	//that has nicked all memory. So call accessed now to prevent that.
  	iWsWin->Accessed(); 
   	}

void CWsRedrawMsgWindow::VisibleRegionChange()
	{
	if (!iFlags & EStoringEntireWindow)
		{
		DiscardSegmentsOutsideViewport();
		}
	if ((!iInvalid.IsEmpty()) && (!iWsWin->VisibleRegion().IsEmpty()))
		{
		STACK_REGION exposed;
		exposed.Copy(iInvalid);
		exposed.Offset(iWsWin->Origin());
		exposed.Intersect(iWsWin->VisibleRegion());
		if (!exposed.IsEmpty())
			{
			QueueRedraw();
			}
		exposed.Close();
		}
	}

TBool CWsRedrawMsgWindow::ReadyToDraw() const
	{
	//We are only ready to draw when we have a complete segment.
	if (iWsWin->HasBeenDrawnToScreen())
		return ETrue;
	
	if (iRedrawSegments.Count() == 0)
		return EFalse;
	
	if (iRedrawSegments.Count() > 1)
		return ETrue;
	
	if (iRedrawSegments[0]->iRedrawSegmentType == ESegmentTypePendingRedraw)
		return EFalse;
	
	return ETrue;
	}

TInt CWsRedrawMsgWindow::SizeInBytes() const
	{
	TInt size = sizeof(CWsRedrawMsgWindow);
	for(TInt i = iRedrawSegments.Count()-1; i >= 0; i--)
		{
		size += iRedrawSegments[i]->SizeInBytes();
		}
	size += iInvalid.Count() * sizeof(TRect);
	size += iLocalRedrawRegion.Count() * sizeof(TRect);
	return size;
	}

void CWsRedrawMsgWindow::PromotePendingSegment()
	{
	if (iRedrawSegments.Count() > 0 && iRedrawSegments[iRedrawSegments.Count() - 1]->iRedrawSegmentType == ESegmentTypePendingRedraw)
		{
		CRedrawSegment * segment = iRedrawSegments[iRedrawSegments.Count() - 1];
		const TRect * rect = segment->iRegion.RectangleList();
		// when we get here there should only ever be one rectangle in the region, but we are playing safe
		for (TInt r = 0; r < segment->iRegion.Count(); ++r)
			{
			SubtractRectFromSegmentArray(*rect);			
			++rect;
			}
		segment->iRedrawSegmentType = ESegmentTypeRedraw;
		}
	}

CFbsBitmapRef::CFbsBitmapRef()
    {
    }

CFbsBitmapRef::~CFbsBitmapRef()
    {
    WS_ASSERT_DEBUG(iRefCount==0,EWsPanicCounterValue);
    }
