// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "glyphatlas.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glyphatlasTraces.h"
#endif


extern void Panic(TFbsPanic aPanic);

static TInt16 Load16(const TUint8* aPtr);
static void DecodeBinaryData(const TSize& aGlyphSize, const TUint8* aEncodedData, TUint8* aByteData);
static void DecodeBinaryDataExLarge(const TSize& aGlyphSize, const TUint8* aEncodedData, TUint8* aByteData);
static void Convert1BppTo8Bpp(TUint32 aSrcData, TUint8*& aDestDataPtr, const TUint8* aDestDataPtrLimit);
static void CopyCharLine(TUint8*& aByteDataPtr, TInt aWidthInBytes, const TUint8* aSrcData, TInt aBitShift, TInt16 aRepeatCount);

// === CGlyphAtlas Functions ===

/**
Glyph Atlas constructor.
@param aMaxCacheSizeInBytes The maximum amount of specialised graphics memory 
	that the glyph atlas should use. If this value is KGlyphAtlasNoCacheLimit,
	then there is no limit and the atlas will use as much memory as is available
	in the system.
*/
CGlyphAtlas::CGlyphAtlas(TInt aMaxCacheSizeInBytes)
	:iLruPageList(_FOFF(CGlyphAtlasPage, iLink)),
	 iFontEntryArray(32, _FOFF(TFontEntryMap, iFont)),
	 iMaxCacheSizeInBytes(aMaxCacheSizeInBytes),
	 iMaxCacheSizeHigh(aMaxCacheSizeInBytes),
	 iGpuCacheSizeLimitIsMax(ETrue)
	{
    iMaxCacheSizeLow = ( KGlyphAtlasNoCacheLimit == aMaxCacheSizeInBytes )
                     ? KGlyphAtlasLowMemCacheLimitDefault
                     : ( aMaxCacheSizeInBytes / KGlyphAtlasLowMemCacheLimitDivisor );
	}

/**
Glyph Atlas destructor.
Frees all the RSgImage handles, frees all the allocated system memory, and
closes the Graphics Resource driver. 
*/
CGlyphAtlas::~CGlyphAtlas()
	{
	// cycle through all the font entries and destroy them
	for (TInt ii = iFontEntryArray.Count()-1; ii >= 0; --ii)
		{
		DeleteFontEntry(iFontEntryArray[ii].iEntry);
		}
	iFontEntryArray.Close();
	__ASSERT_DEBUG(iLruPageList.IsEmpty(), Panic(EFbsPanicGlyphAtlasInconsistentState));
	
	// there shouldn't be any remaining pages, but if there are, destroy them.
	while (!iLruPageList.IsEmpty())
		{
		delete iLruPageList.First();
		}
	iSgDriver.Close();
	}

/**
Factory constructor method. Creates a new glyph atlas.

@param aMaxCacheSizeInBytes The size in bytes, to use as the upper limit
	for the size of memory used by the glyph images in the atlas. If this
	value is KGlyphAtlasNoCacheLimit, then there is no limit and the atlas
	will use as much memory as is available in the system.

@return A pointer to the newly-constructed atlas

@leave KErrNoMemory if there was insufficient memory to create the atlas, 
	or a system wide error code if its RSgDriver failed to open.
*/
CGlyphAtlas* CGlyphAtlas::NewL(TInt aMaxCacheSizeInBytes)
	{
	CGlyphAtlas* self = new (ELeave) CGlyphAtlas(aMaxCacheSizeInBytes);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
	}

/**
Two-phase constructor.
@leave A system wide error code if RSgDriver failed to open.
*/
void CGlyphAtlas::ConstructL()
	{
	User::LeaveIfError(iSgDriver.Open());
	}

/**
Retrieves a glyph from the atlas.
If the glyph is found, the glyph data passed in is populated.

@param[in] aFont The font the glyph belongs to.
@param[in] aGlyphCode The glyph code for the glyph being requested.
@param[out] aGlyphImageInfo The glyph image information if this function is successful.
@return KErrNone if the glyph is found, KErrNotFound if not. 
*/
TInt CGlyphAtlas::GetGlyph(const CBitmapFont& aFont, TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo)
	{
    OstTraceExt2( TRACE_NORMAL, CGLYPHATLAS_GETGLYPH, "> f=%x; gc=%04x",(TUint)&aFont, aGlyphCode);
    
	CGlyphAtlasFontEntry* fontEntry = FindFontEntry(aFont);
	if (!fontEntry)
		{
        OstTrace0( TRACE_NORMAL, CGLYPHATLAS_GETGLYPH_END2, "< KErrNotFound");
		return KErrNotFound;
		}
	TInt err = fontEntry->GetGlyph(aGlyphCode, aGlyphImageInfo);
	
	OstTraceExt5( TRACE_NORMAL, CGLYPHATLAS_GETGLYPH_END1, "< id=%08x%08x; w=%u; h=%u; err=%d", 
	        (TUint)I64HIGH(aGlyphImageInfo.iImageId.iId), (TUint)I64LOW(aGlyphImageInfo.iImageId.iId),
	        (TUint)aGlyphImageInfo.iMetrics.Width(), (TUint)aGlyphImageInfo.iMetrics.Height(), (TInt)err);
	
	return err;
	}

/**
Adds a glyph to the atlas from a bitmap glyph and retrieves the glyph data.
If there is insufficient memory to create a RSgImage, then the least recently 
used pages (and all the glyphs contained within) are removed until there is 
enough memory to continue.

@param[in] aFont The font the glyph belongs to.
@param[in] aArgs The information needed to create a glyph.
@param[out] aGlyphImageInfo Upon return contains all the glyph image information needed to use the 
		glyph in another process.
@return KErrNone if the glyph was successfully added or other system-wide error.
*/
TInt CGlyphAtlas::AddGlyph(const CBitmapFont& aFont, const TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo)
	{
	OstTraceDefExt5( OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_ADDGLYPH, "> f=%x; bp=%08x; gc=%04x; w=%u; h=%u", 
	        (TUint)&aFont, (TUint)aArgs.iBitmapPointer, aArgs.iGlyphCode, 
	        aArgs.iMetrics->Width(), aArgs.iMetrics->Height());
	
    // Find font entry and create if none found
	CGlyphAtlasFontEntry* fontEntry = FindFontEntry(aFont);
	TBool isNewFont = EFalse;
	if (!fontEntry)
		{
		// Create a new font.
		fontEntry = CreateFontEntry(aFont);
		if (!fontEntry)
			{
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_ADDGLYPH_END2, "< KErrNoMemory");
			return KErrNoMemory;
			}
		isNewFont = ETrue;
		}
	TInt glyphSizeInBytes = 0;
	TInt err = fontEntry->AddGlyph(aArgs, aGlyphImageInfo, glyphSizeInBytes);
	if (KErrNone != err)
		{
		if (isNewFont)
			{
			DeleteFontEntry(fontEntry);
			}
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_ADDGLYPH_END3, "< err=%d", err);
		return err;
		}
	iCacheSizeInBytes += glyphSizeInBytes;

	// If there is a cache limit and it is now exceeded, remove the least
	// recently used pages until the cache size is within the upper limit. Do 
	// not remove the page relating to the glyph which is being added, which is 
	// now at the head of the LRU array.
	if (iMaxCacheSizeInBytes != KGlyphAtlasNoCacheLimit)
		{
		TBool morePagesToDelete = ETrue;
		while ((iCacheSizeInBytes > iMaxCacheSizeInBytes) && morePagesToDelete)
			{
			morePagesToDelete = DeleteLeastRecentlyUsedPage(EFalse);
			}
		}
	
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_ADDGLYPH_END1, "< id=%08x%08x", 
	            I64HIGH(aGlyphImageInfo.iImageId.iId), I64LOW(aGlyphImageInfo.iImageId.iId));
    
	return KErrNone;
	}

/**
Releases all glyphs associated with a particular font when it has been
released by the font system.

@param aFont The font which is released.
 */
void CGlyphAtlas::FontReleased(const CBitmapFont& aFont)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_FONTRELEASED, "> f=%x", (TUint)&aFont);
	
    TInt index = iFontEntryArray.FindInUnsignedKeyOrder(TFontEntryMap(&aFont, NULL));
	if (KErrNotFound == index)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_FONTRELEASED_END2, "< KErrNotFound");
		return;
		}
	CGlyphAtlasFontEntry* fontEntry = iFontEntryArray[index].iEntry;
	iCacheSizeInBytes -= fontEntry->SizeInBytes();
	delete fontEntry;
	iFontEntryArray.Remove(index);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_FONTRELEASED_END3, "< KErrNone");
	}

/**
Searches the array of font entries to find the entry for the given font.
If the font entry is found, the entry is returned. If not a NULL pointer is 
returned.

@param aFont The font to match an entry with.
@return The font entry if a match is found, NULL if not. 
*/
CGlyphAtlasFontEntry* CGlyphAtlas::FindFontEntry(const CBitmapFont& aFont) const
	{
	TFontEntryMap entryToMatch(&aFont, NULL);
	TInt index = iFontEntryArray.FindInUnsignedKeyOrder(entryToMatch);
	if (KErrNotFound == index)
		{
		return NULL;
		}
	return iFontEntryArray[index].iEntry;
	}

/**
Deletes the given font entry.
The mapping from the font to the font entry is removed.

@param aFontEntry The entry to delete.
*/
void CGlyphAtlas::DeleteFontEntry(CGlyphAtlasFontEntry* aFontEntry)
	{
    __ASSERT_DEBUG(aFontEntry, Panic(EFbsPanicGlyphAtlasInconsistentState));
    TInt index = iFontEntryArray.FindInUnsignedKeyOrder(TFontEntryMap(&aFontEntry->Font(), NULL));
    __ASSERT_DEBUG(KErrNotFound != index, Panic(EFbsPanicGlyphAtlasInconsistentState));
    if (KErrNotFound != index)
        {
        iFontEntryArray.Remove(index);
        }
    iCacheSizeInBytes -= aFontEntry->SizeInBytes();
    delete aFontEntry;
	}

/**
Moves the given page to the front (the position of the most recently used page) 
of the usage order list.

@param aPage The most recently used page.
*/
void CGlyphAtlas::MovePageToFront(CGlyphAtlasPage& aPage)
	{
	aPage.MoveToFirstInQueue(iLruPageList);
	}

/**
Creates a font entry from the given font and adds a mapping from the font 
to the font entry.
If successful, the font entry is returned.
If either the creation of the font entry or the adding the mapping fails, then 
a NULL pointer is returned.

@param aFont The font used to create a font entry from.
@return A new font entry if successful, NULL if not.
*/
CGlyphAtlasFontEntry* CGlyphAtlas::CreateFontEntry(const CBitmapFont& aFont)
	{
	CGlyphAtlasFontEntry* fontEntry = new CGlyphAtlasFontEntry(aFont, *this);
	if (!fontEntry)
		{
		return NULL;
		}
	// Add font entry to font entry array
	TFontEntryMap fontEntryMap(&aFont, fontEntry);
	TInt err = iFontEntryArray.InsertInUnsignedKeyOrder(fontEntryMap);
	__ASSERT_DEBUG(KErrAlreadyExists != err, Panic(EFbsPanicGlyphAtlasInconsistentState));
	if (KErrNone != err)
		{
		delete fontEntry;
		fontEntry = NULL;
		}
	return fontEntry;
	}

/**
Deletes the least recently used page and removes it from the list of pages
held by the atlas.

@param aAllowMruPageDeletion ETrue if the most recently used page can be deleted, 
	EFalse otherwise.
@return ETrue, if there are pages remaining in the atlas after the deletion, EFalse
	otherwise. If there is only one page in the atlas and aAllowMruPageDeletion is EFalse,
	EFalse is returned. 
*/
TBool CGlyphAtlas::DeleteLeastRecentlyUsedPage(TBool aAllowMruPageDeletion)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_DELETELEASTRECENTLYUSEDPAGE, "> mru=%d", aAllowMruPageDeletion );
    
	CGlyphAtlasPage* lruPage = NULL;
	if (!iLruPageList.IsEmpty())
		{
		lruPage = iLruPageList.Last();
		if (!aAllowMruPageDeletion && (lruPage == iLruPageList.First()))
			{
			lruPage = NULL;
			}
		}
	TBool canDeleteMorePages = EFalse;
	if (lruPage)
		{
		iCacheSizeInBytes -= lruPage->SizeInBytes();
		CGlyphAtlasFontEntry& fontEntry = lruPage->FontEntry();
		fontEntry.DeletePage(lruPage);
		lruPage = NULL;
		if (fontEntry.IsEmpty())
			{
			DeleteFontEntry(&fontEntry);
			}
		canDeleteMorePages = !iLruPageList.IsEmpty();
		}
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_DELETELEASTRECENTLYUSEDPAGE_END, "< more=%u; size=%u", (TUint)canDeleteMorePages, iCacheSizeInBytes);
	return canDeleteMorePages;
	}

/**
Utility function that calculates the number of unique fonts associated with the atlas.
@return Number of fonts in the atlas.
 */
TInt CGlyphAtlas::FontCount() const
	{
	return iFontEntryArray.Count();
	}

/**
Utility function that calculates the number of glyphs across all fonts stored in
the atlas.
@return Number of glyphs in the atlas.
 */
TInt CGlyphAtlas::GlyphCount() const
	{
	TInt glyphCount = 0;
	for (TInt ii = iFontEntryArray.Count() - 1; ii >= 0; --ii)
		{
		glyphCount += iFontEntryArray[ii].iEntry->GlyphCount();
		}
	return glyphCount;
	}

/**
Utility function that calculates the number of glyphs for a given font in the atlas.
@param The font to return the number of glyphs for.
@return Number of glyphs in the atlas.
 */
TInt CGlyphAtlas::GlyphCount(const CBitmapFont& aFont) const
	{
	CGlyphAtlasFontEntry* fontEntry = FindFontEntry(aFont);
	return (fontEntry) ? fontEntry->GlyphCount() : 0;
	}

void CGlyphAtlas::GetGlyphCacheMetrics( TGlyphCacheMetrics& aGlyphCacheMetrics )
    {
    aGlyphCacheMetrics.iMaxCacheSizeInBytes = iMaxCacheSizeInBytes;
    aGlyphCacheMetrics.iMaxCacheSizeHigh = iMaxCacheSizeHigh;
    aGlyphCacheMetrics.iMaxCacheSizeLow = iMaxCacheSizeLow;
    aGlyphCacheMetrics.iCacheSizeInBytes = iCacheSizeInBytes;
    aGlyphCacheMetrics.iGpuCacheSizeLimitIsMax = iGpuCacheSizeLimitIsMax;
    }

/**
 Function to release the GPU cache. Called in response to the GoomMonitor's
 requirement to reduce GPU memory use.

 @param aBytes. The amount of memory the GOoM framework would like us to relinquish.
 @param aFlags. The flags conveyed from the GOoM monitor framework.
*/
void CGlyphAtlas::ReleaseGpuMemory( TInt /*aBytes*/, TInt /*aFlags*/ )
    {
    OstTraceDefExt2( OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_RELEASEGPUMEMORY, "> max=%d; size=%d", iMaxCacheSizeInBytes, iCacheSizeInBytes);

    if ( iCacheSizeInBytes > 0 )
        {
        while( DeleteLeastRecentlyUsedPage(ETrue) )
            {
            // Do nothing
            }
        }

    // If appropriate, reduce the cache-size limit.
    if ( GpuCacheSizeLimitIsMax() )
        {
        SwitchGpuCacheSizeLimit();
        }

    OstTraceDefExt2( OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_RELEASEGPUMEMORY_EXIT, "< max=%d; size=%d", iMaxCacheSizeInBytes, iCacheSizeInBytes);
    }

/**
 Function to establish GPU memory use. Called in response to the GoomMonitor's
 notification that GPU memory may once more be utilised in the usual manner.

 @param aFlags. The flags conveyed from the GOoM monitor framework.
 */
void CGlyphAtlas::InstateGpuMemory( TInt /*aFlags*/ )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_INSTATEGPUMEMORY, "> max=%d", iMaxCacheSizeInBytes );

    // If appropriate, reinstate the full cache-size limit.
    if ( !GpuCacheSizeLimitIsMax() )
        {
        SwitchGpuCacheSizeLimit();
        }

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CGLYPHATLAS_INSTATEGPUMEMORY_EXIT, "< max=%d", iMaxCacheSizeInBytes );
    }

/**
 Utility function to toggle between full and reduced GPU cache-size limits.

 Use in conjunction with TBool GpuCacheSizeLimitIsMax()
 */
void CGlyphAtlas::SwitchGpuCacheSizeLimit()
    {
    if ( GpuCacheSizeLimitIsMax() )
        {
        // The short-circuit operator obviates tautological conditionals.
        while ( (iCacheSizeInBytes >= iMaxCacheSizeLow) && DeleteLeastRecentlyUsedPage(ETrue) )
            {
            // Do Nothing...
            }

        iMaxCacheSizeInBytes = iMaxCacheSizeLow;
        iGpuCacheSizeLimitIsMax = EFalse;
        }
    else
        {
        iMaxCacheSizeInBytes = iMaxCacheSizeHigh;
        iGpuCacheSizeLimitIsMax = ETrue;
        }
    }


/**
 Utility function to report whether the GPU cache-size limit is set to the maximum permissible
 level, or to its reduced level.

 @return ETrue if the cache-size is set to the maximum permissible limit.
         EFalse if it is set to the reduced limit.
 */
TBool CGlyphAtlas::GpuCacheSizeLimitIsMax() const
    {
    return iGpuCacheSizeLimitIsMax;
    }

// === CGlyphAtlasFontEntry Functions ===
/**
Font entry constructor.

@param aFont The font to which this font entry should be associated.
@param aGlyphAtlas The glyph atlas to which this font entry belongs.
*/
CGlyphAtlasFontEntry::CGlyphAtlasFontEntry(const CBitmapFont& aFont, CGlyphAtlas& aGlyphAtlas)
	:iFont(aFont),
	 iPageArray(32, _FOFF(TPageMap, iGlyphCode)),
	 iAtlas(aGlyphAtlas)
	{
	}

/**
Font entry destructor.
Destroys the pages owned by the font entry.
*/
CGlyphAtlasFontEntry::~CGlyphAtlasFontEntry()
	{
	// cycle through all the font entries and destroy them
	for (TInt ii = iPageArray.Count()-1; ii >= 0; --ii)
		{
		DeletePage(iPageArray[ii].iPage);
		}
	__ASSERT_DEBUG(iSizeInBytes == 0, Panic(EFbsPanicGlyphAtlasInconsistentState));
	iPageArray.Close();
	}

/**
Deletes the given page.
The page mapping is removed and the page is deleted.

@param aPage The page to delete.
*/
void CGlyphAtlasFontEntry::DeletePage(CGlyphAtlasPage* aPage)
	{
    __ASSERT_DEBUG(aPage, Panic(EFbsPanicGlyphAtlasInconsistentState));
    __ASSERT_DEBUG(iPageArray.Count() > 0, Panic(EFbsPanicGlyphAtlasInconsistentState));
		
    TInt numGlyphsInPage = aPage->GlyphCount();

    for (TInt ii = 0; ii < numGlyphsInPage; ++ii)
        {
        TInt index = iPageArray.FindInUnsignedKeyOrder(TPageMap(aPage->GlyphCodeAt(ii), NULL));

        __ASSERT_DEBUG(KErrNotFound != index, Panic(EFbsPanicGlyphAtlasInconsistentState));

        if (KErrNotFound != index)
            {
            iPageArray.Remove(index);
            iPageArray.GranularCompress();
            }
        }
    iSizeInBytes -= aPage->SizeInBytes();
    delete aPage;

	}

/**
Adds a glyph to the font from a bitmap glyph and gets the glyph image info back.

@param[in] aArgs The information needed to create a glyph.
@param[out] aGlyphImageInfo Upon return contains all the glyph image information needed to use the 
		glyph in another process.
@param[out] aSizeInBytes Upon return contains the size of the added glyph's image data in bytes.
@return KErrNone if the glyph was successfully added or other system-wide error.
*/
TInt CGlyphAtlasFontEntry::AddGlyph(const CGlyphAtlas::TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo, TInt& aSizeInBytes)
	{
    CGlyphAtlasPage* newPage = new CGlyphAtlasPage(*this);
	if (!newPage)
		{
		return KErrNoMemory;
		}
	
	TInt err = newPage->AddGlyph(aArgs, aGlyphImageInfo, aSizeInBytes);
	if (KErrNone != err)
		{
		delete newPage;
		return err;
		}

	err = iPageArray.InsertInUnsignedKeyOrder(TPageMap(aArgs.iGlyphCode, newPage));
	__ASSERT_DEBUG(KErrAlreadyExists != err, Panic(EFbsPanicGlyphAtlasInconsistentState));
	if (KErrNone != err)
		{
		delete newPage;
		return err;
		}
	iSizeInBytes += aSizeInBytes;
	iAtlas.MovePageToFront(*newPage);
	return err;
	}

/**
Searches the array of pages to find the page containing the given glyph.
If the page is found, the glyph image info is populated and the page is moved 
to the front of the Glyph Atlas' LRU page array.

@param[in] aGlyphCode The glyph code for the glyph being requested.
@param[out] aGlyphImageInfo Upon return contains all the glyph image information needed to use the 
		glyph in another process.
@return KErrNone if the glyph was found, KErrNotFound if not. 
*/
TInt CGlyphAtlasFontEntry::GetGlyph(TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo)
	{
	TInt index = iPageArray.FindInUnsignedKeyOrder(TPageMap(aGlyphCode, NULL));
	if (KErrNotFound == index)
		{
		return KErrNotFound;
		}
	CGlyphAtlasPage* page = iPageArray[index].iPage;
	page->GetGlyph(aGlyphCode, aGlyphImageInfo);
	iAtlas.MovePageToFront(*page);
	return KErrNone;
	}

/**
Gets the font associated with the font entry.

@return The font associated with this entry. 
*/
const CBitmapFont& CGlyphAtlasFontEntry::Font() const
	{
	return iFont;
	}

/**
Gets the amount of memory allocated for all the image data for this font.

@return The size of the font's image data in bytes.
*/
TInt CGlyphAtlasFontEntry::SizeInBytes() const
	{
	return iSizeInBytes;
	}

/**
Tests whether the font entry has any pages.

@return ETrue if the font entry does not contain any pages, EFalse if it does..
*/
TBool CGlyphAtlasFontEntry::IsEmpty() const
	{
	if (iPageArray.Count() == 0)
		{
		__ASSERT_DEBUG(iSizeInBytes == 0, Panic(EFbsPanicGlyphAtlasInconsistentState));
		return ETrue;
		}
	return EFalse;
	}

/**
Gets the glyph atlas the font entry belongs to.

@return The font entry's glyph atlas.
*/
CGlyphAtlas& CGlyphAtlasFontEntry::GlyphAtlas() const
	{
	return iAtlas;
	}

/** 
@return The number of glyphs this font entry has.
 */
TInt CGlyphAtlasFontEntry::GlyphCount() const
	{
	TInt glyphCount = 0;
	for (TInt ii = iPageArray.Count() - 1; ii >= 0; --ii)
		{
		glyphCount += iPageArray[ii].iPage->GlyphCount();
		}
	return glyphCount;
	}


// === CGlyphAtlasPage Functions ===

/**
Page constructor.

@param aFontEntry The font entry to which the page is associated.
*/
CGlyphAtlasPage::CGlyphAtlasPage(CGlyphAtlasFontEntry& aFontEntry)
	:iFontEntry(aFontEntry)
	{
	}

/**
Page destructor.
Releases the RSgImage handles held by the page.
Removes the page from the Glyph Atlas' LRU page array. 
*/
CGlyphAtlasPage::~CGlyphAtlasPage()
	{
	iLink.Deque();
	iGlyphImage.Close();
	}

/**
Adds a glyph to the page from a bitmap glyph and gets the glyph image info back.
An RSgImage handle is acquired for the glyph. 
If there is not enough specialised graphics memory to create a RSgImage, then the
least recently used pages are deleted until there there is either enough memory 
for the creation to be successful or if there are no more pages to delete (in 
which case an appropriate out of memory error message is returned). 

@param[in] aArgs The information needed to create a glyph.
@param[out] aGlyphImageInfo Upon return contains all the glyph image information needed to use the 
		glyph in another process.
@param[out] aSizeInBytes Upon return contains the size of the added glyph's image data in bytes.
@return KErrNone if the glyph was successfully added; 
	KErrNoMemory if there is not enough system memory available;
	KErrNoGraphicsMemory if there is not enough specialised graphics memory available.
*/
TInt CGlyphAtlasPage::AddGlyph(const CGlyphAtlas::TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo, TInt& aSizeInBytes)
	{
	const TSize glyphSize(aArgs.iMetrics->Width(), aArgs.iMetrics->Height());
	// If glyph has zero size (e.g. space), set glyph data and return 
	if (glyphSize.iWidth == 0 || glyphSize.iHeight == 0)
		{
		iPosX = 0;
		iPosY = 0;
		iMetrics = *aArgs.iMetrics;
		iGlyphCode = aArgs.iGlyphCode;
		iSizeInBytes = 0;
		iNumGlyphs++;
		aGlyphImageInfo.iImageId = KSgNullDrawableId;
		aGlyphImageInfo.iPosX = iPosX;
		aGlyphImageInfo.iPosY = iPosY;
		aGlyphImageInfo.iMetrics = iMetrics;
		return KErrNone;
		}
	TUint8* buf = NULL;
	TSgImageInfo info;
	info.iSizeInPixels = glyphSize;
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatA_8;
	TInt dataStride = 0;
	const TInt KDataArraySize = 256;
	TUint8 byteDataArray[KDataArraySize];
	TUint8* tempBuf = NULL;
	TUint8* byteDataBuf = NULL;
	TGlyphBitmapType glyphBitmapType = iFontEntry.Font().GlyphBitmapType(); 

	switch (glyphBitmapType)
		{
		case EMonochromeGlyphBitmap:
			// Decompress to 8bpp buffer
			dataStride = glyphSize.iWidth;
			byteDataBuf = byteDataArray;
			// If data too big to fit in byteDataArray, allocate memory on the heap
			if (glyphSize.iHeight * glyphSize.iWidth > KDataArraySize)
				{
				tempBuf = (TUint8*) User::AllocZ(dataStride * glyphSize.iHeight);
				if (!tempBuf)
					{
					return KErrNoMemory;
					}
				byteDataBuf = tempBuf;
				}
			else
				{
				// fill array with zeros.
				Mem::FillZ(&byteDataArray, KDataArraySize);
				}
			
			if (glyphSize.iWidth >32)
				{
				DecodeBinaryDataExLarge(glyphSize, aArgs.iBitmapPointer, byteDataBuf);
				}
			else
				{
				DecodeBinaryData(glyphSize, aArgs.iBitmapPointer, byteDataBuf);
				}
			buf = byteDataBuf;
			break;
		case EAntiAliasedGlyphBitmap:
			buf = const_cast<TUint8*>(aArgs.iBitmapPointer);
			dataStride = glyphSize.iWidth;
			break;
		default:
			return KErrNotSupported;
		}

	TInt err = iGlyphImage.Create(info, buf, dataStride);

	// If RSgImage creation fails due to out of memory, delete the least
	// recently used pages to free up memory until either creation succeeds or 
	// there are no more pages to remove.
	TBool morePagesToDelete = ETrue;
	while ((KErrNoGraphicsMemory == err || KErrNoMemory == err) && morePagesToDelete)
		{
		// Delete least used page.  Can delete all pages if necessary as this  
		// page has not been added to the LRU array yet.
		morePagesToDelete = iFontEntry.GlyphAtlas().DeleteLeastRecentlyUsedPage(ETrue);
		err = iGlyphImage.Create(info, buf, dataStride);
		}

	User::Free(tempBuf);
	
	if (KErrNone != err)
		{
		return err;
		}
	aSizeInBytes = glyphSize.iHeight * glyphSize.iWidth;
	iGlyphCode = aArgs.iGlyphCode;
	iPosX = 0;
	iPosY = 0;
	iMetrics = *aArgs.iMetrics;
	// As the image is stored as one byte per pixel, the size in bytes is 
	// just the number of pixels.
	// TODO: Replace estimating size with call to SgImage/SgDriver to get accurate size.
	iSizeInBytes += aSizeInBytes;
	iNumGlyphs++;
	aGlyphImageInfo.iPosX = iPosX;
	aGlyphImageInfo.iPosY = iPosY;
	aGlyphImageInfo.iImageId = iGlyphImage.Id();
	aGlyphImageInfo.iMetrics = iMetrics;
	return err;
	}

/**
Retrieves the glyph image information for the given glyph code necessary to be 
able to use the glyph in another process.

@param aGlyphCode The glyph code for the glyph being requested
@param aGlyphImageInfo Upon return contains all the glyph image information needed to use the 
		glyph in another process if the glyph is contained in the page.
*/
void CGlyphAtlasPage::GetGlyph(TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo) const
	{
	__ASSERT_DEBUG(iGlyphCode == aGlyphCode, Panic(EFbsPanicGlyphAtlasInconsistentState));
	aGlyphImageInfo.iMetrics = iMetrics;
	aGlyphImageInfo.iPosX = iPosX;
	aGlyphImageInfo.iPosY = iPosY;
	aGlyphImageInfo.iImageId = iGlyphImage.Id();
	}

/**
Gets the amount of memory allocated for the image data for this page.

@return The size of the page's image data in bytes.
*/
TInt CGlyphAtlasPage::SizeInBytes() const
	{
	return iSizeInBytes;
	}

/**
Gets the glyph code at the given index associated with the page.

@param aIndex The index of the glyph code within the page.
@return The glyph code at the given index.
*/
TUint CGlyphAtlasPage::GlyphCodeAt(TInt aIndex) const
	{
	__ASSERT_DEBUG(0 == aIndex, Panic(EFbsPanicGlyphAtlasInconsistentState));
	return iGlyphCode;
	}

/**
Gets the number of glyphs stored in the page.

@return The number of glyphs in the page.
*/
TInt CGlyphAtlasPage::GlyphCount() const
	{
	return iNumGlyphs;
	}

/**
Gets the font entry which owns the page.

@return The font entry which owns the page. 
*/
CGlyphAtlasFontEntry& CGlyphAtlasPage::FontEntry() const
	{
	return iFontEntry;
	}


void CGlyphAtlasPage::MoveToFirstInQueue(TDblQue<CGlyphAtlasPage>& aList)
	{
	if(!aList.IsFirst(this))
		{
		iLink.Deque();
		aList.AddFirst(*this);
		}
	}

// === Static Utility Functions ===

/**
Combines 2 8-bit unsigned integers into a 16-bit integer.
@param aPtr A pointer to a source buffer of 2 8-bit unsigned integers.
@return The two 8-bit integers combined into a 16-bit integer.
*/
static TInt16 Load16(const TUint8* aPtr)
	{
	return TInt16(aPtr[0]+(aPtr[1]<<8)); 
	}

/**
Decodes binary data for monochrome glyph bitmap.

@param aGlyphSize size of glyph in pixels.
@param aEncodedData Pointer to an encoded source buffer.
@param aByteData Pointer to a destination buffer (8 bits per pixel).
*/
void DecodeBinaryData(const TSize& aGlyphSize, const TUint8* aEncodedData, TUint8* aByteData)
	{
	const TInt dataHeight = aGlyphSize.iHeight;
	const TInt dataWidth = aGlyphSize.iWidth;
	TUint32 binaryData = 0;
	
	// The data is encoded as follows:
	// 1 bit for a multiple lines flag (1=yes)
	// 4 bits for a repeat count which represents:
	// -if the multiple line flag is 0 the number of lines whose data is repeated
	// -if the flag is 1, the number of lines which differ from line to line.
	// n bits representing the data at 1 bit per pixel, where:
	// -if the multiple line flag is 0, n is the width of the glyph.
	// -if the flag is 1, n is width of glyph multiplied by the repeat count for this block of data.
	// This information presented in continuous packed blocks of:
	// [data][reps][multiLineFlag]
	TInt bitIndex = 0;
	TInt16 repeatCount = 0;
	TUint8* byteDataPtr = aByteData;
	TUint8* byteDataPtrLimit = NULL;
	for (TInt charLine = 0; charLine < dataHeight; charLine += repeatCount) // for lines in the character...
		{
		// Get first 5 bits of block
		repeatCount = Load16(aEncodedData + (bitIndex >> 3));
		repeatCount >>= bitIndex & 7;
		// strip out multiple line flag (1st bit)
		TInt multiLineFlag = repeatCount & 1;
		// Get repeat count (last 4 bits)
		repeatCount >>= 1;
		repeatCount &= 0xf;
		// move bit index to point to first bit of image data
		bitIndex += 5;
		// end pointer of destination buffer for this block of data to fill 
		byteDataPtrLimit = aByteData + dataWidth * (charLine + repeatCount);
		if (multiLineFlag)
			{
			while (byteDataPtr < byteDataPtrLimit)
				{
				// Pointer to beginning of data in source buffer for current scanline
				TInt charDataOffsetPtr = TInt(aEncodedData) + (bitIndex >> 3);
				// Pointer to beginning of current word.
				TUint32* charDataWord = (TUint32*)(charDataOffsetPtr &~ 3);
				// Number of bits to shift in current word to get to beginning of scanline
				TInt bitShift = bitIndex & 7;
				bitShift += (charDataOffsetPtr & 3) << 3;
				// Copy scanline data into temporary buffer
				binaryData = (*charDataWord++) >> bitShift;
				// If data crosses a word boundary, get the rest of the data from next word.
				if (bitShift)
					{
					binaryData |= (*charDataWord << (32-bitShift));
					}
				Convert1BppTo8Bpp(binaryData, byteDataPtr, byteDataPtr + dataWidth);
				// Move bit index to beginning of next block
				bitIndex += dataWidth;
				}
			}
		else
			{
			TInt charDataOffsetPtr = TInt(aEncodedData) + (bitIndex >> 3);
			TUint32* charDataWord = (TUint32*)(charDataOffsetPtr &~ 3);
			TInt bitShift = bitIndex & 7;
			bitShift += (charDataOffsetPtr & 3) << 3;
			binaryData = (*charDataWord++) >> bitShift;
			if (bitShift)
				{
				binaryData |= (*charDataWord << (32-bitShift));
				}
			TUint8* startByteDataPtr = byteDataPtr;
			Convert1BppTo8Bpp(binaryData, byteDataPtr, byteDataPtr + dataWidth);
			
			while (byteDataPtr < byteDataPtrLimit)
				{
				Mem::Copy(byteDataPtr, startByteDataPtr, dataWidth);
				byteDataPtr += dataWidth;
				}
			bitIndex += dataWidth;
			}
		}
	}

/**
Converts binary data in 1 bit per pixel format to 8 bits per pixel format, where
0 is converted to 0x00 and 1 is converted to 0xFF.

@param aSrcData Pointer to a 1bpp source buffer.
@param aDestDataPtr Pointer to a 8bpp destination buffer.
@param aDestDataPtrLimit Pointer to the end position in destination buffer to convert to.
*/
void Convert1BppTo8Bpp(TUint32 aSrcData, TUint8*& aDestDataPtr, const TUint8* aDestDataPtrLimit)
	{
	for (; aDestDataPtr < aDestDataPtrLimit; ++aDestDataPtr, aSrcData >>= 1)
		{
		if (aSrcData&1)
			{
			*aDestDataPtr = 0xFF;
			}
		}
	}

/**
Decodes binary data for extra large monochrome glyph bitmap.

@param aGlyphSize Size of glyph in pixels.
@param aEncodedData Pointer to an encoded source buffer.
@param aByteData Pointer to a destination buffer (8 bits per pixel).
*/
void DecodeBinaryDataExLarge(const TSize& aGlyphSize, const TUint8* aEncodedData, TUint8* aByteData)
	{
	const TInt dataWidth = aGlyphSize.iWidth;
	const TInt dataHeight = aGlyphSize.iHeight;
	TInt bitIndex = 0;
	TInt16 repeatCount = 0;

	for (TInt charLine = 0; charLine < dataHeight; charLine += repeatCount) // for lines in the character...
		{
		repeatCount = Load16(aEncodedData + (bitIndex >> 3));
		repeatCount >>= bitIndex & 7;
		const TInt multiLineFlag = repeatCount & 1;
		repeatCount >>= 1;
		repeatCount &= 0xf;
		bitIndex += 5;
		if (multiLineFlag)
			{
			for (TInt currentline = 0; currentline < repeatCount; currentline++)
				{
				CopyCharLine(aByteData, dataWidth, aEncodedData + (bitIndex >> 3), bitIndex & 7, 1);
				bitIndex += dataWidth;
				}
			}
		else
			{
			CopyCharLine(aByteData, dataWidth, aEncodedData + (bitIndex >> 3), bitIndex & 7, repeatCount);
			bitIndex += dataWidth;
			}
		}
	}

/**
Copies glyph image data line(s)(1 bit per pixel) to an 8 bit per pixel
destination buffer.

@param aByteDataPtr Pointer to a destination buffer (8bpp).
@param aWidthInBytes Stride of the image.
@param aSrcData Pointer to a source buffer (1bpp).
@param aBitShift Number of bits the source data pointer will be shifted. 
@param aRepeatCount Number of lines to copy.
*/
void CopyCharLine(TUint8*& aByteDataPtr, TInt aWidthInBytes, const TUint8* aSrcData, TInt aBitShift, TInt16 aRepeatCount)
	{
	aBitShift &= 7;
	TUint8* ptrLimit = aByteDataPtr + aWidthInBytes;
	TUint32* dataWord = (TUint32*)(TInt(aSrcData) &~ 3);
	aBitShift += (TInt(aSrcData) - TInt(dataWord)) << 3;

	TUint8* startByteDataPtr = aByteDataPtr;
	TUint32 binaryData = 0;
	while (aByteDataPtr < ptrLimit)
		{
		binaryData = *dataWord++;
		binaryData >>= aBitShift;
		if (aBitShift)
			{
			binaryData |= (*dataWord << (32-aBitShift));
			}
		TUint8* wordLimit = aByteDataPtr + 32;
		if (wordLimit > ptrLimit)
			{
			wordLimit = ptrLimit;
			}
		Convert1BppTo8Bpp(binaryData, aByteDataPtr, wordLimit);
		}

	while (aRepeatCount > 1)
		{
		Mem::Copy(aByteDataPtr, startByteDataPtr, aWidthInBytes);
		aByteDataPtr += aWidthInBytes;
		--aRepeatCount;
		}
	}


