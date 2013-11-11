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

CFbsBitGc *CWsBackedUpWindow::iBitGc=NULL;

CWsBackedUpWindow::CWsBackedUpWindow(CWsWindow *aWin, TDisplayMode aDisplayMode) : CWsWindowRedraw(aWin), iDisplayMode(aDisplayMode)
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
	Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
	return ETrue;
	}

void CWsBackedUpWindow::ConstructL()
	{
	iDisplayMode=iWsWin->DisplayMode();
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
	}

CWsBackedUpWindow::~CWsBackedUpWindow()
	{
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
			// Andy - shouldn't this use the base area?  Or don't we allow funny shaped
			// backup windows?
			TRegionFix<1> fixRegion(iWsWin->AbsRect());
			Screen()->AddRedrawRegion(fixRegion);
			}
			break;
		case EWsWinOpUpdateScreenRegion:
			{
			TRegionFix<1> fixRegion(iWsWin->AbsRect());
			RWsRegion *clientRegion=NULL;
			TRAPD(err,clientRegion=GetRegionFromClientL(iWsWin->WsOwner(), *aCmd.Int));
			if (err==KErrNone)
				{
				clientRegion->Offset(iWsWin->Origin());
				clientRegion->ClipRect(iWsWin->AbsRect());
				Screen()->AddRedrawRegion(*static_cast<TRegion*>(clientRegion));
				clientRegion->Destroy();
				}
			else
				{
				Screen()->AddRedrawRegion(fixRegion);
				}
			}
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
		Screen()->AddRedrawRegion(fixRegion);		
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
	size.iWidth=Screen()->ScreenDevice()->HorizontalPixelsToTwips(size.iWidth);
	size.iHeight=Screen()->ScreenDevice()->VerticalPixelsToTwips(size.iHeight);
	iBitmap->SetSizeInTwips(size);
	}

void CWsBackedUpWindow::DrawWindow()
	{
	CFbsBitGc& gc = CPlaybackGc::Instance()->BitGc();
	gc.SetUserDisplayMode(iWsWin->DisplayMode());
	gc.SetOrigin(iWsWin->Origin());
	gc.SetClippingRegion(iGlobalRedrawRegion);
	gc.BitBlt(TPoint(0,0), iBitmap);
	}
