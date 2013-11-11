/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of "Accessory unsupported note"
*
*/


// INCLUDE FILES
#include <stringresourcereader.h>
#include <featmgr.h>
//#include <accfwnoteuinotifier.rsg>
#include <akncapserverclient.h>
#include <aknsgcc.h>
#include <aknnotifystd.h>
#include <data_caging_path_literals.hrh> 
#include <aknnotewrappers.h>
#include <eikdialg.h>
#include <f32file.h>
#include "SecondaryDisplayAccFwAPI.h"

#include "AccFwUiNoteNotifierStub.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUid KScreenOutputChannel = {0x00000666};
                      
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
   
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::CAccFwUiNoteNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccFwUiNoteNotifierStub::CAccFwUiNoteNotifierStub()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::CAccFwUiNoteNotifier()" );
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::CAccFwUiNoteNotifier() - return void" );
    }
   
// Destructor
CAccFwUiNoteNotifierStub::~CAccFwUiNoteNotifierStub()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::~CAccFwUiNoteNotifier()" );
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccPolAccessoryPolicy::~CAccFwUiNoteNotifier() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::ConstructL() 
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccFwUiNoteNotifierStub::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::ConstructL()" );
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::ConstructL() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::NewL() 
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccFwUiNoteNotifierStub* CAccFwUiNoteNotifierStub::NewL()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::NewL()" );

    CAccFwUiNoteNotifierStub* self = new( ELeave ) CAccFwUiNoteNotifierStub;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::CAccFwUiNoteNotifier::NewL() - return" );
    return self;
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::Release()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccFwUiNoteNotifierStub::Release()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::Release() - enter" );
    delete this;
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::Release() - return" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::RegisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAccFwUiNoteNotifierStub::TNotifierInfo CAccFwUiNoteNotifierStub::RegisterL()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::RegisterL() - enter" );
    iInfo.iUid=KAccFwUiNoteNotifierUid;
    iInfo.iChannel=KScreenOutputChannel;
    iInfo.iPriority=ENotifierPriorityHigh;
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::RegisterL() - return" );
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::Info()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAccFwUiNoteNotifierStub::TNotifierInfo CAccFwUiNoteNotifierStub::Info() const
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::Info() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::Info() - return" );
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::StartL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC8 CAccFwUiNoteNotifierStub::StartL(
    const TDesC8& /*aBuffer*/ )
    {

    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::StartL() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::StartL() - return" );
    
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::StartL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccFwUiNoteNotifierStub::StartL(
    const TDesC8& /*aBuffer*/, 
    TInt /*aReplySlot*/, 
    const RMessagePtr2& aMessage)
    {    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::StartL() - enter" );
    
    aMessage.Complete( KErrNone );
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::StartL() - return" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::Cancel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccFwUiNoteNotifierStub::Cancel()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::Cancel() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::Cancel() - return" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiNoteNotifier::UpdateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC8 CAccFwUiNoteNotifierStub::UpdateL( const TDesC8& /*aBuffer*/ )
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::UpdateL() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiNoteNotifier::UpdateL() - return" );
    return KNullDesC8();
    }

//  End of File
