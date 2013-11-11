// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent
*/

#ifndef COMPWIN_H
#define COMPWIN_H

#include <w32std.h>
#include "test_step_conf.h"

//common configuration parameter names
_LIT(KT_WservStressParamEnabled, "enabled");
_LIT(KT_WservStressParamTransparent, "transparent");
_LIT(KT_WservStressParamRandomizeAlpha, "randomize_alpha");
_LIT(KT_WservStressParamRandomizePenStyle, "randomize_pen_style");
_LIT(KT_WservStressParamRandomizeBrushStyle, "randomize_brush_style");
_LIT(KT_WservStressParamTransparentForegroundWindow, "transparent_foreground_window");


/*******************************************************************************
Base class for all windows in this test.
*******************************************************************************/
_LIT8(KCommonWindowType, "Common");
class CCompWin : public CBase
	{
public:
	static CCompWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	~CCompWin();
	
	TInt NumChildren() { return iChildren.Count(); }
	RDrawableWindow * Window() { return iWindow; }
	const TSize Size() const { return iSize; }
	const TPoint Pos() const { return iPos; }
	virtual void SetPos(const TPoint & aPos);
	virtual void SetSize(const TSize & aSize);
	CCompWin * CCompWin::RandomWindow();
	void AddChildL(CCompWin* aChild);
	void RemoveChild(CCompWin* aChild);
	void HandleRedraw(TWsRedrawEvent& aEvent);
	void SendToBackL();
	void BringToFrontL();
	void Dump(RFile & aFile, TPoint & aOrigin, TInt aDepth, CCompWin * aMark);
	virtual void DumpDetails(RFile & aFile, TInt aDepth);
	virtual void Redraw(const TRect& aRect) = 0;
	virtual void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	virtual void ClearBitmapBackground(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	virtual const TDesC8& TypeName() { return KCommonWindowType; }
	//If a window has been modified in TickL it should return ETrue
	virtual TBool TickL() { return EFalse; }
	virtual void ToggleVisible();
	virtual TBool QueryReadyForVerification();
	virtual void SubSelfFromRegion(TRegion& aRegion, const TRect& aClip, const TPoint& aOrigin);
	virtual TRgb BackgroundColor() { return TRgb(0, sBackgroundAlpha); }
	TBool IsVisible() { return iVisible; }
	void SetVerifyTick(TUint32 aTick);
	
protected:
	CCompWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void PreConstructL(TBool aTransparency = ETrue, TBool aBackedup = EFalse);
	void PostConstructL();
	static CGraphicsContext::TDrawMode GetRandomDrawMode();
	static CGraphicsContext::TPenStyle GetRandomPenStyle();
	static CGraphicsContext::TBrushStyle GetRandomBrushStyle();
protected:
	static TInt sBackgroundAlpha;
	RWsSession& iWs;
	RWindowGroup* iGroup;
	CCompWin* iParent;
	CWindowGc& iWsGc;
	RDrawableWindow* iWindow;
	RWindow* iRedrawWindow;
	RBackedUpWindow* iBackedupWindow;
	RPointerArray<CCompWin> iChildren;
	TPoint iPos;
	TSize iSize;
	TBool iVisible;
	TUint32 iVerifyTick;
	};

#endif // COMPWIN_H
