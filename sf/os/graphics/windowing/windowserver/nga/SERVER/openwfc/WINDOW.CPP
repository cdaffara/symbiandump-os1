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
// Window functions
// 
//

#include "server.h"
#include "rootwin.h"
#include "window.h"
#include "walkwindowtree.h"
#include "wstop.h"
#include "ScrDev.H"
#include "EVENT.H"
#include "ANIM.H"
#include "Direct.H"
#include "panics.h"
#include "backedupwindow.h"
#include "wstypes.h"
#include "windowelementset.h"

CWsWindow* CWsWindow::iAccessListRecentEnd = 0;
CWsWindow* CWsWindow::iAccessListOldestEnd = 0;

#ifndef _DEBUG

#define LOG_WINDOW_REDRAW_START(wswin)
#define LOG_WINDOW_REDRAW_END(wswin)
#define LOG_WINDOW_ANIM_REDRAW_START(wswin)
#define LOG_WINDOW_ANIM_REDRAW_END(wswin)
#define LOG_WINDOW_SPRITE_REDRAW_START(aWsWin)
#define LOG_WINDOW_SPRITE_REDRAW_END(aWsWin)

#else

#define LOG_WINDOW_REDRAW_START(wswin) LogWindowRedrawStart(wswin)
#define LOG_WINDOW_REDRAW_END(wswin) LogWindowRedrawEnd(wswin)
#define LOG_WINDOW_ANIM_REDRAW_START(wswin) LogWindowAnimRedrawStart(wswin)
#define LOG_WINDOW_ANIM_REDRAW_END(wswin) LogWindowAnimRedrawEnd(wswin)
#define LOG_WINDOW_SPRITE_REDRAW_START(wswin) LogWindowSpriteRedrawStart(wswin)
#define LOG_WINDOW_SPRITE_REDRAW_END(wswin) LogWindowSpriteRedrawEnd(wswin) 

#include "../debuglog/DEBUGLOG.H"
extern CDebugLogBase *wsDebugLog;

extern void LogRegion(const TRegion* aRegion)
    {
    TBuf<LogTBufSize> log;
    TTruncateOverflow overflow;
    TInt rectCount = (aRegion == NULL ? 0 : aRegion->Count());
    log.AppendFormat(_L("Fading region: [%d,"), &overflow, rectCount);
    if (rectCount > 0)
        {
        const TRect* rectangles = aRegion->RectangleList();
        TBuf<1> comma;
        for (TInt ii = 0; ii < rectCount; ii++)
            {
            TRect current = rectangles[ii];
            log.AppendFormat(_L("%S{{%d,%d},{%d,%d}}"), &overflow, &comma,
                             current.iTl.iX,current.iTl.iY,current.iBr.iX,current.iBr.iY);
            comma = _L(",");
            }
        }
    else
        {
        log.AppendFormat(_L("NULL"), &overflow);
        }
    log.AppendFormat(_L("]"), &overflow);
    wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
    }

extern void LogFadeStart(const CWsWindow* aWsWin)
    {
    if (wsDebugLog && aWsWin->WsOwner())
        {
        _LIT(KLogDrawCommandsStart, ">> CWsRedrawMsgWindow::Fade() [%S][app %d] RWindow[%d]");
        const TDesC& clientName = aWsWin->WsOwner()->Client().FullName();
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        log.AppendFormat(KLogDrawCommandsStart, &overflow, &clientName, aWsWin->WsOwner()->ConnectionHandle(), aWsWin->LogHandle());
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

extern void LogFadeRegion(const TRegion* aRegion)
    {
    if (wsDebugLog)
        {
        LogRegion(aRegion);
        }
    }

extern void LogFadeEnd(const CWsWindow* aWsWin)
    {
    if (wsDebugLog && aWsWin->WsOwner())
        {
        _LIT(KLogDrawCommandsEnd, "<< CWsRedrawMsgWindow::Fade() [%S][app %d] RWindow[%d]");
        const TDesC& clientName = aWsWin->WsOwner()->Client().FullName();
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        log.AppendFormat(KLogDrawCommandsEnd, &overflow, &clientName, aWsWin->WsOwner()->ConnectionHandle(), aWsWin->LogHandle());      
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogWindowRedrawStart(const CWsWindow& aWsWin)
    {
    if (wsDebugLog)
         {
         TBuf<LogTBufSize> log;
         TTruncateOverflow overflow;
         if(aWsWin.Type() == WS_HANDLE_ROOT_WINDOW)
             {
             _LIT(KAnnotateWindowRedrawStart, ">> MWsDrawAnnotationObserver::WindowRedrawStart RootWindow for screen:%d");
             log.AppendFormat(KAnnotateWindowRedrawStart, &overflow, aWsWin.Screen()->ScreenNumber());
             }
         else
             {
             _LIT(KAnnotateWindowRedrawStart, ">> MWsDrawAnnotationObserver::WindowRedrawStart [%S][app %d] RWindow[%d]");
             const TDesC& clientName = aWsWin.WsOwner()->Client().FullName();
             log.AppendFormat(KAnnotateWindowRedrawStart, &overflow, &clientName, aWsWin.WsOwner()->ConnectionHandle(), aWsWin.LogHandle());
             }
         wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
         }    
    }

static void LogWindowRedrawEnd(const CWsWindow& aWsWin)
    {
    if (wsDebugLog)
        {
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        if(aWsWin.Type() == WS_HANDLE_ROOT_WINDOW)
            {
            _LIT(KAnnotateWindowRedrawEnd, "<< MWsDrawAnnotationObserver::WindowRedrawEnd RootWindow for screen:%d");
            log.AppendFormat(KAnnotateWindowRedrawEnd, &overflow, aWsWin.Screen()->ScreenNumber());
            }
        else
            {
            _LIT(KAnnotateWindowRedrawEnd, "<< MWsDrawAnnotationObserver::WindowRedrawEnd [%S][app %d] RWindow[%d]");
            const TDesC& clientName = aWsWin.WsOwner()->Client().FullName();
            log.AppendFormat(KAnnotateWindowRedrawEnd, &overflow, &clientName, aWsWin.WsOwner()->ConnectionHandle(), aWsWin.LogHandle());
            }
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogWindowAnimRedrawStart(const CWsWindow& aWsWin)
    {
    if (wsDebugLog)
        {
        _LIT(KAnnotateWindowAnimRedrawStart, " >> MWsDrawAnnotationObserver::WindowAnimRedrawStart [%S][app %d] RWindow[%d]");
        const TDesC& clientName = aWsWin.WsOwner()->Client().FullName();
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        log.AppendFormat(KAnnotateWindowAnimRedrawStart, &overflow, &clientName, aWsWin.WsOwner()->ConnectionHandle(), aWsWin.LogHandle());
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogWindowAnimRedrawEnd(const CWsWindow& aWsWin)
    {
    if (wsDebugLog)
        {
        _LIT(KAnnotateWindowAnimRedrawEnd, " << MWsDrawAnnotationObserver::WindowAnimRedrawEnd [%S][app %d] RWindow[%d]");
        const TDesC& clientName = aWsWin.WsOwner()->Client().FullName();
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        log.AppendFormat(KAnnotateWindowAnimRedrawEnd, &overflow, &clientName, aWsWin.WsOwner()->ConnectionHandle(), aWsWin.LogHandle());
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogWindowSpriteRedrawStart(const CWsWindow& aWsWin)
    {
    if (wsDebugLog)
        {
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        if(aWsWin.Type() == WS_HANDLE_ROOT_WINDOW)
            {
            _LIT(KAnnotateSpriteRedrawStart, "<< MWsDrawAnnotationObserver::WindowSpriteRedrawStart RootWindow for screen:%d");
            log.AppendFormat(KAnnotateSpriteRedrawStart, &overflow, aWsWin.Screen()->ScreenNumber());
            }
        else
            {
            _LIT(KAnnotateSpriteRedrawStart, " >> MWsDrawAnnotationObserver::WindowSpriteRedrawStart [%S][app %d] RWindow[%d]");
            const TDesC& clientName = aWsWin.WsOwner()->Client().FullName();
            log.AppendFormat(KAnnotateSpriteRedrawStart, &overflow, &clientName, aWsWin.WsOwner()->ConnectionHandle(), aWsWin.LogHandle());
            }
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogWindowSpriteRedrawEnd(const CWsWindow& aWsWin)
    {
    if (wsDebugLog)
        {
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        if(aWsWin.Type() == WS_HANDLE_ROOT_WINDOW)
            {
            _LIT(KAnnotateSpriteRedrawEnd, " << MWsDrawAnnotationObserver::WindowSpriteRedrawEnd RootWindow for screen:%d");
            log.AppendFormat(KAnnotateSpriteRedrawEnd, &overflow, aWsWin.Screen()->ScreenNumber());
            }
        else
            {
            _LIT(KAnnotateSpriteRedrawEnd, " << MWsDrawAnnotationObserver::WindowSpriteRedrawEnd [%S][app %d] RWindow[%d]");
            const TDesC& clientName = aWsWin.WsOwner()->Client().FullName();
            log.AppendFormat(KAnnotateSpriteRedrawEnd, &overflow, &clientName, aWsWin.WsOwner()->ConnectionHandle(), aWsWin.LogHandle());
            }
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

#endif

extern void AnnotateWindowRedrawStart(const CWsWindow& aWsWin, const TRegion& aRegion)
    {
    LOG_WINDOW_REDRAW_START(aWsWin);
    MWsDrawAnnotationObserver* annoObs = aWsWin.Screen()->DrawAnnotationObserver();
    if(annoObs)
        {
        annoObs->WindowRedrawStart(aWsWin, aRegion);
        }
    }

extern void AnnotateWindowRedrawEnd(const CWsWindow& aWsWin)
    {
    LOG_WINDOW_REDRAW_END(aWsWin);
    MWsDrawAnnotationObserver* annoObs = aWsWin.Screen()->DrawAnnotationObserver();
    if(annoObs)
        {
        annoObs->WindowRedrawEnd(aWsWin);
        }
    }

extern void AnnotateWindowAnimRedrawStart(const CWsWindow& aWsWin, const CWsAnim& aAnim, const TRegion& aRegion)
    {
    LOG_WINDOW_ANIM_REDRAW_START(aWsWin);
    MWsDrawAnnotationObserver* annoObs = aWsWin.Screen()->DrawAnnotationObserver();
    if(annoObs)
        {
        annoObs->WindowAnimRedrawStart(aAnim, aRegion);
        }
    }

extern void AnnotateWindowAnimRedrawEnd(const CWsWindow& aWsWin, const CWsAnim& aAnim)
    {
    LOG_WINDOW_ANIM_REDRAW_END(aWsWin);
    MWsDrawAnnotationObserver* annoObs = aWsWin.Screen()->DrawAnnotationObserver();
    if(annoObs)
        {
        annoObs->WindowAnimRedrawEnd(aAnim);
        }
    }

extern void AnnotateSpriteRedrawStart(const CWsWindow& aWsWin, const CWsSpriteBase& aSprite, const TRegion& aRegion)
    {
    LOG_WINDOW_SPRITE_REDRAW_START(aWsWin);
    MWsDrawAnnotationObserver* annoObs = aWsWin.Screen()->DrawAnnotationObserver();
    if(annoObs)
        {
        annoObs->SpriteRedrawStart(aSprite, aRegion);
        }   
    }

extern void AnnotateSpriteRedrawEnd(const CWsWindow& aWsWin, const CWsSpriteBase& aSprite)
    {
    LOG_WINDOW_SPRITE_REDRAW_END(aWsWin);
    MWsDrawAnnotationObserver* annoObs = aWsWin.Screen()->DrawAnnotationObserver();
    if(annoObs)
        {
        annoObs->SpriteRedrawEnd(aSprite);
        }
    }

CWsWindow::CWsWindow(CWsClient* aOwner,WH_HANDLES aType, CScreen* aScreen) : CWsWindowBase(aOwner,aType,aScreen),
	iFlags(EFlagHidden),
	iDSAs(_FOFF(CWsDirectScreenAccess,iMultipleDSALink))
	{
	}

CWsWindow::~CWsWindow()
	{
	//2-phase destruction, Shutdown() is called before destructor
	iFadableRegion.Reset();
	iQuickFadeRegion.Reset();
	}

void CWsWindow::Fade(MWsGraphicsContext * aGc, const TRegion& aRegion)
	{
	if ( NULL != iRedraw )
		{
		iRedraw->Fade( aGc, aRegion );
		}
	}

void CWsWindow::Construct()
	{	
	InsertIntoAccessListOldestEnd();
	}

void CWsWindow::Shutdown()
	{
	TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EWindowClosing, 0, 0, this));
	RemoveFromAccessList();
	CWsAnim::WindowClosing(iAnimList);	// Destroy any animated objects attached to this window
	iVisibleRegion.Reset();
	iScheduledRegion.Reset();
	iScheduledSpriteRegion.Reset();
	iDirtyWindowRegion.Reset();
	iDirtySpriteRegion.Reset();
	AbortAllDSAs();
	CWsWindowBase::Shutdown();
	SetPointerCursor(NULL);
	delete iRedraw;
	iRedraw=NULL;
	Screen()->RemoveFromScheduledList(this);
	Screen()->RemoveFromTimedDrawList(this);
	Screen()->RemoveFromQuickFadeList(this);
	}

//
// Region and invalid area functions //
//

TRect CWsWindow::FullRect() const
	{
	return(TRect(iOrigin,iRel.Size()));
	}

//
// Normal regions //
//

void CWsWindow::AreaCovered(TRegion &aRegion)
	{
	aRegion.Copy(WindowArea());
	}

void CWsWindow::SetNonFading(TBool aNonFade)
	{
	const TUint oldFlags = iFlags;

	if (aNonFade)
		iFlags|=EFlagNonFadingWindow;
	else
		iFlags&=(~EFlagNonFadingWindow);
	
 	//Schedule window for quickfade if non-fading flag is changed	
	if (!Screen()->ChangeTracking() &&  CWsTop::IsFadeEnabled() && (oldFlags != iFlags) ) 
		{
		Screen()->AcceptFadeRequest(this, !aNonFade);
		}
	
	MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
	if (windowTreeObserver && oldFlags!=iFlags)
		windowTreeObserver->FlagChanged(*this, MWsWindowTreeObserver::ENonFading, aNonFade); 
	}

TPoint CWsWindow::InquireOffset(TUint32 aHandle) const
	{
	CWsWindowBase *win2;
	iWsOwner->HandleToWindow(aHandle,&win2);
	return(iOrigin-win2->Origin());
	}

TDisplayMode CWsWindow::SetRequiredDisplayModeL(TDisplayMode)
	{
	return DisplayMode();
	}

TDisplayMode CWsWindow::DisplayMode() const
	{
	return iScreen->DisplayMode();
	}

void CWsWindow::StatusDump(TDes &aBuf)
	{
	_LIT(KStatusString1,"CWsWindow[0x%x]RWindow[0x%x,%d],Pos(%d,%d),Size(%d,%d)");
	_LIT(KStatusString3,",Mode=%d");
	_LIT(KStatusInvisible,",Inv");
	aBuf.AppendFormat(KStatusString1,this,iClientHandle,LogHandle(),iRel.iTl.iX,iRel.iTl.iY,Size().iWidth,Size().iHeight);
	TDisplayMode displayMode = DisplayMode();
	aBuf.AppendFormat(KStatusString3,(TInt&)displayMode);
	if (!IsVisible())
		aBuf.Append(KStatusInvisible);
	}

TDblQue<TPointerKeyList> *CWsWindow::PointerKeyList() const
	{
	return(NULL);
	}

TInt CWsWindow::AddDSA(CWsDirectScreenAccess& aDirect)
	{
	TInt error = KErrNone;

	if (iDSAs.IsEmpty() && !aDirect.IsRegionTrackingOnly())
		{
		TRAP(error,iRedraw->SetDsaElementL());
		}

	if (!error)
		{
		iDSAs.AddLast(aDirect);
		}

	return error;
	}

void CWsWindow::RemoveDSA(CWsDirectScreenAccess& aDirect)
	{
	iDSAs.Remove(aDirect);
	}

void CWsWindow::AbortAllDSAs()
	{
	iScreen->AbortDSAs(RDirectScreenAccess::ETerminateRegion,iDSAs);
	}

void CWsWindow::PossibleVisibilityChangedEvent(TBool aForceSend)
	{
	// notify plugin
	//
	// coverity[unchecked_value]
	TWservCrEvent crEvent(TWservCrEvent::EWindowVisibilityChanged, HasBeenDrawnToScreen(), IsVisible()? &iVisibleRegion : NULL, this);
	TWindowServerEvent::NotifyDrawer(crEvent);
	
	if (!(iFlags & EFlagGeneratesVisibilityEvents))
		return;

	if (!IsVisible())
		{
		goto notvisible;
		}

	if (iVisibleRegion.Count() == 0)
		{
		goto notvisible;
		}

		{// braces here to avoid gccxml error
		TInt visibleArea = 0;
		TInt count = iVisibleRegion.Count();
		TInt ii;
		for (ii = 0; ii < count; ii++)
			{
			visibleArea+= iVisibleRegion[ii].Width() * iVisibleRegion[ii].Height();
			}

		const TRegion* baseRegion = (static_cast<CWsClientWindow*>(this))->BaseArea();
		TInt baseArea = 0;
		count = baseRegion->Count();
		for (ii = 0; ii < count; ii++)
			{
			const TRect& rect = (*baseRegion)[ii];
			baseArea+= rect.Width() * rect.Height();
			}

		if (visibleArea == baseArea)
			{
			goto fullyvisible;
			}
		else
			{
			goto partiallyvisible;
			}
		}

fullyvisible:
	if (aForceSend || !(iFlags & EFlagNotCantBeSeen) || !(iFlags & EFlagCanBeSeen))
		{
		iFlags |= (EFlagCanBeSeen | EFlagNotCantBeSeen);
		TWindowServerEvent::SendVisibilityChangedEvents(this, TWsVisibilityChangedEvent::EPartiallyVisible | TWsVisibilityChangedEvent::EFullyVisible);
		// we have to set EPartiallyVisible too for compatibility reasons
		}
	return;

partiallyvisible:
	if (aForceSend || !(iFlags & EFlagCanBeSeen) || (iFlags & EFlagNotCantBeSeen))
		{
		iFlags |= EFlagCanBeSeen;
		iFlags &= ~EFlagNotCantBeSeen;
		TWindowServerEvent::SendVisibilityChangedEvents(this, TWsVisibilityChangedEvent::EPartiallyVisible);
		}
	return;

notvisible:
	if (aForceSend || (iFlags & EFlagCanBeSeen) || (iFlags & EFlagNotCantBeSeen))
		{
		iFlags &= ~(EFlagCanBeSeen | EFlagNotCantBeSeen);
		TWindowServerEvent::SendVisibilityChangedEvents(this, TWsVisibilityChangedEvent::ENotVisible);
		}
	return;
	}

TPoint CWsWindow::Origin() const
	{
	return iOrigin;
	}

TRect CWsWindow::AbsRect() const
	{
	return iAbs;
	}
	
TSize CWsWindow::Size() const
	{
	return iRel.Size();
	}

TBool CWsWindow::SetScheduledRegion(const TRegion& aRegion)
	{
	iScheduledRegion.Copy(aRegion);
	return !iScheduledRegion.CheckError();
	}

void CWsWindow::ClearScheduledRegion()
	{
	iScheduledRegion.Reset();
	}

void CWsWindow::SetFadeBehind(TBool aFade)
	{
	if (aFade != ((iFlags & EFlagFadeBehind) != 0))
		{
		iFlags ^= EFlagFadeBehind;
		}
	}

void CWsWindow::FocusChanged(TBool aNewFocusState)
	{
	TRAPD(err,FocusChangedL(aNewFocusState));
	if (err!=KErrNone)
		OwnerPanic(EWservPanicAnimLeave);
	}

void CWsWindow::FocusChangedL(TBool aNewFocusState)
	{
	for (CWsAnim * anim = iAnimList; anim; anim = anim->Next())
		{
		anim->FocusChanged(aNewFocusState);
		}
	}

// Moves a window to the recent end of the accessed list
void CWsWindow::Accessed()
	{
	WS_ASSERT_DEBUG(iAccessListRecentEnd && iAccessListOldestEnd, EWsPanicAccessList);

	RemoveFromAccessList();	
	InsertIntoAccessListRecentEnd();
	}

void CWsWindow::InsertIntoAccessListOldestEnd()
	{
	iAccessListPrev = 0;
	iAccessListNext = iAccessListOldestEnd;
	if (iAccessListNext)
		iAccessListNext->iAccessListPrev = this;
	iAccessListOldestEnd = this;
	if (!iAccessListRecentEnd)
		iAccessListRecentEnd = this;
	}

void CWsWindow::InsertIntoAccessListRecentEnd()
	{
	iAccessListNext = 0;
	iAccessListPrev = iAccessListRecentEnd;
	if (iAccessListPrev)
		iAccessListPrev->iAccessListNext = this;
	iAccessListRecentEnd = this;
	if (!iAccessListOldestEnd)
		iAccessListOldestEnd = this;
	}

void CWsWindow::RemoveFromAccessList()
	{
	if (iAccessListOldestEnd == this)
		iAccessListOldestEnd = iAccessListNext;
	if (iAccessListRecentEnd == this)
		iAccessListRecentEnd = iAccessListPrev;
	if (iAccessListPrev)
		iAccessListPrev->iAccessListNext = iAccessListNext;
	if (iAccessListNext)
		iAccessListNext->iAccessListPrev = iAccessListPrev;
	}
	
TBool CWsWindow::ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel)
	{
	for (CWsWindow * access = iAccessListOldestEnd; access; access = access->iAccessListNext)
		{
		if(access->Redraw())
			{
			if (access->Redraw()->ReleaseMemory(aLevel))
				return ETrue;
			}
		}
	return EFalse;
	}

const TRegion& CWsWindow::VisibleRegion() const
	{
	return(iVisibleRegion);
	}

const TRegion& CWsWindow::QuickFadeRegion() const
   	{
   	return iQuickFadeRegion;
   	}

const TRegion& CWsWindow::FadableRegion() const
	{
	return iFadableRegion;
	}

TBool CWsWindow::IsDSAHost() const
	{
	TBool res = EFalse; 
	if ( !iDSAs.IsEmpty() )
		{
		TSglQueIter<CWsDirectScreenAccess> iter( (TSglQueBase&)iDSAs );
		iter.SetToFirst();
		CWsDirectScreenAccess* dsa;
		while ( (dsa = iter++) != NULL && !res )
			{
			res = dsa->IsVisible();
			}
		}
	return res;
	}

void CWsWindow::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	aWindowTreeObserver.NodeCreated(*this, ParentNode());
	
	if(iFlags & EFlagActive)
		{
		aWindowTreeObserver.NodeExtentChanged(*this, FullRect());
		aWindowTreeObserver.NodeActivated(*this);
		}
	
	if(!IsVisible())
		{
		aWindowTreeObserver.FlagChanged(*this, MWsWindowTreeObserver::EVisible, EFalse);
		}
	
	if(IsTrackingVisibleRegion())
		{
		MWsWindowVisibilityNotifier* windowVisibilityNotifier = Screen()->WindowVisibilityNotifier();
		if(windowVisibilityNotifier)
			windowVisibilityNotifier->RegisterWindow(*this);
		}

	if(HasAlpha())
		{
		aWindowTreeObserver.FlagChanged(*this, MWsWindowTreeObserver::EAlphaChannelTransparencyEnabled, ETrue);
		}
	
	if(IsNonFading())
		{
		aWindowTreeObserver.FlagChanged(*this, MWsWindowTreeObserver::ENonFading, ETrue); 
		}
	
	if(IsTopClientWindow() && (iFlags&EFlagScreenDeviceInvalid) )
		{
		aWindowTreeObserver.FlagChanged(*this, MWsWindowTreeObserver::EScreenDeviceValid, EFalse);
		}
	
	if(iAnimList)
		iAnimList->SendState(aWindowTreeObserver);
	
	CWsWindowBase::SendState(aWindowTreeObserver);
	}

void CWsWindow::ResetVisibleRegion()
	{
	if (!iVisibleRegion.IsEmpty())
		{
		iVisibleRegion.Reset();
		}
	iFadableRegion.Reset();
	iQuickFadeRegion.Reset();
	}

void CWsWindow::SetVisibleRegion(const TRegion& aNewRegion, const TRegion* aTop)
	{
	WS_ASSERT_DEBUG(iScreen, EWsPanicNoScreen);
	STACK_REGION difference;
	TBool diffs = EFalse;

	difference.Copy(iVisibleRegion);	
	difference.SubRegion(aNewRegion);
	if (!difference.IsEmpty())
		{
		diffs = ETrue;
		if (IsTranslucent())
			{
			iScreen->AddRedrawRegion(difference, EFalse);	
			}
		}

	difference.Copy(aNewRegion);
	if (HasBeenDrawnToScreen())
		{
		difference.SubRegion(iVisibleRegion);
		}
	if (!difference.IsEmpty())
		{
		diffs = ETrue;
		if(!iScreen->ChangeTracking())
			{
			//the following code will restart animations
			STACK_REGION topDiff;
			topDiff.Copy(difference);
			WS_ASSERT_DEBUG(aTop,EWsPanicRegion);
			topDiff.Intersect(*aTop);
			difference.SubRegion(topDiff);
			iScreen->AddRedrawRegion(topDiff, EFalse, ERedrawTopOnly);
			iScreen->AddRedrawRegion(difference, EFalse, ERedrawAll);
			topDiff.Close();
			}
		else if(IsVisible())
			{
			RestartAnimations(aNewRegion);
			}
		}

	difference.Close();

	AbortDsaIfRequired(aNewRegion, aTop);

	if (diffs)
		{
		ResetVisibleRegion();
		iVisibleRegion.Copy(aNewRegion);
		PossibleVisibilityChangedEvent(EFalse);
		
		if (Redraw()->HasElement())
			{
			WS_ASSERT_DEBUG(WinType()==EWinTypeClient,EWsPanicWindowType);
			
			if (WinType()==EWinTypeClient)
				{
				iScreen->WindowElements().SetVisibleRegion(*static_cast<CWsClientWindow*>(this));
				}
			}
		}

	// Just because the visible region (screen coordinates) didn't change doesn't
	// mean the invalid region (window coordinates) didn't change, so we always call this.
	iRedraw->VisibleRegionChange();
	}

//This function sets up the quick fadable region.
//It removes anything that cannot be quick faded, and schedules it to be drawn in the normal fashion.
void CWsWindow::SetFadeableRegion(const TRegion& aNewFadableRegion, const TRegion& aTop)
	{
	WS_ASSERT_DEBUG(iScreen, EWsPanicNoScreen);
	iFadableRegion.Copy(aNewFadableRegion);

	//Try to figure out if any part of iFadableRegion can be quick faded (i.e. fading applied to 
	//the screen without first having to redraw all visible windows intersecting the region).
	if ( !iFadableRegion.IsEmpty() && iScreen->IsQuickFadeScheduled(this) )
		{
		if (IsTranslucent())
			{
			//If a window is semitransparent, then we cannot apply a quickfade to it if
			//the window below is faded too.
			iScreen->AddRedrawRegion(iVisibleRegion, EFalse, ERedrawAll);
			iScreen->RemoveFromQuickFadeList(this);
			}
		else
			{
			iQuickFadeRegion.Intersection(iFadableRegion, aTop);
			
			//Remove any regions not possible to quick fade from iQuickFadeRegion and
			//schedule these regions for full back-front rendering instead.
			STACK_REGION nonQuickFadableRegion;

			for(CWsSpriteBase * sprite = iSpriteList; sprite; sprite = sprite->Next())
				{
				nonQuickFadableRegion.AddRect(sprite->Rect());
				}

			for(CWsAnim * anim = iAnimList; anim; anim = anim->Next())
				{
				nonQuickFadableRegion.AddRect(anim->BestRect());
				}

			RWsTextCursor* const cursor = CWsTop::CurrentTextCursor();
			if( cursor && (cursor->Window()==this) && cursor->IsStandardCursorActive() )
				{
				nonQuickFadableRegion.AddRect(cursor->Rect());
				}

			//Any regions scheduled for fading but partly or fully covered by transparent windows above them
			STACK_REGION coveredFadableRegion;
			coveredFadableRegion.Copy(iFadableRegion);
			coveredFadableRegion.SubRegion(iQuickFadeRegion);
			nonQuickFadableRegion.Union(coveredFadableRegion);
			coveredFadableRegion.Close();

			nonQuickFadableRegion.Tidy();

			//Remove any regions not possible to quick fade from iQuickFadeRegion
			iQuickFadeRegion.SubRegion(nonQuickFadableRegion);

			if (!nonQuickFadableRegion.CheckError())
				{
				//Schedule normal drawing (full back to front rendering) for the region not possible to quick fade
				if (!nonQuickFadableRegion.IsEmpty())
					{ 
					iScreen->AddRedrawRegion(nonQuickFadableRegion, EFalse, ERedrawAll);
					}
				}
			else
				{
				//Schedule normal drawing for the whole iVisibleRegion if the calculations are broken
				iScreen->AddRedrawRegion(iVisibleRegion, EFalse, ERedrawAll);
				}
			nonQuickFadableRegion.Close();
			}
		}
	else
		{
		iQuickFadeRegion.Reset();
		}
	}

void CWsWindow::RestartAnimations(const TRegion& aNewRegion)
	{
	 //When not ChangeTracking, restarting is handled by AddRedrawRegion (called from CWsWindow::SetVisibleRegion) and TWalkWindowTreeScheduleRegions
	WS_ASSERT_DEBUG(iScreen->ChangeTracking(),EWsPanicNoChangetracking);
	
	//Restart uncovered window animations 
	for (CWsAnim* anim = iAnimList; anim; anim = anim->Next())
		{
		if(!iScreen->IsScheduled(EWindowAnim, anim->BestRect(), this) && aNewRegion.Intersects(anim->BestRect()))
			{
			iScreen->ScheduleAnimation(EWindowAnim, anim->BestRect(), 0, 0, 0, this);
			}
		}
	//Restart uncovered sprite animations 
	for (CWsSpriteBase* sprite = iSpriteList; sprite; sprite = sprite->Next())
		{
		if(!iScreen->IsScheduled(ESpriteAnim, sprite->Rect(), sprite->Win()) && aNewRegion.Intersects(sprite->Rect()))
			{
			iScreen->ScheduleAnimation(ESpriteAnim, sprite->Rect(), 0, 0, 0, sprite->Win());
			}
		}
	}

void CWsWindow::ClearVisibleRegion()
	{
	AbortAllDSAs();         
	iScreen->AddRedrawRegion(VisibleRegion(), EFalse);
	ResetVisibleRegion();
	PossibleVisibilityChangedEvent(EFalse);
	if (Redraw()->HasElement())
		{
		WS_ASSERT_DEBUG(WinType()==EWinTypeClient,EWsPanicWindowType);
		
		if (WinType()==EWinTypeClient)
			{
			iScreen->WindowElements().SetVisibleRegion(*static_cast<CWsClientWindow*>(this));
			}
		}
	iFlags &= ~EFlagDrawnToScreen;
	}

void CWsWindow::AbortDsaIfRequired(const TRegion& aNewRegion, const TRegion* aTop)
	{
	if (!iDSAs.IsEmpty())
		{
		// If the top visible region of this window has changed, DSA clients may need
		// to be sent a DSA abort, as they may be drawing to a different region
		STACK_REGION newTopVisible;
		newTopVisible.Copy(aNewRegion); // new visible region
		if (aTop!=NULL)
			{
			newTopVisible.Intersect(*aTop); // area of new visible region not obscured by any other opaque or translucent windows
			}
		// Build a list of DSA clients that need to be sent a DSA abort
		TSglQue<CWsDirectScreenAccess> dsaList(_FOFF(CWsDirectScreenAccess,iAbortLink));
		TSglQueIter<CWsDirectScreenAccess> iter(iDSAs);
		CWsDirectScreenAccess* dsa;
		while ((dsa=iter++)!=NULL)
			{
			if (dsa->IsAbortRequired(newTopVisible))
				{
				dsaList.AddLast(*dsa);
				}
			}
		if (!dsaList.IsEmpty())
			{
			iScreen->AbortDSAs(RDirectScreenAccess::ETerminateRegion, dsaList);
			}
		newTopVisible.Close();
		}
	}

const TRegion* CWsWindow::VisibleRegionIfValid() const
	{
	return iVisibleRegion.CheckError() ? NULL : &iVisibleRegion;
	}
	
TBool CWsWindow::ReadyToDraw() const
	{
	return iRedraw->ReadyToDraw();
	}

/**
This function draws the region specified
*/
void CWsWindow::Render(MWsGraphicsContext& aGc, const TRegion& aWindowRegion, const TRegion& aWindowChildNodeRegion)
	{
	WS_ASSERT_DEBUG(IsVisible() || (WinType() == EWinTypeRoot), EWsPanicScheduledRedraw);

	AnnotateWindowRedrawStart(*this, aWindowRegion);
	
	Accessed();
	iFlags |= EFlagDrawnToScreen;
	
	aGc.Reset();
	iRedraw->PreDrawWindow(&aGc, aWindowRegion);
	iRedraw->DrawWindow();
	iRedraw->PostDrawWindow(&aGc, aWindowChildNodeRegion);
	}

void CWsWindow::SetNextScheduled(CWsWindow * aWin)
	{
	iNextScheduled = aWin;
	}
	
CWsWindow * CWsWindow::NextScheduled()  const
	{
	return iNextScheduled;
	}

void CWsWindow::DeactivateAllSprites()
	{
	CWsSpriteBase * current = iSpriteList;
	while (current)
		{
		CWsSpriteBase * next = current->Next();
		current->Deactivate();
		current = next;
		}
	}

void CWsWindow::ClipRectToViewport(TRect& aRect) const
	{
	const CWsWindowBase * win = this;
	while (win)
		{
		if (win->WinType() != EWinTypeGroup)
			{
			aRect.Intersection(win->AbsRect());
			}
		win = win->BaseParent();
		}
	}

void CWsWindow::AddDirtyWindowRegion(const TRegion& aRegion)
	{
	iDirtyWindowRegion.Union(aRegion);
	if (iDirtyWindowRegion.CheckError())
		{
		iDirtyWindowRegion.Reset();
		iDirtyWindowRegion.AddRect(AbsRect()); // fallback to potentially visible part of window
		}
	}

const TRegion& CWsWindow::DirtyWindowRegion() const
	{
	return iDirtyWindowRegion;
	}

void CWsWindow::ScheduleDirtyWindowRegion()
	{
	iDirtyWindowRegion.Tidy();
	iDirtyWindowRegion.Offset(iOrigin); //convert to screen coords 
	iScheduledRegion.Copy(iDirtyWindowRegion);
	iDirtyWindowRegion.Reset();
	if (iScheduledRegion.CheckError())
		{
		iScheduledRegion.Reset();
		iScheduledRegion.AddRect(AbsRect()); // fallback to potentially visible part of window
		}
	}

void CWsWindow::AddDirtySpriteRegion(const TRegion& aRegion)
	{
	iDirtySpriteRegion.Union(aRegion);
	if (iDirtySpriteRegion.CheckError())
		{
		iDirtySpriteRegion.Reset();
		iDirtySpriteRegion.AddRect(AbsRect()); // fallback to potentially visible part of window
		}
	}

const TRegion& CWsWindow::DirtySpriteRegion() const
	{
	return iDirtySpriteRegion;
	}

void CWsWindow::ScheduleDirtySpriteRegion()
	{
	iDirtySpriteRegion.Tidy();
	iDirtySpriteRegion.Offset(iOrigin); //convert to screen coords 
	iScheduledSpriteRegion.Copy(iDirtySpriteRegion);
	iDirtySpriteRegion.Reset();
	if (iScheduledSpriteRegion.CheckError())
		{
		iScheduledSpriteRegion.Reset();
		iScheduledSpriteRegion.AddRect(AbsRect()); // fallback to potentially visible part of window
		}
	}

void CWsWindow::ClearScheduledSpriteRegion()
	{
	iScheduledSpriteRegion.Reset();
	}

/**
In ChangeTracking mode, wserv is not maintaining the visible region of windows.
Windows that need to keep track of their visible region in ChangeTracking mode 
should call this function.

@param aRegister 	ETrue if iVisibleRegion should be maintained with accurate information, 
					EFalse if not.
					
@internalComponent
*/
void CWsWindow::SetupVisibleRegionTracking(TBool aRegister)
	{
	if (aRegister)
		{
		++iVisibleRegionTrackingCounter;
		if (iVisibleRegionTrackingCounter==1)
			{
			//If visible region tracking has not been setup, let's do it now.
			iScreen->SetupVisibleRegionTracking(*this, ETrue);
			}
		}
	else if (iVisibleRegionTrackingCounter>0)
		{
		--iVisibleRegionTrackingCounter;
		if (iVisibleRegionTrackingCounter==0)
			{
			//If aReason was the last reason to track visibility, disable further notifications.
			iScreen->SetupVisibleRegionTracking(*this, EFalse);
			}
		}
	}

/**
MWsWindow
*/
MWsWindow * CWsWindow::FindChildByHandle(TUint32 aHandle)
	{
	TWalkWindowTreeFindByHandle wwt(aHandle);
	WalkWindowTree(wwt, EWalkChildren);
	return wwt.Found();
	}

TUint32 CWsWindow::Handle() const
	{
	return ClientHandle();
	}

MWsScreen * CWsWindow::WsScreen() const
	{
	return iScreen;
	}

TInt CWsWindow::OrdinalPriority() const
	{
	return iOrdinalPriority;
	}
