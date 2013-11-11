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
#include "offscreenbitmap.h"

CWsWindow* CWsWindow::iAccessListRecentEnd = 0;
CWsWindow* CWsWindow::iAccessListOldestEnd = 0;

CWsWindow::CWsWindow(CWsClient* aOwner,WH_HANDLES aType, CScreen* aScreen) : CWsWindowBase(aOwner,aType,aScreen),
	iFlags(EFlagHidden),
	 iDSAs(_FOFF(CWsDirectScreenAccess,iMultipleDSALink))
	{
	}

CWsWindow::~CWsWindow()
	{
	iFadableRegion.Close();
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
	iVisibleRegion.Close();
	iScheduledRegion.Close();
	AbortAllDSAs();
	CWsWindowBase::Shutdown();
	SetPointerCursor(NULL);
	delete iRedraw;
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

void CWsWindow::SetNonFading(TBool aNonFade)
	{
	if (aNonFade)
		iFlags|=EFlagNonFadingWindow;
	else
		iFlags&=(~EFlagNonFadingWindow);
	}

TPoint CWsWindow::InquireOffset(TUint32 aHandle) const
	{
	CWsWindowBase *win2;
	iWsOwner->HandleToWindow(aHandle,&win2);
	return(iOrigin-win2->Origin());
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
	aBuf.AppendFormat(KStatusString3,(TInt)iScreen->DisplayMode());
	if (!IsVisible())
		aBuf.Append(KStatusInvisible);
	}

TDblQue<TPointerKeyList> *CWsWindow::PointerKeyList() const
	{
	return(NULL);
	}

void CWsWindow::AbortAllDSAs()
	{
	iScreen->AbortDSAs(RDirectScreenAccess::ETerminateRegion,iDSAs);
	}

void CWsWindow::PossibleVisibilityChangedEvent(TBool aForceSend)
	{
	// notify plugin
	//
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

TBool CWsWindow::SetScheduledRegion(const RWsRegion& aRegion)
	{
	iScheduledRegion.Copy(aRegion);
	return !iScheduledRegion.CheckError();
	}

void CWsWindow::ClearScheduledRegion()
	{
	iScheduledRegion.Clear();
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
			res = dsa->IsVisible() || dsa->IsSyncTimeoutPending();
			}
		}
	return res;
	}

void CWsWindow::ResetVisibleRegion()
	{
	if (!iVisibleRegion.IsEmpty())
		{
		iVisibleRegion.Clear();
		}
	iFadableRegion.Clear();
	}

void CWsWindow::SetVisibleRegion(const TRegion& aNewRegion, const TRegion* aTop, TRegion& aNewFadableRegion)
	{
	STACK_REGION difference;
	TBool diffs = EFalse;

	difference.Copy(iVisibleRegion);
	difference.SubRegion(aNewRegion);
	if (!difference.IsEmpty())
		{
		diffs = ETrue;
		if (IsTranslucent())
			{
			// Andy - If this is a client window (what else could it be) we can also subtract the
			// user defined opaque region before doing this:
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
		STACK_REGION topDiff;
		topDiff.Copy(difference);
		WS_ASSERT_DEBUG(aTop,EWsPanicRegion);
		topDiff.Intersect(*aTop);
		difference.SubRegion(topDiff);
		iScreen->AddRedrawRegion(topDiff, EFalse, ERedrawTopOnly);
		iScreen->AddRedrawRegion(difference, EFalse, ERedrawAll);
		topDiff.Close();
		}

	difference.Close();

	AbortDsaIfRequired(aNewRegion, aTop);

	if (diffs)
		{
		ResetVisibleRegion();
		iVisibleRegion.Copy(aNewRegion);
		PossibleVisibilityChangedEvent(EFalse);
		}

	iFadableRegion.Copy( aNewFadableRegion );
	
	// Just because the visible region (screen coordinates) didn't change doesn't
	// mean the invalid region (window coordinates) didn't change, so we always call this.
	iRedraw->VisibleRegionChange();
	}
	
void CWsWindow::ClearVisibleRegion()
	{
	AbortAllDSAs();         
	iScreen->AddRedrawRegion(VisibleRegion(), EFalse);
	ResetVisibleRegion();
	PossibleVisibilityChangedEvent(EFalse);
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
Andy - I would be happier if no region was specified here - the GC should incorporate that information
*/
void CWsWindow::Render(CFbsBitGc * aGc, const TRegion& aRegion)
	{
	WS_ASSERT_DEBUG(IsVisible() || (WinType() == EWinTypeRoot), EWsPanicScheduledRedraw);
	
	Accessed();
	iFlags |= EFlagDrawnToScreen;
	
	iRedraw->PreDrawWindow(aGc, aRegion);
	iRedraw->DrawWindow();
	iRedraw->PostDrawWindow(aGc);
	}

void CWsWindow::SetNextScheduled(CWsWindow * aWin)
	{
	iNextScheduled = aWin;
	}
	
CWsWindow * CWsWindow::NextScheduled()  const
	{
	return iNextScheduled;
	}

void CWsWindow::AddSprite(CWsSpriteBase * aSprite)
	{
	aSprite->SetNext(iSpriteList);
	iSpriteList = aSprite;
	}
	
void CWsWindow::RemoveSprite(CWsSpriteBase * aSprite)
	{
	if (aSprite == iSpriteList)
		{
		iSpriteList = aSprite->Next();
		}
	else
		{
		for (CWsSpriteBase * sprite = iSpriteList; sprite; sprite = sprite->Next())
			{
			if (sprite->Next() == aSprite)
				{
				sprite->SetNext(aSprite->Next());
				}
			}
		}
	aSprite->SetNext(0);
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

const TRegion& CWsWindow::FadableRegion() const
	{
	return iFadableRegion;
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
