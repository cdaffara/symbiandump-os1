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
// SurfaceStream.cpp
// CSurfaceStream implementation

#include <e32base.h>
#include <graphics/suerror.h>
#include <graphics/surfacetypes.h>
#include <hal.h>
#include "surfacestream.h"
#include "openwfcpanic.h"
#include "streammap.h"
#include "symbianstream.h"
#define WFC_INVALID_HANDLE NULL

const TInt KInvalidIndex = KNoAssociatedScreenNumber;

struct CSurfaceStream::TCallBackEntry
	{
	typedef SymbianStreamCallback	TSimpleCallback;
	TCallBackEntry();
	void Reset();
	TSimpleCallback iCallBackFunction;
	TAny*			iCallBackClientParam;
	TInt32			iEventMask;
	TInt32          iScreenNumber;
	};

CSurfaceStream::TCallBackEntry::TCallBackEntry():
iCallBackFunction(NULL),
iCallBackClientParam(NULL),
iEventMask(ESOWF_NoEvent),
iScreenNumber(KNoAssociatedScreenNumber)
    {
    }

void CSurfaceStream::TCallBackEntry::Reset()
    {
    iCallBackFunction = NULL;
    iCallBackClientParam = NULL;
    iEventMask = ESOWF_NoEvent;
    iScreenNumber = KNoAssociatedScreenNumber;
    }

struct CSurfaceStream::TGlobalNotification
    {
    TGlobalNotification();
    void Reset();
    TRequestStatus* iStatus;
    TThreadId       iThreadId;
    TInt            iPendingNotifications;
    TInt            iCompletedOkNotifications;
    TInt            iCanceledNotifications;
    TInt            iOverflowedNotifications;
    TInt            iNotVisibleNotifications;
    TInt            iOtherNotifications;
    };


CSurfaceStream::TGlobalNotification::TGlobalNotification():
iStatus(NULL),
iThreadId(0),
iPendingNotifications(0),
iCompletedOkNotifications(0),
iCanceledNotifications(0),
iOverflowedNotifications(0),
iNotVisibleNotifications(0),
iOtherNotifications(0)
    {
    
    }

void CSurfaceStream::TGlobalNotification::Reset()
    {
    iStatus = NULL;
    iThreadId = 0;
    iPendingNotifications = 0;
    iCompletedOkNotifications = 0;
    iCanceledNotifications = 0;
    iOverflowedNotifications = 0;
    iNotVisibleNotifications = 0;
    iOtherNotifications = 0;
    }

struct CSurfaceStream::TNewGlobalNotifications
    {
    TNewGlobalNotifications();
    TInt iNewAvailableIdx;
    TInt iNewDisplayedIdx;
    TInt iNewDisplayedXIdx;
    };

CSurfaceStream::TNewGlobalNotifications::TNewGlobalNotifications():
iNewAvailableIdx(KInvalidIndex),
iNewDisplayedIdx(KInvalidIndex),
iNewDisplayedXIdx(KInvalidIndex)
    {
    }

CSurfaceStream::Guard::Guard(RFastLock& aLock):
iLock(aLock)
    {
    iLock.Wait();
    }

CSurfaceStream::Guard::~Guard()
    {
    iLock.Signal();
    }

CSurfaceStream::CSurfaceStream():
	iSurfaceId(TSurfaceId::CreateNullId()),
	iBufferInfo(NULL),
	iProtected(EFalse),
	iNumberOfScreenAttachedAvailableNotif(0),
	iNumberOfScreenAttachedDisplayedNotif(0),
	iNumberOfScreenAttachedDisplayedXNotif(),
	iFlipState(EFlippedTargetNormal),
    iNewFlip(EFlipNotSet)
	{
	iStreamProxySurfaceId.iInternal[TSurfaceId::TSurfaceUsage::EObjectRefField]=
			reinterpret_cast<TInt>(this);
	iStreamProxySurfaceId.iInternal[TSurfaceId::TSurfaceUsage::ETypeClassField]=
			TSurfaceId::EStreamHandle<<TSurfaceId::TSurfaceUsage::ETypeClassShift;
	}

CSurfaceStream::ContentUpdatedParams::ContentUpdatedParams(TInt aBuffer,
                                                           TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                                                           TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                                                           TRequestStatus* aStatusConsumed, const TRegion* aRegion,
                                                           TBool aImmediateAvailable, TInt32 aImmediateVisibility,
                                                           const TNewGlobalNotifications& aGlobalNotifications):
iBuffer(aBuffer),
iStatusDisplayed(aStatusDisplayed),
iTimeStamp(aTimeStamp),
iStatusDispXTimes(aStatusDispXTimes),
iDisplayedXTimes(aDisplayedXTimes),
iStatusConsumed(aStatusConsumed),
iRegion(aRegion),
iImmediateAvailable(aImmediateAvailable),
iImmediateVisibility(aImmediateVisibility),
iGlobalNotifications(aGlobalNotifications)
    {
    
    }

CSurfaceStream::~CSurfaceStream()
	{
	// Cancel any outstanding notifications
	CancelNotifications();
    iCallBacks.Close();
    iGlobalNotifications.Close();
    iBufferChunk.Close();
	
    if (!iSurfaceId.IsNull() && iSurfaceId.Type() == TSurfaceTypes::ESurfaceManagerSurface)
        {
        TRAP_IGNORE(GetSingletonL().SurfaceManager().CloseSurface(iSurfaceId));
        }
	
	iRefCountMutex.Close();
	iCallBacksMutex.Close();
	delete [] iBufferInfo;
	}

CSurfaceStream* CSurfaceStream::NewLC(const TSurfaceId& aId)
	{
	CSurfaceStream* self = new (ELeave)CSurfaceStream();
	CleanupStack::PushL(self);
	self->ConstructL(aId);
	return self;
	}

void CSurfaceStream::ConstructL(const TSurfaceId& aId)
	{
	User::LeaveIfError(iRefCountMutex.CreateLocal());
    User::LeaveIfError(iCallBacksMutex.CreateLocal());
   
    RSurfaceManager::TInfoBuf infoBuf;  
    SurfaceInfoL(aId, infoBuf);
	iInfo = infoBuf();

    COpenWfcStreamMap& stream = GetSingletonL();
	//Create array for TBufferInfo
	iBufferInfo = new(ELeave) TBufferInfo[iInfo.iBuffers];
	for(TInt i = 0; i < iInfo.iBuffers; i++)
		{
		TInt offset = 0;
	    switch(aId.Type())
	        {
	        case TSurfaceTypes::ESurfaceManagerSurface:
	            {
	            User::LeaveIfError(stream.SurfaceManager().GetBufferOffset(iSurfaceId,
	                     i, offset));
	            break;
	            }
	        case TSurfaceId::EScreenSurface:
	            {
	            TInt screenId = SurfaceId().iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField];
	            User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayOffsetToFirstPixel, offset));
	            break;
	            }
	        default:
	            User::Leave(KErrNotSupported);
	        }

		iBufferInfo[i].iRefCount = 0;
		iBufferInfo[i].iOffset = offset;
		}
	}

void CSurfaceStream::SurfaceInfoL(const TSurfaceId& aId, RSurfaceManager::TInfoBuf& aInfoBuf)
    {
    COpenWfcStreamMap& stream = GetSingletonL();    
    switch(aId.Type())
        {
        case TSurfaceTypes::ESurfaceManagerSurface:
            {
            iStreamProxySurfaceId.iInternal[TSurfaceId::TSurfaceUsage::EObjectRefField]=
                    reinterpret_cast<TInt>(this);
            iStreamProxySurfaceId.iInternal[TSurfaceId::TSurfaceUsage::ETypeClassField]=
                    TSurfaceId::EStreamHandle<<TSurfaceId::TSurfaceUsage::ETypeClassShift;        
            
            User::LeaveIfError(stream.SurfaceManager().OpenSurface(aId));            
            iSurfaceId = aId;
            User::LeaveIfError(stream.SurfaceManager().SurfaceInfo(iSurfaceId, aInfoBuf));
            break;
            }
        case TSurfaceId::EScreenSurface:
            {
            // DSA surface only has one buffer
            aInfoBuf().iBuffers = 1;
            
            TInt screenId = aId.iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField];
            TInt width;
            User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayXPixels, width));
                        
            TInt height;
            User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayYPixels, height));
            
            aInfoBuf().iSize = TSize(width, height);
            
            TInt bpp = 0;
            User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayBitsPerPixel, bpp));

            TUidPixelFormat pixelFormat = static_cast<TUidPixelFormat> (aId.iInternal[TSurfaceId::TScreenSurfaceUsage::ETypeGuidField]);
            if (!pixelFormat)
                {
                //if (bpp==12 || bpp==4))   //This allows low-color indices to be semi-functionally tested
                if (bpp==12)
                    {
                    pixelFormat = EUidPixelFormatXRGB_4444;
                    }
                else if (bpp == 16)
                    {
                    pixelFormat = EUidPixelFormatRGB_565;
                    }
                else if (bpp== 24 || bpp==32)
                    {
                    pixelFormat = EUidPixelFormatARGB_8888;
                    }
                else
                    {
                    User::Leave(KErrNotSupported);
                    }

                }
            aInfoBuf().iPixelFormat = pixelFormat;
            
#ifdef  SYMBIAN_ROTATION_MODE_CHANGES
            TInt displayMode = aId.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
#else
            TInt displayMode = 0;
            User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayMode, displayMode));
#endif            
            
#if defined(SYMBIAN_ROTATION_MODE_CHANGES)
            if (displayMode & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) // 90 | 270 degree rotation
                {
                // Swap dimensions and recalculate stride. Assume no padding for now.
                aInfoBuf().iSize.iWidth = height;
                aInfoBuf().iSize.iHeight = width;
                //"vertical" stride has already been fetched
                }
#elif defined(SYMBIAN_ROTATION_CHANGES)
            if (aSurface.iInternal[1] & (2 | 8))    // 90 | 270 degree rotation
                {
                // Swap dimensions and recalculate stride. Assume no padding for now.
                aInfoBuf().iSize.iWidth = height;
                aInfoBuf().iSize.iHeight = width;
                aInfoBuf().iStride = infoBuf().iSize.width * bpp;
                }
#endif            
            
            TInt stride = displayMode;
            User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayOffsetBetweenLines, stride));

            aInfoBuf().iStride = stride;             
            
            iSurfaceId = aId;
            break;
            }
        default:
            User::Leave(KErrNotSupported);
        } 
    }

COpenWfcStreamMap& CSurfaceStream::GetSingletonL()
	{
	return COpenWfcStreamMap::InstanceL();
	}

/**
	Helper to resolve handle to stream object
**/
/*static*/
CSurfaceStream* CSurfaceStream::FromHandle(SymbianStreamType aNativeStreamHandle)
	{
	if (aNativeStreamHandle)
		{
		if (aNativeStreamHandle->Type()==TSurfaceId::EStreamHandle)
			{
			return reinterpret_cast<CSurfaceStream*>(aNativeStreamHandle->iInternal[TSurfaceId::TSurfaceUsage::EObjectRefField]);
			}
		}
	return NULL;
	}

/**
	Helper to resolve handle to stream object
**/
SymbianStreamType CSurfaceStream::ToHandle()
	{
	if (this)
		{
		return &iStreamProxySurfaceId;
		}
	else
		{
		return NULL;
		}
	}


/**	Returns internal surface ID.
 *
 * @return surface id asociated with this stream
 **/
const TSurfaceId& CSurfaceStream::SurfaceId()const
	{
	return iSurfaceId;
	}

/*!
 * \brief Increase stream's reference count by one.
 *
 */
 void CSurfaceStream::AddReference()
	 {
	 iRefCountMutex.Wait();
	 iRefCount++;
	 iRefCountMutex.Signal();

	 }

/*!
 * \internal
 *
 * \brief Returns flag if reference count is now zero.
 *
 */
TBool	CSurfaceStream::RemainingReference()
	{
	 iRefCountMutex.Wait();
	 TBool countZero=iRefCount==0;
	 iRefCountMutex.Signal();
	 return countZero;
	}


/*!
 * \brief Decrease stream's reference count by one and destroy
 * the stream, if the reference count goes to zero.
 *
 * All acquired read & write buffers must be released
 * before calling WFC_Native_Destroy.
 *
 */
void CSurfaceStream::ReleaseReference()
	{
	iRefCountMutex.Wait();
	--iRefCount;
	if (iRefCount < 1)
		{
		// it is required to signal the mutex before calling LockDestroy()
		iRefCountMutex.Signal();
		TRAP_IGNORE(GetSingletonL().LockDestroy(this));
		}
	else
		{
		iRefCountMutex.Signal();
		}
	 }

/*!
 * \brief Get stream "frame header". Can be used to query
 * all or some of the frame properties.
 *
 * \param width Pointer to location where width parameter should be saved
 * \param height Pointer to location where height parameter should be saved
 * \param stride Pointer to location where stride (row size in bytes)
 * parameter should be saved
 * \param pixelSize Pointer to location where pizelSize (pixel size in bytes) if the format has more than 8 bits.
 *                  For the formats with fewer than 8 bits per pixel, or ones that do not use packed pixel 
 *                  the parameter is a negative number            
 * parameter should be saved

 * Passing a NULL pointer implies that particular
 * value is of no interest to caller. E.g.
 *   owfNativeStreamGetHeader(stream, &w, &h, NULL, NULL, NULL);
 * would only fetch width & height parameters.
 */
void CSurfaceStream::GetHeader(  khronos_int32_t* aWidth,
                           khronos_int32_t* aHeight,
                           khronos_int32_t* aStride,
                           SymOwfPixelFormat* aFormat,
                           khronos_int32_t* aPixelSize)
	 {
	 Guard g1(iRefCountMutex);
	 if (aWidth)
		 {
		 if (iFlipState == EFlippedTargetFlipped)
		     {
	         *aWidth = static_cast<khronos_int32_t>(iInfo.iSize.iHeight);
		     }
		 else
		     {
             *aWidth = static_cast<khronos_int32_t>(iInfo.iSize.iWidth);
		     }
		 }
	 
	 if (aHeight)
		 {
         if (iFlipState == EFlippedTargetFlipped)
             {
             *aHeight = static_cast<khronos_int32_t>(iInfo.iSize.iWidth);
             }
         else
             {
             *aHeight = static_cast<khronos_int32_t>(iInfo.iSize.iHeight);
             }
		 }
	 if (aStride)
		 {
         if (iFlipState == EFlippedTargetFlipped)
             {
             *aStride = Stride(iInfo.iSize.iHeight, iInfo.iPixelFormat);
             }
         else
             {
             *aStride = static_cast<khronos_int32_t>(iInfo.iStride);
             }
		 }
	 if (aFormat)
		 {
		 *aFormat=iInfo.iPixelFormat;
		 }
	 if (aPixelSize)
		 {
		*aPixelSize = static_cast<khronos_int32_t>(BytesPerPixel(iInfo.iPixelFormat));
		 }
	 }

/*!
 * \brief Acquires read buffer for stream. For > 1 buffer
 * streams this function doesn't block, but simply returns
 * WFC_INVALID_HANDLE if no buffer is available for reading.
 * For 1 buffer stream the caller is blocked until the buffer
 * is ready for reading (the reader has committed the buffer,
 * that is.)
 *
 *
 * \return WFC_INVALID_HANDLE if no buffer is available or
 * handle to last committed buffer.
 *
 * An example sequence for 3 buffer stream where
 * producer produces frames approx. after every ~5th time unit.
 * Consumer consumes buffers at constant rate of 1buf/time unit.
 * Pframe is the number/handle of buffer that is being written by
 * the producer (let's assume that it takes 2 time units
 * for producer to produce a frame/buffer.) Cframe is the number/
 * handle of the buffer the consumer receives from AcquireReadBuffer().
 * "i" stands for WFC_INVALID_HANDLE:
 *
 * \code
 * Time:   0    5    10   15   20   25
 * Pframe: 0    1    2    0    1    ...
 * Cframe: ii00000111112222200000111...
 * \endcode
 */
SymbianStreamBuffer CSurfaceStream::AcquireReadBuffer()
	 {
	 SymbianStreamBuffer buffer = WFC_INVALID_HANDLE;

	 iRefCountMutex.Wait();
	 TInt index = GetReadBufferIndex();
     buffer = IndexToReadHandle(index);
     ++(iBufferInfo[index].iRefCount);
	 iRefCountMutex.Signal();
	 AddReference();
	 return buffer;
	 }

/*!
 * \brief Releases read buffer.
 *
 * When read buffer is released, it is marked as clean to
 * be written again, unless it is the only committed buffer
 * in which case it is recycled so that the same buffer
 * can be read again (as long as no new buffers are committed
 * by the producer)
 *
 * \param buf Buffer handle. Must be valid read buffer handle for
 * given stream.
 */
TInt CSurfaceStream::ReleaseReadBuffer(SymbianStreamBuffer aBuf)
	 {
	 TInt index = BufferHandleToIndex(aBuf);
	 if(index < 0 || index >= iInfo.iBuffers)
		 {
		 return KErrBadHandle;
		 }

     iRefCountMutex.Wait();

      __ASSERT_DEBUG(iBufferInfo[index].iRefCount > 0, 
			 (iRefCountMutex.Signal(),Panic(EOwfPanicSurfaceStreamBufferNotLocked)));

      --(iBufferInfo[index].iRefCount);
     
     iRefCountMutex.Signal();
     ReleaseReference();
     return KErrNone;
	 }

/*!
 * \brief Acquires write buffer for stream.
 *
 * Returns handle to a buffer that can be used to write
 * data into stream. If no clean buffer is available,
 * invalid handle is returned.
 *
 *
 * \return Handle to a writable buffer
 */
SymbianStreamBuffer CSurfaceStream::AcquireWriteBuffer()
	 {
	 SymbianStreamBuffer   buffer = WFC_INVALID_HANDLE;

     iRefCountMutex.Wait();
     TInt index = GetWriteBufferIndex();
     //Writers are currently not blocked in single buffered,
     //but if proper signalling was implemented then they could be blocked
     //Ideally, make signalling/blocking a parameter flag of AcquireWriteBuffer
     if (iInfo.iBuffers>1 && iBufferInfo[index].iRefCount > 0)
       	{
        buffer = WFC_INVALID_HANDLE;
      	}
     else
       	{
        buffer = IndexToWriteHandle(index);
        ++(iBufferInfo[index].iRefCount);
        iAcquiredWriteBuffer = buffer;
       	}
    iRefCountMutex.Signal();
    if (buffer)
    	{
    	AddReference();
    	}
    return buffer;
	}

/*!
 * \brief Releases write buffer to stream.
 * Released buffer is made new front buffer, i.e., producer is expected
 * to release buffer is the same order they were acquired.
 *
 * \param buf Buffer handle. Must be valid write buffer handle
 * for given stream.
 */
void CSurfaceStream::ReleaseWriteBuffer(SymbianStreamBuffer aBuf)
	 {
	 TInt index = BufferHandleToIndex(aBuf);
	 if(index < 0 || index >= iInfo.iBuffers)
		 {
		 return;
		 }

	 iRefCountMutex.Wait();

     __ASSERT_DEBUG(iBufferInfo[index].iRefCount > 0,
			(iRefCountMutex.Signal(),Panic(EOwfPanicSurfaceStreamBufferNotLocked)));

     __ASSERT_DEBUG((iAcquiredWriteBuffer == aBuf) || (iAcquiredWriteBuffer == BUFFER_WRITE_UPDATE_OVERWRITE),
			(iRefCountMutex.Signal(),Panic(EOwfPanicSurfaceStreamBufferNotLocked)));

	 if (iAcquiredWriteBuffer != BUFFER_WRITE_UPDATE_OVERWRITE)
		 {
		 // Update read buffer to point to buffer just finished writing
		 SetReadBufferIndex(index);
		 }
	 iAcquiredWriteBuffer = WFC_INVALID_HANDLE;

     --(iBufferInfo[index].iRefCount);
	 
     if (iNewFlip != EFlipNotSet)
         {
         iFlipState = iNewFlip;
         iNewFlip = EFlipNotSet;
         }
     
	 iRefCountMutex.Signal();

	 NotifyObservers(ESOWF_EventComposed);
	 ReleaseReference();	//Note this means this NotifyObservers can never get the rug pulled out
	 }


void CSurfaceStream::SetReadBufferIndex(TInt aIndex)
	{
	__ASSERT_DEBUG(aIndex >= 0 && aIndex < iInfo.iBuffers,
			(iRefCountMutex.Signal(),Panic(EOwfPanicSurfaceStreamBufferIndexOutOfBounds)));
	iReadBuffer = aIndex;
	}

TInt CSurfaceStream::GetReadBufferIndex()
	{
	return iReadBuffer;
	}
TInt CSurfaceStream::GetWriteBufferIndex()
	{
	return (iReadBuffer+1)%iInfo.iBuffers;
	}

/*! \brief Returns pointer to pixel buffer.
 *
 * \param buffer Handle of buffer
 */
void* CSurfaceStream::GetBufferPtrL(SymbianStreamBuffer aBuffer)
	 {
     TInt bufferIndex = BufferHandleToIndex(aBuffer);  
     Guard g1(iRefCountMutex);
     __ASSERT_DEBUG(iBufferInfo[bufferIndex].iRefCount > 0,
             (iRefCountMutex.Signal(),Panic(EOwfPanicSurfaceStreamBufferNotLocked)));
             
     switch (SurfaceId().Type())
         {
         case TSurfaceTypes::ESurfaceManagerSurface:
             {                         
             if (iBufferChunk.Handle() == 0)
                 {
                 RChunk threadChunk;
                 RSurfaceManager sm;
                 sm.Open();
                 TInt err = sm.MapSurface(iSurfaceId, threadChunk);
                 sm.Close();
                 CleanupClosePushL(threadChunk);
                 				 			
				 RChunk duplicateChunk;
				 duplicateChunk.SetHandle(threadChunk.Handle());
                 User::LeaveIfError(duplicateChunk.Duplicate(RThread(), EOwnerProcess));
 
                 iBufferChunk.SetHandle(duplicateChunk.Handle());
                 CleanupStack::PopAndDestroy(&threadChunk);
                 }
             break;
             }
	     case TSurfaceId::EScreenSurface:
	         {
	         if (iBufferChunk.Handle() == 0)
	             {
	             TInt val = 0;
	             TInt screenId = SurfaceId().iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField];
	             User::LeaveIfError(HAL::Get(screenId, HALData::EDisplayMemoryHandle, val));
                
	             RChunk threadChunk;
				 CleanupClosePushL(threadChunk);
	             User::LeaveIfError(threadChunk.SetReturnedHandle(val));

				 RChunk duplicateChunk;
				 duplicateChunk.SetHandle(threadChunk.Handle());
				 User::LeaveIfError(duplicateChunk.Duplicate(RThread(), EOwnerProcess));

	             iBufferChunk.SetHandle(duplicateChunk.Handle());
	             CleanupStack::PopAndDestroy(&threadChunk);
	             }
	         break;
	         }
	     default:
	         User::Leave(KErrNotSupported);
	     }
	 
     TUint8 *pBufferStart = iBufferChunk.Base() + iBufferInfo[bufferIndex].iOffset;
     return static_cast<void*>(pBufferStart);	 
	 }

void CSurfaceStream::SetProtectionFlag(TBool aFlag)
	{
	iRefCountMutex.Wait();
	if (aFlag!=iProtected)
		{
		iProtected=aFlag;
		iRefCountMutex.Signal();
		if (aFlag)
			{
			AddReference();
			}
		else
			{
			ReleaseReference();
			}
		}
	else
		{
		iRefCountMutex.Signal();
		}
	}

TInt CSurfaceStream::BytesPerPixel(TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
		case EUidPixelFormatARGB_8888:
		case EUidPixelFormatBGRX_8888:
		case EUidPixelFormatXBGR_8888:
		case EUidPixelFormatBGRA_8888:
		case EUidPixelFormatABGR_8888:	
		case EUidPixelFormatABGR_8888_PRE:
		case EUidPixelFormatARGB_8888_PRE:
		case EUidPixelFormatBGRA_8888_PRE:
		case EUidPixelFormatARGB_2101010:
		case EUidPixelFormatABGR_2101010:
			return 4;
		case EUidPixelFormatBGR_888:
		case EUidPixelFormatRGB_888:
			return 3;
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
		case EUidPixelFormatXBGR_4444:
		case EUidPixelFormatRGB_565:
		case EUidPixelFormatBGR_565:
		case EUidPixelFormatARGB_1555:
		case EUidPixelFormatXRGB_1555:
		case EUidPixelFormatARGB_8332:
		case EUidPixelFormatBGRX_5551:
		case EUidPixelFormatBGRA_5551:
		case EUidPixelFormatBGRA_4444:
		case EUidPixelFormatBGRX_4444:
		case EUidPixelFormatAP_88:
			return  2;
		case EUidPixelFormatRGB_332:
		case EUidPixelFormatBGR_332:
	    case EUidPixelFormatA_8:
	    case EUidPixelFormatL_8:
	        return  1;
		case EUidPixelFormatP_8:
			return -1;
	    case EUidPixelFormatP_4:
	    case EUidPixelFormatL_4:
	    	return -2;
	    case EUidPixelFormatL_2:
	    case EUidPixelFormatP_2:
	    	return -4;
	    case EUidPixelFormatL_1 :
            return -8;
		default:
			{
			return 0;
			}
		}
	}

SymbianStreamBuffer CSurfaceStream::IndexToReadHandle(TInt aIndex)
	{
	return static_cast<SymbianStreamBuffer>(aIndex + BUFFER_READ_HANDLE_BASE);
	}

SymbianStreamBuffer CSurfaceStream::IndexToWriteHandle(TInt aIndex)
	{
	return static_cast<SymbianStreamBuffer>(aIndex + BUFFER_WRITE_HANDLE_BASE);
	}

TInt CSurfaceStream::BufferHandleToIndex(SymbianStreamBuffer aBuff)
	{
	TInt retVal= (aBuff > 0) ? (aBuff&0xFF) : (aBuff - BUFFER_READ_HANDLE_BASE);
	__ASSERT_DEBUG(retVal>=0,User::Invariant());
	__ASSERT_DEBUG(retVal<iInfo.iBuffers,User::Invariant());
	return retVal;
	}




void CSurfaceStream::RequestComplete(TThreadId& aThreadId, TRequestStatus*& aRequestStatus, TInt& aGlobalIndexArray, TInt aStatus)
    {
    if (aRequestStatus)
        {
        if (aGlobalIndexArray != KInvalidIndex)
            {
            TGlobalNotification& globalNotification = iGlobalNotifications[aGlobalIndexArray];
            --globalNotification.iPendingNotifications;
            switch (aStatus)
                {
                case KErrNone:
                    globalNotification.iCompletedOkNotifications++;
                    break;
                case KErrOverflow:
                    globalNotification.iOverflowedNotifications++;
                    break;
                case KErrCancel:
                    globalNotification.iCanceledNotifications++;
                    break;
                case KErrNotVisible:
                    globalNotification.iNotVisibleNotifications++;
                    break;
                default:
                    globalNotification.iOtherNotifications++;
                }
            NFLOG(("### CSurfaceStream::RequestComplete globalRequestStatus[%d] iPendingNotifications(%d) "
                    "iCompletedOkNotifications(%d)"
                    "iOverflowedNotifications(%d)"
                    "iCanceledNotifications(%d)"
                    "iNotVisibleNotifications(%d)"
                    "iOtherNotifications(%d)"
                    "requestStatus(%p)",
                    aGlobalIndexArray, 
                    globalNotification.iPendingNotifications, 
                    globalNotification.iCompletedOkNotifications, 
                    globalNotification.iOverflowedNotifications,
                    globalNotification.iCanceledNotifications,
                    globalNotification.iNotVisibleNotifications,
                    globalNotification.iOtherNotifications,
                    aStatus));
            
            if (globalNotification.iStatus && globalNotification.iPendingNotifications == 0)
                {
                TInt status;
                if (globalNotification.iCompletedOkNotifications > 0)
                    {
                    status = KErrNone;
                    }
                else if (globalNotification.iOverflowedNotifications > 0)
                    {
                    status = KErrOverflow;
                    }
                else if (globalNotification.iNotVisibleNotifications > 0)
                    {
                    status = KErrNotVisible;
                    }
                else if (globalNotification.iCanceledNotifications > 0)
                    {
                    status = KErrCancel;
                    }
                else
                    {
                    status = KErrGeneral;
                    }
                    
                RThread thread;
                if (thread.Open(globalNotification.iThreadId) == KErrNone)
                    {
                    NFLOG(("### CSurfaceStream::RequestComplete globalIndex[%d] aRequestComplete(0x%x) status(%d)",
                            aGlobalIndexArray, iGlobalNotifications[aGlobalIndexArray].iStatus, status));
                    thread.RequestComplete(globalNotification.iStatus, status);
                    thread.Close();
                    iGlobalNotifications[aGlobalIndexArray].Reset();
                    }
                }
            }
        else
            {
            RThread thread;
            if (thread.Open(aThreadId) == KErrNone)
                {
                NFLOG(("### CSurfaceStream::RequestComplete aRequestStatus(0x%x) aThreadId(0x%x) aStatus(%d)",
                        aRequestStatus, aThreadId, aStatus));
                thread.RequestComplete(aRequestStatus, aStatus);
                thread.Close();
                }
            }
        aRequestStatus = NULL;
        aThreadId = 0;
        }
    aGlobalIndexArray = KInvalidIndex;
    }

int  CSurfaceStream::AddObserver(SymbianStreamCallback aObserver, 
                                 TInt32 aEvent,
                                 TInt32 aScreenNumber,
                                 void* aData)
     {
     if (!(aEvent & ESOWF_AllEventsMask) || !aData)
         {
         // early exit if the parameters are invalid
         return KErrArgument;
         }
     
     TInt  errRet = KErrNone;
     TCallBackEntry newEntry;
     
     Guard g2(iCallBacksMutex);
     Guard g1(iRefCountMutex);
     
     // Let's do the check that we dont have an other similar observer already inserted
     // traverse the whole aobservers list to see if there is already an observer in place
     TInt  idx = iCallBacks.Count();
     while(idx--)
         {
         if (iCallBacks[idx].iEventMask == aEvent)
             {
             switch (aEvent)
                 {
                 case ESOWF_EventAvailable:
                 case ESOWF_EventDisplayed:
                 case ESOWF_EventDisplayedX:
                     // these are events related to a context and identified by a screen number
                     if (iCallBacks[idx].iScreenNumber == aScreenNumber)
                         {
                         // nothing else to do, the entry is already created
                         return KErrOverflow;
                         }
                     break;
                     
                 case ESOWF_EventComposed:
                 case ESOWF_EventUpdated:
                     if (iCallBacks[idx].iCallBackClientParam == aData)
                         {
                         // nothing else to do, the entry is already created
                         return KErrOverflow;
                         }
                     break;
                     
                 default:
                     // something really unexpected, let's invalidate the entry
                     // possible memomry leaks but we are, still, alive and running.
                     iCallBacks[idx].Reset();
                     break;
                 }
             }
         }
     
     // we need to add a new observer to the list
    newEntry.iCallBackFunction = aObserver;
    newEntry.iEventMask = aEvent;
    newEntry.iScreenNumber = aScreenNumber;
    TInt trapErr;
    switch (aEvent)
        {
        case ESOWF_EventUpdated:
        case ESOWF_EventComposed:
            newEntry.iCallBackClientParam = aData;
            break;
        
        case ESOWF_EventAvailable:
            {
            TNotificationAvailable* available = NULL;
            TRAP(trapErr,available = new(ELeave) TNotificationAvailable());
            if (trapErr != KErrNone)
                {
                return trapErr;
                }
            ResetCallBackData(available, aEvent);
            available->iSerialNumber = KInitialContextSerialNumber;
            newEntry.iCallBackClientParam = available;
            }
            break;
        
        case ESOWF_EventDisplayed:
            {
            TNotificationDisplayed* displayed = NULL;
            TRAP(trapErr,displayed = new(ELeave) TNotificationDisplayed());
            if (trapErr != KErrNone)
                {
                return trapErr;
                }
            ResetCallBackData(displayed, aEvent);
            displayed->iSerialNumber = KInitialContextSerialNumber;
            newEntry.iCallBackClientParam = displayed;
            }
            break;
        
        case ESOWF_EventDisplayedX:
            {
            TNotificationDisplayedX* displayedX = NULL;
            TRAP(trapErr,displayedX = new(ELeave) TNotificationDisplayedX()); 
            if (trapErr != KErrNone)
                {
                return trapErr;
                }
            ResetCallBackData(displayedX, aEvent);
            displayedX->iSerialNumber = KInitialContextSerialNumber;
            newEntry.iCallBackClientParam = displayedX;
            }
            break;
        
        default:
            return KErrArgument;
        }
     
     // look for an entry that already exists
     idx = iCallBacks.Count();
     
     while(idx--)
         {
         // the free entry positions are identified using event mask
         if (iCallBacks[idx].iEventMask == ESOWF_NoEvent) break;
         }
     
     if (idx > -1)
         {
         // ok, we got an existing, free entry
         iCallBacks[idx] = newEntry;
         }
     else
         {
         errRet = iCallBacks.Append(newEntry);
         if (errRet != KErrNone)
             {
             return errRet;
             }
         }

     switch (aEvent)
         {
         case ESOWF_EventAvailable:
             iNumberOfScreenAttachedAvailableNotif++;
             break;
             
         case ESOWF_EventDisplayed:
             iNumberOfScreenAttachedDisplayedNotif++;
             break;
             
         case ESOWF_EventDisplayedX:
             iNumberOfScreenAttachedDisplayedXNotif++;
             break;
             
         default:
             break;
         
         }
     
     return errRet;
     }


int CSurfaceStream::RemoveObserver(TInt32 aEvents, void* aData)
    {
    NFLOG(("ENTER ###CSurfaceStream::RemoveObserver() events(%d) data(0x%x)", aEvents, aData));
    if (!(aEvents & ESOWF_AllEventsMask))
    {
        NFLOG(("EXIT ###CSurfaceStream::RemoveObserver() ERROR: KErrArgument"));
        return KErrArgument;
    }
     
    Guard g2(iCallBacksMutex);
    Guard g1(iRefCountMutex);
    
    TInt32 notFoundEvent = aEvents;
    TInt count = iCallBacks.Count();
    TInt32 susScreenNumber = KNoAssociatedScreenNumber;
    
    if (aData)
        {
        susScreenNumber = *(((TInt32*)aData));
        }
        
    while (count-- && notFoundEvent)
        {
        TInt32 currentEvent = iCallBacks[count].iEventMask;
        if (currentEvent & aEvents)
            {
            void* callBackData = iCallBacks[count].iCallBackClientParam;
            switch (currentEvent)
                {
                case ESOWF_EventDisplayed:
                    if (callBackData && (!aData || (iCallBacks[count].iScreenNumber == susScreenNumber)))
                        {
                        Displayed(ESOWF_ObserverCancel, iCallBacks[count].iScreenNumber, NULL, callBackData, NULL);
                        delete (TNotificationDisplayed*) callBackData;
                        iCallBacks[count].Reset();
                        if (iNumberOfScreenAttachedDisplayedNotif > 0)
                            {
                            iNumberOfScreenAttachedDisplayedNotif--;
                            }
                        notFoundEvent &= ~currentEvent;
                        }
                    break;
             
                case ESOWF_EventAvailable:
                    if (callBackData && (!aData || (iCallBacks[count].iScreenNumber == susScreenNumber)))
                        {
                        Available(ESOWF_ObserverCancel, iCallBacks[count].iScreenNumber, NULL, callBackData, NULL);
                        delete (TNotificationAvailable*) callBackData;
                        iCallBacks[count].Reset();
                        if (iNumberOfScreenAttachedAvailableNotif > 0)
                            {
                            iNumberOfScreenAttachedAvailableNotif--;
                            }
                        notFoundEvent &= ~currentEvent;
                        }                       
                    break;
             
                case ESOWF_EventDisplayedX:
                    if (callBackData && (!aData || (iCallBacks[count].iScreenNumber == susScreenNumber)))
                        {
                        DisplayedXTimes(ESOWF_ObserverCancel, iCallBacks[count].iScreenNumber, NULL, callBackData, NULL);
                        delete (TNotificationDisplayedX*) callBackData;
                        iCallBacks[count].Reset();
                        if (iNumberOfScreenAttachedDisplayedXNotif)
                            {
                            iNumberOfScreenAttachedDisplayedXNotif--;
                            }
                        notFoundEvent &= ~currentEvent;
                        }                   
                    break;
             
                case ESOWF_EventComposed:
                    if (!aData || (aData == iCallBacks[count].iCallBackClientParam))
                        {
                        // just in case that we have to delete the call back, we try, first to execute it
                        // avoiding in this way some deadlocks in CT code
                        if (iCallBacks[count].iCallBackFunction && iCallBacks[count].iCallBackClientParam)
                            {
                            iCallBacks[count].iCallBackFunction(ToHandle(), 
                                                                ESOWF_EventComposed, 
                                                                iCallBacks[count].iCallBackClientParam, 
                                                                NULL);
                            }
                        iCallBacks[count].Reset();
                        notFoundEvent &= ~currentEvent;
                        }
                    break;
                
                case ESOWF_EventUpdated:
                    if (!aData || (aData == iCallBacks[count].iCallBackClientParam))
                        {
                        if (iCallBacks[count].iScreenNumber != KNoAssociatedScreenNumber)
                            {
                            susScreenNumber = iCallBacks[count].iScreenNumber;
                            iCallBacks[count].Reset();
                            notFoundEvent &= ~currentEvent;
                            
                            // Reseting variables to loop back and remove SUS events.
                            notFoundEvent |= ESOWF_SUSEventsMask;
                            aEvents = ESOWF_SUSEventsMask;
                            count = iCallBacks.Count();
                            }
                        else
                            {
                            iCallBacks[count].Reset();
                            notFoundEvent &= ~currentEvent;
                            }
                        }
                    break;
                
                default:
                    break;
                }
            }
        }
    
    NFLOG(("EXIT ###CSurfaceStream::CancelNotifications() err(%d)", notFoundEvent ? KErrNotFound : KErrNone));
    return (notFoundEvent ? KErrNotFound : KErrNone);
    }

TInt CSurfaceStream::NotifyObservers(TInt32 aEvent)
    {
    NFLOG(("### ENTER CSurfaceStream::NotifyObservers()"));
    TInt err = KErrNotFound;
    
    Guard g2(iCallBacksMutex);
    TCallBackEntry localCallBackEntry;
    
    for (TInt i = 0; i < iCallBacks.Count(); ++i)
        {
        localCallBackEntry = iCallBacks[i];
        if (iCallBacks[i].iEventMask & aEvent && 
            iCallBacks[i].iCallBackFunction) 
            {
            err = KErrNone;
            NFLOG(("### EXIT CSurfaceStream::NotifyObservers() callback(%d)", iCallBacks[i].iEventMask));
            localCallBackEntry.iCallBackFunction(ToHandle(), 
                                                 iCallBacks[i].iEventMask, 
                                                 localCallBackEntry.iCallBackClientParam, 
                                                 NULL);
            }
        }
    NFLOG(("### EXIT CSurfaceStream::NotifyObservers() err(%d)", err));
    return err;
    }

TBool CSurfaceStream::NotifyComposerContext(TInt32 aScreenNumber, TInt aOp, SYMOWF_CONTENT_UPDATED_PARAM* aParam)
    {
    NFLOG(("### ENTER CSurfaceStream::NotifyComposerContext()"));
    TCallBackEntry entry;
    TBool ret = EFalse;
    for(TInt i = 0; i < iCallBacks.Count(); i++)
        {
        if (iCallBacks[i].iEventMask == ESOWF_EventUpdated && 
            iCallBacks[i].iCallBackFunction && 
            (((iCallBacks[i].iScreenNumber == KNoAssociatedScreenNumber) && (aOp != SYM_CONTENT_UPDATE_BEGIN)) ||
             ((aScreenNumber == KNoAssociatedScreenNumber) && (aOp != SYM_CONTENT_UPDATE_BEGIN)) ||
             iCallBacks[i].iScreenNumber == aScreenNumber))
            {
            entry = iCallBacks[i];
            iRefCountMutex.Signal();
            NFLOG(("###CSurfaceStream::NotifyComposerContext() ESOWF_EventUpdated aParam(%d)", aParam? aParam->id: -1));
            entry.iCallBackFunction(ToHandle(), ESOWF_EventUpdated, entry.iCallBackClientParam, aParam);
            ret = ETrue;
			// We are meant to hold this lock when we leave this function so coverity warning is false
			//coverity[lock]
            iRefCountMutex.Wait();
            }
        }
    NFLOG(("### EXIT CSurfaceStream::NotifyComposerContext() ret(%d)", ret));
    return ret;
    }

TBool CSurfaceStream::StartUpdateNotifications(TInt aScreenNumber, SYMOWF_CONTENT_UPDATED_PARAM& param)
    {
    NFLOG(("### CSurfaceStream::StartUpdateNotifications()"));
    param.id = SYM_CONTENT_UPDATE_BEGIN;
    param.length = sizeof (param);
    param.par = 0;
    param.serialNumber = KNoAssociatedScreenNumber;
    param.immediateAvailable = 0;
    return NotifyComposerContext(aScreenNumber, SYM_CONTENT_UPDATE_BEGIN, &param);
    }

TBool CSurfaceStream::EndUpdateNotifications(TInt aScreenNum, TInt aBufferNum, TInt32 aUpdatedFlags, const TRegion* aRegion)
    {
    (void)aScreenNum;
    (void)aRegion;
    NFLOG(("### CSurfaceStream::EndUpdateNotifications()"));
    if (aBufferNum < 0 || aBufferNum >= iInfo.iBuffers)
        {
        return EFalse;
        }
    
    if (iAcquiredWriteBuffer != WFC_INVALID_HANDLE)
        {
        iAcquiredWriteBuffer = BUFFER_WRITE_UPDATE_OVERWRITE;
        }
    
    SetReadBufferIndex(aBufferNum);
    
    SYMOWF_CONTENT_UPDATED_PARAM param;
    param.length = sizeof (param);
    param.id = SYM_CONTENT_UPDATE_END;
    param.par = aUpdatedFlags;
    return NotifyComposerContext(aScreenNum, SYM_CONTENT_UPDATE_END, &param);
    }

TBool CSurfaceStream::UpdateNotifications(TInt aScreenNum, 
                                         TInt aBufferNum, 
                                         TInt32 aUpdatedFlags, 
                                         const TRegion* aRegion)
    {
    (void)aScreenNum;
    (void)aRegion;
    NFLOG(("### CSurfaceStream::UpdateNotifications()"));
    if (aBufferNum < 0 || aBufferNum >= iInfo.iBuffers)
        {
        return EFalse;
        }
    
    if (iAcquiredWriteBuffer != WFC_INVALID_HANDLE)
        {
        iAcquiredWriteBuffer = BUFFER_WRITE_UPDATE_OVERWRITE;
        }
    
    SetReadBufferIndex(aBufferNum);
    
    SYMOWF_CONTENT_UPDATED_PARAM param;
    param.length = sizeof (param);
    param.id = SYM_CONTENT_UPDATE;
    param.par = aUpdatedFlags;
    return NotifyComposerContext(aScreenNum, SYM_CONTENT_UPDATE, &param);
    }

void CSurfaceStream::SetNewNotifications(TInt            aBuffer,
                                         TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                                         TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                                         TRequestStatus* aStatusConsumed, const TRegion* aRegion, 
                                         TInt32          aScreenNumber)
    {
    NFLOG(("### ENTER * CSurfaceStream::SetNewNotifications()"));
    Guard g2(iCallBacksMutex);
    Guard g1(iRefCountMutex);
    if (aScreenNumber == KAllScreens)
        {
        SetAllNotifications(aBuffer, 
                            aStatusDisplayed, aTimeStamp, 
                            aStatusDispXTimes, aDisplayedXTimes, 
                            aStatusConsumed, aRegion);
        }
    else
        {
        TNewGlobalNotifications noGlobalNotifications;
        SetNotifications(aBuffer, 
                         aStatusDisplayed, aTimeStamp, 
                         aStatusDispXTimes, aDisplayedXTimes, 
                         aStatusConsumed, aRegion, 
                         aScreenNumber, noGlobalNotifications);
        }
    NFLOG(("### EXIT * CSurfaceStream::SetNewNotifications()"));
    }


void CSurfaceStream::SetNotifications(TInt            aBuffer,
                                      TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                                      TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                                      TRequestStatus* aStatusConsumed, const TRegion* aRegion, 
                                      TInt32          aScreenNumber, const TNewGlobalNotifications& aGlobalNotifications)
    {
    NFLOG(("### ENTER * CSurfaceStream::SetNotifications()"));
    TInt32 eventsFound = 0;
    TInt32 eventsReceived = 0;
    TInt32 eventsToProcess = 0;
    TInt32 contextUpdate = 0;
    // let's take in evidence the events we have to process
    if (aStatusConsumed)
        {
        eventsReceived |= ESOWF_EventAvailable;
        }
    if (aStatusDisplayed)
        {
        eventsReceived |= ESOWF_EventDisplayed;
        }
    if (aStatusDispXTimes)
        {
        eventsReceived |= ESOWF_EventDisplayedX;
        }
    
    TInt availableIndex = -1;
    TInt displayedIndex = -1;
    TInt displayedXIndex = -1;
    
    TInt32 serialNumber = KNoAssociatedScreenNumber;
    TBool immediateAvailable = EFalse;
    TInt32 immediateVisibility = SYM_CONTENT_VISIBLE_NOT_SET;
    
    // guard acquiring/release of the update lock of the composer
    // acquire the update lock of the composer and retrive the composer state info
    SYMOWF_CONTENT_UPDATED_PARAM param;
    TBool startUpdateNotification = StartUpdateNotifications(aScreenNumber, param);
    immediateAvailable = param.immediateAvailable && ETrue;
    immediateVisibility = param.immediateVisibility;
    serialNumber = param.serialNumber;
    
    
    // we take, initially in consideration "available" even if we might not have received a consumed request status
    eventsToProcess = 0;
    
    // we try to figure out which are the events we have to process and to get hold of their position
    // in the observers array, in order to avoid traversing it again
    TInt idx = iCallBacks.Count();
    //we will intend to mark the visited events, as an optimisation
    TInt32 eventsToCheck = eventsReceived | ESOWF_EventAvailable;
    TInt32 currentEvent = 0;
    while(--idx > -1 && eventsToCheck)
        {
        currentEvent = iCallBacks[idx].iEventMask;
        if ((currentEvent & eventsToCheck) && (iCallBacks[idx].iScreenNumber == aScreenNumber))
            {
            switch (currentEvent)
                {
                case ESOWF_EventAvailable:
                    // mark observer visited
                    eventsToCheck &= ~currentEvent;
                    // mark the events found only if the corresponding status request has been recived
                    eventsFound |= (currentEvent & eventsReceived);
                    // reset the events to process mask
                    eventsToProcess &= ~currentEvent;
                    {
                    TNotificationAvailable* available = (TNotificationAvailable*) iCallBacks[idx].iCallBackClientParam;
                    if (available && (aStatusConsumed || available->iStatus || available->iNewStatus))
                        {
                        // set the mask of the events to be processed because we have either to overflow some available request
                        // statuses, or to process them further
                        eventsToProcess |= ESOWF_EventAvailable;
                        availableIndex = idx;
                        }
                    }
                    break;
                    
                case ESOWF_EventDisplayed:
                    // mark observer visited
                    eventsFound |= currentEvent;
                    eventsToProcess |= currentEvent;
                    eventsToCheck &= ~currentEvent;
                    displayedIndex = idx;
                    break;
                    
                case ESOWF_EventDisplayedX:
                    // mark observer visited
                    eventsFound |= currentEvent;
                    eventsToProcess |= currentEvent;
                    eventsToCheck &= ~currentEvent;
                    displayedXIndex = idx;
                    break;
                    
                default:
                    Panic(EOwfSymbianUnexpectedObserverId);
                    break;
                }
            }
        }
    
    NFLOG(("### CSurfaceStream::SetNotifications eventsToProcess(0x%x)", eventsToProcess));
    if (eventsToProcess)
        {
        
        // from this momment on, the composer cannot process any notifications related to this stream
        
        ContentUpdatedParams updateParameters(aBuffer, aStatusDisplayed, aTimeStamp,
                                              aStatusDispXTimes, aDisplayedXTimes,
                                              aStatusConsumed, aRegion, 
                                              immediateAvailable, immediateVisibility,
                                              aGlobalNotifications);
        
        // process the observer corresponding to aStatusConsumed request status
        if ((ESOWF_EventAvailable & eventsToProcess) && (availableIndex > -1))
            {
            Available(ESOWF_ObserverProcessing, 
                      serialNumber, 
                      &updateParameters, 
                      iCallBacks[availableIndex].iCallBackClientParam, 
                      &contextUpdate);
            }
        
        // process the observer corresponding to aStatusDisplayed request status
        if ((ESOWF_EventDisplayed & eventsToProcess) && (displayedIndex > -1))
            {
            Displayed(ESOWF_ObserverProcessing, 
                      serialNumber, 
                      &updateParameters, 
                      iCallBacks[displayedIndex].iCallBackClientParam, 
                      &contextUpdate);
            }
        
        // process the observer corresponding to aStatusDispXTimes request status
        if ((ESOWF_EventDisplayedX & eventsToProcess) && (displayedXIndex > -1))
            {
            DisplayedXTimes(ESOWF_ObserverProcessing, 
                            serialNumber, 
                            &updateParameters, 
                            iCallBacks[displayedXIndex].iCallBackClientParam, 
                            &contextUpdate);
            }
        
        }
    
    if (startUpdateNotification)
        {
        EndUpdateNotifications(aScreenNumber, aBuffer, contextUpdate, aRegion);
        }
    else
        {
        UpdateNotifications(aScreenNumber, aBuffer, contextUpdate, aRegion);
        }
    
    if (eventsReceived != eventsFound)
        {
        if (aStatusConsumed && !(eventsFound & ESOWF_EventAvailable))
            {
            User::RequestComplete(aStatusConsumed, KErrCancel);
            }
        
        if (aStatusDisplayed && !(eventsFound & ESOWF_EventDisplayed))
            {
            *aTimeStamp = 0;
            User::RequestComplete(aStatusDisplayed, KErrCancel);
            }
        
        if (aStatusDispXTimes && !(eventsFound & ESOWF_EventDisplayedX))
            {
            User::RequestComplete(aStatusDispXTimes, KErrCancel);
            }
        }
    NFLOG(("### EXIT * CSurfaceStream::SetNotifications()"));
    }

TInt CSurfaceStream::AddNewGlobalNotification(TRequestStatus* aStatusDisplayed, TInt aAssociatedScreens)
    {
    TInt maxIdx = iGlobalNotifications.Count();
    TInt retIdx = KInvalidIndex;
    for (TInt i = 0; i < maxIdx; i++)
        {
        if (iGlobalNotifications[i].iStatus == NULL)
            {
            NFLOG(("### ENTER * CSurfaceStream::AddNewGlobalNotification found free idx(%d)", i));
            retIdx = i;
            break;
            }
        }
    
    if (retIdx == KInvalidIndex)
        {
        TGlobalNotification newNotification;
        if (iGlobalNotifications.Append(newNotification) == KErrNone)
            {
            retIdx = iGlobalNotifications.Count() - 1;
            }
        }
    
    if (retIdx != KInvalidIndex)
        {
        NFLOG(("### ENTER * CSurfaceStream::AddNewGlobalNotification populating idx(%d, status(%p) associated screens=0x%x)", retIdx, aStatusDisplayed, aAssociatedScreens));
        iGlobalNotifications[retIdx].Reset();
        iGlobalNotifications[retIdx].iStatus = aStatusDisplayed;
        iGlobalNotifications[retIdx].iThreadId = RThread().Id();
        iGlobalNotifications[retIdx].iPendingNotifications = aAssociatedScreens;
        }
    
    return retIdx;
    }

void CSurfaceStream::SetAllNotifications(TInt            aBuffer,
                                         TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                                         TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                                         TRequestStatus* aStatusConsumed, const TRegion* aRegion)
    {
    NFLOG(("### ENTER * CSurfaceStream::SetAllNotifications()"));
    TInt32 eventsToProcess = 0;
    // let's take in evidence the events we have to process
    
    TNewGlobalNotifications newGlobalNotifications;
    TInt idx = 0;
    
    if (aStatusConsumed && iNumberOfScreenAttachedAvailableNotif > 0)
        {
        if ((idx = AddNewGlobalNotification(aStatusConsumed, iNumberOfScreenAttachedAvailableNotif)) != KInvalidIndex)
            {
            newGlobalNotifications.iNewAvailableIdx = idx;
            eventsToProcess |= ESOWF_EventAvailable;
            }
        }
    
    if (aStatusDisplayed && iNumberOfScreenAttachedDisplayedNotif > 0)
        {
        if ((idx = AddNewGlobalNotification(aStatusDisplayed, iNumberOfScreenAttachedDisplayedNotif)) != KInvalidIndex)
            {
            newGlobalNotifications.iNewDisplayedIdx = idx;
            eventsToProcess |= ESOWF_EventDisplayed;
            }
        }
    
    if (aStatusDispXTimes && iNumberOfScreenAttachedDisplayedXNotif > 0)
        {
        if ((idx = AddNewGlobalNotification(aStatusDispXTimes, iNumberOfScreenAttachedDisplayedXNotif)) != KInvalidIndex)
            {
            newGlobalNotifications.iNewDisplayedXIdx = idx;
            eventsToProcess |= ESOWF_EventDisplayedX;
            }
        }
    
    NFLOG(("### CSurfaceStream::SetAllNotifications eventsToProcess(0x%x)", eventsToProcess));
    
    idx = iCallBacks.Count();
    TInt prevScreenNumber = KNoAssociatedScreenNumber;
    TInt screenNumber = KNoAssociatedScreenNumber;
    idx = iCallBacks.Count();
    while (idx--)
        {
        if (iCallBacks[idx].iCallBackClientParam &&
            (iCallBacks[idx].iEventMask == ESOWF_EventUpdated) &&
            ((screenNumber = iCallBacks[idx].iScreenNumber) != KNoAssociatedScreenNumber) 
            && prevScreenNumber != screenNumber)
            {
            NFLOG(("### CSurfaceStream::SetAllNotifications update composer %d", screenNumber));
            SetNotifications(aBuffer,
                             aStatusDisplayed, aTimeStamp,
                             aStatusDispXTimes, aDisplayedXTimes,
                             aStatusConsumed, aRegion, screenNumber, newGlobalNotifications);
            
            prevScreenNumber = screenNumber;
            }
        
        }
    
    if (aStatusConsumed && !(eventsToProcess & ESOWF_EventAvailable))
        {
        User::RequestComplete(aStatusConsumed, KErrCancel);
        }
    
    if (aStatusDisplayed && !(eventsToProcess & ESOWF_EventDisplayed))
        {
        *aTimeStamp = 0;
        User::RequestComplete(aStatusDisplayed, KErrCancel);
        }
    
    if (aStatusDispXTimes && !(eventsToProcess & ESOWF_EventDisplayedX))
        {
        User::RequestComplete(aStatusDispXTimes, KErrCancel);
        }
    NFLOG(("### EXIT * CSurfaceStream::SetAllNotifications()"));
        
    }

void CSurfaceStream::ProcessNotifications(TInt32 aEvent, 
                                          TInt32 aScreenNumber, 
                                          TInt32 aOperation, 
                                          TInt32 aSerialNumber, 
                                          TInt32* aReturnMask)
    {
    Guard g(iRefCountMutex);
    NFLOG(("### ENTER CSurfaceStream::ProcessNotifications()"));
    
    TInt32 eventsToFind = aEvent & ESOWF_SUSEventsMask;
    TInt idx = iCallBacks.Count();
    while(idx-- && eventsToFind)
        {
        TInt32 currentEvent = iCallBacks[idx].iEventMask;
        if (currentEvent & aEvent)
            {
            TNotificationBase* notifBase = (TNotificationBase*) iCallBacks[idx].iCallBackClientParam;
            if (notifBase && notifBase->iStatus && iCallBacks[idx].iScreenNumber == aScreenNumber)
                {
                TInt callBackOperation = (aOperation == EDefaultOperation) ? currentEvent : ESOWF_ObserverCheckVisible;
                eventsToFind &= ~currentEvent;
                switch (currentEvent)
                    {
                    case ESOWF_EventAvailable:
                        Available(callBackOperation, aSerialNumber, NULL, iCallBacks[idx].iCallBackClientParam, aReturnMask);
                        break;
                        
                    case ESOWF_EventDisplayed:
                        Displayed(callBackOperation, aSerialNumber, NULL, iCallBacks[idx].iCallBackClientParam, aReturnMask);
                        break;
                        
                    case ESOWF_EventDisplayedX:
                        DisplayedXTimes(callBackOperation, aSerialNumber, NULL, iCallBacks[idx].iCallBackClientParam, aReturnMask);
                        break;
                        
                    default:
                        break;
                    }
                }
            }
        }
    NFLOG(("### EXIT CSurfaceStream::ProcessNotifications()"));
    }

TInt CSurfaceStream::CheckBufferNumber(TInt aBuffer,
                                       TRequestStatus* aStatusDisplayed,
                                       TRequestStatus* aStatusDispXTimes,
                                       TRequestStatus* aStatusConsumed)
    {
    if (aBuffer < 0 || aBuffer >= iInfo.iBuffers)
        {
            if (aStatusConsumed)
                {
                User::RequestComplete(aStatusConsumed, KErrArgument);
                }
            if (aStatusDisplayed)
                {
                User::RequestComplete(aStatusDisplayed, KErrArgument);
                }
            if (aStatusDispXTimes)
                {
                User::RequestComplete(aStatusDispXTimes, KErrArgument);
                }
            return KErrArgument;
        }
    else
        {
        return KErrNone;
        }
    }

void CSurfaceStream::Available(TInt32 aEvent, 
                               TInt32 aSerialNumber, 
                               ContentUpdatedParams* aParams, 
                               void* aCallBackData, 
                               TInt32* aReturnMask)
    {
    (void) aReturnMask;
    NFLOG(("### ENTER CSurfaceStream::Available aEvent(0x%x) aSerialNumber(0x%x) "
            "aParams(0x%x) aCallBackData(0x%x) aReturnMask(0x%x)", 
            aEvent, aSerialNumber, aParams, aCallBackData, aReturnMask));
    
    TNotificationAvailable* callBackData = (TNotificationAvailable*) aCallBackData;
    if (!callBackData)
        {
        return;
        }
    
    switch(aEvent)
        {
        case ESOWF_ObserverProcessing:
            NFLOG(("###CSurfaceStream::Available ESOWF_ObserverProcessing"));
            // when a new available notification request is issued by SUS, we have to:
            // 1. Overflow the old request status (GCE behaviour)
            // 2. Update the observer
            // 3. If immediate availabilty chec if the old request status can be completed
            // 4. Inform the composer about any active available notifications requests
            {
            if (aParams)
                {
                // let's check first for overflow conditions
                // and overflow the oldest if exists (GCE behaviour)
                if (callBackData->iStatus)
                    {
                    NFLOG(("###CSurfaceStream::Available ESOWF_ObserverProcessing Overflow"));
                    // oldest notifications is overflowed (like GCE behaviour)
                    RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrOverflow);
                    callBackData->iStatus = NULL;
                    callBackData->iThreadId = 0;
                    callBackData->iBufferNumber = -1;
                    }
                
                // propagate the new data
                callBackData->iSerialNumber = aSerialNumber;
                TBool multibuffered = iInfo.iBuffers > 1;
                if (multibuffered)
                    {
                    callBackData->iStatus = callBackData->iNewStatus;
                    callBackData->iThreadId = callBackData->iNewThreadId;
                    callBackData->iBufferNumber = callBackData->iNewBufferNumber;
                    callBackData->iGlobalIndex = callBackData->iNewGlobalIndex;
                    }
                else
                    {
                    callBackData->iStatus = aParams->iStatusConsumed;
                    callBackData->iThreadId = RThread().Id();
                    callBackData->iBufferNumber = aParams->iBuffer;
                    callBackData->iGlobalIndex = aParams->iGlobalNotifications.iNewAvailableIdx;
                    }
                 
                // the availability can be immediately completed if the
                // visibility status is known and the composer is not composing at that momment
                if (callBackData->iStatus && aParams->iImmediateAvailable)
                    {
                    NFLOG(("###CSurfaceStream::Available ESOWF_ObserverProcessing Immediate Available"));
                    if (iAcquiredWriteBuffer != WFC_INVALID_HANDLE)
                        {
                        iAcquiredWriteBuffer = BUFFER_WRITE_UPDATE_OVERWRITE;
                        }
                    
                    SetReadBufferIndex(aParams->iBuffer);
                    // immediate notification is possible because the context
                    // is not composing at this momment
                    if (aParams->iImmediateVisibility == SYM_CONTENT_NOT_VISIBLE)
                        {
                        RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrNotVisible);
                        }
                    else
                        {
                        RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrNone);
                        }
                    ResetCallBackData(callBackData, aEvent);
                    }
                    
                if (multibuffered)
                    {
                    callBackData->iNewStatus = aParams->iStatusConsumed;
                    callBackData->iNewThreadId = RThread().Id();
                    callBackData->iNewBufferNumber = aParams->iBuffer;
                    callBackData->iGlobalIndex = aParams->iGlobalNotifications.iNewAvailableIdx;
                    }
                
                // let the composer know that the Availability has to be analysed further
                if (aReturnMask && callBackData->iStatus)
                    {
                    NFLOG(("###CSurfaceStream::Available ESOWF_ObserverProcessing *aReturnMask |= ESOWF_EventAvailable"));
                    *aReturnMask |= ESOWF_EventAvailable;
                    }
                }
            }
            break;

        case ESOWF_EventAvailable:
            NFLOG(("###CSurfaceStream::Available ESOWF_EventAvailable"));
            if (callBackData->iStatus)
                {
                // if it is an event just added during composition, we wxpect the same serial number
                RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrNone);
                // clean the old reques notification related info
                callBackData->iSerialNumber = aSerialNumber;
                callBackData->iStatus = NULL;
                callBackData->iThreadId = 0;
                callBackData->iBufferNumber = -1;
                }
            break;
            
        case ESOWF_ObserverCheckVisible:
            NFLOG(("###CSurfaceStream::Available ESOWF_ObserverCheckVisible"));
            if (callBackData->iStatus)
                {
                // complete the old request status (standard GCE behaviour)
                RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrNotVisible);
                // clean the old reques notification related info
                callBackData->iSerialNumber = aSerialNumber;
                callBackData->iStatus = NULL;
                callBackData->iThreadId = 0;
                callBackData->iBufferNumber = -1;
                }
            break;
            
        case ESOWF_ObserverCancel:
            {
            NFLOG(("###CSurfaceStream::Available ESOWF_ObserverCancel"));
            // cancel both requests if they are valid
            RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrCancel);
            RequestComplete(callBackData->iNewThreadId, callBackData->iNewStatus, callBackData->iGlobalIndex, KErrCancel);
            ResetCallBackData(callBackData, aEvent);
            }
            break;
            
        default:
            return;
        }
    NFLOG(("EXIT ###CSurfaceStream::Available()"));
    }


void CSurfaceStream::Displayed(TInt32 aEvent, TInt32 aSerialNumber, ContentUpdatedParams* aParams, void* aCallBackData, TInt32* aReturnMask)
    {
    TInt notification = KErrNone;
    (void) aReturnMask;
    NFLOG(("### ENTER CSurfaceStream::Displayed aEvent(0x%x) aSerialNumber(0x%x) "
            "aParams(0x%x) aCallBackData(0x%x) aReturnMask(0x%x)", 
            aEvent, aSerialNumber, aParams, aCallBackData, aReturnMask));
    TNotificationDisplayed* callBackData = (TNotificationDisplayed*) aCallBackData;
    if (!callBackData)
        {
        return;
        }
    
    switch(aEvent)
        {
        case ESOWF_ObserverProcessing:
            NFLOG(("###CSurfaceStream::Displayed ESOWF_ObserverProcessing"));
            // When a new available notification request is issued by SUS, we have to:
            // 1. Overflow the previous request status
            // 2. Update the observer
            // 3. Inform the composer about any active available notifications requests
            
            if (callBackData->iStatus)
                {
                NFLOG(("###CSurfaceStream::Displayed ESOWF_ObserverProcessing Overflowing"));
                RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrOverflow);
                ResetCallBackData(callBackData, aEvent);
                }
            if (aParams && aParams->iStatusDisplayed)
                {
                __ASSERT_DEBUG(aParams->iTimeStamp,
                       (iRefCountMutex.Signal(),Panic(EOwfInvalidSUSDisplayedParameter)));
                callBackData->iStatus = aParams->iStatusDisplayed;
                callBackData->iThreadId = RThread().Id();
                callBackData->iBufferNumber = aParams->iBuffer;
                callBackData->iSerialNumber = aSerialNumber;
                callBackData->iTimeStamp = aParams->iTimeStamp;
                callBackData->iGlobalIndex = aParams->iGlobalNotifications.iNewDisplayedIdx;
                NFLOG(("###CSurfaceStream::Displayed iGlobalIndex(%d)", callBackData->iGlobalIndex));
                }
            if (aReturnMask && callBackData->iStatus)
                {
                NFLOG(("###CSurfaceStream::Displayed ESOWF_ObserverProcessing *aReturnMask |= ESOWF_EventDisplayed"));
                *aReturnMask |= ESOWF_EventDisplayed;
                }
            return;
            
        case ESOWF_EventDisplayed:
            // this invoked by composer
            NFLOG(("###CSurfaceStream::Displayed ESOWF_EventDisplayed"));
            if (!callBackData->iStatus)
                {
                // no active notification, nothing to do
                NFLOG(("###CSurfaceStream::Displayed ESOWF_EventDisplayed no active notifications"));
                return;
                }
            // deffer for next composition the processing if the updated happended during a composition
            if (callBackData->iSerialNumber == aSerialNumber && aReturnMask)
                {
                NFLOG(("###CSurfaceStream::Displayed ESOWF_EventDisplayed *aReturnMask |= ESOWF_EventDisplayed"));
                *aReturnMask |= ESOWF_EventDisplayed;
                return;
                }
            // complete the active request status
            callBackData->iSerialNumber = aSerialNumber;
            *callBackData->iTimeStamp = User::FastCounter();
            notification = KErrNone;
            break;

        case ESOWF_ObserverCheckVisible:
            // visibility check comming from composer
            NFLOG(("###CSurfaceStream::Displayed ESOWF_ObserverCheckVisible"));
            if (!callBackData->iStatus)
                {
                // deffer for next composition the processing if the updated happended during a composition
                // or if no active notification, nothing to do
                NFLOG(("###CSurfaceStream::Displayed ESOWF_ObserverCheckVisible = Not Visible"));
                return;
                }
            notification = KErrNotVisible;
            callBackData->iSerialNumber = aSerialNumber;
            break;
            
        case ESOWF_ObserverCancel:
            // cancel the active notification
            NFLOG(("###CSurfaceStream::Displayed ESOWF_ObserverCancel"));
            if (!callBackData->iStatus)
                {
                return;
                }
            notification = KErrCancel;
            break;
            
        default:
            return;
        }
    
    RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, notification);
    ResetCallBackData(callBackData, aEvent);
    NFLOG(("EXIT ###CSurfaceStream::Displayed()"));
    }

void CSurfaceStream::DisplayedXTimes(TInt32 aEvent, TInt32 aSerialNumber, ContentUpdatedParams* aParams, void* aCallBackData, TInt32* aReturnMask)
    {
    TInt notification = KErrNone;
    
    NFLOG(("### ENTER CSurfaceStream::DisplayedXTimes aEvent(0x%x) aSerialNumber(0x%x) "
            "aParams(0x%x) aCallBackData(0x%x) aReturnMask(0x%x)", 
            aEvent, aSerialNumber, aParams, aCallBackData, aReturnMask));
    
    TNotificationDisplayedX* callBackData = (TNotificationDisplayedX*) aCallBackData;
    if (!callBackData)
        {
        return;
        }

    switch(aEvent)
        {
        case ESOWF_ObserverProcessing:
            // When a new available notification request is issued by SUS, we have to:
            // 1. Overflow the previous request status
            // 2. Update the observer
            // 3. Inform the composer about any active available notifications requests
            NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_ObserverProcessing"));
            if (callBackData->iStatus)
                {
                NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_ObserverProcessing Overflowing"));
                RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, KErrOverflow);
                ResetCallBackData(callBackData, aEvent);
                }
            if (aParams && aParams->iStatusDispXTimes)
                {
                __ASSERT_DEBUG(aParams->iDisplayedXTimes && *aParams->iDisplayedXTimes >= 1,
                       (iRefCountMutex.Signal(),Panic(EOwfInvalidSUSDisplayedXTimesParameter)));
                callBackData->iStatus = aParams->iStatusDispXTimes;
                callBackData->iThreadId = RThread().Id();
                callBackData->iBufferNumber = aParams->iBuffer;
                callBackData->iSerialNumber = aSerialNumber;
                callBackData->iCount = *aParams->iDisplayedXTimes;
                callBackData->iGlobalIndex = aParams->iGlobalNotifications.iNewDisplayedXIdx;
                }
            
            if (aReturnMask && callBackData->iStatus)
                {
                NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_ObserverProcessing *aReturnMask |= ESOWF_EventDisplayedX"));
                *aReturnMask |= ESOWF_EventDisplayedX;
                }
            return;
            
        case ESOWF_EventDisplayedX:
            NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_EventDisplayedX"));
            if (!callBackData->iStatus)
                {
                // no active notification, nothing to do
                return;
                }
            
            // deffer for next composition the processing if the updated happended during a composition
            if (callBackData->iSerialNumber == aSerialNumber)
                {
                if (aReturnMask)
                    {
                    NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_EventDisplayedX *aReturnMask |= ESOWF_EventDisplayedX"));
                    *aReturnMask |= ESOWF_EventDisplayedX;
                    }
                return;
                }
            
            callBackData->iSerialNumber = aSerialNumber;
            
            // complete the active request status
            if (callBackData->iCount > 1)
                {
                // inform the composer a new notification is needed
                callBackData->iCount--;
                if (aReturnMask)
                    {
                    NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_EventDisplayedX *aReturnMask |= ESOWF_EventDisplayedX iCount(%d)", callBackData->iCount));
                    *aReturnMask |= ESOWF_EventDisplayedX;
                    }
                return;
                }
            
            NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_EventDisplayedX iCount(%d)", callBackData->iCount));
            notification = KErrNone;
            break;

        case ESOWF_ObserverCheckVisible:
            NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_ObserverCheckVisible"));
            // visibility check comming from composer
            if (!callBackData->iStatus || 
                callBackData->iSerialNumber == aSerialNumber)
                {
                // deffer for next composition the processing if the updated happended during a composition
                // or if no active notification, nothing to do
                return;
                }
            callBackData->iSerialNumber = aSerialNumber;
            notification = KErrNotVisible;
            break;
            
        case ESOWF_ObserverCancel:
            // cancel the active notification
            NFLOG(("###CSurfaceStream::DisplayedXTimes ESOWF_ObserverCancel"));
            if (!callBackData->iStatus)
                {
                return;
                }
            notification = KErrCancel;
            break;
            
        default:
            return;
        }
    
    RequestComplete(callBackData->iThreadId, callBackData->iStatus, callBackData->iGlobalIndex, notification);
    ResetCallBackData(callBackData, aEvent);
    NFLOG(("EXIT ###CSurfaceStream::DisplayedXTimes()"));
    }

void CSurfaceStream::ResetCallBackData(void* aCallBackData,
                                       TInt32 aEvent)
    {
    switch (aEvent)
        {
        case ESOWF_EventAvailable:
            {
            TNotificationAvailable* available = (TNotificationAvailable*) aCallBackData;
            available->iStatus = NULL;
            available->iThreadId = 0;
            available->iBufferNumber = -1;
            available->iGlobalIndex = KInvalidIndex;
            available->iNewBufferNumber = -1;
            available->iNewStatus = NULL;
            available->iNewThreadId = 0;
            available->iNewGlobalIndex = KInvalidIndex;
            }
            break;
            
        case ESOWF_EventDisplayed:
            {
            TNotificationDisplayed* displayed = (TNotificationDisplayed*) aCallBackData; 
            displayed->iStatus = NULL;
            displayed->iThreadId = 0;
            displayed->iBufferNumber = -1;
            displayed->iTimeStamp = NULL;
            displayed->iGlobalIndex = KInvalidIndex;
            }
            break;
            
        case ESOWF_EventDisplayedX:
            {
            TNotificationDisplayedX* displayed = (TNotificationDisplayedX*) aCallBackData; 
            displayed->iStatus = NULL;
            displayed->iThreadId = 0;
            displayed->iBufferNumber = -1;
            displayed->iCount = 0;
            displayed->iGlobalIndex = KInvalidIndex;
            }
            break;
            
        default:
            break;
        }
    }

void CSurfaceStream::CancelNotifications()
    {
    NFLOG(("ENTER ###CSurfaceStream::CancelNotifications()"));
    TInt cancelEvents = ESOWF_AllEventsMask;
    RemoveObserver(cancelEvents, NULL);
    NFLOG(("EXIT ###CSurfaceStream::CancelNotifications()"));
    }

TInt CSurfaceStream::Stride(TInt aWidth, TUidPixelFormat aPixelFormat)
    {
    
    TInt bytesPerPixel = BytesPerPixel(aPixelFormat);
    
    if (bytesPerPixel >= 0)
        {
         return bytesPerPixel * aWidth;  // number of bytes between start of one line and start of next  
        }
    else
        {
        return (aWidth-(bytesPerPixel+1)) / (-bytesPerPixel);
        }
    }

void CSurfaceStream::SetFlipState(TBool aFlip)
    {
    Guard g1(iRefCountMutex);
    if (aFlip)
        {
        iNewFlip = EFlippedTargetFlipped;
        }
    else
        {
        iNewFlip = EFlippedTargetNormal;
        }
    }

TInt CSurfaceStream::GetChunkHandle()
    {
    return iBufferChunk.Handle();
    }
