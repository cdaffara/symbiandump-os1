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
// TWalkWindowTreeBase and associated classes definitions
// 
//

#ifndef __WALKWINDOWTREE_H__
#define __WALKWINDOWTREE_H__

#include "server.h"

class CWsWindow;
class CWsWindowBase;
class RWsTextCursor;

class TWalkWindowTreeBase
	{
public:
	virtual TBool DoIt(CWsWindow *aWin)=0;
	};

class TResumableWalkWindowTreeBase
	{
public:
	virtual TBool DoIt(CWsWindow *aWin)=0;
private: // walk state
	friend class CWsWindowBase;
	CWsWindowBase* iWin;
	CWsWindowBase* iEnd;
	CWsWindowBase* iNextChild;
	CWsWindowBase* iParent;
	};
	
class TWalkWindowTreeRegionBase : public TWalkWindowTreeBase
	{
public:
	enum TTranslucentBehaviour
		{
		EDontWalkTranslucent,              //< Default behaviour - stop when you reach a window
		EWalkTranslucent,                  //< Walk through translucent parts of windows
		};
protected:
	TWalkWindowTreeRegionBase(RWsRegion *aRegion, TTranslucentBehaviour aTranslucentBehaviour = EDontWalkTranslucent);
	TBool DoIt(CWsWindow *aWin);
	virtual void DoIt2(CWsWindow *aWin)=0;
	virtual TBool DoIt3(CWsWindow *aWin);
protected:
	TTranslucentBehaviour iTranslucentBehaviour;
	RWsRegion *iRegion;
	RWsRegion *iSubRegion;
	};
	
/**
This tree walker calculates the visible regions of all windows and schedules
redraws for anywhere which has changed.
*/
class TWalkWindowTreeUpdateRegions : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeUpdateRegions(CScreen & aScreen);
	TBool DoIt(CWsWindow * aWin);
	void Walk();

private:
	CScreen & iScreen;
	RWsRegion iVisible;
	RWsRegion iTop;
	RWsRegion iRemainsOfFadableScreen; // The remains after the accumulation of already faded regions
	};

/**
This schedules the visible regions of all windows walked for redraw.
*/
class TWalkWindowTreeScheduleRedraws : public TWalkWindowTreeBase
	{
public:
	enum
	{ // bit mask
	ERedrawFilterNoFilter = 0,
	ERedrawFilterOmitDSA	= 0x001
	};
	
public:
	TWalkWindowTreeScheduleRedraws();
	TWalkWindowTreeScheduleRedraws( TUint32 aFilter );
	TBool DoIt(CWsWindow * aWin);

private:
	TUint32	iScheduleRedrawFilter;
	};

/**
This offsets all the transparent regions
*/
class TWalkWindowTreeOffsetTransparentRegions : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeOffsetTransparentRegions(const TPoint& aOffset);
	TBool DoIt(CWsWindow * aWin);

private:
	const TPoint & iOffset;	
	};

/**
This recalculates the user opaque regions
*/
class TWalkWindowTreeRecalcOpaque : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeRecalcOpaque();
	TBool DoIt(CWsWindow * aWin);
	};

// Tree walkers which schedule a set of windows to be drawn derive from this
class TWalkWindowTreeSchedule: public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeSchedule();
	CWsWindow * HeadWindow() const;
	virtual const TRegion * Region(const CWsWindow* aWin) const = 0;
protected:
	CWsWindow* iHead;
	};
	
// This walker uses regions to work out the minimum set of pixels that need updating
// It requires memory allocation, and so can fail.  Check ScheduledRegionsOk before
// relying on the results.
class TWalkWindowTreeScheduleRegions: public TWalkWindowTreeSchedule
	{
public:
	TWalkWindowTreeScheduleRegions(RWsRegion *aRegion, const TRegion& aTopLayer);
	TBool DoIt(CWsWindow *aWin);
	TBool ScheduledRegionsOk() const;
	const TRegion * Region(const CWsWindow* aWin) const;
private:
	RWsRegion *iRegion;
	const TRegion &iTopLayer;
	TBool iScheduledRegionsOk;
	};
	
// This walker uses the screens fallback mechanism.  This does not require memory
// allocation and so should never fail, but is significantly less efficient than the
// region based walker.
class TWalkWindowTreeScheduleFallback: public TWalkWindowTreeSchedule
	{
public:
	TWalkWindowTreeScheduleFallback(CScreen::CFallbackMap * aFallbackMap);
	TBool DoIt(CWsWindow *aWin);
	const TRegion * Region(const CWsWindow* aWin) const;
private:
	CScreen::CFallbackMap * iFallbackMap;
	};
	
class TWalkWindowTreeFocusChanged : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeFocusChanged(TBool aNewFocusState);
	TBool DoIt(CWsWindow *aWin);
// Data	
private:
	TBool iNewFocusState;
	};
	
class TResumableWalkWindowTreeFindInvalid : public TResumableWalkWindowTreeBase
	{
public:
	TResumableWalkWindowTreeFindInvalid(CWsWindowRedraw** aResult);
	TBool DoIt(CWsWindow* aWin);
// Data
private:
	CWsWindowRedraw** iResult;
	};
	
class TWalkWindowTreeDisconnect : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeDisconnect(RWsTextCursor *aCursor);
	TBool DoIt(CWsWindow *aWin);
private:
	RWsTextCursor *iTextCursor;
	};
	
class TWalkWindowTreeIsObscured : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeIsObscured(TBool &aResult);
	TBool DoIt(CWsWindow *aWin);
// Data	
	TBool *iResult;
	};

class TWalkWindowTreeSetNonFading : public TWalkWindowTreeBase  
	{
public:
	TWalkWindowTreeSetNonFading(TBool aNonFading);
	TBool DoIt(CWsWindow *aWin);
// Data	
	const TBool iNonFading;
	};
	
class TWalkWindowTreeSetFaded : public TWalkWindowTreeBase 
	{
public:
	TWalkWindowTreeSetFaded(TBool aFaded,CWsWindowBase* aWin,TUint8 aBlackMap,TUint8 aWhiteMap);
	TBool DoIt(CWsWindow *aWin);
// Data	
	const TUint8 iBlackMap;
	const TUint8 iWhiteMap;
	const TBool iFaded;
	const CWsWindowGroup* iGroup;
	};
	
class TWalkWindowTreePurgeEvents : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreePurgeEvents();
	TBool DoIt(CWsWindow *aWin);
	};

class TWalkWindowTreeCalcInvalidGraphics: public TWalkWindowTreeRegionBase
	{
public:
	TWalkWindowTreeCalcInvalidGraphics(RWsRegion *aRegion,TRegion &aDirty,const TArray<TGraphicDrawerId>& aInvalid);
	void CalcInvalid(CScreen& aScreen);
	TBool CreateSubRegion();
	void DoIt2(CWsWindow */*aWin*/) {}
	TBool DoIt3(CWsWindow *aWin);
	void DestroyRegions();
private:
	TRegion& iDirty;
	const TArray<TGraphicDrawerId>& iInvalid;
	};

#if defined(_DEBUG)
class TWalkWindowTreeCheck : public TWalkWindowTreeBase
	{
public:
	TBool DoIt(CWsWindow *aWin);
	};	
	
#endif

class TWalkWindowTreeRedrawStoreSize : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeRedrawStoreSize();
	TBool DoIt(CWsWindow *aWin);
	TInt iTotalSize;
	};


// We can't do a proper find_if unless we are prepared to write our own
// mem_fun, but this isn't a bad start
class TWalkWindowTreeFindWithFlag : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeFindWithFlag(TUint aFlag) : iFlag(aFlag), iFound(0) { }
	TBool DoIt(CWsWindow *aWin);
	CWsWindow * Found() { return iFound; }
private:
	TUint iFlag;
	CWsWindow * iFound;
	};

class TWalkWindowTreeFindByHandle : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeFindByHandle(TUint32 aHandle) : iHandle(aHandle) { }
	TBool DoIt(CWsWindow * aWin);
	CWsWindow * Found() { return iFound; }
private:
	TUint32 iHandle;
	CWsWindow * iFound;
	};

class TWalkWindowTreeReactivateGcs : public TWalkWindowTreeBase
	{
public:
	TBool DoIt(CWsWindow *aWin);
	};

class TWalkWindowTreeScheduleFadeNoRedraw : public TWalkWindowTreeBase
	{
public:
	TWalkWindowTreeScheduleFadeNoRedraw();
	TBool DoIt(CWsWindow *aWin);
	};

#endif
