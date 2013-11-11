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
	virtual const TRegion& WindowArea() const;
	virtual void GenerateWindowRegion(RWsRegion &aRegion) const;
	void GenerateTopRegion(RWsRegion &aRegion) const;
	
public:	// from CWsObject
	virtual void CommandL(TInt aOpcode, const TAny *aCmdData);

public: // from MWsWindow
	virtual void Invalidate(const TRect * aRect = 0);
	
public:
	void GcActivated(CWsGc *aGc);
	void GcDeactivated(CWsGc *aGc);
	void ResetHiddenFlag();
	void SetClippedBaseArea(RWsRegion &aRegion) const;
	void SetOpaqueClippedBaseArea(RWsRegion &aRegion) const;
	inline const TRegion *BaseArea() const;
	virtual TUint RedrawPriority(TInt *aShift=NULL) const;
	inline void UpdateAnimArea();
	void AddKeyRectL(const TRect &aRect, TInt aScanCode, TBool aActivatedByPointerSwitchOn);
	void RemoveAllKeyRects();
	void SetVisible(TBool aState);
	inline TBool IsFadeBehind() const;
//
	TBool IsHidden();
	void SetFaded(TBool aFade,TUint8 aBlackMap,TUint8 aWhiteMap); 
	void RecalcChildAbs(const TPoint *aOffset);
	void ResetHiddenFlagsInParentAndChildren();
	static inline void SetAbsoluteFading(TBool aAbsoluteFading);
	inline const TRegion * GetUserOpaqueRegion();
	void OffsetUserTransparentRegion(const TPoint& aOffset);
	void SetUserOpaqueRegion();
	void ReactivateGcs();
	void ScheduleRegionUpdate(const TRegion* aDefinitelyDirty);
	
	void DoMoveWindowToGroupL(TInt aIdentifier);
	void SetScreenDeviceValidState(TBool aState);
	TBool SetScreenDeviceValidStateFlag(TBool aState);
	void SetInactive();
protected:
	void Activate();
	void SetExtentL(const TPoint *aPos, const TSize *aSize);
	static void ClipWindows(TRegion &region,const CWsClientWindow *start, const CWsClientWindow *end, TBool aClipTranslucent);
	void GenerateArea(RWsRegion &aArea, TBool aClipTranslucent) const;
	void SetAbsFromRel();
	void Scroll(const TRect &aClipRect, const TPoint &aPoint, const TRect &aRect);
	void CalcBaseArea();
	void OffsetBaseArea(const TPoint &aOffset);
	void DeleteBaseArea();
	void SetCornerTypeL(TCornerType aType, TInt aFlags, TRegion *aNewBaseArea=NULL);
	void ResetHiddenFlags();
	TDblQue<TPointerKeyList> *PointerKeyList() const;
	void SetUserTransparentRegion(RWsRegion* aRegion);
	// from CWsWindowBase thru CWsWindow
	TBool IsDSAHost() const;
	
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

inline CWsClientWindow *CWsClientWindow::PrevSiblingMultiParent() const
	{
	return((CWsClientWindow *)CWsWindowBase::PrevSiblingMultiParent());
	}

inline CWsClientWindow *CWsClientWindow::NextSiblingMultiParent() const
	{
	return((CWsClientWindow *)CWsWindowBase::NextSiblingMultiParent());
	}

inline const TRegion * CWsClientWindow::GetUserOpaqueRegion()
	{
	return iUserDefinedOpaqueRegion;
	}
	
#endif
