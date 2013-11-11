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

#ifndef ENORMOUSWIN_H
#define ENORMOUSWIN_H


#include <w32std.h>
#include "compwin.h"


/*******************************************************************************
These windows are very very big, but do not attempt to draw themselves in their
entirety at any point.  They request that the redraw store is disabled so that
they don't get asked to.
*******************************************************************************/
_LIT8(KEnormousWindowType, "Enormous");
class CEnormousWin : public CCompWin
	{
private:
	enum { EScale = 1000, ESegmentSize = 32 };
public:
	static CEnormousWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }
	~CEnormousWin();
	
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	virtual const TDesC8& TypeName() { return KEnormousWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CEnormousWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();

private:
	static TBool iEnabled;
	static TBool iTransparent;
	static TBool iRandomizePenStyle;
	static TBool iRandomizeBrushStyle;
	CGraphicsContext::TPenStyle iPenStyle;
	CGraphicsContext::TBrushStyle iBrushStyle;
	};


#endif // ENORMOUSWIN_H
