// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>
#include "GDIPANIC.h"

_LIT(KCTypefaceStore, "CTypefaceStore");
static const TInt KDefaultNumOfFontAccess = 4;

//
// CTypefaceStore
//

EXPORT_C CTypefaceStore::CTypefaceStore()
	{
	}


EXPORT_C CTypefaceStore::~CTypefaceStore()
/** Destroys the typeface store and reclaims the memory allocated to it. */
	{
	if (iFontAccess)
		{
		const TInt count = iFontAccess->Count();
		for (TInt i = 0; i < count; i++)
			{
			GDI_ASSERT_DEBUG_GENERAL((*iFontAccess)[0].iAccessCount > 0, User::Panic(KCTypefaceStore, KErrCorrupt));
			GDI_ASSERT_DEBUG_GENERAL((*iFontAccess)[0].iAccessCount == 1, User::Panic(KCTypefaceStore, KErrInUse));
			delete (*iFontAccess)[0].iFont;
			iFontAccess->Delete(0);
			}
		delete iFontAccess;
		}
	}

EXPORT_C void CTypefaceStore::ConstructL()
/** Second phase constructor. */
	{
	iFontAccess = new(ELeave) CArrayFixFlat<TFontAccess>(KDefaultNumOfFontAccess);
	}

EXPORT_C void CTypefaceStore::AddFontL(CFont* aFont)
/** Adds a hold, by a client of the typeface store, on the specified font.

If the specified font is not currently accessed by any clients of the typeface store
then the font is added to the font list and the access count set to 1.
If it is currently accessed then the access count for the font is incremented by 1.

@param aFont Pointer to the device specific font accessed. */
	{
	GDI_ASSERT_DEBUG_GENERAL(NULL != iFontAccess, User::Panic(KCTypefaceStore, KErrNoMemory));
	if (IncrementFontCount(aFont))
		return;
	TFontAccess fontAccess;
	fontAccess.iAccessCount = 1;
	fontAccess.iFont = aFont;
	iFontAccess->AppendL(fontAccess);
	}

EXPORT_C void CTypefaceStore::ReleaseFont(CFont* aFont)
/** Releases the hold of a typeface store client on a specified font.

Decrements the access-count for the specified font by one. If this reduces the access-count 
to zero then the font is no longer needed by any client, and is deleted from 
the typeface store list.

@param aFont The font to be released by the client. */
	{
	
	GDI_ASSERT_DEBUG_GENERAL(NULL != iFontAccess, User::Panic(KCTypefaceStore, KErrNoMemory));
	if (!aFont)
		return;
	TInt idx = 0;
	GDI_ASSERT_ALWAYS_GENERAL(FindFont(aFont, idx), User::Panic(KCTypefaceStore, KErrNotFound));
	GDI_ASSERT_DEBUG_GENERAL(0 < iFontAccess->At(idx).iAccessCount, User::Panic(KCTypefaceStore, KErrCorrupt));
	iFontAccess->At(idx).iAccessCount--;
	if (0 == iFontAccess->At(idx).iAccessCount)
		{
		CFont *font=iFontAccess->At(idx).iFont;
		iFontAccess->Delete(idx);
		delete font;
		}
	}

EXPORT_C TBool CTypefaceStore::IncrementFontCount(const CFont* aFont)
/**
Search for the font in iFontAccess and increment its count.
@param aFont Font to search for.
@return ETrue if the specified font is found, EFalse otherwise.
*/
	{
	TInt idx = 0;
	if (FindFont(aFont, idx))
		{
		iFontAccess->At(idx).iAccessCount++;
		return ETrue;
		}
	return EFalse;
	}

TBool CTypefaceStore::FindFont(const CFont* aFont, TInt& aIdx) const
/**
Search for the font in iFontAccess.
@param aFont Font to search for.
@param aIdx Index of the font found.
@return ETrue if the specified font is found, EFalse with aIdx = -1 otherwise.
*/
	{
	aIdx = -1;
	if (!iFontAccess)
		return EFalse;
	const TInt count = iFontAccess->Count();
	for (TInt i = 0; i < count; i++)
		{
		if ((*iFontAccess)[i].iFont == aFont)
			{
			aIdx = i;
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_C TInt CTypefaceStore::BaselineOffset(TInt aHeight,TFontPrintPosition aPos)
/** Gets the baseline offset, in twips, for any font in subscript or superscript 
form.

If the print position is normal then zero is returned. If it is superscript 
then the baseline offset returned is a percentage, KSuperscriptOffsetPercentage, 
of aHeight. If it is subscript then the baseline offset returned is a percentage, 
KSubscriptOffsetPercentage, of aHeight.

@param aHeight The height of a font. 
@param aPos The print position of the required font: superscript, subscript 
or normal. 
@return The baseline offset for the font. */
	{
	TInt offset=0;
	if (aPos==EPrintPosSuperscript)
		offset=KSuperscriptOffsetPercentage*aHeight/100;
	else if (aPos==EPrintPosSubscript)
		offset=KSubscriptOffsetPercentage*aHeight/100;
	return offset;
	}
	 
 
EXPORT_C TInt CTypefaceStore::SuperSubHeight(TInt aHeight,TFontPrintPosition aPos)
/** Gets the height for a font in subscript or superscript form. 

If the print position is normal then aHeight is returned unchanged. If it 
is superscript or subscript then the height returned is a percentage, KSuperSubScalingPercentage, 
of aHeight.

@param aHeight The height of a font. 
@param aPos The print position of the font: superscript, subscript or normal.
@return The required height of the font. */
	{
	if(aPos!=EPrintPosNormal)
		aHeight=(KSuperSubScalingPercentage*aHeight)/100;
	return aHeight;
	}

//
// CFontCache
//
 
EXPORT_C CFontCache::CFontCache():
	CBase(),
	iMaxEntries(KMaxFontCacheEntries)
/** Default constructor.

This constructs a CFontCache of size KMaxFontCacheEntries. */
	{
	__DECLARE_NAME(_S("CFontCache"));
	}
 
EXPORT_C CFontCache::CFontCache(TInt aMaxEntries):
	CBase(),
	iMaxEntries(aMaxEntries)
/** Constructor specifying the number of cache entries.

@param aMaxEntries Number of entries for this cache. You must ensure this 
is less than or equal to KMaxFontCacheEntries. */
	{
	__DECLARE_NAME(_S("CFontCache"));
	}

 
EXPORT_C CFontCache::~CFontCache()
/** Destructor. 

This destroys the cashe and releases its allocated memory. */
	{
	CFontCacheEntry* entry=iFirst;
	while(entry)
		{
		iFirst=entry->iNext;
		delete entry;
		entry=iFirst;
		}
	}

 
EXPORT_C CFont* CFontCache::Search(const TFontSpec& aFontSpec)
/** Searches the cache for a specified font. 

The CFont returned is that which corresponds to the font specification aFontSpec.

@param aFontSpec The specification of the font to be searched for. 
@return If an entry for the font specification is found in the cache, the pointer 
to the font corresponding to the font specification is returned. Otherwise 
NULL is returned. */
	{
	CFontCacheEntry* entry=iFirst;
	CFontCacheEntry* previous=NULL;
	while(entry)
		{
		if(entry->iSpec==aFontSpec)
			{
			iNumHits++;
			if(previous)
				{
				previous->iNext=entry->iNext;
				entry->iNext=iFirst;
				iFirst=entry;
				}
			return(entry->iFont);
			}
		previous=entry;
		entry=entry->iNext;
		}
	iNumMisses++;
	return(NULL);
	}

 
EXPORT_C CFont* CFontCache::AddEntryL(CFont* aFont,const TFontSpec& aFontSpec)
/** Adds a font entry to the cache. 

The font, and the font specification required to extract it from the cache, 
are both specified. If the cache is already full, the font replaces the oldest 
entry already in the cache, which is returned.

@param aFont The font to be stored in the cache. 
@param aFontSpec The font's corresponding font specification. 
@return If the cache isn't full, NULL is returned. If the cache is full, the 
displaced cache entry is returned. */
	{
	CFontCacheEntry* entry=new(ELeave) CFontCacheEntry(aFont,aFontSpec,iFirst);
	iFirst=entry;
	iNumEntries++;
	if(iNumEntries<=iMaxEntries)
		return(NULL);
	CFontCacheEntry* previous=NULL;
	while(entry->iNext)
		{
		previous=entry;
		entry=entry->iNext;
		}
	CFont* discardfont=entry->iFont;
	delete entry;
	iNumEntries--;
	if(previous)
		previous->iNext=NULL;
	else
		iFirst=NULL;
	return(discardfont);
	}

 
EXPORT_C CFont* CFontCache::RemoveFirstEntry()
/** Removes the first entry from the font cache and returns it.

@return The entry removed from the font cache. If the cache is empty, NULL 
is returned. */
	{
	if(iFirst==NULL) return(NULL);
	CFontCacheEntry* entry=iFirst;
	iFirst=entry->iNext;
	CFont* font=entry->iFont;
	delete entry;
	iNumEntries--;
	return(font);
	}

// CFontCacheEntry

CFontCache::CFontCacheEntry::CFontCacheEntry(CFont* aFont,const TFontSpec& aFontSpec,CFontCacheEntry* aNext):
	CBase(),
	iFont(aFont),
	iSpec(aFontSpec),
	iNext(aNext)
	{
	__DECLARE_NAME(_S("CFontCacheEntry"));
	}
