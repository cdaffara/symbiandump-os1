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
// Font & Bitmap Server Glyph Atlas
//

#ifndef GLYPHATLAS_H
#define GLYPHATLAS_H

#include <e32base.h>
#include <sgresource/sgimage.h>
#include <fbs.h>
#include "UTILS.H"

class CGlyphAtlasFontEntry;
class CGlyphAtlasPage;
class TGlyphCacheMetrics;

/**
The Glyph Atlas manages the storing and retrieval of glyphs stored in GPU 
memory as RSgImages.

CGlyphAtlas is the main class which processes requests to add and retrieve
glyphs. Only one instance of this class will exist throughout the lifetime of 
the Font and Bitmap Server.
@internalComponent
*/
NONSHARABLE_CLASS(CGlyphAtlas) : public CBase
	{
friend class CGlyphAtlasTestWrapper;
public:
	/**
	Structure used to pass information required to create a new glyph so that it can 
	be added to the glyph atlas.
	@internalComponent 
	*/
	class TAddGlyphArgs
		{
	public:
		inline TAddGlyphArgs(const TUint8* aBitmapPointer, TUint aGlyphCode, const TOpenFontCharMetrics& aMetrics)
			: iBitmapPointer(aBitmapPointer), iGlyphCode(aGlyphCode), iMetrics(&aMetrics) {}
	public:
		const TUint8* iBitmapPointer;	/**< The address of the bitmap glyph. No ownership.*/
		TUint iGlyphCode;	/**< The glyph code for the glyph being added. No ownership.*/
		const TOpenFontCharMetrics* iMetrics;	/**< The metrics for the glyph being added. No ownership.*/
		};

public:
	~CGlyphAtlas();
	static CGlyphAtlas* NewL(TInt aMaxCacheSizeInBytes);
	TInt GetGlyph(const CBitmapFont& aFont, TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo);
	TInt AddGlyph(const CBitmapFont& aFont, const TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo);
	void FontReleased(const CBitmapFont& aFont);
	void MovePageToFront(CGlyphAtlasPage& aPage);
	TBool DeleteLeastRecentlyUsedPage(TBool aAllowMruPageDeletion);
	TInt FontCount() const;
	TInt GlyphCount() const;
	TInt GlyphCount(const CBitmapFont& aFont) const;
	void GlyphCacheMetrics(TInt& aSizeInBytes, TInt& aMaxSizeInBytes, TBool& aGpuCacheSizeLimitIsMax);
	void ReleaseGpuMemory( TInt /*aBytes*/, TInt /*aFlags*/ );
	void InstateGpuMemory( TInt /*aFlags*/ );
	void GetGlyphCacheMetrics( TGlyphCacheMetrics& aGlyphCacheMetrics );

private:
	CGlyphAtlas(TInt aMaxCacheSizeInBytes);
	void ConstructL();
	CGlyphAtlasFontEntry* CreateFontEntry(const CBitmapFont& aFont);
	CGlyphAtlasFontEntry* FindFontEntry(const CBitmapFont& aFont) const;
	void DeleteFontEntry(CGlyphAtlasFontEntry* aFontEntry);
    void SwitchGpuCacheSizeLimit();
    TBool GpuCacheSizeLimitIsMax() const;

private:
	class TFontEntryMap
		{
	public:
		inline TFontEntryMap(const CBitmapFont* aFont, CGlyphAtlasFontEntry* aEntry)
				: iFont(aFont),iEntry(aEntry) {}
	public:			
		const CBitmapFont* iFont;
		CGlyphAtlasFontEntry* iEntry;
		};
private:
	RSgDriver iSgDriver;
	TDblQue<CGlyphAtlasPage> iLruPageList; /**< Least-recently used ordered queue (most-recently used at head).*/
	RArray<TFontEntryMap> iFontEntryArray; /**< Array of font entries maintained in unsigned ordered by font pointer.*/
	TUint iMaxCacheSizeInBytes;	/**< Maximum specialised graphics memory the cache should use. If zero, there is no limit.*/
	TInt iMaxCacheSizeHigh;                /**< The high cach-size threshold.*/
	TInt iMaxCacheSizeLow;                 /**< The low cach-size threshold.*/
	TUint iCacheSizeInBytes; 	/**< Actual amount of specialised graphics memory used by the entire atlas.*/
	TBool iGpuCacheSizeLimitIsMax;         /**< Is the cache-size limit set to it's maximum value? */
	};


/**
Each font entry manages the storing and retrieval of glyphs belonging to a font.
@internalComponent
*/
NONSHARABLE_CLASS(CGlyphAtlasFontEntry): public CBase
	{
public:
	CGlyphAtlasFontEntry(const CBitmapFont& aFont, CGlyphAtlas& aGlyphAtlas);
	~CGlyphAtlasFontEntry();
	const CBitmapFont& Font() const;
	void DeletePage(CGlyphAtlasPage* aPage);
	TInt GetGlyph(TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo);
	TInt AddGlyph(const CGlyphAtlas::TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo, TInt& aSizeInBytes);
	TInt SizeInBytes() const;
	TBool IsEmpty() const;
	CGlyphAtlas& GlyphAtlas() const;
	TInt GlyphCount() const;

private:
	// Class that handles association between glyph code and page the 
	// image that glyph code resides on. Used in pointer arrays.
	class TPageMap
		{
	public:
		inline TPageMap(TUint aGlyphCode, CGlyphAtlasPage* aPage)
				: iGlyphCode(aGlyphCode),iPage(aPage) {}
	public:			
		TUint iGlyphCode;
		CGlyphAtlasPage* iPage;
		};
private:
	const CBitmapFont& iFont;	/**< The font which the stored glyph belongs to.*/
	RArray<TPageMap> iPageArray;	/**< Array of pages maintained in unsigned ordered by glyph code.*/
	CGlyphAtlas& iAtlas;		/**< The glyph atlas the font entry belongs to.*/
	TInt iSizeInBytes;			/**< The amount of specialised graphics memory used by all owned pages.*/
	};


/**
Each page in the glyph atlas stores one glyph in one RSgImage.
@internalComponent
*/
NONSHARABLE_CLASS(CGlyphAtlasPage): public CBase
	{
friend CGlyphAtlas::CGlyphAtlas(TInt);
public:
	CGlyphAtlasPage(CGlyphAtlasFontEntry& aFontEntry);
	~CGlyphAtlasPage();
	TInt AddGlyph(const CGlyphAtlas::TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo, TInt& aSizeInBytes);
	void GetGlyph(TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo) const;
	TUint GlyphCodeAt(TInt aIndex) const;
	TInt GlyphCount() const;
	CGlyphAtlasFontEntry& FontEntry() const;
	TInt SizeInBytes() const;
	void MoveToFirstInQueue(TDblQue<CGlyphAtlasPage>& aList);

private:
	TDblQueLink iLink;
	// Data for the stored glyph
	CGlyphAtlasFontEntry& iFontEntry;	/**< The font entry asssociated with the page.*/
	RSgImage iGlyphImage;		/**< The image which contains the stored glyph.*/
	TInt16 iPosX;				/**< The x-coord of the position of the stored glyph within the image.*/
	TInt16 iPosY;				/**< The y-coord of the position of the stored glyph within the image.*/
	TOpenFontCharMetrics iMetrics;	/**< The character metrics for the stored glyph.*/
	TUint iGlyphCode;			/**< The glyph code for the stored glyph.*/
	TInt iSizeInBytes;			/**< The amount of specialised graphics memory used by this page.*/
	TInt iNumGlyphs;			/**< The number of glyphs contained in the page.*/
	};


// If no limit to the size of the glyph atlas cache is desired, this
// constant must be passed to CGlyphCache::NewL()
// In this case the cache lower maximum limit, deployed in response to GooM,
// will be set to KGlyphAtlasLowMemCacheLimitDefault
const TInt KGlyphAtlasNoCacheLimit = 0;
const TInt KGlyphAtlasLowMemCacheLimitDivisor = 2;
const TInt KGlyphAtlasLowMemCacheLimitDefault = 0x40000;


#endif // GLYPHATLAS_H

