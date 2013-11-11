// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Debug bar
// 
//

#include "debugbar.h"
#include <bitstd.h>
#include "screen.h"
#include "wstop.h"
#include <e32cmn.h>


CDebugBar* CDebugBar::NewL(CScreen * aScreen,const TRect& aScreenRect,TTimeIntervalMicroSeconds32 aRefreshInterval)
	{
	CDebugBar* self = new(ELeave) CDebugBar(aScreen,aScreenRect);
	CleanupStack::PushL(self);
	self->ConstructL(aRefreshInterval);
	CleanupStack::Pop(self);
	return self;
	}

CDebugBar::~CDebugBar()
	{
	iScreen->ScreenDevice()->ReleaseFont(iFont);
	delete iTimer;
	}
	
CDebugBar::CDebugBar(CScreen * aScreen,const TRect& aScreenRect) : iScreen(aScreen), iScreenRect(aScreenRect)
	{
	}
	
void CDebugBar::ConstructL(TTimeIntervalMicroSeconds32 aRefreshInterval)
	{
	iTimer = CPeriodic::NewL(EPriorityNormal);
	iTimer->Start(TTimeIntervalMicroSeconds32(3000000),aRefreshInterval,TCallBack(RedrawDebugBarCallback, this));
	TFontSpec fspec(_L("DejaVu Sans Condensed"),9);
	User::LeaveIfError(iScreen->ScreenDevice()->GetNearestFontToDesignHeightInPixels(iFont, fspec));
	iBaseline = (iFont->BaselineOffsetInPixels() + iFont->AscentInPixels());
	}
	
const TRect& CDebugBar::Rect() const
	{
	return iScreenRect;
	}

TBool CDebugBar::RedrawDebugBarCallback(TAny * aAny)
	{
	CDebugBar* self = reinterpret_cast<CDebugBar *>(aAny);
	self->iPolls++; //minus-out the events which cause this debug-bar to 'tick'
	self->RedrawDebugBar();
	self->iScreen->Update(TRegionFix<1>(self->iScreenRect));
	return ETrue;
	}
	
#include "walkwindowtree.h"	
#include "rootwin.h"
void CDebugBar::RedrawDebugBar() const
	{
	_LIT(KLine1Format, "KeyEvents %d; %Ld; WServ %.1f%%; Drawing %.1f%%; Reclaimed %.1f%%");
	_LIT(KLine2Format, "RedrawStore size %d3B; updates %d/s, pixels %d/s"); 
	CWsActiveScheduler* scheduler = CWsActiveScheduler::Static();
	TInt updatesPerSecond;
	TInt64 pixelsPerSecond;
	scheduler->DrawStats(updatesPerSecond,pixelsPerSecond,2);
	CFbsBitGc *gc = iScreen->ScreenGdi();
	gc->Reset();
	gc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	gc->SetPenColor(KRgbWhite);
	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetBrushColor(KRgbBlack);
	gc->DrawRect(iScreenRect);
	gc->UseFont(iFont);
	
	TInt64 wserv = scheduler->Total() - iLastTotal;
	TInt64 nonwserv = scheduler->Idle() - iLastIdle;
	TInt64 drawing = scheduler->Drawing() - iLastDrawing;
	TInt64 reclaimed = scheduler->ReclaimedIdleTime() - iLastReclaimedIdleTime;
	TReal wservPercent =  wserv+nonwserv? ((TReal)wserv / (TReal)(wserv + nonwserv)) * 100.0 : 0.0 ;
	TReal drawingPercent = wserv? ((TReal)drawing / (TReal)wserv) * 100.0: 0 ;
	TReal reclaimedPercent = wserv+nonwserv? ((TReal)reclaimed / (TReal)(wserv + nonwserv)) * 100.0: 100.0 ;
	
	TWalkWindowTreeRedrawStoreSize redrawStoreWalker;
	iScreen->RootWindow()->WalkWindowTree(redrawStoreWalker, EWalkChildrenAndBehind);
	
	TRect textRect(iScreenRect);
	textRect.SetHeight(textRect.Height() / 2);
	iBuf.Format(KLine1Format,iKeyEvents,scheduler->Requests()-iPolls,wservPercent,drawingPercent,reclaimedPercent);
	gc->DrawText(iBuf,textRect,iBaseline);
	
	textRect.Move(TPoint(0,textRect.Height()));
	iBuf.Format(KLine2Format,redrawStoreWalker.iTotalSize, updatesPerSecond, pixelsPerSecond);
	gc->DrawText(iBuf,textRect,iBaseline);
	
	gc->DiscardFont();
	
	iLastTotal=scheduler->Total();
	iLastIdle=scheduler->Idle();
	iLastDrawing=scheduler->Drawing();
	iLastReclaimedIdleTime=scheduler->ReclaimedIdleTime();
	}

void CDebugBar::OnKeyEvent()
	{
	iKeyEvents++;
	RedrawDebugBar();
	iScreen->Update(TRegionFix<1>(iScreenRect));
	}
	
