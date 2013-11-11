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
// Sending a redraw message to the client (see redrawmsgwindow.cpp)
// Drawing from backup bitmap
// Simply clearing the window
// 
//

#include "wnredraw.h"
#include "server.h"
#include "playbackgc.h"
#include "wstop.h"
#include "ANIM.H"
#include "EVQUEUE.H"
#include "offscreenbitmap.h"
#include <s32mem.h>
#include <gdi.h>
#include "panics.h"
#include "inifile.h"
#include "rootwin.h"
#include "EVENT.H"
#include "wstypes.h"

struct TFadingParams
	{
	TUint8 blackMap;
	TUint8 whiteMap;
	};

CWsWindowRedraw::CWsWindowRedraw(CWsWindow *aWin) : iWsWin(aWin)
	{
	}

CWsWindowRedraw::~CWsWindowRedraw()
	{
	if (iWsWin->WsOwner())
		{
		iWsWin->WsOwner()->RedrawQueue()->RemoveInvalid(this);
		}
	}

void CWsWindowRedraw::ConstructL()
	{
	}

void CWsWindowRedraw::ClipInvalidRegion(const TRect &)
	{
	}

void CWsWindowRedraw::Resize(const TSize &, const TSize &)
	{
	}

void CWsWindowRedraw::SetReply(TInt aReply)
	{
	iWsWin->WsOwner()->SetReply(aReply);
	}

void CWsWindowRedraw::OwnerPanic(TClientPanic aPanic)
	{
	iWsWin->OwnerPanic(aPanic);
	}

CWsBackedUpWindow *CWsWindowRedraw::Backup() const
	{
	return(NULL);
	}

void CWsWindowRedraw::Scroll(const TRect &, const TPoint &,const TRect &)
	{
	}

void CWsWindowRedraw::UpdateAnimArea()
	{
	}

void CWsWindowRedraw::PrepareForResizeL(const TSize &, TSize &)
	{
	}

TBool CWsWindowRedraw::DrawCommand(CWsGc*,const TAny*)
	{
	return ETrue;
	}

void CWsWindowRedraw::GcAttributeChange(CWsGc*,const TAny*)
	{
	}

void CWsWindowRedraw::GcDeactivate(CWsGc*)
	{
	}

CFbsDevice* CWsWindowRedraw::OutputDevice() const
	{
	return iWsWin->Screen()->GetFbsDevice();
	}

void CWsWindowRedraw::ClientExposing()
	{
	}

void CWsWindowRedraw::ClearRedrawStore(TBool)
	{}

void CWsWindowRedraw::PreDrawWindow(CFbsBitGc* aGc, const TRegion &aRegion)
	{
	WS_ASSERT_DEBUG(iGlobalRedrawRegion == NULL, EWsPanicScheduledRedraw);
	iGlobalRedrawRegion = &aRegion;
	CPlaybackGc::Instance()->SetTargetRegion(iGlobalRedrawRegion);
	CWsClient::iCurrentCommand.iOpcode=0; // Andy - urgh - loose me please
	CPlaybackGc::Instance()->Activate(CliWin(), aGc, iGlobalRedrawRegion);
	}

void CWsWindowRedraw::PostDrawWindow(CFbsBitGc* aGc)
	{	
	if(CWsTop::IsFadeEnabled() && iWsWin->FadeCount()>0 && !iWsWin->IsNonFading())
		{
		//x86gcc compiler needs this syntax to perform 
		//multiple inheritance duplicate common base class declaration resolution
		//combined with the template resolution.
		MWsGc* objectInterfaceResolver=CPlaybackGc::Instance();
		MWsFader* fader = objectInterfaceResolver->ObjectInterface<MWsFader>();
		if(fader)
			{
	      	TFadingParams parameters;
	      	iWsWin->GetFadingParams(parameters.blackMap,parameters.whiteMap);
	      	TPckgBuf<TFadingParams> buf(parameters);
	      	fader->SetFadingParameters(buf);
	      	// Only fade the region that hasn't been faded before
      		STACK_REGION fdRgn;
      		fdRgn.Copy( *iGlobalRedrawRegion );
      		fdRgn.Intersect( iWsWin->FadableRegion() );
      		fader->FadeArea( aGc, &fdRgn );
      		fdRgn.Close();
	      	}
		}

	CPlaybackGc::Instance()->Deactivate();
	
	WS_ASSERT_DEBUG(iGlobalRedrawRegion, EWsPanicScheduledRedraw);
	DrawAnims(aGc, *iGlobalRedrawRegion);
	
	CPlaybackGc::Instance()->SetTargetRegion(NULL);
	iGlobalRedrawRegion = 0;
	}
	
void CWsWindowRedraw::DrawAnims(CFbsBitGc * aGc, const TRegion& aRegion)
	{
	if (iWsWin->iAnimList)
		{
		// If an anim panics, it will leave and set the panic flag on the client
		// The client itself won't actually panic yet, and we don't want to leave from here.
		TRAP_IGNORE(DrawAnimsL(aGc, aRegion));
		}

	// Draw standard text cursor if required
	RWsTextCursor* const cursor = CWsTop::CurrentTextCursor();
	if (cursor && cursor->Win() == iWsWin && cursor->IsStandardCursorActive())
		{
		// Standard text cursor is active on this window
		const TBool flashing = cursor->IsFlashing();
		TFlashState flashState = EFlashOff;
		if (flashing)
			{
			flashState = Screen()->SpriteManager()->CurrentCursorFlashState();
			}
		if (flashState == EFlashOn || !flashing)
			{
			// Cursor should be visible, so draw it
			aGc->Reset();
			cursor->Draw(aGc, aRegion);
			}
		if (flashing)
			{
			// Reschedule to flash the standard cursor on or off
			Screen()->ScheduleAnimation(cursor->RectRelativeToScreen(), Screen()->SpriteManager()->NextCursorFlashStateChange(), 0, 0);
			}
		}

	for (CWsSpriteBase * sprite = iWsWin->iSpriteList; sprite; sprite = sprite->Next())
		{
		aGc->Reset();
		sprite->Redraw(aGc, aRegion);
		}
	}

void CWsWindowRedraw::DrawAnimsL(CFbsBitGc * aGc, const TRegion& aRegion)
	{
	for (CWsAnim * anim = iWsWin->iAnimList; anim; anim = anim->Next())
		{
		// This can leave
		TRAPD(err,anim->Redraw(aGc, &aRegion));
		if(err!=KErrNone)
			{
			anim->Panic(EWservPanicAnimLeave);
			}
		}
	}

TBool CWsWindowRedraw::Contains(const TArray<TGraphicDrawerId>& /*aDrawers*/,const TRegion& aRegion) const
	{
	// if in doubt, assume we do
	return !aRegion.IsEmpty();
	}

TInt CWsWindowRedraw::DrawBackgroundColor(const TRegion* aRegion)
	{
	if(Screen()->BltOffScreen())
		{
		return KErrNone;
		}
	TRgb col = BackColor();
	if (col.Alpha() == 0)
		{
		return KErrNone;
		}
	TRect abs(CliWin()->AbsRect());
	abs.Move(-CliWin()->Origin());
	TBuf8<sizeof(TRect)> cmd;
	cmd.Copy(reinterpret_cast<TUint8*>(&abs),sizeof(TRect));
	CPlaybackGc::Instance()->BitGc().SetClippingRegion(aRegion);
	CPlaybackGc::Instance()->BitGc().SetBrushColor(col);
	CPlaybackGc::Instance()->BitGc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	CPlaybackGc::Instance()->BitGc().SetPenStyle(CGraphicsContext::ENullPen);
	// Andy - this is pretty ridiculous - can't we just draw using the gc?
	TRAPD(err,CPlaybackGc::Instance()->CommandL(EWsGcOpDrawRect, cmd));		//Should not leave ###
	CPlaybackGc::Instance()->BitGc().SetDrawMode(CGraphicsContext::EDrawModePEN);
	CPlaybackGc::Instance()->BitGc().CancelClippingRegion();
	return err;
	}

TBool CWsWindowRedraw::ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel)
	{
	return EFalse;
	}

void CWsWindowRedraw::VisibleRegionChange()
	{
	}
	
TBool CWsWindowRedraw::ReadyToDraw() const
	{
	return ETrue;
	}

void CWsWindowRedraw::WindowClosing()
	{}
	
//
// Blank up window //
//

CWsBlankWindow::CWsBlankWindow(CWsWindow *aWin) : CWsWindowRedraw(aWin), iColor(iWsWin->RootWindow()->DefaultBackgroundColor()), iNoColor(EFalse)
	{
	}

CWsBlankWindow::~CWsBlankWindow()
	{
	}

void CWsBlankWindow::SetColor(TRgb aColor)
	{
	iColor=aColor;
	iNoColor=EFalse;
	Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
	}

TBool CWsBlankWindow::CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)
	{
	switch(aOpcode)
		{
		case EWsWinOpSetColor:
			SetColor(*aCmd.rgb);
			break;
		case EWsWinOpSetNoBackgroundColor:
			SetBackgroundClear();
			break;
		default:
			return(EFalse);
		}
	return(ETrue);
	}

TRgb CWsBlankWindow::BackColor() const
	{
	return(iColor);
	}

TBool CWsBlankWindow::GetRedrawRect(TRect &) const
	{
	if (!iNoColor || iWsWin->iAnimList)
		iWsWin->Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
	return(EFalse);
	}

TBool CWsBlankWindow::NeedsRedraw() const
	{
	return(EFalse);
	}

void CWsBlankWindow::DrawWindow()
	{
	if (!iNoColor)
		{
		DrawBackgroundColor(iGlobalRedrawRegion);
		}
	}
