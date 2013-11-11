// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <stdlib.h>
#include <s32file.h> 
#include "surfaceutility.h"

#ifdef __cplusplus
extern "C" {
#endif

WFC_API_CALL WFCNativeStreamType WFC_APIENTRY
extwfcGetOnScreenStream(WFCDevice dev, WFCContext context) WFC_APIEXIT;

#ifdef __cplusplus
}
#endif

CSurfaceUtility::CSurfaceUtility()
: iSurfaces(NULL)
	{
	}
	
CSurfaceUtility* CSurfaceUtility::NewL()
	{
	CSurfaceUtility* utility = new (ELeave)CSurfaceUtility();
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
		TInt offset;
		err = iManager.GetBufferOffset(iSurfaces[jj],0,offset);
		if (err==KErrNone)
		    {
            LOG(("Error: closed surface still accessible: index %i surface %08X %08X %08X %08X\n", jj, iSurfaces[jj]));
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
 * Three strategies are needed for 7 methods...
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
@param aContiguous  Contiguous flag for creating surfaces
@param aBuffers Number of buffers in the surface
@leave May leave due to lack of memory.
@return New surface's ID.
*/
TSurfaceId CSurfaceUtility::CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride, TBool aContiguous, TInt aBuffers)
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
	b.iContiguous = aContiguous;
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
Fill buffer 0 of the given surface with a color.

@param aSurface	The surface to be filled.
@param aColor	The color to fill it with.
*/
void CSurfaceUtility::FillSurfaceL(TSurfaceId& aSurface, const TRgb& aColor)
	{
    FillSurfaceL(aSurface, 0, aColor);
	}

/**
Fill a specified buffer number of the given surface with a color.

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
Get surface header information

@param aSurface A surface to get the header info from.
@param aInfo Returned package info of the surface header.

@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
 */
TInt CSurfaceUtility::GetHeader(const TSurfaceId& aSurface, RSurfaceManager::TInfoBuf& aInfo) 
    {
    return iManager.SurfaceInfo(aSurface, aInfo);
    }

/**
Get buffer pointer to a surface

@param aSurface Surface of the buffer pointer.
@param aNumOfBuffer A number of buffer.
@param aChunk A chunk of memory.

@return A buffer pointer of the surface.
 */
TUint8* CSurfaceUtility::GetBufferPointerL(const TSurfaceId& aSurface, TInt aNumOfBuffer, RChunk& aChunk) 
    {
    TInt offsetToBuffer;
    User::LeaveIfError(iManager.MapSurface(aSurface, aChunk));
    User::LeaveIfError(iManager.GetBufferOffset(aSurface, aNumOfBuffer, offsetToBuffer));
    TUint8* surfacePtr = aChunk.Base() + offsetToBuffer;
    return surfacePtr;
    }

/**
Get pixel color at a position.

@param aInfo Package info of a surface.
@param aPixelData Surface buffer pointer.
@param aPosition Position of the pixel.

@return Color of the pixel position.
*/
TRgb CSurfaceUtility::GetPixelL(RSurfaceManager::TInfoBuf& aInfo, TAny* aPixelData, TPoint aPosition) 
    {
    RSurfaceManager::TSurfaceInfoV01& info = aInfo();
    TInt stride = info.iStride;
    TUidPixelFormat pixelFormat = info.iPixelFormat;
    TInt bytesPerPixel = BytesPerPixelL(pixelFormat);
    TInt pixelStride = stride / bytesPerPixel;
    TUint pixel = aPosition.iY * pixelStride + aPosition.iX;
    TUint* pixels = reinterpret_cast< TUint* >( aPixelData );
    TRgb colour;
    colour.SetInternal(pixels[ pixel ]);
    return colour;
    }

/**
Check pixel color within a rectangle is as expected.
It checks every color channel of every pixel within the rectangle.

@param aSurface The surface to be checked.
@param aRect The rectangle for pixel checking.
@param aNumOfBuffer Number of buffer.
@param aExpectedColor The expected color.
@param aTolerance A tolerance value.

@return EFalse if a color channel of a pixel is outside the tolerance range. 
        ETrue if all pixel colors are within the tolerance range.
*/
TBool CSurfaceUtility::CheckRectColor(const TSurfaceId& aSurface, TRect& aRect, TInt aNumOfBuffer, const TRgb& aExpectedColor, TInt aTolerance) 
    {
    RSurfaceManager::TInfoBuf infoBuf;
    RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
    TInt error = GetHeader(aSurface, infoBuf);
    if (error != KErrNone)
    	{
		RDebug::Printf("Line %d GetHeader failed",__LINE__);
		return EFalse;
    	}

    RChunk chunk;
    TUint8* surfacePtr = NULL;
    TRAP_IGNORE(surfacePtr = GetBufferPointerL(aSurface, aNumOfBuffer, chunk));
    if (!surfacePtr)
    	{
		RDebug::Printf("Line %d GetBufferPointerL failed",__LINE__);
		return EFalse;
    	}
    
    TRgb color;
    // Check every colour channel of every pixel is within the tolerance
    for (TInt ii = aRect.iTl.iX; ii < aRect.iBr.iX; ++ii)
        {
        for (TInt jj = aRect.iTl.iY; jj < aRect.iBr.iY; ++jj)
            {
            color = GetPixelL(infoBuf, surfacePtr, TPoint(ii, jj));      
            TBool checkR = (color.Red() <= (aExpectedColor.Red() + aTolerance) && color.Red() >= (aExpectedColor.Red() - aTolerance));
            TBool checkG = (color.Green() <= (aExpectedColor.Green() + aTolerance) && color.Green() >= (aExpectedColor.Green() - aTolerance));
            TBool checkB = (color.Blue() <= (aExpectedColor.Blue() + aTolerance) && color.Blue() >= (aExpectedColor.Blue() - aTolerance));
            if (!checkR || !checkG || !checkB)
                {
                RDebug::Printf("At x=%d y=%d CheckRectColor has failed:",ii,jj);
                RDebug::Printf("Expected Red %d - Actual Red %d",aExpectedColor.Red(),color.Red());
                RDebug::Printf("Expected Green %d - Actual Green %d",aExpectedColor.Green(),color.Green());
                RDebug::Printf("Expected Blue %d - Actual Blue %d",aExpectedColor.Blue(),color.Blue());
                return EFalse;            
                }               
            }
        }
    return ETrue;
    }

/**
Save on screen image to a .tga file

@param aSurface A surface to be saved
@param aBufferNumber The surface's buffer number
@param aDestination The path and name of the tga to save eg c:\\test\\img\\image1.tga
@return ETrue on successful calls
        Fails if GetBufferPointerL returns NULL pointer
 */
TBool CSurfaceUtility::SaveResultImageTGAL(const TSurfaceId& aSurface, TInt aBufferNumber, TDesC& aDestination)
    {
    RSurfaceManager::TInfoBuf infoBuf;
    RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
    User::LeaveIfError(GetHeader(aSurface, infoBuf));
    TInt stride = info.iStride;
    TSize surfaceSize = info.iSize;
    TUidPixelFormat pixelFormat = info.iPixelFormat;
    TInt bytesPerPixel = BytesPerPixelL(pixelFormat);
    TInt widthInBytes = surfaceSize.iWidth * bytesPerPixel;
    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs); 
        
    // Create image file
    RFileWriteStream fstream;
    User::LeaveIfError(fstream.Replace(fs, aDestination, EFileShareAny|EFileWrite));
    CleanupClosePushL(fstream); 
    
    // Write header
    fstream.WriteUint8L(0);                 // ID Length
    fstream.WriteUint8L(0);                 // Color map type
    fstream.WriteUint8L(2);                 // Image type - Uncompressed, True-color Image
    fstream.WriteUint32L(0);                // Color map specification 5 bytes
    fstream.WriteUint8L(0);                 // Color map specification
    fstream.WriteUint32L(0);                // Image specification - origin of image
    fstream.WriteUint16L(static_cast<TUint16>(surfaceSize.iWidth));      // Image specification - Image width
    fstream.WriteUint16L(static_cast<TUint16>(surfaceSize.iHeight));     // Image specification - Image height
    fstream.WriteUint8L(32);                // Image specification - Pixel Depth (bits per pixel)
    fstream.WriteUint8L(1 << 5);            // Image specification - Image Descriptor, Screen destination of first pixel is top left 

    RChunk chunk;
    TUint8* surfacePtr = GetBufferPointerL(aSurface, aBufferNumber, chunk);
    if(surfacePtr == NULL)
        {
        CleanupStack::PopAndDestroy(2); 
        return EFalse;
        }
    
    // Write image line by line
    for(TInt ii = 0; ii < surfaceSize.iHeight; ++ii)
        {
        fstream.WriteL(surfacePtr, widthInBytes); 
        surfacePtr += stride;
        }

    fstream.CommitL();
    chunk.Close();
    CleanupStack::PopAndDestroy(2); 
    
    return ETrue;
    }

/**
Create directory for images to be saved

@param aDir Directory for images to be saved
@return ETrue on success
 */
TBool CSurfaceUtility::CreateImagePath(TDesC& aDir)
    {
    RFs fs;
    TInt err = fs.Connect();
    if (err == KErrNone)
        {
        err = fs.MkDirAll(aDir);
        if (err == KErrAlreadyExists)
            {
            err = KErrNone;
            }
        fs.Close();
        }
    return (err == KErrNone);
    }
    
/**
Submit an update to a surface to the update server.

@param aSurface	The surface which has been updated.
@param aRegion	The area of the surface affected, or NULL for all of it.
*/
TInt CSurfaceUtility::SubmitUpdate(const TSurfaceId& aSurface,TInt aBufferNumber, const TRegion* aRegion)
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

SymbianStreamType CSurfaceUtility::GetOnScreenStream(WFCDevice aDev, WFCContext aContext)
    {
    return reinterpret_cast<SymbianStreamType>(wfcGetOnScreenStream(aDev, aContext));
    }



