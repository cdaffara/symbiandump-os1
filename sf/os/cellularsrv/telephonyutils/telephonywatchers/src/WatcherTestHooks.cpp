// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "WatcherTestHooks.h"

#ifdef WATCHER_TESTING
#include <e32math.h>

//
// ------> WHelpers (source)
//
TInt WHelpers::Rand(const TInt aLow, const TInt aHigh, TInt64& aSeed)
	{
	TReal initialRand = (Math::FRand(aSeed) * (aHigh - aLow));
	TInt32 rand;

	// Round to 0 decimal places, ie. the nearest whole numer
	Math::Round(initialRand, initialRand, 0);
	Math::Int(rand, initialRand);

	//aSeed = seed;
	return (aLow + rand);
	}

#endif
