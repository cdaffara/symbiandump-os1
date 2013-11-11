// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ScreenRedraw.h"

#include <hal.h>

#include "debugbar.h"
#include "inifile.h"
#include "screen.h"
#include "pointer.h"
#include "rootwin.h"
#include "walkwindowtree.h"
#include "wstop.h"
#include "WsMemMgr.h"
#include "renderstagemanager.h"
#include "graphics/WsRenderStageFactory.h"
#include "graphics/WsRenderStage.h"
#include "graphics/wsgraphicscontext.h"
#include "EVENT.H"

#ifdef USE_DEBUG_FRAME_CAPTURE
#include <graphics/wsscreendevice.h>
#include "../debuglog/debuglog.h"
#endif

GLREF_D CDebugLogBase *wsDebugLog;

#ifdef USE_DEBUG_REGIONS
#	define DEBUG_REGION(col,fill,reg) DebugRegion(col,fill,reg)
#	define DEBUG_RECT(col,fill,rect) DebugRect(col,fill,rect)
#else
#	define DEBUG_REGION(col,fill,reg)
#	define DEBUG_RECT(col,fill,rect)
#endif

#ifdef _DEBUG
# define LOG_SCREEN_REDRAW_START {if (wsDebugLog) {_LIT(KLogScreenRedrawStart, ">> CScreenRedraw::OnAnimation()"); wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, KLogScreenRedrawStart);}}
# define LOG_SCREEN_REDRAW_END {if (wsDebugLog) {_LIT(KLogScreenRedrawEnd, "<< CScreenRedraw::OnAnimation()"); wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, KLogScreenRedrawEnd);}}
#else
# define LOG_SCREEN_REDRAW_START
# define LOG_SCREEN_REDRAW_END
#endif


#ifdef USE_DEBUG_FRAME_CAPTURE
#	ifdef __WINS__
	_LIT(KDefaultFrameCaptureLocation, "c:\\");
#	else
	_LIT(KDefaultFrameCaptureLocation, "e:\\");
#	endif
#endif //USE_DEBUG_FRAME_CAPTURE

CScreenRedraw::TTimedRect::TTimedRect(TAnimType aType, const TRect& aRect, const TTime& aTime, CWsWindow* aWindow)
	: iType(aType), iRect(aRect), iTime(aTime), iWindow(aWindow)
	{
	}

TInt CScreenRedraw::TTimedRect::Compare(const TTimedRect& aOne,const TTimedRect& aOther)
	{
	if(aOne.iTime < aOther.iTime)
		return -1;
	else if(aOne.iTime > aOther.iTime)
		return 1;
	else
		return 0;
	}

CScreenRedraw * CScreenRedraw::NewL(CScreen& aScreen)
	{
	CScreenRedraw * self = new (ELeave) CScreenRedraw(aScreen);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CScreenRedraw::CScreenRedraw(CScreen& aScreen): iScreen(aScreen)
	{
	}

CScreenRedraw::~CScreenRedraw()
	{
	CRenderStageManager::Release(iRenderStages);
	iRenderStages = NULL;
	iTimedDrawRect.Close();
	iInvalid.Reset();
	iTopElement.Reset();
	iQuickFadeList.Reset();
#ifdef USE_DEBUG_FRAME_CAPTURE
	delete iDebugBitmap;
	delete iDebugBitmapDevice;
	delete iDebugBitmapGc;
#endif //USE_DEBUG_FRAME_CAPTURE
	}

void CScreenRedraw::ConstructL()
	{
	
	LEAVE_LOG_INSTALL_C;
	
	iRenderStages = CRenderStageManager::ConnectL(
						iScreen.ScreenNumber(),
						static_cast<MWsScreen*>(&iScreen),
						this
						);
	
	LEAVE_LOG_UNINSTALL_C;
	
	WS_ASSERT_ALWAYS(iRenderStages, EWsPanicNoRenderStagePipeline);
	iRenderStageTextCursor = static_cast<MWsTextCursor*>(iRenderStages->ResolveObjectInterface(KMWsTextCursor));
	WS_ASSERT_ALWAYS(iRenderStageTextCursor, EWsPanicTextCursorInterfaceMissing);

#ifdef USE_DEBUG_FRAME_CAPTURE
	_LIT(KWSERVIniFileVarFrameCapture,"FRAMECAPTURE");
	iFrameCapture = WsIniFile->FindVar(KWSERVIniFileVarFrameCapture);

	// Location to save captured images
	if (!WsIniFile->FindVar(KWSERVIniFileVarFrameCapture, iFrameCaptureLocation) || iFrameCaptureLocation.Length() == 0)
		{
		iFrameCaptureLocation.Set(KDefaultFrameCaptureLocation);
		}
#endif //USE_DEBUG_FRAME_CAPTURE
	}

MWsTextCursor* CScreenRedraw::RenderStageTextCursor() const
	{
	return iRenderStageTextCursor;
	}

const TTime& CScreenRedraw::Now() const
	{
	if(!iAnimating)
		{
		iNow.UniversalTime();
		}
	return iNow;
	}

void CScreenRedraw::ScheduleRender(const TTimeIntervalMicroSeconds& aFromNow)
	{
	iRenderScheduled = ETrue;
	TTime then(Now() + aFromNow);
	if ((!iScheduled) || then < iNext)
		iNext = then;
	iScheduled = ETrue;
	CWsTop::WindowServer()->AnimationScheduler()->ScheduleAnimation(iScreen,iNext);
	}

void CScreenRedraw::ScheduleRedraw()
	{
	iNext = Now();
	iScheduled = ETrue;
		
	// The other scheduler also removes future animations which this one encompasses.
	// We choose not to do the region calculations needed to achieve that here.
	CWindowServer* server = CWsTop::WindowServer();
	ASSERT(server);
	MWsAnimationScheduler* sched = server->AnimationScheduler();
	if(sched)
		sched->ScheduleRedraw(iScreen,iNext);
	else
		RDebug::Printf("CWsTop::WindowServer()->RedrawScheduler() is NULL!!!");			
	}

/**
@param aRect in screen coordinates 
*/
void CScreenRedraw::ScheduleAnimation(TAnimType aType, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& /*aFreq*/,const TTimeIntervalMicroSeconds& /*aStop*/, CWsWindow* aWindow)
	{
	TRect scheduledRect(aRect);
	TRect test(aRect);
	// In changetracking mode, animation is either scheduled via a window dirty region (aWindow non-NULL)
	// or via a sprite manager dirty region (aWindow NULL) if scheduling a floating sprite
	if (iScreen.ChangeTracking())
		{
		if (aWindow)
			{
			// scheduling a window dirty rect for a window/anim/sprite
			test.Intersection(aWindow->Abs());
			scheduledRect.Move(-aWindow->Origin()); // convert to window coordinates
			}
		// else, // scheduling a sprite manager dirty rect for a floating sprite
		}
	else
		{
		// scheduling a screen dirty rect
		test.Intersection(iScreen.DrawableArea());
		aWindow = NULL; // ensure all future timed draw screen rects are checked below
		}
	if(!test.IsEmpty())
		{
		const TTime then(Now() + aFromNow);
		TTimedRect tRect(aType, scheduledRect, then, aWindow);

		const TInt error = iTimedDrawRect.InsertInOrderAllowRepeats(tRect,TTimedRect::Compare);
		if (KErrNone == error)
	   		{
			if (iScheduled)
				{
				if (then < iNext)
					{
					iNext = then;
					}
				}
			else
				{
				iNext = then;
				iScheduled = ETrue;
				}
			// remove further futures that are completely contained
			TInt count = iTimedDrawRect.Count();
			for(TInt i=0; i<count; i++)
				{
				const TTimedRect& future = iTimedDrawRect[i];
				if (future.iWindow != aWindow) // only check futures for the window/floating sprite/screen we're scheduling
					continue;
				if(future.iTime.Int64() > then.Int64())
					{
					TRect rect(scheduledRect);
					rect.BoundingRect(future.iRect);
					if(rect == scheduledRect) // future is completely contained within scheduledRect
						{
						iTimedDrawRect.Remove(i);
						count--;
						i--;
						}
					}
				}
			CWsTop::WindowServer()->AnimationScheduler()->ScheduleAnimation(iScreen,iNext);

			// Blue rectangles for scheduled animations
			DEBUG_RECT(TRgb(0x00, 0x00, 0xFF),TRgb(0x00, 0x00, 0xFF, 0x20),&aRect);
			}
		}
	}

TBool CScreenRedraw::IsScheduled(TAnimType aType, const TRect& aRect, CWsWindow* aWindow) const
	{
	TRect rect(aRect);
	if(aWindow)
		{
		rect.Move(-aWindow->Origin()); //convert to window coordinates
		}
	
	const TInt count(iTimedDrawRect.Count());
	for(TInt i=0; i<count; i++)
		{
		if ((iTimedDrawRect[i].iType == aType) && 
				(iTimedDrawRect[i].iRect == rect) && 
				(iTimedDrawRect[i].iWindow == aWindow))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// This adds a region to the stored invalid region.
// The invalid region is the area of the screen that needs to be redrawn in addition to any animations.
// The draw region is the area of the screen on which only the top window needs to be redrawn.
// If the top window has transparency, this can only be true when it has been made newly visible.
// The value of aSchedule could be determined automatically from iAnimating, but passing it this way
// allows us to have the assert, which is a very valuable assert.
void CScreenRedraw::AddRedrawRegion(const TRegion& aRegion, TBool aSchedule, TRedrawDepth aDepth)
	{
	WS_ASSERT_DEBUG(!iAnimating || !aSchedule, EWsPanicScheduledRedraw);

	if(aRegion.CheckError())
		{
		iInvalid.ForceError();

		if (aSchedule)
			ScheduleRedraw();
		}
	else if(aRegion.Count()) // often called despite window not being visible
		{
		if (aDepth == ERedrawAll)
			{
			// red lines for an invalid region which is ready to be drawn
			DEBUG_REGION(TRgb(0xFF, 0x00, 0x00),TRgb(0xFF, 0x00, 0x00, 0x20),&aRegion);
			iInvalid.Union(aRegion);
			
			if (aSchedule)
				ScheduleRedraw();
			}
		else
			{
			// yellow lines for a valid region which we will draw on top of
			DEBUG_REGION(TRgb(0xFF, 0xFF, 0x00),TRgb(0xFF, 0xFF, 0x00, 0x20),&aRegion);
				
			iTopElement.Union(aRegion);
			
			if (aSchedule)
				ScheduleRedraw();			
			}
		}
	}

// This causes any asynchronously scheduled redraw to happen immediately
// It should be avoided where possible for performance reasons, but is 
// needed whenever the redraw store is discarded for a window which still
// has a redraw region pending.
// @note This method always attempts to DoRedrawNow(), even if currently
//		 animating. In this context, animation is a frame update. With NGA
// 		 that update completes at some point in the future. If you get a request
//		 to DoRedrawNow() the expectation is that all updates scheduled on or 
//		 before that point in time will have completed by the time the call 
//		 returns. An update may have been scheduled during a current (asynchronous)
//		 Animate call...the animation scheduler has this knowledge, so let it
//		 decide what to do.

void CScreenRedraw::DoRedrawNow()
	{
	CWsTop::WindowServer()->AnimationScheduler()->DoRedrawNow(iScreen);
	}
	
#ifdef USE_DEBUG_REGIONS
void CScreenRedraw::DebugRect(TRgb aColor, TRgb aFill, const TRect* aRect)
	{
	if (aRect)
		{
		CFbsBitGc * gc = iScreen.GetBitGc();
		gc->SetPenColor(aColor);
		gc->SetPenStyle(CGraphicsContext::ESolidPen);
		gc->SetPenSize(TSize(2,2));
		gc->SetBrushColor(aFill);
		gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		TRect smaller = *aRect;
		smaller.iBr.iX -= 1;
		smaller.iBr.iY -= 1;
		gc->DrawRect(smaller);
		iScreen.Update();
		}
	}

void CScreenRedraw::DebugRegion(TRgb aColor, TRgb aFill, const TRegion * aRegion)
	{
	if (aRegion)
		{
		CFbsBitGc * gc = iScreen.GetBitGc();
		gc->SetPenColor(aColor);
		gc->SetPenStyle(CGraphicsContext::ESolidPen);
		gc->SetPenSize(TSize(2,2));
		gc->SetBrushColor(aFill);
		gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		for (const TRect *rect = aRegion->RectangleList(); rect - aRegion->RectangleList() < aRegion->Count(); ++rect)
			{
			TRect smaller = *rect;
			smaller.iBr.iX -= 1;
			smaller.iBr.iY -= 1;
			gc->DrawRect(smaller);
			}
		iScreen.Update();
		}
	}
#endif 

void CScreenRedraw::OnAnimation(TRequestStatus* aFinished)
	{
	LOG_SCREEN_REDRAW_START
	WS_ASSERT_ALWAYS(!iAnimating,EWsPanicAnimationAlreadyAnimating);
	WS_ASSERT_ALWAYS(iScheduled,EWsPanicAnimationNotScheduled);
	iAnimating = ETrue;
	iScheduled = EFalse;

	const TBool changeTracking = iScreen.ChangeTracking();
	
 	const TRegionFix<1> fallbackRegion(iScreen.RootWindow()->Abs());

	CWsActiveScheduler::Static()->PrepareDraw();
	
	// Calculate any updates required by region changes:
	RegionUpdate();

	// Use the timed rectangles to mark screen, window or floating sprite as dirty
	RWsRegionBuf<10> floatingSpriteDirtyRegion;
 	const TBool futureAnimationRequired = ScheduleTimedRects(floatingSpriteDirtyRegion);
 	if (floatingSpriteDirtyRegion.CheckError())
 		{
 		floatingSpriteDirtyRegion.Reset();
 		floatingSpriteDirtyRegion.Copy(fallbackRegion);
 		}
 	
 	TWalkWindowTreeSchedule* scheduler = NULL;
 	TWalkWindowListSchedule windowScheduler(iScheduledWindowList, iInvalid); //ChangeTracking
	if (!changeTracking)
		{
		// Animating rectangles could cause iInvalid to overlap iTopElement, in which case iTopElement won't work.
		iTopElement.SubRegion(iInvalid);
		iTopElement.Intersect(iScreen.RootWindow()->WindowArea());
		iTopElement.Tidy();
		// Anything in the top element is implicitly invalid
		iInvalid.Union(iTopElement);
		iInvalid.Intersect(iScreen.RootWindow()->WindowArea());
		iInvalid.Tidy();
		
		if(iInvalid.CheckError())
			{
			iTopElement.Reset();
			iInvalid.Reset();
			iInvalid.Copy(fallbackRegion);
			}
		}
	else 
		{
		// In ChangeTracking mode, iInvalid is only used to tell the render stage
		// which part of the screen needs updating.
		iInvalid.Reset();
		iInvalid.Copy(floatingSpriteDirtyRegion);
		scheduler = &windowScheduler;
		windowScheduler.WalkWindowList();
		CWsTop::TriggerRedraws(iScreen.RootWindow()); //In case WalkWindowList did queue a request for the client to provide draw commands
		iInvalid.Tidy();
		if(iInvalid.CheckError())
			{
			iInvalid.Reset();
			iInvalid.Copy(fallbackRegion);
			}
		}
		
	// At this point, if the DEBUG_REGION is being used:
	// Red represents invalid regions that need to be redrawn completely.
	// Yellow represents regions that only need the top window to be drawn.
	// Blue represents regions which are being animated server side.
	if (iRenderScheduled || !iInvalid.IsEmpty() || iQuickFadeList.Count() > 0)  
		{
		iRenderScheduled = EFalse;
		
		TWalkWindowTreeScheduleRegions regionScheduler(iInvalid, iTopElement); //!ChangeTeacking
		TWalkWindowTreeScheduleFallback fallbackScheduler(iScreen.FallbackMap());//!ChangeTeacking

		RWsRegionBuf<20> animationRegion;
		
		if (!changeTracking)
			{	
			animationRegion.Copy(iInvalid);
			AddQuickFadeableRegions(animationRegion);
			if (animationRegion.CheckError())
				{
				animationRegion.Reset();
				animationRegion.Copy(fallbackRegion);
				}
			iAnimationRegion = &animationRegion; //iAnimationRegion must be reset to NULL before the call stack unwinds.
			
			scheduler = &regionScheduler;
			iScreen.RootWindow()->WalkWindowTree(regionScheduler,EWalkChildren);
			if (!regionScheduler.ScheduledRegionsOk())
				{
				// our region calculations for what to draw failed at some point.
				// From this point on we MUST NOT rely on allocating memory
				scheduler = &fallbackScheduler;
				iScreen.FallbackMap()->Prepare();
				iScreen.RootWindow()->WalkWindowTree(fallbackScheduler,EWalkChildren);
				iAnimationRegion = iScreen.FallbackMap()->Region();
				}
			}
		else
			{
			iAnimationRegion = &iInvalid;
			}
		
		CWsActiveScheduler::Static()->StartDraw();
		CWsMemoryManager::Static()->EnableReserve();
		
		// Redraw debug regions more brightly than before:
		DEBUG_REGION(TRgb(0xFF, 0x00, 0x00),TRgb(0xFF, 0x00, 0x00, 0x80),iAnimationRegion);
		DEBUG_REGION(TRgb(0xFF, 0xFF, 0x00),TRgb(0xFF, 0xFF, 0x00, 0x80),&iTopElement);
		
		// Pipe the drawing into the first render stage:
		iRenderStages->Begin(iAnimationRegion);
		
		RWsRegionBuf<10> accumulatedDrawing;
		
		MWsGraphicsContext * stageGc = static_cast<MWsGraphicsContext*>(iRenderStages->ResolveObjectInterface(KMWsGraphicsContext));
		for (CWsWindow * win = scheduler->HeadWindow(); (win!=NULL); win = win->NextScheduled())
			{
			if(!changeTracking)
					accumulatedDrawing.Union(scheduler->WindowRegion(*win));
			
			Render(*win, *stageGc, *scheduler);
			}

		if(!changeTracking && !accumulatedDrawing.CheckError())
			{
			//Fade any region of the screen scheduled for fading which has not been redrawn,
			DoQuickFade(stageGc, accumulatedDrawing);
			}
		accumulatedDrawing.Reset();
		
		//We limit floating sprite drawing to regions already touched
		iScreen.SpriteManager()->DrawFloatingSprites(stageGc, iScreen.ChangeTracking() ? floatingSpriteDirtyRegion : *iAnimationRegion);
		
		iRenderStages->End(aFinished);
		stageGc = NULL; //we're not allowed to draw outside Begin()/End()

#if defined(__WINS__) && defined(_DEBUG)
		MWsDebugBar * debugBar = static_cast<MWsDebugBar*>(iRenderStages->ResolveObjectInterface(KMWsDebugBar));
		if (debugBar) //optional for the licensees
			{
			if (CDebugBar* dbg = iScreen.DebugBar())
				{	
				RArray<TPtrC> debugText;
				dbg->DebugBarInfo(debugText);
				debugBar->DrawDebugBar(debugText.Array());
				debugText.Close();
				}
			}
#endif
		
		iScheduledWindowList = NULL;
		CWsMemoryManager::Static()->DisableReserve();

#ifdef USE_DEBUG_FRAME_CAPTURE
		if (iFrameCapture)
			{
			CaptureFrame(iAnimationRegion);
			}
#endif
		
		// DEBUG_REGION does not work with render stages.
		// These comments refer to what used to happen.
		
		// At this point, if the DEBUG_REGION is being used, there should usually be only green regions
		// displayed.  If we see red or yellow, then something didn't get redrawn that should have been.
		// If we see purple then a window has disobeyed the const setting on the region to draw.
		// Red or yellow is valid - a window can decide it isn't ready to draw yet - purple is very bad.
		
		//iScreen.Update();
		
		// At this point, if the DEBUG_REGION is being used, there should be no regions visible
		// of any colour.  If we see green, then it means an area of the screen was drawn to which
		// wasn't invalid, or the screen update call failed.  The former is more likely.
		// If we still see red or yellow it is a region that is not yet ready to draw.

		const TRect* rect = iAnimationRegion->RectangleList();
		TInt pixels = 0;
		for(TInt r = iAnimationRegion->Count(); r>0; r--)
			{
			pixels += (rect->Width()*rect->Height());
			rect++;
			}
		CWsActiveScheduler::Static()->StopDraw(pixels);
		
		TWindowServerEvent::NotifyScreenDrawingEvent(iAnimationRegion);

		iAnimationRegion = NULL; //iAnimationRegion must be reset to NULL before the call stack unwinds.
		iInvalid.Reset();
		animationRegion.Reset();
		}
	else
		{
		// There was nothing to compose. Signal that composition is complete.
		if (aFinished) 
			{
			*aFinished = KRequestPending;
			User::RequestComplete(aFinished, KErrNone);
			}
		
		CWsActiveScheduler::Static()->CancelPrepare();
		}
	
	floatingSpriteDirtyRegion.Reset();
	iInvalid.Reset();
	iTopElement.Reset();
	iAnimating = EFalse;
	
	if (futureAnimationRequired && iTimedDrawRect.Count() > 0 && !iScheduled)
		{
		// If this flag is set then it means there were already some animations scheduled when we ran,
		// but they themselves didn't run.  We need to make sure we have _something_ scheduled.
		CWsTop::WindowServer()->AnimationScheduler()->ScheduleAnimation(iScreen, iTimedDrawRect[0].iTime);
		iScheduled = ETrue;
		}
	
	if(iObserver)
		{
		iObserver->ScreenUpdated(iScreen.ScreenNumber());
		iObserver=NULL; //once signalled we are never going to call it again
		}
	LOG_SCREEN_REDRAW_END
	}

void CScreenRedraw::AddQuickFadeableRegions(TRegion& aRegion)
	{
	if (iQuickFadeList.Count() > 0)
		{
		for (TInt idx = iQuickFadeList.Count() - 1; idx >= 0; idx --)
			{
			CWsWindow* win = iQuickFadeList[ idx ];

			if ( !win->IsDSAHost() )
				{
				aRegion.Union(win->QuickFadeRegion() );
				}
			}
		aRegion.Tidy();
		}
	}

void CScreenRedraw::DoQuickFade(MWsGraphicsContext* aGc, TRegion& aAccumulatedDrawing)
	{
	if ( iQuickFadeList.Count() > 0 )
		{
		for ( TInt idx = iQuickFadeList.Count() - 1; idx >= 0 ; idx -- )
			{

			CWsWindow* win = iQuickFadeList[ idx ];
			
			if ( !win->IsDSAHost() )
				{				
				STACK_REGION winFadeRgn;				
				winFadeRgn.Copy( win->QuickFadeRegion() );
				winFadeRgn.SubRegion( aAccumulatedDrawing );
				winFadeRgn.Tidy();
				if ( !winFadeRgn.IsEmpty() && !winFadeRgn.CheckError() )
					{ // now fade any region that has not been redrawn (via win->Render())
					win->Fade( aGc, winFadeRgn );
					aAccumulatedDrawing.Union( winFadeRgn );	      		
					}		      	
	  			winFadeRgn.Close();
				}
			}
	
		iQuickFadeList.Reset();
		}
	}

/**
This function will iterate the timed rectangles and find any animation or sprite 
that are due to be scheduled for render now. Then schedule them and remove them 
from iTimedDrawRect.
@return ETrue if iTimedDrawRect still contains future animations
*/
TBool CScreenRedraw::ScheduleTimedRects(TRegion& aScheduledFloatingSpriteRegion)
 	{
	iNow.UniversalTime();
	TInt count(iTimedDrawRect.Count());
	TBool futureAnimationRequired = EFalse;
	while (0 < count)
		{
		if(iTimedDrawRect[0].iTime.Int64() <= iNow.Int64())
			{
			if (iScreen.ChangeTracking())
				{
				switch(iTimedDrawRect[0].iType)
					{
					case ECrpAnim:
						{
						CWsWindow* win = iTimedDrawRect[0].iWindow;
						WS_ASSERT_DEBUG(win,EWsPanicWindowNull);
						if(!win->IsTrackingVisibleRegion() || (win->IsTrackingVisibleRegion() && !win->VisibleRegion().IsEmpty()))
							{
							TRegionFix<1> region;
							region.AddRect(iTimedDrawRect[0].iRect);
							win->AddDirtyWindowRegion(region);
							ScheduleWindow(win);
							}
						break;
						}
					case EWindowAnim:
					case ESpriteAnim:
					case ETextCursor:
					case EWindowSprite:
						{
						CWsWindow* win = iTimedDrawRect[0].iWindow;
						WS_ASSERT_DEBUG(win,EWsPanicWindowNull);
						if(!win->IsTrackingVisibleRegion() || (win->IsTrackingVisibleRegion() && !win->VisibleRegion().IsEmpty()))
							{
							TRegionFix<1> region;
							region.AddRect(iTimedDrawRect[0].iRect);
							win->AddDirtySpriteRegion(region);
							ScheduleWindow(win);
							}
						break;
						}
					case EFloatingSprite:
					case EFloatingSpriteAnim:
						{
						aScheduledFloatingSpriteRegion.AddRect(iTimedDrawRect[0].iRect);
						break;
						}
					}
				
				TRect screenRect(iTimedDrawRect[0].iRect);
				if(iTimedDrawRect[0].iWindow)
					{
					screenRect.Move(iTimedDrawRect[0].iWindow->Origin()); // convert to screen coordinates
					}	
				}
			else
				{ 
				// schedule a screen dirty rect
				iInvalid.AddRect(iTimedDrawRect[0].iRect);
				}
			iTimedDrawRect.Remove(0);
			count--;
			}
		else
			{
			futureAnimationRequired = ETrue;	
			break;
			}
		}
	return futureAnimationRequired;
 	}

void CScreenRedraw::Render(CWsWindow& aWin, MWsGraphicsContext& aGc, const TWalkWindowTreeSchedule& aScheduler)
	{
	const TRegion* windowRegion = &(aScheduler.WindowRegion(aWin));
	const TRegion* spriteRegion = &(aScheduler.SpriteRegion(aWin));

	if(windowRegion->IsEmpty() && spriteRegion->IsEmpty()) 
	    return; //Don't call CWsWindow::Render if there is nothing that can be rendered 
	
	//Make sure we don't try to draw outside screen
	STACK_REGION clippedWindowRegion;
	if(!windowRegion->IsContainedBy(iScreen.RootWindow()->Abs()))
		{
		TRegionFix<1> screen(iScreen.RootWindow()->Abs());
		clippedWindowRegion.Intersection(*windowRegion, screen);
		windowRegion = &clippedWindowRegion;
		}
	STACK_REGION clippedSpriteRegion;
	if(!spriteRegion->IsContainedBy(iScreen.RootWindow()->Abs()))
		{
		TRegionFix<1> screen(iScreen.RootWindow()->Abs());
		clippedSpriteRegion.Intersection(*spriteRegion, screen);
		spriteRegion = &clippedSpriteRegion;
		}	
	
	if(!windowRegion->CheckError() && !spriteRegion->CheckError())
		{
		// Purple regions are about to be drawn
		DEBUG_REGION(TRgb(0x80, 0x00, 0x80),TRgb(0x80, 0x00, 0x80, 0x80), windowRegion);
		DEBUG_REGION(TRgb(0x80, 0x00, 0x80),TRgb(0x80, 0x00, 0x80, 0x80), spriteRegion);
		
		aWin.Render(aGc, *windowRegion, *spriteRegion); //When not in ChangeTracking mode, both windowRegion and spriteRegion points to the same region
		aWin.ClearScheduledRegion();
		aWin.ClearScheduledSpriteRegion();
		
		// Green regions have been drawn
		DEBUG_REGION(TRgb(0x00, 0xFF, 0x00),TRgb(0x00, 0xFF, 0x00, 0x80), windowRegion);
		DEBUG_REGION(TRgb(0x00, 0xFF, 0x00),TRgb(0x00, 0xFF, 0x00, 0x80), spriteRegion);
		}
	else
		{
		OomRender(aWin, aGc, aScheduler);		
		}
	
	clippedSpriteRegion.Close();
	clippedWindowRegion.Close();
	}

void CScreenRedraw::OomRender(CWsWindow& aWin, MWsGraphicsContext& aGc, const TWalkWindowTreeSchedule& aScheduler)
	{	
	const TRegion* windowRegion = &(aScheduler.WindowRegion(aWin));
	const TRegion* spriteRegion = &(aScheduler.SpriteRegion(aWin));

	WS_ASSERT_DEBUG(!(windowRegion->IsEmpty() && spriteRegion->IsEmpty()), EWsPanicRegionNull);
	
	TRect validWindow(aWin.Abs());
	validWindow.Intersection(iScreen.RootWindow()->Abs());
	
    TRegionFix<1> fallbackRegion(validWindow);
    
    if(windowRegion->CheckError())
        windowRegion = &fallbackRegion;
    if(spriteRegion->CheckError())
        spriteRegion = &fallbackRegion;
    
    // Purple regions are about to be drawn
    DEBUG_REGION(TRgb(0x80, 0x00, 0x80),TRgb(0x80, 0x00, 0x80, 0x80), windowRegion);
    DEBUG_REGION(TRgb(0x80, 0x00, 0x80),TRgb(0x80, 0x00, 0x80, 0x80), spriteRegion);
    
    aWin.Render(aGc, *windowRegion, *spriteRegion); //When not in ChangeTracking mode, both windowRegion and spriteRegion points to the same region
    aWin.ClearScheduledRegion();
    aWin.ClearScheduledSpriteRegion();
    
    // Green regions have been drawn
    DEBUG_REGION(TRgb(0x00, 0xFF, 0x00),TRgb(0x00, 0xFF, 0x00, 0x80), windowRegion);
    DEBUG_REGION(TRgb(0x00, 0xFF, 0x00),TRgb(0x00, 0xFF, 0x00, 0x80), spriteRegion);	
	}

//
void CScreenRedraw::DiscardAllSchedules()
	{
	ASSERT(!iAnimating);
	
	iTimedDrawRect.Reset();
	iInvalid.Reset();
	}

/**
Indicates that a window has moved or changed ordinal position so that the visible regions
of all windows needs to be recalculated
*/
void CScreenRedraw::ScheduleRegionUpdate(const TRegion* aDefinitelyDirty)
	{
	if (iScreen.ChangeTracking() && iScreen.WindowVisibilityNotifier())
		return;

	iRegionUpdateScheduled = ETrue;
	ScheduleRedraw();
	if(!iScreen.ChangeTracking() && aDefinitelyDirty)
		{
		iInvalid.Union(*aDefinitelyDirty);
		// Cyan regions for invalidations caused by this:
		DEBUG_REGION(TRgb(0x00, 0xFF, 0xFF),TRgb(0x00, 0xFF, 0xFF, 0x20),aDefinitelyDirty);
		}
	}

/**
Only used in CHANGETRACKING mode.
Windows are scheduled to:
- if required, ask client to validate (from lower loop)
- render the dirty window region
*/
void CScreenRedraw::ScheduleWindow(CWsWindow* aWindow)
	{
	// Add a scheduled window to the linked list, ignoring duplicates
	CWsWindow* win = iScheduledWindowList;
	while (win && win != aWindow)
		{
		win = win->NextScheduled();
		}
	if (!win)
		{
		aWindow->SetNextScheduled(iScheduledWindowList);
		iScheduledWindowList = aWindow;
		}
	ScheduleRedraw();
	}

void CScreenRedraw::RemoveFromScheduledList(CWsWindow* aWindow)
	{
	// Search for the window
	CWsWindow* win = iScheduledWindowList;
	CWsWindow* previous = NULL;
	while (win && win != aWindow)
		{
		previous = win;
		win = win->NextScheduled();
		}
	if (win)
		{
		// Found it, remove from list
		if (win == iScheduledWindowList)
			{
			iScheduledWindowList = win->NextScheduled();
			}
		else
			{
			previous->SetNextScheduled(win->NextScheduled());
			}
		}
	}

void CScreenRedraw::RemoveFromTimedDrawList(CWsWindow* aWindow)
	{
	if(aWindow)
		{	
		TInt count(iTimedDrawRect.Count());
		TInt index = 0;
		while(index < count)
			{
			if (iTimedDrawRect[index].iWindow == aWindow)
				{
				iTimedDrawRect.Remove(index);
				--count;
				}
			else
				{
				++index;
				}
			}
		}
	}

/**
Recalculates visible regions and schedules redraws or queues redraw events in response to
any changes
*/
void CScreenRedraw::RegionUpdate()
	{
	TBool somethingScheduled = EFalse;

	if (iVisibilityUpdateScheduled)
		{
		iVisibilityUpdateScheduled = EFalse;
		somethingScheduled = ETrue;
		MWsWindowVisibilityNotifier* const notifier = iScreen.WindowVisibilityNotifier();
		if(notifier)
			{
			notifier->SendVisibilityChanges(); // Should result in one callback to SetWindowVisibility for each window subscribing for visible region updates
			}
		}

	if (iRegionUpdateScheduled)
		{
		iRegionUpdateScheduled = EFalse;
		somethingScheduled = ETrue;
		TWalkWindowTreeUpdateRegions updater(iScreen);
		updater.Walk();
		
		for ( TInt idx = iQuickFadeList.Count() - 1; idx >= 0; idx-- )
			{
			CWsWindow* win = iQuickFadeList[ idx ];
			const TRegion& quickFadeRegion = win->QuickFadeRegion();
			//If QuickFadeRegion().IsEmpty() we should remove the window from iQuickFadeList.
			//And if this window has not been drawn to the screen, then it is not possible to quick fade it.
			if (quickFadeRegion.IsEmpty() || !win->HasBeenDrawnToScreen())
				{
				iQuickFadeList.Remove( idx );
				}
			}
		}

	if (somethingScheduled)
		{
		TWsPointer::ReLogPointersCurrentWindows();
		CWsTop::TriggerRedraws(iScreen.RootWindow());
		}
	}

/**
To be called by MWsWindowVisibilityNotifier (when running in ChangeTracking mode) 
in response to MWsWindowVisibilityNotifier::SendVisibilityChanges()  
*/
void CScreenRedraw::SetWindowVisibility(const MWsWindow& aWindow, const TRegion& aVisibleRegion)
	{
	CWsWindow& win = static_cast<CWsWindow&>(const_cast<MWsWindow&>(aWindow));
	WS_ASSERT_DEBUG(win.IsTrackingVisibleRegion(), EWsPanicVisibleRegionTracking);
	WS_ASSERT_DEBUG(!aVisibleRegion.CheckError(), EWsPanicRegion);
	
	if(aVisibleRegion.IsEmpty() && !win.VisibleRegion().IsEmpty())
		{
		win.ClearVisibleRegion();
		}
	else if(!aVisibleRegion.IsEmpty() && !aVisibleRegion.CheckError())
		{
		// Assert that aVisibleRegion is contained by aWin
		TRect bounds = win.AbsRect();
		bounds.Resize(1,1);
		WS_ASSERT_DEBUG(bounds.Contains(aVisibleRegion.BoundingRect().iTl), EWsPanicRegion);
		WS_ASSERT_DEBUG(bounds.Contains(aVisibleRegion.BoundingRect().iBr), EWsPanicRegion);
		win.SetVisibleRegion(aVisibleRegion, NULL);
		}
	else if(aVisibleRegion.CheckError())
		{
		const TRegionFix<1> fallback(win.Abs());
		win.SetVisibleRegion(fallback, NULL);
		}
	}

/**
To be called by MWsWindowVisibilityNotifier (when running in ChangeTracking mode) when the 
iVisibleRegion is changed for a window that subscribes for this information. The visible region 
of the window has not been updated yet at the time of this function call, but the new metrics will be retrieved 
from MWsWindowVisibilityNotifier (through a call to MWsWindowVisibilityObserver::SetWindowVisibility())
and set to each window next time OnAnimation is called.   
*/
void CScreenRedraw::VisibilityChanged()
	{
	iVisibilityUpdateScheduled = ETrue;
	ScheduleRedraw();
	}

void CScreenRedraw::SetObserver(MWsScreenRedrawObserver* aObserver)
	{
	iObserver = aObserver;
	}

/**
Returns ETrue if an update or animation is scheduled.

Note: Now that WSERV surface and composition updates are asynchronous (with NGA)
it is possible for this method to return EFalse, even if the last update has
not been completed by the rendering pipeline. This is dependant on the configuration
and implementation of render stages that make up the rendering pipeline.

If in doubt, it is best to call CScreenRedraw::DoRedrawNow(), which will not return
untill all updates have been signalled as being complete (note that if signalling
is not used, then even this cannot guarantee completion).
*/ 
TBool CScreenRedraw::IsUpdatePending()
	{
	if(iScheduled || iAnimating)
		return ETrue;
	else 
		return EFalse;
	}

/**
 Overidding MWsObjectProvider
*/
TAny* CScreenRedraw::ResolveObjectInterface(TUint aTypeId)
	{
	TAny* interface = NULL;

	switch (aTypeId)
		{
		case KWsScreenRedraw:
			interface = static_cast<MWsScreenRedraw*>(this);
			break;
		}

	if (!interface)
		{
		interface = iRenderStages->ResolveObjectInterface(aTypeId);
		}
	
	return interface;
	}

const TRegion * CScreenRedraw::AnimationRegion() const
	{
	if (iAnimating)
		return iAnimationRegion;
	else
		return 0;
	}

void CScreenRedraw::UpdateDevice()
	{
	//this used to call iScreen->Update, not needed anymore in NGA
	}

TBool CScreenRedraw::IsQuickFadeScheduled( CWsWindow* aWin ) const
	{
	const TInt idx = iQuickFadeList.Find( aWin );	
	return (idx > KErrNotFound);
	}

void CScreenRedraw::ScheduleQuickFade( CWsWindow* aWin )
	{
	const TInt idx = iQuickFadeList.Find( aWin );	
	if ( idx == KErrNotFound )
		{ // not yet enlisted		
		if ( KErrNone != iQuickFadeList.Append(aWin) )
			{  // out of resources?
			const TRegion& winReg = aWin->VisibleRegion();
			ScheduleRegionUpdate( &winReg );
			}		
		}
	}

void CScreenRedraw::RemoveFromQuickFadeList( CWsWindow* aWin )
	{	
	const TInt idx = iQuickFadeList.Find( aWin );	
	if ( idx > KErrNotFound )
		{
		iQuickFadeList.Remove( idx );
		}
	}

void CScreenRedraw::AcceptFadeRequest( CWsWindow* aWin, TBool aFadeOn )
	{
	if (aFadeOn)
		{
		if (aWin->WinType() != EWinTypeGroup)
			{
			ScheduleQuickFade(aWin);
			ScheduleRegionUpdate(NULL);
			}
		}
	else
		{ // fade off, just initiate redraw
		if ( !aWin->IsDSAHost() )
			{
			AddRedrawRegion(aWin->VisibleRegion() );
			ScheduleRegionUpdate(NULL);
			}
		}
	}


#ifdef USE_DEBUG_FRAME_CAPTURE
class TTruncateOverflow : public TDesOverflow
	{
public: //from TDesOverflow
	void Overflow(TDes&) {};
	};

void CScreenRedraw::CaptureFrame(const TRegion* aRegion)
	{
	MWsScreenDevice* screenDevice = static_cast<MWsScreen&>(iScreen).ObjectInterface<MWsScreenDevice>();
	WS_ASSERT_ALWAYS(screenDevice, EWsPanicScreenDeviceMissing);
	const TSize screenSize(screenDevice->SizeInPixels());
	const TDisplayMode screenDisplayMode(screenDevice->DisplayMode());

	//copy the whole screen
	TRAPD(err, SetupFrameCaptureResourcesL(screenSize, screenDisplayMode);
	           screenDevice->CopyScreenToBitmapL(iDebugBitmap, TRect(screenSize)));

	if(err)
		{
		if(wsDebugLog)
			wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, _L("CScreenRedraw::CaptureFrame(): Failed to create resources for screen capture"));
		return;
		}

	//remove what's not part of the region
	iDebugBitmapGc->Activate(iDebugBitmapDevice);
	
	if(aRegion && !aRegion->IsEmpty() && !aRegion->CheckError())
		{
		RWsRegion inverseRegion;
		inverseRegion.AddRect(TRect(screenSize));
		const TRect* rectList = aRegion->RectangleList();
		for(TInt i = aRegion->Count() - 1; i >= 0; i--)
			{
			inverseRegion.SubRect(rectList[i]);
			}
		if(!inverseRegion.CheckError())
			{
			iDebugBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
			iDebugBitmapGc->SetClippingRegion(inverseRegion);
			iDebugBitmapGc->SetBrushColor(TRgb(55, 55, 55, 0));
			iDebugBitmapGc->Clear();
			iDebugBitmapGc->CancelClippingRegion();
			iDebugBitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			}
		inverseRegion.Close();
		}

	//save to file
	const TUint timestamp = User::FastCounter();	
	TFileName filename;
	TTruncateOverflow overflow;
	filename.AppendFormat(iFrameCaptureLocation, &overflow);
	filename.AppendFormat(_L("frame_%010u.mbm"), &overflow, timestamp);
	iDebugBitmap->Save(filename);

	//log region
	LogRegion(filename, _L(" CScreenRedraw::CaptureFrame() "), aRegion);	
	}

void CScreenRedraw::SetupFrameCaptureResourcesL(const TSize& aScreenSize, TDisplayMode aScreenDisplayMode)
	{
	//make sure the existing bitmap has the correct display mode
	if(iDebugBitmap && iDebugBitmap->DisplayMode() != aScreenDisplayMode)
		{
		if(iDebugBitmap->SetDisplayMode(aScreenDisplayMode) != KErrNone)
			{
			delete iDebugBitmap;
			iDebugBitmap = NULL;
			delete iDebugBitmapDevice;
			iDebugBitmapDevice = NULL;
			}
		}

	//make sure the existing bitmap has the correct size
	if(iDebugBitmap && iDebugBitmap->SizeInPixels() != aScreenSize)
		{
		if(iDebugBitmap->Resize(aScreenSize) != KErrNone)
			{
			delete iDebugBitmap;
			iDebugBitmap = NULL;
			delete iDebugBitmapDevice;
			iDebugBitmapDevice = NULL;
			}
		}

	//make sure the bitmap and bitmap device is created
	if(!iDebugBitmap)
		{
		WS_ASSERT_ALWAYS(!iDebugBitmapDevice, EWsPanicTemp); //this should never occur, they should always be created/destroyed in tandem
		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		User::LeaveIfError(bitmap->Create(aScreenSize, aScreenDisplayMode));

		iDebugBitmapDevice = CFbsBitmapDevice::NewL(bitmap);
		iDebugBitmap = bitmap;
		CleanupStack::Pop(bitmap);
		}
	
	//make sure the gc is created
	if(!iDebugBitmapGc)
		{
		User::LeaveIfError(iDebugBitmapDevice->CreateContext(iDebugBitmapGc));
		}
	}

void CScreenRedraw::LogRegion(const TDesC& aPrefix, const TDesC& aFunctionName, const TRegion* aRegion)
	{
	if(!wsDebugLog)
		return;

	TBuf<LogTBufSize> log;
	TTruncateOverflow overflow;
	TInt rectCount = (aRegion == NULL ? 0 : aRegion->Count());
	log.AppendFormat(aPrefix, &overflow);
	log.AppendFormat(aFunctionName, &overflow);
	log.AppendFormat(_L("region [%d,"), &overflow, rectCount);
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
#endif //USE_DEBUG_FRAME_CAPTURE

