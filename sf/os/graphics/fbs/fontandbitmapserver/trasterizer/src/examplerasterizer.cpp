// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "examplerasterizer.h"
#include <s32mem.h>
#include <fbs.h>

/** Create a new example rasterizer.

@return A pointer to a newly constructed CFbsRasterizer object if successful,
or NULL if no memory is available.
 */
EXPORT_C CFbsRasterizer* CFbsRasterizer::New()
	{
	return new CExampleRasterizer;
	}


/** Constructor.
 */
CExampleRasterizer::CExampleRasterizer()
	: iRegisteredBmps(_FOFF(CRegisteredBitmap, iLink)), iRecentBmps(_FOFF(CRegisteredBitmap, iLink))
	{}

/** Destructor.
 */
CExampleRasterizer::~CExampleRasterizer()
	{
	while (!iRegisteredBmps.IsEmpty())
		{
		delete iRegisteredBmps.First();
		}
	while (!iRecentBmps.IsEmpty())
		{
		delete iRecentBmps.First();
		}
	delete iIdle;
	}

/** Register a bitmap with this rasterizer for rendering.

In this example an extended bitmap is a tricolour flag with three coloured stripes of the same size 
being either vertically or horizontally oriented. The colours to be used in the flag and a TUint8 
that specifies horizontal or vertical stripes are passed in the aBitmapDesc.iData parameter, which 
should point to a buffer containing the three stripe colours first, each colour as three TUint8 
values, followed by a TUint8 value where 1 means horizontal stripes and 0 means vertical stripes.

The only display modes supported by this example rasterizer are EGray256, EColor64K, EColor16MU and EColor16MAP.

If the rasterizer cannot allocate enough memory for rendering the extended bitmap this function 
will return silently. If an unsupported display mode is passed this function will return silently.

@see CFbsRasterizer::BeginBitmap()
@see EndBitmap()
 */
void CExampleRasterizer::BeginBitmap(TInt64 aBitmapId, const TBitmapDesc& aBitmapDesc, const TRegion* aRegionOfInterest)
	{
	// Check that the passed extended bitmap description is valid
	if ((aBitmapDesc.iSizeInPixels.iWidth <= 0) || (aBitmapDesc.iSizeInPixels.iHeight <= 0)
		// This test rasterizer only supports EGray256, EColor64K, EColor16MU and EColor16MAP
		|| ((aBitmapDesc.iDispMode != EGray256) && (aBitmapDesc.iDispMode != EColor64K) && (aBitmapDesc.iDispMode != EColor16MU) && (aBitmapDesc.iDispMode != EColor16MAP))
		|| (aBitmapDesc.iDataType != KUidExampleExtendedBitmap)
		|| (aBitmapDesc.iData == NULL)
		|| (aBitmapDesc.iDataSize <= 0))
		{
#ifdef _DEBUG
		RDebug::Printf("Example Rasterizer Error: Invalid TBitmapDesc passed to CExampleRasterizer::BeginBitmap()");
#endif // _DEBUG
		return;
		}
	
	// Check if the bitmap is already registered
	CRegisteredBitmap* foundRegBmp = RegisteredBitmap(aBitmapId);
	if (!foundRegBmp)
		{
		// Not registered: check if the bitmap is in the cache of recently used bitmaps
		foundRegBmp = RecentBitmap(aBitmapId);
		if (foundRegBmp)
			{
			// Take the bitmap out of the cache and put it in the list of registered bitmaps
			foundRegBmp->iLink.Deque();
			iTotalRecentBmpSize -= foundRegBmp->iBufLength;
			iRegisteredBmps.AddLast(*foundRegBmp);
			}
		}
	if (foundRegBmp)
		{
#ifdef _DEBUG
		// Make sure an extended bitmap is not registered more than once with different attributes
		if ((foundRegBmp->iSizeInPixels != aBitmapDesc.iSizeInPixels)
			|| (foundRegBmp->iDispMode != aBitmapDesc.iDispMode)
			|| (foundRegBmp->iDataType != aBitmapDesc.iDataType))
			{
			RDebug::Printf("Example Rasterizer Error: CExampleRasterizer::BeginBitmap() called for an extended bitmap that has already been registered with a different TBitmapDesc");
			}
#endif // _DEBUG
		
		// This bitmap is already registered, just increment its reference count and return
		foundRegBmp->iRefCount++;
		return;
		}
	
	// Get the scanline length we should use for this display mode as the scanlines we draw must be 32-bit aligned
	TInt scanLineLength = CFbsBitmap::ScanLineLength(aBitmapDesc.iSizeInPixels.iWidth, aBitmapDesc.iDispMode);
	// Create a CRegisteredBitmap object to save the pre-rendered bitmap
	TInt bufLength = scanLineLength * aBitmapDesc.iSizeInPixels.iHeight;
	// Allocate bufLength extra bytes for the embedded pixel buffer
	CRegisteredBitmap* regBmp = new(bufLength) CRegisteredBitmap(aBitmapId, aBitmapDesc, bufLength);
	if (!regBmp)
		{
#ifdef _DEBUG
		RDebug::Printf("Example Rasterizer Error: Failed to allocate memory");
#endif // _DEBUG		
		return;
		}
	
	// Pre-render the flag that the extended bitmap represents
	RMemReadStream readStream;
	readStream.Open(aBitmapDesc.iData, aBitmapDesc.iDataSize);
	TInt err = regBmp->Draw(readStream, aRegionOfInterest);
	readStream.Close();
	if (err != KErrNone)
		{
#ifdef _DEBUG
		RDebug::Printf("Example Rasterizer Error: CExampleRasterizer::CRegisteredBitmap::Draw() returned %d", err);
#endif // _DEBUG
		delete regBmp;
		return;
		}
	
	// Success
	iRegisteredBmps.AddLast(*regBmp);
	regBmp->iRefCount = 1;
	}

/** Unregister an extended bitmap from this rasterizer. 

@see CFbsRasterizer::EndBitmap()
@see BeginBitmap()
 */
void CExampleRasterizer::EndBitmap(TInt64 aBitmapId)
	{
	CRegisteredBitmap* regBmp = RegisteredBitmap(aBitmapId);
	if (regBmp)
		{
		if (--regBmp->iRefCount == 0)
			{
			// Put unregistered bitmap in the cache of recently used bitmaps if wholly pre-rendered
			// and there is an active scheduler to add the idle-time clean-up active object to
			if (regBmp->iWhole && CActiveScheduler::Current())
				{
				if (!iIdle)
					{
					iIdle = CIdle::New(CActive::EPriorityIdle);
					if (!iIdle)
						{
						delete regBmp;
						return;
						}
					}
				regBmp->iLink.Deque();
				iRecentBmps.AddFirst(*regBmp);
				iTotalRecentBmpSize += regBmp->iBufLength;
				// Delete the least recently used bitmaps if the maximum size of the cache is exceeded
				while (iTotalRecentBmpSize > KMaxRecentBmpCacheSize)
					{
					regBmp = iRecentBmps.Last();
					iTotalRecentBmpSize -= regBmp->iBufLength;
					delete regBmp;
					}
				// If the cache is not empty make sure the idle-time clean-up active object is scheduled to run
				if (!iRecentBmps.IsEmpty() && !iIdle->IsActive())
					{
					iIdle->Start(TCallBack(IdleFunction, this));
					}
				}
			else
				{
				delete regBmp;
				}
			}
		}
#ifdef _DEBUG
	else
		{
		RDebug::Printf("Example Rasterizer Error: CExampleRasterizer::EndBitmap() called for a bitmap that is not registered");
		}
#endif // _DEBUG
	}

/** Return a scanline from the passed extended bitmap given its bitmap id. 

@see CFbsRasterizer::ScanLine()
@see BeginBitmap()
@see EndBitmap()
 */
const TUint32* CExampleRasterizer::ScanLine(TInt64 aBitmapId, const TPoint& aPixel, TInt aLength)
	{
	// Check that BeginBitmap() was called successfully and the passed bitmap was registered
	// with this rasterizer
	CRegisteredBitmap* regBmp = RegisteredBitmap(aBitmapId);
	if (regBmp)
		{
		// Parameter check
		if ((aPixel.iX < 0) || (aPixel.iX >= regBmp->iSizeInPixels.iWidth)
			|| (aPixel.iY < 0) || (aPixel.iY >= regBmp->iSizeInPixels.iHeight)
			|| (aLength > (regBmp->iSizeInPixels.iWidth - aPixel.iX))) // the requested length in pixels is greater than the declared width of the bitmap
			{
#ifdef _DEBUG
			RDebug::Printf("Example Rasterizer Error: Invalid parameter passed to CExampleRasterizer::ScanLine()");
#endif // _DEBUG
			return NULL;
			}
		
		TInt scanLineLength = CFbsBitmap::ScanLineLength(regBmp->iSizeInPixels.iWidth, regBmp->iDispMode);
		return reinterpret_cast<TUint32*>(regBmp->Buffer() + (aPixel.iY * scanLineLength));
		}
#ifdef _DEBUG
	else
		{
		RDebug::Printf("Example Rasterizer Error: CExampleRasterizer::ScanLine() called for a bitmap that is not registered");
		}
#endif // _DEBUG
	
	return NULL;
	}

/** Only MFbsRasterizerClearCache available, KErrExtensionNotSupported returned for all other cases.

@see CFbsRasterizer::GetInterface()
 */
TInt CExampleRasterizer::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt err = KErrNone;
	
	switch (aInterfaceId.iUid)
		{
		case KUidFbsRasterizerClearCache:
			aInterface = static_cast<MFbsRasterizerClearCache*>(this);
			break;
		default:
			err = KErrExtensionNotSupported;						
			break;
		}
	
	return err;
	}


/** Gets a bitmap that has been registered with this rasterizer.

@param aBitmapId The ID of the bitmap to check for.

@return A pointer to the registered bitmap if found, NULL otherwise.
 */
CExampleRasterizer::CRegisteredBitmap* CExampleRasterizer::RegisteredBitmap(TInt64 aBitmapId)
	{
	TDblQueIter<CRegisteredBitmap> iter(iRegisteredBmps);
	while (CRegisteredBitmap* regBmp = iter++)
		{
		if (regBmp->iBitmapId == aBitmapId)
			{
			return regBmp;
			}
		}
	return NULL;
	}

/** Gets a bitmap that has been recently used with this rasterizer.

@param aBitmapId The ID of the bitmap to check for.

@return A pointer to the recently used bitmap if found, NULL otherwise.
 */
CExampleRasterizer::CRegisteredBitmap* CExampleRasterizer::RecentBitmap(TInt64 aBitmapId)
	{
	TDblQueIter<CRegisteredBitmap> iter(iRecentBmps);
	while (CRegisteredBitmap* regBmp = iter++)
		{
		if (regBmp->iBitmapId == aBitmapId)
			{
			return regBmp;
			}
		}
	return NULL;
	}

/** Releases the memory for all the recently rasterized extended bitmaps kept in the cache.
This function is called by the idle-time active object.

@param aSelf A pointer to the rasterizer object.

@return Zero to indicate that this function needn't be automatically called again.
 */
TInt CExampleRasterizer::IdleFunction(TAny* aPtr)
	{
	CExampleRasterizer* self = static_cast<CExampleRasterizer*>(aPtr);
	while (!self->iRecentBmps.IsEmpty())
		{
		delete self->iRecentBmps.First();
		}
	self->iTotalRecentBmpSize = 0;
	return 0;
	}


/** Constructor.
 */
CExampleRasterizer::CRegisteredBitmap::CRegisteredBitmap(TInt64 aBitmapId,
	                                                     const TBitmapDesc& aBitmapDesc,
	                                                     TInt aBufLength)
	: iBitmapId(aBitmapId),
	  iSizeInPixels(aBitmapDesc.iSizeInPixels),
	  iDispMode(aBitmapDesc.iDispMode),
	  iDataType(aBitmapDesc.iDataType),
	  iBufLength(aBufLength)
	{}

/** Destructor.
 */
CExampleRasterizer::CRegisteredBitmap::~CRegisteredBitmap()
	{
	iLink.Deque();
	}

/** Rasterize a registered extended bitmap. The results are stored into the memory buffer embedded in this object.

@param aReadStream A stream to read the proprietary data from.
@param aRegionOfInterest The region within which scanline access requests will be made, if known.

@return KErrNone on success, KErrNotSupported if the display mode is not supported,
	KErrEof if there is not enough data to read, KErrArgument if aRegionOfInterest is out of bounds.
 */
TInt CExampleRasterizer::CRegisteredBitmap::Draw(RMemReadStream& aReadStream, const TRegion* aRegionOfInterest)
	{
	if (aRegionOfInterest && !aRegionOfInterest->IsContainedBy(iSizeInPixels))
		{
		return KErrArgument;
		}
	
	// Read the three flag colours and the horizontal stripe flag from the data
	TRgb colour1, colour2, colour3;
	TUint8 horizontalStripe;
	TRAPD(err,
		aReadStream >> colour1;
		aReadStream >> colour2;
		aReadStream >> colour3;
		aReadStream >> horizontalStripe;
		);
	if (err != KErrNone)
		{
		return err;
		}
	TUint32 c1, c2, c3;
	TInt bpp;
	switch (iDispMode)
		{
	case EGray256:
		c1 = colour1.Gray256();
		c2 = colour2.Gray256();
		c3 = colour3.Gray256();
		bpp = 1;
		break;
	case EColor64K:
		c1 = colour1.Color64K();
		c2 = colour2.Color64K();
		c3 = colour3.Color64K();
		bpp = 2;
		break;
	case EColor16MU:
		c1 = colour1.Color16MU() | 0xFF000000;
		c2 = colour2.Color16MU() | 0xFF000000;
		c3 = colour3.Color16MU() | 0xFF000000;
		bpp = 4;
		break;
	case EColor16MAP:
		c1 = colour1.Color16MAP();
		c2 = colour2.Color16MAP();
		c3 = colour3.Color16MAP();
		bpp = 4;
		break;
	default:
		return KErrNotSupported;
		}
	
	// If no region of interest is specified, rasterize the whole size of the bitmap
	// Otherwise rasterize only the rectangles in the region of interest
	// (Method takes no account of overlapping rectangles)
	TRect wholeRect(iSizeInPixels);
	TInt numRects;
	const TRect* rects;
	if (aRegionOfInterest)
		{
		numRects = aRegionOfInterest->Count();
		rects = aRegionOfInterest->RectangleList();
		// Pre-fill the flag with white so that areas outside the region of interest are white
		// (this is not necessary but has been added to allow use of this rasterizer in BitGDI testing)
		Mem::Fill(Buffer(), iBufLength, 0xFF);
		}
	else
		{
		// Set the only rect to be drawn to the whole size of the bitmap as no region of interest is specified
		numRects = 1;
		rects = &wholeRect;
		}
	
	// Get the scanline length we should use for this display mode as the scanlines we draw must be 32-bit aligned
	TInt scanLineLength = CFbsBitmap::ScanLineLength(iSizeInPixels.iWidth, iDispMode);
	
	if (horizontalStripe)
		{
		// Horizontal stripes
		TInt stripeHeight = iSizeInPixels.iHeight / 3;
		for (TInt r = 0; r < numRects; ++r)
			{
			for (TInt h = rects[r].iTl.iY; h < rects[r].iBr.iY; ++h)
				{
				for (TInt w = rects[r].iTl.iX; w < rects[r].iBr.iX; ++w)
					{
					if (h < stripeHeight)
						{
						Mem::Copy(Buffer() + (w * bpp) + (h * scanLineLength), &c1, bpp); // top
						}
					else if (h < iSizeInPixels.iHeight - stripeHeight)
						{
						Mem::Copy(Buffer() + (w * bpp) + (h * scanLineLength), &c2, bpp); // middle
						}
					else
						{
						Mem::Copy(Buffer() + (w * bpp) + (h * scanLineLength), &c3, bpp); // bottom
						}
					}
				}
			}
		}
	else
		{
		// Vertical stripes
		TInt stripeWidth = iSizeInPixels.iWidth / 3;
		for (TInt r = 0; r < numRects; ++r)
			{
			for (TInt h = rects[r].iTl.iY; h < rects[r].iBr.iY; ++h)
				{
				for (TInt w = rects[r].iTl.iX; w < rects[r].iBr.iX; ++w)
					{
					if (w < stripeWidth)
						{
						Mem::Copy(Buffer() + (w * bpp) + (h * scanLineLength), &c1, bpp); // left
						}
					else if (w < iSizeInPixels.iWidth - stripeWidth)
						{
						Mem::Copy(Buffer() + (w * bpp) + (h * scanLineLength), &c2, bpp); // middle
						}
					else
						{
						Mem::Copy(Buffer() + (w * bpp) + (h * scanLineLength), &c3, bpp); // right
						}
					}
				}
			}
		}
	
	iWhole = (numRects == 1 && rects[0] == wholeRect);
	return KErrNone;
	}

/** Gets a pointer to the pre-rendered buffer for this registered bitmap.
 */
TUint8* CExampleRasterizer::CRegisteredBitmap::Buffer()
	{
	// Adding one to "this" is the same as adding sizeof(*this) to the address of this object
	// The result points to the pre-rendered buffer, located right after the instance variables
	return reinterpret_cast<TUint8*>(this + 1);
	}

/** Implementation of MFbsRasterizerClearCache::ClearCache() to allow test code to 
clear the cache of recently used extended bitmaps.
 */
void CExampleRasterizer::ClearCache()
	{
	while (!iRecentBmps.IsEmpty())
		{
		delete iRecentBmps.First();
		}
	iTotalRecentBmpSize = 0;
	if (iIdle && iIdle->IsActive())
		{
		iIdle->Cancel();
		}
	}
