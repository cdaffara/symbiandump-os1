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
// FontCache.h
// 
//

#ifndef __FONTSCACHE_H__
#define __FONTSCACHE_H__

#include <fbs.h>
#include <e32cmn.h>

#include "CacheBase.h"

/**
A font cache that inherits from CCacheBase
*/
NONSHARABLE_CLASS(CFontsCache) : public CCacheBase
	{
public:
	CFontsCache();
	~CFontsCache();

	TInt UseL(TInt aHandle);
	const CFont* Resolve(TInt aHandle);
	};

#endif //__FONTSCACHE_H__

