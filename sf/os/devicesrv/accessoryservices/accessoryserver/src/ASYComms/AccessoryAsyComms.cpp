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
* Description:  Accessory ASY Comms API
*
*/


// INCLUDE FILES
#include "AccessoryAsyComms.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include <s32mem.h>
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TUint8 KDummyPointer = 0xff;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::RAccessoryAsyComms
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryAsyComms::RAccessoryAsyComms() :
    iProcessCmdIdPckg( *reinterpret_cast<TProcessCmdId*>( KDummyPointer ) ),
    iTransactionIDPckg( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::RAccessoryAsyComms()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::CreateSubSession
// This function should never be called, call overloaded function instead
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C TInt RAccessoryAsyComms::CreateSubSession( RAccessoryServer& /*aSession*/ )
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::CreateSubSession()" );

    // Pure virtual function in base class

    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::CreateSubSession - return KErrNotSupported" );

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::CreateSubSession
// Creates subsession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryAsyComms::CreateSubSession(
                                  RAccessoryServer& aSession,
                                  const TUid aMainServiceUid )
    {
    COM_TRACE_1( "[AccFW:AsyComms] RAccessoryAsyComms::CreateSubSession - MainServiceUid = 0x%x)", aMainServiceUid.iUid );
        
    TUid mainServiceUid( aMainServiceUid );
    TPckgBuf<TUid> mainServiceUidPckgBuf( mainServiceUid );

    // Get the thread ID of the ASY
    RThread thisThread;
    TPckgBuf<TUint> thisThreadIDPckgBuf( thisThread.Id() );

    TInt err = RSubSessionBase::CreateSubSession(
                                    aSession, 
                                    EAccSrvCreateSubSessionAccessoryAsyComms,
                                    TIpcArgs( &thisThreadIDPckgBuf,
                                              &mainServiceUidPckgBuf ) );

    COM_TRACE_1( "[AccFW:AsyComms] RAccessoryAsyComms::CreateSubSession - return %d", err );

    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::CloseSubSession
// Closes subsession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryAsyComms::CloseSubSession()
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessoryAsyComms );

    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::NotifyProcessCommand
// NotifyProcessCommand request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::NotifyProcessCommand(
    TRequestStatus& aStatus,
    TProcessCmdId& aCmdn,
    TInt& aTrId,
    TASYCmdParams& aNameValueRecord )
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::NotifyProcessCommand()" );

    aStatus = KRequestPending;

    TAccSrvProcessCmdIdPckg processCmdIdPckg( aCmdn );
    iProcessCmdIdPckg.Set( processCmdIdPckg );

    TPckg<TInt> transactionIDPckg( aTrId );
    iTransactionIDPckg.Set( transactionIDPckg );

    SendReceive( EAccSrvNotifyProcessCommand,
                 TIpcArgs( &iProcessCmdIdPckg,
                           &iTransactionIDPckg,
                           &aNameValueRecord ),
                 aStatus );

    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::NotifyProcessCommand - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::NotifyProcessCommand
// NotifyProcessCommand request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::NotifyProcessCommand( TRequestStatus& aStatus,
                                                        TProxyCommandPckgBuf& aParams,
                                                        TPtr8& aPtr )
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::NotifyProcessCommand()" );
      
    aStatus = KRequestPending;
        
    SendReceive( EAccSrvNotifyProcessCommand,
                 TIpcArgs( &aParams, &aPtr ),
                 aStatus );                                
                 
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::NotifyProcessCommand - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::CancelNotifyProcessCommand
// Cancels NotifyProcessCommand request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C void RAccessoryAsyComms::CancelNotifyProcessCommand( )
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::CancelNotifyProcessCommand()" );

    SendReceive( EAccSrvCancelNotifyProcessCommand,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::CancelNotifyProcessCommand - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::ProcessResponse
// Response for NotifyProcessCommand made before
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::ProcessResponse(
    const TInt aTrid, 
    const TAccValueTypeTBool aValue,
    const TInt aErrCode )
    {
    API_TRACE_( "[AccFW:AsyComms] ProcessResponse()" );

    TPckgBuf<TInt>               tridPckgBuf( aTrid );
    TAccSrvValueTypeTBoolPckgBuf valuePckgBuf( aValue );
    TPckgBuf<TInt>               errCodePckgBuf( aErrCode );

    SendReceive( EAccSrvProcessResponseTBool,
                 TIpcArgs( &tridPckgBuf, &valuePckgBuf, &errCodePckgBuf ) );

    API_TRACE_( "[AccFW:AsyComms] ProcessResponse - return void()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::ProcessResponse
// Response for NotifyProcessCommand made before
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::ProcessResponse(
    const TInt aTrid,
    const TAccValueTypeTInt aValue,
    const TInt aErrCode )
    {
    API_TRACE_( "[AccFW:AsyComms] ProcessResponse()" );
    
    TPckgBuf<TInt>              tridPckgBuf( aTrid );
    TAccSrvValueTypeTIntPckgBuf valuePckgBuf( aValue );
    TPckgBuf<TInt>              errCodePckgBuf( aErrCode );

    SendReceive( EAccSrvProcessResponseTInt,
                 TIpcArgs( &tridPckgBuf, &valuePckgBuf, &errCodePckgBuf ) );

    API_TRACE_( "[AccFW:AsyComms] ProcessResponse - return void()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAsyComms::ProcessResponse
// Response for NotifyProcessCommand made before
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::ProcessResponse(
    const TInt aTrid,
    const TDesC8& aValue,
    const TInt aErrCode )
    {
    API_TRACE_( "[AccFW:AsyComms] ProcessResponse()" );

    TPckgBuf<TInt>              tridPckgBuf( aTrid );
    TPckgBuf<TInt>              errCodePckgBuf( aErrCode );

    SendReceive( EAccSrvProcessResponseTDes, 
                 TIpcArgs( &tridPckgBuf, &aValue, &errCodePckgBuf ) );

    API_TRACE_( "[AccFW:AsyComms] ProcessResponse - return void()" );
    }


// -----------------------------------------------------------------------------
// RAccessoryAsyComms::ProcessResponse
// Response for NotifyProcessCommand made before
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::ProcessResponse( const TInt aTrid,
                                                   TPtr8* aPtr,
                                                   const TInt aErrCode )
    {    
    API_TRACE_( "[AccFW:AsyComms] ProcessResponse()" );
       
    TPckgBuf<TInt> errCodePckgBuf( aErrCode );
    TPckgBuf<TInt> tridPckgBuf( aTrid );    
    SendReceive( EAccSrvProcessResponseObject, 
                 TIpcArgs( &tridPckgBuf, aPtr, &errCodePckgBuf ) );
                 
    API_TRACE_( "[AccFW:AsyComms] ProcessResponse - return void()" );
    }
    
// -----------------------------------------------------------------------------
// RAccessoryAsyComms::GetBuffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAsyComms::GetBuffer( TPtr8 aPtr )
    {
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::GetBuffer()" );
    
    SendReceive( EAccSrvGetObjectBuffer, TIpcArgs( &aPtr ) );
    
    API_TRACE_( "[AccFW:AsyComms] RAccessoryAsyComms::GetBuffer - return void" );
    }
    
// End of File
