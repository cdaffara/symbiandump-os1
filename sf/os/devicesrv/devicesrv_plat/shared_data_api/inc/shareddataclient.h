/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     This module offers an interface for applications to access other
*     applications' settings which are meant to have shared access.
*
*/


#ifndef SHARED_DATA_CLIENT_H
#define SHARED_DATA_CLIENT_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// reasons for panicing client thread

enum TSharedDataPanic
    {
    ESDBadRequest = 0,
    ESDNotAssigned = 1,
    ESDNotConnected = 2,
    ESDNotifyHandlerMissing = 3,
    ESDNoActiveScheduler = 4,
    ESDCannotChangeReadOnlySetting = 5
    };

_LIT( KSharedDataPanicCategory, "SharedData" );

// FORWARD DECLARATIONS
class CDosWrapper;

// CLASS DECLARATION

/**
*  RSharedDataClient API
*  -----------------------------------------------------------------------------
*  Applications use the subsystem via RSharedDataClient class. First, they need
*  to connect to the server by calling Connect() member function.
*
* ------------------------------------------------------------------------------
* RESERVE FILE SUPPORT
* ------------------------------------------------------------------------------
*
* This functionality has nothing to do with the rest of SharedData, but is
* implemented in SharedData server because it is one of the most commonly used
* servers. The idea is to support transaction-based delete operations to succeed
* in a full disk situation. SharedData server checks if the free disk space is
* below the given minimum treshold and if so, reduces the size of a reserve file
* to free up some disk space. No overhead is caused if there is enough free disk
* space already. Thus, unless the reserve file is somehow exhausted,
* the free disk space is guaranteed to be above the given parameter
* after this call. Of course, the parameter is limited to the maximum size of
* the reserve file that will be about 140kB. 
*
* There are two correct usage patterns represented below, other for synchronous
* delete operations and other for asynchronous ones.
* 
* Note! When the SharedData connection is closed, the server will 
* always assume that the reserve space is no longer required by the
* corresponding client.
*
* Usage pattern (synchronous delete operation):
* ------------------------------------------------------------------------------
* // iSharedDataClient is connected to the server here.
*
* iSharedDataClient.RequestFreeDiskSpaceLC( KDiskSpaceNeededForDelete );
*
* // Do the delete operation here, the code may leave.
*
* // Do NOT close the SharedData connection here nor delete the
* // RSharedDataClient object.
*
* CleanupStack::PopAndDestroy(); // cancels free disk space request
* ------------------------------------------------------------------------------
*
* Usage pattern (asynchronous delete operation):
* ------------------------------------------------------------------------------
* // iSharedDataClient is connected to the server here.
*
* iSharedDataClient.RequestFreeDiskSpace( KDiskSpaceNeededForDelete );
*
* // Start the asynchronous delete operation here
*
* // Do NOT close the SharedData connection here nor delete the
* // RSharedDataClient object.
*
* // When the asynchronous request has completed:
*
* iSharedDataClient.CancelFreeDiskSpaceRequest();
* ------------------------------------------------------------------------------
*/

class RSharedDataClient : public RSessionBase
    {

    public:

        /**
        * Constructor.
        *
        * Use this constructor if you don't use notify methods. User implemented
        * notify handler is set to NULL.
        */      
        IMPORT_C RSharedDataClient();

        /**
        * Starts the server if it's not already running. Creates a new session.
        * If a session is already established by the client, does nothing.
        *
        * @param aPriority priority for the active object handling notify
        *                  events. If you haven't given MSharedDataNotifyHandler
        *                  interface in constructor, this has no effect.
        *
        * @return Symbian OS standard error code
        */   
        IMPORT_C TInt Connect();
        
        /**
        * Closes the session. Cancels all notify requests, signals and incoming
        * notifications. Saves the changes if the shared data file was modified.
        * Remember to call this to a connected session in some point.
        *
        * If the UID is handled by a plugin instead of file, 
        * client might need to call Flush() explicitly before Close.
        * Check plugin documentation.
        */
        IMPORT_C void Close();

        
        /**
        * Full disk support for synchronous transactional delete operations.
        * If needed, frees up disk space so that it'll be above the given
        * minimum treshold.
        *
        * Use CleanupStack::PopAndDestroy() to cancel the request after the
        * delete operation.
        *
        * This method can leave only if CleanupStack::PushL() leaves.
        *
        * @param aAmount Requested minimum amount of free disk space in bytes.
        *                The max size of the reserve file will be around
        *                64k so there is no use to request higher amounts
        *                than that.
        */
        IMPORT_C void RequestFreeDiskSpaceLC( const TInt aAmount ) const;


        /**
        * Full disk support for asynchronous transactional delete operations.
        * If needed, frees up disk space so that it'll be above the given
        * minimum treshold.
        *
        * The user is responsible for calling CancelFreeDiskSpaceRequest()
        * after the asynchronous delete operation has been finished.
        *
        * @param aAmount Requested minimum amount of free disk space in bytes.
        *                The max size of the reserve file will be around
        *                64k so there is no use to request higher amounts
        *                than that.
        */
        IMPORT_C void RequestFreeDiskSpace( const TInt aAmount ) const;

        /**
        * Cancels the request made by calling RequestFreeDiskSpace().
        * In case of RequestFreeDiskSpaceLC(), CleanupStack will handle
        * calling this function.
        */
        IMPORT_C void CancelFreeDiskSpaceRequest() const;

    private:
        /**
        * Cleanup function
        */
        static void CleanupFreeDiskSpaceRequest( TAny* aAny );

    private: // data

        CDosWrapper* iDosWrapper;
    private: // friend classes
    };

#endif

// End of File
