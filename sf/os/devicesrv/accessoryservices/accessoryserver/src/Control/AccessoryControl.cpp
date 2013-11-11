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
* Description: 
*
*/


// INCLUDE FILES
#include "AccessoryControl.h"
#include "AccClientServerMessages.h"
#include "AccessoryServer.h"
#include "AccPolAccessoryPolicy.h"
#include "AccPolGenericIDAccessor.h"
#include "acc_debug.h"
#include <AccessoryTypes.h>
#include <AccConGenericID.h>
#include <AccPolNameValueRecord.h>
#include <s32mem.h>
#include <accpolobjectcon.h>

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
// RAccessoryControl::RAccessoryControl
// The default empty constructor, present only to support
// virtual function table export.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryControl::RAccessoryControl() :
    iConnectGIDPckg( *reinterpret_cast<TAccPolGenericID*>( KDummyPointer ) ),
    iFlags( 0 ),
    iFlagsPckg( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved1( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved2( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:Control] RAccessoryControl::RAccessoryControl()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::CreateSubSession
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryControl::CreateSubSession(RAccessoryServer& aSession)
    {
    API_TRACE_( "[AccFW:Control] RAccessoryControl::CreateSubSession()" );

    TInt ret = RSubSessionBase::CreateSubSession( aSession,
                                                  EAccSrvCreateSubSessionAccessoryControl,
                                                  TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_1( "[AccFW:Control] RAccessoryControl::CreateSubSession - return 0x%x", ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::CloseSubSession
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryControl::CloseSubSession()
    {
    API_TRACE_( "[AccFW:Control] RAccessoryControl::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessoryControl );

    API_TRACE_( "[AccFW:Control] RAccessoryControl::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::ConnectAccessory
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::ConnectAccessory( TRequestStatus& aStatus,
                                                   TAccPolGenericID& /*aGenericId*/ )
    {
    API_TRACE_( "[AccFW:Control] RAccessoryControl::ConnectAccessory()" );
  
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    
    API_TRACE_( "[AccFW:Control] RAccessoryControl::ConnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::ConnectAccessory (with detailed accessory information)
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::ConnectAccessory(
    TRequestStatus& aStatus,
    CAccConGenericID* const aGenericID,
    TBool aEvaluateConnectionRules )
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::ConnectAccessory(UniqueID=%d)", TAccPolGenericIDAccessor::GenericIDRef( aGenericID ).UniqueID() );

    if( iFlags & KAccSrvConnectAccessoryBit )
        {
        API_TRACE_( "[AccFW:Control] RAccessoryControl::ConnectAccessory - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvConnectRecord connectRecord;
        RThread              thisThread;        
    
        TAccSrvGenericIDPckg genericIDPckg( 
                TAccPolGenericIDAccessor::GenericIDRef( aGenericID ) );
        iConnectGIDPckg.Set( genericIDPckg ); 
    
        connectRecord.iThreadID = thisThread.Id(); //Get the thread ID of the connector (e.g. thread ID of ASY)
        connectRecord.iNameValueBufSize = 
                TAccPolGenericIDAccessor::NameValueBuf(aGenericID )->Size();
        connectRecord.iEvaluateConnectionRules = aEvaluateConnectionRules; 
        connectRecord.iCapsPtr.Set( 
                TAccPolGenericIDAccessor::NameValueBuf(aGenericID )->Ptr( 0 ) );
        iConnectRecordPckgBuf = connectRecord;
    
        iFlags += KAccSrvConnectAccessoryBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );
        aStatus = KRequestPending;
    
        SendReceive( EAccSrvConnectAccessory2,
                     TIpcArgs( &iConnectGIDPckg, &iConnectRecordPckgBuf, &iFlagsPckg ),
                     aStatus );
        }            
        
    API_TRACE_( "[AccFW:Control] RAccessoryControl::ConnectAccessory - with details - return void" );
    }
   
// -----------------------------------------------------------------------------
// RAccessoryControl::DisconnectAccessory
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::DisconnectAccessory( 
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId )
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::DisconnectAccessory(UniqueID=%d)", aGenericId.UniqueID() );

    if( iFlags & KAccSrvDisconnectAccessoryBit )
        {
        API_TRACE_( "[AccFW:Control] RAccessoryControl::DisconnectAccessory - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        iDisconnectGIDPckgBuf = aGenericId;

        iFlags += KAccSrvDisconnectAccessoryBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvDisconnectAccessory,
                     TIpcArgs( &iDisconnectGIDPckgBuf, 
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Control] RAccessoryControl::DisconnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::AccessoryValueChangedNotifyL
//
// TBool version.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::AccessoryValueChangedNotifyL( 
    const TAccPolGenericID& aGenericId,
    const TAccPolNameRecord& aNameRecord,
    const TAccValueTypeTBool aValue ) const
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::AccessoryValueChangedNotifyL(UniqueID=%d) TBool", aGenericId.UniqueID() );

    TAccSrvGenericIDPckgC        gid( aGenericId );
    TAccSrvNameRecordPckgC       nameRecord( aNameRecord );
    TAccSrvValueTypeTBoolPckgBuf value( aValue );

    User::LeaveIfError( SendReceive( EAccSrvAccessoryValueChangedTBoolNotify,
                                     TIpcArgs( &gid, &nameRecord, &value ) ) );

    API_TRACE_( "[AccFW:Control] RAccessoryControl::AccessoryValueChangedNotifyL TBool - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::AccessoryValueChangedNotifyL
//
// TInt version.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::AccessoryValueChangedNotifyL( 
    const TAccPolGenericID& aGenericId,
    const TAccPolNameRecord& aNameRecord,
    const TAccValueTypeTInt aValue ) const
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::AccessoryValueChangedNotifyL(UniqueID=%d) TInt", aGenericId.UniqueID() );

    TAccSrvGenericIDPckgC        gid( aGenericId );
    TAccSrvNameRecordPckgC       nameRecord( aNameRecord );
    TAccSrvValueTypeTIntPckgBuf  value( aValue );

    User::LeaveIfError( SendReceive( EAccSrvAccessoryValueChangedTIntNotify,
                                     TIpcArgs( &gid, &nameRecord, &value ) ) );

    API_TRACE_( "[AccFW:Control] RAccessoryControl::AccessoryValueChangedNotifyL TInt - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::AccessoryValueChangedNotifyL
//
// Object version.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::AccessoryValueChangedNotifyL( 
    const TAccPolGenericID& aGenericId,
    const TAccPolNameRecord& aNameRecord,
    const CAccPolObjectCon& aValue ) const
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::AccessoryValueChangedNotifyL(UniqueID=%d) TInt", aGenericId.UniqueID() );

    TAccSrvGenericIDPckgC gid( aGenericId );
    TAccSrvNameRecordPckgC nameRecord( aNameRecord );
    
    // Create a buffer for aValue
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );    
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    // Externalize aValue to buf
    RBufWriteStream wStrm;    
    wStrm.Open( *buf );
    CleanupClosePushL( wStrm );    
    aValue.ExternalizeL( wStrm );
    wStrm.CommitL();         
    CleanupStack::PopAndDestroy( &wStrm );
    
    // Create args
    TPtr8 bufPtr( buf->Ptr( 0 ) );
    TIpcArgs args( &gid, &nameRecord, &bufPtr );
    
    // Send the request to the server side
    User::LeaveIfError(
        SendReceive( EAccSrvAccessoryValueChangedObjectNotify, args ) );
    
    // Cleanup
    CleanupStack::PopAndDestroy( buf );

    API_TRACE_( "[AccFW:Control] RAccessoryControl::AccessoryValueChangedNotifyL TInt - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::SetValueL
//
// TBool version.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryControl::SetValueL( const TAccPolGenericID& aGenericId,
                                            const TAccPolNameRecord& aNameRecord,
                                            const TAccValueTypeTBool aValue )
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::SetValueL(UniqueID=%d) TBool", aGenericId.UniqueID() );

    TInt                         err( KErrNone );
    TAccSrvGenericIDPckgC        gidPckgC( aGenericId );
    TAccSrvNameRecordPckgC       nameRecordPckgC( aNameRecord );
    TAccSrvValueTypeTBoolPckgBuf valuePckgBuf( aValue );

    err = SendReceive( EAccSrvSetValueTBool,
                       TIpcArgs( &gidPckgC, &nameRecordPckgC, &valuePckgBuf ) );

    if( KErrNone == err || KErrTimedOut == err || KErrServerBusy == err )
        {
        //These return values are specified
        }
    else
        {
        //These return values are not specified.
        //Thus leave occurs.
        User::Leave( err );
        }

    API_TRACE_1( "[AccFW:Control] RAccessoryControl::SetValueL TBool - return 0x%x", err );

    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::SetValueL
//
// TInt version.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryControl::SetValueL( const TAccPolGenericID& aGenericId,
                                            const TAccPolNameRecord& aNameRecord,
                                            const TAccValueTypeTInt aValue )
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::SetValueL(UniqueID=%d) TInt", aGenericId.UniqueID() );

    TInt err(KErrNone);
    TAccSrvGenericIDPckgC        gidPckgC( aGenericId );
    TAccSrvValueTypeTIntPckgBuf  valuePckgBuf( aValue );
    TAccSrvNameRecordPckgC       nameRecordPckgC( aNameRecord );

    err = SendReceive( EAccSrvSetValueTInt,
                       TIpcArgs( &gidPckgC, &nameRecordPckgC, &valuePckgBuf ) );

    if( KErrNone == err || KErrTimedOut == err || KErrServerBusy == err )
        {
        //These return values are specified
        }
    else
        {
        //These return values are not specified.
        //Thus leave occurs.
        User::Leave( err );
        }

    API_TRACE_1( "[AccFW:Control] RAccessoryControl::SetValueL TInt - return %d", err );

    return err;
    }
    
// -----------------------------------------------------------------------------
// RAccessoryControl::SetValueL
//
// CAccPolObjectBase version.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryControl::SetValueL( const TAccPolGenericID& aGenericId,
                                            const TAccPolNameRecord& aNameRecord,
                                            const CAccPolObjectCon& aValue )
    {
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::SetValueL(UniqueID=%d) CAccPolObjectBase", aGenericId.UniqueID() );
    
    TInt                         err(KErrNone);
    TAccSrvGenericIDPckgC        gidPckgC( aGenericId );
    TAccSrvNameRecordPckgC       nameRecordPckgC( aNameRecord );
    
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );    
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    //Externalize aValue to buf
    RBufWriteStream wStrm;    
    (void) wStrm.Open( *buf );
    CleanupClosePushL( wStrm );    
    aValue.ExternalizeL( wStrm );
    wStrm.CommitL();         
    CleanupStack::PopAndDestroy( &wStrm );

    TPtr8 ptr( buf->Ptr( 0 ) );
    
    err = SendReceive( EAccSrvSetValueObject,
                       TIpcArgs( &gidPckgC, &nameRecordPckgC, &ptr ) );

    if( KErrNone == err || KErrTimedOut == err || KErrServerBusy == err )
        {
        //These return values are specified
        }
    else
        {
        //These return values are not specified.
        //Thus leave occurs.
        User::Leave( err );
        }
        
    CleanupStack::PopAndDestroy( buf );
    
    API_TRACE_1( "[AccFW:Control] RAccessoryControl::SetValueL CAccPolObjectBase - return %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::CancelConnectAccessory
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::CancelConnectAccessory() const
    {
    API_TRACE_( "[AccFW:Control] RAccessoryControl::CancelConnectAccessory()" );

    SendReceive( EAccSrvCancelConnectAccessory,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Control] RAccessoryControl::CancelConnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::CancelDisconnectAccessory
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryControl::CancelDisconnectAccessory() const
    {
    API_TRACE_( "[AccFW:Control] RAccessoryControl::CancelDisconnectAccessory()" );

    SendReceive( EAccSrvCancelDisconnectAccessory,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Control] RAccessoryControl::CancelDisconnectAccessory - return void" );
    }

//  End of File
