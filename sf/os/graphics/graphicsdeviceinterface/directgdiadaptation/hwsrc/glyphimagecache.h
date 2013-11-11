// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef GLYPHIMAGECACHE_H_
#define GLYPHIMAGECACHE_H_

/**
@file
@internalComponent
*/

#include <e32base.h>
#include <gdi.h>
#include <s32btree.h>
#include <VG/openvg.h>
#include <graphics/directgdiextensioninterfaces.h>


/**
Data structure which holds OpenVG images for monochrome and anti-aliased fonts
*/
class TGlyphEntry
	{
public:
	TChar iGlyphCode; 		//unique code, used as an index in a tree
	VGImage	iForeground; 	//blend using a brush colour
	};

/**
Data structure which holds OpenVG images for compound fonts.
These fonts are specified by four components:
	-foreground
	-background  
	-shadow
	-outline
*/
class TGlyphEntryCompound : public TGlyphEntry
	{
public:
	VGImage iOutline; //blend using a pen colour
	VGImage iShadow; //blend using a shadow colour
	};

/**
This class represents the concrete implementation of the CFontGlyphImageStorage.
It holds all entries in the form of a binary tree and provides functionality for image creation and searching.
*/
NONSHARABLE_CLASS(CFontGlyphTree) : public CBase
	{
public:
	static CFontGlyphTree* NewL(TUint32 aFontId, TGlyphBitmapType aGlyphType);
	~CFontGlyphTree();
template <class K>
	void GlyphImageEntryL(TChar aGlyphCode, const TUint8* aGlyphImage, const TSize& aGlyphImageSize, K& aEntry, TUint8* aDataForeground);
	inline TUint32 FontId() const;
	inline TUint CacheSize() const;
	void DestroyAllVGImagesL();
	//do not add to the tree. For OOM conditions
template <class K>
	static void GlyphImageEntryOOML(TGlyphBitmapType aGlyphType, const TUint8* aGlyphImage, const TSize& aGlyphImageSize, K& aEntry, TUint8* aData, TUint8* aDataOutline, TUint8* aDataShadow);
private:
	CFontGlyphTree(TUint32 aFontId, TGlyphBitmapType aGlyphType);
	void ConstructL();
	static void DestroyVGImage(VGImage* aForeground, VGImage* aOutline = NULL, VGImage* aShadow = NULL);
	//for monochrome and antialiased fonts
	static void CreateVGImageL(const TUint8* aGlyphImage, const TSize& aGlyphImageSize, TDisplayMode aDisplayMode, VGImage& aForeground, TUint8* aPreAllocForeground);
	//for four colours fonts
	static void CreateVGImageL(const TUint8* aGlyphImage, const TSize& aGlyphImageSize, VGImage& aForeground, VGImage& aOutline, VGImage& aShadow, TUint8* aPreAllocForeground, TUint8* aPreAllocOutline, TUint8* aPreAllocShadow);
	static void DecodeBinaryData(const TSize& aDataSize, const TUint8* aData, TUint32* aBinaryData);
	static void DecodeBinaryDataExLarge(const TSize& aDataSize, const TUint8* aData, TInt aStride, TUint32* aBinaryData);

	inline static TInt16 Load16(const TUint8* aPtr);
	static void CopyCharLine(TUint32*& aBinaryDataPtr,TInt aBufferWords,const TUint8* aData,TInt aBitShift,TInt aCharWidth, TInt16 aRepeatCount);
private:
	/**
	Binary tree with fixed size. One of following will be used:
	TBtreeFix<TGlyphEntry, TChar> or TBtreeFix<TGlyphEntryCompound, TChar>
	*/
	TBtree* iGlyphTree;

	CMemPagePool* iPagePool; /**< Non persistent pool for pages.*/
	TBtreeKey iKey; /**< Tree key.*/
	TUint32 iFontId; /**< Unique font id.*/
	TGlyphBitmapType iGlyphType; /**< Glyph bitmap type.*/
	TUint iCacheSize; /**< Number of bytes allocated by all elements in the tree.*/
	};

/**
@return The unique id of the font
*/
inline TUint32 CFontGlyphTree::FontId() const
	{
	return iFontId;
	}

/**
@return The total size of the cache
*/
inline TUint CFontGlyphTree::CacheSize() const
	{
	return iCacheSize;
	}

/**
@param aPtr A pointer to a source buffer of 2 8-bit unsigned integers.
@return The two 8-bit integers combined into a 16-bit integer.
*/
inline TInt16 CFontGlyphTree::Load16(const TUint8* aPtr) 
	{
	return TInt16(aPtr[0]+(aPtr[1]<<8)); 
	}

/**
The class provides an interface for handling image caching and will be maintained by the DirectGDI driver at adaptation level.
Any instance of this class cannot be shared between threads.
*/
NONSHARABLE_CLASS(CFontGlyphImageStorage) : public CBase, 
								public MFontGlyphImageStorage
{
public:
	CFontGlyphImageStorage(TInt aMaxCacheSize);
	TInt PreAllocateImages();
	~CFontGlyphImageStorage();

	// from MFontGlyphImageStorage
	TInt GlyphImage(TUint32 aFontId, TChar aGlyphCode, TGlyphBitmapType aGlyphBitmapType, 
						const TUint8* aGlyphImage, const TSize& aGlyphImageSize, 
						TAny* aImageForeground, TAny* aImageShadow, TAny* aImageOutline);	/**< @internalTechnology*/
	void CleanGlyphImageCache();	/**< @internalTechnology*/
	TInt GlyphCacheSize() const;	/**< @internalTechnology*/
	TInt MaxGlyphCacheSize() const;	/**< @internalTechnology*/
	TInt FontIdInOrder(RArray<TUint32> & aFontListId) const;	/**< @internalTechnology*/
	void EnforceOOMFailure(TBool aEnforce);	/**< @internalTechnology*/
	TInt SetMaxGlyphCacheSize(TInt aMaxCacheSize);	/**< @internalTechnology*/
private:
	void DeletePreAllocatedImages();
	void CleanCacheIfRequired();
private:
	RPointerArray<CFontGlyphTree> iFontTreeList;	/**< Array of all image trees in the cache.*/
	TInt iMaxCacheSize;	/**< Maximum cache size the class can hold.*/
	TInt iCacheSize;	/**< Current total cache size which amounts to cache sizes of all trees in the list .*/
	TGlyphEntry iEntry;	/**< Work member, which will be filled when user requests glyph image.*/
	TGlyphEntryCompound iEntryCompound;	/**< Work member, which will be filled when user requests glyph image.*/
	//for OOM conditions
	TUint8* iForegroundData;
	TUint8* iShadowData;
	TUint8* iOutlineData;
	VGImage iImageForeground;	/**< Pre-allocated image for OOM conditions.*/
	VGImage iImageShadow;	/**< Pre-allocated image for OOM conditions.*/
	VGImage iImageOutline;	/**< Pre-allocated image for OOM conditions.*/
	TSize iImageSize;	/**< Pre-defined image size.*/
	TBool iImagesPreAllocated;	/**< True if all OOM variables were created successfully.*/
#ifdef _DEBUG
	TBool iEnforceOOM;	/**< If true will always try to use pre-allocated images.*/
#endif
};


#endif
