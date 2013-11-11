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
* Description:  Base class for disk watchers.
*
*/



#ifndef C_DISK_WATCHER_BASE_H
#define C_DISK_WATCHER_BASE_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MDiskNotifyHandlerCallback;
class RFs;

// CLASS DECLARATION
/**
* Class provides base functionality of disk watcher.
*
* @since S60 5.0
*/
NONSHARABLE_CLASS(CDiskWatcherBase) : public CActive
    {
protected:
    /**
    * C++ constructor.
    *
    * @since S60 5.0
    * @param aCallback Reference to callback
    * @param aFs Reference to open file server session
    */
    CDiskWatcherBase(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs );

    /**
    * Destructor.
    */
    ~CDiskWatcherBase();

private: // From CActive
    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();

private:
    /**
    * Notifies watcher error
    *
    * @since S60 5.0
    * @param aError A system wide error code
    * @param aKilled A reference to indicate if watcher has been killed by callback
    */
    virtual void WatcherError( TInt aError, const TBool& aKilled ) = 0;

    /**
    * Reactivites watcher to catch notifications
    *
    * @since S60 5.0
    */
    virtual void ReactivateWatcher() = 0;

    /**
    * Runs watcher action on notification
    *
    * @since S60 5.0
    * @param aKilled A reference to indicate if watcher has been killed by callback
    */
    virtual void RunWatcher( const TBool& aKilled ) = 0;

    /**
    * Cancels watcher
    *
    * @since S60 5.0
    */
    virtual void CancelWatcher() = 0;

protected:
    /**
    * Returns open file server session. 
    *
    * @since S60 5.0
    * @return A reference to open file server session.
    */
    inline RFs& Fs();

    /**
    * Returns callback. 
    *
    * @since S60 5.0
    * @return A reference to callback.
    */
    inline MDiskNotifyHandlerCallback& Callback();

    /**
    * Compares two items
    *
    * @since S60 5.0
    * @param aItem1 Refefence to first item
    * @param aItem1 Refefence to second item
    * @return Comparison result
    */
    template < class T >
    inline static TInt Compare( const T& aItem1, const T& aItem2 );

private:
    /**
     * Callback instance. Not owned.
     */
    MDiskNotifyHandlerCallback& iCallback;

    /**
     * Open file server session. Not owned.
     */
    RFs& iFs;

    /**
     * Indicates if instance is killed from callback. Not owned.
     */
    TBool* iKilled;

    };

// Include inline method implementations
#include "diskwatcherbase.inl"

#endif // C_DISK_WATCHER_BASE_H   

// End of File
