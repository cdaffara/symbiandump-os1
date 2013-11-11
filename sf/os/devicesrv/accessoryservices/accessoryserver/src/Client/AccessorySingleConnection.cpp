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
* Description:  Client-side handle to a sub-session
*                of Accessory Single Connection
*
*/


// INCLUDE FILES
#include "AccessorySingleConnection.h"
#include "AccPolGenericIDAccessor.h"
#include "AccClientServerMessages.h"
#include "AccessoryServer.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint8 KDummyPointer = 0xff;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::RAccessorySingleConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessorySingleConnection::RAccessorySingleConnection() :
    iNotifyNewAccessoryConnectedGIDPckg(
        *reinterpret_cast<TAccPolGenericID*>( KDummyPointer ) ),
        
    iNotifyValueChangedTBoolValuePckg(
        *reinterpret_cast<TAccValueTypeTBool*>( KDummyPointer ) ),
        
    iNotifyValueChangedTIntValuePckg(
        *reinterpret_cast<TAccValueTypeTInt*>( KDummyPointer ) ),
     
    iFlags( 0 ),
    iFlagsPckg( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved2( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved3( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::RAccessorySingleConnection()" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::CreateSubSession
// Call base class ConstructL() to initialize ptrHolder (resuorses are released
// by Destruct()).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySingleConnection::CreateSubSession(
                                              RAccessoryServer& aSession )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CreateSubSession()" );

    TInt ret = RSubSessionBase::CreateSubSession(
                   aSession,
                   EAccSrvCreateSubSessionAccessorySingleConnection,
                   TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_1( "[AccFW:Client] RAccessorySingleConnection::CreateSubSession - return 0x%x", ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::CloseSubSession
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySingleConnection::CloseSubSession()
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessorySingleConnection );

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::NotifyNewAccessoryConnected
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::NotifyNewAccessoryConnected(
    TRequestStatus& aStatus,
    TAccPolGenericID& aGenericId )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyNewAccessoryConnected()" );

    if( iFlags & KAccSrvNotifyNewAccessoryConnectedBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyNewAccessoryConnected - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGenericIDPckg genericID( aGenericId );
        iNotifyNewAccessoryConnectedGIDPckg.Set( genericID );

        iFlags += KAccSrvNotifyNewAccessoryConnectedBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyNewAccessoryConnected,
                     TIpcArgs( &iNotifyNewAccessoryConnectedGIDPckg,
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyNewAccessoryConnected - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::NotifyAccessoryDisconnected
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::NotifyAccessoryDisconnected(
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryDisconnected()" );

    if( iFlags & KAccSrvNotifyAccessoryDisconnectedBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryDisconnected - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        iNotifyAccessoryDisconnectedGIDPckgBuf = aGenericId;

        iFlags += KAccSrvNotifyAccessoryDisconnectedBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryDisconnected,
                     TIpcArgs( &iNotifyAccessoryDisconnectedGIDPckgBuf,
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryDisconnected - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::NotifyAccessoryValueChanged
// TBool version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::NotifyAccessoryValueChanged(
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId,
    const TAccPolNameRecord& aNameRecord,
    TAccValueTypeTBool& aValue )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged() TBool" );

    if( iFlags & KAccSrvNotifyAccessoryValueChangedTBoolBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGIDNameRecord GIDNameRecord;
        GIDNameRecord.iGenericID  = aGenericId;
        GIDNameRecord.iNameRecord = aNameRecord;
        iNotifyValueChangedTBoolNameRecordPckgBuf = GIDNameRecord;

        TAccSrvValueTypeTBoolPckg value( aValue );
        iNotifyValueChangedTBoolValuePckg.Set( value );

        iFlags += KAccSrvNotifyAccessoryValueChangedTBoolBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryValueChangedTBool,
                     TIpcArgs( &iNotifyValueChangedTBoolNameRecordPckgBuf,
                               &iNotifyValueChangedTBoolValuePckg,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged TBool - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::NotifyAccessoryValueChanged
// TInt version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::NotifyAccessoryValueChanged(
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId,
    const TAccPolNameRecord& aNameRecord,
    TAccValueTypeTInt& aValue )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged() TInt" );

    if( iFlags & KAccSrvNotifyAccessoryValueChangedTIntBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGIDNameRecord GIDNameRecord;
        GIDNameRecord.iGenericID  = aGenericId;
        GIDNameRecord.iNameRecord = aNameRecord;
        iNotifyValueChangedTIntNameRecordPckgBuf = GIDNameRecord;

        TAccSrvValueTypeTIntPckg value( aValue );
        iNotifyValueChangedTIntValuePckg.Set( value );

        iFlags += KAccSrvNotifyAccessoryValueChangedTIntBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryValueChangedTInt,
                     TIpcArgs( &iNotifyValueChangedTIntNameRecordPckgBuf,
                               &iNotifyValueChangedTIntValuePckg,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged TInt - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::NotifyAccessoryValueChangedL
// Object version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::NotifyAccessoryValueChanged(
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId,
    const TAccPolNameRecord& aNameRecord,
    TAccValueTypeObject& aValue )
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged() Object" );

    if( iFlags & KAccSrvNotifyAccessoryValueChangedObjectBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged - Request already exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGIDNameRecord GIDNameRecord;
        GIDNameRecord.iGenericID  = aGenericId;
        GIDNameRecord.iNameRecord = aNameRecord;
        iNotifyValueChangedObjectNameRecordPckgBuf = GIDNameRecord;

        iFlags += KAccSrvNotifyAccessoryValueChangedObjectBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryValueChangedObject,
                     TIpcArgs( &iNotifyValueChangedObjectNameRecordPckgBuf,
                               &aValue.iValue,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::NotifyAccessoryValueChanged Object - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::CancelNotifyNewAccessoryConnected
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::CancelNotifyNewAccessoryConnected() const
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CancelNotifyNewAccessoryConnected()" );

    SendReceive( EAccSrvCancelNotifyNewAccessoryConnected,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CancelNotifyNewAccessoryConnected - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::CancelNotifyAccessoryDisconnected
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::CancelNotifyAccessoryDisconnected() const
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CancelNotifyAccessoryDisconnected()" );

    SendReceive( EAccSrvCancelNotifyAccessoryDisconnected,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CancelNotifyAccessoryDisconnected - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessorySingleConnection::CancelNotifyAccessoryValueChanged
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessorySingleConnection::CancelNotifyAccessoryValueChanged(
    const TAccPolNameRecord& aNameRecord ) const
    {
    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CancelNotifyAccessoryValueChanged()" );

    TAccSrvNameRecordPckg nameRecordPckg( aNameRecord );

    SendReceive( EAccSrvCancelNotifyAccessoryValueChanged,
                 TIpcArgs( &nameRecordPckg ) );

    API_TRACE_( "[AccFW:Client] RAccessorySingleConnection::CancelNotifyAccessoryValueChanged - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
