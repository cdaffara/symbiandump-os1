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
// Root window sub-class of CWsWindow
// 
//

#include "rootwin.h"
#include <e32std.h>
#include <graphics/wsscreendevice.h>
#include "server.h"
#include "windowgroup.h"
#include "walkwindowtree.h"
#include "wstop.h"
#include "ScrDev.H"
#include "panics.h"
#include "inifile.h"
#include "EVENT.H"

_LIT(KNoBlank,"NOBLANKSCREEN");
_LIT(KDefaultBackgroundColor,"BACKGROUNDCOLOR");
_LIT(KDefaultBackgroundAlpha,"BACKGROUNDALPHA");
_LIT(KRootWinDefaultBackgroundColor,"ROOTBACKGROUNDCOLOR");

CWsRootWindow::CWsRootWindow(CWsClient* aOwner, CScreen* aScreen) : CWsWindow(aOwner,WS_HANDLE_ROOT_WINDOW,aScreen)
	{
	iWinType=EWinTypeRoot;
	}

CWsRootWindow::~CWsRootWindow()
	{
	if (Screen() && (iBaseWinFlags&EBaseWinNodeCreated))
		{
		MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
		if (windowTreeObserver)
			{
			windowTreeObserver->NodeReleased(*this);
			iBaseWinFlags &= ~EBaseWinNodeCreated;
			}
		}
	Shutdown();
	}

void CWsRootWindow::ConstructL()
	{
	CWsWindow::Construct();
	iParent=NULL;
	iSibling=NULL;
	iChild=NULL;
	iClientHandle=NULL;
	iAbs.Resize(iScreen->SizeInPixels());
	iRel=iAbs;
	iFlags=EFlagPointerCaptured;
	iPointerFilter=EPointerFilterEnterExit|EPointerFilterMove|EPointerFilterDrag;
	iArea.AddRect(iAbs);
	iRedraw=new(ELeave) CWsBlankWindow(this);
	iRedraw->ConstructL();
	TInt backgroundcolor;
	if(!WsIniFile->FindVar(KDefaultBackgroundColor,backgroundcolor))
		backgroundcolor = KRgbWhite.Value();
	TInt backgroundalpha;
	if(!WsIniFile->FindVar(KDefaultBackgroundAlpha,backgroundalpha))
		backgroundalpha = 0xFF;	
	iDefaultBackgroundColor = TRgb(backgroundcolor,backgroundalpha);
	
	if (WsIniFile->FindVar(KNoBlank))
		BlankRedraw()->SetBackgroundClear();
	else
		{
		if (WsIniFile->FindVar(KRootWinDefaultBackgroundColor,backgroundcolor))
			SetColor(TRgb(backgroundcolor,backgroundalpha));
		else
			SetColor(iDefaultBackgroundColor);
		}

	MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
	if (windowTreeObserver)
		{
		windowTreeObserver->NodeCreated(*this, NULL);
		iBaseWinFlags |= EBaseWinNodeCreated;
		}
	}

void CWsRootWindow::SetColor(TRgb aColor)
	{
	BlankRedraw()->SetColor(aColor);
	}

void CWsRootWindow::SetColorIfClear()
	{
	if (!BlankRedraw()->IsBackgroundColor())
		SetColor(iDefaultBackgroundColor);
	}

const CWsWindow *CWsRootWindow::PointerWindow(const TPoint &aInPos,TPoint *aOutPos, TPoint *aParentPos,
					const CWsWindow *aGrabWin, const CWsWindow *&aOriginalWinItIsIn, const CWsWindowGroup *aForceInGroup)
//
// For aInPos (a global point on screen) find which window it is in, starting the search from 'this'.
// aOutPos is set to be aInPos adjusted relative to the top left of the result window.
// If the pointer is not in any of the searched windows the result is returned as though it was in 'this'
// even though it may actually be oustside the bounds of this.
//
// If aForceInGroup==NULL search all groups otherwise only search it only
//
	{
	aOriginalWinItIsIn=this;
	const CWsWindowGroup *group;
	const CWsWindowGroup *winItIsInGroup=aForceInGroup;
//
// First determine owner of the window the event is in regardless of any capture
// This is so we can decide whether the capture affects this case or not
//
	for(group=(aForceInGroup ? aForceInGroup:Child());group!=NULL;group=group->NextSibling())
		{
		CWsClientWindow *win=group->Child();
		while(win!=NULL)
			{
			const TRegion *baseArea=win->BaseArea();
			if (win->IsVisible() && baseArea->Contains(aInPos))
				{
				aOriginalWinItIsIn=win;
				winItIsInGroup=group;
				win=win->Child();
				}
			else
				win=win->NextSibling();
			}
		if (aOriginalWinItIsIn!=this || aForceInGroup!=NULL)
			break;
		}
//
// Then try again taking note of any pointer capture or grab
//
	const CWsWindow *winItIsIn;
	if (aGrabWin!=NULL)
		winItIsIn=aGrabWin;
	else
		{
		winItIsIn=this;
		for(group=(aForceInGroup ? aForceInGroup:Child());group!=NULL;group=group->NextSibling())
			{
			CWsClientWindow *win=group->Child();
			while(win!=NULL)
				{
				const TRegion *baseArea=win->BaseArea();
				const TBool underTheSameGroup=winItIsInGroup==group;
				if (win->IsVisible() && 
					((win->iFlags&EFlagPointerCaptured && 
					 ((!underTheSameGroup && win->iFlags&EFlagPointerCaptureAllGroups) || 
					  (winItIsInGroup==NULL && group==CWsTop::FocusWindowGroup()) || 
					  (underTheSameGroup && win->iPointerCapturePriority>=aOriginalWinItIsIn->iPointerCapturePriority)))
					   || baseArea->Contains(aInPos)))
					{
				 	winItIsIn=win;
					win=win->Child();
					}
				else
					win=win->NextSibling();
				}
			if (winItIsIn!=this || aForceInGroup!=NULL)
				break;
			}
		}
	if (aOutPos!=NULL)
		{
		*aOutPos=aInPos-winItIsIn->iOrigin;
		}
	if (aParentPos!=NULL)
		{
		const CWsWindowBase *win=winItIsIn->BaseParent();
		if (win==NULL)
			win=winItIsIn;
		*aParentPos=aInPos-win->Origin();
		}
	return(winItIsIn);
	}

void CWsRootWindow::GenerateWindowRegion(RWsRegion &aRegion) const
	{
	aRegion.Clear();
	aRegion.AddRect(iAbs);
	if (iChild)
		{
		for(CWsTopClientWindow *win=FirstTopClientWindow();aRegion.Count()>0 && win!=NULL;win=win->NextSiblingMultiParent())
			{
			if (win->IsVisible())
				aRegion.SubRegion(*win->BaseArea());
			}
		}
	}

void CWsRootWindow::CommandL(TInt , const TAny *)
	{
	WS_PANIC_ALWAYS(EWsPanicRootCommand);
	}

void CWsRootWindow::InvalidateWholeScreen()
	{
	RWsRegion screen(iAbs);
	Invalidate(&screen);
	screen.Close();
	}

void CWsRootWindow::Invalidate(RWsRegion* aRegion)
	{
	iScreen->AddRedrawRegion(*aRegion);
	}

void CWsRootWindow::AdjustCoordsDueToRotation()
	{
	iArea.Clear();
	iAbs=iScreen->DrawableArea();
	iRel=iAbs;
	iArea.AddRect(iAbs);
	iOrigin=iAbs.iTl;
	for(CWsTopClientWindow *win=FirstTopClientWindow();win!=NULL;win=win->NextSiblingMultiParent())
		{
		win->RecalcChildAbs(NULL);
		}
	}

void CWsRootWindow::ClearDisplay()
	{
	MWsScreenDevice *sd=static_cast<MWsScreenDevice*>(iScreen->ResolveObjectInterface(KMWsScreenDevice));
	WS_ASSERT_ALWAYS(sd, EWsPanicScreenDeviceMissing); 
	sd->ClearDisplay(BackColor());	
	}

void CWsRootWindow::SetSystemFaded(TBool aFaded, TUint8 aBlackMap, TUint8 aWhiteMap)
	{
	WS_ASSERT_DEBUG(Screen(),EWsPanicNoScreen);
	
	TBool stateChanged = EFalse; //will be set to true if one or more windows change fade state.
	for(CWsWindowGroup* win=Child();win!=NULL;win=win->NextSibling())
		{
		TWalkWindowTreeSetSystemFaded wwt(aFaded, win, aBlackMap, aWhiteMap, stateChanged);
		win->WalkWindowTree(wwt,EWalkChildren);
		}

	MWsWindowTreeObserver* windowTreeObserver = Screen()->WindowTreeObserver();
	if(windowTreeObserver && stateChanged)
		{
		windowTreeObserver->FadeAllChildren(*this, aFaded);
		}
	}

CWsWindowGroup* CWsRootWindow::WindowGroup(TInt aWindowGroup)
	{
	CWsWindowBase* group=iChild;
	while (aWindowGroup-->0 && group)
		group=group->NextSibling();
	return static_cast<CWsWindowGroup*>(group);
	}

CWsTopClientWindow *CWsRootWindow::FirstTopClientWindow() const
	{
	CWsWindowGroup* group;
	for(group=Child();group && group->Child()==NULL;group=group->NextSibling())
		{}
	return(group?group->Child():NULL);
	}

const TRegion& CWsRootWindow::WindowArea() const
	{
	return iArea;
	}

void CWsRootWindow::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	aWindowTreeObserver.NodeCreated(*this, NULL);
	}

/** @see MWsWindowTreeNode */
const MWsWindow* CWsRootWindow::Window() const
	{
	return NULL;
	}

