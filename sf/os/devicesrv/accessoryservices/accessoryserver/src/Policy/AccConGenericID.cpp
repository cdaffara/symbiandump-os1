/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Public, heap-allocated Generic ID
*
*/


// INCLUDE FILES
#include <AccConGenericID.h>
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KAccSrvCapabilityBufInitialSize = 50;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccConGenericID::CAccConGenericID
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccConGenericID::CAccConGenericID()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::CAccConGenericID()" );
    }

// -----------------------------------------------------------------------------
// CAccConGenericID::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccConGenericID::ConstructL()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::ConstructL()" );
    
    iNameValueBuf = CBufFlat::NewL( KAccSrvCapabilityBufInitialSize );
    
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccConGenericID::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccConGenericID* CAccConGenericID::NewL()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::NewL()" );
    
    CAccConGenericID* self = new( ELeave ) CAccConGenericID();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::NewL - return self" );

    return self;
    }

    
// Destructor
EXPORT_C CAccConGenericID::~CAccConGenericID()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::~CAccConGenericID()" );
    
    if( iNameValueBuf )
        {
        delete iNameValueBuf;
        }
        
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::~CAccConGenericID - return" );
    }

// -----------------------------------------------------------------------------
// CAccConGenericID::GenericID()
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolGenericID CAccConGenericID::GenericID() const
    {    
    API_TRACE_( "[AccFW:ACCPOLICY] CAccConGenericID::GenericID()" );
    return iGenericID;    
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
