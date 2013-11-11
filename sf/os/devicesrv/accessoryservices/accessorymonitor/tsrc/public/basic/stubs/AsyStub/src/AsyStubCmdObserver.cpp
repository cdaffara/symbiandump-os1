/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ASY stub command observer.
*
*/


// INCLUDE FILES
#include "AsyStubCmdObserver.h"
#include "ASYStubMainServiceBase.h"
#include "AsyStubCmdHandlerBase.h"
#include "siftrace.h"

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::CAsyStubCmdObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAsyStubCmdObserver::CAsyStubCmdObserver( CASYStubCmdHandlerBase* aASYStubCmdHandler ) : 
    CTimer( CActive::EPriorityHigh )
    {
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::CAsyStubCmdObserver()" ) ) );

    CActiveScheduler::Add( this );
    iASYStubCmdHandler = aASYStubCmdHandler;

    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::CAsyStubCmdObserver" ) ) );
    }

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAsyStubCmdObserver::ConstructL()
    {
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::ConstructL()" ) ) );

    CTimer::ConstructL();

    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::ConstructL - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAsyStubCmdObserver* CAsyStubCmdObserver::NewL( CASYStubCmdHandlerBase* aASYStubCmdHandler )
    {
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::NewL()" ) ) );

    CAsyStubCmdObserver* self = new (ELeave) CAsyStubCmdObserver( aASYStubCmdHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();    
   
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::NewL - return 0x%x" ), &self  ) );

    return self;
    }
    
// Destructor
CAsyStubCmdObserver::~CAsyStubCmdObserver()
    {
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::~CAsyStubCmdObserver()" ) ) );

    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::~CAsyStubCmdObserver" ) ) );
    }

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAsyStubCmdObserver::RunL()
    {
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::RunL()" ) ) );
    
    TAccValueTypeTBool aCmdResponse;
    aCmdResponse.iValue	= ETrue;
    iASYStubCmdHandler->ProcessResponseL( aCmdResponse );       				   

    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::RunL - return void" ) ) );
    }
    
// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::StartTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAsyStubCmdObserver::StartTimer()
    {
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::StartTimer()" ) ) );
    
    TInt delay( 1000000 );
    Cancel();// Possiple pending timer is cancelled
    After( TTimeIntervalMicroSeconds32( delay ) );
        
    COMPONENT_TRACE( ( _L("ASYStub - CAsyStubCmdObserver::StartTimer - return void" ) ) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
    
//  End of File
