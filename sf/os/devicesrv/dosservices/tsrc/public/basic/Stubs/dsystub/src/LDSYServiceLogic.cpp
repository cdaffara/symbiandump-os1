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
* Description:  Implementation of TLDSYServiceLogic class
*
*/




// INCLUDE FILES 
#include <CDSYMessage.h>
#include <CDSYMessageIDs.h>
#include "LDSYServiceLogic.h"
#include "LDSYTrace.h"



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
// TLDSYServiceLogic::TLDSYServiceLogic
// -----------------------------------------------------------------------------
TLDSYServiceLogic::TLDSYServiceLogic( CCDSYController& aCDSYController )
    : iCDSYController( aCDSYController )
    {
    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::TLDSYServiceLogic(0x%x)" ), &aCDSYController ) );

    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::TLDSYServiceLogic - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// TLDSYServiceLogic::~TLDSYServiceLogic
// -----------------------------------------------------------------------------
TLDSYServiceLogic::~TLDSYServiceLogic()
    {
    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::~TLDSYServiceLogic()" ) ) );

    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::~CLDSYServiceLogic - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// TLDSYServiceLogic::ProcessMessageL
// -----------------------------------------------------------------------------
void TLDSYServiceLogic::ProcessMessageL( TCDSYMessage& aCDSYMessage )
    {
    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::ProcessMessageL(0x%x)" ), &aCDSYMessage ) );

    switch ( aCDSYMessage.Group() )
        {
        case KCDSYMsgGroup:

            ProcessCDSYMessageL( aCDSYMessage );

        break;

        default:

            TRACE_ASSERT_ALWAYS;

        break;
        }

    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::ProcessMessageL - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// TLDSYServiceLogic::ProcessCDSYMessageL
// -----------------------------------------------------------------------------
void TLDSYServiceLogic::ProcessCDSYMessageL( TCDSYMessage& /*aCDSYMessage*/ )
    {
    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::ProcessCDSYMessageL()" ) ) );

    TRACE_ASSERT_ALWAYS;

    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic::ProcessCDSYMessageL - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// TLDSYServiceLogic::ExpireMessageL
// -----------------------------------------------------------------------------
void TLDSYServiceLogic::ExpireMessageL( TCDSYMessage& /*aCDSYMessage*/ )
    {
    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic:ExpireMessageL()" ) ) );

    TRACE_ASSERT_ALWAYS;

    COMPONENT_TRACE( ( _L( "LDSY - TLDSYServiceLogic:ExpireMessageL - return void" ) ) );
    }

