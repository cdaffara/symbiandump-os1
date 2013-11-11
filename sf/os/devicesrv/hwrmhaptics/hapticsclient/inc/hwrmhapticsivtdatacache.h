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
* Description:  Header file of haptics client's IVT-data cache.
*
*/


#ifndef HWRMHAPTICSIVTDATACACHE_H
#define HWRMHAPTICSIVTDATACACHE_H

#include <e32base.h>
#include "hwrmhapticsivtdatacacheao.h"

class RHWRMHapticsSession;

/**
 * Haptics client's IVT-data cache.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CHWRMHapticsIVTDataCache ) : public CBase
    { 
public:
    
    /**
     * Symbian two-phased constructor.
     *
     * @return Instance of the CHWRMHapticsIVTDataCache class.
     */ 
    static CHWRMHapticsIVTDataCache* NewL();

    /**
     * Symbian two-phased constructor.
     *
     * @return Instance of the CHWRMHapticsIVTDataCache class.
     */ 
    static CHWRMHapticsIVTDataCache* NewLC();

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsIVTDataCache();

public:

    /**
     * Adds a new IVT-data item to the cache.
     *
     * @param aIVTData IVT-data in an 8-bit buffer.
     * @param aFileHandle Filehandle to the IVT-data.
     *        This param is used as out parameter. The cache 
     *        implementation allocates its own file handle which is
     *        returned in this parameter.
     * @return KErrNone if successful, otherwise one of the system
     * wide error codes.
     */
    TInt AddCacheItem( const TDesC8& aIVTData, TInt& aFileHandle );

    /**
     * Updates the response listener to the cache item. This is for 
     * asynchronous play methods
     * @param aFileHandle Filehandle to the IVT-data.
     * @param aClientStatus Reference to the TRequestStatus of the client 
     *                      of Haptics with which it is calling the asynch
     *                      play method.
     * @param aClientSession Pointer to the generic client session object 
     *                       throuhg which to make the asynch call.
     * @param aArgs Arguments of the asynch IPC call.
     * @return ETrue if the listener update succeeded and if the asynch
     *         call was made with listener's status, EFalse otherwise.
     *         
     */
    TBool UpdateCacheItemListener( TInt aFileHandle,
                                   TRequestStatus& aClientStatus,
                                   RHWRMHapticsSession* aClientSession,
                                   const TIpcArgs& aArgs );

    /**
     * Removes a cache item identified by the given filehandle.
     *
     * @param aFileHandle Filehandle to IVT-data, which should
     * be removed.
     * @return KErrNone if successful, KErrNotFound if item not
     * found.
     */
    TInt RemoveCacheItem( TInt aFileHandle );

    /**
     * Removes all items from the cache.
     */
    void Reset();

    /**
     * Returns a pointer to the IVT-data buffer, which corresponds to
     * the given filehandle.
     * 
     * @param aFileHandle Filehandle to the IVT-data to be fetched.
     * @return IVT-data buffer pointer corresponding to the given filehandle.
     * If the data is not found, returns NULL. Ownership of the buffer is 
     * not passed (ownership of the buffer remains in the cache).
     */
    const HBufC8* IVTData( TInt aFileHandle ) const;
    
    /**
     * Returns whether or not the IVT-data of the given filehandle
     * is currently loaded into the haptics system.
     *
     * @param aFileHandle Handle to the IVT-data.
     * @return Whether or not the data is currently loaded into haptics.
     */
    TBool IsLoaded( TInt aFileHandle ) const;
    
    /**
     * Updates server side file handle
     * 
     * @param aClientFileHandle Handle (used by client side) to the IVT-data.
     * @param aSetLoaded Whether or not this IVT-data is currently
     * the loaded data to the haptics system.
     */
    void UpdateCacheItem( TInt aClientFileHandle, TBool aSetLoaded );    

    /**
     * Returns client side file handle
     *
     * @param aClientFileHandle Handle (used by client side) to the IVT-data.
     * @return File handle (used by client side) to the IVT-data.
     * KErrNotFound if not found
     */
    TInt ClientFileHandle( const TDesC8& aIVTData );

    /**
     * Method for requesting garbage collection of obsolete TCacheItem
     * items (or just their load observers).
     */
    void RequestGarbageCollection();

    /**
     * Static method that CIdle calls as callback to remove obsolete
     * whole items (or just load observers) from the TCacheItem array
     *
     * @param aObjectPtr Pointer to the CHWRMHapticsIVTDataCache object that
     *                   started the CIdle runner whose RunL calls this
     *                   method as callback.
     * @return TInt      Always returns KErrNone. Required by 
     *                   CIdle( TCallBack ).
     */
    static TInt CollectGarbageIdle( TAny* aObjectPtr );
    
    /**
     * Method that does the actual garbage collection.
     */
    void CollectGarbage();

protected:

    /**
     * Data structure representing one item of data in the cache.
     */
    struct TCacheItem
        {
        /**
         * IVT-data, which has been loaded into the haptics system.
         * Owned (by the object which contains RArray of these TCacheItem
         * items).
         */
        HBufC8* iIVTData;

        /**
         * Filehandle (used by client side) to the same loaded IVT-data.
         */
        TInt iClientFileHandle;
        
        /**
         * Flag that indicates whether the item is marked for deletion or not.
         *       This is only feasible in case that there's ongoing asynch
         *       play with data operation for the data item in question.
         */
        TBool iDeletionRequested;
        
        /**
         * Pointer to CHWRMHapticsIVTDataCacheAO objects that is
         * waiting for completion of PlayIVTEffect with data calls. This is
         * only used in asynch play operation in case that the IVT data has
         * not been loaded to player beforehand.
         * Owned (by the object which contains RArray of these TCacheItem
         * items).
         */
        CHWRMHapticsIVTDataCacheAO* iLoadObserver;
        };

private:

    /**
     * C++ constructor.
     */
    CHWRMHapticsIVTDataCache();

    /**
     * Second phase construction.
     */
    void ConstructL();

    /**
     * Searches the cache for an item with the given filehandle.
     *
     * @param aClientFileHandle Filehandle to the cache item data.
     * @return Index of the item in the cache. KErrNotFound, if not found.
     */
    TInt FindItem( TInt aClientFileHandle ) const;

private:

    /**
     * Array containing all cache items.
     */
    RArray<TCacheItem> iCache;
    
    /**
     * Index to the item, which is currently loaded.
     */
    TInt iLoadedItem;
    
    /**
     * File handle maintained by cache
     */
    TInt iInternalHandle;
    
    /**
     * An Idle object through which garbage collection (clearing of unused
     * items from TCacheItem array) is done
     */ 
    CIdle* iIdle;
    };
    
#endif // HWRMHAPTICSIVTDATACACHE_H   
            
// End of File
