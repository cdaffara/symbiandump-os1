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

#ifndef COORDINATEWIN_H
#define COORDINATEWIN_H

#include <w32std.h>
#include "compwin.h"

const TInt KRegionSafe = 6; // 1 rect + 1 rect can be > 2 rects

/*******************************************************************************
These windows draw ellipses with various origins and clipping regions.
They also use 4 redraw regions, and are completely transparent except for a white edge.
The intent is to test the coordinate space code for offsets and clipping.
*******************************************************************************/
_LIT8(KCoordinateWindowType, "Coordinate space");
class CCoordinateWin : public CCompWin
	{
public:
	static CCoordinateWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }
	~CCoordinateWin();
	
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	void SetSize(const TSize & aSize);
	virtual const TDesC8& TypeName() { return KCoordinateWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CCoordinateWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();

private:
	static TBool iEnabled;
	static TBool iTransparent;
	struct TSegment
		{
		TRect iBoundingRect;
		TPoint iOrigin;
		TRegionFix<KRegionSafe> iClippingRegion;
		TRect iRect;
		};
	TSegment iSegments[4];
	};



#endif // COORDINATEWIN_H
