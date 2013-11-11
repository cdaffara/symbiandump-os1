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
// symbianstream.h
// C API for surface stream implementation of OpenWF-C NativeStream
//

#ifndef __SYMBIANSTREAM_H__
#define __SYMBIANSTREAM_H__


#ifdef __cplusplus
class TSurfaceId;
class RSurfaceManager;
#else
typedef struct struct_TSurfaceId TSurfaceId;
#endif
#include <KHR/khrplatform.h>
typedef khronos_int32_t khronos_bool;
typedef khronos_int32_t TErrCode;

/*!
 * \brief Image stream implementation
 *
 * WF native stream is an abstraction of image stream or
 * a content pipe that can be used to deliver image data from
 * place to another. A stream has a producer (source) and a consumer
 * (sink) as its users.
 *
 * Streams operate on buffers, whose count is fixed at creation
 * time (minimum is 1, but for non-blocking behavior values
 * greater than 1 should be used.) Streams are meant to be used
 * strictly on "point-to-point" basis, i.e. there should be only
 * one producer and one consumer for each stream.
 *
 */
#	ifdef __cplusplus
extern "C" {
#	endif

#define KInitialContextSerialNumber 0
#define KNoAssociatedScreenNumber (-1)

typedef const TSurfaceId*	SymbianStreamType;

typedef
    enum type_SymbianStreamEventBits   {       //note reservations are internal and reorderable
    ESOWF_NoEvent                       = 0x00000000, // no event field: used to invalidate an observer
                                                        // entry in the observer container
    ESOWF_EventComposed                 = 0x00000001, // identifies the callback called when tha internal a stream is updated
    
    ESOWF_EventUpdated                  = 0x00000010, // identifies the content update
                                                        //callback used to trigger a new composition
    ESOWF_SIEventsMask                  = ESOWF_EventComposed | ESOWF_EventUpdated,

    // the following events identify the SUS events
    ESOWF_EventAvailable                = 0x00000100, // SUS notification
    ESOWF_EventDisplayed                = 0x00000200, // SUS notification
    ESOWF_EventDisplayedX               = 0x00000400, // SUS notification
    ESOWF_SUSEventsMask                 = ESOWF_EventDisplayed | ESOWF_EventAvailable | ESOWF_EventDisplayedX,

    ESOWF_AllEventsMask                 = (ESOWF_SIEventsMask | ESOWF_SUSEventsMask),

    // the following fields are additional operations, extending the observer behaviour
    ESOWF_ObserverReturnDefaultEvent    = 0x00010000,
    ESOWF_ObserverProcessing            = 0x00020000,
    ESOWF_ObserverCheckVisible          = 0x00040000,
    ESOWF_ObserverCancel                = 0x00080000,

    // reserved for future extensions
    ESOWF_RESERVED_MASK                 = 0xff000000
    } SymbianStreamEventBits;

typedef khronos_int32_t SymOwfNativeStreamBuffer;

#define SYMBIAN_INVALID_HANDLE  0

typedef khronos_int32_t	MultipleSymbianStreamEventBits;
typedef khronos_int32_t	SymNativeStreamEvent;
typedef khronos_int32_t SymbianStreamBuffer;
typedef khronos_int32_t	SymOwfPixelFormat;
typedef	khronos_int32_t SymOwfBool;

typedef enum
{
    SOWF_STREAM_ERROR_NONE               = 0,
    SOWF_STREAM_ERROR_INVALID_STREAM     = -1,
    SOWF_STREAM_ERROR_INVALID_OBSERVER   = -2,
    SOWF_STREAM_ERROR_OUT_OF_MEMORY      = -3
} SOWF_STREAM_ERROR;

/*! Native stream callback function type */
typedef void (*SymOwfStreamCallback)(SymbianStreamType, SymNativeStreamEvent, void*);
/*! Native stream callback function type */
typedef void (*SymbianStreamCallback)(SymbianStreamType, MultipleSymbianStreamEventBits, void*, void*);

typedef struct{
    TInt32      length;
    TInt32      id;
    TInt32      par;
    TInt32      serialNumber;
    TInt32      immediateAvailable;
    TInt32      immediateVisibility;
} SYMOWF_CONTENT_UPDATED_PARAM;

typedef struct{
    TInt32      length;
    TInt32      event;
} SYMOWF_DEFAULT_EVENT_PARAM;

typedef enum
{
    SYM_CONTENT_UPDATE_BEGIN               = 0x00000001,
    SYM_CONTENT_UPDATE_END                 = 0x00000002,
    SYM_CONTENT_UPDATE                     = 0x00000003,
    SYM_CONTENT_UPDATE_MAX                 = 0xffffffff
} SymContentUpdatedCommandsId;

typedef enum
{
    SYM_CONTENT_VISIBLE_NOT_SET            = 0x00000000,
    SYM_CONTENT_VISIBLE                    = 0x00000001,
    SYM_CONTENT_NOT_VISIBLE                = 0x00000002,
    SYM_CONTENT_MAX                        = 0xffffffff
} SymContextVisibilityState;

typedef enum
{
    SYM_CONTENT_PREVIOUS_COMMIT_FALSE      = 0x00000000,
    SYM_CONTENT_PREVIOUS_COMMIT_TRUE       = 0x00000001,
    SYM_CONTENT_COMMIT_MAX                 = 0xffffffff
} SymContextCommitState;


/** Finds, and creates if necessary, a Native Stream object encapsulating the specified surface.

@param  aId The TSurfaceId for the surface to encapsulate.
@param  aReturnHandle   The Native Stream handle created.

@pre
    -   aId must not be a NULL pointer
    -   aReturnHandle must not be a NULL pointer, and must point to writable storage for the handle.
    -   aId must be a valid active Surface ID created by the Surface Manager.
        -   In particular, aId->IsNull() should return EFalse.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    The location pointed to by aReturnHandle is set to a valid Native Stream object and KErrNone is returned, or NULL is set and an error code is returned appropriately.
    The method may fail due to memory allocation if the Native Stream does not already exist.
    If a stream object is successfully created, then the reference on the stream is increased to ensure it remains live following the method call. See SymbianStreamRemoveReference to release the reference.

@panic  NativeStream 1000001 Null Surface ID
@panic  NativeStream  1000002 Null return value pointer
@return Symbian error code
    -  KErrNone    Method Succeeded
    -  KErrBadHandle   aId was not created by the Surface Manager, or has been destroyed.
    -  KErrNoMemory    Internal allocation of resources for the Native Stream failed.
**/
  IMPORT_C TErrCode SymbianStreamAcquire( const TSurfaceId* aId, SymbianStreamType* aReturnHandle);

/** Opens the current output buffer of the Native Stream object for non-exclusive reading.

@param  aStream Native Stream handle that is to be accessed.
@param  aReturnBuffer   Filled with the handle to use to access the current read buffer

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aReturnBuffer must not be a NULL pointer, and must point to writable storage for the handle.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
    -   To simplify composition, SymbianStreamAcquireReadBuffer will never fail. It will arbitrarily select a buffer if the stream has never been written to or updated.
@post
    -   This method always succeeds and returns quickly.
    -   The current read buffer is locked for reading.
    -   The location pointed to by aReturnBuffer is set to a handle representing the read buffer at the time of this call.
    -   Many readers may access the same buffer, but if the Native Stream represents a multi-buffered TSurfaceId content, writers are blocked from overwriting the current read buffer, which will be the last buffer successfully written to.
        -   In single-buffered streams, the content of the buffer may be overwritten while the reader is reading it, leading to tearing, but this behavior is generally considered preferable to blocking the reader.
    -   The Native Stream will not be destroyed while buffers are acquired.
    -   Note that no operations likely to cause failure will occur during the acquire method, but successful acquire does not guarantee that the buffer pixel data can be successfully mapped using NativeStreamGetBufferPointer.

@panic NativeStream 1000002 Null return value pointer
@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -  KErrNone    Method Succeeded
    -  KErrNoMemory    Internal allocation of resources for the Native Stream failed.
**/
  IMPORT_C TErrCode SymbianStreamAcquireReadBuffer( SymbianStreamType aStream, SymbianStreamBuffer* aReturnBuffer);
  
 
/** Opens the write buffer of the Native Stream for exclusive writable access.

@param  aStream Native Stream handle that is to be accessed.
@param  aReturnBuffer   Filled with the handle to use to access the current write buffer

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aReturnBuffer must not be a NULL pointer, and must point to writable storage for the handle.
    -   In single-buffered Native Streams, access is not controlled, any number of readers and/or writer(s) may access the buffer simultaneously, potentially leading to tearing, but this behavior is generally considered preferable to blocking the reader.
    -   In multi-buffered Native Streams, the next write buffer must be available, or the method call will fail and return an error. It may fail in three scenarios:
        -   There is already an active writer
        -   There are still readers attached to the next write buffer candidate (in buffer swapping architectures)
        -   The copy from write buffer to read buffer is ongoing, or waiting for readers (in buffer copying architectures)
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   This method returns quickly.
    -   If the write buffer cannot be acquired immediately then an error code is returned, indicating why exclusive access to a writable buffer could not be obtained.
    -   If the write buffer is acquired the KErrNone is returned, and the contents referenced by aReturnBuffer are filled with a handle representing the current write buffer. The current write buffer is locked for writing.
    -   The Native Stream will not be destroyed while buffers are acquired.
    -   Note that no resource operations likely to cause failure will occur during the acquire method, but successful acquire does not guarantee that the buffer pixel data can be successfully mapped using NativeStreamGetBufferPointer.
@panic NativeStream 1000002 Null return value pointer
@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrInUse   Write buffer could not be exclusively acquired
**/ 
  IMPORT_C TErrCode SymbianStreamAcquireWriteBuffer( SymbianStreamType aStream, SymbianStreamBuffer* aReturnBuffer);
  

/** Registers observers for OpenWF-C SI specified events.
@code   typedef void (*SymOwfStreamCallback)(
    SymbianStreamType, SymNativeStreamEvent, void*);
@endcode
@param  aStream Native Stream handle that is to be accessed.
@param  aObserver   Client function to be called when any event is called.
@param  aData   Data supplied by client which will be passed to the method call.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aObserver must not be NULL
    -   aData may be NULL
    -   The combination of observer and data may be already registered, in which case it will be called twice if an event occurs.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   The method may fail to add the observer to an internal list of observers, due to memory failure.
    -   The observer function will be called when any event occurs which was specified in the OpenWF-C SI and has been implemented by OpenWF Support. Symbian OS-specific extension events will not be delivered to observers registering using this method.
@panic NativeStream 1000003 Null Native Stream
@panic NativeStream 1000004 Null aObserver
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrNoMemory    Internal allocation of resources for the Native Stream failed.
**/
  IMPORT_C TErrCode SymbianStreamAddObserver( SymbianStreamType aStream, SymbianStreamCallback aObserver, void* aData);

/** Registers observers for specific events including Symbian OS-specified events.
@param  aStream Native Stream handle that is to be accessed.
@param  aObserver   Client function to be called when any specified event is called.
@param  aData   Client data passed to the function.
@param  aScreenNumber   Screen Number of the consumer, where relevant.
@param  aEvents Events for which this observer will be called.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aObserver must not be NULL
    -   aEvents must specify 1 or more event flags
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
    -   aScreenNumber should be one of:
        -   zero:   Screen number 0 associated with the first on-screen composition context
        -   positive:   A real screen number associated other on-screen composition contexts
        -   A good value for aScreenNumber may not be required for all event registrations, and should be set to EScreenNumberAllNoEvents when not required.

@post
    -   The method may fail to add the observer to an internal list of observers, due to memory failure.
    -   The observer function will be called when an event specified in the parameter list occurs.
    -   If the screen number is zero or positive, or EScreenNumberAllEvents and the event includes ESOWF_EVENTUPDATED 
        then this client may be expected to handle bit flagged events - see SymbianStreamEventBits. Such an Observer must 
        also send back Available and Displayed notifications to Symbian Stream as required.
@panic NativeStream 1000003 Null Native Stream
@panic NativeStream 1000004 Null aObserver
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrNoMemory    Internal allocation of resources for the Native Stream failed.
    -   KErrArgument    aEvents does not flag any supported events, or aScreenNumber out of range
**/
  IMPORT_C TErrCode SymbianStreamAddExtendedObserver( SymbianStreamType aStream, SymbianStreamCallback aObserver, void* aData, khronos_int32_t aScreenNumber, SymbianStreamEventBits aEvents);


/** Increases the reference on the Native Stream object, extending its lifetime.
@param  aStream Native Stream handle that is to be accessed.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
@post
    -   The object will not be destroyed until an equivalent number of calls to SymbianStreamRemoveReference.
@panic NativeStream 1000003 Null Native Stream
@panic NativeStream 1000004 Null aObserver
**/
  IMPORT_C void SymbianStreamAddReference(SymbianStreamType aStream);

/** Provides Symbian OS-aware clients access to extended features of the currently open buffer via Surface Manager.
@param  aStream Native Stream handle that is to be accessed.
@param  aBufferHandle   Handle to acquired read or write buffer.
@param  aReturnIndex    Returns internal buffer number.
@param  aReturnSurface  Returns internal TSurfaceId.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aBufferHandle must be a valid acquired buffer on that native stream that has not yet been released.
    -   aReturnIndex must not be a NULL pointer, and must point to writable storage for the index value.
    -   aReturnSurface must not be a NULL pointer, and must point to writable storage for the TSurfaceId pointer.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
    -   SymbianStreamGetBufferPointer or SymbianStreamGetBufferId can only be called after a call to SymbianStreamAcquireReadBuffer or SymbianStreamAcquireWriteBuffer, and before the call to SymbianStreamReleaseReadBuffer or SymbianStreamReleaseWriteBuffer.
    -   They can be called multiple times in this period, but each call is not guaranteed to return the same information, and the information returned by an earlier call should not be used after repeating the call or after calling SymbianStreamReleaseReadBuffer or SymbianStreamReleaseWriteBuffer.

@panic NativeStream 1000002 Null return value pointer
@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrBadHandle   aBufferNumber was NULL or not acquired on this Native Stream
**/
  IMPORT_C TErrCode SymbianStreamGetBufferId( SymbianStreamType aStream, SymbianStreamBuffer aBufferHandle, khronos_int32_t* aReturnIndex, const TSurfaceId** aReturnSurface);

/** Finds the persistent Native Stream instance associated with the surface ID. If the association does not exist then this method fails - it does not attempt to create the association.
@param  aId Surface ID handle that is to be accessed.
@param  aReturnStream   Filled with the handle of the equivalent Native Stream
@pre
    -   aId must not be NULL.
    -   aReturnStream must not be a NULL pointer, and must point to writable storage for the handle.
    -   [Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.]
@post
    -   If a Native Stream has been successfully acquired, using SymbianStreamAcquire, for the surface ID, then that Native Stream is returned.
    -   The reference on the stream is increased to ensure it remains live following the method call. See SymbianStreamRemoveReference to release the reference.
    -   If the identified stream is not currently acquired, then NULL is returned, and an error code is set.
    -   The error codes set by this method may not distinguish between Surface IDs that have not been Acquired and bad values for the surface ID, such as IsNull() or destroyed surfaces.

@panic NativeStream 1000001 Null Surface ID
@panic NativeStream 1000002 Null return value pointer
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrNoMemory    Internal allocation of resources for the Native Stream failed.
    -   KErrNotFound    No Native Stream already exists that encapsulates aId.
**/
  IMPORT_C TErrCode SymbianStreamFind( const TSurfaceId* aId, SymbianStreamType* aReturnStream);

/** Returns a pointer to the pixel data associated with the acquired buffer.
@param  aStream Native Stream handle that is to be accessed.
@param  aBuffer Acquired buffer handle.
@param  aReturnPointer  Filled with the handle to use to access the current read buffer
@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aBuffer should not be NULL
    -   aReturnPointer must not be a NULL pointer, and must point to writable storage for the pointer.
    -   aBuffer must be a currently acquired read or write buffer handle on the specified Native Stream.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
    -   SymbianStreamGetBufferPointer or SymbianStreamGetBufferId should not be called more than once for each buffer acquire. Correct cleanup behavior is not guaranteed if these methods are called multiple times.
    -   The pixel data must be mappable to CPU-accessible RAM. For some multimedia content types this may not be possible, and the method will fail. The error reported will be forwarded from Surface Manager.
@post
    If unsuccessful, an error code is returned.
    -   The method may fail if the pixel data cannot be made available in the caller's address space, for a variety of reasons including running out of virtual memory.
    If Successful:
    -   Pixel data is made available and a pointer to the first pixel is returned.
    -   The layout of the pixel data will be as specified by SymbianStreamGetHeader.
    -   The memory may be tagged as read-only if the buffer was generated using AcquireReadBuffer.
    -   This layout implies a maximum size to the buffer data. The client should not access data outside that range of addresses.

@panic NativeStream 1000001 Null Surface ID
@panic NativeStream 1000002 Null return value pointer
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrBadHandle   aBuffer was NULL or not acquired on this Native Stream
    -   <<other>>   mapping of the TSurfaceId failed - reporting the internal error.
**/
  IMPORT_C TErrCode SymbianStreamGetBufferPointer( SymbianStreamType aStream, SymbianStreamBuffer aBuffer, void** aReturnPointer);

/** Returns a description of the format of the image data stored in the Native Stream.
@param  aStream Native Stream handle that is to be accessed.
@param  aWidth  Pointer to fill in width in pixels of accessible buffer.
@param  aHeight Pointer to fill in height in pixels of accessible buffer.
@param  aStride Pointer to fill in offset between rows of pixels.
@param  aFormat Pointer to fill in format code matching well known Symbian OS surface format UIDs.
@param  aFixelSize  Pointer to fill in number of bytes per pixel for the given format.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   The remaining parameters are each either NULL, or point to local data storage which will be modified.
@post
    -   Targets of non-NULL parameters will be filled in with values describing the surface encapsulated by the Native Stream.
    -   [Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.]
    -   In the current implementation, this information is stored locally and can be efficiently accessed.
@panic NativeStream 1000002 Null return value pointer
@panic NativeStream 1000003 Null Native Stream
**/
  IMPORT_C void SymbianStreamGetHeader( SymbianStreamType aStream, khronos_int32_t* aWidth, khronos_int32_t* aHeight, khronos_int32_t* aStride, khronos_int32_t* aFormat, khronos_int32_t* aPixelSize);

/** Returns extra details of the format of the image data stored in the Native Stream.
 This method is intended to allow compositors to access the SurfaceManager hints associated with the 
 stream without using SurfaceManager-explicit code.
 It is possible in future that some of the hint IDs could be resolved locally rather than calling surface manager.  
@prototype  @internal
@param  aStream Native Stream handle that is to be accessed.
@param  aHint   Well known unique ID for specific hint property
@param  aValueReturn    Pointer to fill in current value for property
@param  aMutableReturn  Pointer to fill in to flag whether the property value is fixed or mutable

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aHint may be any UID value, but only hint UIDs are likely to be successful.
    -   aValueReturn may be NULL, in which case the return value will indicate whether the hint is present
    -   aMutableeReturn may be NULL, in which case the client must presume the property may be mutable.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   If the hint aHint is not associated with the underlying surface then an error is returned.
    -   If the hint is available, KErrNone is returned, and if aValueReturn is not NULL, the location pointed to will be filled with the current value of the hint. If not NULL the location pointed to by aMutableReturn indicates whether the hint is mutable.

@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrArgument    hint not found (from  RSurfaceManager::GetSurfaceHint)
    -   <<other>>   error reported from  RSurfaceManager::GetSurfaceHint
**/
  IMPORT_C TErrCode SymbianStreamGetHint( SymbianStreamType aStream, khronos_int32_t aUid, khronos_int32_t* aValueReturn, khronos_bool* aMutableReturn);


/** Debug method which indicates whether a particular screen number has been registered for screen notifications (from SUS).
@param  aScreenNumber   Any valid screen number.
@param  aReturnState    Points to a Pointer to hold the returned registered object.

@pre
    -   aReturnState    may be NULL, or must point to a location to receive the internal object registered with SUS
    -   [Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.]
@post
    -   aReturnState void*  opaque handle is NULL if the given screen number is not registered. The type of the internal object returned by this method is subject to change.
    -   A change in the value of aReturnState in subsequent calls indicates that the screen has been re-registered.
@return Symbian error code
    -   KErrNone    The screen number is registered with SUS.
    -   KErrNotFound    The screen number is not registered.
    -   KErrNotReady    SUS is not active, but the screen would be registered.
    -   KErrAlreadyExists   The screen number is already registered with SUS.
**/
  IMPORT_C TErrCode SymbianStreamHasRegisteredScreenNotifications( khronos_int32_t aScreenNumber,void** aReturnState);

/** Generates a registration with Surface Update Server (SUS) for the given screen number at the given priority.
@param  aScreenNumber   Number of screen to be registered. This number should be acceptable to SUS.
@param  aPriority   Nominal priority of the screen for global updates. Higher values have higher priority.
@param  aInternalVersion    internal compositor version number to be reported to SUS.

@pre
    -   aScreenNumber and aPriority should be within acceptable value ranges for SUS.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
    -   The thread used to register screen notifications must have an active scheduler and generally run in scheduling mode.
@post
    -   Surface Update Server will be started if it has not already been started.
    -   The given screen number is registered or re-registered with SUS at the given priority.
    -   If SUS cannot be started, for example in debug environments, then an error is returned, but a screen update object is still created, as returned by SymbianStreamHasRegisteredScreenNotifications.
    -   The default heap specified when calling this method will be activated when SUS notifies the Native Stream of an update.
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrNoMemory    Internal allocation of resources for the Native Stream failed.
    -   KErrNotReady    SUS was not launched (may occur in unit testing)
    -   KErrAlreadyExists   the given screen number is already registered
    -   <<other>>   as returned by SUS
**/
  IMPORT_C TErrCode SymbianStreamRegisterScreenNotifications( khronos_int32_t aScreenNumber, khronos_int32_t aPriority, khronos_int32_t aInternalVersion);

/** Releases the buffer previously acquired with SymbianStreamAcquireReadBuffer.
@param  aStream Native Stream handle that is to be accessed.
@param  aBuffer Buffer handle that is to be released.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aBuffer must be a buffer handle acquired on this stream using SymbianStreamAcquireReadBuffer and not yet released.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   The non-exclusive buffer lock is released, potentially allowing writers to modify the content of the buffer.
    -   Any memory made available using SymbianStreamGetBufferPointer on this buffer handle may become unmapped from the caller's address space. The pointer must not continue to be used.
    -   Clients using SymbianStreamBufferId to efficiently access multimedia resources should release those resources before calling this method; otherwise they risk the contents being overwritten.
@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrBadHandle   aBuffer is not a currently open read buffer on this Native Stream;
**/
  IMPORT_C TErrCode SymbianStreamReleaseReadBuffer( SymbianStreamType aStream, SymbianStreamBuffer aBuffer);

/** Releases the buffer previously acquired with SymbianStreamAcquireWriteBuffer.
@param  aStream Native Stream handle that is to be accessed.
@param  aBuffer Buffer handle that is to be released.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aBuffer must be a buffer handle successfully acquired on this stream using SymbianStreamAcquireWriteBuffer and not yet released.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   This buffer content becomes the read buffer for all subsequent calls to SymbianStreamAcquireReadBuffer
    -   The exclusive buffer lock is released.
    -   Any memory made available using SymbianStreamGetBufferPointer on this buffer handle may become unmapped from the caller's address space. The pointer should not continue to be used.
    -   Clients using SymbianStreamBufferId to access multimedia resources should release those resources before calling this method; otherwise they risk the changes being ignored.
    -   Any registered observers will be called with the ESOWF_EVENTUpdated notification.
    -   If the surface is multi-buffered, subsequent attempts to SymbianStreamAcquireWriteBuffer will succeed and will select a different buffer to write to, presuming clients do not still have a non-exclusive read lock on the available write buffers.
@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrBadHandle   aBuffer is not a currently open write buffer on this Native Stream;
**/
  IMPORT_C TErrCode      SymbianStreamReleaseWriteBuffer( SymbianStreamType aStream, SymbianStreamBuffer aBuffer);

/** Removes the observer inserted using SymbianStreamAddObserver.
@param  aStream Native Stream handle that is to be accessed.
@param  aObserver   identifies the registered observer.
@param  aData   Client data supplied when registering the observer.
@param  aEvents Events to remove registration from.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   aData should have been provided as the client data when the observer was registered.
    -   aEvents indicates which events should no longer be notified, and should match the original extended events list.
    -   If aEvents is empty then the observer is removed from all events.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   The observer function will no longer be called when the specified events occur, unless the observer was registered more than once, in which case the number of notifications will be reduced.
    -   There is a small window of opportunity that events triggered on one thread may complete after the observer has been removed from another thread. Clients should be aware of this potential false call.
    
@panic NativeStream 1000006 Null for all search parameters
@panic NativeStream 1000003 Null Native Stream
@return Symbian error code
    -   KErrNone    Method Succeeded
    -   KErrNotFound    The observer function was not found for this Native Stream.
    -   KErrArgument    aEvents does not flag any supported events
**/
  IMPORT_C TErrCode SymbianStreamRemoveObserver( SymbianStreamType aStream, void* aData, SymbianStreamEventBits aEvents);

/** Reduces the references on the Native Stream object and potentially destroys the object.
@param  aStream Native Stream handle that is to be accessed.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    -   The recorded references are reduced. If this is the final reference then the Native Stream is destroyed.
@panic NativeStream 1000003 Null Native Stream
**/
  IMPORT_C void      SymbianStreamRemoveReference(SymbianStreamType aStream);

/** Changes the state of the deletion protection flag. The protection state effectively increases the reference, blocking deletion. Direct use of the reference mechanism is preferred.
@deprecated The protection flag is used for off-screen composition in the current compositor implementation. Direct use of the reference mechanism is preferred.
    Parameters:
@param  aStream Native Stream handle that is to be accessed.
@param  aFlag   Set true to activate protection - set false to deactivate protection.

@pre
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
@post
    If the aFlag is different to the internal protection state then the reference is modified:
    -   If aFlag is set then the reference is effectively artificially raised to protect the Native Stream against deletion.
    -   If aFlag is clear then the reference returns to normal mode, and the Native Stream may be destroyed.
@panic NativeStream 1000003 Null Native Stream
**/
  IMPORT_C void SymbianStreamSetProtectionFlag( SymbianStreamType aStream, khronos_bool aFlag);

/** Compares two Native Stream handles for equivalency. 
 It is probable that the actual handle value is not unique for a particular surface, 
 but binary different handles in fact refer to the same internal object.


@param  aStream First Native Stream handle that is to be compared.
@param  aStream2    Second Native Stream handle that is to be compared.

@pre
    -   aStream must be an active Native Stream object, or it may be NULL.
    -   aStream2 must be an active Native Stream object, or it may be NULL.
    -   [Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack. ]
    -   Direct comparison will be performed first. No allocations or system calls will be made to resolve this comparison, returning false where, for example the surface IDs refer to the same Surface Manager controlled object.
@post
    The given handles are compared, and if necessary the internal objects are compared.


@return khronos_bool If they reference the same TSurfaceId then TRUE is returned, else FALSE is returned.
**/
  IMPORT_C khronos_bool      SymbianStreamSame(SymbianStreamType aStream, SymbianStreamType aStream2);

/** Removes the screen number from SUS registration.
    Parameters:
    aScreenNumber   Screen number to be unregistered.

@pre
    -   aScreenNumber should have previously been registered using SymbianStreamRegisterScreenNotifications.
    -   Calls can be safely made from any thread, but the same default heap must be in place for all calls. The thread should have an activated clean-up stack.
    -   Out of range screen numbers will not have been registered, and will naturally not be found to unregister. No specific error code will indicate these screen numbers.
@post
    Notifications of content updated will no longer be received by the Native Stream for that screen number or passed on to any observer.
@return Symbian error code
    -   KErrNone    Method Succeeded - screen unregistered
    -   KErrNotReady    The Surface Update Server is not present (in test harnesses).
    -   KErrArgument    aScreennNum was not registered
**/
  IMPORT_C TErrCode      SymbianStreamUnregisterScreenNotifications(khronos_int32_t aScreenNumber);
  

/** Indication that the stream has been displayed in the scene.  
 This function is called by a client after it has consumed a buffer following an update notification 
 and indicates the state of that consumption.
 
@param  aStream Native Stream handle that is to be accessed.
@param  aEvent  The type of consumption notification event.
@param  aScreenNumber   The screen number to which the event applies.
@param  aSerialNumber  The serial number to which the event applies. Changes after each composition, allowing repeat calls within one composition to be detected
@param  aReturnMask Used by the recipient to enable repeating of the ESOWF_EventDisplayedX event.

@pre
	-	aSerialNumber must be changed after each composition, and allows the Native stream to detect multiple calls from the same composition.
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   This method, or CheckVisible must be called exactly once after receiving ESOWF_EVENTUpdated, for each type of notification requested in the ESOWF_EVENTUpdated call by any context that has registered both SymbianStreamRegisteredScreenNotifications and SymbianStreamAddObserver (or SymbianStreamAddObserver with ESOWF_EventUpdated).
    -   Not calling this method when expected may cause deadlocks, and the result code delivered to SUS after calling the method too often with conflicting result codes is not defined.
    -   aEvent indicates which event has occurred. It may be one or more of:
        -   ESOWF_EventAvailable    Content is available
        -   ESOWF_EventDisplayed    Content is displayed
        -   ESOWF_EventDisplayedX   Content is displayed and repeated
@post
    -   Matching notifications to the Surface Update Server for this screen number may be completed (via callbacks).
    -   If the notification is to be repeated then aReturnMask is modified to indicate which notifications should be repeated.

**/
  IMPORT_C void SymbianStreamProcessNotifications( SymbianStreamType aStream, khronos_int32_t aEvent, khronos_int32_t aScreenNumber, khronos_int32_t aSerialNumber, khronos_int32_t* aReturnMask);
  
/** Indication that the stream is not visible in the scene. 
 This function is called by a client after it has consumed a buffer following an update notification 
 and indicates the state of that consumption was "not visible".
 Currently this is fired for any remaining streams at the end of composition 
 after all positive SymbianStreamProcessNotifications have been made.
@param  aStream Native Stream handle that is to be accessed.
@param  aEvent  The type of consumption notification event.
@param  aScreenNumber   The screen number to which the event applies.
@param  aSerialNumber  The serial number to which the event applies. Changes after each composition, allowing repeat calls within one composition to be detected.

@pre
	-	aSerialNumber must be changed after each composition event, and allows the Native stream to detect multiple calls from the same composition.
    -   aStream must be an active Native Stream object. It must not be NULL.
    -   This method, or ProcessNotifications must be called exactly once after receiving ESOWF_EVENTUpdated, for each type of notification requested in the ESOWF_EVENTUpdated call by any context that has registered both SymbianStreamRegisteredScreenNotifications and SymbianStreamAddObserver (or SymbianStreamAddObserver with ESOWF_EventUpdated).
    -   aEvent indicates which event has occurred. It may be one or more of:
        -   ESOWF_EventAvailable    Content is available
        -   ESOWF_EventDisplayed    Content is displayed
        -   ESOWF_EventDisplayedX   Content is displayed and repeated
@post
    Notifications to the Surface Update Server may be completed (via callbacks).

**/
  IMPORT_C void  SymbianStreamCheckVisible(SymbianStreamType aStream, khronos_int32_t aEvent, khronos_int32_t aScreenNumber, khronos_int32_t aSerialNumber);

  /** This function is called by a client to adjust the shape of the target stream.
   This only effects the results discovered by SymbianStreamGetHeader(),
   and has no effect on the underlying SurfaceManager Surface.
   The flag should only be called when the stream is opened for writing, 
   and it only effects calls to SymbianStreamGetHeader(0 after the write buffer is closed. 
   When the aFlip flag is not set the width, height and stride are passed to clients as discovered during construction. 
   When the aFlip flag is set, the width and height parameters are exchanged for SymbianStreamGetHeader(), 
   and a calculated value for stride is returned.
  @param  aStream Native Stream handle that is to be accessed.
  @param  aFlip  The flip state

  @post
      The flip state is updated when SymbianStreamReleaseWriteBuffer() call is made

  **/
  IMPORT_C void  SymbianStreamSetFlipState(SymbianStreamType aStream, SymOwfBool aFlip);
  
  IMPORT_C TErrCode SymbianStreamGetChunkHandle(SymbianStreamType aStream, TInt* aHandle);
  
#	ifdef __cplusplus
	}
#	endif

#endif  // __SYMBIANSTREAM_H__
