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

#ifndef DIRECTGDIEXTENSIONINTERFACES_H
#define DIRECTGDIEXTENSIONINTERFACES_H

#include <e32cmn.h>
#include <gdi.h>

/**
An interface used for testing the VGImage cache in the VG DirectGDI adaptation.

@internalTechnology
*/
class MVgImageCache
	{
public:
	virtual TBool IsInCache(TInt64 aSerialNumberList) = 0;
	virtual TInt TouchCount(TInt64 aSerialNumberList) = 0;
	virtual TInt NumEntries() const = 0;
	virtual void GetOrderedCacheEntries(TInt64& aSerialNumberList, TInt aListSize) = 0;
	virtual TInt CacheSizeInBytes() const = 0;
	virtual TInt MaxCacheSize() const = 0;
	virtual void ResetCache() = 0;
	};

const TUint32 KDirectGdiVgImageCacheUid = 0x10285A74;

/**
An interface for retrieving images from the storage and obtaining various glyph cache parameters . 
Many functions are intended to be used by test code only to exersize glyph image cache implementation. 

@internalTechnology
*/
class MFontGlyphImageStorage
	{
public:
	virtual TInt GlyphImage(TUint32 aFontId, TChar aGlyphCode, TGlyphBitmapType aGlyphBitmapType, 
						const TUint8* aGlyphImage, const TSize& aGlyphImageSize, 
						TAny* aImageForeground, TAny* aImageShadow, TAny* aImageOutline) = 0;
	//for test purpose
	//current glyph cache size in bytes
	virtual TInt GlyphCacheSize() const = 0;
	//max glyph cache size in bytes
	virtual TInt MaxGlyphCacheSize() const = 0;
	//mainly for OOM testing
	//delete all VG Images from the cache
	virtual void CleanGlyphImageCache() = 0;
	//Fills aFontListId parameter with font Id in order from most to least usable. 
	virtual TInt FontIdInOrder(RArray<TUint32> & aFontListId) const = 0;
	//Enforce to emulate OOM condition. The system will use pre-allocated buffers.
	virtual void EnforceOOMFailure(TBool aEnforce) = 0;
	};

const TUint32 KDirectGdiGetGlyphStorageUid = 0x10285A75;

/**
Interface intended for implementation by CDirectGdiDriverInternal to allow clients 
control over the maximum sizes used by an image cache and a glyph cache.
 
@internalTechnology
 */
class MDirectGdiDriverCacheSize
	{
public:
	/** Sets the maximum size of the image cache used by the driver 
	implementation.
	@param aSize The maximum size in bytes that the image cache should use.
	@return KErrNone if the maximum cache size was set successfully, 
	KErrNotSupported if this method is not supported.
	 */
	virtual TInt SetMaxImageCacheSize(TInt aSize) = 0;
	/** Gets the maximum size of the driver's image cache.
	@return The current maximum size in bytes of the driver's image cache.
	 */
	virtual TInt MaxImageCacheSize() const = 0;
	/** Sets the maximum size of the glyph cache used by the driver
	implementation.
	@param aSize The maximum size in bytes that the glyph cache should use.
	@return KErrNone if the maximum cache size was set successfully, 
	KErrNotSupported if this method is not supported.
	 */
	virtual TInt SetMaxGlyphCacheSize(TInt aSize) = 0;
	/** Gets the maximum size of the driver's glyph cache.
	@return The current maximum size in bytes of the driver's glyph cache.
	 */
	virtual TInt MaxGlyphCacheSize() const = 0;
	};

const TUint32 KDirectGdiDriverCacheSizeUid = 0x10286A32;

#endif /*DIRECTGDIEXTENSIONINTERFACES_H*/
