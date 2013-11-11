/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A base class for active handlers
*
*/



// INCLUDE FILES
#include "AccSrvHandlerBase.h"
#include "acc_debug.h"

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
// CAccSrvHandlerBase::CAccSrvHandlerBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvHandlerBase::CAccSrvHandlerBase() : 
    CActive( EPriorityStandard ) //This will exceed accessory server thread priority "EPriorityNormal"
    {
    //Never called since only NewL(MAccSrvConnectionControllerObserver* aCallback) is defined
    TRACE_ASSERT_ALWAYS;
    }

// -----------------------------------------------------------------------------
// CAccSrvHandlerBase::CAccSrvHandlerBase
// -----------------------------------------------------------------------------
//
CAccSrvHandlerBase::CAccSrvHandlerBase( 
    MAccSrvConnectionControllerObserver* aCallback) : 
    CActive( EPriorityStandard ),//This will exceed accessory server thread priority "EPriorityNormal"
    iCallback( aCallback )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::CAccSrvHandlerBase()" );

    CActiveScheduler::Add( this );//Add the active object to the active scheduler

    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::CAccSrvHandlerBase - return" );
    }

// Destructor
CAccSrvHandlerBase::~CAccSrvHandlerBase()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::~CAccSrvHandlerBase()" );

    Cancel();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::~CAccSrvHandlerBase - return" );
    }


// -----------------------------------------------------------------------------
// CAccSrvHandlerBase::IssueRequest
// Starts asynchronous handling.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvHandlerBase::IssueRequest()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::IssueRequest()" );

    Cancel();// Latest request will be served because it contains real time
             // information e.g. in connection status update

    iStatus = KRequestPending;
    SetActive();

    TRequestStatus* status = &iStatus;

    User::RequestComplete(status, KErrNone);//RunL() function will get called

    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::IssueRequest - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvHandlerBase::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvHandlerBase::DoCancel()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::DoCancel()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::DoCancel - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvHandlerBase::RunError
//
// Called if RunL Leaves and this is not overwritten in inherited class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAccSrvHandlerBase::RunError(TInt /*aError*/)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::RunError()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvHandlerBase::RunError - return KErrNone" );
    return KErrNone;
    }

// End of File
