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

/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "glyphatlastestwrapper.h"

/**
Panic function required for glyphatlas.cpp to compile in the test project.
*/
GLDEF_C void Panic(TFbsPanic aPanic)
	{
	_LIT(KFBSERVClientPanicCategory,"FBSCLI");
	User::Panic(KFBSERVClientPanicCategory,aPanic);
	}


/**
Glyph Atlas test wrapper constructor.
*/
CGlyphAtlasTestWrapper::CGlyphAtlasTestWrapper()
	{
	}

/**
Two-phase constructor.
@leave A system wide error code if RSgDriver failed to open.
*/
void CGlyphAtlasTestWrapper::ConstructL(TInt aMaxCacheSizeInBytes)
	{
	iAtlas = CGlyphAtlas::NewL(aMaxCacheSizeInBytes);
	}

/**
Factory constructor method. Creates a new glyph atlas test wrapper.

@param aMaxCacheSizeInBytes The size in bytes, to use as the upper limit
	for the size of memory used by the glyph images in the atlas. If this
	value is KGlyphAtlasNoCacheLimit, then there is no limit and the atlas
	will use as much memory as is available in the system.

@return A pointer to the newly-constructed atlas test wrapper

@leave KErrNoMemory if there was insufficient memory to create the atlas, 
	or a system wide error code if its RSgDriver failed to open.
*/
CGlyphAtlasTestWrapper* CGlyphAtlasTestWrapper::NewL(TInt aMaxCacheSizeInBytes)
    {
    CGlyphAtlasTestWrapper* self = CGlyphAtlasTestWrapper::NewLC( aMaxCacheSizeInBytes );
    CleanupStack::Pop(); // self;

    return self;
    }

CGlyphAtlasTestWrapper* CGlyphAtlasTestWrapper::NewLC(TInt aMaxCacheSizeInBytes)
    {
    CGlyphAtlasTestWrapper* self = new (ELeave) CGlyphAtlasTestWrapper();
    CleanupStack::PushL(self);
    self->ConstructL(aMaxCacheSizeInBytes);

    return self;
    }

TInt CGlyphAtlasTestWrapper::GetGlyph(const CBitmapFont& aFont, TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo)
	{
	return iAtlas->GetGlyph(aFont, aGlyphCode, aGlyphImageInfo);
	}

TInt CGlyphAtlasTestWrapper::AddGlyph(const CBitmapFont& aFont, const CGlyphAtlas::TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo)
	{
	return iAtlas->AddGlyph(aFont, aArgs, aGlyphImageInfo);
	}

void CGlyphAtlasTestWrapper::FontReleased(const CBitmapFont& aFont)
	{
	iAtlas->FontReleased(aFont);
	}

/**
Returns the current size of the glyph image memory used by the atlas in bytes.
*/
TInt CGlyphAtlasTestWrapper::SizeInBytes() const
	{
	return iAtlas->iCacheSizeInBytes;
	}

/**
Tests whether the least recently used page contains the given glyph.
@param aGlyphCode The glyph code to match.
@return ETrue if the lru page contains the given glyph, EFalse if not.
*/
TBool CGlyphAtlasTestWrapper::LruPageContainsGlyph(TUint aGlyphCode) const
	{
	CGlyphAtlasPage* page = iAtlas->iLruPageList.Last();
	TInt numGlyphsInPage = page->GlyphCount();
	TBool glyphFound = EFalse;
	for (TInt ii = 0; ii < numGlyphsInPage && !glyphFound; ++ii)
		{
		glyphFound = (aGlyphCode == page->GlyphCodeAt(ii));
		}
	return glyphFound;
	}

/**
Returns the number of glyphs associated to the given font in the atlas.
*/
TInt CGlyphAtlasTestWrapper::GlyphCountByFont(const CBitmapFont* aFont)
	{
	TInt glyphCount = 0;
	TDblQueIter<CGlyphAtlasPage> iter(iAtlas->iLruPageList);
	CGlyphAtlasPage* page;
	while((page=iter++) != NULL)
		{
		if (&page->FontEntry().Font() == aFont)
			{
			glyphCount += page->GlyphCount();
			}
		}
	return glyphCount;
	}

/**
Returns the number of glyphs in the atlas.
*/
TInt CGlyphAtlasTestWrapper::GlyphCount()
	{
	TInt glyphCount = 0;
	TDblQueIter<CGlyphAtlasPage> iter(iAtlas->iLruPageList);
	CGlyphAtlasPage* page;
	while((page=iter++) != NULL)
		{
		glyphCount += page->GlyphCount();
		}
	return glyphCount;
	}

/**
Returns the number of fonts in the atlas.
*/
TInt CGlyphAtlasTestWrapper::FontCount() const
	{
	return iAtlas->iFontEntryArray.Count();
	}

/**
Glyph Atlas test wrapper destructor.
*/
CGlyphAtlasTestWrapper::~CGlyphAtlasTestWrapper()
	{
	delete iAtlas;
	}

