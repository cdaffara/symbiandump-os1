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

#include "BitmapCache.h"

CBitmapCache::CBitmapCache() : CCacheBase()
	{
	
	}

CBitmapCache::~CBitmapCache()
	{
	
	}

/**
Enables the use of a cached bitmap with a specified handle.
If no bitmap with this handle is cached, the bitmap is created and cached.

@return KErrNone if no problem occured, otherwise a systemwide error.
*/
TInt CBitmapCache::UseL(TInt aHandle)
	{
	TCacheEntry entry(aHandle);
	TInt index = iCachedItems.FindInUnsignedKeyOrder(entry);
	if(index != KErrNotFound)
		{
		iCachedItems[index].iIsUsed = ETrue;
		return KErrNone;	
		}
		
	entry.iCachedItem = new (ELeave) CFbsBitmap;	
	TInt err = static_cast<CFbsBitmap*>(entry.iCachedItem)->Duplicate(aHandle);
	if(err)
		{
		delete entry.iCachedItem;
		// coverity[extend_simple_error]
		return err;
		}
	
	entry.iIsUsed = ETrue;		
	CleanupStack::PushL(entry.iCachedItem);
	iCachedItems.InsertInUnsignedKeyOrderL(entry);
	CleanupStack::Pop(entry.iCachedItem);	
	return KErrNone;
	}

/**
Returns a bitmap from the cache corresponding to a specific handle.

@param aHandle A handle to match against a bitmap in the cache.
@return The bitmap that matches the handle provided as a parameter.
*/
const CFbsBitmap* CBitmapCache::Resolve(TInt aHandle)
	{	
	const CBase* item = CCacheBase::Resolve(aHandle);
	if(item != NULL)
		return static_cast<const CFbsBitmap*>(item);
	else
		return NULL;
	}
	

