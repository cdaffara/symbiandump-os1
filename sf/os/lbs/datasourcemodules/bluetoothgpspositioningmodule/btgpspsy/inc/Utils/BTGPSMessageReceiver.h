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




#ifndef BTGPSMESSAGERECEIVER_H
#define BTGPSMESSAGERECEIVER_H

//  INCLUDES

#include <e32base.h>
#include <badesca.h>
#include "BTGPSDeviceListener.h"
#include "BTGPSMessageDef.h"


// CONSTANTS
/**
* The size of the input buffer size. 
*/
const TInt KReadMessageSize = 120;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RSocket;
class MBTGPSMessageListener;
class CBTGPSDeviceManager;

// CLASS DECLARATION

/**
*  This class is used to receive message from socket. It contains buffer
*  that store the received message and informs all the listeners on received
*  message.
*  
*/
class CBTGPSMessageReceiver: public CActive, private MBTGPSDeviceListener
    {
    public: 

        /**
        * Two-phase construction.
        * @param aSocket Reference to RSocket.
        * @param aDeviceManager Reference to device manager.
        */
        static CBTGPSMessageReceiver * NewL(
            RSocket& aSocket,
            CBTGPSDeviceManager& aDeviceManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSMessageReceiver();
        
        /**
        * Add message listener
        * @param aListener Reference to listener
        */
        void AddMessageListenerL(MBTGPSMessageListener& aListener);

        /**
        * Remove message listener;
        * @param aListener Reference to listener
        */
        void RemoveMessageListener(MBTGPSMessageListener& aListener);
        
    private:
        /**
        * From CActive
        */
        virtual void RunL();
        
        /**
        * From CActive
        */
        virtual void DoCancel();
        
        /**
        * From CActive
        */
        virtual TInt RunError(TInt aError);
        
        /**
        * From MDeviceListener
        */
        virtual void BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt aErr=KErrNone);
        

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();
        
        /**
        * Receive message from socket
        */
        void Receive();
        
        /**
        * Handle received message from RSocket
        */
        void HandleReceivedMessage();

        /**
        * Inform listeners on received NMEA sentence
        */
        void InformListeners();
        
        /**
        * Private constructor
        */
        CBTGPSMessageReceiver(
            RSocket& aSocket,
            CBTGPSDeviceManager& aDeviceManager);
            
    private:
        //Socket
        RSocket& iSocket;
        
        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Message listener
        RArray<MBTGPSMessageListener*> iListenerArray;
        
        //Buffer for receiving
        TBuf8<KReadMessageSize> iBuffer;
        
        //Buffer for NMEA message
        TBuf8<KMaxNmeaMessageSize> iNmeaBuf;
        
        //Received message length
        TSockXfrLength iRecLen;
    };
#endif
// End of File

