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
 * @file
*/

#include <e32std.h>
#include <imageconversion.h>
#include "surfaceutility.h"
#include <BitmapTransforms.h> 

//_LIT(KFileName,"surfacemanager");

CSurfaceUtility::CSurfaceUtility(CSurfaceUtility* aClone/*=NULL*/)
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
	
	// the following call is needed because of a bug in CImageDecoder that 
	// leaks heap memory
	REComSession::FinalClose();
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
	RHeap* oldHeap=User::SwitchHeap(&iExternalHeapRef);
	iLocalArray.Close();
	User::SwitchHeap(oldHeap);
	}
TInt CSurfaceUtility::RHeapSurfaceArray::Count() const
	{
	return iUseArray->Count();
	}
// Close only closes the local array, while Reset resets the active array (may be external)
inline void CSurfaceUtility::RHeapSurfaceArray::Reset()
	{
	RHeap* oldHeap=User::SwitchHeap(&iExternalHeapRef);
	iUseArray->Reset();
	User::SwitchHeap(oldHeap);
	}
void CSurfaceUtility::RHeapSurfaceArray::AppendL(const TSurfaceId &anEntry)
	{
	SwitchHeapLC(&iExternalHeapRef);
	iUseArray->AppendL(anEntry);
	CleanupStack::PopAndDestroy();
	}
TInt CSurfaceUtility::RHeapSurfaceArray::Find(const TSurfaceId &anEntry) const
	{
	return iUseArray->Find(anEntry);
	}
void CSurfaceUtility::RHeapSurfaceArray::Remove(TInt anIndex)
	{
	RHeap* oldHeap=User::SwitchHeap(&iExternalHeapRef);
	iUseArray->Remove(anIndex);
	User::SwitchHeap(oldHeap);
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
Read the given image file into a new surface.

@param aFileName The name of the image file.
@param aSurface Filled with the surface ID for the surface containing the pixels.
*/
void CSurfaceUtility::CreateSurfaceFromFileL(const TDesC& aFileName, TSurfaceId& aSurface)
	{
	RFs fs;
	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CImageDecoder* decoder = CImageDecoder::FileNewL(fs, aFileName, CImageDecoder::EOptionAlwaysThread);
	CleanupStack::PushL(decoder);

	const TFrameInfo& info = decoder->FrameInfo();

	TSize size = info.iOverallSizeInPixels;
	TInt stride = size.iWidth << 2;		// Default to four bytes per pixel
	TDisplayMode bmpFormat = info.iFrameDisplayMode;
	TUidPixelFormat pixelFormat = EUidPixelFormatUnknown;

	switch (bmpFormat)
		{
		case EGray2:
		case EGray4:
		case EGray16:
		case EGray256:
		case EColor16:
		case EColor256:
		case EColor16M:
		case EColor16MU:
			{
			bmpFormat = EColor16MU;
			pixelFormat = EUidPixelFormatXRGB_8888;
			break;
			}
		case EColor4K:
			{
			stride = size.iWidth << 1;
			pixelFormat = EUidPixelFormatXRGB_4444;
			break;
			}
		case EColor64K:
			{
			stride = size.iWidth << 1;
			pixelFormat = EUidPixelFormatRGB_565;
			break;
			}
		case EColor16MA:
			{
			pixelFormat = EUidPixelFormatARGB_8888;
			break;
			}
		case EColor16MAP:
			{
			pixelFormat = EUidPixelFormatARGB_8888_PRE;
			break;
			}
		default:
			{
			LOG(("Unsupported display mode: %d", bmpFormat));
			User::Leave(KErrNotSupported);
			break;
			}
		}

	// Create an intermediary bitmap for decoding into
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(size, info.iFrameDisplayMode));

	// Create the final surface.
	aSurface = CreateSurfaceL(size, pixelFormat, stride);
	TSurfaceCleanup surfaceCleanup(*this, aSurface);
	CleanupClosePushL(surfaceCleanup);

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);

	// Convert the image file into a Symbian bitmap
	TRequestStatus status;
	decoder->Convert(&status, *bitmap);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	// Copy the data from the bitmap into the surface.
	TPoint start;
	for (start.iY = 0; start.iY < size.iHeight; start.iY++)
		{
		// Set up a descriptor for the current line in the surface and get pixels.
		TPtr8 ptr(chunk.Base() + start.iY * stride, stride);
		bitmap->GetScanLine(ptr, start, size.iWidth, bmpFormat);
		}

	CleanupStack::PopAndDestroy(/* chunk */);
	CleanupStack::Pop(/* surfaceCleanup */);
	CleanupStack::PopAndDestroy(bitmap);
	CleanupStack::PopAndDestroy(decoder);
	CleanupStack::PopAndDestroy(/* fs */);
	}

void CSurfaceUtility::CopyBitmapSurfaceL(const CFbsBitmap* aBitmap, TSurfaceId& aSurface)
	{
	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);
	TSize bitmapSize = aBitmap->SizeInPixels();
	TSize size = SurfaceSize(aSurface);
	TInt stride = size.iWidth*4;		// Default to four bytes per pixel

	// Copy the data from the bitmap into the surface.
	TPoint start;
	for (start.iY = 0; start.iY < bitmapSize.iHeight; start.iY++)
		{
		// Set up a descriptor for the current line in the surface and get pixels.
		TPtr8 ptr(chunk.Base() + start.iY * stride, stride);
		aBitmap->GetScanLine(ptr, start, bitmapSize.iWidth, EColor16MU);
		}
	CleanupStack::PopAndDestroy(/* chunk */);

	}
/**
Copy the bitmap from a file to a surface.

@param aFileName The name of the image file.
@param aSurface Filled with the surface ID for the surface containing the pixels.
*/
void CSurfaceUtility::CopyBitmapFromFileToSurfaceL(const TDesC& aFileName, const TSurfaceId& aSurface)
	{
	RFs fs;
	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CImageDecoder* decoder = CImageDecoder::FileNewL(fs, aFileName, CImageDecoder::EOptionAlwaysThread);
	CleanupStack::PushL(decoder);

	const TFrameInfo& info = decoder->FrameInfo();

	TSize size = SurfaceSize(aSurface);
	TDisplayMode bmpFormat = info.iFrameDisplayMode;
	TInt stride = size.iWidth << 2;		// Default to four bytes per pixel

	// Create an intermediary bitmap for decoding into
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(size, info.iFrameDisplayMode));

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);

	// Convert the image file into a Symbian bitmap
	TRequestStatus status;
	decoder->Convert(&status, *bitmap);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	// Copy the data from the bitmap into the surface.
	TPoint start;
	for (start.iY = 0; start.iY < size.iHeight; start.iY++)
		{
		// Set up a descriptor for the current line in the surface and get pixels.
		TPtr8 ptr(chunk.Base() + start.iY * stride, stride);
		bitmap->GetScanLine(ptr, start, size.iWidth, bmpFormat);
		}

	CleanupStack::PopAndDestroy(/* chunk */);
	CleanupStack::PopAndDestroy(bitmap);
	CleanupStack::PopAndDestroy(decoder);
	CleanupStack::PopAndDestroy(/* fs */);
	}

/**
Copy the bitmap from a file to a surface scaling the original image to cover the entire surface.

@param aFileName The name of the image file.
@param aSurface Filled with the surface ID for the surface containing the pixels.
*/
void CSurfaceUtility::ScaleBitmapFromFileToSurfaceL(const TDesC& aFileName, const TSurfaceId& aSurface)
	{

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CImageDecoder* decoder = CImageDecoder::FileNewL(fs, aFileName, CImageDecoder::EOptionAlwaysThread);
	CleanupStack::PushL(decoder);

	const TFrameInfo& info = decoder->FrameInfo();

	TSize size = SurfaceSize(aSurface);
	TInt stride = size.iWidth << 2;		// Default to four bytes per pixel

	// Create an intermediary bitmap for decoding into
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(info.iOverallSizeInPixels, info.iFrameDisplayMode));
	
	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);
	TUint8* surfacePixelData = chunk.Base() + PixelDataOffet(aSurface);
	
	// Convert the image file into a Symbian bitmap
	// CImageDecoder::EOptionAlwaysThread setting above avoids need for Active Object
	TRequestStatus status;
	decoder->Convert(&status, *bitmap);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	// scale to fit surface
	CBitmapScaler* scaler = CBitmapScaler::NewL();
	CleanupStack::PushL(scaler);
	scaler->SetQualityAlgorithm(CBitmapScaler::EMaximumQuality);
	
	CActiveListener* activeListener = CActiveListener::NewLC();
	activeListener->Initialize();
	scaler->Scale(&activeListener->iStatus, *bitmap, size, EFalse);
	CActiveScheduler::Start();
	
	User::LeaveIfError(activeListener->iStatus.Int());

	// Copy the data from the bitmap into the surface.
	TPoint start;
	for (start.iY = 0; start.iY < size.iHeight; start.iY++)
		{
		// Set up a descriptor for the current line in the surface and get pixels.
		TPtr8 ptr(surfacePixelData + start.iY * stride, stride);
		bitmap->GetScanLine(ptr, start, size.iWidth, EColor16MA);
		}
	CleanupStack::PopAndDestroy(activeListener);
	CleanupStack::PopAndDestroy(scaler);
	CleanupStack::PopAndDestroy(/* chunk */);
	CleanupStack::PopAndDestroy(bitmap);
	CleanupStack::PopAndDestroy(decoder);
	CleanupStack::PopAndDestroy(/* fs */);	
	}

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
Get the offset into the chunk of the start of pixel data.

@param aSurface The surface to get the size for.
@return The offset bytes, or empty on failure.
*/
TInt CSurfaceUtility::PixelDataOffet(const TSurfaceId& aSurface)
	{
	TInt offsetToFirstBuffer = 0;
	iManager.GetBufferOffset(aSurface, 0, offsetToFirstBuffer);
	
	return offsetToFirstBuffer;
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
	
	b.iSize.iWidth = aSize.iWidth;
	b.iSize.iHeight = aSize.iHeight;
	b.iBuffers = aBuffers;				// number of buffers in the surface
	b.iPixelFormat = aPixelFormat;
	b.iStride = aStride;		// Number of bytes between start of one line and start of next
	b.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	b.iAlignment = 4;			// alignment, 1,2,4,8 byte aligned
	b.iContiguous=EFalse;

	TSurfaceId surface = TSurfaceId::CreateNullId();

	User::LeaveIfError(iManager.CreateSurface(bf, surface));
	iSurfaces.AppendL(surface);
	return surface;
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

	TUint8* surfacePtr = chunk.Base();
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
		Mem::Move(linePtr, surfacePtr, info.iStride);
		}

	chunk.Close();

	TInt err = iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));
	}

/**
Fill the given memory chunk with a color.

@param aSurface	The surface to be filled.
@param aChunk	The surface to be filled.
@param aColor	The color to fill it with.
*/
void CSurfaceUtility::FillChunkL(TSurfaceId& aSurface, RChunk& aChunk, const TRgb& aColor, TInt aBufferNumber)
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

	User::LeaveIfError(iManager.MapSurface(aSurface, aChunk));

	TInt offsetToFirstBuffer;
	User::LeaveIfError(iManager.GetBufferOffset(aSurface, 0, offsetToFirstBuffer));
	TInt offsetToBufferNumber;
	User::LeaveIfError(iManager.GetBufferOffset(aSurface, aBufferNumber, offsetToBufferNumber));
		
	TUint8* chunkPtr = aChunk.Base() + offsetToFirstBuffer;
	TUint8* linePtr = aChunk.Base() + offsetToBufferNumber;
	TUint8* surfPlanePtr = linePtr;

	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			aChunk.Close();
			User::Leave(KErrOverflow);
			}
		TUint16* ptr = reinterpret_cast<TUint16*>(surfPlanePtr);

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
			aChunk.Close();
			User::Leave(KErrOverflow);
			}
		TUint32* ptr = reinterpret_cast<TUint32*>(surfPlanePtr);

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
		Mem::Copy(linePtr, surfPlanePtr, info.iSize.iWidth * BytesPerPixelL(info.iPixelFormat));
		}

	aChunk.Close();
	}

/**
Fill a rectangle on the given surface.

@param aSurface		The surface to be filled.
@param aStartPos	Where to place the rectangle.
@param aSize		Size of the rectangle.
@param aColor		The colour to fill it with.
*/
void CSurfaceUtility::FillRectangleL(TSurfaceId& aSurface, TPoint& aStartPos, TSize& aSize, const TRgb& aColor)
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

	TUint8* surfacePtr = chunk.Base();
	
	// Check for out of bounds
	TBool validRect = ETrue;
	TInt surfaceWidth = info.iSize.iWidth;
	TInt surfaceHeight = info.iSize.iHeight;
	
	// Width and Height
	if ((aStartPos.iX + aSize.iWidth) > surfaceWidth)
		validRect = EFalse;
	
	if ((aStartPos.iY + aSize.iHeight) > surfaceHeight)
		validRect = EFalse;
	
	// Starting position
	if ((aStartPos.iX < 0) || (aStartPos.iY < 0))
		validRect = EFalse;
	
	if (!validRect)
		User::Leave(KErrOverflow);
		
	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr);
		
		// Fill the rectangle
		TInt yPos = aStartPos.iY;
		TInt xPos = aStartPos.iX;
		for (TInt yy = 0; yy < aSize.iHeight; ++yy)
			{
			ptr = reinterpret_cast<TUint16*>(surfacePtr+(yPos*info.iStride));
			for (TInt xx = 0; xx < aSize.iWidth; ++xx)
				{
				ptr[xPos] = color;
				xPos++;
				}
			xPos = aStartPos.iX;
			yPos++;
			}
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}

		TUint32* ptr = reinterpret_cast<TUint32*>(surfacePtr);		
		
		// Fill the rectangle
		TInt yPos = aStartPos.iY;
		TInt xPos = aStartPos.iX;
		for (TInt yy = 0; yy < aSize.iHeight; ++yy)
			{
			ptr = reinterpret_cast<TUint32*>(surfacePtr+(yPos*info.iStride));
			for (TInt xx = 0; xx < aSize.iWidth; ++xx)
				{
				ptr[xPos] = color;
				xPos++;
				}
			xPos = aStartPos.iX;
			yPos++;
			}
		}

	chunk.Close();

	TInt err = iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));
	}

/**
Fill a rectangle on the given surface - does not submit update.

@param aSurface		The surface to be filled.
@param aStartPos	Where to place the rectangle.
@param aSize		Size of the rectangle.
@param aColor		The colour to fill it with.
*/
void CSurfaceUtility::FillRectangleNoUpdateL(TSurfaceId& aSurface, TPoint& aStartPos, TSize& aSize, const TRgb& aColor)
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

	TUint8* surfacePtr = chunk.Base();
	
	// Check for out of bounds
	TBool validRect = ETrue;
	TInt surfaceWidth = info.iSize.iWidth;
	TInt surfaceHeight = info.iSize.iHeight;
	
	// Width and Height
	if ((aStartPos.iX + aSize.iWidth) > surfaceWidth)
		validRect = EFalse;
	
	if ((aStartPos.iY + aSize.iHeight) > surfaceHeight)
		validRect = EFalse;
	
	// Starting position
	if ((aStartPos.iX < 0) || (aStartPos.iY < 0))
		validRect = EFalse;
	
	if (!validRect)
		User::Leave(KErrOverflow);
	
	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr);
		
		// Fill the rectangle
		TInt yPos = aStartPos.iY;
		TInt xPos = aStartPos.iX;
		for (TInt yy = 0; yy < aSize.iHeight; ++yy)
			{
			ptr = reinterpret_cast<TUint16*>(surfacePtr+(yPos*info.iStride));
			for (TInt xx = 0; xx < aSize.iWidth; ++xx)
				{
				ptr[xPos] = color;
				xPos++;
				}
			xPos = aStartPos.iX;
			yPos++;
			}
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}

		TUint32* ptr = reinterpret_cast<TUint32*>(surfacePtr);		
		
		// Fill the rectangle
		TInt yPos = aStartPos.iY;
		TInt xPos = aStartPos.iX;
		for (TInt yy = 0; yy < aSize.iHeight; ++yy)
			{
			ptr = reinterpret_cast<TUint32*>(surfacePtr+(yPos*info.iStride));
			for (TInt xx = 0; xx < aSize.iWidth; ++xx)
				{
				ptr[xPos] = color;
				xPos++;
				}
			xPos = aStartPos.iX;
			yPos++;
			}
		}

	chunk.Close();

	}

/**
Fill the given surface with a grid over a solid color.

Similar to FillSurfaceL(), but with a grid overlayed. The pitch of the grid is
eight pixels.

@param aSurface	The surface to be filled.
@param aColor	The color to fill it with.
@param aLines	The color of the grid lines.
*/
void CSurfaceUtility::GridFillSurfaceL(TSurfaceId& aSurface, const TRgb& aColor, const TRgb& aLines)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 color = 0;
	TUint32 lines = 0;
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
			lines = aLines.Color16MU();
#ifdef ALPHA_FIX_24BIT
			color |= ((ALPHA_FIX_24BIT)&0xff)<<24;
			lines |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			color = aColor.Color16MA();
			lines = aLines.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			color = aColor.Color16MAP();
			lines = aLines.Color16MAP();
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			color = aColor.Color4K();
			lines = aLines.Color4K();
			use16 = ETrue;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			color = aColor.Color64K();
			lines = aLines.Color64K();
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

	TUint8* surfacePtr = chunk.Base();
	TUint8* linePtr = surfacePtr;

	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr);

		// Fill first line
		for (TInt xx1 = 0; xx1 < info.iSize.iWidth; xx1++)
			{
			ptr[xx1] = (TUint16)lines;
			}

		// Fill second line
		ptr = reinterpret_cast<TUint16*>(surfacePtr + info.iStride);
		for (TInt xx2 = 0; xx2 < info.iSize.iWidth; xx2++)
			{
			// Vertical line every 8 pixels across
			ptr[xx2] = (TUint16)((xx2 & 7) ? color : lines);
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
		for (TInt xx3 = 0; xx3 < info.iSize.iWidth; xx3++)
			{
			ptr[xx3] = lines;
			}

		// Fill second line
		ptr = reinterpret_cast<TUint32*>(surfacePtr + info.iStride);
		for (TInt xx4 = 0; xx4 < info.iSize.iWidth; xx4++)
			{
			// Vertical line every 8 pixels across
			ptr[xx4] = (xx4 & 7) ? color : lines;
			}
		}
	linePtr += info.iStride;

	// Now copy that to the other lines
	for (TInt yy = 2; yy < info.iSize.iHeight; yy++)
		{
		linePtr += info.iStride;
		if (yy & 7)
			{
			// Copy second line
			Mem::Move(linePtr, surfacePtr + info.iStride, info.iStride);
			}
		else
			{
			// Copy first line
			Mem::Move(linePtr, surfacePtr, info.iStride);
			}
		}
	chunk.Close();

	TInt err =iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));

	}


/**
Fill the given surface with a pattern suitable for automated testing.

@param aSurface	The surface to be filled.
*/
void CSurfaceUtility::PatternFillSurfaceL(TSurfaceId& aSurface)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));	
	
	// Fill the background
	FillSurfaceL(aSurface, TRgb(0x00000000));

	TInt surfaceWidth = info.iSize.iWidth;
	TInt surfaceHeight = info.iSize.iHeight;
	
	// Create the 4 rectangles in the corners
	TPoint startPos(0,0);
	TSize size(15,15);
	TInt rectWidth = size.iWidth;
	TInt rectHeight = size.iHeight;
	// Top left
	FillRectangleL(aSurface, startPos, size, TRgb(0x0000ff));
	
	// Top right
	startPos.iX = surfaceWidth - rectWidth;
	startPos.iY = 0;
	FillRectangleL(aSurface, startPos, size, TRgb(0x00ff00));
	
	// Bottom left
	startPos.iX = 0;
	startPos.iY = surfaceHeight - rectHeight;
	FillRectangleL(aSurface, startPos, size, TRgb(0x00ffff));
	
	// Bottom right
	startPos.iX = surfaceWidth - rectWidth;
	startPos.iY = surfaceHeight - rectHeight;
	FillRectangleL(aSurface, startPos, size, TRgb(0xffffff));
	
	// Create the 4 side bars
	startPos.iX = 0;
	startPos.iY = 6;
	size.iWidth = 5;
	size.iHeight = surfaceHeight - 12;
	// Left
	FillRectangleL(aSurface, startPos, size, TRgb(0x808000));
	
	startPos.iX = surfaceWidth - size.iWidth;
	startPos.iY = 6;
	// Right
	FillRectangleL(aSurface, startPos, size, TRgb(0xff00ff));
	
	startPos.iX = 6;
	startPos.iY = surfaceHeight - size.iWidth;
	size.iWidth = surfaceWidth - 12;
	size.iHeight = 5;
	// Top
	FillRectangleL(aSurface, startPos, size, TRgb(0xaaaaaa));
	
	startPos.iX = 6;
	startPos.iY = 0;
	// Bottom
	FillRectangleL(aSurface, startPos, size, TRgb(0x000080));
	}


template <class TIntType> void
DdaLine(TUint aX1, TUint aY1,TUint aX2,TUint aY2, TUint aPixPerScan, TIntType* aBuffer, TIntType aColor)
	{
	TInt dx=aX2-aX1;
	TInt dy=aY2-aY1;
	TInt adx=dx,sdx=1;
	if (adx<0)
		{	adx=-adx; sdx=-1;	}
	TInt ady=dy,sdy=aPixPerScan;
	if (ady<0)
		{	ady=-ady; sdy=-aPixPerScan;	}
	//This is simplistic integert DDA.
	//The vertical cases are handled by this 1/2 accumulator:
	//	If adx is zero then we step in sdy indefinitely
	//  If ady is zero then we step in sdx indefinitely
	TInt accum=adx/2;
	
	TIntType* bufferend=aBuffer+aX2+aY2*aPixPerScan;
	aBuffer+=aX1+aY1*aPixPerScan;
	*aBuffer=aColor;
	while (aBuffer!=bufferend)
		{
		if (accum>0)
			{
			accum-=ady;
			aBuffer+=sdx;
			}
		else
			{
			accum+=adx;
			aBuffer+=sdy;
			}
		*aBuffer=aColor;
		}
	

	}
template <class TIntType> void	
FanFill(const TPoint& aInnerXY,TUint aPixPerScan, TIntType* aSurfacePtr, TIntType aLinesTL, 
			TIntType aLinesBR, TIntType aLinesTR, TIntType aLinesBL)
	{
	
		DdaLine(aInnerXY.iX,0,aInnerXY.iX-aInnerXY.iX*180/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTR);
		DdaLine(aInnerXY.iX,0,aInnerXY.iX-aInnerXY.iX*372/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTR);
		DdaLine(aInnerXY.iX,0,aInnerXY.iX-aInnerXY.iX*591/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTR);
		DdaLine(aInnerXY.iX,0,aInnerXY.iX-aInnerXY.iX*859/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTR);

		DdaLine(aInnerXY.iX,0,0,aInnerXY.iY*180/1024,aPixPerScan,aSurfacePtr,aLinesTR);
		DdaLine(aInnerXY.iX,0,0,aInnerXY.iY*372/1024,aPixPerScan,aSurfacePtr,aLinesTR);
		DdaLine(aInnerXY.iX,0,0,aInnerXY.iY*591/1024,aPixPerScan,aSurfacePtr,aLinesTR);
		DdaLine(aInnerXY.iX,0,0,aInnerXY.iY*859/1024,aPixPerScan,aSurfacePtr,aLinesTR);
		
		DdaLine(0,aInnerXY.iY,aInnerXY.iX*180/1024,0,aPixPerScan,aSurfacePtr,aLinesBL);
		DdaLine(0,aInnerXY.iY,aInnerXY.iX*372/1024,0,aPixPerScan,aSurfacePtr,aLinesBL);
		DdaLine(0,aInnerXY.iY,aInnerXY.iX*591/1024,0,aPixPerScan,aSurfacePtr,aLinesBL);
		DdaLine(0,aInnerXY.iY,aInnerXY.iX*859/1024,0,aPixPerScan,aSurfacePtr,aLinesBL);

		DdaLine(0,aInnerXY.iY,aInnerXY.iX,aInnerXY.iY-aInnerXY.iY*180/1024,aPixPerScan,aSurfacePtr,aLinesBL);
		DdaLine(0,aInnerXY.iY,aInnerXY.iX,aInnerXY.iY-aInnerXY.iY*372/1024,aPixPerScan,aSurfacePtr,aLinesBL);
		DdaLine(0,aInnerXY.iY,aInnerXY.iX,aInnerXY.iY-aInnerXY.iY*591/1024,aPixPerScan,aSurfacePtr,aLinesBL);
		DdaLine(0,aInnerXY.iY,aInnerXY.iX,aInnerXY.iY-aInnerXY.iY*859/1024,aPixPerScan,aSurfacePtr,aLinesBL);
		
		DdaLine(0,0,aInnerXY.iX*180/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTL);
		DdaLine(0,0,aInnerXY.iX*372/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTL);
		DdaLine(0,0,aInnerXY.iX*591/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTL);
		DdaLine(0,0,aInnerXY.iX*859/1024,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesTL);

		DdaLine(0,0,aInnerXY.iX,aInnerXY.iY*180/1024,aPixPerScan,aSurfacePtr,aLinesTL);
		DdaLine(0,0,aInnerXY.iX,aInnerXY.iY*372/1024,aPixPerScan,aSurfacePtr,aLinesTL);
		DdaLine(0,0,aInnerXY.iX,aInnerXY.iY*591/1024,aPixPerScan,aSurfacePtr,aLinesTL);
		DdaLine(0,0,aInnerXY.iX,aInnerXY.iY*859/1024,aPixPerScan,aSurfacePtr,aLinesTL);
		
		DdaLine(0,aInnerXY.iY-aInnerXY.iY*180/1024,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
		DdaLine(0,aInnerXY.iY-aInnerXY.iY*372/1024,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
		DdaLine(0,aInnerXY.iY-aInnerXY.iY*591/1024,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
		DdaLine(0,aInnerXY.iY-aInnerXY.iY*859/1024,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);

		DdaLine(aInnerXY.iX-aInnerXY.iX*180/1024,0,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
		DdaLine(aInnerXY.iX-aInnerXY.iX*372/1024,0,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
		DdaLine(aInnerXY.iX-aInnerXY.iX*591/1024,0,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
		DdaLine(aInnerXY.iX-aInnerXY.iX*859/1024,0,aInnerXY.iX,aInnerXY.iY,aPixPerScan,aSurfacePtr,aLinesBR);
	
	}
/**
Fill the given surface with a fan of lines over a solid color.

Similar to FillSurfaceL(), but with a fan of lines overlayed. 
One fan is drawn about the top-left, and second fan at bottom-right.
The fan contains 8 segments.

@param aSurface	The surface to be filled.
@param aColor	The color to fill it with.
@param aLines	The color of the grid lines.
*/
void CSurfaceUtility::FanFillSurfaceL(TSurfaceId& aSurface, const TRgb& aColor, const TRgb& aLinesTL, const TRgb& aLinesBR)
	{
	FillSurfaceL(aSurface,aColor);
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 linesTL = 0;
	TUint32 linesBR = 0;
	TUint32 linesTR = 0;
	TUint32 linesBL = 0;
	TBool use16 = EFalse;
	TRgb	rgbLinesTR(0,0,0);
	TRgb	rgbLinesBL(255,255,255);

	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			linesBR = aLinesBR.Color16MU();
			linesTL = aLinesTL.Color16MU();
			linesTR = rgbLinesTR.Color16MU();
			linesBL = rgbLinesBL.Color16MU();
#ifdef ALPHA_FIX_24BIT
			linesBR |= ((ALPHA_FIX_24BIT)&0xff)<<24;
			linesTL |= ((ALPHA_FIX_24BIT)&0xff)<<24;
			linesTR |= ((ALPHA_FIX_24BIT)&0xff)<<24;
			linesBL |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			linesBR = aLinesBR.Color16MA();
			linesTL = aLinesTL.Color16MA();
			linesTR = rgbLinesTR.Color16MA();
			linesBL = rgbLinesBL.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			linesBR = aLinesBR.Color16MAP();
			linesTL = aLinesTL.Color16MAP();
			linesTR = rgbLinesTR.Color16MAP();
			linesBL = rgbLinesBL.Color16MAP();
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			linesBR = aLinesBR.Color4K();
			linesTL = aLinesTL.Color4K();
			linesTR = rgbLinesTR.Color4K();
			linesBL = rgbLinesBL.Color4K();
			use16 = ETrue;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			linesBR = aLinesBR.Color64K();
			linesTL = aLinesTL.Color64K();
			linesTR = rgbLinesTR.Color64K();
			linesBL = rgbLinesBL.Color64K();
			use16 = ETrue;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}
	if (info.iSize.iHeight<0 || info.iSize.iWidth<0 || info.iStride<0)
		{
		User::Leave(KErrCorrupt);
		}
	if (info.iSize.iHeight==0 || info.iSize.iWidth==0 || info.iStride==0)
		{
		User::Leave(KErrNotReady);
		}
	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	TUint8* surfacePtr = chunk.Base();
	TPoint innerXY(info.iSize.iWidth-1,info.iSize.iHeight-1);
	if (use16)
		{
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		FanFill<TUint16>(innerXY,info.iStride/2,(TUint16*)surfacePtr,linesTL,linesBR,linesBL,linesTR);
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		FanFill<TUint>(innerXY,info.iStride/4,(TUint*)surfacePtr,linesTL,linesBR,linesBL,linesTR);
		}
	
	chunk.Close();

	iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	}
/**
Fill the given surface with vertical line at the given position

Similar to FillSurfaceL(), but with a vertical line overlayed. 
The position along the surface is given as a percentage from the left

@param aSurface	The surface to be filled.
@param aColor	The color to fill it with.
@param aLine	The color of the line.
@param aPosition Position of the vertical line given as a percentage across the surface from the left edge
*/
void CSurfaceUtility::LineFillSurfaceL(TSurfaceId& aSurface, const TRgb& aBackColor, const TRgb& aLineColor, TInt aPosition)
	{
	if (aPosition<0 || aPosition>100)
		{
		aPosition=0;
		}
	FillSurfaceL(aSurface,aBackColor);
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	
	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 lineColor = 0;
	TBool use16 = EFalse;
	
	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			lineColor = aLineColor.Color16MU();
#ifdef ALPHA_FIX_24BIT
			lineColor |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			lineColor = aLineColor.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			lineColor = aLineColor.Color16MAP();
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			lineColor = aLineColor.Color4K();
			use16 = ETrue;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			lineColor = aLineColor.Color64K();
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
	
	TUint8* surfacePtr = chunk.Base();
	if (use16)
		{
		DdaLine<TUint16>((info.iSize.iWidth*aPosition)/100,0,(info.iSize.iWidth*aPosition)/100,
				info.iSize.iHeight-1,info.iStride/2,(TUint16*)surfacePtr,lineColor);
		}
	else
		{
		DdaLine<TUint>((info.iSize.iWidth*aPosition)/100,0,(info.iSize.iWidth*aPosition)/100,
				info.iSize.iHeight-1,info.iStride/4,(TUint*)surfacePtr,lineColor);
		}
	chunk.Close();
	iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	}
/**
 * Generates a bitmap equivalent to the surface.
 * Can reuse an existing bitmap or create a new bitmap.
 * The existing bitmap must be an exact match (eg previously generated by this method)
 **/
CFbsBitmap* CSurfaceUtility::EquivalentBitmapL(TSurfaceId& aSurface,CFbsBitmap* aCopyToMayBeNull)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();

	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TInt bytesPerPixel=0;
	TDisplayMode	bitmapMode = ENone;
	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			bitmapMode = EColor16MU;
			bytesPerPixel = 4;
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			bitmapMode=EColor16MA;
			bytesPerPixel = 4;
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			bitmapMode=EColor16MAP;
			bytesPerPixel = 4;
			break;
			}
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			{
			bitmapMode=EColor4K;
			bytesPerPixel = 2;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			bitmapMode=EColor64K;
			bytesPerPixel = 2;
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}
	CFbsBitmap* retVal=NULL;
	if (aCopyToMayBeNull)
		{
		retVal=aCopyToMayBeNull;
		if (retVal->SizeInPixels()!=info.iSize)
			User::Leave(KErrCorrupt);
		if (retVal->DisplayMode()!=bitmapMode)
			User::Leave(KErrCorrupt);
		}
	else
		{
		retVal=new CFbsBitmap;
		CleanupStack::PushL(retVal);
		User::LeaveIfError(retVal->Create(info.iSize,bitmapMode));
		}
	RChunk chunk;
	CleanupClosePushL(chunk);
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	TUint8* surfacePtr = chunk.Base();
	TUint8* bitmapPtr = (TUint8*)retVal->DataAddress();
	TInt copyBytes=info.iSize.iWidth*bytesPerPixel;
	for (TInt y=0;y<info.iSize.iHeight;y++)
		{
		Mem::Copy(bitmapPtr,surfacePtr,copyBytes);
		surfacePtr+=info.iStride;
		bitmapPtr+=retVal->DataStride();
		}
	CleanupStack::PopAndDestroy(&chunk);
	if (!aCopyToMayBeNull)
		CleanupStack::Pop(retVal);
	return retVal;
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
@param aRegion	The area of the surface affected, or NULL for all of it.*/
void CSurfaceUtility::SubmitUpdate(TInt /* aScreenNumber */, const TSurfaceId& aSurface, const TRegion* aRegion,TInt aBufferNumber)
	{
	TInt err =iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, aBufferNumber, aRegion); 
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));
	}

/**
Map and submit an update to a surface to the update server.

@param aChunk	The chunk of memory to be mapped
@param aScreenNumber	The screen to be updated where the surface is shown.
@param aSurface	The surface which has been updated.
@param aRegion	The area of the surface affected, or NULL for all of it.*/
void CSurfaceUtility::MapAndSubmitUpdateL(RChunk& aChunk, 
		                                TInt /* aScreenNumber */, 
		                                const TSurfaceId& aSurface, 
		                                const TRegion* aRegion)
	{
	User::LeaveIfError(iManager.MapSurface(aSurface, aChunk));
	aChunk.Close();
	TInt err =iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, aRegion); 
	if (err!=KErrNone)
		LOG(("Error submitting update: 0x%X\n", err));
	}

void CSurfaceUtility::MapSurfaceL(const TSurfaceId& aSurface, RChunk& aChunk)
	{
	User::LeaveIfError(iManager.MapSurface(aSurface, aChunk));
	}

void CSurfaceUtility::CopyBitmapToSurfaceL(TSurfaceId& aSurface, const CFbsBitmap& aBitmap)
	{
	TSize size = SurfaceSize(aSurface);
	
	TDisplayMode bmpFormat = aBitmap.DisplayMode();
	TInt stride = size.iWidth * 4;		// Default to four bytes per pixel

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);

	// Copy the data from the bitmap into the surface.
	TPoint start;
	for (start.iY = 0; start.iY < size.iHeight; start.iY++)
		{
		// Set up a descriptor for the current line in the surface and get pixels.
		TPtr8 ptr(chunk.Base() + start.iY * stride, stride);
		aBitmap.GetScanLine(ptr, start, size.iWidth, bmpFormat);
		}

	TInt err =iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurface, 0, NULL);
	if (err!=KErrNone)
		{
		LOG(("Error submitting update: 0x%X\n", err));
		}

	CleanupStack::PopAndDestroy(/* chunk */);
	}

/**
CActiveListener factory function
@return A CActiveListener object
*/
CActiveListener* CActiveListener::NewLC()
	{
    CActiveListener* self = new(ELeave) CActiveListener();
	CleanupStack::PushL(self);
	return self;
	}

/**
Constructor for class CActiveListener
*/
CActiveListener::CActiveListener() : CActive(EPriorityLow)
	{
 	CActiveScheduler::Add(this);
	}

/**
Destructor
*/
CActiveListener::~CActiveListener()
	{
	}

/**
Handles the request.
This function is derived from CActive
*/
void CActiveListener::RunL()
	{
	CActiveScheduler::Stop();
	}

/**
Cancels the outstanding request.
This function is derived from CActive
*/
void CActiveListener::DoCancel()
	{
	}

/**
Initializes the CActiveListener
*/
void CActiveListener::Initialize()
	{
	iStatus = KRequestPending;
	SetActive();
	}

/**
Check that the request has been cancelled.
@return A boolean indicating whether the request has been cancelled or not
*/
TBool CActiveListener::IsRequestCancelled()
	{
	return (iStatus == KErrCancel);
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

