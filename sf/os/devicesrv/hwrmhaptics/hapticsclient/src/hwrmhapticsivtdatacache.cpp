/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of haptics client's IVT-data cache.
*
*/


#include "hwrmhapticsivtdatacache.h"

// ---------------------------------------------------------------------------
// Two-phase constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCache* CHWRMHapticsIVTDataCache::NewL()
    {        
    CHWRMHapticsIVTDataCache* self = CHWRMHapticsIVTDataCache::NewLC();
    CleanupStack::Pop( self );

    return self;      
    }
    
// ---------------------------------------------------------------------------
// Two-phase asynchronous constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCache* CHWRMHapticsIVTDataCache::NewLC()
    {
    CHWRMHapticsIVTDataCache* self = new ( ELeave ) CHWRMHapticsIVTDataCache();
    CleanupStack::PushL( self );
    
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCache::~CHWRMHapticsIVTDataCache()
    {
    Reset();
    if ( iIdle )
        {
        iIdle->Cancel();
        delete iIdle;
        }
    iCache.Close();
    }

// ---------------------------------------------------------------------------
// Adds a new IVT-data item to the cache.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTDataCache::AddCacheItem( const TDesC8& aIVTData, 
                                             TInt& aFileHandle )
    {
    TInt err = KErrNoMemory;
    
    TCacheItem newItem;
    newItem.iIVTData = aIVTData.Alloc();
    newItem.iClientFileHandle = ++iInternalHandle;
    newItem.iDeletionRequested = EFalse;
    newItem.iLoadObserver = NULL;
    
    aFileHandle = iInternalHandle;

    if ( newItem.iIVTData )
        {
        err = iCache.Append( newItem );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Adds a new IVT-data item to the cache. This overload is meant for 
// asynch calls.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsIVTDataCache::UpdateCacheItemListener( 
        TInt aFileHandle,
        TRequestStatus& aClientStatus,
        RHWRMHapticsSession* aClientSession,
        const TIpcArgs& aArgs )
    {
    TBool retVal( EFalse );
    TInt itemIndex = FindItem( aFileHandle );

    if ( itemIndex >= 0 )
        {
        // New load observer listener is only instantiated if there isn't
        // already a listener for the cache item. Note that if there already
        // were one, this method will return EFalse, and the actual asynch
        // call will be made with the original client's status in Impl class.
        if ( !iCache[itemIndex].iLoadObserver )
            {
            // If the below leaves this method will return EFalse,
            // and the actual asynch call will be made with the original 
            // client's status.
            TRAPD(err,  
                  iCache[itemIndex].iLoadObserver = 
                    CHWRMHapticsIVTDataCacheAO::NewL( aFileHandle,
                                                      this,
                                                      aClientStatus ) );
            if ( !err )
                {
                iCache[itemIndex].iLoadObserver->PlayEffectAsynch
                    ( aClientSession, aArgs );
                retVal = ETrue;
                }
            }
        }
        
    return retVal;
    }

// ---------------------------------------------------------------------------
// Removes a cache item identified by the given filehandle.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTDataCache::RemoveCacheItem( TInt aFileHandle )
    {
    TInt err = KErrNotFound;
    
    // search for the item in the cache
    TInt itemIndex = FindItem( aFileHandle );

    if ( itemIndex >= 0 )
        {
        // item was found, either mark it for deletion (if there's active
        // load observer active object waiting for response for previous 
        // play with data request) or otherwise delete and remove it.
        if ( iCache[itemIndex].iLoadObserver && 
             iCache[itemIndex].iLoadObserver->IsActive() )
            {
            iCache[itemIndex].iDeletionRequested = ETrue;
            }
        else
            {
            delete iCache[itemIndex].iIVTData;
            iCache[itemIndex].iIVTData = NULL;
            delete iCache[itemIndex].iLoadObserver;
            iCache[itemIndex].iLoadObserver = NULL;
            iCache.Remove( itemIndex );
        
            err = KErrNone;

            // update loaded item index, if needed
            if ( iLoadedItem == itemIndex )
                {
                iLoadedItem = KErrNotFound;
                }
            else if ( iLoadedItem > itemIndex )
                {
                --iLoadedItem;
                }
            }
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Removes all items from the cache.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCache::Reset()
    {
    // delete and remove all items
    while ( iCache.Count() )
        {
        delete iCache[0].iIVTData;
        iCache[0].iIVTData = NULL;
        delete iCache[0].iLoadObserver;
        iCache[0].iLoadObserver = NULL;
        iCache.Remove( 0 );
        }

    iLoadedItem = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Returns a pointer to the IVT-data buffer, which corresponds to
// the given filehandle.
// ---------------------------------------------------------------------------
//
const HBufC8* CHWRMHapticsIVTDataCache::IVTData( TInt aFileHandle ) const
    {
    HBufC8* ivtData = NULL;
    
    // search for the item in the cache
    TInt itemIndex = FindItem( aFileHandle );
    if ( itemIndex >= 0 )
        {
        ivtData = iCache[itemIndex].iIVTData;
        }
    
    return ivtData;
    }

// ---------------------------------------------------------------------------
// Returns whether or not the IVT-data of the given filehandle
// is currently loaded into the haptics system.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsIVTDataCache::IsLoaded( TInt aFileHandle ) const
    {
    TInt ret = EFalse;
    TInt index = FindItem( aFileHandle );
    
    // if index was found and the index is the loaded item's index,
    // this data is already loaded
    if ( index >= 0 && index == iLoadedItem )
        {
        ret = ETrue;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Updates server side file handle
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCache::UpdateCacheItem( TInt aClientFileHandle,
                                                TBool aSetLoaded )
    {
    // find the item from cache with the given filehandle
    TInt itemIndex = FindItem( aClientFileHandle );
    if ( itemIndex >= 0 )
        {
        if ( aSetLoaded && !iCache[itemIndex].iDeletionRequested )
            {
            iLoadedItem = itemIndex;
            }
        }
    }

// ---------------------------------------------------------------------------
// Returns client side file handle
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTDataCache::ClientFileHandle( const TDesC8& aIVTData )
    {
    // find the item from cache with the given filehandle
    TInt clientFileHandle( KErrNotFound );
    TInt count( iCache.Count() );
    for ( TInt i( 0 ); i < count && KErrNotFound == clientFileHandle; ++i )
        {
        TPtr8 cachedData = iCache[i].iIVTData->Des();
        if ( cachedData == aIVTData )
            {
            clientFileHandle = iCache[i].iClientFileHandle;
            }
        }

    return clientFileHandle;
    }

// ---------------------------------------------------------------------------
// Method for starting the idle object that then calls garbage collection
// callback.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCache::RequestGarbageCollection()
    {
    if ( iIdle && !iIdle->IsActive() )
        {
        iIdle->Start( TCallBack ( CollectGarbageIdle, this ) );
        }
    }

// ---------------------------------------------------------------------------
// Garbage collection static callback.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTDataCache::CollectGarbageIdle( TAny* aObjectPtr )
    {
    CHWRMHapticsIVTDataCache* self = 
        reinterpret_cast<CHWRMHapticsIVTDataCache*>( aObjectPtr );
    if ( self )
        {
        self->CollectGarbage();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Garbage collection actual (object specific) callback. Cleans obsolete
// load observers and whole TCacheItems if needed.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCache::CollectGarbage()
    {
    for ( TInt i = iCache.Count() - 1; i >= 0; --i )
        {
        // This garbage collection is only interested in those entries for
        // which there has been load observer running, i.e., for which the
        // iLoadObserver is non-NULL, and for which the running has stopped,
        // i.e., the iLoadObserver.IsActive() check returns EFalse
        if ( iCache[i].iLoadObserver && !iCache[i].iLoadObserver->IsActive() )
            {
            delete iCache[i].iLoadObserver;
            iCache[i].iLoadObserver = NULL;
            // If the entry was also marked for deletion, delete the whole
            // entry from TCacheItem array
            if ( iCache[i].iDeletionRequested )
                {
                delete iCache[i].iIVTData;
                iCache[i].iIVTData = NULL;
                iCache.Remove( i );

                // update loaded item index, if needed
                if ( iLoadedItem == i )
                    {
                    iLoadedItem = KErrNotFound;
                    }
                else if ( iLoadedItem > i )
                    {
                    --iLoadedItem;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCache::CHWRMHapticsIVTDataCache()
    : iLoadedItem( KErrNotFound )
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCache::ConstructL()
    {
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    }

// ---------------------------------------------------------------------------
// Searches the cache for an item with the given filehandle.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTDataCache::FindItem( TInt aClientFileHandle ) const
    {
    TInt index = KErrNotFound;
    
    // find the item from cache with the given filehandle
    for ( TInt i = 0; i < iCache.Count() && index == KErrNotFound; ++i )
        {
        if ( iCache[i].iClientFileHandle == aClientFileHandle )
            {
            index = i;
            }
        }
   
    return index;
    }

//  End of File  
