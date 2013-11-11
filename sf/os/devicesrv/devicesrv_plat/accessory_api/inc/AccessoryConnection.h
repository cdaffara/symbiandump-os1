/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



#ifndef RACCESSORYCONNECTION_H
#define RACCESSORYCONNECTION_H

//  INCLUDES
#include "AccessoryConnectionBase.h"

// FORWARD DECLARATIONS
class TAccPolGenericIDArray;

// CLASS DECLARATION

/**
*  Accessory Connection sub-session.
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessoryConnection : public RAccessoryConnectionBase
    {
    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryConnection();

    public:

        /**
        * Creates a new sub-session within an existing session.
        *
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @return KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt CreateSubSession( RAccessoryServer& aSession );

        /**
        * Closes the RAccessoryConnection sub-session.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C TInt CloseSubSession();

        /**
        * Get combined accessory connection status, i.e. all currently
        * connected accessories.
        *
        * @since S60 3.1
        * @param aGenericIdArray On return, contains the array of currently connected
        *                        accessories. Accessories are in connection order,
        *                        last connected accessory in index 0.
        * @return KErrNone if successful.
        *         KErrNotReady if a device is not ready to start operation
        */
        IMPORT_C TInt GetAccessoryConnectionStatus( 
                        TAccPolGenericIDArray& aGenericIdArray ) const;

        /**
        * Get combined accessory connection status, i.e. all currently
        * connected accessories.
        *
        * @since S60 3.1
        * @param aGenericIdArray On successful request completion, contains the
        *                        array of currently connected accessories.
        *                        Accessories are in connection order,
        *                        last connected sccessory in index 0.
        * @param aStatus         Indicates the completion status of a request.
        *                        KErrNone if successful, system-wide error code if failed.
        *                        Specifically:
        *                        KErrAlreadyExists, if request allready exist
        *                        KErrCancel, if request is cancelled
        * @return void
        */
        IMPORT_C void GetAccessoryConnectionStatus( 
                        TRequestStatus& aStatus, TAccPolGenericIDArray& aGenericIdArray );

        /**
        * Issues a request for accessory connection status change notification.
        * Request must be renewed after completion if further notifications
        * are needed.
        *
        * @since S60 3.1
        * @param aGenericIdArray On successful request completion, contains the
        *                        array of currently connected accessories.
        *                        Accessories are in connection order,
        *                        last connected sccessory in index 0.
        * @param aStatus         Indicates the completion status of a request.
        *                        KErrNone if successful, system-wide error code if failed.
        *                        Specifically:
        *                        KErrAlreadyExists, if request allready exist
        *                        KErrCancel, if request is cancelled
        * @return void
        */
        IMPORT_C void NotifyAccessoryConnectionStatusChanged( 
                        TRequestStatus& aStatus, TAccPolGenericIDArray& aGenericIdArray );

        /**
        * Cancel NotifyAccessoryConnectionStatusChanged().
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void CancelNotifyAccessoryConnectionStatusChanged() const;

        /**
        * Cancel GetAccessoryConnectionStatus().
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void CancelGetAccessoryConnectionStatus() const;

    private:

        TAccSrvGenericIDArrayPckg iNotifyConnectionStatusChangedGIDArrayPckg;
        TAccSrvGenericIDArrayPckg iGetConnectionStatusGIDArrayPckg;
        TInt                      iFlags;
        TPckg<TInt>               iFlagsPckg;
        TPckg<TInt>               iReserved1;
        TPckg<TInt>               iReserved2;

    };

#endif      // RACCESSORYCONNECTION_H

// End of File
