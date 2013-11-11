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
#include "tcursor.h"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "graphics/wsgraphicdrawerinternal.h"

class CScreen;
class CWsRenderStage;
class TWalkWindowTreeSchedule;

//#define USE_DEBUG_REGIONS

/**
The CScreenRedraw maintains the areas of the screen which need to be updated, and handles some of the
scheduling of draw code to do so.  It has a close relationship with the animation scheduler, which 
by default will be the CDefaultAnimationScheduler in server.cpp
*/
class CScreenRedraw : public CBase, public MWsScreenRedraw, public MWsWindowVisibilityObserver
	{
public:
	static CScreenRedraw * NewL(CScreen& aScreen);
	virtual ~CScreenRedraw();
	const TTime& Now() const;
	void ScheduleRegionUpdate(const TRegion* aDefinitelyDirty);
	void RegionUpdate();
	void ScheduleRedraw();
	void ScheduleAnimation(TAnimType aType, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop, CWsWindow* aWindow);
	TBool IsScheduled(TAnimType aType, const TRect& aRect, CWsWindow* aWindow) const;
	void OnAnimation(TRequestStatus* aFinished);
	void AddRedrawRegion(const TRegion& aRegion, TBool aSchedule = ETrue, TRedrawDepth aDepth = ERedrawAll);
	void DoRedrawNow();
	void DiscardAllSchedules();
	void ScheduleRender(const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleWindow(CWsWindow* aWindow);
	void RemoveFromScheduledList(CWsWindow* aWindow);
	void RemoveFromTimedDrawList(CWsWindow* aWindow);
	void ScheduleQuickFade( CWsWindow* aWin );	
	void RemoveFromQuickFadeList( CWsWindow* aWin );
	TBool IsQuickFadeScheduled( CWsWindow* aWin ) const;
	void AcceptFadeRequest( CWsWindow* aWin, TBool	aFadeOn);
	inline TBool IsAnimating() const { return iAnimating; }
public: // Implementing MWsScreenRedraw
	void SetObserver(MWsScreenRedrawObserver* aObserver);	
	TBool IsUpdatePending();
	const TRegion * AnimationRegion() const;
	void UpdateDevice();
	
public: // Implementing MWsObjectProvider
	TAny * ResolveObjectInterface(TUint aTypeId);

public:	// Implementing MWsWindowVisibilityObserver
	void VisibilityChanged();
	void SetWindowVisibility(const MWsWindow& aWindow, const TRegion& aVisibleRegion);
	
public:
	MWsTextCursor* RenderStageTextCursor() const;

private:
	CScreenRedraw(CScreen& aScreen);
	void ConstructL();
	void AddQuickFadeableRegions(TRegion& aRegion);
	void DoQuickFade(MWsGraphicsContext* aGc, TRegion& aAccumulatedDrawing);
	TBool ScheduleTimedRects(TRegion& aScheduledFloatingSpriteRegion);
	void Render(CWsWindow& aWin, MWsGraphicsContext& aGc, const TWalkWindowTreeSchedule& aScheduler);
	void OomRender(CWsWindow& aWin, MWsGraphicsContext& aGc, const TWalkWindowTreeSchedule& aScheduler);
#ifdef USE_DEBUG_REGIONS
	void DebugRect(TRgb aColor, TRgb aFill, const TRect * aRect);
	void DebugRegion(TRgb aColor, TRgb aFill, const TRegion * aRegion);
#endif
#ifdef USE_DEBUG_FRAME_CAPTURE
	void CaptureFrame(const TRegion* aRegion);
	void SetupFrameCaptureResourcesL(const TSize& aScreenSize, TDisplayMode aScreenDisplayMode);
	void LogRegion(const TDesC& aPrefix, const TDesC& aFunctionName, const TRegion* aRegion);
#endif

private:
	struct TTimedRect
		{
		TTimedRect(TAnimType aType, const TRect& aRect, const TTime& aTime, CWsWindow* aWindow);
		TAnimType iType;
		TRect iRect;
		TTime iTime;
		CWsWindow* iWindow;
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
	RWsRegionBuf<20> iInvalid;  // Region that needs to be redrawn (eg window contents changed).
	RWsRegionBuf<20> iTopElement; // Region that we need to draw, but don't need to redraw (eg new window)
	const TRegion * iAnimationRegion; // Region being redrawn during animation
	MWsScreenRedrawObserver* iObserver;
	CWsRenderStage * iRenderStages;
	MWsTextCursor* iRenderStageTextCursor;
	CWsWindow* iScheduledWindowList; // Linked list of scheduled windows (only used in DirtyWindowTracking mode)
	TBool iVisibilityUpdateScheduled;
	RPointerArray<CWsWindow> iQuickFadeList;
#ifdef USE_DEBUG_FRAME_CAPTURE
	TBool iFrameCapture;
	TPtrC iFrameCaptureLocation;
	CFbsBitmap* iDebugBitmap;
	CFbsBitmapDevice* iDebugBitmapDevice;
	CFbsBitGc* iDebugBitmapGc;
#endif //USE_DEBUG_FRAME_CAPTURE
	};

#endif //__SCREEN_REDRAW_H__
