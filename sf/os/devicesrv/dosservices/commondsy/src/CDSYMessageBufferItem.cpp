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
* Description:  Implementation of CDSYMessageBufferItem class
*
*/




// INCLUDE FILES 
#include "CDSYMessageBufferItem.h"
#include "CDSYController.h"
#include "CDSYMessage.h"
#include "CDSYMessageStructures.h"
#include "CDSYDOSServerRequestManager.h"
#include "commondsy_debug.h"



// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
#ifdef _DEBUG

    // 20 seconds
    const TUint32 KTransactionExpirationTime = 20000000;

#else

    // 10 seconds
    const TUint32 KTransactionExpirationTime = 10000000;

#endif



// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::NewL
// -----------------------------------------------------------------------------
CCDSYMessageBufferItem* CCDSYMessageBufferItem::NewL( CCDSYController& aCDSYController, CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager, TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_3( "CDSY - CCDSYMessageBufferItem::NewL(0x%x, 0x%x, 0x%x)", &aCDSYController, &aCDSYDOSServerRequestManager, &aCDSYMessage );
    
    CCDSYMessageBufferItem* messageBufferItem = new ( ELeave ) CCDSYMessageBufferItem( aCDSYController, aCDSYDOSServerRequestManager, aCDSYMessage );
    CleanupStack::PushL( messageBufferItem );
    messageBufferItem->ConstructL();
    messageBufferItem->iTransactionID = aCDSYMessage.TransactionID();
    CleanupStack::Pop( messageBufferItem );

    COM_TRACE_1( "CDSY - CCDSYMessageBufferItem::NewL - return 0x%x", messageBufferItem );
    return messageBufferItem;
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::NewL
// -----------------------------------------------------------------------------
CCDSYMessageBufferItem* CCDSYMessageBufferItem::NewL( CCDSYController& aCDSYController, CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager, TCDSYMessage& aCDSYMessage, RMessagePtr2 aMessage )
    {
    COM_TRACE_4( "CDSY - CCDSYMessageBufferItem::NewL(0x%x, 0x%x, 0x%x, 0x%x)", &aCDSYController, &aCDSYDOSServerRequestManager, &aCDSYMessage, &aMessage );
    
    TRACE_ASSERT( !aMessage.IsNull() );

    CCDSYMessageBufferItem* messageBufferItem = new ( ELeave ) CCDSYMessageBufferItem( aCDSYController, aCDSYDOSServerRequestManager, aCDSYMessage );
    CleanupStack::PushL( messageBufferItem );
    messageBufferItem->ConstructL();
    // The message and its data are copied since in asynchronous case there is no active wait
    // and the stack-allocated data gets deleted when SendMessageL returns
    if ( aCDSYMessage.Data() != NULL )
        {
        messageBufferItem->iMessageData = HBufC8::NewL( aCDSYMessage.Data()->Length() );
        *messageBufferItem->iMessageData = *aCDSYMessage.Data();
        }
    messageBufferItem->iCDSYMessage = new ( ELeave ) TCDSYMessage( aCDSYMessage.Group(), aCDSYMessage.ID(), messageBufferItem->iMessageData, aCDSYMessage.TransactionID() );
    // The owned flag tells the destructor that the message needs to be deleted
    messageBufferItem->iMessageOwned = ETrue;
    messageBufferItem->iClientServerMessage = aMessage;
    messageBufferItem->iTransactionID = aCDSYMessage.TransactionID();
    CleanupStack::Pop( messageBufferItem );

    COM_TRACE_1( "CDSY - CCDSYMessageBufferItem::NewL - return 0x%x", messageBufferItem );
    return messageBufferItem;
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::CCDSYMessageBufferItem
// -----------------------------------------------------------------------------
CCDSYMessageBufferItem::CCDSYMessageBufferItem( CCDSYController& aCDSYController, CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager, TCDSYMessage& aCDSYMessage )
: iCDSYController( aCDSYController )
, iCDSYDOSServerRequestManager( aCDSYDOSServerRequestManager )
, iCDSYMessage( &aCDSYMessage )
, iMessageData( NULL )
, iMessageOwned( EFalse )
, iActiveWait( EFalse )
, iTimer( NULL )
, iTransactionID( 0xFFFFFFFF )
    {
    COM_TRACE_3( "CDSY - CCDSYMessageBufferItem::CCDSYMessageBufferItem(0x%x, 0x%x, 0x%x)", &aCDSYController, &aCDSYDOSServerRequestManager, &aCDSYMessage );

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::CCDSYMessageBufferItem - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::~CCDSYMessageBufferItem
// -----------------------------------------------------------------------------
CCDSYMessageBufferItem::~CCDSYMessageBufferItem()
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::~CCDSYMessageBufferItem()" );

    delete iTimer;
    delete iMessageData;
    if ( iMessageOwned )
        {
        delete iCDSYMessage;
        }

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::~CCDSYMessageBufferItem - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::ConstructL
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::ConstructL()
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::ConstructL()" );
    
    iTimer = CPeriodic::NewL( CActive::EPriorityHigh );

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::ConstructL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::SendMessageL
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::SendMessageL()
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::SendMessage()" );
    
    if ( iClientServerMessage.IsNull() )
        {
        iCDSYController.ProcessMessageL( *iCDSYMessage );
        if ( iCDSYMessage->ReturnData() == NULL )
            {
            iTimer->Start( KTransactionExpirationTime,  KTransactionExpirationTime, TCallBack( NotifyTimerCallback, this ) );
            // Response not received, since the datacount is same.
            BeginActiveWait();
            }
        if ( iCDSYMessage->ReturnData() == NULL )
            {
            User::Leave( KErrGeneral );
            }
        }
    else
        {
        iTimer->Start( KTransactionExpirationTime,  KTransactionExpirationTime, TCallBack( NotifyTimerCallback, this ) );
        iCDSYController.ProcessMessageL( *iCDSYMessage );
        }

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::SendMessage - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::ProcessResponseL
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::ProcessResponseL( const TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_1( "CDSY - CCDSYMessageBufferItem::ProcessResponseL(0x%x)", &aCDSYMessage );

    if ( iClientServerMessage.IsNull() )
        {
        TRACE_ASSERT( aCDSYMessage.Data() != NULL );
        iCDSYMessage->SetReturnDataL( aCDSYMessage.Data() );
        EndActiveWait();
        }
    else
        {
        COM_TRACE_( "CDSY - CCDSYMessageBufferItem::ProcessResponseL - Completing async request" );
        TRACE_ASSERT( aCDSYMessage.Data() != NULL );
        TRACE_ASSERT( aCDSYMessage.Data()->Length() == sizeof ( TCDSYMsgDataReturn ) );
        iClientServerMessage.Complete( ( ( TCDSYMsgDataReturn* )aCDSYMessage.Data()->Ptr() )->iReturn );
        }

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::ProcessResponseL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::BeginActiveWait
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::BeginActiveWait()
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::BeginActiveWait()" );

    TRACE_ASSERT( iClientServerMessage.IsNull() );

    if ( iActiveWait )
        {
        TRACE_ASSERT_ALWAYS;
        }
    else
        {
        iActiveWait = ETrue;
        iWait.Start();
        }

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::BeginActiveWait - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::EndActiveWait
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::EndActiveWait()
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::EndActiveWait()" );

    TRACE_ASSERT( iClientServerMessage.IsNull() );
    if ( iActiveWait )
        {
        iWait.AsyncStop();
        iActiveWait = EFalse;
        iTimer->Cancel();
        }

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::EndActiveWait - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::NotifyTimerCallback
// -----------------------------------------------------------------------------
TInt CCDSYMessageBufferItem::NotifyTimerCallback( TAny* aPtr )
    {
    COM_TRACE_1( "CDSY - CCDSYMessageBufferItem::NotifyTimerCallback(0x%x)", aPtr );

    TRACE_ASSERT( aPtr != NULL );
    
    TInt err;
    TRAP( err, ( ( CCDSYMessageBufferItem* )aPtr )->DoNotifyTimerCallbackL() );
    TRACE_ASSERT( err == KErrNone );

    COM_TRACE_1( "CDSY - CCDSYMessageBufferItem::NotifyTimerCallback - return 0x%x", ETrue );
    return ETrue;
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::DoNotifyTimerCallbackL
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::DoNotifyTimerCallbackL()
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::DoNotifyTimerCallbackL()" );
    
    TRACE_ASSERT_ALWAYS;

    iCDSYController.ExpireMessageL( *iCDSYMessage );

    if ( iClientServerMessage.IsNull() )
        {
        EndActiveWait();
        }
    else
        {
        iCDSYDOSServerRequestManager.Cancel( iCDSYMessage->TransactionID(), KErrGeneral );
        }

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::DoNotifyTimerCallbackL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessageBufferItem::CancelAsyncCommand
// -----------------------------------------------------------------------------
void CCDSYMessageBufferItem::CancelAsyncCommand( TInt aReason ) const
    {
    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::CancelAsyncCommand()" );

    iClientServerMessage.Complete( aReason );

    COM_TRACE_( "CDSY - CCDSYMessageBufferItem::CancelAsyncCommand - return void" );
    }
    
