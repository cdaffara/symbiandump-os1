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
// CWsRootWindow and associated classes definitions
// 
//

#ifndef __ROOTWIN_H__
#define __ROOTWIN_H__

#include "server.h"
#include "window.h"

class CWsRootWindow : public CWsWindow
	{
public:
	CWsRootWindow(CWsClient* aOwner, CScreen* aScreen);
	~CWsRootWindow();
	void ConstructL();
	const CWsWindow *PointerWindow(const TPoint &inPos,TPoint *outPos, TPoint *parentPos, const CWsWindow *grabWin,
											const CWsWindow *&aOriginalWinItIsIn,const CWsWindowGroup *aForceInGroup);
	CWsWindowGroup *Child() const;
	inline CWsWindowBase *PrevSibling() const;
	inline CWsWindowBase *NextSibling() const;
	CWsWindowBase *Parent() const;
	virtual void GenerateWindowRegion(RWsRegion &aRegion) const;
	void SetColor(TRgb aColor);
	void SetColorIfClear();
	void InvalidateWholeScreen();
	void Invalidate(RWsRegion* aRegion);
	void AdjustCoordsDueToRotation();
	void ClearDisplay();
	void SetSystemFaded(TBool aFaded, TUint8 aBlackMap, TUint8 aWhiteMap);
	CWsWindowGroup* WindowGroup(TInt aWindowGroup);
	CWsTopClientWindow *FirstTopClientWindow() const;
	inline TRgb DefaultBackgroundColor() const;
public:	// from CWsObject
	void CommandL(TInt aOpcode, const TAny *aCmdData);
public: // from MWsWindowTreeNode
	const MWsWindow* Window() const;	
public: // from MWsWindow
	const TRegion& WindowArea() const;
public: // from CWsWindowBase
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
private:
	inline CWsBlankWindow* BlankRedraw();
private:
	TRegionFix<1> iArea;
	TRgb iDefaultBackgroundColor;
	};
	
inline CWsBlankWindow* CWsRootWindow::BlankRedraw()
	{
	return STATIC_CAST(CWsBlankWindow*,iRedraw);
	}

inline CWsWindowGroup *CWsRootWindow::Child() const
	{
	return (CWsWindowGroup *)iChild;
	}

inline CWsWindowBase *CWsRootWindow::NextSibling() const
	{
	return iSibling;
	}

inline CWsWindowBase *CWsRootWindow::PrevSibling() const
	{
	return GetPrevSibling();
	}

inline CWsWindowBase *CWsRootWindow::Parent() const
	{
	return iParent;
	}
inline TRgb CWsRootWindow::DefaultBackgroundColor() const
	{
	return iDefaultBackgroundColor;
	}	

#endif
