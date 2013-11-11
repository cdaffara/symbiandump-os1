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

#ifndef __BACKEDUPWINDOW_H__
#define __BACKEDUPWINDOW_H__

#include "wnredraw.h"
#include "window.h"

class CWsBackedUpWindow : public CWsWindowRedraw
	{
public:
	static void StaticInitL();
	static void StaticDestroy();
public:
	CWsBackedUpWindow(CWsWindow *aWin, TDisplayMode aDisplayMode);
	void ConstructL();
	~CWsBackedUpWindow();
	TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd);
	void Resize(const TSize &aSize, const TSize &aOldSize);
	inline TDisplayMode DisplayMode() const;
	void PrepareForResizeL(const TSize &aSize, TSize &aOldSize);
	void DrawWindow();
	virtual CFbsDevice* OutputDevice() const;
	virtual TBool DrawCommand(CWsGc*,const TAny*);
	
private:
	void ActivateGc();
	void SetSizeInTwips();
	
	//from CWsWindowRedraw
	TRgb BackColor() const;
	TBool NeedsRedraw() const;
	TBool GetRedrawRect(TRect &aRect) const;
	CWsBackedUpWindow *Backup() const;
	void Scroll(const TRect &aClipRect, const TPoint &aOffset,const TRect &aRect);
	
	void MarkDirtyAndSchedule(const TRegion& aRegion);
	void UpdateScreen(const TRegion& aRegion);
private:
	static CFbsBitGc *iBitGc;
	CFbsBitmapDevice *iBitmapDevice;
	CFbsBitmap *iBitmap;
	TDisplayMode iDisplayMode;
	RWsRegionBuf<10> iCurrentRegion; //Calculated when the backedupwindow resizes. Stores local window coordinates.
	TBool iHasBeenScheduled;
	};

inline TDisplayMode CWsBackedUpWindow::DisplayMode() const
	{
	return iDisplayMode;
	}

#endif
