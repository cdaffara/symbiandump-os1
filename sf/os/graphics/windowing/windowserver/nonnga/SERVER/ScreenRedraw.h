// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SCREEN_REDRAW.H
// 
//

#ifndef __SCREEN_REDRAW_H__
#define __SCREEN_REDRAW_H__

#include <e32base.h>
#include <e32std.h>
#include <gdi.h>
#include "wstypes.h"
#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "Graphics/wsgraphicdrawerinternal.h"

class CScreen;
class CWsRenderStage;
class CWsWindow;

//#define USE_DEBUG_REGIONS

/**
The CScreenRedraw maintains the areas of the screen which need to be updated, and handles some of the
scheduling of draw code to do so.  It has a close relationship with the animation scheduler, which 
by default will be the CDefaultAnimationScheduler in server.cpp
*/
class CScreenRedraw : public CBase, public MWsScreenRedraw
	{
public:
	static CScreenRedraw * NewL(CScreen& aScreen);
	virtual ~CScreenRedraw();
	const TTime& Now() const;
	void ScheduleRegionUpdate(const TRegion* aDefinitelyDirty);
	void RegionUpdate();
	void ScheduleRedraw();
	void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	void OnAnimation();
	void AddRedrawRegion(const TRegion& aRegion, TBool aSchedule = ETrue, TRedrawDepth aDepth = ERedrawAll);
	void DoRedrawNow();
	void DiscardAllSchedules();
	void ScheduleRender(const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleWinFadeNoRedraw( CWsWindow* aWin );	
	void AcceptFadeRequest( CWsWindow* aWin, TBool	aIsFaded, TBool aIsBehind, TBool aIncludeChildren = EFalse );
	
public: // Implementing MWsScreenRedraw
	void SetObserver(MWsScreenRedrawObserver* aObserver);	
	TBool IsUpdatePending();
	const TRegion * AnimationRegion() const;
	void UpdateDevice();
	
public: // Implementing MWsObjectProvider
	TAny * ResolveObjectInterface(TUint aTypeId);

	void BanThisRegionUpdate( TRegion& aForbiddenRegion );
	void LiftRegionUpdateBan( TRegion& aFormerForbiddenRegion  );
	
private:
	CScreenRedraw(CScreen& aScreen);
	void ConstructL();
#	ifdef USE_DEBUG_REGIONS
	void DebugRect(TRgb aColor, TRgb aFill, const TRect * aRect);
	void DebugRegion(TRgb aColor, TRgb aFill, const TRegion * aRegion);
#	endif

private:
	struct TTimedRect
		{
		TTimedRect(const TRect& aRect, const TTime& aTime);
		TRect iRect;
		TTime iTime;
		static TInt Compare(const TTimedRect& aOne,const TTimedRect& aOther);
		};

private:
	CScreen& iScreen;
	mutable TTime iNow;
	mutable TTime iNext;
	TBool iScheduled;
	TBool iAnimating;
	TBool iRegionUpdateScheduled;
	TBool iRenderScheduled;
	RArray<TTimedRect> iTimedDrawRect;
	RRegionBuf<20> iInvalid;  // Region that needs to be redrawn (eg window contents changed).
	RRegionBuf<20> iTopLayer; // Region that we need to draw, but don't need to redraw (eg new window)
	const TRegion * iAnimationRegion; // Region being redrawn during animation
	MWsScreenRedrawObserver* iObserver;
	CWsRenderStage * iRenderStages;
	RRegion	iBannedRegion;
	};

#endif //__SCREEN_REDRAW_H__
