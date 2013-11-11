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

#ifndef EDGEDWIN_H
#define EDGEDWIN_H

#include <w32std.h>
#include "compwin.h"


/*******************************************************************************
These windows draw a bordered rectangle (opaque) within a bordered rectangle
(translucent).  The inner rectangle is consistently drawn as a separate
redraw segment.
*******************************************************************************/
_LIT8(KEdgedWindowType, "Edged");
class CEdgedWin : public CCompWin
	{
public:
	static CEdgedWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }
	~CEdgedWin();
	
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	void SetSize(const TSize & aSize);
	virtual const TDesC8& TypeName() { return KEdgedWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CEdgedWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();

private:
	static TBool iEnabled;
	static TBool iTransparent;
	static TBool iRandomizeAlpha;
	static TBool iRandomizePenStyle;
	static TBool iRandomizeBrushStyle;
	TRect iOpaqueRect;
	TRegionFix<4> iTransparentRegion;
	TRgb iTransBgColor;	//outer rectangle, pen colour
	TRgb iTransFgColor; //outer rectangle, brush colour
	TRgb iBgColor;		//inner rectangle, brush colour
	TRgb iFgColor;		//inner rectangle, pen colour
	CGraphicsContext::TPenStyle iPenStyle;
	CGraphicsContext::TBrushStyle iBrushStyle;
	};


#endif // EDGEDWIN_H
