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

#include "FontsCache.h"

NONSHARABLE_CLASS(CCachedFont) : public CFbsFont
	{
public:
	CCachedFont();
	~CCachedFont();
	
	TInt Duplicate(TInt aHandle); //lint !e1511
	};
	
CCachedFont::CCachedFont()
	{		
	}
	
CCachedFont::~CCachedFont()
	{		
	}
	
TInt CCachedFont::Duplicate(TInt aHandle)
	{
	return CFbsFont::Duplicate(aHandle);
	}

CFontsCache::CFontsCache() : CCacheBase()
	{	
	}

CFontsCache::~CFontsCache()
	{	
	}

/**
Enables the use of a cached font with a specified handle.
If no font with this handle is cached, the font is created and cached.

@return KErrNone if no problem occured, otherwise a systemwide error.
*/	
TInt CFontsCache::UseL(TInt aHandle)
	{
	TCacheEntry entry(aHandle);
	TInt index = iCachedItems.FindInUnsignedKeyOrder(entry);
	if(index != KErrNotFound)
		{
		iCachedItems[index].iIsUsed = ETrue;
		return KErrNone;	
		}
		
	entry.iCachedItem = new (ELeave) CCachedFont;	
	TInt err = static_cast<CCachedFont*>(entry.iCachedItem)->Duplicate(aHandle);
	if(err)
		{
		delete entry.iCachedItem;
		return err;
		}
	
	entry.iIsUsed = ETrue;		
	CleanupStack::PushL(entry.iCachedItem);
	iCachedItems.InsertInUnsignedKeyOrderL(entry);
	CleanupStack::Pop(entry.iCachedItem);
	return KErrNone;
	}

/**
Returns a font from the cache corresponding to a specific handle.

@param aHandle A handle to match against a font in the cache.
@return The font that matches the handle provided as a parameter.
*/	
const CFont* CFontsCache::Resolve(TInt aHandle)
	{	
	const CBase* item = CCacheBase::Resolve(aHandle);
	if(item != NULL)
		return static_cast<const CCachedFont*>(item);
	else
		return NULL;
	}

