/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef RACCESSORYCONTROL_H
#define RACCESSORYCONTROL_H

//  INCLUDES
#include <AccessoryConnection.h>

// FORWARD DECLARATIONS
class TAccPolGenericID;
class CAccConGenericID;
class CAccPolObjectCon;

// CLASS DECLARATION

/**
*  Accessory Control sub-session.
*
*  This class is not intended for user derivation.
*
*  @lib AccControl.lib
*  @since S60 3.1
*/
class RAccessoryControl : public RAccessoryConnection
    {
    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryControl();

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
        * Closes the RAccessoryControl sub-session.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C TInt CloseSubSession();

        /**
        * ASY has detected accessory connection and requests 
        * the Accessory Server to connect the accessory. 
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericID On successful request completion, contains
        *                   the updated Generic ID of the connected accessory
        * @return void
        * @see DisconnectAccessory
        */
        /**
        * Deprecated, will be removed.
        */
        IMPORT_C void ConnectAccessory( TRequestStatus& aStatus,
                                        TAccPolGenericID& aGenericID )
        #ifndef __WINSCW__
        __attribute__ ((deprecated))
        #endif
        ;

        /**
        * ASY has detected accessory connection and requests 
        * the Accessory Server to connect the accessory. 
        *
        * If this method is called for already connected accessory it is interpreted
        * as a update for existing connection.		
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist.
        *                   KErrCancel, if request is cancelled.
        * @param aGenericId On successful request completion, contains
        *                   the updated Generic ID of the connected accessory.
		* @param aEvaluateConnectionRules
		*					Indicates that accessory server evaluates connection rules
		*					for this connection.
        * @return void
        * @see DisconnectAccessory
        */
        IMPORT_C void ConnectAccessory( TRequestStatus& aStatus,
                                        CAccConGenericID* const aGenericID,
                                        TBool aEvaluateConnectionRules );
                                        
        /**
        * ASY has detected accessory disconnection and requests 
        * the Accessory Server to disconnect the accessory. 
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId Generic ID of the accessory to be disconnected
        * @return void
        * @see ConnectAccessory
        */
        IMPORT_C void DisconnectAccessory( TRequestStatus& aStatus,
                                           const TAccPolGenericID& aGenericId );

        /**
        * Notifies value change of specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericId  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      Value for the specific accessory capability.
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        */
        IMPORT_C void AccessoryValueChangedNotifyL( const TAccPolGenericID& aGenericId,
                                                    const TAccPolNameRecord& aNameRecord,
                                                    const TAccValueTypeTBool aValue ) const;

        /**
        * Notifies value change of specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericId  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      Value for the specific accessory capability.
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        */
        IMPORT_C void AccessoryValueChangedNotifyL( const TAccPolGenericID& aGenericId,
                                                    const TAccPolNameRecord& aNameRecord,
                                                    const TAccValueTypeTInt aValue ) const;
        
        /**
        * Notifies value change of specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 5.2
        * @param aGenericId  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      Value for the specific accessory capability.
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        *            KErrTooBig The aValue is too big to be streamed.
        */
        IMPORT_C void AccessoryValueChangedNotifyL( const TAccPolGenericID& aGenericId,
                                                    const TAccPolNameRecord& aNameRecord,
                                                    const CAccPolObjectCon& aValue ) const;
        
        /**
        * Set the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericId  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      Value for the specific accessory capability.
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        *         KErrServerBusy if the server is busy handling another request.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        */
        IMPORT_C TInt SetValueL( const TAccPolGenericID& aGenericId,
                                 const TAccPolNameRecord& aNameRecord,
                                 const TAccValueTypeTBool aValue );

        /**
        * Set the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericId  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      Value for the specific accessory capability.
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        *         KErrServerBusy if the server is busy handling another request.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        */
        IMPORT_C TInt SetValueL( const TAccPolGenericID& aGenericId,
                                 const TAccPolNameRecord& aNameRecord,
                                 const TAccValueTypeTInt aValue );
                                 
        /**
        * Set the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *        
        * @param aGenericId  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      Value for the specific accessory capability.        
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        *         KErrServerBusy if the server is busy handling another request.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        * @see accpolobjectbase.h
        * @see accpolobjectcon.h
        * @see AccPolCommonNameValuePairs.h
        */
        IMPORT_C TInt SetValueL( const TAccPolGenericID& aGenericId,
                                 const TAccPolNameRecord& aNameRecord,
                                 const CAccPolObjectCon& aValue );                                 
                                                                  
        /**
        * Cancel ConnectAccessory().
        *
        * @since S60 3.1
        * @return void
        * @see ConnectAccessory
        */
        IMPORT_C void CancelConnectAccessory() const;

        /**
        * Cancel DisconnectAccessory().
        *
        * @since S60 3.1
        * @return void
        * @see DisconnectAccessory
        */
        IMPORT_C void CancelDisconnectAccessory() const;

    private:  //Data

        TAccSrvGenericIDPckg        iConnectGIDPckg;
        TAccSrvGenericIDPckgBuf     iDisconnectGIDPckgBuf;
        TPckgBuf<TUint>             iThisThreadIDPckgBuf;
        TAccSrvConnectRecordPckgBuf iConnectRecordPckgBuf;
        TInt                        iFlags;
        TPckg<TInt>                 iFlagsPckg;        
        TPckg<TInt>                 iReserved1;
        TPckg<TInt>                 iReserved2;

    };

#endif      // RACCESSORYCONTROL_H

// End of File

