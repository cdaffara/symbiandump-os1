/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Sensor server channel listener implemetation
*
*/


#ifndef SENSRVCHANNELBUFFER_H
#define SENSRVCHANNELBUFFER_H

#include <e32base.h>
#include "sensrvtransaction.h"
#include "senserverchannellistener.h"


class CSensrvChannel;

/**
* Indicates recently added tail when there was outstanding request
* for data block. Index will be replaced
* on next invalidation to point to head index.
*/
const TInt KSensrvChannelBufferUninitializedTail(KMaxTInt);

/**
* Indicates this tail is not being used at all.
*/
const TInt KSensrvChannelBufferUnusedTail(KMinTInt);

/**
* Listener tail index item. Also stores data lost count for
* that listener.
*
* @since S60 5.0
*/
class TSensrvTailIndexItem
    {
    public:
        TSensrvTailIndexItem(CSensrvChannelListener* aListener)
          : iListener(aListener),
            iDataTailIndex(KSensrvChannelBufferUnusedTail),
            iConditionTailIndex(KSensrvChannelBufferUnusedTail),
            iDataLostCount(0),
            iDataAvailable(0),
            iConditionDataAvailable(0)
                {
                }
        
        /**
        * Listener for which this index is.
        * Not own.
        */
        CSensrvChannelListener* iListener;
        
        /**
        * Tail index to buffer, indicating the next valid data
        * for data listening.
        * If tail index is same as buffer head index, then there 
        * is no valid data.
        */
        TInt iDataTailIndex;
        
        /**
        * Tail index to buffer, indicating the next valid data
        * for condition listening.
        * If tail index is same as buffer head index, then there 
        * is no valid data.
        */
        TInt iConditionTailIndex;
                    
        /**
        * Number of data items lost to buffer overwrite
        * since last time data was read by this listener.
        */ 
        TInt iDataLostCount;
        
        /**
        * Indicates if listener has unread data.
        * Needed to separate between no data and data starting
        * from head index, as in both cases tail will be at 
        * head index.
        */
        TBool iDataAvailable;
        
        /**
        * Indicates if listener has unevaluated data.
        * Needed to separate between no data and data starting
        * from head index, as in both cases tail will be at 
        * head index.
        */
        TBool iConditionDataAvailable;
    };


/**
* Implements channel buffer related handling.
*
* Channel buffer is a ring buffer with a couple of special twists:
* - There can be multiple tail pointers (one for each client that is
*   listening to the channel).
* - New data is written by SSY or sensor driver directly to buffer,
*   so any pointer given for writing must have at enough continuous
*   space to fit n items (specified when new pointer is requested).
*   -> This leads to situations where end of the buffer contains no
*      valid data, so there is an end index to indicate end of valid data.
*
* @since S60 5.0
*/
class CSensrvChannelBuffer : public CBase
    {
    public:
        
        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aItemSize Size of single buffered item in bytes
        * @param aBufferLength Number of items that need to be buffered.
        * @return New CSensrvChannelBuffer instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvChannelBuffer* NewL( TInt aItemSize,
                                           TInt aBufferLength);

        /**
        * Destructor.
        */
        virtual ~CSensrvChannelBuffer();

        /**
        * Gets buffer length
        * Called from server thread
        *
        * @since S60 5.0
        * @return Buffer length
        */
        inline TInt Length() const { return iBufferLength; };

        /**
        * Gets pointer to next free block of n items.
        * Adjusts indexes accordingly.
        * Called by SSY thread.
        *
        * @since S60 5.0
        * @param aCount Number of items to get free block for.
        */
        TUint8* GetFreeBlock(TInt aCount);

        /**
        * Validates block of n items starting from the head index.
        * Adjusts indexes accordingly.
        * Called by SSY thread.
        *
        * @since S60 5.0
        * @param aCount Number of items that were written.
        */
        void WroteBlock(TInt aCount);
        
        /**
        * Cancels any outstanding free block request
        * Called by SSY thread.
        *
        * @since S60 5.0
        */
        void CancelFreeBlock();


        /**
        * Add new listener tail index. Index is initialized to
        * KSensrvChannelBufferUninitializedTail, if there is an
        * outstanding request for data block, or iHeadIndex if not.
        * This is done so that first write of data will be ignored,
        * as it was generated before listening started for this
        * listener.
        *
        * Called by server thread.
        *
        * @since S60 5.0
        * @param aListener Pointer to the listener. 
        * @param aTailItem Returns pointer to tail item that can be 
        *        supplied to other methods.
        * @param aState channel listener state.
        * @return Symbian error code
        */
        TInt AddListener(CSensrvChannelListener* aListener,
                         TSensrvTailIndexItem*& aTailItem,
                         CSensrvChannelListener::TSensrvChannelListenerState aState);
        
        /**
        * Removes a listener tail index. 
        * Called by server thread.
        *
        * @since S60 5.0
        * @param aListener Pointer to the listener.
        */
        void RemoveListener(CSensrvChannelListener* aListener);
        
        /**
        * Gets data for transaction and adjusts listener tail 
        * accordingly. Data written can be less than the specified
        * count, if there is less data available. 
        * Called by server thread.
        *
        * @since S60 5.0
        * @param aTailItem Pointer to the tail item.
        * @param aMessageData Reference to message data that is filled.
        *        Note that the data is only guaranteed valid as long as proxy's mutex
        *        is not signaled.
        * @param aCount Maximum number of data items to write.
        */
        void GetMessageData(TSensrvTailIndexItem* aTailItem,
                            CSensrvChannelListener::TChannelDataMessage& aMessageData,
                            TInt aCount);
        
        /**
        * Gets data for condition evaluation and resets condition tail
        * to head index.
        *
        * @since S60 5.0
        * @param aTailItem Pointer to the tail item.
        * @param aDataParts Reference to dataparts object is filled.
        *        Note that the data is only guaranteed valid as long as proxy's mutex
        *        is not signaled.
        */
        void GetConditionData(TSensrvTailIndexItem* aTailItem,
                              CSensrvChannelListener::TChannelDataParts& aDataParts);        
        
        /**
        * Gets the initial data item that can be sent to new listerers 
        * if there is no data available by GetMessageData().
        * Called by server thread.
        *
        * @since S60 5.0
        * @param aMessageData Reference to message data that is filled.
        *        Note that the data is only guaranteed valid as long as proxy's mutex
        *        is not signaled.
        */
        void GetInitialMessageData( CSensrvChannelListener::TChannelDataMessage& aMessageData );
        
    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aItemSize Size of single buffered item in bytes
        * @param aBufferLength Number of items that need to be buffered.
        */
        CSensrvChannelBuffer(TInt aItemSize,
                             TInt aBufferLength);

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();
        
        /**
        * Invalidates range of items starting from head index.
        * Adjusts valid and tail indexes accordingly.
        *
        * @since S60 5.0
        * @param aCount Number of items to invalidate.
        */
        void InvalidateRange(TInt aCount);

        /**
        * Checks how much new data is available for listener.
        * If there is no data amount change, no update is needed.
        *
        * @since S60 5.0
        * @param aTailItem Reference to the tail item.
        * @param aMoreData If ETrue, indicates that update is
        *        potentially increasing the amount of data.
        *        If false, the update is potentially decreasing the 
        *        amount of data.
        * @return Amount of new data available.
        */
        void UpdateDataAvailable(TSensrvTailIndexItem& aTailItem,
                                 TBool aMoreData);

        /**
        * Gets tail index for listener.
        *
        * @since S60 5.0
        * @param aListener Pointer to the listener.
        * @return Index for listener in iTailIndexArray or
        *         KErrNotFound if not found.
        */
        TInt GetTailIndex(CSensrvChannelListener* aListener);
        

#ifdef BUFFER_TRACE_DEBUG        
        /**
        * Prints out the buffer contents with RDebug
        *
        * @since S60 5.0
        */
        void DebugPrintBuffer();
#endif // BUFFER_TRACE_DEBUG

    private: // Member variables
    
        /**
        * Single buffered data item size
        */
        TInt iItemSize;
        
        /**
        * Number of items buffer can hold.
        */
        TInt iBufferLength;
        
        /**
        * Actual buffer.
        * Own.
        */
        HBufC8* iBuffer;
        
        /**
        * Indicates cell where SSY will be writing next.
        */
        TInt iHeadIndex;
        
        /**
        * Indicates first invalid data cell after continuous valid cells.
        * If iHeadIndex == iValidEndIndex, valid buffer space is continuous.
        * If iHeadIndex < iValidEndIndex, buffer has wrapped around.
        */
        TInt iValidEndIndex;
        
        /**
        * Array of listener tail indexes
        */
        RPointerArray<TSensrvTailIndexItem> iTailIndexArray;
        
        /**
        * Indicates the length of free block that has been requested.
        */
        TInt iRequestedFreeBlockLength;

        /**
        * Index of the latest written data item or KErrNotFound. 
        * The item located at this index can be sent to new listeners as initial data if 
        * there is nothing else to sent.
        */
        TInt iLatestWrittenItemIndex;

    };



#endif // SENSRVCHANNELBUFFER_H
