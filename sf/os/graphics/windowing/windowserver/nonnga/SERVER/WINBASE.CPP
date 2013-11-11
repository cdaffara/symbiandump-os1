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
// Window virtual base class, windows and window groups are derived from this
// 
//

#include <e32std.h>
#include "server.h"
#include "winbase.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "walkwindowtree.h"
#include "wstop.h"
#include "EVQUEUE.H"
#include "EVENT.H"
#include "panics.h"
#include "pointer.h"

CWsWindowBase::CWsWindowBase(CWsClient* aOwner,WH_HANDLES aType, CScreen* aScreen) : CWsScreenObject(aOwner,aType,aScreen)		
	{
	}

void CWsWindowBase::ConstructL(CWsWindowBase *parent)
	{
	iParent=parent;
	iSibling=parent->iChild;
	parent->iChild=this;
	SetOrdinalPosition(0);
	iFadeCount = iParent->iFadeCount;
	}

CWsWindowBase *CWsWindowBase::GetPrevSibling() const
	{
	CWsWindowBase **prev= &iParent->iChild;
	CWsWindowBase *ret=NULL;
	while ((*prev)!=this)
		{
		ret=*prev;
		prev= &(*prev)->iSibling;
		}
	return(ret);
	}

CWsWindowBase *CWsWindowBase::LastSibling() const
	{
	const CWsWindowBase *win;
	for(win=this;win->iSibling;win=win->iSibling)
		{}
	return (CWsWindowBase*)win;
	}

CWsWindowBase *CWsWindowBase::PrevSiblingMultiParent() const
	{
	CWsWindowBase *win=GetPrevSibling();
	if (win)
		return(win);
	for(CWsWindowBase *parent=iParent->GetPrevSibling();parent;parent=parent->GetPrevSibling())
		if ((win=parent->iChild)!=NULL)
			return(win->LastSibling());
	return(NULL);
	}

// Andy - this is a kind of "next cousin" function
CWsWindowBase *CWsWindowBase::NextSiblingMultiParent() const
	{
	if (iSibling)
		return(iSibling);
	for(CWsWindowBase *parent=iParent->iSibling;parent;parent=parent->iSibling)
		{
		if (parent->iChild!=NULL)
			return(parent->iChild);
		}
	return(NULL);
	}

TInt CWsWindowBase::OrdinalPosition(TBool aFull) const
	{
	if (!iParent)
		{
		OwnerPanic(EWservPanicParentDeleted);
		}
	CWsWindowBase *win=iParent->iChild;
	if (!aFull)
		while(iOrdinalPriority<win->iOrdinalPriority)
			win=win->iSibling;
	TInt count;
	for(count=0;win!=this;count++)
		win=win->iSibling;
	return(count);
	}

/** Removes a window from the list of siblings maintained by its parent window.

The iSibling stored inside the window we remove is kept unchanged as it may be needed later.

@internalComponent
@released
*/
void CWsWindowBase::RemoveFromSiblingList()
	{
	if (iParent!=NULL)
		{
		CWsWindowBase **prev= &iParent->iChild;
		while ((*prev)!=this)
			prev= &(*prev)->iSibling;
		*prev=iSibling;
		}
	}

/* This const casts in this function are horrible and need revisiting. */
CWsWindowGroup *CWsWindowBase::WinGroup() const
	{
	switch (iWinType)
		{
		case EWinTypeClient:
			{
			if (iParent)
				{
				CWsWindowBase* win = const_cast<CWsWindowBase*>(this);
				while(win->WinType()!=EWinTypeGroup)
					win=win->BaseParent();
				return static_cast<CWsWindowGroup*>(win);
				}
			return 0;
			}
		case EWinTypeGroup:
			return const_cast<CWsWindowGroup*>(static_cast<const CWsWindowGroup*>(this));
		default:
			return 0;
		}
	}

TBool CWsWindowBase::CheckOrdinalPositionChange(TInt aPos)
//
// This routine checks to see whether the specified new ordinal position
// will causes a change, if so returns ETrue else EFalse.
//
	{
	if (!iParent)
		{
		OwnerPanic(EWservPanicParentDeleted);
		}
	CWsWindowBase *win= iParent->iChild;
	CWsWindowBase *prev= NULL;
	while(win==this || (win!=NULL && iOrdinalPriority<win->iOrdinalPriority))
		{
		prev=win;
		win=win->iSibling;
		}
	if (prev==this)
		win=this;
	else if (win==NULL || (win->iSibling==this && iOrdinalPriority>win->iOrdinalPriority))
		return ETrue;
	while(aPos--!=0 && win->iSibling!=NULL && iOrdinalPriority==win->iSibling->iOrdinalPriority)
		win=win->iSibling;
	return(win!=this);
	}

void CWsWindowBase::ChangeWindowPosition(TInt aPos,CWsWindowBase* aNewParent)
	{
	if (aNewParent != iParent)
		{
		iScreen->ScheduleRegionUpdate(NULL);		
		TWalkWindowTreeScheduleRedraws wwt;
		WalkWindowTree(wwt, EWalkChildren);
		}
	else if (WinType() == EWinTypeClient)
		{
		CWsClientWindow * cliwin = static_cast<CWsClientWindow*>(this);
		if (cliwin->IsVisible())
			{
			iScreen->ScheduleRegionUpdate(NULL);
			if (cliwin->IsTranslucent())
				{
				// There is still room for optimization here.  These redraws are only required if the window
				// moved through another window and BOTH of them were transparent, otherwise the visible
				// region change will sort out the redraws required.
				TWalkWindowTreeScheduleRedraws wwt;
				WalkWindowTree(wwt, EWalkChildren);
				}
			}
		}
	else if (WinType() == EWinTypeGroup)
		{
		iScreen->ScheduleRegionUpdate(NULL);
		if (static_cast<CWsWindowGroup*>(this)->HasVisibleTranslucentChild())
			{
			TWalkWindowTreeScheduleRedraws wwt;
			WalkWindowTree(wwt, EWalkChildren);				
			}
		}

	RemoveFromSiblingList();
	CWsWindowBase **prevWinPtr= &aNewParent->iChild;
	while((*prevWinPtr)!=NULL && iOrdinalPriority<(*prevWinPtr)->iOrdinalPriority)
		{
		prevWinPtr= &(*prevWinPtr)->iSibling;
		}
	while(aPos--!=0 && *prevWinPtr!=NULL && iOrdinalPriority==(*prevWinPtr)->iOrdinalPriority)
		{
		prevWinPtr= &(*prevWinPtr)->iSibling;
		}
	iSibling=*prevWinPtr;
	iParent=aNewParent;
	*prevWinPtr=this;
	}

void CWsWindowBase::SetOrdinalPosition(TInt aPos)
	{
	if (CheckOrdinalPositionChange(aPos))
		ChangeWindowPosition(aPos,iParent);
	}

TEventQueueWalkRet EventPurgeFunc(TAny *aPtr, TWsEvent *aEvent)
//
// Callback function for event queue walk
//
	{
	return(((CWsWindowBase *)aPtr)->EventPurgeCheck(aEvent));
	}

TEventQueueWalkRet CWsWindowBase::EventPurgeCheck(TWsEvent *aEvent)
	{
	if (aEvent->Handle()==ClientHandle())
		return(EEventQueueWalkDeleteEvent);
	return(EEventQueueWalkOk);
	}

void CWsWindowBase::PurgeEvents()
	{
	iWsOwner->EventQueue()->WalkEventQueue(&EventPurgeFunc,this);
	}

void CWsWindowBase::Shutdown()
//
// Destroy a window, disconnects from the window tree and destroys all it's child windows
//
	{
	if (iWsOwner!=NULL)
		PurgeEvents();
	if (iParent!=NULL)	// Check it's connected to something
		{
		CWsWindowBase *win;
		for(win=this;win && win->iParent!=(CWsWindowBase *)RootWindow();win=win->iParent)
			{}
		RemoveFromSiblingList();
		TWalkWindowTreeDisconnect wwt2(win ? ((CWsWindowGroup *)win)->TextCursor() : NULL);
		WalkWindowTree(wwt2,EWalkChildren); // Disconnect all child windows
		iChild=NULL;
		}
	TWindowServerEvent::RemoveFromSwitchOnEventList(*this);
	TWindowServerEvent::RemoveFromErrorMessageList(*this);
	TWindowServerEvent::RemoveFromGroupChangeEventEventList(*this);
	TWindowServerEvent::RemoveFromFocusChangeEventEventList(*this);
	TWindowServerEvent::RemoveFromGroupListChangeEventEventList(*this);
	TWindowServerEvent::RemoveFromModifierChangedEventList(*this);
	TWindowServerEvent::RemoveFromScreenDeviceChangeEventList(*this);
	CWsTop::StopWindowGettingOffEvents(this);
	}

TBool CWsWindowBase::CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)
//
// If the command is supported by the window base class process it and return ETrue
// if it is not supported return EFalse
//
	{
	switch(aOpcode)
		{
		case EWsWinOpFree:
			delete this;
			break;
		case EWsWinOpSetOrdinalPosition:
			SetOrdinalPosition(*aCmd.Int);
			break;
		case EWsWinOpOrdinalPriority:
			SetReply(iOrdinalPriority);
			break;
		case EWsWinOpOrdinalPosition:
			SetReply(OrdinalPosition(EFalse));
			break;
		case EWsWinOpFullOrdinalPosition:
			SetReply(OrdinalPosition(ETrue));
			break;
		case EWsWinOpClientHandle:
			SetReply(iClientHandle);
			break;
		case EWsWinOpParent:
			if (!iParent)
				{
				OwnerPanic(EWservPanicParentDeleted);
				}
			SetReply(iParent->iClientHandle);
			break;
		case EWsWinOpPrevSibling:
			{
			if (!iParent)
				{
				OwnerPanic(EWservPanicParentDeleted);
				}
			TUint32 reply=NULL;
			for(CWsWindowBase *win=this->GetPrevSibling();win;win=win->GetPrevSibling())
				{
				if (win->iWsOwner==iWsOwner)
					{
					reply=win->iClientHandle;
					break;
					}
				}
			SetReply(reply);
			}
			break;
		case EWsWinOpNextSibling:
			{
			TUint32 reply=NULL;
			for(CWsWindowBase *win=this->iSibling;win;win=win->iSibling)
				{
				if (win->iWsOwner==iWsOwner)
					{
					reply=win->iClientHandle;
					break;
					}
				}
			SetReply(reply);
			}
			break;
		case EWsWinOpChild:
			SetReply(iChild==NULL ? NULL : iChild->iClientHandle);
			break;
		case EWsWinOpScreenNumber:
			SetReply(Screen()->ScreenNumber());
			break;
		case EWsWinOpWindowGroupId:
			if (!WinGroup())
				{
				OwnerPanic(EWservPanicParentDeleted);
				}
			SetReply(WinGroup()->Identifier());
			break;
		case EWsWinOpEnableOnEvents:
			TWindowServerEvent::AddToSwitchOnEventListL(*this, *aCmd.EventControl);
			break;
		case EWsWinOpDisableOnEvents:
			TWindowServerEvent::RemoveFromSwitchOnEventList(*this);
			break;
		case EWsWinOpEnableErrorMessages:
			TWindowServerEvent::AddToErrorMessageListL(*this, *aCmd.EventControl);
			break;
		case EWsWinOpDisableErrorMessages:
			TWindowServerEvent::RemoveFromErrorMessageList(*this);
			break;
		case EWsWinOpEnableModifierChangedEvents:
			TWindowServerEvent::AddToModifierChangedEventListL(*this, aCmd.EnableModifierChangedEvents->modifierMask,
														aCmd.EnableModifierChangedEvents->circumstances);
			break;
		case EWsWinOpDisableModifierChangedEvents:
			TWindowServerEvent::RemoveFromModifierChangedEventList(*this);
			break;
		case EWsWinOpEnableGroupChangeEvents:
			TWindowServerEvent::AddToGroupChangeEventListL(*this);
			break;
		case EWsWinOpDisableGroupChangeEvents:
			TWindowServerEvent::RemoveFromGroupChangeEventEventList(*this);
			break;
		case EWsWinOpEnableFocusChangeEvents:
			TWindowServerEvent::AddToFocusChangeEventListL(*this);
			break;
		case EWsWinOpDisableFocusChangeEvents:
			TWindowServerEvent::RemoveFromFocusChangeEventEventList(*this);
			break;
		case EWsWinOpEnableGroupListChangeEvents:
			TWindowServerEvent::AddToGroupListChangeEventListL(*this);
			break;
		case EWsWinOpDisableGroupListChangeEvents:
			TWindowServerEvent::RemoveFromGroupListChangeEventEventList(*this);
			break;
		case EWsWinOpSetCustomPointerCursor:
			CWsObject *pointercursor;
			if ((pointercursor=iWsOwner->HandleToObj(*aCmd.Int, WS_HANDLE_POINTER_CURSOR))==NULL)
				OwnerPanic(EWservPanicSprite);
			SetPointerCursor((CWsPointerCursor *)pointercursor);
			break;
		case EWsWinOpSetPointerCursor:
			SetPointerCursorByIndex(*aCmd.UInt);
			break;
		case EWsWinOpClearPointerCursor:
			SetPointerCursor(NULL);
			break;
		case EWsWinOpSetNonFading:
			{
			WS_ASSERT_DEBUG(iScreen, EWsPanicNoScreen);
			// No fading will occur from a graphical perspective, but the fade counts
			// are maintained for BC reasons.
			TWalkWindowTreeSetNonFading wwt(*aCmd.Bool);
			WalkWindowTree(wwt,EWalkChildren);
			Screen()->AcceptFadeRequest( reinterpret_cast<CWsWindow*>(this), 
														 ETrue, 
														 EFalse, 
														 ETrue );
			}
			break;
		case EWsWinOpSetFade:
			{
			if (!iParent)
				{
				OwnerPanic(EWservPanicParentDeleted);
				}
			WS_ASSERT_DEBUG(iScreen, EWsPanicNoScreen);
			
			TUint8 blackMap;
			TUint8 whiteMap;
			if (aCmd.SetFaded->UseDefaultMap())
				{
				iScreen->GetFadingParams(blackMap,whiteMap);
				}
			else
				{
				aCmd.SetFaded->GetFadingParams(blackMap,whiteMap);
				}
	
			if (aCmd.SetFaded->IncludeChildren())
				{
				TWalkWindowTreeSetFaded wwt(aCmd.SetFaded->Faded(),this,blackMap,whiteMap);
				WalkWindowTree(wwt,EWalkChildren);
				}
			else
				{
				if (iWinType==EWinTypeGroup)
					OwnerPanic(EWservPanicOpcode);
				static_cast<CWsClientWindow*>(this)->SetFaded(aCmd.SetFaded->Faded(),blackMap,whiteMap); 
				}

			if (CWsTop::IsFadeEnabled()) 
				{
				Screen()->AcceptFadeRequest( reinterpret_cast<CWsWindow *> (this), 
											 aCmd.SetFaded->Faded(), 
											 EFalse, 
											 aCmd.SetFaded->IncludeChildren() );
				}
			}
			break;
		default:
			return(EFalse);
		}
	return(ETrue);
	}

TBool CWsWindowBase::QueueEvent(TInt aEvent) const
	{
	if (WsOwner())
		return(WsOwner()->EventQueue()->QueueEvent(iClientHandle, aEvent));
	return(EFalse);
	}

void CWsWindowBase::SetPointerCursorByIndex(TInt aIndex)
	{
	SetPointerCursor(CWsClient::SystemPointerCursor(aIndex));
	}

void CWsWindowBase::SetPointerCursor(CWsPointerCursor *aCursor)
	{
	CWsPointerCursor *old=iPointerCursor;
	iPointerCursor=aCursor;
	if (iPointerCursor)
		iPointerCursor->Open();
	WsPointer::UpdatePointerCursor();
	if (old)
		old->Close();
	}

TBool CWsWindowBase::TreeIsObscured() const
	{
	TBool result;
	TWalkWindowTreeIsObscured wwt(result);
	CONST_CAST(CWsWindowBase *,this)->WalkWindowTree(wwt,EWalkChildren);
	return(result);
	}

CEventQueue *CWsWindowBase::EventQueue() const
	{
	return(iWsOwner->EventQueue());
	}

void CWsWindowBase::WalkWindowTree(TWalkWindowTreeBase &aWalkClass,TWalkMode aMode)
//
// Walks windows in a front to back order
//
// If mode is EWalkBehind
//	call DoIt for all windows that are behind 'this'
// else if mode is EWalkChildren
//  call DoIt for all descendents
// else if mode is EWalkChildrenAndBehind
//  call DoIt for for all descendents and windows behind
//
	{
	if (this!=NULL)
		{
		CWsWindowBase *win=this;
		CWsWindowBase *end=RootWindow();
		CWsWindowBase *sibling=win->iSibling;
		CWsWindowBase *parent=win->iParent;
		if (aMode!=EWalkBehind)
			{
			if (aMode==EWalkChildren)
				end=win;
			goto start;
			}
		do
			{
			if (sibling!=NULL)
				{
				win=sibling;
start:			while(win->iChild!=NULL)
					win=win->iChild;
				}
			else
				win=parent;
			sibling=win->iSibling;	// De-reference win so it can be destroyed by 'DoIt'
			parent=win->iParent;
			if (win->iWinType!=EWinTypeGroup && aWalkClass.DoIt((CWsWindow *)win))
				return;
			} while(win!=end);
		}
	}

/* Walks windows in a front to back order

 If aResume is EFalse the walk is identical to above.
 Otherwise iWin is taken as the restart point, (any child windows will have been
 walked previously).
 */
void CWsWindowBase::WalkWindowTree(TResumableWalkWindowTreeBase& aWalkClass, TWalkMode aMode, TBool aResume)
	{
	if (this != NULL)
		{ // init
		if (!aResume)
			{
			aWalkClass.iWin = this;
			aWalkClass.iEnd = (aMode == EWalkChildren) ? this : RootWindow();
			aWalkClass.iParent = aWalkClass.iWin->iParent;
			if (aMode == EWalkBehind)
				{
				aWalkClass.iNextChild = aWalkClass.iWin->iSibling;
				}
			else
				{ // ensure walk includes this and its child windows
				aWalkClass.iNextChild = this;
				}
			}
		else if (aWalkClass.iWin == aWalkClass.iEnd)
			{
			return; // walk had already reached end
			}

		do
			{
			if (aWalkClass.iNextChild != NULL)
				{ // walk down tree to a leaf window
				aWalkClass.iWin = aWalkClass.iNextChild;
				while (aWalkClass.iWin->iChild != NULL)
					{
					aWalkClass.iWin = aWalkClass.iWin->iChild;
					}
				}
			else
				{ // walk up tree
				aWalkClass.iWin = aWalkClass.iParent;
				}
			// De-reference iWin so it can be destroyed by 'DoIt'
			aWalkClass.iNextChild = aWalkClass.iWin->iSibling;
			aWalkClass.iParent = aWalkClass.iWin->iParent;
			if ( ( aWalkClass.iWin->iWinType != EWinTypeGroup ) && aWalkClass.DoIt(static_cast<CWsWindow *>(aWalkClass.iWin)) )
				{
				return;
				}
			}
		while (aWalkClass.iWin != aWalkClass.iEnd);
		}
	}

#if defined(_DEBUG)

void CWsWindowBase::CheckTree()
	{
	TWalkWindowTreeCheck wwt1;
	WalkWindowTree(wwt1,EWalkChildren);
	}

enum {ENullWsHandle=0xFFFFFFFF};	// Events delivered to this handle are thrown away
TBool CWsWindowBase::IsClientHandleInUse(TUint32 aHandle)
	{
	if (aHandle==static_cast<TUint>(ENullWsHandle))		//This value has a special meaning in test code
		return EFalse;
	CWsObjectIx* index=iWsOwner->ObjectIndex();
	const CWsObject* obj;
	TInt length=index->Length();
	TInt ii;
	for (ii=0;ii<length;++ii)
		{
		obj=index->At(ii);
		if (obj && (obj->Type()==WS_HANDLE_WINDOW || obj->Type()==WS_HANDLE_GROUP_WINDOW))
			{
			if (STATIC_CAST(const CWsWindowBase*,obj)->ClientHandle()==aHandle)
				return ETrue;
			}
		}
	return EFalse;
	}

#endif

TBool CWsWindowBase::IsDSAHost() const
	{
	return EFalse;
	}

