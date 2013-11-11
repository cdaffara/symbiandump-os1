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
// Interface code for animated DLL's
// 
//

#include <e32std.h>
#include "server.h"
#include "gc.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "ANIM.H"
#include "wstop.h"
#include "EVENT.H"
#include "ScrDev.H"
#include "offscreenbitmap.h"
#include "panics.h"
#include "wsfont.h"

GLREF_D CDebugLogBase *wsDebugLog;

static const TInt64 KFlashOnTime(700000);
static const TInt64 KFlashOffTime(300000);
static const TInt64 KOneSecond(1000000);
static const TInt64 KOneMinute(60 * KOneSecond);
static const TInt64 KOneDay(24 * 60 * 60 * KOneSecond);
static const TInt64 KOneHalfSecond(500000);

enum {EWindowUpdate=0x0001};

// Anim DLL code //
CWsAnimGc *CWsAnim::WsAnimGc=NULL;

void CWsAnim::InitStaticsL()
	{
	WsAnimGc=new (ELeave) CWsAnimGc();
	}

void CWsAnim::DeleteStatics()
	{
	delete WsAnimGc;
	WsAnimGc=NULL;
	}

CWsAnim::CWsAnim(CWsAnimDll *aDll) : iClient(aDll->WsOwner()), iAnimAddedInHandler(EFalse), iLastFrame(-1), iFlashOn(ETrue)
	{
	__DECLARE_NAME(_S("CWsAnim"));
	}

void CWsAnim::WindowClosing(CWsAnim *aWsAnim)
	{
	CWsAnim *anim=aWsAnim;
	CWsAnim *next;
	while(anim)
		{
		next=anim->iNextWin;
		CloseAnim(anim);
		anim=next;
		}
	}

void CWsAnim::CloseAnim(CWsAnim *aWsAnim)
	{
	TInt handle=aWsAnim->iAnimDll->AnimObjectHandle(aWsAnim);
	if (handle<0)
		delete aWsAnim;
	else
		aWsAnim->iAnimDll->Remove(handle);
	}

CWsAnim::~CWsAnim()
	{
	WsAnimGc->AnimDeleted(this);
	if (iWindow)	// In case it never got linked
		{
		CWsAnim **pAnim;
		for(pAnim= &iWindow->iAnimList;(*pAnim)!=this;pAnim= &(*pAnim)->iNextWin)
			{}
		*pAnim=iNextWin;
		}
	else if (iSprite)
		iSprite->iAnim=NULL;
	
	// force the anim for the event handler list.
	TWindowServerEvent::RemoveEventHandler(iAnim);
	TWindowServerEvent::RemoveNotificationHandler(iAnim);
	delete iAnim;
	TWindowServerEvent::PotentialEventHandlerL(-1);		//PotentialEventHandler cannot leave when passed a negative parameter.
	}

void CWsAnim::Connect(CWsClientWindow *aWindow)
	{
	if (iSprite)
		Panic();
	iWindow=aWindow;
	iNextWin=aWindow->iAnimList;
	aWindow->iAnimList=this;
	}

void CWsAnim::Connect(CWsSprite *aSprite)
	{
	if (iWindow)
		Panic();
	iSprite=aSprite;
	iSprite->iAnim=this;
	}

LOCAL_C void HandleLeaveInCWsAnimConstructL(TAny* aAnim)
	{
	STATIC_CAST(CWsAnim*,aAnim)->SetMessage(NULL);
	CWsAnim::UserDeactivateAnimGc();
	}

void CWsAnim::ConstructL(CAnim *aAnim, TAny *aArgs, CWsAnimDll *aAnimDll, TBool aIsWindow)
	{
	TBool isFocused=(iSprite!=NULL);
	if (!isFocused)
		isFocused=CWsTop::FocusWindowGroup()==iWindow->WinGroup();
	iAnimDll=aAnimDll;
	iAnimSync=ESyncNone;
	iAnim=aAnim;
	iAnim->iFunctions=this;
	SetMessage(&iClient->ClientMessage());
	CleanupStack::PushL(TCleanupItem(HandleLeaveInCWsAnimConstructL,this));
	if (aIsWindow)
		{
		CWindowAnim* windowAnim=WindowAnim();
		windowAnim->iWindowFunctions=this;
		windowAnim->iGc=WsAnimGc;
		windowAnim->ConstructL(aArgs, isFocused);
		}
	else
		{
		CSpriteAnim* spriteAnim=STATIC_CAST(CSpriteAnim*,iAnim);
		spriteAnim->iSpriteFunctions=this;
		spriteAnim->ConstructL(aArgs);
		}
	CleanupStack::PopAndDestroy(this); // doesn't really destroy "this" - it actually calls HandleLeaveInCWsAnimConstructL
	}

void CWsAnim::Redraw(CFbsBitGc * aGc, const TRegion *aRegion)
	{
	WS_ASSERT_DEBUG(iWindow,EWsPanicAnimHasNoWindow);
	if (!iWindow)
		return;
	
	TWindowInfo::TRegionPair regionPair;
	regionPair.iRegion1 = aRegion;
	regionPair.iRegion2 = NULL;
	iRedrawRegionPair = &regionPair;
	
	// We don't attempt to make use of iRect because it often isn't set up by the client code.
	
	// Work out which frame we are in:
	TTime now = iWindow->Screen()->Now();
	if (iLastFrame < 0)
		iStartTime = now;
	TInt64 elapsed = now.Int64() - iStartTime.Int64();
	TInt64 adjustedStart = iStartTime.Int64();
	TInt frame = 0;
	switch (iAnimSync)
		{
		case ESyncNone:
			if (iInterval > 0)
				{
				frame = elapsed / iInterval.Int64();
				}
				else
				{
				frame = -1;
				}
			break;
		case ESyncFlash:
			{
			TInt64 fraction = elapsed % (KFlashOnTime + KFlashOffTime);
			frame = (elapsed - fraction) / (KFlashOnTime + KFlashOffTime) * 2;
			if (fraction > KFlashOnTime)
				{
				frame = frame + 1;
				iFlashOn = EFalse;
				}
			else
				{
				iFlashOn = ETrue;
				}
			}
			break;
		case ESyncSecond:
			adjustedStart = iStartTime.Int64() - (iStartTime.Int64() % KOneSecond);
			elapsed = now.Int64() - adjustedStart;
			frame = elapsed / KOneSecond;
			break;
		case ESyncMinute:
			adjustedStart = iStartTime.Int64() - (iStartTime.Int64() % KOneMinute);
			elapsed = now.Int64() - adjustedStart;
			frame = elapsed / KOneMinute;
			break;
		case ESyncDay:
			adjustedStart = iStartTime.Int64() - (iStartTime.Int64() % KOneDay);
			elapsed = now.Int64() - adjustedStart;
			frame = elapsed / KOneDay;
			break;
		}
	
	// If the frame has changed, animate:
	if (frame != iLastFrame && frame != -1)
		{
		if (frame == iLastFrame + 1 && iLastFrame != -1)
			{
			Animate(NULL);
			}
		else
			{
			TDateTime dt = now.DateTime();
			Animate(&dt);
			}
		iLastFrame = frame;
		}

	// Regardless of whether we animated or not, redraw:
	WsAnimGc->Activate(iWindow, this, aRegion, aGc);	
	WindowAnim()->Redraw();
	WsAnimGc->Deactivate();

	// Schedule ourselves again (we usually only have to do this when we animate,
	// but it is possible for our scheduled rectangle to get lost in a redraw):
	TInt64 timeToNextFrame = 0;
	switch (iAnimSync)
		{
		case ESyncNone:
			if (iInterval > 0)
				{
				timeToNextFrame = iStartTime.Int64() + iInterval.Int64() * (frame + 1) - iWindow->Screen()->Now().Int64();
				}
			break;
		case ESyncFlash:
			if (iFlashOn)
				{
				timeToNextFrame = iStartTime.Int64() + (KFlashOnTime + KFlashOffTime) * frame / 2 + KFlashOnTime - iWindow->Screen()->Now().Int64();
				}
			else
				{
				timeToNextFrame = iStartTime.Int64() + (KFlashOnTime + KFlashOffTime) * (frame + 1 ) / 2 - iWindow->Screen()->Now().Int64();
				}
			break;
		case ESyncSecond:
			timeToNextFrame = adjustedStart + KOneSecond * (frame + 1) - iWindow->Screen()->Now().Int64();
			break;
		case ESyncMinute:
			timeToNextFrame = adjustedStart + KOneMinute * (frame + 1) - iWindow->Screen()->Now().Int64();
			break;
		case ESyncDay:
			timeToNextFrame = adjustedStart + KOneDay * (frame + 1) - iWindow->Screen()->Now().Int64();
			break;
		}
	
	if (iAnimSync != ESyncNone || iInterval > 0)
		{
		iWindow->Screen()->ScheduleAnimation(BestRect(), timeToNextFrame, 0, 0);
		}

	iRedrawRegionPair = NULL;				
	}

void CWsAnim::FocusChanged(TBool aNewFocusState)
	{
	WindowAnim()->FocusChanged(aNewFocusState);
	WsAnimGc->UserDeactivate();
	}

void CWsAnim::Animate(TDateTime *aDateTime)
	{
	iAnim->Animate(aDateTime);
	WsAnimGc->UserDeactivate();
	}

void CWsAnim::UserDeactivateAnimGc()
	{
	WsAnimGc->UserDeactivate();
	}

// Callback functions //

void CWsAnim::ActivateGc()
	{
	if (!iWindow)
		Panic();

	// Window animation drawing commands need to go through the render stage pipeline. This means
	// that drawing commands issued outside animation redraws (for instance, during Animate() or
	// when the animation receives a command) will mark the animation area as invalid, but the
	// commands themselves will be ignored as drawing will only happen during the next WSERV redraw
	// cycle (CWindowAnim::Redraw).

	// In this new situation MAnimWindowFunctions::ActivateGc doesn't need to activate the graphics
	// context (drawing commands issued outside CWindowAnim::Redraw are ignored), but to avoid some
	// behavior breaks (for instance, panic situations) we mark the GC as "activated by the user".
	WsAnimGc->UserActivate(iWindow, this);
	}

void CWsAnim::DeactivateGc()
	{
	if (!iWindow)
		Panic();

	// Window animation drawing commands need to go through the render stage pipeline. This means
	// that drawing commands issued outside animation redraws (for instance, during Animate() or
	// when the animation receives a command) will mark the animation area as invalid, but the
	// commands themselves will be ignored as drawing will only happen during the next WSERV redraw
	// cycle (CWindowAnim::Redraw).

	// In this new situation MAnimFreeTimerWindowFunctions::DeactivateGc just marks the animation
	// area as invalid so it gets redrawn later.
	WsAnimGc->UserDeactivate();
	}

/*
Because lots of animations don't set a rectangle, or set an empty one, we need
to make a best guess at what to use rather than assuming anything.
*/
TRect CWsAnim::BestRect() const
	{
	TRect rect;
	if (iRect.IsEmpty())
		{
		rect = iWindow->AbsRect();
		}
	else
		{
		rect = iRect;
		rect.Move(iWindow->Origin());
		rect.Intersection(iWindow->AbsRect());
		}
	return rect;
	}

void CWsAnim::Invalidate(const TRect &aRect)
	{
	if (!iWindow)
		{
		if (iSprite) 
			{
			iSprite->RootWindow()->InvalidateWholeScreen();
			}
		return;
		}
	iWindow->Redraw()->ClientExposing();
	TRect rect(aRect);
	rect.Move(iWindow->Origin());

	CWsTop::TriggerRedraws(iWindow->RootWindow());
	}

void CWsAnim::Update()
	{
	if (!iWindow)
		Panic();
	}

void CWsAnim::Parameters(TWindowInfo &aData)
	{
	if (!iWindow)
		Panic();
	aData.iScreenPos=iWindow->FullRect();
	aData.iMode=iWindow->DisplayMode();
	aData.iRegionPair=iRedrawRegionPair;
	}

void CWsAnim::VisibleRegion(TRegion& aRegion)
	{
	if(iWindow)
		{
		aRegion.Copy(iWindow->VisibleRegion());
		}
	}

void CWsAnim::SetSync(TAnimSync aSyncMode)
	{
	if (iAnimSync != aSyncMode)
		{
		iAnimSync=aSyncMode;	
		iLastFrame = -1;
		if (iAnimSync != ESyncNone)
			iWindow->Screen()->ScheduleAnimation(BestRect(),0,0,0);
		if (iAnimSync == ESyncFlash)
			iFlashOn = ETrue;
		}
	}

void CWsAnim::SetInterval(TInt aInterval)
	{
	if (iAnimSync!=ESyncNone)
		Panic();
	iLastFrame = -1;	
	if (aInterval < 0)
		aInterval = 0;
	// convert intervals to milliseconds (there are two intervals per second)
	iInterval = aInterval*KOneHalfSecond;
	if (iInterval > 0)
		{
		iWindow->Screen()->ScheduleAnimation(BestRect(),iInterval,0,0);
		}
	}

void CWsAnim::SetNextInterval(TInt aInterval)
	{
	if (iAnimSync!=ESyncNone)
		Panic();
	aInterval = (aInterval <= 0) ? 1 : aInterval; 
	iWindow->Screen()->ScheduleAnimation(BestRect(),aInterval*KOneHalfSecond,0,0);
	}

void CWsAnim::SetRect(const TRect &aRect)
	{
	if (!iWindow)
		Panic();
	iRect=aRect;
	iWindow->UpdateAnimArea(); // backed up windows only
	}

const TRect& CWsAnim::Rect() const
	{
	return(iRect);
	}

TDateTime CWsAnim::SystemTime() const
	{
	TDateTime dt=iWindow->Screen()->Now().DateTime();
	TInt hour=dt.Hour();
	TInt minute=dt.Minute();
	TInt second=dt.Second();
	TInt microSecond=dt.MicroSecond();
	switch(iAnimSync)
		{
	case ESyncDay:
		hour=0;
		minute=0;
		/*Fall through*/
	case ESyncMinute:
		second=0; 
		/*Fall through*/
	case ESyncNone:
	case ESyncFlash:
	case ESyncSecond:
		microSecond=0;
		break;
		}
	TDateTime dateTime;
	dateTime.Set(dt.Year(),dt.Month(),dt.Day(),hour,minute,second,microSecond);
	return(dateTime);
	}

TBool CWsAnim::FlashStateOn() const
	{
	return(iFlashOn);
	}

const RThread &CWsAnim::Client()
	{
	return(iClient->Client());
	}

void CWsAnim::ReplyBuf(const TDesC8 &aDes)
	{
	CWsClient::ReplyBuf(aDes);
	}

void CWsAnim::ReplyBuf(const TDesC16 &aDes)
	{
	CWsClient::ReplyBuf(aDes);
	}

void CWsAnim::Panic() const
	{
	iClient->PPanic(EWservPanicAnimDll);
	}
	
void CWsAnim::Panic(TClientPanic aPanic) const
	{
	iClient->PPanic(aPanic);
	}
	
const CFbsScreenDevice *CWsAnim::ScreenDevice()
	{
	CScreen* screen=NULL;		//To stop a warning
	if (iWindow)
		screen=iWindow->Screen();
	else if (iSprite)
		screen=iSprite->Screen();
	else
		Panic();
	return screen->ScreenDevice();
	}

CFbsFont *CWsAnim::DuplicateFontL(TInt aHandle)
	{
	CFbsFont *font=NULL;
	TInt err;
	font=CAnimFbsFont::NewL(aHandle,err);
	if (err!=KErrNone)
		{
		WS_ASSERT_DEBUG(font==NULL,EWsPanicFailedToInitialise);
		if (err==KErrNoMemory)
			User::Leave(err);
		iClient->PPanic(EWservPanicFont);
		}
	return(font);
	}

void CWsAnim::CloseFont(CFbsFont *aFont)
	{
	if (aFont)
		((CAnimFbsFont *)aFont)->Close();
	}

CFbsBitmap *CWsAnim::DuplicateBitmapL(TInt aHandle)
	{
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
	TInt err=bitmap->Duplicate(aHandle);
	if (err!=KErrNone)
		{
		delete bitmap;
		if (err==KErrNoMemory)
			User::Leave(err);
		iClient->PPanic(EWservPanicBitmap);
		}
	return(bitmap);
	}

TSize CWsAnim::WindowSize() const
	{
	if (!iWindow)
		Panic();
	return(iWindow->Size());
	}

TBool CWsAnim::IsHidden()
	{
	if (!iWindow)
		Panic();

	return iWindow->IsHidden();
	}

void CWsAnim::SetVisible(TBool aState)
	{	
	//The (WsAnimGc->IsActive() && aState) part of the below if statement is in place to accomodate bc with 
	//the original wserv. 
	//We panic when we call SetVisible(ETrue) and the CWsAnimGc has been activated because the origininal wserv did.
	//We don't panic when we call SetVisible(EFalse) and the CWsAnimGc is activated because the original wserv didn't.
	if( !iWindow || (WsAnimGc->IsActive() && aState) )
		{
		Panic();	
		}
		
	iWindow->SetVisible(aState);
	
	STACK_REGION region;
	VisibleRegion(region);
	TRect rect = region.BoundingRect();
	region.Close();
	if(!rect.IsEmpty())
		iWindow->Screen()->ScheduleAnimation(rect,0,0,0);
	}

MAnimGeneralFunctions::TAnimSync CWsAnim::Sync() const
	{
	return(iAnimSync);
	}

void CWsAnim::GetRawEvents(TBool aGetEvents) const
	{
	if (aGetEvents)
		{
		if (!iAnimAddedInHandler)
			{
			TWindowServerEvent::AddEventHandler(iAnim);	
			iAnimAddedInHandler = ETrue;
			}
		}
	else
		{
		if (iAnimAddedInHandler)
			{
			TWindowServerEvent::RemoveEventHandler(iAnim);
			iAnimAddedInHandler = EFalse;
			}
		}
	}

void CWsAnim::PostRawEvent(const TRawEvent &aRawEvent) const
	{
	TWindowServerEvent::ProcessRawEvent(aRawEvent);
	}

void CWsAnim::PostKeyEvent(const TKeyEvent &aRawEvent) const
	{
	TWindowServerEvent::ProcessKeyEvent(aRawEvent,0);
	}
	
/**
Generate repeated key events. 
*/	
void CWsAnim::PostKeyEvent(const TKeyEvent& aRawEvent, TInt aRepeats) const
	{
	TWindowServerEvent::ProcessKeyEvent(aRawEvent,aRepeats);
 	}

TInt CWsAnim::RegisterForNotifications(TUint32 aNotifications)
	{
	if (aNotifications)
		{
		return TWindowServerEvent::AddNotificationHandler(iAnim,aNotifications);
		}
	else
		{
		TWindowServerEvent::RemoveNotificationHandler(iAnim);
		return KErrNone;
		}
	}


const RMessagePtr2* CWsAnim::Message()
	{
	return iMessage;
	}

void CWsAnim::SetMessage(const RMessagePtr2* aMessage)
	{
	iMessage=aMessage;
	}

TInt CWsAnim::CommandReply(TInt aOpcode, TAny* aArgs)
	{
	SetMessage(&iClient->ClientMessage()); // ClientMessage returns a reference, so taking the address of it is okay (it it returned it by value, then taking the address would be taking the address of a temporary which would be dodgey)
	TInt returnValue=0;
	TRAP(returnValue,returnValue=iAnim->CommandReplyL(aOpcode, aArgs));
	SetMessage(NULL);
	return returnValue;
	}

TSpriteMember *CWsAnim::GetSpriteMember(TInt aMember) const
	{
	if (!iSprite)
		Panic();
	return REINTERPRET_CAST(TSpriteMember*,&(*iSprite->iMembers)[aMember]->iBitmap);		//The 2 classes involved in the cast have exactly the same data members in the same order
	}

void CWsAnim::UpdateMember(TInt aMember,const TRect& aRect,TBool aFullUpdate)
	{
	if (!iSprite)
		Panic();
	iSprite->Update(aMember,aRect,aFullUpdate);
	}

void CWsAnim::Activate(TBool aActive)
	{
	if (!iSprite)
		Panic();
	if (!aActive)
		iSprite->Deactivate();
	else
		{
		if (iSprite->IsActive())
			Panic();
		iSprite->Activate();
		}
	}

void CWsAnim::SizeChangedL()
	{
	if (!iSprite)
		Panic();
	iSprite->CWsSpriteBase::CompleteL();
	}

void CWsAnim::SetPosition(const TPoint &aPos)
	{
	iSprite->SetPos(aPos);
	}

TAny* CWsAnim::ExtendedInterface(TInt aInterface)
	{
	switch(aInterface)
		{
	case ENumberOfExtendedInterfaces:
		return reinterpret_cast<TAny*>(EInterfaceCount-1);
	case EWindowExtensionInterface:
		return static_cast<MAnimGeneralFunctionsWindowExtension*>(this);
	case EEventExtentionInterface:
		return static_cast<MAnimGeneralFunctionsEventExtension*>(this);
	default:
		return NULL;
		}
	}

TInt CWsAnim::Screens() const
	{
	return CWsTop::NumberOfScreens();
	}

TInt CWsAnim::FocusScreens() const
	{
	return CWsTop::CurrentFocusScreen()->ScreenNumber();
	}

void CWsAnim::SetFocusScreen(TInt aScreenNo)
 	{
 	if (aScreenNo<CWsTop::NumberOfScreens() && aScreenNo>=0)
 		{
 		CWsTop::SetCurrentFocusScreen(aScreenNo);
 		}
 	else
 		{
 		Panic();
 		}
 	}

TInt CWsAnim::WindowGroups(TInt aScreen) const
	{
	return(CWsWindowGroup::NumWindowGroupsOnScreen(CWsTop::Screen(aScreen)->RootWindow()->Child(),ETrue,0));
	}

TBool CWsAnim::WindowGroupInfo(TWindowGroupInfo& aInfo,TInt aScreen,TInt aFullOrdinalPosition) const
	{
	CWsWindowGroup* group=CWsTop::Screen(aScreen)->RootWindow()->WindowGroup(aFullOrdinalPosition);
	if (!group)
		return EFalse;
	aInfo.iId=group->Identifier();
	if (group->ReceivesFocus() && group->ScreenDeviceValid())
		aInfo.iFlags=TWindowGroupInfo::EIsFocusable;
	else
		aInfo.iFlags=0;
	aInfo.iOrdinalPriority=group->OrdinalPriority();
	HBufC* groupName=group->GroupName();
	aInfo.iNameLength=groupName?group->GroupName()->Length():0;
	if (!group->IsChained(aInfo.iParentId))
		aInfo.iParentId=-1;
	return ETrue;
	}

TInt CWsAnim::WindowGroupName(TPtrC& aWindowName,TInt aScreen,TInt aFullOrdinalPosition) const
	{
	CWsWindowGroup* group=CWsTop::Screen(aScreen)->RootWindow()->WindowGroup(aFullOrdinalPosition);
	if (!group)
		return EFalse;
	HBufC* name=group->GroupName();
	if (name)
		aWindowName.Set(*name);
	else
		aWindowName.Set(NULL,0);
	return ETrue;
	}

TInt CWsAnim::SetOrdinalPosition(TInt aWindowGroupId,TInt aPos,TInt aOrdinalPriority)
	{
	CWsWindowGroup* group=CWsWindowGroup::WindowGroupFromIdentifier(aWindowGroupId);
	if (group)
		{
		group->SetOrdinalPriority(aPos,aOrdinalPriority);
		return KErrNone;
		}
	return KErrNotFound;
	}

void CWsAnim::WindowConfig(TWindowConfig& aWindowConfig) const
	{
	aWindowConfig.iFlags = 0x00;
	if (iWindow->IsTranslucent())
		{
		aWindowConfig.iFlags |= TWindowConfig::ETransparencyEnabled;
		if (iWindow->HasAlpha())
			{
			aWindowConfig.iFlags |= TWindowConfig::EAlphaBlendedTransparency;
			}		
		}
	}

TBool CWsAnim::SpriteCanBeSeen() const
	{
	if(!iSprite)
		Panic();
	return iSprite->CanBeSeen();
	}

//

CObjectConIx* CWsAnimDll::AnimObjectConIx=NULL;

void CWsAnimDll::InitStaticsL()
	{
	CWsAnimDll::AnimObjectConIx=CObjectConIx::NewL();
	}

void CWsAnimDll::DeleteStatics()
	{
	delete CWsAnimDll::AnimObjectConIx;
	}

CWsAnimDll::CWsAnimDll(CWsClient *aOwner) : CWsObject(aOwner,WS_HANDLE_ANIM_DLL)
	{
	__DECLARE_NAME(_S("CWsAnimDll"));
	}

CWsAnimDll::~CWsAnimDll()
	{
	delete iInstanceIndex;
	AnimObjectConIx->Remove(iInstanceCon);
	delete iAnimDll;
	iAnimLib.Close();
	}

TInt CWsAnimDll::doCreateInstanceL(CWsAnim *aInstance, TInt aType, TAny *aArgs, TBool aIsWindow)
	{
	iInstanceCon->AddL(aInstance);
	aInstance->ConstructL(iAnimDll->CreateInstanceL(aType),aArgs,this,aIsWindow);
	return(iInstanceIndex->AddL(aInstance));
	}

TInt CWsAnimDll::CreateInstanceL(TUint32 aHandle, TInt aType, TAny *aArgs, TBool aIsWindow)
	{
	TWindowServerEvent::PotentialEventHandlerL(1);
	CWsAnim *instance=new(ELeave) CWsAnim(this);
	CleanupClosePushL(*instance);
	if (aIsWindow)
		{
		CWsClientWindow *win;
		iWsOwner->HandleToClientWindow(aHandle,&win);
		instance->Connect(win);
		}
	else
		{
		CWsObject *sprite=iWsOwner->HandleToObj(aHandle, WS_HANDLE_SPRITE);
		if (!sprite)
			OwnerPanic(EWservPanicSprite);
		instance->Connect(STATIC_CAST(CWsSprite*,sprite));
		}
	TInt handle=doCreateInstanceL(instance, aType, aArgs, aIsWindow);
	CleanupStack::Pop(instance);
	return(handle);
	}

void CWsAnimDll::LoadL(const TDesC &aDllName)
	{
	NewObjL();
	TFileName name(aDllName);
	User::LeaveIfError(iAnimLib.Load(name));
	if (wsDebugLog)
		{
		TBuf<256> buf;
		_LIT(KWSERVLoadedAnimDll,"Loaded Anim DLL:  ");
		buf.Append(KWSERVLoadedAnimDll);
		buf.Append(iAnimLib.FileName());
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf);
		}
	TUidType uid=iAnimLib.Type();
	if (uid[1]!=KWservAnimDllUid)
		User::Leave(KErrNotSupported);
	CreateCAnimDll f;
	f=(CreateCAnimDll)User::LeaveIfNull((TAny *)iAnimLib.Lookup(1));
	iAnimDll=(*f)();
	iInstanceIndex=CObjectIx::NewL();
	iInstanceCon=AnimObjectConIx->CreateL();
	}

void CWsAnimDll::Remove(TInt aHandle)
	{
	iInstanceIndex->Remove(aHandle);
	}

void CWsAnimDll::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	TWsAnimDllCmdUnion pData;

	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsAnimDllOpFree:
			delete this;
			break;
		case EWsAnimDllOpCreateInstance:
		case EWsAnimDllOpCreateInstanceSprite:
			SetReply(CreateInstanceL(*pData.UInt,*((TInt *)(pData.UInt+1)),(TAny *)(pData.UInt+2)
																					,aOpcode==EWsAnimDllOpCreateInstance));
			break;
		case EWsAnimDllOpCommandReply:
		case EWsAnimDllOpCommand:
		case EWsAnimDllOpDestroyInstance:
			{
			CWsAnim *anim=(CWsAnim *)iInstanceIndex->At(*pData.UInt);
			TInt ret;
			//Deleting a non existant Anim is allowed as the Anim will be destroyed
			//when the window it is on (or a parent of) it destroyed
			if (anim==NULL && aOpcode!=EWsAnimDllOpDestroyInstance)
				OwnerPanic(EWservPanicAnim);
			switch(aOpcode)
				{
				case EWsAnimDllOpCommandReply:
					SetReply(anim->CommandReply(*((TInt *)(pData.UInt+1)),(TAny *)(pData.UInt+2)));
					CWsAnim::UserDeactivateAnimGc();
					break;
				case EWsAnimDllOpCommand:
					TRAP(ret,anim->Anim()->Command(*((TInt *)(pData.UInt+1)),(TAny *)(pData.UInt+2)));
					if (ret!=KErrNone && ret!=CWsClient::EPanicLeave)
						OwnerPanic(EWservPanicAnimLeave);
					CWsAnim::UserDeactivateAnimGc();
					break;
				case EWsAnimDllOpDestroyInstance:
					if (anim) // Added to go with changes described above
						Remove(*pData.UInt);
					break;
				default:
					break;
				}
			}
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
		}
	}

CAnimFbsFont::~CAnimFbsFont()
	{}

CAnimFbsFont::CAnimFbsFont()
	{}

CAnimFbsFont* CAnimFbsFont::NewL(TInt aHandle,TInt& aError)
	{
	CAnimFbsFont *font=new(ELeave) CAnimFbsFont();
	font->iAccessCount=1;
	aError=font->Duplicate(aHandle);
	if (aError!=KErrNone)
		{
		delete font;
		font=NULL;
		}
	return(font);
	}

void CAnimFbsFont::Open()
	{
	iAccessCount++;
	}

void CAnimFbsFont::Close()
	{
	if (--iAccessCount==0)
		delete this;
	}


/*MAnimGeneralFunctions*/
void MAnimGeneralFunctions::Reserved1() const
	{}
	
void MAnimGeneralFunctions::Reserved2() const
	{}
	
void MAnimGeneralFunctions::Reserved3() const
	{}

	
/*MAnimGeneralFunctionsExtension*/

void MAnimGeneralFunctionsWindowExtension::Reserved1() const
	{}

void MAnimGeneralFunctionsWindowExtension::Reserved2() const
	{}

void MAnimGeneralFunctionsWindowExtension::Reserved3() const
	{}

/*MAnimWindowFunctions*/

void MAnimWindowFunctions::Reserved() const
	{}

void MAnimWindowFunctions::Reserved1() const
	{}
	
void MAnimWindowFunctions::Reserved2() const
	{}
	
void MAnimWindowFunctions::Reserved3() const
	{}
	

/*MAnimFreeTimerWindowFunctions*/

void MAnimFreeTimerWindowFunctions::Reserved3() const
	{}


/*MAnimSpriteFunctions*/

void MAnimSpriteFunctions::Reserved() const
	{}

void MAnimSpriteFunctions::Reserved2() const
	{}
	
void MAnimSpriteFunctions::Reserved3() const
	{}
	
void MAnimSpriteFunctions::Reserved4() const
	{}	

/*MAnimGeneralFunctionsEventExtension*/

void MAnimGeneralFunctionsEventExtension::Reserved1() const
	{}
	
void MAnimGeneralFunctionsEventExtension::Reserved2() const
	{}
