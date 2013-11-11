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

#ifndef BACKEDUPWIN_H
#define BACKEDUPWIN_H

#include <w32std.h>
#include "compwin.h"

/*******************************************************************************
These are backedup windows and draw a simple cross shape
*******************************************************************************/
_LIT8(KBackedupWindowType, "Backed up");
class CBackedupWin : public CCompWin
	{
public:
	static CBackedupWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }
	~CBackedupWin();
	
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	void SetSize(const TSize & aSize);
	virtual const TDesC8& TypeName() { return KBackedupWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CBackedupWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();
	
private:
	static TBool iEnabled;
	static TBool iRandomizePenStyle;
	static TBool iRandomizeBrushStyle;
	TRgb iBgColor;
	TRgb iFgColor;
	CGraphicsContext::TPenStyle iPenStyle;
	CGraphicsContext::TBrushStyle iBrushStyle;
	CWindowGc * iBkWsGc;
	};


#endif // BACKEDUPWIN_H
