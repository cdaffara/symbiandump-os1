/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#ifndef BTGPSCONNECTMANAGER_H
#define BTGPSCONNECTMANAGER_H

//  INCLUDES

#include <e32base.h> 
#include <es_sock.h>
#include "BTGPSMessageListener.h"
#include "BTGPSConnectManagerExt.h"

// CONSTANTS

// MACROS

// DATA TYPES
/**
* Structure of handler item.
*/
struct THandlerProcessItem
    {
    TInt iHandlerId;        ///The id of this handler
    TInt iSuccessNodeIndex; ///If handler succeed, the index of next node
    TInt iFailedNodeIndex;  ///if  handler failed, the index of next node
    };
        

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSSettingManager;
class MBTGPSMessageListener;
class CBTGPSDeviceManager;
class CBTDevice;
class MBTGPSConnectStatusObserver;
class CBTGPSMessageSender;
class CBTGPSMessageReceiver;
class CBTGPSDeviceListManager;

// CLASS DECLARATION

/**
* Manager class that manage all operation related to Bluetooth connection. This
* class provide interface for other object to connect and disconnect to bluetooth 
* device. It also provide observer interface to other object to get device 
* connection status change. This class also provides observer interface for other
* object get messages from connected BT device. 
* The class defines connection process that defines how  
* to connect to bluetooth device. Each process contains handlers that handle 
* BT connection activities. 
*
*/
class CBTGPSConnectManager: public CBase, private MBTGPSConnectManagerExt
    {
    public: 

        /**
        * Two-phase construction.
        */
        static CBTGPSConnectManager * NewL(
            CBTGPSSettingManager& aSettingManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSConstantsManager& aConstantsManager,
            CBTGPSDeviceListManager& aDeviceListManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSConnectManager();

    public:
        /**
        * Start connecting to BT device.
        * @return KErrCoundNotConnect if the device is in Error status
        */
        TInt Connecting();
        
        /**
        * Reconnecting to GPS. This function will reconnect to GPS device
        * even the GPS device is in error state.
        */
        void Reconnecting();
        
        /**
        * Cancel connecting. This function will also terminate current
		* connection.
        */
        void CancelConnecting();

        /**
        * Add message listener.
		* @param aListener Reference to message listener.
        */
        void AddMessageListenerL(MBTGPSMessageListener& aListener);

        /**
        * Remove message listener;
		* @param aListener Reference to message listener.
        */
        void RemoveMessageListener(MBTGPSMessageListener& aListener);
        
        /**
        * Send message to connected BT device. This function will leave if
		* the message can't be send, for example, the device is not connected, or
		* out of memory.
		* @param aMsg The message to be sent.
        */
        void SendMessageL(const TDesC8& aMsg);
        
        /**
        * Non-leave function to send message. 
		* @param aMsg The message to be sent.
		* @return KErrNone if the message is sent. Otherwise, system wide error code
		*         will be returned.
        */
        TInt SendMessage(const TDesC8& aMsg);
    
    private:
        /**
        * From MBTGPSConnectManagerExt
        */
        virtual CBTGPSDeviceManager& DeviceManager() const;

        /**
        * From MBTGPSConnectManagerExt
        */
        virtual CBTGPSConstantsManager& ConstantsManager() const;

        /**
        * From MBTGPSConnectManagerExt
        */
        virtual CBTGPSSettingManager& SettingManager() const;

        /**
        * From MBTGPSConnectManagerExt
        */
        virtual void GetSocket(
            RSocketServ*& aSocketServ,
            RSocket*& aSocket);

        /**
        * From MBTGPSConnectManagerExt
        */
        void DisconnectBtDevice();


        /**
        * From MBTGPSConnectManagerExt
        */ 
        virtual void HandlerComplete(TInt aId, TInt aErr);

        /**
		* From MBTGPSConnectManagerExt
		*/ 
        virtual CBTGPSDeviceListManager& DeviceListManager() const;
        
    private:
        /**
        * Start to connect to BT device
        */
        void StartConnectDevice();
    
        /**
        * Start connecting process
        */
        void StartConnectingProcessL(const THandlerProcessItem* aProcess);
        
        /**
        * Connect process completed
        */
        void ConnectComplete(TInt aErr = KErrNone);
        
        /**
        * Idle callback to start connect process
        * If process failed, device will be set to connect error status
        */
        void StartConnectIdle();
        
        /**
        * Idle callback to start connect process
        */
        void StartConnectIdleL();
        
        /**
        * Idle callback to start handler
        */
        void StartNextHandler();
        
        /**
        * Static idle callback
        */
        static TInt StaticIdleCallback(TAny* aAny);
        
        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSConnectManager(
            CBTGPSSettingManager& aSettingManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSConstantsManager& aConstantsManager,
            CBTGPSDeviceListManager& aDeviceListManager);
        
    private:
        //Setting Manager
        CBTGPSSettingManager& iSettingManager;

        //BT Device Manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Constants Manager
        CBTGPSConstantsManager& iConstantsManager;
        
        //Device List Manager
        CBTGPSDeviceListManager& iDeviceListManager;

        //Active Process;
        const THandlerProcessItem* iActiveProcess;

        //active handler
        CBase* iActiveHandler;
        
        //Current node index
        TInt iCurrentNode;
        
        //Last error code
        TInt iLastError;
        
        //Socket server
        RSocketServ iSocketServ;
        
        //Socket
        RSocket iSocket;
        
        //Message sender
        CBTGPSMessageSender* iMessageSender;
        
        //Message receiver
        CBTGPSMessageReceiver* iMessageReceiver;
        
        //Idle handler for start connecting process
        CIdle* iIdle;

    };
#endif
// End of File

