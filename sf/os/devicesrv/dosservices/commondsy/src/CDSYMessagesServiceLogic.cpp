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
* Description:  Implementation of CCDSYMessagesServiceLogic class
*
*/




// INCLUDE FILES 
#include "CDSYMessagesServiceLogic.h"
#include "commondsy_debug.h"



// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CCDSYMessagesServiceLogic::CCDSYMessagesServiceLogic
// -----------------------------------------------------------------------------
CCDSYMessagesServiceLogic::CCDSYMessagesServiceLogic( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aMessageTransactionID, const TBool aDynamicRegistration, MCDSYServiceLogic* aCDSYServiceLogic )
: iMessageGroup( aMessageGroup )
, iMessageID( aMessageID )
, iMessageTransactionID( aMessageTransactionID )
, iDynamicRegistration( aDynamicRegistration )
, iCDSYServiceLogic( aCDSYServiceLogic )
    {
    COM_TRACE_4( "CDSY - CCDSYMessagesServiceLogic::CCDSYMessagesServiceLogic(0x%x, 0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aMessageTransactionID, aDynamicRegistration );
    COM_TRACE_1( "CDSY - CCDSYMessagesServiceLogic::CCDSYMessagesServiceLogic(0x%x)", aCDSYServiceLogic );    

    COM_TRACE_( "CDSY - CCDSYMessagesServiceLogic::CCDSYMessagesServiceLogic - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessagesServiceLogic::~CCDSYMessagesServiceLogic
// -----------------------------------------------------------------------------
CCDSYMessagesServiceLogic::~CCDSYMessagesServiceLogic()
    {
    COM_TRACE_( "CDSY - CCDSYMessagesServiceLogic::~CCDSYMessagesServiceLogic()" );

    iCDSYServiceLogic = NULL;

    COM_TRACE_( "CDSY - CCDSYMessagesServiceLogic::~CCDSYMessagesServiceLogic - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYMessagesServiceLogic::NewL
// -----------------------------------------------------------------------------
CCDSYMessagesServiceLogic* CCDSYMessagesServiceLogic::NewL( const TUint32 aMessageGroup, const TUint32 aMessageID, MCDSYServiceLogic* aCDSYServiceLogic )
    {
    COM_TRACE_3( "CDSY - CCDSYMessagesServiceLogic::NewL(0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aCDSYServiceLogic );

    CCDSYMessagesServiceLogic* messagesServiceLogic = new ( ELeave ) CCDSYMessagesServiceLogic( aMessageGroup, aMessageID, 0, EFalse, aCDSYServiceLogic );

    COM_TRACE_1( "CDSY - CCDSYMessagesServiceLogic::NewL - return 0x%x", messagesServiceLogic );
    return messagesServiceLogic;
    }



// -----------------------------------------------------------------------------
// CCDSYMessagesServiceLogic::NewL
// -----------------------------------------------------------------------------
CCDSYMessagesServiceLogic* CCDSYMessagesServiceLogic::NewL( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aMessageTransactionID, MCDSYServiceLogic* aCDSYServiceLogic )
    {
    COM_TRACE_4( "CDSY - CCDSYMessagesServiceLogic::NewL(0x%x, 0x%x, 0x%x, 0x%x)", aMessageGroup, aMessageID, aMessageTransactionID, aCDSYServiceLogic );

    CCDSYMessagesServiceLogic* messagesServiceLogic = new ( ELeave ) CCDSYMessagesServiceLogic( aMessageGroup, aMessageID, aMessageTransactionID, ETrue, aCDSYServiceLogic );

    COM_TRACE_1( "CDSY - CCDSYMessagesServiceLogic::NewL - return 0x%x", messagesServiceLogic );
    return messagesServiceLogic;
    }
    
