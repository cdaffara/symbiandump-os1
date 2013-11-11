/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for entry watcher
*
*/



#ifndef C_ENTRY_WATCHER_H
#define C_ENTRY_WATCHER_H


//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "diskwatcherbase.h"

/**
* Class provides functionality of entry watcher.
*
* @since S60 5.0
*/
NONSHARABLE_CLASS(CEntryWatcher) : public CDiskWatcherBase
    {
public:
    /**
    * This is a two-phase constructor method to create a new watcher instance.
    *
    * @since S60 5.0
    * @param aCallback Reference to callback
    * @param aFs Reference to open file server session
    * @param aWatcherList Reference to watcher list
    * @param aEntry A full path to entry to be watched
    * @param aType A threshold to be watched
    * @return A pointer to a new watcher instance.
    */
    static CEntryWatcher* NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CEntryWatcher >& aWatcherList,
        const TDesC& aEntry,
        TNotifyType aType );

    /**
    * Destructor.
    */
    ~CEntryWatcher();

    /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aEntry Pointer to full path of the entry
    * @param aWatcher Watcher to be compared
    * @return Comparison result.
    */
    static TInt CompareFind(
        const TDesC* aEntry,
        const CEntryWatcher& aWatcher );

    // A class for the data used by find comparison
    class TFindData
        {
        public:
            // Full path of the entry
            TPtrC iEntry;
            // Notity type of the entry
            TNotifyType iType;
        };

   /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aData Find data for comparison
    * @param aWatcher Watcher to be compared
    * @return Comparison result.
    */
    static TInt CompareFindData(
        const TFindData* aData,
        const CEntryWatcher& aWatcher );

    /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aWatcher1 Watcher to be compared
    * @param aWatcher2 Watcher to be compared
    * @return Comparison result.
    */
    static TInt CompareFindObject(
        const CEntryWatcher& aWatcher1,
        const CEntryWatcher& aWatcher2 );

    /**
    * Activates watcher
    *
    * @since S60 5.0
    */
    void Activate();

private: // From CDiskWatcherBase
    void WatcherError( TInt aError, const TBool& aKilled );
    void ReactivateWatcher();
    void RunWatcher( const TBool& aKilled );
    void CancelWatcher();

private:
    CEntryWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CEntryWatcher >& aWatcherList,
        TNotifyType aType );
    void ConstructL( const TDesC& aEntry );
     void RemoveFromListAndDestroy();

private:
    RPointerArray< CEntryWatcher >& iWatcherList;
    HBufC* iEntry;
    TNotifyType iType;
    };


#endif // C_ENTRY_WATCHER_H   

// End of File
