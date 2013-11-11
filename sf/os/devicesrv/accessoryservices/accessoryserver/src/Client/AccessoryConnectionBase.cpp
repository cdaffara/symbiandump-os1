/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client-side handle to a sub-session of Accessory Connection Base
*
*/


// INCLUDE FILES
#include "AccPolAccessoryPolicy.h"
#include "AccPolCapsSubblockNameArraySerial.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include <AccessoryConnectionBase.h>
#include <s32mem.h>
#include <accpolobjectcon.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
// The max buffer size for streaming capabilities, used in
// RAccessoryConnectionBase::GetSubblockNameArrayL()
// Buffer contains 2 bytes for count and 2 bytes for each capability.
// e.g. for 6 capabilities the required size is 2+2*6=14 bytes.
// This buffer size should be increased if its not enough for most cases,
// 2nd IPC call is made if initial buffer size was too small.
const TInt KAccSrvCapabilityStreamBufMaxSize = 64;
const TInt KAccSrvCapabilityStreamBufGranularity = 64;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RAccessoryConnectionBase::RAccessoryConnectionBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryConnectionBase::RAccessoryConnectionBase()
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::RAccessoryConnectionBase()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryConnectionBase::GetSubblockNameArrayL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryConnectionBase::GetSubblockNameArrayL(
    const TAccPolGenericID& aGenericID,
    CAccPolSubblockNameArray& aNameArray ) const
    {
    API_TRACE_1( "[AccFW:Client] RAccessoryConnectionBase::GetSubblockNameArrayL(%d)", aGenericID.UniqueID() );

    TInt                   err( KErrNone );
    RBufReadStream         rStrm;    
    TAccSrvGenericIDPckgC  genericIDPckgC( aGenericID );

    //Create storage buffer for data
    CBufFlat* buf = CBufFlat::NewL( KAccSrvCapabilityStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvCapabilityStreamBufMaxSize );

    TPtr8 ptr( buf->Ptr(0) );

    err = SendReceive( EAccSrvGetSubblockNameArray,
                       TIpcArgs( &genericIDPckgC, 
                                 KAccSrvCapabilityStreamBufMaxSize,
                                 &ptr ) );

    if ( KErrNone == err )
        {
        // Internalize to the local variable
        (void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
                                   // RBufReadStream::Open does not return any values 
        CleanupClosePushL( rStrm );
        TAccPolCapsSubblockNameArraySerial::InternalizeL( rStrm, aNameArray );
        CleanupStack::PopAndDestroy( &rStrm );
        }
    else
        {
        if ( KErrEof == err)
            {
            //buf size was not enough.
            //Read the new buffer size and make a 2nd attempt.
            API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::GetSubblockNameArrayL - 2nd attempt" );
            (void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
                                       // RBufReadStream::Open does not return any values 
            CleanupClosePushL( rStrm );
            TInt newBufferSize( rStrm.ReadInt32L() );
            CleanupStack::PopAndDestroy( &rStrm );

            buf->Reset();
            buf->ResizeL( newBufferSize );
            TPtr8 ptr( buf->Ptr(0) );

            err = SendReceive( EAccSrvGetSubblockNameArray,
                               TIpcArgs( &genericIDPckgC, 
                                         newBufferSize,
                                         &ptr ) );

            if ( KErrNone == err )
                {
                // Internalize to the local variable
                (void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
                                           // RBufReadStream::Open does not return any values 
                CleanupClosePushL( rStrm );
                TAccPolCapsSubblockNameArraySerial::InternalizeL( rStrm, aNameArray );
                CleanupStack::PopAndDestroy( &rStrm );
                }
            else
                {
                User::Leave( err );
                }
            }
        else
            {
            User::Leave( err );
            }
        }

    CleanupStack::PopAndDestroy( buf );

    API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::GetSubblockNameArrayL - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryConnectionBase::GetValueL
// TBool version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnectionBase::GetValueL(
                                 const TAccPolGenericID& aGenericID,
                                 const TAccPolNameRecord& aNameRecord,
                                 TAccValueTypeTBool& aValue ) const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::GetValueL() TBool" );

    TInt                      err(KErrNone);
    TAccSrvGenericIDPckgC     gidPckgC(aGenericID);
    TAccSrvNameRecordPckgC    nameRecordPckgC(aNameRecord);
    TAccSrvValueTypeTBoolPckg valuePckg(aValue);

    err = SendReceive( EAccSrvGetValueTBool,
                       TIpcArgs( &gidPckgC, &nameRecordPckgC, &valuePckg ) );

    if ( KErrNone == err || KErrTimedOut == err /*|| KErrServerBusy == err*/ )
        {
        //These return values are specified
        }
    else
        {
        //These return values are not specified.
        //Thus leave occurs.
        User::Leave( err );
        }

    API_TRACE_1( "[AccFW:Client] RAccessoryConnectionBase::GetValueL TBool - return %d",err );
    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryConnectionBase::GetValueL
// TInt version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnectionBase::GetValueL(
                                const TAccPolGenericID& aGenericID,
                                const TAccPolNameRecord& aNameRecord,
                                TAccValueTypeTInt& aValue ) const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::GetValueL() TInt" );

    TInt                     err(KErrNone);
    TAccSrvGenericIDPckgC    gidPckgC(aGenericID);
    TAccSrvNameRecordPckgC   nameRecordPckgC(aNameRecord);
    TAccSrvValueTypeTIntPckg valuePckg(aValue);

    err = SendReceive( EAccSrvGetValueTInt,
                       TIpcArgs( &gidPckgC, &nameRecordPckgC, &valuePckg ) );

    if ( KErrNone == err || KErrTimedOut == err /*|| KErrServerBusy == err*/ )
        {
        //These return values are specified
        }
    else
        {
        //These return values are not specified.
        //Thus leave occurs.
        User::Leave( err );
        }

    API_TRACE_1( "[AccFW:Client] RAccessoryConnectionBase::GetValueL TInt - return %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryConnectionBase::GetValueL
// TDes8 version.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnectionBase::GetValueL(
                                const TAccPolGenericID& aGenericID,
                                const TAccPolNameRecord& aNameRecord,
                                TDes8& aValue ) const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::GetValueL() TDes8" );

    TInt                   err( KErrNone );
    TInt                   valueSize( aValue.MaxLength() );
    TPckgBuf<TInt>         valueSizePckgBuf( valueSize );    
    TAccSrvGIDNameRecord   GIDNameRecord;
    GIDNameRecord.iGenericID  = aGenericID;
    GIDNameRecord.iNameRecord = aNameRecord;
    TAccSrvGIDNameRecordPckg GIDNameRecordPckg( GIDNameRecord );

    err = SendReceive( EAccSrvGetValueTDes8,
                       TIpcArgs( &GIDNameRecordPckg,
                                 &aValue,
                                 &valueSizePckgBuf ) );

    if ( KErrNone == err || KErrTimedOut == err /*|| KErrServerBusy == err*/ )
        {
        //These return values are specified
        }
    else
        {
        //These return values are not specified.
        //Thus leave occurs.
        User::Leave( err );
        }

    API_TRACE_1( "[AccFW:Client] RAccessoryConnectionBase::GetValueL TDes8 - return %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryConnectionBase::GetValueL
// CAccPolObjectBase version.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnectionBase::GetValueL( const TAccPolGenericID& aGenericID,
                                                   const TAccPolNameRecord& aNameRecord,
                                                   CAccPolObjectCon& aValue ) const
    {
    API_TRACE_1( "[AccFW:Client] RAccessoryConnectionBase::GetValueL(%d) CAccPolObjectBase", aGenericID.UniqueID() ); 
            
    TInt                   err( KErrNone );
    TAccSrvGenericIDPckgC  gidPckgC( aGenericID );
    TAccSrvNameRecordPckgC nameRecordPckgC( aNameRecord );
    RBufReadStream         rStrm;        
    TAccSrvGenericIDPckgC  genericIDPckgC( aGenericID );
    
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

    err = SendReceive( EAccSrvGetValueObjectBase,
                       TIpcArgs( &gidPckgC, &nameRecordPckgC, &ptr ) );   

    if ( KErrNone == err )
        {        
        //Internalize to the local variable
        (void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
        CleanupClosePushL( rStrm );    
        aValue.Reset();
        aValue.InternalizeL( rStrm );
        CleanupStack::PopAndDestroy( &rStrm );
        }

    else
        {

        if ( KErrOverflow == err )
            {
            //buf size was not enough.
            //Read the new buffer size and make a 2nd attempt.
            API_TRACE_( "[AccFW:Client] RAccessoryConnectionBase::GetValueL - CAccPolObjectBase - 2nd attempt" );
            (void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
            CleanupClosePushL( rStrm );    
            TInt newBufferSize( rStrm.ReadInt32L() );
            CleanupStack::PopAndDestroy( &rStrm );

            buf->Reset();
            buf->ResizeL( newBufferSize );
            TPtr8 ptr( buf->Ptr(0) );

            err = SendReceive( EAccSrvGetObjectValue,
                               TIpcArgs( &ptr ) );                                     
                                         
            if ( KErrNone == err )
                {
                //Internalize to the local variable
                (void) rStrm.Open( *buf ); // void is used to prevent warnings from code scanner.
                CleanupClosePushL( rStrm );    
                aValue.InternalizeL( rStrm );
                CleanupStack::PopAndDestroy( &rStrm );
                }
            else
                {
                User::Leave( err );
                }
            }
        else
            {
            User::Leave( err );
            }
        }

    CleanupStack::PopAndDestroy( buf );

    
    API_TRACE_1( "[AccFW:Client] RAccessoryConnectionBase::GetValueL CAccPolObjectBase - return %d", err );
    return err;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
