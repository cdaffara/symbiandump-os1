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
* Description:  Server-side sub-session of Accessory BT Control
*
*/



#ifndef CACCSRVSUBBTCONTROL_H
#define CACCSRVSUBBTCONTROL_H

// INCLUDES
#include "AccSrvSubControl.h"
#include <e32base.h>
#include <btmanclient.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION

/**
*  Accessory BT Control server-side sub-session implementation
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubBTControl ) : public CAccSrvSubControl
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since S60 3.1
        * @param aSession              Handle to the ASY Proxy.
        * @param aConnectionController Handle to the Connection Controller.
        * @param aServerModel          Handle to the Server Model.
        * @return Pointer to a new subsession
        */
        static CAccSrvSubBTControl* NewL( CASYProxyAPI* aSession,
                                          CAccSrvConnectionController* aConnectionController,
                                          CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        ~CAccSrvSubBTControl();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CAccSrvSubBase
        * Handles received messages from client.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void DispatchMessageL( const RMessage2& aMessage );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        *
        * @since S60 3.1
        * @param aMsgID     Message in question.
        * @param aErrorCode Error Code of the operation.
        * @param aUniqueID  Unique ID of the Generic ID in question.
        * @return ETrue if the message was consumpted by a observer.
        *               Call-back order must be renewed after consumption
        *               if further call-backs are needed.
        */
        TBool CompleteMessageL( TMsgID aMsgID,
                                TInt aErrorCode,
                                TInt aUniqueID );

    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvSubBTControl( CASYProxyAPI* aSession,
                             CAccSrvConnectionController* aConnectionController,
                             CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:  // New functions

        /**
        * Handles Bluetooth ConnectAccessory() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void BluetoothConnectAccessoryL( const RMessage2& aMessage );

        /**
        * Handles Bluetooth DisconnectAccessory() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void BluetoothDisconnectAccessoryL( const RMessage2& aMessage );

        /**
        * Handles BluetoothAudioLinkOpenRespL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void BluetoothAudioLinkOpenRespL( const RMessage2& aMessage );

        /**
        * Handles BluetoothAudioLinkCloseRespL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void BluetoothAudioLinkCloseRespL( const RMessage2& aMessage );

        /**
        * Handles BluetoothAudioLinkOpenedNotifyL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void BluetoothAudioLinkOpenedNotifyL( const RMessage2& aMessage );

        /**
        * Handles BluetoothAudioLinkClosedNotifyL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void BluetoothAudioLinkClosedNotifyL( const RMessage2& aMessage );

        /**
        * Handles NotifyBluetoothAudioLinkOpenReq() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyBluetoothAudioLinkOpenReqL( const RMessage2& aMessage );

        /**
        * Handles NotifyBluetoothAudioLinkCloseReq() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyBluetoothAudioLinkCloseReqL( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryValueChanged() TBool client request (From BTControl API).
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyBTAccessoryValueChangedTBoolL( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryValueChanged() TInt client request (From BTControl API).
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.2
        * @param aMessage Client request.
        * @return void
        */
        void NotifyBTAccessoryValueChangedTIntL( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyBluetoothAudioLinkOpenReq() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyBluetoothAudioLinkOpenReq( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyBluetoothAudioLinkCloseReq() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyBluetoothAudioLinkCloseReq( const RMessage2& aMessage );

        /**
        * Handles CancelConnectAccessory() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelConnectAccessory( const RMessage2& aMessage );

        /**
        * Handles CancelDisconnectAccessory() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelDisconnectAccessory( const RMessage2& aMessage );

        /**
        * This will reset the AudioLinkOpen cancellation flag.
        * The flag is reseted if both BluetoothAudioLinkOpenRespL() and 
        * BluetoothAudioLinkOpenedNotifyL() are received.
        * The flag is also reseted if BluetoothAudioLinkOpenRespL() is
        * called with error response (i.e. AudioLinkOpen is failed)
        *
        * @since S60 3.1
        * @param aResponse Indicates the status of the BluetoothAudioLinkOpenRespL().
        * @return void
        */
        void ResetAudioLinkOpenCancelFlag( TInt aResponse );
        
        /**
        * This will reset the AudioLinkClose cancellation flag.
        * The flag is reseted if both BluetoothAudioLinkCloseRespL() and 
        * BluetoothAudioLinkClosedNotifyL() are received.
        * The flag is also reseted if BluetoothAudioLinkCloseRespL() is
        * called with error response (i.e. AudioLinkClose is failed)
        *
        * @since S60 3.1
        * @param aResponse Indicates the status of the BluetoothAudioLinkCloseRespL().
        * @return void
        */
        void ResetAudioLinkCloseCancelFlag( TInt aResponse );
        
        /**
        * Convert TBTDevAddr to TUint64 format.
        *
        * @since S60 3.1
        * @param aBTDevAddr BT device address.
        * @return BT device address in TUint64 format.
        */
        TUint64 MakeTUint64( const TBTDevAddr& aBTDevAddr );

    public:     // Data

    protected:  // Data

    private:    // Data

        //Stored BT ConnectAccessory() client request for later completion.
        RMessage2 iConnectBluetoothAccessoryMsg;

        //Stored NotifyBluetoothAudioLinkCloseReq() client request for later completion.
        RMessage2 iNotifyBluetoothAudioLinkCloseReqMsg;

        //Stored NotifyBluetoothAudioLinkOpenReq() client request for later completion.
        RMessage2 iNotifyBluetoothAudioLinkOpenReqMsg;
        
        //This will count that both BluetoothAudioLinkOpenRespL() and 
        //BluetoothAudioLinkOpenedNotifyL() are received.
        TBool iAudioLinkOpenCancelFlagResetCalledTwice;
        
        //This will count that both BluetoothAudioLinkCloseRespL() and 
        //BluetoothAudioLinkClosedNotifyL() are received.
        TBool iAudioLinkCloseCancelFlagResetCalledTwice;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBBTCONTROL_H

// End of File
