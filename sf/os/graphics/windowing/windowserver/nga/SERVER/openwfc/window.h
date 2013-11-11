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
// CWsWindow and associated classes definitions
// 
//

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "server.h"
#include "winbase.h"

#ifndef _DEBUG

#define LOG_WINDOW_FADE_START(wswin)
#define LOG_WINDOW_FADE_REGION(region)
#define LOG_WINDOW_FADE_END(wswin)

#else

#define LOG_WINDOW_FADE_START(wswin) LogFadeStart(wswin)
#define LOG_WINDOW_FADE_REGION(region) LogFadeRegion(region)
#define LOG_WINDOW_FADE_END(wswin) LogFadeEnd(wswin)

class TTruncateOverflow : public TDesOverflow
    {
    public:
    virtual void Overflow(TDes&) {};
    };

void LogRegion(const TRegion* aRegion);
void LogFadeStart(const CWsWindow* aWsWin);
void LogFadeRegion(const TRegion* aRegion);
void LogFadeEnd(const CWsWindow* aWsWin);

#endif

void AnnotateWindowRedrawStart(const CWsWindow& aWsWin, const TRegion& aRegion);
void AnnotateWindowRedrawEnd(const CWsWindow& aWsWin);
void AnnotateWindowAnimRedrawStart(const CWsWindow& aWsWin, const CWsAnim& aAnim, const TRegion& aRegion);
void AnnotateWindowAnimRedrawEnd(const CWsWindow& aWsWin, const CWsAnim& aAnim);
void AnnotateSpriteRedrawStart(const CWsWindow& aWsWin, const CWsSpriteBase& aSprite, const TRegion& aRegion);
void AnnotateSpriteRedrawEnd(const CWsWindow& aWsWin, const CWsSpriteBase& aSprite);

class MWsGraphicsContext;
class CWsRenderStage;

class CWsWindow : public CWsWindowBase, public MWsWindow
	{
	friend class TWalkWindowTreeDisconnect;
	friend class CWsRootWindow;
	friend class TWalkWindowTreeFindWithFlag;
public:
	CWsWindow(CWsClient* aOwner,WH_HANDLES aType, CScreen* aScreen);
	~CWsWindow();
	void Construct();
	inline const TRect& Rel() const;
	inline const TRect& Abs() const;
	inline CWsWindow *PrevSiblingMultiParent() const;
	inline CWsWindow *NextSiblingMultiParent() const;
	inline TUint PointerFilter() const;
	inline TBool HasPointerGrab() const;
	inline TBool ShutDownInProgress() const;
	inline TRgb BackColor() const;
	virtual void AreaCovered(TRegion &aRegion);
	const TRegion* VisibleRegionIfValid() const;
	const TRegion& VisibleRegion() const;
	void ClearVisibleRegion();
	void SetVisibleRegion(const TRegion& aNewVisibleRegion, const TRegion* aTop);
	void SetFadeableRegion(const TRegion& aNewFadeableRegion, const TRegion& aTop);
	
	virtual void GenerateWindowRegion(RWsRegion &aRegion) const=0;
	TRect FullRect() const;
	inline TBool ClientSetInvisible() const;
	inline TBool IsVisible() const;
	inline TBool DragDropCapture() const;
	inline CWsBackedUpWindow *Backup() const;
	TDisplayMode DisplayMode() const;
	inline TInt UsingPointerBuffer() const;
	virtual TDblQue<TPointerKeyList> *PointerKeyList() const;
	inline CWsWindowRedraw *Redraw() const;
	inline TBool IsTopClientWindow() const;
	TDisplayMode SetRequiredDisplayModeL(TDisplayMode aDisplayMode);
	inline TBool IsActive() const;
	TInt AddDSA(CWsDirectScreenAccess& aDirect);
	void RemoveDSA(CWsDirectScreenAccess& aDirect);
	void AbortAllDSAs();
#if defined(_DEBUG)
	inline const TSglQue<CWsDirectScreenAccess>& DSAs() const;
#endif
	void PossibleVisibilityChangedEvent(TBool aForceSend);
	void SetNonFading(TBool aNonFading); 
	void SetFadeBehind(TBool aFade);
	inline void GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const;
	inline TBool IsTranslucent() const;
	inline TBool HasAlpha() const;
	inline TBool HasBeenDrawnToScreen() const;
	inline void SetDrawnToScreen(TBool aDrawn);
	void FocusChanged(TBool aNewFocusState);
	void ClipRectToViewport(TRect& aRect) const;
	void SetNextScheduled(CWsWindow * aWin);
	CWsWindow * NextScheduled() const;
	inline const TRegion& ScheduledRegion() const;
	inline const TRegion& ScheduledSpriteRegion() const;
	TBool SetScheduledRegion(const TRegion& aRegion);
	void ClearScheduledRegion();
	void ClearScheduledSpriteRegion();
	static TBool ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel);
	TBool ReadyToDraw() const;
	void Render(MWsGraphicsContext& aGc, const TRegion& aWindowRegion, const TRegion& aWindowChildNodeRegion);	
	void Accessed();
	
	inline TBool IsNonFading() const;
	const TRegion& QuickFadeRegion() const;
	void Fade(MWsGraphicsContext * aGc, const TRegion& aRegion);
	
	inline void SetValidRedraw();
	inline TBool IsValidRedraw() const;
	inline TBool HasElement() const;
	inline TBool IsFading() const;
	inline TBool HasAnimation() const;
	inline TBool IsBackgroundClearEnabled() const;
	void AddDirtyWindowRegion(const TRegion& aRegion);
	void AddDirtySpriteRegion(const TRegion& aRegion);
	const TRegion& DirtyWindowRegion() const;
	const TRegion& DirtySpriteRegion() const;
	void ScheduleDirtyWindowRegion();
	void ScheduleDirtySpriteRegion();
	void SetupVisibleRegionTracking(TBool aRegister);
	inline TBool IsTrackingVisibleRegion() const;
	const TRegion& FadableRegion() const;
	
public: // from CWsWindowBase
	TBool IsDSAHost() const;
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;

public:	// from MWsWindow
	TPoint Origin() const;
	TRect AbsRect() const;
	TSize Size() const;
	TUint32 Handle() const;
	MWsScreen * WsScreen() const;
	void Invalidate(const TRect *) {}
	MWsWindow * FindChildByHandle(TUint32 aHandle);
	TInt OrdinalPriority() const;

protected:
	TPoint InquireOffset(TUint32 aHandle) const;
	void Shutdown();	// Part of two phase destruction
	void FocusChangedL(TBool aNewFocusState);
	void RemoveFromAccessList();
	void InsertIntoAccessListOldestEnd();
	void InsertIntoAccessListRecentEnd();
	void DeactivateAllSprites();
private:
	void StatusDump(TDes &aBuf);
	void ResetVisibleRegion();
	void AbortDsaIfRequired(const TRegion& aNewRegion, const TRegion* aTop);
	void RestartAnimations(const TRegion& aNewRegion);
#if defined (_DEBUG)
public:
	TBool iInteresting;  // Often useful while debugging for conditional breakpoints.
#endif
protected:
	TRect iRel;
	TRect iAbs;
	TPoint iOrigin;
	RWsRegionBuf<1> iVisibleRegion;
	TUint iPointerFilter;
	TInt iPointerCapturePriority;
	TUint iFlags;
	CWsWindowRedraw *iRedraw;
	TSglQue<CWsDirectScreenAccess> iDSAs;
	CWsWindow * iNextScheduled;
	RWsRegionBuf<1> iScheduledRegion;
	RWsRegionBuf<1> iScheduledSpriteRegion;
	// The access list is a "most recently drawn" ordering list of all redraw msg windows.
	static CWsWindow * iAccessListRecentEnd;
	static CWsWindow * iAccessListOldestEnd;
	CWsWindow * iAccessListPrev;
	CWsWindow * iAccessListNext;
	TUint8 iBlackMap;
	TUint8 iWhiteMap;		
	RWsRegionBuf<1> iQuickFadeRegion;
	RWsRegionBuf<1> iFadableRegion;  // Region which can be faded (Visible region minus already faded regions)
	RWsRegionBuf<1> iDirtyWindowRegion;
	RWsRegionBuf<1> iDirtySpriteRegion;
	TInt iVisibleRegionTrackingCounter;
public:
	/** The list of graphic contexts associated with this window. The list is managed by calling the
	GcActivated() and GcDeactivated() functions. 
	This is a linked list. The CWsGc class contains a pointer to the next item: CWsGc::iNextWinGc.
	*/
	CWsGc *iWinGcList;
	CWsAnim *iAnimList;	// Start of linked list of animated objects attached to the window
	};

#include "wnredraw.h"

inline TBool CWsWindow::IsVisible() const
	{
	return(!(iFlags&EFlagHidden));
	}

inline TBool CWsWindow::ClientSetInvisible() const
	{
	return(iFlags&EFlagInvisible);
	}

inline TBool CWsWindow::DragDropCapture() const
	{
	return(iFlags&EFlagPointerCaptureDragDrop);
	}

inline TRgb CWsWindow::BackColor() const
	{
	return(iRedraw->BackColor());
	}

inline TInt CWsWindow::UsingPointerBuffer() const
	{
	return(iFlags&EFlagUsingPointerBuffer);
	}

inline CWsWindowRedraw *CWsWindow::Redraw() const
	{
	return iRedraw;
	}

inline const TRect& CWsWindow::Rel() const
	{
	return iRel;
	}
	
inline const TRect& CWsWindow::Abs() const
	{
	return iAbs;
	}
	
inline TBool CWsWindow::IsTopClientWindow() const
	{
	return(iFlags&EFlagIsTopClientWindow);
	}

inline TUint CWsWindow::PointerFilter() const
	{
	return(iPointerFilter);
	}

inline TBool CWsWindow::HasPointerGrab() const
	{
	return(iFlags&EFlagPointerGrab);
	}

inline TBool CWsWindow::ShutDownInProgress() const
	{
	return(iFlags&EFlagShutDownInProgress);
	}

inline CWsBackedUpWindow *CWsWindow::Backup() const
	{
	return(iRedraw->Backup());
	}

inline CWsWindow *CWsWindow::PrevSiblingMultiParent() const
	{
	return((CWsWindow *)CWsWindowBase::PrevSiblingMultiParent());
	}

inline CWsWindow *CWsWindow::NextSiblingMultiParent() const
	{
	return((CWsWindow *)CWsWindowBase::NextSiblingMultiParent());
	}

inline TBool CWsWindow::IsActive() const
	{
	return iFlags&EFlagActive;
	}

inline TBool CWsWindow::IsTranslucent() const
	{
	return (iFlags & static_cast<TUint>(EFlagHasAlpha)) != 0;
	}

inline TBool CWsWindow::HasAlpha() const
	{
	return (iFlags & static_cast<TUint>(EFlagHasAlpha)) !=0;
	}

inline TBool CWsWindow::HasBeenDrawnToScreen() const
	{
	return (iFlags & static_cast<TUint>(EFlagDrawnToScreen)) != 0;
	}

inline void CWsWindow::SetDrawnToScreen(TBool aDrawn)
	{
	if (aDrawn)
		iFlags |= static_cast<TUint>(EFlagDrawnToScreen);
	else
		iFlags &= ~static_cast<TUint>(EFlagDrawnToScreen);
	}

inline const TRegion& CWsWindow::ScheduledRegion() const
	{
	return iScheduledRegion;
	}

inline const TRegion& CWsWindow::ScheduledSpriteRegion() const
	{
	return iScheduledSpriteRegion;
	}

#if defined(_DEBUG)
inline const TSglQue<CWsDirectScreenAccess>& CWsWindow::DSAs() const
	{
	return iDSAs;
	}
#endif

inline TBool CWsWindow::IsNonFading() const
	{
	return (iFlags&EFlagNonFadingWindow);
	}
	
inline void CWsWindow::GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const
    {aBlackMap=iBlackMap;aWhiteMap=iWhiteMap;}

inline TBool CWsWindow::HasElement() const
	{
	return (Redraw())?(Redraw()->HasElement()):(EFalse);
	}

inline TBool CWsWindow::IsFading() const
	{
	return (!IsNonFading() && (iFadeCount > 0));
	}

inline void CWsWindow::SetValidRedraw()
	{
	iFlags |= EFlagValidRedraw;
	}

inline TBool CWsWindow::IsValidRedraw() const
	{
	return (iFlags & EFlagValidRedraw);
	}

inline TBool CWsWindow::HasAnimation() const
	{
	return (NULL != iAnimList);
	}

inline TBool CWsWindow::IsBackgroundClearEnabled() const
	{
	return Redraw()->IsBackgroundClearEnabled();
	}

inline TBool CWsWindow::IsTrackingVisibleRegion() const
	{
	WS_ASSERT_DEBUG(iVisibleRegionTrackingCounter>=0,EWsPanicVisibleRegionTracking);
	return iVisibleRegionTrackingCounter;
	}

#endif
