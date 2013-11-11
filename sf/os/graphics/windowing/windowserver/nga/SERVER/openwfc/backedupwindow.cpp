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
// This class deals with drawing from backup bitmap
// 
//

#include "backedupwindow.h"
#include "server.h"
#include "gc.h"
#include "wstop.h"
#include "ANIM.H"
#include "EVQUEUE.H"
#include <s32mem.h>
#include <gdi.h>
#include "panics.h"
#include "inifile.h"
#include "rootwin.h"
#include "EVENT.H"
#include "playbackgc.h"
#include "devicemap.h"

CFbsBitGc *CWsBackedUpWindow::iBitGc=NULL;

CWsBackedUpWindow::CWsBackedUpWindow(CWsWindow *aWin, TDisplayMode aDisplayMode) 
	: CWsWindowRedraw(aWin), iDisplayMode(aDisplayMode)
	{}

void CWsBackedUpWindow::StaticInitL()
	{
	iBitGc=CFbsBitGc::NewL();
	}

void CWsBackedUpWindow::StaticDestroy()
	{
	delete iBitGc;
	iBitGc = 0;
	}

void CWsBackedUpWindow::ActivateGc()
	{
	iBitGc->Activate(iBitmapDevice);
	iBitGc->Reset();
	iBitGc->SetBrushColor(BackColor());
	}

TBool CWsBackedUpWindow::DrawCommand(CWsGc*,const TAny*)
	{
	if (Screen()->ChangeTracking())
		MarkDirtyAndSchedule(iCurrentRegion);
	else
		Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
	
	return ETrue;
	}

void CWsBackedUpWindow::ConstructL()
	{
	iDisplayMode=iWsWin->SetRequiredDisplayModeL(iDisplayMode);
	TSize size=iWsWin->Size();
	iBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmap->Create(size, iDisplayMode));
	iBitmapDevice=CFbsBitmapDevice::NewL(iBitmap);
	SetSizeInTwips();
//
	ActivateGc();
	iBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	iBitGc->Clear(TRect(size));
	iBitGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	WS_ASSERT_DEBUG(iWsWin->WinType()==EWinTypeClient,EWsPanicWindowType);
	}

void CWsBackedUpWindow::PrepareForResizeL(const TSize &aSize, TSize &aOldSize)
	{
	aOldSize=iBitmapDevice->SizeInPixels();
	if (aOldSize!=aSize)
		{
		User::LeaveIfError(iBitmapDevice->Resize(aSize));
		SetSizeInTwips();
		}
	}

void CWsBackedUpWindow::Resize(const TSize &aSize, const TSize &aOldSize)
	{
	ActivateGc();
	iBitGc->SetClippingRegion(NULL);
	iBitGc->Clear(TRect(aOldSize.iWidth, 0, aSize.iWidth, aOldSize.iHeight));
	iBitGc->Clear(TRect(0, aOldSize.iHeight,aSize.iWidth, aSize.iHeight));
	
	static_cast<CWsClientWindow *>(iWsWin)->ReactivateGcs();
	
	if(Screen()->ChangeTracking())
		{
		//Keep track of the region we need to refresh when we recieve draw commands
		iCurrentRegion.Reset();
		iCurrentRegion.Copy(iWsWin->WindowArea());
		iCurrentRegion.Offset(-iWsWin->Origin());
		iCurrentRegion.Tidy();
		if(iCurrentRegion.CheckError())
			{
			iCurrentRegion.Reset();
			TRegionFix<1> fallback(iWsWin->AbsRect());
			iCurrentRegion.Copy(fallback); 
			iCurrentRegion.Offset(-iWsWin->Origin());
			}
		
		//If the window has nerver been drawn to screen, we now schedule the initial draw. This can't
		//be done in ConstructL because BackedUpWindows are created with size(0,0). And we can't check
		//iWsWin->IsActive() because the client might activate the window before giving it a size.
		if (!iHasBeenScheduled)
			{
			iHasBeenScheduled = ETrue;
			MarkDirtyAndSchedule(iCurrentRegion);
			}
		}
	}

CWsBackedUpWindow::~CWsBackedUpWindow()
	{
	iCurrentRegion.Reset();
	delete iBitmapDevice;
	delete iBitmap;
	}

TBool CWsBackedUpWindow::CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)
	{
	switch(aOpcode)
		{
		case EWsWinOpUpdateBackupBitmap:
			break;
		case EWsWinOpMaintainBackup:
			break;
		case EWsWinOpBitmapHandle:
			SetReply(iBitmap->Handle());
			break;
		case EWsWinOpUpdateScreen:
			{
			TRegionFix<1> fixRegion(iWsWin->AbsRect());
			UpdateScreen(fixRegion);
			}
			break;
		case EWsWinOpUpdateScreenRegion:
			{
			RWsRegion *clientRegion=NULL;
			TRAPD(err,clientRegion=GetRegionFromClientL(iWsWin->WsOwner(), *aCmd.Int));
			if (err==KErrNone && !clientRegion->CheckError())
				{
				clientRegion->Offset(iWsWin->Origin());
				clientRegion->ClipRect(iWsWin->AbsRect());
				UpdateScreen(*clientRegion);
				}
			else
				{
				TRegionFix<1> fixRegion(iWsWin->AbsRect());
				UpdateScreen(fixRegion);
				}
			clientRegion->Destroy();
			}
			break;
		case EWsWinOpSetBackgroundSurface:
		case EWsWinOpSetBackgroundSurfaceConfig:
		case EWsWinOpGetBackgroundSurfaceConfig:
			OwnerPanic(EWservPanicDrawable);	// Backed up windows don't support these
			break;
		default:
			return(EFalse);
		}
	return(ETrue);
	}

CWsBackedUpWindow *CWsBackedUpWindow::Backup() const
	{
	return((CWsBackedUpWindow *)this);
	}

CFbsDevice* CWsBackedUpWindow::OutputDevice() const
	{
	return iBitmapDevice;
	}

TRgb CWsBackedUpWindow::BackColor() const
	{
	return(iWsWin->RootWindow()->DefaultBackgroundColor());
	}

void CWsBackedUpWindow::Scroll(const TRect &aClipRect, const TPoint &aOffset,const TRect &aRect)
	{
	TRect winBorder=TRect(iWsWin->Size());
	TRect clipRect=aClipRect;
	TRect srcRect = aRect;		
	clipRect.Intersection(winBorder);	
	if (!clipRect.IsEmpty())
		{	// If we have to do something (a visible part will change)
		srcRect.Intersection(clipRect);

		STACK_REGION regionToClear;
		regionToClear.AddRect(aRect);
		regionToClear.SubRect(srcRect);
		regionToClear.Offset(aOffset);
		
		ActivateGc();
		iBitGc->SetClippingRect(clipRect);
		iBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		iBitGc->CopyRect(aOffset,srcRect);				
		for (TInt k=0;k<regionToClear.Count();k++)
			{
			iBitGc->Clear(regionToClear[k]);
			}
		iBitGc->SetClippingRect(winBorder);
		iBitGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		TRegionFix<1> fixRegion(iWsWin->AbsRect());
		UpdateScreen(fixRegion);
		regionToClear.Close();
		}
	}

TBool CWsBackedUpWindow::NeedsRedraw() const
	{
	return(EFalse);
	}

TBool CWsBackedUpWindow::GetRedrawRect(TRect &) const
	{
	return(EFalse);
	}

void CWsBackedUpWindow::SetSizeInTwips()
	{
	TSize size=iBitmap->SizeInPixels();
	size.iWidth=Screen()->DeviceMap().HorizontalPixelsToTwips(size.iWidth);
	size.iHeight=Screen()->DeviceMap().VerticalPixelsToTwips(size.iHeight);
	iBitmap->SetSizeInTwips(size);
	}

void CWsBackedUpWindow::DrawWindow()
	{
	MWsGraphicsContext* gc = static_cast<MWsGraphicsContext*>(CPlaybackGc::Instance()->ResolveObjectInterface(KMWsGraphicsContext));
	
	gc->SetOrigin(iWsWin->Origin());
	gc->SetClippingRegion(*iRedrawRegion);
	gc->BitBlt(TPoint(0,0), *iBitmap);
	}

/** 
This function updates the window's dirty region and schedules a redraw if needed.
Only used when the screen is run in CHANGETRACKING mode. 
@param aRegion in window coordinates
*/
void CWsBackedUpWindow::MarkDirtyAndSchedule(const TRegion& aRegion)
	{
	WS_ASSERT_DEBUG(Screen()->ChangeTracking(),EWsPanicNoChangetracking);
	
	if(!aRegion.IsEmpty())
		{
		iWsWin->AddDirtyWindowRegion(aRegion);
		if (iWsWin->IsActive() && iWsWin->IsVisible())
			{
			Screen()->ScheduleWindow(iWsWin);
			}
		}
	}

/**
This function selects the desired behaviour depending on whether the screen is
run in CHANGETRACKING mode or not. 
@param aRegion in screen coordinates
*/
void CWsBackedUpWindow::UpdateScreen(const TRegion& aRegion)
	{
	if (Screen()->ChangeTracking())
		{
		STACK_REGION region;
		region.Copy(aRegion);
		region.Offset(-WsWin()->Origin()); //convert to window coordinates 
		MarkDirtyAndSchedule(region);
		region.Close();		
		}
	else
		{
		Screen()->AddRedrawRegion(aRegion);
		}
	}
