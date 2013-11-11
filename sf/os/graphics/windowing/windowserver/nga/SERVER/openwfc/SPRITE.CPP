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
#include "panics.h"
#include "EVENT.H"
#include "bitgditomwsgraphicscontextmappings.h"
#include "../debuglog/DEBUGLOG.H"

static const TInt64 KFlashHalfSecond(500000); //interval for flashing sprites

GLDEF_D CWsDeltaTimer *CWsSpriteBase::iDeltaTimer=NULL;

TInt TimerCallBack(TAny *aPtr)
	{
	((CWsSpriteBase *)aPtr)->TimerExpired();
	return(KErrNone);
	}

#ifndef _DEBUG

#define LOG_SPRITE_REDRAW_START(sprite)
#define LOG_SPRITE_REDRAW_END(sprite)
#define LOG_SPRITE_FLASH(aSprite)

#else

#define LOG_SPRITE_REDRAW_START(sprite) LogSpriteRedrawStart(sprite)
#define LOG_SPRITE_REDRAW_END(sprite) LogSpriteRedrawEnd(sprite)
#define LOG_SPRITE_FLASH(sprite) LogSpriteFlash(sprite)

extern CDebugLogBase *wsDebugLog;


static void LogSpriteRedrawStart(const CWsSpriteBase& aSprite)
    {
    if (wsDebugLog)
        {
        _LIT(KAnnotateSpriteRedrawStart, ">> MWsDrawAnnotationObserver::FloatingSpriteRedrawStart [%S][app %d]");
        const TDesC& clientName = aSprite.WsOwner()->Client().FullName();
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        log.AppendFormat(KAnnotateSpriteRedrawStart, &overflow, &clientName, aSprite.WsOwner()->ConnectionHandle());
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogSpriteRedrawEnd(const CWsSpriteBase& aSprite)
    {
    if (wsDebugLog)
        {
        _LIT(KAnnotateSpriteRedrawEnd, "<< MWsDrawAnnotationObserver::FloatingSpriteRedrawEnd [%S][app %d]");
        const TDesC& clientName = aSprite.WsOwner()->Client().FullName();
        TBuf<LogTBufSize> log;
        TTruncateOverflow overflow;
        log.AppendFormat(KAnnotateSpriteRedrawEnd, &overflow, &clientName, aSprite.WsOwner()->ConnectionHandle());
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }

static void LogSpriteFlash(const CWsSpriteBase& /*aSprite*/)
    {
    if (wsDebugLog)
        {
        // The following code causes Exception and is commented out, see defect GFX09962
        
//      _LIT(KAnnotateSpriteFlash, "-- MWsDrawAnnotationObserver::SpriteFlash:%d [%S][app %d]");
//      const TDesC& clientName = aSprite.WsOwner()->Client().FullName();
//      TBuf<LogTBufSize> log;
//      TTruncateOverflow overflow;
//      log.AppendFormat(KAnnotateSpriteFlash, &overflow, &clientName, aSprite.WsOwner()->ConnectionHandle());
//      wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
                
        // This code is temporarily here until above problem is resolved
        _LIT(KAnnotateSpriteFlash, "-- MWsDrawAnnotationObserver::SpriteFlash");
        TBuf<LogTBufSize> log;
        log.AppendFormat(KAnnotateSpriteFlash);
        wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, log);
        }
    }


#endif



static void AnnotateSpriteRedrawStart(const CWsSpriteBase& aSprite, const TRegion& aRegion)
	{
	LOG_SPRITE_REDRAW_START(aSprite);
	MWsDrawAnnotationObserver* annoObs = aSprite.Screen()->DrawAnnotationObserver();
	if(annoObs)
		{
		annoObs->SpriteRedrawStart(aSprite, aRegion);
		}
	}

static void AnnotateSpriteRedrawEnd(const CWsSpriteBase& aSprite)
	{
	LOG_SPRITE_REDRAW_END(aSprite);
	MWsDrawAnnotationObserver* annoObs = aSprite.Screen()->DrawAnnotationObserver();
	if(annoObs)
		{
		annoObs->SpriteRedrawEnd(aSprite);
		}		
	}

static void AnnotateSpriteFlash(const CWsSpriteBase& aSprite, TBool aFlashOn)
	{
	LOG_SPRITE_FLASH(aSprite);
	MWsDrawAnnotationObserver* annoObs = aSprite.Screen()->DrawAnnotationObserver();
	if(annoObs)
		{
		annoObs->SpriteFlash(aSprite, aFlashOn);
		}	
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

CWsSpriteBase::CWsSpriteBase(CWsClient *owner, WH_HANDLES aType) : CWsScreenObject(owner,aType,owner->Screen())
	{
	}

CWsSpriteBase::~CWsSpriteBase()
	{
	Deactivate();
	if(IsFloating() && iGroupWin)
		{
		iGroupWin->RemoveSprite(this);
		}
	
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
		iFlags&=~ESpriteActive;
		
		MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
		if (windowTreeObserver)
			{
			windowTreeObserver->NodeReleased(*this);
			}
		
		if (iMembers && iMembers->Count()>1)
			{
			iDeltaTimer->Remove(iDeltaTimerEntry);
			}
		
		if (IsFloating())
			{
			Screen()->SpriteManager()->RemoveFloatingSprite(this);
			if (!Screen()->ChangeTracking())
				{
				ForceRedraw();
				}
			}

		// Note: This could be a floating sprite attached to the root window (PDEF138379)
		if(iWin)
			{
			iWin->RemoveSprite(this);
			}
		}
	}

/** Called from groupwin destructor only */
void CWsSpriteBase::DisconnectGroupWin()
	{
	WS_ASSERT_DEBUG(IsFloating(),EWsPanicFloatingSprite);
	iGroupWin=NULL;
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
	// coverity[leave_without_push]
	// pwsm is not owned by the stack and will be deleted by RWsSpriteBase::Close()
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
	SetDirty(ETrue);
	Screen()->SpriteManager()->Schedule(this);
	if(IsFloating())
		{		
		Screen()->SpriteManager()->AddFloatingSprite(this);
		if (!Screen()->ChangeTracking())
			ForceRedraw();
		}
	

	// As custom text cursors are sprites (CWsCustomTextCursor) and can be activated/deactivated
	// on various different windows during their lifetime, when activating
	// a text cursor, we pretend it's just been created to give us the option
	// of specifying a new parent window. Normal sprites (CWsSprite) are
	// treated the same way just for consistency as it does no harm.
	MWsWindowTreeObserver* const windowTreeObserver = Screen()->WindowTreeObserver();
	if (windowTreeObserver)
		{
		windowTreeObserver->NodeCreated(*this, ParentNode());
		windowTreeObserver->NodeExtentChanged(*this, Rect());
		windowTreeObserver->NodeActivated(*this);
		}
	}

void CWsSpriteBase::SetMember(TInt aIndex)
	{
	const TSize oldSize = iSize;
	const TPoint oldPos = iPos;
	if(IsFloating())
		{		
		TRect rect(iPos,iSize);
		}
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

	if(oldSize!=iSize || oldPos!=iPos)
		NotifyExtentChanged();
	
	}

void CWsSpriteBase::SetPos(const TPoint &aPos)
	{
	//Non-floating anim whose window is destroyed 
	if (!IsFloating() && iWin==NULL) 
		{ 
		OwnerPanic(EWservPanicWindowDestroyed); 
		} 

	//Floating anim whose group window is destroyed 
	if (IsFloating()  && iGroupWin==NULL && iWin->WinType() != EWinTypeRoot) 
		{ 
		OwnerPanic(EWservPanicWindowDestroyed); 
		}
	
	if(IsFloating())
		{		
		TRect rect(iPos,iSize);
		}

	iBasePos=aPos;
	TPoint newPos(iBasePos+(*iMembers)[iCurIndex]->iOffset);
	if (iPos!=newPos)
		{
		if (!Screen()->ChangeTracking())
			//Ensure the region covered by the sprite before as well as after the move gets scheduled for redraw
			Screen()->SpriteManager()->Schedule(this);
		iPos=newPos;
		if (!Screen()->ChangeTracking())
			Screen()->SpriteManager()->Schedule(this);
		NotifyExtentChanged();
		}
	}

void CWsSpriteBase::QueueDeltaTimer()
	{
	iDeltaTimer->Queue((*iMembers)[iCurIndex]->iInterval,iDeltaTimerEntry);
	}

void CWsSpriteBase::TimerExpired()
	{
	if (!Screen()->ChangeTracking())
		Screen()->SpriteManager()->Schedule(this);
	SetMember((iCurIndex+1)==iMembers->Count() ? 0 : iCurIndex+1);
	SetDirty(ETrue);
	Screen()->SpriteManager()->Schedule(this);
	QueueDeltaTimer();
	}

TPoint CWsSpriteBase::Pos() const
	{
	if (iGroupWin || iWin==NULL || iWin->WinType() == EWinTypeRoot )
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

MWsSprite::TSpriteType CWsSpriteBase::SpriteType() const
	{
	if (IsFloating())
		return EFloatingSprite;

	TSpriteType spriteType = EWindowSprite; 
	switch(Type())
		{
	case WS_HANDLE_SPRITE:
		spriteType = EWindowSprite;
		break;
	case WS_HANDLE_POINTER_CURSOR:
		spriteType = EPointerCursorSprite;
		break;
	case WS_HANDLE_TEXT_CURSOR:
		spriteType = ECustomTextCursorSprite;
		break;
	default:
		ASSERT(0);
		}
	return spriteType;
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
				CompleteL();
			break;
		case EWsSpriteOpUpdateMember:
			if (pData.UpdateMember->index==iCurIndex)
				{
				SetDirty(ETrue);
				TRect rect(Pos(), iMaxSize);
				Screen()->SpriteManager()->Schedule(this,&rect);
				}
			break;
		case EWsSpriteOpUpdateMember2:
			{
			SetDirty(ETrue);
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

void CWsSpriteBase::CalcRedrawRegion(const TRegion& aSourceRegion, TRegion& aTarget) const
	{
	aTarget.Copy(aSourceRegion);
	if (ClipSprite())
		{
		TPoint origin(0,0);
		if(iWin)
			origin = iWin->Origin();
		TRect rect(iBasePos + origin + iClipOffset, iClipSize);
		aTarget.ClipRect(rect);
		}
	aTarget.ClipRect(RootWindow()->Abs());

	// Only need to draw if the region being redrawn overlaps the sprite
	const TRect spriteRect(Pos(), iSize);
	STACK_REGION spriteRegion;
	spriteRegion.AddRect(spriteRect);
	aTarget.Intersect(spriteRegion);
	spriteRegion.Close();
	}

/**
 @pre	CWsSpriteBase::CalcRedrawRegion(TRegion&,TRegion&) should have been called.
 @param aRegion Is the region that will definitely be redrawn if dirty. This param should
                be calculated by calling CalcRedrawRegion(TRegion&,TRegion&)
 */
void CWsSpriteBase::Redraw(MWsGraphicsContext * aGc, const TRegion& aRegion)
	{
	TFlashState currentState=EFlashOn;
	if(IsFlashingEnabled())
		{
		currentState=Screen()->SpriteManager()->CurrentSpriteFlashState(this);
		AnnotateSpriteFlash(*this, currentState==EFlashOn);
		}

	if(currentState==EFlashOn && (IsDirty() || HasAnimation()) )
		{
		const TRegion * pr = &aRegion;

		if (pr->CheckError())
			{
			if(iWin)
				{
				if (Screen()->ChangeTracking())
					pr = &iWin->WindowArea();
				else
					pr = &iWin->VisibleRegion();
				}
			else
				pr = &RootWindow()->WindowArea();
			}

		if (!pr->IsEmpty())
			{
			CWsSpriteMember *member=(*iMembers)[iCurIndex];
			if (member->iBitmap)
				{
				aGc->SetClippingRegion(*pr);

				// Calculate which piece (rect) of the bitmap needs to be drawn
				const TRect redrawRect = pr->BoundingRect();
				TRect bitmapRect(Pos(), iSize); // sprite rect relative to screen
				bitmapRect.Intersection(redrawRect);
				bitmapRect.Move(-Pos()); // adjust relative to bitmap origin

				if (member->iMaskBitmap)
					aGc->BitBltMasked(Pos() + bitmapRect.iTl, *member->iBitmap, bitmapRect, *member->iMaskBitmap, member->iInvertMask);
				else
					{
					aGc->SetDrawMode(BitGdiToMWsGraphicsContextMappings::LossyConvert(member->iDrawMode));
					aGc->BitBlt(Pos() + bitmapRect.iTl, *member->iBitmap, bitmapRect);
					aGc->SetDrawMode(MWsGraphicsContext::EDrawModePEN);
					}
				aGc->ResetClippingRegion();
				}
			}
		if (Screen()->ChangeTracking())
			SetDirty(EFalse);
		
		}
		
	//Flashing sprites need to reschedule themselves after drawing (unless they have
	//an animation, because for animating sprites the rescheduling is done in CWsAnim).
	if(IsFlashingEnabled() && !HasAnimation())
		Screen()->SpriteManager()->Schedule(this);
	}

TBool CWsSpriteBase::IsActivated() const
	{
	return(iFlags&ESpriteActive);
	}

void CWsSpriteBase::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	if(iNext)
		iNext->SendState(aWindowTreeObserver);
	
	if(IsActivated())
		{
		//Sprite NodeCreated must only be sent if activated
		aWindowTreeObserver.NodeCreated(*this, ParentNode());
		aWindowTreeObserver.NodeExtentChanged(*this, Rect());
		aWindowTreeObserver.NodeActivated(*this);
		}
	}

/** @see MWsWindowTreeNode */
MWsWindowTreeNode::TType CWsSpriteBase::NodeType() const
	{
	return MWsWindowTreeNode::EWinTreeNodeSprite; 
	}

/** @see MWsWindowTreeNode */
const MWsWindow* CWsSpriteBase::Window() const
	{
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsSprite* CWsSpriteBase::Sprite() const
	{
	return this;
	}

/** @see MWsWindowTreeNode */
const MWsStandardTextCursor* CWsSpriteBase::StandardTextCursor() const
	{
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsWindowGroup* CWsSpriteBase::WindowGroup() const
	{
	if(iWin)
		return iWin->WindowGroup();
	else if (iGroupWin) 
		return static_cast<MWsWindowGroup*>(iGroupWin); //floating Sprite
	
	WS_ASSERT_DEBUG(EFalse,EWsPanicInvalidOperation);
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsWindowTreeNode* CWsSpriteBase::ParentNode() const
	{
	if (iWin)
		return iWin;
	else if (iGroupWin)
		return iGroupWin->BaseParent(); //floating Sprite, will return the rootwin
	
	WS_ASSERT_DEBUG(EFalse,EWsPanicInvalidOperation);
	return NULL;
	}

void CWsSpriteBase::NotifyExtentChanged() const
	{
	if (Screen())
		{
		MWsWindowTreeObserver* windowTreeObserver = Screen()->WindowTreeObserver();
		if (windowTreeObserver && iFlags&ESpriteActive)
			{
			windowTreeObserver->NodeExtentChanged(*this, Rect());
			}
		}
	}

//
// CWsSprite
//

CWsSprite::CWsSprite(CWsClient *owner) : CWsSpriteBase(owner,WS_HANDLE_SPRITE)
	{
	}

CWsSprite::~CWsSprite()
	{
	if (!IsFloating() && IsActivated() && iWin && iWin->IsVisible() && !Screen()->ChangeTracking())
	    ForceRedraw();
	if (iAnim)
		CWsAnim::CloseAnim(iAnim);
	}

void CWsSprite::ConstructL(const TWsClCmdCreateSprite &aParams)
	{
	NewObjL();
	CWsWindowBase *win;
	WsOwner()->HandleToWindow(aParams.window,&win);
	WS_ASSERT_DEBUG(win,EWsPanicWindowNull);
	if (win->WinType()==EWinTypeGroup)
		{
		//If a sprite is attached to a group window it is floating. 
		//Floating sprite drawing is performed by the sprite manager.
		iGroupWin=(CWsWindowGroup *)win;
		win=NULL; //Floating sprites aren't associated with any particular window.
		SetIsFloating(ETrue);
		//In case the GroupWin is destroyed before the sprite it needs to call DisconnectGroupWin 
		iGroupWin->AddSprite(this);
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
			{
			delete this;
			break;
			}
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
	SetDirty(ETrue);
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
	SetIsFloating(ETrue);
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
	SetClipSprite(aClipSprite);
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

CWsSpriteManager::~CWsSpriteManager()
    {
    iFloatingSprites.ResetAndDestroy();
    }
void CWsSpriteManager::AddFloatingSprite(CWsSpriteBase* aSprite)
	{
	iFloatingSprites.Append(aSprite);
	}
	
void CWsSpriteManager::RemoveFloatingSprite(CWsSpriteBase* aSprite)
	{
#ifdef _DEBUG 
	TInt removed = 0; 
#endif
	for (TInt i=0 ; i<iFloatingSprites.Count() ; i++)
		{
		if(iFloatingSprites[i]==aSprite)
			{
			//Just remove the sprite don't delete it. the manager doesn't have ownership
			iFloatingSprites.Remove(i);
#ifdef _DEBUG
			removed++;
#else
			break;
#endif
			}
		}
	WS_ASSERT_DEBUG(removed==1,EWsPanicFloatingSprite);
	}

void CWsSpriteManager::DrawFloatingSprites(MWsGraphicsContext* aGc,const TRegion& aRegion)
	{
	if (iFloatingSprites.Count() == 0)
		return; //avoid sending events unless necessary

	for (TInt i = iFloatingSprites.Count() - 1; i >= 0 ; i--)
		{
		STACK_REGION redrawRegion;
		CWsSpriteBase* sprite = iFloatingSprites[i];
		sprite->CalcRedrawRegion(aRegion, redrawRegion);
		if(redrawRegion.CheckError() || !redrawRegion.IsEmpty())
			{
			if (sprite->IsFlashingEnabled() || sprite->IsDirty() || sprite->HasAnimation())
				{
				AnnotateSpriteRedrawStart(*sprite, redrawRegion);
				
				if(sprite->HasAnimation())
					{
					CWsAnim* anim = static_cast<CWsSprite*>(sprite)->iAnim;
					ASSERT(anim);
					
					TRAPD(err, anim->AnimateSpriteAnimL(sprite->Screen()->Now()));
					if(err!=KErrNone)
						{
						AnnotateSpriteRedrawEnd(*sprite);						
						anim->Panic(EWservPanicAnimLeave);
						continue;
						}
					}
				
				aGc->Reset();
				sprite->Redraw(aGc, redrawRegion);
				
				AnnotateSpriteRedrawEnd(*sprite);
				}
			}
		redrawRegion.Close();
		}
	}
	
void CWsSpriteManager::Schedule(CWsSpriteBase* aSprite, TRect* aRect)
	{
	if (aRect != NULL && aRect->IsEmpty())
		return;

	TRect rect = aSprite->Rect();
	if (aRect)
		rect.Intersection(*aRect);
		
	const TAnimType type = aSprite->Win() ? EWindowSprite : EFloatingSprite;
	
	if(aSprite->IsFlashingEnabled())
		{
		aSprite->Screen()->ScheduleAnimation(type, rect,NextSpriteFlashStateChange(aSprite),0,0, aSprite->Win());
		}
	else
		{
		//Scheduling an animation "now" means it will take place at next animation which might 
		//be the full animation grace period into the future (see KAnimationGrace in server.cpp)
		aSprite->Screen()->ScheduleAnimation(type, rect,0,0,0, aSprite->Win());
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
	ASSERT(nextStateChange > 0);
	return TTimeIntervalMicroSeconds(nextStateChange);
	}	
	
TFlashState CWsSpriteManager::CurrentSpriteFlashState(const CWsSpriteBase* aSprite) const
	{
	return (aSprite->Screen()->Now().DateTime().MicroSecond()<KFlashHalfSecond)?EFlashOn:EFlashOff;
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

void CWsSpriteManager::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	for(TInt i=iFloatingSprites.Count()-1; i>=0; i--)
		{
		CWsSpriteBase* sprite = iFloatingSprites[i];
		sprite->SendState(aWindowTreeObserver);
		}
	}
