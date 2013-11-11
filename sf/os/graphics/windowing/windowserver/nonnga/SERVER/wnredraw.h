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
// CWsWindowRedraw and associated classes definitions
// 
//

#ifndef __WNREDRAW_H__
#define __WNREDRAW_H__

#include "server.h"
#include "panics.h"

class CPlaybackGc;

//
// Base class defining how a window is redrawn, i.e. Backed up, redraw from client, blanked...
//
class CWsWindowRedraw : public CBase
	{
public:
	~CWsWindowRedraw();
	virtual void ConstructL();
	virtual TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)=0;
	virtual TRgb BackColor() const=0;
	virtual TBool NeedsRedraw() const=0;
	virtual TBool GetRedrawRect(TRect &aRect) const=0;
	virtual void ClipInvalidRegion(const TRect &aRect);
	virtual void Resize(const TSize &aSize, const TSize &aOldSize);
	virtual CWsBackedUpWindow *Backup() const;
	virtual void Scroll(const TRect &aClipRect, const TPoint &aOffset,const TRect &aRect);
	virtual void UpdateAnimArea();
	virtual void PrepareForResizeL(const TSize &aSize, TSize &aOldSize);
	virtual void Moved() {}
	virtual void ClearRedrawStore(TBool aClearPendingRedraw=EFalse);
	virtual TBool DrawCommand(CWsGc* aGc,const TAny *aCmdData);
	virtual void GcAttributeChange(CWsGc* aGc,const TAny *aCmdData);
	virtual void GcDeactivate(CWsGc* aGc);
	inline CWsWindow *WsWin();	// Link back to main window class
	virtual void ClientExposing();
	virtual void DrawWindow()=0;
	TInt DrawBackgroundColor(const TRegion* aRegion);
	virtual TBool Contains(const TArray<TGraphicDrawerId>& aDrawers,const TRegion& aRegion) const;
	void DrawAnims(CFbsBitGc * aGc, const TRegion& aRegion);
	const TRegion * GlobalRedrawRegion();
	virtual TBool ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel);
	virtual CFbsDevice* OutputDevice() const;
	virtual void VisibleRegionChange();
	virtual TBool ReadyToDraw() const;
	void PreDrawWindow(CFbsBitGc * aGc, const TRegion &aRegion);
	void PostDrawWindow(CFbsBitGc * aGc);
	virtual void Invalidate(const TRect *) {}	
	virtual TInt SizeInBytes() const { return 0; }
	virtual void WindowClosing();

protected:
	CWsWindowRedraw(CWsWindow *aWin);
	void SetReply(TInt aReply);
	void OwnerPanic(TClientPanic aPanic);
	inline void QueueRedraw();
	inline CScreen* Screen() const;
	inline CWsClientWindow *CliWin() const;
	void DrawAnimsL(CFbsBitGc * aGc, const TRegion& aRegion);
protected:
	CWsWindow *iWsWin;	// Link back to main window class
	const TRegion * iGlobalRedrawRegion;// During a scheduled draw, this is the area scheduled to be drawn.
	};

class CWsBlankWindow : public CWsWindowRedraw
	{
public:
	CWsBlankWindow(CWsWindow *aWin);
	~CWsBlankWindow();
	
	//from CWsWindowRedraw
	TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd);
	TBool NeedsRedraw() const;
	TBool GetRedrawRect(TRect &aRect) const;
	
	void SetColor(TRgb aColor);
	inline void SetBackgroundClear();
	inline TBool IsBackgroundColor();
	void DrawWindow();
private:
	TRgb BackColor() const;
private:
	TRgb iColor;
	TBool iNoColor;
	};
	
#include "window.h"

inline CWsWindow *CWsWindowRedraw::WsWin()
	{
	return iWsWin;
	}

inline CScreen* CWsWindowRedraw::Screen() const
	{
	return iWsWin->Screen();
	}

inline CWsClientWindow *CWsWindowRedraw::CliWin() const
	{
	return (CWsClientWindow *)iWsWin;
	}

inline void CWsBlankWindow::SetBackgroundClear()
	{
	iNoColor=ETrue;
	}

inline TBool CWsBlankWindow::IsBackgroundColor()
	{
	return !iNoColor;
	}

inline void CWsWindowRedraw::QueueRedraw()
	{
	if (iWsWin->IsActive())
		{
		iWsWin->WsOwner()->RedrawQueue()->AddInvalid(this);
		}
	}

inline const TRegion * CWsWindowRedraw::GlobalRedrawRegion()
	{
	return iGlobalRedrawRegion;
	}

#endif
