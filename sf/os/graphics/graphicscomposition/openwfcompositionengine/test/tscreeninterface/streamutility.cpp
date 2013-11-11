// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
//

/**
 @file
*/

#include <e32std.h>
#include <imageconversion.h>
#include <graphics/symbianstream.h>
#include "streamutility.h"

#define WFC_INVALID_HANDLE NULL

CStreamUtility::CStreamUtility()
	{
	}
	
CStreamUtility* CStreamUtility::NewL()
	{
	CStreamUtility* utility = new (ELeave)CStreamUtility();
	CleanupStack::PushL(utility);
	utility->ConstructL();
	CleanupStack::Pop(utility);
	return utility;
	}
	
void CStreamUtility::ConstructL()
	{
	TInt r = iManager.Open();
	if (r != KErrNone)
		{
		LOG(("Surface manager failed to open: %d", r));
		User::Leave(r);
		}
	}
	
CStreamUtility::~CStreamUtility()
	{
	DestroyAll();

	iStreams.Close();

	iManager.Close();
	}

TBool CStreamUtility::DestroyAll()
	{
	TInt err = 	KErrNone;
	TInt jj = iStreams.Count() - 1;
	if (jj<0)
		return EFalse;
	for (; jj >= 0; jj--)
		{
        //The following lines are just to get the surface ID for verification
		SymbianStreamBuffer bufferHandle;
		User::LeaveIfError(SymbianStreamAcquireReadBuffer(iStreams[jj], &bufferHandle));
		long bufferIndex;
		const TSurfaceId* pSurfaceId = NULL;
		
		User::LeaveIfError(SymbianStreamGetBufferId(iStreams[jj],bufferHandle,&bufferIndex,&pSurfaceId));
        User::LeaveIfError(SymbianStreamReleaseReadBuffer(iStreams[jj], bufferHandle));
        
        const TSurfaceId surfaceId = *pSurfaceId;   //Need to copy my reference to the ID.
        //Actually release the stream
        SymbianStreamRemoveReference(iStreams[jj]);
        
        //Verify the stream is now not accessible
        TInt offset;
		err = iManager.GetBufferOffset(surfaceId,0,offset);
		if (err==KErrNone)
			{
			LOG(("Closing stream via DestoryAll did not destroy surface!"));
			}
		}
	iStreams.Reset();
	return ETrue;
	}

/***************************************
 * The aim of the RHeapStreamArray is to locally switch in the specified heap for any array operation
 ***************************************/

CStreamUtility::RHeapStreamArray::RHeapStreamArray(RHeapStreamArray* aUseExternalArray)
	:	iUseArray(aUseExternalArray?aUseExternalArray->iUseArray:&this->iLocalArray),
	iExternalHeapRef(aUseExternalArray?aUseExternalArray->iExternalHeapRef:User::Heap())
	{
	
	}
/************************************
 * The following methods have been used by the streamutility... some require the heap wrapping, and some don't
 * I actually need three different strategies (count em) for 7 methods...
 * Some methods only read the existing objects, so don't need a heap swap at all
 * Leaving methods have to use PopAndDestroy strategy to restore the heap on leaving or success
 * Non-leaving methods must not call PushL, so directly make SwitchHeap calls!
 ************************************/

/// PopAndDestroy method to restore the heap
/*static*/ void	CStreamUtility::RHeapStreamArray::PopHeap(void* aHeapPtr)
	{
	RHeap* heapPtr=(RHeap*)aHeapPtr;
	User::SwitchHeap(heapPtr);
	}

SymbianStreamType& CStreamUtility::RHeapStreamArray::operator[](TUint aIndex)
	{
	return iUseArray->operator[](aIndex);
	}

/// Close only closes the local array, while Reset resets the active array (may be external)
void CStreamUtility::RHeapStreamArray::Close()
	{
	iLocalArray.Close();
	}

TInt CStreamUtility::RHeapStreamArray::Count() const
	{
	return iUseArray->Count();
	}

/// Close only closes the local array, while Reset resets the active array (may be external)
inline void CStreamUtility::RHeapStreamArray::Reset()
	{
	iUseArray->Reset();
	}

void CStreamUtility::RHeapStreamArray::AppendL(const SymbianStreamType &anEntry)
	{
	iUseArray->AppendL(anEntry);
	}

TInt CStreamUtility::RHeapStreamArray::Find(const SymbianStreamType &anEntry) const
	{
	return iUseArray->Find(anEntry);
	}

void CStreamUtility::RHeapStreamArray::Remove(TInt anIndex)
	{
	iUseArray->Remove(anIndex);
	}

/**
Cleanup stack helper object, holding references to both utility and stream, so
that the standard Close() semantics can be used.
*/
class TStreamCleanup
	{
public:
	TStreamCleanup(CStreamUtility& aUtility, SymbianStreamType& aStream)
		: iUtility(aUtility), iStream(aStream)
		{}
	void Close()
		{
		// Removes the stream from the list of streams to clean up, and closes
		// the stream reference.
		iUtility.DestroyStream(iStream);
		}
private:
	CStreamUtility& iUtility;
	SymbianStreamType& iStream;
	};

/**
Get the size of a stream.

@param  aStream The stream to get the size for.
@return The size in pixels, or empty on failure.
*/
TSize CStreamUtility::StreamSize(const SymbianStreamType aStream)
	{
	khronos_int32_t width;
	khronos_int32_t height;
	khronos_int32_t stride;
	khronos_int32_t format;
	khronos_int32_t pixelSize;
		
	SymbianStreamGetHeader(aStream, &width, &height, &stride, &format, &pixelSize);
	
	TSize size = TSize(static_cast<TInt>(width), static_cast<TInt>(height));
	
	return size;
	}

/**
Create a stream using the surface manager.

Stores the ID for tear down, as well as returning it.

@param aSize            Dimensions of the stream.
@param aPixelFormat	    UID of the pixel format.
@param aStride	        Stride value for the stream (usually bytes per pixel * width)
@param aReturnSurface   Returns TSurfaceId wrapped by the stream
@param aContiguous      Contiguous flag for creating surfaces
@param aBuffers         Number of buffers
@leave May leave due to lack of memory.
@return New stream's ID.
*/
SymbianStreamType CStreamUtility::CreateStreamL(const TSize& aSize, TUidPixelFormat aPixelFormat, 
                                            TInt aStride, TSurfaceId& aReturnSurface, 
                                            TBool aContiguous, TInt aBuffers)
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
	b.iContiguous = !!aContiguous;
	b.iMappable = ETrue;

	aReturnSurface = TSurfaceId::CreateNullId();

	User::LeaveIfError(iManager.CreateSurface(bf, aReturnSurface));
	
	SymbianStreamType ns;
	TInt err = SymbianStreamAcquire(&aReturnSurface, &ns);
    iManager.CloseSurface(aReturnSurface);		//The stream should now own the only reference
	if (err != KErrNone)
		{
		User::Leave(err);	
		}
	iStreams.AppendL(ns);
	return ns;
	}

/**
Fill the given stream with a color.

@param aStream	The stream to be filled.
@param aColor	The color to fill it with.
*/
void CStreamUtility::FillStreamL(SymbianStreamType aStream, const TRgb& aColor)
	{
	
	khronos_int32_t width;
	khronos_int32_t height;
	khronos_int32_t stride;
	khronos_int32_t format;
	khronos_int32_t pixelSize;
		
	SymbianStreamGetHeader(aStream, &width, &height, &stride, &format, &pixelSize);

	TUint32 color = 0;
	TBool use16 = EFalse;

	if (height<0 || width<0 || stride<0)
		{
		User::Leave(KErrCorrupt);
		}
	if (height==0 || width==0 || stride==0)
		{
		User::Leave(KErrNotReady);
		}

	switch (format)
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

    SymbianStreamBuffer bufferHandle;
    User::LeaveIfError(SymbianStreamAcquireWriteBuffer(aStream, &bufferHandle));
    long bufferIndex;
    const TSurfaceId* surfaceId = NULL;
    User::LeaveIfError(SymbianStreamGetBufferId(aStream,bufferHandle,&bufferIndex,&surfaceId));

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(*surfaceId, chunk));
	CleanupClosePushL(chunk);

	TInt offsetToFirstBuffer;
	User::LeaveIfError(iManager.GetBufferOffset(*surfaceId, 0, offsetToFirstBuffer));
	TUint8* streamPtr = chunk.Base() + offsetToFirstBuffer;
	TUint8* linePtr = streamPtr;

	if (use16)
		{
		if ( width*2>stride)
			{
			User::Leave(KErrOverflow);
			}
		TUint16* ptr = reinterpret_cast<TUint16*>(streamPtr);

		// Fill first line
		for (TInt xx = 0; xx < width; xx++)
			{
			ptr[xx] = (TUint16)color;
			}
		}
	else
		{
		if ( width*4>stride)
			{
			User::Leave(KErrOverflow);
			}
		TUint32* ptr = reinterpret_cast<TUint32*>(streamPtr);

		// Fill first line
		for (TInt xx = 0; xx < width; xx++)
			{
			ptr[xx] = color;
			}
		}

	// Now copy that to the other lines
	for (TInt yy = 1; yy < height; yy++)
		{
		linePtr += stride;
		Mem::Copy(linePtr, streamPtr, width * BytesPerPixelL(aStream));
		}
	User::LeaveIfError(SymbianStreamReleaseWriteBuffer(aStream, bufferHandle));
	CleanupStack::PopAndDestroy(/* chunk */);
	}

/**
Destroy a stream.

As well as destroying the stream, it is removed from the set held for
destruction during tear down.

@param aStream	The stream to be destroyed.
*/
void CStreamUtility::DestroyStream(SymbianStreamType aStream)
	{
	TInt index = iStreams.Find(aStream);
	
	if (index != KErrNotFound)
		{
		iStreams.Remove(index);
		}

	SymbianStreamRemoveReference(aStream);
	}

/**
A helper function that returns the bytes per pixel for a given pixel format uid

@param      aPixelFormat Pixel format UID to convert
@return     The bytes per pixel
*/
TInt CStreamUtility::BytesPerPixelL(TUidPixelFormat aPixelFormat)
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
A helper function that returns the bytes per pixel for a given stream

@param      aStream The stream which is checked
@return     The bytes per pixel
*/
TInt CStreamUtility::BytesPerPixelL(const SymbianStreamType aStream)
    {
    khronos_int32_t width;
    khronos_int32_t height;
    khronos_int32_t stride;
    khronos_int32_t format;
    khronos_int32_t pixelSize;
        
    SymbianStreamGetHeader(aStream, &width, &height, &stride, &format, &pixelSize);

    return static_cast<TInt>(pixelSize);
    }

