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

#ifndef SPRITEWIN_H
#define SPRITEWIN_H

#include <w32std.h>
#include "compwin.h"


_LIT8(KSpritedWindowType, "Sprited");

class CSpritedWin : public CCompWin
	{
public:
	static CSpritedWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }
	~CSpritedWin();
	
	// from CCompWin
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	void ClearBitmapBackground(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	void SetSize(const TSize & aSize);
	void SetPos(const TPoint & aPos);
	virtual const TDesC8& TypeName() { return KSpritedWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CSpritedWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();
	
	// from CCompWin
	TBool QueryReadyForVerification();
 
private:
	static TBool iEnabled;
	static TBool iTransparent;
	static TBool iRandomizePenStyle;
	static TBool iRandomizeBrushStyle;
	static TBool iRandomizeDrawMode;
	static TBool iTransparentForegroundWindow;
	static TInt  iMaxRandomStepX;
	static TInt  iMaxRandomStepY;
	static TInt  iMaxRandomOffsetX;
	static TInt  iMaxRandomOffsetY;

	enum TValues
	{
	ENofSlides = 4,
	ESpriteSzX  = 64,
	ESpriteSzY  = 64,
	ESpriteSzXHalf  = ESpriteSzX / 2,
	ESpriteSzYHalf  = ESpriteSzY / 2,
	EStepWidth		= ESpriteSzXHalf / (ENofSlides+1),
	EStepHeight		= ESpriteSzYHalf / (ENofSlides+1),
	ESlideDurMult	= 100000,	//duration multiplier (microseconds)
	ESlideDurMaxTenthSec = 15,	//max random sprite frame duration
	EFrameBorderSenseMs = 10000
	};
	
	RWsSprite 				iSprite;
	CFbsBitmap* 			iSpriteContent [ENofSlides];
	CFbsBitmap* 			iSpriteMask [ENofSlides];
	CFbsBitmapDevice*		iSpriteCntDevice [ENofSlides];
	CFbsBitmapDevice*		iSpriteMskDevice [ENofSlides];
	struct TSpriteMember 	iSpriteDef [ENofSlides];
	TBool					iConstructed;
	
	TRgb					iBgColor;
	CGraphicsContext::TPenStyle   iPenStyle;
	CGraphicsContext::TBrushStyle iBrushStyle;
	CGraphicsContext::TDrawMode   iDrawMode;	

	TTimeIntervalMicroSeconds32
							iSlideDuration; //duration for a single slide in microseconds
	
	TUint32 				iSpriteStartup; //time (in ticks) when sprite was activated
	TInt 					iKernelTicksPeriod; //microseconds per tick
	RBlankWindow*  			iForeWin; //transparent foreground window
	};


#endif // SPRITEWIN_H
