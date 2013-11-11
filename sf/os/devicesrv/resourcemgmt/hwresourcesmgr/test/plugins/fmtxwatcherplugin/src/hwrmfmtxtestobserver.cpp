// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include "hwrmfmtxtestobserver.h"
#include "trace.h"

const TInt KTestCategory = 0x101FB3E7;
const TInt KTestKeyNotifyChange = 0x1;
_LIT_SECURITY_POLICY_PASS(KAlwaysPassPolicy);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::NewL()
// -----------------------------------------------------------------------------
//
CHWRMFmtxTestObserver* CHWRMFmtxTestObserver::NewL(
    MHWRMFmtxConnObserverCallback& aObserver )
    {
    FUNC_LOG;
    
    CHWRMFmtxTestObserver* self = new( ELeave ) CHWRMFmtxTestObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::~CHWRMFmtxTestObserver()
// -----------------------------------------------------------------------------
//
CHWRMFmtxTestObserver::~CHWRMFmtxTestObserver()
    {
    FUNC_LOG;
    
    Cancel();
    iTestPubSubKey.Close();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::GetStatusL
// -----------------------------------------------------------------------------
//
TBool CHWRMFmtxTestObserver::GetStatusL()
    {
    FUNC_LOG;
    
    TInt value( 0 );
    iTestPubSubKey.Get( value ); // Ignore read error
    return value > 0;
    }
    

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::StartObservingL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxTestObserver::StartObservingL()
    {
    FUNC_LOG;
    
    if( !IsActive() )
        {
        OrderNotification();
        
        // check the initial status, so that only state change is notified
        iConnected = GetStatusL();
        }
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::StopObserving
// -----------------------------------------------------------------------------
//
void CHWRMFmtxTestObserver::StopObserving()
    {
    FUNC_LOG;
    
    Cancel();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::CHWRMFmtxTestObserver()
// -----------------------------------------------------------------------------
//
CHWRMFmtxTestObserver::CHWRMFmtxTestObserver( MHWRMFmtxConnObserverCallback& aObserver ) :
    CActive( EPriorityStandard ),
    iCallback( aObserver )
    {
    FUNC_LOG;
    
    CActiveScheduler::Add( this );
    }
            
// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxTestObserver::ConstructL()
    {
    FUNC_LOG;
    
    User::LeaveIfError( RProperty::Define(
        TUid::Uid( KTestCategory ), KTestKeyNotifyChange, RProperty::EInt,
        KAlwaysPassPolicy, KAlwaysPassPolicy ) );
    
    User::LeaveIfError( iTestPubSubKey.Attach( TUid::Uid( KTestCategory ),
        KTestKeyNotifyChange ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::OrderNotification()
// -----------------------------------------------------------------------------
//
void CHWRMFmtxTestObserver::OrderNotification()
    {
    FUNC_LOG;
    
    iTestPubSubKey.Subscribe( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::RunL()
// -----------------------------------------------------------------------------
//
void CHWRMFmtxTestObserver::RunL( )
    {
    FUNC_LOG;
    
    TInt status( iStatus.Int() );
    
    LOG_IF_ERROR1( status, "CHWRMFmtxTestObserver::RunL error, status=%d", status );

    if ( status == KErrNone )
        {
        OrderNotification();
        TBool connected( GetStatusL() );
        INFO_LOG2( "CHWRMFmtxTestObserver::RunL: connected=%d, iConnected=%d", connected, iConnected );
        if ( connected != iConnected ) // notify only if state has changed
            {
            iConnected = connected;
            iCallback.HandleConnectionChange( EFmtxWatcherObserverTest, iConnected );
            }    
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxTestObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CHWRMFmtxTestObserver::DoCancel()
    {
    FUNC_LOG;

    iTestPubSubKey.Cancel();
    }

//  End of File  
