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
* Description:  This class is used to perform messenger originated commands.
*
*/


#ifndef RPhCltMessenger_H
#define RPhCltMessenger_H


//  INCLUDES
#include <e32base.h>
#include <CPhCltMessenger.h>

// FORWARD DECLARATIONS
class RPhCltServer;


/**
*  Messenger interface: provides two-way communication between 
*  two or more clients.
*
* @lib PhoneClient.lib 
* @since 3.2
*  
*/
class RPhCltMessenger
:   public RSubSessionBase
    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        RPhCltMessenger();

        /**
        * Desstructor.
        */
        ~RPhCltMessenger();


    public:

        /**
        * Open session ready to be sending and receiving messages.
        *
        * @param aServer      Pre-opened server session.
        * @param aCategoryUid Category of messages to be sent or received.
        *                     Note that there can be many kinds of message uids
        *                     below one category uid.
        * @param aDefaultMessageSize The default size of the messages to be sent
        *                            or received, In bytes. Can be bigger than 
        *                            KPhCltDefMesSize, but then functionality
        *                            is not guaranteed.
        * @return Error code. KErrNone if successful.
        */
        TInt Open(
            RPhCltServer& aServer,
            const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize );


        /**
        * Closes the session.
        */
        void Close();


        /**
        * Receives next message. No memory is allocated during the operation of
        * this method.
        * 
        * @param aStatus This request will be completed once message has been 
        *                received.
        * @param aMessageSize Tells the size of the received message. If smaller
        *        than the size of the aMessagePayload, then only the beginning 
        *        of the received message is copied to aMessagePayload.
        *                     
        * @param aMessagePayload Payload of the message. Contains the TUid at 
        *                        the beginning of the message when completed.
        */ 
        void Receive(
            TRequestStatus& aStatus,
            TInt& aMessageSize,
            TDes8& aMessagePayload );


        /**
        * Sends message. This is completed only after message has been
        * delivered to receiver.
        * No memory is allocated during the operation of this method.
        * 
        * @param aStatus This request will be completed once message has been 
        *                sent.
        * @param aMessagePayload Payload of the message.
        */
        void Send(
            TRequestStatus& aStatus,
            const TDesC8& aMessagePayload );


        /**
        * Cancel request. This method should only be called during destruction 
        * to make sure that messages are not missed.
        * 
        * @param aRequest request to be canceled.
        */
        void CancelRequest( const TPhCltMessengerRequest& aRequest );


        /**
        * Skip one message. This method should be called only if Receive 
        * indicates that there is not enough space in aMessagePayload.
        * Of course, you should attempt to recover, i.e. call Receive with big 
        * enough aMessagePayload buffer before calling this method.
        */
        void Skip();


    private:    // Data

        // Container for received message size.
        TPtr8                               iReceiveMessageSize;

        // Used in Receive and Send methods to contain the parameters.
        TPhCltMessengerParameters           iMessengerParameters;

        // The packaged version of the iMessengerParameters.
        TPckg< TPhCltMessengerParameters >  iMesParPckg;

    };

#endif      // RPhCltMessenger_H

// End of File
