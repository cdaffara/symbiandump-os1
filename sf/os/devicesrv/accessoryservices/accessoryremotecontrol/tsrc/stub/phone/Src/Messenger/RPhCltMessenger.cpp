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
* Description:  This class is used to perform Phone Client Extension 
*                Messenger originated commands.
*
*/



// INCLUDES

#include    "RPhCltMessenger.h"
#include    <RPhCltServer.h>
#include    "PhCltClientServer.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltMessenger::RPhCltMessenger
// 
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RPhCltMessenger::RPhCltMessenger()
:   RSubSessionBase(),
    iReceiveMessageSize( NULL, 0 ),
    iMesParPckg( iMessengerParameters )
    {
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::~RPhCltMessenger
// 
// Destructor.
// -----------------------------------------------------------------------------
//
RPhCltMessenger::~RPhCltMessenger()
    {
    // Ensure that the subsession is closed.
    Close();
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::Open
// 
// Open subsession to Phone Server.
// -----------------------------------------------------------------------------
//
TInt RPhCltMessenger::Open(
    RPhCltServer& aServer,
    const TUid& aCategoryUid,
    const TInt& aDefaultMessageSize )
    {
    __ASSERT_ALWAYS( aServer.Handle(), User::Panic( 
            KPhClientPanicCategory, 
            EPhCltClientSidePanicNullHandle ) );

    TInt err = CreateSubSession( 
        aServer, 
        EPhoneServerMessengerSubSessionOpen, 
        TIpcArgs() );


    // If subsession was created successfully, construct it properly.
    if ( !err )
        {
        // Set the needed variables to be transferred to PhoneServer side.
        iMessengerParameters.iCategoryUid = aCategoryUid;
        iMessengerParameters.iDefaultMessageSize = aDefaultMessageSize;

        err = SendReceive( 
            EPhoneServerMessengerSubSessionConstruct, 
            TIpcArgs( &iMesParPckg ) );

        // If memory reservations failed, close subsession.
        if ( err )
            {
            Close();
            }
        }
    
    return err;
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::Close
// 
// Close subsession.
// -----------------------------------------------------------------------------
//
void RPhCltMessenger::Close()
    {
    CloseSubSession( EPhoneServerMessengerSubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::Receive
// 
// Receive message.
// -----------------------------------------------------------------------------
//
void RPhCltMessenger::Receive(
    TRequestStatus& aStatus,
    TInt& aMessageSize,
    TDes8& aMessagePayload )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // Set message size variables to be byte descriptor.
    iReceiveMessageSize.Set( ( TUint8* )&aMessageSize, 0, sizeof( TInt ) ) ;
    iMessengerParameters.iReceiveMessageSizePtr = &iReceiveMessageSize;

    iMessengerParameters.iReceiveBufferMaxSize = aMessagePayload.MaxSize();

    SendReceive( 
        EPhoneServerMessengerSubSessionReceive, 
        TIpcArgs( 
            &iMesParPckg,
            &aMessagePayload,
            &iReceiveMessageSize ), 
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::Send
// 
// Send message.
// -----------------------------------------------------------------------------
//
void RPhCltMessenger::Send(
    TRequestStatus& aStatus,
    const TDesC8& aMessagePayload )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    iMessengerParameters.iSendPayloadLength = aMessagePayload.Length();

    SendReceive( 
        EPhoneServerMessengerSubSessionSend, 
        TIpcArgs( 
            &iMesParPckg, 
            &aMessagePayload ), 
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::CancelRequest
// 
// Cancel ongoing request.
// -----------------------------------------------------------------------------
//
void RPhCltMessenger::CancelRequest( const TPhCltMessengerRequest& aRequest )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    iMessengerParameters.iCancelRequest = aRequest;

    SendReceive( 
        EPhoneServerMessengerSubSessionCancel, 
        TIpcArgs( &iMesParPckg ) );
    }


// -----------------------------------------------------------------------------
// RPhCltMessenger::Skip
// 
// Skip the current message.
// -----------------------------------------------------------------------------
//
void RPhCltMessenger::Skip()
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    SendReceive( 
        EPhoneServerMessengerSubSessionSkip, 
        TIpcArgs()  );
    }

            
// End of File
