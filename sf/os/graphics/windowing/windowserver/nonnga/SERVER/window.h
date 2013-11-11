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
	virtual const TRegion& WindowArea() const = 0;
	const TRegion* VisibleRegionIfValid() const;
	const TRegion& VisibleRegion() const;
	void ClearVisibleRegion();
	void SetVisibleRegion(const TRegion& aRegion, const TRegion* aTop, TRegion& aNewFadableRegion);
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
	inline TBool IsActive() const;
	inline void AddDSA(CWsDirectScreenAccess& aDirect);
	inline void RemoveDSA(CWsDirectScreenAccess& aDirect);
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
	inline const RWsRegion * ScheduledRegion() const;
	TBool SetScheduledRegion(const RWsRegion& aRegion);
	void ClearScheduledRegion();	
	static TBool ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel);
	TBool ReadyToDraw() const;
	virtual void Render(CFbsBitGc * aGc, const TRegion& aRegion);	
	void AddSprite(CWsSpriteBase * aSprite);
	void RemoveSprite(CWsSpriteBase * aSprite);
	void Accessed();
	
	inline TBool IsNonFading() const;
	const TRegion& FadableRegion() const;
	inline TBool HasSprite() const;
	inline TBool HasAnimation() const;
	
public: // from CWsWindowBase
	TBool IsDSAHost() const;
	
public:	// from MWsWindow
	virtual TPoint Origin() const;
	virtual TRect AbsRect() const;
	virtual TSize Size() const;
	virtual TUint32 Handle() const;
	virtual MWsScreen * WsScreen() const;
	virtual void Invalidate(const TRect *) {}
	virtual MWsWindow * FindChildByHandle(TUint32 aHandle);
	virtual TInt OrdinalPriority() const;
	
protected:
// Shadow generation
	TPoint InquireOffset(TUint32 aHandle) const;
//
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
#if defined (_DEBUG)
public:
	TBool iInteresting;  // Often useful while debugging for conditional breakpoints.
#endif
protected:
	TRect iRel;
	TRect iAbs;
	TPoint iOrigin;
	RWsRegion iVisibleRegion;
	TUint iPointerFilter;
	TInt iPointerCapturePriority;
	TUint iFlags;
	CWsWindowRedraw *iRedraw;
	TSglQue<CWsDirectScreenAccess> iDSAs;
	CWsWindow * iNextScheduled;
	RWsRegion iScheduledRegion;
	// The access list is a "most recently drawn" ordering list of all redraw msg windows.
	static CWsWindow * iAccessListRecentEnd;
	static CWsWindow * iAccessListOldestEnd;
	CWsWindow * iAccessListPrev;
	CWsWindow * iAccessListNext;
	TUint8 iBlackMap;
	TUint8 iWhiteMap;
	RWsRegion iFadableRegion; // Region which can be faded (Visible region minus already faded regions)
public:
	/** The list of graphic contexts associated with this window. The list is managed by calling the
	GcActivated() and GcDeactivated() functions. 
	This is a linked list. The CWsGc class contains a pointer to the next item: CWsGc::iNextWinGc.
	*/
	CWsGc *iWinGcList;
	CWsAnim *iAnimList;	// Start of linked list of animated objects attached to the window
	CWsSpriteBase *iSpriteList; // Start of linked list of sprites attached to the window
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
	return (WinType() == EWinTypeClient && iParent->WinType() == EWinTypeGroup);
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

inline TBool CWsWindow::IsActive() const
	{
	return iFlags&EFlagActive;
	}

inline void CWsWindow::AddDSA(CWsDirectScreenAccess& aDirect)
	{
	iDSAs.AddLast(aDirect);
	}

inline void CWsWindow::RemoveDSA(CWsDirectScreenAccess& aDirect)
	{
	iDSAs.Remove(aDirect);
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

inline const RWsRegion * CWsWindow::ScheduledRegion() const
	{
	return &iScheduledRegion;
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

inline TBool CWsWindow::HasSprite() const
	{
	return (NULL != iSpriteList);
	}

inline TBool CWsWindow::HasAnimation() const
	{
	return (NULL != iAnimList);
	}
#endif
