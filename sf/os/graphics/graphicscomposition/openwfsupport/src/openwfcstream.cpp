// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// openwfcstream.cpp
// C API for surface stream implementation of OpenWF-C NativeStream 
//


//  Include Files  
#include <e32def.h>
#include <pixelformats.h>
#include <graphics/suerror.h>
#include "symbianstream.h"
#include "surfacestream.h"
#include "streammap.h"
#include "openwfcpanic.h"


extern "C"{

EXPORT_C TErrCode SymbianStreamAcquire( const TSurfaceId* aId, SymbianStreamType* aReturnStream)
	{
	if (aId && !aId->IsNull())
		{
        CSurfaceStream* stream = NULL;
        if (aId->Type()==TSurfaceId::EStreamHandle)
            {
            stream=CSurfaceStream::FromHandle(aId);
            __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamUnexpectedError));
            stream->AddReference();
            *aReturnStream= stream->ToHandle();
            return KErrNone;
            }
        else
            {
            TRAPD(err,stream = COpenWfcStreamMap::InstanceL().AcquireL(*aId));
            if (stream)
                {
                *aReturnStream = stream->ToHandle();
                return KErrNone;
                }
            else
                {
                return err;
                }
            }
		}
	else
		{
		*aReturnStream = NULL;
		return KErrBadHandle;
		}	
	}

EXPORT_C TErrCode SymbianStreamFind( const TSurfaceId* aId, SymbianStreamType* aReturnStream)
	{
	if (aId && !aId->IsNull())
		{
        CSurfaceStream* stream=NULL;
        if (aId->Type()==TSurfaceId::EStreamHandle)
            {
            stream=CSurfaceStream::FromHandle(aId);
            __ASSERT_DEBUG(stream, Panic(EOwfSymbianStreamBadArgument));
            stream->AddReference();
            *aReturnStream= stream->ToHandle();
            return KErrNone;
            }
        else
            {
            TRAPD(err,stream = COpenWfcStreamMap::InstanceL().Find(*aId));
            if (stream)
                {
                *aReturnStream = stream->ToHandle();
                return KErrNone;
                }
            else
                {
                *aReturnStream = NULL;
                if (err==KErrNone)
                    {
                    return KErrNotFound;
                    }
                else
                    {
                    return err;
                    }
                }
            }
        }
	else
		{
		*aReturnStream = NULL;
		return KErrBadHandle;
		}	
	}

EXPORT_C void SymbianStreamAddReference(SymbianStreamType aStream)
	{
	CSurfaceStream*	sstream=CSurfaceStream::FromHandle(aStream);
	if (sstream)
		{
		sstream->AddReference();
		}
	}

EXPORT_C TErrCode SymbianStreamGetBufferId( SymbianStreamType aStream, 
                        SymbianStreamBuffer aBufferHandle, 
                        khronos_int32_t* aReturnIndex, 
                        const TSurfaceId** aReturnSurface)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    if (aReturnIndex)
        {
        *aReturnIndex=stream->BufferHandleToIndex(aBufferHandle);
        *aReturnSurface = &stream->SurfaceId();
        }
    return KErrNone;
	}

EXPORT_C void SymbianStreamRemoveReference(SymbianStreamType aStream)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    stream->ReleaseReference();
	}
EXPORT_C void SymbianStreamGetHeader( SymbianStreamType aStream,
                            khronos_int32_t* aWidth, 
                            khronos_int32_t* aHeight, 
                            khronos_int32_t* aStride, 
                            khronos_int32_t* aFormat, 
                            khronos_int32_t* aPixelSize)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    stream->GetHeader(aWidth,aHeight,aStride,aFormat,aPixelSize);
	}

EXPORT_C TErrCode SymbianStreamAcquireReadBuffer( SymbianStreamType aStream, SymbianStreamBuffer* aReturnBuffer)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    *aReturnBuffer = stream->AcquireReadBuffer();
    return KErrNone;
	}
EXPORT_C TErrCode SymbianStreamReleaseReadBuffer( SymbianStreamType aStream, 
                                                    SymbianStreamBuffer aBuffer)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    return stream->ReleaseReadBuffer(aBuffer);
	}

EXPORT_C TErrCode SymbianStreamAcquireWriteBuffer( SymbianStreamType aStream, SymbianStreamBuffer* aReturnBuffer)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    *aReturnBuffer = stream->AcquireWriteBuffer();
    if (*aReturnBuffer != NULL)
        {
        return KErrNone;
        }
    else 
        {
        return KErrInUse;
        }
	}

EXPORT_C TErrCode SymbianStreamReleaseWriteBuffer( SymbianStreamType aStream, 
                                                    SymbianStreamBuffer aBuffer)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    stream->ReleaseWriteBuffer(aBuffer);
    return KErrNone;
	}

EXPORT_C TErrCode SymbianStreamAddObserver( SymbianStreamType aStream, 
                            SymbianStreamCallback aObserver, 
                            void* aData)
	{
    CSurfaceStream* stream=CSurfaceStream::FromHandle(aStream);
    if (stream && aObserver)
        {
        SYMOWF_DEFAULT_EVENT_PARAM eventPar;
        eventPar.length = sizeof(eventPar);
        eventPar.event = ESOWF_NoEvent;
        aObserver(SYMBIAN_INVALID_HANDLE, ESOWF_ObserverReturnDefaultEvent, NULL, &eventPar);
        return stream->AddObserver(aObserver, eventPar.event, KNoAssociatedScreenNumber, aData);
        }
    return KErrBadHandle;
	}

EXPORT_C TErrCode SymbianStreamRemoveObserver( SymbianStreamType aStream, 
                                                void* aData, 
                                                SymbianStreamEventBits aEvents)
    {
    CSurfaceStream* stream = CSurfaceStream::FromHandle(aStream);
    if (stream)
        {
            return stream->RemoveObserver(aEvents, aData);
        }
    return KErrBadHandle;
    }


EXPORT_C TErrCode SymbianStreamGetBufferPointer( SymbianStreamType aStream, 
                            SymbianStreamBuffer aBuffer,
                            void** aReturnPointer)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
    __ASSERT_ALWAYS(stream, Panic(EOwfSymbianStreamBadArgument));
    *aReturnPointer = NULL;
    TRAPD(err,*aReturnPointer = stream->GetBufferPtrL(aBuffer));
    return err;
	}

EXPORT_C void 
SymbianStreamSetProtectionFlag( SymbianStreamType aStream, khronos_bool aFlag)
	{
	CSurfaceStream*	stream=CSurfaceStream::FromHandle(aStream);
	if (stream)
		{
		stream->SetProtectionFlag(aFlag);
		}
	}

/*!
 * \brief Register a screen number for surface update notifications.
 *
 * \param aScreenNum Screen number.
 * 
 * 
 * \return Standard error code or KErrNone
 */
EXPORT_C TErrCode SymbianStreamRegisterScreenNotifications( khronos_int32_t aScreenNumber, 
                                            khronos_int32_t aPriority, 
                                            khronos_int32_t aInternalVersion)
	{
	khronos_int32_t err2=KErrUnknown;
	TRAPD(err,err2=COpenWfcStreamMap::InstanceL().RegisterScreenNotifications(aScreenNumber,aPriority,aInternalVersion));
	if (err)
		{
		return err;
		}
	else
		{
		return err2;
		}
	}
/*!
 * \brief Get handle to registration of a screen number for surface update notifications.
 *
 * \param aScreenNum Screen number.
 * 
 * 
 * \return Handle to internal object (currently a CBase), or NULL if not registered
 */
EXPORT_C TErrCode 
SymbianStreamHasRegisteredScreenNotifications( khronos_int32_t aScreenNumber,void** aReturnState)
    {
    TRAPD(err,*aReturnState=(COpenWfcStreamMap::InstanceL().RegisteredScreenNotifications(aScreenNumber)));
    return err;
    }


/*!
 * \brief Unregister a screen number for surface update notifications.
 *
 * \param aScreenNum Screen number.
 * 
 * 
 * \return Standard error code or KErrNone
 */
EXPORT_C TErrCode 
SymbianStreamUnregisterScreenNotifications(khronos_int32_t aScreenNumber)
	{
	khronos_int32_t err2=KErrUnknown;
	TRAPD(err,err2=COpenWfcStreamMap::InstanceL().UnregisterScreenNotifications(aScreenNumber));
	if (err)
		{
		return err;
		}
	else
		{
		return err2;
		}
	}

/*!
 * \brief Check if two stream objects are the same surface.
 *
 * \param 	aStream 	compare these two streams.
 * \param	aStream2	compare these two streams.
 * 
 * \return TRUE if they are the same object.
 */
EXPORT_C khronos_bool SymbianStreamSame(SymbianStreamType aStream, SymbianStreamType aStream2)
	{
	if  (aStream==aStream2)
		return KHR_BOOLEAN_TRUE;
	if (!aStream || !aStream2)
		return KHR_BOOLEAN_FALSE;
	if (*aStream==*aStream2)
		return KHR_BOOLEAN_TRUE;
	SymbianStreamType found;
	if (	aStream->Type()!=aStream->EStreamHandle
		&&	aStream2->Type()==aStream2->EStreamHandle)
		{
		if(KErrNone==SymbianStreamFind(aStream,&found))
			{
			if  (found==aStream2)
				return KHR_BOOLEAN_TRUE;
			}
		return KHR_BOOLEAN_FALSE;
		}
	if (	aStream2->Type()!=aStream2->EStreamHandle
		&&	aStream->Type()==aStream->EStreamHandle)
		{
		if(KErrNone==SymbianStreamFind(aStream2,&found))
			{
			if  (aStream==found)
				return KHR_BOOLEAN_TRUE;
			}
		return KHR_BOOLEAN_FALSE;
		}
	return KHR_BOOLEAN_FALSE;
	}

EXPORT_C void
SymbianStreamProcessNotifications(SymbianStreamType aStream, 
                                  khronos_int32_t aEvent, 
                                  khronos_int32_t aScreenNumber,
                                  khronos_int32_t aSerialNumber,
                                  khronos_int32_t* aReturnMask)
    {
    CSurfaceStream* stream = CSurfaceStream::FromHandle(aStream);
    if (stream)
        {
        stream->ProcessNotifications(aEvent, aScreenNumber, CSurfaceStream::EDefaultOperation, aSerialNumber, aReturnMask);
        }
    }

EXPORT_C void
SymbianStreamCheckVisible(SymbianStreamType aStream, 
                          khronos_int32_t aEvent, 
                          khronos_int32_t aScreenNumber,
                          khronos_int32_t aSerialNumber)
    {
    CSurfaceStream* stream = CSurfaceStream::FromHandle(aStream);
    if (stream)
        {
        stream->ProcessNotifications(aEvent, aScreenNumber, CSurfaceStream::ECheckVisibleOperation, aSerialNumber, NULL);
        }
    }

EXPORT_C TErrCode 
SymbianStreamAddExtendedObserver(SymbianStreamType aStream, 
                                SymbianStreamCallback aObserver, 
                                void* aData, 
                                khronos_int32_t aScreenNumber, 
                                SymbianStreamEventBits aEvents)
    {
    CSurfaceStream* stream=CSurfaceStream::FromHandle(aStream);
    if (stream && aObserver)
        {
        TInt ret = KErrNone;
        if (!((ret = stream->AddObserver(aObserver, aEvents, aScreenNumber, aData)) == KErrNone) || aEvents != ESOWF_EventUpdated)
            {
            return ret;
            }
        if (!((ret = stream->AddObserver(NULL, ESOWF_EventAvailable, aScreenNumber, aData)) == KErrNone))
            {
            return ret;
            }
        if (!((ret = stream->AddObserver(NULL, ESOWF_EventDisplayed, aScreenNumber, aData)) == KErrNone))
            {
            return ret;
            }
        ret = stream->AddObserver(NULL, ESOWF_EventDisplayedX, aScreenNumber, aData);
        return ret;
        }
    else
        {
        return KErrArgument;
        }
    }



EXPORT_C void
SymbianStreamSetFlipState(SymbianStreamType aStream, SymOwfBool aFlip)
    {
    CSurfaceStream* stream = CSurfaceStream::FromHandle(aStream);
    if (stream)
        {
        stream->SetFlipState(aFlip);
        }
    }

EXPORT_C TErrCode
SymbianStreamGetChunkHandle(SymbianStreamType aStream, TInt* aHandle)
    {
    CSurfaceStream* stream = CSurfaceStream::FromHandle(aStream);
    if (stream && aHandle)
        {
        *aHandle = stream->GetChunkHandle();
        return KErrNone;
        }
    return KErrArgument;
    }

}   //extern "C" helps fix and verify linkage
