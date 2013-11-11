/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Subsession handles are stored here for later handling
*
*/



#ifndef CACCSRVNOTIFICATIONQUEUE_H
#define CACCSRVNOTIFICATIONQUEUE_H

//  INCLUDES
#include <e32std.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDArray.h>
#include <AccPolAccessoryMode.h>
#include <AsyCmdTypes.h>
#include <AccessoryTypes.h>

// CONSTANTS

// MACROS

// DATA TYPES
enum TMsgID
    {
    EConnectAccessory,                      //ConnaecAccessory
    ENewAccessoryConnected,                 //NotifyNewAccessoryConnected
    EAccessoryConnectionStatusChanged,      //NotifyAccessoryConnectionStatusChanged
    EAccessoryConnectionStatusAsyncChanged, //GetAccessoryConnectionStatusAsync
    EAccessoryModeChanged,                  //NotifyAccessoryModeChanged
    EAccessoryModeAsyncChanged,             //GetAccessoryModeAsync
    EAccessoryDisconnected,                 //NotifyAccessoryDisconnected
    EAccessoryValueChangedTInt,             //NotifyAccessoryValueChangedTInt
    EAccessoryValueChangedTBool,            //NotifyAccessoryValueChangedTBool
    EAccessoryValueChangedObject,           //NotifyAccessoryValueChangedObject
    EBluetoothAudioLinkOpenReq,             //NotifyBluetoothAudioLinkOpenReq
    EBluetoothAudioLinkCloseReq,            //NotifyBluetoothAudioLinkCloseReq
    EAccessoryAudioLinkOpened,              //NotifyAccessoryAudioLinkOpened
    EAccessoryAudioLinkClosed,              //NotifyAccessoryAudioLinkClosed
    EAccessoryAudioLinkOpen,                //AccessoryAudioLinkOpen
    EAccessoryAudioLinkClose,               //AccessoryAudioLinkClose
    EBluetoothAudioLinkOpenResp,            //BluetoothAudioLinkOpenResp
    EBluetoothAudioLinkCloseResp,           //BluetoothAudioLinkCloseResp
    ENotifyProcessCommand,                  //NotifyProcessCommand
    ESetValue,                              //SetValueL
    EGetValue,                              //GetValueL
    EUnknown                                //Initialisation value for error tracking
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MQueueObserver;

// CLASS DECLARATION

/**
*  Queue class for observers to register and complete messages
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvNotificationQueue ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvNotificationQueue* NewL();

        /**
        * Destructor.
        */
        virtual ~CAccSrvNotificationQueue();

    public: // New functions

        /**
        * Register a observer to the queue.
        * @since S60 3.1
        * @param aMsgID    Message to which the observer is registered to.
        * @param aCallback Handle to observer.
        * @return void
        */
        void RegisterControlMessage( TMsgID aMsgID,
                                     MQueueObserver* aCallback );

        /**
        * Cancels registeration to the queue.
        * @since S60 3.1
        * @param aMsgID    Message to which the observer registereation is cancelled.
        * @param aCallback Handle to observer.
        * @return void
        */
        void CancelControlMessage( TMsgID aMsgID,
                                   MQueueObserver* aCallback );

        /**
        * Completes the message(s) from the queue. Call-back will be called
        * for all registered observers for message given as a parameter.
        * @since S60 3.1
        * @param aMsgID     Message to complete.
        * @param aErrorCode Error code for call-back.
        * @param aUniqueID  Generic IDs Unique ID for call-back.
        * @return void
        */
        void CompleteControlMessageL( TMsgID aMsgID,
                                      TInt aErrorCode,
                                      TInt aUniqueID );

        /**
        * Completes the message(s) from the queue. Call-back will be called
        * for all registered observers for message given as a parameter.
        * @since S60 3.1
        * @param aMsgID       Message to complete.
        * @param aCmdResponse Response value from ASY for call-back.
        * @param aErrorCode   Error code for call-back.
        * @param aTrId        Transaction ID for call-back.
        * @return void
        */
        void CompleteValueMessageL( TMsgID aMsgID,
                                    TAccValueTypeTBool aCmdResponse,
                                    TInt aErrorCode,
                                    TInt aTrId );

        /**
        * Completes the message(s) from the queue. Call-back will be called
        * for all registered observers for message given as a parameter.
        * @since S60 3.1
        * @param aMsgID       Message to complete.
        * @param aCmdResponse Response value from ASY for call-back.
        * @param aErrorCode   Error code for call-back.
        * @param aTrId        Transaction ID for call-back.
        * @return void
        */
        void CompleteValueMessageL( TMsgID aMsgID,
                                    TAccValueTypeTInt aCmdResponse,
                                    TInt aErrorCode,
                                    TInt aTrId );

        /**
        * Completes the message(s) from the queue. Call-back will be called
        * for all registered observers for message given as a parameter.
        * @since S60 3.1
        * @param aMsgID       Message to complete.
        * @param aCmdResponse Response value from ASY for call-back.
        * @param aErrorCode   Error code for call-back.
        * @param aTrId        Transaction ID for call-back.
        * @return void
        */
        void CompleteValueMessageL( TMsgID aMsgID,
                                    TDesC8& aCmdResponse,
                                    TInt aErrorCode,
                                    TInt aTrId );

        /**
        * Completes the message(s) from the queue. Call-back will be called
        * for all registered observers for message given as a parameter.
        * @since S60 3.1
        * @param aMsgID                 Message to complete.
        * @param aCmdn                  ASY command for call-back.
        * @param aTrId                  Transaction ID for call-back.
        * @param aASYCommandParamRecord ASY command parameters for call-back.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return void
        */
        void CompleteASYCmdL( TMsgID aMsgID,
                              TProcessCmdId aCmdn,
                              TInt aTrId,
                              TASYCommandParamRecord& aASYCommandParamRecord,
                              TPtr8* aPtrBuf=NULL );

       /**
        * Completes the message(s) from the queue. Call-back will be called
        * for all registered observers for message given as a parameter.
        * @param aMsgID       Message to complete.
        * @param aTrId        Transaction ID for call-back.
        * @param aPtrBuf      Response value from ASY for call-back.
        *                     Pointer to CBufFlat buffer where CAccPolObjectCon object 
        *                     is externalized to.
        * @param aErrorCode   Error code for call-back.
        * @return void
        */
        void CompleteValueMessageL( TMsgID aMsgID,
                                    TInt aTrId,
                                    TPtr8* aPtrBuf,
                                    TInt aErrorCode );


        /**
        * Gets the number of waiting control messages.
        * @since S60 3.2
        * @param aMsgID                 Message to check.
        * @return The number of waiting messages
        */
        TInt ControlMessagesWaiting( TMsgID aMsgID );
        
    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccSrvNotificationQueue();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Return reference to correct member array.
        */
        RArray<MQueueObserver*>* ControlMessageArray( TMsgID aMsgID );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Outstanding subsessions for "ConnectAccessory" request
        RArray<MQueueObserver *> iConnectAccessory; //not owned

        // Outstanding subsessions for "NewAccessoryConnected" notification
        RArray<MQueueObserver *> iNewAccessoryConnectedArray; //not owned

        // Outstanding subsessions for "AccessoryDisconnected" notification
        RArray<MQueueObserver *> iAccessoryDisconnectedArray; //not owned

        // Outstanding subsessions for "AccessoryModeChanged" notification
        RArray<MQueueObserver *> iAccessoryModeChangedArray; //not owned

        // Outstanding subsessions for "GetAccessoryModeAsync"
        RArray<MQueueObserver *> iGetAccessoryModeAsyncArray; //not owned

        // Outstanding subsessions for "AccessoryConnectionStatusChanged" notification
        RArray<MQueueObserver *> iAccessoryConnectionStatusChangedArray; //not owned

        // Outstanding subsessions for "GetAccessoryConnectionStatusAsync"
        RArray<MQueueObserver *> iGetAccessoryConnectionStatusAsyncArray; //not owned

        // Outstanding subsessions for "AccessoryAudioLinkOpen" requests
        RArray<MQueueObserver *> iAccessoryAudioLinkOpenArray; //not owned

        // Outstanding subsessions for "NotifyAccessoryAudioLinkOpened" notification
        RArray<MQueueObserver *> iAccessoryAudioLinkOpenedArray; //not owned

        // Outstanding subsessions for "AccessoryAudioLinkClosed" notification
        RArray<MQueueObserver *> iAccessoryAudioLinkClosedArray; //not owned

        // Outstanding subsessions for "AccessoryAudioLinkClose" request
        RArray<MQueueObserver *> iAccessoryAudioLinkCloseArray; //not owned

        // Outstanding subsessions for "BluetoothAudioLinkOpenReq" notification
        RArray<MQueueObserver *> iBluetoothAudioLinkOpenReqArray; //not owned

        // Outstanding subsessions for "BluetoothAudioLinkCloseReq" notification
        RArray<MQueueObserver *> iBluetoothAudioLinkCloseReqArray; //not owned

        // Outstanding subsessions for "BluetoothAudioLinkOpenResp" notification
        RArray<MQueueObserver *> iBluetoothAudioLinkOpenRespArray; //not owned

        // Outstanding subsessions for "BluetoothAudioLinkCloseResp" notification
        RArray<MQueueObserver *> iBluetoothAudioLinkCloseRespArray; //not owned

        // Outstanding subsessions for "NotifyProcessCommand" request
        RArray<MQueueObserver *> iNotifyProcessCommandArray; //not owned

        // Outstanding subsessions for "AccessoryValueChangedTBool" notification
        RArray<MQueueObserver *> iNotifyAccessoryValueChangedTBoolArray; //not owned

        // Outstanding subsessions for "AccessoryValueChangedTInt" notification
        RArray<MQueueObserver *> iNotifyAccessoryValueChangedTIntArray; //not owned

        // Outstanding subsessions for "AccessoryValueChangedObject" notification
        RArray<MQueueObserver *> iNotifyAccessoryValueChangedObjectArray; //not owned

        // Outstanding subsessions for SetValueL request
        RArray<MQueueObserver *> iSetValueArray; //not owned

        // Outstanding subsessions for GetValueL request
        RArray<MQueueObserver *> iGetValueArray; //not owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // CACCSRVNOTIFICATIONQUEUE_H

// End of File
