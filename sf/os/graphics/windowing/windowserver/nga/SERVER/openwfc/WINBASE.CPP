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
#include "windowelementset.h"

CWsWindowBase::CWsWindowBase(CWsClient* aOwner,WH_HANDLES aType, CScreen* aScreen) : CWsScreenObject(aOwner,aType,aScreen)		
	{
	}

void CWsWindowBase::ConstructL(CWsWindowBase *aParent)
	{
	iParent=aParent;
	iSibling=aParent->iChild;
	aParent->iChild=this;
	CScreen* screen = aParent->Screen();
	WS_ASSERT_DEBUG(screen,EWsPanicNoScreen);
	MWsWindowTreeObserver* const windowTreeObserver = screen->WindowTreeObserver();
	if (windowTreeObserver)
		{
		windowTreeObserver->NodeCreated(*this, ParentNode());
		iBaseWinFlags |= EBaseWinNodeCreated;
		}
	SetOrdinalPosition(0);
	iFadeCount = iParent->iFadeCount;
	}

CWsWindowBase *CWsWindowBase::GetPrevSibling() const
	{
	if(iParent == NULL) //RootWindow
		return(NULL);
	
	CWsWindowBase* prev=iParent->iChild;
	CWsWindowBase *ret=NULL;
	while (prev!=this)
		{
		ret=prev;
		prev=prev->iSibling;
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

CWsWindowGroup *CWsWindowBase::WinGroup() const
	{
	if (iWinType==EWinTypeClient)
		return(((CWsClientWindow *)this)->TopClientWindow()->Parent());
	if (iWinType==EWinTypeGroup)
		return((CWsWindowGroup *)this);
	return(NULL);
	}

TBool CWsWindowBase::CheckOrdinalPositionChange(TInt aPos)
//
// This routine checks to see whether the specified new ordinal position
// will causes a change, if so returns ETrue else EFalse.
//
	{
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
	TBool changedWindowGroup = EFalse;
	WS_ASSERT_DEBUG(aNewParent,EWsPanicWindowNull);
	if (aNewParent != iParent)
		{
		iScreen->ScheduleRegionUpdate(NULL);		
		TWalkWindowTreeScheduleRedraws wwt;
		WalkWindowTree(wwt, EWalkChildren);
		changedWindowGroup = ETrue;
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

	Screen()->WindowElements().SortByZOrder();

	MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
	if (windowTreeObserver)
		{
		if(changedWindowGroup && (WinType() == EWinTypeClient))
			{
			windowTreeObserver->MovedToWindowGroup(*this, *(this->WinGroup()));
			}
		else if(!changedWindowGroup)
			{
			windowTreeObserver->SiblingOrderChanged(*this, OrdinalPosition(ETrue));
			}
		else if(changedWindowGroup)
			{
			OwnerPanic(EWservPanicInvalidParameter); //Should be impossible to end up here as only WinType() EWinTypeClient 
			}										 //and EWinTypeGroup can be moved to another windowgroup.
		}											 //@see RWindowBase::MoveToGroup
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
			{
			delete this;
			break;
			}
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
			{
			TUint32 reply=NULL;
			CWsWindowGroup *wg=WinGroup();
			if (wg)
				{
				reply=wg->Identifier();
				}
			SetReply(reply);
			}
			break;
		case EWsWinOpEnableOnEvents:
			{
			const TEventControl circumstances = *aCmd.EventControl;
			TWindowServerEvent::AddToSwitchOnEventListL(*this, circumstances);
			if (iScreen->ChangeTracking())
				{
				if(circumstances & EEventControlOnlyWhenVisible)
					{
					TWalkWindowTreeSetupVisibleRegionTracking wwt(ETrue);
					WalkWindowTree(wwt, EWalkChildren);
					}
				}
			break;
			}
		case EWsWinOpDisableOnEvents:
			{
			TWindowServerEvent::RemoveFromSwitchOnEventList(*this);
			if (iScreen->ChangeTracking())
				{
				TWalkWindowTreeSetupVisibleRegionTracking wwt(EFalse);
				WalkWindowTree(wwt, EWalkChildren);
				}
			break;
			}
		case EWsWinOpEnableErrorMessages:
			{
			const TEventControl circumstances = *aCmd.EventControl;
			TWindowServerEvent::AddToErrorMessageListL(*this, circumstances);
			if (iScreen->ChangeTracking())
				{
				if(circumstances & EEventControlOnlyWhenVisible)
					{
					TWalkWindowTreeSetupVisibleRegionTracking wwt(ETrue);
					WalkWindowTree(wwt, EWalkChildren);
					}
				}
			break;
			}
		case EWsWinOpDisableErrorMessages:
			{
			TWindowServerEvent::RemoveFromErrorMessageList(*this);
			if (iScreen->ChangeTracking())
				{
				TWalkWindowTreeSetupVisibleRegionTracking wwt(EFalse);
				WalkWindowTree(wwt, EWalkChildren);
				}
			break;
			}
		case EWsWinOpEnableModifierChangedEvents:
			{
			const TInt modifierMask = aCmd.EnableModifierChangedEvents->modifierMask;
			const TEventControl circumstances = aCmd.EnableModifierChangedEvents->circumstances;
			TWindowServerEvent::AddToModifierChangedEventListL(*this, modifierMask, circumstances);
			if (iScreen->ChangeTracking())
				{
				if(circumstances & EEventControlOnlyWhenVisible)
					{
					TWalkWindowTreeSetupVisibleRegionTracking wwt(ETrue);
					WalkWindowTree(wwt, EWalkChildren);
					}
				}
			break;
			}
		case EWsWinOpDisableModifierChangedEvents:
			{
			TWindowServerEvent::RemoveFromModifierChangedEventList(*this);
			if (iScreen->ChangeTracking())
				{
				TWalkWindowTreeSetupVisibleRegionTracking wwt(EFalse);
				WalkWindowTree(wwt, EWalkChildren);
				}
			break;
			}
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
			}
			break;
		case EWsWinOpSetFade:
			{
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
				
				const TBool KNotifyObserver = ETrue; //yes please
				const TBool KFaded = aCmd.SetFaded->Faded();
				static_cast<CWsClientWindow*>(this)->SetFaded(KFaded, blackMap, whiteMap, KNotifyObserver); 
				}
			}
			break;
		case EWsWinOpEnableAdvancedPointers:
      if(!IsActivated())
          {
          // Must call this BEFORE activating the window.
          iBaseWinFlags |= EBaseWinAdvancedPointersEnabled;
          }
      else
          {                    
          // Called after activation, so panic the client.                
          OwnerPanic(EWservPanicUnableToEnableAdvPointer);
          }
			break;
		case EWsWinOpSetSurfaceTransparency:
			RDebug::Printf("[Bug 3343] OpCode EWsWinOpSetSurfaceTransparency not supported.");
			break;
		default:
			return(EFalse);
		}
	return(ETrue);
	}

/** @see MWsWindowTreeNode */
MWsWindowTreeNode::TType CWsWindowBase::NodeType() const
	{
	return static_cast<MWsWindowTreeNode::TType>(iWinType); //TWinType is a subset of MWsWindowTreeNode::TType
	}

/** @see MWsWindowTreeNode */
const MWsWindow* CWsWindowBase::Window() const
	{
	return (iWinType!=EWinTypeGroup) ? (static_cast<const CWsWindow*>(this)) : NULL;
	}

/** @see MWsWindowTreeNode */
const MWsSprite* CWsWindowBase::Sprite() const
	{
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsStandardTextCursor* CWsWindowBase::StandardTextCursor() const
	{
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsWindowGroup* CWsWindowBase::WindowGroup() const
	{
	return static_cast<MWsWindowGroup*>(WinGroup()); 
	}

/** @see MWsWindowTreeNode */
const MWsWindowTreeNode* CWsWindowBase::ParentNode() const
	{
	return iParent;
	}

TBool CWsWindowBase::QueueEvent(TInt aEvent, TInt aIntVal) const
	{
	if (WsOwner())
		return(WsOwner()->EventQueue()->QueueEvent(iClientHandle, aEvent, aIntVal));
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
	TWsPointer::UpdatePointerCursor();
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
	CEventQueue* eventQueue = NULL;
	if (iWsOwner)
		eventQueue = iWsOwner->EventQueue();
	return eventQueue;
	}

TInt CWsWindowBase::Depth() const
	{
	TInt count=0;
	const CWsWindowBase *win=this;
	while (win->WinType()!=EWinTypeRoot)
		{
		++count;
		win=win->iParent;
		}
	return(count);
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

void CWsWindowBase::WalkWindowTreeBackToFront(TWalkWindowTreeBase &aWalkClass, TWalkModeBackToFront aMode)
	{
	// Walks windows in a back to front order
	//
	// If mode is EVisitParentNodesFirst
	// call DoIt() on each node before walking their child windows.	
	
	if(iSibling)
		iSibling->WalkWindowTreeBackToFront(aWalkClass,aMode);
	
	if(aMode == EVisitParentNodesFirst)
		aWalkClass.DoIt(static_cast<CWsWindow *>(this));
	
	if(iChild)
		iChild->WalkWindowTreeBackToFront(aWalkClass,aMode);
	
	}

TBool CWsWindowBase::IsDSAHost() const
	{
	return EFalse;
	}

TBool CWsWindowBase::IsActivated() const
	{
	return EFalse;
	}

void CWsWindowBase::AddSprite(CWsSpriteBase * aSprite)
	{
	aSprite->SetNext(iSpriteList);
	iSpriteList = aSprite;
	}
	
void CWsWindowBase::RemoveSprite(CWsSpriteBase * aSprite)
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

void CWsWindowBase::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	//Sprites
	if(iSpriteList)
		iSpriteList->SendState(aWindowTreeObserver);
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
