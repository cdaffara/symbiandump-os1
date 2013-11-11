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
* Description:  Declaration of CCDSYMessageBufferItem class
*
*/


#ifndef CDSYMESSAGEBUFFERITEM_H
#define CDSYMESSAGEBUFFERITEM_H

// INCLUDES
#include <e32base.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CCDSYController;
class TCDSYMessage;
class CCDSYDOSServerRequestManager;



/**
*  CCDSYMessageBufferItem
*  This class implements the CCDSYMessageBufferItem, which keeps the DSY message in buffer and waits for the response.
*/
NONSHARABLE_CLASS( CCDSYMessageBufferItem ): public CBase
    {
    public:
        /**
        * Destructor.
        */
        ~CCDSYMessageBufferItem();

        /**
        * Creates a new instance of CCDSYMessageBufferItem.
        * @param CCDSYController& aCDSYController                           Reference to CCDSYController.
        * @param CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager Reference to CCDSYDOSServerRequestManager.
        * @param TCDSYMessage& aCDSYMessage                                 Reference to the message to be sent.
        * @return static CCDSYMessageBufferItem*                            Pointer to instance of CCDSYMessageBufferItem.
        */
        static CCDSYMessageBufferItem* NewL( CCDSYController& aCDSYController, CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager, TCDSYMessage& aCDSYMessage );

        /**
        * Creates a new instance of CCDSYMessageBufferItem.
        * @param CCDSYController& aCDSYController                           Reference to CCDSYController.
        * @param CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager Reference to CCDSYDOSServerRequestManager.
        * @param TCDSYMessage& aCDSYMessage                                 Reference to the message to be sent.
        * @param RMessagePtr2 aMessage                                      Client/server message.
        * @return static CCDSYMessageBufferItem*                            Pointer to instance of CCDSYMessageBufferItem.
        */
        static CCDSYMessageBufferItem* NewL( CCDSYController& aCDSYController, CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager, TCDSYMessage& aCDSYMessage, RMessagePtr2 aMessage );
        
        /**
        * Sends the message to the controller and waits for the response.
        * @return void
        */
        void SendMessageL();

        /**
        * Processes the response to the message sent.
        * @param const TCDSYMessage& aCDSYMessage   Reference to CCDSYMessage to be sent.
        * @return void 
        */
        void ProcessResponseL( const TCDSYMessage& aCDSYMessage );

        /**
        * Cancels the Async command.
        * @param TInt aReason   Cancel reason.
        * @return void 
        */
        void CancelAsyncCommand( TInt aReason ) const;

        /**
        * Returns the messages transaction ID.
        * @return TUint32       Transaction ID.
        */
        inline TUint32 TransactionID() const
            {
            return iTransactionID;
            }

        /**
        * Returns if the message is Async.
        * @return TBool         Async flag.
        */
        inline TBool IsAsyncMessage()
            {
            return !iClientServerMessage.IsNull();
            }

    private:
        /**
        * Constructor.
        * @param CCDSYController& aCDSYController                           Reference to CCDSYController.
        * @param CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager Reference to CCDSYDOSServerRequestManager.
        * @param TCDSYMessage& aCDSYMessage                                 Reference to the message to be sent.
        */
        CCDSYMessageBufferItem( CCDSYController& aCDSYController, CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager, TCDSYMessage& aCDSYMessage );

        /**
        * 2nd phase constructor.
        * @return void 
        */
        void ConstructL();

        /**
        * Starts waiting for the resopnse to the message.
        * @return void 
        */
        void BeginActiveWait();

        /**
        * Ends the waiting for the response to the message.
        * @return void 
        */
        void EndActiveWait();

        /**
        * Timer callback function.
        * @param TAny* aPtr     Pointer to data.
        * @return static TInt   Error code.
        */
        static TInt NotifyTimerCallback( TAny* aPtr );

        /**
        * Timer callback function.
        * @return void 
        */
        void DoNotifyTimerCallbackL();

    private:
        CCDSYController& iCDSYController;                           // Reference to CCDSYController
        CCDSYDOSServerRequestManager& iCDSYDOSServerRequestManager; // Request manager
        TCDSYMessage* iCDSYMessage;                                 // Message in buffer
        HBufC8* iMessageData;                                       // Messages data
        TBool iMessageOwned;                                        // The owned flag tells the destructor that the message needs to be deleted
        TBool iActiveWait;                                          // Wait boolean flag
        CActiveSchedulerWait iWait;                                 // Wait object
        CPeriodic* iTimer;                                          // Expiration timer
        TUint32 iTransactionID;                                     // Messages transaction ID
        RMessagePtr2 iClientServerMessage;                          // Async message
    };



#endif //#ifndef CDSYMESSAGEBUFFERITEM_H



//  End of File
