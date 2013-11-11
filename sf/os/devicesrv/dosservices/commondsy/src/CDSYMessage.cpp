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
* Description:  Implementation of CDSYMessage class
*
*/




// INCLUDE FILES 
#include "CDSYMessage.h"
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
// TCDSYMessage::TCDSYMessage
// -----------------------------------------------------------------------------
EXPORT_C TCDSYMessage::TCDSYMessage( const TUint32 aGroup, const TUint32 aID, const TDesC8* aData, const TUint32 aTransactionID )
    : iGroup( aGroup ),
    iID( aID ),
    iData( aData ),
    iTransactionID( aTransactionID ),
    iReturnData( NULL )
    {
    COM_TRACE_4( "CDSY - TCDSYMessage::TCDSYMessage(0x%x, 0x%x, 0x%x, 0x%x)", aGroup, aID, aData, aTransactionID );

    COM_TRACE_( "CDSY - TCDSYMessage::TCDSYMessage - return void" );
    }



// -----------------------------------------------------------------------------
// TCDSYMessage::TCDSYMessage
// -----------------------------------------------------------------------------
EXPORT_C TCDSYMessage::TCDSYMessage( const TUint32 aGroup, const TUint32 aID, const TDesC8* aData )
    : iGroup( aGroup ),
    iID( aID ), 
    iData( aData ),
    iTransactionID( 0 ),
    iReturnData( NULL )
    {
    COM_TRACE_3( "CDSY - TCDSYMessage::TCDSYMessage(0x%x, 0x%x, 0x%x)", aGroup, aID, aData );

    COM_TRACE_( "CDSY - TCDSYMessage::TCDSYMessage - return void" );
    }



// -----------------------------------------------------------------------------
// TCDSYMessage::~TCDSYMessage
// -----------------------------------------------------------------------------
EXPORT_C TCDSYMessage::~TCDSYMessage()
    {
    COM_TRACE_( "CDSY - TCDSYMessage::~TCDSYMessage()" );

    if ( iReturnData != NULL )
        {
        delete iReturnData;
        iReturnData = NULL;
        }

    iData = NULL;

    COM_TRACE_( "CDSY - TCDSYMessage::~TCDSYMessage - return void" );
    }



// -----------------------------------------------------------------------------
// TCDSYMessage::SetReturnDataL
// -----------------------------------------------------------------------------
EXPORT_C void TCDSYMessage::SetReturnDataL( const TDesC8* aReturnData )
    {
    COM_TRACE_1( "CDSY - TCDSYMessage::SetReturnDataL(0x%x)", aReturnData );

    TRACE_ASSERT( iReturnData == NULL );
    TRACE_ASSERT( aReturnData != NULL );

    iReturnData = HBufC8::NewL( aReturnData->Length() );
    *iReturnData = *aReturnData;
    
    COM_TRACE_( "CDSY - TCDSYMessage::SetReturnDataL - return void" );
    }
    
