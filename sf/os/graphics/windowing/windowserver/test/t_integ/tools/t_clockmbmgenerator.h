// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined __TCLOCKMBMGENERATOR_H__
#define __TCLOCKMBMGENERATOR_H__

#include <w32std.h>		//CWindowGc, CWsScreenDevice

_LIT(KClockFace, "z:\\graphics\\clock.mbm");
_LIT(KClockFrame,"c:\\graphics\\clockFrame");
_LIT(KMultiClockFrame,"c:\\graphics\\multi_clock_frame.mbm");
_LIT(KEmpty, "");
_LIT(KBmpSuffix, ".bmp");
_LIT(KTClockMbmGeneratorAppPanic, "T_ClockMbmGenerator");

typedef struct clockhand
	{
	TReal handLength;
	TReal arrowLength;
	TReal arrowAngle;
	TInt  red;
	TInt  green;
	TInt  blue;
	TInt  handSize;
	TInt  divisor;
	}clockhand;

const TInt KScreenNo = 0;
const TInt KMultiBitmapFileSize = 100;
const TInt KFixedNumWidth = 3;

GLDEF_C TInt E32Main(void);
LOCAL_C void StartGeneratorL(void);
LOCAL_C void AddHand(TInt aSecondsAfterMidnight, clockhand aHand, CFbsBitGc* context);
LOCAL_C void AddFrameCount(TInt aFrameCount, CFbsBitGc* context);

#endif //__TCLOCKMBMGENERATOR_H__
