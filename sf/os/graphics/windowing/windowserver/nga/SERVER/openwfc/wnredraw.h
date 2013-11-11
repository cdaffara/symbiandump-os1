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

class TSurfaceId;

class CPlaybackGc;
class MWsGraphicsContext;
class TBackgroundAttributes;

//
// Base class defining how a window is redrawn, i.e. Backed up, redraw from client, blanked...
//
class CWsWindowRedraw : public CBase
	{
private:
	enum TStateFlags
		{
		EHasElement=0x01,
		EMayContainElement=0x10,
		};
public:
	~CWsWindowRedraw();
	virtual void ConstructL();
	virtual TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)=0;
	virtual TRgb BackColor() const=0;
	virtual TBool NeedsRedraw() const=0;
	virtual TBool GetRedrawRect(TRect &aRect) const=0;
	virtual const TRegion &BaseDrawRegion() const;
	virtual void ClipInvalidRegion(const TRect &aRect);
	virtual const TRegion& InvalidArea() const;
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
	TInt DrawBackgroundColor(const TRegion& aRegion, TBool aDoFillColor);
	virtual TBool Contains(const TArray<TGraphicDrawerId>& aDrawers,const TRegion& aRegion) const;
	const TRegion * RedrawRegion();
	virtual TBool ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel);
	virtual CFbsDevice* OutputDevice() const;
	virtual void VisibleRegionChange();
	virtual TBool ReadyToDraw() const;
	void PreDrawWindow(MWsGraphicsContext * aGc, const TRegion &aWindowRegion);
	void PostDrawWindow(MWsGraphicsContext* aGc, const TRegion& aWindowChildNodeRegion);
	virtual void Invalidate(const TRect *) {}	
	virtual TInt SizeInBytes() const { return 0; }
	inline TBool HasElement() const;
	inline TBool MayContainElement() const;
	inline void SetHasElement(TBool);
	inline TBool HasElementExtentDefined() const;
	inline void  ClearMayContainElement();
	virtual TBool RedrawingInProgress() const; 
	virtual void WindowClosing();
	virtual TBool IsRedrawStoreEmpty() const { return EFalse; }
	virtual TBool IsBackgroundClearEnabled() const { return ETrue; }
	virtual CFbsBitmap* BitmapFromHandle(TInt /*aHandle*/) const { return NULL; }

	void SetDsaElementL();
	inline void QueueRedraw();
	void Fade(MWsGraphicsContext * aGc, const TRegion& aRegion);
protected:
	CWsWindowRedraw(CWsWindow *aWin);
	void SetReply(TInt aReply);
	void OwnerPanic(TClientPanic aPanic);
	inline CScreen* Screen() const;
	inline CWsClientWindow *CliWin() const;
	void DrawWindowAnimsL(MWsGraphicsContext * aGc, const TRegion& aRegion);
	TBackgroundAttributes& AcquireBackgroundElementL();
	void SetBackgroundSurfaceL(const TSurfaceId& aSurface);
	void SetBackgroundSurfaceL(const TSurfaceConfiguration& aConfiguration, TBool aTriggerRedraw, TBool aAllowScreenSurface);
	void SetElementExtentL(TRect& aNewExtent, TBackgroundAttributes& aAttributes);
	void ElementRedraw(const TRect& aOldExtent, const TRect& aNewExtent, TBool aAlwaysScheduleUpdate);
	void RemoveBackgroundSurface(TBool aTriggerRedraw);
	void RemoveBackgroundElement(TBool aTriggerRedraw);
	void GetBackgroundSurfaceL(TSurfaceConfiguration& aConfiguration);
	void ReleaseBackgroundElement();
	void SetMayContainElementFlags();
private:
	void DoFade(const TRegion& aRegion);
	void DrawWindowAnims(MWsGraphicsContext * aGc, const TRegion& aRegion);
	void DrawCursorAndSprites(MWsGraphicsContext * aGc, const TRegion& aRegion);
	TBool HasDsaElement() const;

protected:
	CWsWindow *iWsWin;	// Link back to main window class
	TUint iStateFlags; 	// Flags affecting how drawing is done
	const TRegion * iRedrawRegion; // During a scheduled draw, this is the window area scheduled to be drawn.
	};							   // In ChangeTracking mode, iRedrawRegion doesn't include the area to be drawn by window-sprites, cursors or window-anims.
								   // All of them use the sprite region, declared in CWsWindow::iDirtySpriteRegion.

class CWsBlankWindow : public CWsWindowRedraw
	{
public:
	CWsBlankWindow(CWsWindow *aWin);
	~CWsBlankWindow();
	void ConstructL();
	
	//from CWsWindowRedraw
	TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd);
	TBool NeedsRedraw() const;
	TBool GetRedrawRect(TRect &aRect) const;
	void DrawWindow();
	
	void SetColor(TRgb aColor);
	inline void SetBackgroundClear();
	inline TBool IsBackgroundColor();
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

inline TBool CWsWindowRedraw::HasElement() const
	{
	return (iStateFlags&EHasElement)>0;
	}
inline void CWsWindowRedraw::SetHasElement(TBool aSet)
	{
	TInt oldStateFlags = iStateFlags; 
	if (aSet)
		{
		iStateFlags|=EHasElement;
		}
	else
		{
		iStateFlags&=~EHasElement;
		}
	if (oldStateFlags!=iStateFlags)
		{
		iWsWin->SetupVisibleRegionTracking(aSet);
		}
	}

inline TBool CWsWindowRedraw::MayContainElement() const
	{
	return (iStateFlags&EMayContainElement)>0;
	}

inline void CWsWindowRedraw::ClearMayContainElement()
	{
	iStateFlags&=~EMayContainElement;
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

inline const TRegion * CWsWindowRedraw::RedrawRegion()
	{
	return iRedrawRegion;
	}

#endif
