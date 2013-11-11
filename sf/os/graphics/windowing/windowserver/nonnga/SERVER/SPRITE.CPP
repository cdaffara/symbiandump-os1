// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Sprite
// 
//

#include <e32std.h>
#include "sprite.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "ScrDev.H"
#include "wstop.h"
#include "ANIM.H"
#include "offscreenbitmap.h"
#include "panics.h"
#include "EVENT.H"

static const TInt64 KFlashHalfSecond(500000); //interval for flashing sprites

GLDEF_D CWsDeltaTimer *CWsSpriteBase::iDeltaTimer=NULL;

TInt TimerCallBack(TAny *aPtr)
	{
	((CWsSpriteBase *)aPtr)->TimerExpired();
	return(KErrNone);
	}

//
// CWsSpriteMember
//

CWsSpriteMember::CWsSpriteMember()
	{
	}

CWsSpriteMember::~CWsSpriteMember()
	{
	delete iBitmap;
	delete iMaskBitmap;
	}

TBool CWsSpriteMember::SetL(const TCmdSpriteMember &aCmdSpriteMember)
	{
	if (aCmdSpriteMember.iBitmap!=0)	// Null member of sequence, time is only valid field in member data
		{
		iBitmap=new(ELeave) CFbsBitmap();
		TInt ret = iBitmap->Duplicate(aCmdSpriteMember.iBitmap);
		if (ret == KErrNoMemory)
			{
			User::Leave(ret);
			}
		if (ret != KErrNone)
			return(ETrue);

		if (aCmdSpriteMember.iMaskBitmap)
			{
			iMaskBitmap=new(ELeave) CFbsBitmap();
			TInt ret = iMaskBitmap->Duplicate(aCmdSpriteMember.iMaskBitmap);
			if (ret == KErrNoMemory)
				{
				User::Leave(ret);
				}
			if (ret != KErrNone)
				return(ETrue);
			}
		iInvertMask=aCmdSpriteMember.iInvertMask;
		iDrawMode=aCmdSpriteMember.iDrawMode;
		iOffset=aCmdSpriteMember.iOffset;
		}
	iInterval=aCmdSpriteMember.iInterval;
	return(EFalse);
	}

//
// CWsSpriteBase
//

TBool CWsSpriteBase::UpdateMemberL(CWsSpriteMember *aMember, const TCmdSpriteMember &aCmdSpriteMember)
	{
	CFbsBitmap *old=aMember->iBitmap;
	CFbsBitmap *oldMask=aMember->iMaskBitmap;
	aMember->iBitmap=NULL;
	aMember->iMaskBitmap=NULL;
	TBool ret=EFalse;
	TRAPD(err,ret=aMember->SetL(aCmdSpriteMember));
	if (err!=KErrNone)
		{
um_error:
		delete aMember->iBitmap;
		delete aMember->iMaskBitmap;
		aMember->iBitmap=old;
		aMember->iMaskBitmap=oldMask;
		User::Leave(err);
		}
	TRAP(err,CheckSizesL());
	if (err!=KErrNone)
		goto um_error;
	SetMember(0);
	delete old;
	delete oldMask;
	return(ret);
	}

void CWsSpriteBase::InitStaticsL()
	{
	iDeltaTimer=CWsDeltaTimer::NewL(ESpriteAnimatePriority);
	}

void CWsSpriteBase::DeleteStatics()
	{
	delete iDeltaTimer;
	}

CWsSpriteBase::CWsSpriteBase(CWsClient *owner, WH_HANDLES aType) : CWsScreenObject(owner,aType,owner->Screen()), iClipSprite(EFalse)
	{
	}

CWsSpriteBase::~CWsSpriteBase()
	{
	Deactivate();

	//iDeltaTimer->Remove(iDeltaTimerEntry);
	if (iMembers)
		{
		iMembers->ResetAndDestroy();
		delete iMembers;
		}
	}

void CWsSpriteBase::ForceRedraw()
	{
    TRegionFix<1> region;
    region.AddRect(Rect());
    Screen()->AddRedrawRegion(region);
	}

void CWsSpriteBase::Deactivate()
	{
	//Disconnect from the sprite list and hide the sprite
	if (iFlags & ESpriteActive)
		{
		if(iWin)
			iWin->RemoveSprite(this);
		if (iMembers && iMembers->Count()>1)
			iDeltaTimer->Remove(iDeltaTimerEntry);
		iFlags&=~ESpriteActive;
		if (iFloating)
			{
			Screen()->SpriteManager()->RemoveFloatingSprite(this);
			ForceRedraw();
			}	
		}
	}

void CWsSpriteBase::CheckSizesL()
	{
	iMaxSize.SetSize(0,0);
	for(TInt index=0;index<iMembers->Count();index++)
		{
		CWsSpriteMember *wsm=(*iMembers)[index];
		if (wsm->iBitmap)
			{
			TSize size=wsm->iBitmap->SizeInPixels();
			if (wsm->iMaskBitmap)
				{
				TSize maskSize=wsm->iMaskBitmap->SizeInPixels();
				if (maskSize.iWidth<size.iWidth || maskSize.iHeight<size.iHeight)
					OwnerPanic(EWservPanicMaskSize);
				}
			if (size.iWidth>iMaxSize.iWidth)
				iMaxSize.iWidth=size.iWidth;
			if (size.iHeight>iMaxSize.iHeight)
				iMaxSize.iHeight=size.iHeight;
			}
		}
	}

void CWsSpriteBase::ConstructL(TUint aFlags, CWsWindow *aWindow)
	{
	// Common part of construct for both sprites and pointer cursors
	iFlags=aFlags;
/*
	if (iFlags&ESpriteNoChildClip)
		iLink.iPriority+=ENoChildPriorityBoost;
	if (iFlags&ESpritePointer)
		iLink.iPriority+=EPointerPriorityBoost;
*/
	iWin=aWindow;
	TCallBack callback(TimerCallBack,this);
	iDeltaTimerEntry.Set(callback);
	iMembers=new(ELeave) CArrayPtrFlat<CWsSpriteMember>(10);
	}

void CWsSpriteBase::AppendMemberL(const TCmdSpriteMember &aCmdSpriteMember)
	{
	CWsSpriteMember *&pwsm=iMembers->ExtendL();
	pwsm=NULL;	// In case new leaves
	pwsm=new(ELeave) CWsSpriteMember();
	if (pwsm->SetL(aCmdSpriteMember))
		OwnerPanic(EWservPanicBitmap);
	}

void CWsSpriteBase::CompleteL()
	{
	if (iMembers->Count() <= 0)
		{
		if(iWin)
			iWin->OwnerPanic(EWservPanicNoSpriteMember);
		//Not sure if this is a neccessary fall back if iWin is NULL.
		else if(iWin==NULL && iGroupWin)
			iGroupWin->OwnerPanic(EWservPanicNoSpriteMember);
		}
	else
		{
		iMembers->Compress();
		CheckSizesL();
		SetMember(0);
		}
	}

void CWsSpriteBase::Activate()
	{
	if (iFlags&ESpriteDisabled)
		{
		iFlags&=~ESpriteDisabled;
		}
	if (iMembers->Count()>1)
		{
		QueueDeltaTimer();
		iDeltaTimer->Activate();
		}
	iFlags|=ESpriteActive;
	if(iWin)	
		iWin->AddSprite(this);
	Screen()->SpriteManager()->Schedule(this);
	if(iFloating)
		{
		Screen()->SpriteManager()->AddFloatingSprite(this);
		ForceRedraw();
		}
	}

void CWsSpriteBase::SetMember(TInt aIndex)
	{
	iCurIndex=aIndex;
	iPos=iBasePos+(*iMembers)[iCurIndex]->iOffset;
	if ((*iMembers)[iCurIndex]->iBitmap)
		iSize=(*iMembers)[iCurIndex]->iBitmap->SizeInPixels();
	else
		iSize.SetSize(0,0);

	if (iSize.iWidth > iMaxSize.iWidth || iSize.iHeight > iMaxSize.iHeight)
		{
		WS_ASSERT_DEBUG(EFalse, EWsPanicSpriteBitmapSizeChange);
		CheckSizesL();
		}
	}

void CWsSpriteBase::SetPos(const TPoint &aPos)
	{
	//Non-floating anim whose window is destroyed 
	if (!iFloating && iWin==NULL) 
		{ 
		OwnerPanic(EWservPanicWindowDestroyed); 
		} 

	//Floating anim whose group window is destroyed 
	if (iFloating  && iGroupWin==NULL) 
		{ 
		OwnerPanic(EWservPanicWindowDestroyed); 
		}
		 
	iBasePos=aPos;
	TPoint newPos(iBasePos+(*iMembers)[iCurIndex]->iOffset);
	if (iPos!=newPos)
		{
		//Ensure the region covered by the sprite before as well as after the move gets scheduled for redraw
		Screen()->SpriteManager()->Schedule(this);
		iPos=newPos;
		Screen()->SpriteManager()->Schedule(this);
		}
	}

// Andy - replace delta timer with animation scheduling via screen
void CWsSpriteBase::QueueDeltaTimer()
	{
	iDeltaTimer->Queue((*iMembers)[iCurIndex]->iInterval,iDeltaTimerEntry);
	}

void CWsSpriteBase::TimerExpired()
	{
	Screen()->SpriteManager()->Schedule(this);
	SetMember((iCurIndex+1)==iMembers->Count() ? 0 : iCurIndex+1);
	Screen()->SpriteManager()->Schedule(this);
	QueueDeltaTimer();
	iScreen->Update();
	}

TPoint CWsSpriteBase::Pos() const
	{
	if (iGroupWin || iWin==NULL)
		{
		return(iPos);
		}
	return(iPos+iWin->Origin());
	}

TRect CWsSpriteBase::Rect() const
	{
	TRect rect;
	rect.iTl=Pos();
	rect.iBr=rect.iTl+iSize;
	return(rect);
	}

void CWsSpriteBase::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	TWsSpriteCmdUnion pData;

	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsSpriteOpAppendMember:
			AppendMemberL(*pData.SpriteMember);
			break;
		case EWsSpriteOpActivate:
			if(!(iFlags&ESpriteActive))
				{
				CompleteL();
				}
			break;
		case EWsSpriteOpUpdateMember:
			if (pData.UpdateMember->index==iCurIndex)
				{
				TRect rect(Pos(), iMaxSize);
				Screen()->SpriteManager()->Schedule(this,&rect);
				}
			break;
		case EWsSpriteOpUpdateMember2:
			{
			Screen()->SpriteManager()->Schedule(this);
			if (pData.UpdateMember->index<0 || pData.UpdateMember->index>=iMembers->Count())
				User::Leave(KErrArgument);
			CWsSpriteMember *member=(*iMembers)[pData.UpdateMember->index];
			TBool ret=EFalse;
			TRAPD(err,ret=UpdateMemberL(member,pData.UpdateMember->data));
			if (err==KErrNone)
				{
				TRAP(err,CheckSizesL());
				SetMember(0);
				}
			Screen()->SpriteManager()->Schedule(this);
			User::LeaveIfError(err);
			if (ret)
				OwnerPanic(EWservPanicBitmap);
			}
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
			break;
		}
	}

TBool CWsSpriteBase::CanBeSeen() const
	{
	if(iWin)
		return (!(iFlags&ESpriteDisabled)) && (!iWin->VisibleRegion().IsEmpty());
	else 
		return (!(iFlags&ESpriteDisabled));
	}

void CWsSpriteBase::Redraw(CFbsBitGc * aGc, const TRegion& aRegion)
	{
	TFlashState currentState=EFlashOn;
	if(IsFlashingEnabled())
		currentState=Screen()->SpriteManager()->CurrentSpriteFlashState(this);
	
	if(currentState==EFlashOn)
		{
		STACK_REGION region;
		region.Copy(aRegion);
		const TRegion * pr = &region;
		if (iClipSprite)
			{
			//PeterI iWin shouldn't be null as iClipSprite is currently only set by the text cursor (which is never floating)
			//but just in case make sure we don't derefernce if it is null.
			TPoint origin(0,0);
			if(iWin)
				origin = iWin->Origin();
			TRect rect(iBasePos + origin + iClipOffset, iClipSize);
			region.ClipRect(rect);
			}
		region.ClipRect(RootWindow()->Abs());

		// Only need to draw if the region being redrawn overlaps the sprite
		const TRect spriteRect(Pos(), iSize);
		STACK_REGION spriteRegion;
		spriteRegion.AddRect(spriteRect);
		region.Intersect(spriteRegion);
		spriteRegion.Close();

		if (pr->CheckError())
			{
			if(iWin)
				pr = &iWin->VisibleRegion();
			else
				pr = &RootWindow()->WindowArea();
			}

		if (!pr->IsEmpty())
			{
			CWsSpriteMember *member=(*iMembers)[iCurIndex];
			if (member->iBitmap)
				{
				aGc->SetClippingRegion(pr);

				// Calculate which piece (rect) of the bitmap needs to be drawn
				const TRect redrawRect = pr->BoundingRect();
				TRect bitmapRect(spriteRect); // sprite rect relative to screen
				bitmapRect.Intersection(redrawRect);
				bitmapRect.Move(-Pos()); // adjust relative to bitmap origin

				if (member->iMaskBitmap)
					aGc->BitBltMasked(Pos() + bitmapRect.iTl, member->iBitmap, bitmapRect, member->iMaskBitmap, member->iInvertMask);
				else
					{
					aGc->SetDrawMode(member->iDrawMode);
					aGc->BitBlt(Pos() + bitmapRect.iTl, member->iBitmap, bitmapRect);
					aGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
					}
				aGc->SetClippingRegion(NULL);
				}
			}
		region.Close();
		}
	//flashing sprites need to reschedule themselves after drawing
	if(IsFlashingEnabled())
		Screen()->SpriteManager()->Schedule(this);
	}

TBool CWsSpriteBase::IsActivated() const
	{
	return (iFlags&ESpriteActive);
	}

//
// CWsSprite
//

CWsSprite::CWsSprite(CWsClient *owner) : CWsSpriteBase(owner,WS_HANDLE_SPRITE)
	{
	}

CWsSprite::~CWsSprite()
	{
    if (!iFloating && IsActivated() && iWin && iWin->IsVisible())
        ForceRedraw();
    
	if (iAnim)
		CWsAnim::CloseAnim(iAnim);
	}

void CWsSprite::ConstructL(const TWsClCmdCreateSprite &aParams)
	{
	NewObjL();
	CWsWindowBase *win;
	WsOwner()->HandleToWindow(aParams.window,&win);
	if (win->WinType()==EWinTypeGroup)
		{
		//If a sprite is attached to a group window it is floating. 
		//Floating sprite drawing is performed by the sprite manager.
		iGroupWin=(CWsWindowGroup *)win;
		win=NULL; //Floating sprites aren't associated with any particular window.
		iFloating=ETrue;
		}
	CWsSpriteBase::ConstructL(aParams.flags&ESpriteNonSystemFlags,(CWsWindow *)win);
	iBasePos=aParams.pos;
	}

void CWsSprite::CompleteL()
	{
	CWsSpriteBase::CompleteL();
	Activate();
	}

void CWsSprite::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	TWsSpriteCmdUnion pData;
	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsSpriteOpSetPosition:
			SetPos(*pData.Point);
			break;
		case EWsSpriteOpFree:
			delete this;
			break;
		default:
			CWsSpriteBase::CommandL(aOpcode, aCmdData);
			break;
		}
	}

/**
@see MAnimSpriteFunctions::UpdateMember
@param aFullUpdate	Not used. Wserv2 always do full back to front rendering, so there is no distinction between changes needing aFullUpdate or not 
 */
void CWsSprite::Update(TInt aMember,TRect aRect,TBool /*aFullUpdate*/) 
	{
	if (iCurIndex!=aMember)
		return;
	aRect.Move(Pos());
	aRect.Intersection(iScreen->CurrentScreenSize());
	Screen()->SpriteManager()->Schedule(this, &aRect);
	}

//
// CWsPointerCursor
//

CWsPointerCursor::CWsPointerCursor(CWsClient *owner) : CWsSpriteBase(owner,WS_HANDLE_POINTER_CURSOR)
	{
	}

void CWsPointerCursor::CloseObject()
	{
	RemoveFromIndex();
	Close();
	}

void CWsPointerCursor::Close()
	{
	WS_ASSERT_DEBUG(iAccessCount>0, EWsPanicPointerCursorAccessCount);
	if (--iAccessCount==0)
		delete this;
	}

void CWsPointerCursor::Open()
	{
	iAccessCount++;
	}

CWsPointerCursor::~CWsPointerCursor()
	{
	WS_ASSERT_DEBUG(iAccessCount==0, EWsPanicPointerCursorAccessCount);
	}

void CWsPointerCursor::ConstructL(const TWsClCmdCreatePointerCursor &aParams)
	{
	NewObjL();
	CWsSpriteBase::ConstructL(ESpriteNoShadows|ESpriteNoChildClip|ESpritePointer|(aParams.flags&ESpriteNonSystemFlags),RootWindow());
	Open();
	}

void CWsPointerCursor::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	switch(aOpcode)
		{
		case EWsSpriteOpFree:
			CloseObject();
			break;
		default:
			CWsSpriteBase::CommandL(aOpcode, aCmdData);
			break;
		}
	}

//
// CWsCustomTextCursor
//

CWsCustomTextCursor::CWsCustomTextCursor (CWsClient *aOwner, RWsSession::TCustomTextCursorAlignment aAlignment) 
: CWsSpriteBase(aOwner, WS_HANDLE_TEXT_CURSOR), iAlignment(aAlignment)
	{
	}

CWsCustomTextCursor::~CWsCustomTextCursor()
	{
	}

void CWsCustomTextCursor::ConstructL(TInt aFlags)
	{
	NewObjL();
	CWsSpriteBase::ConstructL(ESpriteNoShadows|ESpriteNoChildClip|ESpritePointer|(aFlags&ESpriteNonSystemFlags), NULL);
	}

void CWsCustomTextCursor::CompleteL(CWsWindow *aWin, TBool aFlash, TBool aClipSprite, const TPoint& aClipOffset, const TSize& aClipSize)
	{
	iWin = aWin;
	iFlags = aFlash ? iFlags | ESpriteFlash : iFlags & ~ESpriteFlash;
	iClipSprite = aClipSprite;
	iClipOffset = aClipOffset;
	iClipSize = aClipSize;
	CWsSpriteBase::CompleteL();
	}

// Use SetPositionNoRedraw instead of SetPos when you just want to update
// the custom text cursor position without redrawing it
void CWsCustomTextCursor::SetPositionNoRedraw(const TPoint& aPos)
	{
	iBasePos = aPos;
	TPoint newPos(iBasePos+(*iMembers)[iCurIndex]->iOffset);
	iPos=newPos;
	}

void CWsCustomTextCursor::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	switch(aOpcode)
		{
		case EWsSpriteOpFree:
			// CWsCustomTextCursor objects are owned by the text cursor list.
			// They are not deleted when the client closes it's R class.
			RemoveFromIndex();
			break;
		default:
			CWsSpriteBase::CommandL(aOpcode, aCmdData);
			break;
		}
	}

//
// CWsDeltaTimer, nicked from CDeltaTimer and tweaked so it doesn't re-activate			//
// the timers until RunL has finished running all ready timers.							//
//																						//
// This is to stop a problem in Wserv where sprites could hog 100% CPU if the time		//
// it took to process them was longer than the time of the timer queued when the first	//
// sprite was updated																	//
//

CWsDeltaTimer* CWsDeltaTimer::NewL(TInt aPriority)
	{
	CWsDeltaTimer* wsdt=new(ELeave) CWsDeltaTimer(aPriority);
	CleanupStack::PushL(wsdt);
	User::LeaveIfError(wsdt->iTimer.CreateLocal());
	CActiveScheduler::Add(wsdt);
	CleanupStack::Pop(wsdt);
	return(wsdt);
	}

CWsDeltaTimer::CWsDeltaTimer(TInt aPriority) : CActive(aPriority),iQueue(_FOFF(TWsDeltaTimerEntry,iLink))
	{
	}

CWsDeltaTimer::~CWsDeltaTimer()
	{
	Cancel();
	while(iQueue.RemoveFirst()!=NULL)
		{}
	}

void CWsDeltaTimer::Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TWsDeltaTimerEntry& anEntry)
	{
	TInt intervals=aTimeInMicroSeconds.Int()/CWsDeltaTimerGranularity;
	if (intervals==0)
		intervals=1;
	iQueue.Add(anEntry,intervals);
	}

void CWsDeltaTimer::Activate()
	{
	// Queue a request on the timer.
	// The timer runs every tenth of a second and decremented the delta of the head of the queue.
	if (IsActive())
		return;
	if (!iQueue.IsEmpty())
		{
		SetActive();
		iTimer.After(iStatus,CWsDeltaTimerGranularity-1); // -1 to compensate for +1 in kernel!
		}
	}

void CWsDeltaTimer::RunL()
	{
	// Call all zero delta callbacks
	iQueue.CountDown();
	TWsDeltaTimerEntry* ent=iQueue.RemoveFirst();
	while (ent)
		{       
		ent->iCallBack.CallBack();
		ent=iQueue.RemoveFirst();
		}
	Activate();
	}

void CWsDeltaTimer::DoCancel()
	{
	iTimer.Cancel();
	}

void CWsDeltaTimer::Remove(TWsDeltaTimerEntry& anEntry)
	{
	if (anEntry.IsPending())
		{
		iQueue.Remove(anEntry);
		Activate();
		}
	}


//
// CWsSpriteManager -handles floating and flashing sprites including flashing custom text cursors. i.e. cursors
//					that have an associated sprite.


CWsSpriteManager::CWsSpriteManager()
	{
	}
	
CWsSpriteManager::~CWsSpriteManager()
	{
	iFloatingSprites.ResetAndDestroy();
	}
		
CWsSpriteManager* CWsSpriteManager::NewL()
	{
	CWsSpriteManager* self = new (ELeave) CWsSpriteManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void CWsSpriteManager::ConstructL()
	{
	}

void CWsSpriteManager::AddFloatingSprite(CWsSpriteBase* aSprite)
	{
	iFloatingSprites.Append(aSprite);
	}
	
void CWsSpriteManager::RemoveFloatingSprite(CWsSpriteBase* aSprite)
	{
	for (TInt i=0 ; i<iFloatingSprites.Count() ; i++)
		{
		if(iFloatingSprites[i]==aSprite)
			{
			//Just remove the sprite don't delete it. the manager doesn't have ownership
			iFloatingSprites.Remove(i); 
			break;
			}
		}
	}
	
void CWsSpriteManager::DrawFloatingSprites(CFbsBitGc* aGc,const TRegion& aRegion)
	{
	for (TInt i=0 ; i<iFloatingSprites.Count() ; i++)
		{
		aGc->Reset();
		iFloatingSprites[i]->Redraw(aGc, aRegion);
		}
	}
	
void CWsSpriteManager::Schedule(CWsSpriteBase* aSprite, TRect* aRect)
	{
	if (aRect != NULL && aRect->IsEmpty())
		return;

	TRect rect = aSprite->Rect();
	if (aRect)
		rect.Intersection(*aRect);
		
	if(aSprite->IsFlashingEnabled())
		{
		aSprite->Screen()->ScheduleAnimation(rect,NextSpriteFlashStateChange(aSprite),0,0);
		}
	else
		{
		//Scheduling an animation "now" means it will take place at next animation which might 
		//be the full animation grace period into the future (see KAnimationGrace in server.cpp)
		aSprite->Screen()->ScheduleAnimation(rect,0,0,0);
		}
	}	

// Sprite flashing is clamped to half second intervals in relation to the global time.
// For the first half of each second all sprites have the EFlashOn state (visible)
// For the second half of each second all sprites have the EFlashOff state (not visible) 
TTimeIntervalMicroSeconds CWsSpriteManager::NextSpriteFlashStateChange(const CWsSpriteBase* aSprite) const
	{
	const TTimeIntervalMicroSeconds remainder = aSprite->Screen()->Now().DateTime().MicroSecond();
	return CalculateTimeToNextFlash(remainder);
	}

TTimeIntervalMicroSeconds CWsSpriteManager::NextCursorFlashStateChange() const
	{
	const TTimeIntervalMicroSeconds remainder = CWsTop::CurrentFocusScreen()->Now().DateTime().MicroSecond();
	return CalculateTimeToNextFlash(remainder);
	}

TTimeIntervalMicroSeconds CWsSpriteManager::CalculateTimeToNextFlash(TTimeIntervalMicroSeconds aTime) const
	{
	TInt64 nextStateChange;
	if(aTime<KFlashHalfSecond)
		nextStateChange=KFlashHalfSecond-aTime.Int64();
	else
		nextStateChange=KFlashHalfSecond - (aTime.Int64() - KFlashHalfSecond);
	return TTimeIntervalMicroSeconds(nextStateChange);
	}	
	
TFlashState CWsSpriteManager::CurrentSpriteFlashState(const CWsSpriteBase* aSprite) const
	{
	return (aSprite->Screen()->Now().DateTime().MicroSecond()<KFlashHalfSecond)?EFlashOn:EFlashOff;
	}
	
TFlashState CWsSpriteManager::CurrentCursorFlashState() const
	{
	return (CWsTop::CurrentFocusScreen()->Now().DateTime().MicroSecond()<KFlashHalfSecond)?EFlashOn:EFlashOff;
	}

void CWsSpriteManager::CalcFloatingSpriteRgn( TRegion& aResultRgn, const TRect& aDefaultRect )
	{
	aResultRgn.Clear();
	for (TInt i=0 ; i<iFloatingSprites.Count() && !aResultRgn.CheckError(); i++)
		{
		CWsSpriteBase* sprite = iFloatingSprites[i]; 
		if ( sprite->CanBeSeen() && ( sprite->IsActive() || sprite->IsActivated() ) )
			{
			aResultRgn.AddRect( sprite->Rect() );
			}
		}
	aResultRgn.Tidy();
	if ( aResultRgn.CheckError() && iFloatingSprites.Count() > 0 )
		{
		aResultRgn.Clear();
		aResultRgn.AddRect( aDefaultRect );
		}
	}
