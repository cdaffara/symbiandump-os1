/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CCDSYDOSServerRequestManager class
*
*/


#ifndef CDSYDOSSERVERREQUESTMANAGER_H
#define CDSYDOSSERVERREQUESTMANAGER_H

// INCLUDES
#include <e32base.h>
#include "CDSYServiceLogic.h"


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class TCDSYMessage;
class CCDSYMessageBufferItem;
class CCDSYController;


/**
*  CCDSYDOSServerRequestManager
*  This class implements the CCDSYDOSServerRequestManager, 
*  which sends the messages to controller and gets the responses to the messages.
*/
NONSHARABLE_CLASS( CCDSYDOSServerRequestManager ): public CBase, public MCDSYServiceLogic
    {
    public:
        /**
        * Destructor.
        */
        ~CCDSYDOSServerRequestManager();

        /**
        * Creates a new instance of CCDSYDOSServerRequestManager.
        * @param CCDSYController& aCDSYController   Reference to CCDSYController.
        * @return CCDSYDOSServerRequestManager*     Pointer to an instance of CCDSYDOSServerRequestManager.
        */
        static CCDSYDOSServerRequestManager* NewL( CCDSYController& aCDSYController );

        /**
        * Sends the message to CCDSYController.     Provides waits for the response to the message.
        * @param TCDSYMessage& aCDSYMessage         Reference to the message to be sent.
        * @return void 
        */
        void SendMessageL( TCDSYMessage& aCDSYMessage );

        /**
        * Sends the message to CCDSYController.     Returns after the message has been sent.
        * @param TCDSYMessage& aCDSYMessage         Reference to the message to be sent.
        * @param RMessagePtr2 aMessage              Pointer to the message.
        * @return void 
        */
        void SendMessageL( TCDSYMessage& aCDSYMessage, RMessagePtr2 aMessage );

        /**
        * Processes the response to the message sent.
        * @param TCDSYMessage& aCDSYMessage         Reference to the message received.
        * @return void 
        */
        void ProcessMessageL( TCDSYMessage& aCDSYMessage );

        /**
        * Expires the message. Does nothing.
        * @param TCDSYMessage& aCDSYMessage         Message to be expired.
        * @return void 
        */
        void ExpireMessageL( TCDSYMessage& aCDSYMessage );

        /**
        * Cancels the async request.
        * @param TUint32 aTransactionID             Transaction ID.
        * @param TInt aReason                       Cancel reason.
        * @return void 
        */
        void Cancel( TUint32 aTransactionID, TInt aReason );

    private:
        /**
        * Constructor.
        * @param CCDSYController& aCDSYController   Reference to CCDSYController.
        */
        CCDSYDOSServerRequestManager( CCDSYController& aCDSYController );

        /**
        * 2nd phase constructor.
        * @return void 
        */
        void ConstructL();

    private:
        CCDSYController& iCDSYController;               // Reference to CCDSYController.
        RPointerArray<CCDSYMessageBufferItem> iMessages;// Messages waiting for response.
        
    public:
        TBool iLDSYFind;
    };



#endif //#ifndef CDSYDOSSERVERREQUESTMANAGER_H



//  End of File
