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

/**
 @file
*/

#include <e32std.h>
#include <imageconversion.h>
#include "surfaceutility.h"

CSurfaceUtility::CSurfaceUtility(CSurfaceUtility* aClone)
	:	iSurfaces(aClone?&(aClone->iSurfaces):NULL)
	{
	}
	
CSurfaceUtility* CSurfaceUtility::NewL(CSurfaceUtility* aClone/*=NULL*/)
	{
	CSurfaceUtility* utility = new (ELeave)CSurfaceUtility(aClone);
	CleanupStack::PushL(utility);
	utility->ConstructL();
	CleanupStack::Pop(utility);
	return utility;
	}
	
void CSurfaceUtility::ConstructL()
	{
	TInt r = iManager.Open();
	if (r != KErrNone)
		{
		LOG(("Surface manager failed to open: %d", r));
		User::Leave(r);
		}
	
	r = iSurfaceUpdateSession.Connect();
	if (r != KErrNone)
		{
		LOG(("Failed to connect to update server: %d", r));
		User::Leave(r);
		}
	}
	
CSurfaceUtility::~CSurfaceUtility()
	{
	DestroyAll();

	iSurfaces.Close();

	iManager.Close();

	iSurfaceUpdateSession.Close();
	}

TBool CSurfaceUtility::DestroyAll()
	{
	TInt err = 	KErrNone;
	TInt jj = iSurfaces.Count() - 1;
	if (jj<0)
		return EFalse;
	for (; jj >= 0; jj--)
		{
		err = iManager.CloseSurface(iSurfaces[jj]);
		if (err!=KErrNone)
			{
			LOG(("Error closing surface: 0x%X\n", err));
			}
		}
	iSurfaces.Reset();
	return ETrue;
	}

/***************************************
 * The aim of the THeapSurfaceArray is to locally switch in the specified heap for any array operation
 ***************************************/

CSurfaceUtility::RHeapSurfaceArray::RHeapSurfaceArray(RHeapSurfaceArray* aUseExternalArray)
	:	iUseArray(aUseExternalArray?aUseExternalArray->iUseArray:&this->iLocalArray),
	iExternalHeapRef(aUseExternalArray?aUseExternalArray->iExternalHeapRef:User::Heap())
	{
	
	}
/************************************
 * The following methods have been used by the surfaceutility... some require the heap wrapping, and some don't
 * I actually need three different startegies (count em) for 7 methods...
 * Some methods only read the existing objects, so don't need a heap swap at all
 * Leaving methods have to use PopAndDestroy strategy to restore the heap on leaving or success
 * Non-leaving methods must not call PushL, so directly make SwitchHeap calls!
 ************************************/

// PopAndDestroy method to restore the heap
/*static*/ void	CSurfaceUtility::RHeapSurfaceArray::PopHeap(void* aHeapPtr)
	{
	RHeap* heapPtr=(RHeap*)aHeapPtr;
	User::SwitchHeap(heapPtr);
	}

// Switches and pushes the previous heap so it can be restored with PopAndDestroy
/*static*/ void CSurfaceUtility::RHeapSurfaceArray::SwitchHeapLC(RHeap* aNewHeap)
	{
	CleanupStack::PushL(TCleanupItem(PopHeap,NULL));
	CleanupStack::PushL(TCleanupItem(PopHeap,NULL));
	CleanupStack::PushL(TCleanupItem(PopHeap,NULL));
	CleanupStack::Pop(3);
	RHeap* oldHeap=User::SwitchHeap(aNewHeap);
	delete new char;
	CleanupStack::PushL(TCleanupItem(PopHeap,oldHeap));
	}


TSurfaceId& CSurfaceUtility::RHeapSurfaceArray::operator[](TUint aIndex)
	{
	return iUseArray->operator[](aIndex);
	}

// Close only closes the local array, while Reset resets the active array (may be external)
void CSurfaceUtility::RHeapSurfaceArray::Close()
	{
	iLocalArray.Close();
	}

TInt CSurfaceUtility::RHeapSurfaceArray::Count() const
	{
	return iUseArray->Count();
	}

// Close only closes the local array, while Reset resets the active array (may be external)
inline void CSurfaceUtility::RHeapSurfaceArray::Reset()
	{
	iUseArray->Reset();
	}

void CSurfaceUtility::RHeapSurfaceArray::AppendL(const TSurfaceId &anEntry)
	{
	iUseArray->AppendL(anEntry);
	}

TInt CSurfaceUtility::RHeapSurfaceArray::Find(const TSurfaceId &anEntry) const
	{
	return iUseArray->Find(anEntry);
	}

void CSurfaceUtility::RHeapSurfaceArray::Remove(TInt anIndex)
	{
	iUseArray->Remove(anIndex);
	}

/**
Cleanup stack helper object, holding references to both utility and surface, so
that the standard Close() semantics can be used.
*/
class TSurfaceCleanup
	{
public:
	TSurfaceCleanup(CSurfaceUtility& aUtility, TSurfaceId& aSurface)
		: iUtility(aUtility), iSurface(aSurface)
		{}
	void Close()
		{
		// Removes the surface from the list of surfaces to clean up, and closes
		// the surface reference.
		iUtility.DestroySurface(iSurface);
		}
private:
	CSurfaceUtility& iUtility;
	TSurfaceId& iSurface;
	};

/**
Get the size of a surface.

@param aSurface The surface to get the size for.
@return The size in pixels, or empty on failure.
*/
TSize CSurfaceUtility::SurfaceSize(const TSurfaceId& aSurface)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	if (iManager.SurfaceInfo(aSurface, infoBuf) == KErrNone)
		{
		return info.iSize;
		}

	return TSize();
	}

/**
Create a surface using the surface manager.

Stores the ID for tear down, as well as returning it.

@param aSize Dimensions of the surface.
@param aPixelFormat	UID of the pixel format.
@param aStride	Stride value for the surface (usually bytes per pixel * width)
@leave May leave due to lack of memory.
@return New surface's ID.
*/
TSurfaceId CSurfaceUtility::CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride, TInt aBuffers)
	{
	RSurfaceManager::TSurfaceCreationAttributesBuf bf;
	RSurfaceManager::TSurfaceCreationAttributes& b = bf();
	if (aStride<aSize.iWidth*BytesPerPixelL(aPixelFormat))
	    {
	    User::Leave(KErrOverflow);
	    }
	b.iSize.iWidth = aSize.iWidth;
	b.iSize.iHeight = aSize.iHeight;
	b.iBuffers = aBuffers;				// number of buffers in the surface
	b.iPixelFormat = aPixelFormat;
	b.iStride = aStride;		// Number of bytes between start of one line and start of next
	b.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	b.iAlignment = 4;			// alignment, 1,2,4,8 byte aligned
	b.iContiguous = EFalse;
	b.iMappable = ETrue;

	TSurfaceId surface = TSurfaceId::CreateNullId();

	User::LeaveIfError(iManager.CreateSurface(bf, surface));
	iSurfaces.AppendL(surface);
	return surface;
	}

/**
A helper function that returns the bytes per pixel for a given pixel format uid

@param aPixelFormat Pixel format UID to convert
@return The bytes per pixel
*/
TInt CSurfaceUtility::BytesPerPixelL(TUidPixelFormat aPixelFormat)
	{
	TInt bytesPerPixel = 0;
	switch (aPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
		case EUidPixelFormatARGB_8888:
		case EUidPixelFormatARGB_8888_PRE:
			{
			bytesPerPixel = 4;
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
		case EUidPixelFormatRGB_565:
			{
			bytesPerPixel = 2;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}
	return bytesPerPixel;
	}

/**
Fill the given surface with a color.

@param aSurface	The surface to be filled.
@param aColor	The color to fill it with.
*/
void CSurfaceUtility::FillSurfaceL(TSurfaceId& aSurface, const TRgb& aColor)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 color = 0;
	TBool use16 = EFalse;

	if (info.iSize.iHeight<0 || info.iSize.iWidth<0 || info.iStride<0)
		{
		User::Leave(KErrCorrupt);
		}
	if (info.iSize.iHeight==0 || info.iSize.iWidth==0 || info.iStride==0)
		{
		User::Leave(KErrNotReady);
		}

	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			color = aColor.Color16MU();
#ifdef ALPHA_FIX_24BIT
			color |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			color = aColor.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			color = aColor.Color16MAP();
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			color = aColor.Color4K();
			use16 = ETrue;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			color = aColor.Color64K();
			use16 = ETrue;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);

	TInt offsetToFirstBuffer;
	User::LeaveIfError(iManager.GetBufferOffset(aSurface, 0, offsetToFirstBuffer));
	TUint8* surfacePtr = chunk.Base() + offsetToFirstBuffer;
	TUint8* linePtr = surfacePtr;

	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr);

		// Fill first line
		for (TInt xx = 0; xx < info.iSize.iWidth; xx++)
			{
			ptr[xx] = (TUint16)color;
			}
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint32* ptr = reinterpret_cast<TUint32*>(surfacePtr);

		// Fill first line
		for (TInt xx = 0; xx < info.iSize.iWidth; xx++)
			{
			ptr[xx] = color;
			}
		}

	// Now copy that to the other lines
	for (TInt yy = 1; yy < info.iSize.iHeight; yy++)
		{
		linePtr += info.iStride;
		Mem::Copy(linePtr, surfacePtr, info.iSize.iWidth * BytesPerPixelL(info.iPixelFormat));
		}
	
	TInt err = SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));

	CleanupStack::PopAndDestroy(/* chunk */);
	}

/**
Fill the given surface with a color.

@param aSurface	The surface to be filled.
@param aBuffer 	The buffer to fill.
@param aColor	The color to fill it with.
*/
void CSurfaceUtility::FillSurfaceL(TSurfaceId& aSurface, TInt aBuffer, const TRgb& aColor)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 color = 0;
	TBool use16 = EFalse;
	TInt numBuffers = info.iBuffers;
	if (aBuffer < 0 || aBuffer >= numBuffers)
		{
		User::Leave(KErrArgument);
		}

	if (info.iSize.iHeight<0 || info.iSize.iWidth<0 || info.iStride<0)
		{
		User::Leave(KErrCorrupt);
		}
	if (info.iSize.iHeight==0 || info.iSize.iWidth==0 || info.iStride==0)
		{
		User::Leave(KErrNotReady);
		}

	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			color = aColor.Color16MU();
#ifdef ALPHA_FIX_24BIT
			color |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			color = aColor.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			color = aColor.Color16MAP();
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			color = aColor.Color4K();
			use16 = ETrue;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			color = aColor.Color64K();
			use16 = ETrue;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);

	TInt offsetToBuffer;
	User::LeaveIfError(iManager.GetBufferOffset(aSurface, aBuffer, offsetToBuffer));
	TUint8* surfacePtr = chunk.Base() + offsetToBuffer;
	TUint8* linePtr = surfacePtr;

	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr);

		// Fill first line
		for (TInt xx = 0; xx < info.iSize.iWidth; xx++)
			{
			ptr[xx] = (TUint16)color;
			}
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint32* ptr = reinterpret_cast<TUint32*>(surfacePtr);

		// Fill first line
		for (TInt xx = 0; xx < info.iSize.iWidth; xx++)
			{
			ptr[xx] = color;
			}
		}

	// Now copy that to the other lines
	for (TInt yy = 1; yy < info.iSize.iHeight; yy++)
		{
		linePtr += info.iStride;
		Mem::Copy(linePtr, surfacePtr, info.iSize.iWidth * BytesPerPixelL(info.iPixelFormat));
		}
	
	TInt err = SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));

	CleanupStack::PopAndDestroy(/* chunk */);
	}


/**
Destroy a surface.

As well as destroying the surface, it is removed from the set held for
destruction during tear down.

@param aSurface	The surface to be destroyed.
*/
void CSurfaceUtility::DestroySurface(TSurfaceId& aSurface)
	{
	TInt index = iSurfaces.Find(aSurface);
	
	if (index != KErrNotFound)
		{
		iSurfaces.Remove(index);
		}

	TInt err = iManager.CloseSurface(aSurface);
	if (err!=KErrNone)
		LOG(("Error closing surfaces: 0x%X\n", err));
	}


/**
Submit an update to a surface to the update server.

@param aScreenNumber	The screen to be updated where the surface is shown.
@param aSurface	The surface which has been updated.
@param aRegion	The area of the surface affected, or NULL for all of it.
*/
TInt CSurfaceUtility::SubmitUpdate(TInt /* aScreenNumber */, const TSurfaceId& aSurface,TInt aBufferNumber, TInt aNullRegion)
    {
    if (aNullRegion==0)
        {
        return SubmitUpdate(KAllScreens, aSurface, aBufferNumber);
        }
    else
        if (aBufferNumber==0)
            {
            return SubmitUpdate(KAllScreens, aSurface, aNullRegion);
            }
        else
            {
            return KErrNotSupported;
            }
    }

TInt CSurfaceUtility::SubmitUpdate(TInt /* aScreenNumber */, const TSurfaceId& aSurface, const TRegion* aRegion,TInt aBufferNumber)
    {
    return SubmitUpdate(KAllScreens, aSurface, aBufferNumber, aRegion);
    }

TInt CSurfaceUtility::SubmitUpdate(TInt /* aScreenNumber */, const TSurfaceId& aSurface,TInt aBufferNumber, const TRegion* aRegion)
	{
	if (!iSurfaceUpdateSession.Handle())
	    {
	    iSurfaceUpdateSession.Connect();
	    }
    if (!iSurfaceUpdateSession.Handle())
        {
        LOG(("Error - SUS client not started!"));
        return KErrNotReady;
        }
    else
        {
        TInt err =iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, aBufferNumber, aRegion); 
        if (err!=KErrNone)
            LOG(("Error submitting update: 0x%X\n", err));
        return err;
        }
	}

void CSurfaceUtility::FillNativeStreamSurfaceL(TSurfaceId& aSurface, TUint8* aBufferPtr, const TRgb& aColor)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 color = 0;
	TBool use16 = EFalse;

	if (info.iSize.iHeight<0 || info.iSize.iWidth<0 || info.iStride<0)
		{
		User::Leave(KErrCorrupt);
		}
	if (info.iSize.iHeight==0 || info.iSize.iWidth==0 || info.iStride==0)
		{
		User::Leave(KErrNotReady);
		}

	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			color = aColor.Color16MU();
#ifdef ALPHA_FIX_24BIT
			color |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			color = aColor.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			color = aColor.Color16MAP();
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			color = aColor.Color4K();
			use16 = ETrue;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			color = aColor.Color64K();
			use16 = ETrue;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}

	TUint8* surfacePtr = aBufferPtr;
	TUint8* linePtr = surfacePtr;

	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr);

		// Fill first line
		for (TInt xx = 0; xx < info.iSize.iWidth; xx++)
			{
			ptr[xx] = (TUint16)color;
			}
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint32* ptr = reinterpret_cast<TUint32*>(surfacePtr);

		// Fill first line
		for (TInt xx = 0; xx < info.iSize.iWidth; xx++)
			{
			ptr[xx] = color;
			}
		}

	// Now copy that to the other lines
	for (TInt yy = 1; yy < info.iSize.iHeight; yy++)
		{
		linePtr += info.iStride;
		Mem::Copy(linePtr, surfacePtr, info.iSize.iWidth * BytesPerPixelL(info.iPixelFormat));
		}	
	}

TBool CSurfaceUtility::CompareSurfacesL(TSurfaceId& aSurface, TInt aBuffer, TSurfaceId& aStreamSurface, TUint8* aBufferPtr)
	{
	RSurfaceManager::TInfoBuf infoBuf1;
	RSurfaceManager::TSurfaceInfoV01& info1 = infoBuf1();
	
	RSurfaceManager::TInfoBuf infoBuf2;
	RSurfaceManager::TSurfaceInfoV01& info2 = infoBuf2();	

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf1));
	User::LeaveIfError(iManager.SurfaceInfo(aStreamSurface, infoBuf2));
	TBool use16 = EFalse;
	
	if (aBuffer < 0 || aBuffer >= info1.iBuffers)
		{
		User::Leave(KErrArgument);
		}

	if (info1.iPixelFormat != info2.iPixelFormat)
		{
		User::Leave(KErrArgument);
		}	
	
	if ((info1.iSize.iHeight<0 || info1.iSize.iWidth<0 || info1.iStride<0) ||
		(info2.iSize.iHeight<0 || info2.iSize.iWidth<0 || info2.iStride<0))
		{
		User::Leave(KErrCorrupt);
		}
	if ((info1.iSize.iHeight==0 || info1.iSize.iWidth==0 || info1.iStride==0) ||
		(info2.iSize.iHeight==0 || info2.iSize.iWidth==0 || info2.iStride==0))
		{
		User::Leave(KErrNotReady);
		}
	if (info1.iSize != info2.iSize)
		{
		User::Leave(KErrArgument);
		}

	switch (info1.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
		case EUidPixelFormatARGB_8888:
		case EUidPixelFormatARGB_8888_PRE:
			{
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
		case EUidPixelFormatRGB_565:
			{
			use16 = ETrue;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}

	// Surface
	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);
	
	TInt offsetToBuffer;
	User::LeaveIfError(iManager.GetBufferOffset(aSurface, aBuffer, offsetToBuffer));
	TUint8* surfacePtr1 = chunk.Base() + offsetToBuffer;

	// Native stream
	TUint8* surfacePtr2 = aBufferPtr;
	
	TUint32 color1 = 0;
	TUint32 color2 = 0;
	
	if (use16)
		{
		if ((info1.iSize.iWidth*2>info1.iStride) ||
			(info2.iSize.iWidth*2>info2.iStride))
			{
			User::Leave(KErrOverflow);
			}
		
		TUint16* ptr1 = reinterpret_cast<TUint16*>(surfacePtr1);
		TUint16* ptr2 = reinterpret_cast<TUint16*>(surfacePtr2);

		// Fill first line
		for (TInt xx = 0; xx < info1.iSize.iWidth; xx++)
			{
			for (TInt yy = 0; yy < info1.iSize.iHeight; yy++)
				{
				color1 = (TUint16)ptr1[xx];
				color2 = (TUint16)ptr2[xx];
				if (color1 != color2)
					{
					return EFalse;
					}
				}
			}
		}
	else
		{
		if ((info1.iSize.iWidth*4>info1.iStride) ||
			(info2.iSize.iWidth*4>info2.iStride))
			{
			User::Leave(KErrOverflow);
			}

		TUint32* ptr1 = reinterpret_cast<TUint32*>(surfacePtr1);
		TUint32* ptr2 = reinterpret_cast<TUint32*>(surfacePtr2);		

		// Fill first line
		for (TInt xx = 0; xx < info1.iSize.iWidth; xx++)
			{
			for (TInt yy = 0; yy < info1.iSize.iHeight; yy++)
				{
				color1 = ptr1[xx];
				color2 = ptr2[xx];
				if (color1 != color2)
					{
					CleanupStack::PopAndDestroy(/* chunk */);
					return EFalse;
					}
				}
			}
		}	
		
	CleanupStack::PopAndDestroy(/* chunk */);
	return ETrue;
	}

void CSurfaceUtility::NotifyWhenDisplayed(TRequestStatus& aStatusDisplayed, TTimeStamp& aTimeStamp)
    {
    iSurfaceUpdateSession.NotifyWhenDisplayed(aStatusDisplayed, aTimeStamp);
    }

void CSurfaceUtility::NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus& aStatusDisplayedX)
    {
    iSurfaceUpdateSession.NotifyWhenDisplayedXTimes(aCount, aStatusDisplayedX);
    }

void CSurfaceUtility::NotifyWhenAvailable(TRequestStatus& aStatusAvailable)
    {
    iSurfaceUpdateSession.NotifyWhenAvailable(aStatusAvailable);
    }
