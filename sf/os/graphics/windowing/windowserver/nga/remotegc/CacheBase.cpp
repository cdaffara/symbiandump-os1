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

#include "CacheBase.h"

/**
Constructor
Initialized the iCachedItems array with granularity 8 and an offset to iHandle in TCacheEntry,
which will be used to find specific entries later.
*/
CCacheBase::CCacheBase() : iCachedItems(8, _FOFF(TCacheEntry, iHandle))
	{
	
	}
	
CCacheBase::~CCacheBase()
	{
	for(TInt i = 0; i < iCachedItems.Count(); i++)
		{
		delete iCachedItems[i].iCachedItem;
		}
		
	iCachedItems.Reset();	
	}

/**
Begins an update of the cache.
*/
void CCacheBase::BeginUpdate()
	{
	// Sets all cached items to not used
	for(TInt i = 0; i < iCachedItems.Count(); i++)
		{
		iCachedItems[i].iIsUsed = EFalse;
		}		
	}

/**
End an update of the cache.
*/
void CCacheBase::EndUpdate()
	{
	// Removes all cached items that are not used
	for(TInt i = 0; i < iCachedItems.Count(); i++)
		{
		if(!iCachedItems[i].iIsUsed)
			{
			delete iCachedItems[i].iCachedItem;
			iCachedItems.Remove(i--);
			}			
		}
	// coverity[extend_simple_error]	
	}
	
/**
Returns an item from the cache corresponding to a specific handle.

@param aHandle A handle to match against an item in the cache.
@return The item that matches the handle provided as a parameter.
*/
const CBase* CCacheBase::Resolve(TInt aHandle)
	{
	TCacheEntry entry(aHandle);
	TInt index = iCachedItems.FindInUnsignedKeyOrder(entry);
	
	if(index != KErrNotFound)
		return iCachedItems[index].iCachedItem;		
	else
		return NULL;
	}


