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

#ifndef VGIMAGECACHE_H_
#define VGIMAGECACHE_H_

/**
@file
@internalComponent Reference implementation of Open VG hardware accelerated DirectGDI adaptation.
*/
#include <fbs.h>
#include <VG/openvg.h>
#include <e32hashtab.h>
#include <graphics/directgdiextensioninterfaces.h>


/**
A class for managing a cache of VGImages.
Each VGImage has a CFbsBitmap associated with it.
The CFbsBitmap's serial number is used to retrieve VGImages from the cache
as these are unique to the bitmap for the lifetime of the system.
*/
NONSHARABLE_CLASS(CVgImageCache): public CBase, public MVgImageCache
	{
public:
	CVgImageCache(TInt aMaxCacheSize);
	~CVgImageCache();
	TBool AddImage(const CFbsBitmap& aBitmap, VGImage& aImage, const TPoint& aOrigin);
	VGImage GetVgImageFromBitmap(const CFbsBitmap& aBitmap, const TPoint& aOrigin);
	
	//MVgImageCache commands
	TBool IsInCache(TInt64 aSerialNumber);	/**< @internalTechnology*/
	TInt TouchCount(TInt64 aSerialNumber);	/**< @internalTechnology*/
	TInt NumEntries() const;	/**< @internalTechnology*/
	void GetOrderedCacheEntries(TInt64& aSerialNumberList, TInt aListSize);	/**< @internalTechnology*/
	TInt CacheSizeInBytes() const;	/**< @internalTechnology*/
	TInt MaxCacheSize() const;	/**< @internalTechnology*/
	void ResetCache();	/**< @internalTechnology*/
	TInt SetMaxCacheSize(TInt aMaxCacheSize); /**< @internalTechnology*/

private:
	NONSHARABLE_CLASS(CVgImageCacheItem): public CBase
		{
		public:
		CVgImageCacheItem();
		~CVgImageCacheItem();
		
		TDblQueLink iLink;
		VGImage iImage;			/**< VGImage created from CFbsBitmap.*/
		TInt64 iSerialNumber;			/**< Bitmap ID used to select VGImage.*/
		TInt iImageSizeInBytes;
		TInt iTouchCount;		/**< Touch count of CFbsBitmap when VGImage was stored.*/
		TPoint iOrigin;			/**< Origin used to tile image when created.*/
		};
	void DeleteItem(CVgImageCacheItem* aItem);
	static TUint32 Hash(const TInt64& aKey);

private:
	RHashMap<TInt64, CVgImageCacheItem*> iCacheItemMap;	/**< Maps bitmap's unique identifier to a cache item.*/
	TDblQue<CVgImageCacheItem> iVgImageCache;	/**< Least-recently used ordered queue (most-recently used at head).*/
	TInt iCacheSizeInBytes;
	TInt iMaxCacheSizeInBytes;
	TInt iNumMatchMisses;	/**< Number of unsuccessful matches when retrieving VGImage from cache.*/
	TInt iNumMatchTries; /**< Number of attempts to retrieve VGImage from cache.*/
	};

#endif /*VGIMAGECACHE_H_*/
