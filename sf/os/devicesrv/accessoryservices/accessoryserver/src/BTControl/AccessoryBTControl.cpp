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
* Description:  Client-side handle to a sub-session of Accessory BT Control
*
*/


// INCLUDE FILES
#include "AccClientServerMessages.h"
#include "AccessoryServer.h"
#include "acc_debug.h"
#include "AccPolGenericID.h"
#include "AccPolAccessoryPolicy.h"
#include <AccessoryBTControl.h>

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
// RAccessoryBTControl::RAccessoryBTControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryBTControl::RAccessoryBTControl() :
    iNotifyBTAudioLinkOpenReqBTAddrPckg(
            *reinterpret_cast<TBTDevAddr*>( KDummyPointer ) ),

    iNotifyBTAudioLinkCloseReqBTAddrPckg(
            *reinterpret_cast<TBTDevAddr*>( KDummyPointer ) ),

    iNotifyBTAudioLinkOpenReqAudioTypePckg(
            *reinterpret_cast<TAccAudioType*>( KDummyPointer ) ),

    iNotifyBTAudioLinkCloseReqAudioTypePckg(
            *reinterpret_cast<TAccAudioType*>( KDummyPointer ) ),

    iFlags( 0 ),
    iFlagsPckg( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved1( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved2( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::RAccessoryBTControl()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::CreateSubSession
// - Construct Base class (RAccessorySubsessionBase) to get PtrHolder
//   constructed (must be freed explicitly also).
// - Creates a new sub-session within an existing session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryBTControl::CreateSubSession( RAccessoryServer& aSession )
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CreateSubSession()" );

    TInt ret = RSubSessionBase::CreateSubSession(
                                    aSession,
                                    EAccSrvCreateSubSessionAccessoryBTControl,
                                    TIpcArgs() );

    API_TRACE_1( "[AccFW:BTControl] RAccessoryBTControl::CreateSubSession - return %d", ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::CloseSubSession
// Closes sub-session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryBTControl::CloseSubSession()
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessoryBTControl );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::ConnectAccessory
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::ConnectAccessory(
    TRequestStatus& aStatus,
    const TBTDevAddr& aBTDevAddr )
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::ConnectAccessory()" );

    if( iFlags & KAccSrvConnectBTAccessoryBit )
        {
        API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::ConnectAccessory - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        iConnectBTDevAddrPckgBuf = aBTDevAddr;

        // Get the thread ID of the connector (e.g. thread ID of ASY)
        RThread thisThread;
        iThisThreadIDPckgBuf = thisThread.Id();

        iFlags += KAccSrvConnectBTAccessoryBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvBluetoothConnectAccessory,
                     TIpcArgs( &iConnectBTDevAddrPckgBuf,
                               &iThisThreadIDPckgBuf,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::ConnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::DisconnectAccessory
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::DisconnectAccessory(
    TRequestStatus& aStatus,
    const TBTDevAddr& aBTDevAddr )
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::DisconnectAccessory()" );

    if( iFlags & KAccSrvDisconnectBTAccessoryBit )
        {
        API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::DisconnectAccessory - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        iDisconnectBTDevAddrPckgBuf = aBTDevAddr;

        iFlags += KAccSrvDisconnectBTAccessoryBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvBluetoothDisconnectAccessory,
                     TIpcArgs( &iDisconnectBTDevAddrPckgBuf,
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::DisconnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::BluetoothAudioLinkOpenRespL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::BluetoothAudioLinkOpenRespL(
    const TBTDevAddr& aBTDevAddr,
    TInt& aResponse ) const
    {
    API_TRACE_2( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkOpenRespL(0x%x, 0x%x)", &aBTDevAddr, &aResponse );

    TAccSrvBTDevAddrPckgC BTDevAddrPckgC( aBTDevAddr );
    TPckg<TInt> response( aResponse );

    User::LeaveIfError( SendReceive( EAccSrvBluetoothAudioLinkOpenResp,
                        TIpcArgs(&BTDevAddrPckgC, &response) ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkOpenRespL - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::BluetoothAudioLinkCloseRespL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::BluetoothAudioLinkCloseRespL(
    const TBTDevAddr& aBTDevAddr,
    TInt& aResponse ) const
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkCloseRespL()" );

    TAccSrvBTDevAddrPckgC BTDevAddrPckgC( aBTDevAddr );
    TPckg<TInt> response( aResponse );

    User::LeaveIfError( SendReceive( EAccSrvBluetoothAudioLinkCloseResp,
                        TIpcArgs(&BTDevAddrPckgC, &response) ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkCloseRespL - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::BluetoothAudioLinkOpenedNotifyL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::BluetoothAudioLinkOpenedNotifyL(
    const TBTDevAddr& aBTDevAddr,
    TAccAudioType aAudioType ) const
    {
    API_TRACE_1( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkOpenedNotifyL(AudioType=%d)", aAudioType );

    TAccSrvBTDevAddrPckgC   BTDevAddrPckgC( aBTDevAddr );
    TAccSrvAudioTypePckgBuf audioTypePckgBuf( aAudioType );

    User::LeaveIfError( SendReceive( EAccSrvBluetoothAudioLinkOpenedNotify,
                        TIpcArgs( &BTDevAddrPckgC, &audioTypePckgBuf ) ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkOpenedNotifyL - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::BluetoothAudioLinkClosedNotifyL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::BluetoothAudioLinkClosedNotifyL(
    const TBTDevAddr& aBTDevAddr,
    TAccAudioType aAudioType ) const
    {
    API_TRACE_1( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkClosedNotifyL(AudioType=%d)", aAudioType );

    TAccSrvBTDevAddrPckgC   BTDevAddrPckgC( aBTDevAddr );
    TAccSrvAudioTypePckgBuf audioTypePckgBuf( aAudioType );

    User::LeaveIfError( SendReceive( EAccSrvBluetoothAudioLinkClosedNotify,
                        TIpcArgs( &BTDevAddrPckgC, &audioTypePckgBuf ) ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::BluetoothAudioLinkClosedNotifyL - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::NotifyBluetoothAudioLinkOpenReq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::NotifyBluetoothAudioLinkOpenReq(
    TRequestStatus& aStatus,
    TBTDevAddr& aBTDevAddr,
    TAccAudioType& aAudioType )
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::NotifyBluetoothAudioLinkOpenReq()" );

    if( iFlags & KAccSrvNotifyBluetoothAudioLinkOpenReqBit )
        {
        API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::NotifyBluetoothAudioLinkOpenReq - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvBTDevAddrPckg BTDevAddrPackage(aBTDevAddr);
        iNotifyBTAudioLinkOpenReqBTAddrPckg.Set( BTDevAddrPackage );

        TAccSrvAudioTypePckg audioTypePackage( aAudioType );
        iNotifyBTAudioLinkOpenReqAudioTypePckg.Set( audioTypePackage );

        iFlags += KAccSrvNotifyBluetoothAudioLinkOpenReqBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyBluetoothAudioLinkOpenReq,
                     TIpcArgs( &iNotifyBTAudioLinkOpenReqBTAddrPckg,
                               &iNotifyBTAudioLinkOpenReqAudioTypePckg,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::NotifyBluetoothAudioLinkOpenReq - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::NotifyBluetoothAudioLinkCloseReq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::NotifyBluetoothAudioLinkCloseReq(
    TRequestStatus& aStatus,
    TBTDevAddr& aBTDevAddr,
    TAccAudioType& aAudioType )
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::NotifyBluetoothAudioLinkCloseReq()" );

    if( iFlags & KAccSrvNotifyBluetoothAudioLinkCloseReqBit )
        {
        API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::NotifyBluetoothAudioLinkCloseReq - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvBTDevAddrPckg BTDevAddrPackage( aBTDevAddr );
        iNotifyBTAudioLinkCloseReqBTAddrPckg.Set( BTDevAddrPackage );

        TAccSrvAudioTypePckg audioTypePackage( aAudioType );
        iNotifyBTAudioLinkCloseReqAudioTypePckg.Set( audioTypePackage );

        iFlags += KAccSrvNotifyBluetoothAudioLinkCloseReqBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyBluetoothAudioLinkCloseReq,
                     TIpcArgs( &iNotifyBTAudioLinkCloseReqBTAddrPckg,
                               &iNotifyBTAudioLinkCloseReqAudioTypePckg,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::NotifyBluetoothAudioLinkCloseReq - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::AccessoryValueChangedNotifyL
//
// TBool version.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::AccessoryValueChangedNotifyL(
    const TBTDevAddr& aBTDevAddr,
    const TAccPolNameRecord& aNameRecord,
    const TAccValueTypeTBool aValue ) const
    {
    API_TRACE_1( "[AccFW:BTControl] RAccessoryBTControl::AccessoryValueChangedNotifyL(Value=%d) TBool", aValue.iValue );

    TAccSrvBTDevAddrPckgC        BTDevAddrPckgC( aBTDevAddr );
    TAccSrvNameRecordPckgC       nameRecord( aNameRecord );
    TAccSrvValueTypeTBoolPckgBuf value( aValue );

    User::LeaveIfError( SendReceive( EAccSrvBTAccessoryValueChangedTBoolNotify,
                                     TIpcArgs( &BTDevAddrPckgC,
                                               &nameRecord,
                                               &value ) ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::AccessoryValueChangedNotifyL TBool - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryControl::AccessoryValueChangedNotifyL
//
// TInt version.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::AccessoryValueChangedNotifyL(
    const TBTDevAddr& aBTDevAddr,
    const TAccPolNameRecord& aNameRecord,
    const TAccValueTypeTInt aValue ) const
    {
    API_TRACE_1( "[AccFW:BTControl] RAccessoryBTControl::AccessoryValueChangedNotifyL(Value=%d) TInt", aValue.iValue );

    TAccSrvBTDevAddrPckgC        BTDevAddrPckgC( aBTDevAddr );
    TAccSrvNameRecordPckgC       nameRecord( aNameRecord );
    TAccSrvValueTypeTIntPckgBuf  value( aValue );

    User::LeaveIfError( SendReceive( EAccSrvBTAccessoryValueChangedTIntNotify,
                                     TIpcArgs( &BTDevAddrPckgC,
                                               &nameRecord,
                                               &value ) ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::AccessoryValueChangedNotifyL TInt - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::CancelConnectAccessory
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::CancelConnectAccessory() const
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelConnectAccessory()" );

    SendReceive( EAccSrvCancelConnectAccessory,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelConnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::CancelDisconnectAccessory
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::CancelDisconnectAccessory() const
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelDisconnectAccessory()" );

    SendReceive( EAccSrvCancelDisconnectAccessory,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelDisconnectAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::CancelNotifyBluetoothAudioLinkOpenReq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::CancelNotifyBluetoothAudioLinkOpenReq() const
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelNotifyBluetoothAudioLinkOpenReq()" );

    SendReceive( EAccSrvCancelNotifyBluetoothAudioLinkOpenReq,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelNotifyBluetoothAudioLinkOpenReq - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryBTControl::CancelNotifyBluetoothAudioLinkCloseReq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryBTControl::CancelNotifyBluetoothAudioLinkCloseReq() const
    {
    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelNotifyBluetoothAudioLinkCloseReq()" );

    SendReceive( EAccSrvCancelNotifyBluetoothAudioLinkCloseReq,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:BTControl] RAccessoryBTControl::CancelNotifyBluetoothAudioLinkCloseReq - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
