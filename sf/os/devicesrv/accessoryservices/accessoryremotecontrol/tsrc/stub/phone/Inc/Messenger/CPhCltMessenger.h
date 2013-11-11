/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Messenger interface: provides two-way communication between 
*                two or more clients.
*
*/


#ifndef CPhCltMessenger_H
#define CPhCltMessenger_H
#include <etelmm.h>

// FORWARD DECLARATIONS
class RPhCltServer;

// The default Messenger data size.
const TInt KPhCltDefMesSize = 64;


/**
* Enumerates asynchronous messenger requests.
* 
* @since 2.6.
* 
* EPhCltMesRequestReceive - request corresponding to the Receive method.
* EPhCltMesRequestSend - request corresponding to the Send method.
*/
enum TPhCltMessengerRequest
    {
    EPhCltMesRequestReceive = 1,
    EPhCltMesRequestSend = 2
    };
    
    


/**
* Messenger Negotiator commands.
*
* EPhCltMesCommandUnknown       The command was unknown.
* 
* EPhCltMesCommandSend          The Send command.
* EPhCltMesCommandReceive       The Receive command.
* EPhCltMesCommandSkip          The Skip command.
* 
* EPhCltMesConstruct            The Construct command.
* EPhCltMesCommandDelete        The Delete command.
*
* EPhCltMesCommandCancelSend    Send request canceled.
* EPhCltMesCommandCancelReceive Receive request canceled.
*/
enum TPhCltMessengerCommand
    {
    EPhCltMesCommandUnknown       = 0,

    EPhCltMesCommandReceive       = 1,
    EPhCltMesCommandSend          = 2,
    EPhCltMesCommandSkip          = 3,

    EPhCltMesCommandConstruct     = 4,
    EPhCltMesCommandDelete        = 5,

    EPhCltMesCommandCancelSend    = 6,
    EPhCltMesCommandCancelReceive = 7
    };
    
/**
* The parameter storage class used in PhoneClient and PhoneServer
* data transfer.
* 
* @since 2.6.
*/
class TPhCltMessengerParameters
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        inline TPhCltMessengerParameters() :
            iMessengerCommand( EPhCltMesCommandUnknown ), 
            iDefaultMessageSize( 0 ),
            iReceiveMessageSizePtr( NULL ),
            iReceiveBufferMaxSize( 0 ),
            iSendPayloadLength( 0 ),
            iCancelRequest( EPhCltMesRequestReceive )
            {
            }

    public:    // Data

        // The Messenger Negotiator command.
        TPhCltMessengerCommand  iMessengerCommand;

        // The message types that are listened.
        TUid                    iCategoryUid;

        // The default message size.
        TInt                    iDefaultMessageSize;

        // The pointer to the descriptor that contains the 
        // received message size TInt.
        TAny*                   iReceiveMessageSizePtr;

        // The maximum size of the receive buffer.
        TInt                    iReceiveBufferMaxSize;

        // The length of the Send data.
        TInt                    iSendPayloadLength;

        // The canceled command.
        TPhCltMessengerRequest  iCancelRequest;
    };

/**
*  Messenger interface: provides two-way communication between 
*  two or more clients. 
*  Meant for Phone and Video Telephony application information transfer.
*
*  @lib PhoneClient.lib
*  @since 3.2 
*/
class CPhCltMessenger : public CBase
    {
    
    public:  // Constructors  
      
        /**
        * Two-phased constructor.
        *
        * @param aCategoryUid Category of messages to be sent or received.
        *                     Note that there can be many kinds of message uids
        *                     below one category uid.
        * @param aDefaultMessageSize The default size of the messages to be sent
        *                            or received in bytes. Can be bigger than 
        *                            KPhCltDefMesSize, but then functionality
        *                            is not guaranteed.
        */
        IMPORT_C static CPhCltMessenger* NewL( const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize );
        
    public: // New functions

        /**
        * Receives next message. No memory is allocated during the operation of
        * this method.
        * 
        * @param aStatus This request will be completed once message has been 
        *                received.
        * @param aMessageSize Tells the size of the received message. If bigger
        *        than the size of the aMessagePayload, then only the beginning 
        *        of the received message is copied to aMessagePayload. This
        *        means that either Receive must be called with bigger buffer to 
        *        receive full message or Skip must be called to indicate that
        *        the full message is not needed.
        *                     
        * @param aMessagePayload Storage to the message to be received. The 
        *                        format of the message is determined by the 
        *                        category Uid used in Open method.
        *                        .
        */ 
        IMPORT_C virtual void Receive(
            TRequestStatus& aStatus,
            TInt& aMessageSize,
            TDes8& aMessagePayload ) = 0;


        /**
        * Sends message. This is completed only after message has been
        * delivered to all receivers of the same category.
        * No memory is allocated during the operation of this method.
        * 
        * @param aStatus This request will be completed once message has been 
        *                sent.
        * @param aMessagePayload The sent message.The format of the message is 
        *                determined by the category Uid used in Open method.
        */
        IMPORT_C virtual void Send(
            TRequestStatus& aStatus,
            const TDesC8& aMessagePayload ) = 0;


        /**
        * Cancel request. This method should only be called in destruction 
        * to make sure that messages are not missed.
        * 
        * @param aRequest request to be canceled.
        */
        IMPORT_C virtual void CancelRequest( 
            const TPhCltMessengerRequest& aRequest ) = 0;


        /**
        * Skip one (i.e. next) message. This method should be called only if 
        * Receive completion indicates that there is not enough space in 
        * aMessagePayload.
        * Of course, you should attempt to recover, i.e. try to call Receive 
        * with big enough aMessagePayload buffer before calling this method.
        */
        IMPORT_C virtual void Skip() = 0;
        
    protected:

        /**
        * C++ constructor.
        */
        CPhCltMessenger();

    };


#endif      // CPhCltMessenger_H 

// End of File
