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

#if !defined(__WATCHERTESTHOOKS_H__) && defined(WATCHER_TESTING)
#define __WATCHERTESTHOOKS_H__

// System includes
#include <e32property.h>

// Classes referenced
class CWatcherBase;

/**
Globals
@internalComponent
*/
static TInt64 TheSeed;


//
// ------> WHelpers (header)
//
class WHelpers
/**
@internalComponent
*/
	{
//
public:
//
	static TInt Rand(const TInt aLow, const TInt aHigh, TInt64& aSeed);
	};

#endif
