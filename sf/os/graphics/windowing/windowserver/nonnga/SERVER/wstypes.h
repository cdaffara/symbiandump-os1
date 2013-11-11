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
//

#ifndef __WSTYPES_H__
#define __WSTYPES_H__

#include <e32std.h>
#include "panics.h"

/**
Enums
*/
enum TRedrawDepth
	{
	ERedrawAll,
	ERedrawTopOnly
	};

/**
Regions
*/
#if defined(_DEBUG)
#define STACK_REGION RWsRegion	// Good for testing alloc heaven as RRegionBufs can hide it
#else
#define STACK_REGION RRegionBuf<10>
#endif

#if defined(_DEBUG)
class RWsRegion : public RRegion
	{
public:
	enum { EDefGran = 5 };
public:
	RWsRegion();
	RWsRegion(TInt aGran);
	RWsRegion(const RRegion& aRegion);
	RWsRegion(const TRect& aRect,TInt aGran=EDefGran);
	RWsRegion(TInt aCount,TRect* aRectangleList,TInt aGran=EDefGran);
	TBool Contains(const RWsRegion& aRegion);
	~RWsRegion() { WS_ASSERT_DEBUG(iCount == 0, EWsPanicInvalidRegion); WS_ASSERT_DEBUG(iAllocedRects == 0, EWsPanicInvalidRegion);}
	};
#else
typedef RRegion RWsRegion;
#endif

GLREF_D RWsRegion nullRegion;

#endif
