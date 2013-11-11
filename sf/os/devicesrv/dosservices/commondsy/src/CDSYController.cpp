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
* Description:  Implementation of CCDSYController class
*
*/


// INCLUDE FILES 
#include "CDSYController.h"
#include "CDSYMessage.h"
#include "CDSYServiceLogic.h"
#include "CDSYMessagesServiceLogic.h"
#include "commondsy_debug.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
const TInt KServiceLogicArrayGranularity = 100;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CCDSYController::CCDSYController
// -----------------------------------------------------------------------------
CCDSYController::CCDSYController()
: iCurrentTransactionID( 0 )
, iServiceLogics( KServiceLogicArrayGranularity )
    {
    COM_TRACE_( "CDSY - CCDSYController::CCDSYController()" );
    COM_TRACE_( "CDSY - CCDSYController::CCDSYController - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::~CCDSYController
// -----------------------------------------------------------------------------
CCDSYController::~CCDSYController()
    {
    COM_TRACE_( "CDSY - CCDSYController::~CCDSYController()" );

    iServiceLogics.ResetAndDestroy();

    COM_TRACE_( "CDSY - CCDSYController::~CCDSYController - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::NewL
// -----------------------------------------------------------------------------
CCDSYController* CCDSYController::NewL()
    {
    COM_TRACE_( "CDSY - CCDSYController::NewL()" );

    CCDSYController* controller = new ( ELeave ) CCDSYController();

    COM_TRACE_1( "CDSY - CCDSYController::NewL - return 0x%x", controller );
    return controller;
    }



// -----------------------------------------------------------------------------
// CCDSYController::ProcessMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CCDSYController::ProcessMessageL( TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_1( "CDSY - CCDSYController::ProcessMessageL(0x%x)", &aCDSYMessage );

    TUint count = 0;
    for ( TInt i = 0; i < iServiceLogics.Count(); i++ )
        {
        if ( ( ( iServiceLogics[i] )->MessageGroup() == aCDSYMessage.Group() )
            && ( ( iServiceLogics[i] )->MessageID() == aCDSYMessage.ID() )
            && ( ( ( iServiceLogics[i] )->DynamicRegistration() == EFalse )
                || ( ( ( iServiceLogics[i] )->DynamicRegistration() )
                    && ( ( iServiceLogics[i] )->MessageTransactionID() == aCDSYMessage.TransactionID() ) ) ) )
            {
            count++;
            ( ( iServiceLogics[i] )->CDSYServiceLogic() )->ProcessMessageL( aCDSYMessage );
            }
        }
    TRACE_ASSERT( count > 0 );
    TRACE_ASSERT( count < 2 );
   
    COM_TRACE_( "CDSY - CCDSYController::ProcessMessageL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::RegisterServiceLogicL
// -----------------------------------------------------------------------------
EXPORT_C void CCDSYController::RegisterServiceLogicL( const TUint32 aMessageGroup, const TUint32 aMessageID, MCDSYServiceLogic* aCDSYServiceLogic )
    {
    COM_TRACE_3( "CDSY - CCDSYController::RegisterServiceLogicL(0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aCDSYServiceLogic );

#ifdef _DEBUG

    // Check that this hasn't been already registered
    for ( TInt i = 0; i < iServiceLogics.Count(); i++ )
        {
        if ( ( ( iServiceLogics[i] )->MessageGroup() == aMessageGroup )
            && ( ( iServiceLogics[i] )->MessageID() == aMessageID ) )
            {
            TRACE_ASSERT_ALWAYS;
            }
        }

#endif //#ifdef _DEBUG

    CCDSYMessagesServiceLogic* serviceLogic = CCDSYMessagesServiceLogic::NewL( aMessageGroup, aMessageID, aCDSYServiceLogic );
    TInt err = iServiceLogics.Append( serviceLogic );
    if ( err != KErrNone )
        {
        TRACE_ASSERT_ALWAYS;
        delete serviceLogic;
        User::Leave( err );
        }
     
    COM_TRACE_( "CDSY - CCDSYController::RegisterServiceLogicL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::RegisterServiceLogicL
// -----------------------------------------------------------------------------
EXPORT_C void CCDSYController::RegisterServiceLogicL( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aTransactionID, MCDSYServiceLogic* aCDSYServiceLogic )
    {
    COM_TRACE_4( "CDSY - CCDSYController::RegisterServiceLogicL(0x%x, 0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aTransactionID, aCDSYServiceLogic );

#ifdef _DEBUG

    // Check that this hasn't been already registered
    for ( TInt i = 0; i < iServiceLogics.Count(); i++ )
        {
        if ( ( ( iServiceLogics[i] )->MessageGroup() == aMessageGroup )
            && ( ( iServiceLogics[i] )->MessageID() == aMessageID ) )
            {
            TRACE_ASSERT_ALWAYS;
            }
        }

#endif //#ifdef _DEBUG

    CCDSYMessagesServiceLogic* serviceLogic = CCDSYMessagesServiceLogic::NewL( aMessageGroup, aMessageID, aTransactionID, aCDSYServiceLogic );
    TInt err = iServiceLogics.Append( serviceLogic );
    if ( err != KErrNone )
        {
        TRACE_ASSERT_ALWAYS;
        delete serviceLogic;
        User::Leave( err );
        }
   
    COM_TRACE_( "CDSY - CCDSYController::RegisterServiceLogicL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::UnregisterServiceLogic
// -----------------------------------------------------------------------------
EXPORT_C void CCDSYController::UnregisterServiceLogic( const TUint32 aMessageGroup, const TUint32 aMessageID, const MCDSYServiceLogic* aCDSYServiceLogic )
    {
    COM_TRACE_3( "CDSY - CCDSYController::UnregisterServiceLogic(0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aCDSYServiceLogic );

    for ( TInt i = iServiceLogics.Count() - 1; i >= 0; i-- )
        {
        if ( ( ( iServiceLogics[i] )->CDSYServiceLogic() == aCDSYServiceLogic )
            && ( ( iServiceLogics[i] )->MessageGroup() == aMessageGroup )
            && ( ( iServiceLogics[i] )->MessageID() == aMessageID ) )
            {
            delete iServiceLogics[i];
            iServiceLogics.Remove( i );
            i = -1;
            }
        }

    COM_TRACE_( "CDSY - CCDSYController::UnregisterServiceLogic - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::UnregisterServiceLogic
// -----------------------------------------------------------------------------
EXPORT_C void CCDSYController::UnregisterServiceLogic( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aTransactionID, const MCDSYServiceLogic* aCDSYServiceLogic )
    {
    COM_TRACE_4( "CDSY - CCDSYController::UnregisterServiceLogic(0x%x, 0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aTransactionID, aCDSYServiceLogic );

    for ( TInt i = iServiceLogics.Count() - 1; i >= 0; i-- )
        {
        if ( ( ( iServiceLogics[i] )->CDSYServiceLogic() == aCDSYServiceLogic )
            && ( ( iServiceLogics[i] )->MessageGroup() == aMessageGroup )
            && ( ( iServiceLogics[i] )->MessageID() == aMessageID )
            && ( ( iServiceLogics[i] )->MessageTransactionID() == aTransactionID ) )
            {
            delete iServiceLogics[i];
            iServiceLogics.Remove( i );
            i = -1;
            }
        }

    COM_TRACE_( "CDSY - CCDSYController::UnregisterServiceLogic - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::ExpireMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CCDSYController::ExpireMessageL( TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_1( "CDSY - CCDSYController::ExpireMessageL(0x%x)", &aCDSYMessage );

    TUint count = 0;
    for ( TInt i = 0; i < iServiceLogics.Count(); i++ )
        {
        if((((iServiceLogics[i])->MessageGroup() == aCDSYMessage.Group())
                && ((iServiceLogics[i])->MessageID() == aCDSYMessage.ID())
                && ((iServiceLogics[i])->DynamicRegistration() == EFalse))
            || (((iServiceLogics[i])->DynamicRegistration() != EFalse)
                && ((iServiceLogics[i])->MessageTransactionID() == aCDSYMessage.TransactionID())))
            {
            count++;
            ( ( iServiceLogics[i] )->CDSYServiceLogic() )->ExpireMessageL( aCDSYMessage );
            }
        if(((iServiceLogics[i])->DynamicRegistration() != EFalse)
            && ((iServiceLogics[i])->MessageTransactionID() == aCDSYMessage.TransactionID()))
            {
            delete iServiceLogics[i];
            iServiceLogics.Remove( i );
            i--;
            }
        }
    TRACE_ASSERT( count > 0 );
    TRACE_ASSERT( count < 2 );

    COM_TRACE_( "CDSY - CCDSYController::ExpireMessageL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYController::GetNewTransactionID
// -----------------------------------------------------------------------------
EXPORT_C TUint8 CCDSYController::GetNewTransactionID()
    {
    COM_TRACE_( "CDSY - CCDSYController::GetNewTransactionID()" );

    iCurrentTransactionID++;

    COM_TRACE_1( "CDSY - CCDSYController::GetNewTransactionID - return 0x%x", iCurrentTransactionID );
    return iCurrentTransactionID;
    }
    
