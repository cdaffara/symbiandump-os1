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
#include "walkwindowtree.h"	
#include "rootwin.h"

CDebugBar* CDebugBar::NewL(CScreen * aScreen, TTimeIntervalMicroSeconds32 aRefreshInterval)
	{
	CDebugBar* self = new(ELeave) CDebugBar(aScreen);
	CleanupStack::PushL(self);
	self->ConstructL(aRefreshInterval);
	CleanupStack::Pop(self);
	return self;
	}

CDebugBar::~CDebugBar()
	{
	delete iTimer;	
	}
	
CDebugBar::CDebugBar(CScreen * aScreen) : iScreen(aScreen)
	{
	}
	
void CDebugBar::ConstructL(TTimeIntervalMicroSeconds32 aRefreshInterval)
	{
	iTimer = CPeriodic::NewL(EPriorityNormal);
	iTimer->Start(TTimeIntervalMicroSeconds32(3000000),aRefreshInterval,TCallBack(RedrawDebugBarCallback, this));
	}
	
TBool CDebugBar::RedrawDebugBarCallback(TAny * aAny)
	{
	CDebugBar* self = reinterpret_cast<CDebugBar *>(aAny);
	self->iPolls++; //minus-out the events which cause this debug-bar to 'tick'
	self->iScreen->ScheduleRender(0);
	return ETrue;
	}
	
void CDebugBar::DebugBarInfo(RArray<TPtrC>& aArray) const
	{	
	_LIT(KLine1Format, "KeyEvents %d; %Ld; WServ %.1f%%; Drawing %.1f%%; Reclaimed %.1f%%");
	_LIT(KLine2Format, "RedrawStore size %d3B; updates %d/s, pixels %d/s"); 
	CWsActiveScheduler* scheduler = CWsActiveScheduler::Static();
	TInt updatesPerSecond;
	TInt64 pixelsPerSecond;
	scheduler->DrawStats(updatesPerSecond,pixelsPerSecond,2);

	TInt64 wserv = scheduler->Total() - iLastTotal;
	TInt64 nonwserv = scheduler->Idle() - iLastIdle;
	TInt64 drawing = scheduler->Drawing() - iLastDrawing;
	TInt64 reclaimed = scheduler->ReclaimedIdleTime() - iLastReclaimedIdleTime;
	TReal wservPercent =  wserv+nonwserv? ((TReal)wserv / (TReal)(wserv + nonwserv)) * 100.0 : 0.0 ;
	TReal drawingPercent = wserv? ((TReal)drawing / (TReal)wserv) * 100.0: 0 ;
	TReal reclaimedPercent = wserv+nonwserv? ((TReal)reclaimed / (TReal)(wserv + nonwserv)) * 100.0: 100.0 ;
		
	TWalkWindowTreeRedrawStoreSize redrawStoreWalker;
	iScreen->RootWindow()->WalkWindowTree(redrawStoreWalker, EWalkChildrenAndBehind);
		
	iTextLine0.Format(KLine1Format,iKeyEvents,scheduler->Requests()-iPolls,wservPercent,drawingPercent,reclaimedPercent);
	iTextLine1.Format(KLine2Format,redrawStoreWalker.iTotalSize, updatesPerSecond, pixelsPerSecond);
		
	iLastTotal=scheduler->Total();
	iLastIdle=scheduler->Idle();
	iLastDrawing=scheduler->Drawing();
	iLastReclaimedIdleTime=scheduler->ReclaimedIdleTime();	
			
	aArray.Reset();
	TPtrC ptr(iTextLine0);
	aArray.Append(ptr);
	ptr.Set(iTextLine1);
	aArray.Append(ptr);	
	}

void CDebugBar::OnKeyEvent()
	{
	iKeyEvents++;
	iScreen->ScheduleRender(0);
	}


