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
* Description: ?Description
*
*/



#ifndef RACCESSORYSINGLECONNECTION_H
#define RACCESSORYSINGLECONNECTION_H

//  INCLUDES
#include "AccessoryConnectionBase.h"


// FORWARD DECLARATIONS
class CAccPolObjectBase;

// CLASS DECLARATION

/**
*  Accessory Single Connection sub-session represents single accessory
*  connection. The class provides methods for detecting new accessory
*  connection and disconnection as well as observing changes in internal
*  status of accessory connection.
*
*  Example:
*    - New single accessory connection is created when wired car kit is
*      connected to the mobile terminal.
*    - Internal status of accessory connection will change when wired car kit
*      privacy handset status changes (on-hook / off-hook).
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessorySingleConnection : public RAccessoryConnectionBase
    {
    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessorySingleConnection();

    public:

        /**
        * Creates a new sub-session within an existing session.
        *
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @return KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C virtual TInt CreateSubSession(RAccessoryServer& aSession);

        /**
        * Closes the RAccessorySingleConnection sub-session.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C virtual TInt CloseSubSession();

        /**
        * Issues a request for new accessory connected notification.
        * Request must be renewed after completion if further notifications
        * are needed.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId On succesful request completion, contains
        *                   the updated Generic ID of the connected accessory
        * @return void
        */
        IMPORT_C void NotifyNewAccessoryConnected( TRequestStatus& aStatus,
                                                   TAccPolGenericID& aGenericId );

        /**
        * Issues a request for accessory disconnected notification.
        * Request must be renewed after completion if further notifications
        * are needed.
        *
        * Note: If client needs to listen disconnection for more than one
        *       existing accessory, separate sub-session(s) need to be created
        *       for each connected accessory.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId Generic ID of the target accessory.
        * @return void
        */
        IMPORT_C void NotifyAccessoryDisconnected( TRequestStatus& aStatus,
                                                   const TAccPolGenericID& aGenericId );

        /**
        * Issues a request for the specific accessory capability
        * value changed notification. Request must be renewed after completion
        * if further notifications are needed.
        *
        * Note: If client needs to listen internal changes of accessory
        *       connection status for more than one existing accessory,
        *       separate sub-session(s) need to be created for each connected
        *       accessory.
        *
        * @since S60 3.1
        * @param aStatus     Indicates the completion status of a request.
        *                    KErrNone if successful, system-wide error code if failed.
        *                    Specifically:
        *                    KErrAlreadyExists, if request for another
        *                                       Generic ID allready exist
        *                    KErrCancel, if request is cancelled (this happens e.g.
        *                                when target accessory is disconnected)
        *                    KErrNotFound, if target accessory is not connected
        *                    KErrNotSupported, if Accessory capability is
        *                                      not supported
        * @param aGenericId  Generic ID of the target accessory
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @return void
        */
        IMPORT_C void NotifyAccessoryValueChanged( TRequestStatus& aStatus,
                                                   const TAccPolGenericID& aGenericId,
                                                   const TAccPolNameRecord& aNameRecord,
                                                   TAccValueTypeTBool& aValue );

        /**
        * Issues a request for the specific accessory capability
        * value changed notification. Request must be renewed after completion
        * if further notifications are needed.
        *
        * Note: If client needs to listen internal changes of accessory
        *       connection status for more than one existing accessory,
        *       separate sub-session(s) need to be created for each connected
        *       accessory.
        *
        * @since S60 3.1
        * @param aStatus     Indicates the completion status of a request.
        *                    KErrNone if successful, system-wide error code if failed.
        *                    Specifically:
        *                    KErrAlreadyExists, if request for another
        *                                       Generic ID allready exist
        *                    KErrCancel, if request is cancelled (this happens e.g.
        *                                when target accessory is disconnected)
        *                    KErrNotFound, if target accessory is not connected
        *                    KErrNotSupported, if accessory capability is
        *                                      not supported
        * @param aGenericId  Generic ID of the target accessory
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @return void
        */
        IMPORT_C void NotifyAccessoryValueChanged( TRequestStatus& aStatus,
                                                   const TAccPolGenericID& aGenericId,
                                                   const TAccPolNameRecord& aNameRecord,
                                                   TAccValueTypeTInt& aValue );
        
        /**
        * Issues a request for the specific accessory capability
        * value changed notification. Request must be renewed after completion
        * if further notifications are needed.
        *
        * Note: If client needs to listen internal changes of accessory
        *       connection status for more than one existing accessory,
        *       separate sub-session(s) need to be created for each connected
        *       accessory.
        *       
        * @since S60 5.2
        * @param aStatus     Indicates the completion status of a request.
        *                    KErrNone if successful, system-wide error code if failed.
        *                    Specifically:
        *                    KErrAlreadyExists, if request for another
        *                                       Generic ID allready exist
        *                    KErrCancel, if request is cancelled (this happens e.g.
        *                                when target accessory is disconnected)
        *                    KErrNotFound, if target accessory is not connected
        *                    KErrNotSupported, if accessory capability is
        *                                      not supported
        * @param aGenericId  Generic ID of the target accessory
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @exception If the buffer for aValue cannot be created a leave will occur.
        * @return void
        */
        IMPORT_C void NotifyAccessoryValueChanged( TRequestStatus& aStatus,
                                                   const TAccPolGenericID& aGenericId,
                                                   const TAccPolNameRecord& aNameRecord,
                                                   TAccValueTypeObject& aValue );
        
        /**
        * Cancel NotifyNewAccessoryConnected().
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void CancelNotifyNewAccessoryConnected() const;

        /**
        * Cancel NotifyAccessoryDisconnected().
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void CancelNotifyAccessoryDisconnected() const;

        /**
        * Cancel NotifyAccessoryValueChanged().
        *
        * @since S60 3.1
        * @param aNameRecord Accessory capability, identifies the notification
        *                    to be cancelled. If empty, all pending
        *                    NotifyAccessoryValueChanged() requests are cancelled.
        * @return void
        */
        IMPORT_C void CancelNotifyAccessoryValueChanged( 
                        const TAccPolNameRecord& aNameRecord ) const;

    private:  //Data

        TAccSrvGenericIDPckg        iNotifyNewAccessoryConnectedGIDPckg;
        TAccSrvGenericIDPckgBuf     iNotifyAccessoryDisconnectedGIDPckgBuf;
        TAccSrvGIDNameRecordPckgBuf iNotifyValueChangedTBoolNameRecordPckgBuf;
        TAccSrvValueTypeTBoolPckg   iNotifyValueChangedTBoolValuePckg;
        TAccSrvGIDNameRecordPckgBuf iNotifyValueChangedTIntNameRecordPckgBuf;
        TAccSrvValueTypeTIntPckg    iNotifyValueChangedTIntValuePckg;
        TAccSrvGIDNameRecordPckgBuf iNotifyValueChangedObjectNameRecordPckgBuf;
        TInt                        iFlags;
        TPckg<TInt>                 iFlagsPckg;
        TPckg<TInt>                 iReserved2;
        TPckg<TInt>                 iReserved3;
    };

#endif      // RACCESSORYSINGLECONNECTION_H

// End of File
