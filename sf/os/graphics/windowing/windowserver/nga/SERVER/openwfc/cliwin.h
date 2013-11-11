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
// CWsClientWindow and associated classes definitions
// 
//

#ifndef __CLIWIN_H__
#define __CLIWIN_H__

#include "server.h"
#include "window.h"
#include <graphics/wselement.h>

class CWsClientWindow : public CWsWindow
	{
protected:
	enum
		{
		KWinRedrawPriBitsPerLevel=4,
		KWinRedrawPriMaxOrdinal=(1<<KWinRedrawPriBitsPerLevel)-1,		// 15
		KWinRedrawPriMaxLevel=32/KWinRedrawPriBitsPerLevel-1,			// 7
		};
				
public:
  	CWsClientWindow(CWsClient* aOwner, CScreen* aScreen);
	~CWsClientWindow();
	virtual void ConstructL(const TWsClCmdCreateWindow &aCmd,CWsWindowBase *aParent, TBool aScreenDeviceIsInvalid);
	CWsClientWindow *Child() const;
	inline CWsClientWindow *PrevSibling() const;
	inline CWsClientWindow *NextSibling() const;
	inline CWsClientWindow *PrevSiblingMultiParent() const;
	inline CWsClientWindow *NextSiblingMultiParent() const;
	virtual void GenerateWindowRegion(RWsRegion &aRegion) const;
	void GenerateTopRegion(RWsRegion &aRegion) const;
	
public:	// from CWsObject
	void CommandL(TInt aOpcode, const TAny *aCmdData);

public: // from MWsWindow
	void Invalidate(const TRect * aRect = 0);
	const TRegion& WindowArea() const;
	
public:
	void GcActivated(CWsGc *aGc);
	void GcDeactivated(CWsGc *aGc);
	void ResetHiddenFlag();
	const TRegion &InvalidArea() const;
	void GetClippedBaseArea(RWsRegion &aRegion) const;
	void GetOpaqueClippedBaseArea(RWsRegion &aRegion) const;
	inline const TRegion *BaseArea() const;
	virtual TUint RedrawPriority(TInt *aShift=NULL) const;
	inline void UpdateAnimArea();
	void AddKeyRectL(const TRect &aRect, TInt aScanCode, TBool aActivatedByPointerSwitchOn);
	void RemoveAllKeyRects();
	void SetVisible(TBool aState);
	CWsTopClientWindow *TopClientWindow();
	const TRegion *DrawingRegion();
	inline TBool IsFadeBehind() const;
//
	TBool IsHidden();
	void SetFaded(TBool aFade, TUint8 aBlackMap, TUint8 aWhiteMap, TBool aNotifyObserver);
	void SetFaded(TBool aFade, TUint8 aBlackMap, TUint8 aWhiteMap, TBool aNotifyObserver, TBool& aStateChanged); 
	void RecalcChildAbs(const TPoint *aOffset);
	void ResetHiddenFlagsInParentAndChildren();
	static inline void SetAbsoluteFading(TBool aAbsoluteFading);
	inline TBool IsBecomingOpaque() const;	
	TBool IsInfrontOf(const CWsWindowBase* aWin) const;
	inline const TRegion * GetUserOpaqueRegion();
	void OffsetUserTransparentRegion(const TPoint& aOffset);
	void SetUserOpaqueRegion();
	void ReactivateGcs();
	void ScheduleRegionUpdate(const TRegion* aDefinitelyDirty);
	void AddRedrawRegion(const TRegion& aRegion, TBool aSchedule, TRedrawDepth aDepth);
	const TRegion*	DebugRegion(TInt aType)const;
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
	
	inline void SetOriginalSrcElementRect(const TRect &aSrcRect);
	inline void SetOriginalDestElementRect(const TRect &aDestRect);
	TRect GetOriginalSrcElementRect() const;
	TRect GetOriginalDestElementRect() const;
	void GetElementFlipAndRotation(TBool& aElemetFlip, MWsElement::TElementRotation& aElemenetRotation);

protected:
	virtual void Shutdown();	// Part of two phase destruction
	void Activate();	
	void SetExtentL(const TPoint *aPos, const TSize *aSize);
	static void ClipWindows(TRegion &region,const CWsClientWindow *start, const CWsClientWindow *end, TBool aClipTranslucent);
	void GenerateArea(RWsRegion &aArea, TBool aClipTranslucent) const;
	void SetAbsFromRel();
	void MovePosition(const TPoint& aPos);
	void IncreaseSize(const TSize& aSize,TSize& aOldSize);
	void ReduceSize(const TSize& aSize,TSize& aOldSize);
	void Scroll(const TRect &aClipRect, const TPoint &aPoint, const TRect &aRect);
	void CalcBaseArea();
	void OffsetBaseArea(const TPoint &aOffset);
	void DeleteBaseArea();
	void SetCornerTypeL(TCornerType aType, TInt aFlags, TRegion *aNewBaseArea=NULL, TBool aNotifyShapeChanged=ETrue);
	void ResetHiddenFlags();
	TDblQue<TPointerKeyList> *PointerKeyList() const;
	void SetUserTransparentRegion(RWsRegion* aRegion);
	void UpdateElementExtent(const TPoint* aOffset = NULL);
	void SetElementOpacity(TInt aOpacity);

	// from CWsWindowBase
	TBool IsDSAHost() const;
	TBool IsActivated() const;

private:
    void GetBaseAreaOfNode(RWsRegion& aRegion) const;
    void GetOpaqueBaseAreaOfNode(RWsRegion& aRegion) const;
    TInt GetNonOpaqueBaseAreaOfNode(RWsRegion& aRegion) const;
    void ClipRegionToBaseArea(RWsRegion& aRegion) const;
    void ClipRegionToOpaqueBaseArea(RWsRegion& aRegion) const;


private:
	TInt iCornerData;
	TUint iBackupsRequested;
	TRegion *iBaseArea;			// The base area of the window without any clipping from windows taken into account
	TDblQue<TPointerKeyList> *iPointerKeyList;
	static TBool iAbsoluteFading;
	/**
	By default the full window region is transparent if the window is marked 
	as transparent. If this is not the desired behaviour the client can
	define the transparent region itself.
	
	@see RWindow::SetTransparentRegion()
	*/
	RWsRegion* iUserDefinedTransparentRegion;
	RWsRegion* iUserDefinedOpaqueRegion;
	
	TRect iOriginalSrcElementRect;
	TRect iOriginalDestElementRect;
	};

class CWsTopClientWindow : public CWsClientWindow
	{
public:
  	CWsTopClientWindow(CWsClient* aOwner, CScreen* aScreen);
	void ConstructL(const TWsClCmdCreateWindow &cmd,CWsWindowBase *aParent, TBool aScreenDeviceIsInvalid);
	virtual void SetOrdinalPosition(TInt pos);
	void DoMoveWindowL(TInt aIdentifier);
	inline CWsTopClientWindow *NextSiblingTop() const;
	inline CWsTopClientWindow *PrevSiblingTop() const;
	inline CWsTopClientWindow *PrevSiblingMultiParent() const;
	inline CWsTopClientWindow *NextSiblingMultiParent() const;
	inline CWsWindowGroup *Parent() const;
	
	void SetInactive();
	void SetScreenDeviceValidState(TBool aState);
	TBool SetScreenDeviceValidStateFlag(TBool aState);
	virtual TUint RedrawPriority(TInt *aShift=NULL) const;
	};

inline CWsClientWindow *CWsClientWindow::Child() const
	{
	return((CWsClientWindow *)iChild);
	}

inline CWsClientWindow *CWsClientWindow::NextSibling() const
	{
	return((CWsClientWindow *)iSibling);
	}

inline CWsClientWindow *CWsClientWindow::PrevSibling() const
	{
	return((CWsClientWindow *)GetPrevSibling());
	}

inline const TRegion *CWsClientWindow::BaseArea() const
	{
	return iBaseArea;
	}

inline void CWsClientWindow::UpdateAnimArea()
	{
	iRedraw->UpdateAnimArea();
	}

inline TBool CWsClientWindow::IsFadeBehind() const
	{
	return(iFlags&EFlagFadeBehind);
	}

inline void CWsClientWindow::SetAbsoluteFading(TBool aAbsoluteFading)
	{
	iAbsoluteFading=aAbsoluteFading;
	}

inline CWsTopClientWindow *CWsTopClientWindow::NextSiblingTop() const
	{
	return((CWsTopClientWindow *)iSibling);
	}

inline CWsTopClientWindow *CWsTopClientWindow::PrevSiblingTop() const
	{
	return((CWsTopClientWindow *)GetPrevSibling());
	}

inline CWsTopClientWindow *CWsTopClientWindow::PrevSiblingMultiParent() const
	{
	return((CWsTopClientWindow *)CWsWindowBase::PrevSiblingMultiParent());
	}

inline CWsTopClientWindow *CWsTopClientWindow::NextSiblingMultiParent() const
	{
	return((CWsTopClientWindow *)CWsWindowBase::NextSiblingMultiParent());
	}

inline CWsWindowGroup *CWsTopClientWindow::Parent() const
	{
	return((CWsWindowGroup *)iParent);
	}

inline const TRegion * CWsClientWindow::GetUserOpaqueRegion()
	{
	return iUserDefinedOpaqueRegion;
	}

inline void CWsClientWindow::SetOriginalSrcElementRect(const TRect &aSrcRect)
    {
    iOriginalSrcElementRect = aSrcRect;
    }

inline void CWsClientWindow::SetOriginalDestElementRect(const TRect &aDestRect)
    {
    iOriginalDestElementRect = aDestRect;
    }
	
#endif
