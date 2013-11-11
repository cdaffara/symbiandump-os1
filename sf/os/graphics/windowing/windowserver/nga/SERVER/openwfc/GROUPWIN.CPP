// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Group window sub-class of CWsWindow
// 
//

#include <e32std.h>
#include "W32STD.H"
#include "W32CLICK.H"
#include "server.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "walkwindowtree.h"
#include "wstop.h"
#include "EVENT.H"
#include "KEYCLICK.H"
#include "PRIKEY.H"
#include "panics.h"
#include "windowelementset.h"
#include "pointer.h"

GLREF_D TPtr nullDescriptor;
GLREF_D CDebugLogBase* wsDebugLog;

#if defined(_DEBUG)
TInt CWsWindowGroup::iSkipCount=0;
#endif

TBool CWsWindowGroup::iEventQueueTest=EFalse;			// For stress testing the EventQueue code

TInt CWsWindowGroup::iIdentifierCount=1;
TBool CWsWindowGroup::iFocusGainPreProcess=EFalse;		//'REMOVEFADINGONFOCUSGAIN' flag in INI file
RPointerArray< TDblQue<CWsWindowGroup> > CWsWindowGroup::iChains(3);
static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_SwEvent,ECapabilitySwEvent);
static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_WriteDeviceData,ECapabilityWriteDeviceData);
const TInt KArrayMaxGranularity=0x10000000;

CWsWindowGroup* CWsWindowGroup::NewL(CWsClient* aOwner, CScreen* aScreen, 
	const TWsClCmdCreateWindowGroup& aCmd)
	{
	CWsWindowGroup* self = new(ELeave) CWsWindowGroup(aOwner, aScreen);
	CleanupStack::PushL(self);
	self->ConstructL(aCmd);
	CleanupStack::Pop(self);
	return self;
	}

CWsWindowGroup::CWsWindowGroup(CWsClient* aOwner, CScreen* aScreen) : CWsWindowBase(aOwner,WS_HANDLE_GROUP_WINDOW,aScreen)
	{
	__DECLARE_NAME(_S("CWsWindowGroup"));
	iWinType=EWinTypeGroup;
	}

void CWsWindowGroup::PurgeCapturedKeys()
	{
	CWsObjectIx& objix=*WsOwner()->ObjectIndex();
	const TWsObject* ptr=objix.FirstObject();
	const TWsObject* end=ptr+objix.Length();
	while(++ptr<end)		//Fisrt one should always have a NULL object
		{
		const CWsObject* obj=ptr->iObject;
		if (obj 
			&& ((obj->Type()==WS_HANDLE_CAPTURE_KEY && STATIC_CAST(const CWsCaptureKey*,obj)->WindowGroup()==this)
				|| (obj->Type()==WS_HANDLE_CAPTURE_KEY_UPDOWNS && STATIC_CAST(const CWsCaptureKeyUpsAndDowns*,obj)->WindowGroup()==this)
				|| (obj->Type()==WS_HANDLE_CAPTURE_LONG_KEY && STATIC_CAST(const CWsCaptureLongKey*,obj)->WindowGroup()==this)))
			{
			objix.Remove(ptr);
			delete obj;
			}
		}
	objix.Tidy();
	CKeyboardRepeat::CancelRepeat(this);
	}

void CWsWindowGroup::SwitchToOwningWindow(CWsWindowGroup *aClosingWindow)
	{
	if (this==CWsTop::FocusWindowGroup())
		{
		CWsWindowGroup *winGroup=NULL;
//
// First try for an 'owning' window
//
		if (iOwningWindowGroup)
			{
			for(winGroup=RootWindow()->Child();winGroup;winGroup=winGroup->NextSibling())
				if (winGroup->Identifier()==iOwningWindowGroup && winGroup->iOrdinalPriority==iOrdinalPriority)
					goto gotIt;
			}
//
// If that failed look for the frontmost window belonging to the owner of dying window
//
		for(winGroup=RootWindow()->Child();winGroup;winGroup=winGroup->NextSibling())
			if (winGroup!=this && winGroup->WsOwner()==WsOwner() && winGroup->iOrdinalPriority==iOrdinalPriority)
				goto gotIt;
//
// Next try for the nominated default owning window group
//
		winGroup=iScreen->DefaultOwningWindowGroup();
		if (winGroup && winGroup->iOrdinalPriority==iOrdinalPriority)
			{
gotIt:		winGroup->SetOrdinalPosition(0,this);
			return;
			}
		}
	ResetFocus(aClosingWindow);
	}

CWsWindowGroup::~CWsWindowGroup()
	{
	MWsWindowTreeObserver* windowTreeObserver = NULL;
	if (Screen()&& iBaseWinFlags&EBaseWinNodeCreated)
		{
		windowTreeObserver = Screen()->WindowTreeObserver();
		if (windowTreeObserver)
			{
			if( iQueue && (iQueue->Last()!=this) )
				{
				//This node is part of chain, send notification unless this is the last node in the chain
				windowTreeObserver->WindowGroupChainBrokenAfter(*this);
				}
			windowTreeObserver->NodeReleased(*this);
			iBaseWinFlags &= ~EBaseWinNodeCreated;
			}
		}
	DisconnectFloatingSprites();
	
	if (wsDebugLog)
		{
		TLogMessageText buf;
		_LIT(KWSERVDebugLogGroupWindowId,"Destroying: RWindowGroup[0x%x,%d],Id=%d");
		buf.Format(KWSERVDebugLogGroupWindowId,iClientHandle,LogHandle(),iIdentifier);
		wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, buf);
		}
	if (CClick::IsHandler())
		CClick::OtherEvent(EEventGroupWindowClose,reinterpret_cast<TAny*>(iIdentifier));
	if (iQueue)
		{
		if (iQueue->Last()!=this)
			{		//Unlink all the children of the window that is being deleted
			TDblQueIter<CWsWindowGroup> iter(*iQueue);
			CWsWindowGroup* groupWin;
			iter.SetToLast();
			while ((groupWin=iter--)!=this)
				{
				WS_ASSERT_DEBUG(groupWin!=NULL && groupWin->iQueue==iQueue,EWsPanicGroupWindowChainError);
				if ( windowTreeObserver && (iQueue->Last()!=groupWin) )
					{
					//Send notification unless groupWin is the last node in the chain
					windowTreeObserver->WindowGroupChainBrokenAfter(*groupWin);
					}
				groupWin->iChainLink.Deque();
				// coverity[extend_simple_error]
				groupWin->iQueue=NULL;
				}
			}
		WS_ASSERT_DEBUG(iQueue->Last()==this,EWsPanicGroupWindowChainError);
		if(windowTreeObserver && !iQueue->IsEmpty())
			{
			//If there are chained nodes before this one, notify "chain broken" immediately before this node 
			TDblQueIter<CWsWindowGroup> iter(*iQueue);
			iter.SetToLast(); 	//i.e. set to this (see assert above)
			iter--;				//i.e. set to parent
			CWsWindowGroup* parent = iter;
			if(parent)
				{
				windowTreeObserver->WindowGroupChainBrokenAfter(*parent);
				}
			}
		TDblQueLinkBase* parentLink=iChainLink.iPrev;
		iChainLink.Deque();
		if (parentLink->iNext==parentLink->iPrev)		//Check to see chain no longer required
			{
			if (!iQueue->IsEmpty())
				{				//Only the parent is left in queue
				CWsWindowGroup* parent=iQueue->First();
				static_cast<TDblQueLink*>(parentLink)->Deque();
				WS_ASSERT_DEBUG(parent->iQueue==iQueue,EWsPanicGroupWindowChainError);
				// coverity[extend_simple_error]
				parent->iQueue=NULL;
				}
			DeleteQueue(iQueue);
			}
		}
	RemoveAllPriorityKeys();
	PurgeCapturedKeys();
	iTextCursor.Close();
	SetPointerCursor(NULL);
	for(CWsTopClientWindow *win=Child();win;win=win->NextSiblingTop())
		win->SetInactive();
	if (iScreen)
		{
		iScreen->RemoveFromDefaultOwningList(this);
		}
	CWsWindowBase::Shutdown();
	TWindowServerEvent::SendGroupChangedEvents();
	// coverity[extend_simple_error]
	iClientHandle=0;	// To block focus lost events being sent
// Decide which window to give focus to if WServ isn't shutting down
	if (iScreen && !CWsTop::ShuttingDown())
		SwitchToOwningWindow(this);
	delete iGroupName;
	delete iMessageArray;
	}

void CWsWindowGroup::DisconnectFloatingSprites()
	{
	CWsSpriteBase * current = iSpriteList;
	while (current)
		{
		CWsSpriteBase * next = current->Next();
		current->Deactivate();
		current->DisconnectGroupWin();
		current = next;
		}
	}

void CWsWindowGroup::DeleteQueue(TDblQue<CWsWindowGroup>* aQueue)
	{
	iChains.Remove(iChains.Find(aQueue));
	delete aQueue;
	if (iChains.Count()==0)
		{
		iChains.Compress();
		}
	}

void CWsWindowGroup::AdvanceIdentifierCount()
	{
	if (++iIdentifierCount>EMaxIdentifierCount)
		iIdentifierCount=1;		// so limit it to low value
	}

void CWsWindowGroup::ConstructL(const TWsClCmdCreateWindowGroup &aCmd)
	{
#if defined(_DEBUG)
	if (IsClientHandleInUse(aCmd.clientHandle))
		{
		OwnerPanic(EWservPanicDuplicateHandle);
		}
#endif
	NewObjL();
	iFlags=EGroupFlagAutoForeground|EGroupFlagMsgQueueNew;
	if (aCmd.focus)
		{
		iFlags|=EGroupFlagReceivesFocus;
		}
	iTextCursor.ConstructL(this);
	iClientHandle=aCmd.clientHandle;

	if(aCmd.screenDeviceHandle <= 0)
		{
		//Use primary screen. Client should make sure PrimaryScreenDevice is correct set up immediately after establishing session.
		iScreenDevice=iWsOwner->PrimaryScreenDevice();
		}
	else
		{
		//Use the specified screen
		iScreenDevice=STATIC_CAST(DWsScreenDevice*,iWsOwner->HandleToObj(aCmd.screenDeviceHandle,WS_HANDLE_SCREEN_DEVICE));
		}

	iScreen = (iScreenDevice) ? iScreenDevice->Screen() : CWsTop::Screen(); //if no screen device use screen 0

	CWsWindowGroup* parent=NULL;
	if (aCmd.parentId>0)
		{
		parent=CWsWindowGroup::WindowGroupFromIdentifier(aCmd.parentId);
		if (!parent)
			{
			OwnerPanic(EWservPanicWindow);
			}
		
 		if(parent->Screen() != iScreen)
 			{
 			OwnerPanic(EWservPanicWrongScreen);
 			}
		
		if (parent->iOrdinalPriorityAdjust>0)
			{
			WS_ASSERT_DEBUG(parent->iQueue==NULL,EWsPanicGroupWindowChainError);
			parent->iOrdinalPriorityAdjust=0;
			parent->UpdateOrdinalPriority(ETrue);
			}
		iOrdinalPriorityBase=parent->iOrdinalPriorityBase;
		iOrdinalPriority=iOrdinalPriorityBase;
		}

	do
		{
		AdvanceIdentifierCount();	// Always advance by at least one to stop re-using last id
		} while (WindowGroupFromIdentifier(iIdentifierCount)); // If current count is in use try again
	iIdentifier=iIdentifierCount;

	CWsWindowBase::ConstructL(RootWindow());
	iScreen->ResetFocus(NULL);
	
	if (parent)
		{
		TDblQue<CWsWindowGroup>* queue=parent->iQueue;
		if (queue && queue->Last()!=parent)
			User::Leave(KErrInUse);
		if (parent->iWsOwner!=iWsOwner)
			{
			_LIT_SECURITY_POLICY_S0(securityPolicy,parent->iChildSID);
			if (!securityPolicy().CheckPolicy(iWsOwner->ClientMessage()))
				User::Leave(KErrPermissionDenied);
			}
		if (!queue)
			{
			queue=new(ELeave) TDblQue<CWsWindowGroup>(_FOFF(CWsWindowGroup,iChainLink));
			CleanupStack::PushL(queue);
			User::LeaveIfError(iChains.Append(queue));
			CleanupStack::Pop(queue);
			queue->AddFirst(*parent);
			parent->iQueue=queue;			
			}
		iQueue=queue;		//Shouldn't set the queue until after it can leave
		iChainLink.Enque(&parent->iChainLink);
		
		MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();						
		if (windowTreeObserver)
			{
			windowTreeObserver->WindowGroupChained(*parent, *this);
			}			
		}
	iMessageArray=new(ELeave) CArrayVarSeg<TWsMessage>(1);
	if (CClick::IsHandler())
		{
		TGroupWindowOpenData params;
		params.iIdentifier=iIdentifier;
		params.iClient=iWsOwner->ConnectionHandle();
		params.iNumClientWindowGroups=NumClientWindowGroups()-1;	//Don't include this one
		CClick::OtherEvent(EEventGroupWindowOpen,&params);
		}
	if (wsDebugLog)
		{
		TLogMessageText buf;
		_LIT(KWSERVDebugLogGroupWindowId,"Creating: RWindowGroup[0x%x,%d],Id=%d");
		buf.Format(KWSERVDebugLogGroupWindowId,iClientHandle,LogHandle(),iIdentifier);
		wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, buf);
		}
	}

void CWsWindowGroup::UpdateOrdinalPriority(TBool aDoAdjust)
	{
	TInt newPri;
	newPri=iOrdinalPriorityBase;
	if (iWsOwner==CWsTop::FocusWindowGroupOwner())
		newPri+=iOrdinalPriorityAdjust;
	CheckCapability(newPri);
	if (newPri!=iOrdinalPriority)
		{
		iOrdinalPriority=newPri;
		if (aDoAdjust)
			SetOrdinalPosition(0);
		}
	}

void CWsWindowGroup::SetOrdinalPriority(TInt aPos,TInt aPriority)
	{
	if (!iQueue)
		{
		iOrdinalPriorityBase=aPriority;
		UpdateOrdinalPriority(EFalse);
		}
	else
		{
		TDblQueIter<CWsWindowGroup> iter(*iQueue);
		CWsWindowGroup* group;
		while ((group=iter++)!=NULL)
			{
			group->iOrdinalPriorityBase=aPriority;
			group->iOrdinalPriority=aPriority;
			}
		}
	SetOrdinalPosition(aPos);
	}

void CWsWindowGroup::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
#ifdef _DEBUG
	// Save root window for performing CheckTree at the end of this func.
	// When aOpcode is EWsWinOpFree, this object would've been destroyed
	// and a call to RootWindow() in that case would be impossible
	CWsRootWindow* rootWindow=RootWindow();

	// For certain opcodes, check for the 'screen device deleted' condition. If it
	// has occured for the screen device associated with this group window then
	// those op-codes are not valid, and the client is panicked.
	switch (aOpcode)
		{
		case EWsWinOpEnableScreenChangeEvents:
		case EWsWinOpAllowChildWindowGroup:
		case EWsWinOpReceiveFocus:
		case EWsWinOpAutoForeground:
		case EWsWinOpSetOrdinalPositionPri:
		case EWsWinOpSetOrdinalPriorityAdjust:
		case EWsWinOpCaptureKey:
		case EWsWinOpCaptureKeyUpsAndDowns:
		case EWsWinOpCaptureLongKey:
		case EWsWinOpAddPriorityKey:
		case EWsWinOpSetTextCursor:
		case EWsWinOpSetTextCursorClipped:
		case EWsWinOpSetOwningWindowGroup:
		case EWsWinOpDefaultOwningWindow:
		case EWsWinOpSetName:
		case EWsWinOpDisableKeyClick:
		case EWsWinOpSendPointerEvent:
		case EWsWinOpSendAdvancedPointerEvent:
			{
			if (ScreenDeviceDeleted())
				OwnerPanic(EWservPanicGroupWinScreenDeviceDeleted);
			break;				
			};
		}
#endif

	TWsWinCmdUnion pData;
	pData.any=aCmdData;
	if (CWsWindowBase::CommandL(aOpcode,pData)==EFalse)
		{
		switch(aOpcode)
			{
			case EWsWinOpAllowChildWindowGroup:
				iChildSID=*pData.UInt;
				break;
			case EWsWinOpEnableScreenChangeEvents:
				SetScreenChangeEventStateL(ETrue);
				break;
			case EWsWinOpDisableScreenChangeEvents:
				SetScreenChangeEventStateL(EFalse);
				break;
			case EWsWinOpReceiveFocus:
				if (*pData.Bool!=(iFlags&EGroupFlagReceivesFocus))
					{
					iFlags&=~EGroupFlagReceivesFocus;
					if (*pData.Bool)
						iFlags|=EGroupFlagReceivesFocus;
					iScreen->ResetFocus(NULL);
					}
				break;
			case EWsWinOpAutoForeground:
				iFlags&=~EGroupFlagAutoForeground;
				if (*pData.Bool)
					iFlags|=EGroupFlagAutoForeground;
				break;
			case EWsWinOpSetOrdinalPositionPri:
			case EWsWinOpSetOrdinalPositionErr:
				{
				TInt priority=pData.OrdinalPos->ordinalPriority;
				TBool hascap = CheckCapability(priority);
				SetOrdinalPriority(pData.OrdinalPos->pos, priority);
				if (aOpcode == EWsWinOpSetOrdinalPositionErr)
					{
					SetReply(hascap?KErrNone:KErrPermissionDenied);
					}
				}
				break;
			case EWsWinOpSetOrdinalPriorityAdjust:
				if (!iQueue)
					{
					iOrdinalPriorityAdjust=*pData.Int;
					UpdateOrdinalPriority(ETrue);
					}
				break;
			case EWsWinOpCaptureKey:
				{
				if(!KSecurityPolicy_SwEvent().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWindowGroup::CaptureKey API")))
					{
					User::Leave(KErrPermissionDenied);
					}
				CWsCaptureKey *cKey=new(ELeave) CWsCaptureKey(this);
				CleanupStack::PushL(cKey);
				cKey->ConstructL(*pData.CaptureKey);
				CleanupStack::Pop();
				}
				break;
			case EWsWinOpCaptureKeyUpsAndDowns:
				{
				if(!KSecurityPolicy_SwEvent().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWindowGroup::CaptureKeyUpsAndDowns API")))
					{
					User::Leave(KErrPermissionDenied);
					}
				CWsCaptureKeyUpsAndDowns *cKey=new(ELeave) CWsCaptureKeyUpsAndDowns(this);
				CleanupStack::PushL(cKey);
				cKey->ConstructL(*pData.CaptureKey);
				CleanupStack::Pop();
				}
				break;
			case EWsWinOpCaptureLongKey:
				{
				if(!KSecurityPolicy_SwEvent().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWindowGroup::CaptureLongKey API")))
					{
					User::Leave(KErrPermissionDenied);
					}
				CWsCaptureLongKey *cKey=new(ELeave) CWsCaptureLongKey(this);
				CleanupStack::PushL(cKey);
				cKey->ConstructL(*pData.CaptureLongKey);
				CleanupStack::Pop();
				}
				break;
			case EWsWinOpCancelCaptureKey:
				if (*pData.UInt!=0)	// Ignore null handle
					{
					CWsObject *destroyObj = iWsOwner->HandleToObj(*pData.UInt, WS_HANDLE_CAPTURE_KEY);
					if (destroyObj)
						{
						// Cancel any repeat that is underway for this capture
						CKeyboardRepeat::CancelRepeat(destroyObj, EFalse);
						delete destroyObj;
						}
					else
						{
#ifdef _DEBUG
						// Attempt to cancel key capture with an incorrect handle
						OwnerPanic(EWservPanicDestroy);  
#endif // _DEBUG
						}					
					}
				break;
			case EWsWinOpCancelCaptureKeyUpsAndDowns:
				if (*pData.UInt!=0)	// Ignore null handle
					{
					CWsObject *destroyObj = iWsOwner->HandleToObj(*pData.UInt, WS_HANDLE_CAPTURE_KEY_UPDOWNS);
					if (destroyObj)
						{
						delete destroyObj;
						}
					else
						{
#ifdef _DEBUG						
						// Attempt to cancel ups and downs key capture with an incorrect handle
						OwnerPanic(EWservPanicDestroy);
#endif // _DEBUG
						}					
					}
				break;
			case EWsWinOpCancelCaptureLongKey:
				if (*pData.UInt!=0)	// Ignore null handle
					{
					CWsObject *destroyObj = iWsOwner->HandleToObj(*pData.UInt, WS_HANDLE_CAPTURE_LONG_KEY);
					if (destroyObj)
						{
						// Cancel any repeat that is underway for this capture
						CKeyboardRepeat::CancelRepeat(destroyObj, ETrue);
						delete destroyObj;
						} 
					else
						{
#ifdef _DEBUG						
						// Attempt to cancel long key capture with an incorrect handle
						OwnerPanic(EWservPanicDestroy);
#endif // _DEBUG
						}					
					}
				break;
			case EWsWinOpAddPriorityKey:
				AddPriorityKeyL(pData.PriorityKey->keycode, pData.PriorityKey->modifierMask, pData.PriorityKey->modifiers);
				break;
			case EWsWinOpRemovePriorityKey:
				RemovePriorityKey(pData.PriorityKey->keycode, pData.PriorityKey->modifierMask, pData.PriorityKey->modifiers);
				break;
			case EWsWinOpSetTextCursor:
				iTextCursor.SetL(*pData.SetTextCursor, EFalse);
				break;
			case EWsWinOpSetTextCursorClipped:
				iTextCursor.SetL(*pData.SetTextCursor, ETrue);
				break;
			case EWsWinOpCancelTextCursor:
				iTextCursor.Cancel();
				break;
			case EWsWinOpSetOwningWindowGroup:
				iOwningWindowGroup=*pData.Int;
				break;
			case EWsWinOpDefaultOwningWindow:
				{
				if(KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWindowGroup::DefaultOwningWindow API")))
					{
					iScreen->SetDefaultOwningWindow(this);
					}
				}
				break;
			case EWsWinOpName:
				iWsOwner->ReplyGroupName(iGroupName,*pData.Int);
				break;
			case EWsWinOpSetName:
				{
				HBufC *newName=NULL;
				const TInt size=*pData.Int;
				if (size>0)
					{
					newName=HBufC::NewLC(size);
					TPtr ptr(newName->Des());
					iWsOwner->RemoteReadL(ptr,0);
					CleanupStack::Pop(newName);
					}
				//Window Group Name is unchanged
				if (iGroupName && newName && *iGroupName == *newName)
					{
					delete newName;
					} 
				else  //Window Group Name is changed
					{  
					delete iGroupName;
					iGroupName=newName;
					TWindowServerEvent::SendGroupChangedEvents();
					if (iScreen && iScreen->ChangeTracking())
					    {
				        MWsWindowTreeObserver* const windowTreeObserver = iScreen->WindowTreeObserver();
				        if (windowTreeObserver)
				            windowTreeObserver->AttributeChanged(*this, MWsWindowTreeObserver::EWindowGroupName);
					    }
					}
				}
				break;
			case EWsWinOpIdentifier:
				SetReply(Identifier());
				break;
			case EWsWinOpDisableKeyClick:
				if (*pData.Bool)
					iFlags|=EGroupFlagDisableKeyClick;
				else
					iFlags&=~EGroupFlagDisableKeyClick;
				if (this==CWsTop::FocusWindowGroup())
					UpdateKeyClickState();
				break;
			case EWsWinOpSendAdvancedPointerEvent:
			case EWsWinOpSendPointerEvent:
				{
				TRawEvent eventCopy = *pData.RawEvent;
				if (TWsPointer::PreProcessClientEvent(eventCopy, aOpcode == EWsWinOpSendAdvancedPointerEvent))
					{
					if (!TWindowServerEvent::MousePress(eventCopy,this))
						{
						OwnerPanic(EWservPanicEventType);
						}
					}
				}
				break;
			case EWsWinOpClearChildGroup:
				if(iQueue)
					{
					TBool fBefore=EFalse; 
					TBool fAfter=EFalse; 
					// If there is nothing to clear, return KErrArgument
					if(iQueue->Last()==this)
						{
						SetReply(KErrArgument);
						break;
						}
					// fBefore is True if there is AT LEAST one window group queued before the current one
					else if(iQueue->First()!=this)
						{
						fBefore=ETrue;
						}
					// fAfter is True if there is MORE THAN one window group queued after the current one
					TDblQueIter<CWsWindowGroup> iter(*iQueue);
					iter.SetToLast();
					if(iter--!=this && iter!=this)
						{
						fAfter=ETrue;
						}
					TDblQue<CWsWindowGroup>* queue=NULL;
					// if fBefore and fAfter are True, create a new queue and copy all window groups after the current one into that queue
					if(fBefore && fAfter)
						{
						TInt ret=KErrNoMemory;
						queue=new TDblQue<CWsWindowGroup>(_FOFF(CWsWindowGroup,iChainLink));
						if(queue)
							{
							ret=iChains.Append(queue);	
							if(ret!=KErrNone)
								{
								delete queue;
								queue=NULL; 
								}
							}
						// Check that the queue creation and appending worked (we deque all the child groups even if it didn't) 
						if(ret!=KErrNone)
							{
							SetReply(ret);
							}
						}
					// If we've got zero or one window groups after, don't need to queue them
					if(!fAfter || fBefore)
						{
						iter.SetToLast();
						CWsWindowGroup* groupWin;
						while((groupWin=iter--)!=this)
							{
							groupWin->iChainLink.Deque();
							groupWin->iQueue=queue;
							if(queue)
								queue->AddFirst(*groupWin);
							}
						}
					// if we've got no window groups before, don't need to have a queue for this anymore
					if(!fBefore)
						{
						iChainLink.Deque();
						if (!fAfter)
							{
							DeleteQueue(iQueue);
							}
						iQueue=NULL;						
						}
					if (Screen())
						{
					MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
					if (windowTreeObserver)
						{
						windowTreeObserver->WindowGroupChainBrokenAfter(*this);
							}
						}					
					}
				else // if this window group isn't queued, we can't clear any children
					{
					SetReply(KErrArgument);
					}
				break;
			case EWsWinOpSetChildGroup:
				{
				CWsWindowGroup* childWinGroup = CWsWindowGroup::WindowGroupFromIdentifier(*pData.Int);
				if(!childWinGroup																	//(no child to append)
					|| (iQueue && (!iQueue->IsLast(this) || (childWinGroup->iQueue==iQueue)))		//(GpWin has a child) || (GpWin and childGpWin in the same queue)
					|| (childWinGroup->iQueue && !childWinGroup->iQueue->IsFirst(childWinGroup))	//(childGpWin has a parent)
					|| (childWinGroup == this))														//(childGpWin == GpWin)
					{
					SetReply(KErrArgument);
					break;
					}
				if(iQueue)
				// If we have a chain, we're prepending ourselves to the child window group
				// So we take the childs chain and prepend each of the window groups in our own chain
				// beginning with the current window group and working backward
					{
					TDblQueIter<CWsWindowGroup> iter(*iQueue);
					iter.SetToLast();
					CWsWindowGroup* groupWin;
					if(childWinGroup->iQueue)
						{
						TDblQue<CWsWindowGroup>* oldQueue=iQueue;
						while((groupWin=iter--)!=NULL)
							{
							groupWin->iChainLink.Deque();
							childWinGroup->iQueue->AddFirst(*groupWin);
							groupWin->iQueue=childWinGroup->iQueue;
							}
						DeleteQueue(oldQueue);
						}
					else
						{
						iQueue->AddLast(*childWinGroup);
						childWinGroup->iQueue=iQueue;							
						}
					}
				else
				// 1. If we don't have a chain, and if the child has a chain, we can simply prepend this wg to the child
				// wg chain
				// 2. If we don't have a chain, and if the child does not have a chain, need to create a chain with the child
				//    as the owning member, and prepend our window group
					{
					if(childWinGroup->iQueue)
						{
						childWinGroup->iQueue->AddFirst(*this);
						iQueue=childWinGroup->iQueue;
						}
					else
						{
						TDblQue<CWsWindowGroup>* queue=new TDblQue<CWsWindowGroup>(_FOFF(CWsWindowGroup,iChainLink));
						TInt ret=KErrNoMemory;
						if (queue)
							{
							ret=iChains.Append(queue);
							if(ret!=KErrNone)
								{
								delete queue;
								}
							}
						if(ret!=KErrNone)
							{
							SetReply(ret);
							break;
							}
						queue->AddFirst(*childWinGroup);
						childWinGroup->iQueue=queue;
						queue->AddFirst(*this);
						iQueue=queue;						
						}						
					}
				if (Screen())
					{
				MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();						
				if (windowTreeObserver)
					{
					windowTreeObserver->WindowGroupChained(*this, *childWinGroup);
						}
					}
				}
				break;
			default:						// All other window commands disallowed
				OwnerPanic(EWservPanicOpcode);
			}
		}
#if defined(_DEBUG)
	rootWindow->CheckTree();
#endif
	}
	
TPoint CWsWindowGroup::Origin() const
	{
	return TPoint(0,0);
	}
	
TRect CWsWindowGroup::AbsRect() const
	{
	return (TRect(RootWindow()->Abs().iTl,RootWindow()->Size()));
	}

TSize CWsWindowGroup::Size() const
	{
	return RootWindow()->Size();
	}

void CWsWindowGroup::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{	
	aWindowTreeObserver.NodeCreated(*this, ParentNode());
	}

void CWsWindowGroup::SendStateWindowGroupChain(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	if ( iQueue && (!iQueue->IsEmpty()) && (iQueue->First()==this) )
		{
		TDblQueIter<CWsWindowGroup> iter(*iQueue);
		CWsWindowGroup* groupParent;
		CWsWindowGroup* groupChild;
		
		while ( (groupParent=iter++)!=NULL && (groupChild=iter)!=NULL )
			{
			aWindowTreeObserver.WindowGroupChained(*groupParent, *groupChild);
			}
		}
	}

TInt CWsWindowGroup::Identifier() const
	{
	return iIdentifier;
	}

TPtrC CWsWindowGroup::Name() const
	{
	return (iGroupName) ? *iGroupName : KNullDesC();
	}

TBool CWsWindowGroup::IsFocusable() const
	{
	return ReceivesFocus();
	}

TInt CWsWindowGroup::OrdinalPriority() const
	{
	return iOrdinalPriorityBase;
	}

const MWsClient * CWsWindowGroup::Client() const
    {
    return static_cast<const MWsClient*>(WsOwner());
    }


void CWsWindowGroup::UpdateKeyClickState()
	{
	CClick::SetKeyClickOveride(iFlags&EGroupFlagDisableKeyClick);
	}

void CWsWindowGroup::AreaCovered(TRegion &aRegion)
	{
	aRegion.Clear();
	for(CWsClientWindow *win=Child();win;win=win->NextSibling())
		aRegion.Union(*win->BaseArea());
	}

void CWsWindowGroup::SetOrdinalPosition(TInt aPos)
	{
	if (aPos==(TInt)KOrdinalPositionSwitchToOwningWindow)
		SwitchToOwningWindow(NULL);
	else
		SetOrdinalPosition(aPos,NULL);
	}

TBool CWsWindowGroup::SetOrdinalPosition(TInt aPos,CWsWindowGroup* aClosingWindow)
	{
	TBool ret=ETrue;
	
	// Remember if the window group tree is actually changed or not, so that we know whether to 
	// check the render orientation after the re-ordering is done ( see CWsTop::CheckRenderOrientation()
	// call at end of this method
	TBool changed = CheckOrdinalPositionChange(aPos);	
	
	if (!iQueue)
		ret=DoSetOrdinalPosition1(aPos,aClosingWindow);
	else
		{
		TDblQueIter<CWsWindowGroup> iter(*iQueue);
		CWsWindowGroup* group;
		iter.SetToLast();
		TInt after=0;
		TInt before=0;
		TInt* inc=&before;
		while ((group=iter--)!=NULL)
			{
			if (group==this)
				inc=&after;
			++(*inc);
			}
		TInt lastWinGpPos=NumWindowGroupsOnMyScreen(OrdinalPriority())-after;
		if (aPos<0)
			aPos=lastWinGpPos;
		else
			aPos=Min(aPos,lastWinGpPos);
		aPos-=before;
		aPos=Max(aPos,0);
		iter.SetToLast();
		CWsWindowGroup* firstForward=iter--;
		while (firstForward && firstForward->OrdinalPosition(EFalse)<aPos)
			{
			firstForward=iter--;
			++aPos;
			}
		if (!firstForward)
			iter.SetToFirst();
		else
			{
			iter.Set(*firstForward);
			MoveChainedWindows(iter,ETrue,aPos,aClosingWindow);
			iter.Set(*firstForward);
			iter++;
			}
		MoveChainedWindows(iter,EFalse,--aPos,aClosingWindow);
#if defined(_DEBUG)
		iter.SetToLast();
		TInt pos1=-1;
		TInt pos2;
		TBool ok=ETrue;
		while ((group=iter--)!=this)
			{
			pos2=group->OrdinalPosition(EFalse);
			if (pos2<=pos1)
				ok=EFalse;
			pos1=pos2;
			}
		WS_ASSERT_DEBUG(ok, EWsPanicGroupWindowChainError);
#endif
		}

	// If the ordinal positions have changed, check to see if there is a new render orientation
	// and publish it if so
	if(changed)
	    CWsTop::CheckRenderOrientation();
	
	return ret;
	}


void CWsWindowGroup::MoveChainedWindows(TDblQueIter<CWsWindowGroup>& aIter,TBool aForward,TInt aPos,CWsWindowGroup* aClosingWindow)
	{
	CWsWindowGroup* groupWindow;
	while ((groupWindow=(aForward ? aIter-- : aIter++))!=NULL)
		{
		groupWindow->DoSetOrdinalPosition1(aPos,aClosingWindow);
		(aForward ? ++aPos : --aPos);
		}
	}

TBool CWsWindowGroup::DoSetOrdinalPosition1(TInt aPos,CWsWindowGroup* aClosingWindow)
	{
	TBool ret=EFalse;
	if (CheckOrdinalPositionChange(aPos))
		{
		if (Child())	// A group window with no children can not affect shadows
			{
			ret=ETrue;
			}
		DoSetOrdinalPosition2(aPos,aClosingWindow);
		}
	else if (aClosingWindow)    // do not reset focus if current groupwindow did not change its ordinal position
		iScreen->ResetFocus(aClosingWindow);
	return ret;
	}

void CWsWindowGroup::DoSetOrdinalPosition2(TInt aPos, CWsWindowGroup *aClosingWindow)
	{
	ChangeWindowPosition(aPos,iParent);
	ResetFocus(aClosingWindow);
	}

void CWsWindowGroup::LostFocus()
	{
	iTextCursor.LostFocus();
	iWsOwner->UpdateWindowOrdinalPrioritys();
	if (iClientHandle!=0)
		QueueEvent(EEventFocusLost);
	TWalkWindowTreeFocusChanged wwt(EFalse);
	WalkWindowTree(wwt,EWalkChildren);
	iWsOwner->SetClientPriority();
	}

void CWsWindowGroup::ReceivedFocus()
	{
	iWsOwner->UpdateWindowOrdinalPrioritys();
	iTextCursor.ReceivedFocus();
	// Used for event queue testing
	// Calling MoveToFront sets the queue of the focused window to first place, 
	// not doing so puts the queues in unusual situation thus stress testing the queue code. 
	// One such situation is where the focus queue is first but there is a gap before it (iEventPtr>iGlobalEventPtr)" 
#if defined(_DEBUG)
	if ((iEventQueueTest) && (++iSkipCount==5))
		{
		iSkipCount=0;
		}
	else
		{
		WsOwner()->EventQueue()->MoveToFront();
		}
#else
	WsOwner()->EventQueue()->MoveToFront();
#endif

	QueueEvent(EEventFocusGained);
	TWalkWindowTreeFocusChanged wwt(ETrue);
	WalkWindowTree(wwt,EWalkChildren);
	iWsOwner->SetClientPriority();
	UpdateKeyClickState();
	}

TInt CWsWindowGroup::NumWindowGroups(TBool aAllPriorities, TInt aPriority)
	{
	TInt count=0;
	TInt screenNo;
	for(screenNo=0;screenNo<CWsTop::NumberOfScreens();++screenNo)
		{
		count+=CWsWindowGroup::NumWindowGroupsOnScreen(CWsTop::Screen(screenNo)->RootWindow()->Child(),aAllPriorities,aPriority);
		}
	return(count);
	}

TInt CWsWindowGroup::NumWindowGroupsOnScreen(const CWsWindowGroup* aGroupWin,TBool aAllPriorities,TInt aPriority)
	{
	TInt count=0;
	while (aGroupWin)
		{
		if (aAllPriorities || aGroupWin->iOrdinalPriority==aPriority)
			++count;
		aGroupWin=aGroupWin->NextSibling();
		}
	return count;
	}

inline TInt CWsWindowGroup::NumWindowGroupsOnMyScreen(TInt aPriority)
	{
	return(CWsWindowGroup::NumWindowGroupsOnScreen(Parent()->Child(),EFalse,aPriority));
	}

void CWsWindowGroup::GetFocusWindowGroupL(TInt aScreenNumber)
	{
	CWsWindowGroup *groupWin=(aScreenNumber==KDummyScreenNumber)?CWsTop::FocusWindowGroup():CWsTop::Screen(aScreenNumber)->FocusWindowGroup();	
	if (!groupWin)
		User::Leave(KErrGeneral);
	CWsClient::SetReply(groupWin->Identifier());
	}

TInt CWsWindowGroup::GetWindowGroupListL(TInt aScreenNo,TBool aAllPriorities,TInt aPriority,TInt aCount,CArrayFixFlat<TInt>* aList)
	{
	TInt count=aList->Count();
	CWsWindowGroup* groupWin=CWsTop::Screen(aScreenNo)->RootWindow()->Child();
	while(!aAllPriorities && groupWin && groupWin->iOrdinalPriority!=aPriority)
		groupWin=groupWin->NextSibling();
	while(groupWin && (aAllPriorities || groupWin->iOrdinalPriority==aPriority) && count<aCount)
		{
		aList->AppendL(groupWin->Identifier());
		++count;
		groupWin=groupWin->NextSibling();
		}
	return count;
	}
	
TInt CWsWindowGroup::SendWindowGroupListL(TInt aScreenNumber, TBool aAllPriorities, TInt aPriority, TInt aCount)
	{
	if ((aCount<1) || (aCount>(KArrayMaxGranularity/sizeof(TInt))))
		User::Leave(KErrArgument);
	CArrayFixFlat<TInt>* list=new(ELeave) CArrayFixFlat<TInt>(aCount);
	CleanupStack::PushL(list);
	TInt count(0);
	TInt requestedScreen=aScreenNumber;	
	if(requestedScreen==KDummyScreenNumber)
		{
		// get list from current focus screen first
		TInt focusScreenNo=CWsTop::CurrentFocusScreen()->ScreenNumber();
		count=GetWindowGroupListL(focusScreenNo, aAllPriorities, aPriority, aCount, list);
		if(count<aCount)
			{
			// now get from the remaining screen
			TInt screenNo;
			for(screenNo=0;screenNo<CWsTop::NumberOfScreens() && count<aCount;++screenNo)
				{
				// skip focus screen
				if (screenNo==focusScreenNo)
					continue;
				// count hold total number of window groups collected so far
				count=GetWindowGroupListL(screenNo, aAllPriorities, aPriority, aCount, list);
				}			
			}
		}
	else
		{
		count=GetWindowGroupListL(requestedScreen, aAllPriorities, aPriority, aCount, list);	
		}
	if (list->Count() > 0)	
		CWsClient::ReplyBuf(&list->At(0),count*sizeof(TInt));
	CleanupStack::PopAndDestroy(list);
	return(count);	// How many actually returned, may be less than asked for, but not more
	}

void CWsWindowGroup::GetWindowGroupListAndChainL(TInt aScreen,TBool aAllPriorities,TInt aPriority
																,RArray<RWsSession::TWindowGroupChainInfo>& list,TInt& aCountLeft)
	{
	CWsWindowGroup *groupWin=CWsTop::Screen(aScreen)->RootWindow()->Child();
	while(!aAllPriorities && groupWin && groupWin->iOrdinalPriority!=aPriority)
		groupWin=groupWin->NextSibling();
	while(groupWin && (aAllPriorities || groupWin->iOrdinalPriority==aPriority) && aCountLeft>0)
		{
		RWsSession::TWindowGroupChainInfo windowId;
		windowId.iId=groupWin->Identifier();
		if(!groupWin->IsChained(windowId.iParentId))
			windowId.iParentId=-1;	//Unchained window group
		list.AppendL(windowId);
		--aCountLeft;
		groupWin=groupWin->NextSibling();
		}
	}

TInt CWsWindowGroup::SendWindowGroupListAndChainL(TBool aAllPriorities, TInt aPriority, TInt aCount)
	{
	if ((aCount<1) || (aCount>(KArrayMaxGranularity/sizeof(RWsSession::TWindowGroupChainInfo))))
		User::Leave(KErrArgument);
	RArray<RWsSession::TWindowGroupChainInfo> list(aCount);
	CleanupClosePushL(list);
	TInt count=aCount;
	TInt focusScreenNo=CWsTop::CurrentFocusScreen()->ScreenNumber();
	GetWindowGroupListAndChainL(focusScreenNo,aAllPriorities,aPriority,list,count);
	TInt screenNo;
	for(screenNo=0;screenNo<CWsTop::NumberOfScreens();++screenNo)
		{
		if (screenNo!=focusScreenNo)
			GetWindowGroupListAndChainL(screenNo,aAllPriorities,aPriority,list,count);
		}
	if (list.Count()>0)
		CWsClient::ReplyBuf(&list[0],aCount*sizeof(RWsSession::TWindowGroupChainInfo));
	CleanupStack::PopAndDestroy(&list);
	return(aCount-count);	// How many actually returned, may be less than asked for, but not more
	}

TBool CWsWindowGroup::SendEventToAllGroups(TBool aAllPriorities,TBool aOnePerClient,const TWsClCmdSendEventToWindowGroup& aData)
	{
	TWsEvent event=aData.event;
	if (event.Type()==EEventKey && event.Key()->iRepeats!=0)
		CKeyboardRepeat::CancelRepeat(NULL);		//Otherwise we will trip an invarient
	TInt priority=aData.parameter;
	TBool sentToAll=ETrue;
	TInt screenNo;
	for(screenNo=0;screenNo<CWsTop::NumberOfScreens();++screenNo)
		{
		CWsWindowGroup *groupWin=CWsTop::Screen(screenNo)->RootWindow()->Child();
		if (!aAllPriorities)
			{
			while(groupWin && groupWin->iOrdinalPriority!=priority)
				groupWin=groupWin->NextSibling();
			}
		CWsWindowGroup* firstGroupWin=groupWin;
		CWsClient* lastOwner=NULL;
		CWsWindowGroup* groupWin2;
		while(groupWin && (aAllPriorities || groupWin->iOrdinalPriority==priority))
			{
			if (aOnePerClient)
				{
				if (lastOwner==groupWin->iWsOwner)
					goto ContinueLoop;
				lastOwner=groupWin->iWsOwner;
				for(groupWin2=firstGroupWin;groupWin2!=groupWin;groupWin2=groupWin2->NextSibling())
					{
					if (groupWin2->iWsOwner==groupWin->iWsOwner)
						break;
					}
				if (groupWin2->iWsOwner==groupWin->iWsOwner && groupWin2!=groupWin)
					goto ContinueLoop;
				}
			event.SetHandle(groupWin->ClientHandle());
			if (!groupWin->EventQueue()->QueueEvent(event))
				sentToAll=EFalse;
		ContinueLoop:
			groupWin=groupWin->NextSibling();
			}
		}
	return sentToAll;
	}

void CWsWindowGroup::ReleasePendedMessagesToAllGroups(CWsClient * aClient)
	{
	TInt screenNo;
	for (screenNo = 0; screenNo < CWsTop::NumberOfScreens(); ++screenNo)
		{
		CWsWindowGroup* groupWin = CWsTop::Screen(screenNo)->RootWindow()->Child();
		while (groupWin)
			{
			if (groupWin->WsOwner() == aClient)
				{
				groupWin->ReleasePendedMessage();
				}
			groupWin = groupWin->NextSibling();
			}
		}
	}

void CWsWindowGroup::ReleasePendedMessage()
    {
    if (iMessageArray->Count() > 0 && !(iFlags & EGroupFlagMessageSignalled))
        {
        if (!SignalMessageReady())
			{
			//The event queue is overflow
			// Cannot send a message notification event. 
			WsOwner()->WgMsgQueueOverflow(); // Set flag for client about having pended message(s)
			}
        }
    }

void CWsWindowGroup::SendMessageToAllGroupsL(CWsClient& aSender,TBool aAllPriorities,const TWsClCmdSendMessageToWindowGroup& aData)
	{
	TInt screenNo;
	for(screenNo=0;screenNo<CWsTop::NumberOfScreens();++screenNo)
		{
		CWsWindowGroup* groupWin=CWsTop::Screen(screenNo)->RootWindow()->Child();		
		if (!aAllPriorities)
			{
			while(groupWin && groupWin->iOrdinalPriority!=aData.identifierOrPriority)
				groupWin=groupWin->NextSibling();
			}
		while(groupWin && (aAllPriorities || (groupWin->iOrdinalPriority==aData.identifierOrPriority)))
			{
			groupWin->QueueMessageL(aData.uid, aData.dataLength, aSender);
			groupWin=groupWin->NextSibling();
			}
		}
	}

CWsWindowGroup *CWsWindowGroup::WindowGroupFromIdentifier(TInt aIdentifier)
	{
	// apply to all screens
	TInt screenNo;
	for (screenNo=0; screenNo<CWsTop::NumberOfScreens(); ++screenNo)
		{
		CWsWindowGroup* group;
		for(group=CWsTop::Screen(screenNo)->RootWindow()->Child(); group; group=group->NextSibling())
			{
			if (group->Identifier() == aIdentifier)
				return group;
			}
			
		}
		
	return NULL;
	}

CWsWindowGroup *CWsWindowGroup::WindowGroupFromIdentifierL(TInt aIdentifier)
	{
	CWsWindowGroup *group=WindowGroupFromIdentifier(aIdentifier);
	if (!group)
		User::Leave(KErrNotFound);
	return(group);
	}

CWsWindowGroup *CWsWindowGroup::FindWindowGroupL(CWsClient* aClient, TInt aIdentifier,TInt aOffset,const TPtrC *aMatch,const TThreadId *aThreadId)
	{
	CWsWindowGroup *group;
	if (aIdentifier)
		{
		group=WindowGroupFromIdentifier(aIdentifier);
		if (group)	// NULL group will cause KErrNotFound to be returned
			group=group->NextSibling();
		}
	else
		{
		// get window group for this session
		//
		group = aClient->Screen()->RootWindow()->Child();
		}

	for(;group;group=group->NextSibling())
		{
		if (aThreadId)
			{
			if (group->WsOwner()->Client().Id()==*aThreadId)
				break;	// Found one
			}
		else
			{
			const TDesC *groupName=&nullDescriptor;
			if (group->GroupName())
				groupName=group->GroupName();
			if (groupName->Length()>=aOffset && groupName->Mid(aOffset).MatchF(*aMatch)>=0)
				break;	// Found one
			}
		}
	if (!group)
		User::Leave(KErrNotFound);
	return(group);
	}

void CWsWindowGroup::AddPriorityKeyL(TUint aKeycode, TUint aModifierMask, TUint aModifiers)
	{
	iPriorityKeys=new(ELeave) TPriorityKey(aKeycode,aModifierMask,aModifiers,iPriorityKeys);
	}

void CWsWindowGroup::RemovePriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifiers)
	{
	for(TPriorityKey **ppk=&iPriorityKeys;*ppk;ppk=&((*ppk)->iNext))
		if ((*ppk)->Equals(aKeycode, aModifierMask, aModifiers))
			{
			TPriorityKey *next=(*ppk)->iNext;
			delete *ppk;
			*ppk=next;
			break;
			}
	}

void CWsWindowGroup::RemoveAllPriorityKeys()
	{
	TPriorityKey *pk=iPriorityKeys;
	while(pk)
		{
		TPriorityKey *next=pk->iNext;
		delete pk;
		pk=next;
		}
	}

TBool CWsWindowGroup::CheckForPriorityKey(const TKeyEvent &aKeyEvent)
	{
	for(TPriorityKey *pk=iPriorityKeys;pk;pk=pk->iNext)
		{
		if (pk->KeyMatches(aKeyEvent))
			{	
			WsOwner()->PriorityKeyPressed(ClientHandle(), aKeyEvent);
			return(ETrue);
			}
		}
	return(EFalse);
	}

void CWsWindowGroup::StatusDump(TDes &aBuf)
	{
	_LIT(KWSERVStatusDumpWindowGroupInfo,"CWsWindowGroup[0x%x]RWindowGroup[0x%x,%d],Pri=%d,Id=%d,SizeMode=%d");
	aBuf.AppendFormat(KWSERVStatusDumpWindowGroupInfo,this,iClientHandle,LogHandle(),iOrdinalPriority,iIdentifier,iScreenDevice?iScreenDevice->AppMode():0);
	}

TBool CWsWindowGroup::SignalMessageReady()
	{
	TWsEvent event;
	event.SetType(EEventMessageReady);
	event.SetHandle(ClientHandle());
	event.SetTimeNow();
	SEventMessageReady& eventMessageReady=*(SEventMessageReady*)event.EventData();
	eventMessageReady.iWindowGroupIdentifier=Identifier();
	eventMessageReady.iMessageUid=(*iMessageArray)[0].iUid;
	eventMessageReady.iMessageParametersSize=iMessageArray->Length(0)-sizeof(TUid);
	TBool result = WsOwner()->EventQueue()->QueueEvent(event, EEventPriorityHigh);
	if (result)
		{
		iFlags |= EGroupFlagMessageSignalled;
		}
	return result;
	}

void CWsWindowGroup::QueueMessageL(TUid aUid, TInt aDataLength, CWsClient& aSender)
	{
	WS_ASSERT_DEBUG(iFlags&(EGroupFlagMsgQueueActive|EGroupFlagMsgQueueNew) || iMessageArray->Count()>=1,EWsPanicMsgQueueError);
	if (!(iFlags&(EGroupFlagMsgQueueActive|EGroupFlagMsgQueueNew)) && iMessageArray->Count()>=KMaxNumberOfMsgsInInactiveQueue)
		{
		WS_ASSERT_DEBUG(iMessageArray->Count()<=KMaxNumberOfMsgsInInactiveQueue,EWsPanicMsgQueueError);
		iMessageArray->Delete(1,iMessageArray->Count()-1);
		}
	TWsMessage* message=NULL;
	TRAPD(err,message=&iMessageArray->ExtendL(aDataLength+sizeof(aUid)));
	if ((err || (iFlags&EGroupFlagMsgQueueNew)) && iMessageArray->Count()>KMaxNumberOfMsgsInQueue)
		{
		iFlags&=~(EGroupFlagMsgQueueActive|EGroupFlagMsgQueueNew);
		iMessageArray->Delete(1,iMessageArray->Count()-(err?1:2));
		iMessageArray->Compress();
		}
	User::LeaveIfError(err);
	if (message)
		{
		message->iUid=aUid;
		TPtr8 ptr(&message->iTheRest[0],aDataLength);
		TRAP(err,aSender.RemoteReadL(ptr,0));
		if (err)
			{
			iMessageArray->Delete(iMessageArray->Count()-1);
			User::Leave(err);
			}
		if (!(iFlags & EGroupFlagMessageSignalled))
			{
			if (!SignalMessageReady())
				{
				//The event queue is overflow
				// Cannot send a message notification event. 
				WsOwner()->WgMsgQueueOverflow(); // Set flag for client about having pended message(s)
				}
			}
		}
	}

void CWsWindowGroup::FetchMessageL()
	{
	if (!(iFlags & EGroupFlagMessageSignalled))
		{
		OwnerPanic(EWservPanicFetchMessage);
		}
	CWsClient::ReplyBuf(&((*iMessageArray)[0].iTheRest[0]), (TInt) iMessageArray->Length(0) - sizeof(TUid));
	iMessageArray->Delete(0);
	iFlags |= EGroupFlagMsgQueueActive;
	iFlags &= ~(EGroupFlagMessageSignalled | EGroupFlagMsgQueueNew);
	if (iMessageArray->Count() > 0)
		{
		if (!SignalMessageReady())
			{
			//The event queue is overflow
			// Cannot send a message notification event. 
			WsOwner()->WgMsgQueueOverflow(); // Set flag for client about having pended message(s)
			}
		}
	}

TBool CWsWindowGroup::ScreenDeviceValid() const
	{
	return(iScreenDevice?iScreenDevice->ScreenDeviceValidState():(iScreen->ScreenSizeMode()==0));
	}

TBool CWsWindowGroup::CanReceiveFocus() const
	{
	return(ReceivesFocus() && iWsOwner->NotClosing() && (ScreenDeviceValid() || iFlags&EGroupFlagHandlesDeviceChange));
	}

void CWsWindowGroup::SetScreenChangeEventStateL(TBool aEnabled)
	{
	iFlags&=~EGroupFlagHandlesDeviceChange;
	if (aEnabled)
		{
		iFlags|=EGroupFlagHandlesDeviceChange;
		TWindowServerEvent::AddToScreenDeviceChangeEventListL(*this);
		if (iScreen->ScreenSizeMode()!=0)
			TWindowServerEvent::SendScreenDeviceChangedEvent(this);
		}
	else
		TWindowServerEvent::RemoveFromScreenDeviceChangeEventList(*this);
	iScreen->ResetFocus(NULL);
	}

void CWsWindowGroup::SetScreenDeviceValidState(const DWsScreenDevice *aDevice)
	{
	if (iScreenDevice==aDevice)
		{
		TBool state=ScreenDeviceValid();
		for(CWsTopClientWindow *win=Child();win;win=win->NextSiblingTop())
			{
			win->SetScreenDeviceValidState(state);
			}
		}
	}

void CWsWindowGroup::SetScreenDeviceValidStates(const DWsScreenDevice *aDevice)
	{
	for(CWsWindowGroup *groupWin=aDevice->RootWindow()->Child();groupWin;groupWin=groupWin->NextSibling())
		groupWin->SetScreenDeviceValidState(aDevice);
	}

void CWsWindowGroup::SetScreenDeviceValidStates(CScreen* aScreen)
	{
	CWsRootWindow* rootWindow = aScreen->RootWindow();

	CWsWindowGroup* groupWin;
	CWsTopClientWindow* win;
	for(groupWin=rootWindow->Child();groupWin;groupWin=groupWin->NextSibling())
		{
		TBool state=groupWin->ScreenDeviceValid();
		for(win=groupWin->Child();win;win=win->NextSiblingTop())
			{
			win->SetScreenDeviceValidStateFlag(state);
			win->ResetHiddenFlagsInParentAndChildren();
			}
		}
	}

void CWsWindowGroup::SetScreenDevice(DWsScreenDevice *aDevice)
	{
	iScreenDevice=aDevice;	
	}

void CWsWindowGroup::NewOrientation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation, CWsRootWindow* aRootWindow)
	{
	for(CWsWindowGroup *groupWin=aRootWindow->Child();groupWin;groupWin=groupWin->NextSibling())
		{
		DWsScreenDevice *device=groupWin->Device();
		if (device)
			device->NewOrientation(aMode,aRotation);
		}
	}

void CWsWindowGroup::ResetFocus(CWsWindowGroup *aClosingWindow)
	{
	if (iScreen)
		{
		iScreen->ResetFocus(aClosingWindow);
		}
	}

TBool CWsWindowGroup::IsChained(TInt& aParentId)
	{
	if (!iQueue)
		return EFalse;
	if (iQueue->First()==this)
		aParentId=0;
	else
		aParentId=BeforeInChain()->Identifier();
	return ETrue;
	}

inline CWsWindowGroup* CWsWindowGroup::BeforeInChain()
	{		//You should only call this function if you know the window has a parent
	return reinterpret_cast<CWsWindowGroup*>(PtrSub(iChainLink.iPrev,_FOFF(CWsWindowGroup,iChainLink)));
	}

TBool CWsWindowGroup::CheckCapability(TInt& aOrdinalPriority)
	{
	if(aOrdinalPriority>=KPasswordWindowGroupPriority)
		{
		if(!KSecurityPolicy_SwEvent().CheckPolicy(WsOwner()->Client(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed")))
			{
			aOrdinalPriority=KPasswordWindowGroupPriority-1;
			return EFalse;
			}
		}
 	return ETrue;
	}
	
TBool CWsWindowGroup::HasVisibleTranslucentChild()
	{
	CWsWindowBase * child = iChild;
	while (child)
		{
		if (child->WinType() == EWinTypeClient)
			{
			CWsClientWindow * cliwin = static_cast<CWsClientWindow *>(child);
			if (cliwin->IsTranslucent() && cliwin->IsVisible())
				return ETrue;
			}
		else if (static_cast<CWsWindowGroup*>(child)->HasVisibleTranslucentChild())
			{
			return ETrue;
			}
		child = child->NextSibling();
		}
	return EFalse;
	}


TInt CWsWindowGroup::NumClientWindowGroups()
	{
	CWsObjectIx& objix=*WsOwner()->ObjectIndex();
	const TWsObject* ptr=objix.FirstObject();
	const TWsObject* end=ptr+objix.Length();
	TInt count=0;
	while(++ptr<end)		//First one should always have a NULL object
		{
		const CWsObject* obj=ptr->iObject;
		if (obj && obj->Type()==WS_HANDLE_GROUP_WINDOW)
			++count;
		}
	return count;
	}
	
void CWsWindowGroup::SetEventQueueTestState(TBool aEventQueState)
	{
	iEventQueueTest = aEventQueState;
	}
