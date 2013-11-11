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

#include "vgimagecache.h"
#include "directgdiadapter.h"

/**
Given a TDisplayMode, returns the closest TDisplayMode that is pixel-for-pixel-compatible
with an OpenVG format, such that the given TDisplayMode may be converted into the result
without loss of colour information.

@param 	aDisplayMode Pixel format to find a match for.

@return Closest TDisplayMode for which there is a OpenVG-compatible match.
*/
static TDisplayMode ClosestVgCompatibleDisplayMode (TDisplayMode aDisplayMode)
	{
	switch (aDisplayMode)
		{
		case EGray2:
		case EGray4:
		case EGray16:
			return EGray256;

		case EColor16:
		case EColor256:
		case EColor4K:
			return EColor64K;
		
		case EColor16M:
			return EColor16MU;
			
		default:
			return aDisplayMode;
		}
	}


/**
Image Cache Item constructor.
*/
CVgImageCache::CVgImageCacheItem::CVgImageCacheItem()
	{
	
	}

/**
Image Cache Item destructor.
Destroys the VGImage owned by the cache item.
*/
CVgImageCache::CVgImageCacheItem::~CVgImageCacheItem()
	{
	vgDestroyImage(iImage);
	}



/**
Forms a 32-bit hash value from a 64-bit integer.

@param aKey The 64-bit key to be hashed.
@return 32-bit hash value.
*/
TUint32 CVgImageCache::Hash(const TInt64& aKey)
	{
	TPckgC<TInt64> bytes(aKey);
	return DefaultHash::Des8(bytes);
	}

/**
Image Cache constructor.
Initialises the hashmap and double-linked queue
*/
CVgImageCache::CVgImageCache(TInt aMaxCacheSize)
	:iCacheItemMap(THashFunction32<TInt64>(CVgImageCache::Hash), TIdentityRelation<TInt64>()),
	iVgImageCache(_FOFF(CVgImageCacheItem, iLink)),
	iMaxCacheSizeInBytes(aMaxCacheSize)
	{
	}

/**
Image Cache destructor.
Iterates through all the items in the cache and deletes them.
 */
CVgImageCache::~CVgImageCache()
	{
	// cycle through all the items and destroy the VGImages.
	TDblQueIter<CVgImageCacheItem> iter(iVgImageCache);
	CVgImageCacheItem* item;
	while((item=iter++)!=NULL)
		{
		DeleteItem(item);
		}
	iCacheItemMap.Close();
	}

/**
Deletes a cache item.
This removes the supplied item from the queue and the hashmap,
destroys the VGImage in that item, and deletes the item itself.
@param aItem The image cache item to be deleted.
 */
void CVgImageCache::DeleteItem(CVgImageCacheItem* aItem)
	{
	aItem->iLink.Deque();
	iCacheSizeInBytes -= aItem->iImageSizeInBytes;
	iCacheItemMap.Remove(aItem->iSerialNumber);
	delete aItem;
	}

/**
Adds a VGImage to the cache using the associated CFbsBitmap as a reference.

A new image cache item is created which stores the VGImage, the size of the image in pixels,
the serial number of the CFbsBitmap (which acts as a unique identifier) and the touch count
of the CFbsBitmap.  The touch count determines the number of times the underlying data of
the CFbsBitmap has changed.

The least-recently used items will be removed from the cache to create space for the new item, if needed.

@param aBitmap	The bitmap from which the VGImage was created.
@param aImage	The VGImage to store in the cache.
@param aOrigin	The origin used to create a tiled VGImage.

@return ETrue if the VGImage was successfully added to the cache, EFalse if not.
 */
TBool CVgImageCache::AddImage(const CFbsBitmap& aBitmap, VGImage& aImage, const TPoint& aOrigin)
	{
	// Calculate approximate size in bytes of image
	TDisplayMode vgCompatibleDisplayMode = ClosestVgCompatibleDisplayMode(aBitmap.DisplayMode());
	TSize imageSize = aBitmap.SizeInPixels();
	TInt dataStride = CFbsBitmap::ScanLineLength(imageSize.iWidth, vgCompatibleDisplayMode);
	TInt imageSizeInBytes = imageSize.iHeight * dataStride;
	// if size of image is too large to fit in cache 
	if(imageSizeInBytes > iMaxCacheSizeInBytes)
		{
		return EFalse;
		}
	
	CVgImageCacheItem* newItem = new CVgImageCacheItem;
	if (newItem == NULL)
		{
		return EFalse;
		}

	// check there is enough room in the cache
	// i.e. less than user-specified max memory allowed for cache
	// if not enough space, remove items from end of cache until enough space is available.
	while(iMaxCacheSizeInBytes < iCacheSizeInBytes + imageSizeInBytes)
		{
		DeleteItem(iVgImageCache.Last());
		}
	newItem->iSerialNumber = aBitmap.SerialNumber();
	newItem->iImage = aImage;
	newItem->iTouchCount = aBitmap.TouchCount();
	newItem->iOrigin = aOrigin;
	newItem->iImageSizeInBytes = imageSizeInBytes;
	TInt err = iCacheItemMap.Insert(newItem->iSerialNumber, newItem);
	if (err != KErrNone)
		{
		delete newItem;
		return EFalse;
		}
	iVgImageCache.AddFirst(*newItem);
	iCacheSizeInBytes += newItem->iImageSizeInBytes;
	return ETrue;
	}

/**
Retrieves the VGImage from the cache that was created from the supplied CFbsBitmap.
The cache is first searched to find the item containing the VGImage that was created from the CFbsBitmap.
If no matching item is found, no VGImage is returned.
If the matching item is found, the touch count is checked to determine whether the
CFbsBitmap has been updated since the stored VGImage was created from it.  If it has, the matching item 
and VGImage is deleted from the cache and no VGImage is returned.
If the matching item is found and the CFbsitmap has not been updated sionce the VGImage was created,
the associated VGImage is returned.

@param aBitmap	The bitmap used to reference the item containing the VGImage.
@param aOrigin  The origin of the VGImage, relative to the top left corner of the source image.

@return	VG_INVALID_HANDLE if no VGImage exists for the supplied CFbsBitmap or if the stored VGImage is out of date.
		Otherwise the VGImage associated with the CFbsBitmap is returned.
 */
VGImage CVgImageCache::GetVgImageFromBitmap(const CFbsBitmap& aBitmap, const TPoint& aOrigin)
	{
	// search through cache to find the item with a matching bitmap ID
	CVgImageCacheItem** itemPtr = iCacheItemMap.Find(aBitmap.SerialNumber());
	++iNumMatchTries;
	if(itemPtr == NULL)
		{
		// searched all the way through cache and there is no matching image
		++iNumMatchMisses;
		return VG_INVALID_HANDLE;
		}
	CVgImageCacheItem* item = *itemPtr;
	// Check whether the VGImage held by the item is up-to-date
	// - check touch counts are equal.
	// - check origins used for creating VGImage are equal.
	if (aBitmap.TouchCount() != item->iTouchCount || aOrigin != item->iOrigin)
		{
		// VGImage in item needs updating, so remove and delete the entry
		// and return NULL to indicate that a new entry needs to be created.
		DeleteItem(item);
		return VG_INVALID_HANDLE;
		}
	// VGImage is up-to date.
	// If item is not already at front of list, move it there
	if(!iVgImageCache.IsFirst(item))
		{
		item->iLink.Deque();
		iVgImageCache.AddFirst(*item);
		}
	return item->iImage;
	}

// Test APIs

/**
Determines whether an item exists for a specified bitmap's serial number.

@param aSerialNumber	The unique identifier of a CFbsBitmap.

@return ETrue if a matching item is found using the serial number.  EFalse if not.
 */
TBool CVgImageCache::IsInCache(TInt64 aSerialNumber)
	{
	return (iCacheItemMap.Find(aSerialNumber) != NULL);
	}

/**
The touch count stored in the item associated with the supplied serial number.

@param aSerialNumber	The unique identifier of a CFbsBitmap.

@return	The touch count stored in the item associated with the supplied serial number.
		KErrNotFound if no matching item is found using the serial number.
 */
TInt CVgImageCache::TouchCount(TInt64 aSerialNumber)
	{
	CVgImageCacheItem** itemPtr = iCacheItemMap.Find(aSerialNumber);
	if(itemPtr == NULL)
		{
		// searched all the way through cache and there is no matching image
		return KErrNotFound;
		}
	return (*itemPtr)->iTouchCount;
	}

/**
The number of entries in the cache.
@return The number of entries in the cache.
 */
TInt CVgImageCache::NumEntries() const
	{
	return iCacheItemMap.Count();
	}

/**
Returns a list of the serial numbers of all the cache items, with the most-recently used item at ordinal 0
and the least-recently used item and the end of the list.

@param aSerialNumberList A reference to a list in which to return the serial number list.
@param aListSize The number of entries allocated for the list.
 */
void CVgImageCache::GetOrderedCacheEntries(TInt64& aSerialNumberList, TInt aListSize)
	{
	GRAPHICS_ASSERT_ALWAYS(aSerialNumberList, EDirectGdiPanicInvalidPointArray)
	TDblQueIter<CVgImageCacheItem> iter(iVgImageCache);

	TInt n = iCacheItemMap.Count();
	// If n is greater than number of entries in list, restrict to number of entries.
	if (n > aListSize) n = aListSize;
	for (TInt ii = 0; ii < n; ++ii)
		{
		CVgImageCacheItem* item = iter++;
		(&aSerialNumberList)[ii] = item->iSerialNumber;
		}
	
	// If n is less than number of entries in list, pad out entries with 0
	// (i.e. invlaid serial numbers)
	while (n < aListSize)
		{
		(&aSerialNumberList)[n++] = 0;
		}
	}

/**
The total size of the cache in bytes.
@return The total size of the cache in bytes.
 */
TInt CVgImageCache::CacheSizeInBytes() const
	{
	return iCacheSizeInBytes;
	}

/**
The maximum size of the cache in bytes.
@return The maximum size of the cache in bytes.
 */
TInt CVgImageCache::MaxCacheSize() const
	{
	return iMaxCacheSizeInBytes;
	}

/**
Resets the cache. Iterates through all the items in the cache and deletes them.
 */
void CVgImageCache::ResetCache()
	{
	// cycle through all the items and destroy the VGImages.
	TDblQueIter<CVgImageCacheItem> iter(iVgImageCache);
	CVgImageCacheItem* item;
	while((item=iter++)!=NULL)
		{
		DeleteItem(item);
		}
	}

/**
Sets the maximum size in bytes of the cache. Checks the current size of the
cache and sets the maximum cache size if the current cache size is smaller 
or equal to aCacheSize.
@param aMaxCacheSize The maximum size in bytes to allow for the cache.
@return KErrNone if the maximum cache size has been changed successfully,
KErrArgument if aMaxCacheSize is smaller than the current cache size.
 */
TInt CVgImageCache::SetMaxCacheSize(TInt aMaxCacheSize)
	{
	if (iCacheSizeInBytes <= aMaxCacheSize) 
		{
		iMaxCacheSizeInBytes = aMaxCacheSize;
		return KErrNone;
		}
	
	return KErrArgument;
	}
