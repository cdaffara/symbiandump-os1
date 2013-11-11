/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef RACCESSORYBTCONTROL_H
#define RACCESSORYBTCONTROL_H

//  INCLUDES
#include <AccessoryControl.h>
#include <bttypes.h>//TBTDevAddr

// DATA TYPES
typedef TPckg<TBTDevAddr>    TAccSrvBTDevAddrPckg;
typedef TPckgC<TBTDevAddr>   TAccSrvBTDevAddrPckgC;
typedef TPckgBuf<TBTDevAddr> TAccSrvBTDevAddrPckgBuf;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Accessory Bluetooth Control sub-session.
*
*  This class is not intended for user derivation.
*
*  @lib AccBTControl.lib
*  @since S60 3.1
*/
class RAccessoryBTControl : public RAccessoryControl
    {
    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryBTControl();

    public:

        /**
        * Creates a new sub-session within an existing session.
        *
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @return KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C virtual TInt CreateSubSession( RAccessoryServer& aSession );

        /**
        * Closes the RAccessoryBTControl sub-session.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C virtual TInt CloseSubSession();

        /**
        * Bluetooth subsystem has detected accessory connection and requests 
        * the Accessory Server to connect the accessory. 
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aBTDevAddr Bluetooth device address of connected BT accessory
        * @return KErrNone if successful, system-wide error code if failed.
        * @see DisconnectAccessory
        *
        * Note: Opened Bluetooth control subsessions must be explicitly closed
        *       by the client. A memory leak will result if
        *       the RAccessoryBTControl object is destroyed before
        *       the subsession is closed.
        */
        IMPORT_C void ConnectAccessory( TRequestStatus& aStatus,
                                        const TBTDevAddr& aBTDevAddr );

        /**
        * Bluetooth subsystem has detected accessory disconnection and requests 
        * the Accessory Server to disconnect the accessory. 
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        *                   KErrNotFound, if accessory is not found
        * @param aBTDevAddr Bluetooth device address of disconnected BT accessory
        * @return void
        * @see ConnectAccessory
        */
        IMPORT_C void DisconnectAccessory( TRequestStatus& aStatus,
                                           const TBTDevAddr& aBTDevAddr );

        /**
        * This method is used to send response for previously received 
        * open audio link request.
        *
        * @since S60 3.1
        * @param aBTDevAddr Bluetooth device address
        * @param aResponse  Indicates the status of a request
        *                   KErrNone if successful, or a Symbian OS standard error code
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range
        * @see NotifyBluetoothAudioLinkOpenReq
        */
        IMPORT_C void BluetoothAudioLinkOpenRespL( const TBTDevAddr& aBTDevAddr,
                                                   TInt& aResponse ) const;

        /**
        * This method is used to send response for previously received 
        * close audio link request.
        *        
        * @since S60 3.1
        * @param aBTDevAddr Bluetooth device address
        * @param aResponse Indicates the status of a request
        *                  KErrNone if successful, or a Symbian OS standard error code
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range
        * @see NotifyBluetoothAudioLinkCloseReq
        */
        IMPORT_C void BluetoothAudioLinkCloseRespL( const TBTDevAddr& aBTDevAddr,
                                                    TInt& aResponse ) const;

        /**
        * Audio link is open. Bluetooth subsystem uses this
        * method to notify that audio link is opened towards remote accessory.
        *
        * @since S60 3.1
        * @param aBTDevAddr Bluetooth device address
        * @param aAudioType Type of the audio connection
        * @return void
        * @see NotifyBluetoothAudioLinkOpenReq
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        *            KErrAlreadyExists if audio link open is not possible at the moment.
        *                              This means that audio is routed to wired accessory.
        */
        IMPORT_C void BluetoothAudioLinkOpenedNotifyL( const TBTDevAddr& aBTDevAddr,
                                                       TAccAudioType aAudioType ) const;

        /**
        * Audio link is closed. Bluetooth subsystem uses this
        * method to notify that audio link is closed towards remote accessory.
        *
        * @since S60 3.1
        * @param aBTDevAddr Bluetooth device address
        * @param aAudioType Type of the audio connection
        * @return void
        * @see NotifyBluetoothAudioLinkCloseReq
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range
        */
        IMPORT_C void BluetoothAudioLinkClosedNotifyL( const TBTDevAddr& aBTDevAddr,
                                                       TAccAudioType aAudioType ) const;

        /**
        * This method is used to order notification of 
        * bluetooth audio link open request.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aBTDevAddr On successful request completion, contains
        *                   the updated Bluetooth device address
        * @param aAudioType On successful request completion, contains
        *                   the type of audio connection
        * @return void
        * @see BluetoothAudioLinkOpenResp
        */
        IMPORT_C void NotifyBluetoothAudioLinkOpenReq( TRequestStatus& aStatus,
                                                       TBTDevAddr& aBTDevAddr,
                                                       TAccAudioType& aAudioType );

        /**
        * This method is used to order notification of 
        * bluetooth audio link close request.
        *
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aBTDevAddr On successful request completion, contains
        *                   the updated Bluetooth device address
        * @param aAudioType On successful request completion, contains
        *                   the type of audio connection
        * @return void
        * @see BluetoothAudioLinkCloseResp
        */
        IMPORT_C void NotifyBluetoothAudioLinkCloseReq( TRequestStatus& aStatus,
                                                        TBTDevAddr& aBTDevAddr,
                                                        TAccAudioType& aAudioType );

        /**
        * Notifies value change of specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aBTDevAddr  Bluetooth device address.
        * @param aNameRecord Accessory capability name.
        * @param aValue      Value for the specific accessory capability.
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        */
        IMPORT_C void AccessoryValueChangedNotifyL( const TBTDevAddr& aBTDevAddr,
                                                    const TAccPolNameRecord& aNameRecord,
                                                    const TAccValueTypeTBool aValue ) const;

        /**
        * Notifies value change of specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.2
        * @param aBTDevAddr  Bluetooth device address.
        * @param aNameRecord Accessory capability name.
        * @param aValue      Value for the specific accessory capability.
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        */
        IMPORT_C void AccessoryValueChangedNotifyL( const TBTDevAddr& aBTDevAddr,
                                                    const TAccPolNameRecord& aNameRecord,
                                                    const TAccValueTypeTInt aValue ) const;
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

        /**
        * Cancel NotifyBluetoothAudioLinkOpenReq().
        *
        * @since S60 3.1
        * @return void
        * @see NotifyBluetoothAudioLinkOpenReq
        */
        IMPORT_C void CancelNotifyBluetoothAudioLinkOpenReq() const;

        /**
        * Cancel NotifyBluetoothAudioLinkCloseReq.
        *
        * @since S60 3.1
        * @return void
        * @see NotifyBluetoothAudioLinkCloseReq
        */
        IMPORT_C void CancelNotifyBluetoothAudioLinkCloseReq() const;

    private: //Data

        TAccSrvBTDevAddrPckgBuf  iConnectBTDevAddrPckgBuf;
        TPckgBuf<TUint>          iThisThreadIDPckgBuf;
        TAccSrvBTDevAddrPckgBuf  iDisconnectBTDevAddrPckgBuf;
        TAccSrvBTDevAddrPckg     iNotifyBTAudioLinkOpenReqBTAddrPckg;
        TAccSrvBTDevAddrPckg     iNotifyBTAudioLinkCloseReqBTAddrPckg;
        TAccSrvAudioTypePckg     iNotifyBTAudioLinkOpenReqAudioTypePckg;
        TAccSrvAudioTypePckg     iNotifyBTAudioLinkCloseReqAudioTypePckg;
        TInt                     iFlags;
        TPckg<TInt>              iFlagsPckg;
        TPckg<TInt>              iReserved1;
        TPckg<TInt>              iReserved2;

    };

#endif      // RACCESSORYBTCONTROL_H

// End of File
