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
 @publishedPartner
 @prototype
*/

#ifndef FBSRASTERIZERCLEARCACHE_H
#define FBSRASTERIZERCLEARCACHE_H

#include <e32def.h>

const TUint32 KUidFbsRasterizerClearCache = 0x10286595;

/** Extension interface returned by CFbsRasterizer::GetInterface() to allow 
clearing of the cache of recently rasterized extended bitmaps.
 */
class MFbsRasterizerClearCache
	{
public:
	/** Release all the memory used by the cache of recently rasterized extended bitmaps.
	 */
	virtual void ClearCache() = 0;
	};

#endif // FBSRASTERIZERCLEARCACHE_H
