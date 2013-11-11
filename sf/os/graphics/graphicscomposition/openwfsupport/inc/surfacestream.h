// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SurfaceStream.h
// CSurfaceStream declaration

#ifndef SURFACESTREAM_H
#define SURFACESTREAM_H

// INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <e32debug.h>
#include <graphics/surface.h>
#include <pixelformats.h>
#include <graphics/surfacemanager.h>
#include "symbianstream.h"

#if defined(ENABLE_NF_LOGGING)
#define NFLOG(X)  RDebug::Printf X
#else
#define NFLOG(X)
#endif

// CLASS DECLARATION

// Each surface buffer has its corresponding TBufferInfo to hold reference count and memory offset
struct TBufferInfo
	{
	TInt iRefCount;
	TInt iOffset;
	};

// Notification data
struct TNotificationBase
    {
    TRequestStatus* iStatus;
    TThreadId       iThreadId;
    TInt            iBufferNumber;
    TInt            iSerialNumber;
    TInt            iGlobalIndex;
    };

struct TNotificationDisplayed: public TNotificationBase
    {
    TUint32*        iTimeStamp;
    };

struct TNotificationDisplayedX: public TNotificationBase
    {
    TInt            iCount;
    };

struct TNotificationAvailable: public TNotificationBase
    {
    TRequestStatus* iNewStatus;
    TThreadId       iNewThreadId;
    TInt            iNewBufferNumber;
    TInt            iNewGlobalIndex;
    };


class COpenWfcStreamMap;
/**
 *  CSurfaceStream
 * 	internal
 */
NONSHARABLE_CLASS( CSurfaceStream ) : public CBase
	{
    struct TNewGlobalNotifications;
    
	private:
	    class Guard
	        {
	        public:
	        Guard(RFastLock& aLock);
	        ~Guard();
	        private:
	            RFastLock& iLock;
	            RHeap* iHeap;
	        };
	    
	    struct ContentUpdatedParams
	        {
	        ContentUpdatedParams(TInt aBuffer,
	                             TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
	                             TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
	                             TRequestStatus* aStatusConsumed, const TRegion* aRegion, 
	                             TBool aImmediateAvailable,
	                             TInt32 aImmediateVisibility, const TNewGlobalNotifications& aGlobalNotifications);
	        TInt iBuffer;
	        TRequestStatus* iStatusDisplayed;
	        TUint32* iTimeStamp;
	        TRequestStatus* iStatusDispXTimes;
	        TInt* iDisplayedXTimes;
	        TRequestStatus* iStatusConsumed;
	        const TRegion* iRegion;
	        TBool iImmediateAvailable;
	        TInt32 iImmediateVisibility;
	        const TNewGlobalNotifications& iGlobalNotifications;
	        };
	    
public:
    enum CallBackOperations
        {
        EDefaultOperation,
        ECheckVisibleOperation
        };
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CSurfaceStream();

	/**
	 * Two-phased constructor.
	 */
	static CSurfaceStream* NewL(const TSurfaceId& aId);

	/**
	 * Two-phased constructor.
	 */
	static CSurfaceStream* NewLC(const TSurfaceId& aId);
	
	/**
		Helper to resolve handle to stream object
	**/
    static CSurfaceStream* FromHandle(SymbianStreamType aNativeStreamHandle);
	
    /**
     * A helper function that returns the bytes per pixel for a given pixel format UID
     * @param aPixelFormat Pixel format UID to convert
     * @return Positive: bytes per pixel; negative is pixels per byte; 0 is error 
     */
    
    static TInt BytesPerPixel(TUidPixelFormat aPixelFormat);
    /**
		Helper to resolve handle to stream object
	**/
	SymbianStreamType ToHandle();
			
public:
	//Internal helpers
	/**	Returns internal surface ID.
	 * 	
	 * @return surface id asociated with this stream
	 **/
	const TSurfaceId& SurfaceId()const;
	
public:
	//OpenWF-C SI/CT/MIG API implementation

	/**
	 * Increase stream's reference count by one.
	 *
	 */
	 void
	AddReference();

	/**
	 * Decrease stream's reference count by one and destroy
	 * the stream, if the reference count goes to zero.
	 *
	 * All acquired read & write buffers must be released
	 * before calling WFC_Native_Destroy.
	 *
	 */
	void
	ReleaseReference();

	/**
	 * internal
	 *
	 * @return flag if reference count is now zero.
	 *
	 */
	TBool
	RemainingReference();

    /**
     * internal
     *
     * Sets flipped state.
     *
     */
    void
    SetFlipState(TBool aFlip);

    /**
	 * Get stream "frame header". Can be used to query
	 * all or some of the frame properties.
	 *
	 * @param width Pointer to location where width parameter should be saved
	 * @param height Pointer to location where height parameter should be saved
	 * @param stride Pointer to location where stride (row size in bytes)
	 * parameter should be saved
	 * @param pixelSize Pointer to location where pizelSize (pixel size in bytes)
	 * parameter should be saved

	 * Passing a NULL pointer implies that particular
	 * value is of no interest to caller. E.g.
	 *   GetHeader(stream, &w, &h, NULL, NULL, NULL);
	 * would only fetch width & height parameters.
	 */
	void
	GetHeader(khronos_int32_t* width,
	                           khronos_int32_t* height,
	                           khronos_int32_t* stride,
	                           SymOwfPixelFormat* format,
	                           khronos_int32_t* pixelSize);

	/**
	 * Acquires read buffer for stream. For > 1 buffer
	 * streams this function doesn't block, but simply returns
	 * WFC_INVALID_HANDLE if no buffer is available for reading.
	 * For 1 buffer stream the caller is blocked until the buffer
	 * is ready for reading (the reader has committed the buffer,
	 * that is.)
	 *
	 *
	 * @return WFC_INVALID_HANDLE if no buffer is available or
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
	 * Time:   0    5    10   15   20   25
	 * Pframe: 0    1    2    0    1    ...
	 * Cframe: ii00000111112222200000111...
	 */
	SymbianStreamBuffer
	AcquireReadBuffer();

	/**
	 * Releases read buffer.
	 *
	 * When read buffer is released, it is marked as clean to
	 * be written again, unless it is the only committed buffer
	 * in which case it is recycled so that the same buffer
	 * can be read again (as long as no new buffers are committed
	 * by the producer)
	 *
	 * @param buf Buffer handle. Must be valid read buffer handle for
	 * given stream.
     * @return KErrNone if succeessful or KErrBadHandle if buf is not a currently
     *          open write buffer on this stream;
	 */
	TInt
	ReleaseReadBuffer(SymbianStreamBuffer buf);

	/**
	 * Acquires write buffer for stream.
	 *
	 * Returns handle to a buffer that can be used to write
	 * data into stream. If no clean buffer is available,
	 * invalid handle is returned.
	 *
	 *
	 * @return Handle to a writable buffer
	 */
	SymbianStreamBuffer
	AcquireWriteBuffer();

	/**
	 * Releases write buffer to stream.
	 * Released buffer is made new front buffer, i.e., producer is expected
	 * to release buffer is the same order they were acquired.
	 *
	 * @param buf Buffer handle. Must be valid write buffer handle
	 * for given stream.
	 */
	void
	ReleaseWriteBuffer(SymbianStreamBuffer buf);

	/**
	 *  Add event observer for stream. Observers are served in
	 *  first-come-first-serve fashion. That is, newest observer
	 *  is always placed at the end of the chain. If the observer
	 *  is already in the chain, it's popped out and moved to
	 *  the end of the chain.
	 *
	 *  @param observer Observer (callback function) who should
	 *  be notified when something interesting happens in the stream.
	 *  @param data Additional data to pass to callback function
	 *
	 *  @return 0 if successful, -1 if stream handle is invalid, -2 if
	 *  OOM situation occurs.
	 */
	int AddObserver(SymOwfStreamCallback observer,
	                           void* data,MultipleSymbianStreamEventBits aEvents);

	/**
	 *  Remove stream event observer. Observer is removed from
	 *  the stream's event nofitication chain and won't receive
	 *  any events from the stream afterwards.
	 *
	 *  @param observer Observer (callback function)
	 *
	 *  @param 0 if successful, -1 otherwise
	 */
	int RemoveObserver(SymOwfStreamCallback observer,
            		void* aData,MultipleSymbianStreamEventBits aEvents);
	
	/** 
	 * Returns pointer to pixel buffer.
	 *
	 * @param buffer Handle of buffer
	 */
	void*
	GetBufferPtrL(SymbianStreamBuffer buffer);
	
	/**  
	 * Undocumented protection flag
	 *
	 * @param flag enable disable protection
	 */
	void
	SetProtectionFlag(TBool flag);

    TInt BufferHandleToIndex(SymbianStreamBuffer aBuff);
    
    /**
     *  Add event observer for stream. Observers are served in
     *  first-come-first-serve fashion. That is, newest observer
     *  is always placed at the end of the chain. 
     *
     *  @param observer Observer (callback function) who should
     *  be notified when something interesting happens in the stream.
     *  @aEvent The event corresponding to the observer
     *  @aScreenNumber The context identifier (screen number)
     *  @param data Additional data to pass to callback function
     *
     *  @return KErrNone if successful
     *          KErrArgument if un unknown event is registered
     *          KErrOverflow if the observer was already registered
     *          An other system wide error if container couldn't be appended
     *  OOM situation occurs.
     */
    int AddObserver(SymbianStreamCallback aObserver, 
                    TInt32 aEvent,
                    TInt32 aScreenNumber,
                    void* aData);

    /**
     *  Remove stream event observer. Observer is removed from
     *  the stream's event nofitication chain and won't receive
     *  any events from the stream afterwards.
     *
     *  @param aObserver The callback function
     *  @param aEvent The event id corresponding to the observer
     *  @param aScreenNumber The screen number
     *  @param aData Data must uniquely identify the observer
     *
     *  @param KErrNone if successful, -1 otherwise
     *         KErrArgument if un unknown event passed as parameter
     *         KErrNotFount if the event is not found
     */
    int RemoveObserver(TInt32 aEvents, void* aData);
    
    /*!
     *  Notifies the observers not associated with a context.
     *  The aim is to support legacy SI behavior.
     *  
     *  @param aEvent Observer identifier
     *
     */
    int NotifyObservers(TInt32 aEvent);

    /**
     *  Implements surface content notifications.
     *
     *  When the contents of a surface change, this function gets called
     *  MCompositionSurfaceUpdate implementation. 
     *
     * @param   aSurface            The surface that has been updated.
     * @param   aBuffer             The buffer of the surface to be used in
     *                              composition. Integer starting from 0.
     * @param   aRegion             The sub-area that has the updates. If NULL, the
     *                              whole surface is considered changed.
     * @param   aStatusConsumed     A request status object or NULL. If not NULL, then the
     *                              request status is completed once the backend
     *                              does not anymore need the contents of the
     *                              surface to render the update. This may happen
     *                              before actually displaying the finished frame.
     * @param   aStatusDisplayed    This is signaled after the composited frame
     *                              is posted the to display for the first time after
     *                              the update. After this the value in
     *                              aTimeStamp is valid, if the value in the
     *                              status object is KErrNone. Can be NULL, if
     *                              no signal is desired.
     * @param   aTimeStamp          Value of the User::FastCounter() right after the
     *                              display refresh that signaled aStatusDisplayed.
     * @param   aStatusDispXTimes   This is signaled when the surface has been on
     *                              the screen for aDisplayedXTimes refreshes,
     *                              including the update that signaled aStatusDisplayed.
     *                              Can be NULL, if no signal is wanted.
     * @param   aDisplayedXTimes    The number of refreshes after which aStatusDispXTimes
     *                              is signaled or NULL. If values is provided, it must be
     *                              >= 1.
     * @param   aScreenNumber       Uniquelly identifies the context (composer)
     */
    void SetNewNotifications(TInt            aBuffer,
                             TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                             TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                             TRequestStatus* aStatusConsumed, const TRegion* aRegion, 
                             TInt32 aScreenNumber);
    
    /**
     *  Implements surface content notifications.
     *
     *  When the contents of a surface change, this function gets called
     *  MCompositionSurfaceUpdate implementation. 
     *
     * @param   aSurface            The surface that has been updated.
     * @param   aBuffer             The buffer of the surface to be used in
     *                              composition. Integer starting from 0.
     * @param   aRegion             The sub-area that has the updates. If NULL, the
     *                              whole surface is considered changed.
     * @param   aStatusConsumed     A request status object or NULL. If not NULL, then the
     *                              request status is completed once the backend
     *                              does not anymore need the contents of the
     *                              surface to render the update. This may happen
     *                              before actually displaying the finished frame.
     * @param   aStatusDisplayed    This is signaled after the composited frame
     *                              is posted the to display for the first time after
     *                              the update. After this the value in
     *                              aTimeStamp is valid, if the value in the
     *                              status object is KErrNone. Can be NULL, if
     *                              no signal is desired.
     * @param   aTimeStamp          Value of the User::FastCounter() right after the
     *                              display refresh that signaled aStatusDisplayed.
     * @param   aStatusDispXTimes   This is signaled when the surface has been on
     *                              the screen for aDisplayedXTimes refreshes,
     *                              including the update that signaled aStatusDisplayed.
     *                              Can be NULL, if no signal is wanted.
     * @param   aDisplayedXTimes    The number of refreshes after which aStatusDispXTimes
     *                              is signaled or NULL. If values is provided, it must be
     *                              >= 1.
     * @param   aScreenNumber       Uniquelly identifies the context (composer)
     */
    void SetAllNotifications(TInt            aBuffer,
                             TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                             TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                             TRequestStatus* aStatusConsumed, const TRegion* aRegion);
    
    /**
     *   Process the notifications by accessing the information stored in the observer container.
     *  
     *  This method is expected to be called from the observer context every time composer has finished processing
     *  a stream and the rigger condition is met.
     *  
     *  @param aEvent        Events map to identify the observer to be processed.
     *  @param aScreenNumber Screen ID used to identify the target composer that invokes the method
     *  @param aOperation    The Operation expected to be executed
     *  @param aSerialNumber A number used to identify the composition operation
     *  @param aReturnMask  Parameter to be retrieved by composer, representing the event to be processed
     *                       when composed next time. A new composition is automatically triggered.
     *
     */
    void ProcessNotifications(TInt32 aEvent, TInt32 aScreenNumber, TInt32 aOperation, TInt32 aSerialNumber, TInt32* aReturnMask);

    /**
     *    A function that checks the validity of the new buffer provided by SUS
     * 
     *
     * When the contents of a surface change, this function gets called by the MCompositionSurfaceUpdate implementation.
     * If the buffer is invalid all request statuses will be completed with KErrArgument
     *
     * @param   aBuffer             The buffer of the surface to be used in
     *                              composition. Integer starting from 0.
     * @param   aStatusConsumed     A request status object or NULL.
     * @param   aStatusDisplayed    A request status object or NULL.
     * @param   aStatusDispXTimes   A request status object or NULL.
     * 
     * @return  KErrNone if successful
     *          KErrArgument if aBuffer parameter is invalid
     *
     */
    TInt CheckBufferNumber(TInt aBuffer,
                           TRequestStatus* aStatusDisplayed,
                           TRequestStatus* aStatusDispXTimes,
                           TRequestStatus* aStatusConsumed);
    
    TInt GetChunkHandle();
    
private:
    /**
     *   Notifies the composer that the content has been updated.
     *  
     *  
     *  @param aScreenNumber Screen ID used to identify the target composer
     *  @param aOp The Operation expected the callback to execute
     *  @param aParam Parameter containing aaditional information to push/pop from targetet composer
     *
     */
    TBool NotifyComposerContext(TInt32 aScreenNumber, TInt aOp, SYMOWF_CONTENT_UPDATED_PARAM* aParam);
    
    /**
     *   Notifies the composer that the content is in process to be updated. The composer will have to not access
     *  observer related information
     *  
     *  
     *  Note that while calling the callback the context update mutex is acquired.
     *  
     *  @param aScreenNumber Screen ID used to identify the target composer
     *  @param aBufferNum The buffer number to be updated 
     *  @param aUpdatedFlags The events that triggers this function call
     *  @param aRegion The sub-area that has the updates. If NULL, the whole surface is considered changed.
     */
    TBool StartUpdateNotifications(TInt aScreenNumber, SYMOWF_CONTENT_UPDATED_PARAM& param);
    
    /**
     *   Notifies the composer that process of updating the content has finisshed
     *  
     *  The composer releases the content mutex and triggers a new composition
     *  
     *  @param aScreenNumber Screen ID used to identify the target composer
     *  @param aBufferNum The buffer number to be updated 
     *  @param aUpdatedFlags The events that triggers this function call
     *  @param aRegion The sub-area that has the updates. If NULL, the whole surface is considered changed.
     */
    TBool EndUpdateNotifications(TInt aScreenNum, 
                                 TInt aBufferNum, 
                                 TInt32 aUpdatedFlags, 
                                 const TRegion* aRegion);
    
    /**
     *   Notifies the composer that content has been updated
     *  
     *  It is used to support old SI behaviour. The content update mutex is acuired 
     *  and released while a new composition is triggered
     *  
     *  @param aScreenNumber Screen ID used to identify the target composer
     *  @param aBufferNum The buffer number to be updated 
     *  @param aUpdatedFlags The events that triggers this function call
     *  @param aRegion The sub-area that has the updates. If NULL, the whole surface is considered changed.
     */
    TBool UpdateNotifications(TInt aScreenNum, 
                              TInt aBufferNum, 
                              TInt32 aUpdatedFlags, 
                              const TRegion* aRegion);
    
    /**
     *   Processes the available observer
     *  
     *  @param aEvent               Events map to identify the operation to be executed.
     *  @param aSerialNumber        A number used to identify the composition operation
     *  @param ContentUpdatedParams Packs the information used when a new request from SUS has to be processed
     *  @param aCallBackData        The observer data stored in the container of observers
     *  @param aReturnMask         Parameter to be retrieved by composer, representing the event to be processed
     */
    void Available(TInt32 aEvent, TInt32 aSerialNumber, ContentUpdatedParams* aParams, void* aCallBackData, TInt32* aReturnMask);
    /**
     *   Processes the available observer
     *  
     *  @param aEvent               Events map to identify the operation to be executed.
     *  @param aSerialNumber        A number used to identify the composition operation
     *  @param ContentUpdatedParams Packs the information used when a new request from SUS has to be processed
     *  @param aCallBackData        The observer data stored in the container of observers
     *  @param aReturnMask         Parameter to be retrieved by composer, representing the event to be processed
     */
    void Displayed(TInt32 aEvent, TInt32 aSerialNumber, ContentUpdatedParams* aParams, void* aCallBackData, TInt32* aReturnMask);
    /**
     *   Processes the available observer
     *  
     *  @param aEvent               Events map to identify the operation to be executed.
     *  @param aSerialNumber        A number used to identify the composition operation
     *  @param ContentUpdatedParams Packs the information used when a new request from SUS has to be processed
     *  @param aCallBackData        The observer data stored in the container of observers
     *  @param aReturnMask         Parameter to be retrieved by composer, representing the event to be processed
     */
    void DisplayedXTimes(TInt32 aEvent, TInt32 aSerialNumber, ContentUpdatedParams* aParams, void* aCallBackData, TInt32* aReturnMask);
    /**
     *   Function used to reset observer data fields
     *  
     *  @param aEvent               Event identifier for whis the operation is to be executed.
     *  @param aCallBackData        The observer data stored in the container of observers
     */
    void ResetCallBackData(void* aCallBackData, TInt32 aEvent);
    /**
     *   Cancels all active notifications by completeting the associated requests
     */
    void CancelNotifications();
	
    void SetNotifications(TInt            aBuffer,
                          TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                          TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                          TRequestStatus* aStatusConsumed, const TRegion* aRegion, 
                          TInt32 aScreenNumber, const TNewGlobalNotifications& aGlobalNotifications);
    
	void RequestComplete(TThreadId& aThreadId, TRequestStatus*& aRequestStatus, TInt& aGlobalIndexArray, TInt aStatus);
	/**
	 * Constructor for performing 1st stage construction
	 */
	CSurfaceStream();

	/**
	 * Symbian's default constructor for performing 2nd stage construction
	 */
	void ConstructL(const TSurfaceId& aId);
	
	TInt AddNewGlobalNotification(TRequestStatus* aStatusDisplayed, TInt aAssociatedScreens);
	void SetReadBufferIndex(TInt aIndex);
	TInt GetReadBufferIndex();
	TInt GetWriteBufferIndex();
	TInt Stride(TInt aWidth, TUidPixelFormat aPixelFormat);	
	static SymbianStreamBuffer IndexToReadHandle(TInt aIndex);
	static SymbianStreamBuffer IndexToWriteHandle(TInt aIndex);
	
	static COpenWfcStreamMap& GetSingletonL();
	
	void SurfaceInfoL(const TSurfaceId& aSurface, RSurfaceManager::TInfoBuf& aInfo);
private:
    enum FlippedTarget
        {
        EFlipNotSet,
        EFlippedTargetNormal,
        EFlippedTargetFlipped
        };
    
private:
	// Each surface buffer has its corresponding TBufferInfo to hold reference count and memory offset
	struct TBufferInfo
		{
		TInt iRefCount;
		TInt iOffset;
		};
	
private:
	struct TCallBackEntry;
    struct TGlobalNotification;
	TSurfaceId	iSurfaceId;				//< Surface ID associated with stream.
	TSurfaceId  iStreamProxySurfaceId;	//< Surface ID generated to represent stream 
	TInt		iRefCount;
	RFastLock	iRefCountMutex;
	TInt		iReadBuffer;
    RChunk      iBufferChunk;
	TBufferInfo* iBufferInfo; //< Array of buffer info
	RSurfaceManager::TSurfaceInfoV01 iInfo;
	static const TInt BUFFER_READ_HANDLE_BASE = 0x100;
	static const TInt BUFFER_WRITE_HANDLE_BASE = 0x200;
	static const TInt BUFFER_WRITE_UPDATE_OVERWRITE = -1;
	TInt iAcquiredWriteBuffer;
	TAny*		iCallBackHighestPriority;
	RArray<TCallBackEntry>	iCallBacks;
    RFastLock   iCallBacksMutex;
	TBool		iProtected;
    RArray<TGlobalNotification>  iGlobalNotifications;
    TInt        iNumberOfScreenAttachedAvailableNotif;
    TInt        iNumberOfScreenAttachedDisplayedNotif;
    TInt        iNumberOfScreenAttachedDisplayedXNotif;
    FlippedTarget iFlipState;
    FlippedTarget iNewFlip;
	};

#endif // SURFACESTREAM_H
