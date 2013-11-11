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
* Description:  Wrapper for file server notification handling.
*
*/



#ifndef C_DISK_NOTIFY_HANDLER_H
#define C_DISK_NOTIFY_HANDLER_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class MDiskNotifyHandlerCallback;
class CDiskNotifyHandlerImpl;

// CLASS DECLARATION
/**
* Disk Notification API provides an easy-to-use implementation of a wrapper 
* to handle file server notifications. It contains all required active objects 
* needed for handling the notifications. In the most cases, it automatically 
* resubscribes the notifications. 
* The API consists of classes CDiskNotifyHandler and MDiskNotifyHandlerCallback.
* The user of CDiskNotifyHandler class needs to implement relevant 
* MDiskNotifyHandlerCallback interface methods to handle the notifications.
*
* Usage:
*  
* Initialization example (from a class that implements MDiskNotifyHandlerCallback interface):
* @code
* // iFsSession contains open file server session
* iNotifyHandler = CDiskNotifyHandler::NewL( *this, iFsSession );
* User::LeaveIfError( iNotifyHandler->NotifyDisk() ); // Subscribe disk notifications
* // Note that also the other notifications can be subcribed simultaneously using 
* // the same disk notify handler
* @endcode
*
* Uninitialization example:
* @code
* delete iNotifyHandler; // Cancel all notifications set by this handler
* @endcode
*
* Handler method implementation example:
* @code
* // Implement just the needed callback methods
* void CMyDiskNotifyTest::HandleNotifyDisk( TInt aError, const TDiskEvent& aEvent )
*    {
*    // Print out the event data
*    RDebug::Print( _L("Error: %d, Disk %d changed, Change type: %d"),
*       aError, aEvent.iDrive, aEvent.iType );
*    }
* @endcode
*
* @lib disknotifyhandler.lib
* @since S60 5.0
*/
NONSHARABLE_CLASS(CDiskNotifyHandler) : public CBase
    {

public:

    /**
    * This is a two-phase constructor method that is used to
    * create a new instance for listening to the disk changes.
    *
    * @since S60 5.0
    * @param aCallback Reference to a callback instance, MDiskNotifyHandlerCallback
    * @param aFs       Reference to an open file server session, RFs
    *                  Do not close this session until all CDiskNotifyHandler
    *                  instances referring to it have been deleted.
    * @return          A pointer to a new instance of the CDiskNotifyHandler class.
    *
    * @see MDiskNotifyHandlerCallback
    * @see RFs
    */
    IMPORT_C static CDiskNotifyHandler* NewL(
        MDiskNotifyHandlerCallback& aCallback, RFs& aFs );

    /**
    * Destructor.
    */
    IMPORT_C ~CDiskNotifyHandler();

    /**
    * When this method is called, the CDiskNotifyHandler starts 
    * listening for disk notifications. If it is already listening disk notifications,
    * KErrAlreadyExists is returned.
    *
    * This notification is automatically resubscibed until explicitly canceled or 
    * error has happened. The notifications and errors are informed using 
    * MDiskNotifyHandlerCallback's HandleNotifyDisk method.
    *
    * @since S60 5.0
    * @return A system wide error code.
    *
    * @see MDiskNotifyHandlerCallback
    */
    IMPORT_C TInt NotifyDisk();

    /**
    * When this method is called, the CDiskNotifyHandler cancels 
    * listening for disk notifications. If it is not listening disk notifications,
    * nothing happens.
    *
    * @since S60 5.0
    */
    IMPORT_C void CancelNotifyDisk();

    /**
    * When this method is called, the CDiskNotifyHandler starts listening 
    * for dismount notification. If it is already listening dismount notification for 
    * given drive, KErrAlreadyExists is returned.
    *
    * This notification is not resubscibed because drive is not available anymore 
    * after dismount notification. The notification and error are informed using 
    * MDiskNotifyHandlerCallback's HandleNotifyDismount method.
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    * @return A system wide error code.
    *
    * @see TDriveNumber
    * @see MDiskNotifyHandlerCallback
    *
    */
    IMPORT_C TInt NotifyDismount( TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler cancels
    * listening for dismount notification of given drive.
    * If it is not listening the dismount of given drive, nothing happens.
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    *
    * @see TDriveNumber
    */
    IMPORT_C void CancelNotifyDismount( TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler cancels
    * listening for all dismount notifications that are set by this handler.
    * If it is not listening any dismount notifications, nothing happens.
    *
    * @since S60 5.0
    */
    IMPORT_C void CancelNotifyDismount();

    /**
    * When this method is called, the CDiskNotifyHandler allows the dismount of given drive.
    * If it is not listening the dismount of given drive, nothing happens.
    *
    * This method must be called only from MDiskNotifyHandlerCallback's HandleNotifyDismount 
    * to inform file server that dismount of the drive can be done.
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    * @return A system wide error code.
    *
    * @see TDriveNumber
    */
    IMPORT_C TInt AllowDismount( TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler starts listening
    * for disk space notifications. If it is already listening disk space for 
    * given drive with given threshold, KErrAlreadyExists is returned.
    *
    * This notification is automatically resubscibed until explicitly canceled or 
    * error has happened. The notification and error are informed using 
    * MDiskNotifyHandlerCallback's HandleNotifyDiskSpace method.
    *
    * @since S60 5.0
    * @param aThreshold A threshold that causes notification when crossed.
    * @param aDrive A drive identifier specified by TDriveNumber
    * @return A system wide error code.
    *
    * @see TDriveNumber
    * @see MDiskNotifyHandlerCallback
    *
    */
    IMPORT_C TInt NotifyDiskSpace( const TInt64& aThreshold, TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler cancels listening 
    * for disk space notification of given drive with given threshold.
    * If it is not listening the disk space of given drive with given threshold,
    * nothing happens.
    *
    * @since S60 5.0
    * @param aThreshold A threshold that causes notification when crossed.
    * @param aDrive A drive identifier specified by TDriveNumber
    *
    * @see TDriveNumber
    */
    IMPORT_C void CancelNotifyDiskSpace( const TInt64& aThreshold, TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler cancels listening 
    * for all disk space notifications of given drive. 
    * If it is not listening the disk space of given drive, nothing happens. 
    *
    * @since S60 5.0
    * @param aDrive A drive identifier specified by TDriveNumber
    *
    * @see TDriveNumber
    */
    IMPORT_C void CancelNotifyDiskSpace( TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler cancels listening 
    * for all disk space notifications that are set by this handler. 
    * If it is not listening any disk space notifications, nothing happens. 
    *
    * @since S60 5.0
    */
    IMPORT_C void CancelNotifyDiskSpace();

    /**
    * When this method is called, the CDiskNotifyHandler starts 
    * listening for entry (i.e. file or folder) change notifications. 
    * If it is already listening any type of change notification for 
    * given entry with given notification type, KErrAlreadyExists is returned. 
    *
    * A given folder entry's full path must include backslash ending. 
    *
    * This notification is automatically resubscibed until explicitly canceled or 
    * error has happened. The notification and error are informed using 
    * MDiskNotifyHandlerCallback's HandleNotifyEntry method. 
    *
    * @since S60 5.0
    * @param aType A type of notification specified by TNotifyType.
    * @param aEntry A full path of the file or folder to be listened.
    * @return A system wide error code.
    *
    * @see TNotifyType
    * @see TDriveNumber
    * @see MDiskNotifyHandlerCallback
    */
    IMPORT_C TInt NotifyEntry( TNotifyType aType, const TDesC& aEntry );

    /**
    * When this method is called, the CDiskNotifyHandler cancels listening 
    * for change notification of given entry with notify type. 
    * If it is not listening the given entry with notify type, nothing happens. 
    *
    * @since S60 5.0
    * @param aType A type of notification specified by TNotifyType.
    * @param aEntry A full path of the file or folder entry to listen.
    *
    * @see TDriveNumber
    */
    IMPORT_C void CancelNotifyEntry( TNotifyType aType, const TDesC& aEntry );

    /**
    * When this method is called, the CDiskNotifyHandler cancels listening 
    * for all change notifications of given entry. 
    * If it is not listening the given entry, nothing happens. 
    *
    * @since S60 5.0
    * @param aEntry A full path of the file or folder to be listened.
    *
    * @see TDriveNumber
    */
    IMPORT_C void CancelNotifyEntry( const TDesC& aEntry );

    /**
    * When this method is called, the CDiskNotifyHandler cancels listening 
    * for entry notifications that are set by this handler. 
    * If it is not listening any entry notifications, nothing happens. 
    *
    * @since S60 5.0
    */
    IMPORT_C void CancelNotifyEntry();

    /**
    * When this method is called, the CDiskNotifyHandler starts dismount 
    * with sending notification to registered clients. If dismount is 
    * pending after given timeout, forced dismount is done.
    * If dismount of given drive was already started, KErrAlreadyExists 
    * is returned.
    *
    * This notification is not resubscibed because drive is not available anymore 
    * after dismount. The notification and error are informed using 
    * MDiskNotifyHandlerCallback's HandleNotifyDismountFinished method.
    *
    * @since S60 5.2
    * @param aDrive A drive identifier specified by TDriveNumber
    * @param aForcedTimeout A timeout to forced dismount in micro seconds.
    * @return A system wide error code.
    */
    IMPORT_C TInt StartDismount( TInt aDrive, TTimeIntervalMicroSeconds32 aForcedTimeout );

    /**
    * When this method is called, the CDiskNotifyHandler cancels started 
    * dismount of given drive.
    * If dismount of given drive was not started, nothing happens.
    *
    * @since S60 5.2
    * @param aDrive A drive identifier specified by TDriveNumber
    */
    IMPORT_C void CancelStartedDismount( TInt aDrive );

    /**
    * When this method is called, the CDiskNotifyHandler cancels all  
    * started dismounts.
    * If any dismount was not started, nothing happens.
    *
    * @since S60 5.2
    */
    IMPORT_C void CancelStartedDismount();

private:

    /**
    * C++ default constructor.
    */
    CDiskNotifyHandler();

	/**
	* Symbian two-phased constructor.
	*/
    void ConstructL( MDiskNotifyHandlerCallback& aCallback, RFs& aFs );


private: // Data

    // Owned. The actual implementation.
    CDiskNotifyHandlerImpl* iImpl;

    };


/**
* Class provides a callback interface for handling the notififications 
* from the file server. The Client derives a class from this interface 
* and implements the HandleNotify-methods that interest it. 
* An empty default implementation is provided for all of the methods. 
* In debug build the default implementations print out a debug trace. 
*
* @lib disknotifyhandler.lib
* @since S60 5.0
*/
class MDiskNotifyHandlerCallback
    {

public:

    /**
     * Defines the disk notification types.
     */
    enum TDiskEventType
        {
        /** To indicate disk notification error.
        * It also indicates that there is no valid disk event data available.
        */
        EDiskError = 0,

        /** To indicate that a new drive has been added to the drive list of file server.
        */
        EDiskAdded,

        /** To indicate that a drive has been removed from the drive list of file server.
        */
        EDiskRemoved,

        /** To indicate that drive status has been changed.
        * E.g. A memory card has been inserted, removed or unlocked.
        */
        EDiskStatusChanged
        };

    /**
     * Defines the data of disk notification event.
     */
    class TDiskEvent
        {
        public:
            /** To indicates the disk event type
            */
            TDiskEventType iType;

            /** To store the drive identifier for EDiskAdded,
            * EDiskRemoved and EDiskStatusChanged events.
            * The drive indentifier is specified bt TDriveNumber.
            */
            TInt iDrive;

            /** To store the drive info for EDiskAdded and
            * EDiskStatusChanged events.
            */
            TDriveInfo iInfo;

            /** To store the previous drive info for EDiskRemoved and
            * EDiskStatusChanged events.
            */
            TDriveInfo iPrevInfo;
        };

    /** 
    * This callback method is used to notify the client about
    * disk notifications, i.e. memory card has been inserted
    *
    * @param aError System wide error code from file server
    * @param aEvent The disk event data data specified by TDiskEvent
    *
    * TDiskEvent
    */
    IMPORT_C virtual void HandleNotifyDisk(
        TInt aError,
        const TDiskEvent& aEvent );

    /**
     * Defines the data of dismount notification event.
     */
    class TDismountEvent
        {
        public:
            // Stores the drive identifier specified by TDriveNumber to be dismounted
            TInt iDrive;
        };

    /** 
    * This callback method is used to notify the client about
    * dismount notifications. Client have to call CDiskNotifyHandler's AllowDismount
    * after it has finished preparing for dismount.
    *
    * @param aError System wide error code from file server
    * @param aEvent The dismount event data specified by TDismountEvent
    *
    * @see TDismountEvent
    */
    IMPORT_C virtual void HandleNotifyDismount(
        TInt aError,
        const TDismountEvent& aEvent );

    /**
     * Defines the data of disk space notification event.
     */
    class TDiskSpaceEvent
        {
        public:
            // Stores the drive identifier specified by TDriveNumber
            TInt iDrive;
            // Stores the crosses threshold
            TInt64 iThreshold;
        };

    /** 
    * This callback method is used to notify the client about
    * disk space notifications.
    *
    * @param aError System wide error code from file server
    * @param aEvent The disk space event data specified by TDiskSpaceEvent
    *
    * @see TDiskSpaceEvent
    */
    IMPORT_C virtual void HandleNotifyDiskSpace(
        TInt aError,
        const TDiskSpaceEvent& aEvent );

    /**
     * Defines the data of entry notification event.
     */
    class TEntryEvent
        {
        public:
            // Stores the notification type
            TNotifyType iType;
            // Stores full path of the file or folder entry
            TFileName iEntry;
        };

    /** 
    * This callback method is used to notify the client about
    * entry notifications.
    *
    * @param aError System wide error code from file server
    * @param aEvent The entry event data specified by TEntryEvent
    *
    * @see TEntryEvent
    */
    IMPORT_C virtual void HandleNotifyEntry(
        TInt aError,
        const TEntryEvent& aEvent );

    /**
     * Defines the data of dismount finished notification event.
     */
    class TDismountFinishedEvent
        {
        public:
            // Stores the dismounted drive identifier specified by TDriveNumber
            TInt iDrive;
            // Indicates if dismount was forced or not
            TBool iForcedDismount;
        };

    /**
    * This callback method is used to notify the client when 
    * started dismount has finished.
    *
    * @param aError System wide error code
    * @param aEvent The event data specified by TDismountFinishedEvent
    *
    * @see TDismountFinishedEvent
    */
    IMPORT_C virtual void HandleNotifyDismountFinished(
        TInt aError,
        const TDismountFinishedEvent& aEvent );

    };

#endif      // C_DISK_NOTIFY_HANDLER_H   

// End of File
