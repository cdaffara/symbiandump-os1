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
* Description:  Class for dismount watcher
*
*/



#ifndef C_DISMOUNT_WATCHER_H
#define C_DISMOUNT_WATCHER_H


//  INCLUDES
#include <e32std.h>
#include "diskwatcherbase.h"

// CLASS DECLARATION
/**
* Class provides functionality of dismount watcher.
*
* @since S60 5.0
*/
NONSHARABLE_CLASS(CDismountWatcher) : public CDiskWatcherBase
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
    * @return A pointer to a new watcher instance
    */
    static CDismountWatcher* NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDismountWatcher >& aWatcherList,
        TInt aDrive );

    /**
    * Destructor.
    */
    ~CDismountWatcher();

    /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aDrive Pointer to drive identifier
    * @param aWatcher Watcher to be compared
    * @return Comparison result
    */
    static TInt CompareFind(
        const TInt* aDrive,
        const CDismountWatcher& aWatcher );

    /**
    * Compare function for find functionality
    *
    * @since S60 5.0
    * @param aWatcher1 Watcher to be compared
    * @param aWatcher2 Watcher to be compared
    * @return Comparison result
    */
    static TInt CompareFindObject(
        const CDismountWatcher& aWatcher1,
        const CDismountWatcher& aWatcher2 );

    /**
    * Activates watcher manually
    *
    * @since S60 5.0
    */
    void Activate();

    /**
    * Allows dismount
    *
    * @since S60 5.0
    * @return Dismount result
    */
    TInt AllowDismount();

private: // From CDiskWatcherBase
    void WatcherError( TInt aError, const TBool& aKilled );
    void ReactivateWatcher();
    void RunWatcher( const TBool& aKilled );
    void CancelWatcher();

private:
    CDismountWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDismountWatcher >& aWatcherList,
        TInt aDrive );
    void ConstructL();
    void RemoveFromListAndDestroy();

private:
    RPointerArray< CDismountWatcher >& iWatcherList;
    TInt iDrive;
    TBool iWaitConfirm;
    };


#endif // C_DISMOUNT_WATCHER_H   

// End of File
