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
	virtual const TRegion& WindowArea() const;
	virtual void GenerateWindowRegion(RWsRegion &aRegion) const;
	void SetColor(TRgb aColor);
	void SetColorIfClear();
	void InvalidateWholeScreen();
	void Invalidate(RWsRegion* aRegion);
	void ScreenSizeChanged(const TBool aSwapWidthAndHeight);
	void OrientationChanged();
	void ClearDisplay();
	void SetSystemFaded(TBool aFaded, TUint8 aBlackMap, TUint8 aWhiteMap);
	CWsWindowGroup* WindowGroup(TInt aWindowGroup);
	CWsClientWindow *FirstTopClientWindow() const;
	inline TRgb DefaultBackgroundColor();	
	
public:	// from CWsObject
	virtual void CommandL(TInt aOpcode, const TAny *aCmdData);
	
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

inline TRgb CWsRootWindow::DefaultBackgroundColor() 
	{
	return iDefaultBackgroundColor;
	}	

#endif
