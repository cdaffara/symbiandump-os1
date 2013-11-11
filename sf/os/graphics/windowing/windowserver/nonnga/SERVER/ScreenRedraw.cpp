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
// SCREEN_REDRAW.CPP
// 
//

#include "ScreenRedraw.h"

#include <hal.h>

#include "debugbar.h"
#include "screen.h"
#include "inifile.h"
#include "offscreenbitmap.h"
#include "wspluginmanager.h"
#include "pointer.h"
#include "rootwin.h"
#include "walkwindowtree.h"
#include "wstop.h"
#include "WsMemMgr.h"
#include "Graphics/WsRenderStageFactory.h"
#include "Graphics/WsRenderStage.h"
#include "EVENT.H"

GLREF_D CDebugLogBase *wsDebugLog;

#ifdef USE_DEBUG_REGIONS
#	define DEBUG_REGION(col,fill,reg) DebugRegion(col,fill,reg)
#	define DEBUG_RECT(col,fill,rect) DebugRect(col,fill,rect)
#else
#	define DEBUG_REGION(col,fill,reg)
#	define DEBUG_RECT(col,fill,rect)
#endif

#if defined(__WINS__) && defined(_DEBUG)
#	define DEBUGOSB { CWsOffScreenBitmap * ofb = iScreen.OffScreenBitmap(); if (ofb) ofb->Update(); }
#else
#	define DEBUGOSB
#endif				

#ifdef _DEBUG
# define LOG_SCREEN_REDRAW_START {if (wsDebugLog) {_LIT(KLogScreenRedrawStart, ">> CScreenRedraw::OnAnimation()"); wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, KLogScreenRedrawStart);}}
# define LOG_SCREEN_REDRAW_END {if (wsDebugLog) {_LIT(KLogScreenRedrawEnd, "<< CScreenRedraw::OnAnimation()"); wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, KLogScreenRedrawEnd);}}
#else
# define LOG_SCREEN_REDRAW_START
# define LOG_SCREEN_REDRAW_END
#endif

CScreenRedraw::TTimedRect::TTimedRect(const TRect& aRect, const TTime& aTime):
	iRect(aRect), iTime(aTime)
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
	CWsRenderStage * stage = iRenderStages;
	while (stage!=NULL)
   		{
		CWsRenderStage *next=stage->Next();
   		delete stage;
		stage=next;
   		}
	iTimedDrawRect.Close();
	iInvalid.Close();
	iTopLayer.Close();
	iBannedRegion.Close();	
	}

void CScreenRedraw::ConstructL()
	{
	CWsPluginManager * pluginMgr = CWsTop::WindowServer()->PluginManager();
	
	// Setup the render stages for this screen:
	_LIT(KDefaultRenderStages, "std");
	_LIT(KDefaultFlickerFreeRenderStages, "flickerbuffer std");
	_LIT(KRenderStages,"RENDERSTAGES");
	TPtrC stagesString;
	const TBool customStages = WsIniFile->FindVar(iScreen.ScreenNumber(),KRenderStages,stagesString);
	
	// If noone specifies stages for this screen, assume the standard implementation:
	const TDesC * stages;
	if (customStages)
		stages = &stagesString;
	else if (iScreen.OffScreenBitmap())
		stages = &KDefaultFlickerFreeRenderStages();
	else
		stages = &KDefaultRenderStages();
	
	CWsRenderStage * lastStage = 0;

	// Parse the string for implementation IDs:
	TLex lex(*stages);
	while(true)
		{
		TPtrC ptr = lex.NextToken();
		if (ptr.Length() > 0)
			{
			TInt err = KErrNone;
			MWsRenderStageFactory * factory = pluginMgr->FindNamedImplementation<MWsRenderStageFactory>(ptr);
			if (factory)
				{
				CWsRenderStage * stage = 0;
				TRAP(err, stage = factory->CreateStageL(static_cast<MWsScreen*>(&iScreen), this));
				if (err == KErrNone)
					{
					if (!stage)
						{
						err = KErrNotFound;
						}
					else
						{
						if (lastStage)
							lastStage->SetNext(stage);
						else
							iRenderStages = stage;
						lastStage = stage;
						}
					}
				}
			else
				{
				err = KErrNotFound;
				}

			if (wsDebugLog)
				{
				TBuf<64> buf;
				if (err == KErrNone)
					{
					_LIT(KAddedRenderStage,"Added render stage: ");
					buf.Append(KAddedRenderStage);
					buf.Append(ptr);
					wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf);
					}
				else
					{
					_LIT(KMissingRenderStage,"Failed to add render stage (%d): ");
					buf.Append(KMissingRenderStage);
					buf.Append(ptr);
					wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf,err);
					}
				}
			}
		else
			{
			break;
			}
		}
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
	MWsAnimationScheduler* animSched=CWsTop::WindowServer()->AnimationScheduler();
	if (animSched)
		{
		animSched->ScheduleRedraw(iScreen,iNext);
		}
	}

void CScreenRedraw::ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& /*aFreq*/,const TTimeIntervalMicroSeconds& /*aStop*/)
	{
	TRect test(aRect);
	test.Intersection(iScreen.DrawableArea());
	if(!test.IsEmpty())
		{
		const TTime then(Now() + aFromNow);
		TTimedRect tRect(aRect, then);

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
				if(future.iTime.Int64() > then.Int64())
					{
					TRect rect(aRect);
					rect.BoundingRect(future.iRect);
					if(rect == aRect) // future is completely contained within aRect
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
				
			iTopLayer.Union(aRegion);
			
			if (aSchedule)
				ScheduleRedraw();			
			}
		}
	}

// This causes any asynchronously scheduled redraw to happen immediately
// It should be avoided where possible for performance reasons, but is 
// needed whenever the redraw store is discarded for a window which still
// has a redraw region pending.
void CScreenRedraw::DoRedrawNow()
	{
	if(!iAnimating)
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

void CScreenRedraw::OnAnimation()
	{
	LOG_SCREEN_REDRAW_START
	ASSERT(!iAnimating);
	ASSERT(iScheduled);
	iAnimating = ETrue;
	iScheduled = EFalse;
	TBool futureAnimationRequired = EFalse;	

	CWsActiveScheduler::Static()->PrepareDraw();
	
	// Calculate any updates required by region changes:
	RegionUpdate();

	// Add the timed rectangles to the invalid region:
	iNow.UniversalTime();
	TInt count(iTimedDrawRect.Count());
	while (0 < count)
		{
		if(iTimedDrawRect[0].iTime.Int64() <= iNow.Int64())
			{
			iInvalid.AddRect(iTimedDrawRect[0].iRect);
			iTimedDrawRect.Remove(0);
			count--;
			}
		else
			{
			futureAnimationRequired = ETrue;	
			break;
			}
		}

	// Animating rectangles could cause iInvalid to overlap iTopLayer,
	// in which case iTopLayer won't work.
	iTopLayer.SubRegion(iInvalid);
	iTopLayer.Intersect(iScreen.RootWindow()->WindowArea());
	iTopLayer.Tidy();
	// Anything in the top layer is implcitly invalid
	iInvalid.Union(iTopLayer);
	iInvalid.Intersect(iScreen.RootWindow()->WindowArea());
	
/*
	if (const CDebugBar* dbg = iScreen.DebugBar())
		{
		iTopLayer.SubRect(dbg->Rect());
		iInvalid.SubRect(dbg->Rect());
		}
*/

	iInvalid.Tidy();

	if(iInvalid.CheckError()) //Will: agree with Andy, want bounding rects instead!
		{
		iTopLayer.Clear();
		iInvalid.Clear();
		iInvalid.Copy(iScreen.RootWindow()->WindowArea()); // assumed cannot fail, all regions can contain at least 1 rect..
		}

	iInvalid.SubRegion( iBannedRegion );	
	iInvalid.Tidy();
	iTopLayer.SubRegion( iBannedRegion );
	iTopLayer.Tidy();
	
	STACK_REGION invalidCopy;
	invalidCopy.Copy(iInvalid);
	TWalkWindowTreeScheduleRegions regionScheduler(&invalidCopy, iTopLayer);
	TWalkWindowTreeScheduleFallback fallbackScheduler(iScreen.FallbackMap());
	TWalkWindowTreeSchedule * scheduler = &regionScheduler;
	
	// At this point, if the DEBUG_REGION is being used:
	// Red represents invalid regions that need to be redrawn completely.
	// Yellow represents regions that only need the top window to be drawn.
	// Blue represents regions which are being animated server side.
	if (iRenderScheduled || !iInvalid.IsEmpty())
		{
		iRenderScheduled = EFalse;
		// invalidCopy.ForceError(); //### DEBUG
		
		iScreen.RootWindow()->WalkWindowTree(regionScheduler,EWalkChildren);
		if (!regionScheduler.ScheduledRegionsOk())
			{
			// our region calculations for what to draw failed at some point.
			// From this point on we MUST NOT rely on allocating memory
			// Andy - localRedrawRegion allocates
			// Andy - setPenSize allocates (even if you don't call it)
			// Andy - all draw commands add to a gdi dirty region (which allocates)
			// Andy - combining client clipping regions with window clipping regions allocates
			scheduler = &fallbackScheduler;
			iScreen.FallbackMap()->Prepare();
			iScreen.RootWindow()->WalkWindowTree(fallbackScheduler,EWalkChildren);
			}
		
		CWsActiveScheduler::Static()->StartDraw();
		CWsMemoryManager::Static()->EnableReserve();

		if (&fallbackScheduler == scheduler)
			iAnimationRegion = iScreen.FallbackMap()->Region();
		else
			iAnimationRegion = &iInvalid;
		
		// Redraw debug regions more brightly than before:
		DEBUG_REGION(TRgb(0xFF, 0x00, 0x00),TRgb(0xFF, 0x00, 0x00, 0x80),&iInvalid);
		DEBUG_REGION(TRgb(0xFF, 0xFF, 0x00),TRgb(0xFF, 0xFF, 0x00, 0x80),&iTopLayer);
		
		RWsRegion accumulatedDrawing;
		
		// Pipe the drawing into the first render stage:
		CFbsBitGc * stageGc = iRenderStages->Begin();
		
		for (CWsWindow * win = scheduler->HeadWindow(); win; win = win->NextScheduled())
			{
			const TRegion * targetRegion = scheduler->Region(win);
			const TRect * screenRect = 0;
			if ((&fallbackScheduler == scheduler) && !targetRegion->IsContainedBy(iScreen.RootWindow()->Abs()))
				{
				screenRect = &iScreen.RootWindow()->Abs();
				}
			if (!screenRect)
				{
				// Purple regions are about to be drawn
				DEBUG_REGION(TRgb(0x80, 0x00, 0x80),TRgb(0x80, 0x00, 0x80, 0x80),targetRegion);
				// Do the drawing
				stageGc->Reset();
				win->Render(stageGc, *targetRegion);
				accumulatedDrawing.Union(*targetRegion);
				// Green regions have been drawn
				DEBUG_REGION(TRgb(0x00, 0xFF, 0x00),TRgb(0x00, 0xFF, 0x00, 0x80),targetRegion);
				}
			else
				{
				// Our region overlaps the edges of the screen, and we have no memory
				// to create a clipped one, so we will use a single-rect region for each rect
				// and call Draw multiple times.
				TRegionFix<1> rectRegion;
				for (const TRect * rect = targetRegion->RectangleList() + targetRegion->Count() - 1; rect >= targetRegion->RectangleList(); --rect)
					{
					rectRegion.Clear();
					TRect combined(*screenRect);
					combined.Intersection(*rect);
					rectRegion.AddRect(combined);
					// Purple regions are about to be drawn
					DEBUG_REGION(TRgb(0x80, 0x00, 0x80),TRgb(0x80, 0x00, 0x80, 0x80),&rectRegion);
					// Do the drawing
					stageGc->Reset();
					win->Render(stageGc, rectRegion);
					accumulatedDrawing.AddRect(combined);
					// Green regions have been drawn
					DEBUG_REGION(TRgb(0x00, 0xFF, 0x00),TRgb(0x00, 0xFF, 0x00, 0x80),&rectRegion);
					}
				}
			DEBUGOSB
			}

		DEBUGOSB			
		iScreen.SpriteManager()->DrawFloatingSprites(stageGc,accumulatedDrawing); // we limit sprite drawing over only actually affected ares but necessary to all "planned" for redraw
		if (!accumulatedDrawing.CheckError() && iScreen.SpriteManager()->SpriteCount() == 0)
			{
			iAnimationRegion = &accumulatedDrawing;
			}

		// Tell the render stage we've finished:
		iRenderStages->End();
		
		// We nolonger need the regions
		for (CWsWindow * win = scheduler->HeadWindow(); win; win = win->NextScheduled())
			{
			win->ClearScheduledRegion();
			}
			
		CWsMemoryManager::Static()->DisableReserve();

		if (const CDebugBar* dbg = iScreen.DebugBar())
			{
			dbg->RedrawDebugBar();
			}

		// At this point, if the DEBUG_REGION is being used, there should usually be only green regions
		// displayed.  If we see red or yellow, then something didn't get redrawn that should have been.
		// If we see purple then a window has disobeyed the const setting on the region to draw.
		// Red or yellow is valid - a window can decide it isn't ready to draw yet - purple is very bad.
		iScreen.Update();
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

		iAnimationRegion = 0;
		accumulatedDrawing.Close();
		iInvalid.Clear();
		}
	else
		{
		CWsActiveScheduler::Static()->CancelPrepare();
		}

	iInvalid.Clear();
	iTopLayer.Clear();
	invalidCopy.Close();
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

//
void CScreenRedraw::DiscardAllSchedules()
	{
	ASSERT(!iAnimating);
	
	iTimedDrawRect.Reset();
	iInvalid.Clear();
	iInvalid.Tidy();
	}

/**
Indicates that a window has moved or changed ordinal position so that the visible regions
of all windows needs to be recalculated
*/
void CScreenRedraw::ScheduleRegionUpdate(const TRegion* aDefinitelyDirty)
	{
	iRegionUpdateScheduled = ETrue;
	ScheduleRedraw();
	if(aDefinitelyDirty)
		{
		iInvalid.Union(*aDefinitelyDirty);
		// Cyan regions for invalidations caused by this:
		DEBUG_REGION(TRgb(0x00, 0xFF, 0xFF),TRgb(0x00, 0xFF, 0xFF, 0x20),aDefinitelyDirty);
		}
	}
	
/**
Recalculates visible regions and schedules redraws or queues redraw events in response to
any changes
*/
void CScreenRedraw::RegionUpdate()
	{
	if (iRegionUpdateScheduled)
		{
		iRegionUpdateScheduled = EFalse;
		
		TWalkWindowTreeUpdateRegions updater(iScreen);
		updater.Walk();

		WsPointer::ReLogCurrentWindow();
		CWsTop::TriggerRedraws(iScreen.RootWindow());
		}
	}

void CScreenRedraw::SetObserver(MWsScreenRedrawObserver* aObserver)
	{
	iObserver = aObserver;
	}

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
	iScreen.Update();
	}

void CScreenRedraw::BanThisRegionUpdate( TRegion& aForbiddenRegion )
	{
	iBannedRegion.Union( aForbiddenRegion );
	iBannedRegion.Tidy();
	}

void CScreenRedraw::LiftRegionUpdateBan( TRegion& aFormerForbiddenRegion  )
	{
	iBannedRegion.SubRegion( aFormerForbiddenRegion );
	iBannedRegion.Tidy();
	}

void CScreenRedraw::AcceptFadeRequest( CWsWindow* aWin, TBool aFadeOn, TBool aFadeBehind, TBool aIncludeChildren )
	{
	if ( aFadeOn )
		{
		TWalkWindowTreeScheduleFadeNoRedraw walkerFadeNoRedraw;
		if ( aFadeBehind )
			{
			aWin->WalkWindowTree( walkerFadeNoRedraw, EWalkBehind );				
			}
		else
			{
			if(aWin->WinType() != EWinTypeGroup)
				{
				ScheduleRegionUpdate( aWin->VisibleRegionIfValid() );
				}
			
			if ( aIncludeChildren )
				{
				aWin->WalkWindowTree( walkerFadeNoRedraw, EWalkChildren );
				}				
			}
		}
	else
		{	// fade off, just initiate redraw
		TWalkWindowTreeScheduleRedraws walkerRedraw( TWalkWindowTreeScheduleRedraws::ERedrawFilterOmitDSA ) ;
		if ( aFadeBehind )
			{
			aWin->WalkWindowTree( walkerRedraw, EWalkBehind );								
			}
		else
			{ // fade this win not behind
			if ( !aWin->IsDSAHost() )
				{
				AddRedrawRegion( aWin->VisibleRegion() );
				}
			if ( aIncludeChildren )
				{
				aWin->WalkWindowTree( walkerRedraw, EWalkChildren );
				}				
			}			
		}		
	ScheduleRegionUpdate(NULL);
	}
