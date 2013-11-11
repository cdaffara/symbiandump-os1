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
* Description:  Sensor server channel buffer implementation
*
*/


#include "sensrvdefines.h"
#include "sensrvchannel.h"
#include "sensrvpluginproxy.h"
#include "sensrvchannelbuffer.h"
#include "sensrvchannellistener.h"


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelBuffer* CSensrvChannelBuffer::NewL(TInt aItemSize,
                                                 TInt aBufferLength)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::NewL(aItemSize: %d, aBufferLength: %d)" ), aItemSize, aBufferLength ) );

    CSensrvChannelBuffer* self = new( ELeave ) CSensrvChannelBuffer(aItemSize, aBufferLength);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelBuffer::CSensrvChannelBuffer(TInt aItemSize,
                                           TInt aBufferLength)
    : iItemSize(aItemSize),
      iBufferLength(aBufferLength < KSensrvMinimumBufferLength ? KSensrvMinimumBufferLength : aBufferLength),
      iLatestWrittenItemIndex( KErrNotFound )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::CSensrvChannelBuffer()" )) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::CSensrvChannelBuffer - return" ) ) );
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::ConstructL()" ) ) );

    // Check parameter validity here, as constructor cannot leave.
    TInt bufferSize = iItemSize*iBufferLength;
    if (   iItemSize <= 0 
        || iBufferLength <= 0 
        || (KMaxTInt/iItemSize) <= iBufferLength )
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::ConstructL - ERROR: Invalid parameters" ) ) );
        User::Leave(KErrArgument);
        }

    // Construct buffer
    iBuffer = HBufC8::NewL(bufferSize);

    BUFFER_TRACE( ( _L( "#### CSensrvChannelBuffer::ConstructL(): iBufferLength: %d" ), iBufferLength ) );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelBuffer::~CSensrvChannelBuffer()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::~CSensrvChannelBuffer()" ) ) );

    delete iBuffer;
    
    iTailIndexArray.ResetAndDestroy();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::~CSensrvChannelBuffer - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Get pointer to next free block (the beginning of head index item)
// Invalidate next aCount items and move indexes accordingly.
// ---------------------------------------------------------------------------
//
TUint8* CSensrvChannelBuffer::GetFreeBlock(TInt aCount)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetFreeBlock(aCount: %d)" ), aCount ) );

    __ASSERT_ALWAYS(aCount <= iBufferLength, User::Panic(KSensrvPanicCategory, ESensrvPanicOutOfBounds));
    __ASSERT_ALWAYS(!iRequestedFreeBlockLength, User::Panic(KSensrvPanicCategory, ESensrvPanicPreviousBlockNotYetWritten));

    // Check if there is enough space in the end of the buffer to allocate continuous
    // block of aCount items
    if (iHeadIndex + aCount > iBufferLength)
        {
        // No room, invalidate the end of buffer and move head to start of buffer.
        InvalidateRange(iBufferLength);
        iHeadIndex = 0;
        }
    else
        {
        // There is room at end of buffer, do nothing
        }
    
    // Invalidate desired range
    InvalidateRange(aCount);

    // Get head pointer
    TUint8* ptr = const_cast<TUint8*>(iBuffer->Ptr());
    ptr += iHeadIndex*iItemSize;
    
    iRequestedFreeBlockLength = aCount;
    
    DEBUG_PRINT_BUFFER;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetFreeBlock - return %d" ), ptr ) );
    
    return ptr;
    }

// ---------------------------------------------------------------------------
// Validate specified count of items and adjust indexes.
// Head index is moved aCount items forward, as well as iValidEndIndex,
// if that is at head.
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::WroteBlock(TInt aCount)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::WroteBlock(aCount: %d)" ), aCount ) );

    __ASSERT_ALWAYS(iRequestedFreeBlockLength >= aCount, User::Panic(KSensrvPanicCategory, ESensrvPanicRequestedFreeBlockTooSmall));

    // Negative count indicates an error
    TInt total(iHeadIndex + aCount);
    
    if (aCount < 0 || total > iBufferLength)
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::WroteBlock - Invalid count: %d, iBufferLength: %d, iHeadIndex: %d" ), aCount, iBufferLength, iHeadIndex ) );
        }
    else if (aCount)
        {
        // Store the latest written item index to be used as initial data
        iLatestWrittenItemIndex = total - 1;
        
        iHeadIndex = total;

        if (iValidEndIndex < iHeadIndex)
            {
            iValidEndIndex = iHeadIndex;
            }
            
        if (iHeadIndex == iBufferLength)    
            {
            iHeadIndex = 0;
            }
        
        for(TInt i = 0; i < iTailIndexArray.Count(); i++)
            {
            UpdateDataAvailable(*(iTailIndexArray[i]), ETrue);
            }
        }
    else
        {
        // Nothing to do when count is zero
        }

    iRequestedFreeBlockLength = 0;

    DEBUG_PRINT_BUFFER;
    }

// ---------------------------------------------------------------------------
// Cancels any outstanding free block request
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::CancelFreeBlock()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::CancelFreeBlock()" ) ) );

    iRequestedFreeBlockLength = 0;

    DEBUG_PRINT_BUFFER;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::CancelFreeBlock - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Adds a listener tail index
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelBuffer::AddListener(CSensrvChannelListener* aListener,
                                       TSensrvTailIndexItem*& aTailItem,
                                       CSensrvChannelListener::TSensrvChannelListenerState aState)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::AddListener(aListener: 0x%x, <retval>, aState: %d)" ), aListener, aState ) );

    __ASSERT_ALWAYS(aListener, User::Panic(KSensrvPanicCategory, ESensrvPanicNullListener));
    __ASSERT_ALWAYS(GetTailIndex(aListener) == KErrNotFound, User::Panic(KSensrvPanicCategory, ESensrvPanicDuplicateListener));
    
    aTailItem = new TSensrvTailIndexItem(aListener);

    TInt err(KErrNoMemory);

    if (aTailItem)
        {
        if (aState == CSensrvChannelListener::EChannelListenerStateDataListening)
            {
            // If no outstanding request for data block (i.e. first listener),
            // initialize index to head index.
            if (iRequestedFreeBlockLength)
                {
                aTailItem->iDataTailIndex = KSensrvChannelBufferUninitializedTail;
                }
            else
                {
                aTailItem->iDataTailIndex = iHeadIndex;
                }
            }
        else if (aState == CSensrvChannelListener::EChannelListenerStateConditionListening)
            {
            // Unlike data, conditions are available immediately, regardless of 
            // current listening state. Each condition set will individually
            // be evaluated if it should ignore new data.
            aTailItem->iConditionTailIndex = iHeadIndex;
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::AddListener - ERROR: Invalid state: %d" ), aState ) );
            }


        err = iTailIndexArray.Append(aTailItem);
        
        if (err != KErrNone)
            {
            delete aTailItem;
            aTailItem = NULL;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::AddListener - return %d, aTailItem: 0x%x" ), err, aTailItem ) );
   
    return err;
    }

// ---------------------------------------------------------------------------
// Removes a listener tail index
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::RemoveListener(CSensrvChannelListener* aListener)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::DeleteListener(aListener: 0x%x)" ), aListener ) );

    __ASSERT_ALWAYS(aListener, User::Panic(KSensrvPanicCategory, ESensrvPanicNullListener));
    TBool removed(EFalse);

    for(TInt i = 0; !removed && i < iTailIndexArray.Count(); i++)
        {
        if (iTailIndexArray[i]->iListener == aListener)
            {
            delete iTailIndexArray[i];
            iTailIndexArray.Remove(i);
            removed = ETrue;
            }
        }
    
    // Clear requested free block length    
    if( iTailIndexArray.Count() == 0 )
        {
        CancelFreeBlock();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::DeleteListener - return (removed: %d)" ), removed ) );
    }

// ---------------------------------------------------------------------------
// Get new data for listener to transaction data item
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::GetMessageData(TSensrvTailIndexItem* aTailItem,
                                          CSensrvChannelListener::TChannelDataMessage& aMessageData,
                                          TInt aCount)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetMessageData(aCount: %d)" ), aCount ) );
    
    if( aTailItem == NULL )
        return;
    
    TBool done(EFalse);
    TInt actualCount(aCount > aTailItem->iDataAvailable ? aTailItem->iDataAvailable : aCount);

    // Get tail item pointer
    TUint8* startPtr = const_cast<TUint8*>(iBuffer->Ptr());
    TUint8* tailPtr = startPtr + aTailItem->iDataTailIndex*iItemSize;

    TPtrC8* activePart = &aMessageData.iFirstDataPart;

    aMessageData.iDataLostCount = aTailItem->iDataLostCount;
    
    while (!done)
        {
        TInt available(0);

        // Adjust tail index
        if (!aTailItem->iDataAvailable)
            {
            // No new data available, do nothing
            BUFFER_TRACE((_L("#### No more data available")));
            done = ETrue;
            }
        else if ( aTailItem->iDataTailIndex < iHeadIndex )
            {
            // Data available up to head index
            available = iHeadIndex - aTailItem->iDataTailIndex;
            TInt remainder = actualCount - aMessageData.iDataItemCount;
            if (remainder < available)
                {
                available = remainder;
                }
            BUFFER_TRACE((_L("#### More data was available")));
            done = ETrue;
            }
        else // iTailIndexArray[index].iDataTailIndex >= iHeadIndex
            {
            // New data available up to iValidEndIndex and possibly more again from start
            // Only possible to get here on first pass.
            available  = iValidEndIndex - aTailItem->iDataTailIndex;
            if (actualCount <= available)
                {
                available = actualCount;
                done = ETrue; // No second pass needed
                BUFFER_TRACE((_L("#### Just single block of data was available")));
                }
            else
                {
                BUFFER_TRACE((_L("#### Possibly second block of data available")));
                }
            } 

        aTailItem->iDataTailIndex += available;
        activePart->Set(tailPtr, available*iItemSize);
        aMessageData.iDataItemCount += available;
        
        // Roll tail index over to zero if at end of buffer,
        // unless valid buffer ends at head index, in which case
        // SSY has reserved a block beginning from iValidEndIndex,
        // and we should wait for new data at current index.
        if (aTailItem->iDataTailIndex == iValidEndIndex && iValidEndIndex != iHeadIndex)
            {
            aTailItem->iDataTailIndex = 0;
            tailPtr = startPtr;
            activePart = &aMessageData.iSecondDataPart;
            }

        UpdateDataAvailable(*aTailItem, EFalse);
        }

    aTailItem->iDataLostCount = 0;
    
    DEBUG_PRINT_BUFFER;
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetMessageData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Get new data for condition listener.
// Always get all the data available.
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::GetConditionData(TSensrvTailIndexItem* aTailItem,
                                            CSensrvChannelListener::TChannelDataParts& aDataParts)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetConditionData" ) ) );

    if( aTailItem == NULL )
        return;
    
    TInt actualCount1(0);
    TInt actualCount2(0);
    
    // Condition data is always cleared at every new data notification, so 
    // it is unlikely that it ever will build up to more than maximum buffering count,
    // or have two parts, unless SSY thread somehow manages to call BufferFilled twice
    // or more times in a row without new data handling on server main thread. 
    // However, it is theoretically possible, so check the available count
    // and need for multiple parts.
    // If further optimization of condition checking is required, this possibility can 
    // probably be quite safely ignored and second data part gotten rid of.
    if (!aTailItem->iConditionDataAvailable)
        {
        // No new data available, do nothing
        BUFFER_TRACE((_L("#### No more condition data available")));
        }        
    else if (aTailItem->iConditionTailIndex >= iHeadIndex)
        {
        // Condition tail after (or at) head, so get up to end of the buffer
        // and again from the beginning up to the head
        actualCount1 = iValidEndIndex - aTailItem->iConditionTailIndex;
        actualCount2 = iHeadIndex;
        BUFFER_TRACE((_L("#### Two parts of condition data available")));
        }
    else
        {
        // Condition tail before head, so get up to head
        actualCount1 = iHeadIndex - aTailItem->iConditionTailIndex;
        BUFFER_TRACE((_L("#### One part of condition data available")));
        }
    
    // Get data parts
    TUint8* startPtr = const_cast<TUint8*>(iBuffer->Ptr());
    TUint8* tailPtr = startPtr + aTailItem->iConditionTailIndex*iItemSize;

    aDataParts.iFirstDataPart.Set(tailPtr, actualCount1*iItemSize);   
    aDataParts.iSecondDataPart.Set(startPtr, actualCount2*iItemSize);        

    // Reset tail
    aTailItem->iConditionTailIndex = iHeadIndex;
    aTailItem->iConditionDataAvailable = 0;

    DEBUG_PRINT_BUFFER;
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetConditionData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Invalidates range of items starting from head index.
// Adjusts valid and tail indexes accordingly.
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::InvalidateRange(TInt aCount)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::InvalidateRange(aCount: %d)" ), aCount ) );
    
    TInt invalidEnd(iHeadIndex+aCount);
    
    // Data loss occurs for items between iHeadIndex and invalidIndex,
    // but only up to iValidEndIndex
    TInt dataLossIndex(0);
    if (invalidEnd > iValidEndIndex)
        {
        dataLossIndex = iValidEndIndex;
        }
    else
        {
        dataLossIndex = invalidEnd;
        }

    // Update tail indexes and data lost for tail indexes in invalidated area
    for(TInt i = 0; i < iTailIndexArray.Count(); i++)
        {
        if (   iTailIndexArray[i]->iDataAvailable 
            && iTailIndexArray[i]->iDataTailIndex >= iHeadIndex 
            && iTailIndexArray[i]->iDataTailIndex < invalidEnd)
            {
            // Indicate missed data
            iTailIndexArray[i]->iDataLostCount += dataLossIndex - iTailIndexArray[i]->iDataTailIndex;
     
            // Move tail to end of invalid range
            if (invalidEnd >= iValidEndIndex)
                {
                // Roll tail to zero index at end of valid buffer.
                iTailIndexArray[i]->iDataTailIndex = 0;
                }
            else
                {
                iTailIndexArray[i]->iDataTailIndex = invalidEnd;
                }
            
            // It is necessary to update available amount after invalidation ate some of it.
            UpdateDataAvailable(*(iTailIndexArray[i]), EFalse);
            }
        else
            {
            // Do not adjust, as tail is before head, at head (with no unread data),
            // or beyond invalidation range.
            // -> It cannot be affected by the invalidation
            }
        
        // Update condition tail index. Should very rarely if ever come here.
        // Only possible if SSY thread is able to make multiple buffer fills
        // before server gets to handle new data available.
        if (   iTailIndexArray[i]->iConditionDataAvailable 
            && iTailIndexArray[i]->iConditionTailIndex >= iHeadIndex
            && iTailIndexArray[i]->iConditionTailIndex < invalidEnd )
            {
            if (invalidEnd >= iValidEndIndex)
                {
                // Roll tail to zero index at end of valid buffer.
                iTailIndexArray[i]->iConditionTailIndex = 0;
                }
            else
                {
                iTailIndexArray[i]->iConditionTailIndex = invalidEnd;
                }
            
            // It is necessary to update available amount after invalidation ate some of it.
            UpdateDataAvailable(*(iTailIndexArray[i]), EFalse);
            }
        else
            {
            // Do not adjust, as tail is before head, at head (with no unread data),
            // or beyond invalidation range.
            // -> It cannot be affected by the invalidation
            }
        }
        
    // Valid data ends at head index, unless it was beyond invalidated range.    
    if (invalidEnd >= iValidEndIndex)
        {
        iValidEndIndex = iHeadIndex;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::InvalidateRange - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Checks how much new data is available for listener.
//
// The data available is iHeadIndex - iDataTailIndex if iHeadIndex
// is larger than iTailIndex, or iBufferLength - iDataTailIndex + iHeadIndex,
// if iHeadIndex is smaller than iTailIndex.
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::UpdateDataAvailable(TSensrvTailIndexItem& aTailItem,
                                               TBool aMoreData)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::UpdateDataAvailable(aTailItem.iListener: 0x%x, aMoreData: %d)" ), aTailItem.iListener, aMoreData ) );

    // Update data available
    if (aTailItem.iDataTailIndex != KSensrvChannelBufferUnusedTail)
        {
        if (aTailItem.iDataTailIndex == KSensrvChannelBufferUninitializedTail)
            {
            aTailItem.iDataAvailable = 0;
            aTailItem.iDataTailIndex = iHeadIndex;
            }
        else if (aTailItem.iDataTailIndex == iHeadIndex)
            {
            if (aMoreData)
                {
                // The amount of available data increased with this update;
                // being at head index means that buffer is full
                aTailItem.iDataAvailable = iValidEndIndex;
                }
            else
                {
                // The amount of data potentially decreased with this update;
                // being at headindex means there is no data available
                aTailItem.iDataAvailable = 0;
                }
            }
        else if (aTailItem.iDataTailIndex < iHeadIndex)
            {
            aTailItem.iDataAvailable = iHeadIndex - aTailItem.iDataTailIndex;
            }
        else // aTailItem.iDataTailIndex > iHeadIndex
            {
            aTailItem.iDataAvailable = iValidEndIndex - aTailItem.iDataTailIndex + iHeadIndex;
            }    
        }
    
    // Update condition data available
    if (aTailItem.iConditionTailIndex != KSensrvChannelBufferUnusedTail)
        {
        if (aTailItem.iConditionTailIndex == KSensrvChannelBufferUninitializedTail)
            {
            aTailItem.iConditionDataAvailable = 0;
            aTailItem.iConditionTailIndex = iHeadIndex;
            }
        else if (aTailItem.iConditionTailIndex == iHeadIndex)
            {
            if (aMoreData)
                {
                // The amount of available data increased with this update;
                // being at head index means that buffer is full
                aTailItem.iConditionDataAvailable = iValidEndIndex;
                }
            else
                {
                // The amount of data potentially decreased with this update;
                // being at headindex means there is no data available
                aTailItem.iConditionDataAvailable = 0;
                }
            }
        else if (aTailItem.iConditionTailIndex < iHeadIndex)
            {
            aTailItem.iConditionDataAvailable = iHeadIndex - aTailItem.iConditionTailIndex;
            }
        else // aTailItem.iConditionTailIndex > iHeadIndex
            {
            aTailItem.iConditionDataAvailable = iValidEndIndex - aTailItem.iConditionTailIndex + iHeadIndex;
            }    
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::UpdateDataAvailable - return. Data available: %d" ), aTailItem.iDataAvailable ) );
    }
    
// ---------------------------------------------------------------------------
// Returns tail index for listener
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelBuffer::GetTailIndex(CSensrvChannelListener* aListener)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetTailIndex(aListener: 0x%x)" ), aListener ) );
 
    TInt index(0);
    
    for(TInt i = 0; !index && i < iTailIndexArray.Count(); i++)
        {
        if (iTailIndexArray[i]->iListener == aListener)
            {
            index = i;
            }
        }
        
    if (!index)        
        {
        index = KErrNotFound;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetTailIndex - return %d" ), index ) );
    
    return index;
    }

#ifdef BUFFER_TRACE_DEBUG        
// ---------------------------------------------------------------------------
// Prints out buffer details with RDebug
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::DebugPrintBuffer()
    {
    BUFFER_TRACE((_L("#### iHeadIndex: %d, iValidEndIndex: %d, iRequestedFreeBlockLength: %d"), iHeadIndex, iValidEndIndex, iRequestedFreeBlockLength));
    for(TInt i = 0; i < iTailIndexArray.Count(); i++)
        {
        BUFFER_TRACE((_L("#### iListener: 0x%x, iDataTailIndex: %d, iConditionTailIndex: %d, iDataLostCount: %d, iDataAvailable: %d, iConditionDataAvailable: %d"), 
                      iTailIndexArray[i]->iListener,
                      iTailIndexArray[i]->iDataTailIndex,
                      iTailIndexArray[i]->iConditionTailIndex,
                      iTailIndexArray[i]->iDataLostCount,
                      iTailIndexArray[i]->iDataAvailable,
                      iTailIndexArray[i]->iConditionDataAvailable));
        }
    }
#endif // BUFFER_TRACE_DEBUG


// ---------------------------------------------------------------------------
// Get the initial data for listener to transaction data item
// ---------------------------------------------------------------------------
//
void CSensrvChannelBuffer::GetInitialMessageData( CSensrvChannelListener::TChannelDataMessage& aMessageData )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelBuffer::GetInitialMessageData iLatestWrittenItemIndex %d" ), 
        iLatestWrittenItemIndex ) );
    
    aMessageData.iDataItemCount = 0;
    aMessageData.iDataLostCount = 0;
    
    if ( iLatestWrittenItemIndex >= 0 )
        {
        TUint8* itemPtr = const_cast<TUint8*>( iBuffer->Ptr() ) + iLatestWrittenItemIndex * iItemSize;
        aMessageData.iFirstDataPart.Set( itemPtr, iItemSize );
        aMessageData.iSecondDataPart.Set( KNullDesC8 );
        aMessageData.iDataItemCount = 1;
        }
    
    COMPONENT_TRACE( _L( "Sensor Server - CSensrvChannelBuffer::GetInitialMessageData - return" ) );
    }

