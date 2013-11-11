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
* Description:  Class for disk space watcher
*
*/



#ifndef C_DISK_SPACE_WATCHER_H
#define C_DISK_SPACE_WATCHER_H

//  INCLUDES
#include <e32std.h>
#include "diskwatcherbase.h"

// CLASS DECLARATION
/**
* Class provides functionality of disk space watcher.
*
* @since S60 5.0
*/
NONSHARABLE_CLASS(CDiskSpaceWatcher) : public CDiskWatcherBase
    {
public:
    /**
    * This is a two-phase constructor method to create a new watcher instance.
    *
    * @since S60 5.0
    * @param aCallback Reference to callback
    * @param aFs Reference to open file server session
    * @param aWatcherList Reference to watcher list
    * @param aDrive A drive identifier to be watched
    * @param aThreshold A threshold to be watched
    * @return A pointer to a new watcher instance.
    */
    static CDiskSpaceWatcher* NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDiskSpaceWatcher >& aWatcherList,
        TInt aDrive,
        const TInt64& aThreshold );

    /**
    * Destructor.
    */
    ~CDiskSpaceWatcher();

    /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aDrive Pointer to drive identifier
    * @param aWatcher Watcher to be compared
    * @return Comparison result.
    */
    static TInt CompareFind(
        const TInt* aDrive,
        const CDiskSpaceWatcher& aWatcher );

    // A class for the data used by find comparison
    class TFindData
        {
        public:
            // Drive identifier
            TInt iDrive;
            // Drive threshold
            TInt64 iThreshold;
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
        const CDiskSpaceWatcher& aWatcher );

    /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aWatcher1 Watcher to be compared
    * @param aWatcher2 Watcher to be compared
    * @return Comparison result.
    */
    static TInt CompareFindObject(
        const CDiskSpaceWatcher& aWatcher1,
        const CDiskSpaceWatcher& aWatcher2 );

    /**
    * Activates watcher manually
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
    CDiskSpaceWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDiskSpaceWatcher >& aWatcherList,
        TInt aDrive,
        const TInt64& iThreshold );
    void ConstructL();
    void RemoveFromListAndDestroy();

private:
    RPointerArray< CDiskSpaceWatcher >& iWatcherList;
    TInt iDrive;
    TInt64 iThreshold;
    };


#endif // C_DISK_SPACE_WATCHER_H   

// End of File
