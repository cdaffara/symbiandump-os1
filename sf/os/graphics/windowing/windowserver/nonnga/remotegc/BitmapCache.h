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

#ifndef __BITMAPCACHE_H__
#define __BITMAPCACHE_H__

#include <fbs.h>
#include <e32cmn.h>

#include "CacheBase.h"

/**
A bitmap cache that inherits from CCacheBase
*/
NONSHARABLE_CLASS(CBitmapCache): public CCacheBase
	{
public:
	CBitmapCache();
	~CBitmapCache();

	TInt UseL(TInt aHandle);
	const CFbsBitmap* Resolve(TInt aHandle);
	};

#endif //__BITMAPCACHE_H__

