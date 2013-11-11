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
* Description:  Class for notify disk watcher
*
*/



#ifndef C_DISK_WATCHER_H
#define C_DISK_WATCHER_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "diskwatcherbase.h"
#include "disknotifyhandler.h"

// CLASS DECLARATION
/**
* Class provides functionality of notify disk watcher.
*
* @since S60 5.0
*/
NONSHARABLE_CLASS(CDiskWatcher) : public CDiskWatcherBase
    {
public:
    /**
    * This is a two-phase constructor method to create a new watcher instance.
    *
    * @since S60 5.0
    * @param aCallback Reference to callback
    * @param aFs Reference to open file server session
    * @return A pointer to a new watcher instance.
    */
    static CDiskWatcher* NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs );

    /**
    * Destructor.
    */
    ~CDiskWatcher();

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
    CDiskWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs );
    void ConstructL();

    NONSHARABLE_CLASS( TWatcherInfo )
        {
        public:
            void Read( RFs& aFs, TInt aDrive );
            TBool IsChanged( const TWatcherInfo& aNewInfo ) const;
            void Reset();
        public:
            TDriveInfo iDriveInfo;
            TUint iDriveStatus;
        };

private:
    TFixedArray< TWatcherInfo, KMaxDrives > iInfoList;
    };


#endif // C_DISK_WATCHER_H   

// End of File
