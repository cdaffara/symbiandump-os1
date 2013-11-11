/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for started dismount watcher
*
*/



#ifndef C_STARTED_DISMOUNT_WATCHER_H
#define C_STARTED_DISMOUNT_WATCHER_H


//  INCLUDES
#include <e32std.h>
#include "diskwatcherbase.h"

// CLASS DECLARATION
/**
* Class provides functionality of dismount watcher.
*
* @since S60 5.2
*/
NONSHARABLE_CLASS(CStartedDismountWatcher) : public CDiskWatcherBase
    {
public:
    /**
    * This is a two-phase constructor method to create a new watcher instance.
    *
    * @since S60 5.2
    * @param aCallback Reference to callback
    * @param aFs Reference to open file server session
    * @param aWatcherList Reference to watcher list
    * @param aDrive A drive identifier to be watched
    * @param aForcedTimeout A timeout to forced dismount
    * @return A pointer to a new watcher instance
    */
    static CStartedDismountWatcher* NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CStartedDismountWatcher >& aWatcherList,
        TInt aDrive,
        TTimeIntervalMicroSeconds32 aForcedTimeout );

    /**
    * Destructor.
    */
    ~CStartedDismountWatcher();

    /**
    * Compare function for find functionality
    *
    * @since S60 5.2
    * @param aDrive Pointer to drive identifier
    * @param aWatcher Watcher to be compared
    * @return Comparison result
    */
    static TInt CompareFind(
        const TInt* aDrive,
        const CStartedDismountWatcher& aWatcher );

    /**
    * Compare function for find functionality
    *
    * @since S60 5.2
    * @param aWatcher1 Watcher to be compared
    * @param aWatcher2 Watcher to be compared
    * @return Comparison result
    */
    static TInt CompareFindObject(
        const CStartedDismountWatcher& aWatcher1,
        const CStartedDismountWatcher& aWatcher2 );

    /**
    * Activates watcher manually
    *
    * @since S60 5.2
    * @param aForcedTimeout A timeout to forced dismount
    */
    void Activate( TTimeIntervalMicroSeconds32 aForcedTimeout );

private: // From CDiskWatcherBase
    void WatcherError( TInt aError, const TBool& aKilled );
    void ReactivateWatcher();
    void RunWatcher( const TBool& aKilled );
    void CancelWatcher();

private:
    CStartedDismountWatcher();
    CStartedDismountWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CStartedDismountWatcher >& aWatcherList,
        TInt aDrive,
        TTimeIntervalMicroSeconds32 aForcedTimeout );
    void ConstructL();
    void RemoveFromListAndDestroy();
    static TInt ForcedDismountCB( TAny* aPtr );
    void RequestForcedDismount();
    void NotifyDismount( TInt aError, const TBool& aKilled );

private:
    // Reference to array owning the watchers
    RPointerArray< CStartedDismountWatcher >& iWatcherList;
    // Drive to dismount
    TInt iDrive;
    // Timeout for forced dismount
    TTimeIntervalMicroSeconds32 iForcedTimeout;
    // Timer for forced dismount. Owned.
    CPeriodic* iForcedDismountTimer;
    // Indicates if the dismount in progress is forced
    TBool iForcedDismount;
    };


#endif // C_DISMOUNT_WATCHER_H   

// End of File
