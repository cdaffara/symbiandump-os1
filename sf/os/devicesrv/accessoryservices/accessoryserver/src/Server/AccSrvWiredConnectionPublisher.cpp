/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CAccSrvWiredConnectionPublihser implementation.
*
*/


#include <accessoryservicesinternalpskeys.h>
#include "AccSrvWiredConnectionPublisher.h"
#include "acc_debug.h"

_LIT_SECURITY_POLICY_C1( KWiredConnectionReadPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_S0( KWiredConnectionWritePolicy, KPSUidAccessoryServices.iUid );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::NewL
// ---------------------------------------------------------------------------
//
CAccSrvWiredConnectionPublisher* CAccSrvWiredConnectionPublisher::NewL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::NewL" );
    CAccSrvWiredConnectionPublisher* self = CAccSrvWiredConnectionPublisher::NewLC();
    CleanupStack::Pop( self );
    COM_TRACE_( "[AccFW:AccServer] CCAccSrvWiredConnectionPublisher::NewL - return self" );
    return self;
    }


// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::NewLC
// ---------------------------------------------------------------------------
//
CAccSrvWiredConnectionPublisher* CAccSrvWiredConnectionPublisher::NewLC()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::NewLC" );
    CAccSrvWiredConnectionPublisher* self = new( ELeave ) CAccSrvWiredConnectionPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    COM_TRACE_( "[AccFW:AccServer] CCAccSrvWiredConnectionPublisher::NewLC - return self" );
    return self;
    }


// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::~CAccSrvWiredConnectionPublisher
// ---------------------------------------------------------------------------
//
CAccSrvWiredConnectionPublisher::~CAccSrvWiredConnectionPublisher()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::~CAccSrvWiredConnectionPublisher" );
    
    iProperty.Close();
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::~CAccSrvWiredConnectionPublisher - end" );
    }

// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::HandleConnectL
// ---------------------------------------------------------------------------
//
void CAccSrvWiredConnectionPublisher::HandleConnectL( const TAccPolGenericID& aGenericID )
    {
    TUint32 physConnCaps( aGenericID.PhysicalConnectionCaps() );
    
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::HandleConnectL: physConnCaps=0x%x, iConnectionCount=%d", 
                        physConnCaps, iConnectionCount );
    
    if ( physConnCaps & (KPCWired | KPCHDMI) )
        {
        if ( !iConnectionCount++ )
            {
            // first detected wired connection, publish it 
            COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::HandleConnectL: WIRED CONNECTED" );
            
            User::LeaveIfError( iProperty.Set( EAccSrvcsWiredConnConnected ) );
            }
        }
    COM_TRACE_( "[AccFW:AccServer] CCAccSrvWiredConnectionPublisher::HandleConnectL - return void" );
    }

// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::HandleDisconnectL
// ---------------------------------------------------------------------------
//
void CAccSrvWiredConnectionPublisher::HandleDisconnectL( const TAccPolGenericID& aGenericID )
    {
    TUint32 physConnCaps( aGenericID.PhysicalConnectionCaps() );
    
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::HandleDisconnectL: physConnCaps=0x%x, iConnectionCount=%d", 
                        physConnCaps, iConnectionCount );
    
    if ( (physConnCaps & (KPCWired | KPCHDMI)) && iConnectionCount )
        {
        if ( !(--iConnectionCount) )
            {
            // last wired connection disconnected
            COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::HandleDisconnectL: WIRED DISCONNECTED" );
            
            User::LeaveIfError( iProperty.Set( EAccSrvcsWiredConnDisconnected ) );
            }
        }
    COM_TRACE_( "[AccFW:AccServer] CCAccSrvWiredConnectionPublisher::HandleDisconnectL - return void" );
    }

// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::CAccSrvWiredConnectionPublisher
// ---------------------------------------------------------------------------
//
CAccSrvWiredConnectionPublisher::CAccSrvWiredConnectionPublisher() :
    iConnectionCount( 0 )
    {
    }
    
// ---------------------------------------------------------------------------
// CAccSrvWiredConnectionPublisher::ConstructL
// ---------------------------------------------------------------------------
//
void CAccSrvWiredConnectionPublisher::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvWiredConnectionPublisher::ConstructL" );
    User::LeaveIfError( RProperty::Define( KPSUidAccessoryServices, 
                                           KAccSrvcsWiredConnection,
                                           RProperty::EInt,
                                           KWiredConnectionReadPolicy,
                                           KWiredConnectionWritePolicy ) );
    
    User::LeaveIfError( iProperty.Attach( KPSUidAccessoryServices, 
                                          KAccSrvcsWiredConnection ) );
    
    User::LeaveIfError( iProperty.Set( EAccSrvcsWiredConnUninitialized ) );
    
    COM_TRACE_( "[AccFW:AccServer] CCAccSrvWiredConnectionPublisher::ConstructL - return void" );
    }

