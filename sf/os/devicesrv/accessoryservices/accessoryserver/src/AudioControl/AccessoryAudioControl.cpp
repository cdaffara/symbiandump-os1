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
* Description:  Client-side handle to a sub-session of Accessory Audio Control
*
*/


// INCLUDE FILES

#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include <AccessoryServer.h>
#include <AccessoryAudioControl.h>


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
// RAccessoryAudioControl::RAccessoryAudioControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryAudioControl::RAccessoryAudioControl() :
    iNotifyAudioLinkOpenedGIDPckg(
         *reinterpret_cast<TAccPolGenericID*>( KDummyPointer ) ),

    iNotifyAudioLinkOpenedAudioTypePckg(
         *reinterpret_cast<TAccAudioType*>( KDummyPointer ) ),

    iNotifyAudioLinkClosedGIDPckg(
         *reinterpret_cast<TAccPolGenericID*>( KDummyPointer ) ),

    iNotifyAudioLinkClosedAudioTypePckg(
         *reinterpret_cast<TAccAudioType*>( KDummyPointer ) ),

    iFlags( 0 ),
    iFlagsPckg( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved1( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved2( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::RAccessoryAudioControl()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::CreateSubSession
// Creates the subsession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryAudioControl::CreateSubSession(RAccessoryServer& aSession)
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CreateSubSession()" );

    //Get the thread ID of the caller
    RThread thisThread;
    TPckgBuf<TUint> thisThreadIDPckgBuf( thisThread.Id() );

    TInt ret = RSubSessionBase::CreateSubSession(
                     aSession,
                     EAccSrvCreateSubSessionAccessoryAudioControl,
                     TIpcArgs( &thisThreadIDPckgBuf ) );

    API_TRACE_1( "[AccFW:AudioControl] RAccessoryAudioControl::CreateSubSession - return 0x%x", ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::CloseSubSession
// Closes the subsession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryAudioControl::CloseSubSession()
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessoryAudioControl );

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::AccessoryAudioLinkOpen
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::AccessoryAudioLinkOpen(
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId,
    TAccAudioType aAudioType )
    {
    API_TRACE_2( "[AccFW:AudioControl] RAccessoryAudioControl::AccessoryAudioLinkOpen(UniqueID=%d AudioType=%d)", aGenericId.UniqueID(), aAudioType );

    if( iFlags & KAccSrvAccessoryAudioLinkOpenBit )
        {
        API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::AccessoryAudioLinkOpen - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        iAudioLinkOpenGIDPckgBuf = aGenericId;
        iAudioLinkOpenAudioTypePckgBuf = aAudioType;

        iFlags += KAccSrvAccessoryAudioLinkOpenBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvAccessoryAudioLinkOpen,
                     TIpcArgs( &iAudioLinkOpenGIDPckgBuf,
                               &iAudioLinkOpenAudioTypePckgBuf,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::AccessoryAudioLinkOpen - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::AccessoryAudioLinkClose
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::AccessoryAudioLinkClose(
    TRequestStatus& aStatus,
    const TAccPolGenericID& aGenericId,
    TAccAudioType aAudioType )
    {
    API_TRACE_2( "[AccFW:AudioControl] RAccessoryAudioControl::AccessoryAudioLinkClose(UniqueID=%d AudioType=%d)", aGenericId.UniqueID(), aAudioType );

    if( iFlags & KAccSrvAccessoryAudioLinkCloseBit )
        {
        API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::AccessoryAudioLinkClose - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        iAudioLinkCloseGIDPckgBuf = aGenericId;

        iAudioLinkCloseAudioTypePckgBuf = aAudioType;

        iFlags += KAccSrvAccessoryAudioLinkCloseBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvAccessoryAudioLinkClose,
                     TIpcArgs( &iAudioLinkCloseGIDPckgBuf,
                               &iAudioLinkCloseAudioTypePckgBuf,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::AccessoryAudioLinkClose - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::AudioRoutingStatusNotify
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::AudioRoutingStatusNotify(
    TBool aActiveAudio,
    const TAccPolGenericID& aGenericId )
    {
    API_TRACE_2( "[AccFW:AudioControl] RAccessoryAudioControl::AudioRoutingStatusNotify(UniqueID=%d ActiveAudio=%d)", aGenericId.UniqueID(), aActiveAudio );

    TPckgBuf<TBool> activeAudioPckgBuf( aActiveAudio );
    TAccSrvGenericIDPckgC genericIDPckgC( aGenericId );

    SendReceive( EAccSrvAudioRoutingStatusNotify,
                 TIpcArgs( &activeAudioPckgBuf, &genericIDPckgC ) );

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::AudioRoutingStatusNotify - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::NotifyAccessoryAudioLinkOpened
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::NotifyAccessoryAudioLinkOpened(
    TRequestStatus& aStatus,
    TAccPolGenericID& aGenericId,
    TAccAudioType& aAudioType )
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::NotifyAccessoryAudioLinkOpened()" );

    if( iFlags & KAccSrvNotifyAccessoryAudioLinkOpenedBit )
        {
        API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::NotifyAccessoryAudioLinkOpened - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGenericIDPckg genericIDPackage( aGenericId );
        iNotifyAudioLinkOpenedGIDPckg.Set( genericIDPackage );

        TAccSrvAudioTypePckg audioTypePackage( aAudioType );
        iNotifyAudioLinkOpenedAudioTypePckg.Set( audioTypePackage );

        iFlags += KAccSrvNotifyAccessoryAudioLinkOpenedBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryAudioLinkOpened,
                     TIpcArgs( &iNotifyAudioLinkOpenedGIDPckg,
                               &iNotifyAudioLinkOpenedAudioTypePckg,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::NotifyAccessoryAudioLinkOpened - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::NotifyAccessoryAudioLinkClosed
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::NotifyAccessoryAudioLinkClosed(
    TRequestStatus& aStatus,
    TAccPolGenericID& aGenericId,
    TAccAudioType& aAudioType )
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::NotifyAccessoryAudioLinkClosed()" );

    if( iFlags & KAccSrvNotifyAccessoryAudioLinkClosedBit )
        {
        API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::NotifyAccessoryAudioLinkClosed - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGenericIDPckg genericIDPackage( aGenericId );
        iNotifyAudioLinkClosedGIDPckg.Set( genericIDPackage );

        TAccSrvAudioTypePckg audioTypePackage( aAudioType );
        iNotifyAudioLinkClosedAudioTypePckg.Set( audioTypePackage );

        iFlags += KAccSrvNotifyAccessoryAudioLinkClosedBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryAudioLinkClosed,
                     TIpcArgs( &iNotifyAudioLinkClosedGIDPckg,
                               &iNotifyAudioLinkClosedAudioTypePckg,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::NotifyAccessoryAudioLinkClosed - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::CancelAccessoryAudioLinkOpen
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::CancelAccessoryAudioLinkOpen() const
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelAccessoryAudioLinkOpen()" );

    SendReceive( EAccSrvCancelAccessoryAudioLinkOpen,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelAccessoryAudioLinkOpen - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::CancelAccessoryAudioLinkClose
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::CancelAccessoryAudioLinkClose() const
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelAccessoryAudioLinkClose()" );

    SendReceive( EAccSrvCancelAccessoryAudioLinkClose,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelAccessoryAudioLinkClose - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkOpened
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkOpened() const
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkOpened()" );

    SendReceive( EAccSrvCancelNotifyAccessoryAudioLinkOpened,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkOpened - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkClosed
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkClosed() const
    {
    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkClosed()" );

    SendReceive( EAccSrvCancelNotifyAccessoryAudioLinkClosed,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:AudioControl] RAccessoryAudioControl::CancelNotifyAccessoryAudioLinkClosed - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
