// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

// Window server redraw queue handling
//
#include <e32std.h>
#include "server.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "walkwindowtree.h"
#include "EVQUEUE.H"
#include "panics.h"
#include "wstop.h"

const TInt KGranularity = 10;

void TWsRedrawEvent::SetHandle(TUint aHandle)
	{
	iHandle=aHandle;
	}

void TWsRedrawEvent::SetRect(TRect aRect)
	{
	iRect=aRect;
	}

TWsRedrawEvent CRedrawQueue::iNullRedrawEvent;

CRedrawQueue::CRedrawQueue(CWsClient *aOwner) : CEventBase(aOwner)
	{
	__DECLARE_NAME(_S("CRedrawQueue"));
	}

void CRedrawQueue::ConstructL()
	{
	iRedrawTrigger=EFalse;
	iRedraws=new(ELeave) CArrayFixSeg<TRedraw>(KGranularity);
	iKeyPriority=new(ELeave) TKeyArrayFix(_FOFF(TRedraw,iPriority),ECmpTUint32);
	iKeyWindow=new(ELeave) TKeyArrayFix(_FOFF(TRedraw,iRedraw),ECmpTUint32);
	Mem::FillZ(&iNullRedrawEvent,sizeof(iNullRedrawEvent));
	}

CRedrawQueue::~CRedrawQueue()
	{
	delete iRedraws;
	delete iKeyPriority;
	delete iKeyWindow;
	}

void CRedrawQueue::ReCalcOrder()
	{
	const TInt redrawsCount=iRedraws->Count();
	for(TInt index=0;index<redrawsCount;index++)
		{
		TRedraw *redraw=&iRedraws->At(index);
		redraw->iPriority=static_cast<CWsClientWindow*>(redraw->iRedraw->WsWin())->RedrawPriority();
		}
	iRedraws->Sort(*iKeyPriority);
	}

void CRedrawQueue::AddInvalid(CWsWindowRedraw *aRedrawWin) 
//
// Add a window to the update list.
//
	{
	TInt index;
	TRedraw redraw;

	redraw.iRedraw=aRedrawWin;
	if (iRedraws->Find(redraw,*iKeyWindow,index)!=0)
		{
		redraw.iPriority=static_cast<CWsClientWindow*>(aRedrawWin->WsWin())->RedrawPriority();
		TRAPD(err,iRedraws->InsertIsqAllowDuplicatesL(redraw,*iKeyPriority));
		if (err!=KErrNone)
			{
			WS_ASSERT_DEBUG(err==KErrNoMemory,EWsPanicRedrawQueueError);
			iAllocError=ETrue;
			}
		iRedrawTrigger=ETrue;
		}
	}

void CRedrawQueue::DeleteFromQueue(TInt aIndex)
	{
	iRedraws->Delete(aIndex,1);

	//We are certain we will need iRedraws again, so it would be silly to compress away the last KGranularity slots.
	const TInt count = iRedraws->Count();
	if((count >= KGranularity) && (count % KGranularity == 0))
		{
		iRedraws->Compress();
		}
	}

void CRedrawQueue::RemoveInvalid(CWsWindowRedraw *aRedrawWin)
//
// remove the window from the invalid window list.
// harmless to call if the window is not in the list.
//
	{
	TInt index;
	TRedraw redraw;

	redraw.iRedraw=aRedrawWin;
	redraw.iPriority=0;
	if ((iRedraws->Find(redraw,*iKeyWindow,index))==KErrNone)
		DeleteFromQueue(index);
	}

TBool CRedrawQueue::TriggerRedraw()
//
// Trigger any pending redraw messages in the queue
// Returns ETrue if a redraw was sent, EFalse if not.
//
	{
	TBool ret=EFalse;
	if (iRedrawTrigger)
		{
		iRedrawTrigger=EFalse;
		if (!iEventMsg.IsNull() && (iRedraws->Count()>0 || iAllocError))
			{
			SignalEvent();
			ret=ETrue;
			}
		}
	return(ret);
	}

void CRedrawQueue::EventReady(const RMessagePtr2& aEventMsg)
//
// Queue a read of an event from the queue
//
	{
	CEventBase::EventReady(aEventMsg);
	iRedrawTrigger=ETrue;
	TriggerRedraw();
	}

TBool CRedrawQueue::FindOutstandingRedrawEvent(CWsWindowRedraw& aRedraw, TWsRedrawEvent& aEvent)
	{
	TRect rect;
	if (aRedraw.GetRedrawRect(rect))
		{
		aEvent.SetRect(rect);
		aEvent.SetHandle(aRedraw.WsWin()->ClientHandle());
		CEventBase::GetData(&aEvent,sizeof(aEvent));
		return ETrue;
		}
	return EFalse;
	}

TBool CRedrawQueue::FindWindowNeedingRedrawEvent(TWsRedrawEvent& aEvent)
	{
#if defined(_DEBUG)
	CWsWindowRedraw* previousRedraw = NULL;
#endif
	// search all screens
	TInt invalidWindows = 0;
	for (TInt screenNo = 0; screenNo < CWsTop::NumberOfScreens(); ++screenNo)
		{
		const CScreen* screen = CWsTop::Screen(screenNo);
		WS_ASSERT_ALWAYS(screen, EWsPanicNoScreen);
		CWsRootWindow* rootWindow = screen->RootWindow();
		for (CWsWindowGroup *groupWin = rootWindow->Child(); groupWin; groupWin = groupWin->NextSibling())
			{
			if (groupWin->WsOwner() == iWsOwner)
				{
				CWsWindowRedraw* redrawWin = NULL;
				// use a window tree walk that can be resumed to find windows with an invalid region
				TResumableWalkWindowTreeFindInvalid wwt(&redrawWin);
				groupWin->WalkWindowTree(wwt, EWalkChildren, EFalse);
				while (redrawWin != NULL)
					{
					WS_ASSERT_DEBUG(redrawWin != previousRedraw, EWsPanicRedrawQueueError);
					// (the window may not actually need the client to redraw it, e.g. a CWsBlankWindow can redraw itself)
					if (FindOutstandingRedrawEvent(*redrawWin, aEvent))
						{
						return ETrue;
						}
					else
						{ // continue the Tree Walk
#if defined(_DEBUG)
						previousRedraw = redrawWin;
#endif
						if (redrawWin->NeedsRedraw())
							{ // needs to be redrawn later?
							++invalidWindows;
							}
						redrawWin = NULL;
						groupWin->WalkWindowTree(wwt, EWalkChildren, ETrue);
						}
					}
				}
			}
		}

	if (invalidWindows == 0)
		{ // error recovery is complete
		iAllocError = 0;
		}
	return EFalse;
	}

void CRedrawQueue::GetData()
//
// If there is an outstanding redraw event in the queue, reply with it's data
//
	{
	CWsWindowRedraw *redraw;
	TWsRedrawEvent event;

	while (iRedraws->Count()>0)
		{
		redraw=(*iRedraws)[0].iRedraw;
		if (FindOutstandingRedrawEvent(*redraw,event))
			{
			return;
			}
		TInt toDelete=0;
		if (redraw!=(*iRedraws)[0].iRedraw)
			{		//In low memory conditions calls to FindOutstandingRedrawEvent can cause extra entries to be added to the array
			TRedraw redrawFind;
			redrawFind.iRedraw=redraw;
			iRedraws->Find(redrawFind,*iKeyWindow,toDelete);
			}
		DeleteFromQueue(toDelete);
		}

	if (iAllocError && FindWindowNeedingRedrawEvent(event))
		{
		return;
		}

	CEventBase::GetData(&iNullRedrawEvent,sizeof(iNullRedrawEvent));
	}

TUint CRedrawQueue::RedrawPriority(CWsWindowRedraw *aRedrawWin)
	{
	TInt index;
	TRedraw redraw;
	TUint priority=0;
	redraw.iRedraw=aRedrawWin;

	if ((iRedraws->Find(redraw,*iKeyWindow,index))==KErrNone)
		{
		priority=iRedraws->At(index).iPriority;
		}
	return priority;
	}
