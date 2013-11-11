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




#ifndef BTGPSMESSAGESENDER_H
#define BTGPSMESSAGESENDER_H

//  INCLUDES

#include <e32base.h>
#include <badesca.h>
#include "BTGPSDeviceListener.h"


// CONSTANTS
/**
* The size of the output buffer size. This
* have to be longer than longest NMEA message
*/
const TInt KSendMessageSize = 200;


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RSocket;
class CBTGPSDeviceManager;

// CLASS DECLARATION

/**
*  This class is used to send messages to connected BT device. 
*  It doesn't garentee the message can be received from connected
*  BT device. The message are buffered if necessary to prevent
*  jams on BT connection.
*  
*/
class CBTGPSMessageSender: public CActive, private MBTGPSDeviceListener
    {
    public: 

        /**
        * Two-phase construction.
        * @param aSocket Reference to RSocket.
        * @param aDeviceManager Reference to device manager
        */
        static CBTGPSMessageSender * NewL(
            RSocket& aSocket,
            CBTGPSDeviceManager& aDeviceManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSMessageSender();
        
        /**
        * Send message
        * @param aMsg Message to be sent.
        */
        void SendL(const TDesC8& aMsg);

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
        * From MBTGPSDeviceListener
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
        * Private constructor
        */
        CBTGPSMessageSender(
            RSocket& aSocket,
            CBTGPSDeviceManager& aDeviceManager);
        
        /**
        * Start timer
        */
        void StartTimer();
        
        /**
        * Timer callback
        */
        void TimerTick();
        
        /**
        * Static timer callback
        */
        static TInt TimerCallback(TAny* aAny);
        
    private:
        //Socket
        RSocket& iSocket;
        
        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Sending buffer
        CDesC8ArrayFlat* iBuffer;
        
        //Timer
        CPeriodic* iTimer;
    };
#endif
// End of File

