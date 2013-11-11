/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wrapper implementation for file server notification handling.
*
*/



#ifndef C_DISK_NOTIFY_HANDLER_IMPL_H
#define C_DISK_NOTIFY_HANDLER_IMPL_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class MDiskNotifyHandlerCallback;
class CDiskWatcher;
class CDismountWatcher;
class CDiskSpaceWatcher;
class CEntryWatcher;
class CStartedDismountWatcher;

// CLASS DECLARATION
/**
* Class provides actual implementation of disk notify handler.
*
* @since S60 5.0
*/
NONSHARABLE_CLASS(CDiskNotifyHandlerImpl) : public CBase
    {

public:

    /**
    * This is a two-phase constructor method that is used to
    * create a new instance for listening to the disk changes.
    *
    * @since S60 5.0
    * @param aCallback Reference to a callback instance, MDiskNotifyHandlerCallback
    * @param aFs       Reference to an open file server session
    * @return          A pointer to a new instance of the CDiskNotifyHandlerImpl class.
    */
    static CDiskNotifyHandlerImpl* NewL(
        MDiskNotifyHandlerCallback& aCallback, RFs& aFs );

    /**
    * Destructor.
    */
    ~CDiskNotifyHandlerImpl();

    /**
    * Starts listening for disk notifications.
    *
    * @since S60 5.0
    * @return A system wide error code.
    */
    TInt NotifyDisk();

    /**
    * Cancels listening for disk notifications. 
    *
    * @since S60 5.0
    */
    void CancelNotifyDisk();

    /**
    * Starts listening for dismount notifications. 
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    TInt NotifyDismount( TInt aDrive );

    /**
    * Cancels listening for dismount notifications. 
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    void CancelNotifyDismount( TInt aDrive );

    /**
    * Cancels listening for all dismount notifications. 
    *
    * @since S60 5.0
    */
    void CancelNotifyDismount();

    /**
    * Allows the dismount of given drive.
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    TInt AllowDismount( TInt aDrive );

    /**
    * Starts listening for disk space notifications.
    *
    * @since S60 5.0
    * @param aThreshold A threshold that causes notification when crossed.
    * @param aDrive A drive identifier specified by TDriveNumber
    * @return A system wide error code.
    */
    TInt NotifyDiskSpace( const TInt64& aThreshold, TInt aDrive );

    /**
    * Cancels listening for disk space notifications with given threshold.
    *
    * @since S60 5.0
    * @param aThreshold A threshold that causes notification when crossed.
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    void CancelNotifyDiskSpace( const TInt64& aThreshold, TInt aDrive );

    /**
    * Cancels listening for all disk space notifications with given drive.
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    void CancelNotifyDiskSpace( TInt aDrive );

    /**
    * Cancels listening for all disk space notifications.
    *
    * @since S60 5.0
    */
    void CancelNotifyDiskSpace();

    /**
    * Starts listening for entry (i.e. file or folder) change notifications. 
    *
    * @since S60 5.0
    * @param aType A type of notification specified by TNotifyType.
    * @param aEntry A full path of the file or folder to be listened.
    * @return A system wide error code.
    */
    TInt NotifyEntry( TNotifyType aType, const TDesC& aEntry );

    /**
    * Cancels listening for entry (i.e. file or folder) change notifications with
    * given entry and notify type. 
    *
    * @since S60 5.0
    * @param aType A type of notification specified by TNotifyType.
    * @param aEntry A full path of the file or folder to be listened.
    */
    void CancelNotifyEntry( TNotifyType aType, const TDesC& aEntry );

    /**
    * Cancels listening for all entry (i.e. file or folder) change notifications with
    * given entry. 
    *
    * @since S60 5.0
    * @param aEntry A full path of the file or folder to be listened.
    */
    void CancelNotifyEntry( const TDesC& aEntry );

    /**
    * Cancels listening for all entry (i.e. file or folder) change notifications.
    *
    * @since S60 5.0
    */
    void CancelNotifyEntry();

    /**
    * Starts dismount with sending notification to registered clients. If dismount is 
    * pending after given timeout, forced dismount is done.
    *
    * @since S60 5.2
    * @param aDrive A drive identifier specified by TDriveNumber
    * @param aForcedTimeout A timeout to forced dismount in micro seconds.
    * @return A system wide error code.
    */
    TInt StartDismount( TInt aDrive, TTimeIntervalMicroSeconds32 aForcedTimeout );

    /**
    * Cancels started dismount of given drive.
    *
    * @since S60 5.2
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    void CancelStartedDismount( TInt aDrive );

    /**
    * Cancels started dismount of all drives.
    *
    * @since S60 5.2
    */
    void CancelStartedDismount();

private:
    /**
    * C++ default constructor.
    */
    CDiskNotifyHandlerImpl( MDiskNotifyHandlerCallback& aCallback, RFs& aFs );

private:
    MDiskNotifyHandlerCallback& iCallback;
    RFs& iFs;
    CDiskWatcher* iDiskWatcher;
    RPointerArray< CDismountWatcher > iDismountWatcherList;
    RPointerArray< CDiskSpaceWatcher > iDiskSpaceWatcherList;
    RPointerArray< CEntryWatcher > iEntryWatcherList;
    RPointerArray< CStartedDismountWatcher > iStartedDismountWatcherList;

    };

#endif      // C_DISK_NOTIFY_HANDLER_IMPL_H   

// End of File
